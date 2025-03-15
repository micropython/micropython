/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
 * Copyright (c) 2018 Ayke van Laethem
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

#include "py/mphal.h"
#include "py/runtime.h"
#include "rtcounter.h"
#include "nrfx_rtc.h"
#include "nrf_clock.h"

#if MICROPY_PY_MACHINE_RTCOUNTER

// Count every 125ms (~maximum prescaler setting)
#define RTC_FREQUENCY (8UL)

enum {
    RTC_MODE_ONESHOT,
    RTC_MODE_PERIODIC,
};

// Volatile part of the RTCounter object.
typedef struct {
    mp_obj_t callback;
    uint32_t period;
} machine_rtc_config_t;

// Non-volatile part of the RTCounter object.
typedef struct _machine_rtc_obj_t {
    mp_obj_base_t base;
    const nrfx_rtc_t *p_rtc;        // Driver instance
    nrfx_rtc_handler_t handler;     // interrupt callback
    machine_rtc_config_t *config;   // pointer to volatile part
} machine_rtc_obj_t;

static const nrfx_rtc_t machine_rtc_instances[] = {
    NRFX_RTC_INSTANCE(0),
    NRFX_RTC_INSTANCE(1),
    #if defined(NRF52_SERIES)
    NRFX_RTC_INSTANCE(2),
    #endif
};

static machine_rtc_config_t configs[MP_ARRAY_SIZE(machine_rtc_instances)];

static void interrupt_handler0(nrfx_rtc_int_type_t int_type);
static void interrupt_handler1(nrfx_rtc_int_type_t int_type);
#if defined(NRF52_SERIES)
static void interrupt_handler2(nrfx_rtc_int_type_t int_type);
#endif

static const machine_rtc_obj_t machine_rtc_obj[] = {
    {{&machine_rtcounter_type}, .p_rtc = &machine_rtc_instances[0], .handler = interrupt_handler0, .config = &configs[0]},
    {{&machine_rtcounter_type}, .p_rtc = &machine_rtc_instances[1], .handler = interrupt_handler1, .config = &configs[1]},
    #if defined(NRF52_SERIES)
    {{&machine_rtcounter_type}, .p_rtc = &machine_rtc_instances[2], .handler = interrupt_handler2, .config = &configs[2]},
    #endif
};

static void interrupt_handler(size_t instance_id) {
    const machine_rtc_obj_t *self = &machine_rtc_obj[instance_id];
    machine_rtc_config_t *config = self->config;
    if (config->callback != NULL) {
        mp_call_function_1((mp_obj_t)config->callback, (mp_obj_t)self);
    }
    if (config->period == 0) {
        nrfx_rtc_cc_disable(self->p_rtc, 0);
    } else { // periodic
        uint32_t val = nrfx_rtc_counter_get(self->p_rtc) + config->period;
        nrfx_rtc_cc_set(self->p_rtc, 0, val, true);
    }
}

static void interrupt_handler0(nrfx_rtc_int_type_t int_type) {
    interrupt_handler(0);
}

static void interrupt_handler1(nrfx_rtc_int_type_t int_type) {
    interrupt_handler(1);
}

#if defined(NRF52_SERIES)
static void interrupt_handler2(nrfx_rtc_int_type_t int_type) {
    interrupt_handler(2);
}
#endif

void rtc_init0(void) {
}

static int rtc_find(mp_obj_t id) {
    // given an integer id
    int rtc_id = mp_obj_get_int(id);
    if (rtc_id >= 0 && rtc_id < MP_ARRAY_SIZE(machine_rtc_obj)) {
        return rtc_id;
    }
    mp_raise_ValueError(MP_ERROR_TEXT("RTCounter doesn't exist"));
}

static void rtc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_rtc_obj_t *self = self_in;
    mp_printf(print, "RTCounter(%u)", self->p_rtc->instance_id);
}

/******************************************************************************/
/* MicroPython bindings for machine API                                       */

const nrfx_rtc_config_t machine_rtc_config = {
    .prescaler = RTC_FREQ_TO_PRESCALER(RTC_FREQUENCY),
    .reliable = 0,
    .tick_latency = 0, // ignored when reliable == 0
    #ifdef NRF51
    .interrupt_priority = 3,
    #else
    .interrupt_priority = 6,
    #endif
};

