#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class FilterComponent : public juce::Component {
public:
	FilterComponent(Synth_JUCEAudioProcessor& p,
		juce::String filterTypeId,
		juce::String cutoffFreqId,
		juce::String qId,
		juce::String resonanceId);
private:
	Synth_JUCEAudioProcessor& audioProcessor;
	juce::ComboBox filterTypeSelection;
	juce::AudioProcessorValueTreeState::ComboBoxAttachment filterTypeAttachment;

	juce::Slider cutoffFreqSlider, qSlider, resonanceSlider;
	juce::AudioProcessorValueTreeState::SliderAttachment cutoffFreqAttachment, qAttachment, resonanceAttachment;

	juce::Label filterTypeLabel{ {}, "Filter Type:" },
				cutoffFreqLabel{ {}, "Frequency:" },
				qLabel{ {}, "Q:" },
				resonanceLabel{ {}, "Resonance:" };

	// Visual areas
	juce::Rectangle<int> componentBorder;
	juce::Rectangle<int> filterBorder;
	juce::Rectangle<int> adsrBorder;

	void filterTypeSelectionChanged();

	void paint(juce::Graphics&) override;
	void resized() override;
};