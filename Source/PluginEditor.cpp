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
    oscillatorComponent(p)
{

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
    addAndMakeVisible(oscillatorComponent);
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

    timecodeDisplayLabel.setBounds(getLocalBounds().reduced(2).removeFromTop(26));
    midiKeyboard.setBounds(getLocalBounds().reduced(2).removeFromBottom(70));

    auto sliderArea = getLocalBounds().reduced(2);
    sliderArea.removeFromTop(40);

    oscillatorComponent.setBounds(sliderArea.removeFromTop(80));

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
    return oscillatorComponent.getControlParameterIndex(control);
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

