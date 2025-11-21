# ESP32 MicroPython Firmware Flashing Guide

Complete guide to building and flashing MicroPython firmware to ESP32 devices using esptool.

---

## Table of Contents
1. [Install esptool](#install-esptool)
2. [Connect Your ESP32](#connect-your-esp32)
3. [Build Firmware](#build-firmware)
4. [Flash Firmware](#flash-firmware)
5. [Verify Installation](#verify-installation)
6. [Troubleshooting](#troubleshooting)

---

## Install esptool

### Method 1: Using pip (Recommended)
```bash
# Install esptool.py
pip3 install esptool

# Verify installation
esptool.py version
```

### Method 2: Using apt (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install esptool
```

### Method 3: From Source
```bash
git clone https://github.com/espressif/esptool.git
cd esptool
pip3 install .
```

---

## Connect Your ESP32

### 1. Physical Connection
- Connect ESP32 to your computer via USB cable
- Most ESP32 boards use USB-to-Serial chips (CP210x, CH340, FTDI)

### 2. Find Serial Port
```bash
# Linux
ls /dev/ttyUSB*
# or
ls /dev/ttyACM*

# macOS
ls /dev/cu.usbserial*
# or
ls /dev/cu.usbmodem*

# Windows (PowerShell)
Get-WmiObject Win32_SerialPort | Select-Object Name,DeviceID
```

### 3. Set Permissions (Linux only)
```bash
# Add your user to dialout group
sudo usermod -a -G dialout $USER

# Log out and back in, or use:
newgrp dialout

# Verify permissions
ls -l /dev/ttyUSB0
```

---

## Build Firmware

### Prerequisites
1. **Install ESP-IDF v5.5.1** (recommended version)
```bash
# Clone ESP-IDF
git clone -b v5.5.1 --recursive https://github.com/espressif/esp-idf.git
cd esp-idf

# Install toolchain
./install.sh esp32   # or esp32s2, esp32s3, esp32c3, etc.

# Set up environment (run this in every new terminal)
source export.sh
```

2. **Build MicroPython cross-compiler**
```bash
cd /path/to/micropython
make -C mpy-cross
```

### Build for Different ESP32 Variants

#### ESP32 (Original)
```bash
cd ports/esp32
make submodules
make BOARD=ESP32_GENERIC
```

#### ESP32-S2
```bash
make BOARD=ESP32_GENERIC_S2
```

#### ESP32-S3
```bash
make BOARD=ESP32_GENERIC_S3
```

#### ESP32-C3
```bash
make BOARD=ESP32_GENERIC_C3
```

#### ESP32-C6
```bash
make BOARD=ESP32_GENERIC_C6
```

### Build Output
Firmware will be in `build-BOARDNAME/` directory:
- `firmware.bin` - Combined firmware (bootloader + partitions + application)
- `bootloader/bootloader.bin` - Bootloader only
- `partition_table/partition-table.bin` - Partition table
- `micropython.bin` - MicroPython application

---

## Flash Firmware

### Auto-detect ESP32 and Flash
```bash
# Erase flash first (recommended for first-time install)
esptool.py erase_flash

# Flash firmware (esptool.py auto-detects port and chip)
esptool.py write_flash 0x1000 build-ESP32_GENERIC/firmware.bin
```

### Manual Port and Chip Selection
```bash
# Specify port and chip type
esptool.py --port /dev/ttyUSB0 --chip esp32 write_flash 0x1000 firmware.bin

# For ESP32-S3
esptool.py --port /dev/ttyUSB0 --chip esp32s3 write_flash 0x0 firmware.bin

# For ESP32-C3
esptool.py --port /dev/ttyUSB0 --chip esp32c3 write_flash 0x0 firmware.bin
```

### High-Speed Flashing
```bash
# Use higher baud rate for faster flashing
esptool.py --baud 921600 write_flash 0x1000 firmware.bin

# Common baud rates: 115200, 460800, 921600
```

### Flash Individual Components (Advanced)
```bash
# Flash bootloader, partition table, and application separately
esptool.py --chip esp32 write_flash \
    0x1000 build-ESP32_GENERIC/bootloader/bootloader.bin \
    0x8000 build-ESP32_GENERIC/partition_table/partition-table.bin \
    0x10000 build-ESP32_GENERIC/micropython.bin
```

### Using Make (Convenience)
```bash
cd ports/esp32

# Erase flash
make erase

# Build and flash in one command
make deploy

# Specify port
make deploy PORT=/dev/ttyUSB0
```

---

## Verify Installation

### 1. Check Flash Info
```bash
esptool.py flash_id
```

### 2. Read MAC Address
```bash
esptool.py read_mac
```

### 3. Connect to REPL
```bash
# Using picocom
picocom -b 115200 /dev/ttyUSB0

# Using screen
screen /dev/ttyUSB0 115200

# Using miniterm.py
miniterm.py /dev/ttyUSB0 115200

# Using mpremote (if installed)
mpremote connect /dev/ttyUSB0
```

### 4. Test MicroPython
```python
>>> import sys
>>> sys.implementation
(name='micropython', version=(1, 23, 0), _machine='ESP32 module with ESP32', _mpy=6406)

>>> import esp
>>> esp.flash_size()
4194304  # 4MB flash

>>> import network
>>> wlan = network.WLAN(network.STA_IF)
>>> wlan.active(True)
```

---

## Complete Flashing Script

Create a script `flash_esp32.sh`:

```bash
#!/bin/bash
set -e

# Configuration
PORT=${1:-/dev/ttyUSB0}
CHIP=${2:-esp32}
FIRMWARE=${3:-build-ESP32_GENERIC/firmware.bin}
BAUD=460800

echo "==================================="
echo "ESP32 Firmware Flashing Script"
echo "==================================="
echo "Port: $PORT"
echo "Chip: $CHIP"
echo "Firmware: $FIRMWARE"
echo "Baud Rate: $BAUD"
echo ""

# Check if esptool is installed
if ! command -v esptool.py &> /dev/null; then
    echo "ERROR: esptool.py not found!"
    echo "Install with: pip3 install esptool"
    exit 1
fi

# Check if firmware file exists
if [ ! -f "$FIRMWARE" ]; then
    echo "ERROR: Firmware file not found: $FIRMWARE"
    exit 1
fi

# Check if port exists
if [ ! -e "$PORT" ]; then
    echo "ERROR: Serial port not found: $PORT"
    echo "Available ports:"
    ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null || echo "No USB serial ports found"
    exit 1
fi

# Erase flash
echo "Step 1: Erasing flash..."
esptool.py --port $PORT --chip $CHIP erase_flash

# Flash firmware
echo ""
echo "Step 2: Flashing firmware..."
esptool.py --port $PORT --chip $CHIP --baud $BAUD write_flash 0x1000 $FIRMWARE

# Verify
echo ""
echo "Step 3: Verifying flash..."
esptool.py --port $PORT flash_id

echo ""
echo "==================================="
echo "Flashing complete!"
echo "==================================="
echo "Connect to REPL with:"
echo "  picocom -b 115200 $PORT"
echo "or"
echo "  mpremote connect $PORT"
```

Usage:
```bash
chmod +x flash_esp32.sh
./flash_esp32.sh /dev/ttyUSB0 esp32 build-ESP32_GENERIC/firmware.bin
```

---

## Pre-built Firmware Downloads

### Option 1: Official MicroPython Downloads
Download pre-built firmware from:
https://micropython.org/download/esp32/

```bash
# Download latest stable
wget https://micropython.org/resources/firmware/esp32-20231005-v1.21.0.bin

# Flash
esptool.py write_flash 0x1000 esp32-20231005-v1.21.0.bin
```

### Option 2: Build from This Repository
```bash
cd /path/to/micropython/ports/esp32
make BOARD=ESP32_GENERIC
# Firmware at: build-ESP32_GENERIC/firmware.bin
```

---

## Troubleshooting

### Issue 1: "Failed to connect to ESP32"
**Solutions:**
```bash
# 1. Put ESP32 in bootloader mode manually
#    - Hold BOOT button
#    - Press and release RESET button
#    - Release BOOT button
#    Then run esptool command

# 2. Try lower baud rate
esptool.py --baud 115200 write_flash 0x1000 firmware.bin

# 3. Check USB cable (use data cable, not charge-only cable)
```

### Issue 2: "Permission denied" on /dev/ttyUSB0
```bash
# Temporary fix
sudo chmod 666 /dev/ttyUSB0

# Permanent fix
sudo usermod -a -G dialout $USER
# Then log out and back in
```

### Issue 3: Driver Issues (Windows)
- Install CH340/CH341 driver: http://www.wch.cn/downloads/CH341SER_EXE.html
- Install CP210x driver: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
- Install FTDI driver: https://ftdichip.com/drivers/vcp-drivers/

### Issue 4: "A fatal error occurred: MD5 of file does not match"
```bash
# Erase flash and try again
esptool.py erase_flash
esptool.py write_flash 0x1000 firmware.bin
```

### Issue 5: Continuous Reboots After Flashing
```bash
# Wrong flash mode - rebuild with correct flash mode
cd ports/esp32
make BOARD=ESP32_GENERIC BOARD_VARIANT=SPIRAM  # if you have SPIRAM
make clean
make
```

### Issue 6: "Timed out waiting for packet header"
```bash
# 1. Reduce baud rate
esptool.py --baud 115200 write_flash 0x1000 firmware.bin

# 2. Check USB cable quality

# 3. Try different USB port (avoid USB hubs)
```

---

## Advanced: OTA (Over-The-Air) Updates

After initial flashing, you can update firmware wirelessly:

```python
# On ESP32 (connected to WiFi)
import network
import espota

# Connect to WiFi first
wlan = network.WLAN(network.STA_IF)
wlan.active(True)
wlan.connect('SSID', 'password')

# Enable OTA updates
espota.enable()
```

From your computer:
```bash
# Install espota
pip3 install espota

# Flash new firmware over WiFi
espota.py -i <ESP32_IP_ADDRESS> -f firmware.bin
```

---

## Quick Reference Card

```bash
# Install esptool
pip3 install esptool

# Find port
ls /dev/ttyUSB*   # Linux
ls /dev/cu.*      # macOS

# Set permissions (Linux)
sudo usermod -a -G dialout $USER

# Erase flash
esptool.py erase_flash

# Flash firmware
esptool.py write_flash 0x1000 firmware.bin

# Connect to REPL
picocom -b 115200 /dev/ttyUSB0

# Exit picocom: Ctrl+A then Ctrl+X
# Exit screen: Ctrl+A then :quit
```

---

## Supported ESP32 Variants

| Chip | Flash Offset | Notes |
|------|-------------|-------|
| ESP32 | 0x1000 | Original ESP32 |
| ESP32-S2 | 0x1000 | USB native support |
| ESP32-S3 | 0x0 | Dual-core, USB native |
| ESP32-C3 | 0x0 | RISC-V based |
| ESP32-C6 | 0x0 | Wi-Fi 6 support |
| ESP32-C2 | 0x0 | Low-cost variant |

---

## Resources

- **esptool GitHub:** https://github.com/espressif/esptool
- **ESP-IDF Documentation:** https://docs.espressif.com/projects/esp-idf/
- **MicroPython ESP32 Docs:** https://docs.micropython.org/en/latest/esp32/
- **ESP32 Datasheet:** https://www.espressif.com/en/products/socs/esp32
- **MicroPython Forum:** https://forum.micropython.org/

---

## Summary

**Complete workflow:**
1. `pip3 install esptool` - Install tool
2. `cd ports/esp32 && make BOARD=ESP32_GENERIC` - Build firmware
3. `esptool.py erase_flash` - Erase old firmware
4. `esptool.py write_flash 0x1000 firmware.bin` - Flash new firmware
5. `picocom -b 115200 /dev/ttyUSB0` - Connect to REPL

**That's it! Your ESP32 is now running MicroPython!**
