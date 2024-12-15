#pragma once

#include <JuceHeader.h>

using namespace juce;

class NameGenerator
{
public:
    static String createName(String prefix) {
        String name;
        if(prefixWithTheirNumbers.contains(prefix)) {
            int counter = 0;
            while(true) {
                if(prefixWithTheirNumbers.getReference(prefix).contains(counter)) {
                    ++counter;
                } else {
                    prefixWithTheirNumbers.getReference(prefix).add(counter);
                    break;
                }
            }
            name = prefix + " " + String(counter);
        } else {
            SortedSet<int> set;
            set.add(0);
            prefixWithTheirNumbers.set(prefix, set);
            name = prefix;
        }
        
        return name;
    }
    
    static Identifier createIdentifier(String prefix) {
        return convertToIdentifier(createName(prefix));
    }
    
    static Identifier convertToIdentifier(String name) {
        return Identifier{name.trim().replace(" ", "_")};
    }
    
    static String fromIdentifier(Identifier identifier) {
        return identifier.toString().replace("_", " ");
    }
    
    static void freeName(String name, String prefix) {
        int number = name.trimCharactersAtStart(prefix).trimStart().getIntValue();
        prefixWithTheirNumbers.getReference(prefix).removeValue(number);
    }
    
private:
    inline static HashMap<String, SortedSet<int>> prefixWithTheirNumbers;
};
