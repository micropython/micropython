# Setup

The `Electronut Labs Papyr` board is based on the `nRF52840` SoC from
Nordic Semiconductors. It has an e-ink display on it, along with a CR2477
battery holder.

Papyr can be programmed with the [`Adafruit nRF52 bootloader`](https://github.com/adafruit/Adafruit_nRF52_Bootloader) to

Schematic, datasheet default pin mapping etc. can be found over [here](https://docs.electronut.in/papyr/). The default pin mapping can be found in the board directory.

## Installing CircuitPython submodules

Before you can build, you will need to run the following commands once, which
will install the submodules that are part of the CircuitPython ecosystem, and
build the `mpy-cross` tool:

```
$ cd circuitpython
$ git submodule update --init
$ make -C mpy-cross
```

## Installing the Bootloader

If the `Adafruit nRF52 bootloader` is installed on the board, then the
bootloader allows you to update the core CircuitPython firmware and internal
file system contents using serial, or USB CDC, or USB mass storage.

On empty devices, the bootloader will need to be flashed once using a
HW debugger such as a Segger J-Link, or Blackmagicprobe
(or [Electronut labs Bumpy](https://docs.electronut.in/bumpy/)).


## Building and Flashing CircuitPython

No special notes for this, follow `ports/nrf` generic `README.md`.

### Flashing CircuitPython with MSC UF2

`uf2` file is generated last by `all` target.

```
$ cd ports/nrf
$ make V=1 SD=s140 SERIAL=/dev/ttyACM0 BOARD=electronut_labs_papyr all
...
...
python3 ../../tools/uf2/utils/uf2conv.py -f 0xADA52840 -c -o "build-electronut_labs_papyr-s140/firmware.uf2" build-electronut_labs_papyr-s140/firmware.hex
Converting to uf2, output size: 536576, start address: 0x26000
Wrote 536576 bytes to build-electronut_labs_papyr-s140/firmware.uf2
```

Simply drag and drop firmware.uf2 to the MSC, the nrf52840 will blink fast and reset after done.

### Other tips

Once circuitpython is running on your board, it will come up as a mass storage
device named `CIRCUITPY`, where you can drop in your python code. The file names
it looks for are `main.py`, `main.txt`, `code.py` or `code.txt`.
