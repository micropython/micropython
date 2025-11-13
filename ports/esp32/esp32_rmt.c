/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 "Matt Trentini" <matt.trentini@gmail.com>
 * Copyright (c) 2024 "Elvis Pfützenreuter" <elvis.pfutzenreuter@gmail.com>
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
#include "py/stream.h"
#include "modmachine.h"
#include "modesp32.h"

#include "esp_task.h"

#if SOC_RMT_SUPPORTED
#include "esp_clk_tree.h"
#include "driver/rmt_tx.h"
#include "driver/rmt_encoder.h"

// This exposes the ESP32's RMT module to MicroPython. RMT is provided by the Espressif ESP-IDF:
//
//    https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/rmt.html
//
// With some examples provided:
//
//    https://github.com/espressif/arduino-esp32/tree/master/libraries/ESP32/examples/RMT
//
// RMT allows accurate (down to 12.5ns resolution) transmit - and receive - of pulse signals.
// Originally designed to generate infrared remote control signals, the module is very
// flexible and quite easy-to-use.
//
// This code exposes the RMT TX feature.

// Forward declaration
extern const mp_obj_type_t esp32_rmt_type;

typedef struct _esp32_rmt_obj_t {
    mp_obj_base_t base;
    rmt_channel_handle_t channel;
    bool enabled;
    gpio_num_t pin;
    uint32_t clock_freq;
    int resolution_hz;
    mp_uint_t cap_items;
    rmt_symbol_word_t *items;
    int loop_count;
    int tx_ongoing;

    rmt_encoder_handle_t encoder;
    mp_uint_t idle_level;
} esp32_rmt_obj_t;

// Decide RMT usage in the machine_bitstream_high_low_rmt implementation.
bool esp32_rmt_bitstream_enabled = true;

static bool IRAM_ATTR esp32_rmt_tx_trans_done(rmt_channel_handle_t channel, const rmt_tx_done_event_data_t *edata, void *user_ctx) {
    esp32_rmt_obj_t *self = user_ctx;
    self->tx_ongoing -= 1;
    return false;
}

