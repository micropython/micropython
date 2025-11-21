# Quick Start: Upload Firmware in 3 Minutes

**Goal:** Get MicroPython v1.27.1 running on your device in under 3 minutes.

---

## Step 1: Choose Your Device (10 seconds)

<table>
<tr>
<td><strong>I have a Raspberry Pi Pico</strong></td>
<td>→ <a href="#pico-quick-start">Pico Quick Start</a></td>
</tr>
<tr>
<td><strong>I have an ESP32</strong></td>
<td>→ <a href="#esp32-quick-start">ESP32 Quick Start</a></td>
</tr>
<tr>
<td><strong>I have a Pyboard/STM32</strong></td>
<td>→ <a href="#stm32-quick-start">STM32 Quick Start</a></td>
</tr>
</table>

---

## Pico Quick Start

### Total Time: ~2 minutes

#### 1. Enter BOOTSEL Mode (20 seconds)
```
1. Hold BOOTSEL button on Pico
2. Plug USB cable into computer
3. Release BOOTSEL button
```
✓ You should see a drive named **RPI-RP2**

#### 2. Run Upload Tool (30 seconds)
```bash
cd micropython/tools
python3 upload_firmware.py
```

#### 3. Follow Prompts (30 seconds)
- Tool detects your Pico automatically
- Select "Raspberry Pi Pico" from menu
- Type 'yes' to confirm
- Done!

#### 4. Connect to REPL (40 seconds)
```bash
# Linux
screen /dev/ttyACM0 115200

# macOS
screen /dev/cu.usbmodem* 115200

# Or use Thonny IDE
```

Test it:
```python
>>> print("Hello MicroPython v1.27.1!")
Hello MicroPython v1.27.1!
```

**That's it! You're done!** ✅

---

## ESP32 Quick Start

### Total Time: ~3 minutes (includes first-time setup)

#### 1. Install esptool (ONE TIME, 30 seconds)
```bash
pip install esptool
```

#### 2. Connect ESP32 (10 seconds)
- Plug ESP32 into USB
- Note the port: usually `/dev/ttyUSB0`

#### 3. Run Upload Tool (2 minutes)
```bash
cd micropython/tools
python3 upload_firmware.py
```

#### 4. Follow Prompts
- Tool detects ESP32 automatically
- Erases old firmware (~30 seconds)
- Writes new firmware (~60 seconds)
- Prompts you to press RESET

#### 5. Connect to REPL
```bash
screen /dev/ttyUSB0 115200
```

Test it:
```python
>>> import sys
>>> print(sys.implementation.version)
(1, 27, 1, '')
```

**Done!** ✅

---

## STM32 Quick Start

### Total Time: ~3 minutes (includes first-time setup)

#### 1. Install dfu-util (ONE TIME, 30 seconds)
```bash
# Linux
sudo apt-get install dfu-util

# macOS
brew install dfu-util
```

#### 2. Enter DFU Mode (30 seconds)
```
1. Hold BOOT0 button
2. Press and release RESET button
3. Release BOOT0 button
```

#### 3. Verify DFU Mode (10 seconds)
```bash
dfu-util --list
```
You should see "Found DFU device"

#### 4. Run Upload Tool (90 seconds)
```bash
cd micropython/tools
python3 upload_firmware.py
```

#### 5. Follow Prompts
- Select STM32 from menu
- Tool flashes firmware (~30 seconds)
- Board auto-resets

#### 6. Connect to REPL
```bash
screen /dev/ttyACM0 115200
```

**Done!** ✅

---

## Troubleshooting (30 seconds)

### "No devices detected"

**Pico:**
- Make sure you held BOOTSEL **before** plugging in USB
- Try a different USB cable (must be data cable)

**ESP32:**
- Check port: `ls /dev/ttyUSB* /dev/ttyACM*`
- Add yourself to dialout group: `sudo usermod -a -G dialout $USER`
- Log out and back in

