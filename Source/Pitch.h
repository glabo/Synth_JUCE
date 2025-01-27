#pragma once


class Pitch {
private:
	double currentAngle = 0.0;
	double angleDelta = 0.0;
	double freq = 0.0;
	double sampleRate = 1.0; // Avoiding div by 0, maybe bad practice
	int currentMidiNote = -1;
	int pitchShift = 0;

	void calculateAngleDelta();
public:
	void setSampleRate(double sr);
	void setPitchShift(int pitchShift);
	void noteOn(int midiNoteNumber);
	void clearNote();

	bool angleApproxZero();
	double getNextSample();
};