/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#include "py/asmthumb.h"

#if MICROPY_EMIT_INLINE_THUMB

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
    asm_thumb_t as;
    uint16_t pass;
    mp_obj_t *error_slot;
    mp_uint_t max_num_labels;
    qstr *label_lookup;
};

STATIC void emit_inline_thumb_error_msg(emit_inline_asm_t *emit, const char *msg) {
    *emit->error_slot = mp_obj_new_exception_msg(&mp_type_SyntaxError, msg);
}

STATIC void emit_inline_thumb_error_exc(emit_inline_asm_t *emit, mp_obj_t exc) {
    *emit->error_slot = exc;
}

emit_inline_asm_t *emit_inline_thumb_new(mp_uint_t max_num_labels) {
    emit_inline_asm_t *emit = m_new_obj(emit_inline_asm_t);
    memset(&emit->as, 0, sizeof(emit->as));
    mp_asm_base_init(&emit->as.base, max_num_labels);
    emit->max_num_labels = max_num_labels;
    emit->label_lookup = m_new(qstr, max_num_labels);
    return emit;
}

void emit_inline_thumb_free(emit_inline_asm_t *emit) {
    m_del(qstr, emit->label_lookup, emit->max_num_labels);
    mp_asm_base_deinit(&emit->as.base, false);
    m_del_obj(emit_inline_asm_t, emit);
}

STATIC void emit_inline_thumb_start_pass(emit_inline_asm_t *emit, pass_kind_t pass, mp_obj_t *error_slot) {
    emit->pass = pass;
    emit->error_slot = error_slot;
    if (emit->pass == MP_PASS_CODE_SIZE) {
        memset(emit->label_lookup, 0, emit->max_num_labels * sizeof(qstr));
    }
    mp_asm_base_start_pass(&emit->as.base, pass == MP_PASS_EMIT ? MP_ASM_PASS_EMIT : MP_ASM_PASS_COMPUTE);
    asm_thumb_entry(&emit->as, 0);
}

STATIC void emit_inline_thumb_end_pass(emit_inline_asm_t *emit, mp_uint_t type_sig) {
    asm_thumb_exit(&emit->as);
    asm_thumb_end_pass(&emit->as);
}

STATIC mp_uint_t emit_inline_thumb_count_params(emit_inline_asm_t *emit, const byte *p, const byte *ptop) {
    mp_uint_t n_params = 0;
    while (p != ptop) {
        if (++n_params > 4) {
            emit_inline_thumb_error_msg(emit, "can only have up to 4 parameters to Thumb assembly");
            return 0;
        }
        if (!pt_is_any_id(p)) {
            emit_inline_thumb_error_msg(emit, "parameters must be registers in sequence r0 to r3");
            return 0;
        }
        qstr qst;
        p = pt_extract_id(p, &qst);
        const char *param = qstr_str(qst);
        if (!(strlen(param) == 2 && param[0] == 'r' && param[1] == '0' + n_params - 1)) {
            emit_inline_thumb_error_msg(emit, "parameters must be registers in sequence r0 to r3");
            return 0;
        }
    }
    return n_params;
}

STATIC bool emit_inline_thumb_label(emit_inline_asm_t *emit, mp_uint_t label_num, qstr label_id) {
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

typedef struct _reg_name_t { byte reg; byte name[3]; } reg_name_t;
STATIC const reg_name_t reg_name_table[] = {
    {0, "r0\0"},
    {1, "r1\0"},
    {2, "r2\0"},
    {3, "r3\0"},
    {4, "r4\0"},
    {5, "r5\0"},
    {6, "r6\0"},
    {7, "r7\0"},
    {8, "r8\0"},
    {9, "r9\0"},
    {10, "r10"},
    {11, "r11"},
    {12, "r12"},
    {13, "r13"},
    {14, "r14"},
    {15, "r15"},
    {10, "sl\0"},
    {11, "fp\0"},
    {13, "sp\0"},
    {14, "lr\0"},
    {15, "pc\0"},
};

#define MAX_SPECIAL_REGISTER_NAME_LENGTH 7
typedef struct _special_reg_name_t { byte reg; char name[MAX_SPECIAL_REGISTER_NAME_LENGTH + 1]; } special_reg_name_t;
STATIC const special_reg_name_t special_reg_name_table[] = {
    {5, "IPSR"},
    {17, "BASEPRI"},
};

// return empty string in case of error, so we can attempt to parse the string
// without a special check if it was in fact a string
STATIC const char *get_arg_str(const byte *pn) {
    if (pt_is_any_id(pn)) {
        qstr qst;
        pt_extract_id(pn, &qst);
        return qstr_str(qst);
    } else {
        return "";
    }
}

STATIC mp_uint_t get_arg_reg(emit_inline_asm_t *emit, const char *op, const byte *pn, mp_uint_t max_reg) {
    const char *reg_str = get_arg_str(pn);
    for (mp_uint_t i = 0; i < MP_ARRAY_SIZE(reg_name_table); i++) {
        const reg_name_t *r = &reg_name_table[i];
        if (reg_str[0] == r->name[0]
            && reg_str[1] == r->name[1]
            && reg_str[2] == r->name[2]
            && (reg_str[2] == '\0' || reg_str[3] == '\0')) {
            if (r->reg > max_reg) {
                emit_inline_thumb_error_exc(emit,
                    mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
                        "'%s' expects at most r%d", op, max_reg));
                return 0;
            } else {
                return r->reg;
            }
        }
    }
    emit_inline_thumb_error_exc(emit,
        mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
            "'%s' expects a register", op));
    return 0;
}

