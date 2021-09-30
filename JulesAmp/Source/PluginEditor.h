/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

struct LookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider(juce::Graphics&,
        int x, int y, int width, int height,
        float sliderPosProportional,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider&) override;

};

struct RotarySliderWithLabels : juce::Slider
{
    RotarySliderWithLabels(juce::RangedAudioParameter& rap, const juce::String& unitSuffix) :
        juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            juce::Slider::TextEntryBoxPosition::NoTextBox),
        param(&rap),
        suffix(unitSuffix)
    {
        setLookAndFeel(&lnf);
    }

    ~RotarySliderWithLabels()
    {
        setLookAndFeel(nullptr);
    }

    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 14; }
    juce::String getDisplayString() const;
private:
    LookAndFeel lnf;

    juce::RangedAudioParameter* param;
    juce::String suffix;
};

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

    juce::ScopedPointer<juce::Slider> driveKnob;
    juce::ScopedPointer<juce::Slider> rangeKnob;
    juce::ScopedPointer<juce::Slider> blendKnob;
    juce::ScopedPointer<juce::Slider> volumeKnob;

    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachement;
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> rangeAttachement;
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> blendAttachement;
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachement;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    JulesAmpAudioProcessor& audioProcessor;
    std::vector<juce::Component*> getComps();
    LookAndFeel lnf;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JulesAmpAudioProcessorEditor)
};
