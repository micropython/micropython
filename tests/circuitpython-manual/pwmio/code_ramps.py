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
    2.333,
    2.667,
    3,
    3.333,
    3.667,
    4,
    4.333,
    4.667,
    5,
    5.333,
    5.667,
    6,
    6.333,
    6.667,
    7,
]

freqs = [int(10**f) for f in exponents]

top = 65536
den = 10
duties = [int(top * num / den) for num in range(1, den)]
duties = [1, 65534, 1] + duties
freq_duration = 1.0
duty_duration = 0.000000001

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
            while time.monotonic() < freq_time:
                j = random.randrange(0, len(duties))
                duty = duties[j]
                if j > 1:
                    duty = duties[j] + offset
                    if duty > 65533:
                        duty -= 65533
                pwm.duty_cycle = duty
                offset += increment
                if offset > 65533:
                    offset = 0
                while time.monotonic() < duty_time and time.monotonic() < freq_time:
                    pass
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
