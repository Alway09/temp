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
    
    //void initialise() { scene->createShaders(); }
    void shutdown() { scene->shutdown(); }
    void render() { scene->render(); }
    
    void mouseDown(const MouseEvent& e) override {
        if(listener != nullptr) {
            listener->sceneMouseDown(scene);
        }
        ResizableWindow::mouseDown(e);
    }
    
    class Listener {
    public:
        virtual ~Listener(){}
        virtual void sceneMouseDown(Scene* scene) = 0;
    };
    
    void setSceneListener(Listener* l) { listener = l; }
    
private:
    Scene* scene;
    
    Rectangle<int> boundsInParent;
    int parentHeight;
    
    Listener* listener = nullptr;
};
