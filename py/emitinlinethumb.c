/*
 * This file is part of the Micro Python project, http://micropython.org/
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
#define DEF_RULE(rule, comp, kind, ...) PN_##rule,
#include "py/grammar.h"
#undef DEF_RULE
    PN_maximum_number_of,
} pn_kind_t;

struct _emit_inline_asm_t {
    uint16_t pass;
    scope_t *scope;
    mp_obj_t *error_slot;
    mp_uint_t max_num_labels;
    qstr *label_lookup;
    asm_thumb_t *as;
};

STATIC void emit_inline_thumb_error_msg(emit_inline_asm_t *emit, const char *msg) {
    *emit->error_slot = mp_obj_new_exception_msg(&mp_type_SyntaxError, msg);
}

STATIC void emit_inline_thumb_error_exc(emit_inline_asm_t *emit, mp_obj_t exc) {
    *emit->error_slot = exc;
}

emit_inline_asm_t *emit_inline_thumb_new(mp_uint_t max_num_labels) {
    emit_inline_asm_t *emit = m_new_obj(emit_inline_asm_t);
    emit->max_num_labels = max_num_labels;
    emit->label_lookup = m_new(qstr, max_num_labels);
    memset(emit->label_lookup, 0, emit->max_num_labels * sizeof(qstr));
    emit->as = asm_thumb_new(max_num_labels);
    return emit;
}

void emit_inline_thumb_free(emit_inline_asm_t *emit) {
    m_del(qstr, emit->label_lookup, emit->max_num_labels);
    asm_thumb_free(emit->as, false);
    m_del_obj(emit_inline_asm_t, emit);
}

STATIC void emit_inline_thumb_start_pass(emit_inline_asm_t *emit, pass_kind_t pass, scope_t *scope, mp_obj_t *error_slot) {
    emit->pass = pass;
    emit->scope = scope;
    emit->error_slot = error_slot;
    asm_thumb_start_pass(emit->as, pass == MP_PASS_EMIT ? ASM_THUMB_PASS_EMIT : ASM_THUMB_PASS_COMPUTE);
    asm_thumb_entry(emit->as, 0);
}

STATIC void emit_inline_thumb_end_pass(emit_inline_asm_t *emit) {
    asm_thumb_exit(emit->as);
    asm_thumb_end_pass(emit->as);

    if (emit->pass == MP_PASS_EMIT) {
        void *f = asm_thumb_get_code(emit->as);
        mp_emit_glue_assign_native(emit->scope->raw_code, MP_CODE_NATIVE_ASM, f, asm_thumb_get_code_size(emit->as), emit->scope->num_pos_args, 0);
    }
}

STATIC mp_uint_t emit_inline_thumb_count_params(emit_inline_asm_t *emit, mp_uint_t n_params, mp_parse_node_t *pn_params) {
    if (n_params > 4) {
        emit_inline_thumb_error_msg(emit, "can only have up to 4 parameters to Thumb assembly");
        return 0;
    }
    for (mp_uint_t i = 0; i < n_params; i++) {
        if (!MP_PARSE_NODE_IS_ID(pn_params[i])) {
            emit_inline_thumb_error_msg(emit, "parameters must be registers in sequence r0 to r3");
            return 0;
        }
        const char *p = qstr_str(MP_PARSE_NODE_LEAF_ARG(pn_params[i]));
        if (!(strlen(p) == 2 && p[0] == 'r' && p[1] == '0' + i)) {
            emit_inline_thumb_error_msg(emit, "parameters must be registers in sequence r0 to r3");
            return 0;
        }
    }
    return n_params;
}

STATIC void emit_inline_thumb_label(emit_inline_asm_t *emit, mp_uint_t label_num, qstr label_id) {
    assert(label_num < emit->max_num_labels);
    emit->label_lookup[label_num] = label_id;
    asm_thumb_label_assign(emit->as, label_num);
}

STATIC void emit_inline_thumb_align(emit_inline_asm_t *emit, mp_uint_t align) {
    asm_thumb_align(emit->as, align);
}

STATIC void emit_inline_thumb_data(emit_inline_asm_t *emit, mp_uint_t bytesize, mp_uint_t val) {
    asm_thumb_data(emit->as, bytesize, val);
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

// return empty string in case of error, so we can attempt to parse the string
// without a special check if it was in fact a string
STATIC const char *get_arg_str(mp_parse_node_t pn) {
    if (MP_PARSE_NODE_IS_ID(pn)) {
        qstr qst = MP_PARSE_NODE_LEAF_ARG(pn);
        return qstr_str(qst);
    } else {
        return "";
    }
}

STATIC mp_uint_t get_arg_reg(emit_inline_asm_t *emit, const char *op, mp_parse_node_t pn, mp_uint_t max_reg) {
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

STATIC mp_uint_t get_arg_reglist(emit_inline_asm_t *emit, const char *op, mp_parse_node_t pn) {
    // a register list looks like {r0, r1, r2} and is parsed as a Python set

    if (!MP_PARSE_NODE_IS_STRUCT_KIND(pn, PN_atom_brace)) {
        goto bad_arg;
    }

    mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
    assert(MP_PARSE_NODE_STRUCT_NUM_NODES(pns) == 1); // should always be
    pn = pns->nodes[0];

    mp_uint_t reglist = 0;

    if (MP_PARSE_NODE_IS_ID(pn)) {
        // set with one element
        reglist |= 1 << get_arg_reg(emit, op, pn, 15);
    } else if (MP_PARSE_NODE_IS_STRUCT(pn)) {
        pns = (mp_parse_node_struct_t*)pn;
        if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_dictorsetmaker) {
            assert(MP_PARSE_NODE_IS_STRUCT(pns->nodes[1])); // should succeed
            mp_parse_node_struct_t *pns1 = (mp_parse_node_struct_t*)pns->nodes[1];
            if (MP_PARSE_NODE_STRUCT_KIND(pns1) == PN_dictorsetmaker_list) {
                // set with multiple elements

                // get first element of set (we rely on get_arg_reg to catch syntax errors)
                reglist |= 1 << get_arg_reg(emit, op, pns->nodes[0], 15);

                // get tail elements (2nd, 3rd, ...)
                mp_parse_node_t *nodes;
                int n = mp_parse_node_extract_list(&pns1->nodes[0], PN_dictorsetmaker_list2, &nodes);

                // process rest of elements
                for (int i = 0; i < n; i++) {
                    reglist |= 1 << get_arg_reg(emit, op, nodes[i], 15);
                }
            } else {
                goto bad_arg;
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

STATIC int get_arg_i(emit_inline_asm_t *emit, const char *op, mp_parse_node_t pn, int fit_mask) {
    if (!MP_PARSE_NODE_IS_SMALL_INT(pn)) {
        emit_inline_thumb_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, "'%s' expects an integer", op));
        return 0;
    }
    int i = MP_PARSE_NODE_LEAF_SMALL_INT(pn);
    if ((i & (~fit_mask)) != 0) {
        emit_inline_thumb_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, "'%s' integer 0x%x does not fit in mask 0x%x", op, i, fit_mask));
        return 0;
    }
    return i;
}

STATIC bool get_arg_addr(emit_inline_asm_t *emit, const char *op, mp_parse_node_t pn, mp_parse_node_t *pn_base, mp_parse_node_t *pn_offset) {
    if (!MP_PARSE_NODE_IS_STRUCT_KIND(pn, PN_atom_bracket)) {
        goto bad_arg;
    }
    mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
    if (!MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_testlist_comp)) {
        goto bad_arg;
    }
    pns = (mp_parse_node_struct_t*)pns->nodes[0];
    if (MP_PARSE_NODE_STRUCT_NUM_NODES(pns) != 2) {
        goto bad_arg;
    }

    *pn_base = pns->nodes[0];
    *pn_offset = pns->nodes[1];
    return true;

bad_arg:
    emit_inline_thumb_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, "'%s' expects an address of the form [a, b]", op));
    return false;
}

STATIC int get_arg_label(emit_inline_asm_t *emit, const char *op, mp_parse_node_t pn) {
    if (!MP_PARSE_NODE_IS_ID(pn)) {
        emit_inline_thumb_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, "'%s' expects a label", op));
        return 0;
    }
    qstr label_qstr = MP_PARSE_NODE_LEAF_ARG(pn);
    for (int i = 0; i < emit->max_num_labels; i++) {
        if (emit->label_lookup[i] == label_qstr) {
            return i;
        }
    }
    // only need to have the labels on the last pass
    if (emit->pass == MP_PASS_EMIT) {
        emit_inline_thumb_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, "label '%s' not defined", qstr_str(label_qstr)));
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

typedef struct _format_9_10_op_t { uint16_t op; char name[5]; } format_9_10_op_t;
#define X(x) (x)
STATIC const format_9_10_op_t format_9_10_op_table[] = {
    { X(ASM_THUMB_FORMAT_9_LDR | ASM_THUMB_FORMAT_9_WORD_TRANSFER), "ldr" },
    { X(ASM_THUMB_FORMAT_9_LDR | ASM_THUMB_FORMAT_9_BYTE_TRANSFER), "ldrb" },
    { X(ASM_THUMB_FORMAT_10_LDRH), "ldrh" },
    { X(ASM_THUMB_FORMAT_9_STR | ASM_THUMB_FORMAT_9_WORD_TRANSFER), "str" },
    { X(ASM_THUMB_FORMAT_9_STR | ASM_THUMB_FORMAT_9_BYTE_TRANSFER), "strb" },
    { X(ASM_THUMB_FORMAT_10_STRH), "strh" },
};
#undef X

STATIC void emit_inline_thumb_op(emit_inline_asm_t *emit, qstr op, mp_uint_t n_args, mp_parse_node_t *pn_args) {
    // TODO perhaps make two tables:
    // one_args =
    // "b", LAB, asm_thumb_b_n,
    // "bgt", LAB, asm_thumb_bgt_n,
    // two_args =
    // "movs", RLO, I8, asm_thumb_movs_reg_i8
    // "movw", REG, REG, asm_thumb_movw_reg_i16
    // three_args =
    // "subs", RLO, RLO, I3, asm_thumb_subs_reg_reg_i3

    const char *op_str = qstr_str(op);
    mp_uint_t op_len = strlen(op_str);

    if (n_args == 0) {
        if (strcmp(op_str, "nop") == 0) {
            asm_thumb_op16(emit->as, ASM_THUMB_OP_NOP);
        } else if (strcmp(op_str, "wfi") == 0) {
            asm_thumb_op16(emit->as, ASM_THUMB_OP_WFI);
        } else {
            goto unknown_op;
        }

    } else if (n_args == 1) {
        if (strcmp(op_str, "b") == 0) {
            int label_num = get_arg_label(emit, op_str, pn_args[0]);
            if (!asm_thumb_b_n_label(emit->as, label_num)) {
                goto branch_not_in_range;
            }
        } else if (strcmp(op_str, "bl") == 0) {
            int label_num = get_arg_label(emit, op_str, pn_args[0]);
            if (!asm_thumb_bl_label(emit->as, label_num)) {
                goto branch_not_in_range;
            }
        } else if (strcmp(op_str, "bx") == 0) {
            mp_uint_t r = get_arg_reg(emit, op_str, pn_args[0], 15);
            asm_thumb_op16(emit->as, 0x4700 | (r << 3));
        } else if (op_str[0] == 'b' && op_len == 3) {
            mp_uint_t cc = -1;
            for (mp_uint_t i = 0; i < MP_ARRAY_SIZE(cc_name_table); i++) {
                if (op_str[1] == cc_name_table[i].name[0] && op_str[2] == cc_name_table[i].name[1]) {
                    cc = cc_name_table[i].cc;
                }
            }
            if (cc == -1) {
                goto unknown_op;
            }
            int label_num = get_arg_label(emit, op_str, pn_args[0]);
            if (!asm_thumb_bcc_n_label(emit->as, cc, label_num)) {
                goto branch_not_in_range;
            }
        } else if (op_str[0] == 'i' && op_str[1] == 't') {
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
            if (cc == -1) {
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
            asm_thumb_it_cc(emit->as, cc, it_mask);
        } else if (strcmp(op_str, "cpsid") == 0) {
            // TODO check pn_args[0] == i
            asm_thumb_op16(emit->as, ASM_THUMB_OP_CPSID_I);
        } else if (strcmp(op_str, "cpsie") == 0) {
            // TODO check pn_args[0] == i
            asm_thumb_op16(emit->as, ASM_THUMB_OP_CPSIE_I);
        } else if (strcmp(op_str, "push") == 0) {
            mp_uint_t reglist = get_arg_reglist(emit, op_str, pn_args[0]);
            if ((reglist & 0xff00) == 0) {
                asm_thumb_op16(emit->as, 0xb400 | reglist);
            } else {
                asm_thumb_op32(emit->as, 0xe92d, reglist);
            }
        } else if (strcmp(op_str, "pop") == 0) {
            mp_uint_t reglist = get_arg_reglist(emit, op_str, pn_args[0]);
            if ((reglist & 0xff00) == 0) {
                asm_thumb_op16(emit->as, 0xbc00 | reglist);
            } else {
                asm_thumb_op32(emit->as, 0xe8bd, reglist);
            }
        } else {
            goto unknown_op;
        }

    } else if (n_args == 2) {
        if (MP_PARSE_NODE_IS_ID(pn_args[1])) {
            // second arg is a register (or should be)
            mp_uint_t op_code, op_code_hi;
            if (strcmp(op_str, "mov") == 0) {
                mp_uint_t reg_dest = get_arg_reg(emit, op_str, pn_args[0], 15);
                mp_uint_t reg_src = get_arg_reg(emit, op_str, pn_args[1], 15);
                asm_thumb_mov_reg_reg(emit->as, reg_dest, reg_src);
            } else if (strcmp(op_str, "clz") == 0) {
                op_code_hi = 0xfab0;
                op_code = 0xf080;
                mp_uint_t rd, rm;
                op_clz_rbit:
                rd = get_arg_reg(emit, op_str, pn_args[0], 15);
                rm = get_arg_reg(emit, op_str, pn_args[1], 15);
                asm_thumb_op32(emit->as, op_code_hi | rm, op_code | (rd << 8) | rm);
            } else if (strcmp(op_str, "rbit") == 0) {
                op_code_hi = 0xfa90;
                op_code = 0xf0a0;
                goto op_clz_rbit;
            } else {
                if (strcmp(op_str, "and_") == 0) {
                    op_code = ASM_THUMB_FORMAT_4_AND;
                    mp_uint_t reg_dest, reg_src;
                    op_format_4:
                    reg_dest = get_arg_reg(emit, op_str, pn_args[0], 7);
                    reg_src = get_arg_reg(emit, op_str, pn_args[1], 7);
                    asm_thumb_format_4(emit->as, op_code, reg_dest, reg_src);
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
            if (strcmp(op_str, "mov") == 0) {
                op_code = ASM_THUMB_FORMAT_3_MOV;
                mp_uint_t rlo_dest, i8_src;
                op_format_3:
                rlo_dest = get_arg_reg(emit, op_str, pn_args[0], 7);
                i8_src = get_arg_i(emit, op_str, pn_args[1], 0xff);
                asm_thumb_format_3(emit->as, op_code, rlo_dest, i8_src);
            } else if (strcmp(op_str, "cmp") == 0) {
                op_code = ASM_THUMB_FORMAT_3_CMP;
                goto op_format_3;
            } else if (strcmp(op_str, "add") == 0) {
                op_code = ASM_THUMB_FORMAT_3_ADD;
                goto op_format_3;
            } else if (strcmp(op_str, "sub") == 0) {
                op_code = ASM_THUMB_FORMAT_3_SUB;
                goto op_format_3;
            } else if (strcmp(op_str, "movw") == 0) {
                op_code = ASM_THUMB_OP_MOVW;
                mp_uint_t reg_dest;
                op_movw_movt:
                reg_dest = get_arg_reg(emit, op_str, pn_args[0], 15);
                int i_src = get_arg_i(emit, op_str, pn_args[1], 0xffff);
                asm_thumb_mov_reg_i16(emit->as, op_code, reg_dest, i_src);
            } else if (strcmp(op_str, "movt") == 0) {
                op_code = ASM_THUMB_OP_MOVT;
                goto op_movw_movt;
            } else if (strcmp(op_str, "movwt") == 0) {
                // this is a convenience instruction
                // we clear the MSB since it might be set from extracting the small int value
                mp_uint_t reg_dest = get_arg_reg(emit, op_str, pn_args[0], 15);
                int i_src = get_arg_i(emit, op_str, pn_args[1], 0xffffffff);
                asm_thumb_mov_reg_i16(emit->as, ASM_THUMB_OP_MOVW, reg_dest, i_src & 0xffff);
                asm_thumb_mov_reg_i16(emit->as, ASM_THUMB_OP_MOVT, reg_dest, (i_src >> 16) & 0x7fff);
            } else if (strcmp(op_str, "ldrex") == 0) {
                mp_uint_t r_dest = get_arg_reg(emit, op_str, pn_args[0], 15);
                mp_parse_node_t pn_base, pn_offset;
                if (get_arg_addr(emit, op_str, pn_args[1], &pn_base, &pn_offset)) {
                    mp_uint_t r_base = get_arg_reg(emit, op_str, pn_base, 15);
                    mp_uint_t i8 = get_arg_i(emit, op_str, pn_offset, 0xff) >> 2;
                    asm_thumb_op32(emit->as, 0xe850 | r_base, 0x0f00 | (r_dest << 12) | i8);
                }
            } else {
                // search table for ldr/str instructions
                for (mp_uint_t i = 0; i < MP_ARRAY_SIZE(format_9_10_op_table); i++) {
                    if (strcmp(op_str, format_9_10_op_table[i].name) == 0) {
                        op_code = format_9_10_op_table[i].op;
                        mp_parse_node_t pn_base, pn_offset;
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
                            asm_thumb_format_9_10(emit->as, op_code, rlo_dest, rlo_base, i5);
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
        if (strcmp(op_str, "add") == 0) {
            op_code = ASM_THUMB_FORMAT_2_ADD;
            mp_uint_t rlo_dest, rlo_src;
            op_format_2:
            rlo_dest = get_arg_reg(emit, op_str, pn_args[0], 7);
            rlo_src = get_arg_reg(emit, op_str, pn_args[1], 7);
            int src_b;
            if (MP_PARSE_NODE_IS_ID(pn_args[2])) {
                op_code |= ASM_THUMB_FORMAT_2_REG_OPERAND;
                src_b = get_arg_reg(emit, op_str, pn_args[2], 7);
            } else {
                op_code |= ASM_THUMB_FORMAT_2_IMM_OPERAND;
                src_b = get_arg_i(emit, op_str, pn_args[2], 0x7);
            }
            asm_thumb_format_2(emit->as, op_code, rlo_dest, rlo_src, src_b);
        } else if (strcmp(op_str, "sdiv") == 0) {
            op_code = 0xfb90; // sdiv high part
            mp_uint_t rd, rn, rm;
            op_sdiv_udiv:
            rd = get_arg_reg(emit, op_str, pn_args[0], 15);
            rn = get_arg_reg(emit, op_str, pn_args[1], 15);
            rm = get_arg_reg(emit, op_str, pn_args[2], 15);
            asm_thumb_op32(emit->as, op_code | rn, 0xf0f0 | (rd << 8) | rm);
        } else if (strcmp(op_str, "udiv") == 0) {
            op_code = 0xfbb0; // udiv high part
            goto op_sdiv_udiv;
        } else if (strcmp(op_str, "sub") == 0) {
            op_code = ASM_THUMB_FORMAT_2_SUB;
            goto op_format_2;
        } else if (strcmp(op_str, "strex") == 0) {
            mp_uint_t r_dest = get_arg_reg(emit, op_str, pn_args[0], 15);
            mp_uint_t r_src = get_arg_reg(emit, op_str, pn_args[1], 15);
            mp_parse_node_t pn_base, pn_offset;
            if (get_arg_addr(emit, op_str, pn_args[2], &pn_base, &pn_offset)) {
                mp_uint_t r_base = get_arg_reg(emit, op_str, pn_base, 15);
                mp_uint_t i8 = get_arg_i(emit, op_str, pn_offset, 0xff) >> 2;
                asm_thumb_op32(emit->as, 0xe840 | r_base, (r_src << 12) | (r_dest << 8) | i8);
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
    emit_inline_thumb_align,
    emit_inline_thumb_data,
    emit_inline_thumb_op,
};

#endif // MICROPY_EMIT_INLINE_THUMB
