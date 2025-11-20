#!/usr/bin/env micropython
"""
Test Camera functionality on Raspberry Pi 5
Requires V4L2-compatible camera (e.g., Raspberry Pi Camera Module 3)
Run with: sudo ./build-standard/micropython examples/pi5_camera_test.py
"""

from machine import Camera
import time

print("Testing Camera on Raspberry Pi 5...")

try:
    # Initialize camera with default settings
    # /dev/video0 is typically the first camera device
    camera = Camera(device=0, width=640, height=480, format='RGB')
    print("Camera initialized successfully!")
    print(f"  Device: /dev/video{0}")
    print(f"  Resolution: 640x480")
    print(f"  Format: RGB24")

    # Capture a single frame
    print("\nCapturing frame...")
    frame = camera.capture()
    print(f"  Captured {len(frame)} bytes")
    print(f"  Expected: {640 * 480 * 3} bytes (640x480 RGB)")

    # Save frame to file (raw RGB data)
    print("\nSaving frame to 'capture.rgb'...")
    with open('capture.rgb', 'wb') as f:
        f.write(frame)
    print("  Saved! Convert with: ffmpeg -f rawvideo -pixel_format rgb24 -video_size 640x480 -i capture.rgb capture.png")

    # Test different resolutions
    print("\nTesting different resolutions...")
    resolutions = [(320, 240), (640, 480), (1280, 720)]

    for width, height in resolutions:
        camera = Camera(device=0, width=width, height=height, format='RGB')
        frame = camera.capture()
        expected_size = width * height * 3
        print(f"  {width}x{height}: captured {len(frame)} bytes (expected {expected_size})")
        camera.deinit()

    print("\nCamera test complete!")

except OSError as e:
    print(f"Error: {e}")
    print("Make sure:")
    print("  1. Camera is connected and enabled")
    print("  2. /dev/video0 exists (check with: ls -l /dev/video*)")
    print("  3. You have proper permissions (run with sudo)")
    print("  4. V4L2 drivers are loaded (check with: v4l2-ctl --list-devices)")
except Exception as e:
    print(f"Unexpected error: {e}")
