# MicroPython Architecture Support Guide

## Overview

MicroPython supports a wide range of CPU architectures, from 8-bit microcontrollers to 64-bit processors. This document provides a comprehensive overview of all supported architectures.

## Supported CPU Architectures

### 1. ARM Cortex-M Series (Most Popular)

#### ARM Cortex-M0/M0+
**32-bit ARM, 24-48 MHz, Low Power**

| Port | MCU | Cores | Clock | RAM | Flash | Status |
|------|-----|-------|-------|-----|-------|--------|
| **samd** | SAMD21 | M0+ | 48 MHz | 32 KB | 256 KB | ✅ Production |
| **samd** | SAMD51 | M4F | 120 MHz | 256 KB | 1 MB | ✅ Production |
| **rp2** | RP2040 | Dual M0+ | 133 MHz | 264 KB | 2-16 MB | ✅ Production |

**Boards:**
- Arduino Zero, MKR series
- Adafruit Feather M0/M4
- Raspberry Pi Pico, Pico W
- Seeed XIAO RP2040

#### ARM Cortex-M3
**32-bit ARM, Medium Performance**

| Port | MCU | Clock | RAM | Flash | Status |
|------|-----|-------|-----|-------|--------|
| **stm32** | STM32F1 | 72 MHz | 64 KB | 512 KB | ✅ Production |
| **stm32** | STM32L1 | 32 MHz | 32 KB | 256 KB | ✅ Production |

**Boards:**
- STM32F103 (Blue Pill)
- Pyboard Lite

#### ARM Cortex-M4/M4F
**32-bit ARM with FPU, High Performance**

| Port | MCU | Clock | RAM | Flash | Status |
|------|-----|-------|-----|-------|--------|
| **stm32** | STM32F4 | 168 MHz | 192 KB | 1 MB | ✅ Production |
| **stm32** | STM32L4 | 80 MHz | 128 KB | 1 MB | ✅ Production |
| **stm32** | STM32G4 | 170 MHz | 128 KB | 512 KB | ✅ Production |
| **stm32** | STM32WB | 64 MHz | 256 KB | 1 MB | ✅ Production |
| **nrf** | nRF52832 | 64 MHz | 64 KB | 512 KB | ✅ Production |
| **nrf** | nRF52840 | 64 MHz | 256 KB | 1 MB | ✅ Production |
| **samd** | SAMD51 | 120 MHz | 256 KB | 1 MB | ✅ Production |
| **mimxrt** | i.MX RT10xx | 600 MHz | 512 KB | 8 MB | ✅ Production |
| **bare-arm** | STM32F4 | 168 MHz | 192 KB | 1 MB | ✅ Minimal |
| **minimal** | STM32F405 | 168 MHz | 192 KB | 1 MB | ✅ Minimal |

**Boards:**
- Pyboard D
- STM32F407 Discovery
- Nordic nRF52840 DK
- Arduino Nano 33 BLE
- Adafruit Feather nRF52
- Teensy 4.0/4.1

#### ARM Cortex-M7
**32-bit ARM with Cache, Very High Performance**

| Port | MCU | Clock | RAM | Flash | Status |
|------|-----|-------|-----|-------|--------|
| **stm32** | STM32F7 | 216 MHz | 512 KB | 2 MB | ✅ Production |
| **stm32** | STM32H7 | 480 MHz | 1 MB | 2 MB | ✅ Production |
| **mimxrt** | i.MX RT106x | 600 MHz | 1 MB | 4 MB | ✅ Production |

**Boards:**
- Pyboard D (STM32F767)
- Arduino Portenta H7
- Arduino Giga R1 WiFi
- Teensy 4.0/4.1 (i.MX RT1062)

#### ARM Cortex-M33
**32-bit ARM with TrustZone**

| Port | MCU | Clock | RAM | Flash | Status |
|------|-----|-------|-----|-------|--------|
| **renesas-ra** | RA4M1 | 48 MHz | 32 KB | 256 KB | ✅ Production |
| **renesas-ra** | RA6M1 | 120 MHz | 256 KB | 512 KB | ✅ Production |
| **renesas-ra** | RA6M2 | 120 MHz | 256 KB | 512 KB | ✅ Production |
| **alif** | Alif E1 | 160 MHz | 512 KB | 4 MB | ✅ Production |
| **alif** | Alif E7 | 400 MHz | 4 MB | External | ✅ Production |

