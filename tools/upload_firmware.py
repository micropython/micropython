#!/usr/bin/env python3
"""
MicroPython Firmware Upload Utility
====================================

This tool helps you upload MicroPython firmware to USB-connected devices.
Supports: Raspberry Pi Pico/Pico 2, ESP32, STM32, and other boards.

Usage:
    python3 upload_firmware.py [--device DEVICE_TYPE] [--port PORT] [--firmware FIRMWARE_FILE]
"""

import os
import sys
import subprocess
import argparse
import time
import glob
from pathlib import Path

# Color codes for terminal output
class Colors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def print_header(message):
    print(f"\n{Colors.HEADER}{Colors.BOLD}{'='*70}{Colors.ENDC}")
    print(f"{Colors.HEADER}{Colors.BOLD}{message:^70}{Colors.ENDC}")
    print(f"{Colors.HEADER}{Colors.BOLD}{'='*70}{Colors.ENDC}\n")

def print_step(step_num, message):
    print(f"{Colors.OKCYAN}{Colors.BOLD}Step {step_num}:{Colors.ENDC} {message}")

def print_success(message):
    print(f"{Colors.OKGREEN}✓ {message}{Colors.ENDC}")

def print_error(message):
    print(f"{Colors.FAIL}✗ {message}{Colors.ENDC}")

def print_warning(message):
    print(f"{Colors.WARNING}⚠ {message}{Colors.ENDC}")

def print_info(message):
    print(f"{Colors.OKBLUE}ℹ {message}{Colors.ENDC}")

# Device detection functions
def detect_serial_devices():
    """Detect serial devices (ESP32, STM32, etc.)"""
    devices = []

    # Linux serial devices
    for device in glob.glob('/dev/ttyUSB*') + glob.glob('/dev/ttyACM*'):
        try:
            # Try to get device info
            result = subprocess.run(['udevadm', 'info', device],
                                   capture_output=True, text=True, timeout=2)
            info = result.stdout

            device_info = {
                'port': device,
                'type': 'unknown'
            }

            # Detect ESP32
            if 'CP210' in info or 'CH340' in info or 'SLAB_USBtoUART' in info:
                device_info['type'] = 'esp32'
            # Detect STM32
            elif 'STM' in info or 'DFU' in info:
                device_info['type'] = 'stm32'
            # Detect RP2040
            elif 'Pico' in info or '2e8a' in info:
                device_info['type'] = 'rp2'

            devices.append(device_info)
        except:
            pass

    return devices

def detect_rp2_bootloader():
    """Detect RP2040/RP2350 in BOOTSEL mode (appears as USB mass storage)"""
    # Check for RPI-RP2 mount point
    mount_points = []

    try:
        result = subprocess.run(['mount'], capture_output=True, text=True)
        for line in result.stdout.split('\n'):
            if 'RPI-RP2' in line or 'RP2350' in line:
                parts = line.split()
                if len(parts) >= 3:
                    mount_points.append(parts[2])
    except:
        pass

    # Also check /media for mounted drives
    media_paths = []
    for user_dir in Path('/media').iterdir():
        for mount in user_dir.iterdir():
            if 'RPI-RP2' in mount.name or 'RP2' in mount.name:
                media_paths.append(str(mount))

    return mount_points + media_paths

def find_firmware_files():
    """Find built firmware files in the repository"""
    repo_root = Path(__file__).parent.parent
    firmware_files = {}

    # RP2 UF2 files
    rp2_path = repo_root / 'ports' / 'rp2' / 'build-RPI_PICO'
    if rp2_path.exists():
        uf2_files = list(rp2_path.glob('*.uf2'))
        if uf2_files:
            firmware_files['rp2-pico'] = uf2_files[0]

    rp2_2_path = repo_root / 'ports' / 'rp2' / 'build-RPI_PICO2'
    if rp2_2_path.exists():
        uf2_files = list(rp2_2_path.glob('*.uf2'))
        if uf2_files:
            firmware_files['rp2-pico2'] = uf2_files[0]

    # ESP32 bin files
    esp32_path = repo_root / 'ports' / 'esp32' / 'build-ESP32_GENERIC'
    if esp32_path.exists():
        bin_files = list(esp32_path.glob('*.bin'))
        if bin_files:
            firmware_files['esp32'] = bin_files[0]

    # STM32 dfu/hex files
    stm32_path = repo_root / 'ports' / 'stm32' / 'build-PYBV11'
    if stm32_path.exists():
        dfu_files = list(stm32_path.glob('*.dfu'))
        if dfu_files:
            firmware_files['stm32'] = dfu_files[0]

    return firmware_files

