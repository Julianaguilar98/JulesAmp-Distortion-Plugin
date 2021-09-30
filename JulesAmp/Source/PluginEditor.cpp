/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

void LookAndFeel::drawRotarySlider(juce::Graphics& g,
    int x,
    int y,
    int width,
    int height,
    float sliderPosProportional,
    float rotaryStartAngle,
    float rotaryEndAngle,
    juce::Slider& slider)
{
    using namespace juce;

    auto bounds = Rectangle<float>(x, y, width, height);

    g.setColour(Colour(117u, 33u, 255u));
    g.fillEllipse(bounds);

    g.setColour(Colour(170u, 253u, 237u));
    g.drawEllipse(bounds, 1.f);

    if (auto* rswl = dynamic_cast<RotarySliderWithLabels*>(&slider))
    {
        auto center = bounds.getCentre();

        Path p;

        Rectangle<float> r;
        r.setLeft(center.getX() - 2);
        r.setRight(center.getX() + 2);
        r.setTop(bounds.getY());
        r.setBottom(center.getY() - rswl->getTextHeight() * 1.5);

        p.addRoundedRectangle(r, 2.f);

        jassert(rotaryStartAngle < rotaryEndAngle);

        auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);
        p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));
        g.fillPath(p);

        g.setFont(rswl->getTextHeight());
        auto text = rswl->getDisplayString();
        auto strWidth = g.getCurrentFont().getStringWidth(text);
        r.setSize(strWidth + 4, rswl->getTextHeight() + 2);
        r.setCentre(bounds.getCentre());

        g.setColour(Colours::black);
        g.fillRect(r);

        g.setColour(Colours::white);
        g.drawFittedText(text, r.toNearestInt(), juce::Justification::centred, 1);
    }
}

juce::String RotarySliderWithLabels::getDisplayString() const
{
    if (auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param))
        return choiceParam->getCurrentChoiceName();

    juce::String str;
    bool addK = false;

    if (auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param))
    {
        float val = getValue();

        if (val > 999.f)
        {
            val /= 1000.f; //1001 / 1000 = 1.001
            addK = true;
        }

        str = juce::String(val, (addK ? 2 : 0));
    }
    else
    {
        jassertfalse; //this shouldn't happen!
    }

    if (suffix.isNotEmpty())
    {
        str << " ";
        if (addK)
            str << "k";

        str << suffix;
    }

    return str;
}

void RotarySliderWithLabels::paint(juce::Graphics& g)
{

    auto startAng = juce::degreesToRadians(180.f + 45.f);
    auto endAng = juce::degreesToRadians(180.f - 45.f) + juce::MathConstants<float>::twoPi;

    auto range = getRange();

    auto sliderBounds = getSliderBounds();

    getLookAndFeel().drawRotarySlider(g,
        sliderBounds.getX(),
        sliderBounds.getY(),
        sliderBounds.getWidth(),
        sliderBounds.getHeight(),
        juce::jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0),
        startAng,
        endAng,
        *this);

    auto center = sliderBounds.toFloat().getCentre();
    auto radius = sliderBounds.getWidth() * 0.5f;

    g.setColour(juce::Colour(0u, 172u, 1u));
    g.setFont(getTextHeight());

}

juce::Rectangle<int> RotarySliderWithLabels::getSliderBounds() const
{
    return getLocalBounds();
}

//==============================================================================
JulesAmpAudioProcessorEditor::JulesAmpAudioProcessorEditor (JulesAmpAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{   

    //Adding Slider and type of Slider (Rotary)
    addAndMakeVisible(driveKnob = new juce::Slider("Drive"));
    driveKnob->setSliderStyle(juce::Slider::Rotary);
    driveKnob->setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 100);

    addAndMakeVisible(rangeKnob = new juce::Slider("Range"));
    rangeKnob->setSliderStyle(juce::Slider::Rotary);
    rangeKnob->setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 100);

    addAndMakeVisible(blendKnob = new juce::Slider("Blend"));
    blendKnob->setSliderStyle(juce::Slider::Rotary);
    blendKnob->setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 100);

    addAndMakeVisible(volumeKnob = new juce::Slider("Volume"));
    volumeKnob->setSliderStyle(juce::Slider::Rotary);
    volumeKnob->setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 100);

    driveAttachement = new juce::AudioProcessorValueTreeState::SliderAttachment(p.getState(), "drive", *driveKnob);
    rangeAttachement = new juce::AudioProcessorValueTreeState::SliderAttachment(p.getState(), "range", *rangeKnob);
    blendAttachement = new juce::AudioProcessorValueTreeState::SliderAttachment(p.getState(), "blend", *blendKnob);
    volumeAttachement = new juce::AudioProcessorValueTreeState::SliderAttachment(p.getState(), "volume", *volumeKnob);

    setSize (600, 400); //Change size to match JulesEQ
}

JulesAmpAudioProcessorEditor::~JulesAmpAudioProcessorEditor()
{
}

//==============================================================================
void JulesAmpAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
   // g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    //g.setColour (juce::Colours::white);
    //g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
    using namespace juce;
    auto bounds = getLocalBounds();
    g.fillAll(Colour(1u, 8u, 108u));//(Colours::white); //was black
    String title{ "JulesAmp" };
    g.setFont(30);
    g.setColour(Colour(140u, 33u, 255u));
    g.drawFittedText(title, bounds, juce::Justification::centred, 1);


    g.setFont(30);
    g.setColour(Colour(185u, 108u, 255u));
    g.drawText("Range", ((getWidth() / 6) * 1) - (100 / 2), (getHeight() / 2) + 10, 100, 100, Justification::centred, false);

    g.setFont(30);
    g.setColour(Colour(185u, 108u, 255u));
    g.drawText("Volume", ((getWidth() / 1) * 1) - (300 / 2), (getHeight() / 2) + 10, 100, 100, Justification::centred, false);

    g.setFont(30);
    g.setColour(Colour(185u, 108u, 255u));
    g.drawText("Drive", ((getWidth() / 6) * 1) - (100 / 2), (getHeight() / 6) + -50, 100, 100, Justification::centred, false);

    g.setFont(30);
    g.setColour(Colour(185u, 108u, 255u));
    g.drawText("Blend", ((getWidth() / 1) * 1) - (300 / 2), (getHeight() / 6) + -50, 100, 100, Justification::centred, false);
}

void JulesAmpAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto bounds = getLocalBounds();

    auto leftArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    auto rightArea = bounds.removeFromRight(bounds.getWidth() * 0.5);

    driveKnob->setBounds(leftArea.removeFromTop(leftArea.getHeight() * 0.5));
    rangeKnob->setBounds(leftArea);
    blendKnob->setBounds(rightArea.removeFromTop(rightArea.getHeight() * 0.5));
    volumeKnob->setBounds(rightArea);

    /*driveKnob->setBounds(((getWidth() / 5) * 1) - (100 / 2), (getHeight() / 2) - (100 / 2), 100, 100);
    rangeKnob->setBounds(((getWidth() / 5) * 2) - (100 / 2), (getHeight() / 2) - (100 / 2), 100, 100);
    blendKnob->setBounds(((getWidth() / 5) * 3) - (100 / 2), (getHeight() / 2) - (100 / 2), 100, 100);
    volumeKnob->setBounds(((getWidth() / 5) * 4) - (100 / 2), (getHeight() / 2) - (100 / 2), 100, 100);*/
}