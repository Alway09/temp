#include "CustomAudioBuffer.h"

CustomAudioBuffer::CustomAudioBuffer(int numChannels, int numSamples) : AudioBuffer<float>(numChannels, numSamples) {}

void CustomAudioBuffer::append(const AudioBuffer& source) {
    if(source.getNumChannels() > getNumChannels()) {
        throw std::invalid_argument("Channels");
    }
    
    if(source.getNumSamples() > getNumSamples()) {
        throw std::invalid_argument("Num samples");
    }
    
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
    return new ReadBuffer(getArrayOfReadPointers(), startIdx, numSamples, getNumSamples());
}

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
// ---------
void CustomAudioBuffer::init(int numChannels/*, int numSamples*/) {
    if(instance == nullptr) {
        instance = new CustomAudioBuffer(numChannels, 60 * 44100);
    } else {
        instance->setSize(numChannels, 60 * 44100, true, true, true);
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
CustomAudioBuffer::ReadBuffer::ReadBuffer(const float * const * samples, int rangeStartIdx, int numSamples, int allNumSamples) : samples(samples), numSamples(numSamples), allNumSamples(allNumSamples), cursor(rangeStartIdx) {}

bool CustomAudioBuffer::ReadBuffer::getNext(int channel, float& valuePlaceTo) {
    if(counter == numSamples) return false;
    if(cursor == allNumSamples) cursor = 0;

    valuePlaceTo = samples[channel][cursor];
    ++cursor; ++counter;
    return true;
}
