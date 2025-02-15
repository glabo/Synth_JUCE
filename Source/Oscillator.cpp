#include "Oscillator.h"

Oscillator::Oscillator(juce::AudioProcessorValueTreeState& apvts, int initId, double sampleRate) {
	id = initId;
	velocityLevel = 0.0;
	pitch.setSampleRate(sampleRate);
	envelope.setSampleRate(sampleRate);

	auto wavetypeId = "wavetype_" + std::to_string(initId);
	audioParams.waveType = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(wavetypeId));
	jassert(audioParams.waveType);
	auto knobLevelId = "gain_" + std::to_string(initId);
	audioParams.knobLevel = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(knobLevelId));
	jassert(audioParams.knobLevel);
	auto coarsePitchId = "coarse_pitch_" + std::to_string(initId);
	audioParams.coarsePitchParam = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(coarsePitchId));
	jassert(audioParams.coarsePitchParam);
	auto finePitchId = "fine_pitch_" + std::to_string(initId);
	audioParams.finePitchParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(finePitchId));
	jassert(audioParams.finePitchParam);

	auto attackId = "attack_" + std::to_string(initId);
	audioParams.attack = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(attackId));
	jassert(audioParams.attack);
	auto decayId = "decay_" + std::to_string(initId);
	audioParams.decay = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(decayId));
	jassert(audioParams.decay);
	auto sustainId = "sustain_" + std::to_string(initId);
	audioParams.sustain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(sustainId));
	jassert(audioParams.sustain);
	auto releaseId = "release_" + std::to_string(initId);
	audioParams.release = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(releaseId));
	jassert(audioParams.release);

}

void Oscillator::startNote(float velocity, float freq, double sampleRate) {
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
	WAVE_TYPE wt = static_cast<WAVE_TYPE>(audioParams.waveType->getIndex() + 1);
	return wt;
}

void Oscillator::setEnvelopeSampleRate(double sampleRate) {
	envelope.setSampleRate(sampleRate);
}

void Oscillator::setEnvelopeParams() {
	pitch.setPitchShift(audioParams.coarsePitchParam->get(), audioParams.finePitchParam->get());
	envelopeParams.attack = audioParams.attack->get();
	envelopeParams.decay = audioParams.decay->get();
	envelopeParams.sustain = audioParams.sustain->get();
	envelopeParams.release = audioParams.release->get();
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

float Oscillator::generateSample(float fundamentalFreq)
{
	setEnvelopeParams();
	if (isActive()) {
		float envelopeLevel = envelope.getNextSample();
		double currentAngle = pitch.getNextSample(fundamentalFreq);
		return getWaveformSample(getWaveType(), currentAngle) * velocityLevel * envelopeLevel * audioParams.knobLevel->get();
	}
	else {
		return 0.0;
	}
}
