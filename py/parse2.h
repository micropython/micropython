/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2016 Damien P. George
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

#define MP_PT_NULL (0)
#define MP_PT_TOKEN (1)
#define MP_PT_SMALL_INT (3)
#define MP_PT_STRING (4)
#define MP_PT_BYTES (5)
#define MP_PT_CONST_OBJECT (8)
#define MP_PT_ID_BASE (10) // +16
#define MP_PT_RULE_BASE (26) // +173-ish

extern const byte pt_const_int0[];

static inline const byte *pt_tok_extract(const byte *p, byte *tok) {
    //assert(*p == MP_PT_TOKEN);
    p += 1;
    *tok = *p++;
    return p;
}

static inline bool pt_is_null(const byte *p) {
    return *p == MP_PT_NULL;
}

static inline bool pt_is_null_with_top(const byte *p, const byte *ptop) {
    return p == ptop || *p == MP_PT_NULL;
}

static inline bool pt_is_small_int(const byte *p) {
    return *p == MP_PT_SMALL_INT;
}

static inline bool pt_is_any_rule(const byte *p) {
    return *p >= MP_PT_RULE_BASE;
}

static inline mp_uint_t pt_rule_extract_rule_id(const byte *p) {
    return *p - MP_PT_RULE_BASE;
}

static inline bool pt_is_any_id(const byte *p) {
    return *p >= MP_PT_ID_BASE && *p < MP_PT_RULE_BASE;
}

static inline bool pt_is_id(const byte *p, qstr qst) {
    //assert(*p == MP_PT_ID_BASE);
    return qst == ((mp_uint_t)p[1] | (((mp_uint_t)p[0] - MP_PT_ID_BASE) << 8));
}

static inline bool pt_is_any_tok(const byte *p) {
    return p[0] == MP_PT_TOKEN;
}

static inline bool pt_is_tok(const byte *p, int tok) {
    return p[0] == MP_PT_TOKEN && p[1] == tok;
}

static inline bool pt_is_rule(const byte *p, int rule) {
    return *p == MP_PT_RULE_BASE + rule;
}

int pt_num_nodes(const byte *p, const byte *ptop);
const byte *pt_next(const byte *p);

//const byte *pt_extract_id(const byte *p, qstr *qst);
static inline const byte *pt_extract_id(const byte *p, qstr *qst) {
    //assert(*p == MP_PT_ID_BASE);
    *qst = p[1] | ((p[0] - MP_PT_ID_BASE) << 8);
    return p + 2;
}

const byte *pt_extract_const_obj(const byte *p, mp_uint_t *idx);
mp_int_t pt_small_int_value(const byte *p);
const byte *pt_get_small_int(const byte *p, mp_int_t *val);
const byte *pt_rule_first(const byte *p);
const byte *pt_rule_extract_top(const byte *p, const byte **ptop);
const byte *pt_rule_extract(const byte *p, mp_uint_t *rule_id, size_t *src_line, const byte **ptop);
bool pt_is_rule_empty(const byte *p);

bool mp_parse_node_get_int_maybe(const byte *p, mp_obj_t *o);
const byte *mp_parse_node_extract_list(const byte **p, mp_uint_t pn_kind);

typedef enum {
    MP_PARSE_SINGLE_INPUT,
    MP_PARSE_FILE_INPUT,
    MP_PARSE_EVAL_INPUT,
} mp_parse_input_kind_t;

typedef struct _mp_parse_t {
    const byte *root;
    mp_uint_t *co_data;
    struct _mp_parse_chunk_t *chunk;
} mp_parse_tree_t;

// the parser will raise an exception if an error occurred
// the parser will free the lexer before it returns
mp_parse_tree_t mp_parse(struct _mp_lexer_t *lex, mp_parse_input_kind_t input_kind);
void mp_parse_tree_clear(mp_parse_tree_t *tree);

#endif // MICROPY_INCLUDED_PY_PARSE_H
