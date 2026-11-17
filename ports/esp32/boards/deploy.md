Program your board using the esptool.py program, found [here](https://docs.espressif.com/projects/esptool/en/latest/{mcu}/).

*Windows users:* You may find the installed program is called `esptool` instead of `esptool.py`.

### Erasing

If you are putting MicroPython on your board for the first time then you should
first erase the entire flash using:

```bash
esptool.py erase_flash
```

`esptool.py` will try to detect the serial port with the ESP32 automatically,
but if this fails or there might be more than one Espressif-based device
attached to your computer then pass the `--port` option with the name of the
target serial port. For example:

```bash
esptool.py --port PORTNAME erase_flash
```

* On Linux, the port name is usually similar to `/dev/ttyUSB` or `/dev/ttyACM0`.
* On Mac, the port name is usually similar to `/dev/cu.usbmodem01`.
* On Windows, the port name is usually similar to `COM4`.

### Flashing

Then deploy the firmware to the board, starting at address {deploy_options[flash_offset]}:

```bash
esptool.py --baud 460800 write_flash {deploy_options[flash_offset]} ESP32_BOARD_NAME-DATE-VERSION.bin
```

Replace `ESP32_BOARD_NAME-DATE-VERSION.bin` with the `.bin` file downloaded from this page.

As above, if `esptool.py` can't automatically detect the serial port
then you can pass it explicitly on the command line instead. For example:

```bash
esptool.py --port PORTNAME --baud 460800 write_flash {deploy_options[flash_offset]} ESP32_BOARD_NAME-DATE-VERSION.bin
```

### Troubleshooting

If flashing starts and then fails partway through, try removing the `--baud
460800` option to flash at the slower default speed.

If these steps don't work, consult the [MicroPython ESP32 Troubleshooting
steps](https://docs.micropython.org/en/latest/esp32/tutorial/intro.html#troubleshooting-installation-problems)
and the [esptool
documentation](https://docs.espressif.com/projects/esptool/en/latest/{mcu}/esptool/basic-options.html).

**Important**: From the options below, download the ``.bin`` file for your board.
