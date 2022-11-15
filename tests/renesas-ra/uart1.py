import os
from machine import UART

machine = os.uname().machine
if "RA6M2_EK" in machine:
    # 0, 7, 9
    uart_ids = (0, 7, 9)
    try_id = 7
    try_s = "UART(7, baudrate=115200, bits=8, parity=None, stop=1, tx=P401, rx=P402, flow=0, rxbuf=259, timeout=0, timeout_char=2)"
elif "RA4M1_CLICKER" in machine:
    # 0, 1
    uart_ids = (0, 1)
    try_id = 0
    try_s = "UART(0, baudrate=115200, bits=8, parity=None, stop=1, tx=P411, rx=P410, flow=0, rxbuf=259, timeout=0, timeout_char=2)"
elif "RA4M1_EK" in machine:
    # 0, 1, 2, 9
    # vector for 9 is not registered
    uart_ids = (0, 1, 2)
    try_id = 1
    try_s = "UART(1, baudrate=115200, bits=8, parity=None, stop=1, tx=P401, rx=P402, flow=0, rxbuf=259, timeout=0, timeout_char=2)"
elif "RA4W1_EK" in machine:
    # 0, 1, 4, 9
    # 0 is disabled.
    uart_ids = (1, 4, 9)
    try_id = 9
    try_s = "UART(9, baudrate=115200, bits=8, parity=None, stop=1, tx=P109, rx=P110, flow=0, rxbuf=259, timeout=0, timeout_char=2)"
elif "RA6M1_EK" in machine:
    # 0, 1, 2, 3, 4, 8, 9
    # 1/3/4/9 are disabled
    uart_ids = (0, 2, 8)
    try_id = 8
    try_s = "UART(8, baudrate=115200, bits=8, parity=None, stop=1, tx=P105, rx=P104, flow=0, rxbuf=259, timeout=0, timeout_char=2)"
else:
    print("SKIP")
    raise SystemExit

for ch in uart_ids:
    try:
        uart = UART(ch)
    except ValueError:
        print("ValueError", bus)

rxbuf_size = 259

ch = UART(try_id, rxbuf=rxbuf_size)
if str(ch) == try_s:
    print("OK")
else:
    print("NG")
    print("exp: " + try_s)
    print("out: " + str(ch))
