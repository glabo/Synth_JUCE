#pragma once

#include <JuceHeader.h>

enum FILTER_TYPE {
	LOWPASS,
	HIGHPASS,
	BANDPASS
};

class Filter {
public:
	void setFilterType(FILTER_TYPE ft);
	void setFilterParams(std::atomic<float>* cutoffFreq, std::atomic<float>* q, std::atomic<float>* resonance);
	double generateSample(double sample);
private:
	float cutoffFreq;
	float q;
	float resonance;
	FILTER_TYPE filterType;

	juce::ADSR envelope;
};