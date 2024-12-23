#pragma once

#include "AudioSettings.h"

class AudioSettingsComponent : public Component
{
public:
    AudioSettingsComponent(AudioSettings* audioSettings) : audioSettings(audioSettings) 
    {
        deviceSelector.reset(new AudioDeviceSelectorComponent(audioSettings->getDeviceManager(), 0, 256, 0, 0, false, false, true, true));
        addAndMakeVisible(deviceSelector.get());
    }
    
    ~AudioSettingsComponent() {
        delete audioSettings;
        deviceSelector.reset();
    }
    
    void resized() override {
        deviceSelector->setBounds(getLocalBounds());
    }
    
private:
    AudioSettings* audioSettings;
    std::unique_ptr<AudioDeviceSelectorComponent> deviceSelector;
};
