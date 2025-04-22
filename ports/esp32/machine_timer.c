/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2015 Damien P. George
 * Copyright (c) 2016 Paul Sokolovsky
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
#include <stdio.h>

#include "py/mphal.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "modmachine.h"

#include "hal/timer_hal.h"
#include "hal/timer_ll.h"
#include "soc/timer_periph.h"
#include "machine_timer.h"

#define TIMER_DIVIDER  8

// TIMER_BASE_CLK is normally 80MHz. TIMER_DIVIDER ought to divide this exactly
#define TIMER_SCALE    (APB_CLK_FREQ / TIMER_DIVIDER)

#define TIMER_FLAGS    0

const mp_obj_type_t machine_timer_type;

static mp_obj_t machine_timer_init_helper(machine_timer_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
static mp_obj_t machine_timer_deinit(mp_obj_t self_in);

void machine_timer_deinit_all(void) {
    // Disable, deallocate and remove all timers from list
    machine_timer_obj_t **t = &MP_STATE_PORT(machine_timer_obj_head);
    while (*t != NULL) {
        machine_timer_deinit(*t);
        machine_timer_obj_t *next = (*t)->next;
        m_del_obj(machine_timer_obj_t, *t);
        *t = next;
    }
}

static void machine_timer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_timer_obj_t *self = self_in;
    qstr mode = self->repeat ? MP_QSTR_PERIODIC : MP_QSTR_ONE_SHOT;
    uint64_t period = self->period / (TIMER_SCALE / 1000); // convert to ms
    #if SOC_TIMER_GROUP_TIMERS_PER_GROUP == 1
    mp_printf(print, "Timer(%u, mode=%q, period=%lu)", self->group, mode, period);
    #else
    mp_printf(print, "Timer(%u, mode=%q, period=%lu)", (self->group << 1) | self->index, mode, period);
    #endif
}

machine_timer_obj_t *machine_timer_create(mp_uint_t timer) {

    machine_timer_obj_t *self = NULL;
    #if SOC_TIMER_GROUP_TIMERS_PER_GROUP == 1
    mp_uint_t group = timer & 1;
    mp_uint_t index = 0;
    #else
    mp_uint_t group = (timer >> 1) & 1;
    mp_uint_t index = timer & 1;
    #endif

    // Check whether the timer is already initialized, if so use it
    for (machine_timer_obj_t *t = MP_STATE_PORT(machine_timer_obj_head); t; t = t->next) {
        if (t->group == group && t->index == index) {
            self = t;
            break;
        }
    }
    // The timer does not exist, create it.
    if (self == NULL) {
        self = mp_obj_malloc(machine_timer_obj_t, &machine_timer_type);
        self->group = group;
        self->index = index;
        self->handle = NULL;

        // Add the timer to the linked-list of timers
        self->next = MP_STATE_PORT(machine_timer_obj_head);
        MP_STATE_PORT(machine_timer_obj_head) = self;
    }
    return self;
}

static mp_obj_t machine_timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Create the new timer.
    uint32_t timer_number = mp_obj_get_int(args[0]);
    if (timer_number >= SOC_TIMER_GROUP_TOTAL_TIMERS) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid Timer number"));
    }
    machine_timer_obj_t *self = machine_timer_create(timer_number);

    if (n_args > 1 || n_kw > 0) {
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_timer_init_helper(self, n_args - 1, args + 1, &kw_args);
    }

    return self;
}

void machine_timer_disable(machine_timer_obj_t *self) {
    if (self->hal_context.dev != NULL) {
        // Disable the counter and alarm.
        timer_ll_enable_counter(self->hal_context.dev, self->index, false);
        timer_ll_enable_alarm(self->hal_context.dev, self->index, false);
    }

    if (self->handle) {
        // Disable the interrupt
        ESP_ERROR_CHECK(esp_intr_disable(self->handle));
    }

    // We let the disabled timer stay in the list, as it might be
    // referenced elsewhere
}

static void machine_timer_isr(void *self_in) {
    machine_timer_obj_t *self = self_in;

    uint32_t intr_status = timer_ll_get_intr_status(self->hal_context.dev);

    if (intr_status & TIMER_LL_EVENT_ALARM(self->index)) {
        timer_ll_clear_intr_status(self->hal_context.dev, TIMER_LL_EVENT_ALARM(self->index));
        if (self->repeat) {
            timer_ll_enable_alarm(self->hal_context.dev, self->index, true);
        }
        self->handler(self);
    }
}

static void machine_timer_isr_handler(machine_timer_obj_t *self) {
    mp_sched_schedule(self->callback, self);
    mp_hal_wake_main_task_from_isr();
}

