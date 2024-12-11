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
