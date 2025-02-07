#include "LFO.h"

LFO::LFO(juce::AudioProcessorValueTreeState& apvts,
	juce::String lfoAmountId,
	juce::String lfoFreqId,
	juce::String lfoWavetypeId)
{
	freq = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(lfoFreqId));
	jassert(freq);
	knobLevel = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(lfoAmountId));
	jassert(knobLevel);
	waveType = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(lfoWavetypeId));
	jassert(waveType);
}

WAVE_TYPE LFO::getWaveType()
{
	WAVE_TYPE wt = static_cast<WAVE_TYPE>(waveType->getIndex() + 1);
	return wt;
}

void LFO::prepareToPlay(double sr)
{
	sampleRate = sr;
	pitch.setSampleRate(sampleRate);
}

void LFO::noteOn() {
	pitch.noteOn(freq->get(), sampleRate);
}

double LFO::generateSample(int numSamples)
{
	// lfo is only updated once per processed block, so needs a frequency multiplier to be accurate
	double currentAngle = pitch.getNextSample(freq->get() * numSamples);
	return getWaveformSample(getWaveType(), currentAngle) * knobLevel->get();
}