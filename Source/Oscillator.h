#pragma once

#include <JuceHeader.h>
#include "WaveGenerator.h"
#include "OscillatorPitch.h"

class Oscillator {
public:
	Oscillator(juce::AudioProcessorValueTreeState& apvts, int initId, double sampleRate);

	void startNote(float velocityLevel, float freq, double sampleRate);
	void noteOn();
	void noteOff();
	void clearNote();
	WAVE_TYPE getWaveType();
	void setEnvelopeSampleRate(double sampleRate);
	void setEnvelopeParams();

	bool isActive();
	int getId();

	bool angleApproxZero();
	float generateSample(float fundamentalFreq);
private:
	struct OscillatorParams {
		juce::AudioParameterChoice* waveType = nullptr;
		juce::AudioParameterFloat* knobLevel = nullptr;
		juce::AudioParameterInt* coarsePitchParam = nullptr;
		juce::AudioParameterFloat* finePitchParam = nullptr;

		juce::AudioParameterFloat* attack = nullptr;
		juce::AudioParameterFloat* decay = nullptr;
		juce::AudioParameterFloat* sustain = nullptr;
		juce::AudioParameterFloat* release = nullptr;
	};
	int id;
	bool fixedFreq = false;
	
	float velocityLevel;

	OscillatorPitch pitch;

	// GUI Parameters
	OscillatorParams audioParams;

	juce::ADSR envelope;
	juce::ADSR::Parameters envelopeParams;
};