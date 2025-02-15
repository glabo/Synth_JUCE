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

float getWaveformSample(WAVE_TYPE waveType, double dPhase);
