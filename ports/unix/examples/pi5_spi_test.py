#!/usr/bin/env micropython
"""
Test SPI functionality on Raspberry Pi 5
Requires SPI to be enabled and permissions to access /dev/spidev*
Run with: sudo ./build-standard/micropython examples/pi5_spi_test.py
"""

from machine import SPI

print("Testing SPI on Raspberry Pi 5...")

# Initialize SPI bus 0 (default for Raspberry Pi)
# On Pi 5, this is typically /dev/spidev0.0
try:
    spi = SPI(id=0, baudrate=1000000, polarity=0, phase=0)
    print("SPI initialized successfully!")
    print(f"  Baudrate: 1000000 Hz")
    print(f"  Polarity: 0")
    print(f"  Phase: 0")

    # Test write
    print("\nSending test data...")
    test_data = bytes([0x01, 0x02, 0x03, 0x04, 0x05])
    spi.write(test_data)
    print(f"  Sent: {' '.join(f'0x{b:02X}' for b in test_data)}")

    # Test read
    print("\nReading 5 bytes...")
    read_data = spi.read(5)
    print(f"  Received: {' '.join(f'0x{b:02X}' for b in read_data)}")

    print("\nSPI test complete!")

except OSError as e:
    print(f"Error: {e}")
    print("Make sure:")
    print("  1. SPI is enabled (check /boot/firmware/config.txt)")
    print("  2. /dev/spidev0.0 exists")
    print("  3. You're running with sudo")
