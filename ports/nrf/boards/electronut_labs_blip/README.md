# Setup

The `Electronut Labs Blip` board is a development board based on the `nRF52840` SoC from
Nordic Semiconductors. It has a Black Magic Probe compatible programmer and debugger
built in, along with temperature/humidity sensor, ambient light intensity sensor, and
a 3-axis accelerometer. It can be used to prototype very low power devices. It also has
provision for an SD card slot, which makes it a complete and versatile development board.

Schematic, datasheet, pin mapping etc. can be found over [here](https://docs.electronut.in/blip/).

Features:

* Raytac MDBT50Q-1M module based on Nordic Semiconductor's nRF52840
* LIS2DDH12 High-performance 3-axis "femto" accelerometer
* Optical Sensor LTR-329ALS-01
* Si7006-A20 I2C humidity and temperature sensor
* On board STM32F103CBT6 as Black magic probe debugger
* NFC Antenna
* MicroSD slot
* Power Supply: USB, JST connector for Li-ion/Li-po
* BQ24079 battery charging and power management IC

## Installing CircuitPython submodules

Before you can build, you will need to run the following commands once, which
will install the submodules that are part of the CircuitPython ecosystem, and
build the `mpy-cross` tool:

```
$ cd circuitpython
$ git submodule update --init
$ make -C mpy-cross
```

## Building and Flashing CircuitPython

No special notes for this, follow `ports/nrf` generic `README.md`.

### Flashing CircuitPython with GDB using on board Black magic probe debugger

```
$ cd ports/nrf
$ make V=1 SD=s140 SERIAL=/dev/ttyACM0 BOARD=electronut_labs_blip all
...
...
LINK build-electronut_labs_blip-s140/firmware.elf

778588 bytes free in flash out of 1048576 bytes ( 1024.0 kb ).
228320 bytes free in ram for stack out of 245760 bytes ( 240.0 kb ).

Create build-electronut_labs_blip-s140/firmware.bin
Create build-electronut_labs_blip-s140/firmware.hex
Create build-electronut_labs_blip-s140/firmware.uf2
python3 ../../tools/uf2/utils/uf2conv.py -f 0xADA52840 -c -o "build-electronut_labs_blip-s140/firmware.uf2" build-electronut_labs_blip-s140/firmware.hex
Converting to uf2, output size: 540160, start address: 0x26000
Wrote 540160 bytes to build-electronut_labs_blip-s140/firmware.uf2.
```

Now you can use either `.hex` or `.elf` from the generated files inside
`build-electronut_labs_blip-s140` directory. Now you can use `arm-none-eabi-gdb`
to flash circuitpython on Blip.

### Other tips

Once circuitpython is running on your board, it will come up as a mass storage
device named `CIRCUITPY`, where you can drop in your python code. The file names
it looks for are `main.py`, `main.txt`, `code.py` or `code.txt`.
