/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Damien P. George
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
#include <assert.h>

#include "py/emit.h"
#include "py/asmaarch64.h"

#if MICROPY_EMIT_INLINE_AARCH64

typedef enum {
// define rules with a compile function
#define DEF_RULE(rule, comp, kind, ...) PN_##rule,
#define DEF_RULE_NC(rule, kind, ...)
    #include "py/grammar.h"
#undef DEF_RULE
#undef DEF_RULE_NC
    PN_const_object,
// define rules without a compile function
#define DEF_RULE(rule, comp, kind, ...)
#define DEF_RULE_NC(rule, kind, ...) PN_##rule,
    #include "py/grammar.h"
#undef DEF_RULE
#undef DEF_RULE_NC
} pn_kind_t;

struct _emit_inline_asm_t {
    asm_aarch64_t as;
    uint16_t pass;
    mp_obj_t *error_slot;
    mp_uint_t max_num_labels;
    qstr *label_lookup;
};

static void emit_inline_aarch64_error_msg(emit_inline_asm_t *emit, mp_rom_error_text_t msg) {
    *emit->error_slot = mp_obj_new_exception_msg(&mp_type_SyntaxError, msg);
}

static void emit_inline_aarch64_error_exc(emit_inline_asm_t *emit, mp_obj_t exc) {
    *emit->error_slot = exc;
}

emit_inline_asm_t *emit_inline_aarch64_new(mp_uint_t max_num_labels) {
    emit_inline_asm_t *emit = m_new_obj(emit_inline_asm_t);
    memset(&emit->as, 0, sizeof(emit->as));
    mp_asm_base_init(&emit->as.base, max_num_labels);
    emit->max_num_labels = max_num_labels;
    emit->label_lookup = m_new(qstr, max_num_labels);
    return emit;
}

void emit_inline_aarch64_free(emit_inline_asm_t *emit) {
    m_del(qstr, emit->label_lookup, emit->max_num_labels);
    mp_asm_base_deinit(&emit->as.base, false);
    m_del_obj(emit_inline_asm_t, emit);
}

static void emit_inline_aarch64_start_pass(emit_inline_asm_t *emit, pass_kind_t pass, mp_obj_t *error_slot) {
    emit->pass = pass;
    emit->error_slot = error_slot;
    if (emit->pass == MP_PASS_CODE_SIZE) {
        memset(emit->label_lookup, 0, emit->max_num_labels * sizeof(qstr));
    }
    mp_asm_base_start_pass(&emit->as.base, pass == MP_PASS_EMIT ? MP_ASM_PASS_EMIT : MP_ASM_PASS_COMPUTE);
    asm_aarch64_entry(&emit->as, 0);
}

static void emit_inline_aarch64_end_pass(emit_inline_asm_t *emit, mp_uint_t type_sig) {
    asm_aarch64_exit(&emit->as);
    asm_aarch64_end_pass(&emit->as);
}

static mp_uint_t emit_inline_aarch64_count_params(emit_inline_asm_t *emit, mp_uint_t n_params, mp_parse_node_t *pn_params) {
    if (n_params > 4) {
        emit_inline_aarch64_error_msg(emit, MP_ERROR_TEXT("can only have up to 4 parameters to AArch64 assembly"));
        return 0;
    }
    for (mp_uint_t i = 0; i < n_params; i++) {
        if (!MP_PARSE_NODE_IS_ID(pn_params[i])) {
            emit_inline_aarch64_error_msg(emit, MP_ERROR_TEXT("parameters must be registers in sequence x0 to x3"));
            return 0;
        }
        const char *p = qstr_str(MP_PARSE_NODE_LEAF_ARG(pn_params[i]));
        if (!(strlen(p) == 2 && p[0] == 'x' && (mp_uint_t)p[1] == '0' + i)) {
            emit_inline_aarch64_error_msg(emit, MP_ERROR_TEXT("parameters must be registers in sequence x0 to x3"));
            return 0;
        }
    }
    return n_params;
}

