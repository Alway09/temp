#pragma once
#include "SceneObject.h"
#include "CustomAudioBuffer.h"

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
    
    void updateBuffer() {
        vertices.resize(samplesToShow);
        float step = 2.f/samplesToShow;
        for(int i = 0; i < vertices.size(); ++i) {
            vertices.setUnchecked(i, {{-1.f + i * step, 0.f,  0.f},
                {0.5f, 0.5f, 0.5f},
                {0.95f, 0.57f, 0.03f, 0.7f},
                {2.f, 2.f}});
        }
    }
    
    Array<Vertex> vertices;
    int samplesToShow = 44100;
    float gain = 10.f;
};
