The SAMD21 Xplained Pro board is shipped without a bootloader. For use
with MicroPyhton a suitable bootloader has be be installed first. The
following procedure has been found to work and be simple:

1. Get an existing bootloader. Adafruit provides a large set of suitable
bootloaders at https://github.com/adafruit/uf2-samdx1/releases. The
samd21 bootloader have  size of 8k. The file bootloader-generic-v3.16.0.bin
for instance is a suitable bootloader.
2. If the bootloader is a binary file, convert it to Intel Hex format
starting at address 0. For example with 
`objcopy -v -I binary -O ihex binary_file intel_hex_file`.
3. Connect your board to the debug port. A drive with the name XPLAINED
shall appear.
4. Copy the Intel hex file of the bootloader to that drive.
5. Connect your board to the target port. A drive with the name SAMD21 should
appear. If not, push reset twice. Then it should appear. If that does not
happen, the bootloader was not properly installed or is not compatible.
6. Copy the MicroPython firmware, a.uf2 file, to the SAMD21 drive. When the SAMD21
drive disappears, MicroPython is installed.

From now on only steps 5 and 6 are needed to update MicroPython. You can use the
usual methods to invoke the bootloader, namely:

- Push Reset twice.
- Call machine.bootloader().
- Use the touch 1200 procedure by switching the USB baud rate to 1200 baud and back.

At the above link above there are as well .uf2 versions of the bootloader
which one can install using steps 5. and 6. above once a .uf2 capable
bootloader is installed.