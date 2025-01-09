#pragma once
#include <JuceHeader.h>

using namespace juce;

class NamedObject {
public:
    NamedObject(String scope, String prefix) : name(scope, prefix) {
        createName(this);
    }
    
    virtual ~NamedObject() {
        deleteName(this);
    }
    
    String getName() { return name.get(); }
    
    virtual void rename(String newName) {
        Name name = validateCustomName(this->name, newName);
        deleteName(this);
        setCustomName(this, name);
    }
    
    class NameException : public std::exception
    {
    public:
        NameException() {}
        NameException(String message) : message(message) {}
        
        String getMessage() const { return message; }
        bool hasMessage() const { return !message.isEmpty(); }
    private:
        String message;
    };
  
protected:
    struct Name {
        Name(String scope, String prefix) : scope(scope.trim()), prefix(prefix.trim()) {}
        Name(String scope, String prefix, int number) : Name(scope, prefix) { this->number = number; }
        
        String scope;
        String prefix;
        int number;
        
        Name& operator=(const Name& other) {
            this->scope = other.scope;
            this->prefix = other.prefix;
            this->number = other.number;
            return *this;
        }
        
        String get() {
            if(number == 0) {
                return prefix;
            } else {
                return prefix + " " + String(number);
            }
        }
    } name;
    
    NamedObject(Name name) : name(name) {
        // create name
        reserveName(name);
    }
    
    //Name getNameInternal() { return name; }
    
    static Name validateCustomName(Name& objectName, String name) {
        HashMap<String, SortedSet<int>>* scopeMap = getScope(objectName.scope);
        
        if(scopeMap->contains(name)) {
            throw NameException("Name \"" + name + "\" exsists in scope \"" + objectName.scope + "\"!");
        } else {
            // try to split on word(s) and number in the end (autogen like)
            String efficientNumberStr = name.fromLastOccurrenceOf(" ", false, false);
            int efficientNumber = efficientNumberStr.getIntValue();
            if(efficientNumber == 0) { // efficientNumberStr is not a number or 0
                return Name(objectName.scope, name, 0);
            } else {
                String efficientPrefix = name.upToLastOccurrenceOf(" ", false, false);
                if(scopeMap->contains(efficientPrefix)) {
                    SortedSet<int>& prefix = scopeMap->getReference(efficientPrefix);
                    if(prefix.contains(efficientNumber)) {
                        throw NameException("Name \"" + name + "\" exsists in scope \"" + objectName.scope + "\"!");
                    } else {
                        return Name(objectName.scope, efficientPrefix, efficientNumber);
                    }
                } else {
                    return Name(objectName.scope, efficientPrefix, efficientNumber);
                }
            }
        }
    }
private:
    static void createName(NamedObject* object) {
        SortedSet<int>& prefix = getPrefix(object->name.scope, object->name.prefix);
        int counter = 0;
        while(prefix.contains(counter)) {
            ++counter;
        }
        
        prefix.add(counter);
        
        object->name.number = counter;
    }
    
    static void reserveName(Name name) {
        SortedSet<int>& prefix = getPrefix(name.scope, name.prefix);
        prefix.add(name.number);
    }
    
    static void deleteName(NamedObject* object) {
        if(scope_prefix_number_pool.contains(object->name.scope)) {
            HashMap<String, SortedSet<int>>* prefix = scope_prefix_number_pool.getReference(object->name.scope);
            
            if(prefix->contains(object->name.prefix)) {
                SortedSet<int>& number = prefix->getReference(object->name.prefix);
                
                number.removeValue(object->name.number);
                if(number.isEmpty()) {
                    prefix->remove(object->name.prefix);
                    if(prefix->size() == 0) {
                        scope_prefix_number_pool.remove(object->name.scope);
                        delete prefix;
                    }
                }
            }
        }
        
        String nameScope = object->getName();
        if(scope_prefix_number_pool.contains(nameScope)) {
            HashMap<String, SortedSet<int>>* prefix = scope_prefix_number_pool.getReference(nameScope);
            scope_prefix_number_pool.remove(nameScope);
            delete prefix;
        }
    }
    
    static SortedSet<int>& getPrefix(String scope, String prefix) {
        HashMap<String, SortedSet<int>>* scopeMap = getScope(scope);
        if(!scopeMap->contains(prefix)) {
            scopeMap->set(prefix, SortedSet<int>());
        }
        return scopeMap->getReference(prefix);
    }
    
    static HashMap<String, SortedSet<int>>* getScope(String scope) {
        if(!scope_prefix_number_pool.contains(scope)) {
            HashMap<String, SortedSet<int>>* map = new HashMap<String, SortedSet<int>>();
            scope_prefix_number_pool.set(scope, map);
        }
        return scope_prefix_number_pool.getReference(scope);
    }
    
    
    
    void setCustomName(NamedObject* object, Name name) {
        SortedSet<int>& prefix = getPrefix(name.scope, name.prefix);
        prefix.add(name.number);
        object->name = name;
    }
    
    inline static HashMap<String, HashMap<String, SortedSet<int>>*> scope_prefix_number_pool;
};
