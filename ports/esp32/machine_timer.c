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

#include "machine_timer.h"
#include "esp_clk_tree.h"

#if !MICROPY_ENABLE_FINALISER
#error "machine.Timer requires MICROPY_ENABLE_FINALISER."
#endif

#define TIMER_CLK_SRC GPTIMER_CLK_SRC_DEFAULT
#define TIMER_DIVIDER  8

const mp_obj_type_t machine_timer_type;

uint32_t machine_timer_freq_hz(machine_timer_obj_t *self) {
    if (self->id >= 0) {
        // The timer source clock is APB or a fixed PLL (depending on chip), both constant frequency.
        uint32_t freq;
        check_esp_err(esp_clk_tree_src_get_freq_hz(TIMER_CLK_SRC, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &freq));
        assert(freq % TIMER_DIVIDER == 0); // Source clock should divide evenly into TIMER_DIVIDER
        return freq / TIMER_DIVIDER;
    }

    // Virtual timers always use microsecond resolution
    return 1000000;
}

static bool machine_timer_isr_hardware(gptimer_handle_t timer, const gptimer_alarm_event_data_t *event_data, void *self_in) {
    machine_timer_obj_t *self = self_in;
    return self->handler(self);
}

static void machine_timer_isr_virtual(void *self_in) {
    machine_timer_obj_t *self = self_in;
    if (self->repeat) {
        self->virtual_started = esp_timer_get_time();
    } else {
        self->virtual_stopped = esp_timer_get_time();
    }
    if (self->handler(self)) {
        #if CONFIG_ESP_TIMER_SUPPORTS_ISR_DISPATCH_METHOD
        esp_timer_isr_dispatch_need_yield();
        #endif
    }
}

static bool machine_timer_handler(machine_timer_obj_t *self) {
    mp_sched_schedule(self->handler_ctx, self);
    mp_hal_wake_main_task_from_isr();
    // Above function already yields, thus we return false
    // so the timer ISR doesn't needlessly yield again
    return false;
}

machine_timer_obj_t *machine_timer_create(mp_int_t id) {
    // Check hardware timer number is valid
    if (id >= SOC_TIMER_GROUP_TOTAL_TIMERS) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Timer(%d) doesn't exist, there are only %d hardware timers"), id, SOC_TIMER_GROUP_TOTAL_TIMERS);
    }

    // Check whether this hardware timer is already initialized, if so reuse it
    if (id >= 0) {
        for (machine_timer_obj_t *t = MP_STATE_PORT(machine_timer_obj_head); t; t = t->next) {
            if (t->id == id) {
                return t;
            }
        }
    }

    // To ensure numbering stays consistent, we need to enforce
    // the hardware timers are always created in the same order
    if (id > 0) {
        machine_timer_create(id - 1);
    }

    machine_timer_obj_t *self = mp_obj_malloc_with_finaliser(machine_timer_obj_t, &machine_timer_type);
    self->id = id;
    self->period = 0;
    self->virtual_started = 0;
    self->virtual_stopped = 0;
    self->handler = NULL;
    self->handler_ctx = NULL;

    if (id >= 0) {
        const gptimer_config_t hardware_config = {
            .clk_src = GPTIMER_CLK_SRC_DEFAULT,
            .direction = GPTIMER_COUNT_UP,
            .resolution_hz = machine_timer_freq_hz(self),
        };
        check_esp_err(gptimer_new_timer(&hardware_config, &self->handle.hardware));
        gptimer_event_callbacks_t hardware_event_callbacks = {
            .on_alarm = machine_timer_isr_hardware
        };
        esp_err_t result = gptimer_register_event_callbacks(self->handle.hardware, &hardware_event_callbacks, (void *)self);
        if (result != ESP_OK) {
            gptimer_del_timer(self->handle.hardware);
            check_esp_err(result);
        }

        // Hardware timers are immediately added to the
        // linked-list of timers so they can be reused
        self->next = MP_STATE_PORT(machine_timer_obj_head);
        MP_STATE_PORT(machine_timer_obj_head) = self;
    } else {
        const esp_timer_create_args_t virtual_args = {
            .callback = machine_timer_isr_virtual,
            .arg = (void *)self,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "mpy_machine_timer",
            .skip_unhandled_events = true,
        };
        check_esp_err(esp_timer_create(&virtual_args, &self->handle.virtual));
    }

    return self;
}

void machine_timer_configure(machine_timer_obj_t *self) {
    // Period must be non-zero
    if (self->period == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("Timer period is too short for this timer"));
    }

    if (self->id >= 0) {
        gptimer_alarm_config_t alarm_config = {
            .reload_count = 0,
            .alarm_count = self->period,
            .flags.auto_reload_on_alarm = self->repeat,
        };
        check_esp_err(gptimer_set_alarm_action(self->handle.hardware, &alarm_config));
        esp_err_t result = gptimer_enable(self->handle.hardware);
        if (result != ESP_ERR_INVALID_STATE) {
            check_esp_err(result);
        }
    } else {
        // Virtual timers need no configuration but should be
        // be added to the linked-list of timers so they are
        // not automatically garbage collected while running
        for (machine_timer_obj_t *t = MP_STATE_PORT(machine_timer_obj_head); t; t = t->next) {
            if (t == self) {
                return;
            }
        }
        self->next = MP_STATE_PORT(machine_timer_obj_head);
        MP_STATE_PORT(machine_timer_obj_head) = self;
    }
}

