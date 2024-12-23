#pragma once
#include <JuceHeader.h>

using namespace juce;

class NamedObject {
public:
    NamedObject(String scope, String prefix) : scope(scope), prefix(prefix) {
        createName(this);
    }
    
    virtual ~NamedObject() {
        deleteName(this);
    }
    
    String getName() { return name; }
    
    virtual void rename(String newName) {
        validateCustomName(this, newName);
        deleteName(this);
        setCustomName(this, newName);
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
    
private:
    const String scope;
    const String prefix;
    int number;
    String name;
    bool isCustom = false;
    
    static void createName(NamedObject* object) {
        SortedSet<int>& prefix = getPrefix(object->scope, object->prefix);
        int counter = 0;
        while(prefix.contains(counter)) {
            ++counter;
        }
        
        prefix.add(counter);
        
        object->number = counter;
        object->name = object->prefix;
        if(counter != 0) {
            object->name += " " + String(counter);
        }
    }
    
    static void deleteName(NamedObject* object) {
        if(!object->isCustom) {
            if(scope_prefix_number_pool.contains(object->scope)) {
                HashMap<String, SortedSet<int>>* prefix = scope_prefix_number_pool.getReference(object->scope);
                
                if(prefix->contains(object->prefix)) {
                    SortedSet<int>& number = prefix->getReference(object->prefix);
                    
                    number.removeValue(object->number);
                    if(number.isEmpty()) {
                        prefix->remove(object->prefix);
                        if(prefix->size() == 0) {
                            scope_prefix_number_pool.remove(object->scope);
                            delete prefix;
                        }
                    }
                }
            }
        } else {
            if(scope_name_custom_pool.contains(object->scope)) {
                SortedSet<String>& scope = scope_name_custom_pool.getReference(object->scope);
                
                scope.removeValue(object->name);
                if(scope.isEmpty()) {
                    scope_name_custom_pool.remove(object->scope);
                }
            }
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
    
    static void setCustomName(NamedObject* object, String customName) {
        SortedSet<String>& scope = getScopeCustom(object->scope);
        object->name = customName;
        object->isCustom = true;
        scope.add(customName);
    }
    
    static void validateCustomName(NamedObject* object, String name) {
        SortedSet<String>& scope = getScopeCustom(object->scope);
        
        // TODO CHECK IN AUTOMATIC GENERATED SCOPE -> need to delete custom pool and make refactoring
        
        if(name == object->getName() || name.isEmpty()) {
            throw NameException();
        }
        
        if(scope.contains(name)) {
            throw NameException("Name \"" + name + "\" exsists in scope \"" + object->scope + "\"!");
        }
    }
    
    static SortedSet<String>& getScopeCustom(String scope) {
        if(!scope_name_custom_pool.contains(scope)) {
            scope_name_custom_pool.set(scope, SortedSet<String>());
        }
        return scope_name_custom_pool.getReference(scope);
    }
    
    inline static HashMap<String, HashMap<String, SortedSet<int>>*> scope_prefix_number_pool;
    inline static HashMap<String, SortedSet<String>> scope_name_custom_pool;
};
