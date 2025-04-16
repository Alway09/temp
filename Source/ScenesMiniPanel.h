#pragma once

#include <JuceHeader.h>
#include "SceneComponent.h"

using namespace juce;

class ScenesMiniPanel : public Component, public StatefulObject, public SceneComponent::Listener
{
public:
    ScenesMiniPanel(StatefulObject& parent, ScenesRender& render, SceneComponent::Listener* componentListener);
    
    void resized() override {
        auto bounds = selfbounds;
        for(int i = 0; i < wrappers.size(); ++i) {
            auto cellBounds = bounds.removeFromLeft(normalWidth);
            wrappers[i]->setBounds(cellBounds);
            
            if(needToPositionize(sceneComponents[i])) {
                sceneComponents[i]->setBounds(wrappers[i]->getBodyBounds().withX(cellBounds.getX()));
                sceneComponents[i]->getScene().changeBounds(cellBounds.getX() + xOffset, yOffset);
            }
        }
    }
    
    void moved() override {
        int width = 0;
        for(SceneComponent* sc : sceneComponents) {
            if(needToPositionize(sc)) {
                width = sc->getWidth();
                break;
            }
        }
        
        if(width == 0) return;
        
        for(int i = 0; i < sceneComponents.size(); ++i) {
            if(needToPositionize(sceneComponents[i])) {
                sceneComponents[i]->getScene().changeBounds(i * width + xOffset, yOffset);
            }
        }
    }
    
    bool needToPositionize(SceneComponent* sc) {
        return sc != choosenComponent && !sc->isDetached();
    }
    
    int getNormalHeight() const { return normalHeight; }
    
    void sceneMouseClicked(SceneComponent& sc) override {
        if(!sc.isDetached()) {
            setChoosen(&sc);
        }
    }
    
    void returnOnPannel(SceneComponent* sc) {
        if(choosenComponent == sc) {
            setChoosen(nullptr);
        }
        addAndMakeVisible(sc);
        resized();
    }
    
    void sceneDeleting(SceneComponent& sceneComponent) override {
        SceneComponentWrapper* wrap = nullptr;
        for(SceneComponentWrapper* w : wrappers) {
            if(w->isAssociatedWith(&sceneComponent)) {
                wrap = w;
                break;
            }
        }
        wrappers.removeObject(wrap);
        sceneComponents.removeObject(&sceneComponent);
        choosenComponent = nullptr;
        resized();
    }
    
    void sceneDetached(SceneComponent& component, bool isDetached) override {
        if(isDetached) {
            setChoosen(nullptr);
        } else {
            if(choosenComponent == nullptr) {
                setChoosen(&component);
            } else {
                addAndMakeVisible(component);
                resized();
            }
        }
    }
    
    void createScene() {createSceneInternal();}
    
    void setXOffset(int xOffset) {this->xOffset = xOffset; moved();}
    void setYOffset(int yOffset) {this->yOffset = yOffset;}
    
    const Rectangle<int> getSelfbounds() const { return selfbounds; }
    void showOrHide(bool mustBeShown) {
        showing = mustBeShown;
        for(SceneComponent* sc : sceneComponents) {
            if(sc != choosenComponent) {
                sc->getScene().setNeedToRender(mustBeShown);
            }
        }
    }
    
    bool isShowing() const { return showing; }
    
    class Listener {
        public:
        virtual ~Listener(){}
        virtual void returnButtonClicked(SceneComponent* sc) {}
    };
    
    void setListener(ScenesMiniPanel::Listener* l) {
        panelListener = l;
        for(SceneComponentWrapper* w : wrappers) {
            w->setListener(l);
        }
    }
    
    SceneComponent* getChoosenComponent() { return choosenComponent; }
    
private:
    SceneComponent* createSceneInternal(StatefulObject::ObjectState* objectState = nullptr, bool calcBounds = true) {
        SceneComponent* sc = nullptr;
        if(objectState == nullptr) {
            sc = new SceneComponent(scenesRender.getContext(), *this);
            scenesRender.addScene(&sc->getScene());
        } else {
            sc = new SceneComponent(*this, *objectState);
        }
        
        sceneComponents.add(sc);
        sc->addSceneListener(componentListener);
        sc->addSceneListener(this);
        sc->setDeleter(this);
        
        SceneComponentWrapper* wrapper = new SceneComponentWrapper(sc);
        sc->getScene().addListener(wrapper);
        wrapper->setListener(panelListener);
        wrappers.add(wrapper);
        
        addAndMakeVisible(wrapper);
        addAndMakeVisible(sc);
        if(calcBounds) calcLocalBounds();
        
        return sc;
    }
    
    struct IDs {
        inline static const Identifier isChoosenScene{"isChoosen"};
    };
    
    void calcLocalBounds() {
        selfbounds = Rectangle<int>{0, 0, wrappers.size() * normalWidth, normalHeight};
        setBounds(selfbounds);
    }
    
    void setChoosen(SceneComponent* sc) {
        if(sc != nullptr) sc->setProperty("isChoosen", true);
        if(choosenComponent != nullptr) choosenComponent->setProperty("isChoosen", false);
        choosenComponent = sc;
    }
    
    class SceneComponentWrapper : public Component, public NamedObject::Listener
    {
    public:
        SceneComponentWrapper(SceneComponent* associatedComponent) : associatedComponent(associatedComponent) {
            sceneName.setText(associatedComponent->getScene().getName(), NotificationType::dontSendNotification);
            addAndMakeVisible(sceneName);
            
            deleteButton.onClick = [this]() { this->associatedComponent->deleteScene(); };
            addAndMakeVisible(deleteButton);
            
            returnButton.setButtonText("return");
            returnButton.onClick = [this]() { listener->returnButtonClicked(this->associatedComponent); };
            addAndMakeVisible(returnButton);
        }
        
        void resized() override {
            auto bounds = getLocalBounds();
            auto hb = bounds.removeFromTop(getWidth() * 0.12f);
            deleteButton.setBounds(hb.removeFromRight(getHeight() * 0.2f));
            sceneName.setBounds(hb);
            returnButton.setBounds(bounds);
            bodyBounds = bounds;
        }
        
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
    
    SceneComponent* choosenComponent = nullptr;
    OwnedArray<SceneComponent> sceneComponents;
    OwnedArray<SceneComponentWrapper> wrappers;
    Rectangle<int> selfbounds{0, 0, 100, 100};
    
    static const int normalWidth = 200;
    static const int normalHeight = 120;
    int xOffset = 0;
    int yOffset = 0;
    
    bool showing = false;
    
    Listener* panelListener = nullptr;
    SceneComponent::Listener* componentListener = nullptr;
};
