SAMD21x18
=========

This port brings MicroPython to SAMD21x18 based development boards under the name
CircuitPython. Supported boards include:

-  Adafruit CircuitPlayground Express
-  Adafruit Feather M0 Basic
-  Adafruit Feather M0 Express
-  Adafruit Metro M0 Express
-  Adafruit M0 Bluefruit LE
-  Arduino Zero
-  Arduino MKR Zero


Pinout
------

All of the boards share the same core pin functionality but call pins by
different names. The table below matches the pin order in
`the datasheet <http://ww1.microchip.com/downloads/en/DeviceDoc/40001882A.pdf>`_
and omits the pins only available on the largest package because all supported
boards use smaller version.

.. raw:: html

	<iframe src="https://docs.google.com/spreadsheets/d/e/2PACX-1vScaJ9uQ6F4GrqRryacOq_mXEN65M5stznXedXISxaqJ9Udg8bWvizAKO0_pByGftUb5eCmMR2MIxJD/pubhtml?widget=true&amp;headers=false"></iframe>

The full pinout spreadsheet can be found `here <https://docs.google.com/spreadsheets/d/e/2PACX-1vScaJ9uQ6F4GrqRryacOq_mXEN65M5stznXedXISxaqJ9Udg8bWvizAKO0_pByGftUb5eCmMR2MIxJD/pubhtml>`_.

Here is a table about which pins can do what in CircuitPython terms. However,
just because something is listed, doesn't mean it will always work. Existing use
of other pins and functionality will impact your ability to use a pin for your
desired purpose. For example, only certain combinations of SPI pins will work
because they use shared hardware internally.

=====================  ========  =========  =========  =======  =======  =======  =========  =========  =======  ==========  ==========  =========  =========  =========  ============  =======  =======  =========
`microcontroller.pin`  `analogio`           `audioio`  `bitbangio`                `busio`                                                                                 `digitalio`   `pulseio`         `touchio`
---------------------  -------------------  ---------  -------------------------  --------------------------------------------------------------------------------------  ------------  ----------------  ---------
Datasheet              AnalogIn  AnalogOut  AudioOut   I2C      OneWire  SPI      I2C - SDA  I2C - SCL  OneWire  SPI - MISO  SPI - MOSI  SPI - SCK  UART - RX  UART - TX  DigitalInOut  PulseIn  PWMOut   TouchIn
=====================  ========  =========  =========  =======  =======  =======  =========  =========  =======  ==========  ==========  =========  =========  =========  ============  =======  =======  =========
PA00                                                   **Yes**  **Yes**  **Yes**  **Yes**               **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**
PA01                                                   **Yes**  **Yes**  **Yes**             **Yes**    **Yes**  **Yes**                 **Yes**    **Yes**               **Yes**       **Yes**  **Yes**
PA02                   **Yes**   **Yes**    **Yes**    **Yes**  **Yes**  **Yes**                        **Yes**                                                           **Yes**       **Yes**           **Yes**
PA03                   **Yes**                         **Yes**  **Yes**  **Yes**                        **Yes**                                                           **Yes**       **Yes**           **Yes**
PB08                   **Yes**                         **Yes**  **Yes**  **Yes**  **Yes**               **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**           **Yes**
PB09                   **Yes**                         **Yes**  **Yes**  **Yes**             **Yes**    **Yes**  **Yes**                 **Yes**    **Yes**               **Yes**       **Yes**  **Yes**  **Yes**
PA04                   **Yes**                         **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**  **Yes**  **Yes**
PA05                   **Yes**                         **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**                 **Yes**    **Yes**               **Yes**       **Yes**  **Yes**  **Yes**
PA06                   **Yes**                         **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**  **Yes**  **Yes**
PA07                   **Yes**                         **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**                 **Yes**    **Yes**               **Yes**       **Yes**  **Yes**  **Yes**
PA08                   **Yes**                         **Yes**  **Yes**  **Yes**  **Yes**               **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**  **Yes**
PA09                   **Yes**                         **Yes**  **Yes**  **Yes**             **Yes**    **Yes**  **Yes**                 **Yes**    **Yes**               **Yes**       **Yes**  **Yes**
PA10                   **Yes**                         **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**  **Yes**
PA11                   **Yes**                         **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**     **Yes**    **Yes**               **Yes**       **Yes**  **Yes**
PB10                                                   **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**  **Yes**
PB11                                                   **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**     **Yes**    **Yes**               **Yes**       **Yes**  **Yes**
PA12                                                   **Yes**  **Yes**  **Yes**  **Yes**               **Yes**  **Yes**                            **Yes**    **Yes**    **Yes**       **Yes**  **Yes**
PA13                                                   **Yes**  **Yes**  **Yes**             **Yes**    **Yes**  **Yes**                 **Yes**    **Yes**               **Yes**       **Yes**  **Yes**
PA14                                                   **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**  **Yes**
PA15                                                   **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**     **Yes**    **Yes**               **Yes**       **Yes**  **Yes**
PA16                                                   **Yes**  **Yes**  **Yes**  **Yes**               **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**  **Yes**
PA17                                                   **Yes**  **Yes**  **Yes**             **Yes**    **Yes**  **Yes**                 **Yes**    **Yes**               **Yes**       **Yes**  **Yes**
PA18                                                   **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**  **Yes**
PA19                                                   **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**     **Yes**    **Yes**               **Yes**       **Yes**  **Yes**
PA20                                                   **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**  **Yes**
PA21                                                   **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**     **Yes**    **Yes**               **Yes**       **Yes**  **Yes**
PA22                                                   **Yes**  **Yes**  **Yes**  **Yes**               **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**  **Yes**
PA23                                                   **Yes**  **Yes**  **Yes**             **Yes**    **Yes**  **Yes**                 **Yes**    **Yes**               **Yes**       **Yes**  **Yes**
PA24
PA25
PB22                                                   **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**
PB23                                                   **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**     **Yes**    **Yes**               **Yes**       **Yes**
PA27                                                   **Yes**  **Yes**  **Yes**                        **Yes**                                                           **Yes**       **Yes**
PA28                                                   **Yes**  **Yes**  **Yes**                        **Yes**                                                           **Yes**       **Yes**
PA29                                                   **Yes**  **Yes**  **Yes**                        **Yes**                                                           **Yes**       **Yes**
PA30                                                   **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**  **Yes**
PA31                                                   **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**     **Yes**    **Yes**               **Yes**       **Yes**  **Yes**
PB02                   **Yes**                         **Yes**  **Yes**  **Yes**  **Yes**               **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**           **Yes**
PB03                   **Yes**                         **Yes**  **Yes**  **Yes**             **Yes**    **Yes**  **Yes**                 **Yes**    **Yes**               **Yes**       **Yes**           **Yes**
=====================  ========  =========  =========  =======  =======  =======  =========  =========  =======  ==========  ==========  =========  =========  =========  ============  =======  =======  =========