static mp_obj_t esp32_rmt_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,                          MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_pin,       MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_resolution_hz,               MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_clock_div,                   MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_idle_level,                  MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} }, // low voltage
        { MP_QSTR_tx_carrier,                  MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} }, // no carrier
        { MP_QSTR_num_symbols,                 MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = SOC_RMT_MEM_WORDS_PER_CHANNEL} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    // RMT channel is an opaque struct in current RMT API and channel_id is a dummy parameter
    // mp_uint_t channel_id = args[0].u_int;
    gpio_num_t pin_id = machine_pin_get_id(args[1].u_obj);

    uint32_t clock_freq;
    check_esp_err(esp_clk_tree_src_get_freq_hz(RMT_CLK_SRC_DEFAULT, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &clock_freq));

    mp_uint_t resolution_hz;
    if (args[2].u_obj != mp_const_none && args[3].u_obj != mp_const_none) {
        mp_raise_ValueError(MP_ERROR_TEXT("resolution_hz and clock_div are mutually exclusive"));
    } else if (args[2].u_obj == mp_const_none && args[3].u_obj == mp_const_none) {
        // default value
        resolution_hz = 10000000;
    } else if (args[2].u_obj != mp_const_none) {
        resolution_hz = mp_obj_get_int(args[2].u_obj);
        if (resolution_hz <= 0) {
            mp_raise_ValueError(MP_ERROR_TEXT("resolution_hz must be positive"));
        }
    } else if (args[3].u_obj != mp_const_none) {
        mp_uint_t clock_div = mp_obj_get_int(args[3].u_obj);
        if (clock_div < 1 || clock_div > 255) {
            mp_raise_ValueError(MP_ERROR_TEXT("clock_div must be between 1 and 255"));
        }
        resolution_hz = clock_freq / clock_div;
    }

    mp_uint_t idle_level = args[4].u_bool;
    mp_obj_t tx_carrier_obj = args[5].u_obj;
    mp_uint_t num_symbols = args[6].u_int;

    if (num_symbols < SOC_RMT_MEM_WORDS_PER_CHANNEL || ((num_symbols % 2) == 1)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("num_symbols must be even and at least %d"), SOC_RMT_MEM_WORDS_PER_CHANNEL);
    }

    esp32_rmt_obj_t *self = mp_obj_malloc_with_finaliser(esp32_rmt_obj_t, &esp32_rmt_type);
    self->channel = NULL;
    self->pin = pin_id;
    self->clock_freq = clock_freq;
    self->resolution_hz = resolution_hz;
    self->loop_count = 0;
    self->tx_ongoing = 0;
    self->idle_level = idle_level;
    self->enabled = false;

    rmt_tx_channel_config_t tx_chan_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .gpio_num = self->pin,
        .mem_block_symbols = num_symbols,
        .resolution_hz = resolution_hz,
        .trans_queue_depth = 4,
    };

    check_esp_err(rmt_new_tx_channel(&tx_chan_config, &self->channel));

    if (tx_carrier_obj != mp_const_none) {
        mp_obj_t *tx_carrier_details = NULL;
        mp_obj_get_array_fixed_n(tx_carrier_obj, 3, &tx_carrier_details);
        mp_uint_t frequency = mp_obj_get_int(tx_carrier_details[0]);
        mp_uint_t duty = mp_obj_get_int(tx_carrier_details[1]);
        mp_uint_t level = mp_obj_is_true(tx_carrier_details[2]);

        if (frequency == 0) {
            mp_raise_ValueError(MP_ERROR_TEXT("tx_carrier frequency must be >0"));
        }
        if (duty > 100) {
            mp_raise_ValueError(MP_ERROR_TEXT("tx_carrier duty must be 0..100"));
        }

        rmt_carrier_config_t tx_carrier_cfg = {
            .duty_cycle = ((float)duty) / 100.0,
            .frequency_hz = frequency,
            .flags.polarity_active_low = !level,
        };
        check_esp_err(rmt_apply_carrier(self->channel, &tx_carrier_cfg));
    }

    rmt_copy_encoder_config_t copy_encoder_config = {};
    check_esp_err(rmt_new_copy_encoder(&copy_encoder_config, &self->encoder));

    rmt_tx_event_callbacks_t callbacks = {
        .on_trans_done = esp32_rmt_tx_trans_done,
    };
    check_esp_err(rmt_tx_register_event_callbacks(self->channel, &callbacks, self));

    return MP_OBJ_FROM_PTR(self);
}

static void esp32_rmt_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    esp32_rmt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->pin != -1) {
        mp_printf(print, "RMT(pin=%u, source_freq=%u, resolution_hz=%u, idle_level=%u)",
            self->pin, self->clock_freq, self->resolution_hz, self->idle_level);
    } else {
        mp_printf(print, "RMT()");
    }
}

static void esp32_rmt_deactivate(esp32_rmt_obj_t *self) {
    if (self->enabled) {
        // FIXME: panics in ESP32 if called while TX is ongoing and TX sequence is long (>300ms)
        // Does not panic in ESP32-S3, ESP32-C3 and ESP32-C6.
        // Tested with ESP-IDF up to 5.5
        // ESP-IDF issue: https://github.com/espressif/esp-idf/issues/17692
        //
        // Cause is Interrupt WDT to trigger because ESP-IDF rmt_disable() disables
        // interrupts and spinlocks until the ongoing TX sequence is finished.
        //
        // Workaround is never try to stop RMT sequences longer than 300ms (which are unusual
        // anyway). Or apply the patch mentioned at the GitHub issue to ESP-IDF.
        rmt_disable(self->channel);
        self->enabled = false;
    }
}

