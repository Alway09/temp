#include "GlobalOptionsComponent.h"

GlobalOptionsComponent::GlobalOptionsComponent(StatefulObject& parent, AudioDeviceManager& deviceManager) : audioSettings(new AudioSettings(parent, deviceManager))
{
    addAndMakeVisible(audioSettings);
    setSize(600, 600);
}

GlobalOptionsComponent::~GlobalOptionsComponent() {
    
}

void GlobalOptionsComponent::resized() {
    audioSettings.setBounds(getLocalBounds());
}
