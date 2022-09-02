Program your board using the latest version of the esptool.py program, found [here](https://github.com/espressif/esptool).

To flash or erase LOLIN S3, you have to first put it into download mode:

If you use 'UART' USB Port, it will automatically entry download mode.

If you use 'OTG' USB Port, to do this, follow these steps:

- Press and hold the [IO0] button
- Press and release the [RST] button
- Release the [IO0] button



### Linux
```bash
esptool.py --chip esp32s3 --port /dev/ttyACM0 write_flash -e -z 0x0 firmware.bin
```

### Mac
Please do a `ls /dev/cu.usbm*` to determine the port your board has enumerated as.
```bash
esptool.py --chip esp32s3 --port /dev/cu.usbmodem01 write_flash -e -z 0x0 firmware.bin
```

### Windows
Change (X) to whatever COM port is being used by the board
```bash
esptool.py.exe --chip esp32s3 --port COM(X) write_flash -e -z 0x0 firmware.bin
```
