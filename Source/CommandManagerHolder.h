#pragma once

#include <JuceHeader.h>

using namespace juce;

class CommandManagerHolder
{
public:
    static ApplicationCommandManager* getInstance()
    {
        if(commandManager == nullptr) {
            commandManager = new ApplicationCommandManager();
        }
        
        return commandManager;
    }
    
    static void destruct() {
        if(commandManager != nullptr) {
            delete commandManager;
        }
    }
private:
    inline static ApplicationCommandManager* commandManager = nullptr;
};
