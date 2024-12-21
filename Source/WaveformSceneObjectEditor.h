#pragma once

#include "SceneObjectEditor.h"
#include "WaveformSceneObject.h"

class WaveformSceneObjectEditor : public SceneObjectEditor
{
public:
    WaveformSceneObjectEditor(SceneObject& objectEditTo);
    ~WaveformSceneObjectEditor();
    
private:
    SliderPropertyComponent* gainSlider;
    SliderPropertyComponent* secondsToShowSlider;
};
