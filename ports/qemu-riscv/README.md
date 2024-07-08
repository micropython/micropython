This is an experimental, community-supported port for RISC-V RV32IMC emulation
as provided by QEMU (http://qemu.org).

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

This port requires a bare metal RISC-V toolchain with GCC 10 or later, either
with multilib support or 32 bits specific (M, C, and Zicsr extensions must be
supported, along with ilp32 ABI).  Both newlib and picolibc are supported,
with the latter having precedence if found.

Most pre-built toolchains should work out of the box, either coming from your
Linux distribution's package manager, or independently packaged ones like
[xPack](https://xpack.github.io/dev-tools/riscv-none-elf-gcc/).

To build and run the image with builtin testsuite:

    make -f Makefile.test test
