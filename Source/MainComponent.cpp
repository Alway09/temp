#include "MainComponent.h"

MainComponent::MainComponent() : samplesHolder(samplesHolderChannelSize), tree("root")
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
    
    restore();
    
    sceneManagerComponent.reset(new SceneManagerComponent(&samplesHolder, tree));
    addAndMakeVisible(sceneManagerComponent.get());
//    sceneManager.reset(new SceneManager(&samplesHolder, tree));
//    sceneID = sceneManager->createScene();
//    addAndMakeVisible(sceneManager->getScene(sceneID));    
    
    slider.onValueChange = [this] { tree.setProperty(sliderID, slider.getValue(), nullptr); };
    slider.setRange(0.1f, 15.f);
    
    addAndMakeVisible(slider);
    
    
    CommandManagerHolder::getInstance()->registerAllCommandsForTarget(this);
    CommandManagerHolder::getInstance()->getKeyMappings()->resetToDefaultMappings();
    addKeyListener(CommandManagerHolder::getInstance()->getKeyMappings());

    setSize (1200, 600);
    //startTimer(1000);
}

MainComponent::~MainComponent()
{
    CommandManagerHolder::destruct();
    shutdownAudio();
    //sceneManager = nullptr;
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    AudioIODevice* device = deviceManager.getCurrentAudioDevice();
    BigInteger activeInputChannels  = device->getActiveInputChannels();
    int maxInputChannels  = activeInputChannels .getHighestBit() + 1;
    
    samplesHolder.prepare(maxInputChannels);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    AudioIODevice* device = deviceManager.getCurrentAudioDevice();
    BigInteger activeInputChannels  = device->getActiveInputChannels();
    int maxInputChannels  = activeInputChannels.getHighestBit() + 1;

    for (auto channel = 0; channel < maxInputChannels; ++channel)
    {
        auto* inBuffer = bufferToFill.buffer->getReadPointer(channel, bufferToFill.startSample);
        samplesHolder.add(inBuffer, bufferToFill.numSamples, channel);
    }
}

void MainComponent::releaseResources() {}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto r = getLocalBounds().reduced(4);

    //sceneManager->getScene(sceneID)->setBounds(r.removeFromTop(getHeight() - 70));
    sceneManagerComponent->setBounds(r.removeFromTop(getHeight() - 70));
    slider.setBounds(r);
}
