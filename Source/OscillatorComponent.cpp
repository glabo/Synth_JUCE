#include "OscillatorComponent.h"

OscillatorComponent::OscillatorComponent(Synth_JUCEAudioProcessor& p,
                                            int id,
                                            juce::String wavetypeId,
                                            juce::String gainId,
                                            juce::String pitchId,
                                            juce::String delayId,
                                            juce::String attackId,
                                            juce::String decayId,
                                            juce::String sustainId,
                                            juce::String releaseId) :
    audioProcessor(p),
    waveTypeAttachment(p.apvts, wavetypeId, waveTypeSelection),
    gainAttachment(p.apvts, gainId, gainSlider),
    pitchAttachment(p.apvts, pitchId, pitchSlider),
    delayAttachment(p.apvts, delayId, delaySlider),
    attackAttachment(p.apvts, attackId, attackSlider),
    decayAttachment(p.apvts, decayId, decaySlider),
    sustainAttachment(p.apvts, sustainId, sustainSlider),
    releaseAttachment(p.apvts, releaseId, releaseSlider)
{
    oscId = id;
    // Wave type selector
    addAndMakeVisible(waveTypeSelection);

    auto* parameter = p.apvts.getParameter(wavetypeId);
    auto waveType = static_cast<WAVE_TYPE>(parameter->convertFrom0to1(parameter->getValue()) + 1);
    waveTypeSelection.addItemList(parameter->getAllValueStrings(), 1);

    waveTypeSelection.onChange = [this] { waveTypeSelectionChanged(oscId); };
    waveTypeSelection.setSelectedId(waveType);

    waveTypeLabel.attachToComponent(&waveTypeSelection, false);
    waveTypeLabel.setFont(juce::FontOptions(11.0f));

    // Gain slider
    addAndMakeVisible(gainSlider);
    gainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 12);

    gainLabel.attachToComponent(&gainSlider, false);
    gainLabel.setFont(juce::FontOptions(11.0f));
    gainLabel.setJustificationType(juce::Justification::centred);

    // Pitch
    addAndMakeVisible(pitchSlider);
    pitchSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    pitchSlider.setRange(-12, 12, 1.0);
    pitchSlider.setDoubleClickReturnValue(true, 0.0);
    pitchSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 12);

    pitchLabel.attachToComponent(&pitchSlider, false);
    pitchLabel.setFont(juce::FontOptions(11.0f));
    pitchLabel.setJustificationType(juce::Justification::centred);

    // Delay slider
    //addAndMakeVisible(delaySlider);
    //delaySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    //delaySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 12);

    //delayLabel.attachToComponent(&delaySlider, false);
    //delayLabel.setFont(juce::FontOptions(11.0f));
    //delayLabel.setJustificationType(juce::Justification::centred);

    // Attack slider
    addAndMakeVisible(attackSlider);
    attackSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    attackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 12);

    attackLabel.attachToComponent(&attackSlider, false);
    attackLabel.setFont(juce::FontOptions(11.0f));
    attackLabel.setJustificationType(juce::Justification::centred);

    // Decay slider
    addAndMakeVisible(decaySlider);
    decaySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    decaySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 12);

    decayLabel.attachToComponent(&decaySlider, false);
    decayLabel.setFont(juce::FontOptions(11.0f));
    decayLabel.setJustificationType(juce::Justification::centred);

    // Sustain slider
    addAndMakeVisible(sustainSlider);
    sustainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    sustainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 12);

    sustainLabel.attachToComponent(&sustainSlider, false);
    sustainLabel.setFont(juce::FontOptions(11.0f));
    sustainLabel.setJustificationType(juce::Justification::centred);

    // Release slider
    addAndMakeVisible(releaseSlider);
    releaseSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    releaseSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 12);

    releaseLabel.attachToComponent(&releaseSlider, false);
    releaseLabel.setFont(juce::FontOptions(11.0f));
    releaseLabel.setJustificationType(juce::Justification::centred);
}

void OscillatorComponent::resized() {
    auto controlArea = getLocalBounds();

    // Layout:
    // Wave selector is left quarter of initial size
    // Gain slider is left half of REMAINING size
    auto boxSubDivision = juce::jmin(180, controlArea.getWidth() / 7);
    auto waveTypeSelectionBounds = controlArea.removeFromLeft(boxSubDivision);
    waveTypeSelectionBounds.removeFromTop(30);
    waveTypeSelectionBounds.removeFromBottom(30);
    waveTypeSelectionBounds.removeFromRight(10);
    waveTypeSelectionBounds.removeFromLeft(10);
    waveTypeSelection.setBounds(waveTypeSelectionBounds);

    controlArea.removeFromTop(20);
    auto controlBounds = controlArea.removeFromLeft(boxSubDivision);
    auto gainSliderBounds = controlBounds.removeFromLeft(controlBounds.getWidth() / 2);
    auto pitchSliderBounds = controlBounds;
    gainSlider.setBounds(gainSliderBounds);
    pitchSlider.setBounds(pitchSliderBounds);

    auto adsrBounds = controlArea.removeFromLeft(boxSubDivision * 2);
    auto adsrBoxSubDivision = adsrBounds.getWidth() / 4;
    auto attackSliderBounds = adsrBounds.removeFromLeft(adsrBoxSubDivision);
    attackSlider.setBounds(attackSliderBounds);
    auto decaySliderBounds = adsrBounds.removeFromLeft(adsrBoxSubDivision);
    decaySlider.setBounds(decaySliderBounds);
    auto sustainSliderBounds = adsrBounds.removeFromLeft(adsrBoxSubDivision);
    sustainSlider.setBounds(sustainSliderBounds);
    auto releaseSliderBounds = adsrBounds.removeFromLeft(adsrBoxSubDivision);
    releaseSlider.setBounds(releaseSliderBounds);

    //auto delaySliderBounds = controlArea.removeFromLeft(boxSubDivision);
    //delaySlider.setBounds(delaySliderBounds);
}

void OscillatorComponent::waveTypeSelectionChanged(int oscId) {
    WAVE_TYPE waveType = static_cast<WAVE_TYPE>(this->waveTypeSelection.getSelectedId());
    audioProcessor.setWaveType(oscId, waveType);
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