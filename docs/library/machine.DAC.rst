.. currentmodule:: machine
.. _machine.DAC:

class DAC -- digital to analog conversion
=========================================

The DAC is used to output an analog voltage based on a digital value.

The output voltage will be between 0 and 3.3V.

DAC is currently supported on ESP32 (the original ESP32 and ESP32-S2 only), SAMD
and Renesas.

Example usage (ESP32)::

    from machine import DAC

    dac = DAC(pin)    # create a DAC object acting on a pin
    dac.write(128)    # write a value to the DAC

    dac = DAC(17)
    dac.write(255)    # output maximum value, 3.3V

Constructors
------------

.. class:: DAC(pin)

   Construct a new DAC object.

   ``pin`` is a pin object.

Methods
-------

.. method:: DAC.write(value)

  Output an analog voltage to the pin connected to the DAC.

  ``value`` is a representation of the desired output; a linear interpolation of
  0-3.3V, though the range differs depending on the port and micro, see below:

  +--------------+------+--------+
  | *Port/micro* | Bits | Range  |
  +==============+======+========+
  | ESP32        | 8    | 0-255  |
  +--------------+------+--------+
  | SAMD21       | 10   | 0-1023 |
  +--------------+------+--------+
  | SAMD51       | 12   | 0-4095 |
  +--------------+------+--------+
  | Renesas      | 12   | 0-4095 |
  +--------------+------+--------+
