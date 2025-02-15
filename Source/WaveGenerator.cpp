#include "WaveGenerator.h"

float getWaveformSample(WAVE_TYPE waveType, double dPhase)
{
	// freq parameter is currently unused, we probably should just get rid of it...
	float baseValue = 0.0;
	switch (waveType) {
	case SINE:
		baseValue = (float)sin(dPhase);
		break;
	case SQUARE: // Square wave between -1 and +1
		baseValue = sin(dPhase) > 0 ? 1.0f : -1.0f;
		break;
	case TRIANGLE: // Triangle wave between -1 and +1
		baseValue = (float)asin(sin(dPhase)) * (2.0f /
			juce::MathConstants<float>::pi);
		break;
	case SAW_ANALOG: // Saw wave (analogue / warm / slow)
	{
		double dCustom = 50.0;
		float dOutput = 0.0f;
		for (double n = 1.0; n < dCustom; n++)
			dOutput += (float)((sin(n * dPhase)) / n);
		baseValue = dOutput * (2.0f /
			juce::MathConstants<float>::pi);
		break;
	}
	//case SAW_DIGITAL:
	//	baseValue = (2.0 / juce::MathConstants<float>::pi) * 
	//				(freq * juce::MathConstants<float>::pi * std::fmod(dTime, 1.0 / freq) -
	//				(juce::MathConstants<float>::pi / 2.0));
	//	juce::
	//	break;
	case NOISE:
		// nextDouble produces a double between 0.0 and 1.0
		baseValue = 2.0f * juce::Random::getSystemRandom().nextFloat() - 1.0f;
		break;
	default:
		baseValue = 0.0f;
	}
	return baseValue;
}
