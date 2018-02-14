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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "py/mpstate.h"
#include "py/emit.h"
#include "py/bc0.h"

#if MICROPY_ENABLE_COMPILER

#define BYTES_FOR_INT ((BYTES_PER_WORD * 8 + 6) / 7)
#define DUMMY_DATA_SIZE (BYTES_FOR_INT)

struct _emit_t {
    // Accessed as mp_obj_t, so must be aligned as such, and we rely on the
    // memory allocator returning a suitably aligned pointer.
    // Should work for cases when mp_obj_t is 64-bit on a 32-bit machine.
    byte dummy_data[DUMMY_DATA_SIZE];

    pass_kind_t pass : 8;
    mp_uint_t last_emit_was_return_value : 8;

    int stack_size;

    scope_t *scope;

    mp_uint_t last_source_line_offset;
    mp_uint_t last_source_line;

    mp_uint_t max_num_labels;
    mp_uint_t *label_offsets;

    size_t code_info_offset;
    size_t code_info_size;
    size_t bytecode_offset;
    size_t bytecode_size;
    byte *code_base; // stores both byte code and code info

    #if MICROPY_PERSISTENT_CODE
    uint16_t ct_cur_obj;
    uint16_t ct_num_obj;
    uint16_t ct_cur_raw_code;
    #endif
    mp_uint_t *const_table;
};

emit_t *emit_bc_new(void) {
    emit_t *emit = m_new0(emit_t, 1);
    return emit;
}

void emit_bc_set_max_num_labels(emit_t *emit, mp_uint_t max_num_labels) {
    emit->max_num_labels = max_num_labels;
    emit->label_offsets = m_new(mp_uint_t, emit->max_num_labels);
}

void emit_bc_free(emit_t *emit) {
    m_del(mp_uint_t, emit->label_offsets, emit->max_num_labels);
    m_del_obj(emit_t, emit);
}

typedef byte *(*emit_allocator_t)(emit_t *emit, int nbytes);

STATIC void emit_write_uint(emit_t *emit, emit_allocator_t allocator, mp_uint_t val) {
    // We store each 7 bits in a separate byte, and that's how many bytes needed
    byte buf[BYTES_FOR_INT];
    byte *p = buf + sizeof(buf);
    // We encode in little-ending order, but store in big-endian, to help decoding
    do {
        *--p = val & 0x7f;
        val >>= 7;
    } while (val != 0);
    byte *c = allocator(emit, buf + sizeof(buf) - p);
    while (p != buf + sizeof(buf) - 1) {
        *c++ = *p++ | 0x80;
    }
    *c = *p;
}

// all functions must go through this one to emit code info
STATIC byte *emit_get_cur_to_write_code_info(emit_t *emit, int num_bytes_to_write) {
    //printf("emit %d\n", num_bytes_to_write);
    if (emit->pass < MP_PASS_EMIT) {
        emit->code_info_offset += num_bytes_to_write;
        return emit->dummy_data;
    } else {
        assert(emit->code_info_offset + num_bytes_to_write <= emit->code_info_size);
        byte *c = emit->code_base + emit->code_info_offset;
        emit->code_info_offset += num_bytes_to_write;
        return c;
    }
}

STATIC void emit_write_code_info_byte(emit_t* emit, byte val) {
    *emit_get_cur_to_write_code_info(emit, 1) = val;
}

STATIC void emit_write_code_info_uint(emit_t* emit, mp_uint_t val) {
    emit_write_uint(emit, emit_get_cur_to_write_code_info, val);
}

STATIC void emit_write_code_info_qstr(emit_t *emit, qstr qst) {
    #if MICROPY_PERSISTENT_CODE
    assert((qst >> 16) == 0);
    byte *c = emit_get_cur_to_write_code_info(emit, 2);
    c[0] = qst;
    c[1] = qst >> 8;
    #else
    emit_write_uint(emit, emit_get_cur_to_write_code_info, qst);
    #endif
}

#if MICROPY_ENABLE_SOURCE_LINE
STATIC void emit_write_code_info_bytes_lines(emit_t *emit, mp_uint_t bytes_to_skip, mp_uint_t lines_to_skip) {
    assert(bytes_to_skip > 0 || lines_to_skip > 0);
    //printf("  %d %d\n", bytes_to_skip, lines_to_skip);
    while (bytes_to_skip > 0 || lines_to_skip > 0) {
        mp_uint_t b, l;
        if (lines_to_skip <= 6 || bytes_to_skip > 0xf) {
            // use 0b0LLBBBBB encoding
            b = MIN(bytes_to_skip, 0x1f);
            if (b < bytes_to_skip) {
                // we can't skip any lines until we skip all the bytes
                l = 0;
            } else {
                l = MIN(lines_to_skip, 0x3);
            }
            *emit_get_cur_to_write_code_info(emit, 1) = b | (l << 5);
        } else {
            // use 0b1LLLBBBB 0bLLLLLLLL encoding (l's LSB in second byte)
            b = MIN(bytes_to_skip, 0xf);
            l = MIN(lines_to_skip, 0x7ff);
            byte *ci = emit_get_cur_to_write_code_info(emit, 2);
            ci[0] = 0x80 | b | ((l >> 4) & 0x70);
            ci[1] = l;
        }
        bytes_to_skip -= b;
        lines_to_skip -= l;
    }
}
#endif

// all functions must go through this one to emit byte code
STATIC byte *emit_get_cur_to_write_bytecode(emit_t *emit, int num_bytes_to_write) {
    //printf("emit %d\n", num_bytes_to_write);
    if (emit->pass < MP_PASS_EMIT) {
        emit->bytecode_offset += num_bytes_to_write;
        return emit->dummy_data;
    } else {
        assert(emit->bytecode_offset + num_bytes_to_write <= emit->bytecode_size);
        byte *c = emit->code_base + emit->code_info_size + emit->bytecode_offset;
        emit->bytecode_offset += num_bytes_to_write;
        return c;
    }
}

