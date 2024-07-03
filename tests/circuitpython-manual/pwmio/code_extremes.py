import board
import pwmio
import random
import time
import microcontroller
import os
import sys
import random

exponents = [
    2,
    3,
    4,
    5,
    6,
    7,
]

freqs = [int(10**f) for f in exponents]

top = 65536
den = 10
duties = [32767, 65535, 1, 65534, 0, 0]
freq_duration = 1.2
duty_duration = 0.2

print("\n\n")
board_name = sys.implementation[2]

pins = {
    "Feather RP2040": (("D4", ""),),
    "RP2040-Zero": (("GP15", ""),),
    "Grand Central": (("D51", "TCC"), ("A15", "TC")),
    "Metro M0": (("A2", "TC"), ("A3", "TCC")),
    "ESP32-S3-DevKit": (("IO6", ""),),  # marked D5 on board for XIAO-ESP32-s3
    "Feather ESP32-S2": (("D9", ""),),
    "XIAO nRF52840": (("D9", ""),),
}

for board_key in pins:
    if board_key in board_name:
        pins_to_use = pins[board_key]
        break

while True:
    for pin_name, pin_type in pins_to_use:
        pin = getattr(board, pin_name)
        print('title="', end="")
        print(f"{board_name} at {microcontroller.cpu.frequency} Hz, pin {pin_name}", end="")
        if len(pin_type) > 0:
            print(f" ({pin_type})", end="")
        print('",')
        print(f'subtitle="{freq_duration:0.1f}s per frequency",')
        print(f'version="{sys.version}",')
        print("freq_calls=", end="")
        pwm = pwmio.PWMOut(pin, variable_frequency=True)
        t0 = time.monotonic()
        duty_time = t0 + duty_duration
        print("(", end="")
        offset = 0
        increment = 1
        for freq in freqs:
            i = 0
            try:
                pwm.frequency = freq
            except ValueError:
                break
            freq_time = t0 + freq_duration
            duty_time = t0 + duty_duration
            j = 0
            while time.monotonic() < freq_time:
                duty = duties[j]
                pwm.duty_cycle = duty
                while time.monotonic() < duty_time and time.monotonic() < freq_time:
                    pass
                j += 1
                j = min(j, len(duties) - 1)
                duty_time += duty_duration
                i += 1
                if time.monotonic() > freq_time:
                    break
            t0 = freq_time
            print(f"({freq}, {i/freq_duration:.0f}), ", end="")
        print(")")
        print("done.")
        pwm.deinit()
        time.sleep(5)
