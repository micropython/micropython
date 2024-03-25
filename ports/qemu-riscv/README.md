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

This port requires a newlib based bare metal/ELF RISC-V toolchain, either
with multilib support or 32 bits specific (M, C, and Zicsr extensions must
be supported, along with ilp32 ABI).

If your toolchain does not support the above requirements, either download
a pre-built toolchain (and that's a hit or miss situation in its own right)
or follow the instructions on [RISC-V's reference toolchain Github repo](https://github.com/riscv-collab/riscv-gnu-toolchain)
to get a newlib-based multilib toolchain (the target will be
`riscv64-unknown-elf` but said toolchain will be able to generate 32 bits
code as well).

That said, when in doubt, build your own toolchain - it's the fastest way to
get things going for sure.  To build toolchain that will work with this port
on a recent Linux installation after the necessary
[prerequisites](https://github.com/riscv-collab/riscv-gnu-toolchain#prerequisites)
are installed:

```bash
# Check out the bootstrap source code for the toolchain.
git clone -b 2023.10.18 https://github.com/riscv-collab/riscv-gnu-toolchain
cd riscv-gnu-toolchain
# Configure the toolchain.
# NOTE 1: The toolchain that will be built will only generate code for one
#         single ABI/Architecture combination, which is the one that this
#         port supports.  If you want a generic RISC-V toolchain, omit the
#         `--with-multilib-generator` parameter.
# NOTE 2: Pick a prefix that is writable by the user that is building the
#         toolchain, or installation will fail.
./configure \
  --with-multilib-generator="rv32imc_zicsr-ilp32--" \
  --prefix=$TARGETDIR \
  --enable-multilib
# Build the toolchain using all available CPU cores.
make -j `nproc`
# The toolchain is going to be available in $TARGETDIR, just remember to
# add $TARGETDIR/bin to the $PATH environment variable, or it won't be made
# available to the system.
```

To build and run image with builtin testsuite:

    make -f Makefile.test test

However, if your toolchain comes with picolibc rather than newlib (like the
one available on Ubuntu 22.04, where the libc headers are not shipped with
the toolchain), you must pass `PICOLIBC=1` to make and make sure the picolibc
headers are visible to the compiler.  The headers location can also be
provided via a compiler argument in the CFLAGS environment variable.  So, for
example, if you want to run the test suite on Ubuntu 22.04 where picolibc must
be explicitly installed and the headers are not visible to the compiler, your
command line should look like this:

```bash
CFLAGS="-I/usr/lib/picolibc/riscv64-unknown-elf/include" \
  make -f Makefile.test test PICOLIBC=1
```
