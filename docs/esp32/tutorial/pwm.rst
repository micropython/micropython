Pulse Width Modulation
======================

Pulse width modulation (PWM) is a way to get an artificial analog output on a
digital pin.  It achieves this by rapidly toggling the pin from low to high.
There are two parameters associated with this: the frequency of the toggling,
and the duty cycle.  The duty cycle is defined to be how long the pin is high
compared with the length of a single period (low plus high time).  Maximum
duty cycle is when the pin is high all of the time, and minimum is when it is
low all of the time.

On the ESP8266 the pins 0, 2, 4, 5, 12, 13, 14 and 15 all support PWM.  The
limitation is that they must all be at the same frequency, and the frequency
must be between 1Hz and 1kHz.

To use PWM on a pin you must first create the pin object, for example::

    >>> import machine
    >>> p12 = machine.Pin(12)

Then create the PWM object using::

    >>> pwm12 = machine.PWM(p12)

You can set the frequency and duty cycle using::

    >>> pwm12.freq(500)
    >>> pwm12.duty(512)

Note that the duty cycle is between 0 (all off) and 1023 (all on), with 512
being a 50% duty. Values beyond this min/max will be clipped. If you
print the PWM object then it will tell you its current configuration::

    >>> pwm12
    PWM(12, freq=500, duty=512)

You can also call the ``freq()`` and ``duty()`` methods with no arguments to
get their current values.

The pin will continue to be in PWM mode until you deinitialise it using::

    >>> pwm12.deinit()

Fading an LED
-------------

Let's use the PWM feature to fade an LED.  Assuming your board has an LED
connected to pin 2 (ESP-12 modules do) we can create an LED-PWM object using::

    >>> led = machine.PWM(machine.Pin(2), freq=1000)

Notice that we can set the frequency in the PWM constructor.

For the next part we will use timing and some math, so import these modules::

    >>> import time, math

Then create a function to pulse the LED::

    >>> def pulse(l, t):
    ...     for i in range(20):
    ...         l.duty(int(math.sin(i / 10 * math.pi) * 500 + 500))
    ...         time.sleep_ms(t)

You can try this function out using::

    >>> pulse(led, 50)

For a nice effect you can pulse many times in a row::

    >>> for i in range(10):
    ...     pulse(led, 20)

Remember you can use ctrl-C to interrupt the code.

Control a hobby servo
---------------------

Hobby servo motors can be controlled using PWM.  They require a frequency of
50Hz and then a duty between about 40 and 115, with 77 being the centre value.
If you connect a servo to the power and ground pins, and then the signal line
to pin 12 (other pins will work just as well), you can control the motor using::

    >>> servo = machine.PWM(machine.Pin(12), freq=50)
    >>> servo.duty(40)
    >>> servo.duty(115)
    >>> servo.duty(77)
