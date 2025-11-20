#!/usr/bin/env micropython
"""
Test Audio functionality on Raspberry Pi 5
Requires ALSA audio system (arecord/aplay)
Run with: ./build-standard/micropython examples/pi5_audio_test.py
"""

from machine import Audio
import time

print("Testing Audio on Raspberry Pi 5...")

try:
    # Initialize audio with default settings
    audio = Audio(rate=44100, channels=2, bits=16)
    print("Audio initialized successfully!")
    print(f"  Sample rate: 44100 Hz")
    print(f"  Channels: 2 (stereo)")
    print(f"  Bits per sample: 16")

    # Test microphone recording
    print("\nTesting microphone recording...")
    print("Recording 3 seconds of audio...")
    audio_data = audio.record(duration=3000)  # 3000 ms = 3 seconds
    print(f"  Recorded {len(audio_data)} bytes")

    # Calculate expected size
    # Formula: sample_rate * channels * (bits/8) * duration_sec
    expected_size = 44100 * 2 * 2 * 3  # 529,200 bytes
    print(f"  Expected: ~{expected_size} bytes")

    # Save recorded audio to file
    print("\nSaving recorded audio to 'recording.raw'...")
    with open('recording.raw', 'wb') as f:
        f.write(audio_data)
    print("  Saved! Play with: aplay -f S16_LE -c 2 -r 44100 recording.raw")

    # Test playback
    print("\nTesting audio playback...")
    print("Playing back recorded audio...")
    audio.play(audio_data)
    print("  Playback complete!")

    # Test different sample rates
    print("\nTesting different sample rates...")
    sample_rates = [22050, 44100, 48000]

    for rate in sample_rates:
        audio = Audio(rate=rate, channels=1, bits=16)
        print(f"  Recording 1 second at {rate} Hz...")
        data = audio.record(duration=1000)
        print(f"    Captured {len(data)} bytes")
        audio.deinit()

    # Test mono vs stereo
    print("\nTesting mono vs stereo...")

    print("  Recording 1 second in mono...")
    audio = Audio(rate=44100, channels=1, bits=16)
    mono_data = audio.record(duration=1000)
    print(f"    Captured {len(mono_data)} bytes")
    audio.deinit()

    print("  Recording 1 second in stereo...")
    audio = Audio(rate=44100, channels=2, bits=16)
    stereo_data = audio.record(duration=1000)
    print(f"    Captured {len(stereo_data)} bytes")
    audio.deinit()

    print("\nAudio test complete!")

except OSError as e:
    print(f"Error: {e}")
    print("Make sure:")
    print("  1. ALSA is installed (check with: which arecord aplay)")
    print("  2. Microphone is connected and enabled")
    print("  3. Audio device is not in use by another program")
    print("  4. Audio permissions are correct (may need to add user to 'audio' group)")
except Exception as e:
    print(f"Unexpected error: {e}")
