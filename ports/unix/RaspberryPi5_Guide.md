# MicroPython for Raspberry Pi 5 - Complete Guide

This guide explains how to compile, install, and use MicroPython with full hardware control on Raspberry Pi 5.

## Features

MicroPython on Pi 5 now supports:

### Core Hardware
- ✅ **GPIO** - Full Pin control via Linux GPIO character device
- ✅ **I2C** - I2C communication via Linux I2C devices
- ✅ **SPI** - SPI communication via Linux SPI devices
- ✅ **PWM** - PWM output via sysfs PWM interface

### Multimedia
- ✅ **Camera** - Video capture via V4L2 (Video4Linux2)
- ✅ **Display** - Framebuffer control for screen output
- ✅ **Audio** - Microphone recording and speaker playback via ALSA

### AI & Machine Learning
- ✅ **TensorFlow Lite** - AI inference module (API wrapper)

## Prerequisites

### On Your Development Machine (or Pi 5)

```bash
sudo apt update
sudo apt install -y build-essential git python3 pkg-config libffi-dev
```

## Compilation Instructions

### 1. Clone the Repository

```bash
git clone https://github.com/bob10042/micropython.git
cd micropython
git checkout claude/review-open-issues-01NPUBr5vTbxDB8Fzcrudky6
```

### 2. Build the Cross-Compiler

```bash
cd mpy-cross
make
cd ..
```

### 3. Build the Unix Port

```bash
cd ports/unix
make submodules
make
```

The compiled binary will be at: `ports/unix/build-standard/micropython`

### 4. Install (Optional)

```bash
sudo make install
```

This installs to `/usr/local/bin/micropython`

## Hardware Setup on Raspberry Pi 5

### Enable I2C

Edit `/boot/firmware/config.txt` and add:
```
dtparam=i2c_arm=on
```

Reboot and verify:
```bash
ls /dev/i2c-*
```

### Enable SPI

Edit `/boot/firmware/config.txt` and add:
```
dtparam=spi=on
```

Reboot and verify:
```bash
ls /dev/spidev*
```

### Enable PWM (Optional)

PWM requires device tree overlay configuration. This is more advanced and depends on your specific use case.

### Enable Camera

For Raspberry Pi Camera Module:

Edit `/boot/firmware/config.txt` and add:
```
camera_auto_detect=1
```

Or for specific camera:
```
dtoverlay=imx708  # For Camera Module 3
```

Reboot and verify:
```bash
ls /dev/video*
v4l2-ctl --list-devices
```

### Audio Setup

ALSA should be installed by default. Verify:
```bash
which arecord aplay
arecord -l  # List recording devices
aplay -l    # List playback devices
```

### GPIO Permissions

The GPIO character device requires permissions:

```bash
# Add your user to the necessary groups
sudo usermod -a -G gpio,video,audio $USER

# Or run MicroPython with sudo
sudo ./micropython
```

## Usage Examples

### GPIO Example

```python
from machine import Pin
import time

# Create output pin (GPIO 23)
led = Pin(23, Pin.OUT)

# Blink LED
for i in range(10):
    led.on()
    time.sleep(0.5)
    led.off()
    time.sleep(0.5)

# Create input pin with pull-up (GPIO 24)
button = Pin(24, Pin.IN, pull=Pin.PULL_UP)
print(f"Button state: {button.value()}")
```

### I2C Example

```python
from machine import I2C

# Initialize I2C bus 1 at 100 kHz
i2c = I2C(id=1, freq=100000)

# Scan for devices
devices = i2c.scan()
print(f"I2C devices found: {[hex(addr) for addr in devices]}")

# Read from device at address 0x48
data = i2c.readfrom(0x48, 2)

# Write to device
i2c.writeto(0x48, bytes([0x01, 0x02]))

# Read from memory address
data = i2c.readfrom_mem(0x50, 0x00, 16)  # EEPROM example
```

### SPI Example

```python
from machine import SPI

# Initialize SPI bus 0 at 1 MHz
spi = SPI(id=0, baudrate=1000000, polarity=0, phase=0)

# Write data
spi.write(bytes([0x01, 0x02, 0x03]))

# Read data
data = spi.read(5)

# Write and read simultaneously
write_buf = bytes([0xAA, 0xBB, 0xCC])
read_buf = bytearray(3)
spi.write_readinto(write_buf, read_buf)
```

