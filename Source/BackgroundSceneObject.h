#pragma once

#include "SceneObject.h"

class BackgroundSceneObject : public SceneObject
{
public:
    BackgroundSceneObject(ValueTree treeAttachTo) : SceneObject(treeAttachTo, SceneObjectRealisation::Background)
    {
        config.source = SceneObject::Config::DrawSource::Indices;
        config.primitiveType = SceneObject::Config::DrawPrimitiveType::Triangles;
        config.drawBufferUsage = SceneObject::Config::DrawBufferUsage::Static;
        
        vertices.add({{-1.f, -1.f, 0.f},
                      {0.5f, 0.5f, 0.5f},
                      {0.f, 0.f, 0.2f, 1.f},
                      {2.f, 2.f}});
        vertices.add({{-1.f, 1.f, 0.f},
                      {0.5f, 0.5f, 0.5f},
                      {0.f, 0.f, 0.2f, 1.f},
                      {2.f, 2.f}});
        vertices.add({{1.f, -1.f, 0.f},
                      {0.5f, 0.5f, 0.5f},
                      {0.f, 0.f, 0.2f, 1.f},
                      {2.f, 2.f}});
        vertices.add({{1.f, 1.f, 0.f},
                      {0.5f, 0.5f, 0.5f},
                      {0.f, 0.f, 0.2f, 1.f},
                      {2.f, 2.f}});
        
        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;
        indices[3] = 1;
        indices[4] = 2;
        indices[5] = 3;
        
        needToUpdateBuffer = true;
    }
    
    void fillBuffers() override {
        putVertices(vertices);
        putIndices(indices, 6);
        
        needToUpdateBuffer = false;
    }
    
    ~BackgroundSceneObject() {
        delete[] indices;
    }
    
    void changeSettings(const Identifier&) override {};
    
private:
    Array<Vertex> vertices;
    juce::uint32* indices = new juce::uint32[6];
};
