#!/usr/bin/env micropython
"""
Test Display (framebuffer) functionality on Raspberry Pi 5
Requires framebuffer device /dev/fb0
Run with: sudo ./build-standard/micropython examples/pi5_display_test.py
"""

from machine import Display
import time

print("Testing Display on Raspberry Pi 5...")

try:
    # Initialize display (framebuffer)
    display = Display(device=0)
    print("Display initialized successfully!")
    print(f"  Device: /dev/fb0")
    print(f"  Resolution: {display.width}x{display.height}")
    print(f"  Bits per pixel: {display.bpp}")

    # Fill screen with red
    print("\nFilling screen with red...")
    display.fill(0xFF0000)  # RGB: Red
    time.sleep(1)

    # Fill screen with green
    print("Filling screen with green...")
    display.fill(0x00FF00)  # RGB: Green
    time.sleep(1)

    # Fill screen with blue
    print("Filling screen with blue...")
    display.fill(0x0000FF)  # RGB: Blue
    time.sleep(1)

    # Draw some pixels
    print("\nDrawing individual pixels...")
    display.fill(0x000000)  # Clear to black

    # Draw a diagonal line of white pixels
    for i in range(min(display.width, display.height)):
        display.pixel(i, i, 0xFFFFFF)  # White pixel

    time.sleep(1)

    # Draw a simple pattern
    print("Drawing pattern...")
    display.fill(0x000000)  # Clear to black

    # Draw colored squares
    colors = [0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00, 0xFF00FF, 0x00FFFF]
    square_size = 50

    for i, color in enumerate(colors):
        x = (i % 3) * square_size * 2
        y = (i // 3) * square_size * 2

        for dx in range(square_size):
            for dy in range(square_size):
                if x + dx < display.width and y + dy < display.height:
                    display.pixel(x + dx, y + dy, color)

    time.sleep(2)

    # Clear screen
    print("\nClearing screen to black...")
    display.fill(0x000000)

    # Clean up
    display.deinit()
    print("\nDisplay test complete!")

except OSError as e:
    print(f"Error: {e}")
    print("Make sure:")
    print("  1. Framebuffer device /dev/fb0 exists")
    print("  2. You have proper permissions (run with sudo)")
    print("  3. Display is connected and configured")
    print("  4. You're not running in headless mode without a display")
except Exception as e:
    print(f"Unexpected error: {e}")