STATIC void emit_write_bytecode_byte(emit_t *emit, byte b1) {
    byte *c = emit_get_cur_to_write_bytecode(emit, 1);
    c[0] = b1;
}

STATIC void emit_write_bytecode_byte_byte(emit_t* emit, byte b1, byte b2) {
    byte *c = emit_get_cur_to_write_bytecode(emit, 2);
    c[0] = b1;
    c[1] = b2;
}

// Similar to emit_write_bytecode_uint(), just some extra handling to encode sign
STATIC void emit_write_bytecode_byte_int(emit_t *emit, byte b1, mp_int_t num) {
    emit_write_bytecode_byte(emit, b1);

    // We store each 7 bits in a separate byte, and that's how many bytes needed
    byte buf[BYTES_FOR_INT];
    byte *p = buf + sizeof(buf);
    // We encode in little-ending order, but store in big-endian, to help decoding
    do {
        *--p = num & 0x7f;
        num >>= 7;
    } while (num != 0 && num != -1);
    // Make sure that highest bit we stored (mask 0x40) matches sign
    // of the number. If not, store extra byte just to encode sign
    if (num == -1 && (*p & 0x40) == 0) {
        *--p = 0x7f;
    } else if (num == 0 && (*p & 0x40) != 0) {
        *--p = 0;
    }

    byte *c = emit_get_cur_to_write_bytecode(emit, buf + sizeof(buf) - p);
    while (p != buf + sizeof(buf) - 1) {
        *c++ = *p++ | 0x80;
    }
    *c = *p;
}

STATIC void emit_write_bytecode_byte_uint(emit_t *emit, byte b, mp_uint_t val) {
    emit_write_bytecode_byte(emit, b);
    emit_write_uint(emit, emit_get_cur_to_write_bytecode, val);
}

#if MICROPY_PERSISTENT_CODE
STATIC void emit_write_bytecode_byte_const(emit_t *emit, byte b, mp_uint_t n, mp_uint_t c) {
    if (emit->pass == MP_PASS_EMIT) {
        emit->const_table[n] = c;
    }
    emit_write_bytecode_byte_uint(emit, b, n);
}
#endif

STATIC void emit_write_bytecode_byte_qstr(emit_t* emit, byte b, qstr qst) {
    #if MICROPY_PERSISTENT_CODE
    assert((qst >> 16) == 0);
    byte *c = emit_get_cur_to_write_bytecode(emit, 3);
    c[0] = b;
    c[1] = qst;
    c[2] = qst >> 8;
    #else
    emit_write_bytecode_byte_uint(emit, b, qst);
    #endif
}

STATIC void emit_write_bytecode_byte_obj(emit_t *emit, byte b, mp_obj_t obj) {
    #if MICROPY_PERSISTENT_CODE
    emit_write_bytecode_byte_const(emit, b,
        emit->scope->num_pos_args + emit->scope->num_kwonly_args
        + emit->ct_cur_obj++, (mp_uint_t)obj);
    #else
    // aligns the pointer so it is friendly to GC
    emit_write_bytecode_byte(emit, b);
    emit->bytecode_offset = (size_t)MP_ALIGN(emit->bytecode_offset, sizeof(mp_obj_t));
    mp_obj_t *c = (mp_obj_t*)emit_get_cur_to_write_bytecode(emit, sizeof(mp_obj_t));
    // Verify thar c is already uint-aligned
    assert(c == MP_ALIGN(c, sizeof(mp_obj_t)));
    *c = obj;
    #endif
}

STATIC void emit_write_bytecode_byte_raw_code(emit_t *emit, byte b, mp_raw_code_t *rc) {
    #if MICROPY_PERSISTENT_CODE
    emit_write_bytecode_byte_const(emit, b,
        emit->scope->num_pos_args + emit->scope->num_kwonly_args
        + emit->ct_num_obj + emit->ct_cur_raw_code++, (mp_uint_t)(uintptr_t)rc);
    #else
    // aligns the pointer so it is friendly to GC
    emit_write_bytecode_byte(emit, b);
    emit->bytecode_offset = (size_t)MP_ALIGN(emit->bytecode_offset, sizeof(void*));
    void **c = (void**)emit_get_cur_to_write_bytecode(emit, sizeof(void*));
    // Verify thar c is already uint-aligned
    assert(c == MP_ALIGN(c, sizeof(void*)));
    *c = rc;
    #endif
}

// unsigned labels are relative to ip following this instruction, stored as 16 bits
STATIC void emit_write_bytecode_byte_unsigned_label(emit_t *emit, byte b1, mp_uint_t label) {
    mp_uint_t bytecode_offset;
    if (emit->pass < MP_PASS_EMIT) {
        bytecode_offset = 0;
    } else {
        bytecode_offset = emit->label_offsets[label] - emit->bytecode_offset - 3;
    }
    byte *c = emit_get_cur_to_write_bytecode(emit, 3);
    c[0] = b1;
    c[1] = bytecode_offset;
    c[2] = bytecode_offset >> 8;
}

// signed labels are relative to ip following this instruction, stored as 16 bits, in excess
STATIC void emit_write_bytecode_byte_signed_label(emit_t *emit, byte b1, mp_uint_t label) {
    int bytecode_offset;
    if (emit->pass < MP_PASS_EMIT) {
        bytecode_offset = 0;
    } else {
        bytecode_offset = emit->label_offsets[label] - emit->bytecode_offset - 3 + 0x8000;
    }
    byte *c = emit_get_cur_to_write_bytecode(emit, 3);
    c[0] = b1;
    c[1] = bytecode_offset;
    c[2] = bytecode_offset >> 8;
}

