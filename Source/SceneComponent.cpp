#include "SceneComponent.h"

SceneComponent::SceneComponent(Scene* scene) : ResizableWindow("Scene", false), scene(scene)
{
    setContentOwned(new SceneOverlayComponent(this), false);
    //setOpaque(false);
}

void SceneComponent::resized() {
    ResizableWindow::resized();
    boundsInParent = getBoundsInParent();
    parentHeight = getParentHeight();
    scene->changeBounds(boundsInParent, parentHeight);
}

void SceneComponent::moved() {
    ResizableWindow::moved();
    boundsInParent = getBoundsInParent();
    parentHeight = getParentHeight();
    scene->changeBounds(boundsInParent, parentHeight);
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

void SceneComponent::buttonClicked(Button* button) {
    for(auto listener : listeners) {
        if(listener == deleteListener) {
            continue;
        }
        listener->sceneDeleteButtonClicked(this);
    }
    deleteListener->sceneDeleteButtonClicked(this);
}
//=================================================================
SceneComponent::SceneOverlayComponent::SceneOverlayComponent(SceneComponent* parent) : parent(parent)
{
    deleteButton.addListener(parent);
    addAndMakeVisible(deleteButton);
}

void SceneComponent::SceneOverlayComponent::resized()
{
    auto localBounds = getLocalBounds();
    deleteButton.setBounds(localBounds.getWidth() - 20, 0, 20, 20);
}

void SceneComponent::SceneOverlayComponent::mouseEnter(const MouseEvent& e) { 
    for(auto c : getAllComponents()) {
        c->setVisible(true);
    }
    
    parent->mouseEnter(e);
}

void SceneComponent::SceneOverlayComponent::mouseExit(const MouseEvent& e) { 
    for(auto c : getAllComponents()) {
        c->setVisible(false);
    }
    
    parent->mouseExit(e);
}
