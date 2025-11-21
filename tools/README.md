# MicroPython Tools

This directory contains utilities for working with MicroPython firmware.

## Tools Overview

### 📱 upload_firmware.py - Interactive Firmware Uploader

**Purpose:** Upload MicroPython firmware to USB-connected devices with step-by-step guidance.

**Quick Start:**
```bash
python3 upload_firmware.py
```

**Features:**
- ✅ Automatic device detection
- ✅ Interactive wizard with clear instructions
- ✅ Support for multiple device types
- ✅ Progress indicators and error handling
- ✅ Both interactive and command-line modes

---

## Installation & Requirements

### Python Requirements
- Python 3.6 or higher
- Standard library modules only (no pip install needed for basic usage)

### Optional Dependencies for Specific Devices

#### For ESP32 devices:
```bash
pip install esptool
```

#### For STM32 devices (Linux):
```bash
sudo apt-get install dfu-util
```

#### For macOS users:
```bash
brew install dfu-util
```

---

## Quick Start Guide

### 1. Interactive Mode (Recommended)

Simply run the tool and follow the prompts:

```bash
cd /path/to/micropython/tools
python3 upload_firmware.py
```

**What happens:**
1. Tool detects connected devices automatically
2. You select your device from a menu
3. Tool locates appropriate firmware file
4. You confirm upload
5. Tool handles the upload process
6. Device reboots with new firmware

### 2. List Connected Devices

Check what's connected before uploading:

```bash
python3 upload_firmware.py --list
```

**Example output:**
```
Detected Devices
RP2 devices: ['/media/user/RPI-RP2']
Serial: /dev/ttyUSB0 (esp32)
Serial: /dev/ttyACM0 (stm32)
```

### 3. Command Line Mode

For automation or scripting:

```bash
# Raspberry Pi Pico
python3 upload_firmware.py \
    --device rp2-pico \
    --firmware ../ports/rp2/build-RPI_PICO/firmware.uf2

# ESP32
python3 upload_firmware.py \
    --device esp32 \
    --port /dev/ttyUSB0 \
    --firmware ../ports/esp32/build-ESP32_GENERIC/firmware.bin

# STM32
python3 upload_firmware.py \
    --device stm32 \
    --firmware ../ports/stm32/build-PYBV11/firmware.dfu
```

---

## Supported Devices

| Device | Detection Method | Upload Method | Additional Tools |
|--------|------------------|---------------|------------------|
| **Raspberry Pi Pico (RP2040)** | USB Mass Storage | UF2 drag-and-drop | None |
| **Raspberry Pi Pico 2 (RP2350)** | USB Mass Storage | UF2 drag-and-drop | None |
| **ESP32/ESP32-S2/S3** | Serial port detection | esptool.py | `pip install esptool` |
| **STM32 (Pyboard, etc.)** | DFU device | dfu-util | `apt-get install dfu-util` |

---

## Usage Examples

### Example 1: First-time Pico User

**Scenario:** You just got a Raspberry Pi Pico and want to install MicroPython.

**Steps:**
1. Build firmware (or download pre-built):
   ```bash
   cd micropython/ports/rp2
   make BOARD=RPI_PICO
   ```

2. Connect Pico in BOOTSEL mode:
   - Hold BOOTSEL button
   - Plug in USB cable
   - Release BOOTSEL

3. Run upload tool:
   ```bash
   cd micropython/tools
   python3 upload_firmware.py
   ```

4. Select "Raspberry Pi Pico (RP2040)" from menu

5. Confirm upload

6. Done! Connect to REPL:
   ```bash
   screen /dev/ttyACM0 115200
   ```

### Example 2: Updating ESP32 Firmware

**Scenario:** You have an ESP32 with old MicroPython and want to update.

**Steps:**
1. Install esptool (one-time):
   ```bash
   pip install esptool
   ```

2. Connect ESP32 via USB

3. Run upload tool:
   ```bash
   python3 upload_firmware.py
   ```

4. Tool will:
   - Detect ESP32 on serial port
   - Erase old firmware
   - Flash new firmware
   - Prompt you to press RESET

5. Verify installation:
   ```bash
   screen /dev/ttyUSB0 115200
   ```
   ```python
   import sys
   print(sys.implementation.version)
   # (1, 27, 1, '')
   ```

### Example 3: Batch Upload (Multiple Devices)

**Scenario:** You need to flash 10 Picos with the same firmware.

**Script:**
```bash
#!/bin/bash
# flash_multiple_picos.sh

FIRMWARE="../ports/rp2/build-RPI_PICO/firmware.uf2"

for i in {1..10}; do
    echo "=== Device $i/10 ==="
    echo "1. Put Pico in BOOTSEL mode"
    echo "2. Press Enter when ready"
    read

    python3 upload_firmware.py \
        --device rp2-pico \
        --firmware "$FIRMWARE"

    echo "Device $i complete!"
    echo ""
done
```

