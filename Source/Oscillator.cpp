#include "Oscillator.h"

Oscillator::Oscillator(int initId, double sampleRate) {
	id = initId;
	knobLevel = 0.0;
	velocityLevel = 0.0;
	pitch.setSampleRate(sampleRate);
	envelope.setSampleRate(sampleRate);
}

void Oscillator::startNote(double velocity, int midiNoteNumber) {
	velocityLevel = velocity;
	envelope.noteOn();
	pitch.noteOn(midiNoteNumber);
}

void Oscillator::noteOn() {
	envelope.noteOn();
}

// Only triggers envelope note off, doesn't touch pitch
void Oscillator::noteOff() {
	envelope.noteOff();
}

void Oscillator::clearNote()
{
	pitch.clearNote();
}

void Oscillator::setWaveType(WAVE_TYPE newWaveType) {
	waveType = newWaveType;
}

void Oscillator::setEnvelopeSampleRate(double sampleRate) {
	envelope.setSampleRate(sampleRate);
}

void Oscillator::pushEnvelopeParams(float level,
									int ps,
									std::atomic<float>* attack,
									std::atomic<float>* decay,
									std::atomic<float>* sustain,
									std::atomic<float>* release)
{
	knobLevel = level;
	pitch.setPitchShift(ps);
	envelopeParams.attack = *attack;
	envelopeParams.decay = *decay;
	envelopeParams.sustain = *sustain;
	envelopeParams.release = *release;
}

void Oscillator::setEnvelopeParams() {
	envelope.setParameters(envelopeParams);
}

bool Oscillator::isActive() {
	return envelope.isActive();
}

int Oscillator::getId()
{
	return id;
}

bool Oscillator::angleApproxZero()
{
	return pitch.angleApproxZero();
}

double Oscillator::generateSample()
{
	if (isActive()) {
		float envelopeLevel = envelope.getNextSample();
		double currentAngle = pitch.getNextSample();
		return getWaveformSample(waveType, currentAngle) * velocityLevel * envelopeLevel * knobLevel;
	}
	else {
		return 0.0;
	}
}
