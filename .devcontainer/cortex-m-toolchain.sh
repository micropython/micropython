#!/bin/bash
# -----------------------------------------------------------------------------
# cortex-m-toolchain.sh: install toolchain for CircuitPython
#
# Author: Bernhard Bablok
#
# -----------------------------------------------------------------------------

echo -e "[cortex-m-toolchain.sh] starting install"

# --- tooling   --------------------------------------------------------------

echo -e "[cortex-m-toolchain.sh] downloading and installing gcc-arm-non-eabi toolchain"
cd /workspaces

wget -qO gcc-arm-none-eabi.tar.xz \
  https://developer.arm.com/-/media/Files/downloads/gnu/13.2.rel1/binrel/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz

tar -xJf gcc-arm-none-eabi.tar.xz
ln -s arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi gcc-arm-none-eabi
rm -f gcc-arm-none-eabi.tar.xz

echo -e "[cortex-m-toolchain.sh] update PATH in environment"
echo -e "\nexport PATH=/workspaces/gcc-arm-none-eabi/bin:$PATH"  >> $HOME/.bashrc
