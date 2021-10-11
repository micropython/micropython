.. currentmodule:: machine
.. _machine.Counter:

class Counter-- Pulse Counter
==============================================

This class provides a Pulse Counter service.

Example usage::

    from machine import Pin, Counter

    counter = Counter(Pin(0))             # create Counter object
    counter.values()                      # get current counter value
    counter.values(0)                     # Set all counters to 0
    counter.deinit()                      # turn off the Counter

    counter                               # show the Counter object properties

Constructors
------------

.. class:: Counter(input, \*, keyword_arguments)

    Construct and return a new Counter object using the following parameters:

      - *input*  is the Counter input pin, which is usually a
        :ref:`machine.Pin <machine.Pin>` object, but a port may allow other values,
        like integers or strings, which designate a Pin in the machine.PIN class.

    Keyword arguments:

      - *direction*\=value. Specifying the direction of counting. Suitable values are:

        - Counter.UP:   Count up, with a roll-over to 0 at 2**48-1.
        - Counter.DOWN: Count down, with a roll-under to 2**48-1 at 0.
        - a :ref:`machine.Pin <machine.Pin>` object. The level at that pin controls
          the counting direction. Low: Count up, High: Count down.

Methods
-------

.. method:: Counter.init(keyword_arguments)

   Modify settings for the Counter object.  See the above constructor for details
   about the parameters.

.. method:: Counter.deinit()

   Stops the Counter, disables interrupts and releases the resources used by the encoder. On
   Soft Reset, all instances of Encoder and Counter are deinitialized.

.. method:: Counter.position([value])

   Get or set the current counter value of the Counter. The value is returned as a unsigned 48 bit
   integer.

   With a single *value* argument the counter is set to that value.
