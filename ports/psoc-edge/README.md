MicroPython port to PSOC Edge MCUs
==================================

This is a port of MicroPython to PSOC Edge microcontrollers.

Features currently supported:
- UART REPL.

Build environment
-----------------

To build this port requires:
- an ARM cross compiler such as `arm-none-eabi-gcc` and associated binary utilities and
  libc
- Infineon edgeprotecttools, available from https://github.com/Infineon/edgeprotecttools/

Furthermore, to deploy firmware to a device requires a custom version of OpenOCD which
is available here: https://softwaretools-hosting.infineon.com/packages/com.ifx.tb.tool.modustoolboxprogtools/versions/1.5.0.1534/artifacts/ModusToolboxProgtools_1.5.0.1534.deb/download

Build instructions
------------------

Before building the firmware for a given board the MicroPython cross-compiler
must be built; it will be used to pre-compile some of the built-in scripts to
bytecode.  The cross-compiler is built and run on the host machine, using:
```bash
$ make -C mpy-cross
```

This command should be executed from the root directory of this repository.
All other commands below should be executed from the ports/psoc-edge/ directory.

All boards require certain submodules to be obtained before they can be built.
The correct set of submodules can be initialised using (with `KIT_PSE84_AI` as an
example of the selected board):
```bash
make BOARD=KIT_PSE84_AI submodules
```

Then to build the board's firmware run:
```bash
make BOARD=KIT_PSE84_AI
```

The above command should produce binary images in the `build-KIT_PSE84_AI/`
subdirectory (or the equivalent directory for the board specified).

To flash the firmware, run:

```bash
make BOARD=KIT_PSE84_AI deploy
```
