#pragma once

#include <JuceHeader.h>
#include "Filter.h"
#include "TreeLabels.h"
#include "GenericVoice.h"

class Synthesizer : public juce::Synthesiser
{
public:
	Synthesizer(juce::AudioProcessorValueTreeState& apvts);
	void prepareToPlay(double newSampleRate, int samplesPerBlock);

	static void createAndAddOscillatorParameterLayouts(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
	static void createAndAddFilterParameterLayouts(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
	static void createParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
private:
	void noteOn(int midiChannel, int midiNoteNumber, float velocity) override;
	void noteOff(int midiChannel, int midiNoteNumber, float velocity, bool allowTailOff);
	void renderVoices(juce::AudioBuffer<float>& outputAudio, int startSample, int numSamples) override;

	Filter filter;
};