# MicroPython Arduino Board Support and Compatibility Guide

## Overview

MicroPython supports many Arduino boards through existing ports. While MicroPython uses a different API than Arduino, it provides more powerful Python features while maintaining access to all hardware peripherals.

## Supported Arduino Boards

### 1. Arduino Due (SAM3X8E - ARM Cortex-M3)
**Port:** `samd` (extended for SAM3X)
**Status:** ✅ Fully Supported

```bash
cd ports/samd
make BOARD=ARDUINO_DUE
```

**Features:**
- 84 MHz ARM Cortex-M3
- 96 KB SRAM, 512 KB Flash
- GPIO, UART, SPI, I2C, PWM, ADC
- USB device and host

**Pin Mapping:**
```python
# Arduino Pin -> MicroPython Pin
# D0-D53 -> Pin(0) to Pin(53)
# A0-A11 -> Pin(54) to Pin(65)
```

### 2. Arduino Zero / M0 / MKR Series (SAMD21 - ARM Cortex-M0+)
**Port:** `samd`
**Status:** ✅ Fully Supported

**Boards:**
- Arduino Zero
- Arduino M0 Pro
- Arduino MKR WiFi 1010
- Arduino MKR Zero
- Arduino MKR1000

```bash
cd ports/samd
make BOARD=ARDUINO_ZERO
```

**Features:**
- 48 MHz ARM Cortex-M0+
- 32 KB SRAM, 256 KB Flash
- All peripherals supported
- USB device mode

### 3. Arduino Nano 33 BLE (nRF52840 - ARM Cortex-M4)
**Port:** `nrf`
**Status:** ✅ Fully Supported

```bash
cd ports/nrf
make BOARD=arduino_nano_33_ble
```

**Features:**
- 64 MHz ARM Cortex-M4 with FPU
- 256 KB SRAM, 1 MB Flash
- Bluetooth 5.0 / BLE
- 9-axis IMU, microphone, temperature sensor

### 4. Arduino Portenta H7 (STM32H747 - Dual Core ARM Cortex-M7/M4)
**Port:** `stm32`
**Status:** ✅ Fully Supported

```bash
cd ports/stm32
make BOARD=ARDUINO_PORTENTA_H7
```

**Features:**
- 480 MHz M7 + 240 MHz M4
- 1 MB SRAM, 2 MB Flash
- WiFi, Bluetooth, Ethernet
- USB, CAN, SDIO, Camera interface
- Crypto hardware acceleration

### 5. Arduino Nano RP2040 Connect (RP2040 - Dual Core ARM Cortex-M0+)
**Port:** `rp2`
**Status:** ✅ Fully Supported

```bash
cd ports/rp2
make BOARD=ARDUINO_NANO_RP2040_CONNECT
```

**Features:**
- Dual 133 MHz ARM Cortex-M0+
- 264 KB SRAM, 16 MB Flash
- WiFi, Bluetooth
- 6-axis IMU, microphone

### 6. Arduino Nano ESP32 (ESP32-S3)
**Port:** `esp32`
**Status:** ✅ Fully Supported

```bash
cd ports/esp32
make BOARD=ARDUINO_NANO_ESP32
```

**Features:**
- Dual-core Xtensa 240 MHz
- 512 KB SRAM, 16 MB Flash
- WiFi, Bluetooth 5
- All ESP32 features

### 7. Arduino Giga R1 WiFi (STM32H747)
**Port:** `stm32`
**Status:** ✅ Supported

**Features:**
- Same as Portenta H7
- Arduino Mega form factor
- 76 GPIO pins

## Arduino API Compatibility Layer

MicroPython doesn't use Arduino's C++ API, but you can create a compatibility layer:

### arduino.py - Compatibility Module

