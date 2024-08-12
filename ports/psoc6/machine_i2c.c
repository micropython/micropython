/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2024 Infineon Technologies AG
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
#include "machine_pin_phy.h"
#include "mplogger.h"

#define DEFAULT_I2C_FREQ     (400000)
#define PSOC_I2C_MASTER_MODE (CYHAL_I2C_MODE_MASTER)

#define i2c_assert_raise_val(msg, ret)   if (ret != CY_RSLT_SUCCESS) { \
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT(msg), ret); \
}

typedef struct _machine_i2c_obj_t {
    mp_obj_base_t base;
    int id; // This parameter is unused and added for compliance with reference API.
    cyhal_i2c_t i2c_obj;
    uint32_t scl_pin;
    uint32_t sda_pin;
    cyhal_i2c_cfg_t cfg;
    mp_obj_t callback;
} machine_i2c_obj_t;

machine_i2c_obj_t *i2c_obj[MAX_I2C] = { NULL };

static inline machine_i2c_obj_t *i2c_obj_alloc(bool is_slave) {
    for (uint8_t i = 0; i < MAX_I2C; i++)
    {
        if (i2c_obj[i] == NULL) {

            const mp_obj_type_t *obj_type;
            if (is_slave) {
                obj_type = &machine_i2c_slave_type;
            } else {
                obj_type = &machine_i2c_type;
            }
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

static void i2c_init(machine_i2c_obj_t *machine_i2c_obj, uint32_t freq_hz, int16_t slave_addr) {
    // Define the I2C master configuration structure

    // Set cgf frequency
    machine_i2c_obj->cfg.frequencyhal_hz = freq_hz;

    // Set mode master or slave
    if (slave_addr <= 0) {
        machine_i2c_obj->cfg.is_slave = CYHAL_I2C_MODE_MASTER;
        machine_i2c_obj->cfg.address = 0;
    } else {
        machine_i2c_obj->cfg.is_slave = CYHAL_I2C_MODE_SLAVE;
        machine_i2c_obj->cfg.address = slave_addr;
    }

    // Initialize I2C master, set the SDA and SCL pins and assign a new clock
    cy_rslt_t result = cyhal_i2c_init(&machine_i2c_obj->i2c_obj, machine_i2c_obj->sda_pin, machine_i2c_obj->scl_pin, NULL);
    assert_pin_phy_used(result);
    i2c_assert_raise_val("I2C initialisation failed with return code %lx !", result);

    result = cyhal_i2c_configure(&machine_i2c_obj->i2c_obj, &(machine_i2c_obj->cfg));
    i2c_assert_raise_val("I2C initialisation failed with return code %lx !", result);
}

static void machine_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2C(freq=%u, scl=%u, sda=%u, mode=%u, addr=%u)", self->cfg.frequencyhal_hz, self->scl_pin, self->sda_pin, self->cfg.is_slave, self->cfg.address);
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

    // get scl & sda pins & configure them
    self->scl_pin = pin_addr_by_name(args[ARG_scl].u_obj);
    self->sda_pin = pin_addr_by_name(args[ARG_sda].u_obj);

    // initialise I2C at cyhal level
    i2c_init(self, args[ARG_freq].u_int, 0);

    return MP_OBJ_FROM_PTR(self);
}

mp_obj_t machine_i2c_slave_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 0, 4, true);

    enum { ARG_id, ARG_freq, ARG_scl, ARG_sda, ARG_addr };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,   MP_ARG_INT, {.u_int = -1}},
        { MP_QSTR_freq, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_I2C_FREQ} },
        { MP_QSTR_scl, MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_sda, MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_addr, MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1}}
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    machine_i2c_obj_t *self = i2c_obj_alloc(true);
    if (self == NULL) {
        return mp_const_none;
    }

    // set id if provided
    if (args[ARG_id].u_int != -1) {
        self->id = args[ARG_id].u_int;
        mp_printf(&mp_plat_print, "machine.I2C: ID parameter is ignored in this port.\n");
    }

    // get scl & sda pins & configure them
    self->scl_pin = pin_addr_by_name(args[ARG_scl].u_obj);
    self->sda_pin = pin_addr_by_name(args[ARG_sda].u_obj);

    // initialise I2C at cyhal level
    i2c_init(self, args[ARG_freq].u_int, args[ARG_addr].u_int);

    return MP_OBJ_FROM_PTR(self);
}

static void machine_i2c_deinit(mp_obj_base_t *self_in) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    cyhal_i2c_free(&(self->i2c_obj));
    i2c_obj_free(self);
}

static int machine_i2c_transfer(mp_obj_base_t *self_in, uint16_t addr, size_t len, uint8_t *buf, unsigned int flags) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    cy_rslt_t result = CY_RSLT_SUCCESS;
    bool send_stop = (flags & MP_MACHINE_I2C_FLAG_STOP) ? true : false;
    uint32_t timeout = 0;

    if ((flags & MP_MACHINE_I2C_FLAG_READ) == MP_MACHINE_I2C_FLAG_READ) {
        result = cyhal_i2c_master_read(&self->i2c_obj, addr, buf, len, timeout, send_stop);
        i2c_assert_raise_val("I2C transfer failed with return code %lx !", result);

        return len;
    } else {
        // handle scan type bus checks and send stop
        if (buf == NULL) {
            result = cyhal_i2c_master_write(&self->i2c_obj, addr, buf, 0, 50, send_stop);

            if ((result != CY_RSLT_SUCCESS)) {
                // these 2 errors occur if nothing is attached to sda/scl, but they are pulled-up (0xaa2004) or not pulled-up (0xaa2003).
                // In the latter case, due to not reaction at all the timeout has to expire. Latency is therefore high.
                if (result != 0xaa2004 && result != 0xaa2003) {
                    i2c_assert_raise_val("I2C transfer failed with return code %lx !", result);
                }

                return 1;
            }

            return len;
        } else {
            result = cyhal_i2c_master_write(&self->i2c_obj, addr, buf, len, timeout, send_stop);
            i2c_assert_raise_val("I2C transfer failed with return code %lx !", result);
        }

        return len;
    }
}

