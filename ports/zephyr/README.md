MicroPython port to Zephyr RTOS
===============================

This is a work-in-progress port of MicroPython to Zephyr RTOS
(http://zephyrproject.org).

This port requires Zephyr version 2.4.0, and may also work on higher
versions.  All boards supported
by Zephyr (with standard level of features support, like UART console)
should work with MicroPython (but not all were tested).

Features supported at this time:

* REPL (interactive prompt) over Zephyr UART console.
* `utime` module for time measurements and delays.
* `machine.Pin` class for GPIO control, with IRQ support.
* `machine.I2C` class for I2C control.
* `usocket` module for networking (IPv4/IPv6).
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

    $ west init zephyrproject -m https://github.com/zephyrproject-rtos/zephyr --mr v2.4.0

Alternatively, you don't have to redo the Zephyr installation to just
switch from master to a tagged release, you can instead do:

    $ cd zephyrproject/zephyr
    $ git checkout v2.4.0
    $ west update

With Zephyr installed you may then need to configure your environment,
for example by sourcing `zephyrproject/zephyr/zephyr-env.sh`.

Once Zephyr is ready to use you can build the MicroPython port.
In the port subdirectory `ports/zephyr/` run:

    $ make BOARD=<board>

If you don't specify BOARD, the default is `qemu_x86` (x86 target running
in QEMU emulator).  Consult the Zephyr documentation above for the list of
supported boards.  Board configuration files appearing in `ports/zephyr/boards/`
correspond to boards that have been tested with MicroPython and may have
additional options enabled, like filesystem support.


Running
-------

To run the resulting firmware in QEMU (for BOARDs like qemu_x86,
qemu_cortex_m3):

    make run

With the default configuration, networking is now enabled, so you need to
follow instructions in https://wiki.zephyrproject.org/view/Networking-with-Qemu
to setup host side of TAP/SLIP networking. If you get error like:

    could not connect serial device to character backend 'unix:/tmp/slip.sock'

it's a sign that you didn't followed instructions above. If you would like
to just run it quickly without extra setup, see "minimal" build below.

For deploying/flashing a firmware on a real board, follow Zephyr
documentation for a given board, including known issues for that board
(if any). (Mind again that networking is enabled for the default build,
so you should know if there're any special requirements in that regard,
cf. for example QEMU networking requirements above; real hardware boards
generally should not have any special requirements, unless there're known
issues).

For example, to deploy firmware on the FRDM-K64F board run:

    $ make BOARD=frdm_k64f flash


Quick example
-------------

To blink an LED:

    import time
    from machine import Pin

    LED = Pin(("GPIO_1", 21), Pin.OUT)
    while True:
        LED.value(1)
        time.sleep(0.5)
        LED.value(0)
        time.sleep(0.5)

The above code uses an LED location for a FRDM-K64F board (port B, pin 21;
following Zephyr conventions port are identified by "GPIO_x", where *x*
starts from 0). You will need to adjust it for another board (using board's
reference materials). To execute the above sample, copy it to clipboard, in
MicroPython REPL enter "paste mode" using Ctrl+E, paste clipboard, press
Ctrl+D to finish paste mode and start execution.

To respond to Pin change IRQs, on a FRDM-K64F board run:

    from machine import Pin

    SW2 = Pin(("GPIO_2", 6), Pin.IN)
    SW3 = Pin(("GPIO_0", 4), Pin.IN)

    SW2.irq(lambda t: print("SW2 changed"))
    SW3.irq(lambda t: print("SW3 changed"))

    while True:
        pass

Example of using I2C to scan for I2C slaves:

    from machine import I2C

    i2c = I2C("I2C_0")
    i2c.scan()


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

    ./make-minimal BOARD=<board>

To run a minimal build in QEMU without requiring TAP networking setup
run the following after you built image with the previous command:

    ./make-minimal BOARD=<qemu_x86_nommu|qemu_x86|qemu_cortex_m3> run
