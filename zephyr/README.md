MicroPython port to Zephyr RTOS
===============================

This is an initial port of MicroPython to Zephyr RTOS
(http://zephyrproject.org).

The port integrates well with Zephyr build system, using the latest
features which will be available in 1.6.0, and thus requires Zephyr
master to build against. All boards supported by Zephyr (with standard
level of feature support, like UART console) should work with
MicroPython (but not all were tested).

Features supported at this time:

* REPL (interactive prompt) over Zephyr UART console.
* `utime` module for time measurements and delays.
* `machine.Pin` class for GPIO control.
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

    make qemu

For deploying/flashing a firmware on a real board, follow Zephyr
documentation for a given board.


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
