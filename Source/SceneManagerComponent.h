#pragma once
#include <JuceHeader.h>
#include "SceneManager.h"
#include "CommandManagerHolder.h"
#include "SceneEditor.h"
#include "SceneComponent.h"
#include "ScenesView.h"

using namespace juce;

class SceneManagerComponent : public Component, public ActionListener, public ApplicationCommandTarget, public Button::Listener, public SceneComponent::Listener
{
public:
    SceneManagerComponent(SamplesHolder * const samplesHolder, ValueTree valueTree) : sceneManager(samplesHolder, valueTree)
    {
        CommandManagerHolder::getInstance()->registerAllCommandsForTarget(this);
        addKeyListener(CommandManagerHolder::getInstance()->getKeyMappings());
        
        addAndMakeVisible(button); // without it keyboard events is not working
        
        sceneEditor.addListener(this);
        addChildComponent(sceneEditor);
        
        
        
        addAndMakeVisible(scenesView);
        //scenesRender.reset(new ScenesRender(scenesView));
    }
    
    ~SceneManagerComponent()
    {
        //scenesRender.reset();
    }
    
    void resized() override
    {
        auto localBounds = getLocalBounds();
        scenesBound = localBounds.withWidth((localBounds.getWidth() - scenesBoundPadding));
        
        sceneEditor.setBounds(localBounds.removeFromRight(scenesBoundPadding));
        scenesView.setBounds(scenesBound);
    }
    
    void actionListenerCallback (const String &message) override
    {
        if(!sceneEditor.isVisible()) {
            sceneEditor.setVisible(true);
            scenesBoundPadding = 300;
            resized();
        }
        
        Uuid sceneID{message};
        auto sc = sceneManager.getScene(sceneID);
        sceneEditor.attach(sc);
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
            scenesView.addScene(createScene());
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
    SceneComponent* createScene() {
        Uuid tmp = sceneManager.createScene();
        Scene* sc = sceneManager.getScene(tmp);
        SceneComponent* scComp = new SceneComponent(sc);
        scComp->addSceneListener(this);
        sceneComponents.add(scComp);
        scComp->setResizable(true, true);
        
        return scComp;
    }
    
    enum Commands
    {
        addScene = 200
    };
    
    SceneManager sceneManager;
    OwnedArray<SceneComponent> sceneComponents;
    ScenesView scenesView;
    SceneEditor sceneEditor;
    TextButton button{"+"};
    FlexBox sceneFlex;
    int scenesBoundPadding = 0;
    Rectangle<int> scenesBound;
    //std::unique_ptr<ScenesRender> scenesRender;
};
