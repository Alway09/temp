#pragma once

#include <JuceHeader.h>
#include "SceneObjectEditor.h"

using namespace juce;

class SceneEditorModel : public ListBoxModel
{
public:
    int getNumRows() override { return objectEditors.size(); }
    void paintListBoxItem (int rowNumber, Graphics &g, int width, int height, bool rowIsSelected) override {
        SceneObjectEditor* editor = objectEditors[rowNumber];
        editor->setBounds(0, 0, width, height);
        editor->setSelected(rowIsSelected);
        editor->paint(g);
        
        /*if (rowIsSelected)
            g.fillAll (Colours::lightblue);

        g.setColour (LookAndFeel::getDefaultLookAndFeel().findColour (Label::textColourId));
        g.setFont ((float) height * 0.7f);

        g.drawText ("Draggable Thing #" + String (rowNumber + 1),
                    5, 0, width, height,
                    Justification::centredLeft, true);*/
    }
    
    void addObjectEditor(SceneObjectEditor* objectEditor) {
        objectEditors.add(objectEditor);
    }
    
    void clear() { objectEditors.clear(); }
    
private:
    OwnedArray<SceneObjectEditor> objectEditors;
};
