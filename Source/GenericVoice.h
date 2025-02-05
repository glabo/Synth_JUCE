#pragma once

#include <memory>
#include <JuceHeader.h>
#include "WaveGenerator.h"
#include "Oscillator.h"

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
    }

    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<GenericSound*> (sound) != nullptr;
    }

    void startNote(int midiNoteNumber, float velocity,
        juce::SynthesiserSound* /*sound*/,
        int /*currentPitchWheelPosition*/) override
    {
        velocityLevel = velocity * 0.15;
        for (const auto& o : osc) {
            o->startNote(velocityLevel, midiNoteNumber, getSampleRate());
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

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        // Trigger envelope param set for oscillators
        setEnvelopeParams();
        if (!allAngleDeltaEqualZero())
        {
            while (--numSamples >= 0)
            {
                if (anyEnvelopeActive()) {
                    float summedSample = 0.0f;
                    // Sum samples from each oscillator
                    for (auto& o : osc) {
                        summedSample += (float)o->generateSample();
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
        juce::String pitchId,
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
            0.9f);
        auto pitch = std::make_unique<juce::AudioParameterInt>(juce::ParameterID{ pitchId,  1 },
            "Pitch",
            -12, 12, 0);

        // Oscillator envelope controls
        auto attack = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ attackId,  1 },
            "Attack",
            juce::NormalisableRange<float>(0.0f, 10.0f, 0.0f, 0.2f),
            1.0f);
        auto decay = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ decayId,  1 },
            "Decay",
            juce::NormalisableRange<float>(0.0f, 10.0f, 0.0f, 0.2f),
            1.0f);
        auto sustain = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ sustainId,  1 },
            "Sustain Level",
            juce::NormalisableRange<float>(0.0f, 1.0f),
            1.0f);
        auto release = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ releaseId,  1 },
            "Release",
            juce::NormalisableRange<float>(0.0f, 10.0f, 0.0f, 0.2f),
            1.0f);

        auto group = std::make_unique<juce::AudioProcessorParameterGroup>(oscId, oscId, "|",
            std::move(wavetype),
            std::move(gain),
            std::move(pitch),
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
    double velocityLevel = 0.0;

    std::vector<std::unique_ptr<Oscillator>> osc;
};