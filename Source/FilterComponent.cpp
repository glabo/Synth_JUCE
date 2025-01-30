#include "FilterComponent.h"
#include "Filter.h"

FilterComponent::FilterComponent(Synth_JUCEAudioProcessor& p,
									juce::String filterTypeId,
									juce::String cutoffFreqId,
									juce::String qId,
									juce::String resonanceId,
									juce::String adsrAmountId,
									juce::String attackId,
									juce::String decayId,
									juce::String sustainId,
									juce::String releaseId) :
	audioProcessor(p),
	filterTypeAttachment(p.apvts, filterTypeId, filterTypeSelection),
	cutoffFreqAttachment(p.apvts, cutoffFreqId, cutoffFreqSlider),
	qAttachment(p.apvts, qId, qSlider),
	resonanceAttachment(p.apvts, resonanceId, resonanceSlider),
	adsrAmountAttachment(p.apvts, adsrAmountId, adsrAmountSlider),
	attackAttachment(p.apvts, attackId, attackSlider),
	decayAttachment(p.apvts, decayId, decaySlider),
	sustainAttachment(p.apvts, sustainId, sustainSlider),
	releaseAttachment(p.apvts, releaseId, releaseSlider)
{
	// Filter type selector
	addAndMakeVisible(filterTypeSelection);

	auto* parameter = p.apvts.getParameter(filterTypeId);
	auto filterType = static_cast<FilterType>(parameter->convertFrom0to1(parameter->getValue()) + 1);
	filterTypeSelection.addItemList(parameter->getAllValueStrings(), 1);

	filterTypeSelection.onChange = [this] { filterTypeSelectionChanged(); };
	filterTypeSelection.setSelectedId(int(filterType));

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
	
	// ADSR Amount slider
	addAndMakeVisible(adsrAmountSlider);
	adsrAmountSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
	adsrAmountSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 12);
	adsrAmountSlider.setDoubleClickReturnValue(true, 0.0);

	adsrAmountLabel.attachToComponent(&adsrAmountSlider, false);
	adsrAmountLabel.setFont(juce::FontOptions(11.0f));
	adsrAmountLabel.setJustificationType(juce::Justification::centred);

	// Attack slider
	addAndMakeVisible(attackSlider);
	attackSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
	attackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 12);
	attackSlider.setDoubleClickReturnValue(true, 0.0);

	attackLabel.attachToComponent(&attackSlider, false);
	attackLabel.setFont(juce::FontOptions(11.0f));
	attackLabel.setJustificationType(juce::Justification::centred);

	// Decay slider
	addAndMakeVisible(decaySlider);
	decaySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
	decaySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 12);
	decaySlider.setDoubleClickReturnValue(true, 0.0);

	decayLabel.attachToComponent(&decaySlider, false);
	decayLabel.setFont(juce::FontOptions(11.0f));
	decayLabel.setJustificationType(juce::Justification::centred);

	// Sustain slider
	addAndMakeVisible(sustainSlider);
	sustainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
	sustainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 12);
	sustainSlider.setDoubleClickReturnValue(true, 1.0);

	sustainLabel.attachToComponent(&sustainSlider, false);
	sustainLabel.setFont(juce::FontOptions(11.0f));
	sustainLabel.setJustificationType(juce::Justification::centred);

	// Release slider
	addAndMakeVisible(releaseSlider);
	releaseSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
	releaseSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 12);
	releaseSlider.setDoubleClickReturnValue(true, 0.0);

	releaseLabel.attachToComponent(&releaseSlider, false);
	releaseLabel.setFont(juce::FontOptions(11.0f));
	releaseLabel.setJustificationType(juce::Justification::centred);
}

void FilterComponent::paint(juce::Graphics& g)
{
	g.setColour(juce::Colours::dimgrey);
	g.fillRect(componentBorder);

	g.setColour(juce::Colours::black);
	g.drawRect(componentBorder, 3);
	g.drawRect(filterBorder);
	g.drawRect(adsrAmountBorder);
	g.drawRect(adsrBorder);
	g.drawRect(adsrSubBorder);
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

	auto adsrBounds = adsrBorder;
	adsrAmountBorder = adsrBounds.removeFromLeft(filterSubdivision);
	auto adsrAmountBox = adsrAmountBorder;
	adsrAmountBox.removeFromTop(20);
	adsrAmountSlider.setBounds(adsrAmountBox);

	adsrBounds.removeFromTop(20);
	adsrSubBorder = adsrBounds;
	auto adsrBoxSubDivision = juce::jmin(180, adsrBounds.proportionOfWidth(0.25f));
	auto attackSliderBounds = adsrBounds.removeFromLeft(adsrBoxSubDivision);
	attackSlider.setBounds(attackSliderBounds);
	auto decaySliderBounds = adsrBounds.removeFromLeft(adsrBoxSubDivision);
	decaySlider.setBounds(decaySliderBounds);
	auto sustainSliderBounds = adsrBounds.removeFromLeft(adsrBoxSubDivision);
	sustainSlider.setBounds(sustainSliderBounds);
	auto releaseSliderBounds = adsrBounds.removeFromLeft(adsrBoxSubDivision);
	releaseSlider.setBounds(releaseSliderBounds);
}

void FilterComponent::filterTypeSelectionChanged()
{
	FilterType filterType = static_cast<FilterType>(this->filterTypeSelection.getSelectedId());
	audioProcessor.setFilterType(filterType);
}
