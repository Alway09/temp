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
                Scene* scene = new Scene(*this, state);
                scenes.add(scene);
                SceneComponent* sceneComponent = new SceneComponent(*scene);
                sceneComponents.add(sceneComponent);
                sceneComponent->addSceneListener(listener);
                sceneComponent->addSceneListener(this);
                sceneComponent->setDeleter(this);
                
                if(!sceneComponent->isDetached()) {
                    scene->replaceContext(scenesRender->getContext(), false);
                    scenesRender->addScene(scene);
                    addFlexItem(sceneComponent);
                } else {
                    detachedSceneComponents.add(sceneComponent);
                }
                
                addAndMakeVisible(sceneComponent);
            }
            
            resized();
        }
        stopTimer();
    }
}

void ScenesView::sceneDeleting(SceneComponent& sceneComponent) {
    Scene& scene = sceneComponent.getScene();
    scenesRender->removeScene(&scene);
    sceneComponents.removeObject(&sceneComponent);
    scenes.removeObject(&scene);
    
    refillFlex();
}

void ScenesView::refillFlex() {
    scenesFlex.items.clear();
    for(auto sceneComponent : sceneComponents) {
        if(!detachedSceneComponents.contains(sceneComponent)) {
            addFlexItem(sceneComponent);
        }
    }
    resized();
}

void ScenesView::sceneDetached(SceneComponent& component, bool detach) {
    if(detach) {
        scenesRender->removeScene(&component.getScene());
        if(!detachedSceneComponents.contains(&component))
            detachedSceneComponents.add(&component);
        refillFlex();
    } else {
        addAndMakeVisible(component);
        detachedSceneComponents.removeObject(&component, false);
        component.getScene().replaceContext(scenesRender->getContext(), false);
        scenesRender->addScene(&component.getScene());
        refillFlex();
    }
}

void ScenesView::createScene(SceneComponent::Listener* parent) {
    static int amountLimit = 6;
    
    if(scenesFlex.items.size() < amountLimit) {
        Scene* scene = new Scene(scenesRender->getContext(), *this);
        scene->createObject(SceneObjectRealisation::Background);
        scene->createObject(SceneObjectRealisation::Waveform);
        scenes.add(scene);
        SceneComponent* sceneComponent = new SceneComponent(*scene);
        sceneComponents.add(sceneComponent);
        sceneComponent->addSceneListener(parent);
        scenesRender->addScene(scene);
        sceneComponent->addSceneListener(this);
        sceneComponent->setDeleter(this);
        addAndMakeVisible(sceneComponent);
        
        addFlexItem(sceneComponent);
        
        resized();
    } else {
        DBG("Scenes limit exceeded");
    }
    
    
}

void ScenesView::addFlexItem(Component* itemToControl) {
    static int min = 200;
    FlexItem fi{*itemToControl};
    scenesFlex.items.add(fi.withMinWidth(min).withMinHeight(min).withFlex(1.f, 4.f, 1.f));
}