Setup
-----

An ARM compiler is required for the build, along with the associated binary
utilities.  On Ubuntu, these can be installed as follows:

.. code-block:: shell

    sudo add-apt-repository ppa:team-gcc-arm-embedded/ppa
    sudo apt-get install gcc-arm-embedded

On Arch Linux the compiler is available for via the package
``arm-none-eabi-gcc``.

For other systems, the `GNU Arm Embedded Toolchain <https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads>`_
may be available in binary form.

The latest available package from team-gcc-arm-embedded is used to produce the
binaries shipped by AdaFruit.  Other compiler versions, particularly older
ones, may not work properly.  In particular, the ``gcc-arm-none-eabi`` package
in Debian Stretch is too old.

The compiler can be changed using the ``CROSS_COMPILE`` variable when invoking
``make``.

Building
--------

Before building the firmware for a given board, there are two additional steps.
These commands should be executed from the root directory of the repository
(``circuitpython/``).

1. There are various submodules that reside in different repositories. In order
   to have these submodules locally, you must pull them into your clone, using:

.. code-block:: shell

   git submodule update --init --recursive

2. The MicroPython cross-compiler must be built; it will be used to pre-compile
   some of the built-in scripts to bytecode.  The cross-compiler is built and
   run on the host machine, using:

.. code-block:: shell

    make -C mpy-cross


Build commands are run from the ``circuitpython/ports/atmel-samd`` directory.

To build for a given board you must specify it by setting ``BOARD``. For example:

.. code-block:: shell

    make BOARD=feather_m0_basic

Board names are the directory names in the `boards <https://github.com/adafruit/circuitpython/tree/master/ports/atmel-samd/boards>`_ folder.

Deploying
---------

Arduino Bootloader
^^^^^^^^^^^^^^^^^^

If your board has an existing Arduino bootloader on it then you can use bossac
to flash MicroPython. First, activate the bootloader. On Adafruit Feathers you
can double click the reset button and the #13 will fade in and out. Finally,
run bossac:

    tools/bossac_osx -e -w -v -b -R build-feather_m0_basic/firmware.bin

No Bootloader via GDB
^^^^^^^^^^^^^^^^^^^^^

This method works for loading MicroPython onto the Arduino Zero via the
programming port rather than the native USB port.

Note: These instructions are tested on Mac OSX and will vary for different
platforms.

    openocd -f ~/Library/Arduino15/packages/arduino/hardware/samd/1.6.6/variants/arduino_zero/openocd_scripts/arduino_zero.cfg

In another terminal from ``micropython/atmel-samd``:

    arm-none-eabi-gdb build-arduino_zero/firmware.elf
    (gdb) tar ext :3333
    ...
    (gdb) load
    ...
    (gdb) monitor reset init
    ...
    (gdb) continue

Connecting
----------

Serial
^^^^^^

All boards are currently configured to work over USB rather than UART. To
connect to it from OSX do something like this:

    screen /dev/tty.usbmodem142422 115200

You may not see a prompt immediately because it doesn't know you connected. To
get one either hit enter to get `>>>` or do CTRL-B to get the full header.

Mass storage
^^^^^^^^^^^^

All boards will also show up as a mass storage device. Make sure to eject it
before resetting or disconnecting the board.

Port Specific modules
---------------------

.. toctree::
    bindings/samd/__init__
