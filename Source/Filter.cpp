#include "Filter.h"

void Filter::prepareToPlay(double newSampleRate, int samplesPerBlock)
{
    sampleRate = newSampleRate;

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1; // spec for a mono channel
    spec.sampleRate = newSampleRate;
    envelope.setSampleRate(newSampleRate);

    leftChain.prepare(spec);
    rightChain.prepare(spec);
}

void Filter::setFilterType(FilterType ft)
{
	filterType = ft;
    paramsUpdated = true;
}

void Filter::setFilterParams(std::atomic<float>* cof,
    std::atomic<float>* newQ,
    std::atomic<float>* res,
    std::atomic<float>* newAdsrAmount,
    std::atomic<float>* attack,
    std::atomic<float>* decay,
    std::atomic<float>* sustain,
    std::atomic<float>* release)
{
    if (cutoffFreq != *cof) {
        paramsUpdated = true;
        cutoffFreq = *cof;
    }
    if (q != *newQ) {
        paramsUpdated = true;
        q = *newQ;
    }
    if (resonance != *res) {
        paramsUpdated = true;
        resonance = *res;
    }

    adsrAmount = *newAdsrAmount;
    envelopeParams.attack = *attack;
    envelopeParams.decay = *decay;
    envelopeParams.sustain = *sustain;
    envelopeParams.release = *release;
    envelope.setParameters(envelopeParams);
}

void Filter::startNote()
{
    envelope.noteOn();
}

void Filter::noteOn()
{
    // Retrigger filter envelope on each new note, more intuitive
    envelope.reset();
    envelope.noteOn();
}

void Filter::noteOff()
{
    envelope.noteOff();
}

void Filter::process(juce::AudioBuffer<float>& buffer, int startSample)
{
    // Push input coefficients to filters before processing
    setFilters();

    juce::dsp::AudioBlock<float> block(buffer, startSample);
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);
}

void Filter::setFilters()
{
    // Modulate frequency with envelope value
    auto modulatedFreq = getModulatedCutoffFreq();

    auto lowPassCoefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, modulatedFreq, q);
    auto highPassCoefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, modulatedFreq, q);
    auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, modulatedFreq, q,
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

    paramsUpdated = false;
}

void Filter::resetFilters()
{
    // Claimed good practice? Resetting filters AFTER setting their values
    leftChain.get<int(FilterChain::LOWPASS)>().reset();
    rightChain.get<int(FilterChain::LOWPASS)>().reset();
    leftChain.get<int(FilterChain::PEAK)>().reset();
    rightChain.get<int(FilterChain::PEAK)>().reset();
    leftChain.get<int(FilterChain::HIGHPASS)>().reset();
    rightChain.get<int(FilterChain::HIGHPASS)>().reset();
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

float Filter::getModulatedCutoffFreq()
{
    // Filter logic:
    // When adsrAmount = +1
    // freq follows env on positive slope starting from cutoffFreq, ceiling freq of 20k
    float maxFreqRange = 20000.0f;

    float modulatedFreq = cutoffFreq + (adsrAmount * (envelope.getNextSample() * maxFreqRange));
    modulatedFreq = std::min(modulatedFreq, 20000.0f);
    modulatedFreq = std::max(0.0f, modulatedFreq);

    return modulatedFreq;
}
