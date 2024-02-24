/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 "Matt Trentini" <matt.trentini@gmail.com>
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
#include "modmachine.h"
#include "mphalport.h"
#include "modesp32.h"

#include "esp_task.h"
#include "driver/rmt.h"

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
// This current MicroPython implementation lacks some major features, notably receive pulses
// and carrier output.

// Last available RMT channel that can transmit.
#define RMT_LAST_TX_CHANNEL (SOC_RMT_TX_CANDIDATES_PER_GROUP - 1)

// Forward declaration
extern const mp_obj_type_t esp32_rmt_type;

typedef struct _esp32_rmt_obj_t {
    mp_obj_base_t base;
    uint8_t channel_id;
    gpio_num_t pin;
    uint8_t clock_div;
    mp_uint_t num_items;
    rmt_item32_t *items;
    bool loop_en;
} esp32_rmt_obj_t;

// Current channel used for machine.bitstream, in the machine_bitstream_high_low_rmt
// implementation.  A value of -1 means do not use RMT.
int8_t esp32_rmt_bitstream_channel_id = RMT_LAST_TX_CHANNEL;

#if MP_TASK_COREID == 0

typedef struct _rmt_install_state_t {
    SemaphoreHandle_t handle;
    uint8_t channel_id;
    esp_err_t ret;
} rmt_install_state_t;

STATIC void rmt_install_task(void *pvParameter) {
    rmt_install_state_t *state = pvParameter;
    state->ret = rmt_driver_install(state->channel_id, 0, 0);
    xSemaphoreGive(state->handle);
    vTaskDelete(NULL);
    for (;;) {
    }
}

// Call rmt_driver_install on core 1.  This ensures that the RMT interrupt handler is
// serviced on core 1, so that WiFi (if active) does not interrupt it and cause glitches.
esp_err_t rmt_driver_install_core1(uint8_t channel_id) {
    TaskHandle_t th;
    rmt_install_state_t state;
    state.handle = xSemaphoreCreateBinary();
    state.channel_id = channel_id;
    xTaskCreatePinnedToCore(rmt_install_task, "rmt_install_task", 2048 / sizeof(StackType_t), &state, ESP_TASK_PRIO_MIN + 1, &th, 1);
    xSemaphoreTake(state.handle, portMAX_DELAY);
    vSemaphoreDelete(state.handle);
    return state.ret;
}

#else

// MicroPython runs on core 1, so we can call the RMT installer directly and its
// interrupt handler will also run on core 1.
esp_err_t rmt_driver_install_core1(uint8_t channel_id) {
    return rmt_driver_install(channel_id, 0, 0);
}

#endif

STATIC mp_obj_t esp32_rmt_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,        MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_pin,       MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_clock_div,                   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} }, // 100ns resolution
        { MP_QSTR_idle_level,                  MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} }, // low voltage
        { MP_QSTR_tx_carrier,                  MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} }, // no carrier
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    mp_uint_t channel_id = args[0].u_int;
    gpio_num_t pin_id = machine_pin_get_id(args[1].u_obj);
    mp_uint_t clock_div = args[2].u_int;
    mp_uint_t idle_level = args[3].u_bool;
    mp_obj_t tx_carrier_obj = args[4].u_obj;

    if (esp32_rmt_bitstream_channel_id >= 0 && channel_id == esp32_rmt_bitstream_channel_id) {
        mp_raise_ValueError(MP_ERROR_TEXT("channel used by bitstream"));
    }

    if (clock_div < 1 || clock_div > 255) {
        mp_raise_ValueError(MP_ERROR_TEXT("clock_div must be between 1 and 255"));
    }

    esp32_rmt_obj_t *self = mp_obj_malloc_with_finaliser(esp32_rmt_obj_t, &esp32_rmt_type);
    self->channel_id = channel_id;
    self->pin = pin_id;
    self->clock_div = clock_div;
    self->loop_en = false;

    rmt_config_t config = {0};
    config.rmt_mode = RMT_MODE_TX;
    config.channel = (rmt_channel_t)self->channel_id;
    config.gpio_num = self->pin;
    config.mem_block_num = 1;
    config.tx_config.loop_en = 0;

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

        config.tx_config.carrier_en = 1;
        config.tx_config.carrier_freq_hz = frequency;
        config.tx_config.carrier_duty_percent = duty;
        config.tx_config.carrier_level = level;
    } else {
        config.tx_config.carrier_en = 0;
    }

    config.tx_config.idle_output_en = 1;
    config.tx_config.idle_level = idle_level;

    config.clk_div = self->clock_div;

    check_esp_err(rmt_config(&config));
    check_esp_err(rmt_driver_install_core1(config.channel));

    return MP_OBJ_FROM_PTR(self);
}

