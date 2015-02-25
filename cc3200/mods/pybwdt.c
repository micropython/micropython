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


/******************************************************************************
 DECLARE CONSTANTS
 ******************************************************************************/
#define PYBWDT_MILLISECONDS_TO_TICKS(ms)            ((80000000 / 1000) * (ms))
#define PYBWDT_MIN_TIMEOUT_MS                       (500)

/******************************************************************************
 DECLARE TYPES
 ******************************************************************************/
typedef struct {
    bool    servers;
    bool    simplelink;
    bool    running;
}pybwdt_data_t;

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
static  pybwdt_data_t   pybwdt_data;

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
// must be called in main.c just after initializing the hal
void pybwdt_init0 (void) {
    pybwdt_data.running = false;
}

void pybwdt_check_reset_cause (void) {
    // if we are recovering from a WDT reset, trigger
    // a hibernate cycle for a clean boot
    if (MAP_PRCMSysResetCauseGet() == PRCM_WDT_RESET) {
        HWREG(0x400F70B8) = 1;
        UtilsDelay(800000/5);
        HWREG(0x400F70B0) = 1;
        UtilsDelay(800000/5);

        HWREG(0x4402E16C) |= 0x2;
        UtilsDelay(800);
        HWREG(0x4402F024) &= 0xF7FFFFFF;

        MAP_PRCMHibernateWakeupSourceEnable(PRCM_HIB_SLOW_CLK_CTR);
        // set the sleep interval to 10ms
        MAP_PRCMHibernateIntervalSet(330);
        MAP_PRCMHibernateEnter();
    }
}

// minimum timeout value is 500ms
pybwdt_ret_code_t pybwdt_enable (uint32_t timeout) {
    if (timeout >= PYBWDT_MIN_TIMEOUT_MS) {
        if (!pybwdt_data.running) {
            // Enable the WDT peripheral clock
            MAP_PRCMPeripheralClkEnable(PRCM_WDT, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);

            // Unlock to be able to configure the registers
            MAP_WatchdogUnlock(WDT_BASE);

            // Make the WDT stall when the debugger stops on a breakpoint
            MAP_WatchdogStallEnable (WDT_BASE);

            // Set the watchdog timer reload value
            // the WDT trigger a system reset after the second timeout
            // so, divide by the 2 timeout value received
            MAP_WatchdogReloadSet(WDT_BASE, PYBWDT_MILLISECONDS_TO_TICKS(timeout / 2));

            // Start the timer. Once the timer is started, it cannot be disabled.
            MAP_WatchdogEnable(WDT_BASE);
            pybwdt_data.running = true;

            return E_PYBWDT_OK;
        }
        return E_PYBWDT_IS_RUNNING;
    }
    return E_PYBWDT_INVALID_TIMEOUT;
}

void pybwdt_kick (void) {
    // check that the servers and simplelink are running fine
    if (pybwdt_data.servers && pybwdt_data.simplelink && pybwdt_data.running) {
        pybwdt_data.servers = false;
        pybwdt_data.simplelink = false;
        MAP_WatchdogIntClear(WDT_BASE);
    }
}

void pybwdt_srv_alive (void) {
    pybwdt_data.servers = true;
}

void pybwdt_sl_alive (void) {
    pybwdt_data.simplelink = true;
}
