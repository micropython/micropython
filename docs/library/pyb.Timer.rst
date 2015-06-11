.. _pyb.Timer:

class Timer -- control internal timers
======================================

.. only:: port_pyboard

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
    
    *Note:* Timer 3 is reserved for internal use.  Timer 5 controls
    the servo driver, and Timer 6 is used for timed ADC/DAC reading/writing.
    It is recommended to use the other timers in your programs.

.. only:: port_wipy

    Timers can be used for a great variety of tasks, calling a function periodically,
    counting events, and generating a PWM signal are among the most common use cases.
    Each timer consists of 2 16-bit channels and this channels can be tied together to
    form 1 32-bit timer. The operating mode needs to be configured per timer, but then
    the period (or the frequency) can be independently configured on each channel. 
    By using the callback method, the timer event can call a Python function.

    Example usage to toggle an LED at a fixed frequency::

        tim = pyb.Timer(4)                                              # create a timer object using timer 4
        tim.init(mode=Timer.PERIODIC)                                   # initialize it in periodic mode
        tim_ch = tim.channel(Timer.A, freq=2)                           # configure channel A at a frequency of 2Hz
        tim_ch.callback(handler=lambda t:led.toggle())                  # toggle a LED on every cycle of the timer

    Example using named function for the callback::

        tim = Timer(1, mode=Timer.PERIODIC)
        tim_a = tim.channel(Timer.A, freq=1000)

        led = Pin('GPIO2', af=0, mode=Pin.OUT)

        def tick(timer):                # we will receive the timer object when being called
            print(timer.time())         # show current timer's time value (is microseconds)
            led.toggle()                # toggle the LED

        tim_a.callback(handler=tick)

    Further examples::

        tim1 = pyb.Timer(2, mode=Timer.EVENT_COUNT)                     # initialize it capture mode
        tim2 = pyb.Timer(1, mode=Timer.PWM)                             # initialize it in PWM mode
        tim_ch = tim1.channel(Timer.A, freq=1, polarity=Timer.POSITIVE) # start the event counter with a frequency of 1Hz and triggered by positive edges
        tim_ch = tim2.channel(Timer.B, freq=10000, duty_cycle=50)       # start the PWM on channel B with a 50% duty cycle
        tim_ch.time()                                                   # get the current time in usec (can also be set)
        tim_ch.freq(20)                                                 # set the frequency (can also get)
        tim_ch.duty_cycle(30)                                           # set the duty cycle to 30% (can also get)
        tim_ch.duty_cycle(30, Timer.NEGATIVE)                           # set the duty cycle to 30% and change the polarity to negative
        tim_ch.event_count()                                            # get the number of captured events
        tim_ch.event_time()                                             # get the the time of the last captured event
        tim_ch.period(2000000)                                          # change the period to 2 seconds


*Note:* Memory can't be allocated during a callback (an interrupt) and so
exceptions raised within a callback don't give much information.  See
:func:`micropython.alloc_emergency_exception_buf` for how to get around this
limitation.


Constructors
------------

.. class:: pyb.Timer(id, ...)

    .. only:: port_pyboard
    
       Construct a new timer object of the given id.  If additional
       arguments are given, then the timer is initialised by ``init(...)``.
       ``id`` can be 1 to 14, excluding 3.

    .. only:: port_wipy
    
       Construct a new timer object of the given id.  If additional
       arguments are given, then the timer is initialised by ``init(...)``.
       ``id`` can be 1 to 4.


Methods
-------

.. only:: port_pyboard

    .. method:: timer.init(\*, freq, prescaler, period)
    
       Initialise the timer.  Initialisation must be either by frequency (in Hz)
       or by prescaler and period::
       
           tim.init(freq=100)                  # set the timer to trigger at 100Hz
           tim.init(prescaler=83, period=999)  # set the prescaler and period directly
       
       Keyword arguments:
       
         - ``freq`` --- specifies the periodic frequency of the timer. You migh also
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
           - ``Timer.CENTER`` - confgures the timer to count from 0 to ARR and
             then back down to 0.
       
         - ``div`` can be one of 1, 2, or 4. Divides the timer clock to determine
           the sampling clock used by the digital filters.
       
         - ``callback`` - as per Timer.callback()
       
         - ``deadtime`` - specifies the amount of "dead" or inactive time between
           transitions on complimentary channels (both channels will be inactive)
           for this time). ``deadtime`` may be an integer between 0 and 1008, with
           the following restrictions: 0-128 in steps of 1. 128-256 in steps of
           2, 256-512 in steps of 8, and 512-1008 in steps of 16. ``deadime``
           measures ticks of ``source_freq`` divided by ``div`` clock ticks.
           ``deadtime`` is only available on timers 1 and 8.
       
        You must either specify freq or both of period and prescaler.

