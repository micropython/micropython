.. currentmodule:: pyb
.. _pyb.Timer:

class Timer -- control internal timers
======================================

Timers can be used for a great variety of tasks.  At the moment, only
the simplest case is implemented: that of calling a function periodically.

Each timer consists of a counter that counts up at a certain rate.  The rate
at which it counts is the peripheral clock frequency (in Hz) divided by the
timer prescaler.  When the counter reaches the timer period it triggers an
event, and the counter resets back to zero.  By using the callback method,
the timer event can call a Python function.

Example usage to toggle an LED at a fixed frequency::

    tim = pyb.Timer(4)              # create a timer object using timer 4
    tim.init(freq=2)                # trigger at 2Hz
    tim.callback(lambda t:pyb.LED(1).toggle())

Example using named function for the callback::

    def tick(timer):                # we will receive the timer object when being called
        print(timer.counter())      # show current timer's counter value
    tim = pyb.Timer(4, freq=1)      # create a timer object using timer 4 - trigger at 1Hz
    tim.callback(tick)              # set the callback to our tick function

Further examples::

    tim = pyb.Timer(4, freq=100)    # freq in Hz
    tim = pyb.Timer(4, prescaler=0, period=99)
    tim.counter()                   # get counter (can also set)
    tim.prescaler(2)                # set prescaler (can also get)
    tim.period(199)                 # set period (can also get)
    tim.callback(lambda t: ...)     # set callback for update interrupt (t=tim instance)
    tim.callback(None)              # clear callback

*Note:* Timer(2) and Timer(3) are used for PWM to set the intensity of LED(3)
and LED(4) respectively.  But these timers are only configured for PWM if
the intensity of the relevant LED is set to a value between 1 and 254.  If
the intensity feature of the LEDs is not used then these timers are free for
general purpose use.  Similarly, Timer(5) controls the servo driver, and
Timer(6) is used for timed ADC/DAC reading/writing.  It is recommended to
use the other timers in your programs.

*Note:* Memory can't be allocated during a callback (an interrupt) and so
exceptions raised within a callback don't give much information.  See
:func:`micropython.alloc_emergency_exception_buf` for how to get around this
limitation.


Constructors
------------

.. class:: pyb.Timer(id, ...)

   Construct a new timer object of the given id.  If additional
   arguments are given, then the timer is initialised by ``init(...)``.
   ``id`` can be 1 to 14.

Methods
-------

.. method:: Timer.init(*, freq, prescaler, period, mode=Timer.UP, div=1, callback=None, deadtime=0)

   Initialise the timer.  Initialisation must be either by frequency (in Hz)
   or by prescaler and period::

       tim.init(freq=100)                  # set the timer to trigger at 100Hz
       tim.init(prescaler=83, period=999)  # set the prescaler and period directly

   Keyword arguments:

     - ``freq`` --- specifies the periodic frequency of the timer. You might also
       view this as the frequency with which the timer goes through one complete cycle.

     - ``prescaler`` [0-0xffff] - specifies the value to be loaded into the
       timer's Prescaler Register (PSC). The timer clock source is divided by
       (``prescaler + 1``) to arrive at the timer clock. Timers 2-7 and 12-14
       have a clock source of 84 MHz (pyb.freq()[2] \* 2), and Timers 1, and 8-11
       have a clock source of 168 MHz (pyb.freq()[3] \* 2).

     - ``period`` [0-0xffff] for timers 1, 3, 4, and 6-15. [0-0x3fffffff] for timers 2 & 5.
       Specifies the value to be loaded into the timer's AutoReload
       Register (ARR). This determines the period of the timer (i.e. when the
       counter cycles). The timer counter will roll-over after ``period + 1``
       timer clock cycles.

     - ``mode`` can be one of:

       - ``Timer.UP`` - configures the timer to count from 0 to ARR (default)
       - ``Timer.DOWN`` - configures the timer to count from ARR down to 0.
       - ``Timer.CENTER`` - configures the timer to count from 0 to ARR and
         then back down to 0.

     - ``div`` can be one of 1, 2, or 4. Divides the timer clock to determine
       the sampling clock used by the digital filters.

     - ``callback`` - as per Timer.callback()

     - ``deadtime`` - specifies the amount of "dead" or inactive time between
       transitions on complimentary channels (both channels will be inactive)
       for this time). ``deadtime`` may be an integer between 0 and 1008, with
       the following restrictions: 0-128 in steps of 1. 128-256 in steps of
       2, 256-512 in steps of 8, and 512-1008 in steps of 16. ``deadtime``
       measures ticks of ``source_freq`` divided by ``div`` clock ticks.
       ``deadtime`` is only available on timers 1 and 8.

    You must either specify freq or both of period and prescaler.

.. method:: Timer.deinit()

   Deinitialises the timer.

   Disables the callback (and the associated irq).

   Disables any channel callbacks (and the associated irq).
   Stops the timer, and disables the timer peripheral.

.. method:: Timer.callback(fun)

   Set the function to be called when the timer triggers.
   ``fun`` is passed 1 argument, the timer object.
   If ``fun`` is ``None`` then the callback will be disabled.

