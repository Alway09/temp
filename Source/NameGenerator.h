#pragma once

#include <JuceHeader.h>

using namespace juce;

class NameGenerator
{
public:
    static String createName(String scope, String prefix) {
        SortedSet<int>& numSet = getPrefix(scope, prefix);
        
        int counter = 0;
        while(true) {
            if(numSet.contains(counter)) {
                ++counter;
            } else {
                numSet.add(counter);
                break;
            }
        }
        return prefix + " " + String(counter);
    }
    
    static Identifier createIdentifier(String scope, String prefix) {
        return convertToIdentifier(createName(scope, prefix));
    }
    
    static Identifier convertToIdentifier(String name) {
        return Identifier{name.trim().replace(" ", "_")};
    }
    
    static String fromIdentifier(Identifier identifier) {
        return identifier.toString().replace("_", " ");
    }
    
    static void freeName(String scope, String prefix, String name) {
        int number = name.trimCharactersAtStart(prefix).trimStart().getIntValue();
        SortedSet<int>& numSet = getPrefix(scope, prefix);
        numSet.removeValue(number);
    }
    
    static void freeScope(String scope) {
        if(scope_prefix_number.contains(scope)) {
            delete scope_prefix_number[scope];
            scope_prefix_number.remove(scope);
        }
    }
    
    static void destruct() {
        for(auto hm : scope_prefix_number) {
            delete hm;
        }
    }
    
private:
    static SortedSet<int>& getPrefix(String scope, String prefix) {
        HashMap<String, SortedSet<int>>& scopeMap = getScope(scope);
        if(!scopeMap.contains(prefix)) {
            scopeMap.set(prefix, SortedSet<int>());
        }
        return scopeMap.getReference(prefix);
    }
    
    static HashMap<String, SortedSet<int>>& getScope(String scope) {
        if(!scope_prefix_number.contains(scope)) {
            HashMap<String, SortedSet<int>>* map = new HashMap<String, SortedSet<int>>();
            scope_prefix_number.set(scope, map);
        }
        return *scope_prefix_number.getReference(scope);
    }
    
    inline static HashMap<String, HashMap<String, SortedSet<int>>*> scope_prefix_number;
};
