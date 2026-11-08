# gd32.py - small helper module frozen into the GD32 firmware.
#
# Frozen modules are stored as bytecode in Flash rather than being parsed at
# runtime, so importing this costs no RAM for the source text.
#
# Board: LCKFB "Sky Star" (立创梁山派·天空星), GD32F407VE.
# The only user LED is on PB2 and is active high (PB2 -> 2k -> LED -> GND).


def info():
    """Return a short description of the running board."""
    import machine
    import sys

    return {
        "board": sys.platform,
        "freq": machine.freq(),
        "uid": machine.unique_id().hex(),
    }


def blink(times=3, delay_ms=200, pin_name="PB2"):
    """Blink the user LED a few times - handy for a quick sanity check.

    `mode` has to be passed as a keyword argument: this port declares the
    machine.Pin options as keyword-only, so ``machine.Pin(name, machine.Pin.OUT)``
    raises "extra positional arguments given".  Upstream ports accept the
    positional form, which is why it is easy to get wrong here.

    Earlier versions defaulted to PA0 and to a positional mode; PA0 is the WKUP
    button on this board (no LED), so the old default silently did nothing
    visible while also raising TypeError.  Both are fixed.
    """
    import machine
    import time

    pin = machine.Pin(pin_name, mode=machine.Pin.OUT)
    for _ in range(times):
        pin.value(1)
        time.sleep_ms(delay_ms)
        pin.value(0)
        time.sleep_ms(delay_ms)
    return times
