The Xteink X4 is a pocket eInk eReader built around the ESP32-C3, with a
4.26" 800x480 monochrome eInk display (SSD1677 controller), a microSD card
slot, and 16MiB of flash.

The eInk display and microSD card share a single SPI bus. Six navigation
buttons (up, down, left, right, confirm, back) are read via a resistor
ladder across two ADC pins, while the power button is wired to its own
GPIO. See `pins.csv` for the full pin mapping.

USB serial/JTAG is provided natively over the USB-C port.
