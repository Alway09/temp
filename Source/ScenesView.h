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
    ScenesView(ValueTree treeAttachTo);
    ~ScenesView() { scenesRender.reset(); }
    
    void resized() override { scenesFlex.performLayout(getLocalBounds()); }
    
    void sceneMouseUp(Scene*) override { resized(); }
    void sceneDeleteButtonClicked(SceneComponent* sceneComponent) override;
    
    void createScene(SceneComponent::Listener* parent);
    
private:
    void addFlexItem(Component* itemToControl);
    
    FlexBox scenesFlex;
    std::unique_ptr<SceneManager> sceneManager;
    std::unique_ptr<ScenesRender> scenesRender;
    OwnedArray<SceneComponent> sceneComponents;
    
    ValueTree valueTree;
    Identifier identifier{"ScenesView"};
};
