#include "WaveformSceneObjectEditor.h"

WaveformSceneObjectEditor::WaveformSceneObjectEditor(ValueTree treeEditTo) : SceneObjectEditor(treeEditTo)
{
    gainSlider = new SliderPropertyComponent(valueTree.getPropertyAsValue(WaveformSceneObject::IDs::gain, nullptr), "Gain", 1.0, 15.0, 0.1);
    controls.add(gainSlider);
    
    double skewFactor = 1.0 / 6;
    secondsToShowSlider = new SliderPropertyComponent(valueTree.getPropertyAsValue(WaveformSceneObject::IDs::secondsToShow, nullptr), "Seconds to show", 0.1, SamplesHolderHolder::secondsToHold, 0.01, skewFactor, false);
    controls.add(secondsToShowSlider);
}

WaveformSceneObjectEditor::~WaveformSceneObjectEditor() {
    
}
