#pragma once
#include <JuceHeader.h>
#include "SceneObjectRealisationHelper.h"
#include "ArrayUtil.h"

using namespace juce;

class Scene : public StatefulObject
{
public:
    Scene(OpenGLContext& context, StatefulObject& parent) : StatefulObject(parent, parent.getName(), defaultPrefixName), context(&context){ createShaders(); }
    
    Scene(OpenGLContext& context, StatefulObject& parent, ObjectState objectState) : StatefulObject(parent, objectState), context(&context)
    {
        createShaders();
        if(hasChildren()) {
            auto statesArray = getChildrenStates();
            for(auto state : statesArray) {
                createObject(SceneObjectRealisationHelper::fromState(state), &state);
            }
        }
    }
    ~Scene(){ ScopedLock lock(mutex); } // when obects delete need for render is not running
    
    // always calls from message thread
    SceneObject* const createObject(SceneObjectRealisation realisation, ObjectState* objectState = nullptr) {
        assertIfGlThread();
        SceneObject* obj = SceneObjectRealisationHelper::createObject(realisation, *this, objectState);
        getContext().executeOnGLThread([this, obj](OpenGLContext&){ obj->reset(shader); } , true);
        objects.add(obj);
        return obj;
    }
    
    // always calls from message thread
    void createShaders(bool block = true) {
        assertIfGlThread();
        const ScopedLock lock(mutex); // block on message thread for gl thread cannot call render()
        shaderInitialized.set(false);
            getContext().executeOnGLThread([this](OpenGLContext&)
            {
                if(!getContext().isActive()) jassertfalse;
                
                std::unique_ptr<OpenGLShaderProgram> newShader(new OpenGLShaderProgram (getContext()));
                if(compileShader(newShader)) {
                    shader = std::move(newShader);
                    shader->use();
                    uniforms.reset (new Uniforms (*shader));
                    for(auto obj : objects) obj->reset(shader);
                    shaderInitialized.set(true);
                } else {
                    DBG("shaders compile error");
                    //jassertfalse;
                }
            } 
            ,block);
    }
    
    // allways calls from gl thread
    void render() {
        using namespace ::juce::gl;
        
        if(!shaderInitialized.get()) return;
        
        if(!getContext().isActive()) jassertfalse;
        
        {
            const ScopedLock lock(mutex);
            
            glViewport (viewportBounds[0], viewportBounds[1], viewportBounds[2], viewportBounds[3]);
            
            shader->use();
            if (uniforms->projectionMatrix != nullptr) {
                uniforms->projectionMatrix->setMatrix4(getProjectionMatrix().mat, 1, false);
            }
            
            if (uniforms->viewMatrix != nullptr) {
                uniforms->viewMatrix->setMatrix4(getViewMatrix().mat, 1, false);
            }
            
            for(auto obj : objects) obj->draw();
            
            glBindBuffer (GL_ARRAY_BUFFER, 0);
            glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    }
    
    void deleteObject(SceneObject* object) {
        const ScopedLock lock(mutex);
        objects.removeObject(object);
    }
    
    void changeBounds(Rectangle<int>& bounds, int parentHeight, bool inOwnWindow) {
        const ScopedLock lock(mutex);
        this->bounds = bounds;
        
        float desktopScale = getContext().getRenderingScale();
        
        
        viewportBounds.setUnchecked(2, roundToInt(roundToInt(desktopScale * bounds.getWidth())));
        viewportBounds.setUnchecked(3, roundToInt(roundToInt(desktopScale * bounds.getHeight())));
        
        if(inOwnWindow) desktopScale = 0.f;
        viewportBounds.setUnchecked(0, roundToInt(desktopScale * bounds.getX()));
        viewportBounds.setUnchecked(1, roundToInt(desktopScale * (parentHeight - bounds.getHeight() - bounds.getY())));
    }
    
    void objectsReorder(int oldIdx, int newIdx) {
        reorder(objects, oldIdx, newIdx);
        int idx = oldIdx > newIdx ? newIdx + 1 : newIdx;
        objects[idx]->move(idx);
    }
    
    OwnedArray<SceneObject>& getObjects() { return objects; }
    
    void replaceContext(OpenGLContext& newContext) {
        freeResouces();
        context = &newContext;
        createShaders(false);
    }
    
    void freeResouces() {
        shaderInitialized.set(false);
        shader.reset();
        uniforms.reset();
    }
    
private:
    struct Uniforms
    {
        explicit Uniforms (OpenGLShaderProgram& shaderProgram) {
            projectionMatrix.reset(createUniform (shaderProgram, "projectionMatrix"));
            viewMatrix.reset(createUniform (shaderProgram, "viewMatrix"));
        }

        std::unique_ptr<OpenGLShaderProgram::Uniform> projectionMatrix, viewMatrix;
    private:
        static OpenGLShaderProgram::Uniform* createUniform (OpenGLShaderProgram& shaderProgram, const char* uniformName) {
            using namespace ::juce::gl;

            if (glGetUniformLocation (shaderProgram.getProgramID(), uniformName) < 0)
                return nullptr;

            return new OpenGLShaderProgram::Uniform (shaderProgram, uniformName);
        }
    };
    
    OpenGLContext& getContext() {
        if(context) return *context;
        throw std::bad_exception();
    }
    
    bool compileShader(std::unique_ptr<OpenGLShaderProgram>& shader) {
        return shader->addVertexShader(OpenGLHelpers::translateVertexShaderToV3(vertexShaders[currentVertexShaderIndex]))
        && shader->addFragmentShader(OpenGLHelpers::translateFragmentShaderToV3(fragmentShaders[currentFragmentShaderIndex]))
        && shader->link();
    }
    
    void assertIfGlThread() {
        // debug method
        if(getContext().isActive()) jassertfalse;
    }
    
    Matrix3D<float> getProjectionMatrix() const
    {
        const ScopedLock lock (mutex);

        auto w = 1.0f / (0.5f + 0.1f);
        auto h = w * bounds.toFloat().getAspectRatio (false);

        return Matrix3D<float>::fromFrustum (-w, w, -h, h, 4.0f, 30.0f);
    }
    
    Matrix3D<float> getViewMatrix() const
    {
        auto viewMatrix = Matrix3D<float>::fromTranslation ({ 0.0f, 0.0f, -10.0f });
        /*auto rotationMatrix = viewMatrix.rotation ({ -0.3f, 5.0f * std::sin ((float) getFrameCounter() * 0.01f), 0.0f });*/
        auto rotationMatrix = viewMatrix.rotation ({ -0.3f, 5.0f * std::sin ((float) 1 * 0.01f), 0.0f });

        return viewMatrix * rotationMatrix;
    }
    
    OpenGLContext* context;
    std::unique_ptr<OpenGLShaderProgram> shader;
    Atomic<bool> shaderInitialized{false};
    std::unique_ptr<Uniforms> uniforms;
    OwnedArray<SceneObject> objects;
    
    Rectangle<int> bounds;
    Array<int> viewportBounds{0,0,0,0};
    
    CriticalSection mutex;
    
    //bool is3D;    sceneObject.is3D() && is3D - use for render simplicity
    
    int currentVertexShaderIndex = 0;
    int currentFragmentShaderIndex = 0;
    
    static const StringArray vertexShaders;
    static const StringArray fragmentShaders;
    
    static const String defaultPrefixName;
    
    bool checkError(String dbgText, bool dropError = false) {
        GLenum err = gl::glGetError();
        if(err != 0 && !dropError) {
            DBG(dbgText);
            DBG(String(err));
        }
        return err != 0;
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Scene)
};
