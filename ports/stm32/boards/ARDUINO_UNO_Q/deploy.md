## Flashing the firmware

The Arduino UNO Q's STM32U585 has no on-board DFU/USB device exposed to the
host (the USB-C port is owned by the Qualcomm application processor), so the
MicroPython firmware is flashed over SWD.

Flash the raw binary to the start of internal flash:

```
openocd -f interface/stlink.cfg -f target/stm32u5x.cfg \
    -c "program firmware.bin 0x08000000 verify reset exit"
```

Any SWD probe and tool capable of programming an STM32U5 will work; adjust the
interface/target configuration for your probe. After programming, the board
runs MicroPython from reset.
