# Example using PIO to create a UART TX interface

from machine import Pin
from rp2 import PIO, StateMachine, asm_pio

UART_BAUD = 115200
PIN_BASE = 10
NUM_UARTS = 8


@asm_pio(sideset_init=PIO.OUT_HIGH, out_init=PIO.OUT_HIGH, out_shiftdir=PIO.SHIFT_RIGHT)
def uart_tx():
    # fmt: off
    # Block with TX deasserted until data available
    pull()
    # Initialise bit counter, assert start bit for 8 cycles
    set(x, 7)  .side(0)       [7]
    # Shift out 8 data bits, 8 execution cycles per bit
    label("bitloop")
    out(pins, 1)              [6]
    jmp(x_dec, "bitloop")
    # Assert stop bit for 8 cycles total (incl 1 for pull())
    nop()      .side(1)       [6]
    # fmt: on


# Now we add 8 UART TXs, on pins 10 to 17. Use the same baud rate for all of them.
uarts = []
for i in range(NUM_UARTS):
    sm = StateMachine(
        i, uart_tx, freq=8 * UART_BAUD, sideset_base=Pin(PIN_BASE + i), out_base=Pin(PIN_BASE + i)
    )
    sm.active(1)
    uarts.append(sm)


# We can print characters from each UART by pushing them to the TX FIFO
def pio_uart_print(sm, s):
    for c in s:
        sm.put(ord(c))


# Print a different message from each UART
for i, u in enumerate(uarts):
    pio_uart_print(u, "Hello from UART {}!\n".format(i))
