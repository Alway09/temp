#pragma once

#include "NamedObject.h"

class StatefulObject : public NamedObject, public ValueTree::Listener
{
public:
    StatefulObject(String nameScope, String namePrefix) : NamedObject(nameScope, namePrefix), deleteStateWhenDestroyed(true) {
        const Identifier identifier{stringToIdentifier(getName())};
        valueTree = ValueTree(identifier);
        valueTree.addListener(this);
    }
    
    StatefulObject(StatefulObject& parent, String nameScope, String namePrefix, bool deleteStateWhenDestroyed = true) : NamedObject(nameScope, namePrefix), deleteStateWhenDestroyed(deleteStateWhenDestroyed)
    {
        const Identifier identifier{stringToIdentifier(getName())};
        valueTree = parent.valueTree.getOrCreateChildWithName(identifier, nullptr);
        valueTree.addListener(this);
    }
    
    ~StatefulObject() {
        if(deleteStateWhenDestroyed) {
            ValueTree parentTree = valueTree.getParent();
            parentTree.removeChild(valueTree, nullptr);
        }
    }
    
    void rename(String newName) override {
        NamedObject::rename(newName);
        
        ValueTree parentTree = valueTree.getParent();
        const Identifier newIdentifier{stringToIdentifier(getName())};
        ValueTree newValueTree{newIdentifier};
        newValueTree.copyPropertiesAndChildrenFrom(valueTree, nullptr);
        valueTree.removeListener(this);
        parentTree.removeChild(valueTree, nullptr);
        parentTree.addChild(newValueTree, -1, nullptr);
        valueTree = newValueTree;
        valueTree.addListener(this);
    }
    
    void saveState(String filename) {
        File file(filename); // need premissions
        std::unique_ptr<XmlElement> xml = valueTree.createXml();
        xml->writeTo(file);
    }
    
    void restoreState(String filename) {
        File file(filename); // need premissions
        if(file.existsAsFile()) {
            std::unique_ptr<XmlElement> xml = XmlDocument::parse(file);
            ValueTree restoreTree = ValueTree::fromXml(*xml.get());
            
            if(restoreTree.getType() == valueTree.getType()) {
                valueTree = restoreTree;
            } else {
                jassertfalse;
            }
            
            //GlobalOptionsComponent::restoreSettings(getValueTree(), deviceManager);
        }
    }
    
    virtual void stateChanged(const Identifier &property) {}
    
    void setProperty(const Identifier &name, const var &newValue) {
        valueTree.setProperty(name, newValue, nullptr);
    }
    
    const var& getProperty(const Identifier &name) const {
        return valueTree.getProperty(name);
    }
    
    Value getPropertyAsValue(const Identifier &name) {
        return valueTree.getPropertyAsValue(name, nullptr);
    }
    
private:
    ValueTree valueTree;
    const bool deleteStateWhenDestroyed;
    
    void valueTreePropertyChanged(ValueTree &treeWhosePropertyHasChanged, const Identifier &property) override {
        stateChanged(property);
    }
    
    static Identifier stringToIdentifier(String str) {
        String possibleID = str.replace(" ", "_");
        if(!XmlElement::isValidXmlName(possibleID)) {
            possibleID = "_" + possibleID;
        }
        return Identifier(possibleID);
    }
};
