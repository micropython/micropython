#!/usr/bin/env bash
set -e  # Stop on error

# Ensure ARM toolchain is installed
sudo apt-get update
sudo apt-get install -y gcc-arm-none-eabi libnewlib-arm-none-eabi

# Build mpy-cross (MicroPython bytecode compiler)
make -C mpy-cross -j"$(nproc)"

# Build firmware for Tiger_Plus
cd ports/stm32
make BOARD=Tiger_Plus submodules
make BOARD=Tiger_Plus -j"$(nproc)"

echo "Build complete: ports/stm32/build-Tiger_Plus/"
