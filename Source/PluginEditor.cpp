#include "PluginEditor.h"

//==============================================================================
// COLOR SCHEME - Modern Dark Theme
//==============================================================================
namespace Colors
{
    const juce::Colour background = juce::Colour(0xff0a0e0d);
    const juce::Colour panel = juce::Colour(0xff141716);
    const juce::Colour panelLight = juce::Colour(0xff1a1f1d);
    const juce::Colour accent = juce::Colour(0xff00ffaa);
    const juce::Colour accentDim = juce::Colour(0xff00cc88);
    const juce::Colour text = juce::Colour(0xffe0e5e3);
    const juce::Colour textDim = juce::Colour(0xff8c9692);
    const juce::Colour textVeryDim = juce::Colour(0xff505854);
    const juce::Colour meter = juce::Colour(0xff00d4ff);
    const juce::Colour warning = juce::Colour(0xffff6b35);
}

//==============================================================================
// MODERN ROTARY SLIDER IMPLEMENTATION
//==============================================================================
void ModernRotarySlider::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    auto center = bounds.getCentre();

    // Calculate dimensions
    float diameter = juce::jmin(bounds.getWidth(), bounds.getHeight()) - 20.0f;
    float radius = diameter / 2.0f;
    float trackWidth = 4.0f;
    float knobRadius = radius - trackWidth - 4.0f;

    // Get rotation angle
    auto sliderPos = (float)valueToProportionOfLength(getValue());
    float startAngle = juce::MathConstants<float>::pi * 1.25f;
    float endAngle = juce::MathConstants<float>::pi * 2.75f;
    float currentAngle = startAngle + sliderPos * (endAngle - startAngle);

    // Draw outer ring (background track)
    {
        juce::Path track;
        track.addCentredArc(center.x, center.y, radius, radius, 0.0f,
            startAngle, endAngle, true);

        g.setColour(Colors::panelLight);
        g.strokePath(track, juce::PathStrokeType(trackWidth,
            juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // Draw filled arc (value indicator)
    {
        juce::Path valueArc;
        valueArc.addCentredArc(center.x, center.y, radius, radius, 0.0f,
            startAngle, currentAngle, true);

        juce::ColourGradient gradient(
            Colors::accent.withAlpha(0.8f), center.x, center.y - radius,
            Colors::accentDim, center.x, center.y + radius, false);

        g.setGradientFill(gradient);
        g.strokePath(valueArc, juce::PathStrokeType(trackWidth,
            juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // Draw center knob
    {
        g.setColour(Colors::panel);
        g.fillEllipse(center.x - knobRadius, center.y - knobRadius,
            knobRadius * 2.0f, knobRadius * 2.0f);

        // Inner glow
        g.setColour(Colors::accent.withAlpha(0.15f));
        g.fillEllipse(center.x - knobRadius + 2, center.y - knobRadius + 2,
            (knobRadius - 2) * 2.0f, (knobRadius - 2) * 2.0f);
    }

    // Draw indicator line
    {
        float indicatorLength = knobRadius * 0.6f;
        float indicatorX = center.x + std::cos(currentAngle - juce::MathConstants<float>::halfPi) * indicatorLength;
        float indicatorY = center.y + std::sin(currentAngle - juce::MathConstants<float>::halfPi) * indicatorLength;

        juce::Path indicator;
        indicator.startNewSubPath(center.x, center.y);
        indicator.lineTo(indicatorX, indicatorY);

        g.setColour(Colors::accent);
        g.strokePath(indicator, juce::PathStrokeType(2.5f,
            juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Dot at end
        g.fillEllipse(indicatorX - 3.5f, indicatorY - 3.5f, 7.0f, 7.0f);
    }

    // Draw value text
    {
        g.setColour(Colors::text);
        g.setFont(juce::Font(16.0f, juce::Font::bold));

        juce::String valueText = juce::String(getValue(), 2);
        auto textBounds = juce::Rectangle<float>(center.x - 30, center.y - 10, 60, 20);
        g.drawText(valueText, textBounds, juce::Justification::centred);
    }
}

//==============================================================================
// MODERN TOGGLE BUTTON IMPLEMENTATION
//==============================================================================
void ModernToggleButton::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
    auto bounds = getLocalBounds().toFloat().reduced(1.0f);
    bool isOn = getToggleState();

    // Background
    g.setColour(isOn ? Colors::accent.withAlpha(0.2f) : Colors::panelLight);
    g.fillRoundedRectangle(bounds, 6.0f);

    // Border
    g.setColour(isOn ? Colors::accent : Colors::textVeryDim);
    g.drawRoundedRectangle(bounds, 6.0f, isOn ? 2.0f : 1.0f);

    // Hover effect
    if (shouldDrawButtonAsHighlighted)
    {
        g.setColour(Colors::accent.withAlpha(0.1f));
        g.fillRoundedRectangle(bounds, 6.0f);
    }

    // Text
    g.setColour(isOn ? Colors::accent : Colors::text);
    g.setFont(juce::Font(13.0f, juce::Font::bold));
    g.drawText(getButtonText(), bounds, juce::Justification::centred);

    // Indicator dot
    if (isOn)
    {
        g.setColour(Colors::accent);
        g.fillEllipse(bounds.getRight() - 16, bounds.getCentreY() - 3, 6, 6);
    }
}

//==============================================================================
// MAIN EDITOR IMPLEMENTATION
//==============================================================================
LusionSmartReverbAudioProcessorEditor::LusionSmartReverbAudioProcessorEditor(
    LusionSmartReverbAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(900, 600);

    // Initialize waveform data
    waveformData.resize(200, 0.0f);

    // Setup sliders
    wetSlider.setLabel("WET");
    decaySlider.setLabel("DECAY");
    widthSlider.setLabel("WIDTH");

    addAndMakeVisible(wetSlider);
    addAndMakeVisible(decaySlider);
    addAndMakeVisible(widthSlider);

    // Setup labels
    auto setupLabel = [&](juce::Label& label, const juce::String& text)
        {
            label.setText(text, juce::dontSendNotification);
            label.setFont(juce::Font(14.0f, juce::Font::bold));
            label.setColour(juce::Label::textColourId, Colors::textDim);
            label.setJustificationType(juce::Justification::centred);
            addAndMakeVisible(label);
        };

    setupLabel(wetLabel, "WET");
    setupLabel(decayLabel, "DECAY");
    setupLabel(widthLabel, "WIDTH");

    // Setup value labels
    for (int i = 0; i < 3; ++i)
    {
        valueLabels[i].setFont(juce::Font(11.0f));
        valueLabels[i].setColour(juce::Label::textColourId, Colors::textVeryDim);
        valueLabels[i].setJustificationType(juce::Justification::centred);
        addAndMakeVisible(valueLabels[i]);
    }

    // Setup auto button
    autoButton.setClickingTogglesState(true);
    addAndMakeVisible(autoButton);

    // Setup mode selector
    modeSelector.addItem("SHORT REVERB", 1);
    modeSelector.addItem("LONG REVERB", 2);
    modeSelector.addItem("TAIL MODE", 3);
    modeSelector.setSelectedId(2);

    modeSelector.setColour(juce::ComboBox::backgroundColourId, Colors::panelLight);
    modeSelector.setColour(juce::ComboBox::outlineColourId, Colors::textVeryDim);
    modeSelector.setColour(juce::ComboBox::textColourId, Colors::text);
    modeSelector.setColour(juce::ComboBox::arrowColourId, Colors::accent);

    addAndMakeVisible(modeSelector);

    // Create attachments
    wetAttachment = std::make_unique<SA>(p.apvts, "WET", wetSlider);
    decayAttachment = std::make_unique<SA>(p.apvts, "DECAY", decaySlider);
    widthAttachment = std::make_unique<SA>(p.apvts, "WIDTH", widthSlider);
    autoAttachment = std::make_unique<BA>(p.apvts, "AUTO", autoButton);
    modeAttachment = std::make_unique<CA>(p.apvts, "MODE", modeSelector);

    // Start timer for animation
    startTimerHz(60);
}

LusionSmartReverbAudioProcessorEditor::~LusionSmartReverbAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void LusionSmartReverbAudioProcessorEditor::timerCallback()
{
    // Smooth visual parameters
    float targetRms = processor.getRmsLevel() * 2.5f;
    float targetDuck = processor.getDuckAmount();

    rmsSmoothed += (targetRms - rmsSmoothed) * 0.15f;
    duckSmoothed += (targetDuck - duckSmoothed) * 0.1f;

    // Update waveform
    waveformData[waveformIndex] = rmsSmoothed;
    waveformIndex = (waveformIndex + 1) % waveformData.size();

    // Animation
    animationPhase += 0.02f;
    if (animationPhase > juce::MathConstants<float>::twoPi)
        animationPhase -= juce::MathConstants<float>::twoPi;

    // Update value labels
    valueLabels[0].setText(juce::String(wetSlider.getValue(), 2), juce::dontSendNotification);
    valueLabels[1].setText(juce::String(decaySlider.getValue(), 2) + "s", juce::dontSendNotification);
    valueLabels[2].setText(juce::String(widthSlider.getValue(), 2), juce::dontSendNotification);

    repaint();
}

//==============================================================================
void LusionSmartReverbAudioProcessorEditor::paint(juce::Graphics& g)
{
    drawBackground(g);
    drawHeader(g);
    drawVisualization(g);
    drawMeters(g);
}

//==============================================================================
void LusionSmartReverbAudioProcessorEditor::drawBackground(juce::Graphics& g)
{
    g.fillAll(Colors::background);

    // Main panel
    auto mainArea = getLocalBounds().reduced(20);

    // Gradient background
    juce::ColourGradient gradient(
        Colors::panel.brighter(0.1f), mainArea.getCentreX(), mainArea.getY(),
        Colors::panel.darker(0.3f), mainArea.getCentreX(), mainArea.getBottom(),
        false);

    g.setGradientFill(gradient);
    g.fillRoundedRectangle(mainArea.toFloat(), 12.0f);

    // Subtle glow when auto mode is on
    if (autoButton.getToggleState())
    {
        float glowIntensity = 0.08f + rmsSmoothed * 0.12f;
        g.setColour(Colors::accent.withAlpha(glowIntensity));
        g.fillRoundedRectangle(mainArea.toFloat(), 12.0f);
    }

    // Border
    g.setColour(Colors::accent.withAlpha(0.3f));
    g.drawRoundedRectangle(mainArea.toFloat(), 12.0f, 1.5f);
}

//==============================================================================
void LusionSmartReverbAudioProcessorEditor::drawHeader(juce::Graphics& g)
{
    // Title
    g.setColour(Colors::accent);
    g.setFont(juce::Font(32.0f, juce::Font::bold));
    g.drawText("LUSION", 40, 30, 200, 40, juce::Justification::left);

    g.setColour(Colors::text);
    g.setFont(juce::Font(32.0f, juce::Font::plain));
    g.drawText(" SMART REVERB", 150, 30, 300, 40, juce::Justification::left);

    // Subtitle
    g.setColour(Colors::textDim);
    g.setFont(juce::Font(13.0f));

    juce::String modeText = modeSelector.getSelectedItemIndex() == 0 ? "SHORT" :
        modeSelector.getSelectedItemIndex() == 1 ? "LONG" : "TAIL";

    juce::String statusText = autoButton.getToggleState()
        ? "INTELLIGENT PROCESSING  " + modeText + " MODE"
        : "MANUAL MODE  " + modeText;

    g.drawText(statusText, 40, 70, 500, 20, juce::Justification::left);
}

//==============================================================================
void LusionSmartReverbAudioProcessorEditor::drawVisualization(juce::Graphics& g)
{
    auto vizArea = juce::Rectangle<int>(40, 120, getWidth() - 80, 120);

    // Background panel
    g.setColour(Colors::background.brighter(0.05f));
    g.fillRoundedRectangle(vizArea.toFloat(), 8.0f);

    // Draw waveform
    if (!waveformData.empty())
    {
        juce::Path waveform;
        float xStep = vizArea.getWidth() / (float)waveformData.size();

        for (size_t i = 0; i < waveformData.size(); ++i)
        {
            int idx = (waveformIndex + i) % waveformData.size();
            float x = vizArea.getX() + i * xStep;
            float y = vizArea.getCentreY() - (waveformData[idx] * vizArea.getHeight() * 0.35f);

            if (i == 0)
                waveform.startNewSubPath(x, y);
            else
                waveform.lineTo(x, y);
        }

        // Reflection
        juce::Path reflection = waveform;
        reflection.applyTransform(juce::AffineTransform::verticalFlip(vizArea.getCentreY()));

        g.setColour(Colors::accent.withAlpha(0.1f));
        g.strokePath(reflection, juce::PathStrokeType(1.5f));

        // Main waveform
        juce::ColourGradient waveGradient(
            Colors::accent, vizArea.getCentreX(), vizArea.getY(),
            Colors::meter, vizArea.getCentreX(), vizArea.getBottom(), false);

        g.setGradientFill(waveGradient);
        g.strokePath(waveform, juce::PathStrokeType(2.0f));
    }

    // Center line
    g.setColour(Colors::textVeryDim.withAlpha(0.3f));
    g.drawLine(vizArea.getX(), vizArea.getCentreY(),
        vizArea.getRight(), vizArea.getCentreY(), 0.5f);

    // Label
    g.setColour(Colors::textVeryDim);
    g.setFont(juce::Font(11.0f));
    g.drawText("REAL-TIME ANALYZER", vizArea.getX() + 10, vizArea.getY() + 5,
        200, 20, juce::Justification::left);
}

//==============================================================================
void LusionSmartReverbAudioProcessorEditor::drawMeters(juce::Graphics& g)
{
    auto meterArea = juce::Rectangle<int>(getWidth() - 100, 280, 60, 280);

    // RMS Meter
    {
        auto rmsBar = meterArea.removeFromTop(130);

        g.setColour(Colors::panelLight);
        g.fillRoundedRectangle(rmsBar.toFloat(), 6.0f);

        float fillHeight = rmsSmoothed * rmsBar.getHeight();
        auto fillRect = rmsBar.toFloat();
        fillRect = fillRect.removeFromBottom(fillHeight);

        juce::ColourGradient meterGrad(
            Colors::accent, fillRect.getCentreX(), fillRect.getBottom(),
            Colors::meter, fillRect.getCentreX(), fillRect.getY(), false);

        g.setGradientFill(meterGrad);
        g.fillRoundedRectangle(fillRect, 6.0f);

        g.setColour(Colors::textDim);
        g.setFont(juce::Font(11.0f, juce::Font::bold));
        g.drawText("INPUT", rmsBar.getX(), rmsBar.getBottom() + 5,
            rmsBar.getWidth(), 20, juce::Justification::centred);
    }

    meterArea.removeFromTop(20);

    // Duck Meter
    {
        auto duckBar = meterArea;

        g.setColour(Colors::panelLight);
        g.fillRoundedRectangle(duckBar.toFloat(), 6.0f);

        float fillHeight = duckSmoothed * duckBar.getHeight();
        auto fillRect = duckBar.toFloat();
        fillRect = fillRect.removeFromBottom(fillHeight);

        g.setColour(Colors::warning);
        g.fillRoundedRectangle(fillRect, 6.0f);

        g.setColour(Colors::textDim);
        g.setFont(juce::Font(11.0f, juce::Font::bold));
        g.drawText("DUCK", duckBar.getX(), duckBar.getBottom() + 5,
            duckBar.getWidth(), 20, juce::Justification::centred);
    }
}

//==============================================================================
void LusionSmartReverbAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    // Control section
    auto controlArea = area.removeFromBottom(280).reduced(40, 20);

    int knobSize = 180;
    int spacing = (controlArea.getWidth() - knobSize * 3) / 4;

    // Position sliders
    wetSlider.setBounds(controlArea.getX() + spacing,
        controlArea.getY() + 40, knobSize, knobSize);
    decaySlider.setBounds(wetSlider.getRight() + spacing,
        controlArea.getY() + 40, knobSize, knobSize);
    widthSlider.setBounds(decaySlider.getRight() + spacing,
        controlArea.getY() + 40, knobSize, knobSize);

    // Position labels above sliders
    wetLabel.setBounds(wetSlider.getX(), wetSlider.getY() - 25, knobSize, 20);
    decayLabel.setBounds(decaySlider.getX(), decaySlider.getY() - 25, knobSize, 20);
    widthLabel.setBounds(widthSlider.getX(), widthSlider.getY() - 25, knobSize, 20);

    // Position value labels below sliders
    valueLabels[0].setBounds(wetSlider.getX(), wetSlider.getBottom() + 5, knobSize, 20);
    valueLabels[1].setBounds(decaySlider.getX(), decaySlider.getBottom() + 5, knobSize, 20);
    valueLabels[2].setBounds(widthSlider.getX(), widthSlider.getBottom() + 5, knobSize, 20);

    // Position mode controls
    autoButton.setBounds(550, 35, 140, 32);
    modeSelector.setBounds(700, 35, 160, 32);
}