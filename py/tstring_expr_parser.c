#include "py/mpconfig.h"

#if MICROPY_PY_TSTRINGS

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "py/misc.h"
#include "py/qstr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/parse.h"
#include "py/lexer.h"
#include "py/tstring_expr_parser.h"

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

#define COPY_STACK_INITIAL_ALLOC 16

typedef struct {
    mp_parse_node_struct_t *src_node;
    mp_parse_node_struct_t *dst_node;
    size_t child_idx;
    size_t num_children;
} copy_frame_t;

static inline size_t get_num_children(mp_parse_node_struct_t *pns) {
    #if MICROPY_PY_TSTRINGS
    if ((pns->kind_num_nodes & 0xFF) == MP_PARSE_NODE_TEMPLATE_STRING) {
        uint32_t hdr = pns->kind_num_nodes;
        return MP_PARSE_TSTR_HDR_GET_SEG_CNT(hdr) + MP_PARSE_TSTR_HDR_GET_INT_CNT(hdr);
    }
    #endif
    return MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
}

mp_parse_node_t copy_parse_node(void *alloc_ctx, mp_parse_allocator_t allocator, mp_parse_node_t node) {
    if (MP_PARSE_NODE_IS_NULL(node)) {
        return MP_PARSE_NODE_NULL;
    }

    if (MP_PARSE_NODE_IS_LEAF(node)) {
        return node;
    }

    mp_parse_node_struct_t *pns = (mp_parse_node_struct_t *)node;

    if (MP_PARSE_NODE_STRUCT_KIND(pns) == RULE_const_object) {
        size_t n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
        mp_parse_node_struct_t *new_pns = (mp_parse_node_struct_t *)allocator(alloc_ctx,
            sizeof(mp_parse_node_struct_t) + sizeof(mp_parse_node_t) * n);
        new_pns->source_line = pns->source_line;
        new_pns->kind_num_nodes = pns->kind_num_nodes;
        memcpy(new_pns->nodes, pns->nodes, sizeof(mp_parse_node_t) * n);
        return (mp_parse_node_t)new_pns;
    }

    copy_frame_t *stack = m_new(copy_frame_t, COPY_STACK_INITIAL_ALLOC);
    size_t stack_alloc = COPY_STACK_INITIAL_ALLOC;
    size_t stack_top = 0;

    mp_parse_node_t result = MP_PARSE_NODE_NULL;

    size_t num_children = get_num_children(pns);
    mp_parse_node_struct_t *new_pns = (mp_parse_node_struct_t *)allocator(alloc_ctx,
        sizeof(mp_parse_node_struct_t) + sizeof(mp_parse_node_t) * num_children);
    new_pns->source_line = pns->source_line;
    new_pns->kind_num_nodes = pns->kind_num_nodes;
    result = (mp_parse_node_t)new_pns;

    stack[stack_top].src_node = pns;
    stack[stack_top].dst_node = new_pns;
    stack[stack_top].child_idx = 0;
    stack[stack_top].num_children = num_children;
    stack_top++;

    while (stack_top > 0) {
        copy_frame_t *frame = &stack[stack_top - 1];

        if (frame->child_idx >= frame->num_children) {
            stack_top--;
            continue;
        }

        mp_parse_node_t child = frame->src_node->nodes[frame->child_idx];

        if (MP_PARSE_NODE_IS_NULL(child) || MP_PARSE_NODE_IS_LEAF(child)) {
            frame->dst_node->nodes[frame->child_idx] = child;
            frame->child_idx++;
        } else {
            mp_parse_node_struct_t *child_pns = (mp_parse_node_struct_t *)child;

            if (MP_PARSE_NODE_STRUCT_KIND(child_pns) == RULE_const_object) {
                size_t n = MP_PARSE_NODE_STRUCT_NUM_NODES(child_pns);
                mp_parse_node_struct_t *new_child = (mp_parse_node_struct_t *)allocator(alloc_ctx,
                    sizeof(mp_parse_node_struct_t) + sizeof(mp_parse_node_t) * n);
                new_child->source_line = child_pns->source_line;
                new_child->kind_num_nodes = child_pns->kind_num_nodes;
                memcpy(new_child->nodes, child_pns->nodes, sizeof(mp_parse_node_t) * n);
                frame->dst_node->nodes[frame->child_idx] = (mp_parse_node_t)new_child;
                frame->child_idx++;
            } else {
                size_t child_num_children = get_num_children(child_pns);
                mp_parse_node_struct_t *new_child = (mp_parse_node_struct_t *)allocator(alloc_ctx,
                    sizeof(mp_parse_node_struct_t) + sizeof(mp_parse_node_t) * child_num_children);
                new_child->source_line = child_pns->source_line;
                new_child->kind_num_nodes = child_pns->kind_num_nodes;

                frame->dst_node->nodes[frame->child_idx] = (mp_parse_node_t)new_child;
                frame->child_idx++;

                if (stack_top >= stack_alloc) {
                    stack = m_renew(copy_frame_t, stack, stack_alloc, stack_alloc * 2);
                    stack_alloc *= 2;
                }

                stack[stack_top].src_node = child_pns;
                stack[stack_top].dst_node = new_child;
                stack[stack_top].child_idx = 0;
                stack[stack_top].num_children = child_num_children;
                stack_top++;
            }
        }
    }

    m_del(copy_frame_t, stack, stack_alloc);
    return result;
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
        mp_raise_msg(&mp_type_SyntaxError, MP_ERROR_TEXT("t-string: empty expression not allowed"));
    }


    mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_tstring_expr_gt_, expr, len, 0);

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
        nlr_raise(nlr.ret_val);
    }
}

#endif
