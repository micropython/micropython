/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Ayke van Laethem
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

#include "machine_i2s.h"
#include "py/runtime.h"
#include "py/mperrno.h"

STATIC const machine_i2s_obj_t machine_i2s_obj_0 = {
    { &machine_i2s_type },
    .port = I2S_NUM_0,
};

STATIC const machine_i2s_obj_t machine_i2s_obj_1 = {
    { &machine_i2s_type },
    .port = I2S_NUM_1,
};

STATIC void machine_i2s_obj_init_helper(const machine_i2s_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_bclk, ARG_ws, ARG_data_out, ARG_data_in, ARG_samplerate, ARG_bitdepth };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bclk, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_ws, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_data_out, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_data_in, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_samplerate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 48000} },
        { MP_QSTR_bitdepth, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 16} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    i2s_config_t config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
        .sample_rate = args[ARG_samplerate].u_int,
        .bits_per_sample = args[ARG_bitdepth].u_int,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = 0,
    };
    if (i2s_driver_install(self->port, &config, 0, NULL) != ESP_OK) {
        mp_raise_ValueError("I2S: failed to enable");
    }

    i2s_pin_config_t pin_config = {
        .bck_io_num = args[ARG_bclk].u_int,
        .ws_io_num = args[ARG_ws].u_int,
        .data_out_num = args[ARG_data_out].u_int,
        .data_in_num = args[ARG_data_in].u_int,
    };
    if (i2s_set_pin(self->port, &pin_config) != ESP_OK) {
        i2s_driver_uninstall(self->port);
        mp_raise_ValueError("I2S: failed to set pins in");
    }
}

STATIC mp_obj_t machine_i2s_obj_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    machine_i2s_obj_init_helper(args[0], n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_i2s_init_obj, 1, machine_i2s_obj_init);

STATIC mp_obj_t machine_i2s_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_int_t id = 0;
    if (n_args > 0) {
        id = mp_obj_get_int(args[0]);
        n_args--;
        args++;
    }
    const machine_i2s_obj_t *self;
    if (id == 0) {
        self = &machine_i2s_obj_0;
    } else if (id == 1) {
        self = &machine_i2s_obj_1;
    } else {
        mp_raise_ValueError("invalid I2S peripheral");
    }
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    machine_i2s_obj_init_helper(self, n_args, args, &kw_args);
    return (machine_i2s_obj_t*)self; // discard const
}

STATIC mp_obj_t machine_i2s_obj_deinit(mp_obj_t self_in) {
    const machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);
    i2s_driver_uninstall(self->port);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_i2s_deinit_obj, machine_i2s_obj_deinit);

STATIC mp_obj_t machine_i2s_obj_write(mp_obj_t self_in, mp_obj_t buf_in) {
    const machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    if (i2s_write_bytes(self->port, bufinfo.buf, bufinfo.len, portMAX_DELAY) == ESP_FAIL) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(machine_i2s_write_obj, machine_i2s_obj_write);

STATIC const mp_rom_map_elem_t machine_i2s_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_i2s_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_i2s_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&machine_i2s_write_obj) },
};

STATIC MP_DEFINE_CONST_DICT(machine_i2s_locals_dict, machine_i2s_locals_dict_table);

const mp_obj_type_t machine_i2s_type = {
    { &mp_type_type },
    .name = MP_QSTR_I2S,
    .make_new = machine_i2s_make_new,
    .locals_dict = (mp_obj_dict_t*)&machine_i2s_locals_dict,
};
