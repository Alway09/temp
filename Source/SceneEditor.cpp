#include "SceneEditor.h"

SceneEditor::SceneEditor()
{
    addAndMakeVisible(closeButton);
    addAndMakeVisible(headerLabel);
}

void SceneEditor::resized() {
    auto localBounds = getLocalBounds();
    
    auto headerBounds = localBounds.removeFromTop(getHeight() * 0.1f);
    
    closeButton.setBounds(headerBounds.removeFromLeft(50));
    headerLabel.setBounds(headerBounds);
    
    if(!objectEditors.isEmpty()) {
        for(auto editor : objectEditors) {
            editor->setBounds(localBounds);
        }
    }
}

void SceneEditor::attach(Scene* scene) {
    attachedTo = scene;
    headerLabel.setText(scene->getName(), NotificationType::dontSendNotification);
    
    objectEditors.clear();
    
    ValueTree sceneTree = scene->getValueTree();
    ValueTree::Iterator iter = sceneTree.begin();
    while(iter != sceneTree.end()) {
        int realisation = (*iter).getProperty(SceneObject::getTypeID());
        
        SceneObjectEditor* editor;
        if(realisation == SceneObjectRealisation::Waveform) {
            editor = new WaveformSceneObjectEditor(*iter);
            objectEditors.add(editor);
            addAndMakeVisible(editor);
        } else if(realisation == SceneObjectRealisation::Background) {
            
        } else {
            jassertfalse;
        }
        
        ++iter;
    }
    
    resized();
}
