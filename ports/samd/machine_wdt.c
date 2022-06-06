/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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

#include "py/runtime.h"
#include "modmachine.h"

#include "sam.h"

#define MIN_TIMEOUT         512
#define MAX_TIMEOUT         16384

typedef struct _machine_wdt_obj_t {
    mp_obj_base_t base;
} machine_wdt_obj_t;

extern mp_int_t log2i(mp_int_t num);

STATIC const machine_wdt_obj_t machine_wdt = {{&machine_wdt_type}};

STATIC void set_timeout(uint32_t timeout) {
    // Set new timeout. Have to disable WDT first.

    // Confine to the valid range
    if (timeout < MIN_TIMEOUT) {
        timeout = MIN_TIMEOUT;
    } else if (timeout > MAX_TIMEOUT) {
        timeout = MAX_TIMEOUT;
    }

    #if defined(MCU_SAMD21)
    WDT->CTRL.reg = 0;
    while (WDT->STATUS.reg & WDT_STATUS_SYNCBUSY) {
    }
    WDT->CONFIG.reg = log2i(timeout) - 3;
    WDT->CTRL.reg = WDT_CTRL_ENABLE;
    while (WDT->STATUS.reg & WDT_STATUS_SYNCBUSY) {
    }
    #elif defined(MCU_SAMD51)
    WDT->CTRLA.reg = 0;
    while (WDT->SYNCBUSY.reg & WDT_SYNCBUSY_ENABLE) {
    }
    WDT->CONFIG.reg = log2i(timeout) - 3;
    WDT->CTRLA.reg = WDT_CTRLA_ENABLE;
    while (WDT->SYNCBUSY.reg & WDT_SYNCBUSY_ENABLE) {
    }
    #endif
}

STATIC mp_obj_t machine_wdt_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_timeout, ARG_lock };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_timeout, MP_ARG_INT, {.u_int = 5000} },
    };

    // Parse the arguments.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    #if defined(MCU_SAMD51)
    // Verify the WDT id. SAMD51 only, saving a few bytes for SAMD21
    mp_int_t id = args[ARG_id].u_int;
    if (id != 0) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("WDT(%d) doesn't exist"), id);
    }
    #endif

    // Start the watchdog (timeout is in milliseconds).
    uint32_t timeout = args[ARG_timeout].u_int;
    // Configure the WDT
    #if defined(MCU_SAMD21)

    // Enable APBx clocks and GCLK clock
    PM->APBAMASK.reg |= PM_APBAMASK_WDT;
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK8 | GCLK_CLKCTRL_ID_WDT;

    #elif defined(MCU_SAMD51)

    // Enable APBx clocks and 1kHz clock
    MCLK->APBAMASK.reg |= MCLK_APBAMASK_WDT;
    OSC32KCTRL->OSCULP32K.bit.EN1K = 1;

    #endif

    set_timeout(timeout);

    return MP_OBJ_FROM_PTR(&machine_wdt);
}

STATIC mp_obj_t machine_wdt_feed(mp_obj_t self_in) {
    (void)self_in;
    WDT->CLEAR.reg = 0xa5;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_wdt_feed_obj, machine_wdt_feed);

STATIC mp_obj_t machine_wdt_timeout_ms(mp_obj_t self_in, mp_obj_t timout_in) {
    uint32_t timeout = mp_obj_get_int(timout_in);

    set_timeout(timeout);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_wdt_timeout_ms_obj, machine_wdt_timeout_ms);

STATIC const mp_rom_map_elem_t machine_wdt_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_feed), MP_ROM_PTR(&machine_wdt_feed_obj) },
    { MP_ROM_QSTR(MP_QSTR_timeout_ms), MP_ROM_PTR(&machine_wdt_timeout_ms_obj) },
};
STATIC MP_DEFINE_CONST_DICT(machine_wdt_locals_dict, machine_wdt_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_wdt_type,
    MP_QSTR_WDT,
    MP_TYPE_FLAG_NONE,
    make_new, machine_wdt_make_new,
    locals_dict, &machine_wdt_locals_dict
    );
