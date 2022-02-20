Program your board using the esptool.py program, found [here](https://github.com/espressif/esptool).

If you are putting MicroPython on your board for the first time then you should
first erase the entire flash using:

```bash
esptool.py --chip esp32 --port /dev/ttyUSB0 erase_flash
```

From then on program the firmware starting at address 0x1000:

```bash
esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 460800 write_flash -z 0x1000 esp32-20190125-v1.10.bin
```
