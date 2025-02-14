#include "OscillatorPitch.h"
#include <JuceHeader.h>

void OscillatorPitch::calculateAngleDelta(float newFreq, double sampleRate)
{
	freq = newFreq;
	// Coarse pitch calc
	applyCoarseFreqOffset();
	// Fine pitch calc
	applyFineFreqOffset();
	auto cyclesPerSample = freq / sampleRate;
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

double OscillatorPitch::calculateFinePitchShiftInHz(double inFreq)
{
	// Fine pitch shift is the next octave mapped into 1000 (MAX_FINE_PITCH_SHIFT) steps
	// Next octave is 2x current freq, so distance in Hz from this octave to the next is inFreq
	double octaveRange = inFreq;
	return ((double)finePitchShift / (double)MAX_FINE_PITCH_SHIFT) * octaveRange;
}

void OscillatorPitch::noteOn(float freq, double sr)
{
	sampleRate = sr;
	calculateAngleDelta(freq, sr);
	currentAngle = 0.0;
}

void OscillatorPitch::clearNote()
{
	angleDelta = 0.0;
}

void OscillatorPitch::applyCoarseFreqOffset()
{
	// 1 octave up is 2x, 1 octave down is 0.5x, etc.
	double multiplier = pow(2.0, coarsePitchShift / 12.0);
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

double OscillatorPitch::getNextSample(double newFreq)
{
	calculateAngleDelta(newFreq, sampleRate);
	currentAngle += angleDelta;
	return currentAngle;
}
