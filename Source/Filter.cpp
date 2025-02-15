#include "Filter.h"

Filter::Filter(juce::AudioProcessorValueTreeState& apvts)
    : apvts(apvts),
    lfo(apvts, FILTER_LFO_AMOUNT_ID, FILTER_LFO_FREQ_ID, FILTER_LFO_WAVETYPE_ID)
{

    // ====================== FILTER ================================
    audioParams.filterType = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(FILTER_TYPE_ID));
    jassert(audioParams.filterType);
    audioParams.cutoffFreq = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(CUTOFF_FREQ_ID));
    jassert(audioParams.cutoffFreq);
    audioParams.q = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(Q_ID));
    jassert(audioParams.q);
    audioParams.resonance = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(RESONANCE_ID));
    jassert(audioParams.resonance);

    // ====================== ENVELOPE ================================
    audioParams.adsrAmount = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(FILTER_ADSR_AMOUNT_ID));
    jassert(audioParams.adsrAmount);
    audioParams.attack = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(FILTER_ATTACK_ID));
    jassert(audioParams.attack);
    audioParams.decay = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(FILTER_DECAY_ID));
    jassert(audioParams.decay);
    audioParams.sustain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(FILTER_SUSTAIN_ID));
    jassert(audioParams.sustain);
    audioParams.release = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(FILTER_RELEASE_ID));
    jassert(audioParams.release);

}

void Filter::prepareToPlay(double newSampleRate, int samplesPerBlock)
{
    sampleRate = newSampleRate;
    lfo.prepareToPlay(newSampleRate);

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1; // spec for a mono channel
    spec.sampleRate = newSampleRate;
    envelope.setSampleRate(newSampleRate);

    leftChain.prepare(spec);
    rightChain.prepare(spec);
}

std::unique_ptr<juce::AudioProcessorParameterGroup> Filter::createFilterParameterLayoutGroup(
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
    juce::String wavetypeId)
{
    // ====================== FILTER ================================
    auto filterType = std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ filterTypeId,  1 },
        "Filter Type",
        juce::StringArray{ "Low Pass", "High Pass", "Band Pass", "Peak" },
        0);
    auto cutoff = std::make_unique<juce::AudioParameterFloat>(cutoffId,
        "Cutoff Freq",
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.35f),
        20.f);
    auto q = std::make_unique<juce::AudioParameterFloat>(qId,
        "Quality",
        juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f),
        1.f);
    auto resonance = std::make_unique<juce::AudioParameterFloat>(resonanceId,
        "Resonance",
        juce::NormalisableRange<float>(-0.1f, 10.f, 0.05f, 1.f),
        1.f);

    // ====================== ENVELOPE ================================
    auto adsrAmount = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ adsrAmountId,  1 },
        "Amount",
        juce::NormalisableRange<float>(-1.0f, 1.0f, 0.001f),
        0.0f);
    auto attack = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ attackId,  1 },
        "Attack",
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.001f, 0.3f),
        0.0f);
    auto decay = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ decayId,  1 },
        "Decay",
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.001f, 0.3f),
        0.0f);
    auto sustain = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ sustainId,  1 },
        "Sustain Level",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
        1.0f);
    auto release = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ releaseId,  1 },
        "Release",
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.001f, 0.3f),
        0.0f);

    // ====================== LFO ================================
    auto lfoAmount = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ lfoAmountId,  1 },
        "Amount",
        juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f),
        0.0f);
    auto lfoFreq = std::make_unique<juce::AudioParameterFloat>(lfoFreqId,
        "Freq",
        juce::NormalisableRange<float>(0.1f, 100.f, 0.1f, 0.35f),
        1.f);
    auto wavetype = std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ wavetypeId,  1 },
        "Waveform",
        juce::StringArray{ "Sine", "Square", "Triangle", "Saw Analog", "Noise" },
        0);

    auto group = std::make_unique<juce::AudioProcessorParameterGroup>("Filter", "Filter", "|",
        std::move(filterType),
        std::move(cutoff),
        std::move(q),
        std::move(resonance),
        std::move(adsrAmount),
        std::move(attack),
        std::move(decay),
        std::move(sustain),
        std::move(release),
        std::move(lfoAmount),
        std::move(lfoFreq),
        std::move(wavetype)
        );
    return group;
}

void Filter::setEnvelopeParams()
{
    envelopeParams.attack = audioParams.attack->get();
    envelopeParams.decay = audioParams.decay->get();
    envelopeParams.sustain = audioParams.sustain->get();
    envelopeParams.release = audioParams.release->get();
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
    lfo.noteOn();
}

void Filter::noteOff()
{
    envelope.noteOff();
}

void Filter::process(juce::AudioBuffer<float>& buffer, int startSample)
{
    // Push input coefficients to filters before processing
    setEnvelopeParams();
    setFilters(buffer.getNumSamples());

    juce::dsp::AudioBlock<float> block(buffer, startSample);
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);
}

void Filter::setFilters(int numSamples)
{
    // Modulate frequency with envelope value
    auto modulatedFreq = getModulatedCutoffFreq(numSamples);

    auto lowPassCoefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, modulatedFreq, audioParams.q->get());
    auto highPassCoefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, modulatedFreq, audioParams.q->get());
    auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, modulatedFreq, audioParams.q->get(),
        juce::Decibels::decibelsToGain(audioParams.resonance->get()));

    switch (getFilterType()) {
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

FilterType Filter::getFilterType()
{
    FilterType f = static_cast<FilterType>(audioParams.filterType->getIndex()+1);
    return f;
}

float Filter::getEnvelopeValue(int numSamples)
{
    for (auto i = 0; i < numSamples; i++) {
        envelope.getNextSample();
    }
    return envelope.getNextSample();
}

float Filter::getModulatedCutoffFreq(int numSamples)
{
    // Filter logic:
    // When adsrAmount = +1
    // freq follows env on positive slope starting from cutoffFreq, ceiling freq of 20k
    float maxFreqRange = 20000.0f;
    float maxLFOAmount = 5000.0f;

    auto lfoOffset = lfo.generateSample(numSamples) * maxLFOAmount;
    auto adsrOffset = audioParams.adsrAmount->get() * (getEnvelopeValue(numSamples) * maxFreqRange);
    float modulatedFreq = audioParams.cutoffFreq->get() + adsrOffset + lfoOffset;

    modulatedFreq = std::min(modulatedFreq, 20000.0f);
    modulatedFreq = std::max(20.0f, modulatedFreq);

    return modulatedFreq;
}
