# Test machine.UART transmission.
# Does not require any external connections.

try:
    from machine import UART
except ImportError:
    print("SKIP")
    raise SystemExit

import time, sys

initial_delay_ms = 0
bit_margin = 0
timing_margin_us = 100

# Configure pins based on the target.
if "alif" in sys.platform:
    uart_id = 1
    pins = {}
    bit_margin = 1
elif "esp32" in sys.platform:
    uart_id = 1
    pins = {}
    timing_margin_us = 400
elif "mimxrt" in sys.platform:
    uart_id = 1
    pins = {}
    initial_delay_ms = 20  # UART sends idle frame after init, so wait for that
    bit_margin = 1
elif "pyboard" in sys.platform:
    if "STM32WB" in sys.implementation._machine:
        uart_id = "LP1"
    else:
        uart_id = 4
    pins = {}
    initial_delay_ms = 50  # UART sends idle frame after init, so wait for that
    bit_margin = 1  # first start-bit must wait to sync with the UART clock
elif "rp2" in sys.platform:
    uart_id = 0
    pins = {"tx": "GPIO0", "rx": "GPIO1"}
    timing_margin_us = 180
elif "samd" in sys.platform:
    uart_id = 2
    pins = {"tx": "D1", "rx": "D0"}
    timing_margin_us = 300
    bit_margin = 1
else:
    print("SKIP")
    raise SystemExit

# Test that write+flush takes the expected amount of time to execute.
for bits_per_s in (2400, 9600, 115200):
    text = "Hello World"
    uart = UART(uart_id, bits_per_s, bits=8, parity=None, stop=1, **pins)
    time.sleep_ms(initial_delay_ms)

    start_us = time.ticks_us()
    uart.write(text)
    uart.flush()
    duration_us = time.ticks_diff(time.ticks_us(), start_us)

    # 1(startbit) + 8(bits) + 1(stopbit) + 0(parity)
    bits_per_char = 10
    expect_us = (len(text)) * bits_per_char * 1_000_000 // bits_per_s
    delta_us = abs(duration_us - expect_us)
    margin_us = timing_margin_us + bit_margin * 1_000_000 // bits_per_s
    print(bits_per_s, delta_us <= margin_us or delta_us)
