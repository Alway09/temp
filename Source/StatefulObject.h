#pragma once

#include "NamedObject.h"

class StatefulObject : public NamedObject, public ValueTree::Listener
{
public:
    StatefulObject(ValueTree treeAttachTo, String nameScope, String namePrefix) : NamedObject(nameScope, namePrefix)
    {
        const Identifier identifier{stringToIdentifier(getName())};
        valueTree = treeAttachTo.getOrCreateChildWithName(identifier, nullptr);
        valueTree.addListener(this);
    }
    
    ~StatefulObject() {
        ValueTree parentTree = valueTree.getParent();
        parentTree.removeChild(valueTree, nullptr);
    }
    
    void rename(String newName) override {
        NamedObject::rename(newName);
        
        ValueTree parentTree = valueTree.getParent();
        const Identifier newIdentifier{stringToIdentifier(getName())};
        ValueTree newValueTree{newIdentifier};
        newValueTree.copyPropertiesFrom(valueTree, nullptr);
        valueTree.removeListener(this);
        parentTree.removeChild(valueTree, nullptr);
        parentTree.addChild(newValueTree, -1, nullptr);
        valueTree = newValueTree;
        valueTree.addListener(this);
    }
    
    virtual void stateChanged(const Identifier &property) {}
    
    ValueTree& getValueTree() { return valueTree; }
    
private:
    ValueTree valueTree;
    
    void valueTreePropertyChanged(ValueTree &treeWhosePropertyHasChanged, const Identifier &property) override {
        stateChanged(property);
    }
    
    static Identifier stringToIdentifier(String str) {
        return Identifier(str.replace(" ", "_"));
    }
};
