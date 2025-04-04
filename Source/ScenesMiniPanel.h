#pragma once

#include <JuceHeader.h>

using namespace juce;

class ScenesMiniPanel : public Component
{
public:
    ScenesMiniPanel() {
        addAndMakeVisible(expandButton);
        addAndMakeVisible(panel);
    }
    
    void resized() override {
        auto bounds = getLocalBounds();
        expandButton.setBounds(bounds.removeFromTop(getHeight() * 0.1));
        panel.setBounds(bounds);
    }
    
private:
    
    class Panel : public Component
    {
        
    };
    
    TextButton expandButton;
    Panel panel;
};
