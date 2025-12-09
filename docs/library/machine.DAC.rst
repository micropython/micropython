.. currentmodule:: machine
.. _machine.DAC:

class DAC -- digital to analog conversion
=========================================

The DAC is used to output an analog voltage based on a digital value.

The output voltage will be between 0 and 3.3V.

DAC is currently supported on ESP32 [#esp32_dac]_, SAMD and Renesas RA.

.. note::
  The STM32 port has similar functionality to ``machine.DAC``. See
  :ref:`pyb.DAC <pyb.DAC>` for details.

Example usage (ESP32)::

    from machine import DAC

    dac = DAC(pin)    # create a DAC object acting on a pin
    dac.write(128)    # write a value to the DAC
    dac.write(255)    # output maximum value, 3.3V

Constructors
------------

.. class:: DAC(id)

  Construct a new DAC object.

  ``id`` is a pin object (ESP32 and Renesas RA) or an index to a DAC resource (SAMD).

.. note::
  On the ESP32, DAC functionality is available on pins 25 and 26. On the
  ESP32-S2, pins 17 and 18. See :ref:`ESP32 Quickref <esp32_quickref>`
  for more details.

.. note::
  SAMD21 has one DAC resource, SAMD51 has two. See :ref:`SAMD Quickref <samd_quickref>`
  for more details.

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
  | Renesas RA   | 12   | 0-4095 |
  +--------------+------+--------+

.. rubric:: Footnotes

.. [#esp32_dac] The original ESP32 and ESP32-S2 *only*, since DAC hardware is
    not present on other microcontrollers in the family.
