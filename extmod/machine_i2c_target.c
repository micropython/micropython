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

#include "py/runtime.h"

#if MICROPY_PY_MACHINE_I2C_TARGET

#include "extmod/modmachine.h"
#include "shared/runtime/mpirq.h"

enum {
    I2C_TARGET_IRQ_ADDR_MATCH = 1 << 0,
    I2C_TARGET_IRQ_READ_REQ = 1 << 1,
    I2C_TARGET_IRQ_WRITE_REQ = 1 << 2,
    I2C_TARGET_IRQ_END = 1 << 3,
    I2C_TARGET_IRQ_STOP = 1 << 4, // should be able to support this on all targets (rp2 needs custom code though), but maybe can't support addr_match?

    I2C_TARGET_IRQ_MEM_ADDR_MATCH = 1 << 5, // needed?
    I2C_TARGET_IRQ_READ_START = 1 << 6, // needed?
    I2C_TARGET_IRQ_WRITE_START = 1 << 7, // needed?
    I2C_TARGET_IRQ_READ_END = 1 << 8, // needed?
    I2C_TARGET_IRQ_WRITE_END = 1 << 9, // needed?
};

/*
mp_machine_i2c_target_event_addr_match(*, read_write)
mp_machine_i2c_target_event_read_req(*)
mp_machine_i2c_target_event_write_req(*)
mp_machine_i2c_target_event_stop(*)
*/

enum {
    STATE_IDLE,
    STATE_MEM_ADDR_SELECTED,
    STATE_READING,
    STATE_WRITING,
};

typedef struct _machine_i2c_target_data_t {
    uint8_t state;
    uint32_t mem_addr;
    uint32_t mem_len;
    uint8_t *mem_buf;
} machine_i2c_target_data_t;

typedef struct _machine_i2c_target_irq_obj_t {
    mp_irq_obj_t base;
    uint32_t flags;
    uint32_t trigger;
} machine_i2c_target_irq_obj_t;

// The port must provide implementations of these low-level I2C target functions.

static void mp_machine_i2c_target_deinit_all_port(void);

static void mp_machine_i2c_target_event_callback(machine_i2c_target_irq_obj_t *irq);

// read up to N bytes
// return the number of bytes read
static mp_int_t mp_machine_i2c_target_read_bytes(machine_i2c_target_obj_t *self, size_t len, uint8_t *buf);

// write/buffer N bytes
// return the number of bytes written/buffered
static mp_int_t mp_machine_i2c_target_write_bytes(machine_i2c_target_obj_t *self, size_t len, const uint8_t *buf);

static void mp_machine_i2c_target_irq_config(machine_i2c_target_obj_t *self, unsigned int trigger);

static mp_obj_t mp_machine_i2c_target_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);
static void mp_machine_i2c_target_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
static void mp_machine_i2c_target_deinit(machine_i2c_target_obj_t *self);

static const mp_irq_methods_t machine_i2c_target_irq_methods;

#define I2C_TARGET_NUM_IRQ (4)

// Needed to retain a root pointer to the memory object.
MP_REGISTER_ROOT_POINTER(mp_obj_t i2c_target_mem_obj[I2C_TARGET_NUM_IRQ]);

MP_REGISTER_ROOT_POINTER(void *machine_i2c_target_irq_obj[I2C_TARGET_NUM_IRQ]);

void mp_machine_i2c_target_deinit_all(void) {
    for (size_t i = 0; i < I2C_TARGET_NUM_IRQ; ++i) {
        MP_STATE_PORT(i2c_target_mem_obj[i]) = MP_OBJ_NULL;
        MP_STATE_PORT(machine_i2c_target_irq_obj[i]) = NULL;
    }
    mp_machine_i2c_target_deinit_all_port();
}

static bool event(machine_i2c_target_data_t *data, unsigned int trigger, size_t arg0, size_t arg1) {
    unsigned int id = 0; // TODO
    machine_i2c_target_irq_obj_t *irq = MP_STATE_PORT(machine_i2c_target_irq_obj[id]);
    if (irq != NULL && (trigger & irq->trigger)) {
        irq->flags = trigger & irq->trigger;
        mp_machine_i2c_target_event_callback(irq);
        return true; // irq handled
    }
    return false; // irq not handled
}

static void machine_i2c_target_data_init(machine_i2c_target_data_t *data, mp_obj_t mem_obj, mp_int_t mem_addrsize) {
    data->state = STATE_IDLE;
    data->mem_addr = 0;
    data->mem_len = 0;
    data->mem_buf = NULL;
    if (mem_obj != mp_const_none) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(mem_obj, &bufinfo, MP_BUFFER_RW);
        if (mem_addrsize < 8 || mem_addrsize > 32 || mem_addrsize % 8 != 0) {
            mp_raise_ValueError(MP_ERROR_TEXT("mem_addrsize must be 8, 16, 24 or 32"));
        }
        data->mem_len = bufinfo.len;
        data->mem_buf = bufinfo.buf;
    }
}

