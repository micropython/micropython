# Test machine.UART.IRQ_BREAK firing after a break is received.
#
# IMPORTANT: This test requires hardware connections: the UART TX and RX
# pins must be wired together.

try:
    from machine import UART

    UART.IRQ_BREAK
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

import time, sys

# Configure pins based on the target.
if "esp32" in sys.platform:
    _machine = sys.implementation._machine
    if "ESP32S2" in _machine or "ESP32C3" in _machine or "ESP32C6" in _machine:
        print("SKIP")
        raise SystemExit
    uart_id = 1
    tx_pin = 4
    rx_pin = 5
elif "rp2" in sys.platform:
    uart_id = 0
    tx_pin = "GPIO0"
    rx_pin = "GPIO1"
else:
    print("Please add support for this test on this platform.")
    raise SystemExit


def irq(u):
    print("IRQ_BREAK:", bool(u.irq().flags() & u.IRQ_BREAK), "data:", u.read(1))


# Test that the IRQ is called for each break received.
for bits_per_s in (2400, 9600, 57600):
    uart = UART(uart_id, bits_per_s, tx=tx_pin, rx=rx_pin)
    uart.irq(irq, uart.IRQ_BREAK)

    print("write", bits_per_s)
    for i in range(3):
        uart.write(str(i))
        uart.flush()
        time.sleep_ms(10)
        uart.sendbreak()
        time.sleep_ms(10)
        if "esp32" in sys.platform:
            # On esp32 a read is needed to read in the break byte.
            uart.read()
    print("done")
