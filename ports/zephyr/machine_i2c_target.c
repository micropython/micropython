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

static machine_i2c_target_data_t i2c_target_data[4];

typedef struct _machine_i2c_target_obj_t {
    mp_obj_base_t base;
    const struct device *dev;
    struct i2c_target_config cfg;
} machine_i2c_target_obj_t;

static bool writing = false;
static bool reading = false;
static uint8_t global_val;

/******************************************************************************/
// zephyr bindings

static int i2c_target_write_requested(struct i2c_target_config *config) {
    // printk("wr req\n");
    writing = true;
    reading = false;
    machine_i2c_target_data_addr_match(&i2c_target_data[0], false);
    return 0;
}

static int i2c_target_write_received(struct i2c_target_config *config, uint8_t val) {
    // printk("wr recv %d\n", val);
    global_val = val;
    machine_i2c_target_obj_t *self = CONTAINER_OF(config, machine_i2c_target_obj_t, cfg);
    machine_i2c_target_data_t *data = &i2c_target_data[0];
    machine_i2c_target_data_write_request(self, data);
    return 0;
}

// it's possible to get either:
// - req proc proc ... (eg STM32)
// - req proc req proc ... (eg RP2xxx / Design Ware)
static int i2c_target_read_requested(struct i2c_target_config *config, uint8_t *val) {
    // printk("rd req\n");
    writing = false;
    machine_i2c_target_obj_t *self = CONTAINER_OF(config, machine_i2c_target_obj_t, cfg);
    machine_i2c_target_data_t *data = &i2c_target_data[0];
    if (!reading) {
        machine_i2c_target_data_addr_match(data, true);
        machine_i2c_target_data_read_request(self, data);
        reading = true;
    }
    *val = global_val;
    return 0;
}

static int i2c_target_read_processed(struct i2c_target_config *config, uint8_t *val) {
    // printk("rd processed\n");
    machine_i2c_target_obj_t *self = CONTAINER_OF(config, machine_i2c_target_obj_t, cfg);
    machine_i2c_target_data_t *data = &i2c_target_data[0];
    machine_i2c_target_data_read_request(self, data);
    *val = global_val;
    return 0;
}

// called only on stop, not restart
static int i2c_target_stop(struct i2c_target_config *config) {
    // printk("stop\n");
    if (!writing && !reading) {
        // Assume a stop without a start is a 0-byte write.
        machine_i2c_target_data_addr_match(&i2c_target_data[0], false);
    }
    writing = false;
    reading = false;
    machine_i2c_target_data_t *data = &i2c_target_data[0];
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

static void mp_machine_i2c_target_deinit_all_port(void) {
}

static void mp_machine_i2c_target_event_callback(machine_i2c_target_irq_obj_t *irq) {
    char dummy;
    void *orig_top = MP_STATE_THREAD(stack_top);
    mp_uint_t orig_limit = MP_STATE_THREAD(stack_limit);
    MP_STATE_THREAD(stack_top) = &dummy;
    MP_STATE_THREAD(stack_limit) = CONFIG_ISR_STACK_SIZE - 512;
    mp_irq_handler(&irq->base);
    MP_STATE_THREAD(stack_top) = orig_top;
    MP_STATE_THREAD(stack_limit) = orig_limit;
}

static size_t mp_machine_i2c_target_read_bytes(machine_i2c_target_obj_t *self, size_t len, uint8_t *buf) {
    buf[0] = global_val;
    return 1;
}

static size_t mp_machine_i2c_target_write_bytes(machine_i2c_target_obj_t *self, size_t len, const uint8_t *buf) {
    global_val = buf[0];
    return 1;
}

static void mp_machine_i2c_target_irq_config(machine_i2c_target_obj_t *self, unsigned int trigger) {
}

static mp_obj_t mp_machine_i2c_target_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // TODO: reconsider order of arguments
    enum { ARG_id, ARG_addr, ARG_addrsize, ARG_mem, ARG_mem_addrsize, ARG_scl, ARG_sda };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_addr, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 7} },
        { MP_QSTR_mem, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_mem_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
        // { MP_QSTR_scl, MICROPY_I2C_PINS_ARG_OPTS | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        // { MP_QSTR_sda, MICROPY_I2C_PINS_ARG_OPTS | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const struct device *dev = zephyr_device_find(args[ARG_id].u_obj);

    machine_i2c_target_obj_t *self = mp_obj_malloc(machine_i2c_target_obj_t, &machine_i2c_target_type);
    self->dev = dev;
    self->cfg.flags = 0;
    self->cfg.address = args[ARG_addr].u_int;
    self->cfg.callbacks = &i2c_target_callbacks;

    // Initialise data.
    MP_STATE_PORT(i2c_target_mem_obj)[0] = args[ARG_mem].u_obj;
    machine_i2c_target_data_t *data = &i2c_target_data[0];
    machine_i2c_target_data_init(data, args[ARG_mem].u_obj, args[ARG_mem_addrsize].u_int);

    #if 0
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
    #endif

    // Initialise the I2C target.
    // TODO: don't reinitialize if no arguments given.
    int ret = i2c_target_register(self->dev, &self->cfg);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
    /*
    i2c_slave_init(self->i2c_inst, self->addr, i2c_target_handler);
    gpio_set_function(self->scl, GPIO_FUNC_I2C);
    gpio_set_function(self->sda, GPIO_FUNC_I2C);
    gpio_set_pulls(self->scl, true, 0);
    gpio_set_pulls(self->sda, true, 0);
    */

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_i2c_target_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2c_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2CTarget(%s, addr=%u)",
        self->dev->name, self->cfg.address);
}

static void mp_machine_i2c_target_deinit(machine_i2c_target_obj_t *self) {
    i2c_target_unregister(self->dev, &self->cfg);
}
