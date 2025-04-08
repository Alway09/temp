#pragma once

#include <JuceHeader.h>
#include "SceneComponent.h"
#include "ScenesMiniPanel.h"

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
    //bool hasComponents() {return scenesFlex.items.size() != 0;};
    bool makeVisible() {
        if(!isVisible()) {
            setVisible(true);
            scenesRender->attach();
        }
        return isVisible();
    }
    
private:
    void addFlexItem(Component* itemToControl);
    void refillFlex();
    
    void hideIfNothingToRender() {if(!scenesRender->getContext().isAttached()) setVisible(false);}
    
    FlexBox scenesFlex;
    OwnedArray<Scene> scenes;
    //ScenesMiniPanel panel;
    std::unique_ptr<ScenesRender> scenesRender;
    OwnedArray<SceneComponent> sceneComponents;
    OwnedArray<SceneComponent> detachedSceneComponents;
    SceneComponent::Listener* listener;
};
