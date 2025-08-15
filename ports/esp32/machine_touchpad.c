/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Nick Moore
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
#include "modmachine.h"
#include "driver/gpio.h"

#if SOC_TOUCH_SENSOR_SUPPORTED

#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 3, 0)
#if SOC_TOUCH_VERSION_1
#define SOC_TOUCH_SENSOR_VERSION (1)
#elif SOC_TOUCH_VERSION_2
#define SOC_TOUCH_SENSOR_VERSION (2)
#endif
#endif

#if SOC_TOUCH_SENSOR_VERSION == 1 // ESP32 only
#include "driver/touch_pad.h"
#elif SOC_TOUCH_SENSOR_VERSION == 2 // most ESP32
#include "driver/touch_sensor.h"
#elif SOC_TOUCH_SENSOR_VERSION == 3 // At present, it can only be used on ESP32P4.
#include "driver/touch_sens.h"
#include "soc/touch_sensor_channel.h"
#else
#error "Unknown touch hardware version"
#endif

typedef struct _mtp_obj_t {
    mp_obj_base_t base;
    gpio_num_t gpio_id;
    #if SOC_TOUCH_SENSOR_VERSION == 1 || SOC_TOUCH_SENSOR_VERSION == 2
    touch_pad_t touchpad_id;
    #elif SOC_TOUCH_SENSOR_VERSION == 3
    int touchpad_id;
    #endif
} mtp_obj_t;

#if SOC_TOUCH_SENSOR_VERSION == 3
static touch_sensor_handle_t touch_sens_handle;
static touch_channel_handle_t touch_chan_handle[15];
#endif

static const mtp_obj_t touchpad_obj[] = {
    #if CONFIG_IDF_TARGET_ESP32
    {{&machine_touchpad_type}, GPIO_NUM_4, TOUCH_PAD_NUM0},
    {{&machine_touchpad_type}, GPIO_NUM_0, TOUCH_PAD_NUM1},
    {{&machine_touchpad_type}, GPIO_NUM_2, TOUCH_PAD_NUM2},
    {{&machine_touchpad_type}, GPIO_NUM_15, TOUCH_PAD_NUM3},
    {{&machine_touchpad_type}, GPIO_NUM_13, TOUCH_PAD_NUM4},
    {{&machine_touchpad_type}, GPIO_NUM_12, TOUCH_PAD_NUM5},
    {{&machine_touchpad_type}, GPIO_NUM_14, TOUCH_PAD_NUM6},
    {{&machine_touchpad_type}, GPIO_NUM_27, TOUCH_PAD_NUM7},
    {{&machine_touchpad_type}, GPIO_NUM_33, TOUCH_PAD_NUM8},
    {{&machine_touchpad_type}, GPIO_NUM_32, TOUCH_PAD_NUM9},
    #elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
    {{&machine_touchpad_type}, GPIO_NUM_1, TOUCH_PAD_NUM1},
    {{&machine_touchpad_type}, GPIO_NUM_2, TOUCH_PAD_NUM2},
    {{&machine_touchpad_type}, GPIO_NUM_3, TOUCH_PAD_NUM3},
    {{&machine_touchpad_type}, GPIO_NUM_4, TOUCH_PAD_NUM4},
    {{&machine_touchpad_type}, GPIO_NUM_5, TOUCH_PAD_NUM5},
    {{&machine_touchpad_type}, GPIO_NUM_6, TOUCH_PAD_NUM6},
    {{&machine_touchpad_type}, GPIO_NUM_7, TOUCH_PAD_NUM7},
    {{&machine_touchpad_type}, GPIO_NUM_8, TOUCH_PAD_NUM8},
    {{&machine_touchpad_type}, GPIO_NUM_9, TOUCH_PAD_NUM9},
    {{&machine_touchpad_type}, GPIO_NUM_10, TOUCH_PAD_NUM10},
    {{&machine_touchpad_type}, GPIO_NUM_11, TOUCH_PAD_NUM11},
    {{&machine_touchpad_type}, GPIO_NUM_12, TOUCH_PAD_NUM12},
    {{&machine_touchpad_type}, GPIO_NUM_13, TOUCH_PAD_NUM13},
    {{&machine_touchpad_type}, GPIO_NUM_14, TOUCH_PAD_NUM14},
    #elif CONFIG_IDF_TARGET_ESP32P4
    {{&machine_touchpad_type}, GPIO_NUM_2, TOUCH_PAD_GPIO2_CHANNEL},
    {{&machine_touchpad_type}, GPIO_NUM_3, TOUCH_PAD_GPIO3_CHANNEL},
    {{&machine_touchpad_type}, GPIO_NUM_4, TOUCH_PAD_GPIO4_CHANNEL},
    {{&machine_touchpad_type}, GPIO_NUM_5, TOUCH_PAD_GPIO5_CHANNEL},
    {{&machine_touchpad_type}, GPIO_NUM_6, TOUCH_PAD_GPIO6_CHANNEL},
    {{&machine_touchpad_type}, GPIO_NUM_7, TOUCH_PAD_GPIO7_CHANNEL},
    {{&machine_touchpad_type}, GPIO_NUM_8, TOUCH_PAD_GPIO8_CHANNEL},
    {{&machine_touchpad_type}, GPIO_NUM_9, TOUCH_PAD_GPIO9_CHANNEL},
    {{&machine_touchpad_type}, GPIO_NUM_10, TOUCH_PAD_GPIO10_CHANNEL},
    {{&machine_touchpad_type}, GPIO_NUM_11, TOUCH_PAD_GPIO11_CHANNEL},
    {{&machine_touchpad_type}, GPIO_NUM_12, TOUCH_PAD_GPIO12_CHANNEL},
    {{&machine_touchpad_type}, GPIO_NUM_13, TOUCH_PAD_GPIO13_CHANNEL},
    {{&machine_touchpad_type}, GPIO_NUM_14, TOUCH_PAD_GPIO14_CHANNEL},
    {{&machine_touchpad_type}, GPIO_NUM_15, TOUCH_PAD_GPIO15_CHANNEL},
    #else
    #error "Please add GPIO mapping for this SoC"
    #endif
};

