#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class FilterComponent : public juce::Component {
public:
	FilterComponent(Synth_JUCEAudioProcessor& p,
		juce::String filterTypeId,
		juce::String cutoffFreqId,
		juce::String qId,
		juce::String resonanceId,
		juce::String adsrAmountId,
		juce::String attackId,
		juce::String decayId,
		juce::String sustainId,
		juce::String releaseId);
private:
	Synth_JUCEAudioProcessor& audioProcessor;
	juce::ComboBox filterTypeSelection;
	juce::AudioProcessorValueTreeState::ComboBoxAttachment filterTypeAttachment;

	juce::Slider cutoffFreqSlider, qSlider, resonanceSlider, adsrAmountSlider, attackSlider, decaySlider, sustainSlider, releaseSlider;
	juce::AudioProcessorValueTreeState::SliderAttachment cutoffFreqAttachment, qAttachment, resonanceAttachment, adsrAmountAttachment, attackAttachment, decayAttachment, sustainAttachment, releaseAttachment;

	juce::Label filterTypeLabel{ {}, "Filter Type:" },
				cutoffFreqLabel{ {}, "Frequency:" },
				qLabel{ {}, "Q:" },
				resonanceLabel{ {}, "Resonance:" },
				adsrAmountLabel{ {}, "Env. Amount:" },
				attackLabel{ {}, "Attack:" },
				decayLabel{ {}, "Decay:" },
				sustainLabel{ {}, "Sustain:" },
				releaseLabel{ {}, "Release:" };

	// Visual areas
	juce::Rectangle<int> componentBorder;
	juce::Rectangle<int> filterBorder;
	juce::Rectangle<int> adsrAmountBorder;
	juce::Rectangle<int> adsrBorder;
	juce::Rectangle<int> adsrSubBorder;

	void filterTypeSelectionChanged();

	void paint(juce::Graphics&) override;
	void resized() override;
};