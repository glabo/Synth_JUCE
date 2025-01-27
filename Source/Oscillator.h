#pragma once

#include <JuceHeader.h>
#include "WaveGenerator.h"
#include "Pitch.h"

class Oscillator {
public:
	Oscillator(int initId, double sampleRate);

	void startNote(double velocityLevel, int midiNoteNumber);
	void noteOn();
	void noteOff();
	void clearNote();
	void setWaveType(WAVE_TYPE newWaveType);
	void setEnvelopeSampleRate(double sampleRate);
	void pushEnvelopeParams(float level,
							int pitch,
							std::atomic<float>* attack,
							std::atomic<float>* decay,
							std::atomic<float>* sustain,
							std::atomic<float>* release);
	void setEnvelopeParams();

	bool isActive();
	int getId();

	bool angleApproxZero();
	double generateSample();
private:
	int id;
	bool fixedFreq = false;
	
	double velocityLevel;

	Pitch pitch;

	// GUI Parameters
	WAVE_TYPE waveType = SINE;
	float knobLevel;
	juce::ADSR envelope;
	juce::ADSR::Parameters envelopeParams;
};