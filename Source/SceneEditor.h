#pragma once

#include <JuceHeader.h>
#include "Scene.h"
#include "SceneObjectEditorsHolder.h"

using namespace juce;

class SceneEditor : public Component, public Label::Listener
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
                AlertWindow::showAsync(MessageBoxOptions::makeOptionsOk(MessageBoxIconType::WarningIcon, "Warning", e.getMessage()), nullptr);
            }
        } catch (const StatefulObject::StateException& e) {
            AlertWindow::showAsync(MessageBoxOptions::makeOptionsOk(MessageBoxIconType::WarningIcon, "Warning", e.getMessage()), nullptr);
        }
        
        header.setSceneName(attachedTo->getName());
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
    SceneObjectEditorsHolder editorsHolder;
    Scene* attachedTo = nullptr;
};
