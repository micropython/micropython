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

#include "mpconfig.h"
#include "misc.h"
#include "qstr.h"
#include "lexer.h"
#include "parse.h"
#include "obj.h"
#include "emitglue.h"
#include "scope.h"
#include "runtime0.h"
#include "emit.h"
#include "asmthumb.h"

#if MICROPY_EMIT_INLINE_THUMB

typedef enum {
    PN_none = 0,
#define DEF_RULE(rule, comp, kind, ...) PN_##rule,
#include "grammar.h"
#undef DEF_RULE
    PN_maximum_number_of,
} pn_kind_t;

struct _emit_inline_asm_t {
    uint16_t pass;
    uint16_t success;
    scope_t *scope;
    mp_uint_t max_num_labels;
    qstr *label_lookup;
    asm_thumb_t *as;
};

void emit_inline_thumb_error(emit_inline_asm_t *emit, const char *fmt, ...) {
    printf("SyntaxError: ");
    emit->success = false;
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
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

STATIC void emit_inline_thumb_start_pass(emit_inline_asm_t *emit, pass_kind_t pass, scope_t *scope) {
    emit->pass = pass;
    emit->success = true;
    emit->scope = scope;
    asm_thumb_start_pass(emit->as, pass == MP_PASS_EMIT ? ASM_THUMB_PASS_EMIT : ASM_THUMB_PASS_COMPUTE);
    asm_thumb_entry(emit->as, 0);
}

STATIC bool emit_inline_thumb_end_pass(emit_inline_asm_t *emit) {
    asm_thumb_exit(emit->as);
    asm_thumb_end_pass(emit->as);

    if (emit->pass == MP_PASS_EMIT) {
        void *f = asm_thumb_get_code(emit->as);
        mp_emit_glue_assign_native(emit->scope->raw_code, MP_CODE_NATIVE_ASM, f, asm_thumb_get_code_size(emit->as), emit->scope->num_pos_args, 0);
    }

    return emit->success;
}

STATIC mp_uint_t emit_inline_thumb_count_params(emit_inline_asm_t *emit, mp_uint_t n_params, mp_parse_node_t *pn_params) {
    if (n_params > 4) {
        emit_inline_thumb_error(emit, "can only have up to 4 parameters to inline thumb assembly\n");
        return 0;
    }
    for (mp_uint_t i = 0; i < n_params; i++) {
        if (!MP_PARSE_NODE_IS_ID(pn_params[i])) {
            emit_inline_thumb_error(emit, "parameter to inline assembler must be an identifier\n");
            return 0;
        }
        const char *p = qstr_str(MP_PARSE_NODE_LEAF_ARG(pn_params[i]));
        if (!(strlen(p) == 2 && p[0] == 'r' && p[1] == '0' + i)) {
            emit_inline_thumb_error(emit, "parameter %d to inline assembler must be r%d\n", i + 1, i);
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

STATIC mp_uint_t get_arg_reg(emit_inline_asm_t *emit, const char *op, mp_parse_node_t pn, mp_uint_t max_reg) {
    if (MP_PARSE_NODE_IS_ID(pn)) {
        qstr reg_qstr = MP_PARSE_NODE_LEAF_ARG(pn);
        const char *reg_str = qstr_str(reg_qstr);
        for (mp_uint_t i = 0; i < MP_ARRAY_SIZE(reg_name_table); i++) {
            const reg_name_t *r = &reg_name_table[i];
            if (reg_str[0] == r->name[0] && reg_str[1] == r->name[1] && reg_str[2] == r->name[2] && (reg_str[2] == '\0' || reg_str[3] == '\0')) {
                if (r->reg > max_reg) {
                    emit_inline_thumb_error(emit, "'%s' expects at most r%d\n", op, max_reg);
                    return 0;
                } else {
                    return r->reg;
                }
            }
        }
    }
    emit_inline_thumb_error(emit, "'%s' expects a register\n", op);
    return 0;
}

STATIC int get_arg_i(emit_inline_asm_t *emit, const char *op, mp_parse_node_t pn, int fit_mask) {
    if (!MP_PARSE_NODE_IS_SMALL_INT(pn)) {
        emit_inline_thumb_error(emit, "'%s' expects an integer\n", op);
        return 0;
    }
    int i = MP_PARSE_NODE_LEAF_SMALL_INT(pn);
    if ((i & (~fit_mask)) != 0) {
        emit_inline_thumb_error(emit, "'%s' integer 0x%x does not fit in mask 0x%x\n", op, i, fit_mask);
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
    emit_inline_thumb_error(emit, "'%s' expects an address of the form [a, b]\n", op);
    return false;
}

STATIC int get_arg_label(emit_inline_asm_t *emit, const char *op, mp_parse_node_t pn) {
    if (!MP_PARSE_NODE_IS_ID(pn)) {
        emit_inline_thumb_error(emit, "'%s' expects a label\n", op);
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
        emit_inline_thumb_error(emit, "label '%s' not defined\n", qstr_str(label_qstr));
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
        } else if (strcmp(op_str, "ite.ge") == 0) { // TODO correct name for this op?
            asm_thumb_op16(emit->as, ASM_THUMB_OP_ITE_GE);
        } else {
            goto unknown_op;
        }

    } else if (n_args == 1) {
        if (strcmp(op_str, "b") == 0) {
            int label_num = get_arg_label(emit, op_str, pn_args[0]);
            // TODO check that this succeeded, ie branch was within range
            asm_thumb_b_n(emit->as, label_num);
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
            // TODO check that this succeeded, ie branch was within range
            asm_thumb_bcc_n(emit->as, cc, label_num);
        } else if (strcmp(op_str, "cpsid")) {
            // TODO check pn_args[0] == i
            asm_thumb_op16(emit->as, ASM_THUMB_OP_CPSID_I);
        } else if (strcmp(op_str, "cpsie")) {
            // TODO check pn_args[0] == i
            asm_thumb_op16(emit->as, ASM_THUMB_OP_CPSIE_I);
        } else {
            goto unknown_op;
        }

    } else if (n_args == 2) {
        if (MP_PARSE_NODE_IS_ID(pn_args[1])) {
            // second arg is a register (or should be)
            mp_uint_t op_code;
            if (strcmp(op_str, "mov") == 0) {
                mp_uint_t reg_dest = get_arg_reg(emit, op_str, pn_args[0], 15);
                mp_uint_t reg_src = get_arg_reg(emit, op_str, pn_args[1], 15);
                asm_thumb_mov_reg_reg(emit->as, reg_dest, reg_src);
            } else if (strcmp(op_str, "and_") == 0) {
                op_code = ASM_THUMB_FORMAT_4_AND;
                mp_uint_t reg_dest, reg_src;
                op_format_4:
                reg_dest = get_arg_reg(emit, op_str, pn_args[0], 7);
                reg_src = get_arg_reg(emit, op_str, pn_args[1], 7);
                asm_thumb_format_4(emit->as, op_code, reg_dest, reg_src);
            // TODO probably uses less ROM if these ops are in a lookup table
            } else if (strcmp(op_str, "eor") == 0) { op_code = ASM_THUMB_FORMAT_4_EOR; goto op_format_4;
            } else if (strcmp(op_str, "lsl") == 0) { op_code = ASM_THUMB_FORMAT_4_LSL; goto op_format_4;
            } else if (strcmp(op_str, "lsr") == 0) { op_code = ASM_THUMB_FORMAT_4_LSR; goto op_format_4;
            } else if (strcmp(op_str, "asr") == 0) { op_code = ASM_THUMB_FORMAT_4_ASR; goto op_format_4;
            } else if (strcmp(op_str, "adc") == 0) { op_code = ASM_THUMB_FORMAT_4_ADC; goto op_format_4;
            } else if (strcmp(op_str, "sbc") == 0) { op_code = ASM_THUMB_FORMAT_4_SBC; goto op_format_4;
            } else if (strcmp(op_str, "ror") == 0) { op_code = ASM_THUMB_FORMAT_4_ROR; goto op_format_4;
            } else if (strcmp(op_str, "tst") == 0) { op_code = ASM_THUMB_FORMAT_4_TST; goto op_format_4;
            } else if (strcmp(op_str, "neg") == 0) { op_code = ASM_THUMB_FORMAT_4_NEG; goto op_format_4;
            } else if (strcmp(op_str, "cmp") == 0) { op_code = ASM_THUMB_FORMAT_4_CMP; goto op_format_4;
            } else if (strcmp(op_str, "cmn") == 0) { op_code = ASM_THUMB_FORMAT_4_CMN; goto op_format_4;
            } else if (strcmp(op_str, "orr") == 0) { op_code = ASM_THUMB_FORMAT_4_ORR; goto op_format_4;
            } else if (strcmp(op_str, "mul") == 0) { op_code = ASM_THUMB_FORMAT_4_MUL; goto op_format_4;
            } else if (strcmp(op_str, "bic") == 0) { op_code = ASM_THUMB_FORMAT_4_BIC; goto op_format_4;
            } else if (strcmp(op_str, "mvn") == 0) { op_code = ASM_THUMB_FORMAT_4_MVN; goto op_format_4;
            } else {
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
                mp_uint_t reg_dest = get_arg_reg(emit, op_str, pn_args[0], 15);
                int i_src = get_arg_i(emit, op_str, pn_args[1], 0xffff);
                asm_thumb_movw_reg_i16(emit->as, reg_dest, i_src);
            } else if (strcmp(op_str, "movt") == 0) {
                mp_uint_t reg_dest = get_arg_reg(emit, op_str, pn_args[0], 15);
                int i_src = get_arg_i(emit, op_str, pn_args[1], 0xffff);
                asm_thumb_movt_reg_i16(emit->as, reg_dest, i_src);
            } else if (strcmp(op_str, "movwt") == 0) {
                // this is a convenience instruction
                // we clear the MSB since it might be set from extracting the small int value
                mp_uint_t reg_dest = get_arg_reg(emit, op_str, pn_args[0], 15);
                int i_src = get_arg_i(emit, op_str, pn_args[1], 0xffffffff);
                asm_thumb_movw_reg_i16(emit->as, reg_dest, i_src & 0xffff);
                asm_thumb_movt_reg_i16(emit->as, reg_dest, (i_src >> 16) & 0x7fff);
            } else if (strcmp(op_str, "ldr") == 0) {
                op_code = ASM_THUMB_FORMAT_9_LDR | ASM_THUMB_FORMAT_9_WORD_TRANSFER;
                mp_uint_t rlo_dest, rlo_base, i5;
                mp_parse_node_t pn_base, pn_offset;
                op_format_9_10:
                rlo_dest = get_arg_reg(emit, op_str, pn_args[0], 7);
                if (get_arg_addr(emit, op_str, pn_args[1], &pn_base, &pn_offset)) {
                    rlo_base = get_arg_reg(emit, op_str, pn_base, 7);
                    if (op_code & ASM_THUMB_FORMAT_9_BYTE_TRANSFER) {
                        i5 = get_arg_i(emit, op_str, pn_offset, 0x1f);
                    } else if (op_code & ASM_THUMB_FORMAT_10_STRH) { // also catches LDRH
                        i5 = get_arg_i(emit, op_str, pn_offset, 0x3e) >> 1;
                    } else {
                        i5 = get_arg_i(emit, op_str, pn_offset, 0x7c) >> 2;
                    }
                    asm_thumb_format_9_10(emit->as, op_code, rlo_dest, rlo_base, i5);
                }
            } else if (strcmp(op_str, "ldrb") == 0) {
                op_code = ASM_THUMB_FORMAT_9_LDR | ASM_THUMB_FORMAT_9_BYTE_TRANSFER;
                goto op_format_9_10;
            } else if (strcmp(op_str, "ldrh") == 0) {
                op_code = ASM_THUMB_FORMAT_10_LDRH;
                goto op_format_9_10;
            } else if (strcmp(op_str, "str") == 0) {
                op_code = ASM_THUMB_FORMAT_9_STR | ASM_THUMB_FORMAT_9_WORD_TRANSFER;
                goto op_format_9_10;
            } else if (strcmp(op_str, "strb") == 0) {
                op_code = ASM_THUMB_FORMAT_9_STR | ASM_THUMB_FORMAT_9_BYTE_TRANSFER;
                goto op_format_9_10;
            } else if (strcmp(op_str, "strh") == 0) {
                op_code = ASM_THUMB_FORMAT_10_STRH;
                goto op_format_9_10;
            } else {
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
        } else if (strcmp(op_str, "sub") == 0) {
            op_code = ASM_THUMB_FORMAT_2_SUB;
            goto op_format_2;
        } else {
            goto unknown_op;
        }

    } else {
        goto unknown_op;
    }

    return;

unknown_op:
    emit_inline_thumb_error(emit, "unsupported Thumb instruction '%s' with %d arguments\n", op_str, n_args);
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
