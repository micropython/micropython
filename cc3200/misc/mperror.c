/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include <std.h>
#include <stdint.h>
#include <string.h>

#include "py/mpconfig.h"
#include MICROPY_HAL_H
#include "py/obj.h"
#include "hw_ints.h"
#include "hw_types.h"
#include "hw_gpio.h"
#include "hw_memmap.h"
#include "hw_gprcm.h"
#include "hw_common_reg.h"
#include "pin.h"
#include "gpio.h"
#include "rom.h"
#include "rom_map.h"
#include "prcm.h"
#include "pybuart.h"
#include "utils.h"
#include "mperror.h"


/******************************************************************************
 DEFINE CONSTANTS
 ******************************************************************************/
#define MPERROR_TOOGLE_MS                           (200)
#define MPERROR_SIGNAL_ERROR_MS                     (2000)
#define MPERROR_HEARTBEAT_ON_MS                     (80)
#define MPERROR_HEARTBEAT_OFF_MS                    (2920)

#define MPERROR_SAFE_BOOT_REG_IDX                   (0)

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
static bool mperror_heartbeat_enabled;

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
void mperror_init0 (void) {
    // Enable SYS GPIOs peripheral clocks
    MAP_PRCMPeripheralClkEnable(MICROPY_SYS_LED_PRCM, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);
#ifdef BOOTLOADER
    MAP_PRCMPeripheralClkEnable(MICROPY_SAFE_BOOT_PRCM, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);
#endif

    // Configure the bld
    MAP_PinTypeGPIO(MICROPY_SYS_LED_PIN_NUM, PIN_MODE_0, false);
    MAP_PinConfigSet(MICROPY_SYS_LED_PIN_NUM, PIN_STRENGTH_6MA, PIN_TYPE_STD);
    MAP_GPIODirModeSet(MICROPY_SYS_LED_PORT, MICROPY_SYS_LED_PORT_PIN, GPIO_DIR_MODE_OUT);

#ifdef BOOTLOADER
    // Configure the safe boot pin
    MAP_PinTypeGPIO(MICROPY_SAFE_BOOT_PIN_NUM, PIN_MODE_0, false);
    MAP_PinConfigSet(MICROPY_SAFE_BOOT_PIN_NUM, PIN_STRENGTH_4MA, PIN_TYPE_STD_PD);
    MAP_GPIODirModeSet(MICROPY_SAFE_BOOT_PORT, MICROPY_SAFE_BOOT_PORT_PIN, GPIO_DIR_MODE_IN);
#endif
}

void mperror_deinit_sfe_pin (void) {
    // disable the pull-down
    MAP_PinConfigSet(MICROPY_SAFE_BOOT_PIN_NUM, PIN_STRENGTH_4MA, PIN_TYPE_STD);
}

void mperror_signal_error (void) {
    uint32_t count = 0;
    while ((MPERROR_TOOGLE_MS * count++) > MPERROR_SIGNAL_ERROR_MS) {
        // toogle the led
        MAP_GPIOPinWrite(MICROPY_SYS_LED_PORT, MICROPY_SYS_LED_PORT_PIN, ~MAP_GPIOPinRead(MICROPY_SYS_LED_PORT, MICROPY_SYS_LED_PORT_PIN));
        UtilsDelay(UTILS_DELAY_US_TO_COUNT(MPERROR_TOOGLE_MS * 1000));
    }
}

void mperror_request_safe_boot (void) {
    MAP_PRCMOCRRegisterWrite(MPERROR_SAFE_BOOT_REG_IDX, 1);
}

void mperror_clear_safe_boot (void) {
    MAP_PRCMOCRRegisterWrite(MPERROR_SAFE_BOOT_REG_IDX, 0);
}

// returns the last state of the safe boot request and clears the register
bool mperror_safe_boot_requested (void) {
    bool ret = MAP_PRCMOCRRegisterRead(MPERROR_SAFE_BOOT_REG_IDX);
    mperror_clear_safe_boot();
    return ret;
}

void mperror_enable_heartbeat (void) {
    mperror_heartbeat_enabled = true;
}

void mperror_disable_heartbeat (void) {
    mperror_heartbeat_enabled = false;
    mperror_heartbeat_off();
}

void mperror_heartbeat_signal (void) {
    static uint off_start = 0;
    static uint on_start = 0;
    static bool beat = false;

    if (mperror_heartbeat_enabled) {
        if (!beat) {
            if ((on_start = HAL_GetTick()) - off_start > MPERROR_HEARTBEAT_OFF_MS) {
                MAP_GPIOPinWrite(MICROPY_SYS_LED_PORT, MICROPY_SYS_LED_PORT_PIN, MICROPY_SYS_LED_PORT_PIN);
                beat = true;
            }
        }
        else {
            if ((off_start = HAL_GetTick()) - on_start > MPERROR_HEARTBEAT_ON_MS) {
                mperror_heartbeat_off();
                beat = false;
            }
        }
    }
}

void mperror_heartbeat_off (void) {
    MAP_GPIOPinWrite(MICROPY_SYS_LED_PORT, MICROPY_SYS_LED_PORT_PIN, 0);
}

void NORETURN __fatal_error(const char *msg) {
#ifdef DEBUG
    if (msg != NULL) {
        // wait for 20ms
        UtilsDelay(UTILS_DELAY_US_TO_COUNT(20000));
        mp_hal_stdout_tx_str("\r\nFATAL ERROR:");
        mp_hal_stdout_tx_str(msg);
        mp_hal_stdout_tx_str("\r\n");
    }
#endif
    // signal the crash with the system led
    MAP_GPIOPinWrite(MICROPY_SYS_LED_PORT, MICROPY_SYS_LED_PORT_PIN, MICROPY_SYS_LED_PORT_PIN);
    for ( ;; ) {__WFI();}
}

void __assert_func(const char *file, int line, const char *func, const char *expr) {
    (void) func;
    printf("Assertion failed: %s, file %s, line %d\n", expr, file, line);
    __fatal_error(NULL);
}

void nlr_jump_fail(void *val) {
#ifdef DEBUG
    char msg[64];
    snprintf(msg, sizeof(msg), "uncaught exception %p\n", val);
    __fatal_error(msg);
#else
    __fatal_error(NULL);
#endif
}
