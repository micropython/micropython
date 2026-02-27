/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Dryw Wade
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

#ifdef PICO_RP2350

#include "py/runtime.h"
#include "modrp2.h"
#include "machine_pin.h"

#include "hardware/structs/hstx_ctrl.h"
#include "hardware/structs/hstx_fifo.h"

// Default register values (see RP2350 datasheet)
#define DEFAULT_HSTX_CSR ((6 << HSTX_CTRL_CSR_SHIFT_LSB) | \
    (5 << HSTX_CTRL_CSR_N_SHIFTS_LSB) | \
    (1 << HSTX_CTRL_CSR_CLKDIV_LSB))
#define DEFAULT_HSTX_BIT (0)
#define DEFAULT_HSTX_EXPAND_SHIFT ((1 << HSTX_CTRL_EXPAND_SHIFT_RAW_N_SHIFTS_LSB) | \
    (1 << HSTX_CTRL_EXPAND_SHIFT_ENC_N_SHIFTS_LSB))
#define DEFAULT_HSTX_EXPAND_TMDS (0)

typedef struct _rp2_hstx_obj_t {
    mp_obj_base_t base;
} rp2_hstx_obj_t;

typedef struct _rp2_hstx_csr_field_t {
    qstr name;
    uint8_t shift : 5;
    uint8_t length : 3;
} rp2_hstx_ctrl_field_t;

static rp2_hstx_ctrl_field_t rp2_hstx_csr_fields_table[] = {
    { MP_QSTR_enable, HSTX_CTRL_CSR_EN_LSB, 1 },
    { MP_QSTR_expand_enable, HSTX_CTRL_CSR_EXPAND_EN_LSB, 1 },
    { MP_QSTR_coupled_mode, HSTX_CTRL_CSR_COUPLED_MODE_LSB, 1 },
    { MP_QSTR_coupled_select, HSTX_CTRL_CSR_COUPLED_SEL_LSB, 2 },
    { MP_QSTR_shift, HSTX_CTRL_CSR_SHIFT_LSB, 5 },
    { MP_QSTR_n_shifts, HSTX_CTRL_CSR_N_SHIFTS_LSB, 5 },
    { MP_QSTR_clk_phase, HSTX_CTRL_CSR_CLKPHASE_LSB, 4 },
    { MP_QSTR_clk_div, HSTX_CTRL_CSR_CLKDIV_LSB, 4 },
};
static const uint32_t rp2_hstx_csr_field_count = MP_ARRAY_SIZE(rp2_hstx_csr_fields_table);

static rp2_hstx_ctrl_field_t rp2_hstx_bit_fields_table[] = {
    { MP_QSTR_sel_p, HSTX_CTRL_BIT0_SEL_P_LSB, 5 },
    { MP_QSTR_sel_n, HSTX_CTRL_BIT0_SEL_N_LSB, 5 },
    { MP_QSTR_inv, HSTX_CTRL_BIT0_INV_LSB, 1 },
    { MP_QSTR_clk, HSTX_CTRL_BIT0_CLK_LSB, 1 },
};
static const uint32_t rp2_hstx_bit_field_count = MP_ARRAY_SIZE(rp2_hstx_bit_fields_table);

static rp2_hstx_ctrl_field_t rp2_hstx_expand_shift_fields_table[] = {
    { MP_QSTR_raw_shift, HSTX_CTRL_EXPAND_SHIFT_RAW_SHIFT_LSB, 5 },
    { MP_QSTR_raw_n_shifts, HSTX_CTRL_EXPAND_SHIFT_RAW_N_SHIFTS_LSB, 5 },
    { MP_QSTR_enc_shift, HSTX_CTRL_EXPAND_SHIFT_ENC_SHIFT_LSB, 5 },
    { MP_QSTR_enc_n_shifts, HSTX_CTRL_EXPAND_SHIFT_ENC_N_SHIFTS_LSB, 5 },
};
static const uint32_t rp2_hstx_expand_shift_field_count = MP_ARRAY_SIZE(rp2_hstx_expand_shift_fields_table);

