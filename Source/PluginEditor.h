/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "WaveGenerator.h"
#include "OscillatorComponent.h"

//==============================================================================
/**
*/
class Synth_JUCEAudioProcessorEditor : public juce::AudioProcessorEditor,
                                        private juce::Timer,
                                        private juce::Value::Listener
{
public:
    Synth_JUCEAudioProcessorEditor (Synth_JUCEAudioProcessor&);
    ~Synth_JUCEAudioProcessorEditor() override {};

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;
    void hostMIDIControllerIsAvailable(bool controllerIsAvailable) override;
    int getControlParameterIndex(Component& control) override;

    void updateTrackProperties();


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Synth_JUCEAudioProcessor& audioProcessor;

    juce::MidiKeyboardComponent midiKeyboard;

    juce::Label timecodeDisplayLabel;

    static const int NUM_OSC = 4; // Hints for future

    OscillatorComponent oscillatorComponent0;
    OscillatorComponent oscillatorComponent1;
    OscillatorComponent oscillatorComponent2;
    OscillatorComponent oscillatorComponent3;

    // UI regions
    juce::Rectangle<int> oscillatorBorder;
    juce::Rectangle<int> filterBorder;

    juce::Colour backgroundColour;

    // these are used to persist the UI's size - the values are stored along with the
    // filter's other parameters, and the UI component will update them when it gets
    // resized.
    juce::Value lastUIWidth, lastUIHeight;

    Synth_JUCEAudioProcessor& getProcessor() const;
    //==============================================================================
    // quick-and-dirty function to format a timecode string
    static juce::String timeToTimecodeString(double seconds)
    {
        auto millisecs = juce::roundToInt(seconds * 1000.0);
        auto absMillisecs = std::abs(millisecs);

        return juce::String::formatted("%02d:%02d:%02d.%03d",
            millisecs / 3600000,
            (absMillisecs / 60000) % 60,
            (absMillisecs / 1000) % 60,
            absMillisecs % 1000);
    }

    // quick-and-dirty function to format a bars/beats string
    static juce::String quarterNotePositionToBarsBeatsString(double quarterNotes, juce::AudioPlayHead::TimeSignature sig)
    {
        if (sig.numerator == 0 || sig.denominator == 0)
            return "1|1|000";

        auto quarterNotesPerBar = (sig.numerator * 4 / sig.denominator);
        auto beats = (fmod(quarterNotes, quarterNotesPerBar) / quarterNotesPerBar) * sig.numerator;

        auto bar = ((int)quarterNotes) / quarterNotesPerBar + 1;
        auto beat = ((int)beats) + 1;
        auto ticks = ((int)(fmod(beats, 1.0) * 960.0 + 0.5));
        return juce::String::formatted("%d|%d|%03d", bar, beat, ticks);
    }
    void updateTimecodeDisplay(const juce::AudioPlayHead::PositionInfo& pos);

    // called when the stored window size changes
    void valueChanged(juce::Value&) override
    {
        setSize(lastUIWidth.getValue(), lastUIHeight.getValue());
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Synth_JUCEAudioProcessorEditor)
};