void mp_emit_bc_start_pass(emit_t *emit, pass_kind_t pass, scope_t *scope) {
    emit->pass = pass;
    emit->stack_size = 0;
    emit->last_emit_was_return_value = false;
    emit->scope = scope;
    emit->last_source_line_offset = 0;
    emit->last_source_line = 1;
    #ifndef NDEBUG
    // With debugging enabled labels are checked for unique assignment
    if (pass < MP_PASS_EMIT) {
        memset(emit->label_offsets, -1, emit->max_num_labels * sizeof(mp_uint_t));
    }
    #endif
    emit->bytecode_offset = 0;
    emit->code_info_offset = 0;

    // Write local state size and exception stack size.
    {
        mp_uint_t n_state = scope->num_locals + scope->stack_size;
        if (n_state == 0) {
            // Need at least 1 entry in the state, in the case an exception is
            // propagated through this function, the exception is returned in
            // the highest slot in the state (fastn[0], see vm.c).
            n_state = 1;
        }
        emit_write_code_info_uint(emit, n_state);
        emit_write_code_info_uint(emit, scope->exc_stack_size);
    }

    // Write scope flags and number of arguments.
    // TODO check that num args all fit in a byte
    emit_write_code_info_byte(emit, emit->scope->scope_flags);
    emit_write_code_info_byte(emit, emit->scope->num_pos_args);
    emit_write_code_info_byte(emit, emit->scope->num_kwonly_args);
    emit_write_code_info_byte(emit, emit->scope->num_def_pos_args);

    // Write size of the rest of the code info.  We don't know how big this
    // variable uint will be on the MP_PASS_CODE_SIZE pass so we reserve 2 bytes
    // for it and hope that is enough!  TODO assert this or something.
    if (pass == MP_PASS_EMIT) {
        emit_write_code_info_uint(emit, emit->code_info_size - emit->code_info_offset);
    } else  {
        emit_get_cur_to_write_code_info(emit, 2);
    }

    // Write the name and source file of this function.
    emit_write_code_info_qstr(emit, scope->simple_name);
    emit_write_code_info_qstr(emit, scope->source_file);

    // bytecode prelude: initialise closed over variables
    for (int i = 0; i < scope->id_info_len; i++) {
        id_info_t *id = &scope->id_info[i];
        if (id->kind == ID_INFO_KIND_CELL) {
            assert(id->local_num < 255);
            emit_write_bytecode_byte(emit, id->local_num); // write the local which should be converted to a cell
        }
    }
    emit_write_bytecode_byte(emit, 255); // end of list sentinel

    #if MICROPY_PERSISTENT_CODE
    emit->ct_cur_obj = 0;
    emit->ct_cur_raw_code = 0;
    #endif

    if (pass == MP_PASS_EMIT) {
        // Write argument names (needed to resolve positional args passed as
        // keywords).  We store them as full word-sized objects for efficient access
        // in mp_setup_code_state this is the start of the prelude and is guaranteed
        // to be aligned on a word boundary.

        // For a given argument position (indexed by i) we need to find the
        // corresponding id_info which is a parameter, as it has the correct
        // qstr name to use as the argument name.  Note that it's not a simple
        // 1-1 mapping (ie i!=j in general) because of possible closed-over
        // variables.  In the case that the argument i has no corresponding
        // parameter we use "*" as its name (since no argument can ever be named
        // "*").  We could use a blank qstr but "*" is better for debugging.
        // Note: there is some wasted RAM here for the case of storing a qstr
        // for each closed-over variable, and maybe there is a better way to do
        // it, but that would require changes to mp_setup_code_state.
        for (int i = 0; i < scope->num_pos_args + scope->num_kwonly_args; i++) {
            qstr qst = MP_QSTR__star_;
            for (int j = 0; j < scope->id_info_len; ++j) {
                id_info_t *id = &scope->id_info[j];
                if ((id->flags & ID_FLAG_IS_PARAM) && id->local_num == i) {
                    qst = id->qst;
                    break;
                }
            }
            emit->const_table[i] = (mp_uint_t)MP_OBJ_NEW_QSTR(qst);
        }
    }
}

void mp_emit_bc_end_pass(emit_t *emit) {
    if (emit->pass == MP_PASS_SCOPE) {
        return;
    }

    // check stack is back to zero size
    assert(emit->stack_size == 0);

    emit_write_code_info_byte(emit, 0); // end of line number info

    #if MICROPY_PERSISTENT_CODE
    assert(emit->pass <= MP_PASS_STACK_SIZE || (emit->ct_num_obj == emit->ct_cur_obj));
    emit->ct_num_obj = emit->ct_cur_obj;
    #endif

    if (emit->pass == MP_PASS_CODE_SIZE) {
        #if !MICROPY_PERSISTENT_CODE
        // so bytecode is aligned
        emit->code_info_offset = (size_t)MP_ALIGN(emit->code_info_offset, sizeof(mp_uint_t));
        #endif

        // calculate size of total code-info + bytecode, in bytes
        emit->code_info_size = emit->code_info_offset;
        emit->bytecode_size = emit->bytecode_offset;
        emit->code_base = m_new0(byte, emit->code_info_size + emit->bytecode_size);

        #if MICROPY_PERSISTENT_CODE
        emit->const_table = m_new0(mp_uint_t,
            emit->scope->num_pos_args + emit->scope->num_kwonly_args
            + emit->ct_cur_obj + emit->ct_cur_raw_code);
        #else
        emit->const_table = m_new0(mp_uint_t,
            emit->scope->num_pos_args + emit->scope->num_kwonly_args);
        #endif

    } else if (emit->pass == MP_PASS_EMIT) {
        mp_emit_glue_assign_bytecode(emit->scope->raw_code, emit->code_base,
            #if MICROPY_PERSISTENT_CODE_SAVE || MICROPY_DEBUG_PRINTERS
            emit->code_info_size + emit->bytecode_size,
            #endif
            emit->const_table,
            #if MICROPY_PERSISTENT_CODE_SAVE
            emit->ct_cur_obj, emit->ct_cur_raw_code,
            #endif
            emit->scope->scope_flags);
    }
}

