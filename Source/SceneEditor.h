#pragma once

#include <JuceHeader.h>
#include "Scene.h"
#include "SceneObjectEditorsHolder.h"

using namespace juce;

class SceneEditor : public Component, public Label::Listener, public SceneObjectEditorsHolder::Listener
{
public:
    SceneEditor();
 
    void addCloseButtonListener(Button::Listener* listener) { header.addCloseButtonListener(listener); }
    
    void setCloseButtonEnabled(bool shouldBeEnabled) {header.setCloseButtonEnabled(shouldBeEnabled);}
    void attach(Scene* scene);
    bool isAttachedTo(Scene* scene) { return scene == attachedTo; }
    int getCurrentWidth() const { return isVisible() ? normalWidth : 0;};
    
private:
    void resized() override;
    void labelTextChanged(Label* sceneNameLabel) override; // rename scene
    
    SceneObject* addObject(SceneObjectRealisation r) override { return attachedTo->createObject(r); }
    void deleteObject(SceneObject* obj) override { attachedTo->deleteObject(obj); }
    void objectsReorder(int oldIdx, int newIdx) override { attachedTo->objectsReorder(oldIdx, newIdx); }
    
    class Header : public Component
    {
    public:
        Header();
        
        void resized() override;
        
        void addCloseButtonListener(Button::Listener* listener) { closeButton.addListener(listener); }
        void addSceneNameLabelListener(Label::Listener* listener) { sceneNameLabel.addListener(listener); }
        void setSceneName(String name) { sceneNameLabel.setText(name, NotificationType::dontSendNotification); }
        void setCloseButtonEnabled(bool shouldBeEnabled) {closeButton.setEnabled(shouldBeEnabled);}
    private:
        TextButton closeButton{"X"};
        Label sceneNameLabel;
    };
    
    Header header;
    SceneObjectEditorsHolderViewport viewport;
    SceneObjectEditorsHolder editorsHolder;
    Scene* attachedTo = nullptr;
    static const int normalWidth = 300;
};
