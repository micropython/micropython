#!/usr/bin/env micropython
"""
Test GPIO functionality on Raspberry Pi 5
Requires permissions to access /dev/gpiochip*
Run with: sudo ./build-standard/micropython examples/pi5_gpio_test.py
"""

from machine import Pin
import time

print("Testing GPIO on Raspberry Pi 5...")

# Test GPIO 23 as output (can connect an LED here)
led_pin = Pin(23, Pin.OUT)

print("Blinking GPIO 23 for 5 seconds...")
for i in range(10):
    led_pin.on()
    print(f"  LED ON (iteration {i+1})")
    time.sleep(0.5)
    led_pin.off()
    print(f"  LED OFF (iteration {i+1})")
    time.sleep(0.5)

# Test GPIO 24 as input with pull-up
button_pin = Pin(24, Pin.IN, pull=Pin.PULL_UP)
print(f"\nGPIO 24 input value (with pull-up): {button_pin.value()}")

# Test toggle
led_pin.on()
print("\nTesting toggle function...")
for i in range(5):
    led_pin.toggle()
    print(f"  After toggle {i+1}: {led_pin.value()}")
    time.sleep(0.3)

print("\nGPIO test complete!")