**Boards:**
- Renesas RA4M1/RA6M1 boards
- Alif Ensemble E1/E7

### 2. Xtensa (Espressif)

#### Xtensa LX6
**32-bit Xtensa, Dual Core**

| Port | MCU | Cores | Clock | RAM | Flash | WiFi | BT | Status |
|------|-----|-------|-------|-----|-------|------|-------|--------|
| **esp32** | ESP32 | 2 | 240 MHz | 520 KB | 4 MB | ✅ | ✅ | ✅ Production |

**Boards:**
- ESP32-DevKitC
- ESP32-WROOM
- M5Stack
- TTGO T-Display

#### Xtensa LX7
**32-bit Xtensa, Single/Dual Core**

| Port | MCU | Cores | Clock | RAM | Flash | WiFi | BT | Status |
|------|-----|-------|-------|-----|-------|------|-------|--------|
| **esp32** | ESP32-S2 | 1 | 240 MHz | 320 KB | 4 MB | ✅ | ❌ | ✅ Production |
| **esp32** | ESP32-S3 | 2 | 240 MHz | 512 KB | 8 MB | ✅ | ✅ | ✅ Production |

**Boards:**
- ESP32-S2-Saola
- ESP32-S3-DevKitC
- Arduino Nano ESP32

### 3. RISC-V (Open Source Architecture)

#### RISC-V RV32 (32-bit)

| Port | MCU | Clock | RAM | Flash | WiFi | BT | Status |
|------|-----|-------|-----|-------|------|-------|--------|
| **esp32** | ESP32-C2 | 120 MHz | 272 KB | 2 MB | ✅ | ✅ | ✅ Production |
| **esp32** | ESP32-C3 | 160 MHz | 400 KB | 4 MB | ✅ | ✅ | ✅ Production |
| **esp32** | ESP32-C5 | 240 MHz | 512 KB | 4 MB | ✅ | ✅ | ✅ Beta |
| **esp32** | ESP32-C6 | 160 MHz | 512 KB | 4 MB | ✅ | ✅ | ✅ Production |
| **qemu** | RISC-V | Variable | Variable | Variable | ❌ | ❌ | ✅ Emulation |

**Boards:**
- ESP32-C3-DevKitM
- ESP32-C6-DevKitC
- M5Stack NanoC6
- QEMU virt machine (RV32)

#### RISC-V RV64 (64-bit)

| Port | MCU | Clock | RAM | Flash | Status |
|------|-----|-------|-----|-------|--------|
| **qemu** | RISC-V 64 | Variable | Variable | Variable | ✅ Emulation |

**Boards:**
- QEMU virt machine (RV64)

**Features:**
- Full 64-bit RISC-V ISA support
- Virtual machine for testing
- Ideal for RISC-V development and testing

### 4. MIPS

#### MIPS32
**32-bit MIPS Architecture**

| Port | MCU | Clock | RAM | Flash | Status |
|------|-----|-------|-----|-------|--------|
| **pic16bit** | dsPIC33 | 40-70 MHz | 16-256 KB | 256 KB-2 MB | ✅ Enhanced |
| **pic32** | PIC32MX | 80 MHz | 128 KB | 512 KB | ✅ New Port |

**Boards:**
- PIC32MX795F512L
- dsPIC33 Starter Kit
- Custom PIC32 boards

**Features:**
- Full machine module support
- GPIO, UART, I2C, SPI, PWM, ADC
- Timer support

### 5. PowerPC

#### PowerPC 64-bit
**64-bit PowerPC Architecture**

| Port | MCU | Clock | Status |
|------|-----|-------|--------|
| **powerpc** | POWER8/9 | Variable | ⚠️ Basic |

**Boards:**
- IBM POWER systems
- Bare metal PowerPC

**Status:** Basic support, needs HAL enhancement

### 6. x86/x64 (POSIX Platforms)

#### x86-64
**Standard Desktop/Server Platforms**

| Port | Platform | Status |
|------|----------|--------|
| **unix** | Linux | ✅ Production |
| **unix** | macOS | ✅ Production |
| **unix** | FreeBSD | ✅ Production |
| **windows** | Windows | ✅ Production |

