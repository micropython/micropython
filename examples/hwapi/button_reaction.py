import utime
import machine
from hwconfig import LED, BUTTON

# machine.time_pulse_us() function demo

print(
    """\
Let's play an interesting game:
You click button as fast as you can, and I tell you how slow you are.
Ready? Cliiiiick!
"""
)

while 1:
    delay = machine.time_pulse_us(BUTTON, 1, 10 * 1000 * 1000)
    if delay < 0:
        print("Well, you're *really* slow")
    else:
        print("You are as slow as %d microseconds!" % delay)
    utime.sleep_ms(10)
