The SAMD21 Xplained Pro board is shipped without a bootloader. For use
with MicroPyhton a suitable bootloader has be be installed first. The
following procedure has been found to work and be simple:

1. Get the bootloader from https://micropython.org/resources/firmware/bootloader-xplained-pro-v3.16.0-15-gaa52b22.hex.
2. Connect your board to the debug port. A drive with the name XPLAINED
shall appear.
3. Copy the Intel hex file of the bootloader to that drive.
4. Connect your board to the target port. A drive with the name SAMD21XPL should
appear. If not, push reset twice. Then it should appear. If that does not
happen, the bootloader was not properly installed or is not compatible.
5. Copy the MicroPython firmware, a .uf2 file, to the SAMD21 drive. When the SAMD21
drive disappears, MicroPython is installed.

From now on only steps 4 and 5 are needed to update MicroPython. You can use the
usual methods to invoke the bootloader, namely:

- Push Reset twice.
- Call machine.bootloader().
- Use the touch 1200 procedure by switching the USB baud rate to 1200 baud and back.

At the above link above there are as well .uf2 versions of the bootloader
which one can install using steps 5. and 6. above once a .uf2 capable
bootloader is installed.
