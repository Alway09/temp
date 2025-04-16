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

NamedObject::NamedObject(const String& uniqueName) : name("G", uniqueName) {
    createUniqueName(this);
}

void NamedObject::createUniqueName(NamedObject * const object) {
    SortedSet<uint16>& prefix = getPrefix(object->name);
    if(prefix.size() > 0) {
        throw NameException("Name \"" + object->getName() + "\" already exsists!");
    } else {
        prefix.add(0);
    }
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

void NamedObject::rename(const String& newName) {
    if(newName == getName()) {
        return;
    }
    
    Name name = validateAndCreateCustomName(this->name, newName);
    deleteName(this);
    setCustomName(this, name);
    for(NamedObject::Listener* l : listeners) l->objectRenamed(getName());
}

NamedObject::Name NamedObject::validateAndCreateCustomName(Name& objectName, const String& name) {
    HashMap<int64, SortedSet<uint16>>* scopeMap = getScope(objectName);
    // try to split on word(s) and number in the end (autogen like)
    String efficientNumberStr = name.fromLastOccurrenceOf(" ", false, true);
    int efficientNumber = efficientNumberStr.getIntValue();
    if(efficientNumber < 0 || efficientNumber > std::numeric_limits<uint16>::max()) {
        throw NameException("Please, in names use numbers in range (0," + String(std::numeric_limits<uint16>::max()) + ").");
    } else if(efficientNumber == 0 || name.startsWith(String(efficientNumber))) { // means without number after word
        // (efficientNumberStr is not a number or 0) or (whole name is one word starts with number (atoi("123SSS") -> 123))
            
        if(scopeMap->contains(name.hash()) && scopeMap->getReference(name.hash()).contains(0)) {
            throw NameException("Name \"" + name + "\" already exsists!");
        }
            
        return Name(objectName.getScopeString(), name, 0);
    }
            
    String efficientPrefix = name.upToLastOccurrenceOf(" ", false, false);
    if(efficientPrefix.startsWith("0") && efficientPrefix.endsWith(String(efficientNumber))) {
        efficientPrefix = String(efficientNumber);
        efficientNumber = 0;
    }
    
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
