MicroPython port to Alif Ensemble MCUs
======================================

This is a port of MicroPython to the Alif Ensemble series of microcontrollers.

Initial development of this Alif port was sponsored by OpenMV LLC.

Features currently supported:
- UART REPL.
- TinyUSB with CDC and MSC device support.
- Octal SPI flash with XIP mode.
- machine.Pin support with named pins.
- machine.UART, machine.SPI, machine.I2C, machine.RTC peripherals.
- WiFi and Bluetooth using cyw43.
- Dual core support of the HE and HP cores using Open-AMP.
- Low power modes.

The following more advanced features will follow later:
- Ethernet support.
- SDRAM support.
- Other machine modules.

Build instructions
------------------

Before building the firmware for a given board the MicroPython cross-compiler
must be built; it will be used to pre-compile some of the built-in scripts to
bytecode.  The cross-compiler is built and run on the host machine, using:
```bash
$ make -C mpy-cross
```

This command should be executed from the root directory of this repository.
All other commands below should be executed from the ports/alif/ directory.

An ARM compiler is required for the build, along with the associated binary
utilities.  The recommended toolchain version to use with this port is
Arm GNU toolchain version 13.3.Rel1. The compiler can be changed using the
`CROSS_COMPILE` variable when invoking `make`.

Next, the board to build must be selected. The default board is `ALIF_ENSEMBLE`
but any of the names of the subdirectories in the `boards/` directory is valid.
The board name must be passed as the argument to `BOARD=` when invoking `make`.

All boards require certain submodules to be obtained before they can be built.
The correct set of submodules can be initialised using (with `ALIF_ENSEMBLE`
as an example of the selected board):
```bash
make BOARD=ALIF_ENSEMBLE submodules
```

Then to build the board's firmware run:
```bash
make BOARD=ALIF_ENSEMBLE
```

The above command should produce binary images in the `build-ALIF_ENSEMBLE/`
subdirectory (or the equivalent directory for the board specified).

### Update the SE Firmware

The SE firmware must be updated **before** flashing the main firmware to match
the version used by MicroPython. This step only needs to be performed once.
Connect the board to your PC via the **SE UART USB** port (on the Ensemble kit,
this is labeled **PRG USB**), then run:

```bash
make update-system-package
```

**Note:** The board must be power-cycled after this step.

### Deploy MicroPython

To flash the firmware, run:

```bash
make BOARD=ALIF_ENSEMBLE deploy
```
