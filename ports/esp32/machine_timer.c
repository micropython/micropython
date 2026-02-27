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

#if CONFIG_IDF_TARGET_ESP32P4
static uint8_t __DECLARE_RCC_ATOMIC_ENV __attribute__ ((unused));
#endif
const mp_obj_type_t machine_timer_type;

static mp_obj_t machine_timer_init_helper(machine_timer_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
static mp_obj_t machine_timer_deinit(mp_obj_t self_in);

// Used both internally and by board port UART system
uint32_t machine_timer_freq_hz(void) {
    // The timer source clock is APB or a fixed PLL (depending on chip), both constant frequency.
    uint32_t freq;
    check_esp_err(esp_clk_tree_src_get_freq_hz(TIMER_CLK_SRC, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &freq));
    assert(freq % TIMER_DIVIDER == 0); // Source clock should divide evenly into TIMER_DIVIDER
    return freq / TIMER_DIVIDER;
}

// Called by board port soft reset routine to deactivate all timers before reboot.
void machine_timer_deinit_all(void) {
    // Disable all timers in list, gc will handle deletion of allocated python memory
    machine_timer_obj_t **t = &MP_STATE_PORT(machine_timer_obj_head);
    while (*t != NULL) {
        machine_timer_obj_t *next = (*t)->next;
        machine_timer_deinit(*t);
        *t = next;
    }
}

static void machine_timer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_timer_obj_t *self = self_in;
    qstr mode = self->repeat ? MP_QSTR_PERIODIC : MP_QSTR_ONE_SHOT;
    uint64_t period = 0;
    mp_uint_t timer_id = -1;

    if (self->is_virtual) {
        // Virtual timer
        timer_id = -1;

        // convert from us to ms
        period = self->period / 1000;
    } else {
        // Hardware timer
        #if SOC_TIMER_GROUP_TIMERS_PER_GROUP == 1
        timer_id = self->group;
        #else
        timer_id = (self->group << 1) | self->index;
        #endif

        // convert from machine timer value to ms
        period = self->period / (machine_timer_freq_hz() / 1000);
    }

    mp_printf(print, "Timer(%d, mode=%q, period=%lu)", timer_id, mode, (mp_int_t)period);
}

machine_timer_obj_t *machine_timer_create(mp_int_t timer_id) {
    machine_timer_obj_t *self = NULL;

    if (timer_id == -1) {
        // Virtual timer creation
        self = mp_obj_malloc_with_finaliser(machine_timer_obj_t, &machine_timer_type);

        self->is_virtual = true;
        self->vtimer = NULL;
        self->v_start_tick = 0;

        // Add the timer to the linked-list of timers
        self->next = MP_STATE_PORT(machine_timer_obj_head);
        MP_STATE_PORT(machine_timer_obj_head) = self;
    } else {
        // Hardware timer creation
        if (timer_id >= SOC_TIMER_GROUP_TOTAL_TIMERS) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid Timer number, max %d"), SOC_TIMER_GROUP_TOTAL_TIMERS);
        }

        // Find existing hw timer, if none allocate and add to the linked list
        #if SOC_TIMER_GROUP_TIMERS_PER_GROUP == 1
        mp_uint_t group = timer_id & 1;
        mp_uint_t index = 0;
        #else
        mp_uint_t group = (timer_id >> 1) & 1;
        mp_uint_t index = timer_id & 1;
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

        self->is_virtual = false;
    }

    // Default timer settings in case it is printed before initialization
    self->period = 0;
    self->repeat = false;

    return self;
}

static mp_obj_t machine_timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // Virtual timers are -1, then 0, 1... for physical timers
    mp_int_t timer_id = -1;
    machine_timer_obj_t *self = NULL;

    // Collect timer id if positional one was specified; else id=-1 is assumed for virtual timer
    if (n_args > 0) {
        timer_id = mp_obj_get_int(args[0]);
        --n_args;
        ++args;
    }

    self = machine_timer_create(timer_id);

    // If there are any arguments to Timer(id,...) then call init to set them.
    if (n_args > 0 || n_kw > 0) {
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_timer_init_helper(self, n_args, args, &kw_args);
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
                esp_timer_stop(self->vtimer);
                esp_timer_delete(self->vtimer);
            }
            m_del_obj(machine_timer_obj_t, self);
            break;
        } else {
            prev = _timer;
        }
    }

    return mp_const_none;
}

