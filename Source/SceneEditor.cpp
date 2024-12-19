#include "SceneEditor.h"

SceneEditor::SceneEditor()
{
    setHeaderComponent(std::make_unique<Header>(this));
    setModel(&model);
    setRowHeight(40);
    //addAndMakeVisible(closeButton);
    //addAndMakeVisible(headerLabel);
}

void SceneEditor::resized() {
    ListBox::resized();
    //auto localBounds = getLocalBounds();
    
    //auto headerBounds = localBounds.removeFromTop(getHeight() * 0.1f);
    
    /*closeButton.setBounds(headerBounds.removeFromLeft(50));
    headerLabel.setBounds(headerBounds);*/
    
    /*if(!objectEditors.isEmpty()) {
        for(auto editor : objectEditors) {
            editor->setBounds(localBounds);
        }
    }*/
}

void SceneEditor::attach(Scene* scene) {
    if(!isAttachedTo(scene)) {
        attachedTo = scene;
        //headerLabel.setText(scene->getName(), NotificationType::dontSendNotification);
        
        //objectEditors.clear();
        model.clear();
        getHeader()->setText(scene->getName());
        
        ValueTree sceneTree = scene->getValueTree();
        ValueTree::Iterator iter = sceneTree.begin();
        while(iter != sceneTree.end()) {
            int realisation = (*iter).getProperty(SceneObject::getTypeID());
            
            SceneObjectEditor* editor;
            if(realisation == SceneObjectRealisation::Waveform) {
                editor = new WaveformSceneObjectEditor(*iter);
                //objectEditors.add(editor);
                //addAndMakeVisible(editor);
                model.addObjectEditor(editor);
            } else if(realisation == SceneObjectRealisation::Background) {
                
            } else {
                jassertfalse;
            }
            
            ++iter;
        }
        
        //resized();
        updateContent();
    }
    
}
