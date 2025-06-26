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

// This file is never compiled standalone, it's included directly from
// extmod/machine_i2c_target.c via MICROPY_PY_MACHINE_I2C_TARGET_INCLUDEFILE.

#include "pico/i2c_slave.h"

// TODO: should be in a common i2c header
#ifdef MICROPY_HW_I2C_NO_DEFAULT_PINS

// With no default I2C, need to require the pin args.
#define MICROPY_HW_I2C0_SCL (0)
#define MICROPY_HW_I2C0_SDA (0)
#define MICROPY_HW_I2C1_SCL (0)
#define MICROPY_HW_I2C1_SDA (0)
#define MICROPY_I2C_PINS_ARG_OPTS MP_ARG_REQUIRED

#else

// Most boards do not require pin args.
#define MICROPY_I2C_PINS_ARG_OPTS 0

#ifndef MICROPY_HW_I2C0_SCL
#if PICO_DEFAULT_I2C == 0
#define MICROPY_HW_I2C0_SCL (PICO_DEFAULT_I2C_SCL_PIN)
#define MICROPY_HW_I2C0_SDA (PICO_DEFAULT_I2C_SDA_PIN)
#else
#define MICROPY_HW_I2C0_SCL (9)
#define MICROPY_HW_I2C0_SDA (8)
#endif
#endif

#ifndef MICROPY_HW_I2C1_SCL
#if PICO_DEFAULT_I2C == 1
#define MICROPY_HW_I2C1_SCL (PICO_DEFAULT_I2C_SCL_PIN)
#define MICROPY_HW_I2C1_SDA (PICO_DEFAULT_I2C_SDA_PIN)
#else
#define MICROPY_HW_I2C1_SCL (7)
#define MICROPY_HW_I2C1_SDA (6)
#endif
#endif
#endif

// SDA/SCL on even/odd pins, I2C0/I2C1 on even/odd pairs of pins.
// TODO: should be in a common i2c header
#define IS_VALID_SCL(i2c, pin) (((pin) & 1) == 1 && (((pin) & 2) >> 1) == (i2c))
#define IS_VALID_SDA(i2c, pin) (((pin) & 1) == 0 && (((pin) & 2) >> 1) == (i2c))

static machine_i2c_target_data_t i2c_target_data[4];

typedef struct _machine_i2c_target_memory_obj_t {
    mp_obj_base_t base;
    i2c_inst_t *const i2c_inst;
    uint8_t addr; // TODO can we retrieve the addr from pico-sdk / hardware?
    mp_hal_pin_obj_t scl;
    mp_hal_pin_obj_t sda;
} machine_i2c_target_memory_obj_t;

static machine_i2c_target_memory_obj_t machine_i2c_target_memory_obj[] = {
    {{&machine_i2c_target_memory_type}, i2c0, 0, MICROPY_HW_I2C0_SCL, MICROPY_HW_I2C0_SDA},
    {{&machine_i2c_target_memory_type}, i2c1, 0, MICROPY_HW_I2C1_SCL, MICROPY_HW_I2C1_SDA},
};

MP_REGISTER_ROOT_POINTER(mp_obj_t pyb_i2cslave_mem[4]);

static void i2c_target_handler(i2c_inst_t *i2c, i2c_slave_event_t event) {
    unsigned int i2c_id = i2c == i2c0 ? 0 : 1;
    machine_i2c_target_data_t *data = &i2c_target_data[i2c_id];

    switch (event) {
        // Data from controller is available for reading.
        case I2C_SLAVE_RECEIVE:
            machine_i2c_target_data_write(data, i2c_read_byte_raw(i2c));
            break;

        // Controller is requesting data.
        case I2C_SLAVE_REQUEST: {
            i2c_write_byte_raw(i2c, machine_i2c_target_data_read(data));
            break;
        }

        // Controller has sent a Stop or Restart signal.
        case I2C_SLAVE_FINISH:
            machine_i2c_target_data_reset(data);
            break;
    }
}

static mp_obj_t mp_machine_i2c_target_memory_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // TODO: reconsider order of arguments
    enum { ARG_id, ARG_addr, ARG_mem, ARG_scl, ARG_sda };
    static const mp_arg_t allowed_args[] = {
        #ifdef PICO_DEFAULT_I2C
        { MP_QSTR_id, MP_ARG_INT, {.u_int = PICO_DEFAULT_I2C} },
        #else
        { MP_QSTR_id, MP_ARG_INT | MP_ARG_REQUIRED },
        #endif
        { MP_QSTR_addr, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_mem, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_scl, MICROPY_I2C_PINS_ARG_OPTS | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_sda, MICROPY_I2C_PINS_ARG_OPTS | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int i2c_id = args[ARG_id].u_int;

    // Check if the I2C bus is valid
    if (i2c_id < 0 || i2c_id >= MP_ARRAY_SIZE(machine_i2c_target_memory_obj)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2C(%d) doesn't exist"), i2c_id);
    }

    // Get static peripheral object.
    machine_i2c_target_memory_obj_t *self = &machine_i2c_target_memory_obj[i2c_id];

    // Set the target address.
    self->addr = args[ARG_addr].u_int;

    // Initialise data.
    MP_STATE_PORT(pyb_i2cslave_mem)[i2c_id] = args[ARG_mem].u_obj;
    machine_i2c_target_data_t *data = &i2c_target_data[i2c_id];
    machine_i2c_target_data_init(data, args[ARG_mem].u_obj);

    // Set SCL/SDA pins if configured.
    if (args[ARG_scl].u_obj != mp_const_none) {
        int scl = mp_hal_get_pin_obj(args[ARG_scl].u_obj);
        if (!IS_VALID_SCL(i2c_id, scl)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad SCL pin"));
        }
        self->scl = scl;
    }
    if (args[ARG_sda].u_obj != mp_const_none) {
        int sda = mp_hal_get_pin_obj(args[ARG_sda].u_obj);
        if (!IS_VALID_SDA(i2c_id, sda)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad SDA pin"));
        }
        self->sda = sda;
    }

    // Initialise the I2C target.
    // TODO: don't reinitialize if no arguments given.
    i2c_slave_init(self->i2c_inst, self->addr, i2c_target_handler);
    gpio_set_function(self->scl, GPIO_FUNC_I2C);
    gpio_set_function(self->sda, GPIO_FUNC_I2C);
    gpio_set_pulls(self->scl, true, 0);
    gpio_set_pulls(self->sda, true, 0);

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_i2c_target_memory_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2c_target_memory_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2CTargetMemory(%u, addr=%u, scl=%u, sda=%u)",
        self - &machine_i2c_target_memory_obj[0], self->addr, self->scl, self->sda);
}

static void mp_machine_i2c_target_memory_deinit(machine_i2c_target_memory_obj_t *self) {
    i2c_slave_deinit(self->i2c_inst);
}
