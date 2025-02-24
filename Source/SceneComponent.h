#pragma once

#include <JuceHeader.h>
#include "Scene.h"
#include "ScenesRender.h"

using namespace juce;

class SceneComponent : public ResizableWindow
{
public:
    SceneComponent(Scene* scene);
    ~SceneComponent() { /*shutdown();*/ }
    
    void resized() override;
    void moved() override;
    void paint(Graphics&) override {};
    
    void mouseDown(const MouseEvent& e) override;
    void mouseUp(const MouseEvent& e) override;
    
    void deleteButtonClicked();
    void detachButtonClicked(bool detach);
    void topButtonClicked(bool state);
    void fullscreenButtonClicked(bool state);
    void pinButtonClicked(bool state);
    
    // void shutdown() { scene->shutdown(); }
    void render() { scene->render(); }
    Scene* getScene() { return scene; }
    
    class Listener {
    public:
        virtual ~Listener(){}
        virtual void sceneMouseDown(Scene* scene){};
        virtual void sceneMouseUp(Scene* scene){};
        virtual void sceneMouseClicked(Scene* scene){};
        virtual void sceneDetachButtonClicked(SceneComponent* component, bool detach){};
        virtual void sceneDeleteButtonClicked(SceneComponent* sceneComponent){};
    };
    
    void addSceneListener(Listener* l) { listeners.add(l); }
    void setDeleterListener(Listener* l) { deleteListener = l; }
    
    
    
private:
    class SceneOverlayComponent : public Component, public MouseInactivityDetector::Listener
    {
    public:
        SceneOverlayComponent(SceneComponent* parent);
        
        void resized() override;
        
        void mouseEnter(const MouseEvent& e) override;
        void mouseExit(const MouseEvent& e) override;
        void mouseDoubleClick(const MouseEvent& e) override { parent->mouseDoubleClick(e); }
        void mouseDrag(const MouseEvent& e) override {if(!pinButton.getToggleState()) parent->mouseDrag(e);}
        void mouseMove(const MouseEvent& e) override {
            if(isMouseActive) {
                setControlsVisible(isMouseOver());
                parent->mouseMove(e);
            }
             
        }
        void mouseDown(const MouseEvent& e) override { parent->mouseDown(e); }
        void mouseUp(const MouseEvent& e) override { parent->mouseUp(e); }
        
        void setEmbeddedButtonsEnabled(bool shouldBeEnabled) {
            topButton.setToggleState(!shouldBeEnabled, NotificationType::dontSendNotification);
            topButton.setEnabled(shouldBeEnabled);
            
            setFullscreenState(!shouldBeEnabled);
            fullscreenButton.setEnabled(shouldBeEnabled);
            
            pinButton.setToggleState(!shouldBeEnabled, NotificationType::dontSendNotification);
            pinButton.setEnabled(shouldBeEnabled);
        }
        
        void mouseBecameActive() override {
            isMouseActive = true;
            setMouseCursor(MouseCursor(MouseCursor::StandardCursorType::NormalCursor));
            setControlsVisible(true);
        }
        
        void mouseBecameInactive() override {
            if(!isMouseOnControl()) {
                isMouseActive = false;
                setMouseCursor(MouseCursor(MouseCursor::StandardCursorType::NoCursor));
                setControlsVisible(false);
            }
        }
        
        void setFullscreenState(bool state) {
            fullscreenButton.setToggleState(state, NotificationType::dontSendNotification);
        }
        
        void setPinState(bool state) {
            pinButton.setToggleState(state, NotificationType::dontSendNotification);
        }
        
        void setSceneName(const String& name) {
            nameLabel.setText(name, NotificationType::dontSendNotification);
        }
        
        void setControlsVisible(bool shouldBeVisible) {
            if(isVisible == shouldBeVisible) return;
            
            for(auto c : getAllComponents()) {
                c->setVisible(shouldBeVisible);
            }
            isVisible = shouldBeVisible;
        }
        
        bool isMouseOnControl() {
            for (auto c : getAllComponents()) {
                if(c->isMouseOver()) {
                    return true;
                }
            }
            return false;
        }
        
    private:
        Array<Component*> getAllComponents() { return {&deleteButton, &detachButton, &topButton, &fullscreenButton, &pinButton, &nameLabel}; }
        
        struct DelegatingButton : public TextButton
        {
            DelegatingButton(const String& n) : TextButton(n) {}
            void mouseExit(const MouseEvent& e) override {TextButton::mouseExit(e); getParentComponent()->mouseExit(e); }
        };
        
        struct DelegatingLabel : public Label
        {
            void mouseExit(const MouseEvent& e) override {Label::mouseExit(e); getParentComponent()->mouseExit(e); }
        };
        
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
        const int inactivityDelay = 3000; // ms
    };
    
    Scene* scene;
    SceneOverlayComponent* overlay = nullptr;
    
    Rectangle<int> boundsInParent;
    int parentHeight;
    
    Array<Listener*> listeners;
    Listener* deleteListener;
    
    std::unique_ptr<ScenesRender> ownRender;
    
    
};
