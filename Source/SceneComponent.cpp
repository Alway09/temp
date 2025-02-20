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

void SceneComponent::deleteButtonClicked() {
    for(auto listener : listeners) {
        if(listener == deleteListener) {
            continue;
        }
        listener->sceneDeleteButtonClicked(this);
    }
    deleteListener->sceneDeleteButtonClicked(this);
}

void SceneComponent::detachButtonClicked(bool detach) {
    for(auto listener : listeners) {
        listener->sceneDetachButtonClicked(this, detach);
    }
    
    if(detach) {
        addToDesktop();
        ownRender.reset(new ScenesRender(*this));
        scene->replaceContext(ownRender.get()->getContext());
        ownRender->addScene(scene, false);
        //addToDesktop();
    } else {
        ownRender.reset();
    }
}
//=================================================================
SceneComponent::SceneOverlayComponent::SceneOverlayComponent(SceneComponent* parent) : parent(parent)
{
    deleteButton.onClick = [this](){this->parent->deleteButtonClicked();};
    detachButton.onClick = [this](){this->parent->detachButtonClicked(true);};
    addAndMakeVisible(deleteButton);
    addAndMakeVisible(detachButton);
}

void SceneComponent::SceneOverlayComponent::resized()
{
    auto localBounds = getLocalBounds();
    deleteButton.setBounds(localBounds.getWidth() - 20, 0, 20, 20);
    detachButton.setBounds(localBounds.getWidth() - 40, 0, 20, 20);
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
