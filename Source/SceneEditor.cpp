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
        
        for(int i = 0; i < scene->getObjects().size(); ++i) {
            SceneObject* object = scene->getObjects()[i];
            int realisation = object->getRealisation();
            
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
            addSection(object->getName(), editor->getControls());
        }
        
        refreshAll();
    }
    
}
