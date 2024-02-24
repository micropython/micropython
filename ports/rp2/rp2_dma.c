/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Nicko van Someren
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

#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/objarray.h"
#include "shared/runtime/mpirq.h"
#include "modrp2.h"

#include "hardware/irq.h"
#include "hardware/dma.h"

#define CHANNEL_CLOSED 0xff

typedef struct _rp2_dma_ctrl_obj_t {
    mp_obj_base_t base;
    uint32_t value;
} rp2_dma_config_obj_t;

typedef struct _rp2_dma_obj_t {
    mp_obj_base_t base;
    uint8_t channel;
    uint8_t irq_flag : 1;
    uint8_t irq_trigger : 1;
} rp2_dma_obj_t;

typedef struct _rp2_dma_ctrl_field_t {
    qstr name;
    uint8_t shift : 5;
    uint8_t length : 3;
    uint8_t read_only : 1;
} rp2_dma_ctrl_field_t;

STATIC rp2_dma_ctrl_field_t rp2_dma_ctrl_fields_table[] = {
    { MP_QSTR_enable,        0, 1, 0 },
    { MP_QSTR_high_pri,      1, 1, 0 },
    { MP_QSTR_size,          2, 2, 0 },
    { MP_QSTR_inc_read,      4, 1, 0 },
    { MP_QSTR_inc_write,     5, 1, 0 },
    { MP_QSTR_ring_size,     6, 4, 0 },
    { MP_QSTR_ring_sel,     10, 1, 0 },
    { MP_QSTR_chain_to,     11, 4, 0 },
    { MP_QSTR_treq_sel,     15, 6, 0 },
    { MP_QSTR_irq_quiet,    21, 1, 0 },
    { MP_QSTR_bswap,        22, 1, 0 },
    { MP_QSTR_sniff_en,     23, 1, 0 },
    { MP_QSTR_busy,         24, 1, 1 },
    // bits 25 through 28 are reserved
    { MP_QSTR_write_err,    29, 1, 0 },
    { MP_QSTR_read_err,     30, 1, 0 },
    { MP_QSTR_ahb_err,      31, 1, 1 },
};

STATIC const uint32_t rp2_dma_ctrl_field_count = MP_ARRAY_SIZE(rp2_dma_ctrl_fields_table);

#define REG_TYPE_COUNT       0  // Accept just integers
#define REG_TYPE_CONF        1  // Accept integers or ctrl values
#define REG_TYPE_ADDR_READ   2  // Accept integers, buffers or objects that can be read from
#define REG_TYPE_ADDR_WRITE  3  // Accept integers, buffers or objects that can be written to

STATIC uint32_t rp2_dma_register_value_from_obj(mp_obj_t o, int reg_type) {
    if (reg_type == REG_TYPE_ADDR_READ || reg_type == REG_TYPE_ADDR_WRITE) {
        mp_buffer_info_t buf_info;
        mp_uint_t flags = (reg_type == REG_TYPE_ADDR_READ) ? MP_BUFFER_READ : MP_BUFFER_WRITE;
        if (mp_get_buffer(o, &buf_info, flags)) {
            return (uint32_t)buf_info.buf;
        }
    }

    return mp_obj_get_int_truncated(o);
}

STATIC void rp2_dma_irq_handler(void) {
    // Main IRQ handler
    uint32_t irq_bits = dma_hw->ints0;

    for (int i = 0; i < NUM_DMA_CHANNELS; i++) {
        if (irq_bits & (1u << i)) {
            mp_irq_obj_t *handler = MP_STATE_PORT(rp2_dma_irq_obj[i]);
            if (handler) {
                // An rp2.DMA IRQ handler is registered for this channel, so handle it.
                dma_channel_acknowledge_irq0(i);
                rp2_dma_obj_t *self = (rp2_dma_obj_t *)handler->parent;
                self->irq_flag = 1;
                mp_irq_handler(handler);
            }
        }
    }
}

STATIC mp_uint_t rp2_dma_irq_trigger(mp_obj_t self_in, mp_uint_t new_trigger) {
    rp2_dma_obj_t *self = MP_OBJ_TO_PTR(self_in);
    irq_set_enabled(DMA_IRQ_0, false);
    self->irq_flag = 0;
    dma_channel_set_irq0_enabled(self->channel, (new_trigger != 0));
    irq_set_enabled(DMA_IRQ_0, true);
    return 0;
}

STATIC mp_uint_t rp2_dma_irq_info(mp_obj_t self_in, mp_uint_t info_type) {
    rp2_dma_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (info_type == MP_IRQ_INFO_FLAGS) {
        return self->irq_flag;
    } else if (info_type == MP_IRQ_INFO_TRIGGERS) {
        return (dma_hw->ints0 & (1u << self->channel)) != 0;
    }
    return 0;
}

