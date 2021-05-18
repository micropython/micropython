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

#if CONFIG_IDF_TARGET_ESP32

#include "driver/gpio.h"
#include "driver/touch_pad.h"

typedef struct _mtp_obj_t {
    mp_obj_base_t base;
    gpio_num_t gpio_id;
    touch_pad_t touchpad_id;
} mtp_obj_t;

STATIC const mtp_obj_t touchpad_obj[] = {
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
};

STATIC mp_obj_t mtp_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw,
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
        touch_pad_init();
        touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
        initialized = 1;
    }
    esp_err_t err = touch_pad_config(self->touchpad_id, 0);
    if (err == ESP_OK) {
        return MP_OBJ_FROM_PTR(self);
    }
    mp_raise_ValueError(MP_ERROR_TEXT("Touch pad error"));
}

STATIC mp_obj_t mtp_config(mp_obj_t self_in, mp_obj_t value_in) {
    mtp_obj_t *self = self_in;
    uint16_t value = mp_obj_get_int(value_in);
    esp_err_t err = touch_pad_config(self->touchpad_id, value);
    if (err == ESP_OK) {
        return mp_const_none;
    }
    mp_raise_ValueError(MP_ERROR_TEXT("Touch pad error"));
}
MP_DEFINE_CONST_FUN_OBJ_2(mtp_config_obj, mtp_config);

STATIC mp_obj_t mtp_read(mp_obj_t self_in) {
    mtp_obj_t *self = self_in;
    uint16_t value;
    esp_err_t err = touch_pad_read(self->touchpad_id, &value);
    if (err == ESP_OK) {
        return MP_OBJ_NEW_SMALL_INT(value);
    }
    mp_raise_ValueError(MP_ERROR_TEXT("Touch pad error"));
}
MP_DEFINE_CONST_FUN_OBJ_1(mtp_read_obj, mtp_read);

STATIC const mp_rom_map_elem_t mtp_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&mtp_config_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mtp_read_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mtp_locals_dict, mtp_locals_dict_table);

const mp_obj_type_t machine_touchpad_type = {
    { &mp_type_type },
    .name = MP_QSTR_TouchPad,
    .make_new = mtp_make_new,
    .locals_dict = (mp_obj_t)&mtp_locals_dict,
};

#endif // CONFIG_IDF_TARGET_ESP32
