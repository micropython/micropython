# Example using PIO to read a quadrature encoded signal using a jump table.

# ruff: noqa: F821 - @asm_pio decorator adds names to function scope

from machine import Pin
import rp2

# This is a direct port of `quadrature_encoder.pio` from
# https://github.com/raspberrypi/pico-examples/

# By default, this uses pins 16 and 17 for the quadrature encoding input
# signals, and will sample the pins state 200 times per second.  Change
# PIN_BASE and SAMPLE_FREQUENCY below to alter the code's behaviour, keep
# in mind the code will use two contiguous pins.


PIN_BASE = 16
SAMPLE_FREQUENCY = 200  # In Hz


@rp2.asm_pio(
    origin=0, out_shiftdir=rp2.PIO.SHIFT_LEFT, fifo_join=rp2.PIO.JOIN_NONE, autopull=False
)
def encoder():
    # State 00
    jmp("update")
    jmp("decrement")
    jmp("increment")
    jmp("update")

    # State 01
    jmp("increment")
    jmp("update")
    jmp("update")
    jmp("decrement")

    # State 10
    jmp("decrement")
    jmp("update")
    jmp("update")
    jmp("increment")

    # State 11
    jmp("update")
    jmp("increment")

    # Pin2,Pin1 sequence
    label("decrement")
    jmp(y_dec, "update")

    # Main loop
    wrap_target()

    label("update")
    mov(isr, y)
    push(noblock)

    # Read the 2 input pins
    label("sample_pins")
    out(isr, 2)
    in_(pins, 2)

    # Save state and jump to state
    mov(osr, isr)
    mov(pc, isr)

    # Pin1,Pin2 sequence
    label("increment")
    mov(y, invert(y))
    jmp(y_dec, "increment_cont")
    label("increment_cont")
    mov(y, invert(y))

    # Next iteration
    wrap()


pin0 = Pin(PIN_BASE, Pin.IN, Pin.PULL_UP)
pin1 = Pin(PIN_BASE + 1, Pin.IN, Pin.PULL_UP)
# Sampling takes ~10 clock cycles
sm = rp2.StateMachine(0, encoder, in_base=pin0, jmp_pin=pin0, freq=SAMPLE_FREQUENCY * 10)
sm.active(1)
while True:
    items = sm.rx_fifo()
    while items > 0:
        val = sm.get()
        if val & 0x8000_0000:
            val = 0x8000_0000 - val
        print("{:>10}\r".format(val), end="")
        items -= 1
