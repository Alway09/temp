#pragma once

#include <JuceHeader.h>

using namespace juce;

class SceneObjectEditor : public Component
{
public:
    SceneObjectEditor(ValueTree treeEditTo) : valueTree(treeEditTo){}
protected:
    ValueTree valueTree;
private:
};
