#include "SceneEditor.h"

SceneEditor::SceneEditor() : editorsHolder(viewport)
{
    header.addSceneNameLabelListener(this);
    //editorsHolder.addObjectAppendListener(this);
    editorsHolder.setListener(this);
    viewport.setViewedComponent(&editorsHolder, false);
    addAndMakeVisible(header);
    addAndMakeVisible(viewport);
}

void SceneEditor::resized() {
    auto localBounds = getLocalBounds();
    header.setBounds(localBounds.removeFromTop(50));
    viewport.setBounds(localBounds);
}

void SceneEditor::attach(Scene* scene) {
    if(!isAttachedTo(scene)) {
        attachedTo = scene;
        editorsHolder.clear();
        header.setSceneName(scene->getName());
        
        for(int i = 0; i < scene->getObjects().size(); ++i) {
            SceneObject* object = scene->getObjects()[i];
            editorsHolder.addEditor(object);
        }
    }
    
}
