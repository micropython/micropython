# Example using PIO to create a UART RX interface.
#
# To make it work you'll need a wire connecting GPIO4 and GPIO3.
#
# Demonstrates:
#   - PIO shifting in data on a pin
#   - PIO jmp(pin) instruction
#   - PIO irq handler
#   - using the second core via _thread

# ruff: noqa: F821 - @asm_pio decorator adds names to function scope

import _thread
from machine import Pin, UART
from rp2 import PIO, StateMachine, asm_pio

UART_BAUD = 9600

HARD_UART_TX_PIN = Pin(4, Pin.OUT)
PIO_RX_PIN = Pin(3, Pin.IN, Pin.PULL_UP)


@asm_pio(
    autopush=True,
    push_thresh=8,
    in_shiftdir=PIO.SHIFT_RIGHT,
    fifo_join=PIO.JOIN_RX,
)
def uart_rx_mini():
    # fmt: off
    # Wait for start bit
    wait(0, pin, 0)
    # Preload bit counter, delay until eye of first data bit
    set(x, 7)                 [10]
    # Loop 8 times
    label("bitloop")
    # Sample data
    in_(pins, 1)
    # Each iteration is 8 cycles
    jmp(x_dec, "bitloop")     [6]
    # fmt: on


@asm_pio(
    in_shiftdir=PIO.SHIFT_RIGHT,
)
def uart_rx():
    # fmt: off
    label("start")
    # Stall until start bit is asserted
    wait(0, pin, 0)
    # Preload bit counter, then delay until halfway through
    # the first data bit (12 cycles incl wait, set).
    set(x, 7)                 [10]
    label("bitloop")
    # Shift data bit into ISR
    in_(pins, 1)
    # Loop 8 times, each loop iteration is 8 cycles
    jmp(x_dec, "bitloop")     [6]
    # Check stop bit (should be high)
    jmp(pin, "good_stop")
    # Either a framing error or a break. Set a sticky flag
    # and wait for line to return to idle state.
    irq(block, 4)
    wait(1, pin, 0)
    # Don't push data if we didn't see good framing.
    jmp("start")
    # No delay before returning to start; a little slack is
    # important in case the TX clock is slightly too fast.
    label("good_stop")
    push(block)
    # fmt: on


# The handler for a UART break detected by the PIO.
def handler(sm):
    print("break", time.ticks_ms(), end=" ")


# Function for core1 to execute to write to the given UART.
def core1_task(uart, text):
    uart.write(text)


# Set up the hard UART we're going to use to print characters.
uart = UART(1, UART_BAUD, tx=HARD_UART_TX_PIN)

for pio_prog in ("uart_rx_mini", "uart_rx"):
    # Set up the state machine we're going to use to receive the characters.
    sm = StateMachine(
        0,
        globals()[pio_prog],
        freq=8 * UART_BAUD,
        in_base=PIO_RX_PIN,  # For WAIT, IN
        jmp_pin=PIO_RX_PIN,  # For JMP
    )
    sm.irq(handler)
    sm.active(1)

    # Tell core 1 to print some text to UART 1
    text = "Hello, world from PIO, using {}!".format(pio_prog)
    _thread.start_new_thread(core1_task, (uart, text))

    # Echo characters received from PIO to the console.
    for i in range(len(text)):
        print(chr(sm.get() >> 24), end="")
    print()