STATIC mp_uint_t get_arg_special_reg(emit_inline_asm_t *emit, const char *op, const byte *pn) {
    const char *reg_str = get_arg_str(pn);
    for (mp_uint_t i = 0; i < MP_ARRAY_SIZE(special_reg_name_table); i++) {
        const special_reg_name_t *r = &special_reg_name_table[i];
        if (strcmp(r->name, reg_str) == 0) {
            return r->reg;
        }
    }
    emit_inline_thumb_error_exc(emit,
        mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
            "'%s' expects a special register", op));
    return 0;
}

#if MICROPY_EMIT_INLINE_THUMB_FLOAT
STATIC mp_uint_t get_arg_vfpreg(emit_inline_asm_t *emit, const char *op, const byte *pn) {
    const char *reg_str = get_arg_str(pn);
    if (reg_str[0] == 's' && reg_str[1] != '\0') {
        mp_uint_t regno = 0;
        for (++reg_str; *reg_str; ++reg_str) {
            mp_uint_t v = *reg_str;
            if (!('0' <= v && v <= '9')) {
                goto malformed;
            }
            regno = 10 * regno + v - '0';
        }
        if (regno > 31) {
            emit_inline_thumb_error_exc(emit,
                 mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
                       "'%s' expects at most r%d", op, 31));
            return 0;
        } else {
            return regno;
        }
    }
malformed:
    emit_inline_thumb_error_exc(emit,
         mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
            "'%s' expects an FPU register", op));
    return 0;
}
#endif

STATIC mp_uint_t get_arg_reglist(emit_inline_asm_t *emit, const char *op, const byte *p) {
    // a register list looks like {r0, r1, r2} and is parsed as a Python set

    if (!pt_is_rule(p, PN_atom_brace)) {
        goto bad_arg;
    }

    const byte *ptop;
    p = pt_rule_extract_top(p, &ptop);

    mp_uint_t reglist = 0;

    if (p == ptop) {
        goto bad_arg;
    } else if (pt_is_any_id(p)) {
        // set with one element
        reglist |= 1 << get_arg_reg(emit, op, p, 15);
    } else if (pt_is_rule(p, PN_dictorsetmaker)) {
        p = pt_rule_first(p);
        const byte *p1 = pt_next(p);
        if (pt_is_rule(p1, PN_dictorsetmaker_list)) {
            // set with multiple elements

            // get first element of set (we rely on get_arg_reg to catch syntax errors)
            reglist |= 1 << get_arg_reg(emit, op, p, 15);

            // get tail elements (2nd, 3rd, ...)
            const byte *p1_top;
            p1 = pt_rule_extract_top(p1, &p1_top);
            if (p1 != p1_top) {
                mp_parse_node_extract_list(&p1, PN_dictorsetmaker_list2);
            }

            // process rest of elements
            for (; p1 != p1_top; p1 = pt_next(p1)) {
                reglist |= 1 << get_arg_reg(emit, op, p1, 15);
            }
        } else {
            goto bad_arg;
        }
    } else {
        goto bad_arg;
    }

    return reglist;

bad_arg:
    emit_inline_thumb_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, "'%s' expects {r0, r1, ...}", op));
    return 0;
}

