// Implementation of t-string expression parser
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

// Need grammar rule constants
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

// Forward declaration
typedef struct _parser_t parser_t;

// Helper function to copy a parse node tree using the parent allocator
static mp_parse_node_t copy_parse_node(void *alloc_ctx, mp_parse_allocator_t allocator, mp_parse_node_t node) {
    if (MP_PARSE_NODE_IS_NULL(node)) {
        return MP_PARSE_NODE_NULL;
    }

    if (MP_PARSE_NODE_IS_LEAF(node)) {
        // Leaf nodes can be used directly
        return node;
    }

    // It's a struct node - need to deep copy it
    mp_parse_node_struct_t *pns = (mp_parse_node_struct_t *)node;
    size_t n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);


    // Check if this is a const_object node - they have a special structure
    if (MP_PARSE_NODE_STRUCT_KIND(pns) == RULE_const_object) {

        // Allocate new const_object node
        mp_parse_node_struct_t *new_pns = (mp_parse_node_struct_t *)allocator(alloc_ctx,
            sizeof(mp_parse_node_struct_t) + sizeof(mp_parse_node_t) * n);
        new_pns->source_line = pns->source_line;
        new_pns->kind_num_nodes = pns->kind_num_nodes;

        // Copy the mp_obj_t values directly
        memcpy(new_pns->nodes, pns->nodes, sizeof(mp_parse_node_t) * n);

        return (mp_parse_node_t)new_pns;
    }

    // Check if this is a template string node - they have a special header format
    #if MICROPY_PY_TSTRINGS
    if ((pns->kind_num_nodes & 0xFF) == MP_PARSE_NODE_TEMPLATE_STRING) {
        // Template string nodes use a special header format
        uint32_t hdr = pns->kind_num_nodes;
        size_t seg_cnt = TSTR_HDR_GET_SEG_CNT(hdr);
        size_t interp_cnt = TSTR_HDR_GET_INT_CNT(hdr);
        size_t total_nodes = seg_cnt + interp_cnt;

        // Allocate new template string node
        mp_parse_node_struct_t *new_pns = (mp_parse_node_struct_t *)allocator(alloc_ctx,
            sizeof(mp_parse_node_struct_t) + sizeof(mp_parse_node_t) * total_nodes);
        new_pns->source_line = pns->source_line;
        new_pns->kind_num_nodes = pns->kind_num_nodes;

        // Recursively copy all child nodes
        for (size_t i = 0; i < total_nodes; i++) {
            new_pns->nodes[i] = copy_parse_node(alloc_ctx, allocator, pns->nodes[i]);
        }

        return (mp_parse_node_t)new_pns;
    }
    #endif

    // Sanity check to catch corrupted nodes early
    if (n > 100) {
        // Parse nodes shouldn't have this many children
        return MP_PARSE_NODE_NULL;
    }

    // Allocate new node using parent allocator
    mp_parse_node_struct_t *new_pns = (mp_parse_node_struct_t *)allocator(alloc_ctx,
        sizeof(mp_parse_node_struct_t) + sizeof(mp_parse_node_t) * n);

    // Copy node data
    new_pns->source_line = pns->source_line;
    new_pns->kind_num_nodes = pns->kind_num_nodes;

    // Recursively copy child nodes
    for (size_t i = 0; i < n; i++) {
        mp_parse_node_t child = pns->nodes[i];

        // Add validation to catch invalid nodes early
        if (!MP_PARSE_NODE_IS_NULL(child) && !MP_PARSE_NODE_IS_LEAF(child)) {
            // It should be a struct node - validate the pointer
            if ((uintptr_t)child < 0x1000) {
                // This is an invalid pointer - likely a corrupted node
                return MP_PARSE_NODE_NULL;
            }
        }
        new_pns->nodes[i] = copy_parse_node(alloc_ctx, allocator, child);
    }

    return (mp_parse_node_t)new_pns;
}

// Use MicroPython's existing parser for expression evaluation
mp_parse_node_t parse_tstring_expression(void *alloc_ctx, mp_parse_allocator_t allocator,
    const char *expr, size_t len) {
    // Check expression length limit
    if (len > MICROPY_PY_TSTRING_MAX_EXPR_LEN) {
        mp_raise_ValueError(MP_ERROR_TEXT("template expression too long"));
    }

    // Skip leading/trailing whitespace
    while (len > 0 && (expr[0] == ' ' || expr[0] == '\t' || expr[0] == '\n')) {
        expr++;
        len--;
    }
    while (len > 0 && (expr[len - 1] == ' ' || expr[len - 1] == '\t' || expr[len - 1] == '\n')) {
        len--;
    }

    if (len == 0) {
        return mp_parse_node_new_leaf(MP_PARSE_NODE_TOKEN, MP_TOKEN_KW_NONE);
    }


    // Create a lexer from the expression string
    mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR___lt_tstring_expr_gt_, expr, len, 0);

    if (lex == NULL) {
        // Failed to create lexer, return string as fallback
        qstr expr_str = qstr_from_strn(expr, len);
        return mp_parse_node_new_leaf(MP_PARSE_NODE_STRING, expr_str);
    }

    // Try to parse as an expression
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        // Parse the expression using eval_input mode
        mp_parse_tree_t parse_tree = mp_parse(lex, MP_PARSE_EVAL_INPUT);

        // Extract the actual expression from eval_input structure
        mp_parse_node_t root = parse_tree.root;


        // eval_input returns a structure, we need to extract the testlist
        if (MP_PARSE_NODE_IS_STRUCT(root)) {
            mp_parse_node_struct_t *pns = (mp_parse_node_struct_t *)root;
            if (MP_PARSE_NODE_STRUCT_KIND(pns) == RULE_eval_input) {
                // Extract the testlist (first child)
                root = pns->nodes[0];
            }
        }


        // Copy the parse tree to the parent allocator
        mp_parse_node_t result = copy_parse_node(alloc_ctx, allocator, root);

        // Clear the temporary parse tree
        mp_parse_tree_clear(&parse_tree);

        nlr_pop();
        return result;
    } else {
        // Parsing failed - propagate the error
        mp_obj_t exc = MP_OBJ_FROM_PTR(nlr.ret_val);

        // Check if it's a syntax error by checking the exception type
        if (mp_obj_is_exact_type(exc, &mp_type_SyntaxError)) {
            // Re-raise syntax error with context
            mp_raise_msg_varg(&mp_type_SyntaxError, MP_ERROR_TEXT("invalid syntax in template expression: %.*s"), (int)len, expr);
        } else {
            // Re-raise other exceptions
            nlr_raise(exc);
        }
    }
}

#else // MICROPY_PY_TSTRINGS

// Fallback implementation when t-strings are disabled
mp_parse_node_t parse_tstring_expression(void *alloc_ctx, mp_parse_allocator_t allocator,
    const char *expr, size_t len) {
    qstr expr_str = qstr_from_strn(expr, len);
    return mp_parse_node_new_leaf(MP_PARSE_NODE_STRING, expr_str);
}

#endif // MICROPY_PY_TSTRINGS
