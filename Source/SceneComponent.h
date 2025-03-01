#pragma once

#include <JuceHeader.h>
#include "Scene.h"
#include "ScenesRender.h"

using namespace juce;

class SceneComponent : public ResizableWindow, public StatefulObject::Sucker, public Timer
{
public:
    SceneComponent(Scene& scene);
    ~SceneComponent() {}
    
    void resized() override;
    void timerCallback() override;
    
    Scene& getScene() { return scene; }
    bool isDetached() { return overlay->getDetachState(); }
    
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
    struct IDs {
        inline static const Identifier posX{"posX"};
        inline static const Identifier posY{"posY"};
        inline static const Identifier width{"width"};
        inline static const Identifier height{"height"};
        inline static const Identifier isDetached{"isDetached"};
        inline static const Identifier isAlwaysOnTop{"isOnTop"};
        inline static const Identifier isPinned{"isPinned"};
        inline static const Identifier isFullscreen{"isFullscreen"};
    };
    
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
    void setBoundsFromState();
    
    class SceneOverlayComponent : public Component, public MouseInactivityDetector::Listener
    {
    public:
        SceneOverlayComponent(SceneComponent* parent);
        bool getDetachState() { return detachButton.getToggleState(); }
        void setDetachState(bool state) { detachButton.setToggleState(state, NotificationType::dontSendNotification); }
        void setAlwaysOnTopState(bool state) {topButton.setToggleState(state, NotificationType::dontSendNotification);}
        void setPinnedState(bool state) {pinButton.setToggleState(state, NotificationType::dontSendNotification);}
        void setFullscreenState(bool state) {fullscreenButton.setToggleState(state, NotificationType::dontSendNotification);}
        bool getFullscreenState() const {return fullscreenButton.getToggleState();}
        void setSceneName(const String& name) {nameLabel.setText(name, NotificationType::dontSendNotification);}
        void setDetachedMode(bool shouldBeOn);
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
    
    bool tmp = false;
    std::unique_ptr<ScenesRender> ownRender;
};
