#pragma once

#include <JuceHeader.h>
#include "Scene.h"

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
    }
    
    void addListener(Button::Listener* listener) {
        closeButton.addListener(listener);
    }
    
    void attach(Scene* scene) {
        String name{scene->getIdentifier().toString().trim()};
        headerLabel.setText(name, NotificationType::dontSendNotification);
    }
    
private:
    TextButton closeButton;
    Label headerLabel;
};
