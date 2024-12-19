#pragma once

#include "SceneObjectEditor.h"
#include "WaveformSceneObject.h"

class WaveformSceneObjectEditor : public SceneObjectEditor
{
public:
    WaveformSceneObjectEditor(ValueTree treeEditTo);
    ~WaveformSceneObjectEditor();
    
private:
    SliderPropertyComponent* gainSlider;
    SliderPropertyComponent* secondsToShowSlider;
};