static mp_obj_t machine_rtc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_period, ARG_mode, ARG_callback };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_SMALL_INT(-1)} },
        { MP_QSTR_period,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = RTC_FREQUENCY} }, // 1 second
        { MP_QSTR_mode,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = RTC_MODE_PERIODIC} },
        { MP_QSTR_callback, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int rtc_id = rtc_find(args[ARG_id].u_obj);

    #if MICROPY_PY_TIME_TICKS
    if (rtc_id == 1) {
        // time module uses RTC1, prevent using it
        mp_raise_ValueError(MP_ERROR_TEXT("RTC1 reserved by time module"));
    }
    #endif

    // const and non-const part of the RTC object.
    const machine_rtc_obj_t *self = &machine_rtc_obj[rtc_id];
    machine_rtc_config_t *config = self->config;

    if (args[ARG_callback].u_obj == mp_const_none) {
        config->callback = NULL;
    } else if (mp_obj_is_fun(args[ARG_callback].u_obj)) {
        config->callback = args[ARG_callback].u_obj;
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("callback must be a function"));
    }

    // Periodic or one-shot
    if (args[ARG_mode].u_int == RTC_MODE_ONESHOT) {
        // One-shot
        config->period = 0;
    } else {
        // Period between the intervals
        config->period = args[ARG_period].u_int;
    }

    // Start the low-frequency clock (if it hasn't been started already)
    mp_nrf_start_lfclk();

    // Make sure it's uninitialized.
    nrfx_rtc_uninit(self->p_rtc);
    nrfx_rtc_counter_clear(self->p_rtc);

    // Initialize and set the correct IRQ.
    nrfx_rtc_init(self->p_rtc, &machine_rtc_config, self->handler);
    nrfx_rtc_cc_set(self->p_rtc, 0 /*channel*/, args[ARG_period].u_int, true /*enable irq*/);

    return MP_OBJ_FROM_PTR(self);
}

/// \method start()
/// Start the RTCounter. Timeout occurs after number of periods
/// in the configured frequency has been reached.
///
static mp_obj_t machine_rtc_start(mp_obj_t self_in) {
    machine_rtc_obj_t *self = MP_OBJ_TO_PTR(self_in);

    nrfx_rtc_enable(self->p_rtc);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_rtc_start_obj, machine_rtc_start);

/// \method stop()
/// Stop the RTCounter.
///
static mp_obj_t machine_rtc_stop(mp_obj_t self_in) {
    machine_rtc_obj_t *self = MP_OBJ_TO_PTR(self_in);

    nrfx_rtc_disable(self->p_rtc);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_rtc_stop_obj, machine_rtc_stop);

/// \method counter()
/// Return the current counter value. Wraps around after about 24 days
/// with the current prescaler (2^24 / 8 = 2097152 seconds).
///
static mp_obj_t machine_rtc_counter(mp_obj_t self_in) {
    machine_rtc_obj_t *self = MP_OBJ_TO_PTR(self_in);

    uint32_t counter = nrfx_rtc_counter_get(self->p_rtc);

    return MP_OBJ_NEW_SMALL_INT(counter);
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_rtc_counter_obj, machine_rtc_counter);

/// \method deinit()
/// Free resources associated with this RTC.
///
static mp_obj_t machine_rtc_deinit(mp_obj_t self_in) {
    machine_rtc_obj_t *self = MP_OBJ_TO_PTR(self_in);

    nrfx_rtc_uninit(self->p_rtc);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_rtc_deinit_obj, machine_rtc_deinit);


static const mp_rom_map_elem_t machine_rtc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_start),     MP_ROM_PTR(&machine_rtc_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop),      MP_ROM_PTR(&machine_rtc_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_counter),   MP_ROM_PTR(&machine_rtc_counter_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),    MP_ROM_PTR(&machine_rtc_deinit_obj) },

    // constants
    { MP_ROM_QSTR(MP_QSTR_ONESHOT),   MP_ROM_INT(RTC_MODE_ONESHOT) },
    { MP_ROM_QSTR(MP_QSTR_PERIODIC),  MP_ROM_INT(RTC_MODE_PERIODIC) },
    { MP_ROM_QSTR(MP_QSTR_FREQUENCY), MP_ROM_INT(RTC_FREQUENCY) },
};

static MP_DEFINE_CONST_DICT(machine_rtc_locals_dict, machine_rtc_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_rtcounter_type,
    MP_QSTR_RTCounter,
    MP_TYPE_FLAG_NONE,
    make_new, machine_rtc_make_new,
    print, rtc_print,
    locals_dict, &machine_rtc_locals_dict
    );

#endif // MICROPY_PY_MACHINE_RTCOUNTER