**Features:**
- Full Python compatibility layer
- File system support
- Networking
- Testing and development platform

### 7. WebAssembly

#### WASM
**Browser and WASI Platforms**

| Port | Platform | Status |
|------|----------|--------|
| **webassembly** | Browser | ✅ Production |
| **webassembly** | Node.js | ✅ Production |
| **webassembly** | WASI | ✅ Production |

**Features:**
- Run MicroPython in web browsers
- JavaScript interoperability
- WASM/WASI support

## Architecture Comparison

### Performance

| Architecture | Typical Clock | Performance | Power | Cost |
|--------------|---------------|-------------|-------|------|
| ARM Cortex-M0+ | 48 MHz | Low | Very Low | Very Low |
| ARM Cortex-M3 | 72 MHz | Medium | Low | Low |
| ARM Cortex-M4F | 168 MHz | High | Medium | Medium |
| ARM Cortex-M7 | 480 MHz | Very High | Medium-High | High |
| Xtensa LX6/7 | 240 MHz | High | Medium | Low |
| RISC-V RV32 | 160 MHz | Medium-High | Low | Very Low |
| MIPS32 | 80 MHz | Medium | Low | Low |
| PowerPC | Variable | Very High | High | Very High |
| x86-64 | 2+ GHz | Extreme | Very High | High |

### Feature Matrix

| Architecture | FPU | DSP | WiFi | BT | Price |
|--------------|-----|-----|------|-----|-------|
| ARM M0+ | ❌ | ❌ | Optional | Optional | $ |
| ARM M4F | ✅ | ✅ | Optional | Optional | $$ |
| ARM M7 | ✅ | ✅ | Optional | Optional | $$$ |
| Xtensa | ✅ | ❌ | ✅ | ✅ | $ |
| RISC-V | ✅ | ✅ | ✅ | ✅ | $ |
| MIPS32 | Optional | ✅ | ❌ | ❌ | $$ |

## ARM Port Range Summary

MicroPython supports the complete ARM Cortex-M range:

### ARM Cortex-M0/M0+
- ✅ SAMD21 (Cortex-M0+, 48 MHz)
- ✅ RP2040 (Dual Cortex-M0+, 133 MHz)

### ARM Cortex-M3
- ✅ STM32F1 series
- ✅ STM32L1 series

### ARM Cortex-M4/M4F
- ✅ STM32F4 series (F401, F405, F407, F411, F427, F429, F446, F469)
- ✅ STM32L4 series
- ✅ STM32G4 series
- ✅ STM32WB series (with Bluetooth)
- ✅ nRF52832, nRF52840 (Nordic, with BLE)
- ✅ SAMD51
- ✅ i.MX RT10xx (up to 600 MHz!)

### ARM Cortex-M7
- ✅ STM32F7 series (F722, F746, F767)
- ✅ STM32H7 series (H743, H747, H750)
- ✅ i.MX RT106x

### ARM Cortex-M33
- ✅ Renesas RA4M1, RA6M1, RA6M2
- ✅ Alif Ensemble E1, E7

### ARM Cortex-A (Application Processors)
- ⚠️ Can run via Unix port on ARM Linux

## RISC-V Port Range Summary

MicroPython has excellent RISC-V support:

### RISC-V RV32 (32-bit)
- ✅ **ESP32-C2** - WiFi + BLE, 120 MHz, 272 KB RAM
- ✅ **ESP32-C3** - WiFi + BLE, 160 MHz, 400 KB RAM (Most Popular)
- ✅ **ESP32-C5** - WiFi 6 + BLE, 240 MHz, 512 KB RAM
- ✅ **ESP32-C6** - WiFi 6 + BLE, 160 MHz, 512 KB RAM
- ✅ **QEMU RV32** - Virtual machine for testing

### RISC-V RV64 (64-bit)
- ✅ **QEMU RV64** - Virtual machine for testing
- ⚠️ Real hardware support coming (SiFive, StarFive boards)

### RISC-V Features

**Advantages:**
- Open source ISA (no licensing fees)
- Modern, clean architecture
- Growing ecosystem
- Excellent GCC/LLVM support
- Very power efficient

