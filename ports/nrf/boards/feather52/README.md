# Setup

Before you can build, you will need to run the following commands once:

```
$ cd circuitpython
$ git submodule update --init
$ make -C mpy-cross
```

You then need to download the SD and Nordic SDK files:

> This script relies on `wget`, which must be available from the command line.

```
$ cd ports/nrf
$ ./drivers/bluetooth/download_ble_stack.sh
```

# Building and flashing firmware images

## Building CircuitPython

#### REPL over UART (default settings)

To build a CircuitPython binary with default settings for the
`feather52` target enter:

```
$ make BOARD=feather52 V=1
```

#### REPL over BLE UART (AKA `NUS`)

To build a CircuitPython binary with REPL over BLE UART, edit
`bluetooth_conf.h` with the following values (under
`#elif (BLUETOOTH_SD == 132)`):

```
#define MICROPY_PY_BLE                  (1)
#define MICROPY_PY_BLE_NUS              (1)
#define BLUETOOTH_WEBBLUETOOTH_REPL     (1)
```

Then build the CircuitPython binary, including `SD=s132`
to enable BLE support in the build process:

```
$ make BOARD=feather52 V=1 SD=s132
```

## Flashing with `nrfutil`

The Adafruit Bluefruit nRF52 Feather ships with a serial and OTA BLE bootloader
that can be used to flash firmware images over a simple serial connection,
using the on-board USB serial converter.

These commands assume that you have already installed `nrfutil`, as described
in the [learning guide](https://learn.adafruit.com/bluefruit-nrf52-feather-learning-guide/arduino-bsp-setup)
for the Arduino variant of the board.

### 1. **Update bootloader** to single-bank version

The Adafruit nRF52 Feather ships, by default, with a **dual-bank** bootloader
that cuts the available flash memory in half in exchange for safer
OTA updates.

Due to the size of CircuitPython, we must migrate this bootloader to a
**single-bank** version, doubling the amount of flash memory available to us.

> These commands only need to be run once and will update the SoftDevice and
bootloader from the dual-bank version that ships on Arduino-based Adafruit
Feather52 boards to a single-bank CircuitPython compatible version:

#### S132 v2.0.1 single-bank (recommended):

By default s132 v2.0.1 is used when no `SOFTDEV_VERSION` field is passed in:

```
$ make BOARD=feather52 SERIAL=/dev/tty.SLAB_USBtoUART boot-flash
```

#### S132 v5.0.0 (BLE5, experimental):

To enable BLE5 support and the latest S132 release, flash the v5.0.0 bootloader via:

```
$ make BOARD=feather52 SERIAL=/dev/tty.SLAB_USBtoUART SOFTDEV_VERSION=5.0.0 boot-flash
```

### 2. Generate a CircuitPython DFU .zip package and flash it over serial

The following command will package and flash the CircuitPython binary using the
appropriate bootloader mentionned above.

This command assumes you have already build a valid circuitpython
image, as described earlier in this readme.

> The name of the serial port target will vary, depending on your OS.

```
$ make BOARD=feather52 SERIAL=/dev/tty.SLAB_USBtoUART dfu-gen dfu-flash
```

If you built your CircuitPython binary with **BLE UART** support you will
need to add the `SD=s132` flag as shown below:

```
$ make BOARD=feather52 SERIAL=/dev/tty.SLAB_USBtoUART SD=s132 dfu-gen dfu-flash
```
