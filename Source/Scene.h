#pragma once
#include <JuceHeader.h>
#include "SceneObject.h"

using namespace juce;

class Scene
{
public:
    Scene(ValueTree treeAttachTo);
    ~Scene();
    
    void shutdown();
    void render();
    
    Matrix3D<float> getProjectionMatrix() const;
    Matrix3D<float> getViewMatrix() const;
    
    void createShaders();
    void createObject(SceneObject * const obj) {
        if(shader.get() != nullptr)
            obj->reset(*shader);
        objects.add(obj);
    }
    
    Identifier& getIdentifier() { return identifier; }
    Uuid& getUuidIdentifier() { return uuidIdentifier; }
    
    void changeBounds(Rectangle<int>& bounds, int height) {
        const ScopedLock lock (mutex);
        this->bounds = bounds;
        parentHeight = height;
    }
    
private:
    //==============================================================================
    struct Uniforms
    {
        explicit Uniforms (OpenGLShaderProgram& shaderProgram);

        std::unique_ptr<OpenGLShaderProgram::Uniform> projectionMatrix, viewMatrix;
    private:
        static OpenGLShaderProgram::Uniform* createUniform (OpenGLShaderProgram& shaderProgram, const char* uniformName)
        {
            using namespace ::juce::gl;

            if (glGetUniformLocation (shaderProgram.getProgramID(), uniformName) < 0)
                return nullptr;

            return new OpenGLShaderProgram::Uniform (shaderProgram, uniformName);
        }
    };
    //==============================================================================
    
    std::unique_ptr<OpenGLShaderProgram> shader;
    std::unique_ptr<Uniforms> uniforms;
    OwnedArray<SceneObject> objects;
    
    ValueTree valueTree;
    Identifier identifier;
    Uuid uuidIdentifier;
    
    const char* vertexShader;
    const char* fragmentShader;
    String newVertexShader, newFragmentShader;
    
    Rectangle<int> bounds;
    int parentHeight;
    CriticalSection mutex;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Scene)
};
