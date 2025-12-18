/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2023 Damien P. George
 * Copyright (c) 2016 Glenn Ruben Bakke
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

#include <stdio.h>

#include "modmachine.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "lib/oofatfs/ff.h"
#include "lib/oofatfs/diskio.h"
#include "gccollect.h"
#include "pin.h"
#include "spi.h"
#include "i2c.h"
#include "timer.h"
#if MICROPY_PY_MACHINE_HW_PWM || MICROPY_PY_MACHINE_SOFT_PWM
#include "pwm.h"
#endif
#if MICROPY_PY_MACHINE_TEMP
#include "temp.h"
#endif
#if MICROPY_PY_MACHINE_RTCOUNTER
#include "rtcounter.h"
#endif

#define PYB_RESET_HARD      (0)
#define PYB_RESET_WDT       (1)
#define PYB_RESET_SOFT      (2)
#define PYB_RESET_LOCKUP    (3)
#define PYB_RESET_POWER_ON  (16)
#define PYB_RESET_LPCOMP    (17)
#define PYB_RESET_DIF       (18)
#define PYB_RESET_NFC       (19)

#if MICROPY_PY_MACHINE_RTCOUNTER
#define MICROPY_PY_MACHINE_RTCOUNTER_ENTRY { MP_ROM_QSTR(MP_QSTR_RTCounter), MP_ROM_PTR(&machine_rtcounter_type) },
#else
#define MICROPY_PY_MACHINE_RTCOUNTER_ENTRY
#endif

#if MICROPY_PY_MACHINE_TIMER_NRF
#define MICROPY_PY_MACHINE_TIMER_ENTRY { MP_ROM_QSTR(MP_QSTR_Timer), MP_ROM_PTR(&machine_timer_type) },
#else
#define MICROPY_PY_MACHINE_TIMER_ENTRY
#endif

#if MICROPY_PY_MACHINE_TEMP
#define MICROPY_PY_MACHINE_TEMP_ENTRY { MP_ROM_QSTR(MP_QSTR_Temp), MP_ROM_PTR(&machine_temp_type) },
#else
#define MICROPY_PY_MACHINE_TEMP_ENTRY
#endif

#if defined(NRF52_SERIES)
#define MICROPY_PY_MACHINE_NFC_RESET_ENTRY { MP_ROM_QSTR(MP_QSTR_NFC_RESET), MP_ROM_INT(PYB_RESET_NFC) },
#else
#define MICROPY_PY_MACHINE_NFC_RESET_ENTRY
#endif

#define MICROPY_PY_MACHINE_EXTRA_GLOBALS \
    { MP_ROM_QSTR(MP_QSTR_info),               MP_ROM_PTR(&machine_info_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_sleep),              MP_ROM_PTR(&machine_lightsleep_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_Pin),                MP_ROM_PTR(&pin_type) }, \
    \
    MICROPY_PY_MACHINE_RTCOUNTER_ENTRY \
    MICROPY_PY_MACHINE_TIMER_ENTRY \
    MICROPY_PY_MACHINE_TEMP_ENTRY \
    { MP_ROM_QSTR(MP_QSTR_HARD_RESET),         MP_ROM_INT(PYB_RESET_HARD) }, \
    { MP_ROM_QSTR(MP_QSTR_WDT_RESET),          MP_ROM_INT(PYB_RESET_WDT) }, \
    { MP_ROM_QSTR(MP_QSTR_SOFT_RESET),         MP_ROM_INT(PYB_RESET_SOFT) }, \
    { MP_ROM_QSTR(MP_QSTR_LOCKUP_RESET),       MP_ROM_INT(PYB_RESET_LOCKUP) }, \
    { MP_ROM_QSTR(MP_QSTR_PWRON_RESET),        MP_ROM_INT(PYB_RESET_POWER_ON) }, \
    { MP_ROM_QSTR(MP_QSTR_LPCOMP_RESET),       MP_ROM_INT(PYB_RESET_LPCOMP) }, \
    { MP_ROM_QSTR(MP_QSTR_DEBUG_IF_RESET),     MP_ROM_INT(PYB_RESET_DIF) }, \
    MICROPY_PY_MACHINE_NFC_RESET_ENTRY \

static uint32_t reset_cause;

