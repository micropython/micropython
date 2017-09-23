The PWM Driver(bare-bone)
=========================

Pulse-width modulation (PWM) is used to create an analog behavior
digitally by controlling the amount of power transferred to the
connected peripheral. This is achieved by controlling the high period
(duty-cycle) of a periodic signal.

User can change the period or duty cycle whenever PWM is running. The
function pwm_set_parameters is used to configure these two parameters.
Note these are raw register values and the parameter duty_cycle means
the period of first half during one cycle, which should be not beyond
total period value.

In addition, user can also get multi PWM channels output from different
peripherals at the same time, which is implemented more flexible by the
function pointers.

Features
--------

* Initialization/de-initialization
* Enabling/disabling
* Run-time control of PWM duty-cycle and period
* Notifications about errors and one PWM cycle is done

Applications
------------

Motor control, ballast, LED, H-bridge, power converters, and
other types of power control applications.

Dependencies
------------

The peripheral which can perform waveform generation like frequency
generation and pulse-width modulation, such as Timer/Counter.

Concurrency
-----------

N/A

Limitations
-----------

The current driver doesn't support the features like recoverable,
non-recoverable faults, dithering, dead-time insertion.

Known issues and workarounds
----------------------------

N/A
