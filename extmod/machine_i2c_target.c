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
    // Events exposed to Python.
    I2C_TARGET_IRQ_ADDR_MATCH_READ = 1 << 0,
    I2C_TARGET_IRQ_ADDR_MATCH_WRITE = 1 << 1,
    I2C_TARGET_IRQ_READ_REQ = 1 << 2,
    I2C_TARGET_IRQ_WRITE_REQ = 1 << 3,
    I2C_TARGET_IRQ_END_READ = 1 << 4,
    I2C_TARGET_IRQ_END_WRITE = 1 << 5,

    // Internal event, not exposed to Python.
    I2C_TARGET_IRQ_MEM_ADDR_MATCH = 1 << 6,
};

// Define the IRQs that require a hard interrupt.
#define I2C_TARGET_IRQ_ALL_HARD ( \
    I2C_TARGET_IRQ_ADDR_MATCH_READ \
    | I2C_TARGET_IRQ_ADDR_MATCH_WRITE \
    | I2C_TARGET_IRQ_READ_REQ \
    | I2C_TARGET_IRQ_WRITE_REQ \
    )

enum {
    STATE_INACTIVE,
    STATE_IDLE,
    STATE_ADDR_MATCH_READ,
    STATE_ADDR_MATCH_WRITE,
    STATE_MEM_ADDR_SELECT,
    STATE_READING,
    STATE_WRITING,
};

typedef struct _machine_i2c_target_data_t {
    uint8_t state;
    uint8_t mem_addr_count;
    uint8_t mem_addrsize;
    uint32_t mem_addr_last;
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

static void mp_machine_i2c_target_event_callback(machine_i2c_target_irq_obj_t *irq);

// Read up to N bytes, and return the number of bytes read.
static size_t mp_machine_i2c_target_read_bytes(machine_i2c_target_obj_t *self, size_t len, uint8_t *buf);

// Write (or buffer) N bytes, and return the number of bytes written/buffered.
static size_t mp_machine_i2c_target_write_bytes(machine_i2c_target_obj_t *self, size_t len, const uint8_t *buf);

// Configure the given events to trigger an interrupt.
static void mp_machine_i2c_target_irq_config(machine_i2c_target_obj_t *self, unsigned int trigger);

static mp_obj_t mp_machine_i2c_target_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);
static void mp_machine_i2c_target_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
static void mp_machine_i2c_target_deinit(machine_i2c_target_obj_t *self);

static const mp_irq_methods_t machine_i2c_target_irq_methods;

static machine_i2c_target_data_t machine_i2c_target_data[MICROPY_PY_MACHINE_I2C_TARGET_MAX];

// Needed to retain a root pointer to the memory object.
MP_REGISTER_ROOT_POINTER(mp_obj_t machine_i2c_target_mem_obj[MICROPY_PY_MACHINE_I2C_TARGET_MAX]);

// Needed to retain a root pointer to the IRQ object.
MP_REGISTER_ROOT_POINTER(void *machine_i2c_target_irq_obj[MICROPY_PY_MACHINE_I2C_TARGET_MAX]);

static bool handle_event(machine_i2c_target_data_t *data, unsigned int trigger) {
    unsigned int id = data - &machine_i2c_target_data[0];
    if (trigger & I2C_TARGET_IRQ_MEM_ADDR_MATCH) {
        data->mem_addr_last = data->mem_addr;
    }
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
    data->mem_addr_count = 0;
    data->mem_addrsize = 0;
    data->mem_addr_last = 0;
    data->mem_addr = 0;
    data->mem_len = 0;
    data->mem_buf = NULL;
    if (mem_obj != mp_const_none) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(mem_obj, &bufinfo, MP_BUFFER_RW);
        if (mem_addrsize < 0 || mem_addrsize > 32 || mem_addrsize % 8 != 0) {
            mp_raise_ValueError(MP_ERROR_TEXT("mem_addrsize must be 0, 8, 16, 24 or 32"));
        }
        data->mem_addrsize = mem_addrsize / 8;
        data->mem_len = bufinfo.len;
        data->mem_buf = bufinfo.buf;
    }
}

static void machine_i2c_target_data_reset_helper(machine_i2c_target_data_t *data) {
    if (data->state == STATE_READING) {
        handle_event(data, I2C_TARGET_IRQ_END_READ);
    } else if (data->state == STATE_ADDR_MATCH_WRITE || data->state == STATE_WRITING) {
        handle_event(data, I2C_TARGET_IRQ_END_WRITE);
    }
    data->state = STATE_IDLE;
}