**STM32:**
- Re-do DFU sequence: BOOT0 + RESET
- Check: `sudo dfu-util --list`

### "Permission denied"

```bash
# Add yourself to dialout group (one time)
sudo usermod -a -G dialout $USER

# Then log out and log back in
```

### Still stuck?

```bash
# Run with verbose output
python3 upload_firmware.py --list

# Check system logs
dmesg | tail -20
```

---

## Command Cheat Sheet

```bash
# Interactive mode (easiest)
python3 upload_firmware.py

# List devices
python3 upload_firmware.py --list

# Direct upload (Pico)
python3 upload_firmware.py --device rp2-pico --firmware /path/to/firmware.uf2

# Direct upload (ESP32)
python3 upload_firmware.py --device esp32 --port /dev/ttyUSB0 --firmware firmware.bin

# Direct upload (STM32)
python3 upload_firmware.py --device stm32 --firmware firmware.dfu
```

---

## Build Firmware First (Optional)

If firmware files don't exist yet:

```bash
# Pico
cd micropython/ports/rp2
make BOARD=RPI_PICO

# Pico 2
make BOARD=RPI_PICO2

# ESP32
cd micropython/ports/esp32
make BOARD=ESP32_GENERIC

# STM32
cd micropython/ports/stm32
make BOARD=PYBV11
```

---

## Verify Installation (30 seconds)

After uploading, connect to REPL and run:

```python
import sys

# Check version (should be 1.27.1)
print("Version:", sys.implementation.version)

# Test basic functionality
print("Hello from MicroPython!")

# Test security fixes (would crash in old versions)
a = [1, 2, 3] * 100
b = [4, 5, 6] * 100
c = a + b  # Integer overflow fix
print("Security fixes verified!")
```

Expected output:
```
Version: (1, 27, 1, '')
Hello from MicroPython!
Security fixes verified!
```

✅ **All good!**

---

## What's Next?

### Try Example Code
```python
# Blink LED (Pico)
from machine import Pin
import time

led = Pin(25, Pin.OUT)
while True:
    led.toggle()
    time.sleep(0.5)

# Scan I2C bus (ESP32)
from machine import I2C, Pin
i2c = I2C(0, scl=Pin(22), sda=Pin(21))
devices = i2c.scan()
print("I2C devices:", [hex(d) for d in devices])

# Read ADC (Pico)
from machine import ADC
adc = ADC(26)
voltage = adc.read_u16() * 3.3 / 65535
print(f"Voltage: {voltage:.2f}V")
```

### Learn More
- **Full Guide:** `FIRMWARE_UPLOAD_GUIDE.md` - Detailed instructions
- **Tool Docs:** `README.md` - Complete documentation
- **MicroPython Docs:** https://docs.micropython.org
- **Forum:** https://forum.micropython.org

---

## Common Workflows

### Update Existing Installation
```bash
# 1. Build new firmware
cd micropython/ports/rp2
make clean && make BOARD=RPI_PICO

# 2. Upload
cd micropython/tools
python3 upload_firmware.py
```

### Flash Multiple Devices
```bash
# Interactive mode makes it easy
for i in {1..10}; do
    echo "Device $i - Connect and press Enter"
    read
    python3 upload_firmware.py
done
```

### Automated Deployment
```bash
# Non-interactive mode for CI/CD
python3 upload_firmware.py \
    --device rp2-pico \
    --firmware ../ports/rp2/build-RPI_PICO/firmware.uf2
```

---

## Support

**Need help?** Include this info when asking:
- Device type (Pico, ESP32, STM32)
- Operating system
- Output of: `python3 upload_firmware.py --list`
- Complete error message

**Report issues:** GitHub issues in micropython/micropython

---

**Time to first blink: 2-3 minutes** ⚡

**Questions?** Check `FIRMWARE_UPLOAD_GUIDE.md` for detailed answers.
