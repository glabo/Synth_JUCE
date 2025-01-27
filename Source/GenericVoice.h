#pragma once

#include <memory>
#include <JuceHeader.h>
#include "WaveGenerator.h"
#include "Oscillator.h"

const int NUM_OSC = 2;

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
    GenericVoice() {
        for (auto i = 0; i < NUM_OSC; i++) {
            osc.push_back(std::make_unique<Oscillator>());
        }
    }
    GenericVoice(WAVE_TYPE waveType) {
        waveType = waveType;
        for (auto i = 0; i < NUM_OSC; i++) {
            osc.push_back(std::make_unique<Oscillator>());
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
        currentAngle = 0.0;
        velocityLevel = velocity * 0.15;
        for (auto& o : osc) {
            o->startNote(velocityLevel, cyclesPerSecond);
        }

        cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        auto cyclesPerSample = cyclesPerSecond / getSampleRate();

        angleDelta = cyclesPerSample * juce::MathConstants<double>::twoPi;
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

            clearCurrentNote();
            angleDelta = 0.0;
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

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        // Trigger envelope param set for oscillators
        setEnvelopeParams();
        if (!juce::approximatelyEqual(angleDelta, 0.0))
        {
            while (--numSamples >= 0)
            {
                if (anyEnvelopeActive()) {
                    float summedSample = 0.0f;
                    for (auto& o : osc) {
                        summedSample += (float)o->generateSample(currentAngle, cyclesPerSecond);
                    }
                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, summedSample);

                    currentAngle += angleDelta;
                    ++startSample;
                }
                else {
                    clearCurrentNote();
                    angleDelta = 0.0;
                    break;

                }
            }
        }
    }

    void setWaveType(int oscId, WAVE_TYPE newWaveType) {
        osc[oscId]->setWaveType(newWaveType);
        waveType = newWaveType;
    }

    void setEnvelopeSampleRate(double sampleRate) {
        for (auto& o : osc) {
            o->setEnvelopeSampleRate(sampleRate);
        }
    }

    void setEnvelopeParams() {
        for (auto& o : osc) {
            o->setEnvelopeParams();
        }
    }

    void linkEnvelopeParams(int oscId, float level, std::atomic<float>* attack, std::atomic<float>* decay, std::atomic<float>* sustain, std::atomic<float>* release)
    {
        osc[oscId]->linkEnvelopeParams(level, attack, decay, sustain, release);
    }

    using SynthesiserVoice::renderNextBlock;

private:
    double cyclesPerSecond = 0.0;
    double currentAngle = 0.0;
    double angleDelta = 0.0;
    double velocityLevel = 0.0;

    std::vector<std::unique_ptr<Oscillator>> osc;

    WAVE_TYPE waveType = SINE;

    juce::ADSR envelope;
};