static mp_obj_t mtp_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw,
    const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, true);
    gpio_num_t pin_id = machine_pin_get_id(args[0]);
    const mtp_obj_t *self = NULL;
    for (int i = 0; i < MP_ARRAY_SIZE(touchpad_obj); i++) {
        if (pin_id == touchpad_obj[i].gpio_id) {
            self = &touchpad_obj[i];
            break;
        }
    }
    if (!self) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid pin for touchpad"));
    }

    static int initialized = 0;
    if (!initialized) {
        #if SOC_TOUCH_SENSOR_VERSION == 1 || SOC_TOUCH_SENSOR_VERSION == 2
        touch_pad_init();
        touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
        #elif SOC_TOUCH_SENSOR_VERSION == 3
        touch_sensor_sample_config_t sample_cfg[1] = {
            TOUCH_SENSOR_V3_DEFAULT_SAMPLE_CONFIG(1, 1, 1),
        };
        touch_sensor_config_t sens_cfg = TOUCH_SENSOR_DEFAULT_BASIC_CONFIG(1, sample_cfg);
        check_esp_err(touch_sensor_new_controller(&sens_cfg, &touch_sens_handle));
        touch_sensor_filter_config_t filter_cfg = TOUCH_SENSOR_DEFAULT_FILTER_CONFIG();
        check_esp_err(touch_sensor_config_filter(touch_sens_handle, &filter_cfg));
        #endif
        initialized = 1;
    }
    #if SOC_TOUCH_SENSOR_VERSION == 3
    else {
        // Stop the touch controller so a new channel can be added.
        check_esp_err(touch_sensor_stop_continuous_scanning(touch_sens_handle));
        check_esp_err(touch_sensor_disable(touch_sens_handle));
    }
    #endif
    #if SOC_TOUCH_SENSOR_VERSION == 1
    esp_err_t err = touch_pad_config(self->touchpad_id, 0);
    #elif SOC_TOUCH_SENSOR_VERSION == 2
    esp_err_t err = touch_pad_config(self->touchpad_id);
    #elif SOC_TOUCH_SENSOR_VERSION == 3
    touch_channel_config_t chan_cfg = {
        .active_thresh = {1000},
    };
    esp_err_t err = ESP_OK;
    if (touch_chan_handle[self->touchpad_id] != NULL) {
        err = touch_sensor_del_channel(touch_chan_handle[self->touchpad_id]);
        touch_chan_handle[self->touchpad_id] = NULL;
    }
    if (err == ESP_OK) {
        err = touch_sensor_new_channel(touch_sens_handle, self->touchpad_id, &chan_cfg, &touch_chan_handle[self->touchpad_id]);
    }
    check_esp_err(touch_sensor_enable(touch_sens_handle));
    check_esp_err(touch_sensor_start_continuous_scanning(touch_sens_handle));
    #endif
    if (err == ESP_OK) {
        #if SOC_TOUCH_SENSOR_VERSION == 2
        touch_pad_fsm_start();
        #endif

        return MP_OBJ_FROM_PTR(self);
    }
    mp_raise_ValueError(MP_ERROR_TEXT("Touch pad error"));
}

