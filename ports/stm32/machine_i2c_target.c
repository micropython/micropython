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

#include "i2c.h"
#include "i2cslave.h"
#include "irq.h"

typedef struct _machine_i2c_target_obj_t {
    mp_obj_base_t base;
    I2C_TypeDef *i2c;
    uint16_t irqn_ev;
    uint16_t irqn_er;
    mp_hal_pin_obj_t scl;
    mp_hal_pin_obj_t sda;
} machine_i2c_target_obj_t;

static const machine_i2c_target_obj_t machine_i2c_target_obj[] = {
    #if defined(MICROPY_HW_I2C1_SCL)
    {{&machine_i2c_target_type}, I2C1, I2C1_EV_IRQn, I2C1_ER_IRQn, MICROPY_HW_I2C1_SCL, MICROPY_HW_I2C1_SDA},
    #else
    {{NULL}, NULL, 0, 0, NULL, NULL},
    #endif
    #if defined(MICROPY_HW_I2C2_SCL)
    {{&machine_i2c_target_type}, I2C2, I2C2_EV_IRQn, I2C2_ER_IRQn, MICROPY_HW_I2C2_SCL, MICROPY_HW_I2C2_SDA},
    #else
    {{NULL}, NULL, 0, 0, NULL, NULL},
    #endif
    #if defined(MICROPY_HW_I2C3_SCL)
    {{&machine_i2c_target_type}, I2C3, I2C3_EV_IRQn, I2C3_ER_IRQn, MICROPY_HW_I2C3_SCL, MICROPY_HW_I2C3_SDA},
    #else
    {{NULL}, NULL, 0, 0, NULL, NULL},
    #endif
    #if defined(MICROPY_HW_I2C4_SCL)
    {{&machine_i2c_target_type}, I2C4, I2C4_EV_IRQn, I2C4_ER_IRQn, MICROPY_HW_I2C4_SCL, MICROPY_HW_I2C4_SDA},
    #else
    {{NULL}, NULL, 0, 0, NULL, NULL},
    #endif
};

/******************************************************************************/
// stm32 hardware bindings

static machine_i2c_target_obj_t *get_self(i2c_slave_t *i2c) {
    size_t i2c_id = ((uintptr_t)i2c - I2C1_BASE) / (I2C2_BASE - I2C1_BASE);
    return (machine_i2c_target_obj_t *)&machine_i2c_target_obj[i2c_id];
}

static machine_i2c_target_data_t *get_data(i2c_slave_t *i2c) {
    size_t i2c_id = ((uintptr_t)i2c - I2C1_BASE) / (I2C2_BASE - I2C1_BASE);
    return &machine_i2c_target_data[i2c_id];
}

int i2c_slave_process_addr_match(i2c_slave_t *i2c, int rw) {
    machine_i2c_target_data_addr_match(get_data(i2c), rw);
    return 0;
}

int i2c_slave_process_rx_byte(i2c_slave_t *i2c) {
    machine_i2c_target_data_write_request(get_self(i2c), get_data(i2c));
    return 0;
}

void i2c_slave_process_rx_end(i2c_slave_t *i2c) {
    machine_i2c_target_data_stop(get_data(i2c));
}

void i2c_slave_process_tx_byte(i2c_slave_t *i2c) {
    machine_i2c_target_data_read_request(get_self(i2c), get_data(i2c));
}

void i2c_slave_process_tx_end(i2c_slave_t *i2c) {
    machine_i2c_target_data_stop(get_data(i2c));
}

/******************************************************************************/
// I2CTarget port implementation

static inline size_t mp_machine_i2c_target_get_index(machine_i2c_target_obj_t *self) {
    return self - &machine_i2c_target_obj[0];
}

static inline void mp_machine_i2c_target_event_callback(machine_i2c_target_irq_obj_t *irq) {
    mp_irq_handler(&irq->base);
}

static size_t mp_machine_i2c_target_read_bytes(machine_i2c_target_obj_t *self, size_t len, uint8_t *buf) {
    if (len > 0) {
        buf[0] = i2c_slave_read_byte(self->i2c);
        len = 1;
    }
    return len;
}

static size_t mp_machine_i2c_target_write_bytes(machine_i2c_target_obj_t *self, size_t len, const uint8_t *buf) {
    if (len > 0) {
        i2c_slave_write_byte(self->i2c, buf[0]);
        len = 1;
    }
    return len;
}

static inline void mp_machine_i2c_target_irq_config(machine_i2c_target_obj_t *self, unsigned int trigger) {
    (void)self;
    (void)trigger;
}

static mp_obj_t mp_machine_i2c_target_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Parse arguments.
    enum { ARG_id, ARG_addr, ARG_addrsize, ARG_mem, ARG_mem_addrsize };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_addr, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 7} },
        { MP_QSTR_mem, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_mem_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Work out I2C bus.
    int i2c_id = i2c_find_peripheral(args[ARG_id].u_obj);

    // Get static target object.
    machine_i2c_target_obj_t *self = (machine_i2c_target_obj_t *)&machine_i2c_target_obj[i2c_id - 1];

    // Initialise data.
    MP_STATE_PORT(machine_i2c_target_mem_obj)[i2c_id - 1] = args[ARG_mem].u_obj;
    machine_i2c_target_data_t *data = &machine_i2c_target_data[i2c_id - 1];
    machine_i2c_target_data_init(data, args[ARG_mem].u_obj, args[ARG_mem_addrsize].u_int);

    // Initialise the I2C target.
    mp_hal_pin_config_alt(self->scl, MP_HAL_PIN_MODE_ALT_OPEN_DRAIN, MP_HAL_PIN_PULL_NONE, AF_FN_I2C, i2c_id);
    mp_hal_pin_config_alt(self->sda, MP_HAL_PIN_MODE_ALT_OPEN_DRAIN, MP_HAL_PIN_PULL_NONE, AF_FN_I2C, i2c_id);
    i2c_slave_init(self->i2c, self->irqn_ev, IRQ_PRI_I2C, args[ARG_addr].u_int);
    NVIC_SetPriority(self->irqn_er, IRQ_PRI_I2C);
    NVIC_EnableIRQ(self->irqn_er);

    i2c_target_enabled |= 1 << (i2c_id - 1);

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_i2c_target_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2c_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2CTarget(%u, addr=%u)",
        self - &machine_i2c_target_obj[0] + 1,
        (self->i2c->OAR1 >> 1) & 0x7f);
}

static void mp_machine_i2c_target_deinit(machine_i2c_target_obj_t *self) {
    i2c_slave_shutdown(self->i2c, self->irqn_ev);
    NVIC_DisableIRQ(self->irqn_er);
}
