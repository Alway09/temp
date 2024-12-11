#pragma once

#include <JuceHeader.h>
#include "SceneComponent.h"

using namespace juce;

class ScenesRender : public OpenGLRenderer, public SceneComponent::Listener
{
public:
    ScenesRender(Component& componentAttachTo)
    {
        context.setRenderer(this);
        //context.setComponentPaintingEnabled(false);
        context.attachTo (componentAttachTo);
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
        //scene->addListener(this);
    }
    
    void sceneMouseDown(Scene* scene) override {
        bringToFront(scene);
    }
    
private:
    void bringToFront(Scene* scene) {
        int currentScenePos = scenes.indexOf(scene);
        Scene* tmp = scenes[currentScenePos];
        
        for(int i = currentScenePos; i < scenes.size() - 1; ++i) {
            scenes.setUnchecked(i, scenes[i + 1]);
        }
        
        scenes.setUnchecked(scenes.size() - 1, tmp);
    }
    
    OpenGLContext context;
    Array<Scene*> scenes;
    
    Colour clearColour{0.f, 0.f, 0.f, 0.f};
};
