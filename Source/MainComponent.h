#pragma once

#include <JuceHeader.h>
#include "SceneManagerComponent.h"
#include "GlobalOptionsComponent.h"
#include "CommandManagerHolder.h"
#include "CustomAudioBuffer.h"

#ifdef TESTS_ENABLED
#include "TestRunnerComponent.h"
#endif

using namespace juce;

class MainComponent  : public AudioAppComponent, public ApplicationCommandTarget, public StatefulObject
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;
    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override  {};
    //==============================================================================
    void resized() override;
    //==============================================================================
    ApplicationCommandTarget* getNextCommandTarget() override { return findFirstTargetParentComponent(); }
    void getAllCommands(Array<CommandID> &commands) override;
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo &result) override;
    bool perform(const InvocationInfo &info) override;
    //==============================================================================
private:
    void showSettings();
    
    std::unique_ptr<SceneManagerComponent> sceneManagerComponent;
    
    File operationalConfigFile = File::getSpecialLocation(File::SpecialLocationType::userHomeDirectory).getFullPathName()
    + File::getSeparatorString()
    + ".deleteMe"
    + File::getSeparatorString()
    + "operConfig.xml";

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
