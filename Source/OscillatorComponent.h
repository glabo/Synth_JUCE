#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class OscillatorComponent : public juce::Component {
public:
    OscillatorComponent(Synth_JUCEAudioProcessor& p,
                        int id,
                        juce::String wavetypeId,
                        juce::String gainId,
                        juce::String delayId,
                        juce::String attackId,
                        juce::String decayId,
                        juce::String sustainId,
                        juce::String releaseId);

    int getControlParameterIndex(Component& control);
private:
    Synth_JUCEAudioProcessor& audioProcessor;

    // parameter IDs
    int oscId;
    juce::String wavetypeId;
    juce::String gainId;
    juce::String delayId;
    juce::String attackId;
    juce::String decayId;
    juce::String sustainId;
    juce::String releaseId;

    juce::ComboBox waveTypeSelection;
    juce::AudioProcessorValueTreeState::ComboBoxAttachment waveTypeAttachment;

    juce::Slider gainSlider, delaySlider,
        attackSlider, decaySlider, sustainSlider, releaseSlider;
    juce::AudioProcessorValueTreeState::SliderAttachment gainAttachment, delayAttachment,
                            attackAttachment, decayAttachment, sustainAttachment, releaseAttachment;

    juce::Label gainLabel{ {}, "Level:" },
                //delayLabel{ {}, "Delay:" },
                waveTypeLabel{ {}, "Waveform:" },
                attackLabel{ {}, "Attack:" },
                decayLabel{ {}, "Decay:" },
                sustainLabel{ {}, "Sustain:" },
                releaseLabel{ {}, "Release:" };

    void waveTypeSelectionChanged(int oscId = 0);

    void resized() override;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscillatorComponent)
};