### PWM Example

```python
from machine import PWM

# Initialize PWM (chip 0, channel 0, 1kHz, 50% duty)
pwm = PWM(chip=0, channel=0, freq=1000, duty_u16=32768)

# Change frequency
pwm.freq(2000)  # 2 kHz

# Change duty cycle (0-65535)
pwm.duty_u16(16384)  # 25% duty cycle

# Clean up
pwm.deinit()
```

### Camera Example

```python
from machine import Camera

# Initialize camera (640x480, RGB format)
camera = Camera(device=0, width=640, height=480, format='RGB')

# Capture a single frame
frame = camera.capture()
print(f"Captured {len(frame)} bytes")

# Save to file
with open('capture.raw', 'wb') as f:
    f.write(frame)

# Convert to image: ffmpeg -f rawvideo -pixel_format rgb24 -video_size 640x480 -i capture.raw capture.png

# Clean up
camera.deinit()
```

### Display Example

```python
from machine import Display
import time

# Initialize display (framebuffer)
display = Display(device=0)
print(f"Display: {display.width}x{display.height}, {display.bpp}bpp")

# Fill screen with red
display.fill(0xFF0000)
time.sleep(1)

# Fill screen with green
display.fill(0x00FF00)
time.sleep(1)

# Fill screen with blue
display.fill(0x0000FF)
time.sleep(1)

# Draw individual pixels
for x in range(100):
    for y in range(100):
        display.pixel(x, y, 0xFFFFFF)  # White square

# Clean up
display.deinit()
```

### Audio Example

```python
from machine import Audio
import time

# Initialize audio (44.1kHz, stereo, 16-bit)
audio = Audio(rate=44100, channels=2, bits=16)

# Record 3 seconds of audio
print("Recording...")
audio_data = audio.record(duration=3000)
print(f"Recorded {len(audio_data)} bytes")

# Save to file
with open('recording.raw', 'wb') as f:
    f.write(audio_data)

# Play back the recorded audio
print("Playing...")
audio.play(audio_data)

# Clean up
audio.deinit()
```

### TensorFlow Lite Example

```python
import tflite

# Load a TensorFlow Lite model
interpreter = tflite.Interpreter('model.tflite')
print(f"Interpreter: {interpreter}")

# Allocate tensors
interpreter.allocate_tensors()

# Get input/output details
input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()

# Set input tensor
input_data = b'\x00\x01\x02\x03'
interpreter.set_tensor(0, input_data)

# Run inference
interpreter.invoke()

# Get output tensor
output_data = interpreter.get_tensor(0)
print(f"Output: {output_data}")

# Note: This is a demo API. For production, link with TensorFlow Lite C library
```

## Pin Mapping for Raspberry Pi 5

Raspberry Pi 5 uses BCM GPIO numbering:

| Physical Pin | GPIO # | Function         |
|--------------|--------|------------------|
| 3            | 2      | I2C1 SDA         |
| 5            | 3      | I2C1 SCL         |
| 19           | 10     | SPI0 MOSI        |
| 21           | 9      | SPI0 MISO        |
| 23           | 11     | SPI0 SCLK        |
| 24           | 8      | SPI0 CE0         |
| 26           | 7      | SPI0 CE1         |