STATIC uint32_t get_arg_i(emit_inline_asm_t *emit, const char *op, const byte *pn, uint32_t fit_mask) {
    mp_obj_t o;
    if (!mp_parse_node_get_int_maybe(pn, &o)) {
        emit_inline_thumb_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, "'%s' expects an integer", op));
        return 0;
    }
    uint32_t i = mp_obj_get_int_truncated(o);
    if ((i & (~fit_mask)) != 0) {
        emit_inline_thumb_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, "'%s' integer 0x%x does not fit in mask 0x%x", op, i, fit_mask));
        return 0;
    }
    return i;
}

STATIC bool get_arg_addr(emit_inline_asm_t *emit, const char *op, const byte *p, const byte **p_base, const byte **p_offset) {
    if (!pt_is_rule(p, PN_atom_bracket)) {
        goto bad_arg;
    }
    if (pt_is_rule_empty(p)) {
        goto bad_arg;
    }
    p = pt_rule_first(p);
    if (!pt_is_rule(p, PN_testlist_comp)) {
        goto bad_arg;
    }
    const byte *ptop;
    p = pt_rule_extract_top(p, &ptop);
    if (pt_num_nodes(p, ptop) != 2) {
        goto bad_arg;
    }

    *p_base = p;
    *p_offset = pt_next(p);
    return true;

bad_arg:
    emit_inline_thumb_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, "'%s' expects an address of the form [a, b]", op));
    return false;
}

STATIC int get_arg_label(emit_inline_asm_t *emit, const char *op, const byte *p) {
    if (!pt_is_any_id(p)) {
        emit_inline_thumb_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, "'%s' expects a label", op));
        return 0;
    }
    qstr label_qstr;
    pt_extract_id(p, &label_qstr);
    for (uint i = 0; i < emit->max_num_labels; i++) {
        if (emit->label_lookup[i] == label_qstr) {
            return i;
        }
    }
    // only need to have the labels on the last pass
    if (emit->pass == MP_PASS_EMIT) {
        emit_inline_thumb_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, "label '%q' not defined", label_qstr));
    }
    return 0;
}

typedef struct _cc_name_t { byte cc; byte name[2]; } cc_name_t;
STATIC const cc_name_t cc_name_table[] = {
    { ASM_THUMB_CC_EQ, "eq" },
    { ASM_THUMB_CC_NE, "ne" },
    { ASM_THUMB_CC_CS, "cs" },
    { ASM_THUMB_CC_CC, "cc" },
    { ASM_THUMB_CC_MI, "mi" },
    { ASM_THUMB_CC_PL, "pl" },
    { ASM_THUMB_CC_VS, "vs" },
    { ASM_THUMB_CC_VC, "vc" },
    { ASM_THUMB_CC_HI, "hi" },
    { ASM_THUMB_CC_LS, "ls" },
    { ASM_THUMB_CC_GE, "ge" },
    { ASM_THUMB_CC_LT, "lt" },
    { ASM_THUMB_CC_GT, "gt" },
    { ASM_THUMB_CC_LE, "le" },
};

typedef struct _format_4_op_t { byte op; char name[3]; } format_4_op_t;
#define X(x) (((x) >> 4) & 0xff) // only need 1 byte to distinguish these ops
STATIC const format_4_op_t format_4_op_table[] = {
    { X(ASM_THUMB_FORMAT_4_EOR), "eor" },
    { X(ASM_THUMB_FORMAT_4_LSL), "lsl" },
    { X(ASM_THUMB_FORMAT_4_LSR), "lsr" },
    { X(ASM_THUMB_FORMAT_4_ASR), "asr" },
    { X(ASM_THUMB_FORMAT_4_ADC), "adc" },
    { X(ASM_THUMB_FORMAT_4_SBC), "sbc" },
    { X(ASM_THUMB_FORMAT_4_ROR), "ror" },
    { X(ASM_THUMB_FORMAT_4_TST), "tst" },
    { X(ASM_THUMB_FORMAT_4_NEG), "neg" },
    { X(ASM_THUMB_FORMAT_4_CMP), "cmp" },
    { X(ASM_THUMB_FORMAT_4_CMN), "cmn" },
    { X(ASM_THUMB_FORMAT_4_ORR), "orr" },
    { X(ASM_THUMB_FORMAT_4_MUL), "mul" },
    { X(ASM_THUMB_FORMAT_4_BIC), "bic" },
    { X(ASM_THUMB_FORMAT_4_MVN), "mvn" },
};
#undef X

