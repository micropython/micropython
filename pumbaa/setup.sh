#
# Setup script for the Pumbaa development environment. Source this file
# in your shell to setup the environment.
#

# ARM compiler fails on Cygwin if files have absolute paths.
if [ "${OSTYPE}" != "cygwin" ]; then
    export PUMBAA_ROOT=$(readlink -f .)
    export SIMBA_ROOT=$(readlink -f simba)
fi

export PATH=${PATH}:$(readlink -f simba/bin)

# ESP8266 toolchain
export PATH=$PATH:$(readlink -f ../../esp-open-sdk/xtensa-lx106-elf/bin)

# ARM toolchain for Cygwin.
export PATH=$PATH:$(readlink -f ../arm-toolchain-windows/arm-none-eabi-gcc/4.8.3-2014q1/bin)
export PATH=$PATH:$(readlink -f ../arm-toolchain-windows/bossac/1.3a-arduino)
