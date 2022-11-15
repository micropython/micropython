Port of MicroPython to NXP iMX RT 10xx
======================================

Currently supports Teensy 4.0, Teensy 4.1, and the
MIMXRT1010_EVK, MIMXRT1020_EVK, MIMXRT1050_EVK, MIMXRT1060_EVK and
MIMXRT1064_EVK boards.

Features:
  - REPL over USB VCP
  - machine.ADC
  - machine.I2C
  - machine.LED
  - machine.Pin
  - machine.PWM
  - machine.RTC
  - machine.SDCard
  - machine.SPI
  - machine.Signal
  - machine.SoftI2C
  - machine.SoftSPI
  - machine.Timer
  - machine.UART
  - LFS2 file system at the internal Flash
  - SDCard support (not on MIMXRT1010_EVK)
  - Ethernet (not on Teensy 4.0 and MIMXRT1010_EVK)

Known issues:

TODO:
  - More peripherals (Counter, I2S, CAN, etc)
  - More Python options
