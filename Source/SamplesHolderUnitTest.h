/*
  ==============================================================================

    SamplesHolderUnitTest.h
    Created: 5 Nov 2024 1:05:55pm
    Author:  Валентин Чекулаев

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "SamplesHolder.h"
#include "CustomAudioBuffer.h"

using namespace juce;

class SamplesHolderUnitTest : public UnitTest
{
public:
    SamplesHolderUnitTest(const String &name, const String &category=String()) : UnitTest(name, category) {};
    
    void runTest() override {
        CustomAudioBuffer buffer(2, 10);
        logMessage(buffer.toString());
        //===============================
        AudioBuffer<float> bufToAppend{2, 8};
        for(int ch = 0; ch < bufToAppend.getNumChannels(); ++ch) {
            for(int s = 0; s < bufToAppend.getNumSamples(); ++s) {
                bufToAppend.setSample(ch, s, s);
            }
        }
        for(int i = 1 ; i <= 2; ++i) {
            logMessage(String(i) + "------");
            buffer.append(bufToAppend);
            logMessage(buffer.toString());
        }
        logMessage("-------");
        //=============================
        buffer.setReadPoint();
        auto readbuf = buffer.get(7);
        float val = -1;
        while(readbuf->getNext(0, val)) {
            logMessage(String(val));
        }
    }
    
private:
    
    void test_old() {
        bool sectionPassed = false;
        //===================================================
        beginTest("Prepare");
        SamplesHolder holder(10);
        int channelsAmount = 2;
        
        holder.prepare(channelsAmount);
        logMessage(holder.toString());
        //===================================================
        beginTest("Add to empty");
        int bufferSize = 7;
        float buffer[bufferSize];
        for(int i = 1; i < bufferSize + 1; ++i) { // 1..bufferSize
            buffer[i-1] = i;
        }
        
        holder.add(buffer, bufferSize, 0);
        holder.add(buffer, bufferSize, 1);
        
        for(int channel = 0; channel < channelsAmount; ++channel) {          //expect 1..7 for each channel
            auto iter = holder.get(channel, bufferSize);
            for(int i = 0; i < bufferSize; ++i) {
                sectionPassed = buffer[i] == iter.getNext();
            }
        }
        
        logMessage(holder.toString());

        expect(sectionPassed, "Add to empty: failure");
        //===================================================
        beginTest("Add to not empty");
        sectionPassed = false;
        
        holder.add(buffer, bufferSize, 0);
        holder.add(buffer, bufferSize, 1);
        
        for(int channel = 0; channel < channelsAmount; ++channel) {      //expect 5,6,7 from prev iteration and 1..7 next
            auto iter = holder.get(channel, 10);
            sectionPassed = iter.getNext() == 5;
            sectionPassed = iter.getNext() == 6;
            sectionPassed = iter.getNext() == 7;
            for(int i = 0; i < bufferSize; ++i) {
                sectionPassed = buffer[i] == iter.getNext();
            }
        }
        
        logMessage(holder.toString());
        
        expect(sectionPassed, "Add to not empty: failure");
        //===================================================
        beginTest("Get tests");
        
        for(int channel = 0; channel < channelsAmount; ++channel) {
            auto iter = holder.get(channel, 1);
            expect(iter.getNext() == 7);
            expect(!iter.hasNext());
            
            iter = holder.get(channel, 3);
            expect(iter.getNext() == 5);
            expect(iter.getNext() == 6);
            expect(iter.getNext() == 7);
            expect(!iter.hasNext());
            
            iter = holder.get(channel, 10);
            expect(iter.getNext() == 5);
            expect(iter.getNext() == 6);
            expect(iter.getNext() == 7);
            expect(iter.getNext() == 1);
            expect(iter.getNext() == 2);
            expect(iter.getNext() == 3);
            expect(iter.getNext() == 4);
            expect(iter.getNext() == 5);
            expect(iter.getNext() == 6);
            expect(iter.getNext() == 7);
            expect(!iter.hasNext());
        }
        
        //===================================================
        beginTest("Enlarge channels and samples");
        sectionPassed = false;
        
        holder.prepare(3); // plus 1 channel
        logMessage(holder.toString());
        
        holder.prepare(5); // plus 2 channel
        logMessage(holder.toString());
        
        holder.prepare(9); // plus 4 channel
        logMessage(holder.toString());
        
        holder.prepare(5); // minus 4 channel
        logMessage(holder.toString());
        
        holder.prepare(3); // minus 2 channel
        logMessage(holder.toString());
        
        holder.prepare(2); // minus 1 channel
        logMessage(holder.toString());
    }
};

static SamplesHolderUnitTest samplesHolderUnitTest("Samples Holder Unit Test", "Samples Holder");