// name is actually a qstr, which should fit in 16 bits
typedef struct _format_9_10_op_t { uint16_t op; uint16_t name; } format_9_10_op_t;
#define X(x) (x)
STATIC const format_9_10_op_t format_9_10_op_table[] = {
    { X(ASM_THUMB_FORMAT_9_LDR | ASM_THUMB_FORMAT_9_WORD_TRANSFER), MP_QSTR_ldr },
    { X(ASM_THUMB_FORMAT_9_LDR | ASM_THUMB_FORMAT_9_BYTE_TRANSFER), MP_QSTR_ldrb },
    { X(ASM_THUMB_FORMAT_10_LDRH), MP_QSTR_ldrh },
    { X(ASM_THUMB_FORMAT_9_STR | ASM_THUMB_FORMAT_9_WORD_TRANSFER), MP_QSTR_str },
    { X(ASM_THUMB_FORMAT_9_STR | ASM_THUMB_FORMAT_9_BYTE_TRANSFER), MP_QSTR_strb },
    { X(ASM_THUMB_FORMAT_10_STRH), MP_QSTR_strh },
};
#undef X

#if MICROPY_EMIT_INLINE_THUMB_FLOAT
// actual opcodes are: 0xee00 | op.hi_nibble, 0x0a00 | op.lo_nibble
typedef struct _format_vfp_op_t { byte op; char name[3]; } format_vfp_op_t;
STATIC const format_vfp_op_t format_vfp_op_table[] = {
    { 0x30, "add" },
    { 0x34, "sub" },
    { 0x20, "mul" },
    { 0x80, "div" },
};
#endif

// shorthand alias for whether we allow ARMv7-M instructions
#define ARMV7M MICROPY_EMIT_INLINE_THUMB_ARMV7M

