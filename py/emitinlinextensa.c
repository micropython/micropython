/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2016 Damien P. George
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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "py/emit.h"
#include "py/asmxtensa.h"

#if MICROPY_EMIT_INLINE_XTENSA

#include "py/persistentcode.h"

#if MICROPY_DYNAMIC_COMPILER
// Make sure all instructions are available to `mpy-cross`.
#define XTENSA_MIN_CORE_TARGET 9999
#elif MICROPY_EMIT_INLINE_XTENSA_TARGET > 0
#define XTENSA_MIN_CORE_TARGET (MICROPY_EMIT_INLINE_XTENSA_TARGET)
#else
#define XTENSA_MIN_CORE_TARGET (MPY_XTENSA_LX_CORE)
#endif

static inline bool emit_windowed_code() {
    #if MICROPY_DYNAMIC_COMPILER
    return mp_dynamic_compiler.native_arch == MP_NATIVE_ARCH_XTENSAWIN;
    #elif MICROPY_EMIT_XTENSAWIN
    return true;
    #else
    return false;
    #endif
}

#define LX3 (0)
#define LX4 (1)
#define LX5 (2)
#define LX6 (3)
#define LX7 (4)
#define LX8 (5)

static inline int minimum_allowed_lx_core_version(void) {
    #if MICROPY_DYNAMIC_COMPILER
    if (mp_dynamic_compiler.backend_options != NULL) {
        return ((asm_xtensa_backend_options_t *)mp_dynamic_compiler.backend_options)->core_version;
    } else {
        // Be compatible with Espressif targets at least.
        return mp_dynamic_compiler.native_arch == MP_NATIVE_ARCH_XTENSAWIN ? LX6 : LX3;
    }
    #else
    assert(XTENSA_MIN_CORE_TARGET >= 3 && XTENSA_MIN_CORE_TARGET <= 8 && "Invalid Xtensa LX core version");
    return XTENSA_MIN_CORE_TARGET - 2;
    #endif
}

struct _emit_inline_asm_t {
    asm_xtensa_t as;
    uint16_t pass;
    mp_obj_t *error_slot;
    mp_uint_t max_num_labels;
    qstr *label_lookup;
};

static void emit_inline_xtensa_error_msg(emit_inline_asm_t *emit, mp_rom_error_text_t msg) {
    *emit->error_slot = mp_obj_new_exception_msg(&mp_type_SyntaxError, msg);
}

static void emit_inline_xtensa_error_exc(emit_inline_asm_t *emit, mp_obj_t exc) {
    *emit->error_slot = exc;
}

emit_inline_asm_t *emit_inline_xtensa_new(mp_uint_t max_num_labels) {
    emit_inline_asm_t *emit = m_new_obj(emit_inline_asm_t);
    memset(&emit->as, 0, sizeof(emit->as));
    mp_asm_base_init(&emit->as.base, max_num_labels);
    emit->max_num_labels = max_num_labels;
    emit->label_lookup = m_new(qstr, max_num_labels);
    return emit;
}

void emit_inline_xtensa_free(emit_inline_asm_t *emit) {
    m_del(qstr, emit->label_lookup, emit->max_num_labels);
    mp_asm_base_deinit(&emit->as.base, false);
    m_del_obj(emit_inline_asm_t, emit);
}

static void emit_inline_xtensa_start_pass(emit_inline_asm_t *emit, pass_kind_t pass, mp_obj_t *error_slot) {
    emit->pass = pass;
    emit->error_slot = error_slot;
    if (emit->pass == MP_PASS_CODE_SIZE) {
        memset(emit->label_lookup, 0, emit->max_num_labels * sizeof(qstr));
    }
    mp_asm_base_start_pass(&emit->as.base, pass == MP_PASS_EMIT ? MP_ASM_PASS_EMIT : MP_ASM_PASS_COMPUTE);
    if (emit_windowed_code()) {
        asm_xtensa_entry_win(&emit->as, 0);
    } else {
        asm_xtensa_entry(&emit->as, 0);
    }
}

static void emit_inline_xtensa_end_pass(emit_inline_asm_t *emit, mp_uint_t type_sig) {
    if (emit_windowed_code()) {
        asm_xtensa_exit_win(&emit->as);
    } else {
        asm_xtensa_exit(&emit->as);
    }
    asm_xtensa_end_pass(&emit->as);
}

