#pragma once
#include "CircularArray.h"

class SamplesHolder
{
public:
    
    
    SamplesHolder(size_t channelSize) : channelSize(channelSize) {
        
    };
    
    ~SamplesHolder() {
        clear();
    };
    
    void prepare(size_t amountOfChannels) {
        if(amountOfChannels == 0) {
            clear();
        }
        
        if(samples.empty()) {
            for(int i = 0; i < amountOfChannels; ++i) {
                samples.push_back(createChannel());
            }
        } else if (samples.size() < amountOfChannels) {
            const size_t cursor = samples[0]->getCursor();
            size_t delta = amountOfChannels - samples.size();
            for(int i = 0; i < delta; ++i) {
                auto channel = createChannel(cursor);
                samples.push_back(channel);
            }
        } else if (samples.size() > amountOfChannels) {
            size_t delta = samples.size() - amountOfChannels;
            deleteBack(delta);
        }
    };
    
    void add(const float* buffer, unsigned long bufferSize, int channelNumber) {
        validateChannelNumber(channelNumber);
        samples[channelNumber]->put(buffer, bufferSize);
        
    }
    
    CircularArray::Iterator get(int channelNumber, int samplesAmount) const {
        validateChannelNumber(channelNumber);
        return samples[channelNumber]->getIterator(samplesAmount);
    }
    
    juce::String toString() {
        juce::String str = "";
        for(int i = 0; i < samples.size(); ++i) {
            str.append(samples[i]->toString() + "\n", std::numeric_limits<size_t>::max());
        }
        return str;
    }
    
    size_t getChannelsAmount() {
        return samples.size();
    }
    
private:
    void deleteBack(size_t amount) {
        for(int i = 0; i < amount; ++i) {
            delete samples.back();
            samples.pop_back();
        }
    }
    
    void clear() { deleteBack(samples.size()); }
    
    CircularArray* createChannel(size_t cursor = 0) { return new CircularArray(channelSize, cursor); }
    
    void validateChannelNumber(int channelNumber) const {
        if(channelNumber >= samples.size()) {
            throw std::invalid_argument("Channel number is not exist.");
        }
    }
    
    std::vector<CircularArray*> samples;
    size_t channelSize; // in samples
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SamplesHolder)
};
