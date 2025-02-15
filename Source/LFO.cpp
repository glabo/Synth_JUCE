#include "LFO.h"

LFO::LFO(juce::AudioProcessorValueTreeState& apvts,
	juce::String lfoAmountId,
	juce::String lfoFreqId,
	juce::String lfoWavetypeId)
{
	audioParams.freq = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(lfoFreqId));
	jassert(audioParams.freq);
	audioParams.knobLevel = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(lfoAmountId));
	jassert(audioParams.knobLevel);
	audioParams.waveType = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(lfoWavetypeId));
	jassert(audioParams.waveType);
}

WAVE_TYPE LFO::getWaveType()
{
	WAVE_TYPE wt = static_cast<WAVE_TYPE>(audioParams.waveType->getIndex() + 1);
	return wt;
}

void LFO::prepareToPlay(double sr)
{
	sampleRate = sr;
	pitch.setSampleRate(sampleRate);
}

void LFO::noteOn() {
	pitch.noteOn(audioParams.freq->get(), sampleRate);
}

double LFO::generateSample(int numSamples)
{
	// lfo is only updated once per processed block, so needs a frequency multiplier to be accurate
	double currentAngle = pitch.getNextSample(audioParams.freq->get() * numSamples);
	return getWaveformSample(getWaveType(), currentAngle) * audioParams.knobLevel->get();
}