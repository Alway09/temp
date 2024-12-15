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
        if(property == IDs::secondsToShow) {
            needToUpdateBuffer = false;
            float secondsToShow = valueTree.getProperty(property);
            samplesToShow = SamplesHolderHolder::sampleRate * secondsToShow;
            vertices.clearQuick();
            vertices.resize(samplesToShow);
            needToUpdateBuffer = true;
        }
    }
    
    struct IDs {
        inline static const Identifier gain{"Gain"};
        inline static const Identifier secondsToShow{"SecondsToShow"};
    };
    
private:
    Array<Vertex> vertices;
    int samplesToShow = 44100;
    
    //float secondsToShow = 1.f;
    float gain = 10.f;
};
