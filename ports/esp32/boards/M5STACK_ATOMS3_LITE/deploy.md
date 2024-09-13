Program your board using the `esptool.py` program, found
[here](https://github.com/espressif/esptool).

To place the board in _bootloader mode_ - so `esptool`` can be used - press and
hold the reset button for two seconds. A green LED will flash behind the reset
button when the bootloader mode has been activated.

If you are putting MicroPython on your board for the first time then you should
first erase the entire flash using:

```bash
esptool.py --chip esp32s3 --port /dev/ttyACM0 erase_flash
```

From then on program the firmware starting at address 0:

```bash
esptool.py --chip esp32s3 --port /dev/ttyACM0 write_flash -z 0 board-20240105-v1.22.1.bin
```

After the firmware has been deployed, press the reset button to reset the device
and start the new firmware.
