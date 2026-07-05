/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021-22 Jonathan Hogg
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
#include "py/obj.h"

#if MICROPY_PY_ESP32_PCNT

#include "shared/runtime/mpirq.h"

#include "modesp32.h"
#include "driver/pulse_cnt.h"

#if !MICROPY_ENABLE_FINALISER
#error "esp32.PCNT requires MICROPY_ENABLE_FINALISER."
#endif

#define PCNT_PIN_NOT_USED -1

#define PCNT_EVT_ZERO     (1 << 0)
#define PCNT_EVT_THRES_0  (1 << 1)
#define PCNT_EVT_THRES_1  (1 << 2)
#define PCNT_EVT_L_LIM    (1 << 3)
#define PCNT_EVT_H_LIM    (1 << 4)

typedef struct _esp32_pcnt_irq_obj_t {
    mp_irq_obj_t base;
    uint32_t flags;
    uint32_t trigger;
} esp32_pcnt_irq_obj_t;

typedef struct _esp32_pcnt_obj_t {
    mp_obj_base_t base;
    pcnt_unit_handle_t unit;
    pcnt_channel_handle_t channels[2];
    int id;
    esp32_pcnt_irq_obj_t *irq;
    struct _esp32_pcnt_obj_t *next;
} esp32_pcnt_obj_t;

// Linked list of PCNT units.
MP_REGISTER_ROOT_POINTER(struct _esp32_pcnt_obj_t *esp32_pcnt_obj_head);

static mp_obj_t esp32_pcnt_deinit(mp_obj_t self_in);

void esp32_pcnt_deinit_all(void) {
    esp32_pcnt_obj_t **pcnt = &MP_STATE_PORT(esp32_pcnt_obj_head);
    while (*pcnt != NULL) {
        esp32_pcnt_deinit(MP_OBJ_FROM_PTR(*pcnt));
        *pcnt = (*pcnt)->next;
    }
}