```python
"""
Arduino-style API for MicroPython
Compatible with all MicroPython ports
"""

from machine import Pin, PWM, ADC, time_pulse_us
import time

# Pin modes
INPUT = Pin.IN
OUTPUT = Pin.OUT
INPUT_PULLUP = Pin.PULL_UP

# Pin states
LOW = 0
HIGH = 1

# PWM constants
_pwm_freq = 1000  # Default 1kHz

def pinMode(pin, mode):
    """Arduino pinMode()"""
    if mode == INPUT_PULLUP:
        return Pin(pin, Pin.IN, Pin.PULL_UP)
    else:
        return Pin(pin, mode)

def digitalWrite(pin, value):
    """Arduino digitalWrite()"""
    if isinstance(pin, Pin):
        pin.value(value)
    else:
        Pin(pin, Pin.OUT).value(value)

def digitalRead(pin):
    """Arduino digitalRead()"""
    if isinstance(pin, Pin):
        return pin.value()
    else:
        return Pin(pin, Pin.IN).value()

def analogWrite(pin, value):
    """Arduino analogWrite() - PWM output (0-255)"""
    pwm = PWM(Pin(pin))
    pwm.freq(_pwm_freq)
    pwm.duty_u16(value * 257)  # Convert 0-255 to 0-65535

def analogRead(pin):
    """Arduino analogRead() - Returns 0-1023"""
    adc = ADC(Pin(pin))
    return adc.read_u16() >> 6  # Convert 16-bit to 10-bit

def delay(ms):
    """Arduino delay()"""
    time.sleep_ms(ms)

def delayMicroseconds(us):
    """Arduino delayMicroseconds()"""
    time.sleep_us(us)

def millis():
    """Arduino millis()"""
    return time.ticks_ms()

def micros():
    """Arduino micros()"""
    return time.ticks_us()

def pulseIn(pin, state, timeout=1000000):
    """Arduino pulseIn()"""
    if not isinstance(pin, Pin):
        pin = Pin(pin, Pin.IN)
    return time_pulse_us(pin, state, timeout)

def map_value(x, in_min, in_max, out_min, out_max):
    """Arduino map()"""
    return (x - in_min) * (out_max - out_min) // (in_max - in_min) + out_min

def constrain(x, a, b):
    """Arduino constrain()"""
    return max(a, min(x, b))

# Serial class
class Serial:
    def __init__(self, uart_id, baudrate=9600):
        from machine import UART
        self.uart = UART(uart_id, baudrate=baudrate)

    def begin(self, baudrate):
        from machine import UART
        self.uart = UART(self.uart.uart_id, baudrate=baudrate)

    def available(self):
        return self.uart.any()

    def read(self):
        data = self.uart.read(1)
        return data[0] if data else -1

    def write(self, data):
        if isinstance(data, int):
            self.uart.write(bytes([data]))
        else:
            self.uart.write(data)

    def print(self, *args, **kwargs):
        import io
        buf = io.StringIO()
        print(*args, file=buf, **kwargs)
        self.uart.write(buf.getvalue().encode())

    def println(self, *args, **kwargs):
        import io
        buf = io.StringIO()
        print(*args, file=buf, **kwargs)
        self.uart.write(buf.getvalue().encode() + b'\n')

# Create Serial instances
Serial1 = Serial(1)
Serial2 = Serial(2)
```

## Arduino Sketch Examples Converted to MicroPython

### Example 1: Blink (Classic Arduino)

**Arduino C++:**
```cpp
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}
```

**MicroPython:**
```python
from machine import Pin
import time

led = Pin(25, Pin.OUT)  # Pin 25 on many boards

while True:
    led.on()
    time.sleep(1)
    led.off()
    time.sleep(1)
```

**MicroPython with Arduino compatibility:**
```python
from arduino import *

led = pinMode(25, OUTPUT)

while True:
    digitalWrite(led, HIGH)
    delay(1000)
    digitalWrite(led, LOW)
    delay(1000)
```

### Example 2: AnalogRead

**Arduino C++:**
```cpp
void setup() {
  Serial.begin(9600);
}

void loop() {
  int sensorValue = analogRead(A0);
  Serial.println(sensorValue);
  delay(100);
}
```

**MicroPython:**
```python
from machine import ADC, UART
import time

adc = ADC(Pin(0))
uart = UART(1, baudrate=9600)

while True:
    value = adc.read_u16() >> 6  # Convert to 10-bit
    uart.write(f"{value}\n".encode())
    time.sleep_ms(100)
```

