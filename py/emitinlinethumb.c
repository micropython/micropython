#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "lexer.h"
#include "parse.h"
#include "scope.h"
#include "runtime0.h"
#include "emit.h"
#include "emitglue.h"
#include "asmthumb.h"

#if MICROPY_EMIT_INLINE_THUMB

struct _emit_inline_asm_t {
    int pass;
    scope_t *scope;
    uint max_num_labels;
    qstr *label_lookup;
    asm_thumb_t *as;
};

emit_inline_asm_t *emit_inline_thumb_new(uint max_num_labels) {
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
    emit->scope = scope;
    asm_thumb_start_pass(emit->as, pass);
    asm_thumb_entry(emit->as, 0);
}

STATIC void emit_inline_thumb_end_pass(emit_inline_asm_t *emit) {
    asm_thumb_exit(emit->as);
    asm_thumb_end_pass(emit->as);

    if (emit->pass == PASS_3) {
        void *f = asm_thumb_get_code(emit->as);
        mp_emit_glue_assign_inline_asm_code(emit->scope->unique_code_id, f, asm_thumb_get_code_size(emit->as), emit->scope->num_params);
    }
}

STATIC int emit_inline_thumb_count_params(emit_inline_asm_t *emit, int n_params, mp_parse_node_t *pn_params) {
    if (n_params > 4) {
        printf("SyntaxError: can only have up to 4 parameters to inline thumb assembly\n");
        return 0;
    }
    for (int i = 0; i < n_params; i++) {
        if (!MP_PARSE_NODE_IS_ID(pn_params[i])) {
            printf("SyntaxError: parameter to inline assembler must be an identifier\n");
            return 0;
        }
        const char *p = qstr_str(MP_PARSE_NODE_LEAF_ARG(pn_params[i]));
        if (!(strlen(p) == 2 && p[0] == 'r' && p[1] == '0' + i)) {
            printf("SyntaxError: parameter %d to inline assembler must be r%d\n", i + 1, i);
            return 0;
        }
    }
    return n_params;
}

STATIC void emit_inline_thumb_label(emit_inline_asm_t *emit, int label_num, qstr label_id) {
    assert(label_num < emit->max_num_labels);
    emit->label_lookup[label_num] = label_id;
    asm_thumb_label_assign(emit->as, label_num);
}

STATIC uint get_arg_rlo(const char *op, mp_parse_node_t *pn_args, int wanted_arg_num) {
    if (!MP_PARSE_NODE_IS_ID(pn_args[wanted_arg_num])) {
        printf("SyntaxError: '%s' expects a register in position %d\n", op, wanted_arg_num);
        return 0;
    }
    qstr reg_qstr = MP_PARSE_NODE_LEAF_ARG(pn_args[wanted_arg_num]);
    const char *reg_str = qstr_str(reg_qstr);
    if (!(strlen(reg_str) == 2 && reg_str[0] == 'r' && ('0' <= reg_str[1] && reg_str[1] <= '7'))) {
        printf("SyntaxError: '%s' expects a register in position %d\n", op, wanted_arg_num);
        return 0;
    }
    return reg_str[1] - '0';
}

STATIC int get_arg_i(const char *op, mp_parse_node_t *pn_args, int wanted_arg_num, int fit_mask) {
    if (!MP_PARSE_NODE_IS_SMALL_INT(pn_args[wanted_arg_num])) {
        printf("SyntaxError: '%s' expects an integer in position %d\n", op, wanted_arg_num);
        return 0;
    }
    int i = MP_PARSE_NODE_LEAF_SMALL_INT(pn_args[wanted_arg_num]);
    if ((i & (~fit_mask)) != 0) {
        printf("SyntaxError: '%s' integer 0x%x does not fit in mask 0x%x\n", op, i, fit_mask);
        return 0;
    }
    return i;
}

STATIC int get_arg_label(emit_inline_asm_t *emit, const char *op, mp_parse_node_t *pn_args, int wanted_arg_num) {
    if (!MP_PARSE_NODE_IS_ID(pn_args[wanted_arg_num])) {
        printf("SyntaxError: '%s' expects a label in position %d\n", op, wanted_arg_num);
        return 0;
    }
    qstr label_qstr = MP_PARSE_NODE_LEAF_ARG(pn_args[wanted_arg_num]);
    for (int i = 0; i < emit->max_num_labels; i++) {
        if (emit->label_lookup[i] == label_qstr) {
            return i;
        }
    }
    // only need to have the labels on the last pass
    if (emit->pass == PASS_3) {
        printf("SyntaxError: label '%s' not defined\n", qstr_str(label_qstr));
    }
    return 0;
}

