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

#define IRQ_PRI_I2C NVIC_EncodePriority(NVIC_PRIORITYGROUP_4, 13, 0)

typedef struct _machine_i2c_target_obj_t {
    mp_obj_base_t base;
    I2C_TypeDef *i2c;
    int irqn;
    mp_hal_pin_obj_t scl;
    mp_hal_pin_obj_t sda;
} machine_i2c_target_obj_t;

static const machine_i2c_target_obj_t machine_i2cslave_obj[] = {
    #if defined(MICROPY_HW_I2C1_SCL)
    {{&machine_i2c_target_type}, I2C1, I2C1_EV_IRQn, MICROPY_HW_I2C1_SCL, MICROPY_HW_I2C1_SDA},
    #else
    {{NULL}, NULL, 0, NULL, NULL},
    #endif
    #if defined(MICROPY_HW_I2C2_SCL)
    {{&machine_i2c_target_type}, I2C2, I2C2_EV_IRQn, MICROPY_HW_I2C2_SCL, MICROPY_HW_I2C2_SDA},
    #else
    {{NULL}, NULL, 0, NULL, NULL},
    #endif
    #if defined(MICROPY_HW_I2C3_SCL)
    {{&machine_i2c_target_type}, I2C3, I2C3_EV_IRQn, MICROPY_HW_I2C3_SCL, MICROPY_HW_I2C3_SDA},
    #else
    {{NULL}, NULL, 0, NULL, NULL},
    #endif
    #if defined(MICROPY_HW_I2C4_SCL)
    {{&machine_i2c_target_type}, I2C4, I2C4_EV_IRQn, MICROPY_HW_I2C4_SCL, MICROPY_HW_I2C4_SDA},
    #else
    {{NULL}, NULL, 0, NULL, NULL},
    #endif
};

static machine_i2c_target_data_t i2c_target_data[4];

/******************************************************************************/
// stm32 hardware bindings

static machine_i2c_target_obj_t *get_self(i2c_slave_t *i2c) {
    size_t i2c_id = ((uintptr_t)i2c - I2C1_BASE) / (I2C2_BASE - I2C1_BASE);
    return (machine_i2c_target_obj_t *)&machine_i2cslave_obj[i2c_id];
}

static machine_i2c_target_data_t *get_data(i2c_slave_t *i2c) {
    size_t i2c_id = ((uintptr_t)i2c - I2C1_BASE) / (I2C2_BASE - I2C1_BASE);
    return &i2c_target_data[i2c_id];
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

static void mp_machine_i2c_target_deinit_all_port(void) {
}

static void mp_machine_i2c_target_event_callback(machine_i2c_target_irq_obj_t *irq) {
    mp_irq_handler(&irq->base);
}

static mp_int_t mp_machine_i2c_target_read_bytes(machine_i2c_target_obj_t *self, size_t len, uint8_t *buf) {
    if (len > 0) {
        buf[0] = i2c_slave_read_byte(self->i2c);
        len = 1;
    }
    return len;
}

static mp_int_t mp_machine_i2c_target_write_bytes(machine_i2c_target_obj_t *self, size_t len, const uint8_t *buf) {
    if (len > 0) {
        i2c_slave_write_byte(self->i2c, buf[0]);
        len = 1;
    }
    return len;
}

static void mp_machine_i2c_target_irq_config(machine_i2c_target_obj_t *self, unsigned int trigger) {
}

static mp_obj_t mp_machine_i2c_target_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // parse args
    enum { ARG_id, ARG_addr, ARG_addrsize, ARG_mem, ARG_mem_addrsize };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_addr, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 7} },
        { MP_QSTR_mem, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_mem_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
        /*
        { MP_QSTR_scl, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_sda, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        */
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Work out i2c bus
    int i2c_id = 0;
    if (MP_OBJ_IS_STR(args[ARG_id].u_obj)) {
        const char *port = mp_obj_str_get_str(args[ARG_id].u_obj);
        if (0) {
        #ifdef MICROPY_HW_I2C1_NAME
        } else if (strcmp(port, MICROPY_HW_I2C1_NAME) == 0) {
            i2c_id = 1;
        #endif
        #ifdef MICROPY_HW_I2C2_NAME
        } else if (strcmp(port, MICROPY_HW_I2C2_NAME) == 0) {
            i2c_id = 2;
        #endif
        #ifdef MICROPY_HW_I2C3_NAME
        } else if (strcmp(port, MICROPY_HW_I2C3_NAME) == 0) {
            i2c_id = 3;
        #endif
        } else {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                MP_ERROR_TEXT("I2C(%s) doesn't exist"), port));
        }
    } else {
        i2c_id = mp_obj_get_int(args[ARG_id].u_obj);
        if (i2c_id < 1 || i2c_id > MP_ARRAY_SIZE(machine_i2cslave_obj)
            || machine_i2cslave_obj[i2c_id - 1].base.type == NULL) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                MP_ERROR_TEXT("I2C(%d) doesn't exist"), i2c_id));
        }
    }

    // Initialise data.
    MP_STATE_PORT(i2c_target_mem_obj)[i2c_id - 1] = args[ARG_mem].u_obj;
    machine_i2c_target_data_t *data = &i2c_target_data[i2c_id - 1];
    machine_i2c_target_data_init(data, args[ARG_mem].u_obj, args[ARG_mem_addrsize].u_int);

    // Get static target object.
    machine_i2c_target_obj_t *self = (machine_i2c_target_obj_t *)&machine_i2cslave_obj[i2c_id - 1];

    // Initialise the I2C target.
    mp_hal_pin_config_alt(self->scl, MP_HAL_PIN_MODE_ALT_OPEN_DRAIN, MP_HAL_PIN_PULL_NONE, AF_FN_I2C, i2c_id);
    mp_hal_pin_config_alt(self->sda, MP_HAL_PIN_MODE_ALT_OPEN_DRAIN, MP_HAL_PIN_PULL_NONE, AF_FN_I2C, i2c_id);
    i2c_slave_init(self->i2c, self->irqn, IRQ_PRI_I2C, args[ARG_addr].u_int);

    i2c_target_enabled |= 1 << (i2c_id - 1);

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_i2c_target_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2c_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2CTarget(%u, addr=%u)",
        self - &machine_i2cslave_obj[0] + 1,
        (self->i2c->OAR1 >> 1) & 0x7f);
}

static void mp_machine_i2c_target_deinit(machine_i2c_target_obj_t *self) {
    i2c_slave_shutdown(self->i2c, self->irqn);
}
