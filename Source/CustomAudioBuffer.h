#pragma once

#include <JuceHeader.h>

using namespace juce;

class CustomAudioBuffer : public AudioBuffer<float>
{
public:
    CustomAudioBuffer(int numChannels, int numSamples);
    
    void append(const AudioBuffer& source);
    void setSecondsToHold(int secondsToHold) { this->secondsToHold = secondsToHold; }
    int getSecondsToHold() const { return secondsToHold; }
    float getSampleRate() const { return sampleRate; }
    void setReadPoint() { readIdx = writeIdx; }
    String toString() const;
    
    class ReadBuffer : public ReferenceCountedObject { // SingleThreadedReferenceCountedObject
    public:
        using Ptr = ReferenceCountedObjectPtr<ReadBuffer>;
        
        ReadBuffer(const float * const * samples, int rangeStartIdx, int numSamples, int allNumSamples);
        bool getNext(int channel, float& valuePlaceTo);
    private:
        const float * const * samples;
        int numSamples = 0;
        int allNumSamples = 0;
        int cursor = 0;
        int counter = 0;
    };
    
    ReadBuffer::Ptr get(int numSamples) const;
    
    static void init(int numChannels/*, int numSamples*/);
    static void destruct();
    static CustomAudioBuffer * const getInst();
private:
    inline static CustomAudioBuffer* instance = nullptr;
    
    int writeIdx = 0;
    int readIdx = 0;
    
    int secondsToHold = 60;
    float sampleRate = 44100.f;
};
