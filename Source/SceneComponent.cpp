#include "SceneComponent.h"

SceneComponent::SceneComponent(Scene* scene) : ResizableWindow("Scene", false), scene(scene)
{
    overlay = new SceneOverlayComponent(this);
    setContentOwned(overlay, false);
    //setOpaque(false);
}

void SceneComponent::resized() {
    ResizableWindow::resized();
    boundsInParent = getBoundsInParent();
    parentHeight = getParentHeight();
    scene->changeBounds(boundsInParent, parentHeight, ownRender != nullptr);
    overlay->setFullscreenState(isFullScreen()); // case when window exit fullscreen with window button
}

void SceneComponent::moved() {
    ResizableWindow::moved();
    boundsInParent = getBoundsInParent();
    parentHeight = getParentHeight();
    scene->changeBounds(boundsInParent, parentHeight, ownRender != nullptr);
}

void SceneComponent::mouseDown(const MouseEvent& e) {
    ResizableWindow::mouseDown(e);
    
    for(auto listener : listeners) {
        listener->sceneMouseDown(scene);
    }
}

void SceneComponent::mouseUp(const MouseEvent& e){
    ResizableWindow::mouseUp(e);
    
    if(e.mouseWasClicked()) {
        for(auto listener : listeners) {
            listener->sceneMouseClicked(scene);
        }
    } else {
        for(auto listener : listeners) {
            listener->sceneMouseUp(scene);
        }
    }
}

void SceneComponent::deleteButtonClicked() {
    for(auto listener : listeners) {
        if(listener == deleteListener) {
            continue;
        }
        listener->sceneDeleteButtonClicked(this);
    }
    deleteListener->sceneDeleteButtonClicked(this);
}

void SceneComponent::pinButtonClicked(bool state) {
    getPeer()->setAlwaysOnTop(state);
}

void SceneComponent::fullscreenButtonClicked(bool state) {
    setFullScreen(state);
}

void SceneComponent::detachButtonClicked(bool detach) {
    if(detach) {
        for(auto listener : listeners) {
            listener->sceneDetachButtonClicked(this, detach);
        }
        
        addToDesktop();
        overlay->setEmbeddedButtonsEnabled(true);
        pinButtonClicked(false);
        setFullScreen(false);
        ownRender.reset(new ScenesRender(*this));
        scene->replaceContext(ownRender.get()->getContext());
        ownRender->addScene(scene);
    } else {
        overlay->setEmbeddedButtonsEnabled(false);
        pinButtonClicked(false);
        setFullScreen(false);
        ownRender.reset();
        
        for(auto listener : listeners) {
            listener->sceneDetachButtonClicked(this, detach);
        }
    }
}
//=================================================================
SceneComponent::SceneOverlayComponent::SceneOverlayComponent(SceneComponent* parent) : parent(parent)
{
    detachButton.setToggleable(true);
    detachButton.setClickingTogglesState(true);
    detachButton.setToggleState(false, NotificationType::dontSendNotification);
    detachButton.onClick = [this](){this->parent->detachButtonClicked(detachButton.getToggleState());};
    
    pinButton.setToggleable(true);
    pinButton.setClickingTogglesState(true);
    pinButton.setToggleState(false, NotificationType::dontSendNotification);
    pinButton.onClick = [this](){this->parent->pinButtonClicked(pinButton.getToggleState());};
    pinButton.setEnabled(false);
    
    fullscreenButton.setToggleable(true);
    fullscreenButton.setClickingTogglesState(true);
    fullscreenButton.setToggleState(false, NotificationType::dontSendNotification);
    fullscreenButton.onClick = [this](){this->parent->fullscreenButtonClicked(fullscreenButton.getToggleState());};
    fullscreenButton.setEnabled(false);
    
    deleteButton.onClick = [this](){this->parent->deleteButtonClicked();};
    
    addAndMakeVisible(deleteButton);
    addAndMakeVisible(detachButton);
    addAndMakeVisible(pinButton);
    addAndMakeVisible(fullscreenButton);
}

void SceneComponent::SceneOverlayComponent::resized()
{
    auto localBounds = getLocalBounds();
    deleteButton.setBounds(localBounds.getWidth() - 20, 0, 20, 20);
    detachButton.setBounds(localBounds.getWidth() - 40, 0, 20, 20);
    pinButton.setBounds(localBounds.getWidth() - 60, 0, 20, 20);
    fullscreenButton.setBounds(localBounds.getWidth() - 80, 0, 20, 20);
}

void SceneComponent::SceneOverlayComponent::mouseEnter(const MouseEvent& e) { 
    /*for(auto c : getAllComponents()) {
        c->setVisible(true);
    }*/
    
    parent->mouseEnter(e);
}

void SceneComponent::SceneOverlayComponent::mouseExit(const MouseEvent& e) { 
    /*for(auto c : getAllComponents()) {
        c->setVisible(false);
    }*/
    
    parent->mouseExit(e);
}
