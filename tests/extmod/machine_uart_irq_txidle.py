# Test machine.UART.IRQ_TXIDLE firing after a transmission.
# Does not require any external connections.

try:
    from machine import UART

    UART.IRQ_TXIDLE
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

import time, sys

# Configure pins based on the target.
if "alif" in sys.platform:
    uart_id = 1
    tx_pin = None
elif "rp2" in sys.platform:
    uart_id = 0
    tx_pin = "GPIO0"
    rx_pin = "GPIO1"
elif "samd" in sys.platform and "ItsyBitsy M0" in sys.implementation._machine:
    uart_id = 0
    tx_pin = "D1"
    rx_pin = "D0"
elif "samd" in sys.platform and "ItsyBitsy M4" in sys.implementation._machine:
    uart_id = 3
    tx_pin = "D1"
    rx_pin = "D0"
elif "mimxrt" in sys.platform:
    uart_id = 1
    tx_pin = None
elif "nrf" in sys.platform:
    uart_id = 0
    tx_pin = None
else:
    print("Please add support for this test on this platform.")
    raise SystemExit


def irq(u):
    print("IRQ_TXIDLE:", u.irq().flags() == u.IRQ_TXIDLE)


text = "Hello World" * 20

# Test that the IRQ is called after the write has completed.
for bits_per_s in (2400, 9600, 115200):
    if tx_pin is None:
        uart = UART(uart_id, bits_per_s)
    else:
        uart = UART(uart_id, bits_per_s, tx=tx_pin, rx=rx_pin)

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
