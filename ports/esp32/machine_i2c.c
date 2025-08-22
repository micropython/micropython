/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
 * Copyright (c) 2025 Vincent1-python
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
#include "py/mperrno.h"
#include "extmod/modmachine.h"
#include "machine_i2c.h"

#include "driver/i2c_master.h"
#include "hal/i2c_ll.h"

#if MICROPY_PY_MACHINE_I2C || MICROPY_PY_MACHINE_SOFTI2C

#define I2C_DEFAULT_TIMEOUT_US (50000) // 50ms

typedef struct _machine_hw_i2c_obj_t {
    mp_obj_base_t base;
    i2c_master_bus_handle_t bus_handle;
    uint8_t port : 8;
    gpio_num_t scl : 8;
    gpio_num_t sda : 8;
    uint32_t freq;
    uint32_t timeout_us;
} machine_hw_i2c_obj_t;

static machine_hw_i2c_obj_t machine_hw_i2c_obj[I2C_NUM_MAX];

static void machine_hw_i2c_init(machine_hw_i2c_obj_t *self, bool first_init) {

    if (!first_init && self->bus_handle) {
        i2c_del_master_bus(self->bus_handle);
        self->bus_handle = NULL;
    }

    i2c_master_bus_config_t bus_cfg = {
        .i2c_port = self->port,
        .scl_io_num = self->scl,
        .sda_io_num = self->sda,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &self->bus_handle));
}

static int machine_i2c_transfer_single(mp_obj_base_t *self_in, uint16_t addr, size_t len, uint8_t *buf, unsigned int flags) {
    machine_hw_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // 1. Probe the address to see if any device responds.
    //    This test uses a fixed scl freq of 100_000.
    esp_err_t err = i2c_master_probe(self->bus_handle, addr, self->timeout_us / 1000);
    if (err != ESP_OK) {
        return -MP_ENODEV;   // No device at address, return immediately
    }

    if (len > 0) {
        // 2. Create a temporary device handle for this transaction
        //    This step for every transaction can be omitted in
        //    esp idf v5.5+, which supports handle address changing.
        i2c_device_config_t dev_cfg = {
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address = addr,
            .scl_speed_hz = self->freq,
        };
        i2c_master_dev_handle_t dev_handle;
        err = i2c_master_bus_add_device(self->bus_handle, &dev_cfg, &dev_handle);
        if (err != ESP_OK) {
            return -MP_ENODEV;
        }
        // 3. Transfer data
        if (flags & MP_MACHINE_I2C_FLAG_READ) {
            err = i2c_master_receive(dev_handle, buf, len, self->timeout_us / 1000);
        } else {
            err = i2c_master_transmit(dev_handle, buf, len, self->timeout_us / 1000);
        }
        // 4. Destroy the temporary handle
        i2c_master_bus_rm_device(dev_handle);
        // 5. Map errors
        if (err == ESP_FAIL) {
            return -MP_ENODEV;
        }
        if (err == ESP_ERR_TIMEOUT) {
            return -MP_ETIMEDOUT;
        }
        if (err != ESP_OK) {
            return -abs(err);
        }
    }
    return len;
}

static void machine_hw_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_hw_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2C(%u, scl=%u, sda=%u, freq=%u, timeout=%u)",
        self->port, self->scl, self->sda, self->freq, self->timeout_us);
}

mp_obj_t machine_hw_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Create a SoftI2C instance if no id is specified (or is -1) but other arguments are given
    if (n_args != 0) {
        MP_MACHINE_I2C_CHECK_FOR_LEGACY_SOFTI2C_CONSTRUCTION(n_args, n_kw, all_args);
    }

    // Parse args
    enum { ARG_id, ARG_scl, ARG_sda, ARG_freq, ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_INT, {.u_int = I2C_NUM_0} },
        { MP_QSTR_scl, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_sda, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_freq, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t i2c_id = args[ARG_id].u_int;
    if (!(I2C_NUM_0 <= i2c_id && i2c_id < I2C_NUM_MAX)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2C(%d) doesn't exist"), i2c_id);
    }

    machine_hw_i2c_obj_t *self = &machine_hw_i2c_obj[i2c_id];

    bool first_init = (self->base.type == NULL);
    if (first_init) {
        self->base.type = &machine_i2c_type;
        self->port = i2c_id;
        self->scl = (i2c_id == I2C_NUM_0) ? MICROPY_HW_I2C0_SCL : MICROPY_HW_I2C1_SCL;
        self->sda = (i2c_id == I2C_NUM_0) ? MICROPY_HW_I2C0_SDA : MICROPY_HW_I2C1_SDA;
        self->freq = 400000;
        self->timeout_us = I2C_DEFAULT_TIMEOUT_US;
    }

    if (args[ARG_scl].u_obj != MP_OBJ_NULL) {
        self->scl = machine_pin_get_id(args[ARG_scl].u_obj);
    }
    if (args[ARG_sda].u_obj != MP_OBJ_NULL) {
        self->sda = machine_pin_get_id(args[ARG_sda].u_obj);
    }
    if (args[ARG_freq].u_int != -1) {
        self->freq = args[ARG_freq].u_int;
    }
    if (args[ARG_timeout].u_int != -1) {
        self->timeout_us = args[ARG_timeout].u_int;
    }

    machine_hw_i2c_init(self, first_init);
    return MP_OBJ_FROM_PTR(self);
}

static const mp_machine_i2c_p_t machine_hw_i2c_p = {
    .transfer = mp_machine_i2c_transfer_adaptor,
    .transfer_single = machine_i2c_transfer_single,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_i2c_type,
    MP_QSTR_I2C,
    MP_TYPE_FLAG_NONE,
    make_new, machine_hw_i2c_make_new,
    print, machine_hw_i2c_print,
    protocol, &machine_hw_i2c_p,
    locals_dict, &mp_machine_i2c_locals_dict
    );

#endif // MICROPY_PY_MACHINE_I2C || MICROPY_PY_MACHINE_SOFTI2C
