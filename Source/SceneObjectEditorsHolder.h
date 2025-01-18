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
            auto bounds = editor->getBoundingRectangle(heightPadding);
            editor->setBounds(bounds);
            
            heightPadding += bounds.getHeight();
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
    
    void clear() {
        editors.clear();
    }
    
private:
    OwnedArray<SceneObjectEditor> editors;
};
