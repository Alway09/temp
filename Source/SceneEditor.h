#pragma once

#include <JuceHeader.h>
#include "Scene.h"
#include "SceneObject.h"
#include "WaveformSceneObjectEditor.h"
#include "SceneEditorModel.h"

using namespace juce;

class SceneEditor : public ListBox, public Button::Listener
{
public:
    SceneEditor();
    
    void resized() override;
    
    void buttonClicked(Button* b) override {
        if(closeButtonListener) {
            closeButtonListener->buttonClicked(b);
        }
    }
    void setCloseButtonListener(Button::Listener* listener) { closeButtonListener = listener; }
    
    void attach(Scene* scene);
    bool isAttachedTo(Scene* scene) { return scene == attachedTo; }
private:
    class Header : public Component
    {
    public:
        Header(Button::Listener* closeButtonListener) {
            closeButton.addListener(closeButtonListener);
            addAndMakeVisible(headerLabel);
            addAndMakeVisible(closeButton);
            
            setBounds(0, 0, 0, 50);
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
    
    Header* getHeader() {
        return dynamic_cast<Header*>(getHeaderComponent());
    }
    
    //Header header;
    std::unique_ptr<Header> header;
    Scene* attachedTo = nullptr;
    
    //OwnedArray<SceneObjectEditor> objectEditors;
    SceneEditorModel model;
    Button::Listener* closeButtonListener = nullptr;
};