static mp_obj_t esp32_rmt_active(size_t n_args, const mp_obj_t *args) {
    esp32_rmt_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args == 1) {
        return mp_obj_new_bool(self->enabled && self->tx_ongoing > 0);
    } else if (mp_obj_is_true(args[1])) {
        mp_raise_ValueError(MP_ERROR_TEXT("activate by calling write_pulses()"));
    }

    esp32_rmt_deactivate(self);

    return mp_const_false;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp32_rmt_active_obj, 1, 2, esp32_rmt_active);

static mp_obj_t esp32_rmt_deinit(mp_obj_t self_in) {
    esp32_rmt_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->pin != -1) { // Check if channel has already been deinitialised.
        esp32_rmt_deactivate(self);
        rmt_tx_event_callbacks_t callbacks = {
            .on_trans_done = NULL,
        };
        rmt_tx_register_event_callbacks(self->channel, &callbacks, self);
        rmt_del_encoder(self->encoder);
        rmt_del_channel(self->channel);
        self->pin = -1; // -1 to indicate RMT is unused
        self->tx_ongoing = 0;
        m_free(self->items);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_rmt_deinit_obj, esp32_rmt_deinit);

// Return the source frequency.
// Currently only the default clock (80MHz) can be used but it is possible other
// clock sources will added in the future.
static mp_obj_t esp32_rmt_source_freq() {
    uint32_t clock_freq;
    check_esp_err(esp_clk_tree_src_get_freq_hz(RMT_CLK_SRC_DEFAULT, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &clock_freq));
    return mp_obj_new_int(clock_freq);
}
static MP_DEFINE_CONST_FUN_OBJ_0(esp32_rmt_source_freq_obj, esp32_rmt_source_freq);
static MP_DEFINE_CONST_STATICMETHOD_OBJ(esp32_rmt_source_obj, MP_ROM_PTR(&esp32_rmt_source_freq_obj));

// Return the clock divider.
static mp_obj_t esp32_rmt_clock_div(mp_obj_t self_in) {
    esp32_rmt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->pin == -1) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("already deinitialized"));
    }

    return mp_obj_new_int(self->clock_freq / self->resolution_hz);
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_rmt_clock_div_obj, esp32_rmt_clock_div);

// Query whether the channel has finished sending pulses. Takes an optional
// timeout (in milliseconds), returning true if the pulse stream has
// completed or false if they are still transmitting (or timeout is reached).
static mp_obj_t esp32_rmt_wait_done(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_self,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    esp32_rmt_obj_t *self = MP_OBJ_TO_PTR(args[0].u_obj);
    if (self->pin == -1) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("already deinitialized"));
    } else if (!self->enabled) {
        return mp_const_true;
    } else if (args[1].u_int == 0 && self->tx_ongoing > 0) {
        // shortcut to avoid console spamming with timeout msgs by rmt_tx_wait_all_done()
        return mp_const_false;
    }

    esp_err_t err = rmt_tx_wait_all_done(self->channel, args[1].u_int);
    return err == ESP_OK ? mp_const_true : mp_const_false;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(esp32_rmt_wait_done_obj, 1, esp32_rmt_wait_done);

static mp_uint_t esp32_rmt_stream_ioctl(
    mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    if (request != MP_STREAM_POLL) {
        *errcode = MP_EINVAL;
        return MP_STREAM_ERROR;
    }
    esp32_rmt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t ret = 0;
    if ((arg & MP_STREAM_POLL_WR) && self->tx_ongoing == 0) {
        ret |= MP_STREAM_POLL_WR;
    }
    return ret;
}

static const mp_stream_p_t esp32_rmt_stream_p = {
    .ioctl = esp32_rmt_stream_ioctl,
};

static void esp32_rmt_loop_in(esp32_rmt_obj_t *self, int new_loop_count) {
    if (self->enabled && self->tx_ongoing > 0 && self->loop_count != 0 && new_loop_count == 0) {
        // Break ongoing loop
        esp32_rmt_deactivate(self);
    }
    self->loop_count = new_loop_count;
}