# Flashing functions
def flash_rp2_uf2(firmware_path, mount_point):
    """Flash RP2040/RP2350 via UF2 bootloader"""
    print_header("Flashing Raspberry Pi Pico/Pico 2")

    print_step(1, "Verify device is in BOOTSEL mode")
    if not mount_point:
        print_error("No RP2 device found in BOOTSEL mode")
        print_info("To enter BOOTSEL mode:")
        print("  1. Disconnect the Pico from USB")
        print("  2. Hold down the BOOTSEL button")
        print("  3. While holding BOOTSEL, connect USB cable")
        print("  4. Release BOOTSEL button")
        print("  5. Device should appear as USB drive 'RPI-RP2'")
        return False

    print_success(f"Found RP2 device at: {mount_point}")

    print_step(2, "Copy firmware to device")
    firmware_dest = Path(mount_point) / Path(firmware_path).name

    try:
        import shutil
        shutil.copy2(firmware_path, firmware_dest)
        print_success(f"Firmware copied to {firmware_dest}")

        print_step(3, "Wait for device to reboot")
        print_info("Device will automatically reboot and dismount...")
        time.sleep(2)

        print_success("Firmware upload complete!")
        print_info("Your Pico should now be running MicroPython v1.27.1")
        return True

    except Exception as e:
        print_error(f"Failed to copy firmware: {e}")
        return False

def flash_esp32_esptool(firmware_path, port):
    """Flash ESP32 using esptool.py"""
    print_header("Flashing ESP32")

    print_step(1, "Check for esptool.py")
    try:
        result = subprocess.run(['esptool.py', 'version'],
                              capture_output=True, text=True, timeout=5)
        print_success(f"Found esptool.py: {result.stdout.split()[0]}")
    except:
        print_error("esptool.py not found")
        print_info("Install with: pip install esptool")
        return False

    print_step(2, f"Erase flash on {port}")
    try:
        print_info("Erasing flash (this may take 10-30 seconds)...")
        subprocess.run(['esptool.py', '--port', port, 'erase_flash'],
                      check=True, timeout=60)
        print_success("Flash erased")
    except subprocess.CalledProcessError as e:
        print_error(f"Flash erase failed: {e}")
        return False

    print_step(3, f"Write firmware to {port}")
    try:
        print_info("Writing firmware (this may take 30-60 seconds)...")
        subprocess.run([
            'esptool.py',
            '--chip', 'esp32',
            '--port', port,
            '--baud', '460800',
            'write_flash',
            '-z',
            '0x1000', str(firmware_path)
        ], check=True, timeout=120)
        print_success("Firmware written successfully")

        print_step(4, "Reset device")
        print_info("Press the RESET button on your ESP32")

        print_success("Firmware upload complete!")
        return True

    except subprocess.CalledProcessError as e:
        print_error(f"Firmware write failed: {e}")
        return False

def flash_stm32_dfu(firmware_path, port=None):
    """Flash STM32 using DFU mode"""
    print_header("Flashing STM32")

    print_step(1, "Check for dfu-util")
    try:
        result = subprocess.run(['dfu-util', '--version'],
                              capture_output=True, text=True, timeout=5)
        print_success("Found dfu-util")
    except:
        print_error("dfu-util not found")
        print_info("Install with: sudo apt-get install dfu-util")
        return False

    print_step(2, "Put STM32 in DFU mode")
    print_info("To enter DFU mode:")
    print("  1. Hold down the BOOT0 button")
    print("  2. Press and release RESET button")
    print("  3. Release BOOT0 button")
    input(f"{Colors.WARNING}Press Enter when device is in DFU mode...{Colors.ENDC}")

    print_step(3, "List DFU devices")
    try:
        result = subprocess.run(['dfu-util', '--list'],
                              capture_output=True, text=True, timeout=5)
        if 'Found DFU' in result.stdout:
            print_success("DFU device detected")
        else:
            print_error("No DFU device found")
            return False
    except:
        print_error("Failed to list DFU devices")
        return False

    print_step(4, "Flash firmware")
    try:
        print_info("Flashing (this may take 10-30 seconds)...")
        subprocess.run([
            'dfu-util',
            '-a', '0',
            '-D', str(firmware_path),
            '-s', ':leave'
        ], check=True, timeout=60)
        print_success("Firmware flashed successfully")

        print_success("Firmware upload complete!")
        print_info("Device should automatically reset and run MicroPython")
        return True

    except subprocess.CalledProcessError as e:
        print_error(f"Flashing failed: {e}")
        return False

