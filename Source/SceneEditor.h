#pragma once

#include <JuceHeader.h>
#include "Scene.h"
#include "SceneObject.h"
#include "WaveformSceneObjectEditor.h"

using namespace juce;

class SceneEditor : public Component
{
public:
    SceneEditor() 
    {
        addAndMakeVisible(closeButton);
        addAndMakeVisible(headerLabel);
    }
    
    void resized() override {
        auto localBounds = getLocalBounds();
        
        auto headerBounds = localBounds.removeFromTop(getHeight() * 0.1f);
        
        closeButton.setBounds(headerBounds.removeFromLeft(50));
        headerLabel.setBounds(headerBounds);
        
        if(!objectEditors.isEmpty()) {
            for(auto editor : objectEditors) {
                editor->setBounds(localBounds);
            }
        }
    }
    
    void addListener(Button::Listener* listener) {
        closeButton.addListener(listener);
    }
    
    void attach(Scene* scene) {
        attachedTo = scene;
        headerLabel.setText(scene->getName(), NotificationType::dontSendNotification);
        
        objectEditors.clear();
        
        ValueTree sceneTree = scene->getValueTree();
        ValueTree::Iterator iter = sceneTree.begin();
        while(iter != sceneTree.end()) {
            int realisation = (*iter).getProperty(SceneObject::getTypeID());
            
            SceneObjectEditor* editor;
            if(realisation == SceneObjectRealisation::Waveform) {
                editor = new WaveformSceneObjectEditor(*iter);
                objectEditors.add(editor);
                addAndMakeVisible(editor);
            } else if(realisation == SceneObjectRealisation::Background) {
                
            } else {
                jassertfalse;
            }
            
            ++iter;
        }
        
        resized();
    }
    
    bool isAttachedTo(Scene* scene) { return scene == attachedTo; }
    
private:
    Scene* attachedTo = nullptr;
    TextButton closeButton;
    Label headerLabel;
    OwnedArray<SceneObjectEditor> objectEditors;
};
