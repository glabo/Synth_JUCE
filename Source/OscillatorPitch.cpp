#include "OscillatorPitch.h"
#include <JuceHeader.h>

void OscillatorPitch::calculateAngleDelta(float newFreq, double sr)
{
	freq = newFreq;
	// Coarse pitch calc
	applyCoarseFreqOffset();
	// Fine pitch calc
	applyFineFreqOffset();
	auto cyclesPerSample = freq / sr;
	angleDelta = cyclesPerSample * juce::MathConstants<double>::twoPi;
}

void OscillatorPitch::setSampleRate(double sr) {
	sampleRate = sr;
}

void OscillatorPitch::setPitchShift(int ps, float fine)
{
	coarsePitchShift = ps;
	finePitchShift = fine;
}

float OscillatorPitch::calculateFinePitchShiftInHz(float inFreq)
{
	// Fine pitch shift is the next octave mapped into 1000 (MAX_FINE_PITCH_SHIFT) steps
	// Next octave is 2x current freq, so distance in Hz from this octave to the next is inFreq
	float octaveRange = inFreq;
	return ((float)finePitchShift / (float)MAX_FINE_PITCH_SHIFT) * octaveRange;
}

void OscillatorPitch::noteOn(float newFreq, double sr)
{
	sampleRate = sr;
	calculateAngleDelta(newFreq, sr);
	currentAngle = 0.0;
}

void OscillatorPitch::clearNote()
{
	angleDelta = 0.0;
}

void OscillatorPitch::applyCoarseFreqOffset()
{
	// 1 octave up is 2x, 1 octave down is 0.5x, etc.
	float multiplier = (float)pow(2.0, coarsePitchShift / 12.0);
	freq = freq * multiplier;
}

void OscillatorPitch::applyFineFreqOffset()
{
	freq += calculateFinePitchShiftInHz(freq);
}

bool OscillatorPitch::angleApproxZero()
{
	return juce::approximatelyEqual(angleDelta, 0.0);
}

double OscillatorPitch::getNextSample(float newFreq)
{
	calculateAngleDelta(newFreq, sampleRate);
	currentAngle += angleDelta;
	return currentAngle;
}