bool mp_emit_bc_last_emit_was_return_value(emit_t *emit) {
    return emit->last_emit_was_return_value;
}

void mp_emit_bc_adjust_stack_size(emit_t *emit, mp_int_t delta) {
    if (emit->pass == MP_PASS_SCOPE) {
        return;
    }
    assert((mp_int_t)emit->stack_size + delta >= 0);
    emit->stack_size += delta;
    if (emit->stack_size > emit->scope->stack_size) {
        emit->scope->stack_size = emit->stack_size;
    }
    emit->last_emit_was_return_value = false;
}

static inline void emit_bc_pre(emit_t *emit, mp_int_t stack_size_delta) {
    mp_emit_bc_adjust_stack_size(emit, stack_size_delta);
}

void mp_emit_bc_set_source_line(emit_t *emit, mp_uint_t source_line) {
    //printf("source: line %d -> %d  offset %d -> %d\n", emit->last_source_line, source_line, emit->last_source_line_offset, emit->bytecode_offset);
#if MICROPY_ENABLE_SOURCE_LINE
    if (MP_STATE_VM(mp_optimise_value) >= 3) {
        // If we compile with -O3, don't store line numbers.
        return;
    }
    if (source_line > emit->last_source_line) {
        mp_uint_t bytes_to_skip = emit->bytecode_offset - emit->last_source_line_offset;
        mp_uint_t lines_to_skip = source_line - emit->last_source_line;
        emit_write_code_info_bytes_lines(emit, bytes_to_skip, lines_to_skip);
        emit->last_source_line_offset = emit->bytecode_offset;
        emit->last_source_line = source_line;
    }
#else
    (void)emit;
    (void)source_line;
#endif
}

void mp_emit_bc_label_assign(emit_t *emit, mp_uint_t l) {
    emit_bc_pre(emit, 0);
    if (emit->pass == MP_PASS_SCOPE) {
        return;
    }
    assert(l < emit->max_num_labels);
    if (emit->pass < MP_PASS_EMIT) {
        // assign label offset
        assert(emit->label_offsets[l] == (mp_uint_t)-1);
        emit->label_offsets[l] = emit->bytecode_offset;
    } else {
        // ensure label offset has not changed from MP_PASS_CODE_SIZE to MP_PASS_EMIT
        assert(emit->label_offsets[l] == emit->bytecode_offset);
    }
}

void mp_emit_bc_import_name(emit_t *emit, qstr qst) {
    emit_bc_pre(emit, -1);
    emit_write_bytecode_byte_qstr(emit, MP_BC_IMPORT_NAME, qst);
}

void mp_emit_bc_import_from(emit_t *emit, qstr qst) {
    emit_bc_pre(emit, 1);
    emit_write_bytecode_byte_qstr(emit, MP_BC_IMPORT_FROM, qst);
}

void mp_emit_bc_import_star(emit_t *emit) {
    emit_bc_pre(emit, -1);
    emit_write_bytecode_byte(emit, MP_BC_IMPORT_STAR);
}

void mp_emit_bc_load_const_tok(emit_t *emit, mp_token_kind_t tok) {
    emit_bc_pre(emit, 1);
    switch (tok) {
        case MP_TOKEN_KW_FALSE: emit_write_bytecode_byte(emit, MP_BC_LOAD_CONST_FALSE); break;
        case MP_TOKEN_KW_NONE: emit_write_bytecode_byte(emit, MP_BC_LOAD_CONST_NONE); break;
        case MP_TOKEN_KW_TRUE: emit_write_bytecode_byte(emit, MP_BC_LOAD_CONST_TRUE); break;
        default:
            assert(tok == MP_TOKEN_ELLIPSIS);
            emit_write_bytecode_byte_obj(emit, MP_BC_LOAD_CONST_OBJ, MP_OBJ_FROM_PTR(&mp_const_ellipsis_obj));
            break;
    }
}

void mp_emit_bc_load_const_small_int(emit_t *emit, mp_int_t arg) {
    emit_bc_pre(emit, 1);
    if (-16 <= arg && arg <= 47) {
        emit_write_bytecode_byte(emit, MP_BC_LOAD_CONST_SMALL_INT_MULTI + 16 + arg);
    } else {
        emit_write_bytecode_byte_int(emit, MP_BC_LOAD_CONST_SMALL_INT, arg);
    }
}

void mp_emit_bc_load_const_str(emit_t *emit, qstr qst) {
    emit_bc_pre(emit, 1);
    emit_write_bytecode_byte_qstr(emit, MP_BC_LOAD_CONST_STRING, qst);
}

void mp_emit_bc_load_const_obj(emit_t *emit, mp_obj_t obj) {
    emit_bc_pre(emit, 1);
    emit_write_bytecode_byte_obj(emit, MP_BC_LOAD_CONST_OBJ, obj);
}

void mp_emit_bc_load_null(emit_t *emit) {
    emit_bc_pre(emit, 1);
    emit_write_bytecode_byte(emit, MP_BC_LOAD_NULL);
}

void mp_emit_bc_load_fast(emit_t *emit, qstr qst, mp_uint_t local_num) {
    (void)qst;
    emit_bc_pre(emit, 1);
    if (local_num <= 15) {
        emit_write_bytecode_byte(emit, MP_BC_LOAD_FAST_MULTI + local_num);
    } else {
        emit_write_bytecode_byte_uint(emit, MP_BC_LOAD_FAST_N, local_num);
    }
}

void mp_emit_bc_load_deref(emit_t *emit, qstr qst, mp_uint_t local_num) {
    (void)qst;
    emit_bc_pre(emit, 1);
    emit_write_bytecode_byte_uint(emit, MP_BC_LOAD_DEREF, local_num);
}

