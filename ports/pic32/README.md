# MicroPython for PIC32

This directory contains the MicroPython port to Microchip PIC32 microcontrollers, specifically targeting the PIC32MX series (MIPS-based 32-bit MCUs).

## Supported Boards

- PIC32MX795F512L (primary target)
- PIC32MX470F512L
- PIC32MX270F256B
- Other PIC32MX variants (may require configuration)

## Features

### Complete Machine Module Support

The PIC32 port provides full hardware abstraction through the `machine` module:

- **GPIO (Pin)** - Digital I/O control
- **UART** - Serial communication (6 channels)
- **I2C** - Inter-Integrated Circuit communication
- **SPI** - Serial Peripheral Interface
- **PWM** - Pulse Width Modulation (via Output Compare)
- **ADC** - Analog-to-Digital Converter (10-bit)
- **Timer** - Hardware timers (5 channels)

### System Features

- **Reset control** - Software and watchdog reset
- **Frequency management** - CPU and peripheral clock control
- **Unique ID** - Device identification
- **Power management** - Idle and sleep modes
- **Interrupt control** - IRQ enable/disable

## Building

### Requirements

1. MPLAB XC32 Compiler (v2.50 or later)
2. MicroPython build tools

### Build Steps

```bash
cd ports/pic32
make
```

The output will be `build/firmware.hex` which can be flashed using:
- MPLAB IPE
- PICkit 3/4
- ICD 3/4
- Direct USB bootloader (if configured)

## Pin Numbering

Pins are numbered sequentially across all ports:

```
Port A: 0-15   (e.g., RA0 = pin 0, RA15 = pin 15)
Port B: 16-31  (e.g., RB0 = pin 16, RB15 = pin 31)
Port C: 32-47  (e.g., RC0 = pin 32, RC15 = pin 47)
Port D: 48-63  (e.g., RD0 = pin 48, RD15 = pin 63)
Port E: 64-79  (e.g., RE0 = pin 64, RE15 = pin 79)
Port F: 80-95  (e.g., RF0 = pin 80, RF15 = pin 95)
Port G: 96-111 (e.g., RG0 = pin 96, RG15 = pin 111)
```

## API Reference

### GPIO - machine.Pin

```python
from machine import Pin

# Create output pin (LED on RB0)
led = Pin(16, Pin.OUT)
led.on()
led.off()
led.value(1)  # Set high
led.value(0)  # Set low

# Create input pin (button on RA5)
button = Pin(5, Pin.IN)
if button.value():
    print("Button pressed")

# Pin modes
Pin.IN        # Input mode
Pin.OUT       # Output mode
Pin.OPEN_DRAIN  # Open-drain output
```

### UART - machine.UART

```python
from machine import UART

# Initialize UART1 at 115200 baud
uart = UART(1, baudrate=115200)

# Write data
uart.write(b'Hello, PIC32!\n')

# Read data
data = uart.read(10)  # Read up to 10 bytes

# Check if data available
if uart.any():
    data = uart.read()

# UART parameters
uart = UART(1,
    baudrate=115200,  # Baud rate: 300-4000000
    bits=8,           # Data bits: 8 or 9
    parity=None,      # Parity: None, 0 (even), 1 (odd)
    stop=1,           # Stop bits: 1 or 2
    timeout=1000      # Timeout in ms
)
```

### I2C - machine.I2C

```python
from machine import I2C

# Initialize I2C1 at 100kHz
i2c = I2C(1, freq=100000)

# Scan for devices
devices = i2c.scan()
print("I2C devices found:", [hex(d) for d in devices])

# Write to device at address 0x50
i2c.writeto(0x50, b'\x00\x01\x02')

# Read from device
data = i2c.readfrom(0x50, 4)

# Write then read (common pattern)
i2c.writeto_mem(0x50, 0x00, b'\x12\x34')
data = i2c.readfrom_mem(0x50, 0x00, 2)
```

### SPI - machine.SPI

```python
from machine import SPI, Pin

# Initialize SPI1 at 1MHz
spi = SPI(1, baudrate=1000000)

# Write data
spi.write(b'\x01\x02\x03\x04')

# Read data
data = spi.read(4)

# Write and read simultaneously
rx_data = spi.write_readinto(b'\xAA\xBB', bytearray(2))

# Using with chip select
cs = Pin(32, Pin.OUT, value=1)
cs.value(0)  # Assert CS
spi.write(b'\x01\x02')
cs.value(1)  # Deassert CS
```

