/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
using namespace juce; //NEEDED THIS

//==============================================================================
/**
*/
class JulesAmpAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    JulesAmpAudioProcessorEditor (JulesAmpAudioProcessor&);
    ~JulesAmpAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    ScopedPointer<Slider> driveKnob;
    ScopedPointer<Slider> rangeKnob;
    ScopedPointer<Slider> blendKnob;
    ScopedPointer<Slider> volumeKnob;

    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> driveAttachement;
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> rangeAttachement;
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> blendAttachement;
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> volumeAttachement;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    JulesAmpAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JulesAmpAudioProcessorEditor)
};