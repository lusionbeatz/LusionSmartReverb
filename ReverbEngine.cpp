#include "ReverbEngine.h"

void ReverbEngine::prepare(double sampleRate, int)
{
    reverb.setSampleRate(sampleRate);

    params.roomSize = 0.5f;
    params.damping = 0.5f;
    params.wetLevel = 0.3f;
    params.dryLevel = 0.7f;
    params.width = 1.0f;
    params.freezeMode = 0.0f;

    reverb.setParameters(params);
}

void ReverbEngine::reset()
{
    reverb.reset();
}

void ReverbEngine::setWet(float value)
{
    params.wetLevel = juce::jlimit(0.0f, 1.0f, value);
    params.dryLevel = 1.0f - params.wetLevel;
    reverb.setParameters(params);
}

void ReverbEngine::setDecay(float seconds)
{
    // Map 0.1 – 6.0 seconds → JUCE roomSize (0–1)
    params.roomSize = juce::jlimit(0.05f, 1.0f, seconds / 6.0f);
    reverb.setParameters(params);
}

void ReverbEngine::setWidth(float value)
{
    params.width = juce::jlimit(0.0f, 1.0f, value);
    reverb.setParameters(params);
}

void ReverbEngine::process(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    if (numChannels == 1)
    {
        reverb.processMono(buffer.getWritePointer(0), numSamples);
    }
    else
    {
        reverb.processStereo(
            buffer.getWritePointer(0),
            buffer.getWritePointer(1),
            numSamples
        );
    }
}