static bool emit_inline_aarch64_label(emit_inline_asm_t *emit, mp_uint_t label_num, qstr label_id) {
    assert(label_num < emit->max_num_labels);
    if (emit->pass == MP_PASS_CODE_SIZE) {
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

typedef struct _reg_name_t {
    byte reg;
    byte name[3];
} reg_name_t;

static const reg_name_t reg_name_table[] = {
    {0,  {'x', '0' }},
    {1,  {'x', '1' }},
    {2,  {'x', '2' }},
    {3,  {'x', '3' }},
    {4,  {'x', '4' }},
    {5,  {'x', '5' }},
    {6,  {'x', '6' }},
    {7,  {'x', '7' }},
    {8,  {'x', '8' }},
    {9,  {'x', '9' }},
    {10, {'x', '1', '0'}},
    {11, {'x', '1', '1'}},
    {12, {'x', '1', '2'}},
    {13, {'x', '1', '3'}},
    {14, {'x', '1', '4'}},
    {15, {'x', '1', '5'}},
    {16, {'x', '1', '6'}},
    {17, {'x', '1', '7'}},
    {18, {'x', '1', '8'}},
    {19, {'x', '1', '9'}},
    {20, {'x', '2', '0'}},
    {21, {'x', '2', '1'}},
    {22, {'x', '2', '2'}},
    {23, {'x', '2', '3'}},
    {24, {'x', '2', '4'}},
    {25, {'x', '2', '5'}},
    {26, {'x', '2', '6'}},
    {27, {'x', '2', '7'}},
    {28, {'x', '2', '8'}},
    {29, {'x', '2', '9'}},
    {30, {'x', '3', '0'}},
    {31, {'s', 'p' }},
    {30, {'l', 'r' }},
};

static const char *get_arg_str(mp_parse_node_t pn) {
    if (MP_PARSE_NODE_IS_ID(pn)) {
        qstr qst = MP_PARSE_NODE_LEAF_ARG(pn);
        return qstr_str(qst);
    } else {
        return "";
    }
}

static mp_uint_t get_arg_reg(emit_inline_asm_t *emit, const char *op, mp_parse_node_t pn, mp_uint_t max_reg) {
    const char *reg_str = get_arg_str(pn);
    for (mp_uint_t i = 0; i < MP_ARRAY_SIZE(reg_name_table); i++) {
        const reg_name_t *r = &reg_name_table[i];
        if (reg_str[0] == r->name[0]
            && reg_str[1] == r->name[1]
            && reg_str[2] == r->name[2]
            && (reg_str[2] == '\0' || reg_str[3] == '\0')) {
            if (r->reg > max_reg) {
                emit_inline_aarch64_error_exc(emit,
                    mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
                        MP_ERROR_TEXT("'%s' expects at most x%d"), op, max_reg));
                return 0;
            } else {
                return r->reg;
            }
        }
    }
    emit_inline_aarch64_error_exc(emit,
        mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
            MP_ERROR_TEXT("'%s' expects a register"), op));
    return 0;
}

static int get_arg_i(emit_inline_asm_t *emit, const char *op, mp_parse_node_t pn, int min_val, int max_val) {
    mp_obj_t o;
    if (!mp_parse_node_get_int_maybe(pn, &o)) {
        emit_inline_aarch64_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, MP_ERROR_TEXT("'%s' expects an integer"), op));
        return 0;
    }
    mp_int_t i = mp_obj_get_int_truncated(o);
    if (i < min_val || i > max_val) {
        emit_inline_aarch64_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, MP_ERROR_TEXT("'%s' integer %d doesn't fit in range %d..%d"), op, (int)i, min_val, max_val));
        return 0;
    }
    return (int)i;
}

