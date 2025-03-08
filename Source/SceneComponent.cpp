#include "SceneComponent.h"

SceneComponent::SceneComponent(Scene& scene) : ResizableWindow(scene.getName(), false), StatefulObject::Sucker(scene), scene(scene)
{
    overlay = new SceneOverlayComponent(this);
    bool isDetached = getProperty(IDs::isDetached);
    if(isDetached) {
        setBoundsFromState();
    }
    setResizable(true, true);
    overlay->setSceneName(scene.getName());
    setContentOwned(overlay, false);
    
    if(isDetached) {
        overlay->setDetachState(true);
        startTimer(100);
    }
}

void SceneComponent::saveCurrentBounds() {
    auto bounds = getBoundsInParent();
    savePos(bounds);
    saveSize(bounds);
}

void SceneComponent::timerCallback() {
    if(isVisible()) {
        detachScene(true);
        overlay->setDetachedMode(true);
        setBoundsFromState();
        
        bool prop = getProperty(IDs::isAlwaysOnTop);
        if(prop) {
            overlay->setAlwaysOnTopState(true);
            setAlwaysOnTop(true);
        }
        
        prop = getProperty(IDs::isPinned);
        if(prop) {
            overlay->setPinnedState(true);
            setPinState(true);
        }
        
        prop = getProperty(IDs::isFullscreen);
        if(prop) {
            overlay->setFullscreenState(true);
            setFullscreen(true);
        }
        
        stopTimer();
        resized();
    }
}

void SceneComponent::setBoundsFromState() {
    int x = getProperty(IDs::posX);
    int y = getProperty(IDs::posY);
    int w = getProperty(IDs::width);
    int h = getProperty(IDs::height);
    setBounds(x, y, w, h);
}

void SceneComponent::resized() {
    ResizableWindow::resized();
    movedOrResized(false);
    //overlay->setFullscreenState(isFullScreen()); // case when window exit fullscreen with window button
}

void SceneComponent::moved() {
    ResizableWindow::moved();
    movedOrResized(true);
}

void SceneComponent::movedOrResized(bool moved) {
    auto bounds = getBoundsInParent();
    scene.changeBounds(bounds, moved, getParentHeight());
    
    if(isDetached() && !overlay->getFullscreenState()) {
        if(moved) {
            savePos(bounds);
        } else {
            saveSize(bounds);
        }
    }
}

void SceneComponent::savePos(Rectangle<int>& bounds) {
    setProperty(IDs::posX, bounds.getX());
    setProperty(IDs::posY, bounds.getY());
}

void SceneComponent::saveSize(Rectangle<int>& bounds) {
    setProperty(IDs::width, bounds.getWidth());
    setProperty(IDs::height, bounds.getHeight());
}

void SceneComponent::mouseDown(const MouseEvent& e) {
    ResizableWindow::mouseDown(e);
    for(auto listener : listeners) listener->sceneMouseDown(scene);
}

void SceneComponent::mouseUp(const MouseEvent& e){
    ResizableWindow::mouseUp(e);
    if(e.mouseWasClicked()) {
        for(auto listener : listeners) listener->sceneMouseClicked(scene);
    } else {
        for(auto listener : listeners) listener->sceneMouseUp(scene);
    }
}

void SceneComponent::deleteScene() {
    for(auto listener : listeners) {
        if(listener == deleter) continue;
        listener->sceneDeleting(*this);
    }
    deleter->sceneDeleting(*this);
}

void SceneComponent::setAlwaysOnTop(bool mustBeAlwaysOnTop) {
    getPeer()->setAlwaysOnTop(mustBeAlwaysOnTop);
}

void SceneComponent::setFullscreen(bool mustBeFullscreen) {
    setFullScreen(mustBeFullscreen);
}

void SceneComponent::setPinState(bool shouldBePinned) {
    // move enabling/disabling made in overlay by control mouse move event
    if(shouldBePinned) {
        setResizeLimits(getWidth(), getHeight(), getWidth(), getHeight());
    } else {
        setResizeLimits(1, 1, 0x3fffffff, 0x3fffffff);
    }
}

