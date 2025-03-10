/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "TreeLabels.h"

//==============================================================================
Synth_JUCEAudioProcessor::Synth_JUCEAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : MagicProcessor (getBusesProperties()),
       apvts(*this, nullptr, "apvts", createParameterLayout()),
       synth(apvts)
#endif
{
    FOLEYS_SET_SOURCE_PATH(__FILE__);
    // Add a sub-tree to store the state of our UI
    apvts.state.addChild({ "uiState", { { "width",  400 }, { "height", 200 } }, {} }, -1, nullptr);
    magicState.setGuiValueTree(BinaryData::layout_xml, BinaryData::layout_xmlSize);

    analyser = magicState.createAndAddObject<foleys::MagicAnalyser>("output_analyser");
    meter = magicState.createAndAddObject<foleys::MagicLevelSource>("output_meter");
    scope = magicState.createAndAddObject<foleys::MagicOscilloscope>("output_scope");

    initialiseSynth();
}

Synth_JUCEAudioProcessor::~Synth_JUCEAudioProcessor()
{
}

//==============================================================================
void Synth_JUCEAudioProcessor::prepareToPlay (double newSampleRate, int samplesPerBlock)
{
    synth.prepareToPlay(newSampleRate, samplesPerBlock);
    keyboardState.reset();

    // Prep delay buffer
    if (isUsingDoublePrecision())
    {
        delayBufferDouble.setSize(2, 12000);
        delayBufferFloat.setSize(1, 1);
    }
    else
    {
        delayBufferFloat.setSize(2, 12000);
        delayBufferDouble.setSize(1, 1);
    }

    analyser->prepareToPlay(newSampleRate, samplesPerBlock);
    meter->setupSource(getTotalNumOutputChannels(), newSampleRate, 300);
    scope->prepareToPlay(newSampleRate, samplesPerBlock);

    reset();
}

void Synth_JUCEAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    keyboardState.reset();
}

void Synth_JUCEAudioProcessor::reset()
{
    // Use this method as the place to clear any delay lines, buffers, etc, as it
    // means there's been a break in the audio's continuity.
    delayBufferFloat.clear();
    delayBufferDouble.clear();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Synth_JUCEAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // Only mono/stereo and input/output must have same layout
    const auto& mainOutput = layouts.getMainOutputChannelSet();
    const auto& mainInput = layouts.getMainInputChannelSet();

    // input and output layout must either be the same or the input must be disabled altogether
    if (!mainInput.isDisabled() && mainInput != mainOutput)
        return false;

    // only allow stereo and mono
    if (mainOutput.size() > 2)
        return false;

    return true;
}
#endif

void Synth_JUCEAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    jassert(!isUsingDoublePrecision());
    process(buffer, midiMessages, delayBufferFloat);
}

void Synth_JUCEAudioProcessor::initialiseSynth()
{
    auto numVoices = 8;

    // Add some voices...
    for (auto i = 0; i < numVoices; ++i)
        synth.addVoice(new GenericVoice(apvts));

    // ..and give the synth a sound to play
    synth.addSound(new GenericSound());
}

void Synth_JUCEAudioProcessor::updateCurrentTimeInfoFromHost()
{
    const auto newInfo = [&]
    {
        if (auto* ph = getPlayHead())
            if (auto result = ph->getPosition())
                return *result;

        // If the host fails to provide the current time, we'll just use default values
        return juce::AudioPlayHead::PositionInfo{};
    }();

    lastPosInfo.set(newInfo);
}

void Synth_JUCEAudioProcessor::updateTrackProperties(const TrackProperties& properties)
{
    {
        const juce::ScopedLock sl(trackPropertiesLock);
        trackProperties = properties;
    }

    juce::MessageManager::callAsync([this]
        {
            if (auto* editor = dynamic_cast<Synth_JUCEAudioProcessorEditor*> (getActiveEditor()))
            editor->updateTrackProperties();
        });
}

juce::AudioProcessorValueTreeState::ParameterLayout
Synth_JUCEAudioProcessor::createParameterLayout()
{
    // Master oscillator controls
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    Synthesizer::createParameterLayout(layout);
    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Synth_JUCEAudioProcessor();
}
