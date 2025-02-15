#include "GlobalPitch.h"

void GlobalPitch::setSampleRate(double sr)
{
	sampleRate = sr;
}

void GlobalPitch::noteOn(int midiNoteNumber)
{
	currentMidiNote = midiNoteNumber;
	envelope.noteOn();
}

void GlobalPitch::noteOff()
{
	envelope.noteOff();
}

float GlobalPitch::getFundamentalFreq()
{
	// This is where ADSR will be applied, currently it's just a passthrough
	float fundamentalFreq = (float)juce::MidiMessage::getMidiNoteInHertz(currentMidiNote);
	return fundamentalFreq;
}