static mp_obj_t esp32_rmt_loop(mp_obj_t self_in, mp_obj_t loop) {
    esp32_rmt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->pin == -1) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("already deinitialized"));
    }

    bool loop_en = mp_obj_get_int(loop);
    esp32_rmt_loop_in(self, loop_en ? -1 : 0);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(esp32_rmt_loop_obj, esp32_rmt_loop);

static mp_obj_t esp32_rmt_loop_count(mp_obj_t self_in, mp_obj_t loop) {
    esp32_rmt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->pin == -1) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("already deinitialized"));
    }

    int loop_count = mp_obj_get_int(loop);
    if (loop_count < -1) {
        mp_raise_ValueError(MP_ERROR_TEXT("arg must be -1, 0 or positive"));
    }
    esp32_rmt_loop_in(self, loop_count);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(esp32_rmt_loop_count_obj, esp32_rmt_loop_count);

static mp_obj_t esp32_rmt_write_pulses(size_t n_args, const mp_obj_t *args) {
    esp32_rmt_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (self->pin == -1) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("already deinitialized"));
    }

    if (self->enabled) {
        rmt_tx_wait_all_done(self->channel, -1);
    } else {
        check_esp_err(rmt_enable(self->channel));
        self->enabled = true;
    }

    mp_obj_t duration_obj = args[1];
    mp_obj_t data_obj = n_args > 2 ? args[2] : mp_const_true;

    mp_uint_t duration = 0;
    size_t duration_length = 0;
    mp_obj_t *duration_ptr = NULL;
    mp_uint_t data = 0;
    size_t data_length = 0;
    mp_obj_t *data_ptr = NULL;
    mp_uint_t num_pulses = 0;

    if (!(mp_obj_is_type(data_obj, &mp_type_tuple) || mp_obj_is_type(data_obj, &mp_type_list))) {
        // Mode 1: array of durations, toggle initial data value
        mp_obj_get_array(duration_obj, &duration_length, &duration_ptr);
        data = mp_obj_is_true(data_obj);
        num_pulses = duration_length;
    } else if (mp_obj_is_int(duration_obj)) {
        // Mode 2: constant duration, array of data values
        duration = mp_obj_get_int(duration_obj);
        mp_obj_get_array(data_obj, &data_length, &data_ptr);
        num_pulses = data_length;
    } else {
        // Mode 3: arrays of durations and data values
        mp_obj_get_array(duration_obj, &duration_length, &duration_ptr);
        mp_obj_get_array(data_obj, &data_length, &data_ptr);
        if (duration_length != data_length) {
            mp_raise_ValueError(MP_ERROR_TEXT("duration and data must have same length"));
        }
        num_pulses = duration_length;
    }

    if (num_pulses == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("No pulses"));
    }

    mp_uint_t num_items = (num_pulses / 2) + (num_pulses % 2);

    if (num_items > self->cap_items) {
        self->items = (rmt_symbol_word_t *)m_realloc(self->items, num_items * sizeof(rmt_symbol_word_t *));
        self->cap_items = num_items;
    }

    for (mp_uint_t item_index = 0, pulse_index = 0; item_index < num_items; item_index++) {
        self->items[item_index].duration0 = duration_length ? mp_obj_get_int(duration_ptr[pulse_index]) : duration;
        self->items[item_index].level0 = data_length ? mp_obj_is_true(data_ptr[pulse_index]) : data++;
        pulse_index++;
        if (pulse_index < num_pulses) {
            self->items[item_index].duration1 = duration_length ? mp_obj_get_int(duration_ptr[pulse_index]) : duration;
            self->items[item_index].level1 = data_length ? mp_obj_is_true(data_ptr[pulse_index]) : data++;
            pulse_index++;
        } else {
            self->items[item_index].duration1 = 0;
            self->items[item_index].level1 = 0;
        }
    }

    rmt_transmit_config_t tx_config = {
        .loop_count = self->loop_count,
        .flags.eot_level = self->idle_level ? 1 : 0,
    };

    rmt_encoder_reset(self->encoder);
    check_esp_err(rmt_transmit(self->channel, self->encoder, self->items, num_items * sizeof(rmt_symbol_word_t), &tx_config));
    self->tx_ongoing += 1;

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp32_rmt_write_pulses_obj, 2, 3, esp32_rmt_write_pulses);

