#pragma once

#include <JuceHeader.h>
#include "SceneComponent.h"
#include "CustomAudioBuffer.h"

using namespace juce;

class ScenesRender : public OpenGLRenderer, public SceneComponent::Listener
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
private:
    void bringToFront(Scene* scene);
    
    OpenGLContext context;
    OwnedArray<Scene> scenes;
    
    Colour clearColour{0.f, 0.f, 0.f, 0.f};
};
