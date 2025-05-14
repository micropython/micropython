/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2023 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// This file is never compiled standalone, it's included directly from
// extmod/machine_wdt.c via MICROPY_PY_MACHINE_WDT_INCLUDEFILE.

#include "py/mphal.h"

#if defined(STM32H7)
#define IWDG (IWDG1)
#endif

typedef struct _machine_wdt_obj_t {
    mp_obj_base_t base;
} machine_wdt_obj_t;

static const machine_wdt_obj_t machine_wdt = {{&machine_wdt_type}};

static machine_wdt_obj_t *mp_machine_wdt_make_new_instance(mp_int_t id, mp_int_t timeout_ms) {
    if (id != 0) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("WDT(%d) doesn't exist"), id);
    }

    // compute prescaler
    int32_t timeout = timeout_ms;
    uint32_t prescaler;
    for (prescaler = 0; prescaler < 6 && timeout >= 512; ++prescaler, timeout /= 2) {
    }

    // convert milliseconds to ticks
    timeout *= 8; // 32kHz / 4 = 8 ticks per millisecond (approx)
    if (timeout <= 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("WDT timeout too short"));
    } else if (timeout > 0xfff) {
        mp_raise_ValueError(MP_ERROR_TEXT("WDT timeout too long"));
    }
    timeout -= 1;

    // set the reload register
    while (IWDG->SR & 2) {
    }
    IWDG->KR = 0x5555;
    IWDG->RLR = timeout;

    // set the prescaler
    while (IWDG->SR & 1) {
    }
    IWDG->KR = 0x5555;
    IWDG->PR = prescaler;

    // start the watch dog
    IWDG->KR = 0xcccc;

    return (machine_wdt_obj_t *)&machine_wdt;
}

static void mp_machine_wdt_feed(machine_wdt_obj_t *self) {
    (void)self;
    IWDG->KR = 0xaaaa;
}
