# MicroPython for Raspberry Pi 5 - Complete Guide

This guide explains how to compile, install, and use MicroPython with full hardware control on Raspberry Pi 5.

## Features

MicroPython on Pi 5 now supports:
- ✅ **GPIO** - Full Pin control via Linux GPIO character device
- ✅ **I2C** - I2C communication via Linux I2C devices
- ✅ **SPI** - SPI communication via Linux SPI devices
- ✅ **PWM** - PWM output via sysfs PWM interface

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

### GPIO Permissions

The GPIO character device requires permissions:

```bash
# Add your user to the gpio group
sudo usermod -a -G gpio $USER

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

# Test GPIO
sudo ./build-standard/micropython examples/pi5_gpio_test.py

# Test I2C
sudo ./build-standard/micropython examples/pi5_i2c_test.py

# Test SPI
sudo ./build-standard/micropython examples/pi5_spi_test.py

# Test PWM
sudo ./build-standard/micropython examples/pi5_pwm_test.py
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

## Advanced: AI Support (Future)

For AI support on Pi 5, you can:
1. Use Python's ctypes/FFI to interface with AI libraries
2. Utilize the unix port's full Python compatibility
3. Interface with TPU/neural accelerators via system calls

This will be expanded in future updates.

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
