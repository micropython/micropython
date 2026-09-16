# time_pulse_us multi-device test
# Tests machine.time_pulse_us() by generating a known 1 s pulse on one board
# and measuring it on the other.
#
# KIT_PSE84_AI board hardware connections:
#   Board 0 (instance0)   Board 1 (instance1)
#   P16_0 (pulse out)  <-> P16_0 (pulse in)
#   GND                <-> GND

from machine import Pin, time_pulse_us
import time

pulse_pin = "P16_0"


def blocking_delay_ms(delay_ms):
    start = time.ticks_ms()
    while time.ticks_diff(time.ticks_ms(), start) < delay_ms:
        pass


def instance0():
    # Signal generator: drives a 1 s high pulse after the measurer is ready.
    pulse_out = Pin(pulse_pin, Pin.OUT, value=0)

    multitest.next()
    # Wait until instance1 has broadcast that it is inside time_pulse_us.
    multitest.wait("MEASURER_READY")

    # Small delay to ensure instance1 has entered the C measurement loop.
    blocking_delay_ms(50)

    # Generate high pulse for 1 s.
    pulse_out.high()
    blocking_delay_ms(1000)
    pulse_out.low()


def instance1():
    # Measurer: signals instance0 immediately before entering time_pulse_us.
    pulse_in = Pin(pulse_pin, Pin.IN, Pin.PULL_DOWN)

    multitest.next()
    # Broadcast readiness, then enter the measurement loop with no delay.
    multitest.broadcast("MEASURER_READY")

    # Measure the pulse width (timeout 10 s).
    width = time_pulse_us(pulse_in, 1, 10000000)

    print(
        f"Pulse timing verified: {True if (0.98 < (width / 1000000) < 1.2) else 'False, width=' + str(width / 1000000)}"
    )
