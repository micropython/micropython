Hardware timers
===============

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


Additional constants for Timer class
------------------------------------

.. data:: Timer.PWM

   PWM timer operating mode.

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