STATIC void emit_inline_thumb_op(emit_inline_asm_t *emit, qstr op, mp_uint_t n_args, const byte **pn_args) {
    // TODO perhaps make two tables:
    // one_args =
    // "b", LAB, asm_thumb_b_n,
    // "bgt", LAB, asm_thumb_bgt_n,
    // two_args =
    // "movs", RLO, I8, asm_thumb_movs_reg_i8
    // "movw", REG, REG, asm_thumb_movw_reg_i16
    // three_args =
    // "subs", RLO, RLO, I3, asm_thumb_subs_reg_reg_i3

    size_t op_len;
    const char *op_str = (const char*)qstr_data(op, &op_len);

    #if MICROPY_EMIT_INLINE_THUMB_FLOAT
    if (op_str[0] == 'v') {
        // floating point operations
        if (n_args == 2) {
            mp_uint_t op_code = 0x0ac0, op_code_hi;
            if (op == MP_QSTR_vcmp) {
                op_code_hi = 0xeeb4;
                op_vfp_twoargs:;
                mp_uint_t vd = get_arg_vfpreg(emit, op_str, pn_args[0]);
                mp_uint_t vm = get_arg_vfpreg(emit, op_str, pn_args[1]);
                asm_thumb_op32(&emit->as,
                    op_code_hi | ((vd & 1) << 6),
                    op_code | ((vd & 0x1e) << 11) | ((vm & 1) << 5) | (vm & 0x1e) >> 1);
            } else if (op == MP_QSTR_vsqrt) {
                op_code_hi = 0xeeb1;
                goto op_vfp_twoargs;
            } else if (op == MP_QSTR_vneg) {
                op_code_hi = 0xeeb1;
                op_code = 0x0a40;
                goto op_vfp_twoargs;
            } else if (op == MP_QSTR_vcvt_f32_s32) {
                op_code_hi = 0xeeb8; // int to float
                goto op_vfp_twoargs;
            } else if (op == MP_QSTR_vcvt_s32_f32) {
                op_code_hi = 0xeebd; // float to int
                goto op_vfp_twoargs;
            } else if (op == MP_QSTR_vmrs) {
                mp_uint_t reg_dest;
                const char *reg_str0 = get_arg_str(pn_args[0]);
                if (strcmp(reg_str0, "APSR_nzcv") == 0) {
                    reg_dest = 15;
                } else {
                    reg_dest = get_arg_reg(emit, op_str, pn_args[0], 15);
                }
                const char *reg_str1 = get_arg_str(pn_args[1]);
                if (strcmp(reg_str1, "FPSCR") == 0) {
                    // FP status to ARM reg
                    asm_thumb_op32(&emit->as, 0xeef1, 0x0a10 | (reg_dest << 12));
                } else {
                    goto unknown_op;
                }
            } else if (op == MP_QSTR_vmov) {
                op_code_hi = 0xee00;
                mp_uint_t r_arm, vm;
                const char *reg_str = get_arg_str(pn_args[0]);
                if (reg_str[0] == 'r') {
                    r_arm = get_arg_reg(emit, op_str, pn_args[0], 15);
                    vm = get_arg_vfpreg(emit, op_str, pn_args[1]);
                    op_code_hi |= 0x10;
                } else {
                    vm = get_arg_vfpreg(emit, op_str, pn_args[0]);
                    r_arm = get_arg_reg(emit, op_str, pn_args[1], 15);
                }
                asm_thumb_op32(&emit->as,
                    op_code_hi | ((vm & 0x1e) >> 1),
                    0x0a10 | (r_arm << 12) | ((vm & 1) << 7));
            } else if (op == MP_QSTR_vldr) {
                op_code_hi = 0xed90;
                op_vldr_vstr:;
                mp_uint_t vd = get_arg_vfpreg(emit, op_str, pn_args[0]);
                const byte *pn_base, *pn_offset;
                if (get_arg_addr(emit, op_str, pn_args[1], &pn_base, &pn_offset)) {
                    mp_uint_t rlo_base = get_arg_reg(emit, op_str, pn_base, 7);
                    mp_uint_t i8;
                    i8 = get_arg_i(emit, op_str, pn_offset, 0x3fc) >> 2;
                    asm_thumb_op32(&emit->as,
                        op_code_hi | rlo_base | ((vd & 1) << 6),
                        0x0a00 | ((vd & 0x1e) << 11) | i8);
                }
            } else if (op == MP_QSTR_vstr) {
                op_code_hi = 0xed80;
                goto op_vldr_vstr;
            } else {
                goto unknown_op;
            }
        } else if (n_args == 3) {
            // search table for arith ops
            for (mp_uint_t i = 0; i < MP_ARRAY_SIZE(format_vfp_op_table); i++) {
                if (strncmp(op_str + 1, format_vfp_op_table[i].name, 3) == 0 && op_str[4] == '\0') {
                    mp_uint_t op_code_hi = 0xee00 | (format_vfp_op_table[i].op & 0xf0);
                    mp_uint_t op_code = 0x0a00 | ((format_vfp_op_table[i].op & 0x0f) << 4);
                    mp_uint_t vd = get_arg_vfpreg(emit, op_str, pn_args[0]);
                    mp_uint_t vn = get_arg_vfpreg(emit, op_str, pn_args[1]);
                    mp_uint_t vm = get_arg_vfpreg(emit, op_str, pn_args[2]);
                    asm_thumb_op32(&emit->as,
                        op_code_hi | ((vd & 1) << 6) | (vn >> 1),
                        op_code | (vm >> 1) | ((vm & 1) << 5) | ((vd & 0x1e) << 11) | ((vn & 1) << 7));
                    return;
                }
            }
            goto unknown_op;
        } else {
            goto unknown_op;
        }
    } else
    #endif
    if (n_args == 0) {
        if (op == MP_QSTR_nop) {
            asm_thumb_op16(&emit->as, ASM_THUMB_OP_NOP);
        } else if (op == MP_QSTR_wfi) {
            asm_thumb_op16(&emit->as, ASM_THUMB_OP_WFI);
        } else {
            goto unknown_op;
        }

    } else if (n_args == 1) {
        if (op == MP_QSTR_b) {
            int label_num = get_arg_label(emit, op_str, pn_args[0]);
            if (!asm_thumb_b_n_label(&emit->as, label_num)) {
                goto branch_not_in_range;
            }
        } else if (op == MP_QSTR_bl) {
            int label_num = get_arg_label(emit, op_str, pn_args[0]);
            if (!asm_thumb_bl_label(&emit->as, label_num)) {
                goto branch_not_in_range;
            }
        } else if (op == MP_QSTR_bx) {
            mp_uint_t r = get_arg_reg(emit, op_str, pn_args[0], 15);
            asm_thumb_op16(&emit->as, 0x4700 | (r << 3));
        } else if (op_str[0] == 'b' && (op_len == 3
                    || (op_len == 5 && op_str[3] == '_'
                        && (op_str[4] == 'n' || (ARMV7M && op_str[4] == 'w'))))) {
            mp_uint_t cc = -1;
            for (mp_uint_t i = 0; i < MP_ARRAY_SIZE(cc_name_table); i++) {
                if (op_str[1] == cc_name_table[i].name[0] && op_str[2] == cc_name_table[i].name[1]) {
                    cc = cc_name_table[i].cc;
                }
            }
            if (cc == (mp_uint_t)-1) {
                goto unknown_op;
            }
            int label_num = get_arg_label(emit, op_str, pn_args[0]);
            if (!asm_thumb_bcc_nw_label(&emit->as, cc, label_num, op_len == 5 && op_str[4] == 'w')) {
                goto branch_not_in_range;
            }
        } else if (ARMV7M && op_str[0] == 'i' && op_str[1] == 't') {
            const char *arg_str = get_arg_str(pn_args[0]);
            mp_uint_t cc = -1;
            for (mp_uint_t i = 0; i < MP_ARRAY_SIZE(cc_name_table); i++) {
                if (arg_str[0] == cc_name_table[i].name[0]
                    && arg_str[1] == cc_name_table[i].name[1]
                    && arg_str[2] == '\0') {
                    cc = cc_name_table[i].cc;
                    break;
                }
            }
            if (cc == (mp_uint_t)-1) {
                goto unknown_op;
            }
            const char *os = op_str + 2;
            while (*os != '\0') {
                os++;
            }
            if (os > op_str + 5) {
                goto unknown_op;
            }
            mp_uint_t it_mask = 8;
            while (--os >= op_str + 2) {
                it_mask >>= 1;
                if (*os == 't') {
                    it_mask |= (cc & 1) << 3;
                } else if (*os == 'e') {
                    it_mask |= ((~cc) & 1) << 3;
                } else {
                    goto unknown_op;
                }
            }
            asm_thumb_it_cc(&emit->as, cc, it_mask);
        } else if (op == MP_QSTR_cpsid) {
            // TODO check pn_args[0] == i
            asm_thumb_op16(&emit->as, ASM_THUMB_OP_CPSID_I);
        } else if (op == MP_QSTR_cpsie) {
            // TODO check pn_args[0] == i
            asm_thumb_op16(&emit->as, ASM_THUMB_OP_CPSIE_I);
        } else if (op == MP_QSTR_push) {
            mp_uint_t reglist = get_arg_reglist(emit, op_str, pn_args[0]);
            if ((reglist & 0xff00) == 0) {
                asm_thumb_op16(&emit->as, 0xb400 | reglist);
            } else {
                if (!ARMV7M) {
                    goto unknown_op;
                }
                asm_thumb_op32(&emit->as, 0xe92d, reglist);
            }
        } else if (op == MP_QSTR_pop) {
            mp_uint_t reglist = get_arg_reglist(emit, op_str, pn_args[0]);
            if ((reglist & 0xff00) == 0) {
                asm_thumb_op16(&emit->as, 0xbc00 | reglist);
            } else {
                if (!ARMV7M) {
                    goto unknown_op;
                }
                asm_thumb_op32(&emit->as, 0xe8bd, reglist);
            }
        } else {
            goto unknown_op;
        }

    } else if (n_args == 2) {
        if (pt_is_any_id(pn_args[1])) {
            // second arg is a register (or should be)
            mp_uint_t op_code, op_code_hi;
            if (op == MP_QSTR_mov) {
                mp_uint_t reg_dest = get_arg_reg(emit, op_str, pn_args[0], 15);
                mp_uint_t reg_src = get_arg_reg(emit, op_str, pn_args[1], 15);
                asm_thumb_mov_reg_reg(&emit->as, reg_dest, reg_src);
            } else if (ARMV7M && op == MP_QSTR_clz) {
                op_code_hi = 0xfab0;
                op_code = 0xf080;
                mp_uint_t rd, rm;
                op_clz_rbit:
                rd = get_arg_reg(emit, op_str, pn_args[0], 15);
                rm = get_arg_reg(emit, op_str, pn_args[1], 15);
                asm_thumb_op32(&emit->as, op_code_hi | rm, op_code | (rd << 8) | rm);
            } else if (ARMV7M && op == MP_QSTR_rbit) {
                op_code_hi = 0xfa90;
                op_code = 0xf0a0;
                goto op_clz_rbit;
            } else if (ARMV7M && op == MP_QSTR_mrs){
                mp_uint_t reg_dest = get_arg_reg(emit, op_str, pn_args[0], 12);
                mp_uint_t reg_src = get_arg_special_reg(emit, op_str, pn_args[1]);
                asm_thumb_op32(&emit->as, 0xf3ef, 0x8000 | (reg_dest << 8) | reg_src);
            } else {
                if (op == MP_QSTR_and_) {
                    op_code = ASM_THUMB_FORMAT_4_AND;
                    mp_uint_t reg_dest, reg_src;
                    op_format_4:
                    reg_dest = get_arg_reg(emit, op_str, pn_args[0], 7);
                    reg_src = get_arg_reg(emit, op_str, pn_args[1], 7);
                    asm_thumb_format_4(&emit->as, op_code, reg_dest, reg_src);
                    return;
                }
                // search table for ALU ops
                for (mp_uint_t i = 0; i < MP_ARRAY_SIZE(format_4_op_table); i++) {
                    if (strncmp(op_str, format_4_op_table[i].name, 3) == 0 && op_str[3] == '\0') {
                        op_code = 0x4000 | (format_4_op_table[i].op << 4);
                        goto op_format_4;
                    }
                }
                goto unknown_op;
            }
        } else {
            // second arg is not a register
            mp_uint_t op_code;
            if (op == MP_QSTR_mov) {
                op_code = ASM_THUMB_FORMAT_3_MOV;
                mp_uint_t rlo_dest, i8_src;
                op_format_3:
                rlo_dest = get_arg_reg(emit, op_str, pn_args[0], 7);
                i8_src = get_arg_i(emit, op_str, pn_args[1], 0xff);
                asm_thumb_format_3(&emit->as, op_code, rlo_dest, i8_src);
            } else if (op == MP_QSTR_cmp) {
                op_code = ASM_THUMB_FORMAT_3_CMP;
                goto op_format_3;
            } else if (op == MP_QSTR_add) {
                op_code = ASM_THUMB_FORMAT_3_ADD;
                goto op_format_3;
            } else if (op == MP_QSTR_sub) {
                op_code = ASM_THUMB_FORMAT_3_SUB;
                goto op_format_3;
            } else if (ARMV7M && op == MP_QSTR_movw) {
                op_code = ASM_THUMB_OP_MOVW;
                mp_uint_t reg_dest;
                op_movw_movt:
                reg_dest = get_arg_reg(emit, op_str, pn_args[0], 15);
                int i_src = get_arg_i(emit, op_str, pn_args[1], 0xffff);
                asm_thumb_mov_reg_i16(&emit->as, op_code, reg_dest, i_src);
            } else if (ARMV7M && op == MP_QSTR_movt) {
                op_code = ASM_THUMB_OP_MOVT;
                goto op_movw_movt;
            } else if (ARMV7M && op == MP_QSTR_movwt) {
                // this is a convenience instruction
                mp_uint_t reg_dest = get_arg_reg(emit, op_str, pn_args[0], 15);
                uint32_t i_src = get_arg_i(emit, op_str, pn_args[1], 0xffffffff);
                asm_thumb_mov_reg_i16(&emit->as, ASM_THUMB_OP_MOVW, reg_dest, i_src & 0xffff);
                asm_thumb_mov_reg_i16(&emit->as, ASM_THUMB_OP_MOVT, reg_dest, (i_src >> 16) & 0xffff);
            } else if (ARMV7M && op == MP_QSTR_ldrex) {
                mp_uint_t r_dest = get_arg_reg(emit, op_str, pn_args[0], 15);
                const byte *pn_base, *pn_offset;
                if (get_arg_addr(emit, op_str, pn_args[1], &pn_base, &pn_offset)) {
                    mp_uint_t r_base = get_arg_reg(emit, op_str, pn_base, 15);
                    mp_uint_t i8 = get_arg_i(emit, op_str, pn_offset, 0xff) >> 2;
                    asm_thumb_op32(&emit->as, 0xe850 | r_base, 0x0f00 | (r_dest << 12) | i8);
                }
            } else {
                // search table for ldr/str instructions
                for (mp_uint_t i = 0; i < MP_ARRAY_SIZE(format_9_10_op_table); i++) {
                    if (op == format_9_10_op_table[i].name) {
                        op_code = format_9_10_op_table[i].op;
                        const byte *pn_base, *pn_offset;
                        mp_uint_t rlo_dest = get_arg_reg(emit, op_str, pn_args[0], 7);
                        if (get_arg_addr(emit, op_str, pn_args[1], &pn_base, &pn_offset)) {
                            mp_uint_t rlo_base = get_arg_reg(emit, op_str, pn_base, 7);
                            mp_uint_t i5;
                            if (op_code & ASM_THUMB_FORMAT_9_BYTE_TRANSFER) {
                                i5 = get_arg_i(emit, op_str, pn_offset, 0x1f);
                            } else if (op_code & ASM_THUMB_FORMAT_10_STRH) { // also catches LDRH
                                i5 = get_arg_i(emit, op_str, pn_offset, 0x3e) >> 1;
                            } else {
                                i5 = get_arg_i(emit, op_str, pn_offset, 0x7c) >> 2;
                            }
                            asm_thumb_format_9_10(&emit->as, op_code, rlo_dest, rlo_base, i5);
                            return;
                        }
                        break;
                    }
                }
                goto unknown_op;
            }
        }

    } else if (n_args == 3) {
        mp_uint_t op_code;
        if (op == MP_QSTR_lsl) {
            op_code = ASM_THUMB_FORMAT_1_LSL;
            mp_uint_t rlo_dest, rlo_src, i5;
            op_format_1:
            rlo_dest = get_arg_reg(emit, op_str, pn_args[0], 7);
            rlo_src = get_arg_reg(emit, op_str, pn_args[1], 7);
            i5 = get_arg_i(emit, op_str, pn_args[2], 0x1f);
            asm_thumb_format_1(&emit->as, op_code, rlo_dest, rlo_src, i5);
        } else if (op == MP_QSTR_lsr) {
            op_code = ASM_THUMB_FORMAT_1_LSR;
            goto op_format_1;
        } else if (op == MP_QSTR_asr) {
            op_code = ASM_THUMB_FORMAT_1_ASR;
            goto op_format_1;
        } else if (op == MP_QSTR_add) {
            op_code = ASM_THUMB_FORMAT_2_ADD;
            mp_uint_t rlo_dest, rlo_src;
            op_format_2:
            rlo_dest = get_arg_reg(emit, op_str, pn_args[0], 7);
            rlo_src = get_arg_reg(emit, op_str, pn_args[1], 7);
            int src_b;
            if (pt_is_any_id(pn_args[2])) {
                op_code |= ASM_THUMB_FORMAT_2_REG_OPERAND;
                src_b = get_arg_reg(emit, op_str, pn_args[2], 7);
            } else {
                op_code |= ASM_THUMB_FORMAT_2_IMM_OPERAND;
                src_b = get_arg_i(emit, op_str, pn_args[2], 0x7);
            }
            asm_thumb_format_2(&emit->as, op_code, rlo_dest, rlo_src, src_b);
        } else if (ARMV7M && op == MP_QSTR_sdiv) {
            op_code = 0xfb90; // sdiv high part
            mp_uint_t rd, rn, rm;
            op_sdiv_udiv:
            rd = get_arg_reg(emit, op_str, pn_args[0], 15);
            rn = get_arg_reg(emit, op_str, pn_args[1], 15);
            rm = get_arg_reg(emit, op_str, pn_args[2], 15);
            asm_thumb_op32(&emit->as, op_code | rn, 0xf0f0 | (rd << 8) | rm);
        } else if (ARMV7M && op == MP_QSTR_udiv) {
            op_code = 0xfbb0; // udiv high part
            goto op_sdiv_udiv;
        } else if (op == MP_QSTR_sub) {
            op_code = ASM_THUMB_FORMAT_2_SUB;
            goto op_format_2;
        } else if (ARMV7M && op == MP_QSTR_strex) {
            mp_uint_t r_dest = get_arg_reg(emit, op_str, pn_args[0], 15);
            mp_uint_t r_src = get_arg_reg(emit, op_str, pn_args[1], 15);
            const byte *pn_base, *pn_offset;
            if (get_arg_addr(emit, op_str, pn_args[2], &pn_base, &pn_offset)) {
                mp_uint_t r_base = get_arg_reg(emit, op_str, pn_base, 15);
                mp_uint_t i8 = get_arg_i(emit, op_str, pn_offset, 0xff) >> 2;
                asm_thumb_op32(&emit->as, 0xe840 | r_base, (r_src << 12) | (r_dest << 8) | i8);
            }
        } else {
            goto unknown_op;
        }

    } else {
        goto unknown_op;
    }

    return;

unknown_op:
    emit_inline_thumb_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, "unsupported Thumb instruction '%s' with %d arguments", op_str, n_args));
    return;

branch_not_in_range:
    emit_inline_thumb_error_msg(emit, "branch not in range");
    return;
}

const emit_inline_asm_method_table_t emit_inline_thumb_method_table = {
    emit_inline_thumb_start_pass,
    emit_inline_thumb_end_pass,
    emit_inline_thumb_count_params,
    emit_inline_thumb_label,
    emit_inline_thumb_op,
};

#endif // MICROPY_EMIT_INLINE_THUMB
