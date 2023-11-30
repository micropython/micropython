/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2023 Damien P. George
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

#include "modmachine.h"
#include "samd_soc.h"

// ASF 4
#include "hal_flash.h"
#include "hal_init.h"
#include "hpl_gclk_base.h"
#include "hpl_pm_base.h"

#if defined(MCU_SAMD21)
#define DBL_TAP_ADDR    ((volatile uint32_t *)(HMCRAMC0_ADDR + HMCRAMC0_SIZE - 4))
#elif defined(MCU_SAMD51)
#define DBL_TAP_ADDR    ((volatile uint32_t *)(HSRAM_ADDR + HSRAM_SIZE - 4))
#endif
// A board may define a DPL_TAP_ADDR_ALT, which will be set as well
// Needed at the moment for Sparkfun SAMD51 Thing Plus
#define DBL_TAP_MAGIC_LOADER 0xf01669ef
#define DBL_TAP_MAGIC_RESET 0xf02669ef

#define LIGHTSLEEP_CPU_FREQ 200000

#define MICROPY_PY_MACHINE_EXTRA_GLOBALS \
    { MP_ROM_QSTR(MP_QSTR_Pin),                 MP_ROM_PTR(&machine_pin_type) }, \
    { MP_ROM_QSTR(MP_QSTR_Timer),               MP_ROM_PTR(&machine_timer_type) }, \
    { MP_ROM_QSTR(MP_QSTR_RTC),                 MP_ROM_PTR(&machine_rtc_type) }, \
    \
    /* Class constants. */ \
    /* Use numerical constants instead of the symbolic names, */ \
    /* since the names differ between SAMD21 and SAMD51. */ \
    { MP_ROM_QSTR(MP_QSTR_PWRON_RESET),         MP_ROM_INT(0x01) }, \
    { MP_ROM_QSTR(MP_QSTR_HARD_RESET),          MP_ROM_INT(0x10) }, \
    { MP_ROM_QSTR(MP_QSTR_WDT_RESET),           MP_ROM_INT(0x20) }, \
    { MP_ROM_QSTR(MP_QSTR_SOFT_RESET),          MP_ROM_INT(0x40) }, \
    { MP_ROM_QSTR(MP_QSTR_DEEPSLEEP_RESET),     MP_ROM_INT(0x80) }, \

extern bool EIC_occured;
extern uint32_t _dbl_tap_addr;

NORETURN STATIC void mp_machine_reset(void) {
    *DBL_TAP_ADDR = DBL_TAP_MAGIC_RESET;
    #ifdef DBL_TAP_ADDR_ALT
    *DBL_TAP_ADDR_ALT = DBL_TAP_MAGIC_RESET;
    #endif
    NVIC_SystemReset();
}

NORETURN void mp_machine_bootloader(size_t n_args, const mp_obj_t *args) {
    *DBL_TAP_ADDR = DBL_TAP_MAGIC_LOADER;
    #ifdef DBL_TAP_ADDR_ALT
    *DBL_TAP_ADDR_ALT = DBL_TAP_MAGIC_LOADER;
    #endif
    NVIC_SystemReset();
}

STATIC mp_obj_t mp_machine_get_freq(void) {
    return MP_OBJ_NEW_SMALL_INT(get_cpu_freq());
}

STATIC void mp_machine_set_freq(size_t n_args, const mp_obj_t *args) {
    uint32_t freq = mp_obj_get_int(args[0]);
    if (freq >= 1000000 && freq <= MAX_CPU_FREQ) {
        set_cpu_freq(freq);
    }
}

STATIC mp_obj_t mp_machine_unique_id(void) {
    samd_unique_id_t id;
    samd_get_unique_id(&id);
    return mp_obj_new_bytes((byte *)&id.bytes, sizeof(id.bytes));
}

STATIC void mp_machine_idle(void) {
    MICROPY_EVENT_POLL_HOOK;
}

STATIC mp_int_t mp_machine_reset_cause(void) {
    #if defined(MCU_SAMD21)
    return PM->RCAUSE.reg;
    #elif defined(MCU_SAMD51)
    return RSTC->RCAUSE.reg;
    #else
    return 0;
    #endif
}

STATIC void mp_machine_lightsleep(size_t n_args, const mp_obj_t *args) {
    int32_t duration = -1;
    uint32_t freq = get_cpu_freq();
    if (n_args > 0) {
        duration = mp_obj_get_int(args[0]);
    }
    EIC_occured = false;
    // Slow down
    set_cpu_freq(LIGHTSLEEP_CPU_FREQ);
    #if defined(MCU_SAMD21)
    // Switch the peripheral clock off
    GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(2);
    while (GCLK->STATUS.bit.SYNCBUSY) {
    }
    // Switch the EIC temporarily to GCLK3, since GCLK2 is off
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK3 | EIC_GCLK_ID;
    if (duration > 0) {
        uint32_t t0 = systick_ms;
        while ((systick_ms - t0 < duration) && (EIC_occured == false)) {
            __WFI();
        }
    } else {
        while (EIC_occured == false) {
            __WFI();
        }
    }
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2 | EIC_GCLK_ID;

    #elif defined(MCU_SAMD51)
    // Switch the peripheral clock off
    GCLK->GENCTRL[2].reg = 0;
    while (GCLK->SYNCBUSY.bit.GENCTRL2) {
    }
    // Switch the EIC temporarily to GCLK3, since GCLK2 is off
    GCLK->PCHCTRL[EIC_GCLK_ID].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK3;
    if (duration > 0) {
        uint32_t t0 = systick_ms;
        while ((systick_ms - t0 < duration) && (EIC_occured == false)) {
            __WFI();
        }
    } else {
        while (EIC_occured == false) {
            __WFI();
        }
    }
    GCLK->PCHCTRL[EIC_GCLK_ID].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK2;

    #endif
    // Speed up again
    set_cpu_freq(freq);
}

NORETURN STATIC void mp_machine_deepsleep(size_t n_args, const mp_obj_t *args) {
    mp_machine_lightsleep(n_args, args);
    mp_machine_reset();
}
