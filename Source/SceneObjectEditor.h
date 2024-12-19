#pragma once

#include <JuceHeader.h>

using namespace juce;

class SceneObjectEditor : public Component
{
public:
    SceneObjectEditor(ValueTree treeEditTo) : valueTree(treeEditTo){}
    
    void setSelected(bool shouldBeSelected) { selected = shouldBeSelected; }
    
    void paint(Graphics& g) override {
        if(selected) {
            g.setColour(Colours::red);
        } else {
            g.setColour(Colours::white);
        }
        
        g.drawRect(getLocalBounds());
    }
protected:
    ValueTree valueTree;
private:
    bool selected = false;
};
