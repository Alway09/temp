#pragma once

#include <JuceHeader.h>
#include "SceneComponent.h"
#include "ScenesRender.h"

using namespace juce;

class ScenesView : public Component, public SceneComponent::Listener
{
public:
    ScenesView()
    {
        scenesRender.reset(new ScenesRender(*this));
        
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
    
    void addScene(SceneComponent* scene) {
        static int min = 200;
        static int amountLimit = 6;
        
        scenesRender->addScene(scene->getScene());
        scene->addSceneListener(scenesRender.get());
        scene->addSceneListener(this);
        addAndMakeVisible(scene);
        
        if(scenesFlex.items.size() < amountLimit)
        {
            FlexItem fi{*scene};
            scenesFlex.items.add(fi.withMinWidth(min).withMinHeight(min).withFlex(1.f, 4.f, 1.f));
            
            resized();
        } else {
            DBG("Scenes limit exceeded");
        }
    }
    
private:
    FlexBox scenesFlex;
    std::unique_ptr<ScenesRender> scenesRender;
};
