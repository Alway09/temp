#pragma once

#include "WaveformSceneObject.h"
#include "BackgroundSceneObject.h"

struct SceneObjectRealisationHelper {
    static SceneObjectRealisation fromInt(int i) {
        if(i == 0) {
            return SceneObjectRealisation::Waveform;
        }
        
        if(i == 1) {
            return SceneObjectRealisation::Background;
        }
        
        throw std::invalid_argument("Number of SceneObjectRealisation is incorrect.");
    }
    
    static SceneObjectRealisation fromString(String str) {
        if(waveform == str) {
            return SceneObjectRealisation::Waveform;
        }
        
        if(background == str) {
            return SceneObjectRealisation::Background;
        }
        
        throw std::invalid_argument("String name of SceneObjectRealisation is incorrect.");
    }
    
    static Array<SceneObjectRealisation> getAll() {
        return {SceneObjectRealisation::Waveform, SceneObjectRealisation::Background};
    }
    
    static const String& toString(SceneObjectRealisation r) {
        switch (r) {
            case SceneObjectRealisation::Waveform:
                return waveform;
                break;
            case SceneObjectRealisation::Background:
                return background;
                break;
        }
    }
    
    static SceneObject* createObject(SceneObjectRealisation realisation, StatefulObject& parent, StatefulObject::ObjectState* objectState = nullptr) {
        SceneObject* obj = nullptr;
        
        switch(realisation) {
        case Waveform:
            {
                obj = objectState ? new WaveformSceneObject(parent, *objectState) : new WaveformSceneObject(parent);
                break;
            }
        case Background:
            {
                obj = objectState ? new BackgroundSceneObject(parent, *objectState) : new BackgroundSceneObject(parent);
                break;
            }
        }
        
        return obj;
    }
    
    static SceneObjectRealisation fromState(StatefulObject::ObjectState& state) {
        return fromInt(state.getTree().getProperty("Type"));
    }
    
private:
    inline static const String waveform{"Waveform"};
    inline static const String background{"Background"};
};
