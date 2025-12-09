MicroPython port to qemu
========================

This is experimental, community-supported port for Cortex-M and RISC-V
RV32IMC/RV64IMC emulation as provided by QEMU (http://qemu.org).

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

Dependencies
------------

### ARM

For ARM-based boards the build requires a bare-metal ARM toolchain, such as
`arm-none-eabi-gcc`.

### RISC-V 32

For RV32-based boards the build requires a bare metal RISC-V toolchain with GCC 10
or later, either with multilib support or 32 bits specific (M, C, and Zicsr
extensions must be supported, along with ilp32 ABI).  Both newlib and picolibc are
supported, with the latter having precedence if found.

Most pre-built toolchains should work out of the box, either coming from your
Linux distribution's package manager, or independently packaged ones like
[xPack](https://xpack.github.io/dev-tools/riscv-none-elf-gcc/).

### RISC-V 64

For RV64-based boards the build requires a bare metal RISC-V toolchain with GCC 10
or later, either with multilib support or 64 bits specific (M, C, and Zicsr
extensions must be supported, along with lp64 ABI).  In other words, if your
compiler can build a binary that's compliant to any ratified RVA profiles,
you *might* be good to go: if you run your cross-compiler `gcc` binary with `-v` as
its only argument and you don't see mentions of `-mcmodel=medany`, your
toolchain is probably not going to work for this situation (more later).  Both newlib
and picolibc are supported, with the latter having precedence if found.

RISC-V 64 toolchains are a bit more temperamental.  Some toolchains have their `libc`,
`libm`, and `libgcc` libraries built in a way that they cannot be used with code
placed at an offset beyond 0x7FFFF7FF.  QEMU's `VIRT_RV64` board code address space
starts at 0x80000000, making those toolchains not viable if code from those libraries
is brought in by the build process.  A lot of readily available RISC-V toolchains are
unfortunately broken from this point of view.

However, to work around this, a Docker container file is provided to be able to run
tests using a known working compiler (the same one running MicroPython's RV32 and RV64
CI jobs).

To build the provided toolchain image you can use the following commands:

```bash
cd $MICROPYTHON_SOURCE_ROOT/ports/qemu
docker build -t micropython/mpy-qemu-riscv -f Dockerfile.riscv .
```

and then tests can be run with:

```bash
cd $MICROPYTHON_SOURCE_ROOT
docker container run -v .:/micropython --rm -it micropython/mpy-qemu-riscv make -C ports/qemu -- BOARD=VIRT_RV64 test
```

(If you use Podman, you can replace `docker` with `podman` in the commands above).

Also, if you replace `VIRT_RV64` with `VIRT_RV32` you should be able to run
32-bits tests as well!

Build instructions
------------------

First make sure the MicroPython cross-compiler is built (run from this directory):

    $ make -C ../../mpy-cross

Then build using:

    $ make

The default qemu-supported board is `mps2-an385`, a Cortex-M3 board.  To select a
different board pass the `BOARD` argument to `make`, for example:

    $ make BOARD=SABRELITE

Available boards are:

| Name for `BOARD=` | Architecture | Corresponding qemu board |
| ----------------- | ------------ | ------------------------ |
| `MICROBIT`        | `arm`        | `microbit`               |
| `MPS2_AN385`      | `arm`        | `mps2-an385`             |
| `MPS2_AN500`      | `arm`        | `mps2-an500`             |
| `NETDUINO2`       | `arm`        | `netduino2`              |
| `SABRELITE`       | `arm`        | `sabrelite`              |
| `VIRT_RV32`       | `riscv32`    | `virt`                   |
| `VIRT_RV64`       | `riscv64`    | `virt`                   |

Running
-------

When the firmware is run it will provide a REPL on the emulated hardware UART.
To access the REPL directly use:

    $ make repl

This will start `qemu-system-arm` (or `qemu-system-riscv32`) with the UART
redirected to stdio.  It's also possible to redirect the UART to a pty device
using:

    $ make run

This will start the emulation and the name of the pty device will be printed to
stdout.  This serial device then be accessed via a serial terminal program,
for example `mpremote`:

    $ mpremote connect /dev/pts/1

You can disconnect and reconnect to the serial device multiple times.  Once you
are finished, stop the `make run` command by pressing Ctrl-C where that command
was started (or execute `import machine; machine.reset()` at the REPL).

The test suite can be run against the firmware by using the UART redirection.
You can either do this automatically using the single command:

    $ make test

Or manually by first starting the emulation with `make run` and then running the
tests against the serial device, for example:

    $ cd ../../tests
    $ ./run-tests.py -t /dev/pts/1

Selected native modules that come as examples with the MicroPython source tree
can also be tested with this command (this is currently not supported for the
`VIRT_RV64` board):

    $ make test_natmod

The same remarks about manually running the tests apply for native modules, but
`run-natmodtests.py` should be run instead of `run-tests.py`.

Extra make options
------------------

The following options can be specified on the `make` command line:
- `CFLAGS_EXTRA`: pass in extra flags for the compiler.
- `RUN_TESTS_EXTRA`: pass in extra flags for `run-tests.py` and `run-natmodtests.py`
  when invoked via `make test` or `make test_natmod`.
- `QEMU_DEBUG=1`: when running qemu (via `repl`, `run` or `test` target), qemu
  will block until a debugger is connected.  By default it waits for a gdb connection
  on TCP port 1234.
- `QEMU_DEBUG_ARGS`: defaults to `-s` (gdb on TCP port 1234), but can be overridden
  with different qemu gdb arguments.
- `QEMU_DEBUG_EXTRA`: extra options to pass to qemu when `QEMU_DEBUG=1` is used.
- `MICROPY_HEAP_SIZE`: pass in an optional value (in bytes) for overriding the GC
  heap size used by the port.
