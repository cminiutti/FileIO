/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

static const int maxRecordTime = 60 * 3;	// max record time of 3 minutes

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
	, recordBuffer(2, 48000 * maxRecordTime)		// POSSIBLE ISSUE: Hardcoded sample rate
	, fileLoaded(false)
	, fileBufferPosition(0)
	, recording(false)
	, recBufferPosition(0)
	, recSampleCount(0)
#endif
{

}

FileIoAudioProcessor::~FileIoAudioProcessor()
{
}

void FileIoAudioProcessor::readFile()
{
	AudioFormatManager format;
	format.registerBasicFormats();

	FileChooser chooser("Select a Wav file to play", {}, "*.wav");

	if (chooser.browseForFileToOpen()) {
		auto file = chooser.getResult();
		std::unique_ptr<AudioFormatReader> reader(format.createReaderFor(file));

		if (reader.get() != nullptr) {
			fileBuffer.setSize(reader->numChannels, (int)reader->lengthInSamples);
			reader->read(&fileBuffer, 0, (int)reader->lengthInSamples, 0, true, true);
		}
	}

	fileLoaded = true;
}

void FileIoAudioProcessor::writeFile()
{
	File file(File::getSpecialLocation(File::SpecialLocationType::userDesktopDirectory).getChildFile("testBuffer.wav"));
	auto fileStream = file.createOutputStream();
	fileStream->setPosition(0);
	fileStream->truncate();

	AudioBuffer<float> testBuffer(2, systemSampleRate * 10.0f);
	testBuffer.clear();

	WavAudioFormat format;
	std::unique_ptr<AudioFormatWriter> writer;
	writer.reset(format.createWriterFor(fileStream, systemSampleRate, testBuffer.getNumChannels(), 24, {}, 0));

	if (writer != nullptr) {
		writer->writeFromAudioSampleBuffer(testBuffer, 0, testBuffer.getNumSamples());
	}
}

void FileIoAudioProcessor::playFile(AudioBuffer<float>& bufferToFill)
{
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	auto outputSamplesRemaining = bufferToFill.getNumSamples();
	auto outputSampleOffset = 0;

	while (outputSamplesRemaining > 0)
	{
		auto bufferSamplesRemaining = fileBuffer.getNumSamples() - fileBufferPosition;
		auto samplesToCopy = jmin(outputSamplesRemaining, bufferSamplesRemaining);

		for (int channel = 0; channel < totalNumInputChannels; ++channel)
		{
			auto* channelData = bufferToFill.getWritePointer(channel);

			bufferToFill.copyFrom(channel, outputSampleOffset, fileBuffer, channel, fileBufferPosition, samplesToCopy);
		}

		outputSamplesRemaining -= samplesToCopy;
		outputSampleOffset += samplesToCopy;
		fileBufferPosition += samplesToCopy;

		// loop
		if (fileBufferPosition == fileBuffer.getNumSamples()) {
			fileBufferPosition = 0;
		}
	}
}

void FileIoAudioProcessor::startRecording()
{
	recBufferPosition = 0;
	recSampleCount = 0;

	recordBuffer.clear();
}

void FileIoAudioProcessor::record(AudioBuffer<float>& bufferToFill)
{
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	for (int channel = 0; channel < totalNumInputChannels; ++channel)
	{
		auto* channelData = bufferToFill.getWritePointer(channel);

		recordBuffer.copyFrom(channel, recBufferPosition, bufferToFill, channel, 0, bufferToFill.getNumSamples());
	}

	recBufferPosition += bufferToFill.getNumSamples();
	recSampleCount += bufferToFill.getNumSamples();

	// wrap around if max length is reached
	if (recBufferPosition >= (48000 * maxRecordTime)) {
		recBufferPosition = 0;
	}
}

void FileIoAudioProcessor::exportRecording()
{
	if (recording == false) {
		AudioSampleBuffer exportBuffer(recordBuffer);
		int samplesToChop = exportBuffer.getNumSamples() - recSampleCount;
		exportBuffer.setSize(exportBuffer.getNumChannels(), exportBuffer.getNumSamples() - samplesToChop, true, true);

		File file(File::getSpecialLocation(File::SpecialLocationType::userDesktopDirectory).getChildFile("recording.wav"));
		auto fileStream = file.createOutputStream();
		fileStream->setPosition(0);
		fileStream->truncate();

		WavAudioFormat format;
		std::unique_ptr<AudioFormatWriter> writer;
		writer.reset(format.createWriterFor(fileStream, systemSampleRate, exportBuffer.getNumChannels(), 24, {}, 0));

		if (writer != nullptr) {
			writer->writeFromAudioSampleBuffer(exportBuffer, 0, exportBuffer.getNumSamples());
		}
	}
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

void FileIoAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	if (recording == true) {
		record(buffer);
	}

	if (fileLoaded == true && recording == false) {
		playFile(buffer);
	}
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
