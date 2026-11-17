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

#include <zephyr/drivers/i2c.h>

#include "zephyr_device.h"

typedef struct _machine_i2c_target_obj_t {
    mp_obj_base_t base;
    const struct device *dev;
    struct i2c_target_config cfg;
    uint8_t state;
    uint8_t data_byte;
} machine_i2c_target_obj_t;

static machine_i2c_target_obj_t machine_i2c_target_obj[] = {
    {.base = {&machine_i2c_target_type}, .dev = NULL},
};

/******************************************************************************/
// zephyr bindings
//
// Note that it's possible to get callbacks in either of these sequences:
// - read_requested read_processed read_processed ... (eg STM32)
// - read_requested read_processed read_requested read_processed ... (eg RP2xxx / Design Ware)

static int i2c_target_write_requested(struct i2c_target_config *config) {
    machine_i2c_target_obj_t *self = CONTAINER_OF(config, machine_i2c_target_obj_t, cfg);
    self->state = STATE_WRITING;
    machine_i2c_target_data_addr_match(&machine_i2c_target_data[0], false);
    return 0;
}

static int i2c_target_write_received(struct i2c_target_config *config, uint8_t val) {
    machine_i2c_target_obj_t *self = CONTAINER_OF(config, machine_i2c_target_obj_t, cfg);
    machine_i2c_target_data_t *data = &machine_i2c_target_data[0];
    self->data_byte = val;
    machine_i2c_target_data_write_request(self, data);
    return 0;
}

static int i2c_target_read_requested(struct i2c_target_config *config, uint8_t *val) {
    machine_i2c_target_obj_t *self = CONTAINER_OF(config, machine_i2c_target_obj_t, cfg);
    machine_i2c_target_data_t *data = &machine_i2c_target_data[0];
    if (self->state != STATE_READING) {
        machine_i2c_target_data_addr_match(data, true);
        machine_i2c_target_data_read_request(self, data);
        self->state = STATE_READING;
    }
    *val = self->data_byte;
    return 0;
}

static int i2c_target_read_processed(struct i2c_target_config *config, uint8_t *val) {
    machine_i2c_target_obj_t *self = CONTAINER_OF(config, machine_i2c_target_obj_t, cfg);
    machine_i2c_target_data_t *data = &machine_i2c_target_data[0];
    machine_i2c_target_data_read_request(self, data);
    *val = self->data_byte;
    return 0;
}

// called only on stop, not restart
static int i2c_target_stop(struct i2c_target_config *config) {
    machine_i2c_target_obj_t *self = CONTAINER_OF(config, machine_i2c_target_obj_t, cfg);
    if (self->state == STATE_IDLE) {
        // Assume a stop without a start is a 0-byte write.
        machine_i2c_target_data_addr_match(&machine_i2c_target_data[0], false);
    }
    self->state = STATE_IDLE;
    machine_i2c_target_data_t *data = &machine_i2c_target_data[0];
    machine_i2c_target_data_stop(data);
    return 0;
}

static struct i2c_target_callbacks i2c_target_callbacks = {
    .write_requested = i2c_target_write_requested,
    .read_requested = i2c_target_read_requested,
    .write_received = i2c_target_write_received,
    .read_processed = i2c_target_read_processed,
    .stop = i2c_target_stop,
};

/******************************************************************************/
// I2CTarget port implementation

static inline size_t mp_machine_i2c_target_get_index(machine_i2c_target_obj_t *self) {
    return 0;
}

static void mp_machine_i2c_target_event_callback(machine_i2c_target_irq_obj_t *irq) {
    mp_irq_handler(&irq->base);
}

static size_t mp_machine_i2c_target_read_bytes(machine_i2c_target_obj_t *self, size_t len, uint8_t *buf) {
    buf[0] = self->data_byte;
    return 1;
}

static size_t mp_machine_i2c_target_write_bytes(machine_i2c_target_obj_t *self, size_t len, const uint8_t *buf) {
    self->data_byte = buf[0];
    return 1;
}

static inline void mp_machine_i2c_target_irq_config(machine_i2c_target_obj_t *self, unsigned int trigger) {
    (void)self;
    (void)trigger;
}

static mp_obj_t mp_machine_i2c_target_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_addr, ARG_addrsize, ARG_mem, ARG_mem_addrsize, ARG_scl, ARG_sda };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_addr, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 7} },
        { MP_QSTR_mem, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_mem_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
    };

    // Parse arguments.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const struct device *dev = zephyr_device_find(args[ARG_id].u_obj);

    machine_i2c_target_obj_t *self = &machine_i2c_target_obj[0];
    if (!(self->dev == NULL || self->dev == dev)) {
        mp_raise_ValueError(MP_ERROR_TEXT("only one I2CTarget supported"));
    }
    self->dev = dev;
    self->cfg.flags = 0;
    self->cfg.address = args[ARG_addr].u_int;
    self->cfg.callbacks = &i2c_target_callbacks;

    // Initialise data.
    self->state = STATE_IDLE;
    MP_STATE_PORT(machine_i2c_target_mem_obj)[0] = args[ARG_mem].u_obj;
    machine_i2c_target_data_t *data = &machine_i2c_target_data[0];
    machine_i2c_target_data_init(data, args[ARG_mem].u_obj, args[ARG_mem_addrsize].u_int);

    // Initialise the I2C target.
    int ret = i2c_target_register(self->dev, &self->cfg);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_i2c_target_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2c_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2CTarget(%s, addr=%u)",
        self->dev == NULL ? "" : self->dev->name, self->cfg.address);
}

static void mp_machine_i2c_target_deinit(machine_i2c_target_obj_t *self) {
    i2c_target_unregister(self->dev, &self->cfg);
    self->dev = NULL;
}
