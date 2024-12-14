#pragma once

#include "SceneObjectEditor.h"
#include "WaveformSceneObject.h"

class WaveformSceneObjectEditor : public SceneObjectEditor
{
public:
    WaveformSceneObjectEditor(ValueTree treeEditTo) : SceneObjectEditor(treeEditTo) 
    {
        gainSlider.setRange(1.0, 15.0);
        gainSlider.setValue(valueTree.getProperty(WaveformSceneObject::IDs::gain));
        gainSlider.onValueChange = [this] { valueTree.setProperty(WaveformSceneObject::IDs::gain, gainSlider.getValue(), nullptr); };
        addAndMakeVisible(gainSlider);
    }
    
    void resized() override {
        gainSlider.setBounds(getLocalBounds());
    }
private:
    Slider gainSlider;
};
