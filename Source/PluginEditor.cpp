/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"

//==============================================================================
Synth_JUCEAudioProcessorEditor::Synth_JUCEAudioProcessorEditor (Synth_JUCEAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    midiKeyboard(p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard),
    waveTypeAttachment(p.apvts, "wavetype", waveTypeSelection),
    gainAttachment(p.apvts, "gain", gainSlider),
    delayAttachment(p.apvts, "delay", delaySlider)
{
    // add wave type selector
    addAndMakeVisible(waveTypeSelection);
    waveTypeSelection.addItem("Sine", 1);
    waveTypeSelection.addItem("Square", 2);
    waveTypeSelection.addItem("Triangle", 3);
    //waveTypeSelection.addItem("Saw", 4);
    waveTypeSelection.addItem("Saw Analog", 5);
    waveTypeSelection.addItem("Noise", 6);

    waveTypeSelection.onChange = [this] { waveTypeSelectionChanged(); };
    waveTypeSelection.setSelectedId(WAVE_TYPE::SINE);

    waveTypeLabel.attachToComponent(&waveTypeSelection, false);
    waveTypeLabel.setFont(juce::FontOptions(11.0f));

    // add some sliders..
    addAndMakeVisible(gainSlider);
    gainSlider.setSliderStyle(juce::Slider::Rotary);

    addAndMakeVisible(delaySlider);
    delaySlider.setSliderStyle(juce::Slider::Rotary);

    // add some labels for the sliders..
    gainLabel.attachToComponent(&gainSlider, false);
    gainLabel.setFont(juce::FontOptions(11.0f));

    delayLabel.attachToComponent(&delaySlider, false);
    delayLabel.setFont(juce::FontOptions(11.0f));

    // add the midi keyboard component..
    addAndMakeVisible(midiKeyboard);

    // add a label that will display the current timecode and status..
    addAndMakeVisible(timecodeDisplayLabel);
    timecodeDisplayLabel.setFont(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), 15.0f, juce::Font::plain));

    // set resize limits for this plug-in
    setResizeLimits(400, 200, 1024, 700);
    setResizable(true, p.wrapperType !=
        juce::AudioProcessor::WrapperType::wrapperType_AudioUnitv3);

    lastUIWidth.referTo(p.apvts.state.getChildWithName("uiState").getPropertyAsValue("width", nullptr));
    lastUIHeight.referTo(p.apvts.state.getChildWithName("uiState").getPropertyAsValue("height", nullptr));

    // set our component's initial size to be the last one that was stored in the filter's settings
    setSize(lastUIWidth.getValue(), lastUIHeight.getValue());

    lastUIWidth.addListener(this);
    lastUIHeight.addListener(this);

    updateTrackProperties();

    // start a timer which will keep our timecode display updated
    startTimerHz(30);
}

//==============================================================================
void Synth_JUCEAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.setColour(backgroundColour);
    g.fillAll ();
}

void Synth_JUCEAudioProcessorEditor::resized()
{
    // This lays out our child components...

    auto r = getLocalBounds().reduced(8);

    timecodeDisplayLabel.setBounds(r.removeFromTop(26));
    midiKeyboard.setBounds(r.removeFromBottom(70));

    r.removeFromTop(20);
    auto sliderArea = r.removeFromTop(60);

    waveTypeSelection.setBounds(sliderArea.removeFromLeft(juce::jmin(180, sliderArea.getWidth() / 4)));
    gainSlider.setBounds(sliderArea.removeFromLeft(juce::jmin(180, sliderArea.getWidth() / 2)));
    delaySlider.setBounds(sliderArea.removeFromLeft(juce::jmin(180, sliderArea.getWidth())));

    lastUIWidth = getWidth();
    lastUIHeight = getHeight();
}

void Synth_JUCEAudioProcessorEditor::timerCallback()
{
    updateTimecodeDisplay(getProcessor().lastPosInfo.get());
}

void Synth_JUCEAudioProcessorEditor::hostMIDIControllerIsAvailable(bool controllerIsAvailable)
{
    midiKeyboard.setVisible(!controllerIsAvailable);
}

int Synth_JUCEAudioProcessorEditor::getControlParameterIndex(Component& control)
{
    if (&control == &gainSlider)
        return 0;

    if (&control == &delaySlider)
        return 1;

    return -1;
}

void Synth_JUCEAudioProcessorEditor::updateTrackProperties()
{
    auto trackColour = getProcessor().getTrackProperties().colour;
    auto& lf = getLookAndFeel();

    backgroundColour = (trackColour == juce::Colour() ? lf.findColour(juce::ResizableWindow::backgroundColourId)
        : trackColour.withAlpha(1.0f).withBrightness(0.266f));
    repaint();
}

Synth_JUCEAudioProcessor& Synth_JUCEAudioProcessorEditor::getProcessor() const
{
    return static_cast<Synth_JUCEAudioProcessor&> (processor);
}

void Synth_JUCEAudioProcessorEditor::updateTimecodeDisplay(const juce::AudioPlayHead::PositionInfo& pos)
{
    juce::MemoryOutputStream displayText;

    const auto sig = pos.getTimeSignature().orFallback(juce::AudioPlayHead::TimeSignature{});

    displayText << "[" << juce::SystemStats::getJUCEVersion() << "]   "
        << juce::String(pos.getBpm().orFallback(120.0), 2) << " bpm, "
        << sig.numerator << '/' << sig.denominator
        << "  -  " << timeToTimecodeString(pos.getTimeInSeconds().orFallback(0.0))
        << "  -  " << quarterNotePositionToBarsBeatsString(pos.getPpqPosition().orFallback(0.0), sig);

    if (pos.getIsRecording())
        displayText << "  (recording)";
    else if (pos.getIsPlaying())
        displayText << "  (playing)";

    timecodeDisplayLabel.setText(displayText.toString(), juce::dontSendNotification);
}

void Synth_JUCEAudioProcessorEditor::waveTypeSelectionChanged() {
    WAVE_TYPE waveType = static_cast<WAVE_TYPE>(this->waveTypeSelection.getSelectedId());
    audioProcessor.setWaveType(waveType);
}

