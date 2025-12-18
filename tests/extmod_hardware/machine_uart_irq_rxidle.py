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
from target_wiring import uart_loopback_args, uart_loopback_kwargs

# Target tuning options.
tune_wait_initial_rxidle = sys.platform == "pyboard"


def irq(u):
    print("IRQ_RXIDLE:", bool(u.irq().flags() & u.IRQ_RXIDLE), "data:", u.read())


text = ("12345678", "abcdefgh")

# Test that the IRQ is called for each set of byte received.
for bits_per_s in (2400, 9600, 115200):
    print("========")
    print("bits_per_s:", bits_per_s)

    uart = UART(*uart_loopback_args, baudrate=bits_per_s, **uart_loopback_kwargs)

    # Ignore a possible initial RXIDLE condition after creating UART.
    if tune_wait_initial_rxidle:
        uart.irq(lambda _: None, uart.IRQ_RXIDLE)
        time.sleep_ms(10)

    # Configure desired IRQ.
    uart.irq(irq, uart.IRQ_RXIDLE)

    for i in range(2):
        # Write data and wait for IRQ.
        print("write")
        uart.write(text[i])
        uart.flush()
        print("ready")
        time.sleep_ms(100)
        print("done")
