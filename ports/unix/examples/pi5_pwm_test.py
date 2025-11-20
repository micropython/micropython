#!/usr/bin/env micropython
"""
Test PWM functionality on Raspberry Pi 5
Requires PWM to be enabled and permissions to access /sys/class/pwm/
Run with: sudo ./build-standard/micropython examples/pi5_pwm_test.py
"""

from machine import PWM
import time

print("Testing PWM on Raspberry Pi 5...")

try:
    # Initialize PWM on chip 0, channel 0
    # Note: You may need to adjust chip/channel based on your setup
    pwm = PWM(chip=0, channel=0, freq=1000, duty_u16=32768)
    print("PWM initialized successfully!")
    print(f"  Frequency: 1000 Hz")
    print(f"  Duty cycle: 50% (32768/65535)")

    # Test different duty cycles
    print("\nTesting different duty cycles...")
    duty_cycles = [0, 16384, 32768, 49152, 65535]
    duty_percentages = [0, 25, 50, 75, 100]

    for duty, percent in zip(duty_cycles, duty_percentages):
        pwm.duty_u16(duty)
        print(f"  Duty cycle set to {percent}% ({duty}/65535)")
        time.sleep(1)

    # Test different frequencies
    print("\nTesting different frequencies...")
    frequencies = [100, 500, 1000, 2000, 5000]

    for freq in frequencies:
        pwm.freq(freq)
        print(f"  Frequency set to {freq} Hz")
        time.sleep(1)

    # Clean up
    pwm.deinit()
    print("\nPWM test complete!")

except OSError as e:
    print(f"Error: {e}")
    print("Make sure:")
    print("  1. PWM is enabled in device tree")
    print("  2. /sys/class/pwm/ directory exists")
    print("  3. You have proper permissions (run with sudo)")
    print("  4. PWM chip/channel numbers are correct for your setup")
