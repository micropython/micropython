/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Damien P. George
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

#include "py/objcode.h"

#if MICROPY_PY_BUILTINS_CODE == MICROPY_PY_BUILTINS_CODE_NONE

// Code object not implemented at this configuration level.

#elif MICROPY_PY_BUILTINS_CODE <= MICROPY_PY_BUILTINS_CODE_BASIC

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_code,
    MP_QSTR_code,
    MP_TYPE_FLAG_NONE
    );

#elif MICROPY_PY_BUILTINS_CODE <= MICROPY_PY_BUILTINS_CODE_FULL

#include "py/profile.h"

static void code_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_code_t *o = MP_OBJ_TO_PTR(o_in);
    const mp_raw_code_t *rc = o->rc;
    const mp_bytecode_prelude_t *prelude = &rc->prelude;
    mp_printf(print,
        "<code object %q at 0x%p, file \"%q\", line %d>",
        MP_CODE_QSTR_MAP(o->context, prelude->qstr_block_name_idx),
        o,
        MP_CODE_QSTR_MAP(o->context, 0),
        rc->line_of_definition
        );
}

static mp_obj_tuple_t *code_consts(const mp_module_context_t *context, const mp_raw_code_t *rc) {
    mp_obj_tuple_t *consts = MP_OBJ_TO_PTR(mp_obj_new_tuple(rc->n_children + 1, NULL));

    size_t const_no = 0;
    for (size_t i = 0; i < rc->n_children; ++i) {
        mp_obj_t code = mp_obj_new_code(context, rc->children[i], true);
        consts->items[const_no++] = code;
    }
    consts->items[const_no++] = mp_const_none;

    return consts;
}

static mp_obj_t raw_code_lnotab(const mp_raw_code_t *rc) {
    // const mp_bytecode_prelude_t *prelude = &rc->prelude;
    uint start = 0;
    uint stop = rc->fun_data_len - start;

    uint last_lineno = mp_prof_bytecode_lineno(rc, start);
    uint lasti = 0;

    const uint buffer_chunk_size = (stop - start) >> 2; // heuristic magic
    uint buffer_size = buffer_chunk_size;
    byte *buffer = m_new(byte, buffer_size);
    uint buffer_index = 0;

    for (uint i = start; i < stop; ++i) {
        uint lineno = mp_prof_bytecode_lineno(rc, i);
        size_t line_diff = lineno - last_lineno;
        if (line_diff > 0) {
            uint instr_diff = (i - start) - lasti;

            assert(instr_diff < 256);
            assert(line_diff < 256);

            if (buffer_index + 2 > buffer_size) {
                buffer = m_renew(byte, buffer, buffer_size, buffer_size + buffer_chunk_size);
                buffer_size = buffer_size + buffer_chunk_size;
            }
            last_lineno = lineno;
            lasti = i - start;
            buffer[buffer_index++] = instr_diff;
            buffer[buffer_index++] = line_diff;
        }
    }

    mp_obj_t o = mp_obj_new_bytes(buffer, buffer_index);
    m_del(byte, buffer, buffer_size);
    return o;
}

static void code_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    if (dest[0] != MP_OBJ_NULL) {
        // not load attribute
        return;
    }
    mp_obj_code_t *o = MP_OBJ_TO_PTR(self_in);
    const mp_raw_code_t *rc = o->rc;
    const mp_bytecode_prelude_t *prelude = &rc->prelude;
    switch (attr) {
        case MP_QSTR_co_code:
            dest[0] = mp_obj_new_bytes(
                (void *)prelude->opcodes,
                rc->fun_data_len - (prelude->opcodes - (const byte *)rc->fun_data)
                );
            break;
        case MP_QSTR_co_consts:
            dest[0] = MP_OBJ_FROM_PTR(code_consts(o->context, rc));
            break;
        case MP_QSTR_co_filename:
            dest[0] = MP_OBJ_NEW_QSTR(MP_CODE_QSTR_MAP(o->context, 0));
            break;
        case MP_QSTR_co_firstlineno:
            dest[0] = MP_OBJ_NEW_SMALL_INT(mp_prof_bytecode_lineno(rc, 0));
            break;
        case MP_QSTR_co_name:
            dest[0] = MP_OBJ_NEW_QSTR(MP_CODE_QSTR_MAP(o->context, prelude->qstr_block_name_idx));
            break;
        case MP_QSTR_co_names:
            dest[0] = MP_OBJ_FROM_PTR(o->dict_locals);
            break;
        case MP_QSTR_co_lnotab:
            if (!o->lnotab) {
                o->lnotab = raw_code_lnotab(rc);
            }
            dest[0] = o->lnotab;
            break;
    }
}

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_code,
    MP_QSTR_code,
    MP_TYPE_FLAG_NONE,
    print, code_print,
    attr, code_attr
    );

mp_obj_t mp_obj_new_code(const mp_module_context_t *context, const mp_raw_code_t *rc, bool result_required) {
    mp_obj_code_t *o;
    if (result_required) {
        o = m_new_obj(mp_obj_code_t);
    } else {
        o = m_new_obj_maybe(mp_obj_code_t);
        if (o == NULL) {
            return MP_OBJ_NULL;
        }
    }
    o->base.type = &mp_type_code;
    o->context = context;
    o->rc = rc;
    o->dict_locals = mp_locals_get(); // this is a wrong! how to do this properly?
    o->lnotab = MP_OBJ_NULL;
    return MP_OBJ_FROM_PTR(o);
}

#endif
