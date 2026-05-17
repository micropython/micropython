.. _esp32_pwm:

Pulse Width Modulation
======================

Pulse width modulation (PWM) is a way to get an artificial analog output on a
digital pin.  It achieves this by rapidly toggling the pin from low to high.
There are two parameters associated with this: the frequency of the toggling,
and the duty cycle.  The duty cycle is defined to be how long the pin is high
compared with the length of a single period (low plus high time).  Maximum
duty cycle is when the pin is high all of the time, and minimum is when it is
low all of the time.

* More comprehensive example with all **16 PWM channels and 8 timers**::

    from time import sleep
    from machine import Pin, PWM
    try:
        F = 10000  # Hz
        D = 65536 // 16  # 6.25%
        pins = (2, 4, 12, 13, 14, 15, 16, 18, 19, 22, 23, 25, 26, 27, 32, 33)
        pwms = []
        for i, pin in enumerate(pins):
            f = F * (i // 2 + 1)
            d = min(65535, D * (i + 1))
            pwms.append(PWM(pin, freq=f, duty_u16=d))
            sleep(2 / f)
            print(pwms[i])
    finally:
        for pwm in pwms:
            try:
                pwm.deinit()
            except:
                pass

  Output is::

    PWM(Pin(2), freq=10000, duty_u16=4096)
    PWM(Pin(4), freq=10000, duty_u16=8192)
    PWM(Pin(12), freq=20000, duty_u16=12288)
    PWM(Pin(13), freq=20000, duty_u16=16384)
    PWM(Pin(14), freq=30030, duty_u16=20480)
    PWM(Pin(15), freq=30030, duty_u16=24576)
    PWM(Pin(16), freq=40000, duty_u16=28672)
    PWM(Pin(18), freq=40000, duty_u16=32768)
    PWM(Pin(19), freq=50000, duty_u16=36864)
    PWM(Pin(22), freq=50000, duty_u16=40960)
    PWM(Pin(23), freq=60060, duty_u16=45056)
    PWM(Pin(25), freq=60060, duty_u16=49152)
    PWM(Pin(26), freq=69930, duty_u16=53248)
    PWM(Pin(27), freq=69930, duty_u16=57344)
    PWM(Pin(32), freq=80000, duty_u16=61440)
    PWM(Pin(33), freq=80000, duty_u16=65535)


* Example of a **smooth frequency change**::

    from time import sleep
    from machine import Pin, PWM

    F_MIN = 1000
    F_MAX = 10000

    f = F_MIN
    delta_f = F_MAX // 50

    pwm = PWM(Pin(27), f)

    while True:
        pwm.freq(f)
        sleep(1 / f)
        sleep(0.1)
        print(pwm)

        f += delta_f
        if f > F_MAX or f < F_MIN:
            delta_f = -delta_f
            print()
            if f > F_MAX:
                f = F_MAX
            elif f < F_MIN:
                f = F_MIN

  See PWM wave on Pin(27) with an oscilloscope.

  Output is::

    PWM(Pin(27), freq=998, duty_u16=32768)
    PWM(Pin(27), freq=1202, duty_u16=32768)
    PWM(Pin(27), freq=1401, duty_u16=32768)
    PWM(Pin(27), freq=1598, duty_u16=32768)
    ...
    PWM(Pin(27), freq=9398, duty_u16=32768)
    PWM(Pin(27), freq=9615, duty_u16=32768)
    PWM(Pin(27), freq=9804, duty_u16=32768)
    PWM(Pin(27), freq=10000, duty_u16=32768)

    PWM(Pin(27), freq=10000, duty_u16=32768)
    PWM(Pin(27), freq=9804, duty_u16=32768)
    PWM(Pin(27), freq=9615, duty_u16=32768)
    PWM(Pin(27), freq=9398, duty_u16=32768)
    ...
    PWM(Pin(27), freq=1598, duty_u16=32768)
    PWM(Pin(27), freq=1401, duty_u16=32768)
    PWM(Pin(27), freq=1202, duty_u16=32768)
    PWM(Pin(27), freq=998, duty_u16=32768)


* Example of a **smooth duty change**::

    from time import sleep
    from machine import Pin, PWM

    DUTY_MAX = 65535

    duty_u16 = 0
    delta_d = 256

    pwm = PWM(Pin(27), freq=1000, duty_u16=duty_u16)

    while True:
        pwm.duty_u16(duty_u16)
        sleep(2 / pwm.freq())
        print(pwm)

        if duty_u16 >= DUTY_MAX:
            print()
            sleep(2)
        elif duty_u16 <= 0:
            print()
            sleep(2)

        duty_u16 += delta_d
        if duty_u16 >= DUTY_MAX:
            duty_u16 = DUTY_MAX
            delta_d = -delta_d
        elif duty_u16 <= 0:
            duty_u16 = 0
            delta_d = -delta_d

  PWM wave on Pin(27) with an oscilloscope.

  Output is::

    PWM(Pin(27), freq=998, duty_u16=0)
    PWM(Pin(27), freq=998, duty_u16=256)
    PWM(Pin(27), freq=998, duty_u16=512)
    PWM(Pin(27), freq=998, duty_u16=768)
    PWM(Pin(27), freq=998, duty_u16=1024)
    ...
    PWM(Pin(27), freq=998, duty_u16=64512)
    PWM(Pin(27), freq=998, duty_u16=64768)
    PWM(Pin(27), freq=998, duty_u16=65024)
    PWM(Pin(27), freq=998, duty_u16=65280)
    PWM(Pin(27), freq=998, duty_u16=65535)

    PWM(Pin(27), freq=998, duty_u16=65279)
    PWM(Pin(27), freq=998, duty_u16=65023)
    PWM(Pin(27), freq=998, duty_u16=64767)
    PWM(Pin(27), freq=998, duty_u16=64511)
    ...
    PWM(Pin(27), freq=998, duty_u16=1023)
    PWM(Pin(27), freq=998, duty_u16=767)
    PWM(Pin(27), freq=998, duty_u16=511)
    PWM(Pin(27), freq=998, duty_u16=255)
    PWM(Pin(27), freq=998, duty_u16=0)


* Example of a **smooth duty change and PWM output inversion**::

    from utime import sleep
    from machine import Pin, PWM

    try:
        DUTY_MAX = 65535

        duty_u16 = 0
        delta_d = 65536 // 32

        pwm = PWM(Pin(27))
        pwmi = PWM(Pin(32), invert=1)

        while True:
            pwm.duty_u16(duty_u16)
            pwmi.duty_u16(duty_u16)

            duty_u16 += delta_d
            if duty_u16 >= DUTY_MAX:
                duty_u16 = DUTY_MAX
                delta_d = -delta_d
            elif duty_u16 <= 0:
                duty_u16 = 0
                delta_d = -delta_d

            sleep(.01)
            print(pwm)
            print(pwmi)

    finally:
        try:
            pwm.deinit()
        except:
            pass
        try:
            pwmi.deinit()
        except:
            pass

  Output is::

    PWM(Pin(27), freq=5000, duty_u16=0)
    PWM(Pin(32), freq=5000, duty_u16=32768, invert=1)
    PWM(Pin(27), freq=5000, duty_u16=2048)
    PWM(Pin(32), freq=5000, duty_u16=2048, invert=1)
    PWM(Pin(27), freq=5000, duty_u16=4096)
    PWM(Pin(32), freq=5000, duty_u16=4096, invert=1)
    PWM(Pin(27), freq=5000, duty_u16=6144)
    PWM(Pin(32), freq=5000, duty_u16=6144, invert=1)
    PWM(Pin(27), freq=5000, duty_u16=8192)
    PWM(Pin(32), freq=5000, duty_u16=8192, invert=1)
    ...


  See PWM waves on Pin(27) and Pin(32) with an oscilloscope.

Note: New PWM parameters take effect in the next PWM cycle.

    pwm = PWM(2, duty=512)
    print(pwm)
    >>> PWM(Pin(2), freq=5000, duty=1023)  # the duty is not relevant
    pwm.init(freq=2, duty=64)
    print(pwm)
    >>> PWM(Pin(2), freq=2, duty=16)  # the duty is not relevant
    time.sleep(1 / 2)                # wait one PWM period
    print(pwm)
    >>> PWM(Pin(2), freq=2, duty=64)  # the duty is actual

Note: machine.freq(20_000_000) reduces the highest PWM frequency to 10 MHz.

Note: the Pin.OUT mode does not need to be specified. The channel is initialized
to PWM mode internally once for each Pin that is passed to the PWM constructor.

The following code is wrong::

    pwm = PWM(Pin(5, Pin.OUT), freq=1000, duty=512)  # Pin(5) in PWM mode here
    pwm = PWM(Pin(5, Pin.OUT), freq=500, duty=256)  # Pin(5) in OUT mode here, PWM is off

Use this code instead::

    pwm = PWM(Pin(5), freq=1000, duty=512)
    pwm.init(freq=500, duty=256)