static void esp32_pcnt_init_helper(esp32_pcnt_obj_t *self, size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {
        ARG_channel,
        ARG_pin,
        ARG_rising,
        ARG_falling,
        ARG_mode_pin,
        ARG_mode_low,
        ARG_mode_high,
        ARG_min,
        ARG_max,
        ARG_filter,
        ARG_threshold0,
        ARG_threshold1,
        ARG_value,
    };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_channel,     MP_ARG_KW_ONLY | MP_ARG_INT,   {.u_int = 0} },
        // Applies to the channel.
        { MP_QSTR_pin,         MP_ARG_KW_ONLY | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_rising,      MP_ARG_KW_ONLY | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_falling,     MP_ARG_KW_ONLY | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mode_pin,    MP_ARG_KW_ONLY | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mode_low,    MP_ARG_KW_ONLY | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mode_high,   MP_ARG_KW_ONLY | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
        // Applies to the whole unit.
        { MP_QSTR_min,         MP_ARG_KW_ONLY | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_max,         MP_ARG_KW_ONLY | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_filter,      MP_ARG_KW_ONLY | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_threshold0,  MP_ARG_KW_ONLY | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_threshold1,  MP_ARG_KW_ONLY | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
        // Implicitly zero if min, max, threshold0/1 are set.
        { MP_QSTR_value,       MP_ARG_KW_ONLY | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_pos_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    pcnt_unit_disable(self->unit);
    
    // The pin/mode_pin, rising, falling, mode_low, mode_high args all apply
    // to the channel (defaults to channel zero).
    mp_uint_t channel = args[ARG_channel].u_int;
    if (channel >= 2) {
        mp_raise_ValueError(MP_ERROR_TEXT("channel"));
    }

    if (args[ARG_pin].u_obj != MP_OBJ_NULL || args[ARG_mode_pin].u_obj != MP_OBJ_NULL) {
        // If you set mode_pin, you must also set pin.
        if (args[ARG_pin].u_obj == MP_OBJ_NULL) {
            mp_raise_ValueError(MP_ERROR_TEXT("pin"));
        }

        mp_hal_pin_obj_t pin = PCNT_PIN_NOT_USED;
        mp_hal_pin_obj_t mode_pin = PCNT_PIN_NOT_USED;

        // Set to None to disable pin/mode_pin.
        if (args[ARG_pin].u_obj != mp_const_none) {
            pin = mp_hal_get_pin_obj(args[ARG_pin].u_obj);
        }
        if (args[ARG_mode_pin].u_obj != MP_OBJ_NULL && args[ARG_mode_pin].u_obj != mp_const_none) {
            mode_pin = mp_hal_get_pin_obj(args[ARG_mode_pin].u_obj);
        }

        if (self->channels[channel] != NULL) {
            pcnt_del_channel(self->channels[channel]);
            self->channels[channel] = NULL;
        }

        pcnt_chan_config_t chan_config = {
            .edge_gpio_num = pin,
            .level_gpio_num = mode_pin,
        };
        check_esp_err(pcnt_new_channel(self->unit, &chan_config, &self->channels[channel]));
    }

    if (
        args[ARG_rising].u_obj != MP_OBJ_NULL || args[ARG_falling].u_obj != MP_OBJ_NULL ||
        args[ARG_mode_low].u_obj != MP_OBJ_NULL || args[ARG_mode_high].u_obj != MP_OBJ_NULL
        ) {
        if (self->channels[channel] == NULL) {
            mp_raise_ValueError(MP_ERROR_TEXT("channel not initialized"));
        }
        mp_uint_t rising = args[ARG_rising].u_obj == MP_OBJ_NULL ? PCNT_CHANNEL_EDGE_ACTION_HOLD : mp_obj_get_int(args[ARG_rising].u_obj);
        mp_uint_t falling = args[ARG_falling].u_obj == MP_OBJ_NULL ? PCNT_CHANNEL_EDGE_ACTION_HOLD : mp_obj_get_int(args[ARG_falling].u_obj);
        mp_uint_t mode_low = args[ARG_mode_low].u_obj == MP_OBJ_NULL ? PCNT_CHANNEL_LEVEL_ACTION_KEEP : mp_obj_get_int(args[ARG_mode_low].u_obj);
        mp_uint_t mode_high = args[ARG_mode_high].u_obj == MP_OBJ_NULL ? PCNT_CHANNEL_LEVEL_ACTION_KEEP : mp_obj_get_int(args[ARG_mode_high].u_obj);
        check_esp_err(pcnt_channel_set_edge_action(self->channels[channel], rising, falling));
        check_esp_err(pcnt_channel_set_level_action(self->channels[channel], mode_high, mode_low));
    }
    
    // The rest of the arguments apply to the whole unit.

    if (args[ARG_filter].u_obj != MP_OBJ_NULL) {
        mp_uint_t filter = mp_obj_get_int(args[ARG_filter].u_obj);
        if (filter > 1023) {
            mp_raise_ValueError(MP_ERROR_TEXT("filter"));
        }
        if (filter) {
            pcnt_glitch_filter_config_t filter_config = {
                .max_glitch_ns = filter * 1000,
            };
            check_esp_err(pcnt_unit_set_glitch_filter(self->unit, &filter_config));
        } else {
            check_esp_err(pcnt_unit_set_glitch_filter(self->unit, &(pcnt_glitch_filter_config_t) {0}));
        }
    }

    bool clear = false;
    if (args[ARG_value].u_obj != MP_OBJ_NULL) {
        mp_int_t value = mp_obj_get_int(args[ARG_value].u_obj);
        if (value != 0) {
            mp_raise_ValueError(MP_ERROR_TEXT("value"));
        }
        clear = true;
    }

    if (args[ARG_min].u_obj != MP_OBJ_NULL) {
        mp_int_t minimum = mp_obj_get_int(args[ARG_min].u_obj);
        if (minimum < -32768 || minimum > 0) {
            mp_raise_ValueError(MP_ERROR_TEXT("minimum"));
        }
        check_esp_err(pcnt_unit_add_watch_point(self->unit, minimum));
        clear = true;
    }

    if (args[ARG_max].u_obj != MP_OBJ_NULL) {
        mp_int_t maximum = mp_obj_get_int(args[ARG_max].u_obj);
        if (maximum < 0 || maximum > 32767) {
            mp_raise_ValueError(MP_ERROR_TEXT("maximum"));
        }
        check_esp_err(pcnt_unit_add_watch_point(self->unit, maximum));
        clear = true;
    }

    if (args[ARG_threshold0].u_obj != MP_OBJ_NULL) {
        mp_int_t threshold0 = mp_obj_get_int(args[ARG_threshold0].u_obj);
        if (threshold0 < -32768 || threshold0 > 32767) {
            mp_raise_ValueError(MP_ERROR_TEXT("threshold0"));
        }
        check_esp_err(pcnt_unit_add_watch_point(self->unit, threshold0));
        clear = true;
    }

    if (args[ARG_threshold1].u_obj != MP_OBJ_NULL) {
        mp_int_t threshold1 = mp_obj_get_int(args[ARG_threshold1].u_obj);
        if (threshold1 < -32768 || threshold1 > 32767) {
            mp_raise_ValueError(MP_ERROR_TEXT("threshold1"));
        }
        check_esp_err(pcnt_unit_add_watch_point(self->unit, threshold1));
        clear = true;
    }

    if (clear) {
        check_esp_err(pcnt_unit_clear_count(self->unit));
    }
}

// Disable any events, and remove the ISR handler for this unit.
static void esp32_pcnt_disable_events_for_unit(esp32_pcnt_obj_t *self) {
    if (!self->irq) {
        return;
    }

    pcnt_event_callbacks_t cbs = {0};
    check_esp_err(pcnt_unit_register_event_callbacks(self->unit, &cbs, NULL));
    self->irq->base.handler = mp_const_none;
    self->irq->trigger = 0;
}

static mp_obj_t esp32_pcnt_make_new(const mp_obj_type_t *type, size_t n_pos_args, size_t n_kw_args, const mp_obj_t *args) {
    if (n_pos_args < 1) {
        mp_raise_TypeError(MP_ERROR_TEXT("id"));
    }

    int id = mp_obj_get_int(args[0]);
    if (id < 0 || id >= 4) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid id"));
    }

    // Try and find an existing instance for this unit.
    esp32_pcnt_obj_t *self = MP_STATE_PORT(esp32_pcnt_obj_head);
    while (self) {
        if (self->id == id) {
            break;
        }
        self = self->next;
    }

    if (!self) {
        // Unused unit, create a new esp32_pcnt_obj_t instance and put it at
        // the head of the list.
        self = mp_obj_malloc(esp32_pcnt_obj_t, &esp32_pcnt_type);
        self->id = id;
        self->channels[0] = NULL;
        self->channels[1] = NULL;
        self->irq = NULL;
        self->next = MP_STATE_PORT(esp32_pcnt_obj_head);
        MP_STATE_PORT(esp32_pcnt_obj_head) = self;

        pcnt_unit_config_t unit_config = {
            .low_limit = -32768,
            .high_limit = 32767,
        };
        check_esp_err(pcnt_new_unit(&unit_config, &self->unit));
    }

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw_args, args + n_pos_args);
    esp32_pcnt_init_helper(self, 0, args + n_pos_args, &kw_args);



    return MP_OBJ_FROM_PTR(self);
}

