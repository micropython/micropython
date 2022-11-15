Program your board using the latest version of the esptool.py program, found [here](https://github.com/espressif/esptool).

To flash or erase your FeatherS3, you have to first put it into download mode.
To do this, follow these steps:

- Press and hold the [BOOT] button
- Press and release the [RESET] button
- Release the [BOOT] button

Now the board is in download mode and the native USB will have enumerated as a serial device.

If you are putting MicroPython on your board for the first time then you should
first erase the entire flash using:

### Linux
```bash
esptool.py --chip esp32s3 --port /dev/ttyACM0 erase_flash
```

### Mac
Please do a `ls /dev/cu.usbm*` to determine the port your board has enumerated as.
```bash
esptool.py --chip esp32s3 --port /dev/cu.usbmodem01 erase_flash
```

### Windows
Change (X) to whatever COM port is being used by the board
```bash
esptool --chip esp32s3 --port COM(X) erase_flash
```

Now download the version of the firmware you would like to install from the options below,
then use the following command to program the firmware starting at address 0x0,
remembering to replace `feathers3-micropython-firmware-version.bin` with the name of
the firmware you just downloaded:

### Linux
```bash
esptool.py --chip esp32s3 --port /dev/ttyACM0 write_flash -z 0x0 feathers3-micropython-firmware-version.bin
```

### Mac
Please do a `ls /dev/cu.usbm*` to determine the port your board has enumerated as.
```bash
esptool.py --chip esp32s3 --port /dev/cu.usbmodem01 write_flash -z 0x0 feathers3-micropython-firmware-version.bin
```

### Windows
Change (X) to whatever COM port is being used by the board
```bash
esptool --chip esp32s3 --port COM(X) write_flash -z 0x0 feathers3-micropython-firmware-version.bin
```
