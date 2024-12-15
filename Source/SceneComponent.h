#pragma once

#include <JuceHeader.h>
#include "SceneOverlayComponent.h"
#include "Scene.h"

using namespace juce;

class SceneComponent : public ResizableWindow, public Button::Listener
{
public:
    SceneComponent(Scene* scene);
    
    void resized() override;
    void moved() override;
    void paint(Graphics&) override {};
    
    void shutdown() { scene->shutdown(); }
    void render() { scene->render(); }
    Scene* getScene() { return scene; }
    
    void mouseDown(const MouseEvent& e) override {
        ResizableWindow::mouseDown(e);
        
        for(auto listener : listeners) {
            listener->sceneMouseDown(scene);
        }
    }
    
    void mouseUp(const MouseEvent& e) override {
        ResizableWindow::mouseUp(e);
        
        if(e.mouseWasClicked()) {
            for(auto listener : listeners) {
                listener->sceneMouseClicked(scene);
            }
        } else {
            for(auto listener : listeners) {
                listener->sceneMouseUp(scene);
            }
        }
    }
    
    void buttonClicked(Button* button) override {
        const ScopedLock lock (scene->renderMutex);
        //shutdown();
        for(auto listener : listeners) {
            if(listener == deleteListener) {
                continue;
            }
            listener->sceneDeleteButtonClicked(this);
        }
        deleteListener->sceneDeleteButtonClicked(this);
    }
    
    class Listener {
    public:
        virtual ~Listener(){}
        virtual void sceneMouseDown(Scene* scene){};
        virtual void sceneMouseUp(Scene* scene){};
        virtual void sceneMouseClicked(Scene* scene){};
        virtual void sceneDeleteButtonClicked(SceneComponent* sceneComponent){};
    };
    
    void addSceneListener(Listener* l) { listeners.add(l); }
    void setDeleterListener(Listener* l) { deleteListener = l; }
    
private:
    class SceneOverlayComponent : public Component
    {
    public:
        SceneOverlayComponent(SceneComponent* parent) : parent(parent)
        {
            deleteButton.addListener(parent);
            addAndMakeVisible(deleteButton);
        }
        
        void resized() override
        {
            auto localBounds = getLocalBounds();
            deleteButton.setBounds(localBounds.getWidth() - 20, 0, 20, 20);
        }
        
        void mouseDoubleClick(const MouseEvent& e) override { parent->mouseDoubleClick(e); }
        void mouseEnter(const MouseEvent& e) override { parent->mouseEnter(e); }
        void mouseExit(const MouseEvent& e) override { parent->mouseExit(e); }
        void mouseDrag(const MouseEvent& e) override { parent->mouseDrag(e); }
        void mouseMove(const MouseEvent& e) override { parent->mouseMove(e); }
        void mouseDown(const MouseEvent& e) override { parent->mouseDown(e); }
        void mouseUp(const MouseEvent& e) override { parent->mouseUp(e); }
        
    private:
        SceneComponent* parent;
        TextButton deleteButton{"d"};
    };
    
    Scene* scene;
    
    Rectangle<int> boundsInParent;
    int parentHeight;
    
    Array<Listener*> listeners;
    Listener* deleteListener;
};
