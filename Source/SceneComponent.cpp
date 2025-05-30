#include "SceneComponent.h"

SceneComponent::SceneComponent(OpenGLContext& context, StatefulObject& parent) : ResizableWindow("i", false), StatefulObject::Sucker(nullptr)
{
    init(parent, nullptr, &context);
}

SceneComponent::SceneComponent(StatefulObject& parent, StatefulObject::ObjectState sceneState) : ResizableWindow("i", false), StatefulObject::Sucker(nullptr)
{
    init(parent, &sceneState, nullptr);
    bool isDetached = getProperty(IDs::isDetached);
    if(isDetached) {
        setBoundsFromState();
        showOrHideOvelray(true);
    }
    
    if(getProperty(IDs::isAlwaysOnTop)) overlay->setAlwaysOnTopState(true);
    if(getProperty(IDs::isPinned)) overlay->setPinnedState(true);
    if(getProperty(IDs::isFullscreen))overlay->setFullscreenState(true);
    
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
        
        if(getProperty(IDs::isAlwaysOnTop)) setAlwaysOnTop(true);
        if(getProperty(IDs::isPinned)) setPinState(true);
        if(getProperty(IDs::isFullscreen)) setFullscreen(true);
        
        stopTimer();
        resized();
    }
}

void SceneComponent::init(StatefulObject& parent, StatefulObject::ObjectState* sceneState, OpenGLContext* context)
{
    if(sceneState == nullptr) {
        scene = new Scene(*context, parent);
    } else {
        scene = new Scene(parent, *sceneState);
    }
    scene->addListener(this);
    setName(scene->getName());
    setSource(scene);
    
    overlay = new SceneOverlayComponent(this);
    overlay->setSceneName(scene->getName());
    setContentOwned(overlay, false);
}

void SceneComponent::saveCurrentBounds() {
    auto bounds = getBoundsInParent();
    savePos(bounds);
    saveSize(bounds);
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
    scene->changeBounds(bounds, moved, getParentHeight());
    
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
    if(needToSaveSize) {
        setProperty(IDs::width, bounds.getWidth());
        setProperty(IDs::height, bounds.getHeight());
    }
}

void SceneComponent::mouseUp(const MouseEvent& e){
    ResizableWindow::mouseUp(e);
    if(e.mouseWasClicked()) {
        for(auto listener : listeners) listener->sceneMouseClicked(*this);
    }
}

void SceneComponent::deleteScene() {
    for(auto listener : listeners) {
        if(listener != deleter) listener->sceneDeleting(*this);
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

void SceneComponent::detachScene(bool mustBeDetached, bool callWithButtonClick) {
    const auto listenersCallFunc {[this, callWithButtonClick, mustBeDetached](){
        if(callWithButtonClick) {
            for(auto listener : listeners) listener->sceneDetached(*this, mustBeDetached);
        } else {
            overlay->setDetachState(mustBeDetached);
            overlay->setDetachedMode(mustBeDetached);
        }
    }};
    
    if(mustBeDetached) {
        listenersCallFunc();
        setResizable(true);
        addToDesktop();
        ownRender.reset(new ScenesRender(*this));
        scene->replaceContext(ownRender.get()->getContext(), true);
        ownRender->addScene(scene);
        if(hasProperty(IDs::posX)) setBoundsFromState(); // maybe other check?
    } else {
        saveCurrentBounds();
        ownRender.reset();
        listenersCallFunc();
        setResizable(false);
    }
    overlay->setDragAllowed(mustBeDetached);
}

void SceneComponent::setResizable(bool state) {
    needToSaveSize = false;
    ResizableWindow::setResizable(state, state);
    needToSaveSize = true;
}
//=================================================================
SceneComponent::SceneOverlayComponent::SceneOverlayComponent(SceneComponent* parent) : inactivityDetector(*this), parent(parent)
{
    initButton(detachButton, [this]() {
        this->parent->detachScene(detachButton.getToggleState(), true);
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
    
    for(auto c : getAllComponents()) addChildComponent(c);
    
    inactivityDetector.setDelay(inactivityDelay);
    inactivityDetector.setMouseMoveTolerance(0);
    //inactivityDetector.addListener(this);
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
    if(isMouseActive && isMouseMoveAllowed) {
        setControlsVisible(true);
        parent->mouseMove(e);
    }
}

void SceneComponent::SceneOverlayComponent::mouseDrag(const MouseEvent& e) {
    if(!pinButton.getToggleState() && isDragAllowed)
        parent->mouseDrag(e);
}

void SceneComponent::SceneOverlayComponent::setDetachedMode(bool shouldBeOn) {
    if(shouldBeOn)
        parent->setAlwaysOnTop(topButton.getToggleState());
    topButton.setEnabled(shouldBeOn);
    
    if(shouldBeOn)
        parent->setFullscreen(fullscreenButton.getToggleState());
    fullscreenButton.setEnabled(shouldBeOn);
    
    if(shouldBeOn)
        parent->setPinState(pinButton.getToggleState());
    pinButton.setEnabled(shouldBeOn);
}

void SceneComponent::SceneOverlayComponent::setControlsVisible(bool shouldBeVisible) {
    for(auto c : getAllComponents()) c->setVisible(shouldBeVisible);
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

void SceneComponent::SceneOverlayComponent::showOrHideOvelray(bool mustBeShown) {
    if(mustBeShown) {
        inactivityDetector.addListener(this);
    } else {
        inactivityDetector.removeListener(this);
    }
    isMouseMoveAllowed = mustBeShown;
    setControlsVisible(mustBeShown);
    setMouseCursor(MouseCursor(MouseCursor::StandardCursorType::NormalCursor));
}
