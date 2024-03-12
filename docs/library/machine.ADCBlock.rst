.. currentmodule:: machine
.. _machine.ADCBlock:

class ADCBlock -- control ADC peripherals
=========================================

The ADCBlock class provides access to an ADC peripheral which has a
number of channels that can be used to sample analog values.  It allows
finer control over configuration of :ref:`machine.ADC <machine.ADC>`
objects, which do the actual sampling.

This class is not always available.

Example usage::

   from machine import ADCBlock

   block = ADCBlock(id, bits=12)  # create an ADCBlock with 12-bit resolution
   adc = block.connect(4, pin)    # connect channel 4 to the given pin
   val = adc.read_uv()            # read an analog value

Constructors
------------

.. class:: ADCBlock(id, *, bits)

   Access the ADC peripheral identified by *id*, which may be an integer
   or string.

   The *bits* argument, if given, sets the resolution in bits of the
   conversion process.  If not specified then the previous or default
   resolution is used.

Methods
-------

.. method:: ADCBlock.init(*, bits)

   Configure the ADC peripheral.  *bits* will set the resolution of the
   conversion process.

.. method:: ADCBlock.connect(channel, *, ...)
            ADCBlock.connect(source, *, ...)
            ADCBlock.connect(channel, source, *, ...)

   Connect up a channel on the ADC peripheral so it is ready for sampling,
   and return an :ref:`ADC <machine.ADC>` object that represents that connection.

   The *channel* argument must be an integer, and *source* must be an object
   (for example a :ref:`Pin <machine.Pin>`) which can be connected up for sampling.

   If only *channel* is given then it is configured for sampling.

   If only *source* is given then that object is connected to a default
   channel ready for sampling.

   If both *channel* and *source* are given then they are connected together
   and made ready for sampling.

   Any additional keyword arguments are used to configure the returned ADC object,
   via its :meth:`init <machine.ADC.init>` method.
