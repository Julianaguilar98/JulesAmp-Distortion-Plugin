/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JulesAmpAudioProcessor::JulesAmpAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    state = new juce::AudioProcessorValueTreeState(*this, nullptr);

    state->createAndAddParameter("drive", "Drive", "Drive", juce::NormalisableRange<float>(0.f, 1.f, 0.0001), 1.0, nullptr, nullptr);
    state->createAndAddParameter("range", "Range", "Range", juce::NormalisableRange<float>(0.f, 1500.f, 0.0001, 0.25f), 1.0, nullptr, nullptr);
    state->createAndAddParameter("blend", "Blend", "Blend", juce::NormalisableRange<float>(0.f, 1.f, 0.0001), 1.0, nullptr, nullptr);
    state->createAndAddParameter("volume", "Volume", "Volume", juce::NormalisableRange<float>(0.f, 3.f, 0.0001), 1.0, nullptr, nullptr);

    state->state = juce::ValueTree("drive");
    state->state = juce::ValueTree("range");
    state->state = juce::ValueTree("blend");
    state->state = juce::ValueTree("volume");
}

JulesAmpAudioProcessor::~JulesAmpAudioProcessor()
{
}

//==============================================================================
const juce::String JulesAmpAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JulesAmpAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool JulesAmpAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool JulesAmpAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double JulesAmpAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JulesAmpAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int JulesAmpAudioProcessor::getCurrentProgram()
{
    return 0;
}

void JulesAmpAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String JulesAmpAudioProcessor::getProgramName (int index)
{
    return {};
}

void JulesAmpAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void JulesAmpAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void JulesAmpAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool JulesAmpAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void JulesAmpAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    float drive = *state->getRawParameterValue("drive");
    float range = *state->getRawParameterValue("range");
    float blend = *state->getRawParameterValue("blend");
    float volume = *state->getRawParameterValue("volume");

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (int sample = 0; sample < buffer.getNumSamples(); sample++) {

            float cleanSig = *channelData;

            *channelData *= drive * range;
            *channelData = (((((2.f / juce::float_Pi) * atan(*channelData)) * blend) + (cleanSig * (1.f - blend))) / 2) * volume;
            channelData++;
        }
    }
}

juce::AudioProcessorValueTreeState& JulesAmpAudioProcessor::getState() {

    return *state;
}

//==============================================================================
bool JulesAmpAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* JulesAmpAudioProcessor::createEditor()
{
    return new JulesAmpAudioProcessorEditor (*this);
}

//==============================================================================
void JulesAmpAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    juce::MemoryOutputStream stream(destData, false);
    state->state.writeToStream(stream);

}

void JulesAmpAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    juce::ValueTree tree = juce::ValueTree::readFromData(data, sizeInBytes);

    if (tree.isValid()) {
        state->state = tree;
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JulesAmpAudioProcessor();
}
