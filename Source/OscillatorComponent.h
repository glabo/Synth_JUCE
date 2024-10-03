#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class OscillatorComponent : public juce::Component {
public:
    OscillatorComponent(Synth_JUCEAudioProcessor& p);

    int getControlParameterIndex(Component& control);
private:
    Synth_JUCEAudioProcessor& audioProcessor;

    juce::ComboBox waveTypeSelection;
    juce::AudioProcessorValueTreeState::ComboBoxAttachment waveTypeAttachment;

    juce::Slider gainSlider, delaySlider;
    juce::AudioProcessorValueTreeState::SliderAttachment gainAttachment, delayAttachment;

    juce::Label gainLabel{ {}, "Level:" },
                delayLabel{ {}, "Delay:" },
                waveTypeLabel{ {}, "Waveform:" };

    void waveTypeSelectionChanged();

    void resized() override;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscillatorComponent)
};