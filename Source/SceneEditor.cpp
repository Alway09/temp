#include "SceneEditor.h"

SceneEditor::SceneEditor()
{
    header.addSceneNameLabelListener(this);
    addAndMakeVisible(header);
    addAndMakeVisible(editorsHolder);
}

void SceneEditor::resized() {
    //PropertyPanel::resized();
    
    auto localBounds = getLocalBounds();
    
    header.setBounds(localBounds.removeFromTop(50));
    editorsHolder.setBounds(localBounds);
    //getViewport().setBounds(localBounds);
}

void SceneEditor::attach(Scene* scene) {
    if(!isAttachedTo(scene)) {
        attachedTo = scene;
        editorsHolder.clear();
        header.setSceneName(scene->getName());
        
        for(int i = 0; i < scene->getObjects().size(); ++i) {
            SceneObject* object = scene->getObjects()[i];
            editorsHolder.addEditor(object);
            /*SceneObjectRealisation realisation = object->getRealisation();
            
            SceneObjectEditor* editor;
            switch (realisation) {
                case SceneObjectRealisation::Waveform:
                    editor = new WaveformSceneObjectEditor(*object);
                    break;
                case SceneObjectRealisation::Background:
                    editor = new BackgroundSceneObjectEditor(*object);
                    break;
            }
            
            objectEditors.add(editor);
            addSection(object->getName(), editor->getControls());*/
        }
        
        //refreshAll();
    }
    
}