void mp_emit_bc_load_name(emit_t *emit, qstr qst) {
    (void)qst;
    emit_bc_pre(emit, 1);
    emit_write_bytecode_byte_qstr(emit, MP_BC_LOAD_NAME, qst);
    if (MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE_DYNAMIC) {
        emit_write_bytecode_byte(emit, 0);
    }
}

void mp_emit_bc_load_global(emit_t *emit, qstr qst) {
    (void)qst;
    emit_bc_pre(emit, 1);
    emit_write_bytecode_byte_qstr(emit, MP_BC_LOAD_GLOBAL, qst);
    if (MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE_DYNAMIC) {
        emit_write_bytecode_byte(emit, 0);
    }
}

void mp_emit_bc_load_attr(emit_t *emit, qstr qst) {
    emit_bc_pre(emit, 0);
    emit_write_bytecode_byte_qstr(emit, MP_BC_LOAD_ATTR, qst);
    if (MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE_DYNAMIC) {
        emit_write_bytecode_byte(emit, 0);
    }
}

void mp_emit_bc_load_method(emit_t *emit, qstr qst, bool is_super) {
    emit_bc_pre(emit, 1 - 2 * is_super);
    emit_write_bytecode_byte_qstr(emit, is_super ? MP_BC_LOAD_SUPER_METHOD : MP_BC_LOAD_METHOD, qst);
}

void mp_emit_bc_load_build_class(emit_t *emit) {
    emit_bc_pre(emit, 1);
    emit_write_bytecode_byte(emit, MP_BC_LOAD_BUILD_CLASS);
}

void mp_emit_bc_load_subscr(emit_t *emit) {
    emit_bc_pre(emit, -1);
    emit_write_bytecode_byte(emit, MP_BC_LOAD_SUBSCR);
}

void mp_emit_bc_store_fast(emit_t *emit, qstr qst, mp_uint_t local_num) {
    (void)qst;
    emit_bc_pre(emit, -1);
    if (local_num <= 15) {
        emit_write_bytecode_byte(emit, MP_BC_STORE_FAST_MULTI + local_num);
    } else {
        emit_write_bytecode_byte_uint(emit, MP_BC_STORE_FAST_N, local_num);
    }
}

void mp_emit_bc_store_deref(emit_t *emit, qstr qst, mp_uint_t local_num) {
    (void)qst;
    emit_bc_pre(emit, -1);
    emit_write_bytecode_byte_uint(emit, MP_BC_STORE_DEREF, local_num);
}

void mp_emit_bc_store_name(emit_t *emit, qstr qst) {
    emit_bc_pre(emit, -1);
    emit_write_bytecode_byte_qstr(emit, MP_BC_STORE_NAME, qst);
}

void mp_emit_bc_store_global(emit_t *emit, qstr qst) {
    emit_bc_pre(emit, -1);
    emit_write_bytecode_byte_qstr(emit, MP_BC_STORE_GLOBAL, qst);
}

void mp_emit_bc_store_attr(emit_t *emit, qstr qst) {
    emit_bc_pre(emit, -2);
    emit_write_bytecode_byte_qstr(emit, MP_BC_STORE_ATTR, qst);
    if (MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE_DYNAMIC) {
        emit_write_bytecode_byte(emit, 0);
    }
}

void mp_emit_bc_store_subscr(emit_t *emit) {
    emit_bc_pre(emit, -3);
    emit_write_bytecode_byte(emit, MP_BC_STORE_SUBSCR);
}

void mp_emit_bc_delete_fast(emit_t *emit, qstr qst, mp_uint_t local_num) {
    (void)qst;
    emit_write_bytecode_byte_uint(emit, MP_BC_DELETE_FAST, local_num);
}

void mp_emit_bc_delete_deref(emit_t *emit, qstr qst, mp_uint_t local_num) {
    (void)qst;
    emit_write_bytecode_byte_uint(emit, MP_BC_DELETE_DEREF, local_num);
}

void mp_emit_bc_delete_name(emit_t *emit, qstr qst) {
    emit_bc_pre(emit, 0);
    emit_write_bytecode_byte_qstr(emit, MP_BC_DELETE_NAME, qst);
}

void mp_emit_bc_delete_global(emit_t *emit, qstr qst) {
    emit_bc_pre(emit, 0);
    emit_write_bytecode_byte_qstr(emit, MP_BC_DELETE_GLOBAL, qst);
}

void mp_emit_bc_delete_attr(emit_t *emit, qstr qst) {
    mp_emit_bc_load_null(emit);
    mp_emit_bc_rot_two(emit);
    mp_emit_bc_store_attr(emit, qst);
}

void mp_emit_bc_delete_subscr(emit_t *emit) {
    mp_emit_bc_load_null(emit);
    mp_emit_bc_rot_three(emit);
    mp_emit_bc_store_subscr(emit);
}

void mp_emit_bc_dup_top(emit_t *emit) {
    emit_bc_pre(emit, 1);
    emit_write_bytecode_byte(emit, MP_BC_DUP_TOP);
}

void mp_emit_bc_dup_top_two(emit_t *emit) {
    emit_bc_pre(emit, 2);
    emit_write_bytecode_byte(emit, MP_BC_DUP_TOP_TWO);
}

void mp_emit_bc_pop_top(emit_t *emit) {
    emit_bc_pre(emit, -1);
    emit_write_bytecode_byte(emit, MP_BC_POP_TOP);
}

void mp_emit_bc_rot_two(emit_t *emit) {
    emit_bc_pre(emit, 0);
    emit_write_bytecode_byte(emit, MP_BC_ROT_TWO);
}

void mp_emit_bc_rot_three(emit_t *emit) {
    emit_bc_pre(emit, 0);
    emit_write_bytecode_byte(emit, MP_BC_ROT_THREE);
}