See [pinout.xyz](https://pinout.xyz) for complete pinout.

## Testing

Test programs are included in `ports/unix/examples/`:

```bash
cd ports/unix

# Core Hardware Tests
sudo ./build-standard/micropython examples/pi5_gpio_test.py
sudo ./build-standard/micropython examples/pi5_i2c_test.py
sudo ./build-standard/micropython examples/pi5_spi_test.py
sudo ./build-standard/micropython examples/pi5_pwm_test.py

# Multimedia Tests
sudo ./build-standard/micropython examples/pi5_camera_test.py
sudo ./build-standard/micropython examples/pi5_display_test.py
./build-standard/micropython examples/pi5_audio_test.py

# AI Test
./build-standard/micropython examples/pi5_tflite_test.py

# Comprehensive Demo (Camera + Display + Audio)
sudo ./build-standard/micropython examples/pi5_multimedia_demo.py
```

## Troubleshooting

### "Permission denied" errors

Run with `sudo` or add your user to the appropriate groups:
```bash
sudo usermod -a -G gpio,i2c,spi $USER
```
Log out and back in for changes to take effect.

### GPIO chip not found

On Pi 5, the GPIO chip is `/dev/gpiochip4`. On earlier models, it may be `/dev/gpiochip0`. The code automatically tries both.

### I2C/SPI device not found

Make sure the interfaces are enabled in `/boot/firmware/config.txt` and reboot.

### PWM not working

PWM requires proper device tree configuration. Check that `/sys/class/pwm/` exists and contains the appropriate PWM chips.

### Camera not detected

- Check `/dev/video*` devices exist
- Verify camera is enabled in `/boot/firmware/config.txt`
- Test with: `v4l2-ctl --list-devices`
- For Pi Camera Module, use `libcamera-hello` to test first

### Display/Framebuffer issues

- Verify `/dev/fb0` exists
- Ensure you're not in headless mode
- Check display is connected
- Try: `cat /sys/class/graphics/fb0/virtual_size`

### Audio not recording/playing

- Check ALSA is installed: `which arecord aplay`
- Test microphone: `arecord -d 1 test.wav && aplay test.wav`
- Verify audio devices: `arecord -l` and `aplay -l`
- Add user to audio group: `sudo usermod -a -G audio $USER`

## Performance Notes

- GPIO operations go through the Linux kernel, so they're slower than bare-metal MicroPython
- Typical GPIO toggle rate: ~10-50 kHz (vs. MHz on microcontrollers)
- Perfect for sensor reading, LED control, and moderate-speed protocols
- For high-speed applications, consider using dedicated hardware or the RP1 chip directly

## Advanced: Web Server Support

The unix port includes full networking support, so you can create web servers:

```python
import socket

# Create socket
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(('', 8080))
s.listen(5)

print("Web server running on port 8080")
while True:
    conn, addr = s.accept()
    request = conn.recv(1024)
    conn.send(b'HTTP/1.1 200 OK\r\n\r\nHello from MicroPython!')
    conn.close()
```

## Advanced: AI Support

MicroPython now includes a `tflite` module for AI inference on Pi 5.

### Current Implementation

The included `tflite` module provides the TensorFlow Lite API structure:
- `Interpreter` class for loading models
- Methods: `allocate_tensors()`, `invoke()`, `get/set_tensor()`
- Compatible with TensorFlow Lite model format

### For Production AI Inference

To use real AI models:

1. **Install TensorFlow Lite Runtime:**
   ```bash
   pip3 install tflite-runtime
   ```

2. **Link MicroPython with TFLite C Library:**

   Edit `ports/unix/Makefile` and add:
   ```makefile
   LDFLAGS += -ltensorflowlite
   ```

3. **Download Pre-trained Models:**
   - TensorFlow Hub: https://tfhub.dev/
   - Model formats: .tflite files
   - Examples: MobileNet, EfficientNet, PoseNet

4. **Use Cases:**
   - Image classification (Camera + AI)
   - Object detection
   - Pose estimation
   - Voice recognition (Audio + AI)
   - Real-time video analysis (Camera + Display + AI)

### Example AI Application

```python
from machine import Camera
import tflite

# Load image classification model
interpreter = tflite.Interpreter('mobilenet_v2.tflite')
interpreter.allocate_tensors()

# Initialize camera
camera = Camera(device=0, width=224, height=224)

# Capture and classify image
frame = camera.capture()
interpreter.set_tensor(0, frame)
interpreter.invoke()
predictions = interpreter.get_tensor(0)

print(f"Classification: {predictions}")
```

## Contributing

To contribute improvements:
1. Make your changes
2. Test thoroughly on Pi 5
3. Submit pull request to the upstream MicroPython repository

## License

MicroPython is licensed under the MIT License.

## Support

- MicroPython Forum: https://forum.micropython.org/
- MicroPython Discord: https://discord.gg/RB8HZSAExQ
- GitHub Issues: https://github.com/micropython/micropython/issues

---

**Note**: This implementation provides hardware control through Linux kernel interfaces. For bare-metal performance, consider using MicroPython on RP2040/RP2350 microcontrollers (Raspberry Pi Pico).
