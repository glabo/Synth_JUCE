#pragma once


class Pitch {
private:
	double currentAngle = 0.0;
	double angleDelta = 0.0;
	double freq = 0.0;
	double sampleRate = 1.0; // Avoiding div by 0, maybe bad practice
	int currentMidiNote = -1;
	int coarsePitchShift = 0;

	const int MAX_FINE_PITCH_SHIFT = 1000;
	int finePitchShift = 0;

	void calculateAngleDelta();
	void calculateAngleDelta(float freq, double sampleRate);
public:
	void setSampleRate(double sr);
	void setPitchShift(int coarsePitchShift, int finePitchShift);
	double calculateFinePitchShiftInHz(double inFreq);
	void noteOn(int midiNoteNumber, double sampleRate);
	void noteOn(float freq, double sampleRate);
	void clearNote();

	bool angleApproxZero();
	double getNextSample();
	double getNextSample(double freq);
};