.. method:: Timer.channel(channel, mode, ...)

   If only a channel number is passed, then a previously initialized channel
   object is returned (or ``None`` if there is no previous channel).

   Otherwise, a TimerChannel object is initialized and returned.

   Each channel can be configured to perform pwm, output compare, or
   input capture. All channels share the same underlying timer, which means
   that they share the same timer clock.

   Keyword arguments:

     - ``mode`` can be one of:

       - ``Timer.PWM`` --- configure the timer in PWM mode (active high).
       - ``Timer.PWM_INVERTED`` --- configure the timer in PWM mode (active low).
       - ``Timer.OC_TIMING`` --- indicates that no pin is driven.
       - ``Timer.OC_ACTIVE`` --- the pin will be made active when a compare match occurs (active is determined by polarity)
       - ``Timer.OC_INACTIVE`` --- the pin will be made inactive when a compare match occurs.
       - ``Timer.OC_TOGGLE`` --- the pin will be toggled when an compare match occurs.
       - ``Timer.OC_FORCED_ACTIVE`` --- the pin is forced active (compare match is ignored).
       - ``Timer.OC_FORCED_INACTIVE`` --- the pin is forced inactive (compare match is ignored).
       - ``Timer.IC`` --- configure the timer in Input Capture mode.
       - ``Timer.ENC_A`` --- configure the timer in Encoder mode. The counter only changes when CH1 changes.
       - ``Timer.ENC_B`` --- configure the timer in Encoder mode. The counter only changes when CH2 changes.
       - ``Timer.ENC_AB`` --- configure the timer in Encoder mode. The counter changes when CH1 or CH2 changes.

     - ``callback`` - as per TimerChannel.callback()

     - ``pin`` None (the default) or a Pin object. If specified (and not None)
       this will cause the alternate function of the the indicated pin
       to be configured for this timer channel. An error will be raised if
       the pin doesn't support any alternate functions for this timer channel.

   Keyword arguments for Timer.PWM modes:

     - ``pulse_width`` - determines the initial pulse width value to use.
     - ``pulse_width_percent`` - determines the initial pulse width percentage to use.

   Keyword arguments for Timer.OC modes:

     - ``compare`` - determines the initial value of the compare register.

     - ``polarity`` can be one of:

       - ``Timer.HIGH`` - output is active high
       - ``Timer.LOW`` - output is active low

   Optional keyword arguments for Timer.IC modes:

     - ``polarity`` can be one of:

       - ``Timer.RISING`` - captures on rising edge.
       - ``Timer.FALLING`` - captures on falling edge.
       - ``Timer.BOTH`` - captures on both edges.

     Note that capture only works on the primary channel, and not on the
     complimentary channels.

   Notes for Timer.ENC modes:

     - Requires 2 pins, so one or both pins will need to be configured to use
       the appropriate timer AF using the Pin API.
     - Read the encoder value using the timer.counter() method.
     - Only works on CH1 and CH2 (and not on CH1N or CH2N)
     - The channel number is ignored when setting the encoder mode.

   PWM Example::

       timer = pyb.Timer(2, freq=1000)
       ch2 = timer.channel(2, pyb.Timer.PWM, pin=pyb.Pin.board.X2, pulse_width=8000)
       ch3 = timer.channel(3, pyb.Timer.PWM, pin=pyb.Pin.board.X3, pulse_width=16000)

.. method:: Timer.counter([value])

   Get or set the timer counter.

.. method:: Timer.freq([value])

   Get or set the frequency for the timer (changes prescaler and period if set).

.. method:: Timer.period([value])

   Get or set the period of the timer.

.. method:: Timer.prescaler([value])

   Get or set the prescaler for the timer.

.. method:: Timer.source_freq()

   Get the frequency of the source of the timer.

class TimerChannel --- setup a channel for a timer
==================================================

Timer channels are used to generate/capture a signal using a timer.

TimerChannel objects are created using the Timer.channel() method.

Methods
-------

.. method:: timerchannel.callback(fun)

   Set the function to be called when the timer channel triggers.
   ``fun`` is passed 1 argument, the timer object.
   If ``fun`` is ``None`` then the callback will be disabled.

.. method:: timerchannel.capture([value])

   Get or set the capture value associated with a channel.
   capture, compare, and pulse_width are all aliases for the same function.
   capture is the logical name to use when the channel is in input capture mode.

.. method:: timerchannel.compare([value])

   Get or set the compare value associated with a channel.
   capture, compare, and pulse_width are all aliases for the same function.
   compare is the logical name to use when the channel is in output compare mode.

.. method:: timerchannel.pulse_width([value])

   Get or set the pulse width value associated with a channel.
   capture, compare, and pulse_width are all aliases for the same function.
   pulse_width is the logical name to use when the channel is in PWM mode.

   In edge aligned mode, a pulse_width of ``period + 1`` corresponds to a duty cycle of 100%
   In center aligned mode, a pulse width of ``period`` corresponds to a duty cycle of 100%

.. method:: timerchannel.pulse_width_percent([value])

   Get or set the pulse width percentage associated with a channel.  The value
   is a number between 0 and 100 and sets the percentage of the timer period
   for which the pulse is active.  The value can be an integer or
   floating-point number for more accuracy.  For example, a value of 25 gives
   a duty cycle of 25%.

Constants
---------

.. data:: Timer.UP
          Timer.DOWN
          Timer.CENTER

   Configures the timer to count Up, Down, or from 0 to ARR and then back down to 0.
