#pragma once

#include <JuceHeader.h>
#include "SceneObject.h"

using namespace juce;

class SceneObjectEditor : public Component
{
public:
    SceneObjectEditor(SceneObject& objectEditTo) : object(objectEditTo){
        addAndMakeVisible(header);
    }
    
    ~SceneObjectEditor() { // PropertyComponents deletes with pannel
        controls.clear();
    }
    
    void resized() override {
        header.setBounds(getLocalBounds());
    }
    
    const Array<PropertyComponent*>& getControls() { return controls; };
protected:
    SceneObject& object;
    Array<PropertyComponent*> controls;
private:
    class Header : public Component
    {
    public:
        Header() {
            sceneObjectNameLabel.setEditable(false, true, true);
            
            addAndMakeVisible(sceneObjectNameLabel);
            addAndMakeVisible(expandButton);
        }
        
        void resized() override {
            auto localBounds = getLocalBounds();
            
            expandButton.setBounds(localBounds.removeFromLeft(50));
            sceneObjectNameLabel.setBounds(localBounds);
        }
        
        //void addCloseButtonListener(Button::Listener* listener) { closeButton.addListener(listener); }
        //void addSceneNameLabelListener(Label::Listener* listener) { sceneNameLabel.addListener(listener); }
        
        void setObjectName(String name) { sceneObjectNameLabel.setText(name, NotificationType::dontSendNotification); }
    private:
        TextButton expandButton;
        Label sceneObjectNameLabel;
    };
    
    Header header;
};
