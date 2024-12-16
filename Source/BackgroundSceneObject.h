#pragma once

#include "SceneObject.h"

class BackgroundSceneObject : public SceneObject
{
public:
    BackgroundSceneObject(ValueTree treeAttachTo);
    ~BackgroundSceneObject();
    
    void fillBuffers() override;
    void changeSettings(const Identifier&) override {};
    
private:
    Array<Vertex> vertices;
    juce::uint32* indices = new juce::uint32[6];
};
