.. _rp2_general:

General information about the RP2xxx port
=========================================

The rp2 port supports boards powered by the Raspberry Pi Foundation's RP2xxx
family of microcontrollers. That includes RP2040 and RP2350

Technical specifications and SoC datasheets
-------------------------------------------
RP2040
^^^^^^

The RP2040 microcontroller is manufactured on a 40 nm silicon process in a 7x7mm
QFN-56 SMD package. The key features include:

* 133 MHz dual ARM Cortex-M0+ cores (overclockable to over 400 MHz)
* 264KB SRAM in six independent banks
* No internal Flash or EEPROM memory (after reset, the bootloader loads
  firmware from either the external flash memory or USB bus into internal SRAM)
* QSPI bus controller, which
  supports up to 16 MB of external Flash memory
* On-chip programmable LDO to generate core voltage
* 2 on-chip PLLs to generate USB and core clocks
* 30 GPIO pins, of which 4 can optionally be used as analog inputs

The peripherals include:

* 2 UARTs
* 2 SPI controllers
* 2 I2C controllers
* 16 PWM channels
* USB 1.1 controller
* 8 PIO state machines

For detailed technical specifications, please refer to the `rp2040-datasheet
<https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf>`_

RP2350
^^^^^^

The RP2350 microcontroller is manufactured on a 40 nm silicon process and is available in
QFN-60 (7x7 mm) or QFN-80 (10x10 mm) packages. There are two main variants: RP2350A
(QFN-60) and RP2350B (QFN-80). Both variants support selectable dual-core architectures:
either dual ARM Cortex-M33 cores or dual Hazard3 RISC-V cores (or a mix of one of each),
chosen via the ARCHSEL register or OTP settings. The RP2354A/B variants are identical but
include 2 MB internal flash.

The key features include:

* Up to 150 MHz dual cores (ARM Cortex-M33 in Arm mode with TrustZone, Hazard3 RISC-V)
* 520KB SRAM in ten independent banks (8x64KB striped + 2x4KB non-striped), plus 16KB XIP
  cache and 4KB USB DPSRAM
* No internal Flash or EEPROM memory in A variant (after reset, the bootloader loads firmware from
  either external flash memory or USB bus into internal SRAM); 32KB mask ROM (bootrom) and
  8KB OTP (ECC-protected) available
* QMI (QSPI interface) for external flash or PSRAM, supporting Execute-in-Place (XIP) with
  address translation, streaming DMA, and up to 32 MB address space
* On-chip switched-mode buck converter regulator (VREG_VIN 2.7–5.5V to DVDD 0.55–3.3V, up
  to 200 mA) with LDO mode for low-power states (1 mA retention)
* 2 on-chip PLLs (SYS and USB) to generate system, USB, ADC, and other clocks; additional
  clock sources include XOSC (1–50 MHz), ROSC (4.6–96 MHz, randomizable), and LPOSC
  (32.768 kHz)
* GPIO pins: 30 in RP2350A (QFN-60) with 4 analog inputs (GPIO26–29); 48 in RP2350B
  (QFN-80) with 8 analog inputs (GPIO40–47); 5V-tolerant (digital pins only; ADC pins are
  not 5V-tolerant), multi-function support (e.g., UART, I2C, SPI, PWM, USB, PIO, HSTX),
  with slew rate control, pull-up/down, and drive strength (2–12 mA)

The peripherals include:

* 2 UARTs (PL011-compatible with FIFO, DMA, flow control, and interrupts)
* 2 SPI controllers (PL022 SSP-compatible, master/slave, FIFO, DMA, supporting
  Motorola/Microwire/TI formats)
* 2 I2C controllers (DW-compatible, master/slave, up to Fast Mode Plus 1 Mb/s, DMA)
* 12 PWM slices (24 channels, 16-bit counters, fractional dividers, phase correction, DMA,
  input measurement modes)
* USB 2.0 Full-Speed/Low-Speed controller (host/device, integrated PHY, 32 endpoints,
  DPSRAM)
* 12 PIO state machines (3 blocks with 4 machines each, 32-instruction memory per block,
  TX/RX FIFOs, autopull/push, clock dividers, DMA)
* 12-bit SAR ADC (500 kS/s, 4–8 channels shared with GPIO depending on variant,
  temperature sensor, FIFO, DMA)
* 16 DMA channels (dual AHB5 ports, 8/16/32-bit transfers, chaining, pacing, MPU, security
  levels: secure/privileged, secure/unprivileged, non-secure/privileged,
  non-secure/unprivileged)
* HSTX high-speed transmitter (up to 300 Mb/s DDR per pin, TMDS encoding, coupled with
  PIO, on GPIO12–19)

For detailed technical specifications, please refer to the `rp2350-datasheet
<https://datasheets.raspberrypi.com/rp2350/rp2350-datasheet.pdf>`_