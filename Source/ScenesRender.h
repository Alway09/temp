#pragma once

#include <JuceHeader.h>
#include "Scene.h"

using namespace juce;

class ScenesRender : public OpenGLRenderer
{
public:
    ScenesRender(Component* componentAttachTo)
    {
        context.setRenderer(this);
        context.attachTo (*componentAttachTo);
        context.setContinuousRepainting (true);
    }
    
    ~ScenesRender() {
        if(context.isAttached())
            context.detach();
    }
    
    void newOpenGLContextCreated() override {}
    
    void openGLContextClosing() override {
        for(auto sc : scenes) {
            sc->shutdown();
        }
    }
    
    void renderOpenGL() override {
        using namespace ::juce::gl;
        OpenGLHelpers::clear (clearColour);
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        for(auto sc : scenes) {
            sc->render();
        }
    }
    
    void addScene(Scene* scene) {
        context.executeOnGLThread([&scene](OpenGLContext&){ scene->createShaders(); }, true);
        scenes.add(scene);
    }
    
private:
    OpenGLContext context;
    Array<Scene*> scenes;
    
    Colour clearColour{0.f, 0.f, 0.f, 0.f};
};
