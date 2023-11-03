.. currentmodule:: machine
.. _machine.Timer:

class Timer -- control hardware timers
======================================

Hardware timers deal with timing of periods and events. Timers are perhaps
the most flexible and heterogeneous kind of hardware in MCUs and SoCs,
differently greatly from a model to a model. MicroPython's Timer class
defines a baseline operation of executing a callback with a given period
(or once after some delay), and allow specific boards to define more
non-standard behaviour (which thus won't be portable to other boards).

See discussion of :ref:`important constraints <machine_callbacks>` on
Timer callbacks.

.. note::

    Memory can't be allocated inside irq handlers (an interrupt) and so
    exceptions raised within a handler don't give much information.  See
    :func:`micropython.alloc_emergency_exception_buf` for how to get around this
    limitation.

If you are using a WiPy board please refer to :ref:`machine.TimerWiPy <machine.TimerWiPy>`
instead of this class.

Constructors
------------

.. class:: Timer(id, /, ...)

   Construct a new timer object of the given ``id``. ``id`` of -1 constructs a
   virtual timer (if supported by a board).
   ``id`` shall not be passed as a keyword argument.

   See ``init`` for parameters of initialisation.

Methods
-------

.. method:: Timer.init(*, mode=Timer.PERIODIC, freq=-1, period=-1, callback=None)

   Initialise the timer. Example::

       def mycallback(t):
           pass

       # periodic at 1kHz
       tim.init(mode=Timer.PERIODIC, freq=1000, callback=mycallback)

       # periodic with 100ms period
       tim.init(period=100, callback=mycallback)

       # one shot firing after 1000ms
       tim.init(mode=Timer.ONE_SHOT, period=1000, callback=mycallback)

   Keyword arguments:

     - ``mode`` can be one of:

       - ``Timer.ONE_SHOT`` - The timer runs once until the configured
         period of the channel expires.
       - ``Timer.PERIODIC`` - The timer runs periodically at the configured
         frequency of the channel.

     - ``freq`` - The timer frequency, in units of Hz.  The upper bound of
       the frequency is dependent on the port.  When both the ``freq`` and
       ``period`` arguments are given, ``freq`` has a higher priority and
       ``period`` is ignored.

     - ``period`` - The timer period, in milliseconds.

     - ``callback`` - The callable to call upon expiration of the timer period.
       The callback must take one argument, which is passed the Timer object.
       The ``callback`` argument shall be specified. Otherwise an exception
       will occur upon timer expiration:
       ``TypeError: 'NoneType' object isn't callable``

.. method:: Timer.deinit()

   Deinitialises the timer. Stops the timer, and disables the timer peripheral.

Constants
---------

.. data:: Timer.ONE_SHOT
          Timer.PERIODIC

   Timer operating mode.
