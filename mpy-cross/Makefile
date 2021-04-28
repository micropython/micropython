# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

# The following is a temporary hack to forefully undefine vars that might have
# be defined by a calling Makefile (from recursive make).
# TODO: Find a better way to be able to call this Makefile recursively.
ifneq ($(findstring undefine,$(.FEATURES)),)
override undefine COPT
override undefine CFLAGS_EXTRA
override undefine LDFLAGS_EXTRA
override undefine MICROPY_FORCE_32BIT
override undefine CROSS_COMPILE
override undefine FROZEN_DIR
override undefine FROZEN_MPY_DIR
override undefine USER_C_MODULES
override undefine SRC_MOD
override undefine BUILD
override undefine PROG
endif

include mpy-cross.mk
