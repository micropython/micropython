/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Daniel Campora
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

#include <stdint.h>

#include "py/mpconfig.h"
#include MICROPY_HAL_H
#include "py/obj.h"
#include "py/runtime.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "rom_map.h"
#include "wdt.h"
#include "prcm.h"
#include "utils.h"
#include "pybwdt.h"
#include "mpexception.h"
#include "mperror.h"


/******************************************************************************
 DECLARE CONSTANTS
 ******************************************************************************/
#define PYBWDT_MILLISECONDS_TO_TICKS(ms)            ((80000000 / 1000) * (ms))
#define PYBWDT_MIN_TIMEOUT_MS                       (1000)

/******************************************************************************
 DECLARE TYPES
 ******************************************************************************/
typedef struct {
    bool    servers;
    bool    servers_sleeping;
    bool    simplelink;
    bool    running;
} pybwdt_data_t;

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
STATIC pybwdt_data_t   pybwdt_data = {.servers = false, .servers_sleeping = false, .simplelink = false, .running = false};
STATIC const mp_obj_base_t pyb_wdt_obj = {&pyb_wdt_type};

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
// must be called in main.c just after initializing the hal
__attribute__ ((section (".boot")))
void pybwdt_init0 (void) {
}

void pybwdt_srv_alive (void) {
    pybwdt_data.servers = true;
}

void pybwdt_srv_sleeping (bool state) {
    pybwdt_data.servers_sleeping = state;
}

void pybwdt_sl_alive (void) {
    pybwdt_data.simplelink = true;
}

/******************************************************************************/
// Micro Python bindings

/// \function constructor('msec')
/// Enables the watchdog timer with msec timeout value
STATIC mp_obj_t pyb_wdt_make_new (mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check the arguments
    mp_arg_check_num(n_args, n_kw, 0, 1, false);

    if (n_args > 0) {
        mp_int_t msec = mp_obj_get_int(args[0]);
        if (msec < PYBWDT_MIN_TIMEOUT_MS) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
        }
        if (pybwdt_data.running) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_request_not_possible));
        }

        // Enable the WDT peripheral clock
        MAP_PRCMPeripheralClkEnable(PRCM_WDT, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);

        // Unlock to be able to configure the registers
        MAP_WatchdogUnlock(WDT_BASE);

    #ifdef DEBUG
        // make the WDT stall when the debugger stops on a breakpoint
        MAP_WatchdogStallEnable (WDT_BASE);
    #endif

        // set the watchdog timer reload value
        // the WDT trigger a system reset after the second timeout
        // so, divide by 2 the timeout value received
        MAP_WatchdogReloadSet(WDT_BASE, PYBWDT_MILLISECONDS_TO_TICKS(msec / 2));

        // start the timer. Once it's started, it cannot be disabled.
        MAP_WatchdogEnable(WDT_BASE);
        pybwdt_data.running = true;
    }

    return (mp_obj_t)&pyb_wdt_obj;
}

/// \function wdt_kick()
/// Kicks the watchdog timer
STATIC mp_obj_t pyb_kick_wdt(mp_obj_t self) {
    if ((pybwdt_data.servers || pybwdt_data.servers_sleeping) && pybwdt_data.simplelink && pybwdt_data.running) {
        pybwdt_data.servers = false;
        pybwdt_data.simplelink = false;
        MAP_WatchdogIntClear(WDT_BASE);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_kick_wdt_obj, pyb_kick_wdt);

STATIC const mp_map_elem_t pybwdt_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_kick),   (mp_obj_t)&pyb_kick_wdt_obj },
};
STATIC MP_DEFINE_CONST_DICT(pybwdt_locals_dict, pybwdt_locals_dict_table);

const mp_obj_type_t pyb_wdt_type = {
    { &mp_type_type },
    .name = MP_QSTR_WDT,
    .make_new = pyb_wdt_make_new,
    .locals_dict = (mp_obj_t)&pybwdt_locals_dict,
};