void machine_timer_enable(machine_timer_obj_t *self) {
    // Initialise the timer.
    timer_hal_init(&self->hal_context, self->group, self->index);
    timer_ll_enable_counter(self->hal_context.dev, self->index, false);
    timer_ll_set_clock_source(self->hal_context.dev, self->index, GPTIMER_CLK_SRC_DEFAULT);
    timer_ll_set_clock_prescale(self->hal_context.dev, self->index, TIMER_DIVIDER);
    timer_hal_set_counter_value(&self->hal_context, 0);
    timer_ll_set_count_direction(self->hal_context.dev, self->index, GPTIMER_COUNT_UP);

    // Allocate and enable the alarm interrupt.
    timer_ll_enable_intr(self->hal_context.dev, TIMER_LL_EVENT_ALARM(self->index), false);
    timer_ll_clear_intr_status(self->hal_context.dev, TIMER_LL_EVENT_ALARM(self->index));
    if (self->handle) {
        ESP_ERROR_CHECK(esp_intr_enable(self->handle));
    } else {
        ESP_ERROR_CHECK(esp_intr_alloc(
            timer_group_periph_signals.groups[self->group].timer_irq_id[self->index],
            TIMER_FLAGS,
            machine_timer_isr,
            self,
            &self->handle
            ));
    }
    timer_ll_enable_intr(self->hal_context.dev, TIMER_LL_EVENT_ALARM(self->index), true);

    // Enable the alarm to trigger at the given period.
    timer_ll_set_alarm_value(self->hal_context.dev, self->index, self->period);
    timer_ll_enable_alarm(self->hal_context.dev, self->index, true);

    // Set the counter to reload at 0 if it's in repeat mode.
    timer_ll_set_reload_value(self->hal_context.dev, self->index, 0);
    timer_ll_enable_auto_reload(self->hal_context.dev, self->index, self->repeat);

    // Enable the counter.
    timer_ll_enable_counter(self->hal_context.dev, self->index, true);
}

static mp_obj_t machine_timer_init_helper(machine_timer_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {
        ARG_mode,
        ARG_callback,
        ARG_period,
        ARG_tick_hz,
        ARG_freq,
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_callback,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_period,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
        { MP_QSTR_tick_hz,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1000} },
        #if MICROPY_PY_BUILTINS_FLOAT
        { MP_QSTR_freq,         MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        #else
        { MP_QSTR_freq,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
        #endif
    };

    machine_timer_disable(self);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    #if MICROPY_PY_BUILTINS_FLOAT
    if (args[ARG_freq].u_obj != mp_const_none) {
        self->period = (uint64_t)(TIMER_SCALE / mp_obj_get_float(args[ARG_freq].u_obj));
    }
    #else
    if (args[ARG_freq].u_int != 0xffffffff) {
        self->period = TIMER_SCALE / ((uint64_t)args[ARG_freq].u_int);
    }
    #endif
    else {
        self->period = (((uint64_t)args[ARG_period].u_int) * TIMER_SCALE) / args[ARG_tick_hz].u_int;
    }

    self->repeat = args[ARG_mode].u_int;
    self->handler = machine_timer_isr_handler;
    self->callback = args[ARG_callback].u_obj;

    machine_timer_enable(self);

    return mp_const_none;
}

static mp_obj_t machine_timer_deinit(mp_obj_t self_in) {
    machine_timer_obj_t *self = self_in;

    machine_timer_disable(self);
    if (self->handle) {
        ESP_ERROR_CHECK(esp_intr_free(self->handle));
        self->handle = NULL;
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_deinit_obj, machine_timer_deinit);

static mp_obj_t machine_timer_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_timer_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_timer_init_obj, 1, machine_timer_init);

static mp_obj_t machine_timer_value(mp_obj_t self_in) {
    machine_timer_obj_t *self = self_in;
    if (self->handle == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("timer not set"));
    }
    uint64_t result = timer_ll_get_counter_value(self->hal_context.dev, self->index);
    return MP_OBJ_NEW_SMALL_INT((mp_uint_t)(result / (TIMER_SCALE / 1000))); // value in ms
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_value_obj, machine_timer_value);

static const mp_rom_map_elem_t machine_timer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&machine_timer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_timer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_timer_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&machine_timer_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_ONE_SHOT), MP_ROM_INT(false) },
    { MP_ROM_QSTR(MP_QSTR_PERIODIC), MP_ROM_INT(true) },
};
static MP_DEFINE_CONST_DICT(machine_timer_locals_dict, machine_timer_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_timer_type,
    MP_QSTR_Timer,
    MP_TYPE_FLAG_NONE,
    make_new, machine_timer_make_new,
    print, machine_timer_print,
    locals_dict, &machine_timer_locals_dict
    );

MP_REGISTER_ROOT_POINTER(struct _machine_timer_obj_t *machine_timer_obj_head);
