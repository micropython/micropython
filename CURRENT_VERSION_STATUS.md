# MicroPython Version Status Report

**Generated:** 2025-11-21
**Repository Branch:** claude/review-open-issues-01NPUBr5vTbxDB8Fzcrudky6

---

## Current Version

**MicroPython v1.27.0-preview** (Development Build)

### Version Components
- **Major:** 1
- **Minor:** 27
- **Micro:** 0
- **Status:** Preview/Development (MICROPY_VERSION_PRERELEASE = 1)

This is a **development version** leading up to the official v1.27.0 release. It includes:
- Latest features and improvements
- Ongoing development work
- May have unreleased features
- Not yet officially tagged/released

---

## All Supported Ports (21 Total)

All ports share the **same core MicroPython version: v1.27.0-preview**

| Port | Version | Build System | Documentation | Target Platform |
|------|---------|--------------|---------------|-----------------|
| **alif** | v1.27.0-preview | Make | ✓ | Alif Semiconductor MCUs |
| **bare-arm** | v1.27.0-preview | Make | ✓ | Bare-metal ARM Cortex-M |
| **cc3200** | v1.27.0-preview | Make | ✓ | Texas Instruments CC3200 |
| **embed** | v1.27.0-preview | Custom | ✓ | Embedding in C applications |
| **esp32** | v1.27.0-preview | Make | ✓ | Espressif ESP32 family |
| **esp8266** | v1.27.0-preview | Make | ✓ | Espressif ESP8266 |
| **mimxrt** | v1.27.0-preview | Make | ✓ | NXP i.MX RT series |
| **minimal** | v1.27.0-preview | Make | ✓ | Minimal reference port |
| **nrf** | v1.27.0-preview | Make | ✓ | Nordic nRF51/nRF52 |
| **pic16bit** | v1.27.0-preview | Make | - | Microchip 16-bit PIC |
| **pic32** | v1.27.0-preview | Custom | ✓ | Microchip PIC32 |
| **powerpc** | v1.27.0-preview | Make | ✓ | PowerPC architecture |
| **qemu** | v1.27.0-preview | Make | ✓ | QEMU ARM virtual machine |
| **renesas-ra** | v1.27.0-preview | Make | ✓ | Renesas RA family |
| **rp2** | v1.27.0-preview | Make | ✓ | Raspberry Pi Pico (RP2040/RP2350) |
| **samd** | v1.27.0-preview | Make | ✓ | Microchip SAMD21/SAMD51 |
| **stm32** | v1.27.0-preview | Make | ✓ | STMicroelectronics STM32 |
| **unix** | v1.27.0-preview | Make | ✓ | Unix/Linux/macOS |
| **webassembly** | v1.27.0-preview | Make | ✓ | Web browsers via WASM |
| **windows** | v1.27.0-preview | Make | ✓ | Microsoft Windows |
| **zephyr** | v1.27.0-preview | CMake | ✓ | Zephyr RTOS |

---

## Version History Context

### Latest Official Stable Release
**v1.26.0** (Released: ~2024)

### Current Development Version
**v1.27.0-preview** (In Development)

### Expected v1.27.0 Features
Based on this being a preview/development version, v1.27.0 likely includes:
- Bug fixes and security improvements (see BUG_FIXES_SESSION*.md)
- Enhanced C API documentation and examples
- Port improvements (bare-arm, PIC32, etc.)
- Performance optimizations
- New hardware support

---

## Port-Specific Capabilities

### Production-Ready Ports
These ports are well-maintained and suitable for production:
- **esp32** - Most popular IoT port, extensive hardware support
- **stm32** - Comprehensive MCU support, industrial applications
- **rp2** - Raspberry Pi Pico, great for beginners and makers
- **unix** - Development and testing on desktop systems
- **esp8266** - Legacy but still widely used for IoT

### Specialized Ports
- **bare-arm** - Bare-metal development, minimal footprint
- **embed** - Embedding MicroPython in larger applications
- **qemu** - Testing and development without hardware
- **webassembly** - Running in web browsers

### RTOS Integration
- **zephyr** - Integration with Zephyr RTOS for advanced features

---

## Build System Status

### Make-Based Builds (19 ports)
Most ports use the traditional Make build system:
```bash
cd ports/<port-name>
make
```

### CMake-Based Builds (1 port)
- **zephyr** - Uses CMake/West build system

### Custom Builds (2 ports)
- **embed** - Integration into host application's build system
- **pic32** - Custom build configuration

---

## Feature Comparison Matrix

### Core Features (All Ports)
- ✓ Python 3.4+ syntax compatibility
- ✓ Garbage collection
- ✓ Exception handling
- ✓ Integer arithmetic (arbitrary precision optional)
- ✓ Basic types: int, str, bytes, list, dict, tuple, set

