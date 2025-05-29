/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Damien P. George
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

#if MICROPY_PY_MACHINE_I2C_TARGET_MEMORY

#include "extmod/modmachine.h"

typedef struct _machine_i2c_target_data_t {
    uint8_t first_rx;
    uint8_t addr;
    uint8_t len; // stored as len-1 so it can represent up to 256
    uint8_t *mem;
} machine_i2c_target_data_t;

static void machine_i2c_target_data_init(machine_i2c_target_data_t *data, mp_obj_t mem_obj) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(mem_obj, &bufinfo, MP_BUFFER_RW);
    if (bufinfo.len == 0 || bufinfo.len > 256) {
        mp_raise_ValueError(MP_ERROR_TEXT("mem must be between 1 and 256 bytes"));
    }
    data->first_rx = 1;
    data->addr = 0;
    data->len = bufinfo.len - 1;
    data->mem = bufinfo.buf;
}

static void machine_i2c_target_data_reset(machine_i2c_target_data_t *data) {
    data->first_rx = 1;
}

static uint8_t machine_i2c_target_data_read(machine_i2c_target_data_t *data) {
    uint8_t val = data->mem[data->addr++];
    if (data->addr >= data->len + 1) {
        data->addr = 0;
    }
    return val;
}

static void machine_i2c_target_data_write(machine_i2c_target_data_t *data, uint8_t val) {
    if (data->first_rx == 1) {
        data->first_rx = 0;
        data->addr = val % (data->len + 1);
    } else {
        data->mem[data->addr++] = val;
        if (data->addr >= data->len + 1) {
            data->addr = 0;
        }
    }
}

// The port must provide implementations of these low-level I2C target functions.

static mp_obj_t mp_machine_i2c_target_memory_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);
static void mp_machine_i2c_target_memory_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
static void mp_machine_i2c_target_memory_deinit(machine_i2c_target_memory_obj_t *self);

// The port provides implementations of the above in this file.
#include MICROPY_PY_MACHINE_I2C_TARGET_INCLUDEFILE

// I2CTargetMemory.deinit()
static mp_obj_t machine_i2c_target_memory_deinit(mp_obj_t self_in) {
    machine_i2c_target_memory_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_machine_i2c_target_memory_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_i2c_target_memory_deinit_obj, machine_i2c_target_memory_deinit);

static const mp_rom_map_elem_t machine_i2c_target_memory_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_i2c_target_memory_deinit_obj) },
};
static MP_DEFINE_CONST_DICT(machine_i2c_target_memory_locals_dict, machine_i2c_target_memory_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_i2c_target_memory_type,
    MP_QSTR_I2CTargetMemory,
    MP_TYPE_FLAG_NONE,
    make_new, mp_machine_i2c_target_memory_make_new,
    print, mp_machine_i2c_target_memory_print,
    locals_dict, &machine_i2c_target_memory_locals_dict
    );

#endif // MICROPY_PY_MACHINE_I2C_TARGET_MEMORY
