#pragma once

#include <JuceHeader.h>
#include "Scene.h"
#include "ScenesRender.h"

using namespace juce;

class SceneComponent : public ResizableWindow, public StatefulObject::Sucker, public Timer, public NamedObject::Listener
{
public:
    SceneComponent(OpenGLContext& context, StatefulObject& parent);
    SceneComponent(StatefulObject& parent, StatefulObject::ObjectState sceneState);
    ~SceneComponent() { delete scene; }
    
    Scene& getScene() { return *scene; }
    bool isDetached() { return overlay->getDetachState(); }
    
    class Listener {
    public:
        virtual ~Listener(){}
        virtual void sceneMouseClicked(SceneComponent& sceneComponent){};
        virtual void sceneDetached(SceneComponent& component, bool isDetached){};
        virtual void sceneDeleting(SceneComponent& sceneComponent){};
    };
    
    void showOrHideOvelray(bool mustBeVisible) {overlay->showOrHideOvelray(mustBeVisible);}
    void addSceneListener(Listener* l) { listeners.add(l); }
    void setDeleter(Listener* l) { deleter = l; }
    void deleteScene();
    void detachScene(bool mustBeDetached, bool callWithButtonClick = false);
    
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
    
    void objectRenamed(const String& newName) override {
        setName(newName);
        overlay->setSceneName(newName);
    }
    
    void moved() override;
    void resized() override;
    void movedOrResized(bool moved);
    void paint(Graphics&) override {};
    
    void timerCallback() override;
    void mouseUp(const MouseEvent& e) override;
    
    void setAlwaysOnTop(bool mustBeAlwaysOnTop);
    void setFullscreen(bool mustBeFullscreen);
    void setPinState(bool shouldBePinned);
    void setBoundsFromState();
    void savePos(Rectangle<int>& bounds);
    void saveSize(Rectangle<int>& bounds);
    void saveCurrentBounds();
    void setResizable(bool state);
    
    class SceneOverlayComponent : public Component, public MouseInactivityDetector::Listener
    {
    public:
        SceneOverlayComponent(SceneComponent* parent);
        
        void setDetachState(bool state)         {setBtnState(detachButton, state);}
        void setAlwaysOnTopState(bool state)    {setBtnState(topButton, state);}
        void setPinnedState(bool state)         {setBtnState(pinButton, state);}
        void setFullscreenState(bool state)     {setBtnState(fullscreenButton, state);}
        void setDetachedMode(bool shouldBeOn);
        void setSceneName(const String& name)   {nameLabel.setText(name, NotificationType::dontSendNotification);}
        void setDragAllowed(bool shouldDrag)    { isDragAllowed = shouldDrag; };
        bool getFullscreenState() const         {return fullscreenButton.getToggleState();}
        bool getDetachState()                   {return detachButton.getToggleState();}
        
        void showOrHideOvelray(bool mustBeShown);
    private:
        void setBtnState(Button& b,bool s) {b.setToggleState(s, NotificationType::dontSendNotification);}
        
        void setControlsVisible(bool shouldBeVisible);
        void mouseEnter(const MouseEvent& e) override       {parent->mouseEnter(e);}
        void mouseDown(const MouseEvent& e) override        {parent->mouseDown(e);}
        void mouseUp(const MouseEvent& e) override          {parent->mouseUp(e);}
        void mouseDoubleClick(const MouseEvent& e) override {parent->mouseDoubleClick(e);}
        void mouseDrag(const MouseEvent& e) override;
        void mouseExit(const MouseEvent& e) override;
        void mouseMove(const MouseEvent& e) override;
        
        void mouseBecameActive() override;
        void mouseBecameInactive() override;
        void resized() override;
        
        bool isMouseOnControl();
        void initButton(Button& b, std::function<void()> f, bool enabled = true);
        
        struct DelegatingButton : public TextButton
        {DelegatingButton(const String& n) : TextButton(n){}
        void mouseExit(const MouseEvent& e) override {TextButton::mouseExit(e); getParentComponent()->mouseExit(e);}};
        
        struct DelegatingLabel : public Label
        {void mouseExit(const MouseEvent& e) override {Label::mouseExit(e); getParentComponent()->mouseExit(e);}};
        
        bool isMouseActive = true;
        bool isDragAllowed = false;
        bool isMouseMoveAllowed = false;
        
        MouseInactivityDetector inactivityDetector;
        static const int inactivityDelay = 3000; // ms
        
        SceneComponent* parent;
        DelegatingLabel nameLabel;
        DelegatingButton deleteButton{"X"};
        DelegatingButton detachButton{"d"};
        DelegatingButton topButton{"t"};
        DelegatingButton fullscreenButton{"f"};
        DelegatingButton pinButton{"p"};
        Array<Component*> getAllComponents() {
            return {&deleteButton,
                    &detachButton,
                    &topButton,
                    &fullscreenButton,
                    &pinButton,
                    &nameLabel};}
    };
    
    void init(StatefulObject& parent, StatefulObject::ObjectState* sceneState, OpenGLContext* context);
    
    Scene* scene = nullptr;
    SceneOverlayComponent* overlay = nullptr;
    
    Array<Listener*> listeners;
    Listener* deleter;
    
    std::unique_ptr<ScenesRender> ownRender;
    bool needToSaveSize = true;
};
