#include "SceneObject.h"

//==============================================================================
SceneObject::SceneObject(StatefulObject& parent, String namePrefix, SceneObjectRealisation realisation) : StatefulObject(parent, parent.getName(), namePrefix)
{
    setProperty(objectTypeID, realisation);
}

SceneObject::SceneObject(ObjectState objectState) : StatefulObject(objectState)
{
    
}

SceneObject::~SceneObject() {
    attributes.reset();
}

void SceneObject::reset(OpenGLShaderProgram& shaderProgram) {
    attributes.reset(new Attributes(shaderProgram));
}

void SceneObject::draw()
{
    using namespace ::juce::gl;
    
    if(!vertexBuffer.initialized) {
        vertexBuffer.initialize();
    }
    
    if(needToUpdateBuffer) {
        fillBuffers();
    }

    vertexBuffer.bind();
        
    attributes->enable();
    switch (config.source) {
        case SceneObject::Config::DrawSource::Vertices:
            glDrawArrays(config.primitiveType, 0, vertexBuffer.numVertices);
            break;
        case SceneObject::Config::DrawSource::Indices:
            glDrawElements (config.primitiveType, vertexBuffer.numIndices, GL_UNSIGNED_INT, nullptr);
            break;
    }
    attributes->disable();
}

void SceneObject::putVertices(Array<Vertex> vertices) {
    vertexBuffer.putVertices(vertices, config.drawBufferUsage);
}
void SceneObject::putIndices(juce::uint32* indices, int numIndices) {
    vertexBuffer.putIndices(indices, numIndices, config.drawBufferUsage);
}
//==============================================================================
SceneObject::VertexBuffer::VertexBuffer()
{

}

void SceneObject::VertexBuffer::initialize() {
    using namespace ::juce::gl;
    glGenBuffers (1, &vertexBuffer);
    glGenBuffers (1, &indexBuffer);
    
    initialized = true;
}

SceneObject::VertexBuffer::~VertexBuffer()
{
    using namespace ::juce::gl;

    if(initialized) {
        glDeleteBuffers (1, &vertexBuffer);
        glDeleteBuffers (1, &indexBuffer);
    }
}

void SceneObject::VertexBuffer::putIndices(juce::uint32 *indices, int numIndices, SceneObject::Config::DrawBufferUsage drawBufferUsage) {
    using namespace ::juce::gl;
    
    if(initialized) {
        const ScopedLock lock (mutex);
        
        this->numIndices = numIndices;
        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData (GL_ELEMENT_ARRAY_BUFFER,
                      static_cast<GLsizeiptr> (static_cast<size_t> (numIndices) * sizeof (juce::uint32)),
                      indices, drawBufferUsage);
    }
    
}

void SceneObject::VertexBuffer::putVertices(Array<Vertex> vertices, SceneObject::Config::DrawBufferUsage drawBufferUsage) {
    using namespace ::juce::gl;
    
    if(initialized) {
        const ScopedLock lock (mutex);
        
        numVertices = vertices.size();
        glBindBuffer (GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData (GL_ARRAY_BUFFER,
                      static_cast<GLsizeiptr> (static_cast<size_t> (numVertices) * sizeof (Vertex)),
                      vertices.getRawDataPointer(), drawBufferUsage);
    }
    
}

void SceneObject::VertexBuffer::bind()
{
    using namespace ::juce::gl;

    glBindBuffer (GL_ARRAY_BUFFER, vertexBuffer);
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
}

//==============================================================================
SceneObject::Attributes::Attributes(OpenGLShaderProgram& shaderProgram)
{
    position      .reset (createAttribute (shaderProgram, "position"));
    normal        .reset (createAttribute (shaderProgram, "normal"));
    sourceColour  .reset (createAttribute (shaderProgram, "sourceColour"));
    textureCoordIn.reset (createAttribute (shaderProgram, "textureCoordIn"));
}

void SceneObject::Attributes::enable()
{
    using namespace ::juce::gl;

    if (position.get() != nullptr)
    {
        glVertexAttribPointer (position->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), nullptr);
        glEnableVertexAttribArray (position->attributeID);
    }

    if (normal.get() != nullptr)
    {
        glVertexAttribPointer (normal->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 3));
        glEnableVertexAttribArray (normal->attributeID);
    }

    if (sourceColour.get() != nullptr)
    {
        glVertexAttribPointer (sourceColour->attributeID, 4, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 6));
        glEnableVertexAttribArray (sourceColour->attributeID);
    }

    if (textureCoordIn.get() != nullptr)
    {
        glVertexAttribPointer (textureCoordIn->attributeID, 2, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 10));
        glEnableVertexAttribArray (textureCoordIn->attributeID);
    }
}

void SceneObject::Attributes::disable()
{
    using namespace ::juce::gl;

    if (position != nullptr)       glDisableVertexAttribArray (position->attributeID);
    if (normal != nullptr)         glDisableVertexAttribArray (normal->attributeID);
    if (sourceColour != nullptr)   glDisableVertexAttribArray (sourceColour->attributeID);
    if (textureCoordIn != nullptr) glDisableVertexAttribArray (textureCoordIn->attributeID);
}

OpenGLShaderProgram::Attribute* SceneObject::Attributes::createAttribute(OpenGLShaderProgram& shader, const char* attributeName)
{
    using namespace ::juce::gl;

    if (glGetAttribLocation (shader.getProgramID(), attributeName) < 0)
        return nullptr;

    return new OpenGLShaderProgram::Attribute (shader, attributeName);
}
