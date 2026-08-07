# cmake configuration for PKE8721DAF (RTL8721DAF / AmebaDplus)
# This file is included by src/CMakeLists.txt when BOARD=PKE8721DAF.

# SoC name passed to ameba.py build <SOC> — read by the port Makefile.
set(AMEBA_BOARD_SOC "RTL8721Dx")

# Board/MCU name macros and pin assignments now come from
# boards/<BOARD>/mpconfigboard.h, which mpconfigport.h includes directly.
