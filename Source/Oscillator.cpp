#include "Oscillator.h"

Oscillator::Oscillator(juce::AudioProcessorValueTreeState& apvts, int initId, double sampleRate) {
	id = initId;
	velocityLevel = 0.0;
	pitch.setSampleRate(sampleRate);
	envelope.setSampleRate(sampleRate);

	auto wavetypeId = "wavetype_" + std::to_string(initId);
	waveType = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(wavetypeId));
	jassert(waveType);
	auto knobLevelId = "gain_" + std::to_string(initId);
	knobLevel = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(knobLevelId));
	jassert(knobLevel);
	auto coarsePitchId = "coarse_pitch_" + std::to_string(initId);
	coarsePitchParam = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(coarsePitchId));
	jassert(coarsePitchParam);
	auto finePitchId = "fine_pitch_" + std::to_string(initId);
	finePitchParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(finePitchId));
	jassert(finePitchParam);

	auto attackId = "attack_" + std::to_string(initId);
	attack = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(attackId));
	jassert(attack);
	auto decayId = "decay_" + std::to_string(initId);
	decay = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(decayId));
	jassert(decay);
	auto sustainId = "sustain_" + std::to_string(initId);
	sustain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(sustainId));
	jassert(sustain);
	auto releaseId = "release_" + std::to_string(initId);
	release = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(releaseId));
	jassert(release);

}

void Oscillator::startNote(double velocity, double freq, double sampleRate) {
	velocityLevel = velocity;
	envelope.noteOn();
	// Initialize pitch so angleDelta is calculated and oscillators are "triggered"
	// I don't like this
	pitch.noteOn(freq, sampleRate);
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

WAVE_TYPE Oscillator::getWaveType()
{
	WAVE_TYPE wt = static_cast<WAVE_TYPE>(waveType->getIndex() + 1);
	return wt;
}

void Oscillator::setEnvelopeSampleRate(double sampleRate) {
	envelope.setSampleRate(sampleRate);
}

void Oscillator::setEnvelopeParams() {
	pitch.setPitchShift(coarsePitchParam->get(), finePitchParam->get());
	envelopeParams.attack = attack->get();
	envelopeParams.decay = decay->get();
	envelopeParams.sustain = sustain->get();
	envelopeParams.release = release->get();
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

double Oscillator::generateSample(float fundamentalFreq)
{
	setEnvelopeParams();
	if (isActive()) {
		float envelopeLevel = envelope.getNextSample();
		double currentAngle = pitch.getNextSample(fundamentalFreq);
		return getWaveformSample(getWaveType(), currentAngle) * velocityLevel * envelopeLevel * knobLevel->get();
	}
	else {
		return 0.0;
	}
}
