MicroPython port to qemu-arm
============================

This is experimental, community-supported port for Cortex-M emulation as
provided by QEMU (http://qemu.org).

The purposes of this port are to enable:

1. Continuous integration
    - run tests against architecture-specific parts of code base
2. Experimentation
    - simulation & prototyping of anything that has architecture-specific
      code
    - exploring instruction set in terms of optimising some part of
      MicroPython or a module
3. Streamlined debugging
    - no need for JTAG or even an MCU chip itself
    - no need to use OpenOCD or anything else that might slow down the
      process in terms of plugging things together, pressing buttons, etc.

Build instructions
------------------

First make sure the MicroPython cross-compiler is built (run from this directory):

    $ make -C ../../mpy-cross

Then build using:

    $ make

The default qemu-supported board is `mps2-an385`, a Cortex-M3 board.  To select a
different board pass the `BOARD` argument to `make`, for example:

    $ make BOARD=sabrelite

Running
-------

When the firmware is run it will provide a REPL on the emulated hardware UART.
To access the REPL directly use:

    $ make repl

This will start `qemu-system-arm` with the UART redirected to stdio.  It's also
possible to redirect the UART to a pty device using:

    $ make run

This will start the emulation and the name of the pty device will be printed to
stdout.  This serial device then be accessed via a serial terminal program,
for example `mpremote`:

    $ mpremote connect /dev/pts/1

You can disconnect and reconnect to the serial device multiple times.  Once you
are finished, stop the `make run` command by pressing Ctrl-C where that command
was started (or execute `machine.reset()` at the REPL).

The test suite can be run against the firmware by using the UART redirection.
You can either do this automatically using the single command:

    $ make test

Or manually by first starting the emulation with `make run` and then running the
tests against the serial device, for example:

    $ cd ../../tests
    $ ./run-tests.py --target qemu-arm --device /dev/pts/1
