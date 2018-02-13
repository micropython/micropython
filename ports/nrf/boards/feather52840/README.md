# Setup

## Installing CircuitPython submodules

Before you can build, you will need to run the following commands once, which
will install the submodules that are part of the CircuitPython ecosystem, and
build the `mpy-cross` tool:

```
$ cd circuitpython
$ git submodule update --init
$ make -C mpy-cross
```

You then need to download the SD and Nordic SDK files via:

> This script relies on `wget`, which must be available from the command line.

```
$ cd ports/nrf
$ ./drivers/bluetooth/download_ble_stack.sh
```

## Serial Bootloader

The Adafruit nRF52840 Feather uses a serial bootloader that allows you to
update the core CircuitPython firmware and internal file system contents
using only a serial connection.

On empty devices, the serial bootloader will need to be flashed once using a
HW debugger such as a Segger J-Link before the serial updater (`nrfutil`) can
be used.

### Install the Bootloader with `nrfjprog`

```
make SD=s140 BOARD=feather52840 boot-flash
```

## Building and Flashing CircuitPython

```
make SD=s140 SERIAL=/dev/ttyACM0 BOARD=feather52840 dfu-gen dfu-flash
```
