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
#ifndef MICROPY_INCLUDED_PY_SCOPE_H
#define MICROPY_INCLUDED_PY_SCOPE_H

#include "py/parse.h"
#include "py/emitglue.h"

typedef enum {
    ID_INFO_KIND_UNDECIDED,
    ID_INFO_KIND_GLOBAL_IMPLICIT,
    ID_INFO_KIND_GLOBAL_IMPLICIT_ASSIGNED,
    ID_INFO_KIND_GLOBAL_EXPLICIT,
    ID_INFO_KIND_LOCAL, // in a function f, written and only referenced by f
    ID_INFO_KIND_CELL,  // in a function f, read/written by children of f
    ID_INFO_KIND_FREE,  // in a function f, belongs to the parent of f
} id_info_kind_t;

enum {
    ID_FLAG_IS_PARAM = 0x01,
    ID_FLAG_IS_STAR_PARAM = 0x02,
    ID_FLAG_IS_DBL_STAR_PARAM = 0x04,
    ID_FLAG_VIPER_TYPE_POS = 4,
};

typedef struct _id_info_t {
    uint8_t kind;
    uint8_t flags;
    // when it's an ID_INFO_KIND_LOCAL this is the unique number of the local
    // when it's an ID_INFO_KIND_CELL/FREE this is the unique number of the closed over variable
    uint16_t local_num;
    qstr qst;
} id_info_t;

#define SCOPE_IS_FUNC_LIKE(s) ((s) >= SCOPE_LAMBDA)
#define SCOPE_IS_COMP_LIKE(s) (SCOPE_LIST_COMP <= (s) && (s) <= SCOPE_GEN_EXPR)

// scope is a "block" in Python parlance
typedef enum {
    SCOPE_MODULE,
    SCOPE_CLASS,
    SCOPE_LAMBDA,
    SCOPE_LIST_COMP,
    SCOPE_DICT_COMP,
    SCOPE_SET_COMP,
    SCOPE_GEN_EXPR,
    SCOPE_FUNCTION,
} scope_kind_t;

typedef struct _scope_t {
    scope_kind_t kind;
    struct _scope_t *parent;
    struct _scope_t *next;
    mp_parse_node_t pn;
    mp_raw_code_t *raw_code;
    #if MICROPY_DEBUG_PRINTERS
    size_t raw_code_data_len; // for mp_bytecode_print
    #endif
    uint16_t simple_name; // a qstr
    uint16_t scope_flags;  // see runtime0.h
    uint16_t emit_options; // see emitglue.h
    uint16_t num_pos_args;
    uint16_t num_kwonly_args;
    uint16_t num_def_pos_args;
    uint16_t num_locals;
    uint16_t stack_size;     // maximum size of the locals stack
    uint16_t exc_stack_size; // maximum size of the exception stack
    uint16_t id_info_alloc;
    uint16_t id_info_len;
    id_info_t *id_info;
} scope_t;

scope_t *scope_new(scope_kind_t kind, mp_parse_node_t pn, mp_uint_t emit_options);
void scope_free(scope_t *scope);
id_info_t *scope_find_or_add_id(scope_t *scope, qstr qstr, id_info_kind_t kind);
id_info_t *scope_find(scope_t *scope, qstr qstr);
id_info_t *scope_find_global(scope_t *scope, qstr qstr);
void scope_check_to_close_over(scope_t *scope, id_info_t *id);

#endif // MICROPY_INCLUDED_PY_SCOPE_H
