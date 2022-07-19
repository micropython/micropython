### Update the bootloader

Before deploying any firmware, make sure you have the updated Arduino Nano 33 BLE bootloader, which relocates the bootloader so the softdevice doesn't overwrite it. Please see:

https://docs.arduino.cc/tutorials/nano-33-ble/getting-started-omv

### Via Arduino bootloader and BOSSA

Download BOSSA from https://github.com/shumatech/BOSSA/ and double tap reset button to enter the Arduino bootloader

```bash
bossac -e -w --offset=0x16000 --port=ttyACM0 -i -d -U -R build-arduino_nano_33_ble_sense-s140/firmware.bin
```

Alternatively, a Linux binary can be found here: https://github.com/openmv/openmv/blob/master/tools/bossac

### Via nrfprog

This board can also be programmed via nrfjprog (with Jlink for example), from MicroPython source repository:

```bash
make -j8 BOARD=arduino_nano_33_ble_sense SD=s140 deploy
```
