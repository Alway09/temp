#pragma once

#include <JuceHeader.h>
#include "SceneComponent.h"

using namespace juce;

class ScenesMiniPanel : public Component, public SceneComponent::Listener
{
public:
    ScenesMiniPanel(StatefulObject& parent, ScenesRender& render, SceneComponent::Listener* componentListener);
    
    void resized() override;
    void moved() override;
    
    void sceneDeleting(SceneComponent& sceneComponent) override;
    void sceneDetached(SceneComponent& component, bool isDetached) override;
    
    void returnOnPannel(SceneComponent* sc);
    SceneComponent* createScene() {return createSceneInternal();}
    
    void setScenesDrawingXOffset(int xOffset) {this->scenesDrawingXOffset = xOffset; moved();}
    void setScenesDrawingYOffset(int yOffset) {this->scenesDrawingYOffset = yOffset;}
    int getPanelHeight() const { return panelHeight; }
    const Rectangle<int> getSelfbounds() const { return selfbounds; }
    SceneComponent* getChoosenComponent() { return choosenComponent; }
    
    void showOrHide(bool mustBeShown);
    bool isShowing() const { return showing; }
    void setChoosen(SceneComponent* sc);
    
    class Listener {
        public:
        virtual ~Listener(){}
        virtual void returnButtonClicked(SceneComponent* sc) {}
    };
    void setListener(ScenesMiniPanel::Listener* l);
private:
    struct IDs {
        inline static const String panelName{"Panel"};
        inline static const Identifier isChoosenScene{"isChoosen"};
    };
    
    SceneComponent* createSceneInternal(StatefulObject::ObjectState* objectState = nullptr, bool calcBounds = true);
    bool needToPositionize(SceneComponent* sc) { return sc != choosenComponent && !sc->isDetached(); }
    void calcLocalBounds();
    
    class SceneComponentWrapper : public Component, public NamedObject::Listener
    {
    public:
        SceneComponentWrapper(SceneComponent* associatedComponent);
        
        void resized() override;
        const Rectangle<int>& getBodyBounds() { return bodyBounds; }
        void setListener(ScenesMiniPanel::Listener* l) { listener = l; }
        bool isAssociatedWith(SceneComponent* sc) { return associatedComponent == sc; }
        void objectRenamed(const String& newName) override {
            sceneName.setText(newName, NotificationType::dontSendNotification);
        }
    private:
        Label sceneName;
        TextButton deleteButton{"X"};
        Rectangle<int> bodyBounds{0, 0, 1, 1};
        TextButton returnButton;
        ScenesMiniPanel::Listener* listener = nullptr;
        SceneComponent* associatedComponent = nullptr;
    };
    
    ScenesRender& scenesRender;
    StatefulObject& parent;
    
    SceneComponent* choosenComponent = nullptr;
    OwnedArray<SceneComponent> sceneComponents;
    OwnedArray<SceneComponentWrapper> wrappers;
    Rectangle<int> selfbounds;
    
    static const int wrapperWidth = 200;
    static const int panelHeight = 120;
    int scenesDrawingXOffset = 0;
    int scenesDrawingYOffset = 0;
    
    bool showing = false;
    
    Listener* panelListener = nullptr;
    SceneComponent::Listener* componentListener = nullptr;
};
