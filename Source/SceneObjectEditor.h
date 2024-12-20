#pragma once

#include <JuceHeader.h>
#include "NameGenerator.h"

using namespace juce;

class SceneObjectEditor
{
public:
    SceneObjectEditor(ValueTree treeEditTo) : valueTree(treeEditTo){}
    ~SceneObjectEditor() { // PropertyComponents deletes with pannel
        controls.clear();
    }
    
    const Array<PropertyComponent*>& getControls() { return controls; };
    //String getName() const { return NameGenerator::fromIdentifier(valueTree.getType()); }
protected:
    ValueTree valueTree;
    Array<PropertyComponent*> controls;
private:
    
};
