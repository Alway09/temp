#pragma once
#include "SceneObject.h"
#include "SamplesHolderHolder.h"

class WaveformSceneObject : public SceneObject
{
public:
    WaveformSceneObject(StatefulObject& parent);
    
    void fillBuffers() override;
    //void changeSettings(const Identifier& property) override;
    void stateChanged(const Identifier &property) override;
    
    SceneObjectRealisation getRealisation() override { return SceneObjectRealisation::Waveform; }
    
    struct IDs {
        inline static const Identifier gain{"Gain"};
        inline static const Identifier secondsToShow{"SecondsToShow"};
    };
    
private:
    Array<Vertex> vertices;
    int samplesToShow = 44100;
    float gain = 10.f;
};
