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
    
    StatefulObject(StatefulObject& parent, const ObjectState& state, bool deleteStateWhenDestroyed = true);
    
    virtual ~StatefulObject();
    
    Array<ObjectState> getChildrenStates() const;
    ObjectState getChildState(const Identifier& identifier) const;
    void saveState(const String& filename) const;
    void saveState(File& file) const;
    void restoreState(const String& filename);
    void restoreState(File& file);
    virtual void stateChanged(const Identifier &property) {}
    void rename(const String& newName) override;
    void move(int newIdx);
    
    void setProperty(const Identifier &name, const var &newValue);
    const var& getProperty(const Identifier &name) const;
    void removeProperty(const Identifier &name);
    bool hasProperty(const Identifier& name) const;
    void setPropertyIfNotExists(const Identifier &name, const var &newValue);
    const var& getProperty(const Identifier &name, var defaultValue);
    Value getPropertyAsValue(const Identifier &name);
    bool hasChildren() const;
    bool hasChild(const Identifier& identifier) const;
    
    class StateException : public std::exception
    {
    public:
        StateException(const String& message) : message(message) {}
        const String& getMessage() const { return message; }
    private:
        const String message;
    };
    
    struct Sucker {
        Sucker(StatefulObject* obj) : obj(obj) {}
        void setProperty(const Identifier &name, const var &newValue) {obj->setProperty(name, newValue);}
        const var& getProperty(const Identifier &name) const {return obj->getProperty(name);}
        bool hasProperty(const Identifier& name) const {return obj->getProperty(name);}
        void removeProperty(const Identifier &name) {obj->removeProperty(name);}
        void setSource(StatefulObject* obj) { this->obj = obj; }
    private:
        StatefulObject* obj;
    };
    
private:
    void addChild(StatefulObject* child) { children.add(child); }
    void deleteChild(StatefulObject* child) { children.removeValue(child); }
    
    void parentRenamed(ValueTree& currentParentTree) {
        valueTree = currentParentTree.getOrCreateChildWithName(valueTree.getType(), nullptr);
        
        for(int i = 0; i < children.size(); ++i) {
            children[i]->parentRenamed(valueTree);
        }
    }
    
    void valueTreePropertyChanged(ValueTree &treeWhosePropertyHasChanged, const Identifier &property) override {
        stateChanged(property);
    }
    
    Identifier createID();
    static Name createName(const String& scope, const Identifier& identifier);
    
    void initTree();
    void initTreeFromParent(StatefulObject& parent);

    ValueTree valueTree;
    const bool deleteStateWhenDestroyed;
    
    SortedSet<StatefulObject*> children;
    StatefulObject* parent = nullptr;
    
    inline static const String terminator{"_--_"};
};