**ESP32-C3 (Most Popular RISC-V):**
```python
# Same API as ESP32 (Xtensa)
from machine import Pin, UART, I2C, SPI, PWM, ADC
import network

# WiFi
wlan = network.WLAN(network.STA_IF)
wlan.active(True)
wlan.connect('SSID', 'password')

# Bluetooth
import bluetooth
ble = bluetooth.BLE()
ble.active(True)
```

## Building for Different Architectures

### ARM Cortex-M (STM32)

```bash
cd ports/stm32
make BOARD=PYBV11
```

### RISC-V (ESP32-C3)

```bash
cd ports/esp32
make BOARD=ESP32_GENERIC_C3
```

### RISC-V (QEMU)

```bash
cd ports/qemu
make BOARD=VIRT_RV32
make run  # Run in QEMU
```

### MIPS (PIC32)

```bash
cd ports/pic32
make
```

### x86-64 (Unix)

```bash
cd ports/unix
make
```

## Architecture-Specific Features

### ARM Cortex-M
- **NVIC** - Nested Vectored Interrupt Controller
- **MPU** - Memory Protection Unit (M3+)
- **FPU** - Floating Point Unit (M4F, M7)
- **DSP** - Digital Signal Processing (M4+)
- **Cache** - Instruction/Data Cache (M7)
- **TrustZone** - Security (M33+)

### Xtensa
- **Dual Core** - SMP support
- **WiFi** - Integrated 802.11 b/g/n
- **Bluetooth** - BLE and Classic
- **Low Power** - Multiple sleep modes

### RISC-V
- **Open Source** - No licensing
- **Modular ISA** - Extensions (M, A, F, D, C)
- **Clean Design** - Simple instruction set
- **Growing Ecosystem** - Many vendors
- **Low Cost** - Competitive pricing

### MIPS
- **DSP Extensions** - dsPIC33
- **Low Power** - Efficient design
- **Mature** - Long-established
- **Industrial** - Wide adoption

## Choosing an Architecture

### For IoT/WiFi Projects
**Recommendation:** RISC-V (ESP32-C3) or Xtensa (ESP32)
- Low cost ($2-5)
- WiFi + Bluetooth built-in
- Large community
- Excellent MicroPython support

### For Battery-Powered Projects
**Recommendation:** ARM Cortex-M0+ (SAMD21, RP2040)
- Ultra low power
- Efficient sleep modes
- Long battery life

### For High Performance
**Recommendation:** ARM Cortex-M7 (STM32H7, i.MX RT)
- Up to 600 MHz
- Floating point unit
- Hardware acceleration
- Large RAM/Flash

### For Learning
**Recommendation:** RP2040 or ESP32
- Very low cost
- Large community
- Lots of documentation
- Easy to use

### For Industrial
**Recommendation:** STM32F4/F7 or PIC32
- Proven reliability
- Wide temperature range
- Long-term availability
- Safety certifications

## Future Architectures

### Planned/In Development
- ⏳ **RISC-V Real Hardware** - SiFive, StarFive boards
- ⏳ **AVR** - Potential Arduino Uno/Mega support
- ⏳ **ESP32-P4** - Dual RISC-V with AI accelerators

### Community Requests
- ⏳ **ARM Cortex-A** - Native support (RPi Linux boards)
- ⏳ **ESP8266** - Enhanced support

## Resources

### Documentation
- [ARM Architecture](https://developer.arm.com/architectures)
- [RISC-V Specification](https://riscv.org/technical/specifications/)
- [Xtensa ISA](https://www.cadence.com/en_US/home/tools/ip/tensilica-ip.html)
- [MIPS Architecture](https://www.mips.com/)

### Development Tools
- **ARM:** Keil MDK, ARM GCC, OpenOCD
- **RISC-V:** RISC-V GCC, OpenOCD, QEMU
- **Xtensa:** ESP-IDF, Xtensa GCC
- **MIPS:** MPLAB XC32, PIC32 GCC

## Conclusion

MicroPython supports an exceptionally wide range of CPU architectures:

- **ARM:** Complete Cortex-M range (M0+ to M33)
- **RISC-V:** RV32 and RV64 support ✅
- **Xtensa:** ESP32 family
- **MIPS:** PIC32 and dsPIC33
- **x86-64:** Unix/Windows platforms
- **WASM:** Browser and WASI

The addition of RISC-V support makes MicroPython future-proof and positions it well for the growing open hardware ecosystem.