static rp2_hstx_ctrl_field_t rp2_hstx_expand_tmds_fields_table[] = {
    { MP_QSTR_l0_rot, HSTX_CTRL_EXPAND_TMDS_L0_ROT_LSB, 5 },
    { MP_QSTR_l0_nbits, HSTX_CTRL_EXPAND_TMDS_L0_NBITS_LSB, 3 },
    { MP_QSTR_l1_rot, HSTX_CTRL_EXPAND_TMDS_L1_ROT_LSB, 5 },
    { MP_QSTR_l1_nbits, HSTX_CTRL_EXPAND_TMDS_L1_NBITS_LSB, 3 },
    { MP_QSTR_l2_rot, HSTX_CTRL_EXPAND_TMDS_L2_ROT_LSB, 5 },
    { MP_QSTR_l2_nbits, HSTX_CTRL_EXPAND_TMDS_L2_NBITS_LSB, 3 },
};
static const uint32_t rp2_hstx_expand_tmds_field_count = MP_ARRAY_SIZE(rp2_hstx_expand_tmds_fields_table);

static mp_obj_t rp2_hstx_pack_ctrl(size_t n_pos_args, mp_map_t *kw_args, mp_uint_t default_value, rp2_hstx_ctrl_field_t *fields_table, uint32_t field_count) {
    // Pack keyword settings into a control register value
    mp_uint_t value = default_value;

    if (n_pos_args > 1) {
        mp_raise_TypeError(MP_ERROR_TEXT("only keyword arguments are allowed"));
    }
    mp_uint_t remaining = kw_args->used;

    mp_map_elem_t *default_entry = mp_map_lookup(kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_default), MP_MAP_LOOKUP);
    if (default_entry) {
        remaining--;
        value = mp_obj_get_int_truncated(default_entry->value);
    }

    for (mp_uint_t i = 0; i < field_count; i++) {
        mp_map_elem_t *field_entry = mp_map_lookup(
            kw_args,
            MP_OBJ_NEW_QSTR(fields_table[i].name),
            MP_MAP_LOOKUP
            );
        if (field_entry) {
            remaining--;
            mp_uint_t field_value = mp_obj_get_int_truncated(field_entry->value);
            mp_uint_t mask = ((1 << fields_table[i].length) - 1);
            mp_uint_t masked_value = field_value & mask;
            if (field_value != masked_value) {
                mp_raise_ValueError(MP_ERROR_TEXT("bad field value"));
            }
            value &= ~(mask << fields_table[i].shift);
            value |= masked_value << fields_table[i].shift;
        }
    }

    if (remaining) {
        mp_raise_TypeError(NULL);
    }

    return mp_obj_new_int_from_uint(value);
}

static mp_obj_t rp2_hstx_unpack_ctrl(mp_obj_t value_obj, rp2_hstx_ctrl_field_t *fields_table, uint32_t field_count) {
    // Return a dict representing the unpacked fields of a control register value
    mp_obj_t result_dict[field_count * 2];

    mp_uint_t value = mp_obj_get_int_truncated(value_obj);

    for (mp_uint_t i = 0; i < field_count; i++) {
        result_dict[i * 2] = MP_OBJ_NEW_QSTR(fields_table[i].name);
        mp_uint_t field_value =
            (value >> fields_table[i].shift) & ((1 << fields_table[i].length) - 1);
        result_dict[i * 2 + 1] = MP_OBJ_NEW_SMALL_INT(field_value);
    }

    return mp_obj_dict_make_new(&mp_type_dict, 0, field_count, result_dict);
}