STATIC void esp32_rmt_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    esp32_rmt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->pin != -1) {
        bool idle_output_en;
        rmt_idle_level_t idle_level;
        check_esp_err(rmt_get_idle_level(self->channel_id, &idle_output_en, &idle_level));
        mp_printf(print, "RMT(channel=%u, pin=%u, source_freq=%u, clock_div=%u, idle_level=%u)",
            self->channel_id, self->pin, APB_CLK_FREQ, self->clock_div, idle_level);
    } else {
        mp_printf(print, "RMT()");
    }
}

STATIC mp_obj_t esp32_rmt_deinit(mp_obj_t self_in) {
    // fixme: check for valid channel. Return exception if error occurs.
    esp32_rmt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->pin != -1) { // Check if channel has already been deinitialised.
        rmt_driver_uninstall(self->channel_id);
        self->pin = -1; // -1 to indicate RMT is unused
        m_free(self->items);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_rmt_deinit_obj, esp32_rmt_deinit);

// Return the source frequency.
// Currently only the APB clock (80MHz) can be used but it is possible other
// clock sources will added in the future.
STATIC mp_obj_t esp32_rmt_source_freq() {
    return mp_obj_new_int(APB_CLK_FREQ);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp32_rmt_source_freq_obj, esp32_rmt_source_freq);
STATIC MP_DEFINE_CONST_STATICMETHOD_OBJ(esp32_rmt_source_obj, MP_ROM_PTR(&esp32_rmt_source_freq_obj));

// Return the clock divider.
STATIC mp_obj_t esp32_rmt_clock_div(mp_obj_t self_in) {
    esp32_rmt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(self->clock_div);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_rmt_clock_div_obj, esp32_rmt_clock_div);