void mp_emit_bc_jump(emit_t *emit, mp_uint_t label) {
    emit_bc_pre(emit, 0);
    emit_write_bytecode_byte_signed_label(emit, MP_BC_JUMP, label);
}

void mp_emit_bc_pop_jump_if(emit_t *emit, bool cond, mp_uint_t label) {
    emit_bc_pre(emit, -1);
    if (cond) {
        emit_write_bytecode_byte_signed_label(emit, MP_BC_POP_JUMP_IF_TRUE, label);
    } else {
        emit_write_bytecode_byte_signed_label(emit, MP_BC_POP_JUMP_IF_FALSE, label);
    }
}

void mp_emit_bc_jump_if_or_pop(emit_t *emit, bool cond, mp_uint_t label) {
    emit_bc_pre(emit, -1);
    if (cond) {
        emit_write_bytecode_byte_signed_label(emit, MP_BC_JUMP_IF_TRUE_OR_POP, label);
    } else {
        emit_write_bytecode_byte_signed_label(emit, MP_BC_JUMP_IF_FALSE_OR_POP, label);
    }
}

void mp_emit_bc_unwind_jump(emit_t *emit, mp_uint_t label, mp_uint_t except_depth) {
    if (except_depth == 0) {
        emit_bc_pre(emit, 0);
        if (label & MP_EMIT_BREAK_FROM_FOR) {
            // need to pop the iterator if we are breaking out of a for loop
            emit_write_bytecode_byte(emit, MP_BC_POP_TOP);
            // also pop the iter_buf
            for (size_t i = 0; i < MP_OBJ_ITER_BUF_NSLOTS - 1; ++i) {
                emit_write_bytecode_byte(emit, MP_BC_POP_TOP);
            }
        }
        emit_write_bytecode_byte_signed_label(emit, MP_BC_JUMP, label & ~MP_EMIT_BREAK_FROM_FOR);
    } else {
        emit_write_bytecode_byte_signed_label(emit, MP_BC_UNWIND_JUMP, label & ~MP_EMIT_BREAK_FROM_FOR);
        emit_write_bytecode_byte(emit, ((label & MP_EMIT_BREAK_FROM_FOR) ? 0x80 : 0) | except_depth);
    }
}

void mp_emit_bc_setup_with(emit_t *emit, mp_uint_t label) {
    // The SETUP_WITH opcode pops ctx_mgr from the top of the stack
    // and then pushes 3 entries: __exit__, ctx_mgr, as_value.
    emit_bc_pre(emit, 2);
    emit_write_bytecode_byte_unsigned_label(emit, MP_BC_SETUP_WITH, label);
}

void mp_emit_bc_with_cleanup(emit_t *emit, mp_uint_t label) {
    mp_emit_bc_pop_block(emit);
    mp_emit_bc_load_const_tok(emit, MP_TOKEN_KW_NONE);
    mp_emit_bc_label_assign(emit, label);
    emit_bc_pre(emit, 2); // ensure we have enough stack space to call the __exit__ method
    emit_write_bytecode_byte(emit, MP_BC_WITH_CLEANUP);
    emit_bc_pre(emit, -4); // cancel the 2 above, plus the 2 from mp_emit_bc_setup_with
}

void mp_emit_bc_setup_except(emit_t *emit, mp_uint_t label) {
    emit_bc_pre(emit, 0);
    emit_write_bytecode_byte_unsigned_label(emit, MP_BC_SETUP_EXCEPT, label);
}

void mp_emit_bc_setup_finally(emit_t *emit, mp_uint_t label) {
    emit_bc_pre(emit, 0);
    emit_write_bytecode_byte_unsigned_label(emit, MP_BC_SETUP_FINALLY, label);
}

void mp_emit_bc_end_finally(emit_t *emit) {
    emit_bc_pre(emit, -1);
    emit_write_bytecode_byte(emit, MP_BC_END_FINALLY);
}

void mp_emit_bc_get_iter(emit_t *emit, bool use_stack) {
    emit_bc_pre(emit, use_stack ? MP_OBJ_ITER_BUF_NSLOTS - 1 : 0);
    emit_write_bytecode_byte(emit, use_stack ? MP_BC_GET_ITER_STACK : MP_BC_GET_ITER);
}

void mp_emit_bc_for_iter(emit_t *emit, mp_uint_t label) {
    emit_bc_pre(emit, 1);
    emit_write_bytecode_byte_unsigned_label(emit, MP_BC_FOR_ITER, label);
}

void mp_emit_bc_for_iter_end(emit_t *emit) {
    emit_bc_pre(emit, -MP_OBJ_ITER_BUF_NSLOTS);
}

void mp_emit_bc_pop_block(emit_t *emit) {
    emit_bc_pre(emit, 0);
    emit_write_bytecode_byte(emit, MP_BC_POP_BLOCK);
}

void mp_emit_bc_pop_except(emit_t *emit) {
    emit_bc_pre(emit, 0);
    emit_write_bytecode_byte(emit, MP_BC_POP_EXCEPT);
}

void mp_emit_bc_unary_op(emit_t *emit, mp_unary_op_t op) {
    emit_bc_pre(emit, 0);
    emit_write_bytecode_byte(emit, MP_BC_UNARY_OP_MULTI + op);
}

void mp_emit_bc_binary_op(emit_t *emit, mp_binary_op_t op) {
    bool invert = false;
    if (op == MP_BINARY_OP_NOT_IN) {
        invert = true;
        op = MP_BINARY_OP_IN;
    } else if (op == MP_BINARY_OP_IS_NOT) {
        invert = true;
        op = MP_BINARY_OP_IS;
    }
    emit_bc_pre(emit, -1);
    emit_write_bytecode_byte(emit, MP_BC_BINARY_OP_MULTI + op);
    if (invert) {
        emit_bc_pre(emit, 0);
        emit_write_bytecode_byte(emit, MP_BC_UNARY_OP_MULTI + MP_UNARY_OP_NOT);
    }
}

