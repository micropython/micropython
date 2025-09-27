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
from target_wiring import uart_loopback_args, uart_loopback_kwargs


def irq(u):
    print("IRQ_BREAK:", bool(u.irq().flags() & u.IRQ_BREAK), "data:", u.read(1))


# Test that the IRQ is called for each break received.
for bits_per_s in (2400, 9600, 57600):
    uart = UART(*uart_loopback_args, baudrate=bits_per_s, **uart_loopback_kwargs)
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
