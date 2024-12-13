#pragma once
#include <JuceHeader.h>
#include "SamplesHolder.h"

using namespace juce;

class SceneObject
{
public:
    
    SceneObject(SamplesHolder * const samplesHolder);
    virtual ~SceneObject();
    void draw();
    
    void reset(OpenGLShaderProgram& shaderProgram);
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
    
    SamplesHolder * const samplesHolder;
    bool needToUpdateBuffer = false;
private:
    //==============================================================================
    class Attributes
    {
    public:
        explicit Attributes(OpenGLShaderProgram& shaderProgram);
        void enable();
        void disable();

        std::unique_ptr<OpenGLShaderProgram::Attribute> position, normal, sourceColour, textureCoordIn;
    private:
        static OpenGLShaderProgram::Attribute* createAttribute (OpenGLShaderProgram& shader, const char* attributeName) {
            using namespace ::juce::gl;

            if (glGetAttribLocation (shader.getProgramID(), attributeName) < 0)
                return nullptr;

            return new OpenGLShaderProgram::Attribute (shader, attributeName);
        }
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
};
