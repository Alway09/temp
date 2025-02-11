#include "MainComponent.h"

MainComponent::MainComponent() : StatefulObject("root")
{
    setAudioChannels (2, 0);
    restoreState(operationalConfigFile);
    
    if(hasChild(AudioSettings::ID)) {
        AudioSettings(*this, getChildState(AudioSettings::ID), deviceManager);
    } else {
        AudioSettings audioSettings(*this, deviceManager);
    }
    
    sceneManagerComponent.reset(new SceneManagerComponent(*this));
    addAndMakeVisible(sceneManagerComponent.get());
    
    CommandManagerHolder::getInstance()->registerAllCommandsForTarget(this);
    CommandManagerHolder::getInstance()->getKeyMappings()->resetToDefaultMappings();
    addKeyListener(CommandManagerHolder::getInstance()->getKeyMappings());

    setSize (1200, 600);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
    CommandManagerHolder::destruct();
    CustomAudioBuffer::destruct();
}
//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    AudioIODevice* device = deviceManager.getCurrentAudioDevice();
    BigInteger activeInputChannels  = device->getActiveInputChannels();
    int maxInputChannels  = activeInputChannels.getHighestBit() + 1;
    
    CustomAudioBuffer::init(maxInputChannels);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    AudioIODevice* device = deviceManager.getCurrentAudioDevice();
    BigInteger activeInputChannels  = device->getActiveInputChannels();
    int maxInputChannels  = activeInputChannels.getHighestBit() + 1;
    
    float * const * writeBuffer = bufferToFill.buffer->getArrayOfWritePointers();
    AudioBuffer<float> b{writeBuffer, maxInputChannels, bufferToFill.startSample, bufferToFill.numSamples};
    CustomAudioBuffer::getInst()->append(b);
}
//==============================================================================
void MainComponent::getAllCommands(Array<CommandID> &commands) {
    commands.add(1);
    commands.add(2);
    
    #ifdef TESTS_ENABLED
    commands.add(9999);
    #endif
}

void MainComponent::getCommandInfo(CommandID commandID, ApplicationCommandInfo &result) {
    if(commandID == 1) {
        result.setInfo("Options", "Global options", "Options", ApplicationCommandInfo::CommandFlags::wantsKeyUpDownCallbacks);
        result.addDefaultKeypress(',', ModifierKeys::commandModifier);
    }
    
    if(commandID == 2) {
        result.setInfo("Save", "Save", "Save", ApplicationCommandInfo::CommandFlags::wantsKeyUpDownCallbacks);
        result.addDefaultKeypress('S', ModifierKeys::commandModifier);
    }
    
    #ifdef TESTS_ENABLED
    if(commandID == 9999) {
        result.setInfo("Tests", "Tests", "Tests", ApplicationCommandInfo::CommandFlags::wantsKeyUpDownCallbacks);
        result.addDefaultKeypress('T', ModifierKeys::commandModifier);
    }
    #endif
}

bool MainComponent::perform(const InvocationInfo &info) {
    if(info.commandID == 1) {
        if(info.isKeyDown) {
            showSettings();
        }
        return true;
    }
    
    if(info.commandID == 2) {
        if(info.isKeyDown) {
            saveState(operationalConfigFile);
        }
        return true;
    }
    
    #ifdef TESTS_ENABLED
    if(info.commandID == 9999) {
        if(info.isKeyDown) {
            DialogWindow::LaunchOptions opt;
            opt.dialogTitle = "Test";
            opt.content.set(new TestRunnerComponent(), true);
            opt.resizable = false;
            opt.launchAsync();
        }
        return true;
    }
    #endif
    
    return false;
}

void MainComponent::showSettings() {
    DialogWindow::LaunchOptions opt;
    opt.dialogTitle = "Options";
    opt.dialogBackgroundColour = getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId);
    opt.content.set(new GlobalOptionsComponent(*this, deviceManager), true);
    opt.resizable = false;
    opt.launchAsync();
}
//==============================================================================
void MainComponent::resized()
{
    auto r = getLocalBounds().reduced(4);
    sceneManagerComponent->setBounds(r);
}
