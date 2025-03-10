#pragma once

#include <memory>
#include <JuceHeader.h>
#include "WaveGenerator.h"
#include "Oscillator.h"
#include "GlobalPitch.h"
#include "TreeLabels.h"

const int NUM_OSC = 4;

class GenericSound final : public juce::SynthesiserSound
{
public:
    GenericSound() {}

    bool appliesToNote(int /*midiNoteNumber*/) override { return true; }
    bool appliesToChannel(int /*midiChannel*/) override { return true; }
};

//==============================================================================
/** A synth voice that can produce any waveform from WAVE_TYPES. */
class GenericVoice final : public juce::SynthesiserVoice
{
public:
    GenericVoice(juce::AudioProcessorValueTreeState& apvts) {
        for (auto i = 0; i < NUM_OSC; i++) {
            osc.push_back(std::make_unique<Oscillator>(apvts, i, getSampleRate()));
        }

        fmMode = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(FM_MODE_ID));
        jassert(fmMode);
    }

    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<GenericSound*> (sound) != nullptr;
    }

    void startNote(int midiNoteNumber, float velocity,
        juce::SynthesiserSound* /*sound*/,
        int /*currentPitchWheelPosition*/) override
    {
        velocityLevel = velocity * 0.5f;
        globalPitch.noteOn(midiNoteNumber);
        for (const auto& o : osc) {
            o->startNote(velocityLevel, globalPitch.getFundamentalFreq(), getSampleRate());
        }
    }

    void clearNote() {
        clearCurrentNote();
        for (auto& o : osc) {
            o->clearNote();
        }
    }

    void stopNote(float /*velocity*/, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            // start a tail-off by setting this flag. The render callback will pick up on
            // this and do a fade out, calling clearCurrentNote() when it's finished.
            globalPitch.noteOff();
            for (auto& o : osc) {
                o->noteOff();
            }
        }
        else
        {
            // we're being told to stop playing immediately, so reset everything..
            clearNote();
        }
    }

    void pitchWheelMoved(int /*newValue*/) override
    {
        // not implemented for the purposes of this demo!
    }

    void controllerMoved(int /*controllerNumber*/, int /*newValue*/) override
    {
        // not implemented for the purposes of this demo!
    }

    bool anyEnvelopeActive() {
        for (auto& o : osc) {
            if (o->isActive())
                return true;
        }
        return false;
    }

    bool allAngleDeltaEqualZero() {
        for (auto& o : osc) {
            if (!o->angleApproxZero())
                return false;
        }
        return true;
    }

    float calculateFreqOffsetFromSample(float sample) {
        // convert sample to some frequency between -20kHz and +20kHz
        return sample * 20000.0f;
    }

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        // Trigger envelope param set for oscillators
        setEnvelopeParams();
        if (!allAngleDeltaEqualZero())
        {
            while (--numSamples >= 0)
            {
                // Update fundamental for each sample
                float fundamentalFreq = globalPitch.getFundamentalFreq();
                if (anyEnvelopeActive()) {
                    float summedSample = 0.0f;
                    if (fmMode->get()) {
                        // FM Chain in FM mode
                        auto chain1Sample = osc[0]->generateSample(fundamentalFreq);
                        summedSample += osc[1]->generateSample(fundamentalFreq + calculateFreqOffsetFromSample(chain1Sample));

                        auto chain2Sample = osc[2]->generateSample(fundamentalFreq);
                        summedSample += osc[3]->generateSample(fundamentalFreq + calculateFreqOffsetFromSample(chain2Sample));
                    }
                    else {
                        // Non-FM is additive mode
                        for (auto& o : osc) {
                            summedSample += o->generateSample(fundamentalFreq);
                        }
                    }
                    // Populate output buffer
                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, summedSample);
                    ++startSample;
                }
                else {
                    clearNote();
                    break;

                }
            }
        }
    }

    void setEnvelopeSampleRate(double sampleRate) {
        for (auto& o : osc) {
            o->setEnvelopeSampleRate(sampleRate);
        }
    }

    static std::unique_ptr<juce::AudioProcessorParameterGroup> createOscillatorParameterLayoutGroup(
        juce::String oscId,
        juce::String wavetypeId,
        juce::String gainId,
        juce::String coarsePitchId,
        juce::String finePitchId,
        juce::String delayId,
        juce::String attackId,
        juce::String decayId,
        juce::String sustainId,
        juce::String releaseId)
    {
        auto wavetype = std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ wavetypeId,  1 },
            "Waveform",
            juce::StringArray{ "Sine", "Square", "Triangle", "Saw Analog", "Noise" },
            0);
        auto gain = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ gainId,  1 },
            "Gain",
            juce::NormalisableRange<float>(0.0f, 1.0f),
            0.0f);
        auto coarsePitch = std::make_unique<juce::AudioParameterInt>(juce::ParameterID{ coarsePitchId,  1 },
            "Coarse Pitch",
            -12, 12, 0);
        auto finePitch = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ finePitchId,  1 },
            "Fine Pitch",
            juce::NormalisableRange<float>(0.0f, 1000.0f, 1.0f, 0.35f),
            0.0f);

        // Oscillator envelope controls
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

        auto group = std::make_unique<juce::AudioProcessorParameterGroup>(oscId, oscId, "|",
            std::move(wavetype),
            std::move(gain),
            std::move(coarsePitch),
            std::move(finePitch),
            std::move(attack),
            std::move(decay),
            std::move(sustain),
            std::move(release)
            );
        return group;
    }

    void setEnvelopeParams() {
        for (auto& o : osc) {
            o->setEnvelopeParams();
        }
    }

    using SynthesiserVoice::renderNextBlock;

private:
    float velocityLevel = 0.0f;

    // Each voice plays a different MIDI note, thus has a different pitch.
    // They'll use the same parameters aside from their input midi note
    GlobalPitch globalPitch;
    juce::AudioParameterBool* fmMode = nullptr;
    std::vector<std::unique_ptr<Oscillator>> osc;
};