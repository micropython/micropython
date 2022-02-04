### Deployment over STM32 UART bootloader

If a debug adapter is not available, the STM32 UART bootloader can be used to
flash the downloaded hex file.

The WHEELTEC board has a programming USB connector (USB1) located at one of
the board's corners.

USB1 connects to a CP2102 USB<->UART IC. Keeping RTS low, a high DTR pulse
causes a reset of the STM32 into the bootloader.

This sequence can be used by a compatible programmer that speaks the STM32
UART bootloader protocol to flash MicroPython onto the device.
