#pragma once

#include <JuceHeader.h>
#include "TreeLabels.h"
#include "Oscillator.h"
#include "LFO.h"

enum class FilterType {
	NONE, // Placeholder for 1-justified selection box behavior
	LOWPASS,
	HIGHPASS,
	BANDPASS,
	PEAK
};

enum class FilterChain {
	HIGHPASS,
	PEAK,
	LOWPASS
};

class Filter {
public:
	Filter(juce::AudioProcessorValueTreeState& apvts);
	void prepareToPlay(double newSampleRate, int samplesPerBlock);

	static std::unique_ptr<juce::AudioProcessorParameterGroup> createFilterParameterLayoutGroup(
		juce::String filterTypeId,
		juce::String cutoffId,
		juce::String qId,
		juce::String resonanceId,
		juce::String adsrAmountId,
		juce::String attackId,
		juce::String decayId,
		juce::String sustainId,
		juce::String releaseId,
		juce::String lfoAmountId,
		juce::String lfoFreqId,
		juce::String wavetypeId);
	void setEnvelopeParams();

	void startNote();
	void noteOn();
	void noteOff();

	void process(juce::AudioBuffer<float>& buffer, int startSample);
private:
	void setFilters(int numSamples);
	void resetFilters();
	void bypassPeak(bool bypass);
	void bypassLowCut(bool bypass);
	void bypassHighCut(bool bypass);
	void setProcessorBypass(bool highPassBypass, bool peakBypass, bool lowPassBypass);

	FilterType getFilterType();
	float getModulatedCutoffFreq(int numSamples);

	// ====================== FILTER ================================
	using IIRFilter = juce::dsp::IIR::Filter<float>;
	using CutFilter = juce::dsp::ProcessorChain<IIRFilter, IIRFilter, IIRFilter, IIRFilter>;
	using MonoChain = juce::dsp::ProcessorChain<IIRFilter, IIRFilter, IIRFilter>;
	MonoChain leftChain, rightChain;

	bool paramsUpdated = true;
	double sampleRate;
	juce::AudioParameterChoice* filterType;
	juce::AudioParameterFloat* cutoffFreq = nullptr;
	juce::AudioParameterFloat* q = nullptr;
	juce::AudioParameterFloat* resonance = nullptr;

	// ====================== ENVELOPE ================================
	juce::ADSR envelope;
	juce::AudioParameterFloat* adsrAmount = nullptr;
	juce::AudioParameterFloat* attack = nullptr;
	juce::AudioParameterFloat* decay = nullptr;
	juce::AudioParameterFloat* sustain = nullptr;
	juce::AudioParameterFloat* release = nullptr;
	juce::ADSR::Parameters envelopeParams;


	// ====================== LFO ================================
	LFO lfo;

	juce::AudioProcessorValueTreeState& apvts;
};