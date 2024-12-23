#pragma once

#include <JuceHeader.h>
#include "SceneManagerComponent.h"
#include "GlobalOptionsComponent.h"
#include "CommandManagerHolder.h"
#include "SamplesHolderHolder.h"

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
    void paint(juce::Graphics& g) override;
    void resized() override;
    //==============================================================================
    ApplicationCommandTarget* getNextCommandTarget() override { return findFirstTargetParentComponent(); }
    void getAllCommands(Array<CommandID> &commands) override;
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo &result) override;
    bool perform(const InvocationInfo &info) override;
    //==============================================================================
    //void restoreSettings();
    //==============================================================================
private:
    std::unique_ptr<SceneManagerComponent> sceneManagerComponent;
    //ValueTree tree;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
