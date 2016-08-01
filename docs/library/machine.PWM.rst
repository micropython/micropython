.. currentmodule:: machine
.. _machine.PWM:

class PWM -- pulse width modulation
======================================

PWM can be enabled on all pins except Pin(16).  There is a single frequency
for all channels, with range between 1 and 1000 (measured in Hz).  The duty
cycle is between 0 and 1023 inclusive.

Example usage::

    from machine import Pin, PWM

    pwm0 = PWM(Pin(0))      # create PWM object from a pin
    pwm0.freq()             # get current frequency
    pwm0.freq(1000)         # set frequency
    pwm0.duty()             # get current duty cycle
    pwm0.duty(200)          # set duty cycle
    pwm0.deinit()           # turn off PWM on the pin

    pwm2 = PWM(Pin(2), freq=500, duty=512) # create and configure in one go

Constructors
------------

.. class:: PWM(pin,... )

   Create a PWM object. See ``init()`` for parameters if initialization.

Methods
-------

.. method:: PWM.init(pin, freq, duty)

   Create a PWM object from a pin.

.. method:: PWM.deinit()

   Disable PWM on the pin.

.. method:: PWM.freq()

   Get or set the frequency (in Hz).

.. method:: PWM.duty()

   Get or set the duty cycle of the PWM signal.
