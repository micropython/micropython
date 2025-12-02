/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2025 Infineon Technologies AG
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

// std includes
#include <stdio.h>
#include <string.h>


// mpy includes
#include "extmod/modmachine.h"
#include "py/runtime.h"
#include "py/mphal.h"


// MTB includes
#include "cybsp.h"


// port-specific includes
#include "modmachine.h"
// #include "machine_pin_phy.h"
// #include "mplogger.h"
#include "machine_i2c.h" // TODO: Add pin define

#define DEFAULT_I2C_FREQ     (400000)

#define i2c_assert_raise_val(msg, ret)   if (ret != CY_RSLT_SUCCESS) { \
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT(msg), ret); \
}

typedef struct _machine_i2c_obj_t {
    mp_obj_base_t base;
    int id; // This parameter is unused and added for compliance with reference API.
    mtb_hal_i2c_t i2c_obj;
    uint32_t scl_pin;
    uint32_t sda_pin;
    mtb_hal_i2c_cfg_t cfg;
    mp_obj_t callback;
} machine_i2c_obj_t;


// const cy_stc_scb_i2c_config_t CYBSP_I2C_CONTROLLER_config =
// {
//     .i2cMode = CY_SCB_I2C_MASTER,
//     .useRxFifo = true,
//     .useTxFifo = true,
//     .slaveAddress = 0U,
//     .slaveAddressMask = 0U,
//     .acceptAddrInFifo = false,
//     .ackGeneralAddr = false,
//     .enableWakeFromSleep = false,
//     .enableDigitalFilter = false,
//     .lowPhaseDutyCycle = 16,
//     .highPhaseDutyCycle = 9,
// };

machine_i2c_obj_t *i2c_obj[MAX_I2C] = { NULL };

static inline machine_i2c_obj_t *i2c_obj_alloc(bool is_slave) {
    for (uint8_t i = 0; i < MAX_I2C; i++)
    {
        if (i2c_obj[i] == NULL) {
            const mp_obj_type_t *obj_type;
            obj_type = &machine_i2c_type;
            i2c_obj[i] = mp_obj_malloc(machine_i2c_obj_t, obj_type);
            return i2c_obj[i];
        }
    }

    return NULL;
}

static inline void i2c_obj_free(machine_i2c_obj_t *i2c_obj_ptr) {
    for (uint8_t i = 0; i < MAX_I2C; i++)
    {
        if (i2c_obj[i] == i2c_obj_ptr) {
            i2c_obj[i] = NULL;
        }
    }
}

static void i2c_init(machine_i2c_obj_t *machine_i2c_obj, uint32_t freq_hz) {
}

static void machine_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2C(freq=%u, scl=%u, sda=%u, addr=%u)", self->cfg.frequency_hz, self->scl_pin, self->sda_pin, self->cfg.address);
}

mp_obj_t machine_i2c_master_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 0, 4, true);

    enum { ARG_id, ARG_freq, ARG_scl, ARG_sda };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,   MP_ARG_INT, {.u_int = -1}},
        { MP_QSTR_freq, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_I2C_FREQ} },
        { MP_QSTR_scl, MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_sda, MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    machine_i2c_obj_t *self = i2c_obj_alloc(false);
    if (self == NULL) {
        return mp_const_none;
    }

    // set id if provided
    if (args[ARG_id].u_int != -1) {
        self->id = args[ARG_id].u_int;
        mp_printf(&mp_plat_print, "machine.I2C: ID parameter is ignored in this port.\n");
    }

    // initialise I2C at cyhal level
    i2c_init(self, args[ARG_freq].u_int);

    return MP_OBJ_FROM_PTR(self);
}


static void machine_i2c_deinit(mp_obj_base_t *self_in) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    i2c_obj_free(self);
}

static int machine_i2c_transfer(mp_obj_base_t *self_in, uint16_t addr, size_t len, uint8_t *buf, unsigned int flags) {
    return 0;
}

static const mp_machine_i2c_p_t machine_i2c_p = {
    .transfer_single = machine_i2c_transfer,
    // .transfer = mp_machine_i2c_transfer_adaptor
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_i2c_type,
    MP_QSTR_I2C,
    MP_TYPE_FLAG_NONE,
    make_new, machine_i2c_master_make_new,
    print, machine_i2c_print,
    protocol, &machine_i2c_p,
    locals_dict, &mp_machine_i2c_locals_dict
    );
