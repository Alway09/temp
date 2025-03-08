#include "SceneEditor.h"

SceneEditor::SceneEditor() : editorsHolder(viewport)
{
    header.addSceneNameLabelListener(this);
    editorsHolder.setListener(this);
    viewport.setViewedComponent(&editorsHolder, false);
    
    addAndMakeVisible(header);
    addAndMakeVisible(viewport);
}

void SceneEditor::resized() {
    auto localBounds = getLocalBounds();
    header.setBounds(localBounds.removeFromTop(50));
    viewport.setBounds(localBounds);
    editorsHolder.resized();
}

void SceneEditor::labelTextChanged(Label* sceneNameLabel) {
    String newName = sceneNameLabel->getText();
    
    try {
        attachedTo->rename(newName);
    } catch (const NamedObject::NameException& e) {
        if(e.hasMessage()) {
            AlertWindow::showAsync(MessageBoxOptions::makeOptionsOk(MessageBoxIconType::WarningIcon, "Warning", e.getMessage()), nullptr);
        }
    } catch (const StatefulObject::StateException& e) {
        AlertWindow::showAsync(MessageBoxOptions::makeOptionsOk(MessageBoxIconType::WarningIcon, "Warning", e.getMessage()), nullptr);
    }
    
    header.setSceneName(attachedTo->getName());
    editorsHolder.reinitControls();
}

void SceneEditor::attach(Scene* scene) {
    if(!isAttachedTo(scene)) {
        attachedTo = scene;
        editorsHolder.clear();
        header.setSceneName(scene->getName());
        
        for(SceneObject* obj : scene->getObjects()) {
            editorsHolder.addEditor(obj, false);
        }
        editorsHolder.resized();
    }
}
//--------------------------------------
SceneEditor::Header::Header() {
    sceneNameLabel.setEditable(false, true, true);
    
    setSceneName("<not attached>");
    addAndMakeVisible(sceneNameLabel);
    addAndMakeVisible(closeButton);
}

void SceneEditor::Header::resized() {
    auto localBounds = getLocalBounds();
    
    closeButton.setBounds(localBounds.removeFromLeft(50));
    sceneNameLabel.setBounds(localBounds);
}
