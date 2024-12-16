#pragma once

#include "SceneObjectEditor.h"
#include "WaveformSceneObject.h"

class WaveformSceneObjectEditor : public SceneObjectEditor
{
public:
    WaveformSceneObjectEditor(ValueTree treeEditTo);
    
    void resized() override;
private:
    Slider gainSlider;
    Slider secondsToShowSlider;
};