void mp_emit_bc_build_tuple(emit_t *emit, mp_uint_t n_args) {
    emit_bc_pre(emit, 1 - n_args);
    emit_write_bytecode_byte_uint(emit, MP_BC_BUILD_TUPLE, n_args);
}

void mp_emit_bc_build_list(emit_t *emit, mp_uint_t n_args) {
    emit_bc_pre(emit, 1 - n_args);
    emit_write_bytecode_byte_uint(emit, MP_BC_BUILD_LIST, n_args);
}

void mp_emit_bc_build_map(emit_t *emit, mp_uint_t n_args) {
    emit_bc_pre(emit, 1);
    emit_write_bytecode_byte_uint(emit, MP_BC_BUILD_MAP, n_args);
}

void mp_emit_bc_store_map(emit_t *emit) {
    emit_bc_pre(emit, -2);
    emit_write_bytecode_byte(emit, MP_BC_STORE_MAP);
}

#if MICROPY_PY_BUILTINS_SET
void mp_emit_bc_build_set(emit_t *emit, mp_uint_t n_args) {
    emit_bc_pre(emit, 1 - n_args);
    emit_write_bytecode_byte_uint(emit, MP_BC_BUILD_SET, n_args);
}
#endif

#if MICROPY_PY_BUILTINS_SLICE
void mp_emit_bc_build_slice(emit_t *emit, mp_uint_t n_args) {
    emit_bc_pre(emit, 1 - n_args);
    emit_write_bytecode_byte_uint(emit, MP_BC_BUILD_SLICE, n_args);
}
#endif

void mp_emit_bc_store_comp(emit_t *emit, scope_kind_t kind, mp_uint_t collection_stack_index) {
    int t;
    int n;
    if (kind == SCOPE_LIST_COMP) {
        n = 0;
        t = 0;
    } else if (!MICROPY_PY_BUILTINS_SET || kind == SCOPE_DICT_COMP) {
        n = 1;
        t = 1;
    } else if (MICROPY_PY_BUILTINS_SET) {
        n = 0;
        t = 2;
    }
    emit_bc_pre(emit, -1 - n);
    // the lower 2 bits of the opcode argument indicate the collection type
    emit_write_bytecode_byte_uint(emit, MP_BC_STORE_COMP, ((collection_stack_index + n) << 2) | t);
}

void mp_emit_bc_unpack_sequence(emit_t *emit, mp_uint_t n_args) {
    emit_bc_pre(emit, -1 + n_args);
    emit_write_bytecode_byte_uint(emit, MP_BC_UNPACK_SEQUENCE, n_args);
}

void mp_emit_bc_unpack_ex(emit_t *emit, mp_uint_t n_left, mp_uint_t n_right) {
    emit_bc_pre(emit, -1 + n_left + n_right + 1);
    emit_write_bytecode_byte_uint(emit, MP_BC_UNPACK_EX, n_left | (n_right << 8));
}

void mp_emit_bc_make_function(emit_t *emit, scope_t *scope, mp_uint_t n_pos_defaults, mp_uint_t n_kw_defaults) {
    if (n_pos_defaults == 0 && n_kw_defaults == 0) {
        emit_bc_pre(emit, 1);
        emit_write_bytecode_byte_raw_code(emit, MP_BC_MAKE_FUNCTION, scope->raw_code);
    } else {
        emit_bc_pre(emit, -1);
        emit_write_bytecode_byte_raw_code(emit, MP_BC_MAKE_FUNCTION_DEFARGS, scope->raw_code);
    }
}

void mp_emit_bc_make_closure(emit_t *emit, scope_t *scope, mp_uint_t n_closed_over, mp_uint_t n_pos_defaults, mp_uint_t n_kw_defaults) {
    if (n_pos_defaults == 0 && n_kw_defaults == 0) {
        emit_bc_pre(emit, -n_closed_over + 1);
        emit_write_bytecode_byte_raw_code(emit, MP_BC_MAKE_CLOSURE, scope->raw_code);
        emit_write_bytecode_byte(emit, n_closed_over);
    } else {
        assert(n_closed_over <= 255);
        emit_bc_pre(emit, -2 - (mp_int_t)n_closed_over + 1);
        emit_write_bytecode_byte_raw_code(emit, MP_BC_MAKE_CLOSURE_DEFARGS, scope->raw_code);
        emit_write_bytecode_byte(emit, n_closed_over);
    }
}

STATIC void emit_bc_call_function_method_helper(emit_t *emit, mp_int_t stack_adj, mp_uint_t bytecode_base, mp_uint_t n_positional, mp_uint_t n_keyword, mp_uint_t star_flags) {
    if (star_flags) {
        emit_bc_pre(emit, stack_adj - (mp_int_t)n_positional - 2 * (mp_int_t)n_keyword - 2);
        emit_write_bytecode_byte_uint(emit, bytecode_base + 1, (n_keyword << 8) | n_positional); // TODO make it 2 separate uints?
    } else {
        emit_bc_pre(emit, stack_adj - (mp_int_t)n_positional - 2 * (mp_int_t)n_keyword);
        emit_write_bytecode_byte_uint(emit, bytecode_base, (n_keyword << 8) | n_positional); // TODO make it 2 separate uints?
    }
}

void mp_emit_bc_call_function(emit_t *emit, mp_uint_t n_positional, mp_uint_t n_keyword, mp_uint_t star_flags) {
    emit_bc_call_function_method_helper(emit, 0, MP_BC_CALL_FUNCTION, n_positional, n_keyword, star_flags);
}

void mp_emit_bc_call_method(emit_t *emit, mp_uint_t n_positional, mp_uint_t n_keyword, mp_uint_t star_flags) {
    emit_bc_call_function_method_helper(emit, -1, MP_BC_CALL_METHOD, n_positional, n_keyword, star_flags);
}

