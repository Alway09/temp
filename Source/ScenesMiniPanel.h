#pragma once

#include <JuceHeader.h>
#include "SceneComponent.h"

using namespace juce;

class ScenesMiniPanel : public Component, public StatefulObject, public SceneComponent::Listener
{
public:
    ScenesMiniPanel(StatefulObject& parent, ScenesRender& render) : StatefulObject(parent, "Panel"), scenesRender(render)
    {
        setBounds(selfbounds);
    }
    
    void resized() override {
        auto bounds = selfbounds;
        for(int i = 0; i < wrappers.size(); ++i) {
            auto cellBounds = bounds.removeFromLeft(normalWidth);
            wrappers[i]->setBounds(cellBounds);
            
            if(sceneComponents[i] != choosenComponent) {
                sceneComponents[i]->setBounds(wrappers[i]->getBodyBounds().withX(cellBounds.getX()));
                sceneComponents[i]->getScene().changeBounds(cellBounds.getX() + xOffset, yOffset);
            }
        }
    }
    
    void moved() override {
        if(sceneComponents.size() == 0) return;
        
        int width = sceneComponents[0]->getLocalBounds().getWidth();
        for(int i = 0; i < sceneComponents.size(); ++i) {
            if(sceneComponents[i] != choosenComponent) {
                sceneComponents[i]->getScene().changeBounds(i * width + xOffset, yOffset);
            }
        }
    }
    
    //int getCurrentHeight() { return isVisible() ? normalHeight : 0; }
    int getNormalHeight() const { return normalHeight; }
    
    void sceneMouseClicked(SceneComponent& sc) override {
        choosenComponent = &sc;
        //resized();
    }
    
    void returnOnPannel(SceneComponent* sc) {
        addAndMakeVisible(sc);
        choosenComponent = nullptr;
        resized();
    }
    
    void createScene(SceneComponent::Listener* listener) {
        //calcLocalBounds();
        Scene* scene = new Scene(scenesRender.getContext(), *this);
        scene->createObject(SceneObjectRealisation::Background);
        scene->createObject(SceneObjectRealisation::Waveform);
        SceneComponent* sceneComponent = new SceneComponent(*scene);
        sceneComponents.add(sceneComponent);
        sceneComponent->addSceneListener(listener);
        sceneComponent->addSceneListener(this);
        scenesRender.addScene(scene);
        sceneComponent->setDeleter(this);
        SceneComponentWrapper* wrapper = new SceneComponentWrapper(sceneComponent->getScene().getName());
        wrappers.add(wrapper);
        addAndMakeVisible(wrapper);
        addAndMakeVisible(sceneComponent);
        calcLocalBounds();
        resized();
    }
    
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
    
private:
    void calcLocalBounds() {
        selfbounds = Rectangle<int>{0, 0, wrappers.size() * normalWidth, normalHeight};
        setBounds(selfbounds);
    }
    
    class SceneComponentWrapper : public Component
    {
    public:
        SceneComponentWrapper(const String& name) {
            sceneName.setText(name, NotificationType::dontSendNotification);
            addAndMakeVisible(sceneName);
            addAndMakeVisible(deleteButton);
        }
        
        void resized() override {
            auto bounds = getLocalBounds();
            auto hb = bounds.removeFromTop(getWidth() * 0.12f);
            deleteButton.setBounds(hb.removeFromRight(getHeight() * 0.2f));
            sceneName.setBounds(hb);
            bodyBounds = bounds;
        }
        
        const Rectangle<int>& getBodyBounds() { return bodyBounds; }
        
    private:
        Label sceneName;
        TextButton deleteButton{"X"};
        Rectangle<int> bodyBounds{0, 0, 1, 1};
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
};
