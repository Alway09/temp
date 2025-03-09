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
private:
    enum Commands
    {
        addScene = 200
    };
    
    void resized() override;
    
    void handleEditorClose();
    void buttonClicked (Button*) override {handleEditorClose();};
    
    void sceneMouseClicked(Scene& sc) override;
    void sceneDeleting(SceneComponent& sceneComponent) override;
    void sceneDetached(SceneComponent& component, bool isDetached) override;
    
    ApplicationCommandTarget* getNextCommandTarget() override { return findFirstTargetParentComponent(); }
    void getAllCommands (Array<CommandID> &commands) override { commands.add(Commands::addScene); }
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo &result) override;
    bool perform (const InvocationInfo &info) override;
    
    ScenesView scenesView;
    SceneEditor sceneEditor;
    
    Rectangle<int> scenesBound;
};
