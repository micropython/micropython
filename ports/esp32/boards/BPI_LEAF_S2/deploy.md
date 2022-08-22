The board needs to be put into bootloader mode before upgrading.

To do it: Hold down **BOOT** and then press **RST** once, finally release **BOOT**.

Program your board using the esptool.py program, found [here](https://github.com/espressif/esptool).

If you are putting MicroPython on your board for the first time then you should
first erase the entire flash using:

```bash
python -m esptool --chip esp32s2 --port /dev/ttyACM0 erase_flash
```

From then on program the firmware starting at address `0x1000`:

```bash
python -m esptool --chip esp32s2 --port /dev/ttyACM0 write_flash -z 0x1000 firmware.bin
```

`/dev/ttyACM0` is the port format of Linux, in MacOS it might be `/dev/cu.usbmodem01`, in Windows it might be `COM1`, need to confirm the port number of the board.

`firmware.bin` should be the filename of the firmware you want to use.

When the program is finished, press RST.
