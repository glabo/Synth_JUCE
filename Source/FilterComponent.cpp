#include "FilterComponent.h"
#include "Filter.h"

FilterComponent::FilterComponent(Synth_JUCEAudioProcessor& p,
									juce::String filterTypeId,
									juce::String cutoffFreqId,
									juce::String qId,
									juce::String resonanceId) :
	audioProcessor(p),
	filterTypeAttachment(p.apvts, filterTypeId, filterTypeSelection),
	cutoffFreqAttachment(p.apvts, cutoffFreqId, cutoffFreqSlider),
	qAttachment(p.apvts, qId, qSlider),
	resonanceAttachment(p.apvts, resonanceId, resonanceSlider)
{
	// Filter type selector
	addAndMakeVisible(filterTypeSelection);

	auto* parameter = p.apvts.getParameter(filterTypeId);
	auto waveType = static_cast<WAVE_TYPE>(parameter->convertFrom0to1(parameter->getValue()) + 1);
	filterTypeSelection.addItemList(parameter->getAllValueStrings(), 1);

	filterTypeSelection.onChange = [this] { filterTypeSelectionChanged(); };
	filterTypeSelection.setSelectedId(waveType);

	filterTypeLabel.attachToComponent(&filterTypeSelection, false);
	filterTypeLabel.setFont(juce::FontOptions(11.0f));

	// cutoffFreq slider
	addAndMakeVisible(cutoffFreqSlider);
	cutoffFreqSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
	cutoffFreqSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 12);
	cutoffFreqSlider.setDoubleClickReturnValue(true, 0.0);

	cutoffFreqLabel.attachToComponent(&cutoffFreqSlider, false);
	cutoffFreqLabel.setFont(juce::FontOptions(11.0f));
	cutoffFreqLabel.setJustificationType(juce::Justification::centred);

	// q slider
	addAndMakeVisible(qSlider);
	qSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
	qSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 12);
	qSlider.setDoubleClickReturnValue(true, 0.1);

	qLabel.attachToComponent(&qSlider, false);
	qLabel.setFont(juce::FontOptions(11.0f));
	qLabel.setJustificationType(juce::Justification::centred);

	// resonance slider
	addAndMakeVisible(resonanceSlider);
	resonanceSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
	resonanceSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 12);
	resonanceSlider.setDoubleClickReturnValue(true, 0.0);

	resonanceLabel.attachToComponent(&resonanceSlider, false);
	resonanceLabel.setFont(juce::FontOptions(11.0f));
	resonanceLabel.setJustificationType(juce::Justification::centred);
}

void FilterComponent::paint(juce::Graphics& g)
{
	g.setColour(juce::Colours::dimgrey);
	g.fillRect(componentBorder);

	g.setColour(juce::Colours::black);
	g.drawRect(componentBorder, 3);
	g.drawRect(filterBorder);
	g.drawRect(adsrBorder);
}

void FilterComponent::resized()
{
	auto controlArea = getLocalBounds();
	componentBorder = controlArea;

	filterBorder = controlArea.removeFromTop(80);
	adsrBorder = controlArea.removeFromTop(80);

	auto filterBounds = filterBorder;
	auto filterSubdivision = juce::jmin(180, filterBounds.proportionOfWidth(0.25f));
	auto filterTypeBounds = filterBounds.removeFromLeft(filterSubdivision);
	auto cutoffFreqBounds = filterBounds.removeFromLeft(filterSubdivision);
	auto qBounds = filterBounds.removeFromLeft(filterSubdivision);
	auto resonanceBounds = filterBounds.removeFromLeft(filterSubdivision);

	filterTypeBounds.removeFromTop(30);
	filterTypeBounds.removeFromBottom(30);
	filterTypeBounds.removeFromLeft(10);
	filterTypeBounds.removeFromRight(10);
	filterTypeSelection.setBounds(filterTypeBounds);

	cutoffFreqBounds.removeFromTop(20);
	cutoffFreqSlider.setBounds(cutoffFreqBounds);
	qBounds.removeFromTop(20);
	qSlider.setBounds(qBounds);
	resonanceBounds.removeFromTop(20);
	resonanceSlider.setBounds(resonanceBounds);

	//auto adsrSubdivision = juce::jmin(180, adsrBorder.proportionOfWidth(0.25f));
}

void FilterComponent::filterTypeSelectionChanged()
{
	FilterType filterType = static_cast<FilterType>(this->filterTypeSelection.getSelectedId());
	audioProcessor.setFilterType(filterType);
}
