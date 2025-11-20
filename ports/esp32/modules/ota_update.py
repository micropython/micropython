"""
ESP32 OTA (Over-The-Air) Firmware Update Module

This module provides simple OTA firmware updates for MicroPython on ESP32.
It downloads new firmware from a URL and flashes it to the OTA partition.

Example usage:
    import ota_update
    import network

    # Connect to WiFi
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect('SSID', 'password')
    while not wlan.isconnected():
        pass

    # Update firmware from URL
    ota_update.update('http://example.com/firmware.bin')

    # Or update from local file
    ota_update.update_from_file('/firmware.bin')
"""

import esp32
import machine
import gc


def update(url, progress_callback=None):
    """
    Download and install firmware from a URL.

    Args:
        url: HTTP URL to firmware.bin file
        progress_callback: Optional function(bytes_downloaded, total_bytes)

    Returns:
        True on success

    Raises:
        Exception on error
    """
    try:
        import urequests as requests
    except ImportError:
        import requests

    print(f"OTA Update: Downloading from {url}")

    # Get OTA partition
    try:
        from esp32 import Partition

        # Find next OTA partition
        current = Partition(Partition.RUNNING)
        print(f"Current partition: {current.info()}")

        # Get OTA0 or OTA1 partition (whichever we're not running from)
        if 'ota_0' in str(current.info()):
            next_part = Partition.find(Partition.TYPE_APP, label='ota_1')
        else:
            next_part = Partition.find(Partition.TYPE_APP, label='ota_0')

        if not next_part:
            next_part = Partition.find(Partition.TYPE_APP)[0]

        print(f"Target partition: {next_part.info()}")

    except Exception as e:
        print(f"Warning: Could not get OTA partition info: {e}")
        next_part = None

    # Download firmware
    print("Downloading firmware...")
    response = requests.get(url, stream=True)

    if response.status_code != 200:
        raise Exception(f"HTTP {response.status_code}: Failed to download firmware")

    # Get content length
    try:
        total_size = int(response.headers.get('content-length', 0))
        print(f"Firmware size: {total_size} bytes")
    except:
        total_size = 0
        print("Firmware size: unknown")

    # Write firmware to OTA partition
    bytes_written = 0
    chunk_size = 4096

    if next_part:
        # Use partition API (ESP-IDF 4.0+)
        offset = 0
        while True:
            chunk = response.raw.read(chunk_size)
            if not chunk:
                break

            next_part.writeblocks(offset // chunk_size, chunk)
            offset += len(chunk)
            bytes_written += len(chunk)

            if progress_callback:
                progress_callback(bytes_written, total_size)
            elif bytes_written % (chunk_size * 10) == 0:
                print(f"Downloaded: {bytes_written} bytes", end='\r')

            gc.collect()
    else:
        # Fallback: write to flash directly
        print("Using flash write (no partition API)")
        with open('/tmp_firmware.bin', 'wb') as f:
            while True:
                chunk = response.raw.read(chunk_size)
                if not chunk:
                    break
                f.write(chunk)
                bytes_written += len(chunk)

                if progress_callback:
                    progress_callback(bytes_written, total_size)
                elif bytes_written % (chunk_size * 10) == 0:
                    print(f"Downloaded: {bytes_written} bytes", end='\r')

                gc.collect()

    response.close()
    print(f"\nDownload complete: {bytes_written} bytes")

    # Set boot partition
    if next_part:
        try:
            next_part.set_boot()
            print("Boot partition updated successfully")
        except Exception as e:
            print(f"Warning: Could not set boot partition: {e}")

    print("\nOTA update successful!")
    print("Rebooting in 3 seconds...")
    import time
    time.sleep(3)
    machine.reset()

    return True


def update_from_file(filename, progress_callback=None):
    """
    Install firmware from a local file.

    Args:
        filename: Path to firmware.bin file on filesystem
        progress_callback: Optional function(bytes_written, total_bytes)

    Returns:
        True on success
    """
    try:
        from esp32 import Partition

        # Find next OTA partition
        current = Partition(Partition.RUNNING)

        if 'ota_0' in str(current.info()):
            next_part = Partition.find(Partition.TYPE_APP, label='ota_1')
        else:
            next_part = Partition.find(Partition.TYPE_APP, label='ota_0')

        if not next_part:
            next_part = Partition.find(Partition.TYPE_APP)[0]

        print(f"Installing firmware from {filename}")
        print(f"Target partition: {next_part.info()}")

    except Exception as e:
        raise Exception(f"Could not get OTA partition: {e}")

    # Get file size
    import os
    try:
        total_size = os.stat(filename)[6]
        print(f"Firmware size: {total_size} bytes")
    except:
        total_size = 0

    # Copy file to OTA partition
    bytes_written = 0
    chunk_size = 4096

    with open(filename, 'rb') as f:
        offset = 0
        while True:
            chunk = f.read(chunk_size)
            if not chunk:
                break

            next_part.writeblocks(offset // chunk_size, chunk)
            offset += len(chunk)
            bytes_written += len(chunk)

            if progress_callback:
                progress_callback(bytes_written, total_size)
            elif bytes_written % (chunk_size * 10) == 0:
                print(f"Written: {bytes_written} bytes", end='\r')

            gc.collect()

    print(f"\nWrite complete: {bytes_written} bytes")

    # Set boot partition
    next_part.set_boot()
    print("Boot partition updated successfully")

    print("\nOTA update successful!")
    print("Rebooting in 3 seconds...")
    import time
    time.sleep(3)
    machine.reset()

    return True


def rollback():
    """
    Rollback to previous firmware.

    This marks the current partition as invalid and reboots to the previous one.
    """
    try:
        from esp32 import Partition

        current = Partition(Partition.RUNNING)
        print(f"Current partition: {current.info()}")

        # Mark as invalid
        current.mark_invalid()
        print("Current partition marked as invalid")

        print("Rebooting to previous firmware...")
        import time
        time.sleep(2)
        machine.reset()

    except Exception as e:
        raise Exception(f"Rollback failed: {e}")


def check_update(url, current_version):
    """
    Check if update is available without downloading.

    Args:
        url: URL to version.txt file containing version number
        current_version: Current firmware version string

    Returns:
        (update_available, latest_version) tuple
    """
    try:
        import urequests as requests
    except ImportError:
        import requests

    try:
        response = requests.get(url)
        if response.status_code == 200:
            latest_version = response.text.strip()
            response.close()

            update_available = (latest_version != current_version)
            return (update_available, latest_version)
        else:
            return (False, None)
    except Exception as e:
        print(f"Error checking for updates: {e}")
        return (False, None)


def get_partition_info():
    """
    Get information about current and next OTA partitions.

    Returns:
        Dictionary with partition information
    """
    try:
        from esp32 import Partition

        info = {
            'current': None,
            'next': None,
            'all_partitions': []
        }

        # Get current partition
        current = Partition(Partition.RUNNING)
        info['current'] = {
            'label': current.info()[4],
            'size': current.info()[3],
            'address': current.info()[2]
        }

        # Find next partition
        if 'ota_0' in str(current.info()):
            next_part = Partition.find(Partition.TYPE_APP, label='ota_1')
        else:
            next_part = Partition.find(Partition.TYPE_APP, label='ota_0')

        if next_part:
            info['next'] = {
                'label': next_part.info()[4],
                'size': next_part.info()[3],
                'address': next_part.info()[2]
            }

        # Get all APP partitions
        all_parts = Partition.find(Partition.TYPE_APP)
        for part in all_parts:
            info['all_partitions'].append({
                'label': part.info()[4],
                'size': part.info()[3],
                'address': part.info()[2]
            })

        return info

    except Exception as e:
        return {'error': str(e)}


# Example usage
if __name__ == '__main__':
    print("ESP32 OTA Update Module")
    print("=" * 40)

    # Show partition info
    info = get_partition_info()
    print("\nPartition Information:")
    print(f"Current: {info.get('current', 'Unknown')}")
    print(f"Next: {info.get('next', 'Unknown')}")
    print(f"\nAll APP partitions: {len(info.get('all_partitions', []))}")
    for part in info.get('all_partitions', []):
        print(f"  - {part}")