static void esp32_pcnt_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    esp32_pcnt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "PCNT(%u)", self->id);
}

static mp_obj_t esp32_pcnt_init(size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    esp32_pcnt_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    esp32_pcnt_init_helper(self, n_pos_args - 1, pos_args + 1, kw_args);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(esp32_pcnt_init_obj, 1, esp32_pcnt_init);

static mp_obj_t esp32_pcnt_deinit(mp_obj_t self_in) {
    esp32_pcnt_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // Remove IRQ and events.
    esp32_pcnt_disable_events_for_unit(self);
    pcnt_unit_disable(self->unit);

    for (int i = 0; i < 2; i++) {
        if (self->channels[i] != NULL) {
            pcnt_del_channel(self->channels[i]);
            self->channels[i] = NULL;
        }
    }

    check_esp_err(pcnt_unit_set_glitch_filter(self->unit, &(pcnt_glitch_filter_config_t) {0}));
    check_esp_err(pcnt_unit_clear_count(self->unit));

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_pcnt_deinit_obj, esp32_pcnt_deinit);

static mp_obj_t esp32_pcnt_value(size_t n_args, const mp_obj_t *pos_args) {
    esp32_pcnt_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    // Optionally use pcnt.value(True) to clear the counter but only support a
    // value of zero. Note: This can lead to skipped counts.
    if (n_args == 2) {
        if (mp_obj_get_int(pos_args[1]) != 0) {
            mp_raise_ValueError(MP_ERROR_TEXT("value"));
        }
    }

    // This loop ensures that the caller's state (as inferred from IRQs, e.g.
    // under/overflow) corresponds to the returned value, by synchronously
    // flushing all pending IRQs.
    int count = 0;
    while (true) {
        check_esp_err(pcnt_unit_get_count(self->unit, &count));
        if (self->irq && self->irq->flags && self->irq->base.handler != mp_const_none) {
            // The handler must call irq.flags() to clear self->irq->base.flags,
            // otherwise this will be an infinite loop.
            mp_call_function_1(self->irq->base.handler, self->irq->base.parent);
        } else {
            break;
        }
    }

    if (n_args == 2) {
        // Value was given, and we've already checked it was zero, so clear
        // the counter.
        check_esp_err(pcnt_unit_clear_count(self->unit));
    }

    return MP_OBJ_NEW_SMALL_INT(count);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp32_pcnt_value_obj, 1, 2, esp32_pcnt_value);

static bool IRAM_ATTR pcnt_event_callback(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata, void *user_ctx) {
    esp32_pcnt_obj_t *self = (esp32_pcnt_obj_t *)user_ctx;
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    self->irq->flags |= edata->watch_point_value;
    MICROPY_END_ATOMIC_SECTION(atomic_state);
    mp_irq_handler(&self->irq->base);
    return false;
}

static mp_uint_t esp32_pcnt_irq_trigger(mp_obj_t self_in, mp_uint_t new_trigger) {
    esp32_pcnt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->irq->trigger = new_trigger;
    pcnt_event_callbacks_t cbs = {
        .on_reach = pcnt_event_callback,
    };
    check_esp_err(pcnt_unit_register_event_callbacks(self->unit, &cbs, self));
    return 0;
}

static mp_uint_t esp32_pcnt_irq_info(mp_obj_t self_in, mp_uint_t info_type) {
    esp32_pcnt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (info_type == MP_IRQ_INFO_FLAGS) {
        // Atomically get-and-clear the flags.
        mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
        mp_uint_t flags = self->irq->flags;
        self->irq->flags = 0;
        MICROPY_END_ATOMIC_SECTION(atomic_state);
        return flags;
    } else if (info_type == MP_IRQ_INFO_TRIGGERS) {
        return self->irq->trigger;
    }
    return 0;
}

static const mp_irq_methods_t esp32_pcnt_irq_methods = {
    .trigger = esp32_pcnt_irq_trigger,
    .info = esp32_pcnt_irq_info,
};

static mp_obj_t esp32_pcnt_irq(size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_handler, ARG_trigger };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_handler,  MP_ARG_OBJ,  {.u_obj = mp_const_none} },
        { MP_QSTR_trigger,  MP_ARG_INT,  {.u_int = PCNT_EVT_ZERO} },
    };

    esp32_pcnt_obj_t *self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_pos_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (!self->irq) {
        // Create IRQ object if necessary. This instance persists across a
        // de-init.
        self->irq = mp_obj_malloc(esp32_pcnt_irq_obj_t, &mp_irq_type);
        self->irq->base.methods = (mp_irq_methods_t *)&esp32_pcnt_irq_methods;
        self->irq->base.parent = MP_OBJ_FROM_PTR(self);
        self->irq->base.ishard = false;
        self->irq->base.handler = mp_const_none;
        self->irq->trigger = 0;
    }

    if (n_pos_args > 1 || kw_args->used != 0) {
        // Update IRQ data.

        mp_obj_t handler = args[ARG_handler].u_obj;
        mp_uint_t trigger = args[ARG_trigger].u_int;

        if (trigger < PCNT_EVT_ZERO || trigger >= (PCNT_EVT_ZERO << 1)) {
            mp_raise_ValueError(MP_ERROR_TEXT("trigger"));
        }

        if (handler != mp_const_none) {
            self->irq->base.handler = handler;
            self->irq->trigger = trigger;
            pcnt_event_callbacks_t cbs = {
                .on_reach = pcnt_event_callback,
            };
            check_esp_err(pcnt_unit_register_event_callbacks(self->unit, &cbs, self));
            esp32_pcnt_irq_trigger(MP_OBJ_FROM_PTR(self), trigger);
        } else {
            // Remove the ISR, disable all events, clear the IRQ object state.
            esp32_pcnt_disable_events_for_unit(self);
        }
    }

    return MP_OBJ_FROM_PTR(self->irq);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(esp32_pcnt_irq_obj, 1, esp32_pcnt_irq);

