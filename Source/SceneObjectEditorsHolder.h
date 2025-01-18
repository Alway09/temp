#pragma once

#include <JuceHeader.h>
#include "WaveformSceneObjectEditor.h"
#include "BackgroundSceneObjectEditor.h"

using namespace juce;

class SceneObjectEditorsHolder : public Component
{
public:
    SceneObjectEditorsHolder(){}
    ~SceneObjectEditorsHolder(){}
    
    void resized() override {
        int heightPadding = 0;
        for(auto editor : editors) {
            editor->setBounds(0, heightPadding, getWidth(), 50);
            heightPadding += 50;
        }
    }
    
    void addEditor(SceneObject* object) {
        SceneObjectRealisation realisation = object->getRealisation();
        
        SceneObjectEditor* editor;
        switch (realisation) {
            case SceneObjectRealisation::Waveform:
                editor = new WaveformSceneObjectEditor(*object);
                break;
            case SceneObjectRealisation::Background:
                editor = new BackgroundSceneObjectEditor(*object);
                break;
        }
        
        addAndMakeVisible(editor);
        editors.add(editor);
        resized();
    }
    
private:
    OwnedArray<SceneObjectEditor> editors;
};
