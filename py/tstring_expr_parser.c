#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "py/mpconfig.h"
#include "py/misc.h"
#include "py/qstr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/parse.h"
#include "py/lexer.h"
#include "py/tstring_expr_parser.h"

#if MICROPY_PY_TSTRINGS

enum {
#define DEF_RULE(rule, comp, kind, ...) RULE_##rule,
#define DEF_RULE_NC(rule, kind, ...)
    #include "py/grammar.h"
#undef DEF_RULE
#undef DEF_RULE_NC
    RULE_const_object,
#define DEF_RULE(rule, comp, kind, ...)
#define DEF_RULE_NC(rule, kind, ...) RULE_##rule,
    #include "py/grammar.h"
#undef DEF_RULE
#undef DEF_RULE_NC
};

typedef struct _parser_t parser_t;

static mp_parse_node_t copy_parse_node(void *alloc_ctx, mp_parse_allocator_t allocator, mp_parse_node_t node) {
    if (MP_PARSE_NODE_IS_NULL(node)) {
        return MP_PARSE_NODE_NULL;
    }

    if (MP_PARSE_NODE_IS_LEAF(node)) {
        return node;
    }

    mp_parse_node_struct_t *pns = (mp_parse_node_struct_t *)node;
    size_t n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);


    if (MP_PARSE_NODE_STRUCT_KIND(pns) == RULE_const_object) {

        mp_parse_node_struct_t *new_pns = (mp_parse_node_struct_t *)allocator(alloc_ctx,
            sizeof(mp_parse_node_struct_t) + sizeof(mp_parse_node_t) * n);
        new_pns->source_line = pns->source_line;
        new_pns->kind_num_nodes = pns->kind_num_nodes;

        memcpy(new_pns->nodes, pns->nodes, sizeof(mp_parse_node_t) * n);

        return (mp_parse_node_t)new_pns;
    }

    #if MICROPY_PY_TSTRINGS
    if ((pns->kind_num_nodes & 0xFF) == MP_PARSE_NODE_TEMPLATE_STRING) {
        uint32_t hdr = pns->kind_num_nodes;
        size_t seg_cnt = MP_PARSE_TSTR_HDR_GET_SEG_CNT(hdr);
        size_t interp_cnt = MP_PARSE_TSTR_HDR_GET_INT_CNT(hdr);
        size_t total_nodes = seg_cnt + interp_cnt;

        mp_parse_node_struct_t *new_pns = (mp_parse_node_struct_t *)allocator(alloc_ctx,
            sizeof(mp_parse_node_struct_t) + sizeof(mp_parse_node_t) * total_nodes);
        new_pns->source_line = pns->source_line;
        new_pns->kind_num_nodes = pns->kind_num_nodes;

        for (size_t i = 0; i < total_nodes; i++) {
            new_pns->nodes[i] = copy_parse_node(alloc_ctx, allocator, pns->nodes[i]);
        }

        return (mp_parse_node_t)new_pns;
    }
    #endif

    if (n > 100) {
        return MP_PARSE_NODE_NULL;
    }

    mp_parse_node_struct_t *new_pns = (mp_parse_node_struct_t *)allocator(alloc_ctx,
        sizeof(mp_parse_node_struct_t) + sizeof(mp_parse_node_t) * n);

    new_pns->source_line = pns->source_line;
    new_pns->kind_num_nodes = pns->kind_num_nodes;

    for (size_t i = 0; i < n; i++) {
        mp_parse_node_t child = pns->nodes[i];

        if (!MP_PARSE_NODE_IS_NULL(child) && !MP_PARSE_NODE_IS_LEAF(child)) {
            if ((uintptr_t)child < 0x1000) {
                return MP_PARSE_NODE_NULL;
            }
        }
        new_pns->nodes[i] = copy_parse_node(alloc_ctx, allocator, child);
    }

    return (mp_parse_node_t)new_pns;
}

mp_parse_node_t parse_tstring_expression(void *alloc_ctx, mp_parse_allocator_t allocator,
    const char *expr, size_t len) {
    while (len > 0 && (expr[0] == ' ' || expr[0] == '\t' || expr[0] == '\n')) {
        expr++;
        len--;
    }
    while (len > 0 && (expr[len - 1] == ' ' || expr[len - 1] == '\t' || expr[len - 1] == '\n')) {
        len--;
    }

    if (len == 0) {
        mp_raise_msg(&mp_type_SyntaxError, MP_ERROR_TEXT("empty expression not allowed"));
    }


    mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR___lt_tstring_expr_gt_, expr, len, 0);

    if (lex == NULL) {
        qstr expr_str = qstr_from_strn(expr, len);
        return mp_parse_node_new_leaf(MP_PARSE_NODE_STRING, expr_str);
    }

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_parse_tree_t parse_tree = mp_parse(lex, MP_PARSE_EVAL_INPUT);

        mp_parse_node_t root = parse_tree.root;


        if (MP_PARSE_NODE_IS_STRUCT(root)) {
            mp_parse_node_struct_t *pns = (mp_parse_node_struct_t *)root;
            if (MP_PARSE_NODE_STRUCT_KIND(pns) == RULE_eval_input) {
                root = pns->nodes[0];
            }
        }


        mp_parse_node_t result = copy_parse_node(alloc_ctx, allocator, root);

        mp_parse_tree_clear(&parse_tree);

        nlr_pop();
        return result;
    } else {
        mp_obj_t exc = MP_OBJ_FROM_PTR(nlr.ret_val);

        if (mp_obj_is_exact_type(exc, &mp_type_SyntaxError)) {
            mp_raise_msg_varg(&mp_type_SyntaxError, MP_ERROR_TEXT("invalid syntax in template expression: %.*s"), (int)len, expr);
        } else {
            nlr_raise(exc);
        }
    }
}

#else
mp_parse_node_t parse_tstring_expression(void *alloc_ctx, mp_parse_allocator_t allocator,
    const char *expr, size_t len) {
    qstr expr_str = qstr_from_strn(expr, len);
    return mp_parse_node_new_leaf(MP_PARSE_NODE_STRING, expr_str);
}

#endif
