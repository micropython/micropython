Program your board using the esptool.py program, found [here](https://github.com/espressif/esptool).

Your TinyPICO has an auto-reset circuit on it, so there is no need to put it into a
download mode first to erase or flash it.

If you are putting MicroPython on your board for the first time then you should
first erase the entire flash using:

### Linux
```bash
esptool.py --chip esp32 --port /dev/ttyUSB0 erase_flash
```

### Mac
```bash
esptool.py --chip esp32 --port /dev/tty.SLAB_USBtoUART erase_flash
```

### Windows
Change (X) to whatever COM port is being used by the board
```bash
esptool --chip esp32 --port COM(X) erase_flash
```

Now download the version of the firmware you would like to install from the options below,
then use the following command to program the firmware starting at address 0x1000,
remembering to replace `tinypico-micropython-firmware-version.bin` with the name of the
firmware you just downloaded:

From then on program the firmware starting at address 0x1000:

### Linux
```bash
esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 912600 write_flash -z 0x1000 tinypico-micropython-firmware-version.bin
```

### Mac
```bash
esptool.py --chip esp32 --port /dev/tty.SLAB_USBtoUART --baud 912600 write_flash -z 0x1000 tinypico-micropython-firmware-version.bin
```

### Windows
Change (X) to whatever COM port is being used by the board
```bash
esptool --chip esp32 --port COM(X) --baud 912600 write_flash -z 0x1000 tinypico-micropython-firmware-version.bin
```
