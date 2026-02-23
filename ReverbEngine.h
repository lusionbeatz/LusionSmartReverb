#pragma once
#include <JuceHeader.h>

class ReverbEngine
{
public:
    void prepare(double sampleRate, int samplesPerBlock);
    void reset();

    void setWet(float value);
    void setDecay(float seconds);
    void setWidth(float value);

    void process(juce::AudioBuffer<float>& buffer);

private:
    juce::Reverb reverb;
    juce::Reverb::Parameters params;
};
