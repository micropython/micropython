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
#include "esp_private/esp_clk_tree_common.h"
#include "esp_private/periph_ctrl.h"
#include "machine_timer.h"

#define TIMER_CLK_SRC GPTIMER_CLK_SRC_DEFAULT
#define TIMER_DIVIDER  8

#define TIMER_FLAGS    0

const mp_obj_type_t machine_timer_type;

static mp_obj_t machine_timer_init_helper(machine_timer_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
static mp_obj_t machine_timer_deinit(mp_obj_t self_in);

uint32_t machine_timer_freq_hz(void) {
    // The timer source clock is APB or a fixed PLL (depending on chip), both constant frequency.
    uint32_t freq;
    check_esp_err(esp_clk_tree_src_get_freq_hz(TIMER_CLK_SRC, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &freq));
    assert(freq % TIMER_DIVIDER == 0); // Source clock should divide evenly into TIMER_DIVIDER
    return freq / TIMER_DIVIDER;
}

void machine_timer_deinit_all(void) {
    // Disable, deallocate and remove all timers from list
    machine_timer_obj_t **t = &MP_STATE_PORT(machine_timer_obj_head);
    while (*t != NULL) {
        machine_timer_obj_t *next = (*t)->next;
        machine_timer_deinit(*t);
        m_del_obj(machine_timer_obj_t, *t);
        *t = next;
    }
}

static void machine_timer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_timer_obj_t *self = self_in;
    qstr mode = self->repeat ? MP_QSTR_PERIODIC : MP_QSTR_ONE_SHOT;

    // Virtual timer
    if (self->type == 0) {
        mp_printf(print, "Timer(-1, mode=%q, period=%lu)", mode, self->period);
        return;
    }

    // Hardware timer
    uint64_t period = self->period / (machine_timer_freq_hz() / 1000); // convert to ms
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

    // Virtual timers are -1, then 0, 1... for physical timers
    mp_int_t timer_id = -1;
    machine_timer_obj_t *self = NULL;

    timer_id = mp_obj_get_int(args[0]);
    if (timer_id == -1) {
        // Borrowed from Zephyr implementation of timer
        self = mp_obj_malloc_with_finaliser(machine_timer_obj_t, &machine_timer_type);

        // Add the timer to the linked-list of timers
        self->next = MP_STATE_PORT(machine_timer_obj_head);
        MP_STATE_PORT(machine_timer_obj_head) = self;

        self->type = 0; // Virtual timer
        self->vtimer = NULL;
        self->v_start_tick = 0;
    } else {
        // Create the new timer.
        uint32_t timer_number = mp_obj_get_int(args[0]);
        if (timer_number >= SOC_TIMER_GROUP_TOTAL_TIMERS) {
            //mp_raise_ValueError(MP_ERROR_TEXT("invalid Timer number, out of range"));
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid Timer number, max %d"), SOC_TIMER_GROUP_TOTAL_TIMERS);
        }
        // Find existing hw timer, if none allocate and add to the linked list
        self = machine_timer_create(timer_number);
        self->type = 1; // hardware timer  
    }

    // If there are any arguments to Timer(id,...) then call init to set them.
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

    PERIPH_RCC_ACQUIRE_ATOMIC(timer_group_periph_signals.groups[self->index].module, ref_count) {
        if (ref_count == 0) {
            timer_ll_enable_bus_clock(self->index, true);
            timer_ll_reset_register(self->index);
        }
    }

    timer_ll_enable_counter(self->hal_context.dev, self->index, false);
    esp_clk_tree_enable_src(TIMER_CLK_SRC, true);
    #if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 5, 0)
    timer_ll_set_clock_source(self->hal_context.dev, self->index, TIMER_CLK_SRC);
    timer_ll_enable_clock(self->hal_context.dev, self->index, true);
    #else
    timer_ll_set_clock_source(self->group, self->index, TIMER_CLK_SRC);
    timer_ll_enable_clock(self->group, self->index, true);
    #endif
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


static mp_obj_t machine_timer_virtualtimer_del(mp_obj_t self_in) {
    machine_timer_obj_t *self = self_in;
    machine_timer_obj_t *prev = NULL;

    // remove our timer from the linked list
    for (machine_timer_obj_t *_timer = MP_STATE_PORT(machine_timer_obj_head); _timer != NULL; _timer = _timer->next) {
        if (_timer == self) {
            // Remove our timer from the list
            if (prev != NULL) {
                prev->next = _timer->next;
            } else {
                MP_STATE_PORT(machine_timer_obj_head) = _timer->next;
            }

            // Remove memory allocation
            if (self->vtimer != NULL) {
                xTimerDelete(self->vtimer, 0);
            }
            m_del_obj(machine_timer_obj_t, self);
            break;
        } else {
            prev = _timer;
        }
    }

    return mp_const_none;
}

static void machine_timer_virtualtimer_callback(TimerHandle_t timer) {
    machine_timer_obj_t *self = (machine_timer_obj_t *)( pvTimerGetTimerID(timer) );

    // For timers that will repeat, update the start time as now
    // Non-repeating then set the timer counter to zero so value gets fail.
    if (self->repeat == 0)
    {
        self->v_start_tick = 0;
    } else {
        self->v_start_tick = xTaskGetTickCount();
    }

    if (self->callback != mp_const_none) {
        mp_sched_schedule(self->callback, MP_OBJ_FROM_PTR(self));
    }
}

static mp_obj_t machine_timer_init_helper(machine_timer_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {
        ARG_mode,
        ARG_callback,
        ARG_period,
        ARG_tick_hz,
        ARG_freq,
        ARG_hard,
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
        { MP_QSTR_hard,         MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };

    if (self->type == 0 && self->vtimer != NULL) {
        xTimerStop(self->vtimer, 0);
    } else {
        machine_timer_disable(self);
    }

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_hard].u_bool) {
        mp_raise_ValueError(MP_ERROR_TEXT("hard Timers are not implemented"));
    }

    // NOTE: For soft timers (period in ticks) is converted to OS ticks, this is a compile time
    // constant defined in FreeRTOSConfig.h as CONFIG_TICK_RATE_HZ (typically 100Hz or 1 tick every 10ms)
    // however its recommended to set this to 1000Hz (1ms tick) for better resolution by adding to
    // the board sdkconfig.defaults file.

    uint64_t vperiod = 0;
    #if MICROPY_PY_BUILTINS_FLOAT
    if (args[ARG_freq].u_obj != mp_const_none) {
        self->period = (uint64_t)(machine_timer_freq_hz() / mp_obj_get_float(args[ARG_freq].u_obj));
        vperiod = pdMS_TO_TICKS(1000.0 / mp_obj_get_float(args[ARG_freq].u_obj));
    }
    #else
    if (args[ARG_freq].u_int != 0xffffffff) {
        self->period = TIMER_SCALE / ((uint64_t)args[ARG_freq].u_int);
        vperiod = pdMS_TO_TICKS(1000 / args[ARG_freq].u_int);
    }
    #endif
    else {
        self->period = (((uint64_t)args[ARG_period].u_int) * machine_timer_freq_hz()) / args[ARG_tick_hz].u_int;
        vperiod = pdMS_TO_TICKS(args[ARG_period].u_int);
    }

    self->repeat = args[ARG_mode].u_int;
    self->callback = args[ARG_callback].u_obj;

    // Virtual timer
    if (self->type == 0) {
        self->handler = NULL;
        self->period = vperiod;

        if (vperiod == 0) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("period too small for soft timer, less than OS tick period of %d ms.  Try a hardware timer."), 1000 / configTICK_RATE_HZ);
        }

        self->vtimer = xTimerCreate(
            "MyTimer",                          // Timer name
            vperiod,                            // Timer period in ticks
            self->repeat ? pdTRUE : pdFALSE,    // Auto-reload
            (void *)self,                       // Timer ID
            machine_timer_virtualtimer_callback // Callback function
            );

        if (self->vtimer == NULL) {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("failed to create timer, internal python system error."));
        }

        // Start the timer, and record the start tick count
        self->v_start_tick = xTaskGetTickCount();
        xTimerStart(self->vtimer, 0);

    } else {
        // Hardware timer
        self->handler = machine_timer_isr_handler;
        machine_timer_enable(self);
    }

    return mp_const_none;
}