// Query whether the channel has finished sending pulses. Takes an optional
// timeout (in milliseconds), returning true if the pulse stream has
// completed or false if they are still transmitting (or timeout is reached).
STATIC mp_obj_t esp32_rmt_wait_done(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_self,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    esp32_rmt_obj_t *self = MP_OBJ_TO_PTR(args[0].u_obj);

    esp_err_t err = rmt_wait_tx_done(self->channel_id, args[1].u_int / portTICK_PERIOD_MS);
    return err == ESP_OK ? mp_const_true : mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(esp32_rmt_wait_done_obj, 1, esp32_rmt_wait_done);

STATIC mp_obj_t esp32_rmt_loop(mp_obj_t self_in, mp_obj_t loop) {
    esp32_rmt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->loop_en = mp_obj_get_int(loop);
    if (!self->loop_en) {
        bool loop_en;
        check_esp_err(rmt_get_tx_loop_mode(self->channel_id, &loop_en));
        if (loop_en) {
            check_esp_err(rmt_set_tx_loop_mode(self->channel_id, false));
            check_esp_err(rmt_set_tx_intr_en(self->channel_id, true));
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_rmt_loop_obj, esp32_rmt_loop);

STATIC mp_obj_t esp32_rmt_write_pulses(size_t n_args, const mp_obj_t *args) {
    esp32_rmt_obj_t *self = MP_OBJ_TO_PTR(args[0]);
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
    if (self->loop_en && num_pulses > 126) {
        mp_raise_ValueError(MP_ERROR_TEXT("Too many pulses for loop"));
    }

    mp_uint_t num_items = (num_pulses / 2) + (num_pulses % 2);
    if (num_items > self->num_items) {
        self->items = (rmt_item32_t *)m_realloc(self->items, num_items * sizeof(rmt_item32_t *));
        self->num_items = num_items;
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

    if (self->loop_en) {
        bool loop_en;
        check_esp_err(rmt_get_tx_loop_mode(self->channel_id, &loop_en));
        if (loop_en) {
            check_esp_err(rmt_set_tx_intr_en(self->channel_id, true));
            check_esp_err(rmt_set_tx_loop_mode(self->channel_id, false));
        }
        check_esp_err(rmt_wait_tx_done(self->channel_id, portMAX_DELAY));
    }

    #if !CONFIG_IDF_TARGET_ESP32S3
    check_esp_err(rmt_write_items(self->channel_id, self->items, num_items, false));
    #endif

    if (self->loop_en) {
        check_esp_err(rmt_set_tx_intr_en(self->channel_id, false));
        check_esp_err(rmt_set_tx_loop_mode(self->channel_id, true));
    }

    #if CONFIG_IDF_TARGET_ESP32S3
    check_esp_err(rmt_write_items(self->channel_id, self->items, num_items, false));
    #endif

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp32_rmt_write_pulses_obj, 2, 3, esp32_rmt_write_pulses);

STATIC mp_obj_t esp32_rmt_bitstream_channel(size_t n_args, const mp_obj_t *args) {
    if (n_args > 0) {
        if (args[0] == mp_const_none) {
            esp32_rmt_bitstream_channel_id = -1;
        } else {
            mp_int_t channel_id = mp_obj_get_int(args[0]);
            if (channel_id < 0 || channel_id > RMT_LAST_TX_CHANNEL) {
                mp_raise_ValueError(MP_ERROR_TEXT("invalid channel"));
            }
            esp32_rmt_bitstream_channel_id = channel_id;
        }
    }
    if (esp32_rmt_bitstream_channel_id < 0) {
        return mp_const_none;
    } else {
        return MP_OBJ_NEW_SMALL_INT(esp32_rmt_bitstream_channel_id);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp32_rmt_bitstream_channel_fun_obj, 0, 1, esp32_rmt_bitstream_channel);
STATIC MP_DEFINE_CONST_STATICMETHOD_OBJ(esp32_rmt_bitstream_channel_obj, MP_ROM_PTR(&esp32_rmt_bitstream_channel_fun_obj));

STATIC const mp_rom_map_elem_t esp32_rmt_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&esp32_rmt_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&esp32_rmt_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_clock_div), MP_ROM_PTR(&esp32_rmt_clock_div_obj) },
    { MP_ROM_QSTR(MP_QSTR_wait_done), MP_ROM_PTR(&esp32_rmt_wait_done_obj) },
    { MP_ROM_QSTR(MP_QSTR_loop), MP_ROM_PTR(&esp32_rmt_loop_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_pulses), MP_ROM_PTR(&esp32_rmt_write_pulses_obj) },

    // Static methods
    { MP_ROM_QSTR(MP_QSTR_bitstream_channel), MP_ROM_PTR(&esp32_rmt_bitstream_channel_obj) },

    // Class methods
    { MP_ROM_QSTR(MP_QSTR_source_freq), MP_ROM_PTR(&esp32_rmt_source_obj) },

    // Constants
    { MP_ROM_QSTR(MP_QSTR_PULSE_MAX), MP_ROM_INT(32767) },
};
STATIC MP_DEFINE_CONST_DICT(esp32_rmt_locals_dict, esp32_rmt_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    esp32_rmt_type,
    MP_QSTR_RMT,
    MP_TYPE_FLAG_NONE,
    make_new, esp32_rmt_make_new,
    print, esp32_rmt_print,
    locals_dict, &esp32_rmt_locals_dict
    );
