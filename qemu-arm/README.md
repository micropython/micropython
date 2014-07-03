This is experimental, community-supported port for Cortex-M emulation as
provided by QEMU (http://qemu.org).

The purposes of this port are to enable:

1. Continuous integration
    - run tests agains architecture-specific parts of code base
2. Experimentation
    - simulation & prototyping of anything that has architecture-specific
      code
    - exploring instruction set in terms of optimising some part of
      MicroPython or a module
3. Streamlined debugging
    - no need for JTAG or even an MCU chip itself
    - no need to use OpenOCD or anything else that might slow down the
      process in terms of plugging things together, pressing buttons, etc.

This port will only work with with [GCC ARM Embedded](launchpad.net/gcc-arm-embedded)
toolchain and not with CodeSourcery toolchain. You will need to modify
`LDFLAGS` if you want to use CodeSourcery's version of `arm-none-eabi`.
The difference is that CodeSourcery needs `-T generic-m-hosted.ld` while
ARM's version  requires `--specs=nano.specs --specs=rdimon.specs` to be
passed to the linker.
