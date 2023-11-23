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
    { MP_ROM_QSTR(MP_QSTR_unique_id),           MP_ROM_PTR(&machine_unique_id_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_reset),               MP_ROM_PTR(&machine_reset_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_reset_cause),         MP_ROM_PTR(&machine_reset_cause_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_freq),                MP_ROM_PTR(&machine_freq_obj) }, \
    MICROPY_PY_MACHINE_LED_ENTRY \
    { MP_ROM_QSTR(MP_QSTR_Pin),                 MP_ROM_PTR(&machine_pin_type) }, \
    { MP_ROM_QSTR(MP_QSTR_Timer),               MP_ROM_PTR(&machine_timer_type) }, \
    { MP_ROM_QSTR(MP_QSTR_RTC),                 MP_ROM_PTR(&machine_rtc_type) }, \
    MICROPY_PY_MACHINE_SDCARD_ENTRY \
    \
    { MP_ROM_QSTR(MP_QSTR_deepsleep),           MP_ROM_PTR(&machine_deepsleep_obj) }, \
    \
    { MP_ROM_QSTR(MP_QSTR_disable_irq),         MP_ROM_PTR(&machine_disable_irq_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_enable_irq),          MP_ROM_PTR(&machine_enable_irq_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_bootloader),          MP_ROM_PTR(&machine_bootloader_obj) }, \
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

STATIC mp_obj_t machine_unique_id(void) {
    unsigned char id[8];
    mp_hal_get_unique_id(id);
    return mp_obj_new_bytes(id, sizeof(id));
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_unique_id_obj, machine_unique_id);

STATIC mp_obj_t machine_reset(void) {
    WDOG_TriggerSystemSoftwareReset(WDOG1);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_obj, machine_reset);

STATIC mp_obj_t machine_reset_cause(void) {
    #ifdef MIMXRT117x_SERIES
    uint32_t user_reset_flag = kSRC_M7CoreIppUserResetFlag;
    #else
    uint32_t user_reset_flag = kSRC_IppUserResetFlag;
    #endif
    if (SRC->SRSR & user_reset_flag) {
        return MP_OBJ_NEW_SMALL_INT(MP_DEEPSLEEP_RESET);
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
    return MP_OBJ_NEW_SMALL_INT(reset_cause);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_cause_obj, machine_reset_cause);

STATIC mp_obj_t machine_freq(void) {
    return MP_OBJ_NEW_SMALL_INT(mp_hal_get_cpu_freq());
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_freq_obj, machine_freq);

STATIC void mp_machine_idle(void) {
    MICROPY_EVENT_POLL_HOOK;
}

STATIC mp_obj_t machine_deepsleep(size_t n_args, const mp_obj_t *args) {
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

    #ifdef MIMXRT117x_SERIES
    machine_pin_config(pin_WAKEUP_DIG, PIN_MODE_IT_RISING, PIN_PULL_DISABLED, PIN_DRIVE_OFF, 0, PIN_AF_MODE_ALT5);
    GPC_CM_EnableIrqWakeup(GPC_CPU_MODE_CTRL_0, GPIO13_Combined_0_31_IRQn, true);
    #elif defined IOMUXC_SNVS_WAKEUP_GPIO5_IO00
    machine_pin_config(pin_WAKEUP, PIN_MODE_IT_RISING, PIN_PULL_DISABLED, PIN_DRIVE_OFF, 0, PIN_AF_MODE_ALT5);
    GPC_EnableIRQ(GPC, GPIO5_Combined_0_15_IRQn);
    #endif

    SNVS->LPCR |= SNVS_LPCR_TOP_MASK;

    while (true) {
        ;
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_deepsleep_obj, 0, 1, machine_deepsleep);

STATIC mp_obj_t machine_disable_irq(void) {
    uint32_t state = MICROPY_BEGIN_ATOMIC_SECTION();
    return mp_obj_new_int(state);
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_disable_irq_obj, machine_disable_irq);

STATIC mp_obj_t machine_enable_irq(mp_obj_t state_in) {
    uint32_t state = mp_obj_get_int(state_in);
    MICROPY_END_ATOMIC_SECTION(state);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_enable_irq_obj, machine_enable_irq);

NORETURN mp_obj_t machine_bootloader(size_t n_args, const mp_obj_t *args) {
    #if defined(MICROPY_BOARD_ENTER_BOOTLOADER)
    // If a board has a custom bootloader, call it first.
    MICROPY_BOARD_ENTER_BOOTLOADER(n_args, args);
    #elif FSL_ROM_HAS_RUNBOOTLOADER_API
    // If not, enter ROM bootloader in serial downloader / USB mode.
    uint32_t arg = 0xEB110000;
    ROM_RunBootloader(&arg);
    #else
    // No custom bootloader, or run bootloader API, then just reset.
    WDOG_TriggerSystemSoftwareReset(WDOG1);
    #endif
    while (1) {
        ;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_bootloader_obj, 0, 1, machine_bootloader);
