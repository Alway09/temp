#pragma once

#include <JuceHeader.h>
#include "Scene.h"
#include "ScenesRender.h"

using namespace juce;

class SceneComponent : public ResizableWindow
{
public:
    SceneComponent(Scene& scene);
    ~SceneComponent() {}
    
    void resized() override;
    
    Scene& getScene() { return scene; }
    
    class Listener {
    public:
        virtual ~Listener(){}
        virtual void sceneMouseDown(Scene& scene){};
        virtual void sceneMouseUp(Scene& scene){};
        virtual void sceneMouseClicked(Scene& scene){};
        virtual void sceneDetached(SceneComponent& component, bool isDetached){};
        virtual void sceneDeleting(SceneComponent& sceneComponent){};
    };
    
    void addSceneListener(Listener* l) { listeners.add(l); }
    void setDeleter(Listener* l) { deleter = l; }
    
private:
    void moved() override;
    void paint(Graphics&) override {};
    
    void mouseDown(const MouseEvent& e) override;
    void mouseUp(const MouseEvent& e) override;
    
    void movedOrResized(bool moved);
    void deleteScene();
    void detachScene(bool mustBeDetached);
    void setAlwaysOnTop(bool mustBeAlwaysOnTop);
    void setFullscreen(bool mustBeFullscreen);
    void setPinState(bool shouldBePinned);
    
    class SceneOverlayComponent : public Component, public MouseInactivityDetector::Listener
    {
    public:
        SceneOverlayComponent(SceneComponent* parent);
        void setSceneName(const String& name) {nameLabel.setText(name, NotificationType::dontSendNotification);}
    private:
        void mouseEnter(const MouseEvent& e) override {parent->mouseEnter(e);}
        void mouseDown(const MouseEvent& e) override { parent->mouseDown(e); }
        void mouseUp(const MouseEvent& e) override { parent->mouseUp(e); }
        void mouseDoubleClick(const MouseEvent& e) override { parent->mouseDoubleClick(e); }
        void mouseDrag(const MouseEvent& e) override {if(!pinButton.getToggleState()) parent->mouseDrag(e);}
        void mouseExit(const MouseEvent& e) override;
        void mouseMove(const MouseEvent& e) override;
        
        void mouseBecameActive() override;
        void mouseBecameInactive() override;
        void resized() override;
        
        void setDetachedMode(bool shouldBeOn);
        void setControlsVisible(bool shouldBeVisible);
        bool isMouseOnControl();
        
        Array<Component*> getAllComponents() {return {&deleteButton, &detachButton, &topButton, &fullscreenButton, &pinButton, &nameLabel};}
        
        struct DelegatingButton : public TextButton
        {DelegatingButton(const String& n) : TextButton(n){}
        void mouseExit(const MouseEvent& e) override {TextButton::mouseExit(e); getParentComponent()->mouseExit(e);}};
        
        struct DelegatingLabel : public Label
        {void mouseExit(const MouseEvent& e) override {Label::mouseExit(e); getParentComponent()->mouseExit(e);}};
        
        bool isVisible = false;
        bool isMouseActive = true;
        
        SceneComponent* parent;
        DelegatingLabel nameLabel;
        DelegatingButton deleteButton{"X"};
        DelegatingButton detachButton{"d"};
        DelegatingButton topButton{"t"};
        DelegatingButton fullscreenButton{"f"};
        DelegatingButton pinButton{"p"};
        
        MouseInactivityDetector inactivityDetector;
        static const int inactivityDelay = 3000; // ms
    };
    
    Scene& scene;
    SceneOverlayComponent* overlay = nullptr;
    
    Array<Listener*> listeners;
    Listener* deleter;
    
    std::unique_ptr<ScenesRender> ownRender;
};
