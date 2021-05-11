Hardware Pulse Width Modulation
===============================

The pico port has 16 controllable PWM pins, as the RP2040 PWM block has 8 identical slices where each slice can further be divides into two PWM output signals which measure the frequence or suty cycle of an input signal.

The PWM hardware funcions by comparing the input value to a free-running counter which produces a toggling output whhere the amount of time spent at the high ouput level is proportional to the input value.The fraction of time spent at high level signal is known as duty cycle of signal.

The default behaviour of a PWM slice is to count upward until it attains the wrap value(pwm_config_set_wrap) is reached, and then it behaves such that it immediately wraps to 0. PWM slices also offer a phase-correct mode, in which the counter starts to count downward after reaching TOP, unitl it reaches 0 again

	**pwm_clkdiv_mode** - PWM Divider mode settings

	**pwm_clear_irq** - Clears singke PWM channel interrupt

	**pwm_config_set_wrap** - Set PWM counter wrap value in a PWM configuration


	
See `this <https://raspberrypi.github.io/pico-sdk-doxygen/group__hardware__pwm.html>`_ for a more detailed information refarding PWM functions and the Raspberry pico PWM API.