static bool get_arg_addr(emit_inline_asm_t *emit, const char *op, mp_parse_node_t pn, mp_parse_node_t *pn_base, mp_parse_node_t *pn_offset) {
    if (!MP_PARSE_NODE_IS_STRUCT_KIND(pn, PN_atom_bracket)) {
        goto bad_arg;
    }
    mp_parse_node_struct_t *pns = (mp_parse_node_struct_t *)pn;
    if (!MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_testlist_comp)) {
        goto bad_arg;
    }
    pns = (mp_parse_node_struct_t *)pns->nodes[0];
    if (MP_PARSE_NODE_STRUCT_NUM_NODES(pns) != 2) {
        goto bad_arg;
    }
    *pn_base = pns->nodes[0];
    *pn_offset = pns->nodes[1];
    return true;

bad_arg:
    emit_inline_aarch64_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, MP_ERROR_TEXT("'%s' expects an address of the form [a, b]"), op));
    return false;
}

static int get_arg_label(emit_inline_asm_t *emit, const char *op, mp_parse_node_t pn) {
    if (!MP_PARSE_NODE_IS_ID(pn)) {
        emit_inline_aarch64_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, MP_ERROR_TEXT("'%s' expects a label"), op));
        return 0;
    }
    qstr label_qstr = MP_PARSE_NODE_LEAF_ARG(pn);
    for (uint i = 0; i < emit->max_num_labels; i++) {
        if (emit->label_lookup[i] == label_qstr) {
            return i;
        }
    }
    if (emit->pass == MP_PASS_EMIT) {
        emit_inline_aarch64_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, MP_ERROR_TEXT("label '%q' not defined"), label_qstr));
    }
    return 0;
}

