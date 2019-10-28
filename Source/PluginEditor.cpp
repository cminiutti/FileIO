/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FileIoAudioProcessorEditor::FileIoAudioProcessorEditor (FileIoAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 400);

	readButton.setButtonText("Read");
	readButton.onClick = [this] {processor.readFile(); };
	addAndMakeVisible(&readButton);

	writeButton.setButtonText("Write");
	writeButton.onClick = [this] {processor.writeFile(); };
	addAndMakeVisible(&writeButton);
}

FileIoAudioProcessorEditor::~FileIoAudioProcessorEditor()
{
}

//==============================================================================
void FileIoAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    /*g.setColour (Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), Justification::centred, 1);*/
}

void FileIoAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

	readButton.setBounds(100, 100, 200, 50);
	writeButton.setBounds(100, 250, 200, 50);
}
