.. currentmodule:: machine

class Timer -- control internal timers
======================================

.. only:: port_wipy

    Timers can be used for a great variety of tasks, calling a function periodically,
    counting events, and generating a PWM signal are among the most common use cases.
    Each timer consists of two 16-bit channels and this channels can be tied together to
    form one 32-bit timer. The operating mode needs to be configured per timer, but then
    the period (or the frequency) can be independently configured on each channel. 
    By using the callback method, the timer event can call a Python function.

    Example usage to toggle an LED at a fixed frequency::

        from machine import Timer
        from machine import Pin
        led = Pin('GP16', mode=Pin.OUT)                  # enable GP16 as output to drive the LED
        tim = Timer(3)                                   # create a timer object using timer 3
        tim.init(mode=Timer.PERIODIC)                    # initialize it in periodic mode
        tim_ch = tim.channel(Timer.A, freq=5)            # configure channel A at a frequency of 5Hz
        tim_ch.irq(handler=lambda t:led.toggle(), trigger=Timer.TIMEOUT)        # toggle a LED on every cycle of the timer

    Example using named function for the callback::

        from machine import Timer
        from machine import Pin
        tim = Timer(1, mode=Timer.PERIODIC, width=32)
        tim_a = tim.channel(Timer.A | Timer.B, freq=1)   # 1 Hz frequency requires a 32 bit timer

        led = Pin('GP16', mode=Pin.OUT) # enable GP16 as output to drive the LED

        def tick(timer):                # we will receive the timer object when being called
            global led
            led.toggle()                # toggle the LED

        tim_a.irq(handler=tick, trigger=Timer.TIMEOUT)         # create the interrupt

    Further examples::

        from machine import Timer
        tim1 = Timer(1, mode=Timer.ONE_SHOT)                               # initialize it in one shot mode
        tim2 = Timer(2, mode=Timer.PWM)                                    # initialize it in PWM mode
        tim1_ch = tim1.channel(Timer.A, freq=10, polarity=Timer.POSITIVE)  # start the event counter with a frequency of 10Hz and triggered by positive edges
        tim2_ch = tim2.channel(Timer.B, freq=10000, duty_cycle=5000)       # start the PWM on channel B with a 50% duty cycle
        tim2_ch.freq(20)                                                   # set the frequency (can also get)
        tim2_ch.duty_cycle(3010)                                           # set the duty cycle to 30.1% (can also get)
        tim2_ch.duty_cycle(3020, Timer.NEGATIVE)                           # set the duty cycle to 30.2% and change the polarity to negative
        tim2_ch.period(2000000)                                            # change the period to 2 seconds

.. note::

    Memory can't be allocated inside irq handlers (an interrupt) and so
    exceptions raised within a handler don't give much information.  See
    :func:`micropython.alloc_emergency_exception_buf` for how to get around this
    limitation.

Constructors
------------

.. class:: Timer(id, ...)

    .. only:: port_wipy

       Construct a new timer object of the given id. ``id`` can take values from 0 to 3.


Methods
-------

.. only:: port_wipy

    .. method:: Timer.init(mode, \*, width=16)

       Initialise the timer. Example::

           tim.init(Timer.PERIODIC)             # periodic 16-bit timer
           tim.init(Timer.ONE_SHOT, width=32)   # one shot 32-bit timer

       Keyword arguments:
       
         - ``mode`` can be one of:
         
           - ``Timer.ONE_SHOT`` - The timer runs once until the configured 
             period of the channel expires.
           - ``Timer.PERIODIC`` - The timer runs periodically at the configured 
             frequency of the channel.
           - ``Timer.PWM``      - Output a PWM signal on a pin.

         - ``width`` must be either 16 or 32 (bits). For really low frequencies < 5Hz
           (or large periods), 32-bit timers should be used. 32-bit mode is only available
           for ``ONE_SHOT`` AND ``PERIODIC`` modes.

.. method:: Timer.deinit()

   Deinitialises the timer. Disables all channels and associated IRQs.
   Stops the timer, and disables the timer peripheral.

.. only:: port_wipy

    .. method:: Timer.channel(channel, \**, freq, period, polarity=Timer.POSITIVE, duty_cycle=0)
    
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

         - ``polarity`` this is applicable for ``PWM``, and defines the polarity of the duty cycle
         - ``duty_cycle`` only applicable to ``PWM``. It's a percentage (0.00-100.00). Since the WiPy
           doesn't support floating point numbers the duty cycle must be specified in the range 0-10000,
           where 10000 would represent 100.00, 5050 represents 50.50, and so on.

       .. note::

          When the channel is in PWM mode, the corresponding pin is assigned automatically, therefore
          there's no need to assign the alternate function of the pin via the ``Pin`` class. The pins which
          support PWM functionality are the following:

          - ``GP24`` on Timer 0 channel A.
          - ``GP25`` on Timer 1 channel A.
          - ``GP9``  on Timer 2 channel B.
          - ``GP10`` on Timer 3 channel A.
          - ``GP11`` on Timer 3 channel B.

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
            - ``trigger`` must be ``Timer.TIMEOUT`` when the operating mode is either ``Timer.PERIODIC`` or
              ``Timer.ONE_SHOT``. In the case that mode is ``Timer.PWM`` then trigger must be equal to
              ``Timer.MATCH``.

        Returns a callback object.

.. only:: port_wipy

    .. method:: timerchannel.freq([value])

       Get or set the timer channel frequency (in Hz).

    .. method:: timerchannel.period([value])

       Get or set the timer channel period (in microseconds).

    .. method:: timerchannel.duty_cycle([value])

       Get or set the duty cycle of the PWM signal. It's a percentage (0.00-100.00). Since the WiPy
       doesn't support floating point numbers the duty cycle must be specified in the range 0-10000,
       where 10000 would represent 100.00, 5050 represents 50.50, and so on.

Constants
---------

.. data:: Timer.ONE_SHOT
.. data:: Timer.PERIODIC
.. data:: Timer.PWM

   Selects the timer operating mode.

.. data:: Timer.A
.. data:: Timer.B

   Selects the timer channel. Must be ORed (``Timer.A`` | ``Timer.B``) when
   using a 32-bit timer.

.. data:: Timer.POSITIVE
.. data:: Timer.NEGATIVE

   Timer channel polarity selection (only relevant in PWM mode).

.. data:: Timer.TIMEOUT
.. data:: Timer.MATCH

   Timer channel IRQ triggers.
