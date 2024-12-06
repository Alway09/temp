#pragma once
#include <JuceHeader.h>

using namespace juce;

class GlobalOptionsComponent : public Component, ChangeListener
{

    
public:
    GlobalOptionsComponent(AudioDeviceManager& deviceManager, ValueTree treeAttachTo) : deviceManager(deviceManager)
    {
        tree = treeAttachTo.getOrCreateChildWithName(GlobalOptionsComponent::IDs::Global::name, nullptr);
        
        deviceSelector.reset(new AudioDeviceSelectorComponent(deviceManager, 0, 256, 0, 0, false, false, true, true));
        addAndMakeVisible(deviceSelector.get());
        deviceManager.addChangeListener(this);
        setSize(600, 600);
    }
    
    ~GlobalOptionsComponent() {
        deviceSelector.reset();
        deviceManager.removeChangeListener(this);
    }
    
    static void restore(ValueTree treeAttachTo, AudioDeviceManager& deviceManager) {
        ValueTree globalOptsTree = treeAttachTo.getChildWithName(GlobalOptionsComponent::IDs::Global::name);
        if(globalOptsTree.isValid()) {
            ValueTree audioDeviceSetupTree = globalOptsTree.getChildWithName(GlobalOptionsComponent::IDs::DeviceSetup::name);
            if(audioDeviceSetupTree.isValid()) {
                restoreAudioDeviceSetup(audioDeviceSetupTree, deviceManager);
            }
        }
    }
    
    void resized() override {
        deviceSelector->setBounds(getLocalBounds());
    }
    
private:
    
    static void restoreAudioDeviceSetup(ValueTree audioDeviceSetupTree, AudioDeviceManager& deviceManager) {
        using DS = GlobalOptionsComponent::IDs::DeviceSetup;
        
        AudioDeviceManager::AudioDeviceSetup setup;
        setup.outputDeviceName = audioDeviceSetupTree.getProperty(DS::outputDeviceName);
        setup.inputDeviceName = audioDeviceSetupTree.getProperty(DS::inputDeviceName);
        setup.sampleRate = audioDeviceSetupTree.getProperty(DS::sampleRate);
        setup.bufferSize = audioDeviceSetupTree.getProperty(DS::bufferSize);
        setup.inputChannels.parseString(StringRef(audioDeviceSetupTree.getProperty(DS::inputChannels)), 10);
        setup.useDefaultInputChannels = audioDeviceSetupTree.getProperty(DS::useDefaultInputChannels);
        setup.outputChannels.parseString(StringRef(audioDeviceSetupTree.getProperty(DS::outputChannels)), 10);
        setup.useDefaultOutputChannels = audioDeviceSetupTree.getProperty(DS::useDefaultOutputChannels);
        
        deviceManager.setAudioDeviceSetup(setup, false);
    }
    
    void changeListenerCallback(ChangeBroadcaster*) override {
        using DS = GlobalOptionsComponent::IDs::DeviceSetup;
        
        ValueTree setupTree = tree.getOrCreateChildWithName(DS::name, nullptr);
        
        AudioDeviceManager::AudioDeviceSetup setup = deviceManager.getAudioDeviceSetup();
        setupTree.setProperty(DS::outputDeviceName, setup.outputDeviceName, nullptr);
        setupTree.setProperty(DS::inputDeviceName, setup.inputDeviceName, nullptr);
        setupTree.setProperty(DS::sampleRate, setup.sampleRate, nullptr);
        setupTree.setProperty(DS::bufferSize, setup.bufferSize, nullptr);
        setupTree.setProperty(DS::inputChannels, setup.inputChannels.toString(10), nullptr);
        setupTree.setProperty(DS::useDefaultInputChannels, setup.useDefaultInputChannels, nullptr);
        setupTree.setProperty(DS::outputChannels, setup.outputChannels.toString(10), nullptr);
        setupTree.setProperty(DS::useDefaultOutputChannels, setup.useDefaultOutputChannels, nullptr);
    }
    
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