static mp_obj_t machine_timer_deinit(mp_obj_t self_in) {
    machine_timer_obj_t *self = self_in;

    // Virtual timer
    if (self->type == 0) {
        if (self->vtimer == NULL) {
            mp_raise_ValueError(MP_ERROR_TEXT("timer not set"));
        }
        xTimerStop(self->vtimer, 0);
        self->v_start_tick = 0;
        return mp_const_none;
    }

    // Hardware timer
    machine_timer_disable(self);
    if (self->handle) {
        ESP_ERROR_CHECK(esp_intr_free(self->handle));
        self->handle = NULL;
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_deinit_obj, machine_timer_deinit);


// Delete the object and free memory
static mp_obj_t machine_timer_del(mp_obj_t self_in) {
    machine_timer_obj_t *self = self_in;

    // Virtual timer, delete it
    if (self->type == 0) {
        machine_timer_virtualtimer_del(self);
        return mp_const_none;
    }

    // Hardware timer we dont do anything, just disable it
    machine_timer_deinit(self);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_del_obj, machine_timer_del);


static mp_obj_t machine_timer_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_timer_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_timer_init_obj, 1, machine_timer_init);

static mp_obj_t machine_timer_value(mp_obj_t self_in) {
    machine_timer_obj_t *self = self_in;

    // Virtual timers dont have a wauy to get the current value, so use executing task tick count
    if (self->type == 0) {
        int ticks = 0;

        if (self->vtimer == NULL || self->v_start_tick == 0) {
            mp_raise_ValueError(MP_ERROR_TEXT("timer not set"));
        }

        ticks = (int)(self->period - (xTaskGetTickCount() - self->v_start_tick));

        return MP_OBJ_NEW_SMALL_INT(ticks);
    }

    // Hardware timers use the timer counter value
    if (self->handle == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("timer not set"));
    }
    uint64_t result = timer_ll_get_counter_value(self->hal_context.dev, self->index);
    return MP_OBJ_NEW_SMALL_INT((mp_uint_t)(result / (machine_timer_freq_hz() / 1000))); // value in ms
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_value_obj, machine_timer_value);

static const mp_rom_map_elem_t machine_timer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&machine_timer_del_obj) },
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
