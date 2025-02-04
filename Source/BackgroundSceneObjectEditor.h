#pragma once

#include "SceneObjectEditor.h"

class BackgroundSceneObjectEditor : public SceneObjectEditor
{
public:
    BackgroundSceneObjectEditor(SceneObject& objectEditTo) : SceneObjectEditor(objectEditTo) {}
    ~BackgroundSceneObjectEditor() {}
    
    void initControls() override {};
private:
};
