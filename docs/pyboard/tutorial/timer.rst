The Timers
==========

The pyboard has 14 timers which each consist of an independent counter
running at a user-defined frequency.  They can be set up to run a function
at specific intervals.
The 14 timers are numbered 1 through 14, but 3 is reserved
for internal use, and 5 and 6 are used for servo and ADC/DAC control.
Avoid using these timers if possible.

Let's create a timer object::

    >>> tim = pyb.Timer(4)

Now let's see what we just created::

    >>> tim
    Timer(4)

The pyboard is telling us that ``tim`` is attached to timer number 4, but
it's not yet initialised.  So let's initialise it to trigger at 10 Hz
(that's 10 times per second)::

    >>> tim.init(freq=10)

Now that it's initialised, we can see some information about the timer::

    >>> tim
    Timer(4, prescaler=624, period=13439, mode=UP, div=1)

The information means that this timer is set to run at the peripheral
clock speed divided by 624+1, and it will count from 0 up to 13439, at which
point it triggers an interrupt, and then starts counting again from 0.  These
numbers are set to make the timer trigger at 10 Hz: the source frequency
of the timer is 84MHz (found by running ``tim.source_freq()``) so we
get 84MHz / 625 / 13440 = 10Hz.

Timer counter
-------------

So what can we do with our timer?  The most basic thing is to get the
current value of its counter::

    >>> tim.counter()
    21504

This counter will continuously change, and counts up.

Timer callbacks
---------------

The next thing we can do is register a callback function for the timer to
execute when it triggers (see the [switch tutorial](tut-switch) for an
introduction to callback functions)::

    >>> tim.callback(lambda t:pyb.LED(1).toggle())

This should start the red LED flashing right away.  It will be flashing
at 5 Hz (2 toggle's are needed for 1 flash, so toggling at 10 Hz makes
it flash at 5 Hz).  You can change the frequency by re-initialising the
timer::

    >>> tim.init(freq=20)

You can disable the callback by passing it the value ``None``::

    >>> tim.callback(None)

The function that you pass to callback must take 1 argument, which is
the timer object that triggered.  This allows you to control the timer
from within the callback function.

We can create 2 timers and run them independently::

    >>> tim4 = pyb.Timer(4, freq=10)
    >>> tim7 = pyb.Timer(7, freq=20)
    >>> tim4.callback(lambda t: pyb.LED(1).toggle())
    >>> tim7.callback(lambda t: pyb.LED(2).toggle())

Because the callbacks are proper hardware interrupts, we can continue
to use the pyboard for other things while these timers are running.

Making a microsecond counter
----------------------------

You can use a timer to create a microsecond counter, which might be
useful when you are doing something which requires accurate timing.
We will use timer 2 for this, since timer 2 has a 32-bit counter (so
does timer 5, but if you use timer 5 then you can't use the Servo
driver at the same time).

We set up timer 2 as follows::

    >>> micros = pyb.Timer(2, prescaler=83, period=0x3fffffff)

The prescaler is set at 83, which makes this timer count at 1 MHz.
This is because the CPU clock, running at 168 MHz, is divided by
2 and then by prescaler+1, giving a freqency of 168 MHz/2/(83+1)=1 MHz
for timer 2.  The period is set to a large number so that the timer
can count up to a large number before wrapping back around to zero.
In this case it will take about 17 minutes before it cycles back to
zero.

To use this timer, it's best to first reset it to 0::

    >>> micros.counter(0)

and then perform your timing::

    >>> start_micros = micros.counter()

    ... do some stuff ...

    >>> end_micros = micros.counter()
