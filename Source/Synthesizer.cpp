#include "Synthesizer.h"

Synthesizer::Synthesizer(juce::AudioProcessorValueTreeState& apvts)
    : filter(apvts)
{
}

void Synthesizer::prepareToPlay(double newSampleRate, int samplesPerBlock)
{
    juce::Synthesiser::setCurrentPlaybackSampleRate(newSampleRate);

    // Prep filter chains
    filter.prepareToPlay(newSampleRate, samplesPerBlock);
}

void Synthesizer::render(juce::AudioBuffer<float>& outputAudio, const juce::MidiBuffer& inputMidi, int startSample, int numSamples)
{
    juce::Synthesiser::renderNextBlock(outputAudio, inputMidi, startSample, numSamples);
    filter.process(outputAudio, startSample);
}

void Synthesizer::createAndAddOscillatorParameterLayouts(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    auto group = GenericVoice::createOscillatorParameterLayoutGroup(
        OSCILLATOR_0_ID,
        WAVETYPE_ID_0,
        GAIN_ID_0,
        COARSE_PITCH_ID_0,
        FINE_PITCH_ID_0,
        DELAY_ID_0,
        ATTACK_ID_0,
        DECAY_ID_0,
        SUSTAIN_ID_0,
        RELEASE_ID_0
    );
    layout.add(std::move(group));
    group = GenericVoice::createOscillatorParameterLayoutGroup(
        OSCILLATOR_1_ID,
        WAVETYPE_ID_1,
        GAIN_ID_1,
        COARSE_PITCH_ID_1,
        FINE_PITCH_ID_1,
        DELAY_ID_1,
        ATTACK_ID_1,
        DECAY_ID_1,
        SUSTAIN_ID_1,
        RELEASE_ID_1
    );
    layout.add(std::move(group));
    group = GenericVoice::createOscillatorParameterLayoutGroup(
        OSCILLATOR_2_ID,
        WAVETYPE_ID_2,
        GAIN_ID_2,
        COARSE_PITCH_ID_2,
        FINE_PITCH_ID_2,
        DELAY_ID_2,
        ATTACK_ID_2,
        DECAY_ID_2,
        SUSTAIN_ID_2,
        RELEASE_ID_2
    );
    layout.add(std::move(group));
    group = GenericVoice::createOscillatorParameterLayoutGroup(
        OSCILLATOR_3_ID,
        WAVETYPE_ID_3,
        GAIN_ID_3,
        COARSE_PITCH_ID_3,
        FINE_PITCH_ID_3,
        DELAY_ID_3,
        ATTACK_ID_3,
        DECAY_ID_3,
        SUSTAIN_ID_3,
        RELEASE_ID_3
    );
    layout.add(std::move(group));
}

void Synthesizer::createAndAddFilterParameterLayouts(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    auto group = Filter::createFilterParameterLayoutGroup(
        FILTER_TYPE_ID,
        CUTOFF_FREQ_ID,
        Q_ID,
        RESONANCE_ID,
        FILTER_ADSR_AMOUNT_ID,
        FILTER_ATTACK_ID,
        FILTER_DECAY_ID,
        FILTER_SUSTAIN_ID,
        FILTER_RELEASE_ID);
    layout.add(std::move(group));
}

void Synthesizer::createParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // Oscillator parameter layouts
    createAndAddOscillatorParameterLayouts(layout);
    // Filter parameter layouts
    createAndAddFilterParameterLayouts(layout);
}

void Synthesizer::noteOn(int midiChannel, int midiNoteNumber, float velocity)
{
    juce::Synthesiser::noteOn(midiChannel, midiNoteNumber, velocity);
    filter.noteOn();
}

void Synthesizer::noteOff(int midiChannel, int midiNoteNumber, float velocity, bool allowTailOff)
{
    juce::Synthesiser::noteOff(midiChannel, midiNoteNumber, velocity, allowTailOff);
    filter.noteOff();
}

void Synthesizer::processFilter(juce::AudioBuffer<float>& buffer, int startSample)
{
    filter.process(buffer, startSample);
}
