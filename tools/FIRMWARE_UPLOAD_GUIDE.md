# MicroPython Firmware Upload Guide

Complete guide for uploading MicroPython v1.27.1 firmware to your devices.

## Quick Start

```bash
cd /home/user/micropython/tools
python3 upload_firmware.py
```

The interactive wizard will:
1. Detect connected devices
2. Guide you through device-specific steps
3. Upload firmware automatically
4. Verify successful installation

---

## Supported Devices

### ✓ Raspberry Pi Pico / Pico W (RP2040)
- **Method:** UF2 bootloader (drag-and-drop)
- **Firmware:** `ports/rp2/build-RPI_PICO/firmware.uf2`
- **No additional tools required**

### ✓ Raspberry Pi Pico 2 / Pico 2 W (RP2350) [NEW!]
- **Method:** UF2 bootloader (drag-and-drop)
- **Firmware:** `ports/rp2/build-RPI_PICO2/firmware.uf2`
- **No additional tools required**

### ✓ ESP32 / ESP32-S2 / ESP32-S3
- **Method:** esptool.py (serial)
- **Firmware:** `ports/esp32/build-ESP32_GENERIC/firmware.bin`
- **Required:** `pip install esptool`

### ✓ STM32 Boards (Pyboard, etc.)
- **Method:** DFU mode
- **Firmware:** `ports/stm32/build-PYBV11/firmware.dfu`
- **Required:** `sudo apt-get install dfu-util`

---

## Installation Methods by Device

### Method 1: Raspberry Pi Pico/Pico 2 (Easiest!)

#### Step 1: Enter BOOTSEL Mode
1. **Disconnect** the Pico from USB
2. **Hold down** the BOOTSEL button (white button on board)
3. **While holding BOOTSEL**, connect USB cable to computer
4. **Release** BOOTSEL button
5. Device appears as USB drive named **RPI-RP2**

#### Step 2: Run Upload Tool
```bash
python3 upload_firmware.py
```

Select your Pico model when prompted:
- Option 1: Raspberry Pi Pico (RP2040)
- Option 2: Raspberry Pi Pico 2 (RP2350)

#### Step 3: Automatic Upload
The tool will:
- Copy firmware.uf2 to the RPI-RP2 drive
- Device automatically reboots
- MicroPython v1.27.1 is now running!

#### Step 4: Connect to REPL
```bash
# Linux
screen /dev/ttyACM0 115200

# macOS
screen /dev/cu.usbmodem* 115200

# Or use Thonny IDE
```

---

### Method 2: ESP32

#### Prerequisites
```bash
pip install esptool
```

#### Step 1: Connect ESP32
- Connect ESP32 to USB
- Note the serial port (usually `/dev/ttyUSB0` or `/dev/ttyACM0`)
- Check with: `ls /dev/tty*`

#### Step 2: Run Upload Tool
```bash
python3 upload_firmware.py
```

Select ESP32 option when prompted.

#### Step 3: Automatic Process
The tool will:
1. Erase flash (10-30 seconds)
2. Write firmware (30-60 seconds)
3. Prompt you to press RESET

#### Step 4: Verify
```bash
screen /dev/ttyUSB0 115200
```

In MicroPython REPL:
```python
import sys
print(sys.implementation.version)
# Should show: (1, 27, 1, '')
```

---

### Method 3: STM32

#### Prerequisites
```bash
sudo apt-get install dfu-util
```

#### Step 1: Enter DFU Mode
1. Locate **BOOT0** and **RESET** buttons on your board
2. **Hold down BOOT0**
3. **Press and release RESET**
4. **Release BOOT0**
5. Board is now in DFU mode

#### Step 2: Verify DFU Mode
```bash
dfu-util --list
```
Should show "Found DFU device"

#### Step 3: Run Upload Tool
```bash
python3 upload_firmware.py
```

Select STM32 option, confirm DFU mode when prompted.

#### Step 4: Flash and Reset
- Tool flashes firmware (10-30 seconds)
- Board automatically resets
- MicroPython v1.27.1 ready!

---

## Manual Upload (Without Tool)

### Manual Method: Pico/Pico 2

```bash
# 1. Enter BOOTSEL mode (hold BOOTSEL, plug USB)

# 2. Find mount point
mount | grep RPI-RP2

# 3. Copy firmware
cp ports/rp2/build-RPI_PICO/firmware.uf2 /media/$USER/RPI-RP2/

# Device auto-reboots
```

### Manual Method: ESP32

```bash
# 1. Erase flash
esptool.py --port /dev/ttyUSB0 erase_flash

# 2. Flash firmware
esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 460800 write_flash -z 0x1000 ports/esp32/build-ESP32_GENERIC/firmware.bin

# 3. Press RESET button
```

### Manual Method: STM32

```bash
# 1. Enter DFU mode (BOOT0 + RESET)

# 2. Flash
dfu-util -a 0 -D ports/stm32/build-PYBV11/firmware.dfu -s :leave
```

---

## Building Firmware First

Before uploading, you need to build firmware for your target device.

### Build for Raspberry Pi Pico
```bash
cd ports/rp2
make clean
make BOARD=RPI_PICO
# Output: build-RPI_PICO/firmware.uf2
```

