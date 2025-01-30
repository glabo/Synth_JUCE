#include "Pitch.h"
#include <JuceHeader.h>

void Pitch::calculateAngleDelta()
{
	auto shiftedNote = currentMidiNote + pitchShift;
	freq = juce::MidiMessage::getMidiNoteInHertz(shiftedNote);
	auto cyclesPerSample = freq / sampleRate;
	angleDelta = cyclesPerSample * juce::MathConstants<double>::twoPi;
}

void Pitch::setSampleRate(double sr) {
	sampleRate = sr;
}

void Pitch::setPitchShift(int ps)
{
	pitchShift = ps;
}

void Pitch::noteOn(int midiNoteNumber, double sr)
{
	currentMidiNote = midiNoteNumber;
	sampleRate = sr;
	calculateAngleDelta();
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
