This chip supports playing in many formats including MP3 etc.
For this driver we support the standard PCM in 8 bit unsigned and 16 bit signed formats.

Requires GFX_USE_GTIMER

 * 	The VS1053 has an internal processor which can decode many file formats directly.
 * 	If you want to use anything other than raw PCM then you should use GAUDIO_PLAY_FORMAT_FILE and pipe
 * 	the entire file as if it was sound data. It doesn't matter if you choose the mono or stereo channel
 * 	as the codec chip automatically detects the real format itself.
 * 	No testing is made of the file format - if there is an error it can only be detected by the codec chip
 * 	and its behaviour is undefined (we haven't tested).
 * 	Note that some formats require a firmware patch to be installed to play correctly.
 * 	In this case define VS1053_FIRMWARE_PATCH as TRUE in your gfxconf.h file and include the patch file
 * 	in your project directory. The patch file MUST be called "vs1053_patch.plg".
