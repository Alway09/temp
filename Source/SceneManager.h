#pragma once
#include <JuceHeader.h>
#include "Scene.h"
#include "SamplesHolder.h"
#include "WaveformSceneObject.h"

using namespace juce;

enum SceneObjectRealisation
{
    Waveform
};

class SceneManager
{
public:
    SceneManager(SamplesHolder * const samplesHolder, ValueTree valueTree);
    ~SceneManager();
    
    Uuid createScene();
    Scene * const getScene(Uuid sceneID) const;
    HashMap<Uuid, Scene*>& getAllScenes() {
        return scenes;
    }
    
    void createSceneObject(Uuid sceneID, SceneObjectRealisation realisation);
private:
    HashMap<Uuid, Scene*> scenes;
    SamplesHolder * const samplesHolder;
    ValueTree valueTree;
};