static const mp_machine_i2c_p_t machine_i2c_p = {
    .deinit = machine_i2c_deinit,
    .transfer_single = machine_i2c_transfer,
    .transfer = mp_machine_i2c_transfer_adaptor
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

#if MICROPY_PY_MACHINE_I2C_SLAVE

static mp_obj_t machine_i2c_slave_deinit(mp_obj_t self_in) {
    mp_obj_base_t *self = MP_OBJ_TO_PTR(self_in);
    machine_i2c_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_i2c_slave_deinit_obj, machine_i2c_slave_deinit);

static mp_obj_t machine_i2c_slave_configure_receive_buffer(mp_obj_t self_in, mp_obj_t buf_in) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    cy_rslt_t result = cyhal_i2c_slave_config_write_buffer(&self->i2c_obj, bufinfo.buf, bufinfo.len);
    i2c_assert_raise_val("I2C Slave configure receive buffer failed with return code %lx !", result);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_i2c_slave_conf_rx_buffer_obj, machine_i2c_slave_configure_receive_buffer);

static mp_obj_t machine_i2c_slave_configure_transmit_buffer(mp_obj_t self_in, mp_obj_t buf_in) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    cy_rslt_t result = cyhal_i2c_slave_config_read_buffer(&self->i2c_obj, bufinfo.buf, bufinfo.len);
    i2c_assert_raise_val("I2C Slave configure receive buffer failed with return code %lx !", result);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_i2c_slave_conf_tx_buffer_obj, machine_i2c_slave_configure_transmit_buffer);

static void i2c_irq_handler(void *callback_arg, cyhal_i2c_event_t event) {
    machine_i2c_obj_t *self = callback_arg;
    mp_sched_schedule(self->callback, mp_obj_new_int(event));
}

static mp_obj_t machine_i2c_slave_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_callback, ARG_events, ARG_priority};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_callback, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_events, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = CYHAL_I2C_EVENT_NONE  }},
        { MP_QSTR_priority,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 3 }},
    };
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->callback = args[ARG_callback].u_obj;

    cyhal_i2c_register_callback(&self->i2c_obj, i2c_irq_handler, self);
    cyhal_i2c_enable_event(&self->i2c_obj, args[ARG_callback].u_int, args[ARG_priority].u_int, true);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_i2c_slave_irq_obj, 1, machine_i2c_slave_irq);

static mp_obj_t machine_i2c_slave_irq_disable(mp_obj_t self_in) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    cyhal_i2c_enable_event(&self->i2c_obj, CYHAL_I2C_EVENT_NONE, 3, false);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_i2c_slave_irq_disable_obj, machine_i2c_slave_irq_disable);

void mod_i2c_deinit() {
    for (uint8_t i = 0; i < MAX_I2C; i++) {
        if (i2c_obj[i] != NULL) {
            machine_i2c_deinit((mp_obj_base_t *)(i2c_obj[i]));
        }
    }
}


static const mp_rom_map_elem_t machine_i2c_slave_locals_dict_table[] = {
    // Functions
    { MP_ROM_QSTR(MP_QSTR_conf_receive_buffer),         MP_ROM_PTR(&machine_i2c_slave_conf_rx_buffer_obj) },
    { MP_ROM_QSTR(MP_QSTR_conf_transmit_buffer),        MP_ROM_PTR(&machine_i2c_slave_conf_tx_buffer_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq),                         MP_ROM_PTR(&machine_i2c_slave_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq_disable),                 MP_ROM_PTR(&machine_i2c_slave_irq_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),                      MP_ROM_PTR(&machine_i2c_slave_deinit_obj) },

    // Const
    { MP_ROM_QSTR(MP_QSTR_RD_EVENT),                     MP_ROM_INT(CYHAL_I2C_SLAVE_READ_EVENT) },
    { MP_ROM_QSTR(MP_QSTR_WR_EVENT),                     MP_ROM_INT(CYHAL_I2C_SLAVE_WRITE_EVENT) },
    { MP_ROM_QSTR(MP_QSTR_RD_BUF_IN_FIFO_EVENT),         MP_ROM_INT(CYHAL_I2C_SLAVE_RD_IN_FIFO_EVENT) },
    { MP_ROM_QSTR(MP_QSTR_RD_BUF_EMPTY_EVENT),           MP_ROM_INT(CYHAL_I2C_SLAVE_RD_BUF_EMPTY_EVENT) },
    { MP_ROM_QSTR(MP_QSTR_RD_CMPLT_EVENT),               MP_ROM_INT(CYHAL_I2C_SLAVE_RD_CMPLT_EVENT) },
    { MP_ROM_QSTR(MP_QSTR_WR_CMPLT_EVENT),               MP_ROM_INT(CYHAL_I2C_SLAVE_WR_CMPLT_EVENT) },
    { MP_ROM_QSTR(MP_QSTR_ERR_EVENT),                    MP_ROM_INT(CYHAL_I2C_SLAVE_ERR_EVENT) },
};
static MP_DEFINE_CONST_DICT(machine_i2c_slave_locals_dict, machine_i2c_slave_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_i2c_slave_type,
    MP_QSTR_I2CSlave,
    MP_TYPE_FLAG_NONE,
    make_new, machine_i2c_slave_make_new,
    print, machine_i2c_print,
    locals_dict, &machine_i2c_slave_locals_dict
    );
#endif
