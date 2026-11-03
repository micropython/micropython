# SPDX-FileCopyrightText: 2026 Mikey Sklar for Adafruit Industries
#
# SPDX-License-Identifier: MIT

# With USB connected, the display from boot.py should still be in use after the fake deep sleep.
# The CLUE makes its own display with rotation 0, so rotation 90 means the display was kept.
# If USB is not up 5 s after a hard reset (seen on ESP32-S3), sleep later or the sleep is true.
import alarm
import displayio
import supervisor
import time

display = supervisor.runtime.display
print("wake alarm:", alarm.wake_alarm, "display:", display)
if alarm.wake_alarm is None:
    alarm.exit_and_deep_sleep_until_alarms(
        alarm.time.TimeAlarm(monotonic_time=time.monotonic() + 8)
    )

print("rotation:", display.rotation, "PASS" if display.rotation == 90 else "FAIL")
palette = displayio.Palette(1)
palette[0] = 0xFF0000
group = displayio.Group()
group.append(displayio.TileGrid(displayio.Bitmap(240, 60, 1), pixel_shader=palette))
display.root_group = group
print("drawn")
while True:
    time.sleep(1)
