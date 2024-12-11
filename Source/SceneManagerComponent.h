#pragma once
#include <JuceHeader.h>
#include "SceneManager.h"
#include "CommandManagerHolder.h"
#include "SceneEditor.h"
#include "SceneComponent.h"
#include "ScenesRender.h"

using namespace juce;

class SceneManagerComponent : public Component, public ActionListener, public ApplicationCommandTarget, public Button::Listener
{
public:
    SceneManagerComponent(SamplesHolder * const samplesHolder, ValueTree valueTree) : sceneManager(samplesHolder, valueTree)
    {
        CommandManagerHolder::getInstance()->registerAllCommandsForTarget(this);
        addKeyListener(CommandManagerHolder::getInstance()->getKeyMappings());
        
        addAndMakeVisible(button); // without it keyboard events is not working
        
        sceneEditor.addListener(this);
        addChildComponent(sceneEditor);
        
        sceneFlex.flexWrap = FlexBox::Wrap::wrap;
        sceneFlex.justifyContent = FlexBox::JustifyContent::center;
        sceneFlex.flexDirection = FlexBox::Direction::column;
        
        scenesRender.reset(new ScenesRender(this));
    }
    
    ~SceneManagerComponent()
    {
        scenesRender.reset();
    }
    
    void resized() override
    {
        auto localBounds = getLocalBounds();
        scenesBound = localBounds.withWidth((localBounds.getWidth() - scenesBoundPadding));
        
        sceneEditor.setBounds(localBounds.removeFromRight(scenesBoundPadding));
        
        sceneFlex.performLayout(scenesBound);
        
    }
    
    void actionListenerCallback (const String &message) override
    {
        //DBG(message);
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
        static int min = 200;
        static int amountLimit = 6;
        if(info.commandID == Commands::addScene) {
            if(sceneFlex.items.size() < amountLimit)
            {
                FlexItem fi{*createScene()};
                sceneFlex.items.add(fi.withMinWidth(min).withMinHeight(min).withFlex(1.f, 4.f, 1.f));
                
                resized();
            } else {
                DBG("Scenes limit exceeded");
            }
            
            return true;
        }
        
        return false;
    }
    
private:
    Component* createScene() {
        Uuid tmp = sceneManager.createScene();
        Scene* sc = sceneManager.getScene(tmp);
        scenesRender->addScene(sc);
        SceneComponent* scComp = new SceneComponent(sc);
        scComp->setSceneListener(scenesRender.get());
        addAndMakeVisible(scComp);
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
    SceneEditor sceneEditor;
    TextButton button{"+"};
    FlexBox sceneFlex;
    int scenesBoundPadding = 0;
    Rectangle<int> scenesBound;
    std::unique_ptr<ScenesRender> scenesRender;
};
