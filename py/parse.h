struct _py_lexer_t;

// a py_parse_node_t is:
//  - 0000...0000: no node
//  - xxxx...0001: an identifier; bits 4 and above are the qstr
//  - xxxx...0011: a small integer; bits 4 and above are the signed value, 2's complement
//  - xxxx...0101: an integer; bits 4 and above are the qstr holding the value
//  - xxxx...0111: a decimal; bits 4 and above are the qstr holding the value
//  - xxxx...1001: a string; bits 4 and above are the qstr holding the value
//  - xxxx...1011: a string with triple quotes; bits 4 and above are the qstr holding the value
//  - xxxx...1101: a token; bits 4 and above are py_token_kind_t
//  - xxxx...xxx0: pointer to py_parse_node_struct_t

#define PY_PARSE_NODE_NULL      (0)
#define PY_PARSE_NODE_ID        (0x1)
#define PY_PARSE_NODE_SMALL_INT (0x3)
#define PY_PARSE_NODE_INTEGER   (0x5)
#define PY_PARSE_NODE_DECIMAL   (0x7)
#define PY_PARSE_NODE_STRING    (0x9)
#define PY_PARSE_NODE_BYTES     (0xb)
#define PY_PARSE_NODE_TOKEN     (0xd)

typedef machine_uint_t py_parse_node_t; // must be pointer size

typedef struct _py_parse_node_struct_t {
    uint32_t source;            // file identifier, and line number
    uint32_t kind_num_nodes;    // parse node kind, and number of nodes
    py_parse_node_t nodes[];    // nodes
} py_parse_node_struct_t;

// macros for py_parse_node_t usage
// some of these evaluate their argument more than once

#define PY_PARSE_NODE_IS_NULL(pn) ((pn) == PY_PARSE_NODE_NULL)
#define PY_PARSE_NODE_IS_LEAF(pn) ((pn) & 1)
#define PY_PARSE_NODE_IS_STRUCT(pn) ((pn) != PY_PARSE_NODE_NULL && ((pn) & 1) == 0)
#define PY_PARSE_NODE_IS_STRUCT_KIND(pn, k) ((pn) != PY_PARSE_NODE_NULL && ((pn) & 1) == 0 && PY_PARSE_NODE_STRUCT_KIND((py_parse_node_struct_t*)(pn)) == (k))

#define PY_PARSE_NODE_IS_ID(pn) (((pn) & 0xf) == PY_PARSE_NODE_ID)
#define PY_PARSE_NODE_IS_SMALL_INT(pn) (((pn) & 0xf) == PY_PARSE_NODE_SMALL_INT)
#define PY_PARSE_NODE_IS_TOKEN(pn) (((pn) & 0xf) == PY_PARSE_NODE_TOKEN)
#define PY_PARSE_NODE_IS_TOKEN_KIND(pn, k) ((pn) == (PY_PARSE_NODE_TOKEN | (k << 4)))

#define PY_PARSE_NODE_LEAF_KIND(pn) ((pn) & 0xf)
// TODO should probably have int and uint versions of this macro
#define PY_PARSE_NODE_LEAF_ARG(pn) (((machine_int_t)(pn)) >> 4)
#define PY_PARSE_NODE_STRUCT_KIND(pns) ((pns)->kind_num_nodes & 0xff)
#define PY_PARSE_NODE_STRUCT_NUM_NODES(pns) ((pns)->kind_num_nodes >> 8)

py_parse_node_t py_parse_node_new_leaf(machine_int_t kind, machine_int_t arg);

void parse_node_show(py_parse_node_t pn, int indent);

py_parse_node_t py_parse(struct _py_lexer_t *lex, int wanted_rule);
