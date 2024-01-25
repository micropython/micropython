import os, pyb

machine = os.uname().machine
if "PYBv1." in machine or "PYBLITEv1." in machine:
    leds = [pyb.LED(i) for i in range(1, 5)]
    pwm_leds = leds[2:]
elif "PYBD" in machine:
    leds = [pyb.LED(i) for i in range(1, 4)]
    pwm_leds = []
else:
    print("SKIP")
    raise SystemExit

# test printing
for i in range(3):
    print(leds[i])

# test on and off
for l in leds:
    l.on()
    assert l.intensity() == 255
    pyb.delay(100)
    l.off()
    assert l.intensity() == 0
    pyb.delay(100)

# test toggle
for l in 2 * leds:
    l.toggle()
    assert l.intensity() in (0, 255)
    pyb.delay(100)

# test intensity
for l in pwm_leds:
    for i in range(256):
        l.intensity(i)
        assert l.intensity() == i
        pyb.delay(1)
    for i in range(255, -1, -1):
        l.intensity(i)
        assert l.intensity() == i
        pyb.delay(1)
