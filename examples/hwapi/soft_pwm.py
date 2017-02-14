import utime
from hwconfig import LED


# Using sleep_ms() gives pretty poor PWM resolution and
# brightness control, but we use it in the attempt to
# make this demo portable to even more boards (e.g. to
# those which don't provide sleep_us(), or provide, but
# it's not precise, like would be on non realtime OSes).
# We otherwise use 20ms period, to make frequency not less
# than 50Hz to avoid visible flickering (you may still see
# if you're unlucky).
def pwm_cycle(led, duty, cycles):
    duty_off = 20 - duty
    for i in range(cycles):
        if duty:
            led.on()
            utime.sleep_ms(duty)
        if duty_off:
            led.off()
            utime.sleep_ms(duty_off)


# At the duty setting of 1, an LED is still pretty bright, then
# at duty 0, it's off. This makes rather unsmooth transition, and
# breaks fade effect. So, we avoid value of 0 and oscillate between
# 1 and 20. Actually, highest values like 19 and 20 are also
# barely distinguishible (like, both of them too bright and burn
# your eye). So, improvement to the visible effect would be to use
# more steps (at least 10x), and then higher frequency, and use
# range which includes 1 but excludes values at the top.
while True:
    # Fade in
    for i in range(1, 21):
        pwm_cycle(LED, i, 2)
    # Fade out
    for i in range(20, 0, -1):
        pwm_cycle(LED, i, 2)
