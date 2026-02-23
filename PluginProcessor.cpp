#include "PluginProcessor.h"
#include "PluginEditor.h"

//============================================================
static juce::AudioProcessorValueTreeState::ParameterLayout createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "WET", "Wet", 0.0f, 1.0f, 0.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "DECAY", "Decay", 0.1f, 6.0f, 1.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "WIDTH", "Width", 0.0f, 1.0f, 1.0f));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "AUTO", "Auto", false));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "MODE", "Mode", juce::StringArray{ "Short", "Long", "Tail" }, 1));

    return { params.begin(), params.end() };
}

//============================================================
LusionSmartReverbAudioProcessor::LusionSmartReverbAudioProcessor()
    : AudioProcessor(
        BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    apvts(*this, nullptr, "PARAMETERS", createParameters())
{
}

LusionSmartReverbAudioProcessor::~LusionSmartReverbAudioProcessor() {}

//============================================================
void LusionSmartReverbAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    reverb.prepare(sampleRate, samplesPerBlock);
    duckEnv.reset(sampleRate, 0.08);
    duckEnv.setCurrentAndTargetValue(0.0f);
}

void LusionSmartReverbAudioProcessor::releaseResources()
{
    reverb.reset();
}

bool LusionSmartReverbAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

//============================================================
void LusionSmartReverbAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    rmsLevel = 0.0f;
    peakLevel = 0.0f;

    for (int ch = 0; ch < numChannels; ++ch)
    {
        rmsLevel += buffer.getRMSLevel(ch, 0, numSamples);
        peakLevel += buffer.getMagnitude(ch, 0, numSamples);
    }

    rmsLevel /= juce::jmax(1, numChannels);
    peakLevel /= juce::jmax(1, numChannels);

    float wet = apvts.getRawParameterValue("WET")->load();
    float decay = apvts.getRawParameterValue("DECAY")->load();
    float width = apvts.getRawParameterValue("WIDTH")->load();

    const bool autoOn = apvts.getRawParameterValue("AUTO")->load() > 0.5f;
    const int  mode = (int)apvts.getRawParameterValue("MODE")->load();

    if (autoOn)
    {
        if (mode == 0) { wet = 0.25f; decay = 0.8f; }
        else if (mode == 1) { wet = 0.40f; decay = 2.2f; }
        else { wet = 0.55f; decay = 4.5f; }

        if (rmsLevel > 0.30f) { wet *= 0.65f; decay *= 0.7f; }
        else if (rmsLevel < 0.12f) { wet *= 1.2f;  decay *= 1.3f; }

        if (peakLevel > rmsLevel * 2.5f)
            decay *= 0.75f;

        width = (numChannels == 1 ? 0.7f : 0.9f);
    }

    wet = juce::jlimit(0.05f, 0.8f, wet);
    decay = juce::jlimit(0.2f, 6.0f, decay);
    width = juce::jlimit(0.3f, 1.0f, width);

    const float duckTarget =
        juce::jlimit(0.0f, 1.0f, (rmsLevel - 0.08f) * 2.0f);

    duckEnv.setTargetValue(duckTarget);
    duckAmount = duckEnv.getNextValue();

    const float duckedWet = wet * (1.0f - duckAmount * 0.7f);

    reverb.setWet(duckedWet);
    reverb.setDecay(decay);
    reverb.setWidth(width);

    reverb.process(buffer);
}

//============================================================
void LusionSmartReverbAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void LusionSmartReverbAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml)
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

// ===== PRESET HELPERS =====
void LusionSmartReverbAudioProcessor::loadPresetFromXml(const juce::String& xmlText)
{
    std::unique_ptr<juce::XmlElement> xml(juce::XmlDocument::parse(xmlText));
    if (xml)
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::String LusionSmartReverbAudioProcessor::getStateAsXmlString()
{
    auto state = apvts.copyState();
    if (auto xml = state.createXml())
        return xml->toString();

    return {};
}


//============================================================
// ✅ THIS WAS MISSING — CAUSING ALL LINKER ERRORS
juce::AudioProcessorEditor*
LusionSmartReverbAudioProcessor::createEditor()
{
    return new LusionSmartReverbAudioProcessorEditor(*this);
}

//============================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LusionSmartReverbAudioProcessor();
}
