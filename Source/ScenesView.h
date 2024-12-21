#pragma once

#include <JuceHeader.h>
#include "SceneComponent.h"
#include "ScenesRender.h"
#include "SamplesHolderHolder.h"
#include "SceneManager.h"

using namespace juce;

class ScenesView : public Component, public StatefulObject, public SceneComponent::Listener
{
public:
    ScenesView(StatefulObject& parent);
    ~ScenesView() { scenesRender.reset(); }
    
    void resized() override { scenesFlex.performLayout(getLocalBounds()); }
    
    void sceneMouseUp(Scene*) override { resized(); }
    void sceneDeleteButtonClicked(SceneComponent* sceneComponent) override;
    
    void createScene(SceneComponent::Listener* parent);
    
private:
    void addFlexItem(Component* itemToControl);
    
    static void createObject(Scene* scene, SceneObjectRealisation realisation) {
        switch (realisation) {
            case SceneObjectRealisation::Waveform:
            {
                WaveformSceneObject * wf = new WaveformSceneObject(*scene);
                scene->createObject(wf);
                break;
            }
            case SceneObjectRealisation::Background:
            {
                BackgroundSceneObject* bg = new BackgroundSceneObject(*scene);
                scene->createObject(bg);
                break;
            }
        }
    }
    
    FlexBox scenesFlex;
    //std::unique_ptr<SceneManager> sceneManager;
    OwnedArray<Scene> scenes;
    std::unique_ptr<ScenesRender> scenesRender;
    OwnedArray<SceneComponent> sceneComponents;
    
    //ValueTree valueTree;
    Identifier identifier{"ScenesView"};
};
