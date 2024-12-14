#pragma once

#include <JuceHeader.h>
#include "SceneComponent.h"
#include "ScenesRender.h"
#include "SamplesHolderHolder.h"
#include "SceneManager.h"

using namespace juce;

class ScenesView : public Component, public SceneComponent::Listener
{
public:
    ScenesView(ValueTree treeAttachTo)
    {
        scenesRender.reset(new ScenesRender(*this));
        
        valueTree = treeAttachTo.getOrCreateChildWithName(identifier, nullptr);
        sceneManager.reset(new SceneManager(valueTree));
        
        scenesFlex.flexWrap = FlexBox::Wrap::wrap;
        scenesFlex.justifyContent = FlexBox::JustifyContent::center;
        scenesFlex.flexDirection = FlexBox::Direction::column;
    }
    
    ~ScenesView()
    {
        scenesRender.reset();
    }
    
    void resized() override
    {
        scenesFlex.performLayout(getLocalBounds());
    }
    
    void sceneMouseUp(Scene*) override {
        resized();
    }
    
    void createScene(SceneComponent::Listener* parent) {
        static int min = 200;
        static int amountLimit = 6;
        
        Uuid tmp = sceneManager->createScene();
        Scene* scene = sceneManager->getScene(tmp);
        SceneComponent* sceneComponent = new SceneComponent(scene);
        sceneComponents.add(sceneComponent);
        sceneComponent->addSceneListener(parent);
        
        sceneComponent->setResizable(true, true);
        
        scenesRender->addScene(scene);
        sceneComponent->addSceneListener(scenesRender.get());
        sceneComponent->addSceneListener(this);
        addAndMakeVisible(sceneComponent);
        
        if(scenesFlex.items.size() < amountLimit)
        {
            FlexItem fi{*sceneComponent};
            scenesFlex.items.add(fi.withMinWidth(min).withMinHeight(min).withFlex(1.f, 4.f, 1.f));
            
            resized();
        } else {
            DBG("Scenes limit exceeded");
        }
    }
    
private:
    FlexBox scenesFlex;
    std::unique_ptr<SceneManager> sceneManager;
    std::unique_ptr<ScenesRender> scenesRender;
    OwnedArray<SceneComponent> sceneComponents;
    
    ValueTree valueTree;
    Identifier identifier{"ScenesView"};
};
