# Test machine.UART.IRQ_TXIDLE firing after a transmission.
# Does not require any external connections.

try:
    from machine import UART

    UART.IRQ_TXIDLE
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

import time, sys
from target_wiring import uart_loopback_args, uart_loopback_kwargs


def irq(u):
    print("IRQ_TXIDLE:", u.irq().flags() == u.IRQ_TXIDLE)


text = "Hello World" * 20

# Test that the IRQ is called after the write has completed.
for bits_per_s in (2400, 9600, 115200):
    uart = UART(*uart_loopback_args, baudrate=bits_per_s, **uart_loopback_kwargs)
    uart.irq(irq, uart.IRQ_TXIDLE)

    # The IRQ_TXIDLE shall trigger after the message has been sent. Thus
    # the test marks a time window close to the expected of the sending
    # and the time at which the IRQ should have been fired.
    # It is just a rough estimation of 10 characters before and
    # 20 characters after the data's end.

    bits_per_char = 10  # 1(startbit) + 8(bits) + 1(stopbit) + 0(parity)
    start_time_us = (len(text) - 10) * bits_per_char * 1_000_000 // bits_per_s
    window_ms = 20 * bits_per_char * 1_000 // bits_per_s + 1

    print("write", bits_per_s)
    uart.write(text)
    time.sleep_us(start_time_us)
    print("ready")
    time.sleep_ms(window_ms)
    print("done")