.. only:: port_wipy

    .. method:: timer.init(mode, \*, width=16)
    
       Initialise the timer. Example::
       
           tim.init(Timer.PERIODIC)             # periodic 16-bit timer
           tim.init(Timer.ONE_SHOT, width=32)   # one shot 32-bit timer

       Keyword arguments:
       
         - ``mode`` can be one of:
         
           - ``Timer.ONE_SHOT`` - The timer runs once until the configured 
             period of the channel expires.
           - ``Timer.PERIODIC`` - The timer runs periodically at the configured 
             frequency of the channel.
           - ``Timer.EDGE_TIME`` - Meaure the time pin level changes.
           - ``Timer.EDGE_COUNT`` - Count the number of pin level changes.

         - ``width`` must be either 16 or 32 (bits). For really low frequencies <= ~1Hz
           (or large periods), 32-bit timers should be used. 32-bit mode is only available
           for ``ONE_SHOT`` AND ``PERIODIC`` modes.

.. method:: timer.deinit()

   Deinitialises the timer.
   
   .. only:: port_pyboard

      Disables the callback (and the associated irq).

   Disables any channel callbacks (and the associated irq).
   Stops the timer, and disables the timer peripheral.

.. only:: port_pyboard

    .. method:: timer.callback(fun)
    
       Set the function to be called when the timer triggers.
       ``fun`` is passed 1 argument, the timer object.
       If ``fun`` is ``None`` then the callback will be disabled.

.. only:: port_pyboard

    .. method:: timer.channel(channel, mode, ...)
    
       If only a channel number is passed, then a previously initialized channel
       object is returned (or ``None`` if there is no previous channel).
       
       Othwerwise, a TimerChannel object is initialized and returned.
       
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
           - ``Timer.LOW`` - output is acive low
       
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

.. only:: port_wipy

    .. method:: timer.channel(channel, \**, freq, period, polarity=Timer.POSITIVE, duty_cycle=0)
    
       If only a channel identifier passed, then a previously initialized channel
       object is returned (or ``None`` if there is no previous channel).
       
       Othwerwise, a TimerChannel object is initialized and returned.
       
       The operating mode is is the one configured to the Timer object that was used to
       create the channel.

       - ``channel`` if the width of the timer is 16-bit, then must be either ``TIMER.A``, ``TIMER.B``. 
         If the width is 32-bit then it **must be** ``TIMER.A | TIMER.B``.

       Keyword only arguments:

         - ``freq`` sets the frequency in Hz.
         - ``period`` sets the period in microseconds.

         .. note::

            Either ``freq`` or ``period`` must be given, never both.

         - ``polarity`` this is applicable for:
           
           - ``PWM``, defines the polarity of the duty cycle
           - ``EDGE_TIME`` and ``EDGE_COUNT``, defines the polarity of the pin level change to detect.
             To detect both rising and falling edges, make ``polarity=Timer.POSITIVE | Timer.NEGATIVE``.
         - ``duty_cycle`` only applicable to ``PWM``. It's a percentage (0-100)

.. only:: port_pyboard

    .. method:: timer.counter([value])

       Get or set the timer counter.

.. only:: port_pyboard

    .. method:: timer.freq([value])
    
       Get or set the frequency for the timer (changes prescaler and period if set).

.. only:: port_pyboard

    .. method:: timer.period([value])
    
       Get or set the period of the timer.
    
    .. method:: timer.prescaler([value])
    
       Get or set the prescaler for the timer.
    
    .. method:: timer.source_freq()
    
       Get the frequency of the source of the timer.

class TimerChannel --- setup a channel for a timer
==================================================

Timer channels are used to generate/capture a signal using a timer.

TimerChannel objects are created using the Timer.channel() method.

Methods
-------

.. only:: port_pyboard

    .. method:: timerchannel.callback(fun)

       Set the function to be called when the timer channel triggers.
       ``fun`` is passed 1 argument, the timer object.
       If ``fun`` is ``None`` then the callback will be disabled.

.. only:: port_wipy

    .. method:: timerchannel.callback(\**, value, priority=1, handler=None)

        The behavior of this callback is heaviliy dependent on the operating
        mode of the timer channel:

            - If mode is ``Timer.PERIODIC`` the callback is executed periodically
              with the configured frequency or period.
            - If mode is ``Timer.ONE_SHOT`` the callback is executed once when
              the configured timer expires.
            - If mode is ``Timer.EDGE_COUNT`` the callback is executed when reaching
              the configured number of events (see ``value`` param below).
            - If mode is ``Timer.PWM`` the callback is executed when reaching the duty
              cycle value.

        The accepted params are:

            - ``priority`` level of the interrupt. Can take values in the range 1-7.
              Higher values represent higher priorities.
            - ``handler`` is an optional function to be called when the interrupt is triggered.
            - ``value`` is **only valid** when in ``Timer.EDGE_COUNT`` mode and is used to set
              the number of edge events that will trigger the interrupt.

        Returns a callback object.

.. only:: port_pyboard

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

.. only:: port_wipy

    .. method:: timerchannel.freq([value])
    
       Get or set the timer channel frequency (in Hz).

    .. method:: timerchannel.period([value])

       Get or set the timer channel period (in microseconds).
       
    .. method:: timerchannel.time([value])

       Get or set the timer channel current **time** value (in microseconds).
    
    .. method:: timerchannel.event_count()

       Get the number of edge events counted.

    .. method:: timerchannel.event_time()

       Get the time of ocurrance of the last event.

    .. method:: timerchannel.duty_cycle([value])
     
       Get or set the duty cycle of the PWM signal (in the range of 0-100).
