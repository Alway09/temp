#include "SceneManagerComponent.h"

SceneManagerComponent::SceneManagerComponent(StatefulObject& parent) : scenesView(parent, this)
{
    CommandManagerHolder::getInstance()->registerAllCommandsForTarget(this);
    addKeyListener(CommandManagerHolder::getInstance()->getKeyMappings());
    
    setWantsKeyboardFocus(true);
    
    sceneEditor.addCloseButtonListener(this);
    addChildComponent(sceneEditor);
    
    scenesView.setBounds(0, 0, 10, 10); // set bound to avoid context init crash
    if(scenesView.hasChildren()) {
        addAndMakeVisible(scenesView);
    } else {
        addChildComponent(scenesView);
    }
}

void SceneManagerComponent::resized()
{
    auto localBounds = getLocalBounds();
    if(scenesView.isVisible()) {
        scenesBound = localBounds.withWidth((localBounds.getWidth() - sceneEditor.getCurrentWidth()));
        sceneEditor.setBounds(localBounds.removeFromRight(sceneEditor.getCurrentWidth()));
        scenesView.setBounds(scenesBound);
    } else {
        sceneEditor.setBounds(localBounds);
    }
}

void SceneManagerComponent::handleEditorClose() {
    if(!sceneEditor.isVisible())
        return;
    
    sceneEditor.setVisible(false);
    resized();
}

void SceneManagerComponent::sceneMouseClicked(Scene& sc) {
    if(!sceneEditor.isVisible()) {
        sceneEditor.setVisible(true);
        resized();
    }
    
    sceneEditor.attach(&sc);
}

void SceneManagerComponent::sceneDeleting(SceneComponent& sceneComponent) {
    if(sceneEditor.isAttachedTo(&sceneComponent.getScene())) {
        buttonClicked(nullptr);
    }
}

void SceneManagerComponent::sceneDetached(SceneComponent& component, bool isDetached) {
    if(isDetached) {
        if(!scenesView.isVisible()) {
            sceneEditor.setVisible(true);
            sceneEditor.setCloseButtonEnabled(false);
        }
    } else {
        sceneEditor.setCloseButtonEnabled(true);
    }
        
    resized();
}

void SceneManagerComponent::getCommandInfo(CommandID commandID, ApplicationCommandInfo &result) {
    if(commandID == Commands::addScene) {
        result.setInfo("Add scene", "Add scene", "Scene", 0);
        result.addDefaultKeypress('A', ModifierKeys::noModifiers);
    }
}

bool SceneManagerComponent::perform(const InvocationInfo &info) {
    if(info.commandID == Commands::addScene) {
        sceneEditor.setCloseButtonEnabled(scenesView.makeVisible());
        scenesView.createScene(this);
        resized();
        return true;
    }
    
    return false;
}
