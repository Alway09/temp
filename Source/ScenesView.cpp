#include "ScenesView.h"

ScenesView::ScenesView(StatefulObject& parent) : StatefulObject(parent, "Global", "ScenesView")
{
    scenesRender.reset(new ScenesRender(*this));
    
    //valueTree = treeAttachTo.getOrCreateChildWithName(identifier, nullptr);
    //sceneManager.reset(new SceneManager(valueTree));
    
    scenesFlex.flexWrap = FlexBox::Wrap::wrap;
    scenesFlex.justifyContent = FlexBox::JustifyContent::center;
    scenesFlex.flexDirection = FlexBox::Direction::column;
}

void ScenesView::sceneDeleteButtonClicked(SceneComponent* sceneComponent) {
    Scene* scene = sceneComponent->getScene();
    scenesRender->removeScene(scene);
    sceneComponents.removeObject(sceneComponent);
    //sceneManager->deleteScene(scene->getUuidIdentifier());
    scenes.removeObject(scene);
    
    scenesFlex.items.clear();
    for(auto sceneComponent : sceneComponents) {
        addFlexItem(sceneComponent);
    }
    resized();
}

void ScenesView::createScene(SceneComponent::Listener* parent) {
    //Uuid tmp = sceneManager->createScene();
    //Scene* scene = sceneManager->getScene(tmp);
    Scene* scene = new Scene(*this);
    createObject(scene, SceneObjectRealisation::Background);
    createObject(scene, SceneObjectRealisation::Waveform);
    scenes.add(scene);
    SceneComponent* sceneComponent = new SceneComponent(scene);
    sceneComponents.add(sceneComponent);
    sceneComponent->addSceneListener(parent);
    
    sceneComponent->setResizable(true, true);
    
    scenesRender->addScene(scene);
    sceneComponent->addSceneListener(scenesRender.get());
    sceneComponent->addSceneListener(this);
    sceneComponent->setDeleterListener(this);
    addAndMakeVisible(sceneComponent);
    
    addFlexItem(sceneComponent);
    
    resized();
}

void ScenesView::addFlexItem(Component* itemToControl) {
    static int min = 200;
    static int amountLimit = 6;
    
    if(scenesFlex.items.size() < amountLimit)
    {
        FlexItem fi{*itemToControl};
        scenesFlex.items.add(fi.withMinWidth(min).withMinHeight(min).withFlex(1.f, 4.f, 1.f));
    }  else {
        DBG("Scenes limit exceeded");
    }
}
