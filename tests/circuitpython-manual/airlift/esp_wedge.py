# SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
#
# SPDX-License-Identifier: MIT
"""Board side of esp_wedge_host.py: wedge the ESP32 on purpose and keep the VM
running, so the cleanup at the end of the VM run can't fix it. The core should
notice its own commands failing, hard-reset the ESP32 and bring web workflow
back. nina-fw 3.1.0 stops answering SPI commands until a hard reset when
START_SERVER_TCP (0x28) comes before a WiFi join, which _airlift.reset() undoes."""

import struct
import time

import _airlift

_airlift.reset()
try:
    _airlift.transact(0x28, (struct.pack(">H", 5000), (0,), (0,)))
except OSError as e:
    print("START_SERVER_TCP before WiFi:", repr(e))
print("WEDGED")
end = time.monotonic() + 40
while time.monotonic() < end:
    time.sleep(0.1)
# A VM command works again only if the core reset the ESP32.
try:
    print("status after:", _airlift.transact(0x20)[0][0])
    print("PASS")
except OSError as e:
    print("still wedged:", repr(e))
