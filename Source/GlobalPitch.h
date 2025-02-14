#pragma once
#include <JuceHeader.h>

class GlobalPitch {
public:
	void setSampleRate(double sampleRate);

	void noteOn(int midiNoteNumber);
	void noteOff();

	float getFundamentalFreq();
private:
	int currentMidiNote;
	double sampleRate;

	juce::ADSR envelope;
};