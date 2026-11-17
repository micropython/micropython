The OpenMV Cam RT1062 boots through the OpenMV bootloader, a resident build of NXP's
MCU bootloader at the start of the flash, which is installed at the factory.
MicroPython goes in the firmware slot at 0x60040000 and the bootloader stays in place,
so the camera can be switched back to the OpenMV firmware at any time.

### Installing MicroPython

The simplest way is OpenMV IDE: Tools > Load Custom Firmware, with `firmware.bin`.

By hand, install NXP's [SPSDK](https://github.com/nxp-mcuxpresso/spsdk), which provides
`blhost`:

    pip install spsdk

`machine.bootloader()` (from MicroPython or the OpenMV firmware) starts the bootloader,
which enumerates as `15a2:0073`.  It returns to the firmware after about a second
without a host, so start the commands as soon as it appears:

    spsdk blhost -u 0x15A2,0x0073 -t 120000 -- flash-erase-region 0x60040000 0x3C0000
    spsdk blhost -u 0x15A2,0x0073 -- write-memory 0x60040000 firmware.bin
    spsdk blhost -u 0x15A2,0x0073 -- reset

The erase covers the whole firmware slot and takes about a minute.

### Filesystem

The filesystem uses the same flash area as the OpenMV firmware.  MicroPython formats it
as LittleFS on first boot if it can't mount it, which erases files left by the OpenMV
firmware.