#if SOC_TOUCH_SENSOR_VERSION == 1 || SOC_TOUCH_SENSOR_VERSION == 2
static mp_obj_t mtp_config(mp_obj_t self_in, mp_obj_t value_in) {
    mtp_obj_t *self = self_in;
    #if SOC_TOUCH_SENSOR_VERSION == 1
    uint16_t value = mp_obj_get_int(value_in);
    esp_err_t err = touch_pad_config(self->touchpad_id, value);
    #elif SOC_TOUCH_SENSOR_VERSION == 2
    esp_err_t err = touch_pad_config(self->touchpad_id);
    #endif
    if (err == ESP_OK) {
        return mp_const_none;
    }
    mp_raise_ValueError(MP_ERROR_TEXT("Touch pad error"));
}
MP_DEFINE_CONST_FUN_OBJ_2(mtp_config_obj, mtp_config);
#endif

static mp_obj_t mtp_read(mp_obj_t self_in) {
    mtp_obj_t *self = self_in;
    #if SOC_TOUCH_SENSOR_VERSION == 1
    uint16_t value;
    esp_err_t err = touch_pad_read(self->touchpad_id, &value);
    #elif SOC_TOUCH_SENSOR_VERSION == 2
    uint32_t value;
    esp_err_t err = touch_pad_read_raw_data(self->touchpad_id, &value);
    #elif SOC_TOUCH_SENSOR_VERSION == 3
    uint32_t value;
    esp_err_t err = touch_channel_read_data(touch_chan_handle[self->touchpad_id], TOUCH_CHAN_DATA_TYPE_SMOOTH, &value);
    #endif
    if (err == ESP_OK) {
        return MP_OBJ_NEW_SMALL_INT(value);
    }
    mp_raise_ValueError(MP_ERROR_TEXT("Touch pad error"));
}
MP_DEFINE_CONST_FUN_OBJ_1(mtp_read_obj, mtp_read);

static const mp_rom_map_elem_t mtp_locals_dict_table[] = {
    // instance methods
    #if SOC_TOUCH_SENSOR_VERSION == 1 || SOC_TOUCH_SENSOR_VERSION == 2
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&mtp_config_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mtp_read_obj) },
};

static MP_DEFINE_CONST_DICT(mtp_locals_dict, mtp_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_touchpad_type,
    MP_QSTR_TouchPad,
    MP_TYPE_FLAG_NONE,
    make_new, mtp_make_new,
    locals_dict, &mtp_locals_dict
    );

#endif // SOC_TOUCH_SENSOR_SUPPORTED
