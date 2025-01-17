#pragma once

#include <JuceHeader.h>
#include "SceneObject.h"

using namespace juce;

class SceneObjectEditor
{
public:
    SceneObjectEditor(SceneObject& objectEditTo) : object(objectEditTo){
        
    }
    ~SceneObjectEditor() { // PropertyComponents deletes with pannel
        controls.clear();
    }
    
    const Array<PropertyComponent*>& getControls() { return controls; };
protected:
    SceneObject& object;
    Array<PropertyComponent*> controls;
private:
    
};
