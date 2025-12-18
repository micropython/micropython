/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Damien P. George
 * Copyright (c) 2025 Robert Hammelrath
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

#include "fsl_lpi2c.h"
#include "machine_i2c.h"
#include CLOCK_CONFIG_H

typedef struct _machine_i2c_target_obj_t {
    mp_obj_base_t base;
    LPI2C_Type *i2c_inst;
    uint8_t i2c_id;
    uint8_t addr;
    lpi2c_slave_config_t slave_config;
    lpi2c_slave_handle_t handle;
} machine_i2c_target_obj_t;

static void lpi2c_slave_callback(LPI2C_Type *base, lpi2c_slave_transfer_t *xfer, void *param) {
    machine_i2c_target_obj_t *self = (machine_i2c_target_obj_t *)param;
    machine_i2c_target_data_t *data = &machine_i2c_target_data[self->i2c_id];

    switch (xfer->event) {
        case kLPI2C_SlaveAddressMatchEvent:
            // Controller addressed us.
            machine_i2c_target_data_addr_match(data, xfer->receivedAddress & 1);
            break;
        case kLPI2C_SlaveReceiveEvent:
            // Data from controller is available for reading.
            machine_i2c_target_data_write_request(self, data);
            break;
        case kLPI2C_SlaveTransmitEvent:
            // Controller is requesting data.
            machine_i2c_target_data_read_request(self, data);
            break;
        case kLPI2C_SlaveCompletionEvent:
            // Transfer done.
            machine_i2c_target_data_stop(data);
            break;
        default:
            break;
    }
}

/******************************************************************************/
// I2CTarget port implementation

static inline size_t mp_machine_i2c_target_get_index(machine_i2c_target_obj_t *self) {
    return self->i2c_id;
}

static inline void mp_machine_i2c_target_event_callback(machine_i2c_target_irq_obj_t *irq) {
    mp_irq_handler(&irq->base);
}

static size_t mp_machine_i2c_target_read_bytes(machine_i2c_target_obj_t *self, size_t len, uint8_t *buf) {
    // LPI2C_Type *i2c_inst = self->i2c_inst;
    // mp_int_t i = 0;
    // mp_int_t val = 0;
    // while (i < len && !((val = i2c_inst->SRDR) & LPI2C_SRDR_RXEMPTY_MASK)) {
    //     buf[i++] = (uint8_t)(val & 0xff);
    // }
    // return i;
    // Simple and fast version for len == 1
    buf[0] = (uint8_t)(self->i2c_inst->SRDR);
    return 1;
}

static size_t mp_machine_i2c_target_write_bytes(machine_i2c_target_obj_t *self, size_t len, const uint8_t *buf) {
    self->i2c_inst->STDR = buf[0];
    return 1;
}

static inline void mp_machine_i2c_target_irq_config(machine_i2c_target_obj_t *self, unsigned int trigger) {
    (void)self;
    (void)trigger;
}

static mp_obj_t mp_machine_i2c_target_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_addr, ARG_addrsize, ARG_mem, ARG_mem_addrsize, ARG_drive };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_INT, {.u_int = DEFAULT_I2C_ID} },
        { MP_QSTR_addr, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 7} },
        { MP_QSTR_mem, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE}},
        { MP_QSTR_mem_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_drive, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_I2C_DRIVE} },
    };

    // Parse arguments.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get I2C bus.
    int i2c_id = args[ARG_id].u_int;
    if (i2c_id < 0 || i2c_id >= micropy_hw_i2c_num || i2c_index_table[i2c_id] == 0) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2C(%d) doesn't exist"), i2c_id);
    }
    int i2c_hw_id = i2c_index_table[i2c_id]; // the hw i2c number 1..n

    // Get I2C Object.
    machine_i2c_target_obj_t *self = mp_obj_malloc_with_finaliser(machine_i2c_target_obj_t, &machine_i2c_target_type);
    self->i2c_id = i2c_id;
    self->i2c_inst = i2c_base_ptr_table[i2c_hw_id];
    uint8_t drive = args[ARG_drive].u_int;
    if (drive < 1 || drive > 7) {
        drive = DEFAULT_I2C_DRIVE;
    }
    // Set the target address.
    self->addr = args[ARG_addr].u_int;
    // Initialise data.
    MP_STATE_PORT(machine_i2c_target_mem_obj)[i2c_id] = args[ARG_mem].u_obj;
    machine_i2c_target_data_t *data = &machine_i2c_target_data[self->i2c_id];
    machine_i2c_target_data_init(data, args[ARG_mem].u_obj, args[ARG_mem_addrsize].u_int);

    // Initialise the GPIO pins
    lpi2c_set_iomux(i2c_hw_id, drive);
    // Initialise the I2C peripheral
    LPI2C_SlaveGetDefaultConfig(&self->slave_config);
    self->slave_config.address0 = self->addr;
    self->slave_config.sdaGlitchFilterWidth_ns = DEFAULT_I2C_FILTER_NS;
    self->slave_config.sclGlitchFilterWidth_ns = DEFAULT_I2C_FILTER_NS;
    LPI2C_SlaveInit(self->i2c_inst, &self->slave_config, BOARD_BOOTCLOCKRUN_LPI2C_CLK_ROOT);
    // Create the LPI2C handle for the non-blocking transfer
    LPI2C_SlaveTransferCreateHandle(self->i2c_inst, &self->handle, lpi2c_slave_callback, self);
    // Start accepting I2C transfers on the LPI2C slave peripheral
    status_t reVal = LPI2C_SlaveTransferNonBlocking(self->i2c_inst, &self->handle,
        kLPI2C_SlaveAddressMatchEvent | kLPI2C_SlaveTransmitEvent | kLPI2C_SlaveReceiveEvent | kLPI2C_SlaveCompletionEvent);
    if (reVal != kStatus_Success) {
        mp_raise_ValueError(MP_ERROR_TEXT("cannot start I2C"));
    }
    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_i2c_target_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2c_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2CTarget(%u, addr=%u)", self->i2c_id, self->addr);
}

// Stop the Slave transfer and free the memory objects.
static void mp_machine_i2c_target_deinit(machine_i2c_target_obj_t *self) {
    LPI2C_SlaveDeinit(self->i2c_inst);
}
