#include "SceneManagerComponent.h"

SceneManagerComponent::SceneManagerComponent(StatefulObject& parent) : scenesView(parent, this)
{
    CommandManagerHolder::getInstance()->registerAllCommandsForTarget(this);
    addKeyListener(CommandManagerHolder::getInstance()->getKeyMappings());
    
    addAndMakeVisible(button); // without it keyboard events is not working
    
    sceneEditor.addCloseButtonListener(this);
    addChildComponent(sceneEditor);
    
    addAndMakeVisible(scenesView);
}

void SceneManagerComponent::resized()
{
    auto localBounds = getLocalBounds();
    scenesBound = localBounds.withWidth((localBounds.getWidth() - scenesBoundPadding));
    
    sceneEditor.setBounds(localBounds.removeFromRight(scenesBoundPadding));
    scenesView.setBounds(scenesBound);
}

void SceneManagerComponent::buttonClicked (Button*) {
    if(!sceneEditor.isVisible())
        return;
    
    sceneEditor.setVisible(false);
    scenesBoundPadding = 0;
    resized();
}

void SceneManagerComponent::sceneMouseClicked(Scene& sc) {
    if(!sceneEditor.isVisible()) {
        sceneEditor.setVisible(true);
        scenesBoundPadding = 300;
        resized();
    }
    
    sceneEditor.attach(&sc);
}

void SceneManagerComponent::sceneDeleting(SceneComponent& sceneComponent) {
    if(sceneEditor.isAttachedTo(&sceneComponent.getScene())) {
        buttonClicked(nullptr);
    }
}

void SceneManagerComponent::getCommandInfo(CommandID commandID, ApplicationCommandInfo &result) {
    if(commandID == Commands::addScene) {
        result.setInfo("Add scene", "Add scene", "Scene", 0);
        result.addDefaultKeypress('A', ModifierKeys::noModifiers);
    }
}

bool SceneManagerComponent::perform(const InvocationInfo &info) {
    if(info.commandID == Commands::addScene) {
        scenesView.createScene(this);
        return true;
    }
    
    return false;
}