static void machine_timer_virtualtimer_callback(void *timer) {
    machine_timer_obj_t *self = (machine_timer_obj_t *)(timer);

    // For timers that will repeat, update the start time as now
    // Non-repeating then set the timer counter to zero so value gets fail.
    if (self->repeat) {
        self->v_start_tick = esp_timer_get_time();
    } else {
        self->v_start_tick = 0;
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

    if (self->is_virtual && self->vtimer != NULL) {
        esp_timer_stop(self->vtimer);
    } else {
        machine_timer_disable(self);
    }

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_hard].u_bool) {
        mp_raise_ValueError(MP_ERROR_TEXT("hard Timers are not implemented"));
    }

    // Calculate the period setting
    uint64_t vperiod = 0;
    uint64_t hperiod = 0;
    #if MICROPY_PY_BUILTINS_FLOAT
    if (args[ARG_freq].u_obj != mp_const_none) {
        hperiod = (uint64_t)(machine_timer_freq_hz() / mp_obj_get_float(args[ARG_freq].u_obj));
        // 1s as 1,0000,000 microseconds gives us the virtual timer tick for freq=n, 1M/n = m microseconds
        vperiod = 1000000.0 / mp_obj_get_float(args[ARG_freq].u_obj);
    }
    #else
    if (args[ARG_freq].u_int != 0xffffffff) {
        hperiod = TIMER_SCALE / ((uint64_t)args[ARG_freq].u_int);
        // 1s as 1,0000,000 microseconds gives us the virtual timer tick for freq=n, 1M/n = m microseconds
        vperiod = 1000000 / args[ARG_freq].u_int;
    }
    #endif
    else {
        hperiod = (((uint64_t)args[ARG_period].u_int) * machine_timer_freq_hz()) / args[ARG_tick_hz].u_int;
        // ms parameter converted to microseconds for esp timers
        vperiod = 1000 * args[ARG_period].u_int;
    }

    self->repeat = args[ARG_mode].u_int;
    self->callback = args[ARG_callback].u_obj;

    // Virtual timer
    if (self->is_virtual) {
        if (vperiod == 0) {
            mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("period too small for soft timer, less than OS tick period of 1 microsecond.  Try a hardware timer."));
        }

        esp_timer_create_args_t timer_args = {
            .name = "vtimer",
            .callback = machine_timer_virtualtimer_callback,
            .arg = (void *)self,
            .dispatch_method = ESP_TIMER_TASK,
            .skip_unhandled_events = true
        };

        esp_err_t esp_err = esp_timer_create(&timer_args, &self->vtimer);

        if (esp_err != ESP_OK || self->vtimer == NULL) {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("failed to create timer, internal python system error."));
        }

        // Start the timer, and record the start time for the time until expire timer call
        self->period = vperiod;
        self->handler = NULL;
        self->v_start_tick = esp_timer_get_time();

        if (self->repeat) {
            esp_timer_start_periodic(self->vtimer, vperiod);
        } else {
            esp_timer_start_once(self->vtimer, vperiod);
        }
    } else {
        // Hardware timer
        self->period = hperiod;
        self->handler = machine_timer_isr_handler;
        machine_timer_enable(self);
    }

    return mp_const_none;
}

static mp_obj_t machine_timer_deinit(mp_obj_t self_in) {
    machine_timer_obj_t *self = self_in;

    // Virtual timer
    if (self->is_virtual) {
        if (self->vtimer != NULL) {
            esp_timer_stop(self->vtimer);
        }
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

    // Virtual timers require deletion, and removal from the list
    if (self->is_virtual) {
        machine_timer_virtualtimer_del(self);
        return mp_const_none;
    }

    // Hardware timer just disable it, this promotes reuse without reallocation of memory.
    // At software reset gc will collect memory allocation.
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

    // Virtual timers dont have a way to get the current value, so use time since esp32 timer was started
    if (self->is_virtual) {
        int ticks = 0;

        if (self->vtimer == NULL || self->v_start_tick == 0) {
            mp_raise_ValueError(MP_ERROR_TEXT("timer not set"));
        }

        //  Take delta from last periodic timer, or start time for one-shot
        ticks = (int)(self->period - (esp_timer_get_time() - self->v_start_tick));

        // convert from microseconds to ms
        ticks = ticks / 1000;

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
