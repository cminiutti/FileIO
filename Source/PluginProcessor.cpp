/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FileIoAudioProcessor::FileIoAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
	//, writeThread("write thread")
#endif
{
}

FileIoAudioProcessor::~FileIoAudioProcessor()
{
}

//==============================================================================
const String FileIoAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FileIoAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FileIoAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FileIoAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FileIoAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FileIoAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FileIoAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FileIoAudioProcessor::setCurrentProgram (int index)
{
}

const String FileIoAudioProcessor::getProgramName (int index)
{
    return {};
}

void FileIoAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void FileIoAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	systemSampleRate = sampleRate;

	/*// pre-made buffer stuff
	AudioBuffer<float> testBuffer(2, sampleRate * 3.0f);

	// threaded stuff/necessary stuff
	//writeThread.startThread();
	//FileOutputStream* stream = new FileOutputStream(File::getSpecialLocation(File::userHomeDirectory()).getChildFile("testBuffer.wav"));
	File outputFile = File("C:\testBuffer.wav");
	FileOutputStream* outputStream = outputFile.createOutputStream();
	WavAudioFormat format;
	std::unique_ptr<AudioFormatWriter> writer;
	writer.reset(format.createWriterFor(outputStream, sampleRate, testBuffer.getNumChannels(), 24, {}, 0));
	//AudioFormatWriter::ThreadedWriter* threaded = new AudioFormatWriter::ThreadedWriter(writer, writeThread, 16384);

	// file writing
	if (writer != nullptr) {
		writer->writeFromAudioSampleBuffer(testBuffer, 0, testBuffer.getNumSamples());
	}*/
}

void FileIoAudioProcessor::writeFile()
{
	File file("C:\Users\minut\Documents\testBuffer.wav");
	AudioBuffer<float> testBuffer(2, systemSampleRate * 3.0f);
	WavAudioFormat format;
	std::unique_ptr<AudioFormatWriter> writer;
	writer.reset(format.createWriterFor(new FileOutputStream(file), systemSampleRate, testBuffer.getNumChannels(), 24, {}, 0));

	// file writing
	if (writer != nullptr) {
		writer->writeFromAudioSampleBuffer(testBuffer, 0, testBuffer.getNumSamples());
	}
}

void FileIoAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FileIoAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void FileIoAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
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
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }

	writeFile();
}

//==============================================================================
bool FileIoAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* FileIoAudioProcessor::createEditor()
{
    return new FileIoAudioProcessorEditor (*this);
}

//==============================================================================
void FileIoAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FileIoAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FileIoAudioProcessor();
}