static void machine_i2c_target_data_reset_helper(machine_i2c_target_data_t *data) {
    size_t len = 0; // TODO
    if (data->state == STATE_READING) {
        event(data, I2C_TARGET_IRQ_READ_END, data->mem_addr, len);
    } else if (data->state == STATE_WRITING) {
        event(data, I2C_TARGET_IRQ_WRITE_END, data->mem_addr, len);
    }
    data->state = STATE_IDLE;
}

static void machine_i2c_target_data_addr_match(machine_i2c_target_data_t *data, bool read) {
    event(data, I2C_TARGET_IRQ_ADDR_MATCH, read, 0);
    machine_i2c_target_data_reset_helper(data);
}

static void machine_i2c_target_data_read_request(machine_i2c_target_obj_t *self, machine_i2c_target_data_t *data) {
    // Let the user handle the read request.
    bool event_handled = event(data, I2C_TARGET_IRQ_READ_REQ, 0, 0);
    if (data->mem_buf == NULL) {
        if (!event_handled) {
            // No data source, just write out a zero.
            uint8_t val = 0;
            mp_machine_i2c_target_write_bytes(self, 1, &val);
        }
    } else {
        // Have a buffer.
        if (data->state == STATE_MEM_ADDR_SELECTED) {
            data->state = STATE_READING;
            event(data, I2C_TARGET_IRQ_READ_START, data->mem_addr, 0);
        }
        uint8_t val = data->mem_buf[data->mem_addr++];
        if (data->mem_addr >= data->mem_len) {
            data->mem_addr = 0;
        }
        mp_machine_i2c_target_write_bytes(self, 1, &val);
    }
}

static void machine_i2c_target_data_write_request(machine_i2c_target_obj_t *self, machine_i2c_target_data_t *data) {
    // Let the user handle the write request.
    bool event_handled = event(data, I2C_TARGET_IRQ_WRITE_REQ, 0, 0);
    if (data->mem_buf == NULL) {
        if (!event_handled) {
            // No data sink, just read and discard the incoming byte.
            uint8_t buf = 0;
            mp_machine_i2c_target_read_bytes(self, 1, &buf);
        }
    } else {
        // Have a buffer.
        uint8_t val = 0;
        mp_machine_i2c_target_read_bytes(self, 1, &val);
        if (data->state == STATE_IDLE) {
            // TODO allow N bytes for address, with N=0 allowed
            data->state = STATE_MEM_ADDR_SELECTED;
            data->mem_addr = val % data->mem_len;
            event(data, I2C_TARGET_IRQ_MEM_ADDR_MATCH, data->mem_addr, 0);
        } else {
            if (data->state == STATE_MEM_ADDR_SELECTED) {
                data->state = STATE_WRITING;
                event(data, I2C_TARGET_IRQ_WRITE_START, data->mem_addr, 0);
            }
            data->mem_buf[data->mem_addr++] = val;
            if (data->mem_addr >= data->mem_len) {
                data->mem_addr = 0;
            }
        }
    }
}

static inline void machine_i2c_target_data_restart_or_stop(machine_i2c_target_data_t *data) {
    machine_i2c_target_data_reset_helper(data);
    event(data, I2C_TARGET_IRQ_END, 0, 0);
}

static inline void machine_i2c_target_data_stop(machine_i2c_target_data_t *data) {
    machine_i2c_target_data_reset_helper(data);
    event(data, I2C_TARGET_IRQ_END | I2C_TARGET_IRQ_STOP, 0, 0);
}

// The port provides implementations of its bindings in this file.
#include MICROPY_PY_MACHINE_I2C_TARGET_INCLUDEFILE

// I2CTarget.deinit()
static mp_obj_t machine_i2c_target_deinit(mp_obj_t self_in) {
    machine_i2c_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_machine_i2c_target_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_i2c_target_deinit_obj, machine_i2c_target_deinit);

// I2CTarget.readinto(buf)
static mp_obj_t machine_i2c_target_readinto(mp_obj_t self_in, mp_obj_t buf_in) {
    machine_i2c_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);
    return MP_OBJ_NEW_SMALL_INT(mp_machine_i2c_target_read_bytes(self, bufinfo.len, bufinfo.buf));
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_i2c_target_readinto_obj, machine_i2c_target_readinto);

// I2CTarget.write(data)
static mp_obj_t machine_i2c_target_write(mp_obj_t self_in, mp_obj_t data_in) {
    machine_i2c_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data_in, &bufinfo, MP_BUFFER_READ);
    return MP_OBJ_NEW_SMALL_INT(mp_machine_i2c_target_write_bytes(self, bufinfo.len, bufinfo.buf));
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_i2c_target_write_obj, machine_i2c_target_write);

