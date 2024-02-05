### Update the bootloader

Before deploying any firmware images, make sure the board is running the
updated Arduino Nano 33 BLE bootloader (which is relocated to the end of
the flash), otherwise the bootloader will be overwritten by the softdevice.
For more information on how to update the bootloader, see the
[Getting Started with OpenMV/MicroPython](https://docs.arduino.cc/tutorials/nano-33-ble/getting-started-omv) tutorial.

### Via Arduino bootloader and BOSSA

Download the latest BOSSA flash programmer from the
[Arduino BOSSA fork](https://github.com/arduino/BOSSA/releases/latest).
Note this version adds support for nRF devices, and must be used with the
Nano 33 BLE. Double tap the reset button to enter the bootloader, and run
the following command:

```bash
bossac -e -w --offset=0x16000 --port=ttyACM0 -i -d -U -R build-arduino_nano_33_ble_sense-s140/firmware.bin
```

### Via nrfprog

This board can also be programmed via nrfjprog (with Jlink for example), from MicroPython source repository:

```bash
make -j8 BOARD=arduino_nano_33_ble_sense SD=s140 deploy
```
