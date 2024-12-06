#pragma once

#include <JuceHeader.h>
#include "SceneManagerComponent.h"
#include "GlobalOptionsComponent.h"
#include "CommandManagerHolder.h"

#ifdef TESTS_ENABLED
#include "TestRunnerComponent.h"
#endif

using namespace juce;

class MainComponent  : public AudioAppComponent, public Timer, public ApplicationCommandTarget
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;
    //==============================================================================
    ApplicationCommandTarget* getNextCommandTarget () override {
        return findFirstTargetParentComponent();
        //return sceneManagerComponent.get();
    }
    
    void getAllCommands (Array<CommandID> &commands) override {
        commands.add(1);
        commands.add(2);
        
#ifdef TESTS_ENABLED
        commands.add(9999);
#endif
    }
    
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo &result) override {
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
    
    bool perform (const InvocationInfo &info) override {
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
    
    void restore() {
        File file("/Users/alvvay/Desktop/settings.xml"); // need premissions
        if(file.existsAsFile()) {
            std::unique_ptr<XmlElement> xml = XmlDocument::parse(file);
            tree = ValueTree::fromXml(*xml.get());
            
            GlobalOptionsComponent::restore(tree, deviceManager);
            
            if(tree.hasProperty(sliderID)) {
                slider.setValue(tree.getProperty(sliderID));
            } else {
                tree.setProperty(sliderID, 1.f, nullptr);
                slider.setValue(10.f);
            }
        }
    }
        
    //==============================================================================
    void timerCallback() override {
        //sceneManager->createSceneObject(sceneID, SceneObjectRealisation::Waveform);
        stopTimer();
    }
    //==============================================================================
private:
    const size_t samplesHolderChannelSize = 44100 * 60; // 60 seconds with sample rate 44100
    SamplesHolder samplesHolder;
    //Uuid sceneID;
    //std::unique_ptr<SceneManager> sceneManager;
    std::unique_ptr<SceneManagerComponent> sceneManagerComponent;
    ValueTree tree;
    Identifier sliderID{"slider"};
    Slider slider;
    //ApplicationCommandManager commandManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
