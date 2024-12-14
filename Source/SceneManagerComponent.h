#pragma once
#include <JuceHeader.h>
#include "CommandManagerHolder.h"
#include "SceneEditor.h"
#include "SceneComponent.h"
#include "ScenesView.h"

using namespace juce;

class SceneManagerComponent : public Component, public ApplicationCommandTarget, public Button::Listener, public SceneComponent::Listener
{
public:
    SceneManagerComponent(ValueTree valueTree) : scenesView(valueTree)
    {
        CommandManagerHolder::getInstance()->registerAllCommandsForTarget(this);
        addKeyListener(CommandManagerHolder::getInstance()->getKeyMappings());
        
        addAndMakeVisible(button); // without it keyboard events is not working
        
        sceneEditor.addListener(this);
        addChildComponent(sceneEditor);
        
        addAndMakeVisible(scenesView);
    }
    
    ~SceneManagerComponent()
    {
        
    }
    
    void resized() override
    {
        auto localBounds = getLocalBounds();
        scenesBound = localBounds.withWidth((localBounds.getWidth() - scenesBoundPadding));
        
        sceneEditor.setBounds(localBounds.removeFromRight(scenesBoundPadding));
        scenesView.setBounds(scenesBound);
    }
    
    void buttonClicked (Button*) override {
        if(!sceneEditor.isVisible())
            return;
        
        sceneEditor.setVisible(false);
        scenesBoundPadding = 0;
        resized();
    }
    
    ApplicationCommandTarget* getNextCommandTarget () override {
        return findFirstTargetParentComponent();
    }
    
    void getAllCommands (Array<CommandID> &commands) override {
        commands.add(Commands::addScene);
    }
    
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo &result) override {
        if(commandID == Commands::addScene) {
            result.setInfo("Add scene", "Add scene", "Scene", 0);
            result.addDefaultKeypress('A', ModifierKeys::noModifiers);
        }
    }
    
    bool perform (const InvocationInfo &info) override {
        if(info.commandID == Commands::addScene) {
            scenesView.createScene(this);
            return true;
        }
        
        return false;
    }
    
    void sceneMouseClicked(Scene* sc) override {
        if(!sceneEditor.isVisible()) {
            sceneEditor.setVisible(true);
            scenesBoundPadding = 300;
            resized();
        }
        
        sceneEditor.attach(sc);
    }
    
private:
    
    enum Commands
    {
        addScene = 200
    };
    
    ScenesView scenesView;
    SceneEditor sceneEditor;
    TextButton button{"+"};
    
    int scenesBoundPadding = 0;
    Rectangle<int> scenesBound;
};
