#pragma once
#include <JuceHeader.h>
#include "AudioSettingsComponent.h"

using namespace juce;

class GlobalOptionsComponent : public Component
{
public:
    GlobalOptionsComponent(StatefulObject& parent, AudioDeviceManager& deviceManager);
    ~GlobalOptionsComponent();
    
    void resized() override;
    
private:
    struct IDs
    {
        struct Global
        {
            inline static const Identifier name{"GlobalOptions"};
        };
    };
    
    AudioSettingsComponent audioSettings;
};
