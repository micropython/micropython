#!/usr/bin/env micropython
"""
Test I2C functionality on Raspberry Pi 5
Requires I2C to be enabled and permissions to access /dev/i2c-*
Run with: sudo ./build-standard/micropython examples/pi5_i2c_test.py
"""

from machine import I2C

print("Testing I2C on Raspberry Pi 5...")

# Initialize I2C bus 1 (default for Raspberry Pi)
# On Pi 5, this is typically /dev/i2c-1
i2c = I2C(id=1, freq=100000)

print("Scanning I2C bus for devices...")
devices = i2c.scan()

if len(devices) == 0:
    print("No I2C devices found!")
    print("Make sure:")
    print("  1. I2C is enabled (check /boot/firmware/config.txt)")
    print("  2. Devices are properly connected")
    print("  3. You're running with sudo")
else:
    print(f"Found {len(devices)} I2C device(s):")
    for addr in devices:
        print(f"  - 0x{addr:02X} ({addr})")

print("\nI2C test complete!")
