#pragma once

#include <JuceHeader.h>
#include "Scene.h"
#include "SceneObject.h"
#include "WaveformSceneObjectEditor.h"

using namespace juce;

class SceneEditor : public PropertyPanel
{
public:
    SceneEditor();
    
    void resized() override;
    
    void addCloseButtonListener(Button::Listener* listener) { header.getCloseButton().addListener(listener); }
    
    void attach(Scene* scene);
    bool isAttachedTo(Scene* scene) { return scene == attachedTo; }
private:
    void clear() {
        for(int i = 0; i < getSectionNames().size(); ++i) {
            removeSection(i);
        }
        objectEditors.clear();
    }
    
    class Header : public Component
    {
    public:
        Header() {
            addAndMakeVisible(headerLabel);
            addAndMakeVisible(closeButton);
        }
        
        void resized() override {
            auto localBounds = getLocalBounds();
            
            closeButton.setBounds(localBounds.removeFromLeft(50));
            headerLabel.setBounds(localBounds);
        }
        
        
        void setText(String text) { headerLabel.setText(text, NotificationType::dontSendNotification); }
        Button& getCloseButton() { return closeButton; }
    private:
        TextButton closeButton;
        Label headerLabel{"Hello"};
    };
    
    Header header;
    Scene* attachedTo = nullptr;
    OwnedArray<SceneObjectEditor> objectEditors;
};
