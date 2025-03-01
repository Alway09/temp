#include "SceneComponent.h"

SceneComponent::SceneComponent(Scene& scene) : ResizableWindow(scene.getName(), false), StatefulObject::Sucker(scene), scene(scene)
{
    setBoundsFromState();
    setResizable(true, true);
    overlay = new SceneOverlayComponent(this);
    overlay->setSceneName(scene.getName());
    setContentOwned(overlay, false);
    
    bool isDetached = getProperty(IDs::isDetached);
    if(isDetached) {
        overlay->setDetachState(true);
        startTimer(100);
    }
}

void SceneComponent::timerCallback() {
    if(isVisible()) {
        detachScene(true);
        overlay->setDetachedMode(true);
        setBoundsFromState();
        
        bool isAlwaysOnTop = getProperty(IDs::isAlwaysOnTop);
        if(isAlwaysOnTop) {
            overlay->setAlwaysOnTopState(true);
            setAlwaysOnTop(true);
        }
        
        bool isPinned = getProperty(IDs::isPinned);
        if(isPinned) {
            overlay->setPinnedState(true);
            setPinState(true);
        }
        
        bool isFullscreen = getProperty(IDs::isFullscreen);
        if(isFullscreen) {
            // TODO save last fullscreen
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
    
    if(!overlay->getFullscreenState()) {
        if(moved) {
            setProperty(IDs::posX, bounds.getX());
            setProperty(IDs::posY, bounds.getY());
        } else {
            setProperty(IDs::width, bounds.getWidth());
            setProperty(IDs::height, bounds.getHeight());
        }
    }
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
    } else {
        ownRender.reset();
        
        for(auto listener : listeners) listener->sceneDetached(*this, mustBeDetached);
    }
}
//=================================================================
SceneComponent::SceneOverlayComponent::SceneOverlayComponent(SceneComponent* parent) : parent(parent), inactivityDetector(*this)
{
    detachButton.setToggleable(true);
    detachButton.setClickingTogglesState(true);
    detachButton.setToggleState(false, NotificationType::dontSendNotification);
    detachButton.onClick = [this](){
        this->parent->detachScene(detachButton.getToggleState());
        this->parent->setProperty(SceneComponent::IDs::isDetached, detachButton.getToggleState());
        setDetachedMode(detachButton.getToggleState());
    };
    
    topButton.setToggleable(true);
    topButton.setClickingTogglesState(true);
    topButton.setToggleState(false, NotificationType::dontSendNotification);
    topButton.onClick = [this](){
        this->parent->setAlwaysOnTop(topButton.getToggleState());
        this->parent->setProperty(SceneComponent::IDs::isAlwaysOnTop, topButton.getToggleState());
    };
    topButton.setEnabled(false);
    
    fullscreenButton.setToggleable(true);
    fullscreenButton.setClickingTogglesState(true);
    fullscreenButton.setToggleState(false, NotificationType::dontSendNotification);
    fullscreenButton.onClick = [this]() {
        this->parent->setFullscreen(fullscreenButton.getToggleState());
        this->parent->setProperty(SceneComponent::IDs::isFullscreen, fullscreenButton.getToggleState());
        pinButton.setEnabled(!fullscreenButton.getToggleState());
        topButton.setEnabled(!fullscreenButton.getToggleState());
    };
    fullscreenButton.setEnabled(false);
    
    pinButton.setToggleable(true);
    pinButton.setClickingTogglesState(true);
    pinButton.setToggleState(false, NotificationType::dontSendNotification);
    pinButton.onClick = [this](){
        this->parent->setPinState(pinButton.getToggleState());
        this->parent->setProperty(SceneComponent::IDs::isPinned, pinButton.getToggleState());
    };
    pinButton.setEnabled(false);
    
    deleteButton.onClick = [this](){this->parent->deleteScene();};
    
    nameLabel.setJustificationType(Justification::left);
    
    addAndMakeVisible(deleteButton);
    addAndMakeVisible(detachButton);
    addAndMakeVisible(topButton);
    addAndMakeVisible(fullscreenButton);
    addAndMakeVisible(pinButton);
    addAndMakeVisible(nameLabel);
    
    inactivityDetector.setDelay(inactivityDelay);
    inactivityDetector.setMouseMoveTolerance(0);
    inactivityDetector.addListener(this);
}

void SceneComponent::SceneOverlayComponent::resized()
{
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
