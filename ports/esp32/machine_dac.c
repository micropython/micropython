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


#include <stdio.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "modmachine.h"

#if MICROPY_PY_MACHINE_DAC

#include "driver/gpio.h"
#include "driver/dac_oneshot.h"

typedef struct _mdac_obj_t {
    mp_obj_base_t base;
    gpio_num_t gpio_id;
    dac_channel_t dac_id;
    dac_oneshot_handle_t dac_oneshot_handle;
} mdac_obj_t;

static mdac_obj_t mdac_obj[] = {
    #if CONFIG_IDF_TARGET_ESP32
    {{&machine_dac_type}, GPIO_NUM_25, DAC_CHAN_0},
    {{&machine_dac_type}, GPIO_NUM_26, DAC_CHAN_1},
    #else
    {{&machine_dac_type}, GPIO_NUM_17, DAC_CHAN_0},
    {{&machine_dac_type}, GPIO_NUM_18, DAC_CHAN_1},
    #endif
};

static mp_obj_t mdac_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw,
    const mp_obj_t *args) {

    mp_arg_check_num(n_args, n_kw, 1, 1, true);
    gpio_num_t pin_id = machine_pin_get_id(args[0]);
    const mdac_obj_t *self = NULL;
    for (int i = 0; i < MP_ARRAY_SIZE(mdac_obj); i++) {
        if (pin_id == mdac_obj[i].gpio_id) {
            self = &mdac_obj[i];
            break;
        }
    }
    if (!self) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid Pin for DAC"));
    }

    dac_oneshot_config_t dac_oneshot_config = {.chan_id = self->dac_id};
    check_esp_err(dac_oneshot_new_channel(&dac_oneshot_config, (dac_oneshot_handle_t *)&self->dac_oneshot_handle));
    check_esp_err(dac_oneshot_output_voltage(self->dac_oneshot_handle, 0));
    return MP_OBJ_FROM_PTR(self);
}

static void mdac_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mdac_obj_t *self = self_in;
    mp_printf(print, "DAC(Pin(%u))", self->gpio_id);
}

static mp_obj_t mdac_write(mp_obj_t self_in, mp_obj_t value_in) {
    mdac_obj_t *self = self_in;
    int value = mp_obj_get_int(value_in);
    if (value < 0 || value > 255) {
        mp_raise_ValueError(MP_ERROR_TEXT("value out of range"));
    }

    check_esp_err(dac_oneshot_output_voltage(self->dac_oneshot_handle, value));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mdac_write_obj, mdac_write);

static const mp_rom_map_elem_t mdac_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mdac_write_obj) },
};

static MP_DEFINE_CONST_DICT(mdac_locals_dict, mdac_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_dac_type,
    MP_QSTR_DAC,
    MP_TYPE_FLAG_NONE,
    make_new, mdac_make_new,
    print, mdac_print,
    locals_dict, &mdac_locals_dict
    );

#endif // MICROPY_PY_MACHINE_DAC
