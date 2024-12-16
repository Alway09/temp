#pragma once
#include <JuceHeader.h>
#include "SceneObject.h"
#include "NameGenerator.h"

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
    
    Uuid& getUuidIdentifier() { return uuidIdentifier; }
    
    void changeBounds(Rectangle<int>& bounds, int height) {
        const ScopedLock lock (mutex);
        this->bounds = bounds;
        parentHeight = height;
    }
    
    void setName(String name);
    String getName() const { return NameGenerator::fromIdentifier(valueTree.getType()); }
    ValueTree getValueTree() const { return valueTree; }
    
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
    
    ValueTree valueTree;
    Uuid uuidIdentifier;
    
    const char* vertexShader;
    const char* fragmentShader;
    String newVertexShader, newFragmentShader;
    
    Rectangle<int> bounds;
    int parentHeight;
    CriticalSection mutex;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Scene)
};
