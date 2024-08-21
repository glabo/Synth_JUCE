/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Synth_JUCEAudioProcessor::Synth_JUCEAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (getBusesProperties()),
       apvts(*this, nullptr, "apvts", createParameterLayout())
#endif
{
    // Add a sub-tree to store the state of our UI
    apvts.state.addChild({ "uiState", { { "width",  400 }, { "height", 200 } }, {} }, -1, nullptr);

    initialiseSynth();
}

Synth_JUCEAudioProcessor::~Synth_JUCEAudioProcessor()
{
}

//==============================================================================
void Synth_JUCEAudioProcessor::prepareToPlay (double newSampleRate, int /*samplesPerBlock*/)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    synth.setCurrentPlaybackSampleRate(newSampleRate);
    keyboardState.reset();

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

void Synth_JUCEAudioProcessor::processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    jassert(isUsingDoublePrecision());
    process(buffer, midiMessages, delayBufferDouble);
}

void Synth_JUCEAudioProcessor::initialiseSynth()
{
    auto numVoices = 8;

    // Add some voices...
    for (auto i = 0; i < numVoices; ++i)
        synth.addVoice(new GenericVoice());

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

//==============================================================================
bool Synth_JUCEAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Synth_JUCEAudioProcessor::createEditor()
{
    return new Synth_JUCEAudioProcessorEditor (*this);
}

//==============================================================================
void Synth_JUCEAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Store an xml representation of our state.
    if (auto xmlState = apvts.copyState().createXml())
        copyXmlToBinary(*xmlState, destData);
}

void Synth_JUCEAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Restore our plug-in's state from the xml representation stored in the above
    // method.
    if (auto xmlState = getXmlFromBinary(data, sizeInBytes))
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
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
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "gain",  1 }, "Gain", juce::NormalisableRange<float>(0.0f, 1.0f), 0.9f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "delay", 1 }, "Delay Feedback", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));

    // FILTER STUFF that we'll use later
 /*   layout.add(std::make_unique<juce::AudioParameterFloat>( "LowCut Freq",
                                                            "LowCut Freq",
                                                            juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f),
                                                            20.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>( "HighCut Freq",
                                                            "HighCut Freq",
                                                            juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f),
                                                            20000.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>( "Peak Freq",
                                                            "Peak Freq",
                                                            juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f),
                                                            750.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>( "Peak Gain",
                                                            "Peak Gain",
                                                            juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
                                                            0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Peak Quality",
                                                            "Peak Quality",
                                                            juce::NormalisableRange<float>(-0.1f, 10.f, 0.05f, 1.f),
                                                            1.f));

    juce::StringArray stringArray;
    for (int i = 0; i < 4; ++i) {
        juce::String str;
        str << (12 + (12 * i));
        str << " db/Oct";
        stringArray.add(str);
    }

    layout.add(std::make_unique<juce::AudioParameterChoice>("LowCut Slope",
                                                            "LowCut Slope",
                                                            stringArray,
                                                            0));
    layout.add(std::make_unique<juce::AudioParameterChoice>("HighCut Slope",
                                                            "HighCut Slope",
                                                            stringArray,
                                                            0));*/
    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Synth_JUCEAudioProcessor();
}
