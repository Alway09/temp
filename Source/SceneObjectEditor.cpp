#include "SceneObjectEditor.h"

SceneObjectEditor::SceneObjectEditor(SceneObject& objectEditTo) : object(objectEditTo) {
    header.setObjectName(objectEditTo.getName());
    header.addsceneObjectNameLabelListener(this);
    header.addExpandButtonListener(this);
    header.addDeleteButtonListener(this);
    addAndMakeVisible(header);
}
