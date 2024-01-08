# Test machine.UART transmission.
# Does not require any external connections.

try:
    from machine import UART
except ImportError:
    print("SKIP")
    raise SystemExit

import time, sys

# Configure pins based on the target.
if "rp2" in sys.platform:
    uart_id = 0
    tx_pin = "GPIO0"
    rx_pin = "GPIO1"
else:
    print("SKIP")
    raise SystemExit

# Test that write+flush takes the expected amount of time to execute.
for bits_per_s in (2400, 9600, 115200):
    text = "Hello World"
    uart = UART(uart_id, bits_per_s, bits=8, parity=None, stop=1, tx=tx_pin, rx=rx_pin)

    start_us = time.ticks_us()
    uart.write(text)
    uart.flush()
    duration_us = time.ticks_diff(time.ticks_us(), start_us)

    # 1(startbit) + 8(bits) + 1(stopbit) + 0(parity)
    bits_per_char = 10
    expect_us = (len(text)) * bits_per_char * 1_000_000 // bits_per_s
    print(bits_per_s, duration_us <= expect_us)
