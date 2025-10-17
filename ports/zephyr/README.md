MicroPython port to Zephyr RTOS
===============================

This is a work-in-progress port of MicroPython to Zephyr RTOS
(http://zephyrproject.org).

This port tries to support all Zephyr versions supported upstream,
i.e. currently v3.7 (LTS), v4.2 and the development branch. The CI is
setup to use the latest version, i.e. v4.2.

All boards supported by Zephyr (with standard level of features
support, like UART console) should work with MicroPython (but not all
were tested).

Features supported at this time:

* REPL (interactive prompt) over Zephyr UART console.
* `time` module for time measurements and delays.
* `machine.Pin` class for GPIO control, with IRQ support.
* `machine.I2C` class for I2C control.
* `machine.SPI` class for SPI control.
* `machine.PWM` class for PWM control.
* `machine.ADC` class for ADC control.
* `socket` module for networking (IPv4/IPv6).
* `zsensor` module for reading sensors.
* "Frozen modules" support to allow to bundle Python modules together
  with firmware. Including complete applications, including with
  run-on-boot capability.
* virtual filesystem with FAT and littlefs formats, backed by either
  DiskAccess or FlashArea (flash map).

Over time, bindings for various Zephyr subsystems may be added.


Building
--------

Follow to Zephyr web site for Getting Started instruction of installing
Zephyr SDK, getting Zephyr source code, and setting up development
environment. (Direct link:
https://docs.zephyrproject.org/latest/getting_started/index.html).
You may want to build Zephyr's own sample applications to make sure your
setup is correct.

If you already have Zephyr installed but are having issues building the
MicroPython port then try installing the correct version of Zephyr via:

    $ west init zephyrproject -m https://github.com/zephyrproject-rtos/zephyr --mr v4.2.0

Alternatively, you don't have to redo the Zephyr installation to just
switch from master to a tagged release, you can instead do:

    $ cd zephyrproject/zephyr
    $ git checkout v4.2.0
    $ west update

With Zephyr installed you may then need to configure your environment,
for example by sourcing `zephyrproject/zephyr/zephyr-env.sh`.

Once Zephyr is ready to use you can build the MicroPython port just like any
other Zephyr application. You can do this anywhere in your file system, it does
not have to be in the `ports/zephyr` directory. Assuming you have cloned the
MicroPython repository into your home directory, you can build the Zephyr port
for a frdm_k64f board like this:

    $ west build -b frdm_k64f ~/micropython/ports/zephyr

To build for QEMU instead:

    $ west build -b qemu_x86 ~/micropython/ports/zephyr

To build any board with the `_thread` module enabled,
add `-DOVERLAY_CONFIG=thread.conf`, for instance:

    $ west build -b frdm_k64f ~/micropython/ports/zephyr -DOVERLAY_CONFIG=thread.conf

Consult the Zephyr documentation above for the list of
supported boards.  Board configuration files appearing in `ports/zephyr/boards/`
correspond to boards that have been tested with MicroPython and may have
additional options enabled, like filesystem support.

Running
-------

To flash the resulting firmware to your board:

    $ west flash

Or, to flash it to your board and start a gdb debug session:

    $ west debug

To run the resulting firmware in QEMU (for BOARDs like qemu_x86,
qemu_cortex_m3):

    $ west build -t run

Networking is enabled with the default configuration, so you need to follow
instructions in
https://docs.zephyrproject.org/latest/connectivity/networking/qemu_setup.html#networking-with-qemu
to setup the host side of TAP/SLIP networking. If you get an error like:

    could not connect serial device to character backend 'unix:/tmp/slip.sock'

it's a sign that you didn't follow the instructions above. If you would like
to just run it quickly without extra setup, see "minimal" build below.

Quick example
-------------

To blink an LED:

    import time
    from machine import Pin

    LED = Pin(("gpiob", 21), Pin.OUT)
    while True:
        LED.value(1)
        time.sleep(0.5)
        LED.value(0)
        time.sleep(0.5)

The above code uses an LED location for a FRDM-K64F board (port B, pin 21;
following Zephyr conventions ports are identified by their devicetree node
label. You will need to adjust it for another board (using board's reference
materials). To execute the above sample, copy it to clipboard, in MicroPython
REPL enter "paste mode" using Ctrl+E, paste clipboard, press Ctrl+D to finish
paste mode and start execution.

To respond to Pin change IRQs, on a FRDM-K64F board run:

    from machine import Pin

    SW2 = Pin(("gpioc", 6), Pin.IN)
    SW3 = Pin(("gpioa", 4), Pin.IN)

    SW2.irq(lambda t: print("SW2 changed"))
    SW3.irq(lambda t: print("SW3 changed"))

    while True:
        pass

Example of using I2C to scan for I2C slaves:

    from machine import I2C

    i2c = I2C("i2c0")
    i2c.scan()

Example of using SPI to write a buffer to the MOSI pin:

    from machine import SPI

    spi = SPI("spi0")
    spi.init(baudrate=500000, polarity=1, phase=1, bits=8, firstbit=SPI.MSB)
    spi.write(b'abcd')

Example of using ADC to read a pin's analog value (the 'zephyr,user' node must contain
the 'io-channels' property with all the ADC channels):

    from machine import ADC

    adc = ADC(("adc", 0))
    adc.read_uv()

Example of using FlashArea for flash storage access:

    from zephyr import FlashArea

    # FlashArea.areas is a dictionary mapping partition labels to tuples
    # Each tuple contains (partition_id, erase_block_size)
    print(FlashArea.areas)  # e.g. {'storage': (3, 4096)}

    # Create a FlashArea object for a specific partition
    # Constructor takes (partition_id, block_size)
    partition_id, erase_size = FlashArea.areas['storage']
    flash = FlashArea(partition_id, erase_size)

    # Use with virtual filesystem (see _boot.py for automatic mounting)
    import os
    os.VfsFat(flash)  # or os.VfsLfs2(flash)

The `FlashArea.areas` dictionary provides partition information from the Zephyr
devicetree. The erase block size is obtained from the flash controller's
`erase-block-size` property, or defaults to 4096 bytes for devices (like QSPI
NOR flash) that don't specify this property.

Minimal build
-------------

MicroPython is committed to maintain minimal binary size for Zephyr port
below 128KB, as long as Zephyr project is committed to maintain stable
minimal size of their kernel (which they appear to be). Note that at such
size, there is no support for any Zephyr features beyond REPL over UART,
and only very minimal set of builtin Python modules is available. Thus,
this build is more suitable for code size control and quick demonstrations
on smaller systems. It's also suitable for careful enabling of features
one by one to achieve needed functionality and code size. This is in the
contrast to the "default" build, which may get more and more features
enabled over time.

To make a minimal build:

    $ west build -b qemu_x86 ~/micropython/ports/zephyr -- -DCONF_FILE=prj_minimal.conf

To run a minimal build in QEMU without requiring TAP networking setup
run the following after you built an image with the previous command:

    $ west build -t run

