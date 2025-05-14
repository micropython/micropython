# Test machine.UART.IRQ_RXIDLE firing after a set of characters are received.
#
# IMPORTANT: This test requires hardware connections: the UART TX and RX
# pins must be wired together.

try:
    from machine import UART

    UART.IRQ_RXIDLE
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

import time, sys

# Configure pins based on the target.
if "alif" in sys.platform:
    uart_id = 1
    tx_pin = None
    rx_pin = None
elif "esp32" in sys.platform:
    uart_id = 1
    tx_pin = 4
    rx_pin = 5
elif "mimxrt" in sys.platform:
    uart_id = 1
    tx_pin = None
elif "pyboard" in sys.platform:
    uart_id = 4
    tx_pin = None  # PA0
    rx_pin = None  # PA1
elif "renesas-ra" in sys.platform:
    uart_id = 9
    tx_pin = None  # P602 @ RA6M2
    rx_pin = None  # P601 @ RA6M2
elif "rp2" in sys.platform:
    uart_id = 0
    tx_pin = "GPIO0"
    rx_pin = "GPIO1"
elif "samd" in sys.platform and "ItsyBitsy M0" in sys.implementation._machine:
    uart_id = 0
    tx_pin = "D1"
    rx_pin = "D0"
    byte_by_byte = True
elif "samd" in sys.platform and "ItsyBitsy M4" in sys.implementation._machine:
    uart_id = 3
    tx_pin = "D1"
    rx_pin = "D0"
else:
    print("Please add support for this test on this platform.")
    raise SystemExit


def irq(u):
    print("IRQ_RXIDLE:", bool(u.irq().flags() & u.IRQ_RXIDLE), "data:", u.read())


text = "12345678"

# Test that the IRQ is called for each set of byte received.
for bits_per_s in (2400, 9600, 115200):
    if tx_pin is None:
        uart = UART(uart_id, bits_per_s)
    else:
        uart = UART(uart_id, bits_per_s, tx=tx_pin, rx=rx_pin)

    uart.irq(irq, uart.IRQ_RXIDLE)

    print("write", bits_per_s)
    uart.write(text)
    uart.flush()
    print("ready")
    time.sleep_ms(100)
    print("done")
