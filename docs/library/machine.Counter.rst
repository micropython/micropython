.. currentmodule:: machine
.. _machine.Counter:

class Counter -- pulse counter
==============================

Counter implements pulse counting by monitoring an input signal and counting
rising or falling edges.

Minimal example usage::

    from machine import Pin, Counter

    counter = Counter(0, Pin(0, Pin.IN))  # create Counter for pin 0 and begin counting
    value = counter.value()               # retrieve current pulse count

Availability: esp32 port

Constructors
------------

.. class:: Counter(id, ...)

   Construct a Counter object with the given id. Values of *id* depend on a
   particular port and its hardware. Values 0, 1, etc. are commonly used to
   select hardware block #0, #1, etc. Additional arguments are passed to the
   ``init()`` method described below.

Methods
-------

.. method:: Counter.init(src, *, ...)

   Initialise the Counter with the given parameters:

   - *src* specifies the input pin as a :ref:`machine.Pin <machine.Pin>` object.
     May be omitted on ports that have a predefined pin for a given hardware
     block.

   Additional keyword-only parameters that may be supported by a port are:

   - *edge* specifies the edge to count. Either ``Counter.RISING`` (the default)
     or ``Counter.FALLING``.

   - *direction* specifies the direction to count. Either ``Counter.UP`` (the
     default) or ``Counter.DOWN``.

   - *filter_ns* specifies a minimum period of time in nanoseconds that the
     source signal needs to be stable for a pulse to be counted. Implementations
     should use the longest filter supported by the hardware that is less than
     or equal to this value. The default is 0 (no filter).

.. method:: Counter.deinit()

   Stops the Counter, disabling any interrupts and releasing hardware resources.
   A Soft Reset should deinitialize all Counter objects.

.. method:: Counter.value([value])

   Get, and optionally set, the counter value as a signed integer.
   Implementations should aim to do the get and set atomically.

Constants
---------

.. data:: Counter.RISING
          Counter.FALLING

   Select the pulse edge.

.. data:: Counter.UP
          Counter.DOWN

   Select the counting direction.
