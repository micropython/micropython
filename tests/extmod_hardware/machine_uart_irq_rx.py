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

byte_by_byte = False
# Configure pins based on the target.
if "alif" in sys.platform:
    uart_id = 1
    tx_pin = None
    rx_pin = None
elif "esp32" in sys.platform:
    uart_id = 1
    tx_pin = 4
    rx_pin = 5
elif "pyboard" in sys.platform:
    uart_id = 4
    tx_pin = None  # PA0
    rx_pin = None  # PA1
elif "samd" in sys.platform and "ItsyBitsy M0" in sys.implementation._machine:
    uart_id = 0
    tx_pin = "D1"
    rx_pin = "D0"
    byte_by_byte = True
elif "samd" in sys.platform and "ItsyBitsy M4" in sys.implementation._machine:
    uart_id = 3
    tx_pin = "D1"
    rx_pin = "D0"
elif "nrf" in sys.platform:
    uart_id = 0
    tx_pin = None
    rx_pin = None
elif "renesas-ra" in sys.platform:
    uart_id = 9
    tx_pin = None  # P602 @ RA6M2
    rx_pin = None  # P601 @ RA6M2
elif "CC3200" in sys.implementation._machine:
    # CC3200 doesn't work because it's too slow and has an allocation error in the handler.
    print("SKIP")
    raise SystemExit
else:
    print("Please add support for this test on this platform.")
    raise SystemExit


def irq(u):
    print("IRQ_RX:", bool(u.irq().flags() & u.IRQ_RX), "data:", u.read(1))


text = "1234"

# Test that the IRQ is called for each byte received.
# Use slow baudrates so that the IRQ has time to run.
for bits_per_s in (2400, 9600):
    if tx_pin is None:
        uart = UART(uart_id, bits_per_s)
    else:
        uart = UART(uart_id, bits_per_s, tx=tx_pin, rx=rx_pin)

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
