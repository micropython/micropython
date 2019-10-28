.. currentmodule:: machine
.. _machine.ADC:

class ADC -- analog to digital conversion
=========================================

The ADC class provides an interface to analog-to-digital convertors, and
represents a single endpoint that can sample a continuous voltage and
convert it to a discretised value.

Example usage::

   import machine

   adc = machine.ADC(pin)   # create an ADC object acting on a pin
   val = adc.read_u16()     # read a raw analog value in the range 0-65535

Constructors
------------

.. class:: ADC(id)

   Access the ADC associated with a source identified by *id*.  This
   *id* may be an integer (usually specifying a channel number), a
   :ref:`Pin <machine.Pin>` object, or other value supported by the
   underlying machine.

Methods
-------

.. method:: ADC.read_u16()

   Take an analog reading and return an integer in the range 0-65535.
   The return value represents the raw reading taken by the ADC, scaled
   such that the minimum value is 0 and the maximum value is 65535.