void rp2_hstx_init(void) {
    // Check whether FIFO contains data
    if (hstx_fifo_hw->stat & HSTX_FIFO_STAT_LEVEL_BITS) {
        // Enable HSTX, wait for FIFO to drain, then disable HSTX
        hstx_ctrl_hw->csr = DEFAULT_HSTX_CSR | HSTX_CTRL_CSR_EN_BITS;
        while (hstx_fifo_hw->stat & HSTX_FIFO_STAT_LEVEL_BITS) {
        }
        hstx_ctrl_hw->csr = DEFAULT_HSTX_CSR;
    }

    // Check if the WOF flag is set
    if (hstx_fifo_hw->stat & HSTX_FIFO_STAT_WOF_BITS) {
        // Clear the WOF flag
        hstx_fifo_hw->stat = HSTX_FIFO_STAT_WOF_BITS;
    }
}

void rp2_hstx_deinit(void) {
    // Reset registers to their default values
    hstx_ctrl_hw->csr = DEFAULT_HSTX_CSR;
    for (int i = 0; i < 8; i++) {
        hstx_ctrl_hw->bit[i] = DEFAULT_HSTX_BIT;
    }
    hstx_ctrl_hw->expand_shift = DEFAULT_HSTX_EXPAND_SHIFT;
    hstx_ctrl_hw->expand_tmds = DEFAULT_HSTX_EXPAND_TMDS;
}

/******************************************************************************/
// HSTX object

static rp2_hstx_obj_t rp2_hstx_obj = {
    .base = { &rp2_hstx_type },
};

static mp_obj_t rp2_hstx_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // Return the singleton HSTX object
    return MP_OBJ_FROM_PTR(&rp2_hstx_obj);
}

// HSTX.csr([value])
static mp_obj_t rp2_hstx_csr(size_t n_args, const mp_obj_t *args) {
    if (n_args > 1) {
        mp_int_t value = mp_obj_get_int(args[1]);
        hstx_ctrl_hw->csr = value;
        return mp_const_none;
    }
    return mp_obj_new_int_from_uint(hstx_ctrl_hw->csr);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rp2_hstx_csr_obj, 1, 2, rp2_hstx_csr);

// HSTX.bit(pin, [value])
static mp_obj_t rp2_hstx_bit(size_t n_args, const mp_obj_t *args) {
    const machine_pin_obj_t *pin = machine_pin_find(args[1]);
    if (pin->id < 12 || pin->id > 19) {
        mp_raise_ValueError(MP_ERROR_TEXT("HSTX only supported on GPIO12-19"));
    }
    if (n_args > 2) {
        mp_int_t value = mp_obj_get_int(args[2]);
        hstx_ctrl_hw->bit[pin->id - 12] = value;
        return mp_const_none;
    }
    return mp_obj_new_int_from_uint(hstx_ctrl_hw->bit[pin->id - 12]);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rp2_hstx_bit_obj, 2, 3, rp2_hstx_bit);

// HSTX.expand_shift([value])
static mp_obj_t rp2_hstx_expand_shift(size_t n_args, const mp_obj_t *args) {
    if (n_args > 1) {
        mp_int_t value = mp_obj_get_int(args[1]);
        hstx_ctrl_hw->expand_shift = value;
        return mp_const_none;
    }
    return mp_obj_new_int_from_uint(hstx_ctrl_hw->expand_shift);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rp2_hstx_expand_shift_obj, 1, 2, rp2_hstx_expand_shift);

// HSTX.expand_tmds([value])
static mp_obj_t rp2_hstx_expand_tmds(size_t n_args, const mp_obj_t *args) {
    if (n_args > 1) {
        mp_int_t value = mp_obj_get_int(args[1]);
        hstx_ctrl_hw->expand_tmds = value;
        return mp_const_none;
    }
    return mp_obj_new_int_from_uint(hstx_ctrl_hw->expand_tmds);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rp2_hstx_expand_tmds_obj, 1, 2, rp2_hstx_expand_tmds);

