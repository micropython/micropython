// SPDX-FileCopyrightText: Copyright (c) 2013-2014 Damien P. George
// SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
//
// SPDX-License-Identifier: MIT

#include <stdio.h>

#include "py/mpstate.h"
#include "py/gc.h"

#include "lib/utils/gchelper.h"

#if MICROPY_ENABLE_GC

void gc_collect(void) {
    gc_collect_start();
    gc_helper_collect_regs_and_stack();
    gc_collect_end();
}

#endif // MICROPY_ENABLE_GC
