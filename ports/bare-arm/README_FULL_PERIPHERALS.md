# Bare-ARM Port - Full Peripheral Support

## Overview
The bare-arm port now includes complete machine module support with all major peripherals for STM32F4 microcontrollers, matching the functionality of the Pyboard port.

## Supported Peripherals

### 1. GPIO (Pin)
Digital I/O control with configurable modes and pull resistors.

```python
from machine import Pin

# Create output pin (PA5 = pin 5)
led = Pin(5, Pin.OUT)
led.on()
led.off()
led.value(1)

# Create input pin with pull-up
button = Pin(16, Pin.IN, Pin.PULL_UP)
state = button.value()
```

### 2. UART
Serial communication on USART1, USART2, USART3.

```python
from machine import UART

# Initialize UART2 at 115200 baud
uart = UART(2, 115200)

# Write data
uart.write(b'Hello World')

# Read data
data = uart.read(10)

# Check if data available
if uart.any():
    byte = uart.read(1)
```

### 3. I2C
Inter-Integrated Circuit communication for sensors and peripherals.

```python
from machine import I2C

# Initialize I2C1
i2c = I2C(1)

# Scan for devices
devices = i2c.scan()
print('Found devices:', [hex(d) for d in devices])

# Read/write operations
# i2c.writeto(addr, data)
# i2c.readfrom(addr, nbytes)
```

### 4. SPI
Serial Peripheral Interface for high-speed communication.

```python
from machine import SPI

# Initialize SPI1
spi = SPI(1)

# Write data
spi.write(b'\x01\x02\x03\x04')

# Read/write operations
# data = spi.read(10)
# spi.write_readinto(write_buf, read_buf)
```

### 5. PWM
Pulse Width Modulation using Timer 2.

```python
from machine import PWM

# Initialize PWM on channel 1, 1kHz, 50% duty
pwm = PWM(1, 1000, 512)

# Change duty cycle (0-1023)
pwm.duty(256)   # 25%
pwm.duty(768)   # 75%
```

### 6. ADC
12-bit Analog-to-Digital Converter.

```python
from machine import ADC

# Initialize ADC on channel 0
adc = ADC(0)

# Read value (0-4095)
value = adc.read()
voltage = (value / 4095) * 3.3
print('Voltage:', voltage)
```

### 7. Timer
Hardware timer support using Timer 3.

```python
from machine import Timer

# Initialize timer with 1000ms period
tim = Timer(1000)

# Read current value
count = tim.value()
```

## Hardware Configuration

### STM32F4 Specifications
- **CPU:** ARM Cortex-M4 @ 168MHz
- **Flash:** 512KB - 1MB
- **RAM:** 128KB - 192KB
- **Voltage:** 1.8V - 3.6V

### Pin Mapping
Pins are numbered as: `port * 16 + pin_number`

- **GPIOA:** Pins 0-15 (PA0 = 0, PA1 = 1, ..., PA15 = 15)
- **GPIOB:** Pins 16-31 (PB0 = 16, PB1 = 17, ..., PB15 = 31)
- **GPIOC:** Pins 32-47 (PC0 = 32, PC1 = 33, ..., PC15 = 47)
- **GPIOD:** Pins 48-63 (PD0 = 48, PD1 = 49, ..., PD15 = 63)

### UART Pins (Default)
- **USART1:** PA9 (TX), PA10 (RX)
- **USART2:** PA2 (TX), PA3 (RX)
- **USART3:** PB10 (TX), PB11 (RX)

### I2C Pins (Default)
- **I2C1:** PB6 (SCL), PB7 (SDA)
- **I2C2:** PB10 (SCL), PB11 (SDA)

### SPI Pins (Default)
- **SPI1:** PA5 (SCK), PA6 (MISO), PA7 (MOSI)
- **SPI2:** PB13 (SCK), PB14 (MISO), PB15 (MOSI)

### PWM Channels (Timer 2)
- **Channel 1:** PA0
- **Channel 2:** PA1
- **Channel 3:** PA2
- **Channel 4:** PA3

### ADC Channels
- **Channel 0:** PA0
- **Channel 1:** PA1
- **Channel 2:** PA2
- **Channel 3:** PA3
- (Channels 0-15 available on various pins)

## Building

```bash
cd ports/bare-arm
make
```

Output: `build/firmware.elf`

## Flashing

### Using OpenOCD
```bash
openocd -f interface/stlink-v2.cfg -f target/stm32f4x.cfg \
    -c "program build/firmware.elf verify reset exit"
```

### Using ST-Link
```bash
st-flash write build/firmware.bin 0x8000000
```

## Example: Complete IoT Sensor Node

```python
from machine import Pin, UART, I2C, ADC, Timer
import time

# Setup LED indicator
led = Pin(5, Pin.OUT)

# Setup UART for debugging
uart = UART(2, 115200)
uart.write(b'Sensor node starting...\n')

# Setup I2C for sensors
i2c = I2C(1)
devices = i2c.scan()
uart.write(b'Found I2C devices: ' + str(devices).encode() + b'\n')

# Setup ADC for analog sensor
temp_sensor = ADC(0)

# Main loop
while True:
    # Read temperature
    adc_value = temp_sensor.read()
    voltage = (adc_value / 4095) * 3.3
    temp = (voltage - 0.5) * 100  # TMP36 formula

    # Send data
    uart.write(f'Temperature: {temp:.2f}C\n'.encode())

    # Toggle LED
    led.value(not led.value())

    time.sleep(1)
```

## Comparison with Other Ports

| Feature | Bare-ARM | PIC32 | Pyboard | Minimal |
|---------|----------|-------|---------|---------|
| GPIO    | ✅       | ✅    | ✅      | ❌      |
| UART    | ✅       | ✅    | ✅      | ❌      |
| I2C     | ✅       | ✅    | ✅      | ❌      |
| SPI     | ✅       | ✅    | ✅      | ❌      |
| PWM     | ✅       | ✅    | ✅      | ❌      |
| ADC     | ✅       | ✅    | ✅      | ❌      |
| Timer   | ✅       | ✅    | ✅      | ❌      |

## Notes
- This is a bare-metal port with minimal OS overhead
- All peripherals use direct register access for maximum performance
- Clock speeds assume standard STM32F405 configuration
- For production use, consider adding interrupt support and DMA
- Pin configurations may need adjustment for your specific board

## Future Enhancements
- [ ] Interrupt-driven UART
- [ ] DMA support for high-speed transfers
- [ ] CAN bus support
- [ ] USB device support
- [ ] RTC (Real-Time Clock)
- [ ] Watchdog timer
