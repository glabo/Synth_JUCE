#pragma once
#include <JuceHeader.h>

enum WAVE_TYPE {
	NONE,
	SINE,
	SQUARE,
	TRIANGLE,
	//SAW_DIGITAL,
	SAW_ANALOG,
	NOISE,
};

double getWaveformSample(WAVE_TYPE waveType, double dPhase, double freq = 0.0);
