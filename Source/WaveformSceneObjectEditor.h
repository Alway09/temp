#pragma once

#include "SceneObjectEditor.h"
#include "WaveformSceneObject.h"

class WaveformSceneObjectEditor : public SceneObjectEditor
{
public:
    WaveformSceneObjectEditor(SceneObject& objectEditTo);
    ~WaveformSceneObjectEditor();
    
    void initControls() override;
private:
    SliderPropertyComponent* gainSlider;
    SliderPropertyComponent* secondsToShowSlider;
};
