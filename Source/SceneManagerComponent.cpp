#include "SceneManagerComponent.h"

SceneManagerComponent::SceneManagerComponent(StatefulObject& parent) : /*scenesView(parent, this)*/ sidePannel(*this), scenesRender(*this), scenesPanel(parent, scenesRender)
{
    CommandManagerHolder::getInstance()->registerAllCommandsForTarget(this);
    addKeyListener(CommandManagerHolder::getInstance()->getKeyMappings());
    
    setWantsKeyboardFocus(true);
    
    addAndMakeVisible(sidePannel);
    //addChildComponent(scenesPanel);
    addAndMakeVisible(scenesPanelViewport);
    scenesPanelViewport.setViewedComponent(&scenesPanel, false);
    scenesPanel.setListener(this);
    
    addChildComponent(sceneEditor);
    
    scenesPanelViewport.getHorizontalScrollBar().addListener(this);
    scenesPanel.setYOffset(16); // ??
    
    //setBounds(0, 0, 10, 10); // set bound to avoid context init crash
    /*scenesView.setBounds(0, 0, 10, 10); // set bound to avoid context init crash
    if(scenesView.hasChildren()) {
        addAndMakeVisible(scenesView);
    } else {
        addChildComponent(scenesView);
    }*/
    
    //startTimer(100);
}

void SceneManagerComponent::resized()
{
    auto localBounds = getLocalBounds();
    sceneEditor.setBounds(localBounds.removeFromRight(sceneEditor.getCurrentWidth()));
    sidePannel.setBounds(localBounds.removeFromRight(30));
    scenesRender.setScissorsBox(localBounds.withX(0).withY(0));
    if(scenesPanel.isShowing()) {
        scenesPanelViewport.setBounds(localBounds.removeFromBottom(scenesPanel.getNormalHeight() + scenesPanelViewport.getHorizontalScrollBar().getHeight()));
    }
    if(choosenScene != nullptr) choosenScene->setBounds(localBounds);
    /*if(scenesView.isVisible()) {
        scenesBound = localBounds.withWidth((localBounds.getWidth() - sceneEditor.getCurrentWidth()));
        sceneEditor.setBounds(localBounds.removeFromRight(sceneEditor.getCurrentWidth()));
        scenesView.setBounds(scenesBound);
    } else {
        sceneEditor.setBounds(localBounds);
    }*/
}

void SceneManagerComponent::handleEditorVisibility(bool mustBeVisible) {
    sceneEditor.setVisible(mustBeVisible);
    resized();
}

void SceneManagerComponent::handleScenesPanelVisibility(bool mustBeVisible) {
    scenesPanel.showOrHide(mustBeVisible);
    scenesPanelViewport.setVisible(mustBeVisible);
    resized();
}

void SceneManagerComponent::sceneMouseClicked(SceneComponent& sc) {
    /*if(!sceneEditor.isVisible()) {
        sceneEditor.setVisible(true);
        resized();
    }*/
    if(&sc != choosenScene) {
        returnSceneOnPanel();
        choosenScene = &sc;
        addAndMakeVisible(sc);
        sceneEditor.attach(&sc.getScene());
        resized();
    }
}

void SceneManagerComponent::returnSceneOnPanel() {
    if(choosenScene != nullptr) {
        scenesPanel.returnOnPannel(choosenScene);
        choosenScene = nullptr;
        sceneEditor.detach();
    }
}

void SceneManagerComponent::scrollBarMoved(ScrollBar *scrollBarThatHasMoved, double newRangeStart) {
    //DBG(newRangeStart);
    scenesPanel.setXOffset(-newRangeStart);
}

void SceneManagerComponent::sceneDeleting(SceneComponent& sceneComponent) {
    /*if(sceneEditor.isAttachedTo(&sceneComponent.getScene())) {
        buttonClicked(nullptr);
    }*/
    if(choosenScene == &sceneComponent) {
        sceneEditor.detach();
        choosenScene = nullptr;
    }
    scenesRender.removeScene(&sceneComponent.getScene());
}

void SceneManagerComponent::sceneDetached(SceneComponent& component, bool isDetached) {
    /*if(isDetached) {
        if(!scenesView.isVisible()) {
            sceneEditor.setVisible(true);
            sceneEditor.setCloseButtonEnabled(false);
        }
    } else {
        sceneEditor.setCloseButtonEnabled(true);
    }*/
        
    resized();
}

void SceneManagerComponent::getCommandInfo(CommandID commandID, ApplicationCommandInfo &result) {
    if(commandID == Commands::addScene) {
        result.setInfo("Add scene", "Add scene", "Scene", 0);
        result.addDefaultKeypress('A', ModifierKeys::noModifiers);
    }
}

bool SceneManagerComponent::perform(const InvocationInfo &info) {
    /*if(info.commandID == Commands::addScene) {
        sceneEditor.setCloseButtonEnabled(scenesView.makeVisible());
        scenesView.createScene(this);
        resized();
        return true;
    }*/
    
    return false;
}
