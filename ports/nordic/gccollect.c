// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Glenn Ruben Bakke
//
// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <stdint.h>

#include "py/obj.h"
#include "py/gc.h"
#include "gccollect.h"

static inline uint32_t get_msp(void) {
    register uint32_t result;
    __asm volatile ("MRS %0, msp\n" : "=r" (result));
    return result;
}

void gc_collect(void) {
    // start the GC
    gc_collect_start();

    mp_uint_t sp = get_msp(); // Get stack pointer

    // trace the stack, including the registers (since they live on the stack in this function)
    gc_collect_root((void **)sp, ((uint32_t)&_ram_end - sp) / sizeof(uint32_t));

    // end the GC
    gc_collect_end();
}
