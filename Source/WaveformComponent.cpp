#include "WaveformComponent.h"

WaveformComponent::WaveformComponent(const SamplesHolder* samplesHolder) : samplesHolder(samplesHolder)
{
    startTimerHz(fps);
    //setSynchroniseToVBlank(true);
}

WaveformComponent::~WaveformComponent() {
    stopTimer();
    samplesHolder = nullptr;
}


void WaveformComponent::update() {
    auto iter = samplesHolder->get(0, secondsToShow * sampleRate);
    
    int samplesCounter = 0;
    int blockCounter = 0;
    float blockMin = std::numeric_limits<float>::max();
    float blockMax = -1 * std::numeric_limits<float>::max();
    while (iter.hasNext()) {
        float sample = iter.getNext() * 400;
        blockMin = jmin(blockMin, sample);
        blockMax = jmax(blockMax, sample);
        if(samplesCounter % samplesPerPixel == 0) {
            if(blockCounter < lines.size()) {
                auto l = lines.getRawDataPointer() + blockCounter;
                if(blockMax - blockMin < 1.f) {
                    blockMin -= 1.f;
                    blockMax += 1.f;
                }
                l->setStart(blockCounter, blockMax + getHeight() / 2);
                l->setEnd(blockCounter, blockMin + getHeight() / 2);
            }
            
            blockMin = std::numeric_limits<float>::max();
            blockMax = -1 * std::numeric_limits<float>::max();
            ++blockCounter;
        }
        ++samplesCounter;
    }
}

void  WaveformComponent::paint(Graphics &g) {
    g.fillAll(Colours::black);
    g.setColour(Colours::white);
    g.drawText(String(lines.size()), 0, 0, 100, 20, Justification::centred);
    for(int i = 0; i < lines.size(); ++i) {
        g.drawLine(lines[i], 1);
    }
}

void WaveformComponent::resized() {
    samplesPerPixel = averageSamplesAmount / getWidth();
    timerEventType = TimerEventType::resizeLines;
    stopTimer();
    startTimer(100);
}

void WaveformComponent::timerCallback() {
    switch (timerEventType) {
        case TimerEventType::update:
            update();
            repaint();
            break;
        case TimerEventType::resizeLines:
            lines.resize(getWidth());
            timerEventType = TimerEventType::update;
            startTimerHz(fps);
            break;
        default:
            break;
    }
}

void WaveformComponent::prepare(float sampleRate) {
    this->sampleRate = sampleRate;
    averageSamplesAmount = secondsToShow * sampleRate;
}