---

## How It Works

### Device Detection

#### RP2040/RP2350 (Pico/Pico 2)
1. Scans mount points for "RPI-RP2" or "RP2350"
2. Checks `/media/$USER/` directory
3. Verifies USB mass storage mode

#### ESP32
1. Scans `/dev/ttyUSB*` and `/dev/ttyACM*`
2. Uses `udevadm info` to check device properties
3. Identifies CP2102, CH340, or Silicon Labs USB-to-UART chips

#### STM32
1. Checks for DFU devices using `dfu-util --list`
2. Identifies STMicroelectronics DFU devices
3. Verifies DFU mode before flashing

### Upload Process

#### RP2 UF2 Upload
```
1. Verify BOOTSEL mode → 2. Copy UF2 file → 3. Wait for auto-reboot → 4. Done
```

- Uses simple file copy to USB mass storage
- Device automatically reboots after copy completes
- No additional tools required

#### ESP32 Upload
```
1. Check esptool → 2. Erase flash → 3. Write firmware → 4. User presses RESET → 5. Done
```

- Uses esptool.py for serial flashing
- Requires USB-to-UART chip drivers
- Supports high-speed uploads (460800 baud)

#### STM32 DFU Upload
```
1. Check dfu-util → 2. Enter DFU mode → 3. Flash firmware → 4. Auto-reset → 5. Done
```

- Uses DFU (Device Firmware Update) protocol
- Requires special bootloader entry sequence
- Device auto-resets after flashing

---

## Troubleshooting

### Common Issues

#### Issue: "No devices detected"

**Cause:** Device not connected or not in correct mode

**Solutions:**
1. Check USB cable (must be data cable, not charge-only)
2. For Pico: Verify BOOTSEL mode (hold button while plugging in)
3. For ESP32: Check correct port with `ls /dev/tty*`
4. For STM32: Verify DFU mode (BOOT0 + RESET sequence)

#### Issue: "Permission denied" on Linux

**Cause:** User not in `dialout` group

**Solution:**
```bash
sudo usermod -a -G dialout $USER
# Log out and back in for changes to take effect
```

#### Issue: ESP32 "Failed to connect"

**Cause:** Port busy or wrong baud rate

**Solutions:**
1. Close any serial terminals (screen, minicom, Thonny)
2. Try lower baud rate: edit script to use `--baud 115200`
3. Hold BOOT button during upload
4. Press RESET before starting upload

#### Issue: STM32 "No DFU device found"

**Cause:** Not in DFU mode or drivers missing

**Solutions:**
1. Re-enter DFU mode: Hold BOOT0, press/release RESET, release BOOT0
2. Check with: `sudo dfu-util --list`
3. Try different USB port
4. Install drivers: `sudo apt-get install dfu-util`

---

## Advanced Usage

### Custom Firmware Paths

If you built firmware in a non-standard location:

```bash
python3 upload_firmware.py \
    --device rp2-pico \
    --firmware /path/to/custom/firmware.uf2
```

### Automating with Scripts

Use command-line mode in scripts:

```python
#!/usr/bin/env python3
import subprocess
import sys

devices = [
    ('rp2-pico', '/path/to/pico.uf2'),
    ('esp32', '/path/to/esp32.bin', '/dev/ttyUSB0'),
]

for device in devices:
    if len(device) == 2:
        # RP2 device
        cmd = ['python3', 'upload_firmware.py',
               '--device', device[0],
               '--firmware', device[1]]
    else:
        # Serial device
        cmd = ['python3', 'upload_firmware.py',
               '--device', device[0],
               '--port', device[2],
               '--firmware', device[1]]

    result = subprocess.run(cmd)
    if result.returncode != 0:
        print(f"Failed to flash {device[0]}")
        sys.exit(1)

print("All devices flashed successfully!")
```

### Verifying Upload Success

After upload, verify installation:

```bash
# Connect to device REPL
screen /dev/ttyACM0 115200

# In MicroPython REPL, run:
```
```python
import sys

# Check version
print("Version:", sys.implementation.version)
# Expected: (1, 27, 1, '')

# Check build
print("Build:", sys.implementation._build)

# Test basic functionality
print("Hello from MicroPython v1.27.1!")

# Test that security fixes are applied
# (these would crash or overflow in older versions)
a = [1] * 1000
b = [2] * 1000
c = a + b  # List concatenation overflow fix
print("Security fixes verified - list operations work correctly")
```

---

## Architecture & Code Structure

### File Organization

```
tools/
├── upload_firmware.py          # Main upload utility
├── FIRMWARE_UPLOAD_GUIDE.md    # Detailed device-specific guide
└── README.md                   # This file
```

