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

More comprehensive example with all 16 PWM channels and 8 timers::

    from machine import Pin, PWM
    try:
        f = 100  # Hz
        d = 1024 // 16  # 6.25%
        pins = (15, 2, 4, 16, 18, 19, 22, 23, 25, 26, 27, 14 , 12, 13, 32, 33)
        pwms = []
        for i, pin in enumerate(pins):
            pwms.append(PWM(Pin(pin), freq=f * (i // 2 + 1), duty= 1023 if i==15 else d * (i + 1)))
            print(pwms[i])
    finally:
        for pwm in pwms:
            try:
                pwm.deinit()
            except:
                pass

Output is::

    PWM(pin=15, freq=100, duty=64, resolution=10, mode=0, channel=0, timer=0)
    PWM(pin=2, freq=100, duty=128, resolution=10, mode=0, channel=1, timer=0)
    PWM(pin=4, freq=200, duty=192, resolution=10, mode=0, channel=2, timer=1)
    PWM(pin=16, freq=200, duty=256, resolution=10, mode=0, channel=3, timer=1)
    PWM(pin=18, freq=300, duty=320, resolution=10, mode=0, channel=4, timer=2)
    PWM(pin=19, freq=300, duty=384, resolution=10, mode=0, channel=5, timer=2)
    PWM(pin=22, freq=400, duty=448, resolution=10, mode=0, channel=6, timer=3)
    PWM(pin=23, freq=400, duty=512, resolution=10, mode=0, channel=7, timer=3)
    PWM(pin=25, freq=500, duty=576, resolution=10, mode=1, channel=0, timer=0)
    PWM(pin=26, freq=500, duty=640, resolution=10, mode=1, channel=1, timer=0)
    PWM(pin=27, freq=600, duty=704, resolution=10, mode=1, channel=2, timer=1)
    PWM(pin=14, freq=600, duty=768, resolution=10, mode=1, channel=3, timer=1)
    PWM(pin=12, freq=700, duty=832, resolution=10, mode=1, channel=4, timer=2)
    PWM(pin=13, freq=700, duty=896, resolution=10, mode=1, channel=5, timer=2)
    PWM(pin=32, freq=800, duty=960, resolution=10, mode=1, channel=6, timer=3)
    PWM(pin=33, freq=800, duty=1023, resolution=10, mode=1, channel=7, timer=3)
