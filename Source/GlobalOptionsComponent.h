#pragma once
#include <JuceHeader.h>

using namespace juce;

class GlobalOptionsComponent : public Component, ChangeListener
{
public:
    GlobalOptionsComponent(AudioDeviceManager& deviceManager, ValueTree treeAttachTo);
    ~GlobalOptionsComponent();
    
    void resized() override;
    
    static void restoreSettings(ValueTree treeAttachTo, AudioDeviceManager& deviceManager);
private:
    void changeListenerCallback(ChangeBroadcaster*) override;
    static void restoreAudioDeviceSetup(ValueTree audioDeviceSetupTree, AudioDeviceManager& deviceManager);
    
    struct IDs
    {
        struct Global
        {
            inline static const Identifier name{"GlobalOptions"};
        };
        
        struct DeviceSetup
        {
            inline static const Identifier name{"AudioDeviceSetup"};
            inline static const Identifier outputDeviceName{"outputDeviceName"};
            inline static const Identifier inputDeviceName{"inputDeviceName"};
            inline static const Identifier sampleRate{"sampleRate"};
            inline static const Identifier bufferSize{"bufferSize"};
            inline static const Identifier inputChannels{"inputChannels"};
            inline static const Identifier useDefaultInputChannels{"useDefaultInputChannels"};
            inline static const Identifier outputChannels{"outputChannels"};
            inline static const Identifier useDefaultOutputChannels{"useDefaultOutputChannels"};
        };
    };
    
    ValueTree tree;
    std::unique_ptr<AudioDeviceSelectorComponent> deviceSelector;
    AudioDeviceManager& deviceManager;
};
