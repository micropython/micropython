# Example using PIO to read a quadrature encoded signal.

# ruff: noqa: F821 - @asm_pio decorator adds names to function scope

from machine import Pin
import rp2

# This is a direct port of `quadrature_encoder.pio` from
# https://github.com/raspberrypi/pico-examples/

# This uses pins 16 and 17 for the quadrature encoding input signals.


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


pin16 = Pin(16, Pin.IN, Pin.PULL_UP)
pin17 = Pin(17, Pin.IN, Pin.PULL_UP)
sm = rp2.StateMachine(0, encoder, in_base=pin16, jmp_pin=pin16)
sm.active(1)
while True:
    items = sm.rx_fifo()
    while items > 0:
        val = str(sm.get())
        print(val + (" " * (len(val) - 10)) + "\r", end="")
        items -= 1
