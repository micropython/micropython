Program your board using the esptool.py program, found [here](https://github.com/espressif/esptool).

If you are putting MicroPython on your board for the first time then you should
first erase the entire flash using:

```bash
esptool.py --chip esp32s2 --port /dev/ttyACM0 erase_flash
```

From then on program the firmware starting at address 0x1000:

```bash
esptool.py --chip esp32s2 --port /dev/ttyACM0 write_flash -z 0x1000 board-20210902-v1.17.bin
```
