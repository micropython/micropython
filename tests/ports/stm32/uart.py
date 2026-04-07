# Test pyb.UART basic behaviour.

import sys
from pyb import UART

if "STM32WB" in sys.implementation._machine:
    # UART(1) is usually connected to the REPL on these MCUs.
    print("SKIP")
    raise SystemExit

# Test invalid UART identifiers.
for bus in (-1, 0, 100, "doesnotexist"):
    try:
        UART(bus, 9600)
    except ValueError:
        print("ValueError", bus)

# Find the first available UART (assume there is one).
uart_id = None
for bus in (1, 2, 3):
    try:
        UART(bus)
        uart_id = bus
        break
    except ValueError:
        pass


# Print UART without id or invert, and round baudrate to multiple of 2.
# This ensures an output that's consistent across MCUs.
def print_uart(uart):
    s = str(uart).split(", ")
    s[0] = s[0][0:5]
    br_key, br_value = s[1].split("=")
    br_value = (int(br_value) + 1) // 2 * 2
    s[1] = br_key + "=" + str(br_value)
    if s[-1] == "invert=0)":
        del s[-1]
        s[-1] += ")"
    print(", ".join(s))


uart = UART(uart_id)
uart = UART(uart_id, 9600)
uart = UART(uart_id, 9600, bits=8, parity=None, stop=1)
print_uart(uart)

uart.init(2400)
print_uart(uart)

print(uart.any())
print(uart.write("123"))
print(uart.write(b"abcd"))
print(uart.writechar(1))

# make sure this method exists
uart.sendbreak()

# non-blocking mode
uart = UART(uart_id, 9600, timeout=0)
print(uart.write(b"1"))
print(uart.write(b"abcd"))
print(uart.writechar(1))
if uart.any():
    # discard data in case UART is connected externally in loopback
    uart.read(8)
print(uart.read(100))

# set rxbuf
uart.init(9600, rxbuf=8)
print_uart(uart)
uart.init(9600, rxbuf=0)
print_uart(uart)

# set read_buf_len (legacy, use rxbuf instead)
uart.init(9600, read_buf_len=4)
print_uart(uart)
uart.init(9600, read_buf_len=0)
print_uart(uart)
