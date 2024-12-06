#pragma once
#include "SceneObject.h"
#include "SamplesHolder.h"

class WaveformSceneObject : public SceneObject, public ValueTree::Listener
{
public:
    WaveformSceneObject(SamplesHolder* samplesHolder);
    
    void fillBuffers() override;
    
    void valueTreePropertyChanged(ValueTree &treeWhosePropertyHasChanged, const Identifier &property) override {
        gain = treeWhosePropertyHasChanged.getProperty(property, 1.f);
    }
private:
    Array<Vertex> vertices;
    int tps = 60;
    float gain = 10.f;
};
