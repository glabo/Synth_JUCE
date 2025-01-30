#pragma once

#include <JuceHeader.h>
#include "Filter.h"

class Synthesizer : public juce::Synthesiser
{
public:
	void prepareToPlay(double newSampleRate, int samplesPerBlock);
	void setFilterParams(std::atomic<float>* cutoffFreq,
							std::atomic<float>* q,
							std::atomic<float>* resonance,
							std::atomic<float>* adsrAmount,
							std::atomic<float>* attack,
							std::atomic<float>* decay,
							std::atomic<float>* sustain,
							std::atomic<float>* release);
	void setFilterType(FilterType filterType);
private:
	void noteOn(int midiChannel, int midiNoteNumber, float velocity) override;
	void noteOff(int midiChannel, int midiNoteNumber, float velocity, bool allowTailOff);
	void renderVoices(juce::AudioBuffer<float>& outputAudio, int startSample, int numSamples) override;

	Filter filter;
};