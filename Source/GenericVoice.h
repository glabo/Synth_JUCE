#pragma once

#include <JuceHeader.h>
#include "WaveGenerator.h"

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
    GenericVoice() {}

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
        envelope.noteOn();

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

            envelope.noteOff();
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

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        envelope.setParameters(envelopeParams);
        if (!juce::approximatelyEqual(angleDelta, 0.0))
        {
            while (--numSamples >= 0)
            {
                if (envelope.isActive()) {
                    float envelopeLevel = envelope.getNextSample();
                    auto currentSample = (float)(getWaveformSample(waveType, currentAngle, cyclesPerSecond) * velocityLevel * envelopeLevel);

                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, currentSample);

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

    void setWaveType(WAVE_TYPE newWaveType) {
        waveType = newWaveType;
    }

    void setEnvelopeSampleRate(double sampleRate) {
        envelope.setSampleRate(sampleRate);
    }

    void setEnvelopeParams(std::atomic<float>* attack, std::atomic<float>* decay, std::atomic<float>* sustain, std::atomic<float>* release)
    {
        envelopeParams.attack = *attack;
        envelopeParams.decay = *decay;
        envelopeParams.sustain = *sustain;
        envelopeParams.release = *release;
    }

    using SynthesiserVoice::renderNextBlock;

private:
    double cyclesPerSecond = 0.0;
    double currentAngle = 0.0;
    double angleDelta = 0.0;
    double velocityLevel = 0.0;
    WAVE_TYPE waveType = SINE;

    juce::ADSR envelope;
    juce::ADSR::Parameters envelopeParams;
};