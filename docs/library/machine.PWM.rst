.. currentmodule:: machine
.. _machine.PWM:

class PWM -- pulse width modulation
===================================

This class provides pulse width modulation output.

Example usage::

    from machine import PWM

    pwm = PWM(pin)          # create a PWM object on a pin
    pwm.duty_u16(32768)     # set duty to 50%

    # reinitialise with a period of 200us, duty of 5us
    pwm.init(freq=5000, duty_ns=5000)

    pwm.duty_ns(3000)       # set pulse width to 3us

    pwm.deinit()

Constructors
------------

.. class:: PWM(dest, \*, freq, duty_u16, duty_ns)

   Construct and return a new PWM object using the following parameters:

      - *dest* is the entity on which the PWM is output, which is usually a
        :ref:`machine.Pin <machine.Pin>` object, but a port may allow other values,
        like integers.
      - *freq* should be an integer which sets the frequency in Hz for the
        PWM cycle.
      - *duty_u16* sets the duty cycle as a ratio ``duty_u16 / 65535``.
      - *duty_ns* sets the pulse width in nanoseconds.

   Setting *freq* may affect other PWM objects if the objects share the same
   underlying PWM generator (this is hardware specific).
   Only one of *duty_u16* and *duty_ns* should be specified at a time.

Methods
-------

.. method:: PWM.init(\*, freq, duty_u16, duty_ns)

   Modify settings for the PWM object.  See the above constructor for details
   about the parameters.

.. method:: PWM.deinit()

   Disable the PWM output.

.. method:: PWM.freq([value])

   Get or set the current frequency of the PWM output.

   With no arguments the frequency in Hz is returned.

   With a single *value* argument the frequency is set to that value in Hz.  The
   method may raise a ``ValueError`` if the frequency is outside the valid range.

.. method:: PWM.duty_u16([value])

   Get or set the current duty cycle of the PWM output, as an unsigned 16-bit
   value in the range 0 to 65535 inclusive.

   With no arguments the duty cycle is returned.

   With a single *value* argument the duty cycle is set to that value, measured
   as the ratio ``value / 65535``.

.. method:: PWM.duty_ns([value])

   Get or set the current pulse width of the PWM output, as a value in nanoseconds.

   With no arguments the pulse width in nanoseconds is returned.

   With a single *value* argument the pulse width is set to that value.
