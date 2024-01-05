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
        f = 100  # Hz
        d = 2**16 // 16  # 6.25%
        pins = (2, 4, 12, 13, 14, 15, 16, 18, 19, 22, 23, 25, 26, 27, 32, 33)
        pwms = []
        for i, pin in enumerate(pins):
            pwms.append(PWM(Pin(pin), freq=f * (i // 2 + 1), duty_u16=min(2**16 - 1, d * (i + 1))))
            print(pwms[i])
        sleep(60)
    finally:
        for pwm in pwms:
            try:
                pwm.deinit()
            except:
                pass

  Output is::

    PWM(Pin(2), freq=100, duty_u16=4096)  # resolution=16, (duty=6.25%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(4), freq=100, duty_u16=8192)  # resolution=16, (duty=12.50%, resolution=0.002%), mode=0, channel=1, timer=0
    PWM(Pin(12), freq=199, duty_u16=12288)  # resolution=16, (duty=18.75%, resolution=0.002%), mode=0, channel=2, timer=1
    PWM(Pin(13), freq=199, duty_u16=16384)  # resolution=16, (duty=25.00%, resolution=0.002%), mode=0, channel=3, timer=1
    PWM(Pin(14), freq=299, duty_u16=20480)  # resolution=16, (duty=31.25%, resolution=0.002%), mode=0, channel=4, timer=2
    PWM(Pin(15), freq=299, duty_u16=24576)  # resolution=16, (duty=37.50%, resolution=0.002%), mode=0, channel=5, timer=2
    PWM(Pin(16), freq=400, duty_u16=28672)  # resolution=16, (duty=43.75%, resolution=0.002%), mode=0, channel=6, timer=3
    PWM(Pin(18), freq=400, duty_u16=32768)  # resolution=16, (duty=50.00%, resolution=0.002%), mode=0, channel=7, timer=3
    PWM(Pin(19), freq=500, duty_u16=36864)  # resolution=16, (duty=56.25%, resolution=0.002%), mode=1, channel=0, timer=0
    PWM(Pin(22), freq=500, duty_u16=40960)  # resolution=16, (duty=62.50%, resolution=0.002%), mode=1, channel=1, timer=0
    PWM(Pin(23), freq=599, duty_u16=45056)  # resolution=16, (duty=68.75%, resolution=0.002%), mode=1, channel=2, timer=1
    PWM(Pin(25), freq=599, duty_u16=49152)  # resolution=16, (duty=75.00%, resolution=0.002%), mode=1, channel=3, timer=1
    PWM(Pin(26), freq=700, duty_u16=53248)  # resolution=16, (duty=81.25%, resolution=0.002%), mode=1, channel=4, timer=2
    PWM(Pin(27), freq=700, duty_u16=57344)  # resolution=16, (duty=87.50%, resolution=0.002%), mode=1, channel=5, timer=2
    PWM(Pin(32), freq=799, duty_u16=61440)  # resolution=16, (duty=93.75%, resolution=0.002%), mode=1, channel=6, timer=3
    PWM(Pin(33), freq=799, duty_u16=65536)  # resolution=16, (duty=100.00%, resolution=0.002%), mode=1, channel=7, timer=3


* Example of a **smooth frequency change**::

    from time import sleep
    from machine import Pin, PWM

    F_MIN = 100
    F_MAX = 1000

    f = F_MIN
    delta_f = 100

    p = PWM(Pin(27), f)

    while True:
        p.freq(f)
        print(p)

        sleep(.2)

        f += delta_f
        if f > F_MAX or f < F_MIN:
            delta_f = -delta_f
            print()
            if f > F_MAX:
                f = F_MAX
            elif f < F_MIN:
                f = F_MIN

  `See PWM wave on Pin(27) with an oscilloscope. <https://user-images.githubusercontent.com/70886343/224013926-73953f7b-9b75-4e32-9595-83236c76ca1f.mp4>`_

  Output is::

    PWM(Pin(27), freq=100, duty=512)  # resolution=16, (duty=50.00%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=199, duty=512)  # resolution=16, (duty=50.00%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=299, duty=512)  # resolution=16, (duty=50.00%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=400, duty=512)  # resolution=16, (duty=50.00%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=500, duty=512)  # resolution=16, (duty=50.00%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=599, duty=512)  # resolution=16, (duty=50.00%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=700, duty=512)  # resolution=16, (duty=50.00%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=799, duty=512)  # resolution=16, (duty=50.00%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=900, duty=512)  # resolution=16, (duty=50.00%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=998, duty=512)  # resolution=16, (duty=50.00%, resolution=0.002%), mode=0, channel=0, timer=0

    PWM(Pin(27), freq=998, duty=512)  # resolution=16, (duty=50.00%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=900, duty=512)  # resolution=16, (duty=50.00%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=799, duty=512)  # resolution=16, (duty=50.00%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=700, duty=512)  # resolution=16, (duty=50.00%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=599, duty=512)  # resolution=16, (duty=50.00%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=500, duty=512)  # resolution=16, (duty=50.00%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=400, duty=512)  # resolution=16, (duty=50.00%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=299, duty=512)  # resolution=16, (duty=50.00%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=199, duty=512)  # resolution=16, (duty=50.00%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=100, duty=512)  # resolution=16, (duty=50.00%, resolution=0.002%), mode=0, channel=0, timer=0
    ...


* Example of a **smooth duty change**::

    from time import sleep
    from machine import Pin, PWM

    DUTY_MAX = 2**16 - 1

    duty_u16 = 0
    delta_d = 256

    pwm = PWM(Pin(27), 1000, duty_u16=duty_u16)
    print(pwm)

    while True:
        pwm.duty_u16(duty_u16)

        sleep(.001)

        print(pwm)

        duty_u16 += delta_d
        if duty_u16 >= DUTY_MAX:
            duty_u16 = DUTY_MAX
            delta_d = -delta_d
            print()
        elif duty_u16 <= 0:
            duty_u16 = 0
            delta_d = -delta_d
            print()

  See `PWM wave on Pin(27) with an oscilloscope. <https://user-images.githubusercontent.com/70886343/224020123-1c958e85-0c91-4ca6-8b4c-b3bb956892b1.mp4>`_

  Output is::

    PWM(Pin(27), freq=998, duty_u16=0)  # resolution=16, (duty=0.00%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=998, duty_u16=256)  # resolution=16, (duty=0.39%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=998, duty_u16=512)  # resolution=16, (duty=0.78%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=998, duty_u16=768)  # resolution=16, (duty=1.17%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=998, duty_u16=1024)  # resolution=16, (duty=1.56%, resolution=0.002%), mode=0, channel=0, timer=0
    ...
    PWM(Pin(27), freq=998, duty_u16=64256)  # resolution=16, (duty=98.05%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=998, duty_u16=64512)  # resolution=16, (duty=98.44%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=998, duty_u16=64768)  # resolution=16, (duty=98.83%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=998, duty_u16=65024)  # resolution=16, (duty=99.22%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=998, duty_u16=65280)  # resolution=16, (duty=99.61%, resolution=0.002%), mode=0, channel=0, timer=0

    PWM(Pin(27), freq=998, duty_u16=65536)  # resolution=16, (duty=100.00%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=998, duty_u16=65279)  # resolution=16, (duty=99.61%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=998, duty_u16=65023)  # resolution=16, (duty=99.22%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=998, duty_u16=64767)  # resolution=16, (duty=98.83%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=998, duty_u16=64511)  # resolution=16, (duty=98.44%, resolution=0.002%), mode=0, channel=0, timer=0
    ...
    PWM(Pin(27), freq=998, duty_u16=1279)  # resolution=16, (duty=1.95%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=998, duty_u16=1024)  # resolution=16, (duty=1.56%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=998, duty_u16=767)  # resolution=16, (duty=1.17%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=998, duty_u16=511)  # resolution=16, (duty=0.78%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=998, duty_u16=255)  # resolution=16, (duty=0.39%, resolution=0.002%), mode=0, channel=0, timer=0

    PWM(Pin(27), freq=998, duty_u16=0)  # resolution=16, (duty=0.00%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=998, duty_u16=256)  # resolution=16, (duty=0.39%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=998, duty_u16=512)  # resolution=16, (duty=0.78%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=998, duty_u16=768)  # resolution=16, (duty=1.17%, resolution=0.002%), mode=0, channel=0, timer=0
    PWM(Pin(27), freq=998, duty_u16=1024)  # resolution=16, (duty=1.56%, resolution=0.002%), mode=0, channel=0, timer=0


* Example of a **smooth duty change and PWM output inversion**::

    from utime import sleep
    from machine import Pin, PWM

    try:
        DUTY_MAX = 2**16 - 1

        duty_u16 = 0
        delta_d = 2**16 // 32

        pwm = PWM(Pin(27), 5000)
        pwmi = PWM(Pin(32), 5000, invert=1)

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

    ...
    PWM(Pin(27), freq=5000, duty_u16=24576)  # resolution=13, (duty=37.50%, resolution=0.012%), mode=0, channel=0, timer=0
    PWM(Pin(32), freq=5000, duty_u16=24576, invert=1)  # resolution=13, (duty=37.50%, resolution=0.012%), mode=0, channel=1, timer=0
    PWM(Pin(27), freq=5000, duty_u16=26624)  # resolution=13, (duty=40.63%, resolution=0.012%), mode=0, channel=0, timer=0
    PWM(Pin(32), freq=5000, duty_u16=26624, invert=1)  # resolution=13, (duty=40.63%, resolution=0.012%), mode=0, channel=1, timer=0
    PWM(Pin(27), freq=5000, duty_u16=28672)  # resolution=13, (duty=43.75%, resolution=0.012%), mode=0, channel=0, timer=0
    PWM(Pin(32), freq=5000, duty_u16=28672, invert=1)  # resolution=13, (duty=43.75%, resolution=0.012%), mode=0, channel=1, timer=0
    ...

  See `PWM waves on Pin(27) and Pin(32) <https://user-images.githubusercontent.com/70886343/222743883-dca25aa8-681d-471c-933a-6f9beacb6eee.mp4>`_ with an oscilloscope.


Note: the Pin.OUT mode does not need to be specified.  The channel is initialized
to PWM mode internally once for each Pin that is passed to the PWM constructor.

The following code is wrong::

    pwm = PWM(Pin(5, Pin.OUT), freq=1000, duty=512)  # Pin(5) in PWM mode here
    pwm = PWM(Pin(5, Pin.OUT), freq=500, duty=256)  # Pin(5) in OUT mode here, PWM is off

Use this code instead::

    pwm = PWM(Pin(5), freq=1000, duty=512)
    pwm.init(freq=500, duty=256)
