#pragma once

#include "SceneObject.h"

class BackgroundSceneObject : public SceneObject
{
public:
    BackgroundSceneObject(StatefulObject& parent);
    ~BackgroundSceneObject();
    
    void fillBuffers() override;
    //void changeSettings(const Identifier&) override {};
    
    SceneObjectRealisation getRealisation() override { return SceneObjectRealisation::Background; }
    
private:
    Array<Vertex> vertices;
    juce::uint32* indices = new juce::uint32[6];
};
