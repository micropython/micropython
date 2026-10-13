The OpenMV Cam M7 boots through the OpenMV bootloader, which is installed at the factory.
MicroPython is installed alongside it and the bootloader stays in place, so the camera
can be switched back to the OpenMV firmware at any time.

### Installing MicroPython

The bootloader is a USB DFU device (`37c5:9203`), and MicroPython goes in its
FIRMWARE partition (alt setting 2).  Start dfu-util, then connect the camera (the
bootloader waits briefly for a DFU host after reset), or run `machine.bootloader()`:

    dfu-util -w -d ,37c5:9203 -a 2 --reset -D firmware.bin

OpenMV IDE can do the same with Tools > Load Custom Firmware.

### Installing the OpenMV bootloader

A camera that has only run old OpenMV firmware may have an older bootloader that is
not a USB DFU device.  To install the current one:

1. Download the firmware zip from the latest
   [OpenMV release](https://github.com/openmv/openmv/releases/latest) and take
   `OPENMV3/bootloader.bin` from it.
2. Disconnect the camera, connect a jumper wire between its BOOT and RST pins, and
   reconnect it.  It now enumerates as the ST system DFU device (`0483:df11`).
3. Write the bootloader:

       dfu-util -w -d ,0483:df11 -a 0 -s 0x08000000 -D bootloader.bin

4. Remove the jumper and reconnect the camera.
