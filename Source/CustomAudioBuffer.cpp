#include "CustomAudioBuffer.h"

CustomAudioBuffer::CustomAudioBuffer(int numChannels,int secondsToHold, float sampleRate) : AudioBuffer<float>(numChannels, secondsToHold * sampleRate), secondsToHold(secondsToHold), sampleRate(sampleRate) {}

void CustomAudioBuffer::append(const AudioBuffer& source) {
    if(source.getNumChannels() > getNumChannels()) {
        throw std::invalid_argument("Channels");
    }
    
    if(source.getNumSamples() > getNumSamples()) {
        throw std::invalid_argument("Num samples");
    }
    
    const ScopedLock l{appendLock};
    
    int sum = writeIdx + source.getNumSamples();
    if(sum > getNumSamples()) {
        int deltaAfter = sum - getNumSamples();
        int deltaBefore = source.getNumSamples() - deltaAfter;
        for(int channel = 0; channel < source.getNumChannels(); ++ channel) {
            copyFrom(channel, writeIdx, source, channel, 0, deltaBefore);
            copyFrom(channel, 0, source, channel, deltaBefore, deltaAfter);
        }
        writeIdx = deltaBefore + 1;
    } else {
        for(int channel = 0; channel < source.getNumChannels(); ++ channel) {
            copyFrom(channel, writeIdx, source, channel, 0, source.getNumSamples());
        }
        writeIdx += source.getNumSamples();
    }
}

CustomAudioBuffer::ReadBuffer::Ptr CustomAudioBuffer::get(int numSamples) const {
    int delta = readIdx - numSamples;
    int startIdx = delta < 0 ? getNumSamples() + delta : delta;
    return new ReadBuffer(getArrayOfReadPointers(), startIdx, numSamples, getNumSamples(), readLock);
}

/*CustomAudioBuffer::ReadBuffer::Ptr CustomAudioBuffer::get(float seconds) const {
    int numSamples = seconds * sampleRate;
    return get(numSamples);
}*/

String CustomAudioBuffer::toString() const {
    String str = "";
    for(int ch = 0; ch < getNumChannels(); ++ch) {
        for(int s = 0; s < getNumSamples(); ++s) {
            str += String(getSample(ch, s));
        }
        str += "\n";
    }
    return str;
}

void CustomAudioBuffer::resize(int numChannels, int secondsToHold, float sampleRate) {
    const ScopedLock l{appendLock};
    readLock.enterWrite();
    
    int newNumSamples = secondsToHold * sampleRate;
    
    if(newNumSamples < getNumSamples()) {
        writeIdx = newNumSamples - 1;
    }
    setSize(numChannels, newNumSamples, false, true, true);
    
    this->sampleRate = sampleRate;
    this->secondsToHold = secondsToHold;
    
    readLock.exitWrite();
}
// ---------
void CustomAudioBuffer::init(int numChannels, int secondsToHold, float sampleRate) {
    if(instance == nullptr) {
        instance = new CustomAudioBuffer(numChannels, secondsToHold, sampleRate);
    } else {
        instance->resize(numChannels, secondsToHold, sampleRate);
    }
}

void CustomAudioBuffer::destruct() {
    if(instance != nullptr) {
        delete instance;
    }
}

CustomAudioBuffer * const CustomAudioBuffer::getInst() {
    if(instance != nullptr) {
        return instance;
    }
    
    throw std::bad_exception();
}
//======================================
CustomAudioBuffer::ReadBuffer::ReadBuffer(const float * const * samples, int rangeStartIdx, int numSamples, int allNumSamples, const ReadWriteLock& lock) : samples(samples), numSamples(numSamples), allNumSamples(allNumSamples), cursor(rangeStartIdx), lock(lock) 
{
    if(lock.tryEnterRead()) {
        locked = true;
    } else {
        counter = numSamples;
    }
}

bool CustomAudioBuffer::ReadBuffer::getNext(int channel, float& valuePlaceTo) {
    if(counter == numSamples) {
        if(locked) lock.exitRead();
        return false;
    }
        
    if(cursor == allNumSamples) cursor = 0;

    valuePlaceTo = samples[channel][cursor];
    ++cursor; ++counter;
    return true;
}
