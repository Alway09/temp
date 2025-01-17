#pragma once

#include "NamedObject.h"

class StatefulObject : public NamedObject, public ValueTree::Listener
{
public:
    StatefulObject(const String& nameScope, const String& namePrefix);
    StatefulObject(const String& uniqueName);
    
    StatefulObject(StatefulObject& parent, const String& nameScope, const String& namePrefix, bool deleteStateWhenDestroyed = true);
    StatefulObject(StatefulObject& parent, const String& uniqueName, bool deleteStateWhenDestroyed = true);
    
    class ObjectState {
    public:
        ObjectState(ValueTree valueTree, const Name& name) : valueTree(valueTree), name(name) {}
        const Name& getName() const {return name;}
        const ValueTree& getTree() const { return valueTree; }
    private:
        const ValueTree valueTree;
        const Name name;
    };
    
    StatefulObject(const ObjectState& state);
    
    virtual ~StatefulObject();
    
    Array<ObjectState> getChildrenStates();
    void saveState(const String& filename);
    void saveState(File& file);
    void restoreState(const String& filename);
    void restoreState(File& file);
    virtual void stateChanged(const Identifier &property) {}
    void rename(const String& newName) override;
    
    void setProperty(const Identifier &name, const var &newValue);
    const var& getProperty(const Identifier &name) const;
    void setPropertyIfNotExists(const Identifier &name, const var &newValue);
    const var& getProperty(const Identifier &name, var defaultValue);
    Value getPropertyAsValue(const Identifier &name);
    bool hasChildren();
    
    class StateException : public std::exception
    {
    public:
        StateException(const String& message) : message(message) {}
        const String& getMessage() const { return message; }
    private:
        const String message;
    };
    
private:
    void valueTreePropertyChanged(ValueTree &treeWhosePropertyHasChanged, const Identifier &property) override {
        stateChanged(property);
    }
    
    Identifier createID();
    static Name createName(const String& scope, const Identifier& identifier);
    
    void initTree();
    void initTreeFromParent(StatefulObject& parent);

    ValueTree valueTree;
    const bool deleteStateWhenDestroyed;
};
