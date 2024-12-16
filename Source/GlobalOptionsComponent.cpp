#include "GlobalOptionsComponent.h"

GlobalOptionsComponent::GlobalOptionsComponent(AudioDeviceManager& deviceManager, ValueTree treeAttachTo) : deviceManager(deviceManager)
{
    tree = treeAttachTo.getOrCreateChildWithName(GlobalOptionsComponent::IDs::Global::name, nullptr);
    
    deviceSelector.reset(new AudioDeviceSelectorComponent(deviceManager, 0, 256, 0, 0, false, false, true, true));
    addAndMakeVisible(deviceSelector.get());
    deviceManager.addChangeListener(this);
    setSize(600, 600);
}

GlobalOptionsComponent::~GlobalOptionsComponent() {
    deviceSelector.reset();
    deviceManager.removeChangeListener(this);
}

void GlobalOptionsComponent::resized() {
    deviceSelector->setBounds(getLocalBounds());
}

void GlobalOptionsComponent::changeListenerCallback(ChangeBroadcaster*) {
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

void GlobalOptionsComponent::restoreSettings(ValueTree treeAttachTo, AudioDeviceManager& deviceManager) {
    ValueTree globalOptsTree = treeAttachTo.getChildWithName(GlobalOptionsComponent::IDs::Global::name);
    if(globalOptsTree.isValid()) {
        ValueTree audioDeviceSetupTree = globalOptsTree.getChildWithName(GlobalOptionsComponent::IDs::DeviceSetup::name);
        if(audioDeviceSetupTree.isValid()) {
            restoreAudioDeviceSetup(audioDeviceSetupTree, deviceManager);
        }
    }
}

void GlobalOptionsComponent::restoreAudioDeviceSetup(ValueTree audioDeviceSetupTree, AudioDeviceManager& deviceManager) {
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

