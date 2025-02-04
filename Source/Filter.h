#pragma once

#include <JuceHeader.h>

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
	void prepareToPlay(double newSampleRate, int samplesPerBlock);
	void setFilterType(FilterType ft);

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
	void setFilterParams(std::atomic<float>* cutoffFreq,
		std::atomic<float>* q,
		std::atomic<float>* resonance,
		std::atomic<float>* newAdsrAmount,
		std::atomic<float>* newAttack,
		std::atomic<float>* newDecay,
		std::atomic<float>* newSustain,
		std::atomic<float>* newRelease);

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

	float getModulatedCutoffFreq();

	using IIRFilter = juce::dsp::IIR::Filter<float>;
	using CutFilter = juce::dsp::ProcessorChain<IIRFilter, IIRFilter, IIRFilter, IIRFilter>;
	// Mono signal path: LowCut Filter --> Peak Filter --> HighCut Filter
	//using MonoChain = juce::dsp::ProcessorChain<CutFilter, IIRFilter, CutFilter>;
	using MonoChain = juce::dsp::ProcessorChain<IIRFilter, IIRFilter, IIRFilter>;
	MonoChain leftChain, rightChain;

	bool paramsUpdated = true;
	double sampleRate;
	float cutoffFreq;
	float q;
	float resonance;
	FilterType filterType;

	juce::dsp::IIR::Filter<float> filter;

	float adsrAmount = 0.0;
	juce::ADSR envelope;
	juce::ADSR::Parameters envelopeParams;
};