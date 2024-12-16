#pragma once
#include "SceneObject.h"
#include "SamplesHolderHolder.h"

class WaveformSceneObject : public SceneObject
{
public:
    WaveformSceneObject(ValueTree treeAttachTo);
    
    void fillBuffers() override;
    void changeSettings(const Identifier& property) override;
    
    struct IDs {
        inline static const Identifier gain{"Gain"};
        inline static const Identifier secondsToShow{"SecondsToShow"};
    };
    
private:
    Array<Vertex> vertices;
    int samplesToShow = 44100;
    float gain = 10.f;
};
