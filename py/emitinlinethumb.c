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

STATIC bool check_n_arg(qstr op, int n_args, int wanted_n_args) {
    if (wanted_n_args == n_args) {
        return true;
    } else {
        printf("SyntaxError: '%s' expects %d arguments'\n", qstr_str(op), wanted_n_args);
        return false;
    }
}

STATIC uint get_arg_rlo(qstr op, mp_parse_node_t *pn_args, int wanted_arg_num) {
    if (!MP_PARSE_NODE_IS_ID(pn_args[wanted_arg_num])) {
        printf("SyntaxError: '%s' expects a register in position %d\n", qstr_str(op), wanted_arg_num);
        return 0;
    }
    qstr reg_qstr = MP_PARSE_NODE_LEAF_ARG(pn_args[wanted_arg_num]);
    const char *reg_str = qstr_str(reg_qstr);
    if (!(strlen(reg_str) == 2 && reg_str[0] == 'r' && ('0' <= reg_str[1] && reg_str[1] <= '7'))) {
        printf("SyntaxError: '%s' expects a register in position %d\n", qstr_str(op), wanted_arg_num);
        return 0;
    }
    return reg_str[1] - '0';
}

STATIC int get_arg_i(qstr op, mp_parse_node_t *pn_args, int wanted_arg_num, int fit_mask) {
    if (!MP_PARSE_NODE_IS_SMALL_INT(pn_args[wanted_arg_num])) {
        printf("SyntaxError: '%s' expects an integer in position %d\n", qstr_str(op), wanted_arg_num);
        return 0;
    }
    int i = MP_PARSE_NODE_LEAF_SMALL_INT(pn_args[wanted_arg_num]);
    if ((i & (~fit_mask)) != 0) {
        printf("SyntaxError: '%s' integer 0x%x does not fit in mask 0x%x\n", qstr_str(op), i, fit_mask);
        return 0;
    }
    return i;
}

STATIC int get_arg_label(emit_inline_asm_t *emit, qstr op, mp_parse_node_t *pn_args, int wanted_arg_num) {
    if (!MP_PARSE_NODE_IS_ID(pn_args[wanted_arg_num])) {
        printf("SyntaxError: '%s' expects a label in position %d\n", qstr_str(op), wanted_arg_num);
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

    // 1 arg
    if (strcmp(qstr_str(op), "b") == 0) {
        if (!check_n_arg(op, n_args, 1)) {
            return;
        }
        int label_num = get_arg_label(emit, op, pn_args, 0);
        // TODO check that this succeeded, ie branch was within range
        asm_thumb_b_n(emit->as, label_num);
    } else if (strcmp(qstr_str(op), "bgt") == 0) {
        if (!check_n_arg(op, n_args, 1)) {
            return;
        }
        int label_num = get_arg_label(emit, op, pn_args, 0);
        // TODO check that this succeeded, ie branch was within range
        asm_thumb_bcc_n(emit->as, THUMB_CC_GT, label_num);

    // 2 args
    } else if (strcmp(qstr_str(op), "movs") == 0) {
        if (!check_n_arg(op, n_args, 2)) {
            return;
        }
        uint rlo_dest = get_arg_rlo(op, pn_args, 0);
        int i_src = get_arg_i(op, pn_args, 1, 0xff);
        asm_thumb_movs_rlo_i8(emit->as, rlo_dest, i_src);
    } else if (strcmp(qstr_str(op), "movw") == 0) {
        if (!check_n_arg(op, n_args, 2)) {
            return;
        }
        uint rlo_dest = get_arg_rlo(op, pn_args, 0); // TODO can be reg lo or hi
        int i_src = get_arg_i(op, pn_args, 1, 0xffff);
        asm_thumb_movw_reg_i16(emit->as, rlo_dest, i_src);
    } else if (strcmp(qstr_str(op), "cmp") == 0) {
        if (!check_n_arg(op, n_args, 2)) {
            return;
        }
        uint rlo = get_arg_rlo(op, pn_args, 0);
        int i8 = get_arg_i(op, pn_args, 1, 0xff);
        asm_thumb_cmp_rlo_i8(emit->as, rlo, i8);

    // 3 args
    } else if (strcmp(qstr_str(op), "subs") == 0) {
        if (!check_n_arg(op, n_args, 3)) {
            return;
        }
        uint rlo_dest = get_arg_rlo(op, pn_args, 0);
        uint rlo_src = get_arg_rlo(op, pn_args, 1);
        int i3_src = get_arg_i(op, pn_args, 2, 0x7);
        asm_thumb_subs_rlo_rlo_i3(emit->as, rlo_dest, rlo_src, i3_src);

    // unknown op
    } else {
        printf("SyntaxError: unsupported ARM Thumb instruction '%s'\n", qstr_str(op));
        return;
    }
}

const emit_inline_asm_method_table_t emit_inline_thumb_method_table = {
    emit_inline_thumb_start_pass,
    emit_inline_thumb_end_pass,
    emit_inline_thumb_count_params,
    emit_inline_thumb_label,
    emit_inline_thumb_op,
};

#endif // MICROPY_EMIT_INLINE_THUMB
