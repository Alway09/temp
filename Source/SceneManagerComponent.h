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
    SceneManagerComponent(StatefulObject& parent);
    ~SceneManagerComponent() {}
    
    void resized() override;
    
    void buttonClicked (Button*) override;
    
    void sceneMouseClicked(Scene* sc) override;
    void sceneDeleteButtonClicked(SceneComponent* sceneComponent) override;
    
    ApplicationCommandTarget* getNextCommandTarget() override { return findFirstTargetParentComponent(); }
    void getAllCommands (Array<CommandID> &commands) override { commands.add(Commands::addScene); }
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo &result) override;
    bool perform (const InvocationInfo &info) override;
private:
    enum Commands
    {
        addScene = 200
    };
    
    ScenesView scenesView;
    SceneEditor sceneEditor;
    TextButton button{"+"};
    
    int scenesBoundPadding = 0;
    //const int sceneEditorWidth = 300;
    Rectangle<int> scenesBound;
};
