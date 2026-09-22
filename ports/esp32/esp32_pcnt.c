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
 *
 * ESP32 PCNT pulse counter, using the IDF "pulse_cnt" driver API (present
 * since IDF v5.2, and the only API from IDF v6 where the legacy driver/pcnt.h
 * was removed).
 *
 * Mapping from the legacy five-event model:
 *   - PCNT_EVT_ZERO    -> watch point at 0 (dedicated zero-cross event)
 *   - PCNT_EVT_THRES_0 -> watch point at threshold0 (threshold event slot 0)
 *   - PCNT_EVT_THRES_1 -> watch point at threshold1 (threshold event slot 1)
 *   - PCNT_EVT_L_LIM   -> watch point at the unit low limit  (= minimum)
 *   - PCNT_EVT_H_LIM   -> watch point at the unit high limit (= maximum)
 * The unit limits must be set to minimum/maximum for the low/high-limit
 * watch points to land on their dedicated hardware event slots, so the unit
 * is rebuilt if minimum/maximum change. The `id` passed to PCNT() is a
 * Python-level identity only; the driver allocates hardware units from a
 * pool.
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

// Event flag bits, matching the values of the removed legacy PCNT_EVT_* enum
// (which were also the values exposed to Python as esp32.PCNT.IRK_*).
#define PCNT_EVT_ZERO    (1 << 0)
#define PCNT_EVT_THRES_0 (1 << 1)
#define PCNT_EVT_THRES_1 (1 << 2)
#define PCNT_EVT_L_LIM   (1 << 3)
#define PCNT_EVT_H_LIM   (1 << 4)

// SOC_PCNT_UNITS_PER_GROUP was removed from soc_caps.h in IDF v6. Recreate
// the per-target unit count (values match the IDF v5 soc_caps.h). Used only
// to validate the user-supplied unit id.
#ifndef SOC_PCNT_UNITS_PER_GROUP
#if CONFIG_IDF_TARGET_ESP32
#define SOC_PCNT_UNITS_PER_GROUP (8)
#else
// All other supported targets (S2, S3, C2, C3, C5, C6, H2, P4, S31) have 4.
#define SOC_PCNT_UNITS_PER_GROUP (4)
#endif
#endif

#define PCNT_PIN_NOT_USED (-1)
#define PCNT_THRESHOLD_UNSET (INT_MIN)

typedef struct _esp32_pcnt_irq_obj_t {
    mp_irq_obj_t base;
    uint32_t flags;
    uint32_t trigger;
} esp32_pcnt_irq_obj_t;

// Remembered per-channel configuration so a unit can be rebuilt (with new
// limits) while keeping the channel setup.
typedef struct _esp32_pcnt_chan_state_t {
    int edge_gpio;
    int level_gpio;
    int pos_act;   // pcnt_channel_edge_action_t, -1 if never set
    int neg_act;
    int high_act;  // pcnt_channel_level_action_t, -1 if never set
    int low_act;
} esp32_pcnt_chan_state_t;

