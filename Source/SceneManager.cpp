#include "SceneManager.h"

SceneManager::SceneManager(ValueTree treeAttachTo) : treeAttachTo(treeAttachTo)
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
    /*Scene* scene = new Scene(treeAttachTo);
    scenes.set(scene->getUuidIdentifier(), scene);
    createSceneObject(scene, SceneObjectRealisation::Background);
    createSceneObject(scene, SceneObjectRealisation::Waveform);
    return scene->getUuidIdentifier();*/
}

void SceneManager::deleteScene(Uuid sceneID) {
    Scene* scene = getScene(sceneID);
    //NameGenerator::freeName("Global", "Scene", scene->getName());
    //ValueTree sceneTree = scene->getValueTree();
    //sceneTree.getParent().removeChild(sceneTree, nullptr);
    delete scene;
    scenes.remove(sceneID);
}

Scene * const SceneManager::getScene(Uuid sceneID) const {
    return scenes[sceneID];
}

void SceneManager::createSceneObject(Scene* scene, SceneObjectRealisation realisation) {
    switch (realisation) {
        case SceneObjectRealisation::Waveform:
        {
            WaveformSceneObject * wf = new WaveformSceneObject(*scene);
            scene->createObject(wf);
            break;
        }
        case SceneObjectRealisation::Background:
        {
            BackgroundSceneObject* bg = new BackgroundSceneObject(*scene);
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
