#pragma once

#include <JuceHeader.h>

using namespace juce;

class SceneOverlayComponent : public Component
{
public:
    SceneOverlayComponent(Component* parent) : parent(parent)
    {
        addAndMakeVisible(addToDesktopButton);
    }
    
    void resized() override
    {
        auto localBounds = getLocalBounds();
        addToDesktopButton.setBounds(localBounds.getWidth() - 20, 0, 20, 20);
    }
    
    void mouseDoubleClick(const MouseEvent& e) override
    {
        parent->mouseDoubleClick(e);
    }
    
    void mouseEnter(const MouseEvent& e) override {
        parent->mouseEnter(e);
    }
    void mouseExit(const MouseEvent& e) override {
        parent->mouseExit(e);
    }
    
    void mouseDrag(const MouseEvent& e) override {
        parent->mouseDrag(e);
    }
    
    void mouseMove(const MouseEvent& e) override {
        parent->mouseMove(e);
    }
    
    void mouseDown(const MouseEvent& e) override {
        parent->mouseDown(e);
    }
    
    void mouseUp(const MouseEvent& e) override {
        parent->mouseUp(e);
    }
    
private:
    Component* parent;
    TextButton addToDesktopButton{"d"};
};
