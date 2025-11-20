#!/usr/bin/env micropython
"""
Comprehensive multimedia demo for Raspberry Pi 5
Demonstrates integration of Camera, Display, Audio, and AI capabilities

This example shows a simple video preview with audio monitoring
Run with: sudo ./build-standard/micropython examples/pi5_multimedia_demo.py
"""

from machine import Camera, Display, Audio
import time

print("="*60)
print("Raspberry Pi 5 Multimedia Demo")
print("="*60)
print("\nThis demo showcases:")
print("  - Camera: Live video capture")
print("  - Display: Framebuffer output")
print("  - Audio: Sound level monitoring")
print("="*60 + "\n")

def show_progress_bar(label, value, max_value, width=40):
    """Display a simple progress bar"""
    percentage = min(100, int((value / max_value) * 100))
    filled = int((percentage / 100) * width)
    bar = '█' * filled + '░' * (width - filled)
    print(f"\r{label}: [{bar}] {percentage}%", end='', flush=True)

try:
    # Initialize hardware
    print("Initializing hardware...")

    # Camera setup
    print("  - Camera...")
    camera = Camera(device=0, width=320, height=240, format='RGB')
    print("    ✓ Camera ready (320x240)")

    # Display setup
    print("  - Display...")
    display = Display(device=0)
    print(f"    ✓ Display ready ({display.width}x{display.height})")

    # Audio setup
    print("  - Audio...")
    audio = Audio(rate=44100, channels=1, bits=16)
    print("    ✓ Audio ready (44.1kHz mono)")

    print("\nHardware initialization complete!\n")

    # Demo loop
    print("Starting multimedia demo (press Ctrl+C to stop)...")
    print("-" * 60)

    frame_count = 0
    start_time = time.time()

    while True:
        # Capture camera frame
        frame = camera.capture()
        frame_count += 1

        # Display frame info
        if frame_count % 10 == 0:  # Update every 10 frames
            elapsed = time.time() - start_time
            fps = frame_count / elapsed if elapsed > 0 else 0

            print(f"\nFrame {frame_count}:")
            print(f"  Captured: {len(frame)} bytes")
            print(f"  FPS: {fps:.1f}")

            # Sample audio level
            audio_sample = audio.record(duration=100)  # 100ms sample
            if len(audio_sample) > 0:
                # Calculate approximate audio level (very simplified)
                avg_level = sum(audio_sample) / len(audio_sample)
                max_level = max(audio_sample)

                # Show audio level bar
                show_progress_bar("  Audio level", max_level, 255)
                print()  # New line after progress bar

        # Optional: Display the frame on screen
        # Note: This requires proper RGB to framebuffer format conversion
        # which would be more complex in a real implementation

        # Small delay to control frame rate
        time.sleep(0.033)  # ~30 FPS

except KeyboardInterrupt:
    print("\n\nDemo stopped by user")

except OSError as e:
    print(f"\nError: {e}")
    print("\nTroubleshooting:")
    print("  Camera issues:")
    print("    - Check /dev/video0 exists: ls -l /dev/video*")
    print("    - Enable camera: sudo raspi-config > Interface > Camera")
    print("  Display issues:")
    print("    - Check /dev/fb0 exists: ls -l /dev/fb*")
    print("    - Ensure you're running on Pi with display connected")
    print("  Audio issues:")
    print("    - Check ALSA tools: which arecord aplay")
    print("    - Test microphone: arecord -d 1 test.wav")

except Exception as e:
    print(f"\nUnexpected error: {e}")

finally:
    # Cleanup
    print("\nCleaning up...")
    try:
        camera.deinit()
        display.deinit()
        audio.deinit()
        print("  ✓ All hardware released")
    except:
        pass

print("\n" + "="*60)
print("Demo complete!")
print("="*60)
