#pragma once
#include <JuceHeader.h>

using namespace juce;

class NamedObject {
public:
    NamedObject(const String& scope, const String& prefix);
    NamedObject(const String& uniqueName);
    virtual ~NamedObject();
    
    const String& getName() const { return name.get(); }
    
    virtual void rename(const String& newName);
    
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
    class Name {
    public:
        Name(const String& scope, const String& prefix) : scope(scope.trim()), prefix(prefix.trim()) { constructName(); }
        Name(const String& scope, const String& prefix, uint16 number) : Name(scope, prefix) { this->number = number; constructName(); }
        Name(const Name& other) : Name(other.scope, other.prefix, other.number) {}
        
        const String& get() const { return constructedName; }
        
        int64 getScope() const { return scope.hashCode64(); }
        String getScopeString() const { return scope; }
        int64 getPrefix() const { return prefix.hashCode64(); }
        uint16 getNumber() const { return number; }
        
        void setScope(const String& newScope) { scope = newScope; }
        void setPrefix(const String& newPrefix) { prefix = newPrefix; constructName(); }
        void setNumber(uint16 newNumber) { number = newNumber; constructName(); }
        
        Name& operator=(const Name& other) {
            this->scope = other.scope;
            this->prefix = other.prefix;
            this->number = other.number;
            constructName();
            return *this;
        }

    private:
        void constructName() { constructedName = number == 0 ? prefix : prefix + " " + String(number); }
        
        String scope;
        String prefix;
        uint16 number = 0;
        String constructedName;
    };
    
    NamedObject(Name name);
    
    static Name validateAndCreateCustomName(Name& objectName, const String& name);
    const Name& getNameInternal() const { return name; }
    
private:
    Name name;
    static void createName(NamedObject * const object);
    static void createUniqueName(NamedObject * const object);
    static void reserveName(const Name& name);
    void setCustomName(NamedObject * const object, const Name& name);
    static void deleteName(NamedObject * const object);
    
    static SortedSet<uint16>& getPrefix(const Name& name);
    static HashMap<int64, SortedSet<uint16>>* getScope(const Name& name);
    
    inline static HashMap<int64, HashMap<int64, SortedSet<uint16>>*> scope_prefix_number_pool;
};
