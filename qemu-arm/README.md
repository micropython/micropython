The purposes of this port had been enable:
  1. continuos integration
    - run test agains architecture specific part of code base
  2. experimentation
    - simulation & prototyping of anything that has architecture-specific code
    - exploring instruction set in terms of optimising some part of uPy or a
  module
  3. speedy debugging
    - no need for JTAG or even the chip itself
    - no need to use OpenOCD or anything else the might slow down the process in terms of plugging things together and pressing buttons etc

This port will only work with with [GCC ARM Embedded](launchpad.net/gcc-arm-embedded) toolchain and not with CodeSourcery toolchain. You will need to modify `LDFLAGS` if you want to use CodeSourcery's version of `arm-none-eabi`. The difference is that CodeSourcery needs `-T generic-m-hosted.ld` while ARM's version  requires `--specs=nano.specs --specs=rdimon.specs` to be passed to the linker.