static uint64_t get_arg_reglist(emit_inline_asm_t *emit, const char *op, mp_parse_node_t pn) {
    if (!MP_PARSE_NODE_IS_STRUCT_KIND(pn, PN_atom_brace)) {
        goto bad_arg;
    }

    mp_parse_node_struct_t *pns = (mp_parse_node_struct_t *)pn;
    assert(MP_PARSE_NODE_STRUCT_NUM_NODES(pns) == 1);
    pn = pns->nodes[0];

    uint64_t reglist = 0;

    if (MP_PARSE_NODE_IS_ID(pn)) {
        reglist |= 1ULL << get_arg_reg(emit, op, pn, 31);
    } else if (MP_PARSE_NODE_IS_STRUCT(pn)) {
        pns = (mp_parse_node_struct_t *)pn;
        if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_dictorsetmaker) {
            assert(MP_PARSE_NODE_IS_STRUCT(pns->nodes[1]));
            mp_parse_node_struct_t *pns1 = (mp_parse_node_struct_t *)pns->nodes[1];
            if (MP_PARSE_NODE_STRUCT_KIND(pns1) == PN_dictorsetmaker_list) {
                reglist |= 1ULL << get_arg_reg(emit, op, pns->nodes[0], 31);
                mp_parse_node_t *nodes;
                int n = mp_parse_node_extract_list(&pns1->nodes[0], PN_dictorsetmaker_list2, &nodes);
                for (int i = 0; i < n; i++) {
                    reglist |= 1ULL << get_arg_reg(emit, op, nodes[i], 31);
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
    emit_inline_aarch64_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, MP_ERROR_TEXT("'%s' expects {x0, x1, ...}"), op));
    return 0;
}

typedef struct _cc_name_t {
    byte cc;
    byte name[2];
} cc_name_t;

static const cc_name_t cc_name_table[] = {
    { ASM_AARCH64_CC_EQ, { 'e', 'q' }},
    { ASM_AARCH64_CC_NE, { 'n', 'e' }},
    { ASM_AARCH64_CC_CS, { 'c', 's' }},
    { ASM_AARCH64_CC_CC, { 'c', 'c' }},
    { ASM_AARCH64_CC_MI, { 'm', 'i' }},
    { ASM_AARCH64_CC_PL, { 'p', 'l' }},
    { ASM_AARCH64_CC_VS, { 'v', 's' }},
    { ASM_AARCH64_CC_VC, { 'v', 'c' }},
    { ASM_AARCH64_CC_HI, { 'h', 'i' }},
    { ASM_AARCH64_CC_LS, { 'l', 's' }},
    { ASM_AARCH64_CC_GE, { 'g', 'e' }},
    { ASM_AARCH64_CC_LT, { 'l', 't' }},
    { ASM_AARCH64_CC_GT, { 'g', 't' }},
    { ASM_AARCH64_CC_LE, { 'l', 'e' }},
};

static void emit_inline_aarch64_op(emit_inline_asm_t *emit, qstr op, mp_uint_t n_args, mp_parse_node_t *pn_args) {
    size_t op_len;
    const char *op_str = (const char *)qstr_data(op, &op_len);

    if (n_args == 0) {
        if (op == MP_QSTR_nop) {
            asm_aarch64_t *as = &emit->as;
            uint8_t *c = mp_asm_base_get_cur_to_write_bytes(&as->base, 4);
            if (c != NULL) {
                *(uint32_t *)c = 0xd503201f;
            }
        } else if (op == MP_QSTR_wfi) {
            asm_aarch64_t *as = &emit->as;
            uint8_t *c = mp_asm_base_get_cur_to_write_bytes(&as->base, 4);
            if (c != NULL) {
                *(uint32_t *)c = 0xd500409f;
            }
        } else if (op == MP_QSTR_bkpt) {
            asm_aarch64_bkpt(&emit->as);
        } else {
            goto unknown_op;
        }
    } else if (n_args == 1) {
        if (op == MP_QSTR_b) {
            int label_num = get_arg_label(emit, op_str, pn_args[0]);
            asm_aarch64_b_label(&emit->as, label_num);
        } else if (op == MP_QSTR_bl) {
            int label_num = get_arg_label(emit, op_str, pn_args[0]);
            asm_aarch64_bl_label(&emit->as, label_num);
        } else if (op == MP_QSTR_ret) {
            mp_uint_t r = get_arg_reg(emit, op_str, pn_args[0], 30);
            asm_aarch64_br_reg(&emit->as, r);
        } else if (op == MP_QSTR_push) {
            uint64_t reglist = get_arg_reglist(emit, op_str, pn_args[0]);
            asm_aarch64_push(&emit->as, reglist);
        } else if (op == MP_QSTR_pop) {
            uint64_t reglist = get_arg_reglist(emit, op_str, pn_args[0]);
            asm_aarch64_pop(&emit->as, reglist);
        } else if (op_str[0] == 'b' && op_len == 3) {
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
            asm_aarch64_bcc_label(&emit->as, cc, label_num);
        } else {
            goto unknown_op;
        }
    } else if (n_args == 2) {
        if (MP_PARSE_NODE_IS_ID(pn_args[1])) {
            if (op == MP_QSTR_mov) {
                mp_uint_t reg_dest = get_arg_reg(emit, op_str, pn_args[0], 30);
                mp_uint_t reg_src = get_arg_reg(emit, op_str, pn_args[1], 30);
                asm_aarch64_mov_reg_reg(&emit->as, reg_dest, reg_src);
            } else if (op == MP_QSTR_cmp) {
                mp_uint_t reg_dest = get_arg_reg(emit, op_str, pn_args[0], 30);
                mp_uint_t reg_src = get_arg_reg(emit, op_str, pn_args[1], 30);
                asm_aarch64_cmp_reg_reg(&emit->as, reg_dest, reg_src);
            } else if (op == MP_QSTR_mvn) {
                mp_uint_t rd = get_arg_reg(emit, op_str, pn_args[0], 30);
                mp_uint_t rm = get_arg_reg(emit, op_str, pn_args[1], 30);
                asm_aarch64_mvn_reg_reg(&emit->as, rd, rm);
            } else {
                goto unknown_op;
            }
        } else {
            if (op == MP_QSTR_mov) {
                mp_uint_t reg_dest = get_arg_reg(emit, op_str, pn_args[0], 30);
                int imm = get_arg_i(emit, op_str, pn_args[1], -0x80000000, 0x7fffffff);
                asm_aarch64_mov_reg_i64_optimised(&emit->as, reg_dest, imm);
            } else if (op == MP_QSTR_cmp) {
                mp_uint_t reg_dest = get_arg_reg(emit, op_str, pn_args[0], 30);
                int imm = get_arg_i(emit, op_str, pn_args[1], 0, 0xfff);
                asm_aarch64_cmp_reg_i8(&emit->as, reg_dest, imm);
            } else if (op == MP_QSTR_ldr) {
                mp_uint_t r_dest = get_arg_reg(emit, op_str, pn_args[0], 30);
                mp_parse_node_t pn_base, pn_offset;
                if (get_arg_addr(emit, op_str, pn_args[1], &pn_base, &pn_offset)) {
                    mp_uint_t r_base = get_arg_reg(emit, op_str, pn_base, 30);
                    mp_uint_t offset = get_arg_i(emit, op_str, pn_offset, 0, 0x7fff);
                    asm_aarch64_ldr_reg_reg_offset(&emit->as, r_dest, r_base, offset);
                }
            } else if (op == MP_QSTR_ldrb) {
                mp_uint_t r_dest = get_arg_reg(emit, op_str, pn_args[0], 30);
                mp_parse_node_t pn_base, pn_offset;
                if (get_arg_addr(emit, op_str, pn_args[1], &pn_base, &pn_offset)) {
                    mp_uint_t r_base = get_arg_reg(emit, op_str, pn_base, 30);
                    mp_uint_t offset = get_arg_i(emit, op_str, pn_offset, 0, 0xfff);
                    asm_aarch64_ldrb_reg_reg_offset(&emit->as, r_dest, r_base, offset);
                }
            } else if (op == MP_QSTR_ldrh) {
                mp_uint_t r_dest = get_arg_reg(emit, op_str, pn_args[0], 30);
                mp_parse_node_t pn_base, pn_offset;
                if (get_arg_addr(emit, op_str, pn_args[1], &pn_base, &pn_offset)) {
                    mp_uint_t r_base = get_arg_reg(emit, op_str, pn_base, 30);
                    mp_uint_t offset = get_arg_i(emit, op_str, pn_offset, 0, 0xfff);
                    asm_aarch64_ldrh_reg_reg_offset(&emit->as, r_dest, r_base, offset);
                }
            } else if (op == MP_QSTR_str) {
                mp_uint_t r_dest = get_arg_reg(emit, op_str, pn_args[0], 30);
                mp_parse_node_t pn_base, pn_offset;
                if (get_arg_addr(emit, op_str, pn_args[1], &pn_base, &pn_offset)) {
                    mp_uint_t r_base = get_arg_reg(emit, op_str, pn_base, 30);
                    mp_uint_t offset = get_arg_i(emit, op_str, pn_offset, 0, 0x7fff);
                    asm_aarch64_str_reg_reg_offset(&emit->as, r_dest, r_base, offset);
                }
            } else if (op == MP_QSTR_strb) {
                mp_uint_t r_dest = get_arg_reg(emit, op_str, pn_args[0], 30);
                mp_parse_node_t pn_base, pn_offset;
                if (get_arg_addr(emit, op_str, pn_args[1], &pn_base, &pn_offset)) {
                    mp_uint_t r_base = get_arg_reg(emit, op_str, pn_base, 30);
                    mp_uint_t offset = get_arg_i(emit, op_str, pn_offset, 0, 0xfff);
                    asm_aarch64_strb_reg_reg_offset(&emit->as, r_dest, r_base, offset);
                }
            } else if (op == MP_QSTR_strh) {
                mp_uint_t r_dest = get_arg_reg(emit, op_str, pn_args[0], 30);
                mp_parse_node_t pn_base, pn_offset;
                if (get_arg_addr(emit, op_str, pn_args[1], &pn_base, &pn_offset)) {
                    mp_uint_t r_base = get_arg_reg(emit, op_str, pn_base, 30);
                    mp_uint_t offset = get_arg_i(emit, op_str, pn_offset, 0, 0xfff);
                    asm_aarch64_strh_reg_reg_offset(&emit->as, r_dest, r_base, offset);
                }
            } else {
                goto unknown_op;
            }
        }
    } else if (n_args == 3) {
        if (op == MP_QSTR_add) {
            mp_uint_t rd = get_arg_reg(emit, op_str, pn_args[0], 30);
            mp_uint_t rn = get_arg_reg(emit, op_str, pn_args[1], 30);
            if (MP_PARSE_NODE_IS_ID(pn_args[2])) {
                mp_uint_t rm = get_arg_reg(emit, op_str, pn_args[2], 30);
                asm_aarch64_add_reg_reg_reg(&emit->as, rd, rn, rm);
            } else {
                int imm = get_arg_i(emit, op_str, pn_args[2], 0, 0xfff);
                asm_aarch64_add_reg_reg_imm(&emit->as, rd, rn, imm);
            }
        } else if (op == MP_QSTR_sub) {
            mp_uint_t rd = get_arg_reg(emit, op_str, pn_args[0], 30);
            mp_uint_t rn = get_arg_reg(emit, op_str, pn_args[1], 30);
            if (MP_PARSE_NODE_IS_ID(pn_args[2])) {
                mp_uint_t rm = get_arg_reg(emit, op_str, pn_args[2], 30);
                asm_aarch64_sub_reg_reg_reg(&emit->as, rd, rn, rm);
            } else {
                int imm = get_arg_i(emit, op_str, pn_args[2], 0, 0xfff);
                asm_aarch64_sub_reg_reg_imm(&emit->as, rd, rn, imm);
            }
        } else if (op == MP_QSTR_mul) {
            mp_uint_t rd = get_arg_reg(emit, op_str, pn_args[0], 30);
            mp_uint_t rn = get_arg_reg(emit, op_str, pn_args[1], 30);
            mp_uint_t rm = get_arg_reg(emit, op_str, pn_args[2], 30);
            asm_aarch64_mul_reg_reg_reg(&emit->as, rd, rn, rm);
        } else if (op == MP_QSTR_and || op == MP_QSTR_and_) {
            mp_uint_t rd = get_arg_reg(emit, op_str, pn_args[0], 30);
            mp_uint_t rn = get_arg_reg(emit, op_str, pn_args[1], 30);
            if (MP_PARSE_NODE_IS_ID(pn_args[2])) {
                mp_uint_t rm = get_arg_reg(emit, op_str, pn_args[2], 30);
                asm_aarch64_and_reg_reg_reg(&emit->as, rd, rn, rm);
            } else {
                int imm = get_arg_i(emit, op_str, pn_args[2], -0x80000000, 0x7fffffff);
                asm_aarch64_mov_reg_i64_optimised(&emit->as, ASM_AARCH64_REG_X9, imm);
                asm_aarch64_and_reg_reg_reg(&emit->as, rd, rn, ASM_AARCH64_REG_X9);
            }
        } else if (op == MP_QSTR_eor) {
            mp_uint_t rd = get_arg_reg(emit, op_str, pn_args[0], 30);
            mp_uint_t rn = get_arg_reg(emit, op_str, pn_args[1], 30);
            if (MP_PARSE_NODE_IS_ID(pn_args[2])) {
                mp_uint_t rm = get_arg_reg(emit, op_str, pn_args[2], 30);
                asm_aarch64_eor_reg_reg_reg(&emit->as, rd, rn, rm);
            } else {
                int imm = get_arg_i(emit, op_str, pn_args[2], -0x80000000, 0x7fffffff);
                asm_aarch64_mov_reg_i64_optimised(&emit->as, ASM_AARCH64_REG_X9, imm);
                asm_aarch64_eor_reg_reg_reg(&emit->as, rd, rn, ASM_AARCH64_REG_X9);
            }
        } else if (op == MP_QSTR_orr) {
            mp_uint_t rd = get_arg_reg(emit, op_str, pn_args[0], 30);
            mp_uint_t rn = get_arg_reg(emit, op_str, pn_args[1], 30);
            if (MP_PARSE_NODE_IS_ID(pn_args[2])) {
                mp_uint_t rm = get_arg_reg(emit, op_str, pn_args[2], 30);
                asm_aarch64_orr_reg_reg_reg(&emit->as, rd, rn, rm);
            } else {
                int imm = get_arg_i(emit, op_str, pn_args[2], -0x80000000, 0x7fffffff);
                asm_aarch64_mov_reg_i64_optimised(&emit->as, ASM_AARCH64_REG_X9, imm);
                asm_aarch64_orr_reg_reg_reg(&emit->as, rd, rn, ASM_AARCH64_REG_X9);
            }
        } else if (op == MP_QSTR_lsl) {
            mp_uint_t rd = get_arg_reg(emit, op_str, pn_args[0], 30);
            mp_uint_t rn = get_arg_reg(emit, op_str, pn_args[1], 30);
            if (MP_PARSE_NODE_IS_ID(pn_args[2])) {
                mp_uint_t rm = get_arg_reg(emit, op_str, pn_args[2], 30);
                asm_aarch64_lsl_reg_reg_reg(&emit->as, rd, rn, rm);
            } else {
                int imm = get_arg_i(emit, op_str, pn_args[2], 0, 63);
                asm_aarch64_lsl_reg_reg_imm(&emit->as, rd, rn, imm);
            }
        } else if (op == MP_QSTR_lsr) {
            mp_uint_t rd = get_arg_reg(emit, op_str, pn_args[0], 30);
            mp_uint_t rn = get_arg_reg(emit, op_str, pn_args[1], 30);
            if (MP_PARSE_NODE_IS_ID(pn_args[2])) {
                mp_uint_t rm = get_arg_reg(emit, op_str, pn_args[2], 30);
                asm_aarch64_lsr_reg_reg_reg(&emit->as, rd, rn, rm);
            } else {
                int imm = get_arg_i(emit, op_str, pn_args[2], 0, 63);
                asm_aarch64_lsr_reg_reg_imm(&emit->as, rd, rn, imm);
            }
        } else if (op == MP_QSTR_asr) {
            mp_uint_t rd = get_arg_reg(emit, op_str, pn_args[0], 30);
            mp_uint_t rn = get_arg_reg(emit, op_str, pn_args[1], 30);
            if (MP_PARSE_NODE_IS_ID(pn_args[2])) {
                mp_uint_t rm = get_arg_reg(emit, op_str, pn_args[2], 30);
                asm_aarch64_asr_reg_reg_reg(&emit->as, rd, rn, rm);
            } else {
                int imm = get_arg_i(emit, op_str, pn_args[2], 0, 63);
                asm_aarch64_asr_reg_reg_imm(&emit->as, rd, rn, imm);
            }
        } else {
            goto unknown_op;
        }
    } else {
        goto unknown_op;
    }

    return;

unknown_op:
    emit_inline_aarch64_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, MP_ERROR_TEXT("unsupported AArch64 instruction '%s' with %d arguments"), op_str, n_args));
    return;
}

const emit_inline_asm_method_table_t emit_inline_aarch64_method_table = {
    #if MICROPY_DYNAMIC_COMPILER
    emit_inline_aarch64_new,
    emit_inline_aarch64_free,
    #endif

    emit_inline_aarch64_start_pass,
    emit_inline_aarch64_end_pass,
    emit_inline_aarch64_count_params,
    emit_inline_aarch64_label,
    emit_inline_aarch64_op,
};

#endif // MICROPY_EMIT_INLINE_AARCH64
