/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

#include "py/nlr.h"
#include "py/runtime.h"
#include "timer.h"
#include "nrfx_timer.h"

#if MICROPY_PY_MACHINE_TIMER

enum {
    TIMER_MODE_ONESHOT,
    TIMER_MODE_PERIODIC,
};

typedef struct _machine_timer_obj_t {
    mp_obj_base_t  base;
    nrfx_timer_t   p_instance;
} machine_timer_obj_t;

STATIC mp_obj_t machine_timer_callbacks[] = {
    NULL,
    NULL,
    NULL,
#if defined(NRF52_SERIES)
    NULL,
    NULL,
#endif
};

STATIC const machine_timer_obj_t machine_timer_obj[] = {
    {{&machine_timer_type}, NRFX_TIMER_INSTANCE(0)},
#if !defined(MICROPY_PY_MACHINE_SOFT_PWM) || (MICROPY_PY_MACHINE_SOFT_PWM == 0)
    {{&machine_timer_type}, NRFX_TIMER_INSTANCE(1)},
#endif
    {{&machine_timer_type}, NRFX_TIMER_INSTANCE(2)},
#if defined(NRF52_SERIES)
    {{&machine_timer_type}, NRFX_TIMER_INSTANCE(3)},
    {{&machine_timer_type}, NRFX_TIMER_INSTANCE(4)},
#endif
};

void timer_init0(void) {
    for (int i = 0; i < MP_ARRAY_SIZE(machine_timer_obj); i++) {
        nrfx_timer_uninit(&machine_timer_obj[i].p_instance);
    }
}

STATIC int timer_find(mp_obj_t id) {
    // given an integer id
    int timer_id = mp_obj_get_int(id);
    if (timer_id >= 0 && timer_id < MP_ARRAY_SIZE(machine_timer_obj)) {
        return timer_id;
    }
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
        "Timer(%d) does not exist", timer_id));
}

STATIC void timer_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    machine_timer_obj_t *self = o;
    mp_printf(print, "Timer(%u)", self->p_instance.instance_id);
}

STATIC void timer_event_handler(nrf_timer_event_t event_type, void *p_context) {
    machine_timer_obj_t *self = p_context;
    mp_obj_t callback = machine_timer_callbacks[self->p_instance.instance_id];
    if (callback != NULL) {
        mp_call_function_1(callback, self);
    }
}

/******************************************************************************/
/* MicroPython bindings for machine API                                       */

STATIC mp_obj_t machine_timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_period, ARG_mode, ARG_callback };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_SMALL_INT(-1)} },
        { MP_QSTR_period,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1000000} }, // 1 second
        { MP_QSTR_mode,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = TIMER_MODE_PERIODIC} },
        { MP_QSTR_callback, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get static peripheral object
    int timer_id = timer_find(args[ARG_id].u_obj);

#if BLUETOOTH_SD
    if (timer_id == 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Timer(%d) reserved by Bluetooth LE stack.", timer_id));
    }
#endif

#if MICROPY_PY_MACHINE_SOFT_PWM
    if (timer_id == 1) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Timer(%d) reserved by ticker driver.", timer_id));
    }
#endif

    machine_timer_obj_t *self = (machine_timer_obj_t*)&machine_timer_obj[timer_id];

    if (MP_OBJ_IS_FUN(args[ARG_callback].u_obj)) {
        machine_timer_callbacks[timer_id] = args[ARG_callback].u_obj;
    } else if (args[ARG_callback].u_obj == mp_const_none) {
        machine_timer_callbacks[timer_id] = NULL;
    } else {
        mp_raise_ValueError("callback must be a function");
    }

    // Timer peripheral usage:
    // Every timer instance has a numer of capture/compare (CC) registers.
    // These can store either the value to compare against (to trigger an
    // interrupt or a shortcut) or store a value returned from a
    // capture/compare event.
    // We use channel 0 for comparing (to trigger the callback and clear
    // shortcut) and channel 1 for capturing the current time.

    const nrfx_timer_config_t config = {
        .frequency = NRF_TIMER_FREQ_1MHz,
        .mode = NRF_TIMER_MODE_TIMER,
        .bit_width = NRF_TIMER_BIT_WIDTH_24,
        #ifdef NRF51
        .interrupt_priority = 3,
        #else
        .interrupt_priority = 6,
        #endif
        .p_context = self,
    };

    // Initialize the drive.
    // When it is already initialized, this is a no-op.
    nrfx_timer_init(&self->p_instance, &config, timer_event_handler);

    // Configure channel 0.
    nrf_timer_short_mask_t short_mask = NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK |
        ((args[ARG_mode].u_int == TIMER_MODE_ONESHOT) ? NRF_TIMER_SHORT_COMPARE0_STOP_MASK : 0);
    bool enable_interrupts = true;
    nrfx_timer_extended_compare(
            &self->p_instance,
            NRF_TIMER_CC_CHANNEL0,
            args[ARG_period].u_int,
            short_mask,
            enable_interrupts);

    return MP_OBJ_FROM_PTR(self);
}

/// \method period()
/// Return counter value, which is currently in us.
///
STATIC mp_obj_t machine_timer_period(mp_obj_t self_in) {
    machine_timer_obj_t * self = MP_OBJ_TO_PTR(self_in);

    uint32_t period = nrfx_timer_capture(&self->p_instance, NRF_TIMER_CC_CHANNEL1);

    return MP_OBJ_NEW_SMALL_INT(period);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_period_obj, machine_timer_period);

/// \method start()
/// Start the timer.
///
STATIC mp_obj_t machine_timer_start(mp_obj_t self_in) {
    machine_timer_obj_t * self = MP_OBJ_TO_PTR(self_in);

    nrfx_timer_enable(&self->p_instance);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_start_obj, machine_timer_start);

/// \method stop()
/// Stop the timer.
///
STATIC mp_obj_t machine_timer_stop(mp_obj_t self_in) {
    machine_timer_obj_t * self = MP_OBJ_TO_PTR(self_in);

    nrfx_timer_disable(&self->p_instance);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_stop_obj, machine_timer_stop);

/// \method deinit()
/// Free resources associated with the timer.
///
STATIC mp_obj_t machine_timer_deinit(mp_obj_t self_in) {
    machine_timer_obj_t * self = MP_OBJ_TO_PTR(self_in);

    nrfx_timer_uninit(&self->p_instance);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_deinit_obj, machine_timer_deinit);

STATIC const mp_rom_map_elem_t machine_timer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_time),     MP_ROM_PTR(&machine_timer_period_obj) }, // alias
    { MP_ROM_QSTR(MP_QSTR_period),   MP_ROM_PTR(&machine_timer_period_obj) },
    { MP_ROM_QSTR(MP_QSTR_start),    MP_ROM_PTR(&machine_timer_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop),     MP_ROM_PTR(&machine_timer_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),   MP_ROM_PTR(&machine_timer_deinit_obj) },

    // constants
    { MP_ROM_QSTR(MP_QSTR_ONESHOT),  MP_ROM_INT(TIMER_MODE_ONESHOT) },
    { MP_ROM_QSTR(MP_QSTR_PERIODIC), MP_ROM_INT(TIMER_MODE_PERIODIC) },
};

STATIC MP_DEFINE_CONST_DICT(machine_timer_locals_dict, machine_timer_locals_dict_table);

const mp_obj_type_t machine_timer_type = {
    { &mp_type_type },
    .name = MP_QSTR_Timer,
    .print = timer_print,
    .make_new = machine_timer_make_new,
    .locals_dict = (mp_obj_dict_t*)&machine_timer_locals_dict
};

#endif // MICROPY_PY_MACHINE_TIMER
