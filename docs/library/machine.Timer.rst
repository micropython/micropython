.. currentmodule:: machine
.. _machine.Timer:

class Timer -- control hardware timers
======================================

Timer class provides the ability to trigger a Python callback function after an
expiry time, or periodically at a regular interval.

The available features and restrictions of Timer objects vary depending on the
MicroPython board and port.

If you are using a WiPy board please refer to :ref:`machine.TimerWiPy <machine.TimerWiPy>`
instead of this class.

Timer Types
-----------

There are two types of Timer in MicroPython, but not all ports support both:

- Virtual timers. These are managed in software, and are generally more
  flexible. Multiple virtual timers can be constructed and active at once. The
  ``id`` of a virtual timer is ``-1``. Not all ports support virtual timers, but
  it's recommended to use them when available.
- Hardware timers. Hardware timers have integer ``id`` values starting at ``0``.
  The number of available ``id`` values is determined by the hardware. Hardware
  timers may be more accurate for very fine sub-millisecond timing (especially
  when ``hard=True`` is supported and set, see :ref:`isr_rules`.) Most
  microcontroller ports support hardware timers, except Zephyr and RP2 which
  only support virtual timers.

Constructors
------------

.. class:: Timer(id, /, ...)

   Construct a new Timer object with the given ``id``.

   On ports which support virtual timers the ``id`` parameter is optional - the
   default value is ``-1`` which constructs a virtual timer.

   On ports which support hardware timers, setting the ``id`` parameter to a
   non-negative integer determines which timer to use.

   ``id`` shall not be passed as a keyword argument.

   Any additional parameters are handled the same as :func:`Timer.init()`.

Methods
-------

.. method:: Timer.init(*, mode=Timer.PERIODIC, freq=-1, period=-1, callback=None, hard=True)

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

     - ``hard`` can be one of:

       - ``True`` - The callback will be executed in hard interrupt context,
         which minimises delay and jitter but is subject to the limitations
         described in :ref:`isr_rules`. Not all ports support hard interrupts,
         see the port documentation for more information.
       - ``False`` - The callback will be scheduled as a soft interrupt,
         allowing it to allocate but possibly also introducing
         garbage-collection delays and jitter.

       The default value of this parameter is port-specific for historical reasons.

.. method:: Timer.deinit()

   Deinitialises the timer. Stops the timer, and disables the timer peripheral.

Constants
---------

.. data:: Timer.ONE_SHOT
          Timer.PERIODIC

   Timer operating mode.
