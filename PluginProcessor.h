#pragma once
#include <JuceHeader.h>
#include "ReverbEngine.h"

class LusionSmartReverbAudioProcessor : public juce::AudioProcessor
{
public:
    LusionSmartReverbAudioProcessor();
    ~LusionSmartReverbAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "LusionSmartReverb"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 5.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;

    // ===== Preset System =====
    void loadPresetFromXml (const juce::String& xmlText);
    juce::String getStateAsXmlString();

    // Read-only meters
    float getDuckAmount() const { return duckAmount; }
    float getRmsLevel()  const { return rmsLevel; }

    juce::AudioProcessorValueTreeState apvts;

private:
    ReverbEngine reverb;

    float rmsLevel  = 0.0f;
    float peakLevel = 0.0f;

    juce::SmoothedValue<float> duckEnv { 0.0f };
    float duckAmount = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LusionSmartReverbAudioProcessor)
};
