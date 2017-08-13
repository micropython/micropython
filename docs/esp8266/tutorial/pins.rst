GPIO Pins
=========

The way to connect your board to the external world, and control other
components, is through the GPIO pins.  Not all pins are available to use,
in most cases only pins 0, 2, 4, 5, 12, 13, 14, 15, and 16 can be used.

The pins are available in the machine module, so make sure you import that
first.  Then you can create a pin using::

    >>> pin = machine.Pin(0)

Here, the "0" is the pin that you want to access.  Usually you want to
configure the pin to be input or output, and you do this when constructing
it.  To make an input pin use::

    >>> pin = machine.Pin(0, machine.Pin.OUT, machine.Pin.PULL_UP)

You can either use PULL_UP or None for the input pull-mode.  If it's
not specified then it defaults to None, which is no pull resistor.
You can read the value on the pin using::

    >>> pin.value()
    0

The pin on your board may return 0 or 1 here, depending on what it's connected
to.  To make an output pin use::

    >>> pin = machine.Pin(0, machine.Pin.OUT)

Then set its value using::

    >>> pin.value(0)
    >>> pin.value(1)

Or::

    >>> pin.low()
    >>> pin.high()

External interrupts
-------------------

All pins except number 16 can be configured to trigger a hard interrupt if their
input changes.  You can set code (a callback function) to be executed on the
trigger.

Let's first define a callback function, which must take a single argument,
being the pin that triggered the function.  We will make the function just print
the pin::

    >>> def callback(p):
    ...     print('pin change', p)

Next we will create two pins and configure them as inputs::

    >>> from machine import Pin
    >>> p0 = Pin(0, Pin.IN)
    >>> p2 = Pin(2, Pin.IN)

An finally we need to tell the pins when to trigger, and the function to call
when they detect an event::

    >>> p0.irq(trigger=Pin.IRQ_FALLING, handler=callback)
    >>> p2.irq(trigger=Pin.IRQ_RISING | Pin.IRQ_FALLING, handler=callback)

We set pin 0 to trigger only on a falling edge of the input (when it goes from
high to low), and set pin 2 to trigger on both a rising and falling edge.  After
entering this code you can apply high and low voltages to pins 0 and 2 to see
the interrupt being executed.

A hard interrupt will trigger as soon as the event occurs and will interrupt any
running code, including Python code.  As such your callback functions are
limited in what they can do (they cannot allocate memory, for example) and
should be as short and simple as possible.
