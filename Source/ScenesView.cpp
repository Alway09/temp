#include "ScenesView.h"

ScenesView::ScenesView(StatefulObject& parent, SceneComponent::Listener* listener) : StatefulObject(parent, "ScenesView"), listener(listener)
{
    scenesRender.reset(new ScenesRender(*this));
    
    scenesFlex.flexWrap = FlexBox::Wrap::wrap;
    scenesFlex.justifyContent = FlexBox::JustifyContent::center;
    scenesFlex.flexDirection = FlexBox::Direction::column;
    
    startTimer(100);
}

void ScenesView::timerCallback() {
    if(isVisible()) {  // context initialized
        if(hasChildren()) {
            auto statesArray = getChildrenStates();
            
            for(auto state : statesArray) {
                Scene* scene = new Scene(*this, state, scenesRender->getContext());
                scenes.add(scene);
                SceneComponent* sceneComponent = new SceneComponent(scene);
                sceneComponents.add(sceneComponent);
                sceneComponent->addSceneListener(listener);
                
                sceneComponent->setResizable(true, true);
                
                scenesRender->addScene(scene, true);
                //sceneComponent->addSceneListener(scenesRender.get());
                sceneComponent->addSceneListener(this);
                sceneComponent->setDeleterListener(this);
                addAndMakeVisible(sceneComponent);
                
                addFlexItem(sceneComponent);
            }
            
            resized();
        }
        
        stopTimer();
    }
}

void ScenesView::sceneDeleteButtonClicked(SceneComponent* sceneComponent) {
    Scene* scene = sceneComponent->getScene();
    scenesRender->removeScene(scene);
    sceneComponents.removeObject(sceneComponent);
    scenes.removeObject(scene);
    
    refillFlex(nullptr);
}

void ScenesView::refillFlex(SceneComponent* except) {
    scenesFlex.items.clear();
    for(auto sceneComponent : sceneComponents) {
        if(sceneComponent != except) {
            addFlexItem(sceneComponent);
        }
    }
    resized();
}

void ScenesView::sceneDetachButtonClicked(SceneComponent* component, bool detach) {
    if(detach) {
        scenesRender->removeScene(component->getScene());
        //scenes.removeObject(component->getScene(), false);
        //sceneComponents.removeObject(component, false);
        //removeChildComponent(component);
        refillFlex(component);
    } else {
        
    }
}

void ScenesView::createScene(SceneComponent::Listener* parent) {
    Scene* scene = new Scene(*this, scenesRender->getContext());
    scene->createObject(SceneObjectRealisation::Background);
    scene->createObject(SceneObjectRealisation::Waveform);
    scenes.add(scene);
    SceneComponent* sceneComponent = new SceneComponent(scene);
    sceneComponents.add(sceneComponent);
    sceneComponent->addSceneListener(parent);
    
    sceneComponent->setResizable(true, true);
    
    scenesRender->addScene(scene, true);
    //sceneComponent->addSceneListener(scenesRender.get());
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
