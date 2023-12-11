MicroPython port to Alif Ensemble MCUs
======================================

This is a port of MicroPython to the Alif Ensemble series of microcontrollers.

Initial development of this Alif port was sponsored by OpenMV LLC.

Features currently supported:
- UART REPL.
- TinyUSB with CDC and MSC device support.
- Octal SPI flash with XIP mode.
- machine.Pin support with named pins.
- machine.UART, machine.SPI, machine.I2C, machine.RTC peripherals.
- WiFi and Bluetooth using cyw43.
- Dual core support of the HE and HP cores using Open-AMP.
- Low power modes.

The following more advanced features will follow later:
- Ethernet support.
- SDRAM support.
- Other machine modules.
