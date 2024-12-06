#pragma once
#include <JuceHeader.h>
#include "SceneManager.h"
#include "SceneWrapperComponent.h"
#include "CommandManagerHolder.h"
#include "SceneEditor.h"

using namespace juce;

class SceneManagerComponent : public Component, public ActionListener, public ApplicationCommandTarget, public Button::Listener, public ComponentListener
{
public:
    SceneManagerComponent(SamplesHolder * const samplesHolder, ValueTree valueTree) : sceneManager(samplesHolder, valueTree)
    {
        CommandManagerHolder::getInstance()->registerAllCommandsForTarget(this);
        addKeyListener(CommandManagerHolder::getInstance()->getKeyMappings());
        
        addAndMakeVisible(button); // without is keyboard events not working
        
        sceneEditor.addListener(this);
        addChildComponent(sceneEditor);
        //addAndMakeVisible(sceneEditor);
        
        sceneFlex.flexWrap = FlexBox::Wrap::wrap;
        sceneFlex.justifyContent = FlexBox::JustifyContent::center;
        sceneFlex.flexDirection = FlexBox::Direction::column;
    }
    
    ~SceneManagerComponent()
    {
        for(auto sc : scenes) {
            sc->removeAllActionListeners();
        }
    }
    
    void resized() override
    {
        auto localBounds = getLocalBounds();
        scenesBound = localBounds.withWidth((localBounds.getWidth() - scenesBoundPadding));
        
        sceneEditor.setBounds(localBounds.removeFromRight(scenesBoundPadding));
        
        //button.setBounds(getWidth()/2 - 50, getHeight()/2 - 50, 100, 100);
        
        //scenesGrid.performLayout(scenesBound);
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
            /*GridItem gi{createScene()};
            scenesGrid.items.add(gi);
            
            int w = scenesGrid.templateRows.size();
            int h = scenesGrid.templateColumns.size();
            int am = w * h;
            
            int evenTo = sqrt(scenesGrid.items.size());
            
            if(am == 0) {
                scenesGrid.templateRows.add(Grid::TrackInfo(Grid::Fr(1)));
                scenesGrid.templateColumns.add(Grid::TrackInfo(Grid::Fr(1)));
            } else if(scenesGrid.items.size() - 1 == am) {
                if(am % evenTo == 0) {
                    scenesGrid.templateRows.add(Grid::TrackInfo(Grid::Fr(1)));
                } else {
                    scenesGrid.templateColumns.add(Grid::TrackInfo(Grid::Fr(1)));
                    scenesGrid.templateRows.removeLast();
                }
                
            }*/
            
            if(sceneFlex.items.size() < amountLimit)
            {
                FlexItem fi{*createScene()};
                sceneFlex.items.add(fi.withMinWidth(min).withMinHeight(min).withFlex(1.f, 4.f, 1.f));
                
                resized();
            } else {
                DBG("Scenes limit amount");
            }
            
            return true;
        }
        
        return false;
    }
    
    void componentMovedOrResized (Component &component, bool wasMoved, bool wasResized) override {
        if(wasMoved) {
            DBG("MOVED");
            component.toFront(true);
        }
    }
    
private:
    Component* createScene() {
        Uuid scID = sceneManager.createScene();
        sceneManager.createSceneObject(scID, SceneObjectRealisation::Waveform);
        auto sc = new SceneWrapperComponent(sceneManager.getScene(scID), scID);
        scenes.add(sc);
        sc->addActionListener(this);
        addAndMakeVisible(sc);
        //sc->addToDesktop();
        //sc->getPeer()->setAlwaysOnTop(true);
        return sc;
    }
    
    enum Commands
    {
        addScene = 200
    };
    
    SceneManager sceneManager;
    OwnedArray<SceneWrapperComponent> scenes;
    //Grid scenesGrid;
    SceneEditor sceneEditor;
    TextButton button{"+"};
    FlexBox sceneFlex;
    int scenesBoundPadding = 0;
    Rectangle<int> scenesBound;
};