# Interactive mode
def interactive_mode():
    """Interactive firmware upload wizard"""
    print_header("MicroPython Firmware Upload Wizard")
    print(f"Repository: {Path(__file__).parent.parent}")
    print(f"MicroPython Version: v1.27.1")

    print_step(1, "Detect connected devices")

    # Detect RP2 in bootloader mode
    rp2_mounts = detect_rp2_bootloader()
    if rp2_mounts:
        print_success(f"Found RP2 device in BOOTSEL mode: {rp2_mounts[0]}")

    # Detect serial devices
    serial_devices = detect_serial_devices()
    if serial_devices:
        print_success(f"Found {len(serial_devices)} serial device(s)")
        for i, dev in enumerate(serial_devices, 1):
            print(f"  {i}. {dev['port']} ({dev['type']})")

    if not rp2_mounts and not serial_devices:
        print_warning("No MicroPython devices detected")
        print_info("Please connect your device and try again")
        return

    print_step(2, "Select device type to flash")
    print("Available options:")
    options = []

    if rp2_mounts:
        options.append(('rp2-pico', 'Raspberry Pi Pico (RP2040)', rp2_mounts[0]))
        options.append(('rp2-pico2', 'Raspberry Pi Pico 2 (RP2350)', rp2_mounts[0]))

    for dev in serial_devices:
        if dev['type'] == 'esp32':
            options.append(('esp32', f"ESP32 at {dev['port']}", dev['port']))
        elif dev['type'] == 'stm32':
            options.append(('stm32', f"STM32 at {dev['port']}", dev['port']))

    for i, (dev_type, desc, port) in enumerate(options, 1):
        print(f"  {i}. {desc}")

    try:
        choice = int(input(f"\n{Colors.BOLD}Enter choice (1-{len(options)}): {Colors.ENDC}"))
        if choice < 1 or choice > len(options):
            print_error("Invalid choice")
            return

        device_type, device_desc, device_port = options[choice - 1]

    except (ValueError, KeyError):
        print_error("Invalid input")
        return

    print_step(3, "Locate firmware file")
    firmware_files = find_firmware_files()

    if device_type in firmware_files:
        firmware_path = firmware_files[device_type]
        print_success(f"Found firmware: {firmware_path}")
    else:
        print_warning(f"No built firmware found for {device_type}")
        firmware_path = input("Enter firmware file path: ").strip()
        if not os.path.exists(firmware_path):
            print_error("Firmware file not found")
            return

    print_step(4, "Upload firmware")
    print_info(f"Device: {device_desc}")
    print_info(f"Firmware: {firmware_path}")

    confirm = input(f"\n{Colors.WARNING}Proceed with upload? (yes/no): {Colors.ENDC}")
    if confirm.lower() not in ['yes', 'y']:
        print_info("Upload cancelled")
        return

    # Flash based on device type
    if 'rp2' in device_type:
        success = flash_rp2_uf2(firmware_path, device_port)
    elif device_type == 'esp32':
        success = flash_esp32_esptool(firmware_path, device_port)
    elif device_type == 'stm32':
        success = flash_stm32_dfu(firmware_path, device_port)
    else:
        print_error(f"Unsupported device type: {device_type}")
        return

    if success:
        print(f"\n{Colors.OKGREEN}{Colors.BOLD}{'='*70}{Colors.ENDC}")
        print(f"{Colors.OKGREEN}{Colors.BOLD}{'SUCCESS: Firmware uploaded!':^70}{Colors.ENDC}")
        print(f"{Colors.OKGREEN}{Colors.BOLD}{'='*70}{Colors.ENDC}\n")

        print_info("Next steps:")
        print("  1. Connect to device serial port (e.g., screen /dev/ttyACM0 115200)")
        print("  2. Test MicroPython REPL")
        print("  3. Verify version with: import sys; print(sys.implementation.version)")

def main():
    parser = argparse.ArgumentParser(description='MicroPython Firmware Upload Utility')
    parser.add_argument('--device', help='Device type (rp2-pico, rp2-pico2, esp32, stm32)')
    parser.add_argument('--port', help='Serial port or mount point')
    parser.add_argument('--firmware', help='Path to firmware file')
    parser.add_argument('--list', action='store_true', help='List detected devices')

    args = parser.parse_args()

    if args.list:
        print_header("Detected Devices")
        rp2_mounts = detect_rp2_bootloader()
        if rp2_mounts:
            print(f"RP2 devices: {rp2_mounts}")
        serial_devs = detect_serial_devices()
        if serial_devs:
            for dev in serial_devs:
                print(f"Serial: {dev['port']} ({dev['type']})")
        return

    if args.device and args.firmware:
        # Non-interactive mode
        if 'rp2' in args.device:
            mounts = detect_rp2_bootloader()
            if mounts:
                flash_rp2_uf2(args.firmware, mounts[0])
        elif args.device == 'esp32' and args.port:
            flash_esp32_esptool(args.firmware, args.port)
        elif args.device == 'stm32':
            flash_stm32_dfu(args.firmware, args.port)
        else:
            print_error("Missing required arguments")
    else:
        # Interactive mode
        interactive_mode()

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print(f"\n{Colors.WARNING}Upload cancelled by user{Colors.ENDC}")
        sys.exit(1)
