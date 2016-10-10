MicroPython port to Zephyr RTOS
===============================

This is an initial port of MicroPython to Zephyr RTOS
(http://zephyrproject.org).

The port integrates well with Zephyr build system, using the latest
features which will be available in 1.6.0, and thus requires Zephyr
master to build against. All boards supported by Zephyr should be
supported (but not all were tested).

At this time, only basic interactive prompt (REPL) over UART connection
is supported. Over time, bindings for various Zephyr subsystems may
be added.


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

To run the resulting application in QEMU (for BOARDs like qemu_x86,
qemu_cortex_m3):

    make qemu

For deploying/flashing the application on a real board, follow Zephyr
documentation for a given board.
