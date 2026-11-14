# MIT license; Copyright (c) 2026 OpenMV LLC.
#
# Test that I2C devices which are always present on the board respond to a scan.
#
# The devices are listed in `i2c_scan_devices`, see tests/target_wiring/README.md.
#
# requires: target-wiring

import time
from machine import I2C, Pin

try:
    from target_wiring import i2c_scan_devices
except ImportError:
    print("SKIP")
    raise SystemExit

missing = []
for i2c_id, addr, power_pin in i2c_scan_devices:
    if power_pin is not None:
        Pin(power_pin, Pin.OUT, value=1)
        time.sleep_ms(10)
    i2c = I2C(i2c_id, freq=400_000)
    if addr not in i2c.scan():
        missing.append((i2c_id, hex(addr)))
print("missing", missing)
