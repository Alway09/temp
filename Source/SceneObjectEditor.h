#pragma once

#include <JuceHeader.h>
#include "SceneObject.h"

using namespace juce;

class SceneObjectEditor : public Component, public Label::Listener
{
public:
    SceneObjectEditor(SceneObject& objectEditTo) : object(objectEditTo) {
        header.setObjectName(objectEditTo.getName());
        header.addsceneObjectNameLabelListener(this);
        addAndMakeVisible(header);
    }
    
    ~SceneObjectEditor() {
        //controls.clear();
    }
    
    void resized() override {
        auto bounds = getBoundingRectangle(0);
        
        header.setBounds(bounds.removeFromTop(header.getHeight()));
        for(auto c : controls) {
            c->setBounds(bounds.removeFromTop(controlHeight));
        }
    }
    
    Rectangle<int> getBoundingRectangle(int Y) { return Rectangle<int>{getParentWidth(), header.getHeight() + controls.size() * controlHeight}.withY(Y); }
    
    const OwnedArray<PropertyComponent>& getControls() { return controls; };
protected:
    SceneObject& object;
    OwnedArray<PropertyComponent> controls;
private:
    void labelTextChanged(Label* l) override {
        String newName = l->getText();
        
        try {
            object.rename(newName);
        } catch (const NamedObject::NameException& e) {
            if(e.hasMessage()) {
                AlertWindow::showAsync(MessageBoxOptions::makeOptionsOk(MessageBoxIconType::WarningIcon, "Warning", e.getMessage()), nullptr);
            }
        } catch (const StatefulObject::StateException& e) {
            AlertWindow::showAsync(MessageBoxOptions::makeOptionsOk(MessageBoxIconType::WarningIcon, "Warning", e.getMessage()), nullptr);
        }
        
        header.setObjectName(object.getName());
    }
    
    class Header : public Component
    {
    public:
        Header() {
            sceneObjectNameLabel.setEditable(false, true, true);
            
            addAndMakeVisible(sceneObjectNameLabel);
            addAndMakeVisible(expandButton);
        }
        
        void resized() override {
            auto localBounds = getLocalBounds().withHeight(height);
            
            expandButton.setBounds(localBounds.removeFromLeft(50));
            sceneObjectNameLabel.setBounds(localBounds);
        }
        
        int getHeight() const { return height; }
        
        //void addCloseButtonListener(Button::Listener* listener) { closeButton.addListener(listener); }
        void addsceneObjectNameLabelListener(Label::Listener* listener) { sceneObjectNameLabel.addListener(listener); }
        
        void setObjectName(String name) { sceneObjectNameLabel.setText(name, NotificationType::dontSendNotification); }
    private:
        TextButton expandButton;
        Label sceneObjectNameLabel;
        int height = 20;
    };
    
    Header header;
    int controlHeight = 40;
};
