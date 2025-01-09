#pragma once
#include <JuceHeader.h>
//#include "SceneObjectRealisationCreator.h"
#include "WaveformSceneObject.h"
#include "BackgroundSceneObject.h"

using namespace juce;

class Scene : public StatefulObject
{
public:
    Scene(StatefulObject& parent);
    Scene(ObjectState objectState);
    ~Scene();
    
    void shutdown();
    void render();
    
    Matrix3D<float> getProjectionMatrix() const;
    Matrix3D<float> getViewMatrix() const;
    
    void createShaders();
    void createObject(SceneObjectRealisation realisation) {
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
            obj->reset(*shader);

        objects.add(obj);
    }
    
    void createObject(SceneObjectRealisation realisation, ObjectState objectState) {
        SceneObject* obj = nullptr;
        
        switch (realisation) {
            case SceneObjectRealisation::Waveform:
            {
                obj = new WaveformSceneObject(objectState);
                break;
            }
            case SceneObjectRealisation::Background:
            {
                obj = new BackgroundSceneObject(objectState);
                break;
            }
        }
        
        if(shader.get() != nullptr)
            obj->reset(*shader);

        objects.add(obj);
    }
    
    Uuid& getUuidIdentifier() { return uuidIdentifier; }
    
    void changeBounds(Rectangle<int>& bounds, int height) {
        const ScopedLock lock (mutex);
        this->bounds = bounds;
        parentHeight = height;
    }
    
    OwnedArray<SceneObject>& getObjects() { return objects; }
    
    CriticalSection renderMutex;
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
    
    const char* vertexShader;
    const char* fragmentShader;
    String newVertexShader, newFragmentShader;
    
    Rectangle<int> bounds;
    int parentHeight;
    CriticalSection mutex;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Scene)
};
