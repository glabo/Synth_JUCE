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

    // Initial filter setup
    setFilters();
}

void Filter::setFilterType(FilterType ft)
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
    // live update filter state
    setFilters();

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

void Filter::setFilters()
{
    auto lowPassCoefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, cutoffFreq, q);
    auto highPassCoefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, cutoffFreq, q);
    auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, cutoffFreq, q,
        juce::Decibels::decibelsToGain(resonance));

    switch (filterType) {
    case FilterType::LOWPASS:
        *leftChain.get<int(FilterChain::LOWPASS)>().coefficients = *lowPassCoefficients;
        *rightChain.get<int(FilterChain::LOWPASS)>().coefficients = *lowPassCoefficients;
        setProcessorBypass(true, true, false);
        break;
    case FilterType::HIGHPASS:
        *leftChain.get<int(FilterChain::HIGHPASS)>().coefficients = *highPassCoefficients;
        *rightChain.get<int(FilterChain::HIGHPASS)>().coefficients = *highPassCoefficients;
        setProcessorBypass(false, true, true);
        break;
    case FilterType::BANDPASS:
        *leftChain.get<int(FilterChain::LOWPASS)>().coefficients = *lowPassCoefficients;
        *rightChain.get<int(FilterChain::LOWPASS)>().coefficients = *lowPassCoefficients;
        *leftChain.get<int(FilterChain::HIGHPASS)>().coefficients = *highPassCoefficients;
        *rightChain.get<int(FilterChain::HIGHPASS)>().coefficients = *highPassCoefficients;
        setProcessorBypass(false, true, false);
        break;
    case FilterType::PEAK:
        *leftChain.get<int(FilterChain::PEAK)>().coefficients = *peakCoefficients;
        *rightChain.get<int(FilterChain::PEAK)>().coefficients = *peakCoefficients;
        setProcessorBypass(true, false, true);
        break;
    }
}

void Filter::bypassPeak(bool bypass)
{
    leftChain.setBypassed<int(FilterChain::PEAK)>(bypass);
    rightChain.setBypassed<int(FilterChain::PEAK)>(bypass);
}

void Filter::bypassLowCut(bool bypass)
{
    leftChain.setBypassed<int(FilterChain::LOWPASS)>(bypass);
    rightChain.setBypassed<int(FilterChain::LOWPASS)>(bypass);
}

void Filter::bypassHighCut(bool bypass)
{
    leftChain.setBypassed<int(FilterChain::HIGHPASS)>(bypass);
    rightChain.setBypassed<int(FilterChain::HIGHPASS)>(bypass);
}

void Filter::setProcessorBypass(bool highCutBypass, bool peakBypass, bool lowCutBypass)
{
    bypassPeak(peakBypass);
    bypassLowCut(lowCutBypass);
    bypassHighCut(highCutBypass);
}
