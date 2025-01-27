#include "Oscillator.h"

Oscillator::Oscillator(int initId) {
	id = initId;
	knobLevel = 0.0;
	velocityLevel = 0.0;
	cyclesPerSecond = 0.0;
}

void Oscillator::startNote(double velocity, double cps) {
	velocityLevel = velocity;
	cyclesPerSecond = cps;
	envelope.noteOn();
}

void Oscillator::noteOn() {
	envelope.noteOn();
}

void Oscillator::noteOff() {
	envelope.noteOff();
}

void Oscillator::setWaveType(WAVE_TYPE newWaveType) {
	waveType = newWaveType;
}

void Oscillator::setEnvelopeSampleRate(double sampleRate) {
	envelope.setSampleRate(sampleRate);
}

void Oscillator::linkEnvelopeParams(float level,
									int pitch,
									std::atomic<float>* attack,
									std::atomic<float>* decay,
									std::atomic<float>* sustain,
									std::atomic<float>* release)
{
	knobLevel = level;
	pitchShift = pitch;
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

int Oscillator::getPitchShift()
{
	return pitchShift;
}

double Oscillator::generateSample(double currentAngle, double cps)
{
	if (isActive()) {
		float envelopeLevel = envelope.getNextSample();
		return getWaveformSample(waveType, currentAngle, cps) * velocityLevel * envelopeLevel * knobLevel;
	}
	else {
		return 0.0;
	}
}
