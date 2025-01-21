#pragma once
#include "SceneObject.h"
#include "SamplesHolderHolder.h"

class WaveformSceneObject : public SceneObject
{
public:
    WaveformSceneObject(StatefulObject& parent);
    WaveformSceneObject(StatefulObject& parent, ObjectState objectState);
    
    void fillBuffers() override;
    //void changeSettings(const Identifier& property) override;
    void stateChanged(const Identifier &property) override;
    
    SceneObjectRealisation getRealisation() override { return SceneObjectRealisation::Waveform; }
    
    struct IDs {
        inline static const Identifier gain{"Gain"};
        inline static const Identifier secondsToShow{"SecondsToShow"};
        
        static Array<Identifier> getAll() {
            return {gain, secondsToShow};
        }
        
        static var getDefault(Identifier identifier) {
            if(identifier == IDs::gain) {
                return 10.0f;
            };
            if(identifier == IDs::secondsToShow) {
                return 1.f;
            }
            
            throw std::invalid_argument("");
        }
    };
    
private:
    
    
    void init();
    Array<Vertex> vertices;
    int samplesToShow = 44100;
    float gain = 10.f;
};
