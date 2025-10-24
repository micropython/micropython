# STM32H747I-DISCO Board

STMicroelectronics STM32H747I-DISCO development board with STM32H747XI dual-core MCU.

## Features

- STM32H747XIH6 microcontroller
  - Arm Cortex-M7 @ 480 MHz + Cortex-M4 @ 240 MHz
  - 2 MB Flash, 1 MB RAM
  - 32 MB external SDRAM
- Connectivity
  - USB High-Speed via ULPI PHY (USB3320)
  - USB Full-Speed
  - Ethernet RMII (LAN8742A PHY) - **requires hardware modification**
  - microSD card slot (8-bit SDMMC1)
- Display
  - 4" capacitive touchscreen LCD (480×272)
- Audio
  - WM8994 codec with line in/out, headphone, speaker, microphone
  - 2x digital MEMS microphones
- Other peripherals
  - 2× user LEDs
  - 1× user button + reset button
  - Arduino Uno V3 expansion connector
  - PMOD connector
  - Onboard ST-LINK/V3

## Pin Configuration

Default pin assignments:

- UART1: PA9 (TX), PA10 (RX) - Connected to ST-LINK VCP
- USB_HS: ULPI interface on PA3, PA5, PB0, PB1, PB5, PB10-13, PC0, PH4, PI11
- I2C4: PD12 (SCL), PD13 (SDA) - WM8994 audio codec
- SPI5: PF7 (SCK), PF8 (MISO), PF9 (MOSI), PF6 (CS)
- UART8: PJ8 (TX), PJ9 (RX)
- SD Card: 8-bit SDMMC1 interface on PC6-12, PB8-9, PD2

## Ethernet Hardware Limitation

**Important:** The Ethernet interface is not functional with the default board configuration due to a pin conflict between ETH_MDC (PC1) and SAI4_D1 (digital MEMS microphone). A hardware modification is required to disconnect the MEMS microphone from PC1 to enable Ethernet functionality. Refer to the STM32H747I-DISCO user manual (UM2411) for details on the required modification.

## Building Firmware

```bash
cd ports/stm32
make BOARD=STM32H747I_DISCO
```

## Flashing Firmware

Using STM32CubeProgrammer (recommended):
```bash
STM32_Programmer_CLI -c port=SWD mode=UR -w build-STM32H747I_DISCO/firmware.elf -v -rst
```

Using st-flash:
```bash
st-flash write build-STM32H747I_DISCO/firmware.bin 0x08000000
```

## Console Access

The board provides two independent console options:

1. **ST-LINK V3 UART** (recommended):
   - Device: /dev/ttyACM* (Linux), COM* (Windows)
   - Baudrate: 115200
   - Always available regardless of USB state

2. **USB CDC serial**:
   - Requires USB cable connected
   - Provides MicroPython REPL

Using mpremote:
```bash
mpremote connect /dev/ttyACM5  # Adjust device as needed
```

## Testing

The board includes a test suite for verifying hardware functionality:

```python
# Run automated tests
import run_all_tests
run_all_tests.run_automated()

# Run manual tests (requires user interaction)
run_all_tests.run_manual()
```

Test coverage:
- I2C (WM8994 codec)
- ADC (3 channels)
- SPI
- UART
- SD card
- Ethernet (requires hardware modification)
- DAC
- PWM/Timers
- LEDs and buttons
