#include "SceneEditor.h"

SceneEditor::SceneEditor()
{
    addAndMakeVisible(header);
}

void SceneEditor::resized() {
    PropertyPanel::resized();
    
    auto localBounds = getLocalBounds();
    
    header.setBounds(localBounds.removeFromTop(50));
    getViewport().setBounds(localBounds);
}

void SceneEditor::attach(Scene* scene) {
    if(!isAttachedTo(scene)) {
        attachedTo = scene;
        clear();
        header.setText(scene->getName());
        
        ValueTree sceneTree = scene->getValueTree();
        ValueTree::Iterator iter = sceneTree.begin();
        while(iter != sceneTree.end()) {
            int realisation = (*iter).getProperty(SceneObject::getTypeID());
            
            SceneObjectEditor* editor;
            if(realisation == SceneObjectRealisation::Waveform) {
                editor = new WaveformSceneObjectEditor(*iter);
                objectEditors.add(editor);
                addSection(editor->getName(), editor->getControls());
                //model.addObjectEditor(editor);
            } else if(realisation == SceneObjectRealisation::Background) {
                
            } else {
                jassertfalse;
            }
            
            ++iter;
        }
        
        //updateContent();
        refreshAll();
    }
    
}
