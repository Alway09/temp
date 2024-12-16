#include "WaveformSceneObjectEditor.h"

WaveformSceneObjectEditor::WaveformSceneObjectEditor(ValueTree treeEditTo) : SceneObjectEditor(treeEditTo)
{
    gainSlider.setRange(1.0, 15.0);
    gainSlider.setValue(valueTree.getProperty(WaveformSceneObject::IDs::gain));
    gainSlider.onValueChange = [this] { valueTree.setProperty(WaveformSceneObject::IDs::gain, gainSlider.getValue(), nullptr); };
    addAndMakeVisible(gainSlider);
    
    secondsToShowSlider.setRange(0.01f, SamplesHolderHolder::secondsToHold);
    secondsToShowSlider.setSkewFactorFromMidPoint(1.f);
    secondsToShowSlider.setValue(valueTree.getProperty(WaveformSceneObject::IDs::secondsToShow));
    secondsToShowSlider.onValueChange = [this]{ valueTree.setProperty(WaveformSceneObject::IDs::secondsToShow, secondsToShowSlider.getValue(), nullptr); };
    addAndMakeVisible(secondsToShowSlider);
}

void WaveformSceneObjectEditor::resized() {
    auto localBounds = getLocalBounds();
    
    gainSlider.setBounds(localBounds.removeFromTop(getHeight()/2));
    secondsToShowSlider.setBounds(localBounds);
}
