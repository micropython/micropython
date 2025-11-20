"""
ESP32 OTA (Over-The-Air) Firmware Update Examples

This file demonstrates all OTA update methods for ESP32 MicroPython.
"""

import network
import time


def connect_wifi(ssid, password):
    """Connect to WiFi network."""
    print("\nConnecting to WiFi...")
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)

    if not wlan.isconnected():
        print(f"Connecting to {ssid}...")
        wlan.connect(ssid, password)

        # Wait for connection
        timeout = 10
        while not wlan.isconnected() and timeout > 0:
            print(".", end="")
            time.sleep(1)
            timeout -= 1

    if wlan.isconnected():
        print("\nConnected!")
        print(f"IP address: {wlan.ifconfig()[0]}")
        return wlan.ifconfig()[0]
    else:
        print("\nFailed to connect to WiFi")
        return None


# ===================================
# Example 1: Update from HTTP URL
# ===================================
def example_http_update():
    """Download and install firmware from HTTP server."""
    import ota_update

    # Your WiFi credentials
    SSID = "YourWiFiSSID"
    PASSWORD = "YourWiFiPassword"

    # Connect to WiFi
    if not connect_wifi(SSID, PASSWORD):
        return

    # URL to firmware.bin file
    firmware_url = "http://192.168.1.100:8000/firmware.bin"

    print("\nStarting OTA update from HTTP...")
    print(f"URL: {firmware_url}")

    try:
        # Update firmware (will reboot automatically on success)
        ota_update.update(firmware_url)
    except Exception as e:
        print(f"Update failed: {e}")


# ===================================
# Example 2: Update from local file
# ===================================
def example_file_update():
    """Install firmware from a file on the filesystem."""
    import ota_update

    # Path to firmware file (uploaded via WebREPL or copied to SD card)
    firmware_file = "/firmware.bin"

    print("\nStarting OTA update from file...")
    print(f"File: {firmware_file}")

    try:
        # Update from file (will reboot automatically on success)
        ota_update.update_from_file(firmware_file)
    except Exception as e:
        print(f"Update failed: {e}")


# ===================================
# Example 3: Update with progress callback
# ===================================
def example_update_with_progress():
    """Update firmware with progress reporting."""
    import ota_update

    SSID = "YourWiFiSSID"
    PASSWORD = "YourWiFiPassword"

    if not connect_wifi(SSID, PASSWORD):
        return

    def progress(bytes_downloaded, total_bytes):
        """Progress callback function."""
        if total_bytes > 0:
            percent = (bytes_downloaded / total_bytes) * 100
            print(f"Progress: {bytes_downloaded}/{total_bytes} bytes ({percent:.1f}%)")
        else:
            print(f"Downloaded: {bytes_downloaded} bytes")

    firmware_url = "http://192.168.1.100:8000/firmware.bin"

    print("\nStarting OTA update with progress...")
    try:
        ota_update.update(firmware_url, progress_callback=progress)
    except Exception as e:
        print(f"Update failed: {e}")


# ===================================
# Example 4: Check for updates
# ===================================
def example_check_update():
    """Check if new firmware is available without downloading."""
    import ota_update

    SSID = "YourWiFiSSID"
    PASSWORD = "YourWiFiPassword"

    if not connect_wifi(SSID, PASSWORD):
        return

    # Your current version
    CURRENT_VERSION = "1.0.0"

    # URL to version.txt file (contains latest version number)
    version_url = "http://192.168.1.100:8000/version.txt"

    print(f"\nCurrent version: {CURRENT_VERSION}")
    print("Checking for updates...")

    try:
        update_available, latest_version = ota_update.check_update(version_url, CURRENT_VERSION)

        if update_available:
            print(f"New version available: {latest_version}")
            print("Run update to install new firmware")
        else:
            print("You have the latest version")

    except Exception as e:
        print(f"Check failed: {e}")


# ===================================
# Example 5: Start OTA web server
# ===================================
def example_ota_server():
    """
    Start HTTP server for OTA updates.

    Access from browser: http://ESP32_IP:8080
    Or use curl: curl -X POST -F "firmware=@firmware.bin" http://ESP32_IP:8080/update
    """
    import ota_server

    SSID = "YourWiFiSSID"
    PASSWORD = "YourWiFiPassword"

    ip = connect_wifi(SSID, PASSWORD)
    if not ip:
        return

    print("\n" + "="*50)
    print("OTA Update Server Starting")
    print("="*50)
    print(f"\nUpload page: http://{ip}:8080")
    print(f"Partition info: http://{ip}:8080/info")
    print("\nWaiting for firmware upload...")
    print("="*50 + "\n")

    # Start server (runs until Ctrl+C)
    ota_server.start(port=8080)


