/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2023 Damien P. George
 * Copyright (c) 2020 Jim Mussared
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
// extmod/modmachine.c via MICROPY_PY_MACHINE_INCLUDEFILE.

#include "led.h"
#include "pin.h"
#include "modmachine.h"
#include "fsl_gpc.h"
#ifdef MIMXRT117x_SERIES
#include "fsl_soc_src.h"
#else
#include "fsl_src.h"
#endif
#include "fsl_wdog.h"
#if FSL_FEATURE_BOOT_ROM_HAS_ROMAPI
#include "fsl_romapi.h"
#endif

#include CPU_HEADER_H

#if defined(MICROPY_HW_LED1_PIN)
#define MICROPY_PY_MACHINE_LED_ENTRY { MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&machine_led_type) },
#else
#define MICROPY_PY_MACHINE_LED_ENTRY
#endif

#if MICROPY_PY_MACHINE_SDCARD
#define MICROPY_PY_MACHINE_SDCARD_ENTRY { MP_ROM_QSTR(MP_QSTR_SDCard), MP_ROM_PTR(&machine_sdcard_type) },
#else
#define MICROPY_PY_MACHINE_SDCARD_ENTRY
#endif

#define MICROPY_PY_MACHINE_EXTRA_GLOBALS \
    MICROPY_PY_MACHINE_LED_ENTRY \
    { MP_ROM_QSTR(MP_QSTR_Pin),                 MP_ROM_PTR(&machine_pin_type) }, \
    { MP_ROM_QSTR(MP_QSTR_Timer),               MP_ROM_PTR(&machine_timer_type) }, \
    { MP_ROM_QSTR(MP_QSTR_RTC),                 MP_ROM_PTR(&machine_rtc_type) }, \
    MICROPY_PY_MACHINE_SDCARD_ENTRY \
    \
    /* Reset reasons */ \
    { MP_ROM_QSTR(MP_QSTR_PWRON_RESET),         MP_ROM_INT(MP_PWRON_RESET) }, \
    { MP_ROM_QSTR(MP_QSTR_WDT_RESET),           MP_ROM_INT(MP_WDT_RESET) }, \
    { MP_ROM_QSTR(MP_QSTR_SOFT_RESET),          MP_ROM_INT(MP_SOFT_RESET) }, \

typedef enum {
    MP_PWRON_RESET = 1,
    MP_HARD_RESET,
    MP_WDT_RESET,
    MP_DEEPSLEEP_RESET,
    MP_SOFT_RESET
} reset_reason_t;

// Copied from inc/uf2.h in https://github.com/Microsoft/uf2-samd21
#define DBL_TAP_REG   SNVS->LPGPR[3]
#define DBL_TAP_MAGIC 0xf01669ef // Randomly selected, adjusted to have first and last bit set
#define DBL_TAP_MAGIC_QUICK_BOOT 0xf02669ef

static mp_obj_t mp_machine_unique_id(void) {
    unsigned char id[8];
    mp_hal_get_unique_id(id);
    return mp_obj_new_bytes(id, sizeof(id));
}

MP_NORETURN static void mp_machine_reset(void) {
    WDOG_TriggerSystemSoftwareReset(WDOG1);
    while (true) {
        ;
    }
}

static mp_int_t mp_machine_reset_cause(void) {
    #ifdef MIMXRT117x_SERIES
    uint32_t user_reset_flag = kSRC_M7CoreIppUserResetFlag;
    #else
    uint32_t user_reset_flag = kSRC_IppUserResetFlag;
    #endif
    if (SRC->SRSR & user_reset_flag) {
        return MP_DEEPSLEEP_RESET;
    }
    uint16_t reset_cause =
        WDOG_GetStatusFlags(WDOG1) & (kWDOG_PowerOnResetFlag | kWDOG_TimeoutResetFlag | kWDOG_SoftwareResetFlag);
    if (reset_cause == kWDOG_PowerOnResetFlag) {
        reset_cause = MP_PWRON_RESET;
    } else if (reset_cause == kWDOG_TimeoutResetFlag) {
        reset_cause = MP_WDT_RESET;
    } else {
        reset_cause = MP_SOFT_RESET;
    }
    return reset_cause;
}

static mp_obj_t mp_machine_get_freq(void) {
    return MP_OBJ_NEW_SMALL_INT(mp_hal_get_cpu_freq());
}

static void mp_machine_set_freq(size_t n_args, const mp_obj_t *args) {
    mp_raise_NotImplementedError(NULL);
}

static void mp_machine_idle(void) {
    MICROPY_EVENT_POLL_HOOK;
}

static void mp_machine_lightsleep(size_t n_args, const mp_obj_t *args) {
    mp_raise_NotImplementedError(NULL);
}

MP_NORETURN static void mp_machine_deepsleep(size_t n_args, const mp_obj_t *args) {
    if (n_args != 0) {
        mp_int_t seconds = mp_obj_get_int(args[0]) / 1000;
        if (seconds > 0) {
            machine_rtc_alarm_helper(seconds, false);
            #ifdef MIMXRT117x_SERIES
            GPC_CM_EnableIrqWakeup(GPC_CPU_MODE_CTRL_0, SNVS_HP_NON_TZ_IRQn, true);
            #else
            GPC_EnableIRQ(GPC, SNVS_HP_WRAPPER_IRQn);
            #endif
        }
    }

    #if defined(MIMXRT117x_SERIES)
    machine_pin_config(pin_WAKEUP_DIG, PIN_MODE_IT_RISING, PIN_PULL_DISABLED, PIN_DRIVE_OFF, 0, PIN_AF_MODE_ALT5);
    GPC_CM_EnableIrqWakeup(GPC_CPU_MODE_CTRL_0, GPIO13_Combined_0_31_IRQn, true);
    #elif defined(pin_WAKEUP)
    machine_pin_config(pin_WAKEUP, PIN_MODE_IT_RISING, PIN_PULL_DISABLED, PIN_DRIVE_OFF, 0, PIN_AF_MODE_ALT5);
    GPC_EnableIRQ(GPC, GPIO5_Combined_0_15_IRQn);
    #endif

    SNVS->LPCR |= SNVS_LPCR_TOP_MASK;

    while (true) {
        ;
    }
}

MP_NORETURN void mp_machine_bootloader(size_t n_args, const mp_obj_t *args) {
    #if defined(MICROPY_BOARD_ENTER_BOOTLOADER)
    // If a board has a custom bootloader, call it first.
    MICROPY_BOARD_ENTER_BOOTLOADER(n_args, args);
    #elif FSL_ROM_HAS_RUNBOOTLOADER_API && !MICROPY_MACHINE_UF2_BOOTLOADER
    // If not, enter ROM bootloader in serial downloader / USB mode.
    // Skip that in case of the UF2 bootloader being available.
    uint32_t arg = 0xEB110000;
    ROM_RunBootloader(&arg);
    #else
    // No custom bootloader, or run bootloader API, the set
    // the flag for the UF2 bootloader
    // Pretend to be the first of the two reset presses needed to enter the
    // bootloader. That way one reset will end in the bootloader.
    DBL_TAP_REG = DBL_TAP_MAGIC;
    WDOG_TriggerSystemSoftwareReset(WDOG1);
    #endif
    while (1) {
        ;
    }
}