static mp_obj_t esp32_pcnt_start(mp_obj_t self_in) {
    esp32_pcnt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_esp_err(pcnt_unit_enable(self->unit));
    check_esp_err(pcnt_unit_start(self->unit));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_pcnt_start_obj, esp32_pcnt_start);

static mp_obj_t esp32_pcnt_stop(mp_obj_t self_in) {
    esp32_pcnt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_esp_err(pcnt_unit_disable(self->unit));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_pcnt_stop_obj, esp32_pcnt_stop);

static const mp_rom_map_elem_t esp32_pcnt_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_init),            MP_ROM_PTR(&esp32_pcnt_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_value),           MP_ROM_PTR(&esp32_pcnt_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq),             MP_ROM_PTR(&esp32_pcnt_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR_start),           MP_ROM_PTR(&esp32_pcnt_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop),            MP_ROM_PTR(&esp32_pcnt_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),          MP_ROM_PTR(&esp32_pcnt_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__),         MP_ROM_PTR(&esp32_pcnt_deinit_obj) },

    // Constants
    { MP_ROM_QSTR(MP_QSTR_IGNORE),          MP_ROM_INT(PCNT_CHANNEL_EDGE_ACTION_HOLD) },
    { MP_ROM_QSTR(MP_QSTR_INCREMENT),       MP_ROM_INT(PCNT_CHANNEL_EDGE_ACTION_INCREASE) },
    { MP_ROM_QSTR(MP_QSTR_DECREMENT),       MP_ROM_INT(PCNT_CHANNEL_EDGE_ACTION_DECREASE) },
    { MP_ROM_QSTR(MP_QSTR_NORMAL),          MP_ROM_INT(PCNT_CHANNEL_LEVEL_ACTION_KEEP) },
    { MP_ROM_QSTR(MP_QSTR_REVERSE),         MP_ROM_INT(PCNT_CHANNEL_LEVEL_ACTION_INVERSE) },
    { MP_ROM_QSTR(MP_QSTR_HOLD),            MP_ROM_INT(PCNT_CHANNEL_LEVEL_ACTION_HOLD) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_ZERO),        MP_ROM_INT(PCNT_EVT_ZERO) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_THRESHOLD0),  MP_ROM_INT(PCNT_EVT_THRES_0) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_THRESHOLD1),  MP_ROM_INT(PCNT_EVT_THRES_1) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_MIN),         MP_ROM_INT(PCNT_EVT_L_LIM) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_MAX),         MP_ROM_INT(PCNT_EVT_H_LIM) },
};
static MP_DEFINE_CONST_DICT(esp32_pcnt_locals_dict, esp32_pcnt_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    esp32_pcnt_type,
    MP_QSTR_PCNT,
    MP_TYPE_FLAG_NONE,
    make_new, esp32_pcnt_make_new,
    print, esp32_pcnt_print,
    locals_dict, &esp32_pcnt_locals_dict
    );

#endif // MICROPY_PY_ESP32_PCNT
