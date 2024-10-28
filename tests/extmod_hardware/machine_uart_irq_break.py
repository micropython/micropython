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
    # ESP32 needs separate UART instances for the test
    recv_uart_id = 1
    recv_tx_pin = 14
    recv_rx_pin = 5
    send_uart_id = 2
    send_tx_pin = 4
    send_rx_pin = 12
elif "rp2" in sys.platform:
    recv_uart_id = 0
    send_uart_id = 0
    recv_tx_pin = "GPIO0"
    recv_rx_pin = "GPIO1"
else:
    print("Please add support for this test on this platform.")
    raise SystemExit


def irq(u):
    print("IRQ_BREAK:", bool(u.irq().flags() & u.IRQ_BREAK), "data:", u.read(1))


# Test that the IRQ is called for each break received.
for bits_per_s in (2400, 9600, 57600):
    recv_uart = UART(recv_uart_id, bits_per_s, tx=recv_tx_pin, rx=recv_rx_pin)
    if recv_uart_id != send_uart_id:
        send_uart = UART(send_uart_id, bits_per_s, tx=send_tx_pin, rx=send_rx_pin)
    else:
        send_uart = recv_uart

    recv_uart.irq(irq, recv_uart.IRQ_BREAK)

    print("write", bits_per_s)
    for i in range(3):
        send_uart.write(str(i))
        send_uart.flush()
        time.sleep_ms(10)
        send_uart.sendbreak()
        time.sleep_ms(10)
        if "esp32" in sys.platform:
            # On esp32 a read is needed to read in the break byte.
            recv_uart.read()
    print("done")