**MicroPython with Arduino compatibility:**
```python
from arduino import *

Serial1.begin(9600)

while True:
    sensorValue = analogRead(0)  # A0
    Serial1.println(sensorValue)
    delay(100)
```

### Example 3: PWM (Fading LED)

**Arduino C++:**
```cpp
int ledPin = 9;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  for (int i = 0; i <= 255; i++) {
    analogWrite(ledPin, i);
    delay(10);
  }
  for (int i = 255; i >= 0; i--) {
    analogWrite(ledPin, i);
    delay(10);
  }
}
```

**MicroPython:**
```python
from machine import Pin, PWM
import time

pwm = PWM(Pin(9))
pwm.freq(1000)

while True:
    for duty in range(0, 65536, 256):
        pwm.duty_u16(duty)
        time.sleep_ms(10)
    for duty in range(65535, -1, -256):
        pwm.duty_u16(duty)
        time.sleep_ms(10)
```

**MicroPython with Arduino compatibility:**
```python
from arduino import *

ledPin = 9

while True:
    for i in range(0, 256):
        analogWrite(ledPin, i)
        delay(10)
    for i in range(255, -1, -1):
        analogWrite(ledPin, i)
        delay(10)
```

### Example 4: I2C Communication

**Arduino C++:**
```cpp
#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(9600);
}

void loop() {
  Wire.beginTransmission(0x68);
  Wire.write(0x75);  // WHO_AM_I register
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 1);

  if (Wire.available()) {
    byte data = Wire.read();
    Serial.println(data, HEX);
  }
  delay(1000);
}
```

**MicroPython:**
```python
from machine import I2C, Pin, UART
import time

i2c = I2C(0, scl=Pin(22), sda=Pin(21), freq=100000)
uart = UART(1, baudrate=9600)

while True:
    data = i2c.readfrom_mem(0x68, 0x75, 1)
    uart.write(f"{data[0]:02X}\n".encode())
    time.sleep(1)
```

### Example 5: SPI Communication

**Arduino C++:**
```cpp
#include <SPI.h>

const int CS = 10;

void setup() {
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);
  SPI.begin();
}

void loop() {
  digitalWrite(CS, LOW);
  SPI.transfer(0x01);
  byte result = SPI.transfer(0x00);
  digitalWrite(CS, HIGH);
  delay(100);
}
```

**MicroPython:**
```python
from machine import Pin, SPI
import time

cs = Pin(10, Pin.OUT, value=1)
spi = SPI(1, baudrate=1000000)

while True:
    cs.value(0)
    spi.write(b'\x01')
    result = spi.read(1)
    cs.value(1)
    time.sleep_ms(100)
```

## Pin Mapping Reference

### Arduino Uno / Nano (AVR-based)
❌ **Not Supported** - MicroPython doesn't support AVR architecture
**Alternative:** Use Arduino Nano RP2040 Connect or Nano 33 BLE

### Arduino Mega 2560 (AVR-based)
❌ **Not Supported** - MicroPython doesn't support AVR architecture
**Alternative:** Use Arduino Giga R1 WiFi (same form factor)

### Arduino Due Pin Mapping

| Arduino | MicroPython | Function |
|---------|-------------|----------|
| D0-D53 | Pin(0-53) | Digital I/O |
| A0-A11 | Pin(54-65) | Analog Input |
| TX1 | Pin(18) | UART1 TX |
| RX1 | Pin(19) | UART1 RX |
| SDA | Pin(20) | I2C Data |
| SCL | Pin(21) | I2C Clock |
| MISO | Pin(74) | SPI MISO |
| MOSI | Pin(75) | SPI MOSI |
| SCK | Pin(76) | SPI Clock |

### Arduino Nano 33 BLE Pin Mapping

| Arduino | MicroPython | Function |
|---------|-------------|----------|
| D0-D13 | Pin(0-13) | Digital I/O |
| A0-A7 | Pin(14-21) | Analog Input |
| LED_BUILTIN | Pin(13) | Built-in LED |
| TX | Pin(1) | UART TX |
| RX | Pin(0) | UART RX |