void machine_timer_start(machine_timer_obj_t *self) {
    if (self->id >= 0) {
        check_esp_err(gptimer_set_raw_count(self->handle.hardware, 0));
        check_esp_err(gptimer_start(self->handle.hardware));
    } else {
        self->virtual_started = esp_timer_get_time();
        if (self->repeat) {
            check_esp_err(esp_timer_start_periodic(self->handle.virtual, self->period));
        } else {
            check_esp_err(esp_timer_start_once(self->handle.virtual, self->period));
        }
    }
}

void machine_timer_stop(machine_timer_obj_t *self) {
    esp_err_t result;
    if (self->id >= 0) {
        result = gptimer_stop(self->handle.hardware);
    } else {
        result = esp_timer_stop(self->handle.virtual);
        if (result == ESP_OK) {
            self->virtual_stopped = esp_timer_get_time();
        }
    }
    if (result != ESP_ERR_INVALID_STATE) {
        check_esp_err(result);
    }
}

mp_obj_t machine_timer_deinit(mp_obj_t self_in) {
    machine_timer_obj_t *self = self_in;
    machine_timer_stop(self);
    if (self->id >= 0) {
        esp_err_t result = gptimer_disable(self->handle.hardware);
        if (result != ESP_ERR_INVALID_STATE) {
            check_esp_err(result);
        }
    } else {
        // Virtual timers may be immediately garbage collected,
        // hardware timers must stay in the list to be reused
        for (machine_timer_obj_t **t = &MP_STATE_PORT(machine_timer_obj_head); *t != NULL; t = &(*t)->next) {
            if (*t == self) {
                *t = self->next;
                break;
            }
        }
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_deinit_obj, machine_timer_deinit);

// Called by board port soft reset routine to deactivate all timers before reboot.
void machine_timer_deinit_all(void) {
    machine_timer_obj_t **t = &MP_STATE_PORT(machine_timer_obj_head);
    while (*t != NULL) {
        machine_timer_obj_t *next = (*t)->next;
        machine_timer_deinit(*t);
        *t = next;
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

    // Before (re)initializing, ensure timer is stopped and disabled
    machine_timer_deinit(self);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_hard].u_bool) {
        mp_raise_ValueError(MP_ERROR_TEXT("hard Timers are not implemented"));
    }

    #if MICROPY_PY_BUILTINS_FLOAT
    if (args[ARG_freq].u_obj != mp_const_none) {
        self->period = (uint64_t)(machine_timer_freq_hz(self) / mp_obj_get_float(args[ARG_freq].u_obj));
    }
    #else
    if (args[ARG_freq].u_int != 0xffffffff) {
        self->period = machine_timer_freq_hz(self) / ((uint64_t)args[ARG_freq].u_int);
    }
    #endif
    else {
        self->period = (((uint64_t)args[ARG_period].u_int) * machine_timer_freq_hz(self)) / args[ARG_tick_hz].u_int;
    }

    self->repeat = args[ARG_mode].u_int;

    self->handler = machine_timer_handler;
    self->handler_ctx = args[ARG_callback].u_obj;

    machine_timer_configure(self);
    machine_timer_start(self);

    return mp_const_none;
}

static mp_obj_t machine_timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // Get timer ID, or default to -1 (virtual)
    mp_int_t id = -1;
    if (n_args > 0) {
        id = mp_obj_get_int(args[0]);
        --n_args;
        ++args;
    }

    machine_timer_obj_t *self = machine_timer_create(id);

    if (n_args > 0 || n_kw > 0) {
        // Start the timer
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_timer_init_helper(self, n_args, args, &kw_args);
    }

    return self;
}

static mp_obj_t machine_timer_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_timer_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_timer_init_obj, 1, machine_timer_init);

static mp_obj_t machine_timer_del(mp_obj_t self_in) {
    machine_timer_obj_t *self = self_in;

    machine_timer_deinit(self);
    if (self->id >= 0) {
        // Hardware timers must first be removed from the linked-list of timers,
        // virtual timers will already have been removed during deinitialization
        for (machine_timer_obj_t **t = &MP_STATE_PORT(machine_timer_obj_head); *t != NULL; t = &(*t)->next) {
            if (*t == self) {
                *t = self->next;
                break;
            }
        }
        check_esp_err(gptimer_del_timer(self->handle.hardware));
    } else {
        check_esp_err(esp_timer_delete(self->handle.virtual));
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_del_obj, machine_timer_del);

static mp_obj_t machine_timer_value(mp_obj_t self_in) {
    machine_timer_obj_t *self = self_in;
    uint64_t result;
    if (self->id >= 0) {
        check_esp_err(gptimer_get_raw_count(self->handle.hardware, &result));
    } else {
        if (esp_timer_is_active(self->handle.virtual)) {
            result = esp_timer_get_time() - self->virtual_started;
        } else {
            result = self->virtual_stopped - self->virtual_started;
        }
    }
    return MP_OBJ_NEW_SMALL_INT((mp_uint_t)(result / (machine_timer_freq_hz(self) / 1000))); // value in ms
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_value_obj, machine_timer_value);

static void machine_timer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_timer_obj_t *self = self_in;
    qstr mode = self->repeat ? MP_QSTR_ONE_SHOT : MP_QSTR_PERIODIC;
    uint64_t period = self->period / (machine_timer_freq_hz(self) / 1000); // convert to ms
    mp_printf(print, "Timer(%d, mode=%q, period=%lu)", self->id, mode, period);
}

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