### Build for Raspberry Pi Pico 2
```bash
cd ports/rp2
make clean
make BOARD=RPI_PICO2
# Output: build-RPI_PICO2/firmware.uf2
```

### Build for ESP32
```bash
cd ports/esp32
make clean
make BOARD=ESP32_GENERIC
# Output: build-ESP32_GENERIC/firmware.bin
```

### Build for STM32
```bash
cd ports/stm32
make clean
make BOARD=PYBV11
# Output: build-PYBV11/firmware.dfu
```

---

## Command Line Usage

### Interactive Mode (Recommended)
```bash
python3 upload_firmware.py
```

### List Detected Devices
```bash
python3 upload_firmware.py --list
```

### Non-Interactive Upload
```bash
# Pico
python3 upload_firmware.py --device rp2-pico --firmware /path/to/firmware.uf2

# ESP32
python3 upload_firmware.py --device esp32 --port /dev/ttyUSB0 --firmware /path/to/firmware.bin

# STM32
python3 upload_firmware.py --device stm32 --firmware /path/to/firmware.dfu
```

---

## Troubleshooting

### Pico Not Appearing as USB Drive

**Problem:** Pico doesn't show up as RPI-RP2

**Solutions:**
1. Make sure you're holding BOOTSEL **before** plugging USB
2. Try a different USB cable (must be data cable, not charge-only)
3. Try a different USB port
4. Check system logs: `dmesg | tail`
5. On Linux, check: `ls /media/$USER/`

### ESP32 Upload Fails

**Problem:** "Failed to connect to ESP32"

**Solutions:**
1. Check correct port: `ls /dev/ttyUSB* /dev/ttyACM*`
2. Add user to dialout group: `sudo usermod -a -G dialout $USER`
3. Re-login after adding to group
4. Try lower baud rate: `--baud 115200` instead of 460800
5. Hold BOOT button during upload
6. Press RESET before upload

### STM32 DFU Mode Issues

**Problem:** "No DFU device found"

**Solutions:**
1. Verify DFU mode entry sequence:
   - Hold BOOT0
   - Press & release RESET
   - Release BOOT0
2. Check USB connection
3. Try different USB port
4. Install DFU drivers (Windows)
5. Check permissions: `sudo dfu-util --list`

### Permission Denied Errors

**Linux:**
```bash
# Add user to dialout group
sudo usermod -a -G dialout $USER

# Or use sudo
sudo python3 upload_firmware.py
```

**macOS:**
```bash
# Grant terminal full disk access in System Preferences
```

---

## Verify Installation

After uploading firmware, connect to the device REPL:

### Test MicroPython
```python
# Connect via serial terminal
screen /dev/ttyACM0 115200  # Linux
# or
screen /dev/cu.usbmodem* 115200  # macOS

# In MicroPython REPL:
import sys
print("MicroPython version:", sys.implementation.version)
# Should show: MicroPython version: (1, 27, 1, '')

print("Build:", sys.implementation._build)
# Should show: Build: standard

# Test basic functionality
print("Hello from MicroPython v1.27.1!")

# Test new security fixes
a = [1, 2, 3]
b = [4, 5, 6]
c = a + b  # List concatenation (overflow fix applied)
print("List test passed:", c)
```

---

## Firmware Locations

After building, firmware files are located at:

| Device | Firmware Path |
|--------|---------------|
| **Pico (RP2040)** | `ports/rp2/build-RPI_PICO/firmware.uf2` |
| **Pico 2 (RP2350)** | `ports/rp2/build-RPI_PICO2/firmware.uf2` |
| **Pico W** | `ports/rp2/build-RPI_PICO_W/firmware.uf2` |
| **Pico 2 W** | `ports/rp2/build-RPI_PICO2_W/firmware.uf2` |
| **ESP32** | `ports/esp32/build-ESP32_GENERIC/firmware.bin` |
| **ESP32-S2** | `ports/esp32/build-ESP32_GENERIC_S2/firmware.bin` |
| **ESP32-S3** | `ports/esp32/build-ESP32_GENERIC_S3/firmware.bin` |
| **STM32 Pyboard** | `ports/stm32/build-PYBV11/firmware.dfu` |
| **Unix** | `ports/unix/build-standard/micropython` |

---

## Security Note

**MicroPython v1.27.1 includes 40 security fixes:**
- 2 CRITICAL (WebREPL buffer overflows)
- 8 HIGH severity
- 28 MEDIUM severity
- 2 documented (architectural changes needed)

**Upgrading is strongly recommended**, especially for:
- WebREPL users
- Network applications
- Production deployments

---

## Additional Resources

- **MicroPython Docs:** https://docs.micropython.org
- **Forum:** https://forum.micropython.org
- **GitHub:** https://github.com/micropython/micropython
- **Release Notes:** See `RELEASE_NOTES_v1.27.1.md` in repository root

---

## Getting Help

If you encounter issues:

1. **Check device logs:**
   ```bash
   dmesg | tail -20
   ```

2. **Verify USB connection:**
   ```bash
   lsusb
   ```

3. **Test with manual method** (see above)

4. **Report issues** with:
   - Device model
   - Operating system
   - Upload tool output
   - Error messages

---

**Happy Flashing! 🚀**

MicroPython v1.27.1 - More secure, more capable, better documented.
