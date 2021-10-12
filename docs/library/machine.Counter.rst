.. currentmodule:: machine
.. _machine.Counter:

class Counter-- Pulse Counter
=============================

This class provides a Pulse Counter service.

Here is described a basics, commons for hadware-counters-based counters of MicroPython ports:

* :ref:`ESP32.Counter <pcnt.Counter>`
* mimxrt.Counter - under constructions
* STM32.Counter - TODO

Example usage::

    from machine import Pin, Counter

    counter = Counter(Pin(0))  # create Counter object and start to count input pulses
    counter.value()            # get current counter value
    counter.set_value(0)       # Set counter to 0
    counter.deinit()           # turn off the Counter

    print(counter)             # show the Counter object properties

Constructor
-----------

.. class:: Counter(input, \*, keyword_arguments)

    Construct and return a new Counter object using the following parameters:

      - *input*  is the Counter input pin, which is usually a
        :ref:`machine.Pin <machine.Pin>` object, but a port may allow other values,
        like integers or strings, which designate a Pin in the *machine.Pin* class.

    Keyword arguments:

      - *direction*\=value. Specifying the direction of counting. Suitable values are:

        - if value == 0 or False: count down
        - if value != 0 or True: count up
        - a :ref:`machine.Pin <machine.Pin>` object. The level at that pin controls
          the counting direction:

            - if Pin.value() == 0: count down
            - if Pin.value() == 1: count up

      - *scale*\=value. Sets the scale value. The default value is 1.

Methods
-------

.. method:: Counter.init(keyword_arguments)

   Modify settings for the Counter object.  See the above constructor for details
   about the parameters.

.. method:: Counter.deinit()

   Stops the Counter, disables interrupts and releases the resources used by the counter. On
   Soft Reset, all instances of Counter are deinitialized.

.. method:: Counter.value()

   Get the current raw Counter value as a signed integer as fast as possible.

.. method:: Counter.set_value(value)

   Set the current raw Counter value as signed integer.

.. method:: Counter.position([value])

   Get or set the current position of the Counter as signed integer.
   With no arguments the actual position are returned.

   With a single *value* argument the position of Counter is set to that value.

   Pseudocode is::

    def position(self, value=None):
        if value is not None:
            self._value = round(value / self.scale)
        return self._value * self.scale