STATIC const mp_irq_methods_t rp2_dma_irq_methods = {
    .trigger = rp2_dma_irq_trigger,
    .info = rp2_dma_irq_info,
};

STATIC mp_obj_t rp2_dma_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    int dma_channel = dma_claim_unused_channel(false);
    if (dma_channel < 0) {
        mp_raise_OSError(MP_EBUSY);
    }

    rp2_dma_obj_t *self = mp_obj_malloc_with_finaliser(rp2_dma_obj_t, &rp2_dma_type);
    self->channel = dma_channel;

    // Return the DMA object.
    return MP_OBJ_FROM_PTR(self);
}

STATIC void rp2_dma_error_if_closed(rp2_dma_obj_t *self) {
    if (self->channel == CHANNEL_CLOSED) {
        mp_raise_ValueError(MP_ERROR_TEXT("channel closed"));
    }
}

STATIC void rp2_dma_attr(mp_obj_t self_in, qstr attr_in, mp_obj_t *dest) {
    rp2_dma_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (dest[0] == MP_OBJ_NULL) {
        // Load attribute
        dma_channel_hw_t *reg_block = dma_channel_hw_addr(self->channel);
        if (attr_in == MP_QSTR_read) {
            rp2_dma_error_if_closed(self);
            dest[0] = mp_obj_new_int_from_uint((mp_uint_t)reg_block->read_addr);
        } else if (attr_in == MP_QSTR_write) {
            rp2_dma_error_if_closed(self);
            dest[0] = mp_obj_new_int_from_uint((mp_uint_t)reg_block->write_addr);
        } else if (attr_in == MP_QSTR_count) {
            rp2_dma_error_if_closed(self);
            dest[0] = mp_obj_new_int_from_uint((mp_uint_t)reg_block->transfer_count);
        } else if (attr_in == MP_QSTR_ctrl) {
            rp2_dma_error_if_closed(self);
            dest[0] = mp_obj_new_int_from_uint((mp_uint_t)reg_block->al1_ctrl);
        } else if (attr_in == MP_QSTR_channel) {
            dest[0] = mp_obj_new_int_from_uint(self->channel);
        } else if (attr_in == MP_QSTR_registers) {
            mp_obj_array_t *reg_view = m_new_obj(mp_obj_array_t);
            mp_obj_memoryview_init(reg_view, 'I' | MP_OBJ_ARRAY_TYPECODE_FLAG_RW, 0, 16, dma_channel_hw_addr(self->channel));
            dest[0] = reg_view;
        } else {
            // Continue attribute search in locals dict.
            dest[1] = MP_OBJ_SENTINEL;
        }
    } else {
        // Set or delete attribute
        if (dest[1] == MP_OBJ_NULL) {
            // We don't support deleting attributes.
            return;
        }

        rp2_dma_error_if_closed(self);

        if (attr_in == MP_QSTR_read) {
            uint32_t value = rp2_dma_register_value_from_obj(dest[1], REG_TYPE_ADDR_READ);
            dma_channel_set_read_addr(self->channel, (volatile void *)value, false);
            dest[0] = MP_OBJ_NULL; // indicate success
        } else if (attr_in == MP_QSTR_write) {
            uint32_t value = rp2_dma_register_value_from_obj(dest[1], REG_TYPE_ADDR_WRITE);
            dma_channel_set_write_addr(self->channel, (volatile void *)value, false);
            dest[0] = MP_OBJ_NULL; // indicate success
        } else if (attr_in == MP_QSTR_count) {
            uint32_t value = rp2_dma_register_value_from_obj(dest[1], REG_TYPE_COUNT);
            dma_channel_set_trans_count(self->channel, value, false);
            dest[0] = MP_OBJ_NULL; // indicate success
        } else if (attr_in == MP_QSTR_ctrl) {
            uint32_t value = rp2_dma_register_value_from_obj(dest[1], REG_TYPE_CONF);
            dma_channel_set_config(self->channel, (dma_channel_config *)&value, false);
            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC void rp2_dma_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    rp2_dma_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "DMA(%u)", self->channel);
}

// DMA.config(*, read, write, count, ctrl, trigger)
STATIC mp_obj_t rp2_dma_config(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    rp2_dma_obj_t *self = MP_OBJ_TO_PTR(*pos_args);

    rp2_dma_error_if_closed(self);

    enum {
        ARG_read,
        ARG_write,
        ARG_count,
        ARG_ctrl,
        ARG_trigger,
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_read,         MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_write,        MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_count,        MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_ctrl,         MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_trigger,      MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    // Don't include self in arg parsing
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    // We only do anything if there was at least one argument
    if (kw_args->used) {
        bool trigger = args[ARG_trigger].u_bool;
        mp_int_t value_count = trigger ? kw_args->used - 1 : kw_args->used;
        if (trigger && (value_count == 0)) {
            // Only a "true" trigger was passed; just start a transfer
            dma_channel_start(self->channel);
        } else {
            if (args[ARG_read].u_obj != MP_OBJ_NULL) {
                uint32_t value = rp2_dma_register_value_from_obj(args[ARG_read].u_obj, REG_TYPE_ADDR_READ);
                value_count--;
                dma_channel_set_read_addr(self->channel, (volatile void *)value, trigger && (value_count == 0));
            }
            if (args[ARG_write].u_obj != MP_OBJ_NULL) {
                uint32_t value = rp2_dma_register_value_from_obj(args[ARG_write].u_obj, REG_TYPE_ADDR_WRITE);
                value_count--;
                dma_channel_set_write_addr(self->channel, (volatile void *)value, trigger && (value_count == 0));
            }
            if (args[ARG_count].u_obj != MP_OBJ_NULL) {
                uint32_t value = rp2_dma_register_value_from_obj(args[ARG_count].u_obj, REG_TYPE_COUNT);
                value_count--;
                dma_channel_set_trans_count(self->channel, value, trigger && (value_count == 0));
            }
            if (args[ARG_ctrl].u_obj != MP_OBJ_NULL) {
                uint32_t value = rp2_dma_register_value_from_obj(args[ARG_ctrl].u_obj, REG_TYPE_CONF);
                value_count--;
                dma_channel_set_config(self->channel, (dma_channel_config *)&value, trigger && (value_count == 0));
            }
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(rp2_dma_config_obj, 1, rp2_dma_config);

// DMA.active([value])
STATIC mp_obj_t rp2_dma_active(size_t n_args, const mp_obj_t *args) {
    rp2_dma_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    rp2_dma_error_if_closed(self);

    if (n_args > 1) {
        if (mp_obj_is_true(args[1])) {
            dma_channel_start(self->channel);
        } else {
            dma_channel_abort(self->channel);
        }
    }

    uint32_t busy = dma_channel_is_busy(self->channel);
    return mp_obj_new_bool((mp_int_t)busy);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rp2_dma_active_obj, 1, 2, rp2_dma_active);

// Default is quiet, unpaced, read and write incrementing, word transfers, enabled
#define DEFAULT_DMA_CONFIG (1 << 21) | (0x3f << 15) | (1 << 5) | (1 << 4) | (2 << 2) | (1 << 0)

// DMA.pack_ctrl(...)
STATIC mp_obj_t rp2_dma_pack_ctrl(size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // Pack keyword settings into a control register value, using either the default for this
    // DMA channel or the provided defaults
    rp2_dma_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_uint_t value = DEFAULT_DMA_CONFIG | ((self->channel & 0xf) << 11);

    if (n_pos_args > 1) {
        mp_raise_TypeError(MP_ERROR_TEXT("pack_ctrl only takes keyword arguments"));
    }
    mp_uint_t remaining = kw_args->used;

    mp_map_elem_t *default_entry = mp_map_lookup(kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_default), MP_MAP_LOOKUP);
    if (default_entry) {
        remaining--;
        value = mp_obj_get_int_truncated(default_entry->value);
    }

    for (mp_uint_t i = 0; i < rp2_dma_ctrl_field_count; i++) {
        mp_map_elem_t *field_entry = mp_map_lookup(
            kw_args,
            MP_OBJ_NEW_QSTR(rp2_dma_ctrl_fields_table[i].name),
            MP_MAP_LOOKUP
            );
        if (field_entry) {
            remaining--;
            // Silently ignore read-only fields, to allow the passing of a modified unpack_ctrl() results
            if (!rp2_dma_ctrl_fields_table[i].read_only) {
                mp_uint_t field_value = mp_obj_get_int_truncated(field_entry->value);
                mp_uint_t mask = ((1 << rp2_dma_ctrl_fields_table[i].length) - 1);
                mp_uint_t masked_value = field_value & mask;
                if (field_value != masked_value) {
                    mp_raise_ValueError(MP_ERROR_TEXT("bad field value"));
                }
                value &= ~(mask << rp2_dma_ctrl_fields_table[i].shift);
                value |= masked_value << rp2_dma_ctrl_fields_table[i].shift;
            }
        }
    }

    if (remaining) {
        mp_raise_TypeError(NULL);
    }

    return mp_obj_new_int_from_uint(value);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(rp2_dma_pack_ctrl_obj, 1, rp2_dma_pack_ctrl);

// DMA.unpack_ctrl(value)
STATIC mp_obj_t rp2_dma_unpack_ctrl(mp_obj_t value_obj) {
    // Return a dict representing the unpacked fields of a control register value
    mp_obj_t result_dict[rp2_dma_ctrl_field_count * 2];

    mp_uint_t value = mp_obj_get_int_truncated(value_obj);

    for (mp_uint_t i = 0; i < rp2_dma_ctrl_field_count; i++) {
        result_dict[i * 2] = MP_OBJ_NEW_QSTR(rp2_dma_ctrl_fields_table[i].name);
        mp_uint_t field_value =
            (value >> rp2_dma_ctrl_fields_table[i].shift) & ((1 << rp2_dma_ctrl_fields_table[i].length) - 1);
        result_dict[i * 2 + 1] = MP_OBJ_NEW_SMALL_INT(field_value);
    }

    return mp_obj_dict_make_new(&mp_type_dict, 0, rp2_dma_ctrl_field_count, result_dict);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(rp2_dma_unpack_ctrl_fun_obj, rp2_dma_unpack_ctrl);
STATIC MP_DEFINE_CONST_STATICMETHOD_OBJ(rp2_dma_unpack_ctrl_obj, MP_ROM_PTR(&rp2_dma_unpack_ctrl_fun_obj));

STATIC mp_obj_t rp2_dma_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_handler, ARG_hard };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_handler, MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
        { MP_QSTR_hard, MP_ARG_BOOL, {.u_bool = false} },
    };

    // Parse the arguments.
    rp2_dma_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get the IRQ object.
    mp_irq_obj_t *irq = MP_STATE_PORT(rp2_dma_irq_obj[self->channel]);

    // Allocate the IRQ object if it doesn't already exist.
    if (irq == NULL) {
        irq = mp_irq_new(&rp2_dma_irq_methods, MP_OBJ_FROM_PTR(self));
        MP_STATE_PORT(rp2_dma_irq_obj[self->channel]) = irq;

        // Clear any existing IRQs on this DMA channel, they are not for us.
        dma_channel_acknowledge_irq0(self->channel);
    }

    if (n_args > 1 || kw_args->used != 0) {
        // Disable all IRQs while data is updated.
        irq_set_enabled(DMA_IRQ_0, false);

        // Update IRQ data.
        irq->handler = args[ARG_handler].u_obj;
        irq->ishard = args[ARG_hard].u_bool;
        self->irq_flag = 0;

        // Enable IRQ if a handler is given.
        bool enable = (args[ARG_handler].u_obj != mp_const_none);
        dma_channel_set_irq0_enabled(self->channel, enable);

        irq_set_enabled(DMA_IRQ_0, true);
    }

    return MP_OBJ_FROM_PTR(irq);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(rp2_dma_irq_obj, 1, rp2_dma_irq);

// DMA.close()
STATIC mp_obj_t rp2_dma_close(mp_obj_t self_in) {
    rp2_dma_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint8_t channel = self->channel;

    if (channel != CHANNEL_CLOSED) {
        // Clean up interrupt handler to ensure garbage collection
        mp_irq_obj_t *irq = MP_STATE_PORT(rp2_dma_irq_obj[channel]);
        MP_STATE_PORT(rp2_dma_irq_obj[channel]) = MP_OBJ_NULL;
        if (irq) {
            irq->parent = MP_OBJ_NULL;
            irq->handler = MP_OBJ_NULL;
            dma_channel_set_irq0_enabled(channel, false);
        }
        dma_channel_unclaim(channel);
        self->channel = CHANNEL_CLOSED;
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(rp2_dma_close_obj, rp2_dma_close);

STATIC const mp_rom_map_elem_t rp2_dma_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&rp2_dma_config_obj) },
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&rp2_dma_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq), MP_ROM_PTR(&rp2_dma_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&rp2_dma_close_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&rp2_dma_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_pack_ctrl), MP_ROM_PTR(&rp2_dma_pack_ctrl_obj) },
    { MP_ROM_QSTR(MP_QSTR_unpack_ctrl), MP_ROM_PTR(&rp2_dma_unpack_ctrl_obj) },
};
STATIC MP_DEFINE_CONST_DICT(rp2_dma_locals_dict, rp2_dma_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    rp2_dma_type,
    MP_QSTR_DMA,
    MP_TYPE_FLAG_NONE,
    make_new, rp2_dma_make_new,
    print, rp2_dma_print,
    attr, rp2_dma_attr,
    locals_dict, &rp2_dma_locals_dict
    );

void rp2_dma_init(void) {
    // Set up interrupts.
    memset(MP_STATE_PORT(rp2_dma_irq_obj), 0, sizeof(MP_STATE_PORT(rp2_dma_irq_obj)));
    irq_add_shared_handler(DMA_IRQ_0, rp2_dma_irq_handler, PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);
}

void rp2_dma_deinit(void) {
    // Remove our interrupt handler.
    irq_remove_handler(DMA_IRQ_0, rp2_dma_irq_handler);
}

MP_REGISTER_ROOT_POINTER(void *rp2_dma_irq_obj[NUM_DMA_CHANNELS]);
