#pragma once
#include <JuceHeader.h>
#include "Scene.h"
#include "SamplesHolder.h"
#include "WaveformSceneObject.h"
#include "BackgroundSceneObject.h"

using namespace juce;

class SceneManager
{
public:
    SceneManager(ValueTree treeAttachTo);
    ~SceneManager();
    
    Uuid createScene();
    Scene * const getScene(Uuid sceneID) const;
    void deleteScene(Uuid sceneID);
    
    void createSceneObject(Uuid sceneID, SceneObjectRealisation realisation);
private:
    void createSceneObject(Scene* scene, SceneObjectRealisation realisation);
    HashMap<Uuid, Scene*> scenes;
    ValueTree treeAttachTo;
};
