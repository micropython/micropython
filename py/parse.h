/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#ifndef MICROPY_INCLUDED_PY_PARSE_H
#define MICROPY_INCLUDED_PY_PARSE_H

#include <stddef.h>
#include <stdint.h>

#include "py/obj.h"

struct _mp_lexer_t;

// a mp_parse_node_t is:
//  - 0000...0000: no node
//  - xxxx...xxx1: a small integer; bits 1 and above are the signed value, 2's complement
//  - xxxx...xx00: pointer to mp_parse_node_struct_t
//  - xx...xx0010: an identifier; bits 4 and above are the qstr
//  - xx...xx0110: a string; bits 4 and above are the qstr holding the value
//  - xx...xx1010: a string of bytes; bits 4 and above are the qstr holding the value
//  - xx...xx1110: a token; bits 4 and above are mp_token_kind_t

#define MP_PARSE_NODE_NULL      (0)
#define MP_PARSE_NODE_SMALL_INT (0x1)
#define MP_PARSE_NODE_ID        (0x02)
#define MP_PARSE_NODE_STRING    (0x06)
#define MP_PARSE_NODE_BYTES     (0x0a)
#define MP_PARSE_NODE_TOKEN     (0x0e)

typedef uintptr_t mp_parse_node_t; // must be pointer size

typedef struct _mp_parse_node_struct_t {
    uint32_t source_line;       // line number in source file
    uint32_t kind_num_nodes;    // parse node kind, and number of nodes
    mp_parse_node_t nodes[];    // nodes
} mp_parse_node_struct_t;

// macros for mp_parse_node_t usage
// some of these evaluate their argument more than once

#define MP_PARSE_NODE_IS_NULL(pn) ((pn) == MP_PARSE_NODE_NULL)
#define MP_PARSE_NODE_IS_LEAF(pn) ((pn) & 3)
#define MP_PARSE_NODE_IS_STRUCT(pn) ((pn) != MP_PARSE_NODE_NULL && ((pn) & 3) == 0)
#define MP_PARSE_NODE_IS_STRUCT_KIND(pn, k) ((pn) != MP_PARSE_NODE_NULL && ((pn) & 3) == 0 && MP_PARSE_NODE_STRUCT_KIND((mp_parse_node_struct_t*)(pn)) == (k))

#define MP_PARSE_NODE_IS_SMALL_INT(pn) (((pn) & 0x1) == MP_PARSE_NODE_SMALL_INT)
#define MP_PARSE_NODE_IS_ID(pn) (((pn) & 0x0f) == MP_PARSE_NODE_ID)
#define MP_PARSE_NODE_IS_TOKEN(pn) (((pn) & 0x0f) == MP_PARSE_NODE_TOKEN)
#define MP_PARSE_NODE_IS_TOKEN_KIND(pn, k) ((pn) == (MP_PARSE_NODE_TOKEN | ((k) << 4)))

#define MP_PARSE_NODE_LEAF_KIND(pn) ((pn) & 0x0f)
#define MP_PARSE_NODE_LEAF_ARG(pn) (((uintptr_t)(pn)) >> 4)
#define MP_PARSE_NODE_LEAF_SMALL_INT(pn) (((mp_int_t)(intptr_t)(pn)) >> 1)
#define MP_PARSE_NODE_STRUCT_KIND(pns) ((pns)->kind_num_nodes & 0xff)
#define MP_PARSE_NODE_STRUCT_NUM_NODES(pns) ((pns)->kind_num_nodes >> 8)

static inline mp_parse_node_t mp_parse_node_new_small_int(mp_int_t val) {
    return (mp_parse_node_t)(MP_PARSE_NODE_SMALL_INT | ((mp_uint_t)val << 1));
}
static inline mp_parse_node_t mp_parse_node_new_leaf(size_t kind, mp_int_t arg) {
    return (mp_parse_node_t)(kind | ((mp_uint_t)arg << 4));
}
bool mp_parse_node_is_const_false(mp_parse_node_t pn);
bool mp_parse_node_is_const_true(mp_parse_node_t pn);
bool mp_parse_node_get_int_maybe(mp_parse_node_t pn, mp_obj_t *o);
int mp_parse_node_extract_list(mp_parse_node_t *pn, size_t pn_kind, mp_parse_node_t **nodes);
void mp_parse_node_print(mp_parse_node_t pn, size_t indent);

typedef enum {
    MP_PARSE_SINGLE_INPUT,
    MP_PARSE_FILE_INPUT,
    MP_PARSE_EVAL_INPUT,
} mp_parse_input_kind_t;

typedef struct _mp_parse_t {
    mp_parse_node_t root;
    struct _mp_parse_chunk_t *chunk;
} mp_parse_tree_t;

// the parser will raise an exception if an error occurred
// the parser will free the lexer before it returns
mp_parse_tree_t mp_parse(struct _mp_lexer_t *lex, mp_parse_input_kind_t input_kind);
void mp_parse_tree_clear(mp_parse_tree_t *tree);

#endif // MICROPY_INCLUDED_PY_PARSE_H
