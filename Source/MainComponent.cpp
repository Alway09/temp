#include "MainComponent.h"

MainComponent::MainComponent() : tree("root")
{

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 0); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 0);
    }
    
    restoreSettings();
    
    sceneManagerComponent.reset(new SceneManagerComponent(tree));
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
    SamplesHolderHolder::destruct();
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    AudioIODevice* device = deviceManager.getCurrentAudioDevice();
    BigInteger activeInputChannels  = device->getActiveInputChannels();
    int maxInputChannels  = activeInputChannels .getHighestBit() + 1;
    
    SamplesHolderHolder::getInstance()->prepare(maxInputChannels);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    AudioIODevice* device = deviceManager.getCurrentAudioDevice();
    BigInteger activeInputChannels  = device->getActiveInputChannels();
    int maxInputChannels  = activeInputChannels.getHighestBit() + 1;

    for (auto channel = 0; channel < maxInputChannels; ++channel)
    {
        auto* inBuffer = bufferToFill.buffer->getReadPointer(channel, bufferToFill.startSample);
        SamplesHolderHolder::getInstance()->add(inBuffer, bufferToFill.numSamples, channel);
    }
}

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
            DialogWindow::LaunchOptions opt;
            opt.dialogTitle = "Options";
            opt.dialogBackgroundColour = getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId);
            opt.content.set(new GlobalOptionsComponent(deviceManager, tree), true);
            opt.resizable = false;
            opt.launchAsync();
        }
        return true;
    }
    
    if(info.commandID == 2) {
        if(info.isKeyDown) {\
            File file("/Users/alvvay/Desktop/settings.xml"); // need premissions
            std::unique_ptr<XmlElement> xml = tree.createXml();
            xml->writeTo(file);
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
//==============================================================================
void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto r = getLocalBounds().reduced(4);

    sceneManagerComponent->setBounds(r);
}
//==============================================================================
void MainComponent::restoreSettings() {
    File file("/Users/alvvay/Desktop/settings.xml"); // need premissions
    if(file.existsAsFile()) {
        std::unique_ptr<XmlElement> xml = XmlDocument::parse(file);
        tree = ValueTree::fromXml(*xml.get());
        
        GlobalOptionsComponent::restoreSettings(tree, deviceManager);
    }
}
