#pragma once

#include <JuceHeader.h>
#include "Scene.h"
#include "SceneObject.h"
#include "WaveformSceneObjectEditor.h"
#include "BackgroundSceneObjectEditor.h"

using namespace juce;

class SceneEditor : public PropertyPanel, public Label::Listener
{
public:
    SceneEditor();
    
    void resized() override;
    
    void addCloseButtonListener(Button::Listener* listener) { header.addCloseButtonListener(listener); }
    
    void attach(Scene* scene);
    bool isAttachedTo(Scene* scene) { return scene == attachedTo; }
private:
    void labelTextChanged(Label* l) override {
        String newName = l->getText();
        
        try {
            attachedTo->rename(newName);
        } catch (const NamedObject::NameException& e) {
            if(e.hasMessage()) {
                //DBG(e.getMessage());
                AlertWindow::showAsync(MessageBoxOptions::makeOptionsOk(MessageBoxIconType::WarningIcon, "Warning", e.getMessage()), nullptr);
            }
            
            header.setSceneName(attachedTo->getName());
        }
        
    }
    
    void clear() {
        int amount = getSectionNames().size();
        for(int i = 0; i < amount; ++i) {
            removeSection(0);
        }
        objectEditors.clear();
    }
    
    class Header : public Component
    {
    public:
        Header() {
            sceneNameLabel.setEditable(false, true, true);
            
            addAndMakeVisible(sceneNameLabel);
            addAndMakeVisible(closeButton);
        }
        
        void resized() override {
            auto localBounds = getLocalBounds();
            
            closeButton.setBounds(localBounds.removeFromLeft(50));
            sceneNameLabel.setBounds(localBounds);
        }
        
        void addCloseButtonListener(Button::Listener* listener) { closeButton.addListener(listener); }
        void addSceneNameLabelListener(Label::Listener* listener) { sceneNameLabel.addListener(listener); }
        
        void setSceneName(String name) { sceneNameLabel.setText(name, NotificationType::dontSendNotification); }
    private:
        TextButton closeButton;
        Label sceneNameLabel;
    };
    
    Header header;
    Scene* attachedTo = nullptr;
    OwnedArray<SceneObjectEditor> objectEditors;
};
