MicroPython port to Realtek Ameba
=================================

This is a port of MicroPython to the Realtek Ameba family of microcontrollers.
It uses the Realtek ameba-rtos SDK and MicroPython runs as a task under FreeRTOS.

This is an initial, minimal port.  Currently supported:
- REPL (Python prompt) over the LOGUART serial port.
- Many of MicroPython's core features: unicode, arbitrary-precision integers,
  single-precision floats, frozen bytecode, and many of the internal modules.

Support for an internal filesystem, the `machine` module (GPIO, etc.), the
`network` module (Wi-Fi), sockets and other peripherals is not yet included and
is planned for follow-up contributions.

Supported boards
----------------

- PKE8721DAF (RTL8721DAF, AmebaDplus)

Setting up the SDK and toolchain
--------------------------------

This port builds against the Realtek ameba-rtos SDK:

    https://github.com/Ameba-AIoT/ameba-rtos

The SDK provides the FreeRTOS-based runtime, the CMake build framework (driven
by `ameba.py`) and a script (`env.sh`) that installs the required GCC toolchain.

Clone the SDK anywhere you like and check out the exact revision this port is
developed and CI-tested against:

    git clone https://github.com/Ameba-AIoT/ameba-rtos.git
    cd ameba-rtos
    git checkout --detach ffc6e1a53d28bb500a4cc28328b683f20fa223df

Building against a different SDK revision is not supported and may fail.  This
commit must match `AMEBA_RTOS_VER` in `.github/workflows/ports_ameba.yml`;
update both together when bumping the SDK version.

The SDK's own git submodules are all optional AI / audio / UI components that
this minimal port does not need, so there is no need to initialise them.

Then activate the SDK in your shell before building:

    source /path/to/ameba-rtos/env.sh

Sourcing `env.sh` installs/activates the GCC toolchain and Python venv and
exports `AMEBA_SDK` (the SDK root).  The build inherits this environment, so the
SDK location is taken entirely from whatever you sourced — this port makes no
assumption about where the SDK lives.

Building
--------

After sourcing `env.sh` as above (so `AMEBA_SDK` and the toolchain are in your
environment), from this port directory:

    make submodules
    make BOARD=PKE8721DAF

This produces `firmware.bin` (along with `boot.bin` and `app.bin`) in the
`build_<SOC>` directory.

Flashing
--------

Connect the board over USB-serial.  The flash tool attempts to enter download
mode automatically via the DTR/RTS lines; if that fails, hold the Download key,
press and release the Reset key, then release the Download key.  Then run:

    make deploy PORT=/dev/ttyUSB0

Use `BAUD=` to override the flashing baud rate, for example:

    make deploy PORT=/dev/ttyUSB0 BAUD=115200

Connecting to the REPL
----------------------

After flashing, the MicroPython REPL is available on the LOGUART serial port at
115200 baud (the same USB-serial port used for flashing).  Close the flash tool
first, then connect with any serial terminal, for example:

    mpremote connect /dev/ttyUSB0

or:

    picocom -b 115200 /dev/ttyUSB0

or:

    python -m serial.tools.miniterm /dev/ttyUSB0 115200

On Linux/WSL you need permission to access the serial device.  `/dev/ttyUSB0` is
typically owned by the `dialout` group, so add your user to it once, then log
out and back in (or reboot):

    sudo usermod -aG dialout $USER

Troubleshooting
---------------

Flashing fails immediately with `Fail to read eFuse: DEV_TIMEOUT` (or a similar
timeout): the serial port opened but the chip did not respond, which almost
always means it is not in download mode.  Enter it manually — hold the Download
key, press and release the Reset key, then release the Download key — and re-run
`make deploy` straight away.

Flashing fails because the port is busy: close any open serial terminal
(`mpremote`, `picocom`, `miniterm`, SecureCRT, ...) before flashing; only one
program may hold the port at a time.

Flashing is slow or unreliable at the default rate: retry at a lower baud rate
with `make deploy PORT=/dev/ttyUSB0 BAUD=115200`.

On WSL: make sure the board is attached to WSL (`usbipd attach`) and that
`/dev/ttyUSB0` is present.  After a reset the device may briefly disappear and
re-enumerate; wait for it to settle before flashing.
