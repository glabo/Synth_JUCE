#include "Pitch.h"
#include <JuceHeader.h>

void Pitch::calculateAngleDelta()
{
	auto shiftedNote = currentMidiNote + coarsePitchShift;
	auto originalNoteInHz = juce::MidiMessage::getMidiNoteInHertz(shiftedNote);
	freq = originalNoteInHz + calculateFinePitchShiftInHz(originalNoteInHz);
	calculateAngleDelta(freq, sampleRate);
}

void Pitch::calculateAngleDelta(float freq, double sampleRate)
{
	auto cyclesPerSample = freq / sampleRate;
	angleDelta = cyclesPerSample * juce::MathConstants<double>::twoPi;
}

void Pitch::setSampleRate(double sr) {
	sampleRate = sr;
}

void Pitch::setPitchShift(int ps, int fine)
{
	coarsePitchShift = ps;
	finePitchShift = fine;
}

double Pitch::calculateFinePitchShiftInHz(double inFreq)
{
	// Fine pitch shift is the next octave mapped into 1000 (MAX_FINE_PITCH_SHIFT) steps
	// Next octave is 2x current freq, so distance in Hz from this octave to the next is inFreq
	double octaveRange = inFreq;
	return ((double)finePitchShift / (double)MAX_FINE_PITCH_SHIFT) * octaveRange;
}

void Pitch::noteOn(int midiNoteNumber, double sr)
{
	currentMidiNote = midiNoteNumber;
	sampleRate = sr;
	calculateAngleDelta();
	currentAngle = 0.0;
}

void Pitch::noteOn(float freq, double sr)
{
	sampleRate = sr;
	calculateAngleDelta(freq, sr);
	currentAngle = 0.0;
}

void Pitch::clearNote()
{
	angleDelta = 0.0;
}

bool Pitch::angleApproxZero()
{
	return juce::approximatelyEqual(angleDelta, 0.0);
}

double Pitch::getNextSample()
{
	calculateAngleDelta();
	currentAngle += angleDelta;
	return currentAngle;
}

double Pitch::getNextSample(double newFreq)
{
	calculateAngleDelta(newFreq, sampleRate);
	currentAngle += angleDelta;
	return currentAngle;
}
