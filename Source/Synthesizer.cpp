#include "Synthesizer.h"

void Synthesizer::prepareToPlay(double newSampleRate, int samplesPerBlock)
{
    juce::Synthesiser::setCurrentPlaybackSampleRate(newSampleRate);

    // Prep filter chains
    filter.prepareToPlay(newSampleRate, samplesPerBlock);
}

void Synthesizer::setFilterParams(std::atomic<float>* cutoffFreq,
                                    std::atomic<float>* q,
                                    std::atomic<float>* resonance,
                                    std::atomic<float>* adsrAmount,
                                    std::atomic<float>* attack,
                                    std::atomic<float>* decay,
                                    std::atomic<float>* sustain,
                                    std::atomic<float>* release)
{
    filter.setFilterParams(cutoffFreq, q, resonance, adsrAmount, attack, decay, sustain, release);
}

void Synthesizer::setFilterType(FilterType filterType)
{
    filter.setFilterType(filterType);
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

void Synthesizer::renderVoices(juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    // Do the same thing as parent but apply filtering afterwards
    juce::Synthesiser::renderVoices(buffer, startSample, numSamples);
    filter.process(buffer);
}