// HSTX.pack_csr(...)
static mp_obj_t rp2_hstx_pack_csr(size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return rp2_hstx_pack_ctrl(n_pos_args, kw_args, DEFAULT_HSTX_CSR, rp2_hstx_csr_fields_table, rp2_hstx_csr_field_count);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(rp2_hstx_pack_csr_obj, 1, rp2_hstx_pack_csr);

// HSTX.pack_bit(...)
static mp_obj_t rp2_hstx_pack_bit(size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return rp2_hstx_pack_ctrl(n_pos_args, kw_args, DEFAULT_HSTX_BIT, rp2_hstx_bit_fields_table, rp2_hstx_bit_field_count);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(rp2_hstx_pack_bit_obj, 1, rp2_hstx_pack_bit);

// HSTX.pack_expand_shift(...)
static mp_obj_t rp2_hstx_pack_expand_shift(size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return rp2_hstx_pack_ctrl(n_pos_args, kw_args, DEFAULT_HSTX_EXPAND_SHIFT, rp2_hstx_expand_shift_fields_table, rp2_hstx_expand_shift_field_count);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(rp2_hstx_pack_expand_shift_obj, 1, rp2_hstx_pack_expand_shift);

// HSTX.pack_expand_tmds(...)
static mp_obj_t rp2_hstx_pack_expand_tmds(size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return rp2_hstx_pack_ctrl(n_pos_args, kw_args, DEFAULT_HSTX_EXPAND_TMDS, rp2_hstx_expand_tmds_fields_table, rp2_hstx_expand_tmds_field_count);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(rp2_hstx_pack_expand_tmds_obj, 1, rp2_hstx_pack_expand_tmds);

// HSTX.unpack_csr(value)
static mp_obj_t rp2_hstx_unpack_csr(mp_obj_t self_in, mp_obj_t value_obj) {
    return rp2_hstx_unpack_ctrl(value_obj, rp2_hstx_csr_fields_table, rp2_hstx_csr_field_count);
}
static MP_DEFINE_CONST_FUN_OBJ_2(rp2_hstx_unpack_csr_obj, rp2_hstx_unpack_csr);

// HSTX.unpack_bit(value)
static mp_obj_t rp2_hstx_unpack_bit(mp_obj_t self_in, mp_obj_t value_obj) {
    return rp2_hstx_unpack_ctrl(value_obj, rp2_hstx_bit_fields_table, rp2_hstx_bit_field_count);
}
static MP_DEFINE_CONST_FUN_OBJ_2(rp2_hstx_unpack_bit_obj, rp2_hstx_unpack_bit);

// HSTX.unpack_expand_shift(value)
static mp_obj_t rp2_hstx_unpack_expand_shift(mp_obj_t self_in, mp_obj_t value_obj) {
    return rp2_hstx_unpack_ctrl(value_obj, rp2_hstx_expand_shift_fields_table, rp2_hstx_expand_shift_field_count);
}
static MP_DEFINE_CONST_FUN_OBJ_2(rp2_hstx_unpack_expand_shift_obj, rp2_hstx_unpack_expand_shift);

// HSTX.unpack_expand_tmds(value)
static mp_obj_t rp2_hstx_unpack_expand_tmds(mp_obj_t self_in, mp_obj_t value_obj) {
    return rp2_hstx_unpack_ctrl(value_obj, rp2_hstx_expand_tmds_fields_table, rp2_hstx_expand_tmds_field_count);
}
static MP_DEFINE_CONST_FUN_OBJ_2(rp2_hstx_unpack_expand_tmds_obj, rp2_hstx_unpack_expand_tmds);





// HSTX.fifo_level()
static mp_obj_t rp2_hstx_fifo_level(mp_obj_t self_in) {
    return mp_obj_new_int_from_uint((hstx_fifo_hw->stat & HSTX_FIFO_STAT_LEVEL_BITS) >> HSTX_FIFO_STAT_LEVEL_LSB);
}
static MP_DEFINE_CONST_FUN_OBJ_1(rp2_hstx_fifo_level_obj, rp2_hstx_fifo_level);

// HSTX.fifo_wof([value])
static mp_obj_t rp2_hstx_fifo_wof(size_t n_args, const mp_obj_t *args) {
    if (n_args > 1) {
        hstx_fifo_hw->stat = 1 << HSTX_FIFO_STAT_WOF_LSB;
        return mp_const_none;
    }
    return mp_obj_new_bool((hstx_fifo_hw->stat & HSTX_FIFO_STAT_WOF_BITS) >> HSTX_FIFO_STAT_WOF_LSB);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rp2_hstx_fifo_wof_obj, 1, 2, rp2_hstx_fifo_wof);

// HSTX.fifo_put(value)
static mp_obj_t rp2_hstx_fifo_put(mp_obj_t self_in, mp_obj_t value_obj) {
    mp_int_t value = mp_obj_get_int(value_obj);
    hstx_fifo_hw->fifo = value;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(rp2_hstx_fifo_put_obj, rp2_hstx_fifo_put);

// Buffer protocol support for the HSTX FIFO (write only)
static mp_int_t rp2_hstx_get_buffer(mp_obj_t o_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    bufinfo->len = 4;
    bufinfo->typecode = 'I';

    if (flags & MP_BUFFER_WRITE) {
        bufinfo->buf = (void *)&hstx_fifo_hw->fifo;
    } else {
        mp_raise_TypeError(MP_ERROR_TEXT("HSTX is output only"));
    }
    return 0;
}

static const mp_rom_map_elem_t rp2_hstx_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_csr), MP_ROM_PTR(&rp2_hstx_csr_obj) },
    { MP_ROM_QSTR(MP_QSTR_bit), MP_ROM_PTR(&rp2_hstx_bit_obj) },
    { MP_ROM_QSTR(MP_QSTR_expand_shift), MP_ROM_PTR(&rp2_hstx_expand_shift_obj) },
    { MP_ROM_QSTR(MP_QSTR_expand_tmds), MP_ROM_PTR(&rp2_hstx_expand_tmds_obj) },
    { MP_ROM_QSTR(MP_QSTR_pack_csr), MP_ROM_PTR(&rp2_hstx_pack_csr_obj) },
    { MP_ROM_QSTR(MP_QSTR_pack_bit), MP_ROM_PTR(&rp2_hstx_pack_bit_obj) },
    { MP_ROM_QSTR(MP_QSTR_pack_expand_shift), MP_ROM_PTR(&rp2_hstx_pack_expand_shift_obj) },
    { MP_ROM_QSTR(MP_QSTR_pack_expand_tmds), MP_ROM_PTR(&rp2_hstx_pack_expand_tmds_obj) },
    { MP_ROM_QSTR(MP_QSTR_unpack_csr), MP_ROM_PTR(&rp2_hstx_unpack_csr_obj) },
    { MP_ROM_QSTR(MP_QSTR_unpack_bit), MP_ROM_PTR(&rp2_hstx_unpack_bit_obj) },
    { MP_ROM_QSTR(MP_QSTR_unpack_expand_shift), MP_ROM_PTR(&rp2_hstx_unpack_expand_shift_obj) },
    { MP_ROM_QSTR(MP_QSTR_unpack_expand_tmds), MP_ROM_PTR(&rp2_hstx_unpack_expand_tmds_obj) },
    { MP_ROM_QSTR(MP_QSTR_fifo_level), MP_ROM_PTR(&rp2_hstx_fifo_level_obj) },
    { MP_ROM_QSTR(MP_QSTR_fifo_wof), MP_ROM_PTR(&rp2_hstx_fifo_wof_obj) },
    { MP_ROM_QSTR(MP_QSTR_fifo_put), MP_ROM_PTR(&rp2_hstx_fifo_put_obj) },
};
static MP_DEFINE_CONST_DICT(rp2_hstx_locals_dict, rp2_hstx_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    rp2_hstx_type,
    MP_QSTR_HSTX,
    MP_TYPE_FLAG_NONE,
    make_new, rp2_hstx_make_new,
    buffer, rp2_hstx_get_buffer,
    locals_dict, &rp2_hstx_locals_dict
    );

#endif // PICO_RP2350
