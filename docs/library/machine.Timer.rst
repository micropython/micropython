.. _machine.Timer:

class Timer -- control internal timers
======================================

.. only:: port_wipy

    .. note::

        Contrary with the rest of the API, timer IDs start at 1, not a t zero. This is because
        the ``Timer`` API is still provisional. A new MicroPython wide API will come soon.

    Timers can be used for a great variety of tasks, calling a function periodically,
    counting events, and generating a PWM signal are among the most common use cases.
    Each timer consists of 2 16-bit channels and this channels can be tied together to
    form 1 32-bit timer. The operating mode needs to be configured per timer, but then
    the period (or the frequency) can be independently configured on each channel. 
    By using the callback method, the timer event can call a Python function.

    Example usage to toggle an LED at a fixed frequency::

        from machine import Timer
        tim = Timer(4)                                   # create a timer object using timer 4
        tim.init(mode=Timer.PERIODIC)                    # initialize it in periodic mode
        tim_ch = tim.channel(Timer.A, freq=2)            # configure channel A at a frequency of 2Hz
        tim_ch.callback(handler=lambda t:led.toggle())   # toggle a LED on every cycle of the timer

    Example using named function for the callback::

        from machine import Timer
        tim = Timer(1, mode=Timer.PERIODIC)
        tim_a = tim.channel(Timer.A, freq=1000)

        led = Pin('GPIO2', mode=Pin.OUT)

        def tick(timer):                # we will receive the timer object when being called
            print(timer.time())         # show current timer's time value (is microseconds)
            led.toggle()                # toggle the LED

        tim_a.callback(handler=tick)

    Further examples::

        from machine import Timer
        tim1 = Timer(2, mode=Timer.EVENT_COUNT)                         # initialize it capture mode
        tim2 = Timer(1, mode=Timer.PWM)                                 # initialize it in PWM mode
        tim_ch = tim1.channel(Timer.A, freq=1, polarity=Timer.POSITIVE) # start the event counter with a frequency of 1Hz and triggered by positive edges
        tim_ch = tim2.channel(Timer.B, freq=10000, duty_cycle=50)       # start the PWM on channel B with a 50% duty cycle
        tim_ch.time()                                                   # get the current time in usec (can also be set)
        tim_ch.freq(20)                                                 # set the frequency (can also get)
        tim_ch.duty_cycle(30)                                           # set the duty cycle to 30% (can also get)
        tim_ch.duty_cycle(30, Timer.NEGATIVE)                           # set the duty cycle to 30% and change the polarity to negative
        tim_ch.event_count()                                            # get the number of captured events
        tim_ch.event_time()                                             # get the the time of the last captured event
        tim_ch.period(2000000)                                          # change the period to 2 seconds


.. note::

    Memory can't be allocated inside irq handlers (an interrupt) and so
    exceptions raised within a handler don't give much information.  See
    :func:`micropython.alloc_emergency_exception_buf` for how to get around this
    limitation.

Constructors
------------

.. class:: machine.Timer(id, ...)

    .. only:: port_wipy

       Construct a new timer object of the given id.  If additional
       arguments are given, then the timer is initialised by ``init(...)``.
       ``id`` can be 1 to 4.


Methods
-------

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

   Deinitialises the timer. Disables all channels and associated IRQs.
   Stops the timer, and disables the timer peripheral.

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

class TimerChannel --- setup a channel for a timer
==================================================

Timer channels are used to generate/capture a signal using a timer.

TimerChannel objects are created using the Timer.channel() method.

Methods
-------

.. only:: port_wipy

    .. method:: timerchannel.irq(\*, trigger, priority=1, handler=None)

        The behavior of this callback is heaviliy dependent on the operating
        mode of the timer channel:

            - If mode is ``Timer.PERIODIC`` the callback is executed periodically
              with the configured frequency or period.
            - If mode is ``Timer.ONE_SHOT`` the callback is executed once when
              the configured timer expires.
            - If mode is ``Timer.PWM`` the callback is executed when reaching the duty
              cycle value.

        The accepted params are:

            - ``priority`` level of the interrupt. Can take values in the range 1-7.
              Higher values represent higher priorities.
            - ``handler`` is an optional function to be called when the interrupt is triggered.

        Returns a callback object.

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
