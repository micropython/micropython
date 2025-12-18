# Test machine.UART.IRQ_RX firing for each character received.
#
# IMPORTANT: This test requires hardware connections: the UART TX and RX
# pins must be wired together.

try:
    from machine import UART

    UART.IRQ_RX
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

import time, sys

if "CC3200" in sys.implementation._machine:
    # CC3200 doesn't work because it's too slow and has an allocation error in the handler.
    print("SKIP")
    raise SystemExit

from target_wiring import uart_loopback_args, uart_loopback_kwargs

byte_by_byte = "ItsyBitsy M0" in sys.implementation._machine


def irq(u):
    print("IRQ_RX:", bool(u.irq().flags() & u.IRQ_RX), "data:", u.read(1))


text = "1234"

# Test that the IRQ is called for each byte received.
# Use slow baudrates so that the IRQ has time to run.
for bits_per_s in (2400, 9600):
    uart = UART(*uart_loopback_args, baudrate=bits_per_s, **uart_loopback_kwargs)
    uart.irq(irq, uart.IRQ_RX)

    print("write", bits_per_s)
    if byte_by_byte:
        # slow devices need data to be sent slow
        for c in text:
            uart.write(c)
            uart.flush()
    else:
        uart.write(text)
        uart.flush()
    time.sleep_ms(100)
    print("done")
