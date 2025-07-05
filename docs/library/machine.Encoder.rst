.. currentmodule:: machine
.. _machine.Encoder:

class Encoder -- Quadrature Incremental Encoder
===============================================

This class provides a hardware-supported Quadrature Incremental Encoder service.

If your port does not support hardware encoder use `Quadrature incremental encoder based on machine.Pin interrupts <https://github.com/IhorNehrutsa/MicroPython-quadrature-incremental-encoder>`_.
See also Pin-interrupt-based encoders (and problems) from Peter Hinch `Incremental encoders <https://github.com/peterhinch/micropython-samples/blob/master/encoders/ENCODERS.md>`_.
There is also `Dave Hylands an STM specific hardware-Timer-based solution <https://github.com/dhylands/upy-examples/blob/master/encoder.py>`_.

It is currently provided for ports:

    * :ref:`ESP32 <esp32_machine.Encoder>`
    * :ref:`MIMXRT <mimxrt_machine.Encoder>`

Minimal example usage::

    from machine import Pin, Encoder

    enc = Encoder(id, phase_a=Pin(0), phase_b=Pin(1))  # create Quadrature Encoder object and start to encode input pulses
    enc.init(filter_ns=1000)                           # switch source filtering on
    value = enc.value(0)                               # get current Encoder value, set Encoder to 0
    enc.deinit()                                       # turn off the Encoder

    print(enc)                                         # show the Encoder object properties

Constructor
-----------

.. class:: Encoder(id, phase_a=None, phase_b=None, \*, filter_ns=0)

      - *id*. Values of *id* depend on a particular port and its hardware.
        Values 0, 1, etc. are commonly used to select hardware block #0, #1, etc.

      - *phase_a* and *phase_b* are the Quadrature encoder inputs, which are usually
        :ref:`machine.Pin <machine.Pin>` objects, but a port may allow other values,
        like integers or strings, which designate a Pin in the *machine.Pin* class.
        They may be omitted on ports that have predefined pins for *id*-specified hardware block.

      - *filter_ns* specifies a minimum period of time in nanoseconds that the source signal needs to
        be stable for a pulse to be counted. Implementations should use the longest filter supported
        by the hardware that is less than or equal to this value. The default is 0 – no filter.

Methods
-------

.. method:: Encoder.init(*, phase_a, ...)

   Modify the settings of the Encoder object. See the **Constructor** for details about the parameters.

.. method:: Encoder.deinit()

   Stops the Encoder, disables interrupts and releases hardware resources used by the encoder.
   A Soft Reset involve deinitializing all Encoder objects.

.. method:: Encoder.value([value])

   Get, and optionally set, the counter value as a signed integer. Implementations should aim to do the get and set atomically.
