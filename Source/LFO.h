#pragma once

#include "WaveGenerator.h"
#include "Pitch.h"

class LFO {
public:
	LFO(juce::AudioProcessorValueTreeState& apvts,
		juce::String lfoAmountId,
		juce::String lfoFreqId,
		juce::String lfoWavetypeId
	);
	WAVE_TYPE getWaveType();
	void prepareToPlay(double sampleRate);
	void noteOn();
	double generateSample(int numSamples);
private:
	Pitch pitch;

	double sampleRate;

	juce::AudioParameterFloat* knobLevel = nullptr;
	juce::AudioParameterFloat* freq = nullptr;
	juce::AudioParameterChoice* waveType = nullptr;
};