/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024-2025 OpenMV LLC.
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
#include "py/mphal.h"
#include "py/mperrno.h"
#include "extmod/modmachine.h"
#include "shared/timeutils/timeutils.h"
#include "rtc.h"
#include "sys_ctrl_rtc.h"

// The LPRTC (low-power real-time counter) is a 32-bit counter with a 16-bit prescaler,
// and usually clocked by a 32768Hz clock source.  To get a large date range of around
// 136 years, the prescaler is set to 32768 and so the counter clocks at 1Hz.  Then the
// counter counts the number of seconds since the 1970 Epoch.  The prescaler is used to
// get the subseconds which are then converted to microseconds.
//
// The combined counter+prescaler counts starting at 0 from the year 1970 up to the year
// 2106, with a resolution of 30.52 microseconds.
#define LPRTC_PRESCALER_SETTING (32768)

typedef struct _machine_rtc_obj_t {
    mp_obj_base_t base;
    LPRTC_Type *rtc;
} machine_rtc_obj_t;

// Singleton RTC object.
static const machine_rtc_obj_t machine_rtc = {{&machine_rtc_type}, (LPRTC_Type *)LPRTC_BASE};

void LPRTC_IRQHandler(void) {
    lprtc_interrupt_ack(machine_rtc.rtc);
    lprtc_interrupt_disable(machine_rtc.rtc);
}

// Returns the number of seconds and microseconds since the Epoch.
uint32_t mp_hal_time_get(uint32_t *microseconds) {
    uint32_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    uint32_t count = lprtc_get_count(machine_rtc.rtc);
    if (microseconds == NULL) {
        MICROPY_END_ATOMIC_SECTION(atomic_state);
        return count;
    }
    uint32_t prescaler = machine_rtc.rtc->LPRTC_CPCVR;
    uint32_t count2 = lprtc_get_count(machine_rtc.rtc);
    if (count != count2) {
        // The counter incremented during sampling of the prescaler, so resample the prescaler.
        prescaler = machine_rtc.rtc->LPRTC_CPCVR;
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);

    // Compute the microseconds from the up-counting prescaler value.
    MP_STATIC_ASSERT(LPRTC_PRESCALER_SETTING == 32768);
    *microseconds = 15625UL * prescaler / 512UL;

    return count2;
}

static mp_obj_t machine_rtc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    const machine_rtc_obj_t *self = &machine_rtc;

    // Check arguments.
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    lprtc_interrupt_disable(self->rtc);
    lprtc_interrupt_unmask(self->rtc);

    // Initialise the LPRTC if it's not already enabled.
    if (!((VBAT->RTC_CLK_EN & RTC_CLK_ENABLE)
          && (self->rtc->LPRTC_CCR & CCR_LPRTC_EN)
          && (self->rtc->LPRTC_CPSR == LPRTC_PRESCALER_SETTING))) {
        enable_lprtc_clk();
        self->rtc->LPRTC_CCR = 0;
        lprtc_load_prescaler(self->rtc, LPRTC_PRESCALER_SETTING);
        lprtc_load_count(self->rtc, 0);
        self->rtc->LPRTC_CCR = CCR_LPRTC_PSCLR_EN | CCR_LPRTC_EN;
    }

    NVIC_SetPriority(LPRTC_IRQ_IRQn, IRQ_PRI_RTC);
    NVIC_ClearPendingIRQ(LPRTC_IRQ_IRQn);
    NVIC_EnableIRQ(LPRTC_IRQ_IRQn);

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_rtc_datetime(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args == 1) {
        // Get datetime.
        uint32_t microseconds;
        mp_timestamp_t s = mp_hal_time_get(&microseconds);
        timeutils_struct_time_t tm;
        timeutils_seconds_since_epoch_to_struct_time(s, &tm);
        mp_obj_t tuple[8] = {
            mp_obj_new_int(tm.tm_year),
            mp_obj_new_int(tm.tm_mon),
            mp_obj_new_int(tm.tm_mday),
            mp_obj_new_int(tm.tm_wday),
            mp_obj_new_int(tm.tm_hour),
            mp_obj_new_int(tm.tm_min),
            mp_obj_new_int(tm.tm_sec),
            mp_obj_new_int(microseconds),
        };
        return mp_obj_new_tuple(8, tuple);
    } else {
        // Set datetime.
        mp_obj_t *items;
        mp_obj_get_array_fixed_n(args[1], 8, &items);
        timeutils_struct_time_t tm = {
            .tm_year = mp_obj_get_int(items[0]),
            .tm_mon = mp_obj_get_int(items[1]),
            .tm_mday = mp_obj_get_int(items[2]),
            .tm_hour = mp_obj_get_int(items[4]),
            .tm_min = mp_obj_get_int(items[5]),
            .tm_sec = mp_obj_get_int(items[6]),
        };
        mp_timestamp_t s = timeutils_seconds_since_epoch(tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

        // Disable then re-enable the LPRTC so that the prescaler counter resets to 0.
        machine_rtc.rtc->LPRTC_CCR = 0;
        lprtc_load_count(machine_rtc.rtc, s);
        machine_rtc.rtc->LPRTC_CCR = CCR_LPRTC_PSCLR_EN | CCR_LPRTC_EN;
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_datetime_obj, 1, 2, machine_rtc_datetime);

static mp_obj_t machine_rtc_alarm(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_id, ARG_time, ARG_repeat };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_time, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_repeat,  MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };

    machine_rtc_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), allowed_args, args);

    if (mp_obj_is_int(args[ARG_time].u_obj)) {
        uint32_t seconds = mp_obj_get_int(args[1].u_obj) / 1000;

        // Make sure we are guaranteed an interrupt:
        // - if seconds = 0 it won't fire
        // - if seconds = 1 it may miss if the counter rolls over just after it's read
        // - if seconds >= 2 then it will always fire (when read/written close enough)
        seconds = MAX(2, seconds);

        // Configure the counter match as atomically as possible.
        uint32_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
        lprtc_interrupt_ack(self->rtc);
        lprtc_load_counter_match_register(self->rtc, lprtc_get_count(self->rtc) + seconds);
        lprtc_interrupt_enable(self->rtc);
        MICROPY_END_ATOMIC_SECTION(atomic_state);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid argument(s)"));
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_rtc_alarm_obj, 1, machine_rtc_alarm);

static const mp_rom_map_elem_t machine_rtc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_datetime), MP_ROM_PTR(&machine_rtc_datetime_obj) },
    { MP_ROM_QSTR(MP_QSTR_alarm), MP_ROM_PTR(&machine_rtc_alarm_obj) },
};
static MP_DEFINE_CONST_DICT(machine_rtc_locals_dict, machine_rtc_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_rtc_type,
    MP_QSTR_RTC,
    MP_TYPE_FLAG_NONE,
    make_new, machine_rtc_make_new,
    locals_dict, &machine_rtc_locals_dict
    );
