#pragma once
#include "StatefulObject.h"

class AudioSettings : public StatefulObject, public ChangeListener
{
public:
    AudioSettings(StatefulObject& parent, AudioDeviceManager& deviceManager) : StatefulObject(parent, ID.toString(), false), deviceManager(deviceManager)
    {
        deviceManager.addChangeListener(this);
        setStateFromDevice();
    }
    
    AudioSettings(StatefulObject& parent, const ObjectState& state, AudioDeviceManager& deviceManager) : StatefulObject(parent, state, false), deviceManager(deviceManager)
    {
        deviceManager.addChangeListener(this);
        setDeviceFromState();
    }
    
    ~AudioSettings() {
        deviceManager.removeChangeListener(this);
    }
    
    AudioDeviceManager& getDeviceManager() { return deviceManager; };
    
    inline static const Identifier ID{"AudioSettings"};
private:
    void changeListenerCallback(ChangeBroadcaster*) override {
        setStateFromDevice();
    }
    
    struct IDs
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
    
    void setDeviceFromState() {
        AudioDeviceManager::AudioDeviceSetup setup;
        setup.outputDeviceName = getProperty(IDs::outputDeviceName);
        setup.inputDeviceName = getProperty(IDs::inputDeviceName);
        setup.sampleRate = getProperty(IDs::sampleRate);
        setup.bufferSize = getProperty(IDs::bufferSize);
        setup.inputChannels.parseString(StringRef(getProperty(IDs::inputChannels)), 10);
        setup.useDefaultInputChannels = getProperty(IDs::useDefaultInputChannels);
        setup.outputChannels.parseString(StringRef(getProperty(IDs::outputChannels)), 10);
        setup.useDefaultOutputChannels = getProperty(IDs::useDefaultOutputChannels);
        
        deviceManager.setAudioDeviceSetup(setup, false);
    }
    
    void setStateFromDevice() {
        AudioDeviceManager::AudioDeviceSetup setup = deviceManager.getAudioDeviceSetup();
        setProperty(IDs::outputDeviceName, setup.outputDeviceName);
        setProperty(IDs::inputDeviceName, setup.inputDeviceName);
        setProperty(IDs::sampleRate, setup.sampleRate);
        setProperty(IDs::bufferSize, setup.bufferSize);
        setProperty(IDs::inputChannels, setup.inputChannels.toString(10));
        setProperty(IDs::useDefaultInputChannels, setup.useDefaultInputChannels);
        setProperty(IDs::outputChannels, setup.outputChannels.toString(10));
        setProperty(IDs::useDefaultOutputChannels, setup.useDefaultOutputChannels);
    }
    
    AudioDeviceManager& deviceManager;
};
