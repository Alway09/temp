#include "SceneManagerComponent.h"

SceneManagerComponent::SceneManagerComponent(StatefulObject& parent) : StatefulObject(parent, IDs::name), sidePannel(*this), scenesRender(*this)
{
    addAndMakeVisible(sidePannel);
    addChildComponent(sceneEditor);
    addAndMakeVisible(scenesPanelViewport);
    scenesPanelViewport.getHorizontalScrollBar().addListener(this);
    
    handleEditorVisibility(getProperty(IDs::isEditorVisible));
    sidePannel.setExpandEditorState(getProperty(IDs::isEditorVisible));
    
    startTimer(100);
}

void SceneManagerComponent::timerCallback() {
    if(isVisible()) { // context initialized
        scenesPanel.reset(new ScenesMiniPanel(*this, scenesRender, this));
        choosenScene = scenesPanel->getChoosenComponent();
        if(choosenScene != nullptr) {
            addAndMakeVisible(choosenScene);
            choosenScene->showOrHideOvelray(true);
            sceneEditor.attach(&choosenScene->getScene());
        }
        scenesPanelViewport.setViewedComponent(scenesPanel.get(), false);
        scenesPanel->setListener(this);
        scenesPanel->setScenesDrawingYOffset(16); // ?????
        scenesPanel->resized();
        handleScenesPanelVisibility(getProperty(IDs::isPanelVisible));
        sidePannel.setExpandMiniPanelState(getProperty(IDs::isPanelVisible));
        stopTimer();
    }
}

void SceneManagerComponent::resized()
{
    auto localBounds = getLocalBounds();
    sceneEditor.setBounds(localBounds.removeFromRight(sceneEditor.getCurrentWidth()));
    sidePannel.setBounds(localBounds.removeFromRight(sidePannelWidth));
    scenesRender.setScissorsBox(localBounds.withX(0).withY(0));
    if(scenesPanel.get() != nullptr && scenesPanel->isShowing()) {
        scenesPanelViewport.setBounds(localBounds.removeFromBottom(scenesPanel->getPanelHeight() + scenesPanelViewport.getHorizontalScrollBar().getHeight()));
    }
    if(choosenScene != nullptr) choosenScene->setBounds(localBounds);
}

void SceneManagerComponent::handleEditorVisibility(bool mustBeVisible) {
    setProperty(IDs::isEditorVisible, mustBeVisible);
    sceneEditor.setVisible(mustBeVisible);
    resized();
}

void SceneManagerComponent::handleScenesPanelVisibility(bool mustBeVisible) {
    setProperty(IDs::isPanelVisible, mustBeVisible);
    scenesPanel->showOrHide(mustBeVisible);
    scenesPanelViewport.setVisible(mustBeVisible);
    resized();
}

void SceneManagerComponent::sceneMouseClicked(SceneComponent& sc) {
    chooseScene(&sc);
}

void SceneManagerComponent::chooseScene(SceneComponent* sc) {
    if(sc == choosenScene || sc->isDetached()) {
        sceneEditor.attach(&sc->getScene());
    } else {
        returnSceneOnPanel(choosenScene);
        choosenScene = sc;
        choosenScene->showOrHideOvelray(true);
        addAndMakeVisible(sc);
        sceneEditor.attach(&sc->getScene());
        scenesPanel->setChoosen(sc);
        resized();
    }
}

void SceneManagerComponent::returnSceneOnPanel(SceneComponent* sc) {
    if(sc == nullptr) return;
    
    if(sc->isDetached()) {
        sc->detachScene(false);
        sc->getScene().replaceContext(scenesRender.getContext(), false);
        scenesRender.addScene(&sc->getScene());
        scenesPanel->returnOnPannel(sc);
    } else {
        scenesPanel->returnOnPannel(sc);
        choosenScene = nullptr;
        sceneEditor.detach();
    }
    sc->showOrHideOvelray(false);
}

void SceneManagerComponent::scrollBarMoved(ScrollBar *scrollBarThatHasMoved, double newRangeStart) {
    scenesPanel->setScenesDrawingXOffset(-newRangeStart);
}

void SceneManagerComponent::sceneDeleting(SceneComponent& sceneComponent) {
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
        component.getScene().replaceContext(scenesRender.getContext(), false);
        scenesRender.addScene(&component.getScene());
        if(choosenScene == nullptr) {
            choosenScene = &component;
            addAndMakeVisible(component);
            resized();
        }
    }
}

SceneManagerComponent::MySidePanel::MySidePanel(SceneManagerComponent& parent) : parent(parent) {
    expandEditorButton.setToggleable(true);
    expandEditorButton.setClickingTogglesState(true);
    expandEditorButton.onClick = [this](){ this->parent.handleEditorVisibility(expandEditorButton.getToggleState());
        setExpandEditorState(expandEditorButton.getToggleState());
    };
    
    expandMiniPannelButton.setToggleable(true);
    expandMiniPannelButton.setClickingTogglesState(true);
    expandMiniPannelButton.onClick = [this](){ this->parent.handleScenesPanelVisibility(expandMiniPannelButton.getToggleState());
        setExpandMiniPanelState(expandMiniPannelButton.getToggleState());
    };
    
    addSceneButton.setButtonText("+");
    addSceneButton.onClick = [this]() {
        SceneComponent* sc = this->parent.scenesPanel->createScene();
        this->parent.chooseScene(sc);
    };
    addChildComponent(addSceneButton);

    addAndMakeVisible(expandEditorButton);
    addAndMakeVisible(expandMiniPannelButton);
}

void SceneManagerComponent::MySidePanel::resized() {
    auto bounds = getLocalBounds();
    expandEditorButton.setBounds(bounds.removeFromTop(30).withY(20));
    int h = (parent.scenesPanel.get() != nullptr && parent.scenesPanel->isShowing()) ? parent.scenesPanel->getPanelHeight() : 0;
    Rectangle<int> r(0, getHeight() - 30 - h, getWidth(), 30);
    expandMiniPannelButton.setBounds(r);
    addSceneButton.setBounds(bounds.removeFromBottom(h));
}

void SceneManagerComponent::MySidePanel::setExpandMiniPanelState(bool shouldBeToggled) {
    expandMiniPannelButton.setToggleState(shouldBeToggled, NotificationType::dontSendNotification);
    expandMiniPannelButton.setButtonText(shouldBeToggled ? "\\/" : "/\\");
    addSceneButton.setVisible(expandMiniPannelButton.getToggleState());
    resized();
}

void SceneManagerComponent::MySidePanel::setExpandEditorState(bool shouldBeToggled) {
    expandEditorButton.setToggleState(shouldBeToggled, NotificationType::dontSendNotification);
    expandEditorButton.setButtonText(shouldBeToggled ? ">" : "<");
}