void mp_emit_bc_return_value(emit_t *emit) {
    emit_bc_pre(emit, -1);
    emit->last_emit_was_return_value = true;
    emit_write_bytecode_byte(emit, MP_BC_RETURN_VALUE);
}

void mp_emit_bc_raise_varargs(emit_t *emit, mp_uint_t n_args) {
    assert(n_args <= 2);
    emit_bc_pre(emit, -n_args);
    emit_write_bytecode_byte_byte(emit, MP_BC_RAISE_VARARGS, n_args);
}

void mp_emit_bc_yield_value(emit_t *emit) {
    emit_bc_pre(emit, 0);
    emit->scope->scope_flags |= MP_SCOPE_FLAG_GENERATOR;
    emit_write_bytecode_byte(emit, MP_BC_YIELD_VALUE);
}

void mp_emit_bc_yield_from(emit_t *emit) {
    emit_bc_pre(emit, -1);
    emit->scope->scope_flags |= MP_SCOPE_FLAG_GENERATOR;
    emit_write_bytecode_byte(emit, MP_BC_YIELD_FROM);
}

void mp_emit_bc_start_except_handler(emit_t *emit) {
    mp_emit_bc_adjust_stack_size(emit, 4); // stack adjust for the exception instance, +3 for possible UNWIND_JUMP state
}

void mp_emit_bc_end_except_handler(emit_t *emit) {
    mp_emit_bc_adjust_stack_size(emit, -3); // stack adjust
}

#if MICROPY_EMIT_NATIVE
const emit_method_table_t emit_bc_method_table = {
    NULL, // set_native_type is never called when emitting bytecode
    mp_emit_bc_start_pass,
    mp_emit_bc_end_pass,
    mp_emit_bc_last_emit_was_return_value,
    mp_emit_bc_adjust_stack_size,
    mp_emit_bc_set_source_line,

    {
        mp_emit_bc_load_fast,
        mp_emit_bc_load_deref,
        mp_emit_bc_load_name,
        mp_emit_bc_load_global,
    },
    {
        mp_emit_bc_store_fast,
        mp_emit_bc_store_deref,
        mp_emit_bc_store_name,
        mp_emit_bc_store_global,
    },
    {
        mp_emit_bc_delete_fast,
        mp_emit_bc_delete_deref,
        mp_emit_bc_delete_name,
        mp_emit_bc_delete_global,
    },

    mp_emit_bc_label_assign,
    mp_emit_bc_import_name,
    mp_emit_bc_import_from,
    mp_emit_bc_import_star,
    mp_emit_bc_load_const_tok,
    mp_emit_bc_load_const_small_int,
    mp_emit_bc_load_const_str,
    mp_emit_bc_load_const_obj,
    mp_emit_bc_load_null,
    mp_emit_bc_load_attr,
    mp_emit_bc_load_method,
    mp_emit_bc_load_build_class,
    mp_emit_bc_load_subscr,
    mp_emit_bc_store_attr,
    mp_emit_bc_store_subscr,
    mp_emit_bc_delete_attr,
    mp_emit_bc_delete_subscr,
    mp_emit_bc_dup_top,
    mp_emit_bc_dup_top_two,
    mp_emit_bc_pop_top,
    mp_emit_bc_rot_two,
    mp_emit_bc_rot_three,
    mp_emit_bc_jump,
    mp_emit_bc_pop_jump_if,
    mp_emit_bc_jump_if_or_pop,
    mp_emit_bc_unwind_jump,
    mp_emit_bc_unwind_jump,
    mp_emit_bc_setup_with,
    mp_emit_bc_with_cleanup,
    mp_emit_bc_setup_except,
    mp_emit_bc_setup_finally,
    mp_emit_bc_end_finally,
    mp_emit_bc_get_iter,
    mp_emit_bc_for_iter,
    mp_emit_bc_for_iter_end,
    mp_emit_bc_pop_block,
    mp_emit_bc_pop_except,
    mp_emit_bc_unary_op,
    mp_emit_bc_binary_op,
    mp_emit_bc_build_tuple,
    mp_emit_bc_build_list,
    mp_emit_bc_build_map,
    mp_emit_bc_store_map,
    #if MICROPY_PY_BUILTINS_SET
    mp_emit_bc_build_set,
    #endif
    #if MICROPY_PY_BUILTINS_SLICE
    mp_emit_bc_build_slice,
    #endif
    mp_emit_bc_store_comp,
    mp_emit_bc_unpack_sequence,
    mp_emit_bc_unpack_ex,
    mp_emit_bc_make_function,
    mp_emit_bc_make_closure,
    mp_emit_bc_call_function,
    mp_emit_bc_call_method,
    mp_emit_bc_return_value,
    mp_emit_bc_raise_varargs,
    mp_emit_bc_yield_value,
    mp_emit_bc_yield_from,

    mp_emit_bc_start_except_handler,
    mp_emit_bc_end_except_handler,
};
#else
const mp_emit_method_table_id_ops_t mp_emit_bc_method_table_load_id_ops = {
    mp_emit_bc_load_fast,
    mp_emit_bc_load_deref,
    mp_emit_bc_load_name,
    mp_emit_bc_load_global,
};

const mp_emit_method_table_id_ops_t mp_emit_bc_method_table_store_id_ops = {
    mp_emit_bc_store_fast,
    mp_emit_bc_store_deref,
    mp_emit_bc_store_name,
    mp_emit_bc_store_global,
};

const mp_emit_method_table_id_ops_t mp_emit_bc_method_table_delete_id_ops = {
    mp_emit_bc_delete_fast,
    mp_emit_bc_delete_deref,
    mp_emit_bc_delete_name,
    mp_emit_bc_delete_global,
};
#endif

#endif //MICROPY_ENABLE_COMPILER
