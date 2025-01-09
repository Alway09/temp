#pragma once

#include "NamedObject.h"

class StatefulObject : public NamedObject, public ValueTree::Listener
{
public:
    class ObjectState {
    public:
        ObjectState(ValueTree valueTree, Name name) : valueTree(valueTree), name(name) {}
        
        Name getName() {return name;}
        ValueTree getTree() { return valueTree; }
        
    private:
        ValueTree valueTree;
        Name name;
    };
    
    Array<ObjectState> getChildStates() {
        Array<ObjectState> arr;
        
        ValueTree::Iterator iter = valueTree.begin();
        while(iter != valueTree.end()) {
            Name name(this->name.get(), "some");
            ObjectState state(*iter, validateCustomName(name, identifierToString((*iter).getType())));
            arr.add(state);
            ++iter;
        }
        
        return arr;
    }
    
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
    
    StatefulObject(ObjectState state) : NamedObject(state.getName()), deleteStateWhenDestroyed(true)
    {
        valueTree = state.getTree();
        valueTree.addListener(this);
    }
    
    ~StatefulObject() {
        if(deleteStateWhenDestroyed) {
            ValueTree parentTree = valueTree.getParent();
            parentTree.removeChild(valueTree, nullptr);
        }
    }
    
    void rename(String newName) override {
        String nameToRollback = getName();
        NamedObject::rename(newName);
        
        Identifier newIdentifier;
        try {
            newIdentifier = stringToIdentifier(getName());
        } catch (const StateException& e) {
            NamedObject::rename(nameToRollback);
            throw e;
        }
        
        ValueTree parentTree = valueTree.getParent();
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
    
    bool hasChilds() {
        return valueTree.getNumChildren() != 0;
    }
    
    class StateException : public std::exception
    {
    public:
        StateException(String message) : message(message) {}
        
        String getMessage() const { return message; }
    private:
        const String message;
    };
    
    
    
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
            if(!XmlElement::isValidXmlName(possibleID)) {
                throw StateException("Name \"" + str + "\" is not valid!");
            }
        }
        return Identifier(possibleID);
    }
    
    static String identifierToString(Identifier identifier) {
        String possibleStr = identifier.toString().replace("_", " ").trim();
        return possibleStr;
    }
};
