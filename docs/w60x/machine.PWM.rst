.. currentmodule:: machine
.. _machine.PWM:

class PWM -- Pulse Width Modulation
===========================

PWM is a method of digitally encoding analog signal levels. Through the use of high resolution counters,
The duty cycle of the wave is modulated to encode the level of a particular analog signal. The PWM signal is still digital,
Because at any given time, the full-scale DC power supply is either fully (ON) or completely OFF. Voltage
Or the current source is added to the analog load in a repeating pulse sequence of ON or OFF. Time
When the DC power is applied to the load, the power supply is disconnected when it is disconnected. As long as the bandwidth is sufficient,
Any analog value can be encoded using PWM

Example usage::

    from machine import PWM
    pwm = PWM(1,2,3500,50,20)
    pwm.duty(1)
	pwm.duty()
	pwm.freq(115200)
	pwm.freq()

Availability of this class: W60x

Constructors
------------

.. class:: PWM(pin=1,channel=2,freq=3500,duty=50,pnum=20)

	Create a PWM machine interface object.

Methods
-------

.. method:: pwm.init(channel,freq,duty,pnum)
	Initialize Pulse Width Modulation. The channel range is 0 to 4. The frequency 
	range from 1 to 156250. The duty range is 0 to 255. The pnum range from 0 to 255.

.. method:: pwm.deinit()
	used to stop Pulse Width Modulation.

.. method:: pwm.duty(duty)
	used to set duty radio. The duty range is 0 to 255. Otherwise, query current duty 
	if no argument is provided.

.. method:: pwm.freq(freq)
	used to set frequency. The frequency range is 1 to 156250. Otherwise, query current frequency 
	if no argument is provided.
