#pragma once

#include <JuceHeader.h>
#include "SceneComponent.h"
//#include "ScenesRender.h"

using namespace juce;

class ScenesView : public Component, public StatefulObject, public SceneComponent::Listener, public Timer
{
public:
    ScenesView(StatefulObject& parent, SceneComponent::Listener* listener);
    ~ScenesView() { detachedSceneComponents.clearQuick(false); scenesRender.reset(); }
    
    void resized() override {scenesFlex.performLayout(getLocalBounds()); }
    
    void sceneMouseDown(Scene& scene) override {scenesRender->bringToFront(&scene);}
    void sceneMouseUp(Scene&) override { resized(); }
    void sceneDeleting(SceneComponent& sceneComponent) override;
    void sceneDetached(SceneComponent& component, bool detach) override;
    void timerCallback() override;
    
    void createScene(SceneComponent::Listener* parent);
    
private:
    void addFlexItem(Component* itemToControl);
    void refillFlex();
    
    FlexBox scenesFlex;
    OwnedArray<Scene> scenes;
    std::unique_ptr<ScenesRender> scenesRender;
    OwnedArray<SceneComponent> sceneComponents;
    OwnedArray<SceneComponent> detachedSceneComponents;
    SceneComponent::Listener* listener;
};
