#pragma once

#include <JuceHeader.h>
#include "SceneOverlayComponent.h"
#include "Scene.h"

using namespace juce;

class SceneComponent : public ResizableWindow
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
    
    class Listener {
    public:
        virtual ~Listener(){}
        virtual void sceneMouseDown(Scene* scene){};
        virtual void sceneMouseUp(Scene* scene){};
        virtual void sceneMouseClicked(Scene* scene){};
    };
    
    void addSceneListener(Listener* l) { listeners.add(l); }
    
private:
    Scene* scene;
    
    Rectangle<int> boundsInParent;
    int parentHeight;
    
    Array<Listener*> listeners;
};