static void machine_i2c_target_data_addr_match(machine_i2c_target_data_t *data, bool read) {
    machine_i2c_target_data_reset_helper(data);
    if (read) {
        handle_event(data, I2C_TARGET_IRQ_ADDR_MATCH_READ);
        data->state = STATE_ADDR_MATCH_READ;
    } else {
        handle_event(data, I2C_TARGET_IRQ_ADDR_MATCH_WRITE);
        data->state = STATE_ADDR_MATCH_WRITE;
    }
}

static void machine_i2c_target_data_read_request(machine_i2c_target_obj_t *self, machine_i2c_target_data_t *data) {
    // Let the user handle the read request.
    bool event_handled = handle_event(data, I2C_TARGET_IRQ_READ_REQ);
    if (data->mem_buf == NULL) {
        data->state = STATE_READING;
        if (!event_handled) {
            // No data source, just write out a zero.
            uint8_t val = 0;
            mp_machine_i2c_target_write_bytes(self, 1, &val);
        }
    } else {
        // Have a buffer.
        if (data->state == STATE_MEM_ADDR_SELECT) {
            // Got a short memory address.
            data->mem_addr %= data->mem_len;
            handle_event(data, I2C_TARGET_IRQ_MEM_ADDR_MATCH);
        }
        if (data->state != STATE_READING) {
            data->state = STATE_READING;
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
    bool event_handled = handle_event(data, I2C_TARGET_IRQ_WRITE_REQ);
    if (data->mem_buf == NULL) {
        data->state = STATE_WRITING;
        if (!event_handled) {
            // No data sink, just read and discard the incoming byte.
            uint8_t buf = 0;
            mp_machine_i2c_target_read_bytes(self, 1, &buf);
        }
    } else {
        // Have a buffer.
        uint8_t buf[4] = {0};
        size_t n = mp_machine_i2c_target_read_bytes(self, sizeof(buf), &buf[0]);
        for (size_t i = 0; i < n; ++i) {
            uint8_t val = buf[i];
            if (data->state == STATE_ADDR_MATCH_WRITE) {
                data->state = STATE_MEM_ADDR_SELECT;
                data->mem_addr = 0;
                data->mem_addr_count = data->mem_addrsize;
            }
            if (data->state == STATE_MEM_ADDR_SELECT && data->mem_addr_count > 0) {
                data->mem_addr = data->mem_addr << 8 | val;
                --data->mem_addr_count;
                if (data->mem_addr_count == 0) {
                    data->mem_addr %= data->mem_len;
                    handle_event(data, I2C_TARGET_IRQ_MEM_ADDR_MATCH);
                }
            } else {
                if (data->state == STATE_MEM_ADDR_SELECT) {
                    data->state = STATE_WRITING;
                }
                data->mem_buf[data->mem_addr++] = val;
                if (data->mem_addr >= data->mem_len) {
                    data->mem_addr = 0;
                }
            }
        }
    }
}

static inline void machine_i2c_target_data_restart_or_stop(machine_i2c_target_data_t *data) {
    machine_i2c_target_data_reset_helper(data);
}

static inline void machine_i2c_target_data_stop(machine_i2c_target_data_t *data) {
    machine_i2c_target_data_reset_helper(data);
}

// The port provides implementations of its bindings in this file.
#include MICROPY_PY_MACHINE_I2C_TARGET_INCLUDEFILE

static void machine_i2c_target_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    machine_i2c_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    size_t index = mp_machine_i2c_target_get_index(self);
    machine_i2c_target_data_t *data = &machine_i2c_target_data[index];
    if (dest[0] == MP_OBJ_NULL) {
        // Load attribute.
        if (attr == MP_QSTR_memaddr) {
            dest[0] = mp_obj_new_int(data->mem_addr_last);
        } else {
            // Continue lookup in locals_dict.
            dest[1] = MP_OBJ_SENTINEL;
        }
    }
}

// I2CTarget.deinit()
static mp_obj_t machine_i2c_target_deinit(mp_obj_t self_in) {
    machine_i2c_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    size_t index = mp_machine_i2c_target_get_index(self);
    if (machine_i2c_target_data[index].state != STATE_INACTIVE) {
        machine_i2c_target_data[index].state = STATE_INACTIVE;
        mp_machine_i2c_target_deinit(self);
        MP_STATE_PORT(machine_i2c_target_mem_obj[index]) = MP_OBJ_NULL;
        MP_STATE_PORT(machine_i2c_target_irq_obj[index]) = NULL;
    }
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
    size_t index = mp_machine_i2c_target_get_index(self);
    machine_i2c_target_irq_obj_t *irq = MP_STATE_PORT(machine_i2c_target_irq_obj[index]);

    // Allocate the IRQ object if it doesn't already exist.
    if (irq == NULL) {
        irq = m_new_obj(machine_i2c_target_irq_obj_t);
        irq->base.base.type = &mp_irq_type;
        irq->base.methods = (mp_irq_methods_t *)&machine_i2c_target_irq_methods;
        irq->base.parent = MP_OBJ_FROM_PTR(self);
        irq->base.handler = mp_const_none;
        irq->base.ishard = false;
        MP_STATE_PORT(machine_i2c_target_irq_obj[index]) = irq;
    }
    return irq;
}

// I2CTarget.irq(handler=None, trigger=IRQ_END_READ|IRQ_END_WRITE, hard=False)
static mp_obj_t machine_i2c_target_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_handler, ARG_trigger, ARG_hard };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_handler, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_trigger, MP_ARG_INT, {.u_int = I2C_TARGET_IRQ_END_READ | I2C_TARGET_IRQ_END_WRITE} },
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

        #if MICROPY_PY_MACHINE_I2C_TARGET_HARD_IRQ
        if ((trigger & I2C_TARGET_IRQ_ALL_HARD) && !hard) {
            mp_raise_ValueError(MP_ERROR_TEXT("hard IRQ required"));
        }
        #else
        if (hard) {
            mp_raise_ValueError(MP_ERROR_TEXT("hard IRQ unsupported"));
        }
        #endif

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
    #if MICROPY_PY_MACHINE_I2C_TARGET_FINALISER
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&machine_i2c_target_deinit_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_i2c_target_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&machine_i2c_target_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&machine_i2c_target_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq), MP_ROM_PTR(&machine_i2c_target_irq_obj) },

    #if MICROPY_PY_MACHINE_I2C_TARGET_HARD_IRQ
    { MP_ROM_QSTR(MP_QSTR_IRQ_ADDR_MATCH_READ), MP_ROM_INT(I2C_TARGET_IRQ_ADDR_MATCH_READ) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_ADDR_MATCH_WRITE), MP_ROM_INT(I2C_TARGET_IRQ_ADDR_MATCH_WRITE) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_READ_REQ), MP_ROM_INT(I2C_TARGET_IRQ_READ_REQ) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_WRITE_REQ), MP_ROM_INT(I2C_TARGET_IRQ_WRITE_REQ) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_IRQ_END_READ), MP_ROM_INT(I2C_TARGET_IRQ_END_READ) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_END_WRITE), MP_ROM_INT(I2C_TARGET_IRQ_END_WRITE) },
};
static MP_DEFINE_CONST_DICT(machine_i2c_target_locals_dict, machine_i2c_target_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_i2c_target_type,
    MP_QSTR_I2CTarget,
    MP_TYPE_FLAG_NONE,
    make_new, mp_machine_i2c_target_make_new,
    print, mp_machine_i2c_target_print,
    attr, &machine_i2c_target_attr,
    locals_dict, &machine_i2c_target_locals_dict
    );

