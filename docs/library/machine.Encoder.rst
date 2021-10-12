.. currentmodule:: machine
.. _machine.Encoder:

class Encoder -- Quadrature Incremental Encoder
===============================================

This class provides an Quadrature Incremental Encoder service.
See `Incremental encoder <https://en.wikipedia.org/wiki/Incremental_encoder>`_.

Here is described a basics, commons for an Pin-IRQ-based encoders
`Incremental encoders <https://github.com/peterhinch/micropython-samples/blob/master/encoders/ENCODERS.md>`_
and hadware-counters-based encoders of MicroPython ports:

* :ref:`ESP32.Encoder <pcnt.Encoder>`
* mimxrt.Encoder - under constructions
* STM32.Encoder - TODO

Example usage::

    from machine import Pin, Encoder

    enc = Encoder(Pin(0), Pin(1))  # create Quadrature Encoder object and start to encode input pulses
    enc.value()                    # get current raw encoder value
    enc.set_value(0)               # set raw encoder value to 0
    enc.deinit()                   # turn off the Qudrature encoder

    print(enc)                     # show the Encoder object properties

Constructor
-----------


.. class:: Encoder(input_a, input_b, \*, keyword_arguments)

    Construct and return a new quadrature encoder object using the following parameters:

      - *input_a* and *input_b* are the Quadrature encoder inputs, which are usually
        :ref:`machine.Pin <machine.Pin>` objects, but a port may allow other values,
        like integers or strings, which designate a Pin in the *machine.Pin* class.

    Keyword arguments:

      - *scale*\=value. Sets the scale value. The default value is 1.

Methods
-------

.. method:: Encoder.init(keyword_arguments)

   Modify settings for the Encoder object.  See the above constructor for details
   about the parameters.

.. method:: Encoder.deinit()

   Stops the Encoder, disables interrupts and releases the resources used by the encoder. On
   Soft Reset, all instances of Encoder are deinitialized.

.. method:: Encoder.value()

   Get the current raw Encoder value as a signed integer as fast as possible.

.. method:: Encoder.set_value(value)

   Set the current raw Encoder value as signed integer.

.. method:: Encoder.position([value])

   Get or set the current position of the Encoder as signed integer.
   With no arguments the actual position are returned.

   With a single *value* argument the position of Encoder is set to that value.
   Pseudocode is::

    def position(self, value=None):
        if value is not None:
            self._value = round(value / self.scale)
        return self._value * self.scale

The *scale* parameter allows to get *Encoder.position()* in different units.::

    ...
    PPR = 30  # pulses per revolution of the encoder shaft

    enc = Encoder(Pin(0), Pin(1), scale=360 / PPR)  # degreses
    print('degreses', enc.position())

    enc.init(scale=2 * 3.141592 / PPR)              # radians
    print('radians', enc.position())

    enc.init(scale=1 / PPR)                         # rotations
    print('rotations', enc.position())
    ...

Simple check of Encoder performance
`encoders_test.py <https://github.com/IhorNehrutsa/micropython-samples/blob/encoders/encoders/encoders_test.py>`_
