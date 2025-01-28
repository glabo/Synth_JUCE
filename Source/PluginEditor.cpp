/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"

//==============================================================================
Synth_JUCEAudioProcessorEditor::Synth_JUCEAudioProcessorEditor(Synth_JUCEAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),
    midiKeyboard(p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard),
    oscillatorComponent0(p, OSC_0, WAVETYPE_ID_0, GAIN_ID_0, PITCH_ID_0, DELAY_ID_0, ATTACK_ID_0, DECAY_ID_0, SUSTAIN_ID_0, RELEASE_ID_0),
    oscillatorComponent1(p, OSC_1, WAVETYPE_ID_1, GAIN_ID_1, PITCH_ID_1, DELAY_ID_1, ATTACK_ID_1, DECAY_ID_1, SUSTAIN_ID_1, RELEASE_ID_1),
    oscillatorComponent2(p, OSC_2, WAVETYPE_ID_2, GAIN_ID_2, PITCH_ID_2, DELAY_ID_2, ATTACK_ID_2, DECAY_ID_2, SUSTAIN_ID_2, RELEASE_ID_2),
    oscillatorComponent3(p, OSC_3, WAVETYPE_ID_3, GAIN_ID_3, PITCH_ID_3, DELAY_ID_3, ATTACK_ID_3, DECAY_ID_3, SUSTAIN_ID_3, RELEASE_ID_3),
    filterComponent(p, FILTER_TYPE_ID, CUTOFF_FREQ_ID, Q_ID, RESONANCE_ID)
{

    // add the midi keyboard component..
    addAndMakeVisible(midiKeyboard);

    // set resize limits for this plug-in
    setResizeLimits(400, 200, 1024, 700);
    setResizable(true, p.wrapperType !=
        juce::AudioProcessor::WrapperType::wrapperType_AudioUnitv3);

    lastUIWidth.referTo(p.apvts.state.getChildWithName("uiState").getPropertyAsValue("width", nullptr));
    lastUIHeight.referTo(p.apvts.state.getChildWithName("uiState").getPropertyAsValue("height", nullptr));

    // set our component's initial size to be the last one that was stored in the filter's settings
    addAndMakeVisible(oscillatorComponent0);
    addAndMakeVisible(oscillatorComponent1);
    addAndMakeVisible(oscillatorComponent2);
    addAndMakeVisible(oscillatorComponent3);
    addAndMakeVisible(filterComponent);

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

    //g.setColour(juce::Colours::orange);
    //g.drawRect(filterBorder);
}

void Synth_JUCEAudioProcessorEditor::resized()
{
    // Lay out components
    midiKeyboard.setBounds(getLocalBounds().reduced(2).removeFromBottom(70));

    auto sliderArea = getLocalBounds().reduced(2);
    sliderArea.removeFromTop(40);

    auto verticalDivision = juce::jmin(180, sliderArea.proportionOfWidth(0.1f));

    // Define UI regions
    oscillatorBorder = sliderArea;
    oscillatorBorder.removeFromRight(verticalDivision * 4);
    oscillatorBorder.removeFromBottom(oscillatorBorder.getHeight() - 4 * OscillatorComponent::getHeight());

    oscillatorComponent0.setBounds(oscillatorBorder.removeFromTop(80));
    oscillatorComponent1.setBounds(oscillatorBorder.removeFromTop(80));
    oscillatorComponent2.setBounds(oscillatorBorder.removeFromTop(80));
    oscillatorComponent3.setBounds(oscillatorBorder.removeFromTop(80));

    filterBorder = sliderArea;
    filterBorder.removeFromLeft(oscillatorBorder.getWidth());
    // Assuming 80px line height
    filterBorder.removeFromBottom(filterBorder.getHeight() - 2 * 80);

    filterComponent.setBounds(filterBorder);

    lastUIWidth = getWidth();
    lastUIHeight = getHeight();
}

void Synth_JUCEAudioProcessorEditor::timerCallback()
{
    //updateTimecodeDisplay(getProcessor().lastPosInfo.get());
}

void Synth_JUCEAudioProcessorEditor::hostMIDIControllerIsAvailable(bool controllerIsAvailable)
{
    midiKeyboard.setVisible(!controllerIsAvailable);
}

int Synth_JUCEAudioProcessorEditor::getControlParameterIndex(Component& control)
{
    // Maybe we won't have to update this?
    return oscillatorComponent0.getControlParameterIndex(control);
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
    // UNUSED
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

