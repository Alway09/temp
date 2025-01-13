#include "NamedObject.h"

NamedObject::NamedObject(const String& scope, const String& prefix) : name(scope, prefix) {
    createName(this);
}

void NamedObject::createName(NamedObject * const object) {
    SortedSet<uint16>& prefix = getPrefix(object->name);
    
    uint16 counter = 0;
    while(prefix.contains(counter)) {
        ++counter;
    }
    
    prefix.add(counter);
    object->name.setNumber(counter);
}

NamedObject::~NamedObject() {
    deleteName(this);
}

void NamedObject::deleteName(NamedObject * const object) {
    // check name on existance, if exists - delete it
    int64 scopeHash = object->name.getScope();
    if(scope_prefix_number_pool.contains(scopeHash)) {
        // gettting prefix not with getPrefix() for not creating prefix if it not exists
        HashMap<int64, SortedSet<uint16>>* prefix = scope_prefix_number_pool.getReference(scopeHash);
        
        int64 prefixHash = object->name.getPrefix();
        if(prefix->contains(prefixHash)) {
            SortedSet<uint16>& number = prefix->getReference(prefixHash);
            
            number.removeValue(object->name.getNumber());
            // if not numbers exists - names with this prefix not exists - delete prefix
            if(number.isEmpty()) {
                prefix->remove(prefixHash);
                // if prefixes in this scope not exists - names with this scope not exists - delete scope
                if(prefix->size() == 0) {
                    scope_prefix_number_pool.remove(scopeHash);
                    delete prefix;
                }
            }
        }
    }
    
    // check if this name was a scope for some other objects, delete if it is
    int64 nameHash = object->getName().hashCode64();
    if(scope_prefix_number_pool.contains(nameHash)) {
        HashMap<int64, SortedSet<uint16>>* prefix = scope_prefix_number_pool.getReference(nameHash);
        scope_prefix_number_pool.remove(nameHash);
        delete prefix;
    }
}

NamedObject::NamedObject(Name name) : name(name) {
    reserveName(name);
}

void NamedObject::reserveName(const Name& name) {
    SortedSet<uint16>& prefix = getPrefix(name);
    prefix.add(name.getNumber());
}

void NamedObject::rename(String newName) {
    Name name = validateAndCreateCustomName(this->name, newName);
    deleteName(this);
    setCustomName(this, name);
}

NamedObject::Name NamedObject::validateAndCreateCustomName(Name& objectName, String name) {
    HashMap<int64, SortedSet<uint16>>* scopeMap = getScope(objectName);
    
    if(scopeMap->contains(name.hash())) {
        throw NameException("Name \"" + name + "\" already exsists!");
    } else {
        // try to split on word(s) and number in the end (autogen like)
        String efficientNumberStr = name.fromLastOccurrenceOf(" ", false, false);
        int efficientNumber = efficientNumberStr.getIntValue();
        if(efficientNumber == 0) { // efficientNumberStr is not a number or 0
            return Name(objectName.getScopeString(), name, 0);
        } else {
            String efficientPrefix = name.upToLastOccurrenceOf(" ", false, false);
            if(scopeMap->contains(efficientPrefix.hash())) {
                SortedSet<uint16>& prefix = scopeMap->getReference(efficientPrefix.hash());
                if(prefix.contains(efficientNumber)) {
                    throw NameException("Name \"" + name + "\" already exsists!");
                } else {
                    return Name(objectName.getScopeString(), efficientPrefix, efficientNumber);
                }
            } else {
                return Name(objectName.getScopeString(), efficientPrefix, efficientNumber);
            }
        }
    }
}

void NamedObject::setCustomName(NamedObject * const object, const Name& name) {
    reserveName(name);
    object->name = name;
}

SortedSet<uint16>& NamedObject::getPrefix(const Name& name) {
    HashMap<int64, SortedSet<uint16>>* scopeMap = getScope(name);
    int64 prefix = name.getPrefix();
    if(!scopeMap->contains(prefix)) {
        scopeMap->set(prefix, SortedSet<uint16>());
    }
    return scopeMap->getReference(prefix);
}

HashMap<int64, SortedSet<uint16>>* NamedObject::getScope(const Name& name) {
    int64 scope = name.getScope();
    if(!scope_prefix_number_pool.contains(name.getScope())) {
        HashMap<int64, SortedSet<uint16>>* map = new HashMap<int64, SortedSet<uint16>>();
        scope_prefix_number_pool.set(scope, map);
    }
    return scope_prefix_number_pool.getReference(scope);
}
