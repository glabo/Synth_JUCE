#pragma once

#include <JuceHeader.h>
#include "WaveGenerator.h"

class Oscillator {
public:
	Oscillator(int initId = 0);

	void startNote(double velocityLevel, double cyclesPerSecond);
	void noteOn();
	void noteOff();
	void setWaveType(WAVE_TYPE newWaveType);
	void setEnvelopeSampleRate(double sampleRate);
	void linkEnvelopeParams(float level, std::atomic<float>* attack, std::atomic<float>* decay, std::atomic<float>* sustain, std::atomic<float>* release);
	void setEnvelopeParams();

	bool isActive();
	double generateSample(double currentAngle, double cyclesPerSecond);
private:
	int id;
	bool fixedFreq = false;
	
	double velocityLevel;
	double cyclesPerSecond;
	WAVE_TYPE waveType = SINE;

	float knobLevel;
	juce::ADSR envelope;
	juce::ADSR::Parameters envelopeParams;
};