# ===================================
# Example 6: Get partition information
# ===================================
def example_partition_info():
    """Display OTA partition information."""
    import ota_update

    print("\n" + "="*50)
    print("ESP32 Partition Information")
    print("="*50)

    info = ota_update.get_partition_info()

    if 'error' in info:
        print(f"Error: {info['error']}")
        return

    print("\nCurrent Partition:")
    print(f"  Label: {info['current']['label']}")
    print(f"  Size: {info['current']['size']:,} bytes ({info['current']['size']/1024/1024:.2f} MB)")
    print(f"  Address: 0x{info['current']['address']:X}")

    if info['next']:
        print("\nNext OTA Partition:")
        print(f"  Label: {info['next']['label']}")
        print(f"  Size: {info['next']['size']:,} bytes ({info['next']['size']/1024/1024:.2f} MB)")
        print(f"  Address: 0x{info['next']['address']:X}")

    print(f"\nAll APP Partitions ({len(info['all_partitions'])}):")
    for i, part in enumerate(info['all_partitions'], 1):
        print(f"  {i}. {part['label']}: {part['size']:,} bytes @ 0x{part['address']:X}")


# ===================================
# Example 7: Rollback to previous firmware
# ===================================
def example_rollback():
    """Rollback to previous firmware version."""
    import ota_update

    print("\nRolling back to previous firmware...")
    print("This will reboot the ESP32")

    try:
        ota_update.rollback()
    except Exception as e:
        print(f"Rollback failed: {e}")


# ===================================
# Example 8: Automatic update check on boot
# ===================================
def setup_auto_update():
    """
    Add this to boot.py for automatic update checking on startup.
    """
    code = '''# Auto-update check (add to boot.py)
import network
import ota_update

WIFI_SSID = "YourWiFiSSID"
WIFI_PASSWORD = "YourWiFiPassword"
VERSION_URL = "http://example.com/version.txt"
FIRMWARE_URL = "http://example.com/firmware.bin"
CURRENT_VERSION = "1.0.0"

# Connect to WiFi
wlan = network.WLAN(network.STA_IF)
wlan.active(True)
if not wlan.isconnected():
    wlan.connect(WIFI_SSID, WIFI_PASSWORD)
    # Wait 10 seconds for connection
    import time
    timeout = 10
    while not wlan.isconnected() and timeout > 0:
        time.sleep(1)
        timeout -= 1

# Check for updates
if wlan.isconnected():
    try:
        update_available, latest = ota_update.check_update(VERSION_URL, CURRENT_VERSION)
        if update_available:
            print(f"New version {latest} available, updating...")
            ota_update.update(FIRMWARE_URL)
    except Exception as e:
        print(f"Auto-update check failed: {e}")
'''

    print("Auto-update code:")
    print("="*50)
    print(code)
    print("="*50)


# ===================================
# Main Menu
# ===================================
def main():
    """Run examples menu."""
    print("\n" + "="*50)
    print("ESP32 OTA Update Examples")
    print("="*50)
    print("\n1. Update from HTTP URL")
    print("2. Update from local file")
    print("3. Update with progress callback")
    print("4. Check for updates (no download)")
    print("5. Start OTA web server")
    print("6. Show partition information")
    print("7. Rollback to previous firmware")
    print("8. Show auto-update boot.py code")
    print("\n0. Exit")
    print("="*50)

    choice = input("\nEnter choice (0-8): ")

    if choice == '1':
        example_http_update()
    elif choice == '2':
        example_file_update()
    elif choice == '3':
        example_update_with_progress()
    elif choice == '4':
        example_check_update()
    elif choice == '5':
        example_ota_server()
    elif choice == '6':
        example_partition_info()
    elif choice == '7':
        example_rollback()
    elif choice == '8':
        setup_auto_update()
    elif choice == '0':
        print("Goodbye!")
    else:
        print("Invalid choice")


if __name__ == '__main__':
    main()