## Installing MicroPython on Arduino Boards

### Method 1: Using DFU (Due, Zero, Portenta)

```bash
# Put board in bootloader mode (double-tap reset button)

# Flash firmware
dfu-util -a 0 -d 0x2341:0x0000 -D firmware.bin
```

### Method 2: Using UF2 (RP2040-based boards)

```bash
# Put board in bootloader mode (hold BOOTSEL, press reset)

# Drag firmware.uf2 to the RPI-RP2 drive
cp firmware.uf2 /media/RPI-RP2/
```

### Method 3: Using esptool (ESP32-based boards)

```bash
esptool.py --chip esp32s3 --port /dev/ttyUSB0 write_flash -z 0x1000 firmware.bin
```

### Method 4: Using OpenOCD (STM32-based boards)

```bash
openocd -f interface/stlink.cfg -f target/stm32h7x.cfg \
  -c "program firmware.elf verify reset exit"
```

## Feature Comparison: Arduino vs MicroPython

| Feature | Arduino (C++) | MicroPython |
|---------|---------------|-------------|
| Language | C/C++ | Python |
| Compilation | Ahead-of-time | Interactive REPL |
| Memory Usage | Lower | Higher |
| Development Speed | Slower | Faster |
| Libraries | 5000+ | 1000+ |
| Debugging | Serial print | REPL, exceptions |
| OTA Updates | Manual | Built-in (ESP32) |
| File System | Limited | Full VFS |
| Threading | Difficult | Easy (asyncio) |
| Learning Curve | Steeper | Gentler |

## Advantages of MicroPython over Arduino

1. **Interactive REPL** - Test code instantly without compiling
2. **Exception Handling** - Better error messages and debugging
3. **Dynamic Typing** - Faster prototyping
4. **Built-in Modules** - JSON, HTTP, WebSockets, etc.
5. **File System** - Store configuration, logs, data files
6. **asyncio** - Cooperative multitasking made easy
7. **Python Ecosystem** - Use pure Python libraries
8. **Network Features** - WiFi, Bluetooth, HTTP server built-in

## When to Use Arduino vs MicroPython

### Use Arduino When:
- Maximum performance is critical
- Minimal memory usage required
- Using Arduino-specific libraries
- Project requires AVR microcontrollers
- Real-time constraints

### Use MicroPython When:
- Rapid prototyping
- Network/IoT applications
- Complex data processing
- Learning embedded systems
- Dynamic configuration needed
- Interactive debugging required

## Resources

### Official Documentation
- [MicroPython for Arduino Boards](https://docs.micropython.org/en/latest/arduino/)
- [Arduino Compatibility Layer](https://github.com/micropython/micropython-lib)

### Community
- [MicroPython Forum - Arduino Section](https://forum.micropython.org/)
- [Arduino MicroPython Examples](https://github.com/micropython/micropython/tree/master/examples)

### Tools
- [mpremote](https://docs.micropython.org/en/latest/reference/mpremote.html) - MicroPython remote control
- [Thonny IDE](https://thonny.org/) - Python IDE with MicroPython support
- [rshell](https://github.com/dhylands/rshell) - Remote MicroPython shell

## Creating Custom Arduino Board Support

If you have an Arduino-compatible board not listed here, you can create support by:

1. Identify the MCU (STM32, SAMD21, nRF52, etc.)
2. Use the appropriate port
3. Create a board definition file
4. Define pin mappings
5. Build custom firmware

Example board definition (ports/stm32/boards/MY_ARDUINO_BOARD/mpconfigboard.h):

```c
#define MICROPY_HW_BOARD_NAME       "My Arduino Board"
#define MICROPY_HW_MCU_NAME         "STM32F405"

// Define pins
#define MICROPY_HW_LED1             pin_A5
#define MICROPY_HW_UART1_TX         pin_A9
#define MICROPY_HW_UART1_RX         pin_A10
```

## Conclusion

MicroPython provides excellent support for modern Arduino boards while offering significant advantages in development speed, debugging, and functionality. For new projects, especially IoT applications, MicroPython is often the better choice.
