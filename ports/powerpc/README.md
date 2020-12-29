# The PowerPC port that runs on microwatt and qemu

This port is intended to be a minimal MicroPython port that runs in
QEMU, microwatt simulator with ghdl or microwatt on Xilinx FPGA with
potato UART.

## Building

By default the port will be built for the host machine:

    $ make

## Cross compilation for POWERPC

If you need to cross compilers you'll want to grab a powerpc64le
compiler (not powerpc or powerpc64).

On Ubuntu (18.04) you'll want:

    $ apt install gcc-powerpc64le-linux-gnu

*(Use CROSS_COMPILE=powerpc64le-linux-gnu-)*

If your distro doesn't have cross compilers, you can get cross compilers here:
- https://toolchains.bootlin.com/
*(use CROSS_COMPILE=powerpc64le-buildroot-linux-gnu-)*

(Avoid musl libc as it defines __assert_fail() differently to glibc
which breaks the micropython powerpc code)

Then do:

    $ make CROSS_COMPILE=<compiler prefix>

Building will produce the build/firmware.bin file which can be used
QEMU or [microwatt](https://github.com/antonblanchard/microwatt).

To run in QEMU use:

    $ ./qemu-system-ppc64 -M powernv -cpu POWER9 -nographic -bios build/firmware.bin
