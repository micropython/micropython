### Flashing via UF2 bootloader

To get the board in bootloader mode ready for the firmware update, execute
`machine.bootloader()` at the MicroPython REPL.  Alternatively, hold
down the BOOTSEL button while pressing reset (NRST).  The uf2 file below
should then be copied to the USB mass storage device that appears.  Once
programming of the new firmware is complete the device will automatically reset
and be ready for use.
