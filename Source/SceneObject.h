#pragma once
#include <JuceHeader.h>
#include "StatefulObject.h"

using namespace juce;

enum SceneObjectRealisation
{
    Waveform,
    Background
};

class SceneObject : public StatefulObject
{
public:
    SceneObject(StatefulObject& parent, String namePrefix, SceneObjectRealisation realisation);
    SceneObject(StatefulObject& parent, ObjectState objectState);
    virtual ~SceneObject();
    void draw();
    
    virtual SceneObjectRealisation getRealisation() = 0;
    
    void setNeedToRender(bool needToRender) {this->needToRender.set(needToRender);}
    bool getNeedToRender() const { return needToRender.get(); }
    
    void reset(std::unique_ptr<OpenGLShaderProgram>& shaderProgram);
    //static Identifier getTypeID() { return objectTypeID; }
protected:
    
    struct Vertex
    {
        float position[3];
        float normal[3];
        float colour[4];
        float texCoord[2];
    };
    
    void putVertices(Array<Vertex> vertices);
    void putIndices(juce::uint32* indices, int numIndices);
    virtual void fillBuffers() = 0;
    
    struct Config {
        enum DrawSource{
            Vertices,
            Indices
        } source = Vertices;
        
        enum DrawPrimitiveType {
            Point = gl::GL_POINTS,
            Lines = gl::GL_LINE_STRIP,
            Triangles = gl::GL_TRIANGLES
        } primitiveType = Lines;
        
        enum DrawBufferUsage {
            Stream = gl::GL_STREAM_DRAW,
            Static = gl::GL_STATIC_DRAW
        } drawBufferUsage = Stream;
    } config;
    
    bool needToUpdateBuffer = false;
private:
    //==============================================================================
    class Attributes
    {
    public:
        explicit Attributes(std::unique_ptr<OpenGLShaderProgram>& shaderProgram);
        void enable();
        void disable();

        std::unique_ptr<OpenGLShaderProgram::Attribute> position, normal, sourceColour, textureCoordIn;
    private:
        static OpenGLShaderProgram::Attribute* createAttribute (std::unique_ptr<OpenGLShaderProgram>& shader, const char* attributeName);
    };
    //==============================================================================
    struct VertexBuffer
    {
        /*explicit*/ VertexBuffer();
        ~VertexBuffer();
        
        void initialize();

        void putVertices(Array<Vertex> vertices, SceneObject::Config::DrawBufferUsage drawBufferUsage);
        void putIndices(juce::uint32* indices, int numIndices, SceneObject::Config::DrawBufferUsage drawBufferUsage);
        void bind();

        GLuint vertexBuffer, indexBuffer;
        int numIndices;
        int numVertices;
        bool initialized = false;
        CriticalSection mutex;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VertexBuffer)
    };
    
    VertexBuffer vertexBuffer;
    std::unique_ptr<Attributes> attributes;
    
    Atomic<bool> needToRender{true};
    
    inline static Identifier objectTypeID{"Type"};
    
    
};