void machine_init(void) {
    uint32_t state = NRF_POWER->RESETREAS;
    if (state & POWER_RESETREAS_RESETPIN_Msk) {
        reset_cause = PYB_RESET_HARD;
    } else if (state & POWER_RESETREAS_DOG_Msk) {
        reset_cause = PYB_RESET_WDT;
    } else if (state & POWER_RESETREAS_SREQ_Msk) {
        reset_cause = PYB_RESET_SOFT;
    } else if (state & POWER_RESETREAS_LOCKUP_Msk) {
        reset_cause = PYB_RESET_LOCKUP;
    } else if (state & POWER_RESETREAS_OFF_Msk) {
        reset_cause = PYB_RESET_POWER_ON;
    #if !defined(NRF9160_XXAA)
    } else if (state & POWER_RESETREAS_LPCOMP_Msk) {
        reset_cause = PYB_RESET_LPCOMP;
    #endif
    } else if (state & POWER_RESETREAS_DIF_Msk) {
        reset_cause = PYB_RESET_DIF;
    #if defined(NRF52_SERIES)
    } else if (state & POWER_RESETREAS_NFC_Msk) {
        reset_cause = PYB_RESET_NFC;
    #endif
    }

    // clear reset reason
    NRF_POWER->RESETREAS = (1 << reset_cause);
}

// machine.info([dump_alloc_table])
// Print out lots of information about the board.
static mp_obj_t machine_info(mp_uint_t n_args, const mp_obj_t *args) {
    // to print info about memory
    {
        printf("_etext=%p\n", &_etext);
        printf("_sidata=%p\n", &_sidata);
        printf("_sdata=%p\n", &_sdata);
        printf("_edata=%p\n", &_edata);
        printf("_sbss=%p\n", &_sbss);
        printf("_ebss=%p\n", &_ebss);
        printf("_estack=%p\n", &_estack);
        printf("_ram_start=%p\n", &_ram_start);
        printf("_heap_start=%p\n", &_heap_start);
        printf("_heap_end=%p\n", &_heap_end);
        printf("_ram_end=%p\n", &_ram_end);
    }

    // qstr info
    {
        mp_uint_t n_pool, n_qstr, n_str_data_bytes, n_total_bytes;
        qstr_pool_info(&n_pool, &n_qstr, &n_str_data_bytes, &n_total_bytes);
        printf("qstr:\n  n_pool=" UINT_FMT "\n  n_qstr=" UINT_FMT "\n  n_str_data_bytes=" UINT_FMT "\n  n_total_bytes=" UINT_FMT "\n", n_pool, n_qstr, n_str_data_bytes, n_total_bytes);
    }

    // GC info
    {
        gc_info_t info;
        gc_info(&info);
        printf("GC:\n");
        printf("  " UINT_FMT " total\n", info.total);
        printf("  " UINT_FMT " : " UINT_FMT "\n", info.used, info.free);
        printf("  1=" UINT_FMT " 2=" UINT_FMT " m=" UINT_FMT "\n", info.num_1block, info.num_2block, info.max_block);
    }

    if (n_args == 1) {
        // arg given means dump gc allocation table
        gc_dump_alloc_table(&mp_plat_print);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_info_obj, 0, 1, machine_info);

static mp_obj_t mp_machine_unique_id(void) {
    return mp_const_empty_bytes;
}

// Resets the board in a manner similar to pushing the external RESET button.
MP_NORETURN static void mp_machine_reset(void) {
    NVIC_SystemReset();
}

MP_NORETURN void mp_machine_bootloader(size_t n_args, const mp_obj_t *args) {
    MICROPY_BOARD_ENTER_BOOTLOADER(n_args, args);
    for (;;) {
    }
}

static void mp_machine_idle(void) {
    MICROPY_EVENT_POLL_HOOK;
}

static void mp_machine_lightsleep(size_t n_args, const mp_obj_t *args) {
    __WFE();
}

MP_NORETURN static void mp_machine_deepsleep(size_t n_args, const mp_obj_t *args) {
    mp_machine_reset();
}

static mp_int_t mp_machine_reset_cause(void) {
    return reset_cause;
}

static mp_obj_t mp_machine_get_freq(void) {
    mp_raise_NotImplementedError(NULL);
}

static void mp_machine_set_freq(size_t n_args, const mp_obj_t *args) {
    mp_raise_NotImplementedError(NULL);
}
