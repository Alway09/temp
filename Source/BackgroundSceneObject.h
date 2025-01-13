#pragma once

#include "SceneObject.h"

class BackgroundSceneObject : public SceneObject
{
public:
    BackgroundSceneObject(StatefulObject& parent);
    BackgroundSceneObject(ObjectState objectState);
    ~BackgroundSceneObject();
    
    void fillBuffers() override;
    
    SceneObjectRealisation getRealisation() override { return SceneObjectRealisation::Background; }
    
private:
    void init();
    Array<Vertex> vertices;
    juce::uint32* indices = new juce::uint32[6];
};
