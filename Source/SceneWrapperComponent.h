#pragma once

#include <JuceHeader.h>
#include "Scene.h"
#include "CommandManagerHolder.h"

using namespace juce;



class SceneWrapperComponent : public ResizableWindow, public ActionBroadcaster, public ApplicationCommandTarget
{
public:
    
    SceneWrapperComponent(Scene * const scene, Uuid sceneID) : ResizableWindow("Scene", false), scene(scene), sceneID(sceneID)
    {
        CommandManagerHolder::getInstance()->registerAllCommandsForTarget(this);
        addKeyListener(CommandManagerHolder::getInstance()->getKeyMappings());
        
        setContentNonOwned(scene, false);
        setResizable(true, true);
        centreWithSize (300, 200);
        setVisible (true);
    };
    
    ~SceneWrapperComponent() {
        clearContentComponent();
    }
    
    Uuid getId() const
    {
        return sceneID;
    }
    
    void mouseUp(const MouseEvent &event) override
    {
        if(testClick(event)) {
            auto str{sceneID};
            sendActionMessage(sceneID.toString());
        }
    }
    
    ApplicationCommandTarget* getNextCommandTarget () override {
        return findFirstTargetParentComponent();
    }
    
    void getAllCommands (Array<CommandID> &commands) override {
        commands.add(Commands::switchFullscreen);
    }
    
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo &result) override {
        if(commandID == Commands::switchFullscreen) {
            result.setInfo("Fullscreen", "Switches fullscreen mode", "Options", 0);
            result.addDefaultKeypress('F', ModifierKeys::noModifiers);
        }
    }
    
    bool perform (const InvocationInfo &info) override {
        if(info.commandID == Commands::switchFullscreen) {
            if(isFullScreen()) { // need to resize component
                setFullScreen(false);
            } else {
                setFullScreen(true);
             }
            
            return true;
        }
        
        return false;
    }

private:
    bool testClick(const MouseEvent& e) {
        return e.mods.testFlags(ModifierKeys::leftButtonModifier) && e.mouseWasClicked() && e.getNumberOfClicks() == 1;
    }
    
    enum Commands
    {
        switchFullscreen = 100
    };
    Scene * const scene;
    Uuid sceneID;
    
};