void SceneComponent::detachScene(bool mustBeDetached) {
    if(mustBeDetached) {
        for(auto listener : listeners) listener->sceneDetached(*this, mustBeDetached);
        
        addToDesktop();
        ownRender.reset(new ScenesRender(*this));
        scene.replaceContext(ownRender.get()->getContext(), true);
        ownRender->addScene(&scene);
        if(hasProperty(IDs::posX)) setBoundsFromState();
        saveCurrentBounds();
    } else {
        saveCurrentBounds();
        ownRender.reset();
        for(auto listener : listeners) listener->sceneDetached(*this, mustBeDetached);
    }
}
//=================================================================
SceneComponent::SceneOverlayComponent::SceneOverlayComponent(SceneComponent* parent) : parent(parent), inactivityDetector(*this)
{
    initButton(detachButton, [this]() {
        this->parent->detachScene(detachButton.getToggleState());
        this->parent->setProperty(SceneComponent::IDs::isDetached, detachButton.getToggleState());
        setDetachedMode(detachButton.getToggleState());
    });
    
    initButton(topButton, [this]() {
        this->parent->setAlwaysOnTop(topButton.getToggleState());
        this->parent->setProperty(SceneComponent::IDs::isAlwaysOnTop, topButton.getToggleState());
    }, false);
    
    initButton(fullscreenButton, [this]() {
        this->parent->setFullscreen(fullscreenButton.getToggleState());
        this->parent->setProperty(SceneComponent::IDs::isFullscreen, fullscreenButton.getToggleState());
        pinButton.setEnabled(!fullscreenButton.getToggleState());
        topButton.setEnabled(!fullscreenButton.getToggleState());
    }, false);
    
    initButton(pinButton, [this]() {
        this->parent->setPinState(pinButton.getToggleState());
        this->parent->setProperty(SceneComponent::IDs::isPinned, pinButton.getToggleState());
    }, false);
    
    deleteButton.onClick = [this](){this->parent->deleteScene();};
    
    nameLabel.setJustificationType(Justification::left);
    
    for(auto c : getAllComponents()) addAndMakeVisible(c);
    
    inactivityDetector.setDelay(inactivityDelay);
    inactivityDetector.setMouseMoveTolerance(0);
    inactivityDetector.addListener(this);
}

void SceneComponent::SceneOverlayComponent::initButton(Button& b, std::function<void()> f, bool enabled) {
    b.setToggleable(true);
    b.setClickingTogglesState(true);
    b.setToggleState(false, NotificationType::dontSendNotification);
    b.setEnabled(enabled);
    b.onClick = f;
}

void SceneComponent::SceneOverlayComponent::resized() {
    auto localBounds = getLocalBounds();
    deleteButton.setBounds(localBounds.getWidth() - 20, 0, 20, 20);
    detachButton.setBounds(localBounds.getWidth() - 40, 0, 20, 20);
    topButton.setBounds(localBounds.getWidth() - 60, 0, 20, 20);
    fullscreenButton.setBounds(localBounds.getWidth() - 80, 0, 20, 20);
    pinButton.setBounds(localBounds.getWidth() - 100, 0, 20, 20);
    nameLabel.setBounds(0, 0, localBounds.getWidth() - 120, 20);
}

void SceneComponent::SceneOverlayComponent::mouseExit(const MouseEvent& e) {
    parent->mouseExit(e);
    if(!isMouseOver(true)) setControlsVisible(false);
}

void SceneComponent::SceneOverlayComponent::mouseMove(const MouseEvent& e) {
    if(isMouseActive) {
        setControlsVisible(isMouseOver());
        parent->mouseMove(e);
    }
}

void SceneComponent::SceneOverlayComponent::setDetachedMode(bool shouldBeOn) {
    topButton.setToggleState(false, NotificationType::sendNotification);
    topButton.setEnabled(shouldBeOn);
    
    fullscreenButton.setToggleState(false, NotificationType::sendNotification);
    fullscreenButton.setEnabled(shouldBeOn);
    
    pinButton.setToggleState(false, NotificationType::sendNotification);
    pinButton.setEnabled(shouldBeOn);
}

void SceneComponent::SceneOverlayComponent::setControlsVisible(bool shouldBeVisible) {
    if(isVisible == shouldBeVisible) return;
    for(auto c : getAllComponents()) c->setVisible(shouldBeVisible);
    isVisible = shouldBeVisible;
}

bool SceneComponent::SceneOverlayComponent::isMouseOnControl() {
    for (auto c : getAllComponents()) {
        if(c->isMouseOver()) return true;
    }
    return false;
}

void SceneComponent::SceneOverlayComponent::mouseBecameActive() {
    isMouseActive = true;
    setMouseCursor(MouseCursor(MouseCursor::StandardCursorType::NormalCursor));
    setControlsVisible(true);
}

void SceneComponent::SceneOverlayComponent::mouseBecameInactive() {
    if(!isMouseOnControl()) {
        isMouseActive = false;
        setMouseCursor(MouseCursor(MouseCursor::StandardCursorType::NoCursor));
        setControlsVisible(false);
    }
}
