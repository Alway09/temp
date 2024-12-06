#include "SceneManager.h"

SceneManager::SceneManager(SamplesHolder * const samplesHolder, ValueTree valueTree) : samplesHolder(samplesHolder), valueTree(valueTree)
{
    
}

SceneManager::~SceneManager() {
    auto iter = scenes.begin();
    while(iter != scenes.end()) {
        delete iter.getValue();
        iter.next();
    }
    scenes.clear();
}

Uuid SceneManager::createScene() {
    Uuid uuid;
    scenes.set(uuid, new Scene(valueTree));
    return uuid;
}

Scene * const SceneManager::getScene(Uuid sceneID) const {
    return scenes[sceneID];
}

void SceneManager::createSceneObject(Uuid sceneID, SceneObjectRealisation realisation)
{
    Scene* scene = scenes[sceneID];
    
    switch (realisation) {
        case SceneObjectRealisation::Waveform:
            WaveformSceneObject * obj = new WaveformSceneObject(samplesHolder);
            valueTree.addListener(obj);
            scene->createObject(obj);
            break;
    }
}
