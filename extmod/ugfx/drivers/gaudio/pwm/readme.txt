This driver uses a PWM output and a timer to implement an audio play channel.

Whilst the default config settings will probably work for your hardware, you may need to change them.
If so copy gaudio_play_config.h to your project directory and then alter it.

The maximum sample frequency is governed primarily by the number of bits of resolution and the
maximum PWM clock rate. The maximum sample frequency should be less than...

	Max PWM Clock / (2 ^ Bits per sample)
	
eg. For the AT91SAM7 max PWM clock = 48MHz / 2
	For 10 bit PWM accuracy that means GAUDIO_PLAY_MAX_SAMPLE_FREQUENCY = 23,437 Hz
