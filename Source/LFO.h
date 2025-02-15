#pragma once

#include "WaveGenerator.h"
#include "OscillatorPitch.h"

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
	struct LFOParams {
		juce::AudioParameterFloat* knobLevel = nullptr;
		juce::AudioParameterFloat* freq = nullptr;
		juce::AudioParameterChoice* waveType = nullptr;
	};

	OscillatorPitch pitch;
	LFOParams audioParams;

	double sampleRate;
};