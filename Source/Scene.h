#pragma once
#include <JuceHeader.h>
#include "SceneObjectRealisationHelper.h"
#include "ArrayUtil.h"

using namespace juce;

/*class Scene : public StatefulObject
{
public:
    Scene(StatefulObject& parent, OpenGLContext& context);
    Scene(StatefulObject& parent, ObjectState objectState, OpenGLContext& context);
    ~Scene();
    
    void shutdown();
    void render();
    
    Matrix3D<float> getProjectionMatrix() const;
    Matrix3D<float> getViewMatrix() const;
    
    void createShaders();
    SceneObject* createObject(SceneObjectRealisation realisation) {
        SceneObject* obj = nullptr;
        
        switch (realisation) {
            case SceneObjectRealisation::Waveform:
            {
                obj = new WaveformSceneObject(*this);
                break;
            }
            case SceneObjectRealisation::Background:
            {
                obj = new BackgroundSceneObject(*this);
                break;
            }
        }
        
        if(shader.get() != nullptr)
        {
            context->executeOnGLThread([this, obj](OpenGLContext&){ obj->reset(shader); } , true);
            //obj->reset(*shader);
        }

        objects.add(obj);
        return obj;
    }
    
    void createObject(SceneObjectRealisation realisation, ObjectState objectState) {
        SceneObject* obj = nullptr;
        
        switch (realisation) {
            case SceneObjectRealisation::Waveform:
            {
                obj = new WaveformSceneObject(*this, objectState);
                break;
            }
            case SceneObjectRealisation::Background:
            {
                obj = new BackgroundSceneObject(*this, objectState);
                break;
            }
        }
        
        if(shader.get() != nullptr)
            obj->reset(shader);

        objects.add(obj);
    }
    
    void deleteObject(SceneObject* object) {
        //const ScopedLock lock (renderMutex);
        objects.removeObject(object);
    }
    
    void objectsReorder(int oldIdx, int newIdx) {
        reorder(objects, oldIdx, newIdx);
        int idx = oldIdx > newIdx ? newIdx + 1 : newIdx;
        objects[idx]->move(idx);
    }
    
    //Uuid& getUuidIdentifier() { return uuidIdentifier; }
    
    void changeBounds(Rectangle<int>& bounds, int height) {
        const ScopedLock lock (mutex);
        this->bounds = bounds;
        parentHeight = height;
    }
    
    void replaceContext(OpenGLContext& newContext) {
        \\const ScopedLock lock(renderMutex);
        \\newContext.executeOnGLThread([this](OpenGLContext&){
        \\    createShaders();
        \\}, true);
        context = &newContext;
        //uniforms.reset();
        //shader.release();
    }
    
    OwnedArray<SceneObject>& getObjects() { return objects; }
    
    //CriticalSection renderMutex;
private:
    //==============================================================================
    struct Uniforms
    {
        explicit Uniforms (OpenGLShaderProgram& shaderProgram);

        std::unique_ptr<OpenGLShaderProgram::Uniform> projectionMatrix, viewMatrix;
    private:
        static OpenGLShaderProgram::Uniform* createUniform (OpenGLShaderProgram& shaderProgram, const char* uniformName);
    };
    //==============================================================================
    
    std::unique_ptr<OpenGLShaderProgram> shader;
    std::unique_ptr<Uniforms> uniforms;
    OwnedArray<SceneObject> objects;
    
    Uuid uuidIdentifier;
    
    //const char* vertexShader;
    //const char* fragmentShader;
    String newVertexShader, newFragmentShader;
    
    Rectangle<int> bounds;
    int parentHeight;
    CriticalSection mutex;
    
    OpenGLContext* context;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Scene)
};*/

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
        //std::unique_ptr<OpenGLShaderProgram> newShader(new OpenGLShaderProgram (getContext()));
        const ScopedLock lock(mutex); // block on message thread for gl thread cannot call render()
        shaderInitialized.set(false);
            getContext().executeOnGLThread([this](OpenGLContext&)
            {
                if(!getContext().isActive()) jassertfalse;
                
                std::unique_ptr<OpenGLShaderProgram> newShader(new OpenGLShaderProgram (getContext()));
                //checkError("Before compile");
                if(compileShader(newShader)) {
                    //checkError("After compile before move");
                    shader = std::move(newShader);
                    //checkError("After compile after move", true); // always 1281 but ok, why??
                    /*DBG("ProgID");
                    DBG(String(shader->getProgramID()));*/
                    shader->use();
                    //checkError("After use");
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
        
        //float desktopScale = getContext().getRenderingScale();
        
        /*int* viewMassive = new int[4] {
            roundToInt(desktopScale * bounds.getX()),
            roundToInt(desktopScale * (parentHeight - bounds.getHeight() - bounds.getY())),
            roundToInt(roundToInt(desktopScale * bounds.getWidth())),
            roundToInt(roundToInt(desktopScale * bounds.getHeight()))
        };*/
        
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
        
        //delete[] viewMassive;
    }
    
    void deleteObject(SceneObject* object) {
        const ScopedLock lock(mutex);
        objects.removeObject(object);
    }
    
    void changeBounds(Rectangle<int>& bounds, int parentHeight, bool inOwnWindow) {
        const ScopedLock lock(mutex);
        this->bounds = bounds;
        //parentHeight = height;
        
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
        //const ScopedLock lock(mutex);
        freeResouces();
        context = &newContext;
        createShaders(false);
    }
    
    void freeResouces() {
        shaderInitialized.set(false);
        //shader->release();
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
        bool vertex = shader->addVertexShader(OpenGLHelpers::translateVertexShaderToV3(vertexShaders[currentVertexShaderIndex]));
        //checkError("After compile vertex");
        bool fragment = shader->addFragmentShader(OpenGLHelpers::translateFragmentShaderToV3(fragmentShaders[currentFragmentShaderIndex]));
        //checkError("After compile fragment");
        bool link = shader->link();
        //checkError("After compile link");
        return vertex && fragment && link;
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
    //int parentHeight;
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
