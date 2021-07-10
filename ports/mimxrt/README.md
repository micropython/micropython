Port of MicroPython to NXP iMX RT 10xx
======================================

Currently supports Teensy 4.0 and the i.MX RT1010 EVK board.

Features:
  - REPL over USB VCP
  - machine.Pin

Known issues:
  - pyboard.py doesn't work with files larger than 64 bytes
  - machine.Pin class currently does not support GPIOMUX option of
    i.MX RT101x variants

TODO:
  - Enable TCM
  - Peripherals (LED, Timers, etc)