static machine_i2c_target_irq_obj_t *machine_i2c_target_get_irq(machine_i2c_target_obj_t *self) {
    // Get the IRQ object.
    machine_i2c_target_irq_obj_t *irq = MP_STATE_PORT(machine_i2c_target_irq_obj[0]);

    // Allocate the IRQ object if it doesn't already exist.
    if (irq == NULL) {
        irq = m_new_obj(machine_i2c_target_irq_obj_t);
        irq->base.base.type = &mp_irq_type;
        irq->base.methods = (mp_irq_methods_t *)&machine_i2c_target_irq_methods;
        irq->base.parent = MP_OBJ_FROM_PTR(self);
        irq->base.handler = mp_const_none;
        irq->base.ishard = false;
        MP_STATE_PORT(machine_i2c_target_irq_obj[0]) = irq;
    }
    return irq;
}

// I2CTarget.irq(handler=None, trigger=IRQ_FALLING|IRQ_RISING, hard=False)
static mp_obj_t machine_i2c_target_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_handler, ARG_trigger, ARG_hard };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_handler, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_trigger, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_hard, MP_ARG_BOOL, {.u_bool = false} },
    };
    machine_i2c_target_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    machine_i2c_target_irq_obj_t *irq = machine_i2c_target_get_irq(self);

    if (n_args > 1 || kw_args->used != 0) {
        // Update IRQ data.
        mp_obj_t handler = args[ARG_handler].u_obj;
        mp_uint_t trigger = args[ARG_trigger].u_int;
        bool hard = args[ARG_hard].u_bool;

        if (!hard) {
            mp_raise_ValueError(MP_ERROR_TEXT("hard IRQ required"));
        }

        // Disable all IRQs while data is updated.
        mp_machine_i2c_target_irq_config(self, 0);

        // Update IRQ data.
        irq->base.handler = handler;
        irq->base.ishard = hard;
        irq->flags = 0;
        irq->trigger = trigger;

        // Enable IRQ if a handler is given.
        if (handler != mp_const_none && trigger != 0) {
            mp_machine_i2c_target_irq_config(self, trigger);
        }
    }
    return MP_OBJ_FROM_PTR(irq);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_i2c_target_irq_obj, 1, machine_i2c_target_irq);

static const mp_rom_map_elem_t machine_i2c_target_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_i2c_target_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&machine_i2c_target_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&machine_i2c_target_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq), MP_ROM_PTR(&machine_i2c_target_irq_obj) },

    { MP_ROM_QSTR(MP_QSTR_IRQ_ADDR_MATCH), MP_ROM_INT(I2C_TARGET_IRQ_ADDR_MATCH) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_READ_REQ), MP_ROM_INT(I2C_TARGET_IRQ_READ_REQ) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_WRITE_REQ), MP_ROM_INT(I2C_TARGET_IRQ_WRITE_REQ) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_END), MP_ROM_INT(I2C_TARGET_IRQ_END) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_STOP), MP_ROM_INT(I2C_TARGET_IRQ_STOP) },
};
static MP_DEFINE_CONST_DICT(machine_i2c_target_locals_dict, machine_i2c_target_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_i2c_target_type,
    MP_QSTR_I2CTarget,
    MP_TYPE_FLAG_NONE,
    make_new, mp_machine_i2c_target_make_new,
    print, mp_machine_i2c_target_print,
    locals_dict, &machine_i2c_target_locals_dict
    );

static mp_uint_t machine_i2c_target_irq_trigger(mp_obj_t self_in, mp_uint_t new_trigger) {
    machine_i2c_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    machine_i2c_target_irq_obj_t *irq = MP_STATE_PORT(machine_i2c_target_irq_obj[0]);
    mp_machine_i2c_target_irq_config(self, 0);
    irq->flags = 0;
    irq->trigger = new_trigger;
    mp_machine_i2c_target_irq_config(self, new_trigger);
    return 0;
}

static mp_uint_t machine_i2c_target_irq_info(mp_obj_t self_in, mp_uint_t info_type) {
    // machine_i2c_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    machine_i2c_target_irq_obj_t *irq = MP_STATE_PORT(machine_i2c_target_irq_obj[0]);
    if (info_type == MP_IRQ_INFO_FLAGS) {
        return irq->flags;
    } else if (info_type == MP_IRQ_INFO_TRIGGERS) {
        return irq->trigger;
    }
    return 0;
}

static const mp_irq_methods_t machine_i2c_target_irq_methods = {
    .trigger = machine_i2c_target_irq_trigger,
    .info = machine_i2c_target_irq_info,
};

#endif // MICROPY_PY_MACHINE_I2C_TARGET
