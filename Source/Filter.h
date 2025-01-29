#pragma once

#include <JuceHeader.h>

enum FILTER_TYPE {
	LOWPASS,
	HIGHPASS,
	BANDPASS
};

class Filter {
public:
	void prepareToPlay(double newSampleRate, int samplesPerBlock);
	void setFilterType(FILTER_TYPE ft);
	void setFilterParams(std::atomic<float>* cutoffFreq, std::atomic<float>* q, std::atomic<float>* resonance);

	void process(juce::AudioBuffer<float>& buffer);
	double generateSample(double sample);
private:
	using IIRFilter = juce::dsp::IIR::Filter<float>;
	using CutFilter = juce::dsp::ProcessorChain<IIRFilter, IIRFilter, IIRFilter, IIRFilter>;
	// Mono signal path: LowCut Filter --> Peak Filter --> HighCut Filter
	using MonoChain = juce::dsp::ProcessorChain<CutFilter, IIRFilter, CutFilter>;
	MonoChain leftChain, rightChain;

	double sampleRate;
	float cutoffFreq;
	float q;
	float resonance;
	FILTER_TYPE filterType;

	juce::dsp::IIR::Filter<float> filter;

	juce::ADSR envelope;
};