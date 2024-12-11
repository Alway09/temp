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
    
    class Listener {
        
    };
    
private:
    Scene* scene;
    
    Rectangle<int> boundsInParent;
    int parentHeight;
};
