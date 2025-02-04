#pragma once

#include <JuceHeader.h>
#include "TreeLabels.h"

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
		juce::String releaseId);
	void setEnvelopeParams();

	void startNote();
	void noteOn();
	void noteOff();

	void process(juce::AudioBuffer<float>& buffer, int startSample);
private:
	void setFilters();
	void resetFilters();
	void bypassPeak(bool bypass);
	void bypassLowCut(bool bypass);
	void bypassHighCut(bool bypass);
	void setProcessorBypass(bool highPassBypass, bool peakBypass, bool lowPassBypass);

	FilterType getFilterType();
	float getModulatedCutoffFreq();

	using IIRFilter = juce::dsp::IIR::Filter<float>;
	using CutFilter = juce::dsp::ProcessorChain<IIRFilter, IIRFilter, IIRFilter, IIRFilter>;
	// Mono signal path: LowCut Filter --> Peak Filter --> HighCut Filter
	//using MonoChain = juce::dsp::ProcessorChain<CutFilter, IIRFilter, CutFilter>;
	using MonoChain = juce::dsp::ProcessorChain<IIRFilter, IIRFilter, IIRFilter>;
	MonoChain leftChain, rightChain;

	bool paramsUpdated = true;
	double sampleRate;
	juce::AudioParameterChoice* filterType;
	juce::AudioParameterFloat* cutoffFreq = nullptr;
	juce::AudioParameterFloat* q = nullptr;
	juce::AudioParameterFloat* resonance = nullptr;

	juce::dsp::IIR::Filter<float> filter;

	juce::ADSR envelope;
	juce::AudioParameterFloat* adsrAmount = nullptr;
	juce::AudioParameterFloat* attack = nullptr;
	juce::AudioParameterFloat* decay = nullptr;
	juce::AudioParameterFloat* sustain = nullptr;
	juce::AudioParameterFloat* release = nullptr;
	juce::ADSR::Parameters envelopeParams;

	juce::AudioProcessorValueTreeState& apvts;
};