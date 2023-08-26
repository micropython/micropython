# Example of using PIO for PWM, and fading the brightness of an LED

from machine import Pin
from rp2 import PIO, StateMachine, asm_pio
from time import sleep


@asm_pio(sideset_init=PIO.OUT_LOW)
def pwm_prog():
    # fmt: off
    pull(noblock) .side(0)
    mov(x, osr)  # Keep most recent pull data stashed in X, for recycling by noblock
    mov(y, isr)  # ISR must be preloaded with PWM count max
    label("pwmloop")
    jmp(x_not_y, "skip")
    nop()         .side(1)
    label("skip")
    jmp(y_dec, "pwmloop")
    # fmt: on


class PIOPWM:
    def __init__(self, sm_id, pin, max_count, count_freq):
        self._sm = StateMachine(sm_id, pwm_prog, freq=2 * count_freq, sideset_base=Pin(pin))
        # Use exec() to load max count into ISR
        self._sm.put(max_count)
        self._sm.exec("pull()")
        self._sm.exec("mov(isr, osr)")
        self._sm.active(1)
        self._max_count = max_count

    def set(self, value):
        # Minimum value is -1 (completely turn off), 0 actually still produces narrow pulse
        value = max(value, -1)
        value = min(value, self._max_count)
        self._sm.put(value)


# Pin 25 is LED on Pico boards
pwm = PIOPWM(0, 25, max_count=(1 << 16) - 1, count_freq=10_000_000)

while True:
    for i in range(256):
        pwm.set(i**2)
        sleep(0.01)