static mp_obj_t esp32_rmt_bitstream_rmt(size_t n_args, const mp_obj_t *args) {
    if (n_args > 0) {
        esp32_rmt_bitstream_enabled = mp_obj_is_true(args[0]);
    }
    return esp32_rmt_bitstream_enabled ? mp_const_true : mp_const_false;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp32_rmt_bitstream_rmt_fun_obj, 0, 1, esp32_rmt_bitstream_rmt);
static MP_DEFINE_CONST_STATICMETHOD_OBJ(esp32_rmt_bitstream_rmt_obj, MP_ROM_PTR(&esp32_rmt_bitstream_rmt_fun_obj));

static mp_obj_t esp32_rmt_bitstream_channel(size_t n_args, const mp_obj_t *args) {
    if (n_args > 0) {
        if (args[0] == mp_const_none) {
            esp32_rmt_bitstream_enabled = false;
        } else {
            mp_int_t channel_id = mp_obj_get_int(args[0]);
            if (channel_id < 0) {
                mp_raise_ValueError(MP_ERROR_TEXT("invalid channel"));
            }
            esp32_rmt_bitstream_enabled = true;
        }
    }
    if (!esp32_rmt_bitstream_enabled) {
        return mp_const_none;
    } else {
        return MP_OBJ_NEW_SMALL_INT(1);
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp32_rmt_bitstream_channel_fun_obj, 0, 1, esp32_rmt_bitstream_channel);
static MP_DEFINE_CONST_STATICMETHOD_OBJ(esp32_rmt_bitstream_channel_obj, MP_ROM_PTR(&esp32_rmt_bitstream_channel_fun_obj));


static const mp_rom_map_elem_t esp32_rmt_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&esp32_rmt_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&esp32_rmt_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&esp32_rmt_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_clock_div), MP_ROM_PTR(&esp32_rmt_clock_div_obj) },
    { MP_ROM_QSTR(MP_QSTR_wait_done), MP_ROM_PTR(&esp32_rmt_wait_done_obj) },
    { MP_ROM_QSTR(MP_QSTR_loop), MP_ROM_PTR(&esp32_rmt_loop_obj) },
    { MP_ROM_QSTR(MP_QSTR_loop_count), MP_ROM_PTR(&esp32_rmt_loop_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_pulses), MP_ROM_PTR(&esp32_rmt_write_pulses_obj) },

    // Static methods
    { MP_ROM_QSTR(MP_QSTR_bitstream_rmt), MP_ROM_PTR(&esp32_rmt_bitstream_rmt_obj) },
    { MP_ROM_QSTR(MP_QSTR_bitstream_channel), MP_ROM_PTR(&esp32_rmt_bitstream_channel_obj) },

    // Class methods
    { MP_ROM_QSTR(MP_QSTR_source_freq), MP_ROM_PTR(&esp32_rmt_source_obj) },

    // Constants
    { MP_ROM_QSTR(MP_QSTR_PULSE_MAX), MP_ROM_INT(32767) },
};
static MP_DEFINE_CONST_DICT(esp32_rmt_locals_dict, esp32_rmt_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    esp32_rmt_type,
    MP_QSTR_RMT,
    MP_TYPE_FLAG_NONE,
    make_new, esp32_rmt_make_new,
    print, esp32_rmt_print,
    locals_dict, &esp32_rmt_locals_dict,
    protocol, &esp32_rmt_stream_p
    );

#endif // SOC_RMT_SUPPORTED
