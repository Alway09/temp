#pragma once

#include <JuceHeader.h>
#include "Scene.h"
#include "SceneObject.h"
#include "WaveformSceneObjectEditor.h"

using namespace juce;

class SceneEditor : public Component
{
public:
    SceneEditor();
    
    void resized() override;
    
    void addListener(Button::Listener* listener) { closeButton.addListener(listener); }
    
    void attach(Scene* scene);
    bool isAttachedTo(Scene* scene) { return scene == attachedTo; }
private:
    Scene* attachedTo = nullptr;
    TextButton closeButton;
    Label headerLabel;
    OwnedArray<SceneObjectEditor> objectEditors;
};
