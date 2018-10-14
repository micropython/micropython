.. currentmodule:: machine
.. _machine.PWM:

class PWM -- pulse width modulation
===================================

This class provides pulse width modulation output.

Example usage::

    from machine import PWM

    pwm = PWM(pin)          # create a PWM object on a pin
    pwm.duty_u16(32768)     # set duty to 50%

    # reinitialise with a period of 20ms, duty of 2ms
    pwm.init(period=20, tick_hz=1000, duty_ticks=2)

    pwm.duty_ticks(3)       # set duty to 3ms

    pwm.deinit()

Constructors
------------

.. class:: PWM.init(dest, \*, freq, period, tick_hz, duty_ticks, duty_u16, block)

   Construct and return a new PWM object using the following parameters:

      - *dest* is the entity on which the PWM is output, which is usually a
        `machine.Pin <machine.Pin>` object, but a port may allow other values,
        like integers.
      - *freq* should be an integer which sets the frequency in Hz for the 
        PWM cycle.
      - *period* should be an integer which sets the period of the PWM cycle.
        The units of this are given by *tick_hz*.
      - *tick_hz* gives the base unit by which *period* and *duty_ticks*
        are measured.
      - *duty_ticks* sets the duty cycle in ticks.
      - *duty_u16* sets the duty cycle as a ratio ``duty_u16 / 65535``.
      - *block* allows for finer control over the underlying PWM generator
        that is used for this PWM object, and the values it takes are port
        specific (this parameter may or may not be supported by a port).

   Only one of *freq* and *period* should be specified at a time.  Setting these
   values may affect other PWM objects if the objects share the same underlying
   PWM generator (this is hardware specific).

   The value of *tick_hz* will be saved and reused by the ``PWM.duty_ticks``
   method.  This value is unique to each PWM object.

Methods
-------

.. method:: PWM.init(\*, freq, period, tick_hz, duty_ticks, duty_u16)

   Modify settings for the PWM object.  See the above constructor for details
   about the parameters.

.. method:: PWM.deinit()

   Disable the PWM output.

.. method:: PWM.duty_ticks(ticks)

   Change the duty cycle of the output, with the argument *ticks* measured in
   ``ticks_hz`` as set by the constructor or ``PWM.init``.  For example, if
   ``ticks_hz`` is 1000 then *ticks* is measured in milliseconds.

.. method:: PWM.duty_u16(u16)

   Change the duty cycle of the output, measured as the ratio ``u16 / 65535``.
