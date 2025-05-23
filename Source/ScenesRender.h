#pragma once

#include <JuceHeader.h>
//#include "SceneComponent.h"
#include "Scene.h"
#include "CustomAudioBuffer.h"

using namespace juce;

class ScenesRender : public OpenGLRenderer,/* public SceneComponent::Listener,*/ public Timer
{
public:
    ScenesRender(Component& componentAttachTo);
    ~ScenesRender();
    
    void newOpenGLContextCreated() override {}
    void openGLContextClosing() override;
    void renderOpenGL() override;
    
    /*void sceneMouseDown(Scene* scene) override;*/
    
    void addScene(Scene* scene);
    void removeScene(Scene* scene);
    
    OpenGLContext& getContext() { return context; }

    void timerCallback() override {
        //DBG("FPS: " + String(framesCounter - prevFramesCount));
        prevFramesCount = framesCounter;
    }
    
    void bringToFront(Scene* scene);
    
    void attach() { context.attachTo(attachedTo); }
    
    void setScissorsBox(Rectangle<int> box) {
        scissorsBox = box;
        scissorsBoxEnabled = true;
    }
private:
    OpenGLContext context;
    Component& attachedTo;
    OwnedArray<Scene> scenes;
    
    Colour clearColour{47, 79, 79};
    
    Rectangle<int> scissorsBox{0, 0, 0, 0};
    bool scissorsBoxEnabled = false;
    
    uint64_t framesCounter = 0;
    uint64_t prevFramesCount = 0;
};
