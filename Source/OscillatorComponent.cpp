#include "OscillatorComponent.h"

OscillatorComponent::OscillatorComponent(Synth_JUCEAudioProcessor& p) :
    audioProcessor(p),
    waveTypeAttachment(p.apvts, "wavetype", waveTypeSelection),
    gainAttachment(p.apvts, "gain", gainSlider),
    delayAttachment(p.apvts, "delay", delaySlider)
{
    // Wave type selector
    addAndMakeVisible(waveTypeSelection);

    auto* parameter = p.apvts.getParameter("wavetype");
    auto waveType = static_cast<WAVE_TYPE>(parameter->convertFrom0to1(parameter->getValue()) + 1);
    waveTypeSelection.addItemList(parameter->getAllValueStrings(), 1);

    waveTypeSelection.onChange = [this] { waveTypeSelectionChanged(); };
    waveTypeSelection.setSelectedId(waveType);

    waveTypeLabel.attachToComponent(&waveTypeSelection, false);
    waveTypeLabel.setFont(juce::FontOptions(11.0f));

    // Gain slider
    addAndMakeVisible(gainSlider);
    gainSlider.setSliderStyle(juce::Slider::Rotary);
    gainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 12);

    gainLabel.attachToComponent(&gainSlider, false);
    gainLabel.setFont(juce::FontOptions(11.0f));
    gainLabel.setJustificationType(juce::Justification::centred);

    // Delay slider
    addAndMakeVisible(delaySlider);
    delaySlider.setSliderStyle(juce::Slider::Rotary);
    delaySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 12);

    delayLabel.attachToComponent(&delaySlider, false);
    delayLabel.setFont(juce::FontOptions(11.0f));
    delayLabel.setJustificationType(juce::Justification::centred);
}

void OscillatorComponent::resized() {
    auto controlArea = getLocalBounds();

    // Layout:
    // Wave selector is left quarter of initial size
    // Gain slider is left half of REMAINING size
    // Delay slider is left half of REMAINING size
    auto waveTypeSelectionBounds = controlArea.removeFromLeft(juce::jmin(180, controlArea.getWidth() / 4));
    waveTypeSelectionBounds.removeFromTop(30);
    waveTypeSelectionBounds.removeFromBottom(30);
    waveTypeSelectionBounds.removeFromRight(20);
    waveTypeSelectionBounds.removeFromLeft(20);
    waveTypeSelection.setBounds(waveTypeSelectionBounds);
    controlArea.removeFromTop(20);
    gainSlider.setBounds(controlArea.removeFromLeft(juce::jmin(180, controlArea.getWidth() / 2)));
    delaySlider.setBounds(controlArea.removeFromLeft(juce::jmin(180, controlArea.getWidth())));
}

void OscillatorComponent::waveTypeSelectionChanged() {
    WAVE_TYPE waveType = static_cast<WAVE_TYPE>(this->waveTypeSelection.getSelectedId());
    audioProcessor.setWaveType(waveType);
}

int OscillatorComponent::getControlParameterIndex(Component& control)
{
    if (&control == &gainSlider)
        return 0;

    if (&control == &delaySlider)
        return 1;

    if (&control == &waveTypeSelection)
        return 2;

    return -1;
}