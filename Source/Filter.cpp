#include "Filter.h"

void Filter::prepareToPlay(double newSampleRate, int samplesPerBlock)
{
    sampleRate = newSampleRate;

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1; // spec for a mono channel
    spec.sampleRate = newSampleRate;

    leftChain.prepare(spec);
    rightChain.prepare(spec);
}

void Filter::setFilterType(FILTER_TYPE ft)
{
	filterType = ft;
}

void Filter::setFilterParams(std::atomic<float>* cof, std::atomic<float>* newQ, std::atomic<float>* res)
{
	cutoffFreq = *cof;
	this->q = *newQ;
	this->resonance = *res;
}

void Filter::process(juce::AudioBuffer<float>& buffer)
{
    juce::dsp::AudioBlock<float> block(buffer);
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);
}

double Filter::generateSample(double sample)
{
	return sample;
}
