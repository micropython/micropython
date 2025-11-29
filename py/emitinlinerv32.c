/*
 * This file is part of the MicroPython project, https://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Alessandro Gatti
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

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/emit.h"
#include "py/misc.h"

#if MICROPY_EMIT_INLINE_RV32

#include "py/asmrv32.h"

typedef enum {
// define rules with a compile function
#define DEF_RULE(rule, comp, kind, ...) PN_##rule,
#define DEF_RULE_NC(rule, kind, ...)
    #include "py/grammar.h"
#undef DEF_RULE
#undef DEF_RULE_NC
    PN_const_object, // special node for a constant, generic Python object
// define rules without a compile function
#define DEF_RULE(rule, comp, kind, ...)
#define DEF_RULE_NC(rule, kind, ...) PN_##rule,
    #include "py/grammar.h"
#undef DEF_RULE
#undef DEF_RULE_NC
} pn_kind_t;

struct _emit_inline_asm_t {
    asm_rv32_t as;
    uint16_t pass;
    mp_obj_t *error_slot;
    mp_uint_t max_num_labels;
    qstr *label_lookup;
};

static const qstr_short_t REGISTERS_QSTR_TABLE[] = {
    MP_QSTR_zero, MP_QSTR_ra,  MP_QSTR_sp,  MP_QSTR_gp,  MP_QSTR_tp,  MP_QSTR_t0,  MP_QSTR_t1,  MP_QSTR_t2,
    MP_QSTR_s0,   MP_QSTR_s1,  MP_QSTR_a0,  MP_QSTR_a1,  MP_QSTR_a2,  MP_QSTR_a3,  MP_QSTR_a4,  MP_QSTR_a5,
    MP_QSTR_a6,   MP_QSTR_a7,  MP_QSTR_s2,  MP_QSTR_s3,  MP_QSTR_s4,  MP_QSTR_s5,  MP_QSTR_s6,  MP_QSTR_s7,
    MP_QSTR_s8,   MP_QSTR_s9,  MP_QSTR_s10, MP_QSTR_s11, MP_QSTR_t3,  MP_QSTR_t4,  MP_QSTR_t5,  MP_QSTR_t6,
    MP_QSTR_x0,   MP_QSTR_x1,  MP_QSTR_x2,  MP_QSTR_x3,  MP_QSTR_x4,  MP_QSTR_x5,  MP_QSTR_x6,  MP_QSTR_x7,
    MP_QSTR_x8,   MP_QSTR_x9,  MP_QSTR_x10, MP_QSTR_x11, MP_QSTR_x12, MP_QSTR_x13, MP_QSTR_x14, MP_QSTR_x15,
    MP_QSTR_x16,  MP_QSTR_x17, MP_QSTR_x18, MP_QSTR_x19, MP_QSTR_x20, MP_QSTR_x21, MP_QSTR_x22, MP_QSTR_x23,
    MP_QSTR_x24,  MP_QSTR_x25, MP_QSTR_x26, MP_QSTR_x27, MP_QSTR_x28, MP_QSTR_x29, MP_QSTR_x30, MP_QSTR_x31,
};

////////////////////////////////////////////////////////////////////////////////

static inline void emit_inline_rv32_error_msg(emit_inline_asm_t *emit, mp_rom_error_text_t msg) {
    *emit->error_slot = mp_obj_new_exception_msg(&mp_type_SyntaxError, msg);
}

static inline void emit_inline_rv32_error_exc(emit_inline_asm_t *emit, mp_obj_t exc) {
    *emit->error_slot = exc;
}

emit_inline_asm_t *emit_inline_rv32_new(mp_uint_t max_num_labels) {
    emit_inline_asm_t *emit = m_new_obj(emit_inline_asm_t);
    memset(&emit->as, 0, sizeof(emit->as));
    mp_asm_base_init(&emit->as.base, max_num_labels);
    emit->max_num_labels = max_num_labels;
    emit->label_lookup = m_new(qstr, max_num_labels);
    return emit;
}

void emit_inline_rv32_free(emit_inline_asm_t *emit) {
    m_del(qstr, emit->label_lookup, emit->max_num_labels);
    mp_asm_base_deinit(&emit->as.base, false);
    m_del_obj(emit_inline_asm_t, emit);
}

static void emit_inline_rv32_start_pass(emit_inline_asm_t *emit, pass_kind_t pass, mp_obj_t *error_slot) {
    emit->pass = pass;
    emit->error_slot = error_slot;
    if (emit->pass == MP_PASS_CODE_SIZE) {
        memset(emit->label_lookup, 0, emit->max_num_labels * sizeof(qstr));
    }
    mp_asm_base_start_pass(&emit->as.base, pass == MP_PASS_EMIT ? MP_ASM_PASS_EMIT : MP_ASM_PASS_COMPUTE);
}

static void emit_inline_rv32_end_pass(emit_inline_asm_t *emit, mp_uint_t type_sig) {
    // c.jr ra
    asm_rv32_opcode_cjr(&emit->as, ASM_RV32_REG_RA);
    asm_rv32_end_pass(&emit->as);
}

static bool parse_register_node(mp_parse_node_t node, mp_uint_t *register_number, bool compressed) {
    assert(register_number != NULL && "Register number pointer is NULL.");

    if (!MP_PARSE_NODE_IS_ID(node)) {
        return false;
    }

    qstr node_qstr = MP_PARSE_NODE_LEAF_ARG(node);
    for (mp_uint_t index = 0; index < MP_ARRAY_SIZE(REGISTERS_QSTR_TABLE); index++) {
        if (node_qstr == REGISTERS_QSTR_TABLE[index]) {
            mp_uint_t number = index % RV32_AVAILABLE_REGISTERS_COUNT;
            if (!compressed || (compressed && RV32_IS_IN_C_REGISTER_WINDOW(number))) {
                *register_number = compressed ? RV32_MAP_IN_C_REGISTER_WINDOW(number) : number;
                return true;
            }
            break;
        }
    }

    return false;
}

static mp_uint_t lookup_label(emit_inline_asm_t *emit, mp_parse_node_t node, qstr *qstring) {
    assert(qstring && "qstring pointer is NULL");

    *qstring = MP_PARSE_NODE_LEAF_ARG(node);
    for (mp_uint_t label = 0; label < emit->max_num_labels; label++) {
        if (emit->label_lookup[label] == *qstring) {
            return label;
        }
    }

    return emit->max_num_labels;
}

static inline ptrdiff_t label_code_offset(emit_inline_asm_t *emit, mp_uint_t label_index) {
    return emit->as.base.label_offsets[label_index] - emit->as.base.code_offset;
}

static mp_uint_t emit_inline_rv32_count_params(emit_inline_asm_t *emit, mp_uint_t parameters_count, mp_parse_node_t *parameter_nodes) {
    // TODO: Raise this up to 8?  RV32I has 8 A-registers that are meant to
    //       be used for passing arguments.

    if (parameters_count > 4) {
        emit_inline_rv32_error_msg(emit, MP_ERROR_TEXT("can only have up to 4 parameters for RV32 assembly"));
        return 0;
    }

    mp_uint_t register_index = 0;
    for (mp_uint_t index = 0; index < parameters_count; index++) {
        bool valid_register = parse_register_node(parameter_nodes[index], &register_index, false);
        if (!valid_register || (register_index != (ASM_RV32_REG_A0 + index))) {
            emit_inline_rv32_error_msg(emit, MP_ERROR_TEXT("parameters must be registers in sequence a0 to a3"));
            return 0;
        }
    }

    return parameters_count;
}

static bool emit_inline_rv32_label(emit_inline_asm_t *emit, mp_uint_t label_num, qstr label_id) {
    assert(label_num < emit->max_num_labels);
    if (emit->pass == MP_PASS_CODE_SIZE) {
        for (mp_uint_t index = 0; index < emit->max_num_labels; index++) {
            if (emit->label_lookup[index] == label_id) {
                return false;
            }
        }
    }
    emit->label_lookup[label_num] = label_id;
    mp_asm_base_label_assign(&emit->as.base, label_num);
    return true;
}

typedef enum {
    CALL_RRR, // Opcode Register, Register, Register
    CALL_RR,  // Opcode Register, Register
    CALL_RRI, // Opcode Register, Register, Immediate
    CALL_RRL, // Opcode Register, Register, Label
    CALL_RI,  // Opcode Register, Immediate
    CALL_L,   // Opcode Label
    CALL_R,   // Opcode Register
    CALL_RL,  // Opcode Register, Label
    CALL_N,   // Opcode
    CALL_RII, // Opcode Register, Register, Immediate
    CALL_RIR, // Opcode Register, Immediate(Register)
    CALL_COUNT
} call_convention_t;

#define N 0 // No argument
#define R 1 // Register
#define I 2 // Immediate
#define L 3 // Label
#define C (1 << 2) // Compressed register
#define U (1 << 2) // Unsigned immediate
#define Z (1 << 3) // Non-zero

typedef void (*call_ri_t)(asm_rv32_t *state, mp_uint_t rd, mp_int_t immediate);
typedef void (*call_rri_t)(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs1, mp_int_t immediate);
typedef void (*call_rii_t)(asm_rv32_t *state, mp_uint_t rd, mp_uint_t immediate1, mp_int_t immediate2);
typedef void (*call_rrr_t)(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs1, mp_uint_t rs2);
typedef void (*call_rr_t)(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs);
typedef void (*call_i_t)(asm_rv32_t *state, mp_int_t immediate);
typedef void (*call_r_t)(asm_rv32_t *state, mp_uint_t rd);
typedef void (*call_n_t)(asm_rv32_t *state);

typedef struct _opcode_t {
    qstr_short_t qstring;
    uint16_t argument1_mask : 4;
    uint16_t argument2_mask : 4;
    uint16_t argument3_mask : 4;
    uint16_t parse_nodes : 2;
    // 2 bits available here
    uint32_t calling_convention : 4;
    uint32_t argument1_kind : 4;
    uint32_t argument1_shift : 4;
    uint32_t argument2_kind : 4;
    uint32_t argument2_shift : 4;
    uint32_t argument3_kind : 4;
    uint32_t argument3_shift : 4;
    uint32_t required_extensions : 1;
    // 3 bits available here
    void *emitter;
} opcode_t;

#define opcode_li asm_rv32_emit_optimised_load_immediate

static void opcode_la(asm_rv32_t *state, mp_uint_t rd, mp_int_t displacement) {
    // This cannot be optimised for size, otherwise label addresses would move around.
    mp_uint_t upper = (mp_uint_t)displacement & 0xFFFFF000;
    mp_uint_t lower = (mp_uint_t)displacement & 0x00000FFF;
    if ((lower & 0x800) != 0) {
        upper += 0x1000;
    }
    asm_rv32_opcode_auipc(state, rd, upper);
    asm_rv32_opcode_addi(state, rd, rd, lower);
}

#define RC  (R | C)
#define IU  (I | U)
#define IZ  (I | Z)
#define IUZ (I | U | Z)

#define MASK_NOT_USED 0

enum {
    MASK_FFFFFFFF,
    MASK_00000FFF,
    MASK_FFFFF000,
    MASK_00001FFE,
    MASK_0000001F,
    MASK_FFFFFFFE,
    MASK_0000003F,
    MASK_0000FF00,
    MASK_000003FC,
    MASK_000001FE,
    MASK_00000FFE,
    MASK_FFFFFFFA,
    MASK_0001F800,
    MASK_0000007C,
    MASK_000000FC,
    MASK_001FFFFE,
};

static const uint32_t OPCODE_MASKS[] = {
    [MASK_FFFFFFFF] = 0xFFFFFFFF,
    [MASK_00000FFF] = 0x00000FFF,
    [MASK_FFFFF000] = 0xFFFFF000,
    [MASK_00001FFE] = 0x00001FFE,
    [MASK_0000001F] = 0x0000001F,
    [MASK_FFFFFFFE] = 0xFFFFFFFE,
    [MASK_0000003F] = 0x0000003F,
    [MASK_0000FF00] = 0x0000FF00,
    [MASK_000003FC] = 0x000003FC,
    [MASK_000001FE] = 0x000001FE,
    [MASK_00000FFE] = 0x00000FFE,
    [MASK_FFFFFFFA] = 0xFFFFFFFA,
    [MASK_0001F800] = 0x0001F800,
    [MASK_0000007C] = 0x0000007C,
    [MASK_000000FC] = 0x000000FC,
    [MASK_001FFFFE] = 0x001FFFFE,
};

static const opcode_t OPCODES[] = {
    { MP_QSTR_add,        MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_FFFFFFFF, 3, CALL_RRR, R,  0, R,   0,  R,  0, RV32_EXT_NONE, asm_rv32_opcode_add       },
    { MP_QSTR_addi,       MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_00000FFF, 3, CALL_RRI, R,  0, R,   0,  I,  0, RV32_EXT_NONE, asm_rv32_opcode_addi      },
    { MP_QSTR_and_,       MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_FFFFFFFF, 3, CALL_RRR, R,  0, R,   0,  R,  0, RV32_EXT_NONE, asm_rv32_opcode_and       },
    { MP_QSTR_andi,       MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_00000FFF, 3, CALL_RRI, R,  0, R,   0,  I,  0, RV32_EXT_NONE, asm_rv32_opcode_andi      },
    { MP_QSTR_auipc,      MASK_FFFFFFFF, MASK_FFFFF000, MASK_NOT_USED, 2, CALL_RI,  R,  0, I,   12, N,  0, RV32_EXT_NONE, asm_rv32_opcode_auipc     },
    { MP_QSTR_beq,        MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_00001FFE, 3, CALL_RRL, R,  0, R,   0,  L,  0, RV32_EXT_NONE, asm_rv32_opcode_beq       },
    { MP_QSTR_bge,        MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_00001FFE, 3, CALL_RRL, R,  0, R,   0,  L,  0, RV32_EXT_NONE, asm_rv32_opcode_bge       },
    { MP_QSTR_bgeu,       MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_00001FFE, 3, CALL_RRL, R,  0, R,   0,  L,  0, RV32_EXT_NONE, asm_rv32_opcode_bgeu      },
    { MP_QSTR_blt,        MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_00001FFE, 3, CALL_RRL, R,  0, R,   0,  L,  0, RV32_EXT_NONE, asm_rv32_opcode_blt       },
    { MP_QSTR_bltu,       MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_00001FFE, 3, CALL_RRL, R,  0, R,   0,  L,  0, RV32_EXT_NONE, asm_rv32_opcode_bltu      },
    { MP_QSTR_bne,        MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_00001FFE, 3, CALL_RRL, R,  0, R,   0,  L,  0, RV32_EXT_NONE, asm_rv32_opcode_bne       },
    { MP_QSTR_csrrc,      MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_00000FFF, 3, CALL_RRI, R,  0, R,   0,  IU, 0, RV32_EXT_NONE, asm_rv32_opcode_csrrc     },
    { MP_QSTR_csrrs,      MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_00000FFF, 3, CALL_RRI, R,  0, R,   0,  IU, 0, RV32_EXT_NONE, asm_rv32_opcode_csrrs     },
    { MP_QSTR_csrrw,      MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_00000FFF, 3, CALL_RRI, R,  0, R,   0,  IU, 0, RV32_EXT_NONE, asm_rv32_opcode_csrrw     },
    { MP_QSTR_csrrci,     MASK_FFFFFFFF, MASK_00000FFF, MASK_0000001F, 3, CALL_RII, R,  0, IU,  0,  IU, 0, RV32_EXT_NONE, asm_rv32_opcode_csrrci    },
    { MP_QSTR_csrrsi,     MASK_FFFFFFFF, MASK_00000FFF, MASK_0000001F, 3, CALL_RII, R,  0, IU,  0,  IU, 0, RV32_EXT_NONE, asm_rv32_opcode_csrrsi    },
    { MP_QSTR_csrrwi,     MASK_FFFFFFFF, MASK_00000FFF, MASK_0000001F, 3, CALL_RII, R,  0, IU,  0,  IU, 0, RV32_EXT_NONE, asm_rv32_opcode_csrrwi    },
    { MP_QSTR_c_add,      MASK_FFFFFFFE, MASK_FFFFFFFE, MASK_NOT_USED, 2, CALL_RR,  R,  0, R,   0,  N,  0, RV32_EXT_NONE, asm_rv32_opcode_cadd      },
    { MP_QSTR_c_addi,     MASK_FFFFFFFE, MASK_0000003F, MASK_NOT_USED, 2, CALL_RI,  R,  0, IZ,  0,  N,  0, RV32_EXT_NONE, asm_rv32_opcode_caddi     },
    { MP_QSTR_c_addi4spn, MASK_0000FF00, MASK_000003FC, MASK_NOT_USED, 2, CALL_RI,  R,  0, IUZ, 0,  N,  0, RV32_EXT_NONE, asm_rv32_opcode_caddi4spn },
    { MP_QSTR_c_and,      MASK_0000FF00, MASK_0000FF00, MASK_NOT_USED, 2, CALL_RR,  RC, 0, RC,  0,  N,  0, RV32_EXT_NONE, asm_rv32_opcode_cand      },
    { MP_QSTR_c_andi,     MASK_0000FF00, MASK_0000003F, MASK_NOT_USED, 2, CALL_RI,  RC, 0, I,   0,  N,  0, RV32_EXT_NONE, asm_rv32_opcode_candi     },
    { MP_QSTR_c_beqz,     MASK_0000FF00, MASK_000001FE, MASK_NOT_USED, 2, CALL_RL,  RC, 0, L,   0,  N,  0, RV32_EXT_NONE, asm_rv32_opcode_cbeqz     },
    { MP_QSTR_c_bnez,     MASK_0000FF00, MASK_000001FE, MASK_NOT_USED, 2, CALL_RL,  RC, 0, L,   0,  N,  0, RV32_EXT_NONE, asm_rv32_opcode_cbnez     },
    { MP_QSTR_c_ebreak,   MASK_NOT_USED, MASK_NOT_USED, MASK_NOT_USED, 0, CALL_N,   N,  0, N,   0,  N,  0, RV32_EXT_NONE, asm_rv32_opcode_cebreak   },
    { MP_QSTR_c_j,        MASK_00000FFE, MASK_NOT_USED, MASK_NOT_USED, 1, CALL_L,   L,  0, N,   0,  N,  0, RV32_EXT_NONE, asm_rv32_opcode_cj        },
    { MP_QSTR_c_jal,      MASK_00000FFE, MASK_NOT_USED, MASK_NOT_USED, 1, CALL_L,   L,  0, N,   0,  N,  0, RV32_EXT_NONE, asm_rv32_opcode_cjal      },
    { MP_QSTR_c_jalr,     MASK_FFFFFFFE, MASK_NOT_USED, MASK_NOT_USED, 1, CALL_R,   R,  0, N,   0,  N,  0, RV32_EXT_NONE, asm_rv32_opcode_cjalr     },
    { MP_QSTR_c_jr,       MASK_FFFFFFFE, MASK_NOT_USED, MASK_NOT_USED, 1, CALL_R,   R,  0, N,   0,  N,  0, RV32_EXT_NONE, asm_rv32_opcode_cjr       },
    { MP_QSTR_c_li,       MASK_FFFFFFFE, MASK_0000003F, MASK_NOT_USED, 2, CALL_RI,  R,  0, I,   0,  N,  0, RV32_EXT_NONE, asm_rv32_opcode_cli       },
    { MP_QSTR_c_lui,      MASK_FFFFFFFA, MASK_0001F800, MASK_NOT_USED, 2, CALL_RI,  R,  0, IUZ, 12, N,  0, RV32_EXT_NONE, asm_rv32_opcode_clui      },
    { MP_QSTR_c_lw,       MASK_0000FF00, MASK_0000007C, MASK_0000FF00, 2, CALL_RIR, RC, 0, I,   0,  RC, 0, RV32_EXT_NONE, asm_rv32_opcode_clw       },
    { MP_QSTR_c_lwsp,     MASK_FFFFFFFE, MASK_000000FC, MASK_NOT_USED, 2, CALL_RI,  R,  0, I,   0,  N,  0, RV32_EXT_NONE, asm_rv32_opcode_clwsp     },
    { MP_QSTR_c_mv,       MASK_FFFFFFFE, MASK_FFFFFFFE, MASK_NOT_USED, 2, CALL_RR,  R,  0, R,   0,  N,  0, RV32_EXT_NONE, asm_rv32_opcode_cmv       },
    { MP_QSTR_c_nop,      MASK_NOT_USED, MASK_NOT_USED, MASK_NOT_USED, 0, CALL_N,   N,  0, N,   0,  N,  0, RV32_EXT_NONE, asm_rv32_opcode_cnop      },
    { MP_QSTR_c_or,       MASK_0000FF00, MASK_0000FF00, MASK_NOT_USED, 2, CALL_RR,  RC, 0, RC,  0,  N,  0, RV32_EXT_NONE, asm_rv32_opcode_cor       },
    { MP_QSTR_c_slli,     MASK_FFFFFFFE, MASK_0000001F, MASK_NOT_USED, 2, CALL_RI,  R,  0, IU,  0,  N,  0, RV32_EXT_NONE, asm_rv32_opcode_cslli     },
    { MP_QSTR_c_srai,     MASK_0000FF00, MASK_0000001F, MASK_NOT_USED, 2, CALL_RI,  RC, 0, IU,  0,  N,  0, RV32_EXT_NONE, asm_rv32_opcode_csrai     },
    { MP_QSTR_c_srli,     MASK_0000FF00, MASK_0000001F, MASK_NOT_USED, 2, CALL_RI,  RC, 0, IU,  0,  N,  0, RV32_EXT_NONE, asm_rv32_opcode_csrli     },
    { MP_QSTR_c_sub,      MASK_0000FF00, MASK_0000FF00, MASK_NOT_USED, 2, CALL_RR,  RC, 0, RC,  0,  N,  0, RV32_EXT_NONE, asm_rv32_opcode_csub      },
    { MP_QSTR_c_sw,       MASK_0000FF00, MASK_0000007C, MASK_0000FF00, 2, CALL_RIR, RC, 0, I,   0,  RC, 0, RV32_EXT_NONE, asm_rv32_opcode_csw       },
    { MP_QSTR_c_swsp,     MASK_FFFFFFFF, MASK_000000FC, MASK_NOT_USED, 2, CALL_RI,  R,  0, I,   0,  N,  0, RV32_EXT_NONE, asm_rv32_opcode_cswsp     },
    { MP_QSTR_c_xor,      MASK_0000FF00, MASK_0000FF00, MASK_NOT_USED, 2, CALL_RR,  RC, 0, RC,  0,  N,  0, RV32_EXT_NONE, asm_rv32_opcode_cxor      },
    { MP_QSTR_div,        MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_FFFFFFFF, 3, CALL_RRR, R,  0, R,   0,  R,  0, RV32_EXT_NONE, asm_rv32_opcode_div       },
    { MP_QSTR_divu,       MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_FFFFFFFF, 3, CALL_RRR, R,  0, R,   0,  R,  0, RV32_EXT_NONE, asm_rv32_opcode_divu      },
    { MP_QSTR_ebreak,     MASK_NOT_USED, MASK_NOT_USED, MASK_NOT_USED, 0, CALL_N,   N,  0, N,   0,  N,  0, RV32_EXT_NONE, asm_rv32_opcode_ebreak    },
    { MP_QSTR_ecall,      MASK_NOT_USED, MASK_NOT_USED, MASK_NOT_USED, 0, CALL_N,   N,  0, N,   0,  N,  0, RV32_EXT_NONE, asm_rv32_opcode_ecall     },
    { MP_QSTR_jal,        MASK_FFFFFFFF, MASK_001FFFFE, MASK_NOT_USED, 2, CALL_RL,  R,  0, L,   0,  N,  0, RV32_EXT_NONE, asm_rv32_opcode_jal       },
    { MP_QSTR_jalr,       MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_00000FFF, 3, CALL_RRI, R,  0, R,   0,  I,  0, RV32_EXT_NONE, asm_rv32_opcode_jalr      },
    { MP_QSTR_la,         MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_NOT_USED, 2, CALL_RL,  R,  0, L,   0,  N,  0, RV32_EXT_NONE, opcode_la                 },
    { MP_QSTR_lb,         MASK_FFFFFFFF, MASK_00000FFF, MASK_FFFFFFFF, 2, CALL_RIR, R,  0, I,   0,  R,  0, RV32_EXT_NONE, asm_rv32_opcode_lb        },
    { MP_QSTR_lbu,        MASK_FFFFFFFF, MASK_00000FFF, MASK_FFFFFFFF, 2, CALL_RIR, R,  0, I,   0,  R,  0, RV32_EXT_NONE, asm_rv32_opcode_lbu       },
    { MP_QSTR_lh,         MASK_FFFFFFFF, MASK_00000FFF, MASK_FFFFFFFF, 2, CALL_RIR, R,  0, I,   0,  R,  0, RV32_EXT_NONE, asm_rv32_opcode_lh        },
    { MP_QSTR_lhu,        MASK_FFFFFFFF, MASK_00000FFF, MASK_FFFFFFFF, 2, CALL_RIR, R,  0, I,   0,  R,  0, RV32_EXT_NONE, asm_rv32_opcode_lhu       },
    { MP_QSTR_li,         MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_NOT_USED, 2, CALL_RI,  R,  0, I,   0,  N,  0, RV32_EXT_NONE, opcode_li                 },
    { MP_QSTR_lui,        MASK_FFFFFFFF, MASK_FFFFF000, MASK_NOT_USED, 2, CALL_RI,  R,  0, I,   12, N,  0, RV32_EXT_NONE, asm_rv32_opcode_lui       },
    { MP_QSTR_lw,         MASK_FFFFFFFF, MASK_00000FFF, MASK_FFFFFFFF, 2, CALL_RIR, R,  0, I,   0,  R,  0, RV32_EXT_NONE, asm_rv32_opcode_lw        },
    { MP_QSTR_mv,         MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_NOT_USED, 2, CALL_RR,  R,  0, R,   0,  N,  0, RV32_EXT_NONE, asm_rv32_opcode_cmv       },
    { MP_QSTR_mul,        MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_FFFFFFFF, 3, CALL_RRR, R,  0, R,   0,  R,  0, RV32_EXT_NONE, asm_rv32_opcode_mul       },
    { MP_QSTR_mulh,       MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_FFFFFFFF, 3, CALL_RRR, R,  0, R,   0,  R,  0, RV32_EXT_NONE, asm_rv32_opcode_mulh      },
    { MP_QSTR_mulhsu,     MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_FFFFFFFF, 3, CALL_RRR, R,  0, R,   0,  R,  0, RV32_EXT_NONE, asm_rv32_opcode_mulhsu    },
    { MP_QSTR_mulhu,      MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_FFFFFFFF, 3, CALL_RRR, R,  0, R,   0,  R,  0, RV32_EXT_NONE, asm_rv32_opcode_mulhu     },
    { MP_QSTR_or_,        MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_FFFFFFFF, 3, CALL_RRR, R,  0, R,   0,  R,  0, RV32_EXT_NONE, asm_rv32_opcode_or        },
    { MP_QSTR_ori,        MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_00000FFF, 3, CALL_RRI, R,  0, R,   0,  I,  0, RV32_EXT_NONE, asm_rv32_opcode_ori       },
    { MP_QSTR_rem,        MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_FFFFFFFF, 3, CALL_RRR, R,  0, R,   0,  R,  0, RV32_EXT_NONE, asm_rv32_opcode_rem       },
    { MP_QSTR_remu,       MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_FFFFFFFF, 3, CALL_RRR, R,  0, R,   0,  R,  0, RV32_EXT_NONE, asm_rv32_opcode_remu      },
    { MP_QSTR_sb,         MASK_FFFFFFFF, MASK_00000FFF, MASK_FFFFFFFF, 2, CALL_RIR, R,  0, I,   0,  R,  0, RV32_EXT_NONE, asm_rv32_opcode_sb        },
    { MP_QSTR_sh,         MASK_FFFFFFFF, MASK_00000FFF, MASK_FFFFFFFF, 2, CALL_RIR, R,  0, I,   0,  R,  0, RV32_EXT_NONE, asm_rv32_opcode_sh        },
    { MP_QSTR_sh1add,     MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_FFFFFFFF, 3, CALL_RRR, R,  0, R,   0,  R,  0, RV32_EXT_ZBA,  asm_rv32_opcode_sh1add    },
    { MP_QSTR_sh2add,     MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_FFFFFFFF, 3, CALL_RRR, R,  0, R,   0,  R,  0, RV32_EXT_ZBA,  asm_rv32_opcode_sh2add    },
    { MP_QSTR_sh3add,     MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_FFFFFFFF, 3, CALL_RRR, R,  0, R,   0,  R,  0, RV32_EXT_ZBA,  asm_rv32_opcode_sh3add    },
    { MP_QSTR_sll,        MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_FFFFFFFF, 3, CALL_RRR, R,  0, R,   0,  R,  0, RV32_EXT_NONE, asm_rv32_opcode_sll       },
    { MP_QSTR_slli,       MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_0000001F, 3, CALL_RRI, R,  0, R,   0,  IU, 0, RV32_EXT_NONE, asm_rv32_opcode_slli      },
    { MP_QSTR_slt,        MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_FFFFFFFF, 3, CALL_RRR, R,  0, R,   0,  R,  0, RV32_EXT_NONE, asm_rv32_opcode_slt       },
    { MP_QSTR_slti,       MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_00000FFF, 3, CALL_RRI, R,  0, R,   0,  I,  0, RV32_EXT_NONE, asm_rv32_opcode_slti      },
    { MP_QSTR_sltiu,      MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_00000FFF, 3, CALL_RRI, R,  0, R,   0,  I,  0, RV32_EXT_NONE, asm_rv32_opcode_sltiu     },
    { MP_QSTR_sltu,       MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_FFFFFFFF, 3, CALL_RRR, R,  0, R,   0,  R,  0, RV32_EXT_NONE, asm_rv32_opcode_sltu      },
    { MP_QSTR_sra,        MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_FFFFFFFF, 3, CALL_RRR, R,  0, R,   0,  R,  0, RV32_EXT_NONE, asm_rv32_opcode_sra       },
    { MP_QSTR_srai,       MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_0000001F, 3, CALL_RRI, R,  0, R,   0,  IU, 0, RV32_EXT_NONE, asm_rv32_opcode_srai      },
    { MP_QSTR_srl,        MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_FFFFFFFF, 3, CALL_RRR, R,  0, R,   0,  R,  0, RV32_EXT_NONE, asm_rv32_opcode_srl       },
    { MP_QSTR_srli,       MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_0000001F, 3, CALL_RRI, R,  0, R,   0,  IU, 0, RV32_EXT_NONE, asm_rv32_opcode_srli      },
    { MP_QSTR_sub,        MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_FFFFFFFF, 3, CALL_RRR, R,  0, R,   0,  R,  0, RV32_EXT_NONE, asm_rv32_opcode_sub       },
    { MP_QSTR_sw,         MASK_FFFFFFFF, MASK_00000FFF, MASK_FFFFFFFF, 2, CALL_RIR, R,  0, I,   0,  R,  0, RV32_EXT_NONE, asm_rv32_opcode_sw        },
    { MP_QSTR_xor,        MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_FFFFFFFF, 3, CALL_RRR, R,  0, R,   0,  R,  0, RV32_EXT_NONE, asm_rv32_opcode_xor       },
    { MP_QSTR_xori,       MASK_FFFFFFFF, MASK_FFFFFFFF, MASK_00000FFF, 3, CALL_RRI, R,  0, R,   0,  I,  0, RV32_EXT_NONE, asm_rv32_opcode_xori      },
};

#undef RC
#undef IU
#undef IZ
#undef IUZ

// These two checks assume the bitmasks are contiguous.

static bool is_in_signed_mask(mp_uint_t mask, mp_uint_t value) {
    mp_uint_t leading_zeroes = mp_clz(mask);
    if (leading_zeroes == 0 || leading_zeroes > 32) {
        return true;
    }
    mp_uint_t positive_mask = ~(mask & ~(1U << (31 - leading_zeroes)));
    if ((value & positive_mask) == 0) {
        return true;
    }
    mp_uint_t negative_mask = ~(mask >> 1);
    mp_uint_t trailing_zeroes = mp_ctz(mask);
    if (trailing_zeroes > 0) {
        mp_uint_t trailing_mask = (1U << trailing_zeroes) - 1;
        if ((value & trailing_mask) != 0) {
            return false;
        }
        negative_mask &= ~trailing_mask;
    }
    return (value & negative_mask) == negative_mask;
}

static inline bool is_in_unsigned_mask(mp_uint_t mask, mp_uint_t value) {
    return (value & ~mask) == 0;
}

static bool validate_integer(mp_uint_t value, mp_uint_t mask, mp_uint_t flags) {
    if (flags & U) {
        if (!is_in_unsigned_mask(mask, value)) {
            return false;
        }
    } else {
        if (!is_in_signed_mask(mask, value)) {
            return false;
        }
    }

    if ((flags & Z) && (value == 0)) {
        return false;
    }

    return true;
}

#define ET_WRONG_ARGUMENT_KIND   MP_ERROR_TEXT("opcode '%q' argument %d: expecting %q")
#define ET_WRONG_ARGUMENTS_COUNT MP_ERROR_TEXT("opcode '%q': expecting %d arguments")
#define ET_OUT_OF_RANGE          MP_ERROR_TEXT("opcode '%q' argument %d: out of range")

static bool serialise_argument(emit_inline_asm_t *emit, const opcode_t *opcode, mp_parse_node_t node, mp_uint_t node_index, mp_uint_t *serialised) {
    assert((node_index < 3) && "Invalid argument node number.");
    assert(serialised && "Serialised value pointer is NULL.");

    uint32_t kind = 0;
    uint32_t shift = 0;
    uint32_t mask = 0;

    switch (node_index) {
        case 0:
            kind = opcode->argument1_kind;
            shift = opcode->argument1_shift;
            mask = OPCODE_MASKS[opcode->argument1_mask];
            break;

        case 1:
            kind = opcode->argument2_kind;
            shift = opcode->argument2_shift;
            mask = OPCODE_MASKS[opcode->argument2_mask];
            break;

        case 2:
            kind = opcode->argument3_kind;
            shift = opcode->argument3_shift;
            mask = OPCODE_MASKS[opcode->argument3_mask];
            break;

        default:
            break;
    }

    mp_uint_t serialised_value = 0;

    switch (kind & 0x03) {
        case N:
            assert(mask == OPCODE_MASKS[MASK_NOT_USED] && "Invalid mask index for missing operand.");
            break;

        case R: {
            mp_uint_t register_index;
            if (!parse_register_node(node, &register_index, false)) {
                emit_inline_rv32_error_exc(emit,
                    mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
                        ET_WRONG_ARGUMENT_KIND, opcode->qstring, node_index + 1, MP_QSTR_register));
                return false;
            }

            if ((mask & (1U << register_index)) == 0) {
                emit_inline_rv32_error_exc(emit,
                    mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
                        MP_ERROR_TEXT("opcode '%q' argument %d: unknown register"),
                        opcode->qstring, node_index + 1));
                return false;
            }

            serialised_value = (kind & C) ? RV32_MAP_IN_C_REGISTER_WINDOW(register_index) : register_index;
        }
        break;

        case I: {
            mp_obj_t object;
            if (!mp_parse_node_get_int_maybe(node, &object)) {
                emit_inline_rv32_error_exc(emit,
                    mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
                        ET_WRONG_ARGUMENT_KIND, opcode->qstring, node_index + 1, MP_QSTR_integer));
                return false;
            }

            mp_uint_t immediate = mp_obj_get_int_truncated(object) << shift;
            if (kind & U) {
                if (!is_in_unsigned_mask(mask, immediate)) {
                    goto out_of_range;
                }
            } else {
                if (!is_in_signed_mask(mask, immediate)) {
                    goto out_of_range;
                }
            }

            if ((kind & Z) && (immediate == 0)) {
                goto zero_immediate;
            }

            serialised_value = immediate;
        }
        break;

        case L: {
            if (!MP_PARSE_NODE_IS_ID(node)) {
                emit_inline_rv32_error_exc(emit,
                    mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
                        ET_WRONG_ARGUMENT_KIND, opcode->qstring, node_index + 1, MP_QSTR_label));
                return false;
            }

            qstr qstring;
            mp_uint_t label_index = lookup_label(emit, node, &qstring);
            if (label_index >= emit->max_num_labels && emit->pass == MP_PASS_EMIT) {
                emit_inline_rv32_error_exc(emit,
                    mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
                        MP_ERROR_TEXT("opcode '%q' argument %d: undefined label '%q'"),
                        opcode->qstring, node_index + 1, qstring));
                return false;
            }

            mp_uint_t displacement = (mp_uint_t)(label_code_offset(emit, label_index));
            if (kind & U) {
                if (!is_in_unsigned_mask(mask, displacement)) {
                    goto out_of_range;
                }
            } else {
                if (!is_in_signed_mask(mask, displacement)) {
                    goto out_of_range;
                }
            }

            serialised_value = displacement;
        }
        break;

        default:
            assert(!"Unknown argument kind");
            MP_UNREACHABLE;
            break;
    }

    *serialised = serialised_value;
    return true;

out_of_range:
    emit_inline_rv32_error_exc(emit,
        mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, ET_OUT_OF_RANGE, opcode->qstring, node_index + 1));
    return false;

zero_immediate:
    emit_inline_rv32_error_exc(emit,
        mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
            MP_ERROR_TEXT("opcode '%q' argument %d: must not be zero"),
            opcode->qstring, node_index + 1));
    return false;
}

static bool serialise_register_offset_node(emit_inline_asm_t *emit, const opcode_t *opcode_data, mp_parse_node_t node, mp_uint_t node_index, mp_uint_t *offset, mp_uint_t *base) {
    assert(offset && "Attempting to store the offset value into NULL.");
    assert(base && "Attempting to store the base register into NULL.");

    if (!MP_PARSE_NODE_IS_STRUCT_KIND(node, PN_atom_expr_normal) && !MP_PARSE_NODE_IS_STRUCT_KIND(node, PN_factor_2)) {
        goto invalid_structure;
    }
    mp_parse_node_struct_t *node_struct = (mp_parse_node_struct_t *)node;
    bool negative = false;
    if (MP_PARSE_NODE_IS_STRUCT_KIND(node, PN_factor_2)) {
        if (MP_PARSE_NODE_IS_TOKEN_KIND(node_struct->nodes[0], MP_TOKEN_OP_MINUS)) {
            negative = true;
        } else {
            if (!MP_PARSE_NODE_IS_TOKEN_KIND(node_struct->nodes[0], MP_TOKEN_OP_PLUS)) {
                goto invalid_structure;
            }
        }
        if (!MP_PARSE_NODE_IS_STRUCT_KIND(node_struct->nodes[1], PN_atom_expr_normal)) {
            goto invalid_structure;
        }
        node_struct = (mp_parse_node_struct_t *)node_struct->nodes[1];
    }

    if (negative) {
        // If the value is negative, RULE_atom_expr_normal's first token will be the
        // offset stripped of its negative marker; range check will then fail if the
        // default method is used, so a custom check is used instead.
        mp_obj_t object;
        if (!mp_parse_node_get_int_maybe(node_struct->nodes[0], &object)) {
            emit_inline_rv32_error_exc(emit,
                mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, ET_WRONG_ARGUMENT_KIND, opcode_data->qstring, 2, MP_QSTR_integer));
            return false;
        }
        mp_uint_t value = mp_obj_get_int_truncated(object);
        value = (~value + 1) & (mp_uint_t)-1;
        if (!validate_integer(value << opcode_data->argument2_shift, OPCODE_MASKS[opcode_data->argument2_mask], opcode_data->argument2_kind)) {
            emit_inline_rv32_error_exc(emit,
                mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, ET_OUT_OF_RANGE, opcode_data->qstring, 2));
            return false;
        }
        *offset = value;
    } else {
        if (!serialise_argument(emit, opcode_data, node_struct->nodes[0], 1, offset)) {
            return false;
        }
    }

    node_struct = (mp_parse_node_struct_t *)node_struct->nodes[1];
    if (!serialise_argument(emit, opcode_data, node_struct->nodes[0], 2, base)) {
        return false;
    }
    return true;

invalid_structure:
    emit_inline_rv32_error_exc(emit,
        mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
            ET_WRONG_ARGUMENT_KIND, opcode_data->qstring, node_index + 1, MP_QSTR_offset));
    return false;
}

static void handle_opcode(emit_inline_asm_t *emit, const opcode_t *opcode_data, mp_uint_t *arguments) {
    switch (opcode_data->calling_convention) {
        case CALL_RRR:
            ((call_rrr_t)opcode_data->emitter)(&emit->as, arguments[0], arguments[1], arguments[2]);
            break;

        case CALL_RR:
            ((call_rr_t)opcode_data->emitter)(&emit->as, arguments[0], arguments[1]);
            break;

        case CALL_RRI:
            ((call_rri_t)opcode_data->emitter)(&emit->as, arguments[0], arguments[1], arguments[2]);
            break;

        case CALL_RI:
            ((call_ri_t)opcode_data->emitter)(&emit->as, arguments[0], arguments[1]);
            break;

        case CALL_R:
            ((call_r_t)opcode_data->emitter)(&emit->as, arguments[0]);
            break;

        case CALL_RRL:
            ((call_rri_t)opcode_data->emitter)(&emit->as, arguments[0], arguments[1], (ptrdiff_t)arguments[2]);
            break;

        case CALL_RL:
            ((call_ri_t)opcode_data->emitter)(&emit->as, arguments[0], (ptrdiff_t)arguments[1]);
            break;

        case CALL_L:
            ((call_i_t)opcode_data->emitter)(&emit->as, (ptrdiff_t)arguments[0]);
            break;

        case CALL_N:
            ((call_n_t)opcode_data->emitter)(&emit->as);
            break;

        case CALL_RII:
            ((call_rii_t)opcode_data->emitter)(&emit->as, arguments[0], arguments[1], arguments[2]);
            break;

        case CALL_RIR:
            // The last two arguments indices are swapped on purpose.
            ((call_rri_t)opcode_data->emitter)(&emit->as, arguments[0], arguments[2], arguments[1]);
            break;

        default:
            assert(!"Unhandled call convention.");
            MP_UNREACHABLE;
            break;
    }
}

static void emit_inline_rv32_opcode(emit_inline_asm_t *emit, qstr opcode, mp_uint_t arguments_count, mp_parse_node_t *argument_nodes) {
    const opcode_t *opcode_data = NULL;
    for (mp_uint_t index = 0; index < MP_ARRAY_SIZE(OPCODES); index++) {
        if (OPCODES[index].qstring == opcode) {
            opcode_data = &OPCODES[index];
            break;
        }
    }

    if (!opcode_data || (asm_rv32_allowed_extensions() & opcode_data->required_extensions) != opcode_data->required_extensions) {
        emit_inline_rv32_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
            MP_ERROR_TEXT("invalid RV32 instruction '%q'"), opcode));
        return;
    }

    assert((opcode_data->argument1_mask < MP_ARRAY_SIZE(OPCODE_MASKS)) && "Argument #1 opcode mask index out of bounds.");
    assert((opcode_data->argument2_mask < MP_ARRAY_SIZE(OPCODE_MASKS)) && "Argument #2 opcode mask index out of bounds.");
    assert((opcode_data->argument3_mask < MP_ARRAY_SIZE(OPCODE_MASKS)) && "Argument #3 opcode mask index out of bounds.");
    assert((opcode_data->calling_convention < CALL_COUNT) && "Calling convention index out of bounds.");
    mp_uint_t serialised_arguments[3] = { 0 };
    if (arguments_count != opcode_data->parse_nodes) {
        emit_inline_rv32_error_exc(emit,
            mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
                ET_WRONG_ARGUMENTS_COUNT, opcode, opcode_data->parse_nodes));
        return;
    }

    if (opcode_data->parse_nodes >= 1 && !serialise_argument(emit, opcode_data, argument_nodes[0], 0, &serialised_arguments[0])) {
        return;
    }
    if (opcode_data->calling_convention == CALL_RIR) {
        // "register, offset(base)" calls require some preprocessing to
        // split the offset and base nodes - not to mention that if the offset
        // is negative, the parser won't see the offset as a single node but as
        // a sequence of the minus sign token followed by the number itself.

        if (!serialise_register_offset_node(emit, opcode_data, argument_nodes[1], 1, &serialised_arguments[1], &serialised_arguments[2])) {
            return;
        }
    } else {
        if (opcode_data->parse_nodes >= 2 && !serialise_argument(emit, opcode_data, argument_nodes[1], 1, &serialised_arguments[1])) {
            return;
        }
        if (opcode_data->parse_nodes >= 3 && !serialise_argument(emit, opcode_data, argument_nodes[2], 2, &serialised_arguments[2])) {
            return;
        }
    }

    handle_opcode(emit, opcode_data, serialised_arguments);
}

#undef N
#undef R
#undef I
#undef L
#undef C
#undef U

const emit_inline_asm_method_table_t emit_inline_rv32_method_table = {
    #if MICROPY_DYNAMIC_COMPILER
    emit_inline_rv32_new,
    emit_inline_rv32_free,
    #endif

    emit_inline_rv32_start_pass,
    emit_inline_rv32_end_pass,
    emit_inline_rv32_count_params,
    emit_inline_rv32_label,
    emit_inline_rv32_opcode,
};

#endif // MICROPY_EMIT_INLINE_RV32
