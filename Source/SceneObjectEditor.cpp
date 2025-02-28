#include "SceneObjectEditor.h"

SceneObjectEditor::SceneObjectEditor(SceneObject& objectEditTo) : object(objectEditTo), header(*this) {
    header.setObjectName(objectEditTo.getName());
    header.addSceneObjectNameLabelListener(this);
    header.addExpandButtonListener(this);
    header.addDeleteButtonListener(this);
    addAndMakeVisible(header);
}

void SceneObjectEditor::resized() {
    auto bounds = getBoundingRectangle(0);
    
    header.setBounds(bounds.removeFromTop(header.getHeight()));
    for(auto c : controls) {
        c->setBounds(bounds.removeFromTop(controlHeight));
    }
}

void SceneObjectEditor::mouseDrag(const MouseEvent& e) {
    Component* parent = getParentComponent();
    parent->mouseDrag(e.getEventRelativeTo(parent));
}

void SceneObjectEditor::labelTextChanged(Label* l) {
    String newName = l->getText();
    
    try {
        object.rename(newName);
    } catch (const NamedObject::NameException& e) {
        if(e.hasMessage()) {
            AlertWindow::showAsync(MessageBoxOptions::makeOptionsOk(MessageBoxIconType::WarningIcon, "Warning", e.getMessage()), nullptr);
        }
    } catch (const StatefulObject::StateException& e) {
        AlertWindow::showAsync(MessageBoxOptions::makeOptionsOk(MessageBoxIconType::WarningIcon, "Warning", e.getMessage()), nullptr);
    }
    
    header.setObjectName(object.getName());
    initControls();
}

void SceneObjectEditor::buttonClicked(Button* b) {
    if(header.isExpandButton(b)) {
        expanded = !expanded;
        
        header.changeState(expanded);
        
        if(!controls.isEmpty()) {
            for(auto c : controls) {
                c->setVisible(expanded);
            }
            getParentComponent()->resized();
        }
    } else if(header.isDeleteButton(b)) {
        listener->deleteButtonClicked(this, &object);
    }
}
//------------------------------------
SceneObjectEditor::Header::Header(SceneObjectEditor& parent) : parent(parent) {
    sceneObjectNameLabel.setEditable(false, true, true);
    
    expandButton.shouldUseOnColours(false);
    expandButton.setBorderSize(BorderSize<int>(3));
    
    nonExpandedPath.startNewSubPath(-1, 1);
    nonExpandedPath.lineTo(1, 0);
    nonExpandedPath.lineTo(-1, -1);
    setExpandButtonPath(nonExpandedPath);
    
    expandedPath.startNewSubPath(-1, -1);
    expandedPath.lineTo(0, 1);
    expandedPath.lineTo(1, -1);
    
    visibilityButton.setToggleable(true);
    visibilityButton.setClickingTogglesState(true);
    visibilityButton.setToggleState(true, NotificationType::dontSendNotification);
    visibilityButton.onClick = [this](){
        visibilityButton.setButtonText(visibilityButton.getToggleState() ? "O" : "-");
        this->parent.setRenderState(visibilityButton.getToggleState());
    };
    
    addAndMakeVisible(sceneObjectNameLabel);
    addAndMakeVisible(expandButton);
    addAndMakeVisible(deleteButton);
    addAndMakeVisible(visibilityButton);
}

void SceneObjectEditor::Header::mouseDrag(const MouseEvent& e) {
    Component* parent = getParentComponent();
    parent->mouseDrag(e.getEventRelativeTo(parent));
}

void SceneObjectEditor::Header::resized() {
    auto localBounds = getLocalBounds().withHeight(height);
    
    expandButton.setBounds(localBounds.removeFromLeft(expandButtonWidth));
    deleteButton.setBounds(localBounds.removeFromRight(deleteButtonWidth));
    visibilityButton.setBounds(localBounds.removeFromRight(visibilityButtonWidth));
    sceneObjectNameLabel.setBounds(localBounds);
}