### PWM - machine.PWM

```python
from machine import PWM, Pin

# Create PWM on pin RB5 (pin 21)
pwm = PWM(Pin(21))

# Set frequency to 1kHz
pwm.freq(1000)

# Set duty cycle (0-1023)
pwm.duty(512)  # 50% duty cycle

# Fade LED
for duty in range(0, 1024, 10):
    pwm.duty(duty)
    time.sleep_ms(10)

# Control servo (50Hz, 1-2ms pulse)
servo = PWM(Pin(22), freq=50)
servo.duty(77)   # 1.5ms center position
```

### ADC - machine.ADC

```python
from machine import ADC, Pin

# Create ADC on pin RA0 (analog input)
adc = ADC(Pin(0))

# Read 10-bit value (0-1023)
value = adc.read()
voltage = value * 3.3 / 1023
print(f"Voltage: {voltage:.2f}V")

# Read 16-bit value (0-65535)
value_16 = adc.read_u16()
```

### Timer - machine.Timer

```python
from machine import Timer

# Create periodic timer
def callback(timer):
    print("Timer fired!")

tim = Timer(1)
tim.init(mode=Timer.PERIODIC, period=1000, callback=callback)

# One-shot timer
tim2 = Timer(2)
tim2.init(mode=Timer.ONE_SHOT, period=5000, callback=lambda t: print("Done!"))

# Disable timer
tim.deinit()
```

## Complete Examples

### Example 1: Blink LED

```python
from machine import Pin
import time

led = Pin(16, Pin.OUT)  # RB0

while True:
    led.on()
    time.sleep_ms(500)
    led.off()
    time.sleep_ms(500)
```

### Example 2: UART Communication

```python
from machine import UART
import time

uart = UART(1, baudrate=115200)

while True:
    uart.write(b"Hello from PIC32!\n")

    if uart.any():
        data = uart.read()
        print("Received:", data)

    time.sleep(1)
```

### Example 3: I2C Temperature Sensor (MCP9808)

```python
from machine import I2C
import time

i2c = I2C(1, freq=100000)

MCP9808_ADDR = 0x18
TEMP_REG = 0x05

while True:
    data = i2c.readfrom_mem(MCP9808_ADDR, TEMP_REG, 2)
    temp_raw = (data[0] << 8) | data[1]
    temp_c = (temp_raw & 0x0FFF) / 16.0
    if temp_raw & 0x1000:
        temp_c -= 256.0

    print(f"Temperature: {temp_c:.2f}°C")
    time.sleep(1)
```

### Example 4: PWM Motor Control

```python
from machine import PWM, Pin
import time

# Motor control pins
motor_pwm = PWM(Pin(21), freq=1000)
motor_dir = Pin(22, Pin.OUT)

def set_motor(speed, direction):
    motor_dir.value(direction)
    motor_pwm.duty(abs(speed))

# Forward at 75% speed
set_motor(768, 1)
time.sleep(2)

# Reverse at 50% speed
set_motor(512, 0)
time.sleep(2)

# Stop
set_motor(0, 0)
```

### Example 5: ADC Data Logger

```python
from machine import ADC, Pin, UART
import time

adc = ADC(Pin(0))
uart = UART(1, baudrate=115200)

while True:
    value = adc.read()
    voltage = value * 3.3 / 1023
    uart.write(f"{time.ticks_ms()},{voltage:.3f}\n".encode())
    time.sleep_ms(100)
```

### Example 6: SPI SD Card Interface

```python
from machine import SPI, Pin

spi = SPI(1, baudrate=1000000)
cs = Pin(32, Pin.OUT, value=1)

def sd_command(cmd, arg=0):
    cs.value(0)
    spi.write(bytes([0x40 | cmd,
                     (arg >> 24) & 0xFF,
                     (arg >> 16) & 0xFF,
                     (arg >> 8) & 0xFF,
                     arg & 0xFF,
                     0x95]))  # CRC
    response = spi.read(1)
    cs.value(1)
    return response[0]

# Initialize SD card
response = sd_command(0)  # CMD0: GO_IDLE_STATE
print(f"SD Card response: 0x{response:02X}")
```

### Example 7: Multi-Threading with Timers

