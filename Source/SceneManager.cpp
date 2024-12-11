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
    Scene* scene = new Scene(valueTree);
    scenes.set(scene->getUuidIdentifier(), scene);
    createSceneObject(scene, SceneObjectRealisation::Background);
    createSceneObject(scene, SceneObjectRealisation::Waveform);
    return scene->getUuidIdentifier();
}

Scene * const SceneManager::getScene(Uuid sceneID) const {
    return scenes[sceneID];
}

void SceneManager::createSceneObject(Scene* scene, SceneObjectRealisation realisation) {
    switch (realisation) {
        case SceneObjectRealisation::Waveform:
        {
            WaveformSceneObject * wf = new WaveformSceneObject(samplesHolder);
            valueTree.addListener(wf);
            scene->createObject(wf);
            break;
        }
        case SceneObjectRealisation::Background:
        {
            BackgroundSceneObject* bg = new BackgroundSceneObject(samplesHolder);
            scene->createObject(bg);
            break;
        }
    }
}

void SceneManager::createSceneObject(Uuid sceneID, SceneObjectRealisation realisation)
{
    Scene* scene = scenes[sceneID];
    createSceneObject(scene, realisation);
    
}
