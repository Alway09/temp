#pragma once

#include <JuceHeader.h>
#include "SamplesHolder.h"

using namespace juce;

enum TimerEventType
{
    update,
    resizeLines
};

class WaveformComponent : public Component, public Timer
{
public:
    
    
    WaveformComponent(const SamplesHolder* samplesHolder);
    ~WaveformComponent();
    
    void timerCallback() override;
    void paint(Graphics& g) override;
    void resized() override;
    
    void update();
    void prepare(float sampleRate);
    
    void setSecondsToShow(float secs) {secondsToShow = secs;}
private:
    
    const SamplesHolder* samplesHolder;
    Array<Line<float>> lines;
    
    float sampleRate = 44100.f;
    
    float secondsToShow = 1.f;
    int samplesPerPixel = 0;
    int averageSamplesAmount = 0;
    
    int fps = 120;
    TimerEventType timerEventType = TimerEventType::update;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformComponent)
};
