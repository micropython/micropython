struct _mp_lexer_t;

// a mp_parse_node_t is:
//  - 0000...0000: no node
//  - xxxx...0001: an identifier; bits 4 and above are the qstr
//  - xxxx...0011: a small integer; bits 4 and above are the signed value, 2's complement
//  - xxxx...0101: an integer; bits 4 and above are the qstr holding the value
//  - xxxx...0111: a decimal; bits 4 and above are the qstr holding the value
//  - xxxx...1001: a string; bits 4 and above are the qstr holding the value
//  - xxxx...1011: a string with triple quotes; bits 4 and above are the qstr holding the value
//  - xxxx...1101: a token; bits 4 and above are mp_token_kind_t
//  - xxxx...xxx0: pointer to mp_parse_node_struct_t

// makes sure the top 5 bits of x are all cleared (positive number) or all set (negavite number)
// these macros can probably go somewhere else because they are used more than just in the parser
#define MP_UINT_HIGH_5_BITS (~((~((machine_uint_t)0)) >> 5))
#define MP_FIT_SMALL_INT(x) (((((machine_uint_t)(x)) & MP_UINT_HIGH_5_BITS) == 0) || ((((machine_uint_t)(x)) & MP_UINT_HIGH_5_BITS) == MP_UINT_HIGH_5_BITS))

#define MP_PARSE_NODE_NULL      (0)
#define MP_PARSE_NODE_ID        (0x1)
#define MP_PARSE_NODE_SMALL_INT (0x3)
#define MP_PARSE_NODE_INTEGER   (0x5)
#define MP_PARSE_NODE_DECIMAL   (0x7)
#define MP_PARSE_NODE_STRING    (0x9)
#define MP_PARSE_NODE_BYTES     (0xb)
#define MP_PARSE_NODE_TOKEN     (0xd)

typedef machine_uint_t mp_parse_node_t; // must be pointer size

typedef struct _mp_parse_node_struct_t {
    uint32_t source_line;       // line number in source file
    uint32_t kind_num_nodes;    // parse node kind, and number of nodes
    mp_parse_node_t nodes[];    // nodes
} mp_parse_node_struct_t;

// macros for mp_parse_node_t usage
// some of these evaluate their argument more than once

#define MP_PARSE_NODE_IS_NULL(pn) ((pn) == MP_PARSE_NODE_NULL)
#define MP_PARSE_NODE_IS_LEAF(pn) ((pn) & 1)
#define MP_PARSE_NODE_IS_STRUCT(pn) ((pn) != MP_PARSE_NODE_NULL && ((pn) & 1) == 0)
#define MP_PARSE_NODE_IS_STRUCT_KIND(pn, k) ((pn) != MP_PARSE_NODE_NULL && ((pn) & 1) == 0 && MP_PARSE_NODE_STRUCT_KIND((mp_parse_node_struct_t*)(pn)) == (k))

#define MP_PARSE_NODE_IS_ID(pn) (((pn) & 0xf) == MP_PARSE_NODE_ID)
#define MP_PARSE_NODE_IS_SMALL_INT(pn) (((pn) & 0xf) == MP_PARSE_NODE_SMALL_INT)
#define MP_PARSE_NODE_IS_TOKEN(pn) (((pn) & 0xf) == MP_PARSE_NODE_TOKEN)
#define MP_PARSE_NODE_IS_TOKEN_KIND(pn, k) ((pn) == (MP_PARSE_NODE_TOKEN | (k << 4)))

#define MP_PARSE_NODE_LEAF_KIND(pn) ((pn) & 0xf)
// TODO should probably have int and uint versions of this macro
#define MP_PARSE_NODE_LEAF_ARG(pn) (((machine_int_t)(pn)) >> 4)
#define MP_PARSE_NODE_STRUCT_KIND(pns) ((pns)->kind_num_nodes & 0xff)
#define MP_PARSE_NODE_STRUCT_NUM_NODES(pns) ((pns)->kind_num_nodes >> 8)

mp_parse_node_t mp_parse_node_new_leaf(machine_int_t kind, machine_int_t arg);
uint mp_parse_node_free(mp_parse_node_t pn);

void mp_parse_node_print(mp_parse_node_t pn, int indent);

typedef enum {
    MP_PARSE_SINGLE_INPUT,
    MP_PARSE_FILE_INPUT,
    MP_PARSE_EVAL_INPUT,
} mp_parse_input_kind_t;

// returns MP_PARSE_NODE_NULL on error, and then exc_id_out and exc_msg_out are valid
mp_parse_node_t mp_parse(struct _mp_lexer_t *lex, mp_parse_input_kind_t input_kind, qstr *exc_id_out, const char **exc_msg_out);
