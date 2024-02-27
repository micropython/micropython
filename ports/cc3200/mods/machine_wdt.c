/*
 * This file is part of the MicroPython project, http://micropython.org/
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

// This file is never compiled standalone, it's included directly from
// extmod/machine_wdt.c via MICROPY_PY_MACHINE_WDT_INCLUDEFILE.

#include "py/mperrno.h"
#include "py/mphal.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "rom_map.h"
#include "wdt.h"
#include "prcm.h"
#include "utils.h"
#include "pybwdt.h"

/******************************************************************************
 DECLARE CONSTANTS
 ******************************************************************************/
#define PYBWDT_MILLISECONDS_TO_TICKS(ms)            ((80000000 / 1000) * (ms))
#define PYBWDT_MIN_TIMEOUT_MS                       (1000)

/******************************************************************************
 DECLARE TYPES
 ******************************************************************************/
typedef struct _machine_wdt_obj_t {
    mp_obj_base_t base;
    bool    servers;
    bool    servers_sleeping;
    bool    simplelink;
    bool    running;
} machine_wdt_obj_t;

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
static machine_wdt_obj_t machine_wdt_obj = {.servers = false, .servers_sleeping = false, .simplelink = false, .running = false};

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
// must be called in main.c just after initializing the hal
__attribute__ ((section (".boot")))
void pybwdt_init0 (void) {
}

void pybwdt_srv_alive (void) {
    machine_wdt_obj.servers = true;
}

void pybwdt_srv_sleeping (bool state) {
    machine_wdt_obj.servers_sleeping = state;
}

void pybwdt_sl_alive (void) {
    machine_wdt_obj.simplelink = true;
}

/******************************************************************************/
// MicroPython bindings

static machine_wdt_obj_t *mp_machine_wdt_make_new_instance(mp_int_t id, mp_int_t timeout_ms) {
    if (id != 0) {
        mp_raise_OSError(MP_ENODEV);
    }
    if (timeout_ms < PYBWDT_MIN_TIMEOUT_MS) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid argument(s) value"));
    }
    if (machine_wdt_obj.running) {
        mp_raise_OSError(MP_EPERM);
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
    MAP_WatchdogReloadSet(WDT_BASE, PYBWDT_MILLISECONDS_TO_TICKS(timeout_ms / 2));

    // start the timer. Once it's started, it cannot be disabled.
    MAP_WatchdogEnable(WDT_BASE);
    machine_wdt_obj.base.type = &machine_wdt_type;
    machine_wdt_obj.running = true;

    return &machine_wdt_obj;
}

static void mp_machine_wdt_feed(machine_wdt_obj_t *self) {
    if ((self->servers || self->servers_sleeping) && self->simplelink && self->running) {
        self->servers = false;
        self->simplelink = false;
        MAP_WatchdogIntClear(WDT_BASE);
    }
}
