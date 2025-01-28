#include "Filter.h"

void Filter::setFilterType(FILTER_TYPE filterType)
{
	this->filterType = filterType;
}

void Filter::setFilterParams(std::atomic<float>* cutoffFreq, std::atomic<float>* q, std::atomic<float>* resonance)
{
	this->cutoffFreq = *cutoffFreq;
	this->q = *q;
	this->resonance = *resonance;
}

double Filter::generateSample(double sample)
{
	return sample;
}
