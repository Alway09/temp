#pragma once

#include <JuceHeader.h>
#include "SceneComponent.h"
#include "CustomAudioBuffer.h"

using namespace juce;

class ScenesRender : public OpenGLRenderer, public SceneComponent::Listener, public Timer
{
public:
    ScenesRender(Component& componentAttachTo);
    ~ScenesRender();
    
    void newOpenGLContextCreated() override {}
    void openGLContextClosing() override;
    void renderOpenGL() override;
    
    void sceneMouseDown(Scene* scene) override;
    
    void addScene(Scene* scene);
    void removeScene(Scene* scene);
    
    OpenGLContext& getContext() { return context; }

    void timerCallback() override {
        DBG("FPS: " + String(framesCounter - prevFramesCount));
        prevFramesCount = framesCounter;
    }
private:
    void bringToFront(Scene* scene);
    
    OpenGLContext context;
    OwnedArray<Scene> scenes;
    
    Colour clearColour{0.f, 0.f, 0.f, 0.f};
    
    uint64_t framesCounter = 0;
    uint64_t prevFramesCount = 0;
};