static mp_uint_t emit_inline_xtensa_count_params(emit_inline_asm_t *emit, mp_uint_t n_params, mp_parse_node_t *pn_params) {
    if (n_params > 4) {
        emit_inline_xtensa_error_msg(emit, MP_ERROR_TEXT("can only have up to 4 parameters to Xtensa assembly"));
        return 0;
    }
    for (mp_uint_t i = 0; i < n_params; i++) {
        if (!MP_PARSE_NODE_IS_ID(pn_params[i])) {
            emit_inline_xtensa_error_msg(emit, MP_ERROR_TEXT("parameters must be registers in sequence a2 to a5"));
            return 0;
        }
        const char *p = qstr_str(MP_PARSE_NODE_LEAF_ARG(pn_params[i]));
        if (!(strlen(p) == 2 && p[0] == 'a' && (mp_uint_t)p[1] == '2' + i)) {
            emit_inline_xtensa_error_msg(emit, MP_ERROR_TEXT("parameters must be registers in sequence a2 to a5"));
            return 0;
        }
    }
    return n_params;
}

static bool emit_inline_xtensa_label(emit_inline_asm_t *emit, mp_uint_t label_num, qstr label_id) {
    assert(label_num < emit->max_num_labels);
    if (emit->pass == MP_PASS_CODE_SIZE) {
        // check for duplicate label on first pass
        for (uint i = 0; i < emit->max_num_labels; i++) {
            if (emit->label_lookup[i] == label_id) {
                return false;
            }
        }
    }
    emit->label_lookup[label_num] = label_id;
    mp_asm_base_label_assign(&emit->as.base, label_num);
    return true;
}

static const qstr_short_t REGISTERS[16] = {
    MP_QSTR_a0, MP_QSTR_a1, MP_QSTR_a2, MP_QSTR_a3, MP_QSTR_a4, MP_QSTR_a5, MP_QSTR_a6, MP_QSTR_a7,
    MP_QSTR_a8, MP_QSTR_a9, MP_QSTR_a10, MP_QSTR_a11, MP_QSTR_a12, MP_QSTR_a13, MP_QSTR_a14, MP_QSTR_a15
};

static mp_uint_t get_arg_reg(emit_inline_asm_t *emit, const char *op, mp_parse_node_t pn) {
    if (MP_PARSE_NODE_IS_ID(pn)) {
        qstr node_qstr = MP_PARSE_NODE_LEAF_ARG(pn);
        for (size_t i = 0; i < MP_ARRAY_SIZE(REGISTERS); i++) {
            if (node_qstr == REGISTERS[i]) {
                return i;
            }
        }
    }

    emit_inline_xtensa_error_exc(emit,
        mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
            MP_ERROR_TEXT("'%s' expects a register"), op));
    return 0;
}

static uint32_t get_arg_i(emit_inline_asm_t *emit, const char *op, mp_parse_node_t pn, int min, int max) {
    mp_obj_t o;
    if (!mp_parse_node_get_int_maybe(pn, &o)) {
        emit_inline_xtensa_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, MP_ERROR_TEXT("'%s' expects an integer"), op));
        return 0;
    }
    uint32_t i = mp_obj_get_int_truncated(o);
    if (min != max && ((int)i < min || (int)i > max)) {
        emit_inline_xtensa_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, MP_ERROR_TEXT("'%s' integer %d isn't within range %d..%d"), op, i, min, max));
        return 0;
    }
    return i;
}

static int get_arg_label(emit_inline_asm_t *emit, const char *op, mp_parse_node_t pn) {
    if (!MP_PARSE_NODE_IS_ID(pn)) {
        emit_inline_xtensa_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, MP_ERROR_TEXT("'%s' expects a label"), op));
        return 0;
    }
    qstr label_qstr = MP_PARSE_NODE_LEAF_ARG(pn);
    for (uint i = 0; i < emit->max_num_labels; i++) {
        if (emit->label_lookup[i] == label_qstr) {
            return i;
        }
    }
    // only need to have the labels on the last pass
    if (emit->pass == MP_PASS_EMIT) {
        emit_inline_xtensa_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, MP_ERROR_TEXT("label '%q' not defined"), label_qstr));
    }
    return 0;
}

static const qstr_short_t BRANCH_OPCODE_NAMES[] = {
    MP_QSTR_bnone, MP_QSTR_beq,  MP_QSTR_blt,   MP_QSTR_bltu, MP_QSTR_ball,
    MP_QSTR_bbc,   MP_QSTR_,     MP_QSTR_,      MP_QSTR_bany, MP_QSTR_bne,
    MP_QSTR_bge,   MP_QSTR_bgeu, MP_QSTR_bnall, MP_QSTR_bbs,
};

#define RRR_R0 (1 << 4)
#define RRR_R1 (2 << 4)
#define RRR_R2 (3 << 4)

#define RRR    (0)
#define RRI8   (1)
#define RRRN   (2)

// Some of the opcode discriminants are not entirely correct, on purpose.
// The Xtensa HAL header files provide a lot of definitions indicating
// available CPU features, going deeper than it's needed for this.
//
// As a compromise, for "small" feature flags that cover features that are
// not found in the ESP8266 core (eg. XCHAL_HAVE_MINMAX, XCHAL_HAVE_LOOPS,
// etc.), it is assumed that any core version more recent than LX3 already has
// all of those small bits in.  This should help in reducing keep complexity
// and footprint of a feature that is not even enabled by default on certain
// ports/boards.

