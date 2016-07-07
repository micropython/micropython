This driver uses the Win32 audio system to provide GAUDIO play and record channels.

For PLAY   - It supports 2 channels, Channel 0 being a mono channel and Channel 1 being a stereo channel.
For RECORD - It supports 2 channels, Channel 0 being a mono channel and Channel 1 being a stereo channel.

For stereo, the samples are interleaved. Remember to allocate enough space for two samples per
sample period.

This is a simple driver that makes no changes to the mixer so set up the audio mixer using
the windows control panel audio mixer before starting recording/playback.
