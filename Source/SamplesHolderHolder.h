#pragma once

#include "SamplesHolder.h"

class SamplesHolderHolder // FIX NAMING!!!
{
public:
    inline static const int secondsToHold = 60;
    inline static const int sampleRate = 44100;
    inline static const int samplesHolderChannelSize = 44100 * 60;
    
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