// Windowed opcodes marker
#define _ (0)
#define W (1)

static const struct opcode_entry_t {
    qstr_short_t name;
    uint16_t operands : 2;
    uint16_t op2 : 4;
    uint16_t op1 : 4;
    uint16_t op0 : 4;
    uint16_t windowed : 1;
    // 1 bit available here
    uint32_t r : 6;
    uint32_t s : 6;
    uint32_t t : 6;
    uint32_t shift : 3;
    uint32_t kind : 2;
    uint32_t min_core : 3;
    // 6 bits available here
} OPCODE_TABLE[] = {
    { MP_QSTR_abs_,    2,  6, 0,  0, _, RRR_R0,     1,  RRR_R1, 0, RRR,  LX3 },
    { MP_QSTR_add,     3,  8, 0,  0, _, RRR_R0, RRR_R1, RRR_R2, 0, RRR,  LX3 },
    { MP_QSTR_add_n,   3,  0, 0, 10, _, RRR_R0, RRR_R1, RRR_R2, 0, RRRN, LX3 },
    { MP_QSTR_addi,    3,  0, 0,  2, _,     12, RRR_R1, RRR_R0, 0, RRI8, LX3 },
    { MP_QSTR_addx2,   3,  9, 0,  0, _, RRR_R0, RRR_R1, RRR_R2, 0, RRR,  LX3 },
    { MP_QSTR_addx4,   3, 10, 0,  0, _, RRR_R0, RRR_R1, RRR_R2, 0, RRR,  LX3 },
    { MP_QSTR_addx8,   3, 11, 0,  0, _, RRR_R0, RRR_R1, RRR_R2, 0, RRR,  LX3 },
    { MP_QSTR_and_,    3,  1, 0,  0, _, RRR_R0, RRR_R1, RRR_R2, 0, RRR,  LX3 },
    { MP_QSTR_callx0,  1,  0, 0,  0, _,      0, RRR_R0,     12, 0, RRR,  LX3 },
    #if XTENSA_MIN_CORE_TARGET >= 4
    { MP_QSTR_callx4,  1,  0, 0,  0, W,      0, RRR_R0,     13, 0, RRR,  LX4 },
    { MP_QSTR_callx8,  1,  0, 0,  0, W,      0, RRR_R0,     14, 0, RRR,  LX4 },
    { MP_QSTR_callx12, 1,  0, 0,  0, W,      0, RRR_R0,     15, 0, RRR,  LX4 },
    #endif
    { MP_QSTR_jx,      1,  0, 0,  0, _,      0, RRR_R0,     10, 0, RRR,  LX3 },
    { MP_QSTR_l16si,   3,  0, 0,  2, _,      9, RRR_R1, RRR_R0, 3, RRI8, LX3 },
    { MP_QSTR_l16ui,   3,  0, 0,  2, _,      1, RRR_R1, RRR_R0, 3, RRI8, LX3 },
    { MP_QSTR_l32i,    3,  0, 0,  2, _,      2, RRR_R1, RRR_R0, 5, RRI8, LX3 },
    { MP_QSTR_l8ui,    3,  0, 0,  2, _,      0, RRR_R1, RRR_R0, 1, RRI8, LX3 },
    #if XTENSA_MIN_CORE_TARGET >= 4
    { MP_QSTR_max_,    3,  6, 3,  0, _, RRR_R0, RRR_R1, RRR_R2, 0, RRR,  LX4 },
    { MP_QSTR_maxu,    3,  7, 3,  0, _, RRR_R0, RRR_R1, RRR_R2, 0, RRR,  LX4 },
    { MP_QSTR_min_,    3,  4, 3,  0, _, RRR_R0, RRR_R1, RRR_R2, 0, RRR,  LX4 },
    { MP_QSTR_minu,    3,  6, 3,  0, _, RRR_R0, RRR_R1, RRR_R2, 0, RRR,  LX4 },
    #endif
    { MP_QSTR_mov,     2,  0, 0, 13, _,      0, RRR_R1, RRR_R0, 0, RRRN, LX3 },
    { MP_QSTR_mov_n,   2,  0, 0, 13, _,      0, RRR_R1, RRR_R0, 0, RRRN, LX3 },
    { MP_QSTR_mull,    3,  8, 2,  0, _, RRR_R0, RRR_R1, RRR_R2, 0, RRR,  LX3 },
    { MP_QSTR_neg,     2,  6, 0,  0, _, RRR_R0,     0,  RRR_R1, 0, RRR,  LX3 },
    { MP_QSTR_nop,     0,  0, 0,  0, _,      2,     0,      15, 0, RRR,  LX3 },
    { MP_QSTR_nop_n,   0,  0, 0, 13, _,     15,     0,       3, 0, RRRN, LX3 },
    { MP_QSTR_nsa,     2,  4, 0,  0, _,     14, RRR_R1, RRR_R0, 0, RRR,  LX3 },
    { MP_QSTR_nsau,    2,  4, 0,  0, _,     15, RRR_R1, RRR_R0, 0, RRR,  LX3 },
    { MP_QSTR_or_,     3,  2, 0,  0, _, RRR_R0, RRR_R1, RRR_R2, 0, RRR,  LX3 },
    #if XTENSA_MIN_CORE_TARGET >= 4
    { MP_QSTR_quos,    3, 13, 2,  0, _, RRR_R0, RRR_R1, RRR_R2, 0, RRR,  LX4 },
    { MP_QSTR_quou,    3, 12, 2,  0, _, RRR_R0, RRR_R1, RRR_R2, 0, RRR,  LX4 },
    { MP_QSTR_rems,    3, 15, 2,  0, _, RRR_R0, RRR_R1, RRR_R2, 0, RRR,  LX4 },
    { MP_QSTR_remu,    3, 14, 2,  0, _, RRR_R0, RRR_R1, RRR_R2, 0, RRR,  LX4 },
    #endif
    { MP_QSTR_ret,     0,  0, 0, 13, _,     15,     0,       0, 0, RRRN, LX3 },
    { MP_QSTR_ret_n,   0,  0, 0, 13, _,     15,     0,       0, 0, RRRN, LX3 },
    #if XTENSA_MIN_CORE_TARGET >= 4
    { MP_QSTR_retw,    0,  0, 1, 13, W,     15,     0,       0, 0, RRRN, LX4 },
    { MP_QSTR_retw_n,  0,  0, 1, 13, W,     15,     0,       0, 0, RRRN, LX4 },
    #endif
    { MP_QSTR_s16i,    3,  0, 0,  2, _,      5, RRR_R1, RRR_R0, 3, RRI8, LX3 },
    { MP_QSTR_s32i,    3,  0, 0,  2, _,      6, RRR_R1, RRR_R0, 5, RRI8, LX3 },
    { MP_QSTR_s8i,     3,  0, 0,  2, _,      4, RRR_R1, RRR_R0, 1, RRI8, LX3 },
    #if XTENSA_MIN_CORE_TARGET >= 6
    { MP_QSTR_salt,    3,  7, 2,  0, _, RRR_R0, RRR_R1, RRR_R2, 0, RRR,  LX7 },
    { MP_QSTR_saltu,   3,  6, 2,  0, _, RRR_R0, RRR_R1, RRR_R2, 0, RRR,  LX7 },
    #endif
    { MP_QSTR_sll,     2, 10, 1,  0, _, RRR_R0, RRR_R1,      0, 0, RRR,  LX3 },
    { MP_QSTR_sra,     2, 11, 1,  0, _, RRR_R0,      0, RRR_R1, 0, RRR,  LX3 },
    { MP_QSTR_src,     3,  8, 1,  0, _, RRR_R0, RRR_R1, RRR_R2, 0, RRR,  LX3 },
    { MP_QSTR_srl,     2,  9, 1,  0, _, RRR_R0,      0, RRR_R1, 0, RRR,  LX3 },
    { MP_QSTR_ssa8b,   1,  4, 0,  0, _,      3, RRR_R0,      0, 0, RRR,  LX3 },
    { MP_QSTR_ssa8l,   1,  4, 0,  0, _,      2, RRR_R0,      0, 0, RRR,  LX3 },
    { MP_QSTR_ssl,     1,  4, 0,  0, _,      1, RRR_R0,      0, 0, RRR,  LX3 },
    { MP_QSTR_ssr,     1,  4, 0,  0, _,      0, RRR_R0,      0, 0, RRR,  LX3 },
    { MP_QSTR_sub,     3, 12, 0,  0, _, RRR_R0, RRR_R1, RRR_R2, 0, RRR,  LX3 },
    { MP_QSTR_subx2,   3, 13, 0,  0, _, RRR_R0, RRR_R1, RRR_R2, 0, RRR,  LX3 },
    { MP_QSTR_subx4,   3, 14, 0,  0, _, RRR_R0, RRR_R1, RRR_R2, 0, RRR,  LX3 },
    { MP_QSTR_subx8,   3, 15, 0,  0, _, RRR_R0, RRR_R1, RRR_R2, 0, RRR,  LX3 },
    { MP_QSTR_xor,     3,  3, 0,  0, _, RRR_R0, RRR_R1, RRR_R2, 0, RRR,  LX3 },
    #if MICROPY_EMIT_INLINE_XTENSA_UNCOMMON_OPCODES
    { MP_QSTR_dsync,   0,  0, 0,  0, _,      2,      0,      3, 0, RRR,  LX3 },
    { MP_QSTR_esync,   0,  0, 0,  0, _,      2,      0,      2, 0, RRR,  LX3 },
    { MP_QSTR_extw,    0,  0, 0,  0, _,      2,      0,     13, 0, RRR,  LX3 },
    { MP_QSTR_ill,     0,  0, 0,  0, _,      2,      0,      0, 0, RRR,  LX3 },
    { MP_QSTR_ill_n,   0,  0, 0, 13, _,     15,      0,      6, 0, RRRN, LX3 },
    { MP_QSTR_isync,   0,  0, 0,  0, _,      2,      0,      0, 0, RRR,  LX3 },
    { MP_QSTR_memw,    0,  0, 0,  0, _,      2,      0,     12, 0, RRR,  LX3 },
    #if XTENSA_MIN_CORE_TARGET >= 4
    { MP_QSTR_movsp,   2,  0, 0,  0, W,      1, RRR_R1, RRR_R0, 0, RRR,  LX4 },
    #endif
    { MP_QSTR_rer,     2,  4, 0,  0, _,      6, RRR_R1, RRR_R0, 0, RRR,  LX3 },
    { MP_QSTR_rsync,   0,  0, 0,  0, _,      2,      0,      1, 0, RRR,  LX3 },
    { MP_QSTR_wer,     2,  4, 0,  0, _,      7, RRR_R1, RRR_R0, 0, RRR,  LX3 },
    #endif
};