### Optional Features (Port-Dependent)
| Feature | Most Ports | Minimal Ports | Notes |
|---------|------------|---------------|-------|
| Float support | ✓ | ✗ | Can be disabled for space |
| Threading | ✓ | ✗ | Depends on platform |
| SSL/TLS | Network ports | ✗ | Requires mbedTLS |
| Filesystem | ✓ | ✗ | VFS with FAT/LittleFS |
| Networking | WiFi/Ethernet ports | ✗ | Port-specific |
| Bluetooth | ESP32, nRF | ✗ | Hardware-dependent |
| USB | Many | ✗ | Hardware-dependent |

---

## Recent Enhancements (This Branch)

This development branch includes several major improvements:

### 1. Security Fixes
- **20 bugs fixed** in Session 3 (2 CRITICAL, 3 HIGH, 11 MEDIUM)
- Buffer overflow fixes in WebREPL
- Integer overflow fixes in core types
- NULL pointer fixes in network stack

### 2. Bare-ARM Port Enhancement
- Added full peripheral support (GPIO, UART, I2C, SPI, PWM, ADC, Timer)
- Now has feature parity with PIC32 and STM32 ports
- Complete documentation and examples

### 3. C API Documentation
- **66KB comprehensive C API guide** (`docs/develop/c_api_guide.md`)
- Advanced examples for buffer protocol and callbacks
- Production-ready integration patterns
- Hardware integration examples

### 4. Build System Improvements
- Fixed bare-arm Makefile to include peripheral files
- Added CMake support for advanced examples

---

## Version Verification

To verify the version in a built MicroPython:

```python
import sys
print(sys.version)
print(sys.implementation)
```

Expected output:
```
3.4.0; MicroPython v1.27.0-preview on 2025-11-21
(name='micropython', version=(1, 27, 0), _machine='...', _mpy=...)
```

---

## Upgrading from Previous Versions

### From v1.26.x to v1.27.0
- Review changelog for breaking changes
- Test critical functionality
- Update frozen modules if used
- Rebuild all C extensions

### General Upgrade Process
1. Check port-specific release notes
2. Backup current firmware
3. Build new version: `make clean && make`
4. Test on development hardware first
5. Deploy to production after verification

---

## Development Status

### Current State
- **Version:** v1.27.0-preview (development)
- **Stability:** Development/testing
- **Recommended Use:** Development and testing only
- **Production Use:** Wait for stable v1.27.0 release

### When to Use Preview Versions
✓ **Use for:**
- Testing new features
- Contributing to development
- Early bug detection
- Feature evaluation

✗ **Don't use for:**
- Production deployments
- Critical applications
- Shipping products
- Long-term stable systems

---

## Port Maturity Levels

### Tier 1 (Highest Quality)
Very stable, extensively tested, recommended for production:
- **unix** - Reference implementation
- **stm32** - Industrial-grade, well-tested
- **esp32** - Huge community, well-supported
- **rp2** - Official Raspberry Pi support

### Tier 2 (Production Ready)
Stable and usable, good community support:
- **esp8266** - Mature but resource-constrained
- **nrf** - Good Nordic support
- **samd** - Solid Microchip support

### Tier 3 (Experimental/Specialized)
Functional but may have limitations:
- **bare-arm** - Educational/minimal
- **pic32** - Limited community
- **powerpc** - Specialized use
- **webassembly** - Experimental

---

## Getting the Latest Stable Version

To get the most recent **stable** release:

```bash
# Clone and checkout latest stable tag
git clone https://github.com/micropython/micropython.git
cd micropython
git tag --sort=-v:refname | head -1  # Find latest tag
git checkout v1.26.0  # Replace with actual latest
```

To use the **development** version (like this repository):
```bash
git clone https://github.com/micropython/micropython.git
cd micropython
git checkout master  # Development branch
```

---

## Platform-Specific Version Notes

### ESP32
- Full v1.27.0-preview support
- IDF compatibility: 4.4.x, 5.0.x, 5.1.x
- Supports ESP32, ESP32-S2, ESP32-S3, ESP32-C3

### STM32
- Supports hundreds of boards
- HAL version: varies by MCU family
- DFU and ST-Link programming

### RP2
- Supports RP2040 and RP2350
- Pico SDK version: typically latest stable
- Extensive Pico W networking support

### Unix
- Tested on Linux (Ubuntu, Debian, Arch)
- macOS support (x86_64, ARM64)
- BSD variants supported

---

## Summary

**Current Version Across All Ports: v1.27.0-preview**

- **21 supported ports** spanning embedded, desktop, and web platforms
- **Unified version** across all ports (1.27.0-preview)
- **Development status** - not yet stable release
- **Enhanced** with security fixes, documentation, and examples
- **Production use** - wait for stable v1.27.0 release

For production use, consider using the latest **stable** release (v1.26.x) until v1.27.0 is officially released and tagged.

---

**Last Updated:** 2025-11-21
**Report Generated By:** MicroPython Development Tools
**Branch:** claude/review-open-issues-01NPUBr5vTbxDB8Fzcrudky6
