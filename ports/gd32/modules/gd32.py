# gd32.py - small helper module frozen into the GD32 firmware.
#
# Frozen modules are stored as bytecode in Flash rather than being parsed at
# runtime, so importing this costs no RAM for the source text.


def info():
    """Return a short description of the running board."""
    import machine
    import sys

    return {
        "board": sys.platform,
        "freq": machine.freq(),
        "uid": machine.unique_id().hex(),
    }


def blink(pin_name="PA0", times=3, delay_ms=200):
    """Blink an LED a few times - handy for a quick sanity check."""
    import machine
    import time

    pin = machine.Pin(pin_name, machine.Pin.OUT)
    for _ in range(times):
        pin.value(1)
        time.sleep_ms(delay_ms)
        pin.value(0)
        time.sleep_ms(delay_ms)
    return times
