.. currentmodule:: machine
.. _machine.Counter:

class Counter-- Pulse Counter
=============================

This class provides access to hardware-supported pulse counting.

It is currently provided for ports:

  * :ref:`ESP32 <esp32_machine.Counter>`
  * :ref:`MIMXRT <mimxrt_machine.Counter>`

Minimal example usage::

    from machine import Pin, Counter

    counter = Counter(0, src=Pin(0, mode=Pin.IN))  # create Counter object and start to count input pulses
    counter.init(filter_ns=1000)                   # switch source filtering on
    value = counter.value(0)                       # get current Counter value, set counter to 0
    counter.deinit()                               # turn off the Counter

    print(counter)                                 # show the Counter object properties

Constructor
-----------

.. class:: Counter(id, src=None, \*, direction=Counter.UP, filter_ns=0)

      - *id*. Values of *id* depend on a particular port and its hardware.
        Values 0, 1, etc. are commonly used to select hardware block #0, #1, etc.

      - *src*. The Counter pulses input pin, which is usually a
        :ref:`machine.Pin <machine.Pin>` object, but a port may allow other values,
        like integers or strings, which designate a Pin in the *machine.Pin* class.
        It may be omitted on ports that have a predefined pin for *id*-specified hardware block.

      - *direction* specifies the direction to count. Values for this include the constants
        ``Counter.UP`` (default value) and ``Counter.DOWN``. Ports may support additional values or
        objects, such as a :ref:`machine.Pin <machine.Pin>` object to control the direction externally.

      - *filter_ns* specifies a minimum period of time in nanoseconds that the source signal needs to
        be stable for a pulse to be counted. Implementations should use the longest filter supported
        by the hardware that is less than or equal to this value. The default is 0 – no filter.

Methods
-------

.. method:: Counter.init(*, src, ...)

   Modify the settings of the Counter object. See the **Constructor** for details about the parameters.

.. method:: Counter.deinit()

   Stops the Counter, disables interrupts and releases hardware resources used by the counter.
   A Soft Reset involve deinitializing all Encoder objects.

.. method:: Counter.value([value])

   Get, and optionally set, the counter value as a signed integer. Implementations should aim to do the get and set atomically.

Constants
---------

.. data:: Counter.UP
          Counter.DOWN

   Select the counter direction.
