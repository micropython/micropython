MicroPython port to Zephyr RTOS
===============================

This is an work-in-progress port of MicroPython to Zephyr RTOS
(http://zephyrproject.org).

This port requires Zephyr version 1.8 or higher. All boards supported
by Zephyr (with standard level of features support, like UART console)
should work with MicroPython (but not all were tested).

Features supported at this time:

* REPL (interactive prompt) over Zephyr UART console.
* `utime` module for time measurements and delays.
* `machine.I2C` class for I2C control.
* `machine.Pin` class for GPIO control.
* `usocket` module for networking (IPv4/IPv6).
* "Frozen modules" support to allow to bundle Python modules together
  with firmware. Including complete applications, including with
  run-on-boot capability.

Over time, bindings for various Zephyr subsystems may be added.


Building
--------

Follow to Zephyr web site for Getting Started instruction of installing
Zephyr SDK, getting Zephyr source code, and setting up development
environment. (Direct link:
https://www.zephyrproject.org/doc/getting_started/getting_started.html).
You may want to build Zephyr's own sample applications to make sure your
setup is correct.

To build MicroPython port, in the port subdirectory (zephyr/), run:

    make BOARD=<board>

If you don't specify BOARD, the default is `qemu_x86` (x86 target running
in QEMU emulator). Consult Zephyr documentation above for the list of
supported boards.


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

To scan for I2C slaves:

    from machine import I2C

    i2c = I2C("I2C_0")
    i2c.scan()

The above code uses an LED location for a FRDM-K64F board (port B, pin 21;
following Zephyr conventions port are identified by "GPIO_x", where *x*
starts from 0). You will need to adjust it for another board (using board's
reference materials). To execute the above sample, copy it to clipboard, in
MicroPython REPL enter "paste mode" using Ctrl+E, paste clipboard, press
Ctrl+D to finish paste mode and start execution.


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
