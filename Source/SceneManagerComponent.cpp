#include "SceneManagerComponent.h"

SceneManagerComponent::SceneManagerComponent(StatefulObject& parent) : StatefulObject(parent, "Scene Manager"), sidePannel(*this), scenesRender(*this)
{
    addAndMakeVisible(sidePannel);
    //addChildComponent(scenesPanel);
    addAndMakeVisible(scenesPanelViewport);
    //scenesPanelViewport.setViewedComponent(&scenesPanel, false);
    //scenesPanel.setListener(this);
    
    addChildComponent(sceneEditor);
    
    scenesPanelViewport.getHorizontalScrollBar().addListener(this);
    //scenesPanel.setYOffset(16); // ??
    
    //setBounds(0, 0, 10, 10); // set bound to avoid context init crash
    /*scenesView.setBounds(0, 0, 10, 10); // set bound to avoid context init crash
    if(scenesView.hasChildren()) {
        addAndMakeVisible(scenesView);
    } else {
        addChildComponent(scenesView);
    }*/
    
    if(hasProperty("isEditorVisible")) {
        handleEditorVisibility(getProperty("isEditorVisible"));
        sidePannel.setExpandEditorState(getProperty("isEditorVisible"));
    }
    
    startTimer(100);
}

void SceneManagerComponent::timerCallback() {
    if(isVisible()) { // context initialized
        scenesPanel.reset(new ScenesMiniPanel(*this, scenesRender, this));
        choosenScene = scenesPanel->getChoosenComponent();
        if(choosenScene != nullptr) {
            addAndMakeVisible(choosenScene);
            choosenScene->setOverlayVisibility(true);
            sceneEditor.attach(&choosenScene->getScene());
        }
        scenesPanelViewport.setViewedComponent(scenesPanel.get(), false);
        scenesPanel->setListener(this);
        scenesPanel->setScenesDrawingYOffset(16);
        scenesPanel->resized();
        if(hasProperty("isPanelVisible")) {
            handleScenesPanelVisibility(getProperty("isPanelVisible"));
            sidePannel.setExpandMiniPanelState(getProperty("isPanelVisible"));
        }
        stopTimer();
    }
}

void SceneManagerComponent::resized()
{
    auto localBounds = getLocalBounds();
    sceneEditor.setBounds(localBounds.removeFromRight(sceneEditor.getCurrentWidth()));
    sidePannel.setBounds(localBounds.removeFromRight(30));
    scenesRender.setScissorsBox(localBounds.withX(0).withY(0));
    if(scenesPanel.get() != nullptr && scenesPanel->isShowing()) {
        scenesPanelViewport.setBounds(localBounds.removeFromBottom(scenesPanel->getPanelHeight() + scenesPanelViewport.getHorizontalScrollBar().getHeight()));
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
    setProperty("isEditorVisible", mustBeVisible);
    sceneEditor.setVisible(mustBeVisible);
    resized();
}

void SceneManagerComponent::handleScenesPanelVisibility(bool mustBeVisible) {
    setProperty("isPanelVisible", mustBeVisible);
    scenesPanel->showOrHide(mustBeVisible);
    scenesPanelViewport.setVisible(mustBeVisible);
    resized();
}

void SceneManagerComponent::sceneMouseClicked(SceneComponent& sc) {
    /*if(!sceneEditor.isVisible()) {
        sceneEditor.setVisible(true);
        resized();
    }*/
    if(&sc != choosenScene) {
        if(sc.isDetached()) {
            sceneEditor.attach(&sc.getScene());
        } else {
            returnSceneOnPanel(choosenScene);
            choosenScene = &sc;
            choosenScene->setOverlayVisibility(true);
            addAndMakeVisible(sc);
            sceneEditor.attach(&sc.getScene());
            resized();
        }
    } else {
        sceneEditor.attach(&sc.getScene());
    }
}

void SceneManagerComponent::returnSceneOnPanel(SceneComponent* sc) {
    if(sc == nullptr) return;
    
    if(sc->isDetached()) {
        sc->detachScene(false, false);
        scenesRender.addScene(&sc->getScene());
        sc->getScene().replaceContext(scenesRender.getContext(), false);
        scenesPanel->returnOnPannel(sc);
    } else {
        scenesPanel->returnOnPannel(sc);
        choosenScene = nullptr;
        sceneEditor.detach();
    }
    sc->setOverlayVisibility(false);
}

void SceneManagerComponent::scrollBarMoved(ScrollBar *scrollBarThatHasMoved, double newRangeStart) {
    //DBG(newRangeStart);
    scenesPanel->setScenesDrawingXOffset(-newRangeStart);
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
    if(isDetached) {
        scenesRender.removeScene(&component.getScene());
        choosenScene = nullptr;
    } else {
        scenesRender.addScene(&component.getScene());
        component.getScene().replaceContext(scenesRender.getContext(), false);
        if(choosenScene == nullptr) {
            choosenScene = &component;
            addAndMakeVisible(component);
            resized();
        }
    }
}