typedef struct _esp32_pcnt_obj_t {
    mp_obj_base_t base;
    int id;
    pcnt_unit_handle_t unit;
    pcnt_channel_handle_t channels[2];
    esp32_pcnt_chan_state_t chan_state[2];
    int low_limit;   // unit low limit (= minimum)
    int high_limit;  // unit high limit (= maximum)
    int threshold[2];
    bool enabled;
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

static bool IRAM_ATTR esp32_pcnt_event_callback(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata, void *user_ctx) {
    esp32_pcnt_obj_t *self = (esp32_pcnt_obj_t *)user_ctx;
    if (!self->irq || self->irq->base.handler == mp_const_none) {
        return false;
    }
    // Map the watch point value that fired back onto the Python-visible
    // event flag bits.
    uint32_t flag = 0;
    int value = edata->watch_point_value;
    if (value == 0) {
        flag = PCNT_EVT_ZERO;
    }
    if (self->threshold[0] != PCNT_THRESHOLD_UNSET && value == self->threshold[0]) {
        flag |= PCNT_EVT_THRES_0;
    }
    if (self->threshold[1] != PCNT_THRESHOLD_UNSET && value == self->threshold[1]) {
        flag |= PCNT_EVT_THRES_1;
    }
    if (self->low_limit != -32768 && value == self->low_limit) {
        flag |= PCNT_EVT_L_LIM;
    }
    if (self->high_limit != 32767 && value == self->high_limit) {
        flag |= PCNT_EVT_H_LIM;
    }
    if (flag) {
        mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
        self->irq->flags |= flag;
        MICROPY_END_ATOMIC_SECTION(atomic_state);
        mp_irq_handler(&self->irq->base);
    }
    return false;
}

// Add or remove watch points so that exactly the events in `trigger` are
// armed (mirrors the legacy pcnt_event_enable/disable semantics).
static void esp32_pcnt_set_watch_points(esp32_pcnt_obj_t *self, uint32_t trigger) {
    // Remove all five possible watch points first (ESP_ERR_INVALID_STATE is
    // returned for ones that were not added; ignore it).
    pcnt_unit_remove_watch_point(self->unit, 0);
    if (self->threshold[0] != PCNT_THRESHOLD_UNSET) {
        pcnt_unit_remove_watch_point(self->unit, self->threshold[0]);
    }
    if (self->threshold[1] != PCNT_THRESHOLD_UNSET) {
        pcnt_unit_remove_watch_point(self->unit, self->threshold[1]);
    }
    if (self->low_limit != -32768) {
        pcnt_unit_remove_watch_point(self->unit, self->low_limit);
    }
    if (self->high_limit != 32767) {
        pcnt_unit_remove_watch_point(self->unit, self->high_limit);
    }

    if (trigger & PCNT_EVT_ZERO) {
        check_esp_err(pcnt_unit_add_watch_point(self->unit, 0));
    }
    if ((trigger & PCNT_EVT_THRES_0) && self->threshold[0] != PCNT_THRESHOLD_UNSET) {
        check_esp_err(pcnt_unit_add_watch_point(self->unit, self->threshold[0]));
    }
    if ((trigger & PCNT_EVT_THRES_1) && self->threshold[1] != PCNT_THRESHOLD_UNSET) {
        check_esp_err(pcnt_unit_add_watch_point(self->unit, self->threshold[1]));
    }
    if ((trigger & PCNT_EVT_L_LIM) && self->low_limit != -32768) {
        check_esp_err(pcnt_unit_add_watch_point(self->unit, self->low_limit));
    }
    if ((trigger & PCNT_EVT_H_LIM) && self->high_limit != 32767) {
        check_esp_err(pcnt_unit_add_watch_point(self->unit, self->high_limit));
    }
}

// Tear down the hardware unit and channels (keeps the remembered state so
// esp32_pcnt_ensure_unit can rebuild).
static void esp32_pcnt_teardown_unit(esp32_pcnt_obj_t *self) {
    if (self->unit == NULL) {
        return;
    }
    for (int i = 0; i < 2; ++i) {
        if (self->channels[i] != NULL) {
            pcnt_del_channel(self->channels[i]);
            self->channels[i] = NULL;
        }
    }
    if (self->enabled) {
        pcnt_unit_stop(self->unit);
        pcnt_unit_disable(self->unit);
        self->enabled = false;
    }
    pcnt_del_unit(self->unit);
    self->unit = NULL;
}

// (Re)create the hardware unit (and any previously-configured channels),
// re-arm watch points for the active IRQ trigger, and restore the enabled
// state.
static void esp32_pcnt_ensure_unit(esp32_pcnt_obj_t *self) {
    if (self->unit != NULL) {
        return;
    }
    bool was_enabled = self->enabled;
    pcnt_unit_config_t unit_config = {
        .low_limit = self->low_limit,
        .high_limit = self->high_limit,
    };
    check_esp_err(pcnt_new_unit(&unit_config, &self->unit));

    pcnt_event_callbacks_t cbs = {
        .on_reach = esp32_pcnt_event_callback,
    };
    check_esp_err(pcnt_unit_register_event_callbacks(self->unit, &cbs, self));

    for (int i = 0; i < 2; ++i) {
        esp32_pcnt_chan_state_t *cs = &self->chan_state[i];
        if (cs->edge_gpio == PCNT_PIN_NOT_USED && cs->level_gpio == PCNT_PIN_NOT_USED) {
            continue;
        }
        pcnt_chan_config_t chan_config = {
            .edge_gpio_num = cs->edge_gpio,
            .level_gpio_num = cs->level_gpio,
        };
        check_esp_err(pcnt_new_channel(self->unit, &chan_config, &self->channels[i]));
        if (cs->pos_act >= 0) {
            check_esp_err(pcnt_channel_set_edge_action(self->channels[i],
                (pcnt_channel_edge_action_t)cs->pos_act, (pcnt_channel_edge_action_t)cs->neg_act));
        }
        if (cs->high_act >= 0) {
            check_esp_err(pcnt_channel_set_level_action(self->channels[i],
                (pcnt_channel_level_action_t)cs->high_act, (pcnt_channel_level_action_t)cs->low_act));
        }
    }

    if (self->irq && self->irq->trigger) {
        esp32_pcnt_set_watch_points(self, self->irq->trigger);
    }

    if (was_enabled) {
        check_esp_err(pcnt_unit_enable(self->unit));
    }
}

// Rebuild the unit if the requested limits differ from the current ones (the
// low/high-limit watch points only exist when they equal the unit limits).
static void esp32_pcnt_apply_limits(esp32_pcnt_obj_t *self, int minimum, int maximum) {
    if (minimum != self->low_limit || maximum != self->high_limit) {
        esp32_pcnt_teardown_unit(self);
        self->low_limit = minimum;
        self->high_limit = maximum;
        esp32_pcnt_ensure_unit(self);
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

        // A pin change rebuilds the channel.
        esp32_pcnt_chan_state_t *cs = &self->chan_state[channel];
        if (cs->edge_gpio != (int)pin || cs->level_gpio != (int)mode_pin) {
            if (self->channels[channel] != NULL) {
                pcnt_del_channel(self->channels[channel]);
                self->channels[channel] = NULL;
            }
            cs->edge_gpio = pin;
            cs->level_gpio = mode_pin;
            if (pin != PCNT_PIN_NOT_USED || mode_pin != PCNT_PIN_NOT_USED) {
                pcnt_chan_config_t chan_config = {
                    .edge_gpio_num = cs->edge_gpio,
                    .level_gpio_num = cs->level_gpio,
                };
                check_esp_err(pcnt_new_channel(self->unit, &chan_config, &self->channels[channel]));
            }
        }
    }

    if (
        args[ARG_rising].u_obj != MP_OBJ_NULL || args[ARG_falling].u_obj != MP_OBJ_NULL ||
        args[ARG_mode_low].u_obj != MP_OBJ_NULL || args[ARG_mode_high].u_obj != MP_OBJ_NULL
        ) {
        // The numeric values of the legacy PCNT_COUNT_*/PCNT_MODE_* constants
        // (exposed to Python) match the new pcnt_channel_*_action_t enums.
        mp_int_t rising = args[ARG_rising].u_obj == MP_OBJ_NULL ? PCNT_CHANNEL_EDGE_ACTION_HOLD : mp_obj_get_int(args[ARG_rising].u_obj);
        mp_int_t falling = args[ARG_falling].u_obj == MP_OBJ_NULL ? PCNT_CHANNEL_EDGE_ACTION_HOLD : mp_obj_get_int(args[ARG_falling].u_obj);
        mp_int_t mode_low = args[ARG_mode_low].u_obj == MP_OBJ_NULL ? PCNT_CHANNEL_LEVEL_ACTION_KEEP : mp_obj_get_int(args[ARG_mode_low].u_obj);
        mp_int_t mode_high = args[ARG_mode_high].u_obj == MP_OBJ_NULL ? PCNT_CHANNEL_LEVEL_ACTION_KEEP : mp_obj_get_int(args[ARG_mode_high].u_obj);
        if (rising < 0 || rising > PCNT_CHANNEL_EDGE_ACTION_DECREASE) {
            mp_raise_ValueError(MP_ERROR_TEXT("rising"));
        }
        if (falling < 0 || falling > PCNT_CHANNEL_EDGE_ACTION_DECREASE) {
            mp_raise_ValueError(MP_ERROR_TEXT("falling"));
        }
        if (mode_low < 0 || mode_low > PCNT_CHANNEL_LEVEL_ACTION_HOLD) {
            mp_raise_ValueError(MP_ERROR_TEXT("mode_low"));
        }
        if (mode_high < 0 || mode_high > PCNT_CHANNEL_LEVEL_ACTION_HOLD) {
            mp_raise_ValueError(MP_ERROR_TEXT("mode_high"));
        }
        if (self->channels[channel] == NULL) {
            mp_raise_ValueError(MP_ERROR_TEXT("channel not initialised"));
        }
        check_esp_err(pcnt_channel_set_edge_action(self->channels[channel],
            (pcnt_channel_edge_action_t)rising, (pcnt_channel_edge_action_t)falling));
        check_esp_err(pcnt_channel_set_level_action(self->channels[channel],
            (pcnt_channel_level_action_t)mode_high, (pcnt_channel_level_action_t)mode_low));
        esp32_pcnt_chan_state_t *cs = &self->chan_state[channel];
        cs->pos_act = rising;
        cs->neg_act = falling;
        cs->high_act = mode_high;
        cs->low_act = mode_low;
    }

    // The rest of the arguments apply to the whole unit.

    if (args[ARG_filter].u_obj != MP_OBJ_NULL) {
        mp_uint_t filter = mp_obj_get_int(args[ARG_filter].u_obj);
        if (filter > 1023) {
            mp_raise_ValueError(MP_ERROR_TEXT("filter"));
        }
        // The legacy API took the filter width in APB clock cycles (80MHz),
        // the new API takes nanoseconds.
        if (filter) {
            pcnt_glitch_filter_config_t filter_config = {
                .max_glitch_ns = filter * 25 / 2,
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

    // minimum/maximum set the unit limits; changing them rebuilds the unit.
    // Note: the new driver requires low_limit < 0 < high_limit, so unlike the
    // legacy API a minimum of 0 is not accepted.
    mp_int_t minimum = self->low_limit;
    mp_int_t maximum = self->high_limit;
    bool limits_changed = false;
    if (args[ARG_min].u_obj != MP_OBJ_NULL) {
        minimum = mp_obj_get_int(args[ARG_min].u_obj);
        if (minimum < -32768 || minimum >= 0) {
            mp_raise_ValueError(MP_ERROR_TEXT("minimum"));
        }
        limits_changed = true;
    }
    if (args[ARG_max].u_obj != MP_OBJ_NULL) {
        maximum = mp_obj_get_int(args[ARG_max].u_obj);
        if (maximum <= 0 || maximum > 32767) {
            mp_raise_ValueError(MP_ERROR_TEXT("maximum"));
        }
        limits_changed = true;
    }
    if (limits_changed) {
        esp32_pcnt_apply_limits(self, minimum, maximum);
    }

    if (args[ARG_threshold0].u_obj != MP_OBJ_NULL) {
        mp_int_t threshold0 = mp_obj_get_int(args[ARG_threshold0].u_obj);
        if (threshold0 < self->low_limit || threshold0 > self->high_limit) {
            mp_raise_ValueError(MP_ERROR_TEXT("threshold0"));
        }
        self->threshold[0] = threshold0;
        clear = true;
    }

    if (args[ARG_threshold1].u_obj != MP_OBJ_NULL) {
        mp_int_t threshold1 = mp_obj_get_int(args[ARG_threshold1].u_obj);
        if (threshold1 < self->low_limit || threshold1 > self->high_limit) {
            mp_raise_ValueError(MP_ERROR_TEXT("threshold1"));
        }
        self->threshold[1] = threshold1;
        clear = true;
    }

    // Re-arm the watch points if the trigger is active (threshold values or
    // limits may have changed).
    if (self->irq && self->irq->trigger) {
        esp32_pcnt_set_watch_points(self, self->irq->trigger);
    }

    if (clear) {
        check_esp_err(pcnt_unit_clear_count(self->unit));
    }
}

// Disable any events, and remove the IRQ handler for this unit.
static void esp32_pcnt_disable_events_for_unit(esp32_pcnt_obj_t *self) {
    if (!self->irq) {
        return;
    }

    // Remove all watch points and the callback.
    esp32_pcnt_set_watch_points(self, 0);

    // Clear IRQ object state.
    self->irq->base.handler = mp_const_none;
    self->irq->trigger = 0;
}

static mp_obj_t esp32_pcnt_make_new(const mp_obj_type_t *type, size_t n_pos_args, size_t n_kw_args, const mp_obj_t *args) {
    if (n_pos_args < 1) {
        mp_raise_TypeError(MP_ERROR_TEXT("id"));
    }

    int id = mp_obj_get_int(args[0]);
    if (id < 0 || id >= SOC_PCNT_UNITS_PER_GROUP) {
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
        self->unit = NULL;
        self->channels[0] = NULL;
        self->channels[1] = NULL;
        for (int i = 0; i < 2; ++i) {
            self->chan_state[i].edge_gpio = PCNT_PIN_NOT_USED;
            self->chan_state[i].level_gpio = PCNT_PIN_NOT_USED;
            self->chan_state[i].pos_act = -1;
            self->chan_state[i].neg_act = -1;
            self->chan_state[i].high_act = -1;
            self->chan_state[i].low_act = -1;
        }
        self->low_limit = -32768;
        self->high_limit = 32767;
        self->threshold[0] = PCNT_THRESHOLD_UNSET;
        self->threshold[1] = PCNT_THRESHOLD_UNSET;
        self->enabled = false;
        self->irq = NULL;
        self->next = MP_STATE_PORT(esp32_pcnt_obj_head);
        MP_STATE_PORT(esp32_pcnt_obj_head) = self;
    }

    // Ensure the unit is in a known (deactivated) state, then apply the
    // requested configuration.
    esp32_pcnt_deinit(MP_OBJ_FROM_PTR(self));
    esp32_pcnt_ensure_unit(self);

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
    esp32_pcnt_ensure_unit(self);
    esp32_pcnt_init_helper(self, n_pos_args - 1, pos_args + 1, kw_args);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(esp32_pcnt_init_obj, 1, esp32_pcnt_init);

static mp_obj_t esp32_pcnt_deinit(mp_obj_t self_in) {
    esp32_pcnt_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // Remove IRQ and events.
    esp32_pcnt_disable_events_for_unit(self);

    // Tear down the unit and channels, pause & clear.
    esp32_pcnt_teardown_unit(self);

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

static mp_uint_t esp32_pcnt_irq_trigger(mp_obj_t self_in, mp_uint_t new_trigger) {
    esp32_pcnt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->irq->trigger = new_trigger;
    esp32_pcnt_set_watch_points(self, new_trigger);
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

    // Re-create the hardware unit if it was torn down by a previous deinit.
    esp32_pcnt_ensure_unit(self);

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

        if (trigger < PCNT_EVT_ZERO || trigger >= (PCNT_EVT_H_LIM << 1)) {
            mp_raise_ValueError(MP_ERROR_TEXT("trigger"));
        }

        if (handler != mp_const_none) {
            self->irq->base.handler = handler;
            self->irq->trigger = trigger;
            esp32_pcnt_irq_trigger(MP_OBJ_FROM_PTR(self), trigger);
        } else {
            // Remove the handler, disable all events, clear the IRQ object state.
            esp32_pcnt_disable_events_for_unit(self);
        }
    }

    return MP_OBJ_FROM_PTR(self->irq);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(esp32_pcnt_irq_obj, 1, esp32_pcnt_irq);

static mp_obj_t esp32_pcnt_start(mp_obj_t self_in) {
    esp32_pcnt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->enabled) {
        check_esp_err(pcnt_unit_enable(self->unit));
        self->enabled = true;
    }
    check_esp_err(pcnt_unit_start(self->unit));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_pcnt_start_obj, esp32_pcnt_start);

static mp_obj_t esp32_pcnt_stop(mp_obj_t self_in) {
    esp32_pcnt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_esp_err(pcnt_unit_stop(self->unit));
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

    // Constants (numeric values match the legacy API, and are identical in
    // the new pcnt_channel_*_action_t enums).
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