static mp_uint_t machine_i2c_target_irq_trigger(mp_obj_t self_in, mp_uint_t new_trigger) {
    machine_i2c_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    size_t index = mp_machine_i2c_target_get_index(self);
    machine_i2c_target_irq_obj_t *irq = MP_STATE_PORT(machine_i2c_target_irq_obj[index]);
    mp_machine_i2c_target_irq_config(self, 0);
    irq->flags = 0;
    irq->trigger = new_trigger;
    mp_machine_i2c_target_irq_config(self, new_trigger);
    return 0;
}

static mp_uint_t machine_i2c_target_irq_info(mp_obj_t self_in, mp_uint_t info_type) {
    machine_i2c_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    size_t index = mp_machine_i2c_target_get_index(self);
    machine_i2c_target_irq_obj_t *irq = MP_STATE_PORT(machine_i2c_target_irq_obj[index]);
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

#if !MICROPY_PY_MACHINE_I2C_TARGET_FINALISER
void mp_machine_i2c_target_deinit_all(void) {
    for (size_t i = 0; i < MICROPY_PY_MACHINE_I2C_TARGET_MAX; ++i) {
        if (machine_i2c_target_data[i].state != STATE_INACTIVE) {
            machine_i2c_target_deinit(MP_OBJ_FROM_PTR(&machine_i2c_target_obj[i]));
        }
    }
}
#endif

#endif // MICROPY_PY_MACHINE_I2C_TARGET
