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
#include "rtc.h"
#include "sys_ctrl_rtc.h"

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

static mp_obj_t machine_rtc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    const machine_rtc_obj_t *self = &machine_rtc;

    // Check arguments.
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    enable_lprtc_clk();
    lprtc_prescaler_disable(self->rtc);
    lprtc_counter_wrap_disable(self->rtc);
    lprtc_interrupt_disable(self->rtc);
    lprtc_interrupt_unmask(self->rtc);

    NVIC_SetPriority(LPRTC_IRQ_IRQn, IRQ_PRI_RTC);
    NVIC_ClearPendingIRQ(LPRTC_IRQ_IRQn);
    NVIC_EnableIRQ(LPRTC_IRQ_IRQn);
    return MP_OBJ_FROM_PTR(self);
}

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

        lprtc_counter_disable(self->rtc);
        lprtc_load_count(self->rtc, 1);
        lprtc_load_counter_match_register(self->rtc, seconds * 32768);

        lprtc_interrupt_ack(self->rtc);
        lprtc_interrupt_enable(self->rtc);
        lprtc_counter_enable(self->rtc);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid argument(s)"));
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_rtc_alarm_obj, 1, machine_rtc_alarm);

static const mp_rom_map_elem_t machine_rtc_locals_dict_table[] = {
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
