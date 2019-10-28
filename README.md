# FileIO

## Current Bugs

- exporting during the audio thread (when track is engaged in DAW or DAW is recording) causes DAW to crash
- sample rate is hardcoded leading to incorrect file writing
- currently, file reading occupies the audio signal path with a looped .wav file
