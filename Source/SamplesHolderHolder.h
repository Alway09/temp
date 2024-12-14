#pragma once

#include "SamplesHolder.h"

class SamplesHolderHolder // FIX NAMING!!!
{
public:
    inline static int samplesHolderChannelSize = 44100 * 60;
    
    static SamplesHolder* getInstance() {
        if(samplesHolder == nullptr) {
            samplesHolder = new SamplesHolder(samplesHolderChannelSize);
        }
        
        return samplesHolder;
    }
    
    static void destruct() {
        delete samplesHolder;
    }
    
private:
    inline static SamplesHolder* samplesHolder = nullptr;
};
