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

#include "machine_i2c.h"
#include "driver/i2c_slave.h"

// These headers are needed to call i2c_ll_txfifo_rst().
#include "hal/i2c_ll.h"
#include "../i2c_private.h"

typedef struct _machine_i2c_target_obj_t {
    mp_obj_base_t base;
    i2c_slave_dev_handle_t handle;
    i2c_slave_config_t config;
    uint8_t state;
    bool stop_pending;
    bool irq_active;
    int index;
    const i2c_slave_rx_done_event_data_t *rx_done_event_data;
} machine_i2c_target_obj_t;

static machine_i2c_target_obj_t machine_i2c_target_obj[I2C_NUM_MAX];

/******************************************************************************/
// ESP-IDF hardware bindings

// Called when the controller is about to read from the TX/send buffer.
static bool i2c_slave_request_cb(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_request_event_data_t *evt_data, void *arg) {
    machine_i2c_target_obj_t *self = arg;
    machine_i2c_target_data_t *data = &machine_i2c_target_data[self->config.i2c_port];

    // Flush hardware TX FIFO to get rid of any data from a previous read.
    i2c_ll_txfifo_rst(self->handle->base->hal.dev);

    // Perform an entire read transaction, including start, read and stop events.
    // We don't know how much data the controller will read, so write the entire
    // memory buffer to the TX FIFO.
    machine_i2c_target_data_addr_match(data, true);
    for (uint32_t i = 0; i < data->mem_len; ++i) {
        machine_i2c_target_data_read_request(self, data);
    }
    machine_i2c_target_data_restart_or_stop(data);

    // A higher priority task was not woken up.
    return false;
}

// Called when the controller has written into the RX/receive buffer.
static bool i2c_slave_receive_cb(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_rx_done_event_data_t *evt_data, void *arg) {
    machine_i2c_target_obj_t *self = arg;
    machine_i2c_target_data_t *data = &machine_i2c_target_data[self->config.i2c_port];

    // Perform an entire write transaction, including start, read and stop events.
    machine_i2c_target_data_addr_match(data, false);
    self->index = 0;
    self->rx_done_event_data = evt_data;
    while (self->index < self->rx_done_event_data->length) {
        machine_i2c_target_data_write_request(self, data);
    }
    machine_i2c_target_data_restart_or_stop(data);

    // A higher priority task was not woken up.
    return false;
}

static void i2c_target_init(machine_i2c_target_obj_t *self, machine_i2c_target_data_t *data, uint32_t addr, uint32_t addrsize, bool first_init) {
    if (!first_init && self->handle != NULL) {
        i2c_del_slave_device(self->handle);
        self->handle = NULL;
    }

    self->config.clk_source = I2C_CLK_SRC_DEFAULT;
    self->config.slave_addr = addr;
    self->config.send_buf_depth = data->mem_len;
    self->config.receive_buf_depth = data->mem_len;
    if (addrsize == 7) {
        self->config.addr_bit_len = I2C_ADDR_BIT_LEN_7;
    } else {
        #if SOC_I2C_SUPPORT_10BIT_ADDR
        self->config.addr_bit_len = I2C_ADDR_BIT_LEN_10;
        #else
        mp_raise_ValueError(MP_ERROR_TEXT("10-bit address unsupported"));
        #endif
    }
    self->config.intr_priority = 0; // 0 selects the default
    self->config.flags.allow_pd = 0;
    self->config.flags.enable_internal_pullup = 1;

    ESP_ERROR_CHECK(i2c_new_slave_device(&self->config, &self->handle));
    i2c_slave_event_callbacks_t cbs = {
        .on_receive = i2c_slave_receive_cb,
        .on_request = i2c_slave_request_cb,
    };
    ESP_ERROR_CHECK(i2c_slave_register_event_callbacks(self->handle, &cbs, self));
}

/******************************************************************************/
// I2CTarget port implementation

static inline size_t mp_machine_i2c_target_get_index(machine_i2c_target_obj_t *self) {
    return self->config.i2c_port;
}

static void mp_machine_i2c_target_event_callback(machine_i2c_target_irq_obj_t *irq) {
    mp_irq_handler(&irq->base);
}

static size_t mp_machine_i2c_target_read_bytes(machine_i2c_target_obj_t *self, size_t len, uint8_t *buf) {
    size_t i = 0;
    while (i < len && self->index < self->rx_done_event_data->length) {
        buf[i++] = self->rx_done_event_data->buffer[self->index++];
    }
    return i;
}

static size_t mp_machine_i2c_target_write_bytes(machine_i2c_target_obj_t *self, size_t len, const uint8_t *buf) {
    uint32_t write_len;
    i2c_slave_write(self->handle, buf, len, &write_len, 1000);
    return write_len;
}

static void mp_machine_i2c_target_irq_config(machine_i2c_target_obj_t *self, unsigned int trigger) {
}

static mp_obj_t mp_machine_i2c_target_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_addr, ARG_addrsize, ARG_mem, ARG_mem_addrsize, ARG_scl, ARG_sda };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_addr, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 7} },
        { MP_QSTR_mem, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_mem_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_scl, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_sda, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int i2c_id = args[ARG_id].u_int;

    // Check if the I2C bus is valid
    if (i2c_id < 0 || i2c_id >= MP_ARRAY_SIZE(machine_i2c_target_obj)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2CTarget(%d) doesn't exist"), i2c_id);
    }

    // Get static peripheral object.
    machine_i2c_target_obj_t *self = &machine_i2c_target_obj[i2c_id];

    bool first_init = false;
    if (self->base.type == NULL) {
        // Created for the first time, set default pins
        self->base.type = &machine_i2c_target_type;
        self->config.i2c_port = i2c_id;
        if (self->config.i2c_port == 0) {
            self->config.scl_io_num = MICROPY_HW_I2C0_SCL;
            self->config.sda_io_num = MICROPY_HW_I2C0_SDA;
        } else {
            self->config.scl_io_num = MICROPY_HW_I2C1_SCL;
            self->config.sda_io_num = MICROPY_HW_I2C1_SDA;
        }
        first_init = true;
    }

    // Initialise data.
    self->state = STATE_IDLE;
    self->stop_pending = false;
    self->irq_active = false;
    MP_STATE_PORT(machine_i2c_target_mem_obj)[i2c_id] = args[ARG_mem].u_obj;
    machine_i2c_target_data_t *data = &machine_i2c_target_data[i2c_id];
    machine_i2c_target_data_init(data, args[ARG_mem].u_obj, args[ARG_mem_addrsize].u_int);

    // Set SCL/SDA pins if configured.
    if (args[ARG_scl].u_obj != mp_const_none) {
        self->config.scl_io_num = mp_hal_get_pin_obj(args[ARG_scl].u_obj);
    }
    if (args[ARG_sda].u_obj != mp_const_none) {
        self->config.sda_io_num = mp_hal_get_pin_obj(args[ARG_sda].u_obj);
    }

    // Initialise the I2C target.
    i2c_target_init(self, data, args[ARG_addr].u_int, args[ARG_addrsize].u_int, first_init);

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_i2c_target_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2c_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2CTarget(%u, addr=%u, scl=%u, sda=%u)",
        self->config.i2c_port, self->config.slave_addr, self->config.scl_io_num, self->config.sda_io_num);
}

static void mp_machine_i2c_target_deinit(machine_i2c_target_obj_t *self) {
    if (self->handle != NULL) {
        i2c_del_slave_device(self->handle);
        self->handle = NULL;
    }
}