#if XTENSA_MIN_CORE_TARGET >= 4
static const struct bri8_branch_opcode_t {
    qstr_short_t name;
    uint8_t t : 4;
    // 12 bits available here.
} BRI8_BRANCH_OPCODES[] = {
    { MP_QSTR_beqi,   2, },
    { MP_QSTR_bgei,  14, },
    { MP_QSTR_bgeui, 15, },
    { MP_QSTR_blti,  10, },
    { MP_QSTR_bltui, 11, },
    { MP_QSTR_bnei,   6, },
};
#endif

// The index of the first four qstrs matches the CCZ condition value to be
// embedded into the opcode.
static const qstr_short_t BCCZ_OPCODES[] = {
    MP_QSTR_beqz, MP_QSTR_bnez, MP_QSTR_bltz, MP_QSTR_bgez,
    MP_QSTR_beqz_n, MP_QSTR_bnez_n
};

static void emit_inline_xtensa_op(emit_inline_asm_t *emit, qstr op, mp_uint_t n_args, mp_parse_node_t *pn_args) {
    size_t op_len;
    const char *op_str = (const char *)qstr_data(op, &op_len);

    for (size_t index = 0; index < MP_ARRAY_SIZE(OPCODE_TABLE); index++) {
        const struct opcode_entry_t *entry = &OPCODE_TABLE[index];
        if (entry->name == op) {
            if (n_args != entry->operands || minimum_allowed_lx_core_version() < entry->min_core || (entry->windowed && !emit_windowed_code())) {
                goto unknown_op;
            }

            uint32_t opcode = ((entry->r & 0x0F) << 12) | ((entry->s & 0x0F) << 8) | ((entry->t & 0x0F) << 4) | entry->op0;
            if (entry->kind == RRR) {
                opcode |= (entry->op2 << 20) | (entry->op1 << 16);
            } else if (entry->kind == RRI8) {
                int min = 0;
                int max = 0;
                int shift = entry->shift;
                if (shift > 0) {
                    shift >>= 1;
                    max = 0xFF << shift;
                } else {
                    min = -128;
                    max = 127;
                }
                uint32_t immediate = get_arg_i(emit, op_str, pn_args[2], min, max);
                opcode |= ((immediate >> shift) & 0xFF) << 16;
            }
            if (entry->r >= RRR_R0) {
                opcode |= get_arg_reg(emit, op_str, pn_args[(entry->r >> 4) - 1]) << 12;
            }
            if (entry->s >= RRR_R0) {
                opcode |= get_arg_reg(emit, op_str, pn_args[(entry->s >> 4) - 1]) << 8;
            }
            if (entry->t >= RRR_R0) {
                opcode |= get_arg_reg(emit, op_str, pn_args[(entry->t >> 4) - 1]) << 4;
            }
            if (entry->kind == RRRN) {
                assert((opcode >> 16) == 0 && "Stray bits in narrow opcode");
                asm_xtensa_op16(&emit->as, (uint16_t)(opcode & 0xFFFF));
            } else {
                asm_xtensa_op24(&emit->as, opcode);
            }
            return;
        }
    }

    if (n_args == 1) {
        if (op == MP_QSTR_j) {
            int label = get_arg_label(emit, op_str, pn_args[0]);
            asm_xtensa_j_label(&emit->as, label);
        } else if (op == MP_QSTR_ssai) {
            mp_uint_t sa = get_arg_i(emit, op_str, pn_args[0], 0, 31);
            asm_xtensa_op24(&emit->as, ASM_XTENSA_ENCODE_RRR(0, 0, 4, 4, sa & 0x0F, (sa >> 4) & 0x01));
        } else if (op == MP_QSTR_call0) {
            mp_uint_t label = get_arg_label(emit, op_str, pn_args[0]);
            asm_xtensa_call0(&emit->as, label);
        #if XTENSA_MIN_CORE_TARGET >= 4
        } else if (op == MP_QSTR_call4 && emit_windowed_code()) {
            mp_uint_t label = get_arg_label(emit, op_str, pn_args[0]);
            asm_xtensa_op_call4(&emit->as, label);
        } else if (op == MP_QSTR_call8 && emit_windowed_code()) {
            mp_uint_t label = get_arg_label(emit, op_str, pn_args[0]);
            asm_xtensa_op_call8(&emit->as, label);
        } else if (op == MP_QSTR_call12 && emit_windowed_code()) {
            mp_uint_t label = get_arg_label(emit, op_str, pn_args[0]);
            asm_xtensa_op_call12(&emit->as, label);
        #endif
        #if MICROPY_EMIT_INLINE_XTENSA_UNCOMMON_OPCODES
        } else if (op == MP_QSTR_fsync) {
            mp_uint_t imm3 = get_arg_i(emit, op_str, pn_args[0], 0, 7);
            asm_xtensa_op24(&emit->as, ASM_XTENSA_ENCODE_RRR(0, 0, 0, 2, 8 | imm3, 0));
        #endif
        #if XTENSA_MIN_CORE_TARGET >= 4
        } else if (op == MP_QSTR_rotw && emit_windowed_code()) {
            mp_uint_t imm4 = get_arg_i(emit, op_str, pn_args[0], -8, 7);
            asm_xtensa_op24(&emit->as, ASM_XTENSA_ENCODE_RRR(0, 0, 4, 8, 0, imm4));
        #endif
        } else {
            goto unknown_op;
        }
    } else if (n_args == 2) {
        uint r0 = get_arg_reg(emit, op_str, pn_args[0]);
        for (size_t index = 0; index < MP_ARRAY_SIZE(BCCZ_OPCODES); index++) {
            if (op == BCCZ_OPCODES[index]) {
                mp_uint_t label = get_arg_label(emit, op_str, pn_args[1]);
                asm_xtensa_bccz_reg_label(&emit->as, index & 0x03, r0, label);
                return;
            }
        }
        if (op == MP_QSTR_movi) {
            // for convenience we emit l32r if the integer doesn't fit in movi
            uint32_t imm = get_arg_i(emit, op_str, pn_args[1], 0, 0);
            asm_xtensa_mov_reg_i32(&emit->as, r0, imm);
        } else if (op == MP_QSTR_l32r) {
            mp_uint_t label = get_arg_label(emit, op_str, pn_args[1]);
            asm_xtensa_l32r(&emit->as, r0, label);
        } else if (op == MP_QSTR_movi_n) {
            mp_int_t imm = get_arg_i(emit, op_str, pn_args[1], -32, 95);
            asm_xtensa_op_movi_n(&emit->as, r0, imm);
        } else
        #if MICROPY_EMIT_INLINE_XTENSA_UNCOMMON_OPCODES
        if (op == MP_QSTR_rsr) {
            mp_uint_t sr = get_arg_i(emit, op_str, pn_args[1], 0, 255);
            asm_xtensa_op24(&emit->as, ASM_XTENSA_ENCODE_RSR(0, 3, 0, sr, r0));
        } else if (op == MP_QSTR_rur) {
            mp_uint_t imm8 = get_arg_i(emit, op_str, pn_args[1], 0, 255);
            asm_xtensa_op24(&emit->as, ASM_XTENSA_ENCODE_RRR(0, 3, 14, r0, (imm8 >> 4) & 0x0F, imm8 & 0x0F));
        } else if (op == MP_QSTR_wsr) {
            mp_uint_t sr = get_arg_i(emit, op_str, pn_args[1], 0, 255);
            asm_xtensa_op24(&emit->as, ASM_XTENSA_ENCODE_RSR(0, 3, 1, sr, r0));
        } else if (op == MP_QSTR_wur) {
            mp_uint_t sr = get_arg_i(emit, op_str, pn_args[1], 0, 255);
            asm_xtensa_op24(&emit->as, ASM_XTENSA_ENCODE_RSR(0, 3, 15, sr, r0));
        } else if (op == MP_QSTR_xsr) {
            mp_uint_t sr = get_arg_i(emit, op_str, pn_args[1], 0, 255);
            asm_xtensa_op24(&emit->as, ASM_XTENSA_ENCODE_RSR(0, 1, 6, sr, r0));
        } else
        #endif
        #if XTENSA_MIN_CORE_TARGET >= 4
        if (op == MP_QSTR_loop || op == MP_QSTR_loopnez || op == MP_QSTR_loopgtz) {
            mp_uint_t as = get_arg_reg(emit, op_str, pn_args[0]);
            mp_uint_t label = get_arg_label(emit, op_str, pn_args[1]);
            mp_uint_t condition = 8;
            switch (op) {
                case MP_QSTR_loop:
                    condition |= ASM_XTENSA_LOOP_AL;
                    break;
                case MP_QSTR_loopnez:
                    condition |= ASM_XTENSA_LOOP_NEZ;
                    break;
                case MP_QSTR_loopgtz:
                    condition |= ASM_XTENSA_LOOP_GTZ;
                    break;
                default:
                    assert(!"Should not get here.");
                    break;
            }
            asm_xtensa_bri8(&emit->as, condition, as, 7, label, op);
        } else
        #endif
        {
            goto unknown_op;
        }
    } else if (n_args == 3) {
        for (size_t index = 0; index < MP_ARRAY_SIZE(BRANCH_OPCODE_NAMES); index++) {
            if (BRANCH_OPCODE_NAMES[index] == op) {
                int r0 = get_arg_reg(emit, op_str, pn_args[0]);
                int r1 = get_arg_reg(emit, op_str, pn_args[1]);
                int label = get_arg_label(emit, op_str, pn_args[2]);
                asm_xtensa_bcc_reg_reg_label(&emit->as, index, r0, r1, label);
                return;
            }
        }

        #if XTENSA_MIN_CORE_TARGET >= 4
        if (minimum_allowed_lx_core_version() >= 4) {
            for (size_t index = 0; index < MP_ARRAY_SIZE(BRI8_BRANCH_OPCODES); index++) {
                if (BRI8_BRANCH_OPCODES[index].name == op) {
                    int r0 = get_arg_reg(emit, op_str, pn_args[0]);
                    int imm = get_arg_i(emit, op_str, pn_args[1], -1, 256);
                    int label = get_arg_label(emit, op_str, pn_args[2]);
                    asm_xtensa_reg_imm_compare_branch(&emit->as, r0, imm, label, BRI8_BRANCH_OPCODES[index].t, op);
                    return;
                }
            }
        }
        #endif

        if (op == MP_QSTR_addi_n) {
            mp_uint_t r0 = get_arg_reg(emit, op_str, pn_args[0]);
            mp_uint_t r1 = get_arg_reg(emit, op_str, pn_args[1]);
            mp_int_t imm4 = get_arg_i(emit, op_str, pn_args[2], -1, 15);
            asm_xtensa_op16(&emit->as, ASM_XTENSA_ENCODE_RRRN(11, r0, r1, (imm4 != 0 ? imm4 : -1)));
        } else if (op == MP_QSTR_addmi) {
            mp_uint_t r0 = get_arg_reg(emit, op_str, pn_args[0]);
            mp_uint_t r1 = get_arg_reg(emit, op_str, pn_args[1]);
            mp_int_t imm8 = get_arg_i(emit, op_str, pn_args[2], -128 * 256, 127 * 256);
            if ((imm8 & 0xFF) != 0) {
                emit_inline_xtensa_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, MP_ERROR_TEXT("%d is not a multiple of %d"), imm8, 256));
            } else {
                asm_xtensa_op24(&emit->as, ASM_XTENSA_ENCODE_RRI8(2, 13, r1, r0, imm8 >> 8));
            }
        } else if (op == MP_QSTR_bbci) {
            mp_uint_t r0 = get_arg_reg(emit, op_str, pn_args[0]);
            mp_uint_t bit = get_arg_i(emit, op_str, pn_args[1], 0, 31);
            mp_int_t label = get_arg_label(emit, op_str, pn_args[2]);
            asm_xtensa_bit_branch(&emit->as, r0, bit, label, 6);
        } else if (op == MP_QSTR_bbsi) {
            mp_uint_t r0 = get_arg_reg(emit, op_str, pn_args[0]);
            mp_uint_t bit = get_arg_i(emit, op_str, pn_args[1], 0, 31);
            mp_uint_t label = get_arg_label(emit, op_str, pn_args[2]);
            asm_xtensa_bit_branch(&emit->as, r0, bit, label, 14);
        } else if (op == MP_QSTR_slli) {
            mp_uint_t r0 = get_arg_reg(emit, op_str, pn_args[0]);
            mp_uint_t r1 = get_arg_reg(emit, op_str, pn_args[1]);
            mp_uint_t bits = 32 - get_arg_i(emit, op_str, pn_args[2], 1, 31);
            asm_xtensa_op24(&emit->as, ASM_XTENSA_ENCODE_RRR(0, 1, 0 | ((bits >> 4) & 0x01), r0, r1, bits & 0x0F));
        } else if (op == MP_QSTR_srai) {
            mp_uint_t r0 = get_arg_reg(emit, op_str, pn_args[0]);
            mp_uint_t r1 = get_arg_reg(emit, op_str, pn_args[1]);
            mp_uint_t bits = get_arg_i(emit, op_str, pn_args[2], 0, 31);
            asm_xtensa_op24(&emit->as, ASM_XTENSA_ENCODE_RRR(0, 1, 2 | ((bits >> 4) & 0x01), r0, bits & 0x0F, r1));
        } else if (op == MP_QSTR_srli) {
            mp_uint_t r0 = get_arg_reg(emit, op_str, pn_args[0]);
            mp_uint_t r1 = get_arg_reg(emit, op_str, pn_args[1]);
            mp_uint_t bits = get_arg_i(emit, op_str, pn_args[2], 0, 15);
            asm_xtensa_op24(&emit->as, ASM_XTENSA_ENCODE_RRR(0, 1, 4, r0, bits, r1));
        } else if (op == MP_QSTR_l32i_n) {
            mp_uint_t r0 = get_arg_reg(emit, op_str, pn_args[0]);
            mp_uint_t r1 = get_arg_reg(emit, op_str, pn_args[1]);
            mp_uint_t imm = get_arg_i(emit, op_str, pn_args[2], 0, 60);
            if ((imm & 0x03) != 0) {
                emit_inline_xtensa_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, MP_ERROR_TEXT("%d is not a multiple of %d"), imm, 4));
            } else {
                asm_xtensa_op_l32i_n(&emit->as, r0, r1, imm >> 2);
            }
        } else if (op == MP_QSTR_s32i_n) {
            mp_uint_t r0 = get_arg_reg(emit, op_str, pn_args[0]);
            mp_uint_t r1 = get_arg_reg(emit, op_str, pn_args[1]);
            mp_uint_t imm = get_arg_i(emit, op_str, pn_args[2], 0, 60);
            if ((imm & 0x03) != 0) {
                emit_inline_xtensa_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, MP_ERROR_TEXT("%d is not a multiple of %d"), imm, 4));
            } else {
                asm_xtensa_op_s32i_n(&emit->as, r0, r1, imm >> 2);
            }
        #if XTENSA_MIN_CORE_TARGET >= 4
        } else if (op == MP_QSTR_clamps) {
            mp_uint_t r0 = get_arg_reg(emit, op_str, pn_args[0]);
            mp_uint_t r1 = get_arg_reg(emit, op_str, pn_args[1]);
            mp_uint_t imm = get_arg_i(emit, op_str, pn_args[2], 7, 22);
            asm_xtensa_op24(&emit->as, ASM_XTENSA_ENCODE_RRR(0, 3, 3, r0, r1, imm - 7));
        } else if (op == MP_QSTR_sext) {
            mp_uint_t r0 = get_arg_reg(emit, op_str, pn_args[0]);
            mp_uint_t r1 = get_arg_reg(emit, op_str, pn_args[1]);
            mp_uint_t imm = get_arg_i(emit, op_str, pn_args[2], 7, 22);
            asm_xtensa_op24(&emit->as, ASM_XTENSA_ENCODE_RRR(0, 3, 2, r0, r1, imm - 7));
        #endif
        } else {
            goto unknown_op;
        }
    } else if (n_args == 4) {
        if (op == MP_QSTR_extui) {
            mp_uint_t r = get_arg_reg(emit, op_str, pn_args[0]);
            mp_uint_t t = get_arg_reg(emit, op_str, pn_args[1]);
            mp_uint_t shiftimm = get_arg_i(emit, op_str, pn_args[2], 0, 31);
            mp_uint_t maskimm = get_arg_i(emit, op_str, pn_args[3], 1, 16);
            asm_xtensa_op24(&emit->as, ASM_XTENSA_ENCODE_RRR(0, 4 | (shiftimm & 0x10) >> 4, maskimm - 1, r, shiftimm & 0x0F, t));
        }
    } else {
        goto unknown_op;
    }

    return;

unknown_op:
    emit_inline_xtensa_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, MP_ERROR_TEXT("unsupported Xtensa instruction '%s' with %d arguments"), op_str, n_args));
    return;

    /*
branch_not_in_range:
    emit_inline_xtensa_error_msg(emit, MP_ERROR_TEXT("branch not in range"));
    return;
    */
}

const emit_inline_asm_method_table_t emit_inline_xtensa_method_table = {
    #if MICROPY_DYNAMIC_COMPILER
    emit_inline_xtensa_new,
    emit_inline_xtensa_free,
    #endif

    emit_inline_xtensa_start_pass,
    emit_inline_xtensa_end_pass,
    emit_inline_xtensa_count_params,
    emit_inline_xtensa_label,
    emit_inline_xtensa_op,
};

#endif // MICROPY_EMIT_INLINE_XTENSA
