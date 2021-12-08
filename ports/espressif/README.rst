CircuitPython on Espressif SoCs
=======================================

This port adds the Espressif line of SoCs to CircuitPython.

Support Status:
---------------------------------------

.. csv-table::
    :header: SoC, Status

    ESP32-C3, "beta"
    ESP32-S2, "stable"
    ESP32-S3, "beta"

How this port is organized:
---------------------------------------

- **bindings/** contains some required bindings to the ESP-IDF for exceptions and memory.
- **boards/** contains the configuration files for each development board and breakout available on the port.
- **common-hal/** contains the port-specific module implementations, used by shared-module and shared-bindings.
- **esp-idf/** contains the Espressif IoT development framework installation, including all the drivers for the port.
- **modules/** contains information specific to certain Espressif SoC based hardware modules, such as the pins used for flash and RAM on the WROVER and WROOM.
- **peripherals/** contains peripheral setup files and peripheral mapping information, sorted by family and sub-variant. Most files in this directory can be generated with the python scripts in **tools/**.
- **supervisor/** contains port-specific implementations of internal flash, serial and USB, as well as the **port.c** file, which initializes the port at startup.
- **tools/** includes useful python scripts for debugging and other purposes.

At the root level, refer to **mpconfigboard.h** and **mpconfigport.mk** for port specific settings and a list of enabled circuitpython modules.

Connecting to the ESP32-C3
---------------------------------------

**USB Connetion:**

On ESP32-C3 REV3 chips, a USB Serial/JTAG Controller is available. Note: This USB connection cannot be used for a ``CIRCUITPY`` drive.

Depending on the board you have, the USB port may or may not be connected to native USB.

Following connections need to be made if native USB isn't available on USB port:

.. csv-table::
    :header: GPIO, USB

    19, "D+ (green)"
    18, "D- (white)"
    GND, "GND (black)"
    5V, "5V (red)"

Connect these pins using a `USB adapter <https://www.adafruit.com/product/4090>`_ or `breakout cable <https://www.adafruit.com/product/4448>`_.

**UART Connection:**

A `USB to UART convertor <https://www.adafruit.com/product/3309>`_ can be used for connecting to ESP32-C3 to get serial console, REPL interface and flashing CircuitPython.

Following connections need to be made in this case.

.. csv-table::
    :header: GPIO, UART

    21, "RX"
    20, "TX"
    GND, "GND"
    5V, "5V"

**BLE Connection:**

This feature is not yet available and currently under development.

Connecting to the ESP32-S2
---------------------------------------

**USB Connetion:**

Depending on the board you have, the USB port may or may not be connected to native USB.

Following connections need to be made if native USB isn't available on USB port:

.. csv-table::
    :header: GPIO, USB

    20, "D+ (green)"
    19, "D- (white)"
    GND, "GND (black)"
    5V, "5V (red)"

Connect these pins using a `USB adapter <https://www.adafruit.com/product/4090>`_ or `breakout cable <https://www.adafruit.com/product/4448>`_ to access the CircuitPython drive.

**UART Connection:**

A `USB to UART convertor <https://www.adafruit.com/product/3309>`_ can be used for connecting to ESP32-S2 to get serial console and flashing CircuitPython.

Following connections need to be made in this case:

.. csv-table::
    :header: GPIO, UART

    43, "RX"
    44, "TX"
    GND, "GND"
    5V, "5V"

**BLE Connection:**

This feature isn't available on ESP32-S2.

Connecting to the ESP32-S3
---------------------------------------

**USB Connetion:**

Depending on the board you have, the USB port may or may not be connected to native USB.

Following connections need to be made if native USB isn't available on USB port:

.. csv-table::
    :header: GPIO, USB

    20, "D+ (green)"
    19, "D- (white)"
    GND, "GND (black)"
    5V, "5V (red)"

Connect these pins using a `USB adapter <https://www.adafruit.com/product/4090>`_ or `breakout cable <https://www.adafruit.com/product/4448>`_ to access the CircuitPython drive.

**UART Connection:**

A `USB to UART convertor <https://www.adafruit.com/product/3309>`_ can be used for connecting to ESP32-S3 to get serial console and flashing CircuitPython.

Following connections need to be made in this case:

.. csv-table::
    :header: GPIO, UART

    43, "RX"
    44, "TX"
    GND, "GND"
    5V, "5V"

**BLE Connection:**

This feature is not yet available and currently under development.

Building and flashing
---------------------------------------

Before building or flashing the, you must `install the esp-idf <https://docs.espressif.com/projects/esp-idf/en/latest/esp32/index.html>`_.

Note: This must be re-done every time the esp-idf is updated, but not every time you build.

Run ``cd ports/espressif`` from ``circuitpython/`` to move to the espressif port root, and run:

.. code-block::

    ./esp-idf/install.sh

After this initial installation, you must add the esp-idf tools to your path.

Note: This must be re-done every time you open a new bash environment for building or flashing.

Run ``cd ports/espressif`` from ``circuitpython/`` to move to the espressif port root, and run:

.. code-block::

    . esp-idf/export.sh

When CircuitPython updates the ESP-IDF to a new release, you may need to run this installation process again. The exact commands used may also vary based on your bash environment.

Building boards is typically done through ``make BOARD=board_id``. The default port is ``tty.SLAB_USBtoUART``, which will only work on certain Mac setups. On most machines, both Mac and Linux, you will need to set the port yourself by running ``ls /dev/tty.usb*`` and selecting the one that only appears when your development board is plugged in. An example make command with the port setting is as follows:

.. code-block::

    make BOARD=board_id PORT=/dev/tty.usbserial-1421120 flash

``board_id`` is the unique board identifier in CircuitPython. It is the same as the name of board in ``boards`` directory.

Debugging
---------------------------------------

TODO: Add documentation for ESP32-C3/S3 JTAG feature.

The ESP32-S2 supports JTAG debugging over OpenOCD using a JLink or other probe hardware. The official tutorials can be found on the Espressif website `here <https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/api-guides/jtag-debugging/index.html>`_, but they are mostly for the ESP32-S2 Kaluga, which has built-in debugging.

OpenOCD is automatically installed and added to your bash environment during the esp-idf installation and setup process. You can double check that it is installed by using ``openocd --version``, as per the tutorial. Attach the JTAG probe pins according to the `instructions for JTAG debugging <https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/api-guides/jtag-debugging/configure-other-jtag.html>`_ on boards that do not contain an integrated debugger.

Once the debugger is connected physically, you must run OpenOCD with attached configuration files specifying the **interface** (your debugger probe) and either a **target** or a **board** (targets are for SoCs only, and can be used when a full board configuration file doesn't exist). You can find the path location of these files by checking the ``OPENOCD_SCRIPTS`` environmental variable by running ``echo $OPENOCD_SCRIPTS`` in bash. Interfaces will be in the ``interface/`` directory, and targets and boards in the ``target/`` and ``board/`` directories, respectively.

**Note:** Unfortunately, there are no board files for the esp32-s2 other than the Kaluga, and the included ``target/esp32s2.cfg`` target file will not work by default on the Jlink for boards like the Saola 1, as the default speed is incorrect. In addition, these files are covered under the GPL and cannot be included in CircuitPython. Thus, you must make a copy of the esp32s2.cfg file yourself and add the following line manually, under ``transport select jtag`` at the start of the file:

.. code-block::

    adapter_khz 1000

Once this is complete, your final OpenOCD command may look something like this:

.. code-block::

    openocd -f interface/jlink.cfg -f SOMEPATH/copied-esp32s2-saola-1.cfg

Where ``SOMEPATH`` is the location of your copied configuration file (this can be placed in the port/boards directory with a prefix to ignore it with ``.gitignore``, for instance). Interface, target and board config files sourced from espressif only need their paths from the $OPENOCD_SCRIPTS location, you don't need to include their full path. Once OpenOCD is running, connect to GDB with:

.. code-block::

    xtensa-esp32s2-elf-gdb build-espressif_saola_1_wrover/firmware.elf

And follow the Espressif GDB tutorial `instructions for connecting <https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/api-guides/jtag-debugging/using-debugger.html>`_, or add them to your ``gdbinit``:

.. code-block::

    target remote :3333
    set remote hardware-watchpoint-limit 2
    mon reset halt
    flushregs
    thb app_main
    c