typedef struct _cc_name_t { byte cc; byte name[2]; } cc_name_t;
STATIC const cc_name_t cc_name_table[] = {
    {THUMB_CC_EQ, "eq"},
    {THUMB_CC_NE, "ne"},
    {THUMB_CC_CS, "cs"},
    {THUMB_CC_CC, "cc"},
    {THUMB_CC_MI, "mi"},
    {THUMB_CC_PL, "pl"},
    {THUMB_CC_VS, "vs"},
    {THUMB_CC_VC, "vc"},
    {THUMB_CC_HI, "hi"},
    {THUMB_CC_LS, "ls"},
    {THUMB_CC_GE, "ge"},
    {THUMB_CC_LT, "lt"},
    {THUMB_CC_GT, "gt"},
    {THUMB_CC_LE, "le"},
};

STATIC void emit_inline_thumb_op(emit_inline_asm_t *emit, qstr op, int n_args, mp_parse_node_t *pn_args) {
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
    uint op_len = strlen(op_str);

    if (n_args == 0) {
        if (strcmp(op_str, "ite.ge") == 0) { // TODO correct name for this op?
            asm_thumb_ite_ge(emit->as);
        } else {
            goto unknown_op;
        }

    } else if (n_args == 1) {
        if (strcmp(op_str, "b") == 0) {
            int label_num = get_arg_label(emit, op_str, pn_args, 0);
            // TODO check that this succeeded, ie branch was within range
            asm_thumb_b_n(emit->as, label_num);
        } else if (op_str[0] == 'b' && op_len == 3) {
            uint cc = -1;
            for (uint i = 0; i < (sizeof cc_name_table) / (sizeof cc_name_table[0]); i++) {
                if (op_str[1] == cc_name_table[i].name[0] && op_str[2] == cc_name_table[i].name[1]) {
                    cc = cc_name_table[i].cc;
                }
            }
            if (cc == -1) {
                goto unknown_op;
            }
            int label_num = get_arg_label(emit, op_str, pn_args, 0);
            // TODO check that this succeeded, ie branch was within range
            asm_thumb_bcc_n(emit->as, cc, label_num);
        } else {
            goto unknown_op;
        }

    } else if (n_args == 2) {
        if (strcmp(op_str, "mov") == 0) {
            uint rlo_dest = get_arg_rlo(op_str, pn_args, 0);
            uint rlo_src = get_arg_rlo(op_str, pn_args, 1);
            asm_thumb_mov_reg_reg(emit->as, rlo_dest, rlo_src);
        } else if (strcmp(op_str, "movs") == 0) {
            uint rlo_dest = get_arg_rlo(op_str, pn_args, 0);
            int i_src = get_arg_i(op_str, pn_args, 1, 0xff);
            asm_thumb_movs_rlo_i8(emit->as, rlo_dest, i_src);
        } else if (strcmp(op_str, "movw") == 0) {
            uint rlo_dest = get_arg_rlo(op_str, pn_args, 0); // TODO can be reg lo or hi
            int i_src = get_arg_i(op_str, pn_args, 1, 0xffff);
            asm_thumb_movw_reg_i16(emit->as, rlo_dest, i_src);
        } else if (strcmp(op_str, "movt") == 0) {
            uint rlo_dest = get_arg_rlo(op_str, pn_args, 0); // TODO can be reg lo or hi
            int i_src = get_arg_i(op_str, pn_args, 1, 0xffff);
            asm_thumb_movt_reg_i16(emit->as, rlo_dest, i_src);
        } else if (strcmp(op_str, "cmp") == 0) {
            uint rlo = get_arg_rlo(op_str, pn_args, 0);
            int i8 = get_arg_i(op_str, pn_args, 1, 0xff);
            asm_thumb_cmp_rlo_i8(emit->as, rlo, i8);
        } else {
            goto unknown_op;
        }

    } else if (n_args == 3) {
        if (strcmp(op_str, "add") == 0) {
            uint rlo_dest = get_arg_rlo(op_str, pn_args, 0);
            uint rlo_src_a = get_arg_rlo(op_str, pn_args, 1);
            uint rlo_src_b = get_arg_rlo(op_str, pn_args, 2);
            asm_thumb_add_reg_reg_reg(emit->as, rlo_dest, rlo_src_a, rlo_src_b);
        } else if (strcmp(op_str, "subs") == 0) {
            uint rlo_dest = get_arg_rlo(op_str, pn_args, 0);
            uint rlo_src = get_arg_rlo(op_str, pn_args, 1);
            int i3_src = get_arg_i(op_str, pn_args, 2, 0x7);
            asm_thumb_subs_rlo_rlo_i3(emit->as, rlo_dest, rlo_src, i3_src);
        } else {
            goto unknown_op;
        }

    } else {
        goto unknown_op;
    }

    return;

unknown_op:
    printf("SyntaxError: unsupported ARM Thumb instruction '%s' with %d arguments\n", op_str, n_args);
}

const emit_inline_asm_method_table_t emit_inline_thumb_method_table = {
    emit_inline_thumb_start_pass,
    emit_inline_thumb_end_pass,
    emit_inline_thumb_count_params,
    emit_inline_thumb_label,
    emit_inline_thumb_op,
};

#endif // MICROPY_EMIT_INLINE_THUMB
