#include "ScenesRender.h"

ScenesRender::ScenesRender(Component& componentAttachTo)
{
    context.setRenderer(this);
    //context.setComponentPaintingEnabled(false);
    context.attachTo (componentAttachTo);
    context.setContinuousRepainting (true);
    
    startTimer(1000);
}

ScenesRender::~ScenesRender() {
    scenes.clearQuick(false);
    if(context.isAttached())
        context.detach();
}

void ScenesRender::openGLContextClosing() {
    //DBG("context closing");
}

void ScenesRender::renderOpenGL() {
    using namespace ::juce::gl;
    
    OpenGLHelpers::clear (clearColour);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    CustomAudioBuffer::getInst()->setReadPoint();
    for(auto sc : scenes) {
        sc->render();
    }
    framesCounter++;
}

/*void ScenesRender::sceneMouseDown(Scene* scene) {
    bringToFront(scene);
}*/

void ScenesRender::addScene(Scene* scene) {
   /* if(tmp) {
        context.executeOnGLThread([&scene](OpenGLContext&){ scene->createShaders(); }, true);
        //scene->createShaders();
    } else {
        context.executeOnGLThread([scene](OpenGLContext&){
            scene->createShaders();
        }, false);
    }*/
    //scene->createShaders();
    
    scenes.add(scene);
}

void ScenesRender::removeScene(Scene* scene) {
    //const ScopedLock lock (scene->renderMutex);
    scenes.removeObject(scene, false);
}

void ScenesRender::bringToFront(Scene* scene) {
    int currentScenePos = scenes.indexOf(scene);
    if(currentScenePos == -1) {
        return;
    }
    Scene* tmp = scenes[currentScenePos];
    
    for(int i = currentScenePos; i < scenes.size() - 1; ++i) {
        scenes.set(i, scenes[i + 1], false);
    }
    
    scenes.set(scenes.size() - 1, tmp, false);
}