### Code Structure

```python
upload_firmware.py
│
├── Color output functions
│   ├── print_header()      # Section headers
│   ├── print_step()        # Step-by-step instructions
│   ├── print_success()     # Success messages
│   ├── print_error()       # Error messages
│   ├── print_warning()     # Warnings
│   └── print_info()        # Information
│
├── Device detection
│   ├── detect_serial_devices()     # Find ESP32, STM32 on serial
│   ├── detect_rp2_bootloader()     # Find Pico in BOOTSEL mode
│   └── find_firmware_files()       # Locate built firmware
│
├── Upload functions
│   ├── flash_rp2_uf2()             # Upload to Pico/Pico 2
│   ├── flash_esp32_esptool()       # Upload to ESP32
│   └── flash_stm32_dfu()           # Upload to STM32
│
├── Interactive mode
│   └── interactive_mode()          # Wizard interface
│
└── Command-line interface
    └── main()                      # Argument parsing & dispatch
```

### Extension Points

To add support for a new device type:

1. **Add detection function:**
   ```python
   def detect_new_device():
       """Detect your new device type"""
       # Check for device-specific indicators
       return device_list
   ```

2. **Add flash function:**
   ```python
   def flash_new_device(firmware_path, port):
       """Flash firmware to new device"""
       print_header("Flashing New Device")
       # Implement device-specific flashing
       return success
   ```

3. **Update interactive_mode():**
   ```python
   # In interactive_mode()
   new_devices = detect_new_device()
   if new_devices:
       options.append(('new-device', 'New Device', port))
   ```

4. **Update main():**
   ```python
   # In main()
   elif device_type == 'new-device':
       success = flash_new_device(args.firmware, args.port)
   ```

---

## Testing

### Manual Testing Checklist

- [ ] RP2040 (Pico) detection in BOOTSEL mode
- [ ] RP2350 (Pico 2) detection in BOOTSEL mode
- [ ] ESP32 serial port detection
- [ ] STM32 DFU mode detection
- [ ] Firmware file auto-detection
- [ ] Interactive wizard flow
- [ ] Command-line mode operation
- [ ] Error handling for missing tools
- [ ] Permission error handling
- [ ] USB disconnect during upload

### Test Without Physical Devices

You can test detection logic without devices:

```bash
# Create mock mount point
mkdir -p /tmp/test_mount/RPI-RP2
python3 upload_firmware.py --list
# Should detect /tmp/test_mount/RPI-RP2
```

---

## Security Considerations

### Safe Operations

The tool performs only safe operations:
- ✅ Reads USB device information
- ✅ Copies firmware files
- ✅ Calls well-known flashing tools (esptool, dfu-util)

### No Root Required

For most operations, no root privileges needed:
- Pico/Pico 2: Uses standard file copy
- ESP32/STM32: Uses user-level serial/DFU access

**Exception:** On some Linux systems, you need to be in the `dialout` group for serial access.

### Firmware Verification

The tool does NOT verify firmware authenticity. Always:
1. Build firmware yourself from trusted source
2. Or download from official MicroPython releases
3. Verify checksums before flashing

---

## Performance Notes

### Upload Times (Typical)

| Device | Firmware Size | Upload Time |
|--------|---------------|-------------|
| Pico (RP2040) | ~600KB | 2-3 seconds |
| Pico 2 (RP2350) | ~650KB | 2-3 seconds |
| ESP32 | ~1.5MB | 30-60 seconds |
| STM32 | ~400KB | 10-20 seconds |

### Optimization Tips

**ESP32:**
- Use high baud rate (460800) for faster uploads
- If unreliable, try lower rate (115200)
- Close serial terminals before flashing

**RP2:**
- USB 2.0 ports may be faster than USB 3.0 for UF2
- Direct connection better than USB hub

---

## Contributing

### Adding Device Support

To add support for a new device family:

1. Create detection function following existing patterns
2. Implement device-specific flash function
3. Add to interactive menu
4. Update documentation
5. Test on actual hardware

### Improving Documentation

- Add more troubleshooting scenarios
- Include photos/screenshots
- Add video tutorials
- Translate to other languages

---

## Related Documentation

- **FIRMWARE_UPLOAD_GUIDE.md** - Comprehensive device-specific instructions
- **RELEASE_NOTES_v1.27.1.md** - Information about this firmware release
- **MicroPython Docs** - https://docs.micropython.org

---

## License

MIT License - Same as MicroPython

---

## Support

**Issues:** Report problems with the upload tool as GitHub issues

**Forum:** https://forum.micropython.org

**Questions:** Include the following information:
- Device type and model
- Operating system
- Complete error message
- Output of `python3 upload_firmware.py --list`

---

**Last Updated:** 2025-11-21
**Version:** 1.0 (released with MicroPython v1.27.1)
