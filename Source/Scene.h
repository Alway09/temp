#pragma once
#include <JuceHeader.h>
#include "SceneObject.h"

using namespace juce;

class Scene : public OpenGLAppComponent
{
public:
    Scene(ValueTree treeAttachTo);
    ~Scene() override;
    
    void initialise() override;
    void shutdown() override;
    void render() override;
    
    void resized() override;
    void paint(Graphics& g) override;
    
    Matrix3D<float> getProjectionMatrix() const;
    Matrix3D<float> getViewMatrix() const;
    
    void createShaders();
    void createObject(SceneObject * const obj) {
        if(shader.get() != nullptr)
            obj->reset(*shader);
        objects.add(obj);
    }
    
    Identifier& getIdentifier() { return identifier; }
    
    void mouseDoubleClick(const MouseEvent& e) override
    {
        getParentComponent()->mouseDoubleClick(e);
    }
    
    void mouseEnter(const MouseEvent& e) override {
        getParentComponent()->mouseEnter(e);
    }
    void mouseExit(const MouseEvent& e) override {
        getParentComponent()->mouseExit(e);
    }
    
    void mouseDrag(const MouseEvent& e) override {
        getParentComponent()->mouseDrag(e);
    }
    
    void mouseMove(const MouseEvent& e) override {
        getParentComponent()->mouseMove(e);
    }
    
    void mouseDown(const MouseEvent& e) override {
        getParentComponent()->mouseDown(e);
    }
    
    void mouseUp(const MouseEvent& e) override {
        getParentComponent()->mouseUp(e);
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
    
    const char* vertexShader;
    const char* fragmentShader;
    String newVertexShader, newFragmentShader;
    
    Rectangle<int> bounds;
    CriticalSection mutex;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Scene)
};