```python
from machine import Timer, Pin
import time

led1 = Pin(16, Pin.OUT)
led2 = Pin(17, Pin.OUT)
counter = 0

def blink_led1(t):
    led1.value(not led1.value())

def blink_led2(t):
    led2.value(not led2.value())

def count_up(t):
    global counter
    counter += 1
    print(f"Count: {counter}")

tim1 = Timer(1)
tim1.init(mode=Timer.PERIODIC, period=500, callback=blink_led1)

tim2 = Timer(2)
tim2.init(mode=Timer.PERIODIC, period=1000, callback=blink_led2)

tim3 = Timer(3)
tim3.init(mode=Timer.PERIODIC, period=1000, callback=count_up)

# Runs indefinitely
while True:
    time.sleep(1)
```

### Example 8: Complete IoT Sensor Node

```python
from machine import Pin, UART, ADC, I2C, Timer
import time

# Initialize peripherals
led = Pin(16, Pin.OUT)
temp_sensor = ADC(Pin(0))
i2c = I2C(1, freq=100000)
uart = UART(1, baudrate=115200)

# Data collection
def collect_data(timer):
    temp = temp_sensor.read() * 3.3 / 1023 * 100  # Convert to temp

    # Read I2C sensor (if available)
    try:
        humidity = i2c.readfrom(0x40, 2)
    except:
        humidity = b'\x00\x00'

    # Send data via UART
    data = f"TEMP:{temp:.1f},HUM:{humidity[0]}\n"
    uart.write(data.encode())

    # Blink LED to show activity
    led.value(1)
    time.sleep_ms(50)
    led.value(0)

# Setup timer for periodic data collection (every 5 seconds)
tim = Timer(1)
tim.init(mode=Timer.PERIODIC, period=5000, callback=collect_data)

print("IoT Sensor Node Started")

while True:
    time.sleep(1)
```

## Hardware Connections

### UART Pins (PIC32MX795F512L)

| UART | TX Pin | RX Pin |
|------|--------|--------|
| UART1 | RF3 (pin 83) | RF2 (pin 82) |
| UART2 | RF5 (pin 85) | RF4 (pin 84) |
| UART3 | RG8 (pin 104) | RG7 (pin 103) |

### I2C Pins

| I2C | SDA Pin | SCL Pin |
|-----|---------|---------|
| I2C1 | RG7 (pin 103) | RG8 (pin 104) |
| I2C2 | RA3 (pin 3) | RA2 (pin 2) |

### SPI Pins

| SPI | MISO | MOSI | SCK |
|-----|------|------|-----|
| SPI1 | RF7 | RF8 | RF6 |
| SPI2 | RG7 | RG8 | RG6 |

## System Functions

```python
import machine
import time

# Get CPU frequency
freq = machine.freq()
print(f"CPU: {freq} Hz")

# Get unique device ID
uid = machine.unique_id()
print("UID:", uid.hex())

# Software reset
machine.reset()

# Enter low power mode
machine.idle()

# Disable/enable interrupts
state = machine.disable_irq()
# Critical section
machine.enable_irq(state)
```

## Memory and Performance

- **RAM**: Up to 128 KB
- **Flash**: Up to 512 KB
- **CPU**: 80 MHz MIPS M4K core
- **Typical power**: 100 mA @ 80 MHz

## Troubleshooting

### Issue: UART not working

1. Check pin assignments match your board
2. Verify baud rate is supported
3. Ensure TX/RX pins are not used for other functions

### Issue: I2C devices not detected

1. Check pull-up resistors (4.7kΩ typical)
2. Verify device address
3. Check SDA/SCL connections

### Issue: PWM not outputting

1. Verify pin supports Output Compare
2. Check frequency and duty cycle values
3. Ensure pin is configured as output

## Contributing

Contributions to improve the PIC32 port are welcome! Please submit pull requests to the main MicroPython repository.

## License

MIT License - See main MicroPython repository for full license text.

## Resources

- [MicroPython Documentation](https://docs.micropython.org/)
- [PIC32MX Family Datasheet](https://www.microchip.com/pic32mx)
- [MPLAB XC32 Compiler](https://www.microchip.com/xc32)
- [MicroPython Forum](https://forum.micropython.org/)

## Support

For issues specific to the PIC32 port, please open an issue on the MicroPython GitHub repository with the tag `port:pic32`.
