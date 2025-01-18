#include "WaveformSceneObjectEditor.h"

WaveformSceneObjectEditor::WaveformSceneObjectEditor(SceneObject& objectEditTo) : SceneObjectEditor(objectEditTo)
{
    gainSlider = new SliderPropertyComponent(object.getPropertyAsValue(WaveformSceneObject::IDs::gain), "Gain", 1.0, 15.0, 0.1);
    controls.add(gainSlider);
    
    double skewFactor = 1.0 / 6;
    secondsToShowSlider = new SliderPropertyComponent(object.getPropertyAsValue(WaveformSceneObject::IDs::secondsToShow), "Seconds to show", 0.1, SamplesHolderHolder::secondsToHold, 0.01, skewFactor, false);
    controls.add(secondsToShowSlider);
    
    for(auto c : controls) {
        addAndMakeVisible(c);
    }
}

WaveformSceneObjectEditor::~WaveformSceneObjectEditor() {
    
}
