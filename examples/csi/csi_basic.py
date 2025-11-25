"""
Basic CSI (Channel State Information) example for ESP32.

This example demonstrates how to:
1. Connect to a WiFi network
2. Enable CSI capture
3. Read and display CSI frames

Requirements:
- ESP32, ESP32-S2, ESP32-S3, ESP32-C3, ESP32-C5, or ESP32-C6
- Active WiFi connection
- CSI enabled in firmware build (MICROPY_PY_NETWORK_WLAN_CSI)

Usage:
    import examples.csi.csi_basic
    # Or copy this file to your device and run it directly

Author: Francesco Pace <francesco.pace@gmail.com>
"""

import network
import time

# WiFi credentials - CHANGE THESE!
WIFI_SSID = "your-ssid"
WIFI_PASSWORD = "your-password"


def connect_wifi():
    """Connect to WiFi"""
    print(f"Connecting to WiFi '{WIFI_SSID}'...")

    wlan = network.WLAN(network.STA_IF)
    if wlan.active():
        wlan.active(False)
        time.sleep(1)

    wlan.active(True)
    time.sleep(3)
    wlan.config(pm=wlan.PM_NONE)
    wlan.connect(WIFI_SSID, WIFI_PASSWORD)

    timeout = 30
    while not wlan.isconnected() and timeout > 0:
        time.sleep(1)
        timeout -= 1

    if not wlan.isconnected():
        raise Exception("WiFi connection failed")

    print(f"✅ Connected - IP: {wlan.ifconfig()[0]}")
    return wlan


def main():
    # Create WLAN interface
    wlan = connect_wifi()

    print("\n" + "=" * 50)
    print("CSI Basic Example")
    print("=" * 50)

    # Enable CSI with default settings
    # buffer_size: Number of frames to store (default: 16)
    # Each frame uses ~552 bytes of RAM
    print("\nEnabling CSI capture...")
    wlan.csi_enable(buffer_size=32)  # Store up to 32 frames

    print("CSI enabled! Waiting for frames...")
    print("\n⚠️  IMPORTANT: CSI requires active Wi-Fi traffic directed to this device.")
    print("   Generate traffic from another device using:")
    print(f"   ping -i 0.1 {wlan.ifconfig()[0]}")
    print("\nPress Ctrl+C to stop\n")

    frame_count = 0
    try:
        while True:
            # Check if frames are available
            available = wlan.csi_available()
            if available > 0:
                print(f"\n[{frame_count}] {available} frame(s) available")

                # Read a frame
                frame = wlan.csi_read()
                if frame:
                    frame_count += 1

                    # Display frame information
                    print(f"  RSSI: {frame['rssi']} dBm")
                    print(f"  Channel: {frame['channel']}")
                    print(f"  MAC: {frame['mac'].hex()}")
                    print(f"  CSI data length: {len(frame['data'])} bytes")
                    print(f"  Timestamp: {frame['timestamp']} us")
                    print(f"  Rate: {frame['rate']}")
                    print(f"  MCS: {frame['mcs']}")

                    # Display first few CSI samples
                    csi_data = frame["data"]
                    if len(csi_data) > 0:
                        print(f"  First 10 CSI samples: {list(csi_data[:10])}")

                    # Check for dropped frames
                    dropped = wlan.csi_dropped()
                    if dropped > 0:
                        print(f"  ⚠️  Dropped frames: {dropped}")
            else:
                # No frames available, wait a bit
                time.sleep(0.1)

    except KeyboardInterrupt:
        print("\n\nStopping CSI capture...")
        wlan.csi_disable()
        print("CSI disabled")
        print(f"\nTotal frames captured: {frame_count}")
        print(f"Total dropped: {wlan.csi_dropped()}")


if __name__ == "__main__":
    main()
