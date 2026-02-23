#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
// Modern Rotary Slider with Custom Look
//==============================================================================
class ModernRotarySlider : public juce::Slider
{
public:
    ModernRotarySlider()
    {
        setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        setRotaryParameters(juce::MathConstants<float>::pi * 1.25f,
            juce::MathConstants<float>::pi * 2.75f,
            true);
    }

    void paint(juce::Graphics& g) override;

    void setLabel(const juce::String& newLabel) { label = newLabel; }
    juce::String getLabel() const { return label; }

private:
    juce::String label;
};

//==============================================================================
// Custom Toggle Button
//==============================================================================
class ModernToggleButton : public juce::ToggleButton
{
public:
    ModernToggleButton(const juce::String& text) : juce::ToggleButton(text) {}
    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown) override;
};

//==============================================================================
// Main Editor Class
//==============================================================================
class LusionSmartReverbAudioProcessorEditor
    : public juce::AudioProcessorEditor,
    private juce::Timer
{
public:
    LusionSmartReverbAudioProcessorEditor(LusionSmartReverbAudioProcessor&);
    ~LusionSmartReverbAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void drawBackground(juce::Graphics& g);
    void drawHeader(juce::Graphics& g);
    void drawMeters(juce::Graphics& g);
    void drawVisualization(juce::Graphics& g);

    LusionSmartReverbAudioProcessor& processor;

    // Controls
    ModernRotarySlider wetSlider, decaySlider, widthSlider;
    ModernToggleButton autoButton{ "AUTO MODE" };
    juce::ComboBox modeSelector;

    juce::Label wetLabel, decayLabel, widthLabel;
    juce::Label valueLabels[3];

    // Attachments
    using SA = juce::AudioProcessorValueTreeState::SliderAttachment;
    using BA = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using CA = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<SA> wetAttachment, decayAttachment, widthAttachment;
    std::unique_ptr<BA> autoAttachment;
    std::unique_ptr<CA> modeAttachment;

    // Visual state
    float rmsSmoothed = 0.0f;
    float duckSmoothed = 0.0f;
    float animationPhase = 0.0f;

    std::vector<float> waveformData;
    int waveformIndex = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LusionSmartReverbAudioProcessorEditor)
};