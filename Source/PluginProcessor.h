/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SpinLockedPosInfo.h"
#include "SineWave.h"
#include "GenericVoice.h"
#include "TreeLabels.h"

//==============================================================================
/**
*/
class Synth_JUCEAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    Synth_JUCEAudioProcessor();
    ~Synth_JUCEAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void reset() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void processBlock(juce::AudioBuffer<double>&, juce::MidiBuffer&) override;

    template <typename FloatType>
    void process(juce::AudioBuffer<FloatType>& buffer, juce::MidiBuffer& midiMessages, juce::AudioBuffer<FloatType>& delayBuffer)
    {
        auto gainParamValue = apvts.getParameter(GAIN_ID)->getValue();
        //auto delayParamValue = apvts.getParameter(DELAY_ID)->getValue();
        for (auto i = 0; i < synth.getNumVoices(); i++) {
            auto voice = dynamic_cast<GenericVoice*>(synth.getVoice(i));
            voice->setEnvelopeSampleRate(getSampleRate());
            voice->setEnvelopeParams(
                apvts.getRawParameterValue(ATTACK_ID),
                apvts.getRawParameterValue(DECAY_ID),
                apvts.getRawParameterValue(SUSTAIN_ID),
                apvts.getRawParameterValue(RELEASE_ID)
            );
        }
        auto numSamples = buffer.getNumSamples();

        // In case we have more outputs than inputs, we'll clear any output
        // channels that didn't contain input data, (because these aren't
        // guaranteed to be empty - they may contain garbage).
        for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
            buffer.clear(i, 0, numSamples);

        // Now pass any incoming midi messages to our keyboard state object, and let it
        // add messages to the buffer if the user is clicking on the on-screen keys
        keyboardState.processNextMidiBuffer(midiMessages, 0, numSamples, true);

        // and now get our synth to process these midi events and generate its output.
        synth.renderNextBlock(buffer, midiMessages, 0, numSamples);

        // Apply our delay effect to the new output..
        //applyDelay(buffer, delayBuffer, delayParamValue);

        // Apply our gain change to the outgoing data..
        applyGain(buffer, delayBuffer, gainParamValue);

        // Now ask the host for the current time so we can store it to be displayed later...
        updateCurrentTimeInfoFromHost();
    }

    template <typename FloatType>
    void applyGain(juce::AudioBuffer<FloatType>& buffer, juce::AudioBuffer<FloatType>& delayBuffer, float gainLevel)
    {
        ignoreUnused(delayBuffer);

        for (auto channel = 0; channel < getTotalNumOutputChannels(); ++channel)
            buffer.applyGain(channel, 0, buffer.getNumSamples(), gainLevel);
    }

    void initialiseSynth();
    void Synth_JUCEAudioProcessor::setWaveType(WAVE_TYPE waveType);
    void updateCurrentTimeInfoFromHost();

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override { return JucePlugin_Name; };

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    //==============================================================================
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "None"; }
    void changeProgramName(int, const juce::String&) override {}

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    void updateTrackProperties(const TrackProperties& properties) override;
    TrackProperties Synth_JUCEAudioProcessor::getTrackProperties() const
    {
        const juce::ScopedLock sl(trackPropertiesLock);
        return trackProperties;
    }

    // our plugin's current state
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts;

    // this is kept up to date with the midi messages that arrive, and the UI component
    // registers with it so it can represent the incoming messages
    juce::MidiKeyboardState keyboardState;

    // this keeps a copy of the last set of time info that was acquired during an audio
    // callback - the UI component will read this and display it.
    SpinLockedPosInfo lastPosInfo;
private:

    using Filter = juce::dsp::IIR::Filter<float>;
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
    // Mono signal path: LowCut Filter --> Peak Filter --> HighCut Filter
    using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;
    MonoChain leftChain, rightChain;

    juce::AudioBuffer<float> delayBufferFloat;
    juce::AudioBuffer<double> delayBufferDouble;

    int delayPosition = 0;

    juce::Synthesiser synth;

    juce::CriticalSection trackPropertiesLock;
    TrackProperties trackProperties;


    static BusesProperties getBusesProperties()
    {
        return BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
            .withOutput("Output", juce::AudioChannelSet::stereo(), true);
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Synth_JUCEAudioProcessor)
};
