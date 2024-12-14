#pragma once
#include "SceneObject.h"
#include "SamplesHolderHolder.h"

class WaveformSceneObject : public SceneObject
{
public:
    WaveformSceneObject(ValueTree treeAttachTo);
    
    void fillBuffers() override;
    
    void changeSettings(const Identifier& property) override {
        if(property == IDs::gain) {
            gain = valueTree.getProperty(property);
        }
    }
    
    struct IDs {
        inline static const Identifier gain{"Gain"};
    };
    
private:
    Array<Vertex> vertices;
    int tps = 60;
    float gain = 10.f;
};
