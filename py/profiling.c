
/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) SatoshiLabs
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

#include "py/runtime.h"

#if MICROPY_PY_SYS_SETTRACE

#include "py/profiling.h"
#include "py/objstr.h"
#include "py/objfun.h"
#include "py/emitglue.h"
#include "py/bc0.h"
#include "py/gc.h"

#include <assert.h>

#define prof_trace_cb MP_STATE_THREAD(prof_trace_callback)

STATIC uint prof_bytecode_lineno(const mp_raw_code_t *rc, size_t bc) {
    const mp_bytecode_prelude_t *prelude = &rc->prelude;
    const byte *ip = prelude->line_info;
    size_t source_line = 1;
    size_t c;

    bc = (bc + (prelude->bytecode - prelude->code_info)) - prelude->code_info_size;

    // The following loop was taken from vm.c
    while ((c = *ip)) {
        size_t b, l;
        if ((c & 0x80) == 0) {
            // 0b0LLBBBBB encoding
            b = c & 0x1f;
            l = c >> 5;
            ip += 1;
        } else {
            // 0b1LLLBBBB 0bLLLLLLLL encoding (l's LSB in second byte)
            b = c & 0xf;
            l = ((c << 4) & 0x700) | ip[1];
            ip += 2;
        }
        if (bc >= b) {
            bc -= b;
            source_line += l;
        } else {
            // found source line corresponding to bytecode offset
            break;
        }
    }

    return source_line;
}

void prof_extract_prelude(const byte *bytecode, mp_bytecode_prelude_t *prelude) {
    const byte *ip = bytecode;

    prelude->n_state = mp_decode_uint(&ip); // ip++
    prelude->n_exc_stack = mp_decode_uint(&ip); // ip++
    prelude->scope_flags = *ip++;
    prelude->n_pos_args = *ip++;
    prelude->n_kwonly_args = *ip++;
    prelude->n_def_pos_args = *ip++;

    prelude->code_info = ip;
    size_t code_info_size = mp_decode_uint(&ip); // ip++
    prelude->code_info_size = code_info_size;

    #if MICROPY_PERSISTENT_CODE
    qstr block_name = ip[0] | (ip[1] << 8);
    qstr source_file = ip[2] | (ip[3] << 8);
    ip += 4;
    #else
    qstr block_name = mp_decode_uint(&ip); // ip++
    qstr source_file = mp_decode_uint(&ip); // ip++
    #endif
    prelude->qstr_block_name = block_name;
    prelude->qstr_source_file = source_file;

    prelude->line_info = ip;

    prelude->locals = (void*)(
        (size_t)(prelude->code_info + code_info_size)
         & (~(sizeof(uint)-1)) // word align
    );

    ip = prelude->locals;
    while (*ip++ != 255);
    prelude->bytecode = ip;
}


/******************************************************************************/
// code object

STATIC void code_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_code_t *o = MP_OBJ_TO_PTR(o_in);
    const mp_raw_code_t *rc = o->rc;
    const mp_bytecode_prelude_t *prelude = &rc->prelude;
    mp_printf(print,
        "<code object %q at 0x%p, file \"%q\", line %d>",
        prelude->qstr_block_name,
        o,
        prelude->qstr_source_file,
        rc->line_of_definition
    );
}

STATIC mp_obj_tuple_t* code_consts(const mp_raw_code_t *rc) {

    const mp_bytecode_prelude_t *prelude = &rc->prelude;
    int start = prelude->n_pos_args + prelude->n_kwonly_args + rc->n_obj;
    int stop  = prelude->n_pos_args + prelude->n_kwonly_args + rc->n_obj + rc->n_raw_code;
    mp_obj_tuple_t *consts = mp_obj_new_tuple(stop - start + 1, NULL);
    size_t const_no = 0;
    int i = 0;

    for (i = start; i < stop; i++ ) {
        consts->items[const_no++] = mp_obj_new_code((const mp_raw_code_t*)rc->const_table[i]);
    }

    consts->items[const_no++] = mp_const_none;

    return consts;
}

STATIC mp_obj_t raw_code_lnotab(const mp_raw_code_t *rc) {
    // const mp_bytecode_prelude_t *prelude = &rc->prelude;
    uint start = 0;
    uint stop = rc->fun_data_len - start;

    uint last_lineno = prof_bytecode_lineno(rc, start);
    uint lasti = 0;

    const uint buffer_chunk_size = (stop-start) >> 2; // heuristic magic
    uint buffer_size = buffer_chunk_size;
    byte *buffer = m_new(byte, buffer_size);
    uint buffer_index = 0;

    int i;
    for (i = start; i < stop; i ++) {
        uint lineno = prof_bytecode_lineno(rc, i);
        size_t line_diff = lineno - last_lineno;
        if (line_diff > 0) {
            uint instr_diff = (i - start) - lasti;

            assert(instr_diff < 256);
            assert(line_diff < 256);

            if (buffer_index + 2 > buffer_size) {
                #if MICROPY_MALLOC_USES_ALLOCATED_SIZE
                buffer = m_renew(byte, buffer, buffer_size, buffer_size + buffer_chunk_size);
                #else
                buffer = m_realloc(buffer, buffer_size, buffer_size + buffer_chunk_size);
                #endif
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

STATIC void code_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    if (dest[0] != MP_OBJ_NULL) {
        // not load attribute
        return;
    }
    mp_obj_code_t *o = MP_OBJ_TO_PTR(self_in);
    const mp_raw_code_t *rc = o->rc;
    const mp_bytecode_prelude_t *prelude = &rc->prelude;
    switch(attr) {
        case MP_QSTR_co_code:
            dest[0] = mp_obj_new_bytes(
                (void*)prelude->bytecode,
                rc->fun_data_len - (prelude->bytecode - (const byte*)rc->fun_data)
            );
            break;
        case MP_QSTR_co_consts:
            dest[0] = MP_OBJ_FROM_PTR(code_consts(rc));
            break;
        case MP_QSTR_co_filename:
            dest[0] = MP_OBJ_NEW_QSTR(prelude->qstr_source_file);
            break;
        case MP_QSTR_co_firstlineno:
            dest[0] = MP_OBJ_NEW_SMALL_INT(prof_bytecode_lineno(rc, 0));
            break;
        case MP_QSTR_co_name:
            dest[0] = MP_OBJ_NEW_QSTR(prelude->qstr_block_name);
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

const mp_obj_type_t mp_type_code = {
    { &mp_type_type },
    .name = MP_QSTR_code,
    .print = code_print,
    .unary_op = mp_generic_unary_op,
    .attr = code_attr,
};

mp_obj_t mp_obj_new_code(const mp_raw_code_t *rc) {
    mp_obj_code_t *o = m_new_obj(mp_obj_code_t);
    o->base.type = &mp_type_code;
    o->rc = rc;
    o->dict_locals = mp_locals_get(); // this is a wrong! how to do this properly?
    o->lnotab = NULL;
    return MP_OBJ_FROM_PTR(o);
}

/******************************************************************************/
// frame object

STATIC void frame_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_frame_t *frame = MP_OBJ_TO_PTR(o_in);
    mp_obj_code_t *code = frame->code;
    const mp_raw_code_t *rc = code->rc;
    const mp_bytecode_prelude_t *prelude = &rc->prelude;
    mp_printf(print,
        "<frame at 0x%p, file '%q', line %d, code %q>",
        frame,
        prelude->qstr_source_file,
        frame->lineno,
        prelude->qstr_block_name
    );
}

STATIC void frame_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    if (dest[0] != MP_OBJ_NULL) {
        // not load attribute
        return;
    }

    mp_obj_frame_t *o = MP_OBJ_TO_PTR(self_in);

    switch(attr) {
        case MP_QSTR_f_back:
            dest[0] = mp_const_none;
            if (o->code_state->prev_state) {
                dest[0] = o->code_state->prev_state->frame;
            }
            break;
        case MP_QSTR_f_code:
            dest[0] = o->code;
            break;
        case MP_QSTR_f_globals:
            dest[0] = o->code_state->fun_bc->globals;
            break;
        case MP_QSTR_f_lasti:
            dest[0] = MP_OBJ_NEW_SMALL_INT(o->lasti);
            break;
        case MP_QSTR_f_lineno:
            dest[0] = MP_OBJ_NEW_SMALL_INT(o->lineno);
            break;
    }
}

const mp_obj_type_t mp_type_frame = {
    { &mp_type_type },
    .name = MP_QSTR_frame,
    .print = frame_print,
    .unary_op = mp_generic_unary_op,
    .attr = frame_attr,
};

mp_obj_t mp_obj_new_frame(const mp_code_state_t *code_state) {

    bool gc_is_locked_tmp = gc_is_locked();
    if (gc_is_locked_tmp) {
        gc_unlock();
    }

    mp_obj_frame_t *o = m_new_obj(mp_obj_frame_t);
    mp_obj_code_t *code = o->code = mp_obj_new_code(code_state->fun_bc->rc);
    const mp_raw_code_t *rc = code->rc;
    const mp_bytecode_prelude_t *prelude = &rc->prelude;
    o->code_state = code_state;
    o->base.type = &mp_type_frame;
    o->back = NULL;
    o->code = code;
    o->lasti = code_state->ip - prelude->bytecode;
    o->lineno = prof_bytecode_lineno(rc, o->lasti);
    o->trace_opcodes = false;
    o->callback = NULL;

    if (gc_is_locked_tmp) {
        gc_lock();
    }

    return MP_OBJ_FROM_PTR(o);
}


/******************************************************************************/
// trace logic

typedef struct {
    struct _mp_obj_frame_t * frame;
    mp_obj_t event;
    mp_obj_t arg;
} prof_callback_args_t;

STATIC mp_obj_t prof_callback_invoke(mp_obj_t callback, prof_callback_args_t *args) {

    assert(mp_obj_is_callable(callback));

    prof_is_executing = true;

    mp_obj_t a[3] = {args->frame, args->event, args->arg};
    mp_obj_t top = mp_call_function_n_kw(callback, 3, 0, a);

    prof_is_executing = false;

    if (MP_STATE_VM(mp_pending_exception) != MP_OBJ_NULL) {
        mp_obj_t obj = MP_STATE_VM(mp_pending_exception);
        MP_STATE_VM(mp_pending_exception) = MP_OBJ_NULL;
        nlr_raise(obj);
    }
    return top;
}

mp_obj_t prof_settrace(mp_obj_t callback) {
    if (mp_obj_is_callable(callback)) {
        prof_trace_cb = callback;
    } else {
        prof_trace_cb = NULL;
    }
    return mp_const_none;
}

mp_obj_t prof_frame_enter(mp_code_state_t *code_state) {

    assert(!prof_is_executing);

    mp_obj_frame_t *frame = MP_OBJ_TO_PTR(mp_obj_new_frame(code_state));
    if (code_state->prev_state && code_state->frame == NULL) {
        // We are entering not-yet-traced frame
        // which means it's a CALL event (not a GENERATOR)
        // so set the function definition line.
        const mp_raw_code_t *rc = code_state->fun_bc->rc;
        frame->lineno = rc->line_of_definition;
        if (!rc->line_of_definition) {
            frame->lineno = prof_bytecode_lineno(rc, 0);
        }
    }
    code_state->frame = MP_OBJ_FROM_PTR(frame);

    if (!prof_trace_cb) {
        return NULL;
    }

    mp_obj_t top;
    prof_callback_args_t _args, *args=&_args;
    args->frame = code_state->frame;

    // SETTRACE event CALL
    args->event = MP_ROM_QSTR(MP_QSTR_call);
    args->arg = mp_const_none;
    top = prof_callback_invoke(prof_trace_cb, args);

    code_state->frame->callback = mp_obj_is_callable(top) ? top : NULL;

    // Invalidate the last executed line number so the LINE trace can trigger after this CALL.
    frame->lineno = 0;

    return top;
}

mp_obj_t prof_frame_update(const mp_code_state_t *code_state) {

    mp_obj_frame_t *frame = code_state->frame;
    assert(frame != NULL);

    mp_obj_frame_t *o = frame;
    mp_obj_code_t *code = o->code;
    const mp_raw_code_t *rc = code->rc;
    const mp_bytecode_prelude_t *prelude = &rc->prelude;

    assert(o->code_state == code_state);

    o->lasti = code_state->ip - prelude->bytecode;
    o->lineno = prof_bytecode_lineno(rc, o->lasti);

    return MP_OBJ_FROM_PTR(o);
}

mp_obj_t prof_instr_tick(mp_code_state_t *code_state, bool is_exception) {

    // Detecet execution recursion.
    assert(!prof_is_executing);
    assert(code_state->frame);
    assert(mp_obj_get_type(code_state->frame) == &mp_type_frame);

    // Detecet data recursion.
    assert(code_state != code_state->prev_state);

    mp_obj_t top = mp_const_none;
    mp_obj_t callback = code_state->frame->callback;

    prof_callback_args_t _args, *args=&_args;
    args->frame = code_state->frame;
    args->event = mp_const_none;
    args->arg = mp_const_none;

    // Call event's are handled inside prof_frame_enter

    // SETTRACE event EXCEPTION
    if (is_exception) {
        args->event = MP_ROM_QSTR(MP_QSTR_exception);
        top = prof_callback_invoke(callback, args);
        return top;
    }

    // SETTRACE event LINE
    const mp_raw_code_t *rc = code_state->fun_bc->rc;
    const mp_bytecode_prelude_t *prelude = &rc->prelude;
    size_t prev_line_no = args->frame->lineno;
    size_t current_line_no = prof_bytecode_lineno(rc, code_state->ip - prelude->bytecode);
    if (prev_line_no != current_line_no) {
        args->frame->lineno = current_line_no;
        args->event = MP_ROM_QSTR(MP_QSTR_line);
        top = prof_callback_invoke(callback, args);
    }

    // SETTRACE event RETURN
    const byte *ip = code_state->ip;
    if (*ip == MP_BC_RETURN_VALUE || *ip == MP_BC_YIELD_VALUE) {
        args->event = MP_ROM_QSTR(MP_QSTR_return);
        top = prof_callback_invoke(callback, args);
        if (code_state->prev_state && *ip == MP_BC_RETURN_VALUE) {
            code_state->frame->callback = NULL;
        }
    }

    // SETTRACE event OPCODE
    // TODO: frame.f_trace_opcodes=True
    if (false) {
        args->event = MP_ROM_QSTR(MP_QSTR_opcode);
    }

    return top;
}


/******************************************************************************/
// DEBUG

// This section is for debugging the settrace feature itself
// and should never make it in to any public build.
// The code structure for this block was taken from the showbc.c
// so don't even use this as a reference.
// To enable this find the definition
// enable `PROF_INSTR_DEBUG_PRINT_ENABLE` in the header file `profiling.h`.
#if PROF_INSTR_DEBUG_PRINT_ENABLE

#include <stdio.h>
#include <string.h>

#define DECODE_UINT { \
    unum = 0; \
    do { \
        unum = (unum << 7) + (*ip & 0x7f); \
    } while ((*ip++ & 0x80) != 0); \
}
#define DECODE_ULABEL do { unum = (ip[0] | (ip[1] << 8)); ip += 2; } while (0)
#define DECODE_SLABEL do { unum = (ip[0] | (ip[1] << 8)) - 0x8000; ip += 2; } while (0)

#define DECODE_QSTR \
    qst = ip[0] | ip[1] << 8; \
    ip += 2;
#define DECODE_PTR \
    DECODE_UINT; \
    ptr = (const byte*)const_table[unum]
#define DECODE_OBJ \
    DECODE_UINT; \
    obj = (mp_obj_t)const_table[unum]


const byte *prof_opcode_decode(const byte *ip, const mp_uint_t *const_table, mp_dis_instruction_t *instruction) {
    mp_uint_t unum;
    const byte* ptr;
    mp_obj_t obj;
    qstr qst;

    instruction->qstr_opname = MP_QSTR_;
    instruction->arg = 0;
    instruction->argobj= mp_const_none;
    instruction->argobjex_cache = mp_const_none;

    switch (*ip++) {
        case MP_BC_LOAD_CONST_FALSE:
            instruction->qstr_opname = MP_QSTR_LOAD_CONST_FALSE;
            break;

        case MP_BC_LOAD_CONST_NONE:
            instruction->qstr_opname = MP_QSTR_LOAD_CONST_NONE;
            break;

        case MP_BC_LOAD_CONST_TRUE:
            instruction->qstr_opname = MP_QSTR_LOAD_CONST_TRUE;
            break;

        case MP_BC_LOAD_CONST_SMALL_INT: {
            mp_int_t num = 0;
            if ((ip[0] & 0x40) != 0) {
                // Number is negative
                num--;
            }
            do {
                num = (num << 7) | (*ip & 0x7f);
            } while ((*ip++ & 0x80) != 0);
            instruction->qstr_opname = MP_QSTR_LOAD_CONST_SMALL_INT;
            instruction->arg = num;
            break;
        }

        case MP_BC_LOAD_CONST_STRING:
            DECODE_QSTR;
            instruction->qstr_opname = MP_QSTR_LOAD_CONST_STRING;
            instruction->arg = qst;
            instruction->argobj= MP_OBJ_NEW_QSTR(qst);
            break;

        case MP_BC_LOAD_CONST_OBJ:
            DECODE_OBJ;
            instruction->qstr_opname = MP_QSTR_LOAD_CONST_OBJ;
            instruction->arg = unum;
            instruction->argobj= obj;
            break;

        case MP_BC_LOAD_NULL:
            instruction->qstr_opname = MP_QSTR_LOAD_NULL;
            break;

        case MP_BC_LOAD_FAST_N:
            DECODE_UINT;
            instruction->qstr_opname = MP_QSTR_LOAD_FAST_N;
            instruction->arg = unum;
            break;

        case MP_BC_LOAD_DEREF:
            DECODE_UINT;
            instruction->qstr_opname = MP_QSTR_LOAD_DEREF;
            instruction->arg = unum;
            break;

        case MP_BC_LOAD_NAME:
            DECODE_QSTR;
            instruction->qstr_opname = MP_QSTR_LOAD_NAME;
            instruction->arg = qst;
            instruction->argobj= MP_OBJ_NEW_QSTR(qst);
            if (MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE) {
                instruction->argobjex_cache = MP_OBJ_NEW_SMALL_INT(*ip++);
            }
            break;

        case MP_BC_LOAD_GLOBAL:
            DECODE_QSTR;
            instruction->qstr_opname = MP_QSTR_LOAD_GLOBAL;
            instruction->arg = qst;
            instruction->argobj= MP_OBJ_NEW_QSTR(qst);
            if (MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE) {
                instruction->argobjex_cache = MP_OBJ_NEW_SMALL_INT(*ip++);
            }
            break;

        case MP_BC_LOAD_ATTR:
            DECODE_QSTR;
            instruction->qstr_opname = MP_QSTR_LOAD_ATTR;
            instruction->arg = qst;
            instruction->argobj= MP_OBJ_NEW_QSTR(qst);
            if (MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE) {
                instruction->argobjex_cache = MP_OBJ_NEW_SMALL_INT(*ip++);
            }
            break;

        case MP_BC_LOAD_METHOD:
            DECODE_QSTR;
            instruction->qstr_opname = MP_QSTR_LOAD_METHOD;
            instruction->arg = qst;
            instruction->argobj= MP_OBJ_NEW_QSTR(qst);
            break;

        case MP_BC_LOAD_SUPER_METHOD:
            DECODE_QSTR;
            instruction->qstr_opname = MP_QSTR_LOAD_SUPER_METHOD;
            instruction->arg = qst;
            instruction->argobj= MP_OBJ_NEW_QSTR(qst);
            break;

        case MP_BC_LOAD_BUILD_CLASS:
            instruction->qstr_opname = MP_QSTR_LOAD_BUILD_CLASS;
            break;

        case MP_BC_LOAD_SUBSCR:
            instruction->qstr_opname = MP_QSTR_LOAD_SUBSCR;
            break;

        case MP_BC_STORE_FAST_N:
            DECODE_UINT;
            instruction->qstr_opname = MP_QSTR_STORE_FAST_N;
            instruction->arg = unum;
            break;

        case MP_BC_STORE_DEREF:
            DECODE_UINT;
            instruction->qstr_opname = MP_QSTR_STORE_DEREF;
            instruction->arg = unum;
            break;

        case MP_BC_STORE_NAME:
            DECODE_QSTR;
            instruction->qstr_opname = MP_QSTR_STORE_NAME;
            instruction->arg = qst;
            instruction->argobj= MP_OBJ_NEW_QSTR(qst);
            break;

        case MP_BC_STORE_GLOBAL:
            DECODE_QSTR;
            instruction->qstr_opname = MP_QSTR_STORE_GLOBAL;
            instruction->arg = qst;
            instruction->argobj= MP_OBJ_NEW_QSTR(qst);
            break;

        case MP_BC_STORE_ATTR:
            DECODE_QSTR;
            instruction->qstr_opname = MP_QSTR_STORE_ATTR;
            instruction->arg = qst;
            instruction->argobj= MP_OBJ_NEW_QSTR(qst);
            if (MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE) {
                instruction->argobjex_cache = MP_OBJ_NEW_SMALL_INT(*ip++);
            }
            break;

        case MP_BC_STORE_SUBSCR:
            instruction->qstr_opname = MP_QSTR_STORE_SUBSCR;
            break;

        case MP_BC_DELETE_FAST:
            DECODE_UINT;
            instruction->qstr_opname = MP_QSTR_DELETE_FAST;
            instruction->arg = unum;
            break;

        case MP_BC_DELETE_DEREF:
            DECODE_UINT;
            instruction->qstr_opname = MP_QSTR_DELETE_DEREF;
            instruction->arg = unum;
            break;

        case MP_BC_DELETE_NAME:
            DECODE_QSTR;
            instruction->qstr_opname = MP_QSTR_DELETE_NAME;
            instruction->arg = qst;
            instruction->argobj= MP_OBJ_NEW_QSTR(qst);
            break;

        case MP_BC_DELETE_GLOBAL:
            DECODE_QSTR;
            instruction->qstr_opname = MP_QSTR_DELETE_GLOBAL;
            instruction->arg = qst;
            instruction->argobj= MP_OBJ_NEW_QSTR(qst);
            break;

        case MP_BC_DUP_TOP:
            instruction->qstr_opname = MP_QSTR_DUP_TOP;
            break;

        case MP_BC_DUP_TOP_TWO:
            instruction->qstr_opname = MP_QSTR_DUP_TOP_TWO;
            break;

        case MP_BC_POP_TOP:
            instruction->qstr_opname = MP_QSTR_POP_TOP;
            break;

        case MP_BC_ROT_TWO:
            instruction->qstr_opname = MP_QSTR_ROT_TWO;
            break;

        case MP_BC_ROT_THREE:
            instruction->qstr_opname = MP_QSTR_ROT_THREE;
            break;

        case MP_BC_JUMP:
            DECODE_SLABEL;
            instruction->qstr_opname = MP_QSTR_JUMP;
            instruction->arg = unum;
            break;

        case MP_BC_POP_JUMP_IF_TRUE:
            DECODE_SLABEL;
            instruction->qstr_opname = MP_QSTR_POP_JUMP_IF_TRUE;
            instruction->arg = unum;
            break;

        case MP_BC_POP_JUMP_IF_FALSE:
            DECODE_SLABEL;
            instruction->qstr_opname = MP_QSTR_POP_JUMP_IF_FALSE;
            instruction->arg = unum;
            break;

        case MP_BC_JUMP_IF_TRUE_OR_POP:
            DECODE_SLABEL;
            instruction->qstr_opname = MP_QSTR_JUMP_IF_TRUE_OR_POP;
            instruction->arg = unum;
            break;

        case MP_BC_JUMP_IF_FALSE_OR_POP:
            DECODE_SLABEL;
            instruction->qstr_opname = MP_QSTR_JUMP_IF_FALSE_OR_POP;
            instruction->arg = unum;
            break;

        case MP_BC_SETUP_WITH:
            DECODE_ULABEL; // loop-like labels are always forward
            instruction->qstr_opname = MP_QSTR_SETUP_WITH;
            instruction->arg = unum;
            break;

        case MP_BC_WITH_CLEANUP:
            instruction->qstr_opname = MP_QSTR_WITH_CLEANUP;
            break;

        case MP_BC_UNWIND_JUMP:
            DECODE_SLABEL;
            instruction->qstr_opname = MP_QSTR_UNWIND_JUMP;
            instruction->arg = unum;
            break;

        case MP_BC_SETUP_EXCEPT:
            DECODE_ULABEL; // except labels are always forward
            instruction->qstr_opname = MP_QSTR_SETUP_EXCEPT;
            instruction->arg = unum;
            break;

        case MP_BC_SETUP_FINALLY:
            DECODE_ULABEL; // except labels are always forward
            instruction->qstr_opname = MP_QSTR_SETUP_FINALLY;
            instruction->arg = unum;
            break;

        case MP_BC_END_FINALLY:
            // if TOS is an exception, reraises the exception (3 values on TOS)
            // if TOS is an integer, does something else
            // if TOS is None, just pops it and continues
            // else error
            instruction->qstr_opname = MP_QSTR_END_FINALLY;
            break;

        case MP_BC_GET_ITER:
            instruction->qstr_opname = MP_QSTR_GET_ITER;
            break;

        case MP_BC_GET_ITER_STACK:
            instruction->qstr_opname = MP_QSTR_GET_ITER_STACK;
            break;

        case MP_BC_FOR_ITER:
            DECODE_ULABEL; // the jump offset if iteration finishes; for labels are always forward
            instruction->qstr_opname = MP_QSTR_FOR_ITER;
            instruction->arg = unum;
            break;

        case MP_BC_BUILD_TUPLE:
            DECODE_UINT;
            instruction->qstr_opname = MP_QSTR_BUILD_TUPLE;
            instruction->arg = unum;
            break;

        case MP_BC_BUILD_LIST:
            DECODE_UINT;
            instruction->qstr_opname = MP_QSTR_BUILD_LIST;
            instruction->arg = unum;
            break;

        case MP_BC_BUILD_MAP:
            DECODE_UINT;
            instruction->qstr_opname = MP_QSTR_BUILD_MAP;
            instruction->arg = unum;
            break;

        case MP_BC_STORE_MAP:
            instruction->qstr_opname = MP_QSTR_STORE_MAP;
            break;

        case MP_BC_BUILD_SET:
            DECODE_UINT;
            instruction->qstr_opname = MP_QSTR_BUILD_SET;
            instruction->arg = unum;
            break;

#if MICROPY_PY_BUILTINS_SLICE
        case MP_BC_BUILD_SLICE:
            DECODE_UINT;
            instruction->qstr_opname = MP_QSTR_BUILD_SLICE;
            instruction->arg = unum;
            break;
#endif

        case MP_BC_STORE_COMP:
            DECODE_UINT;
            instruction->qstr_opname = MP_QSTR_STORE_COMP;
            instruction->arg = unum;
            break;

        case MP_BC_UNPACK_SEQUENCE:
            DECODE_UINT;
            instruction->qstr_opname = MP_QSTR_UNPACK_SEQUENCE;
            instruction->arg = unum;
            break;

        case MP_BC_UNPACK_EX:
            DECODE_UINT;
            instruction->qstr_opname = MP_QSTR_UNPACK_EX;
            instruction->arg = unum;
            break;

        case MP_BC_MAKE_FUNCTION:
            DECODE_PTR;
            instruction->qstr_opname = MP_QSTR_MAKE_FUNCTION;
            instruction->arg = unum;
            instruction->argobj= mp_obj_new_int_from_ull((uint64_t)ptr);
            break;

        case MP_BC_MAKE_FUNCTION_DEFARGS:
            DECODE_PTR;
            instruction->qstr_opname = MP_QSTR_MAKE_FUNCTION_DEFARGS;
            instruction->arg = unum;
            instruction->argobj= mp_obj_new_int_from_ull((uint64_t)ptr);
            break;

        case MP_BC_MAKE_CLOSURE: {
            DECODE_PTR;
            mp_uint_t n_closed_over = *ip++;
            instruction->qstr_opname = MP_QSTR_MAKE_CLOSURE;
            instruction->arg = unum;
            instruction->argobj= mp_obj_new_int_from_ull((uint64_t)ptr);
            instruction->argobjex_cache = MP_OBJ_NEW_SMALL_INT(n_closed_over);
            break;
        }

        case MP_BC_MAKE_CLOSURE_DEFARGS: {
            DECODE_PTR;
            mp_uint_t n_closed_over = *ip++;
            instruction->qstr_opname = MP_QSTR_MAKE_CLOSURE_DEFARGS;
            instruction->arg = unum;
            instruction->argobj= mp_obj_new_int_from_ull((uint64_t)ptr);
            instruction->argobjex_cache = MP_OBJ_NEW_SMALL_INT(n_closed_over);
            break;
        }

        case MP_BC_CALL_FUNCTION:
            DECODE_UINT;
            instruction->qstr_opname = MP_QSTR_CALL_FUNCTION;
            instruction->arg = unum & 0xff;
            instruction->argobjex_cache = MP_OBJ_NEW_SMALL_INT((unum >> 8) & 0xff);
            break;

        case MP_BC_CALL_FUNCTION_VAR_KW:
            DECODE_UINT;
            instruction->qstr_opname = MP_QSTR_CALL_FUNCTION_VAR_KW;
            instruction->arg = unum & 0xff;
            instruction->argobjex_cache = MP_OBJ_NEW_SMALL_INT((unum >> 8) & 0xff);
            break;

        case MP_BC_CALL_METHOD:
            DECODE_UINT;
            instruction->qstr_opname = MP_QSTR_CALL_METHOD;
            instruction->arg = unum & 0xff;
            instruction->argobjex_cache = MP_OBJ_NEW_SMALL_INT((unum >> 8) & 0xff);
            break;

        case MP_BC_CALL_METHOD_VAR_KW:
            DECODE_UINT;
            instruction->qstr_opname = MP_QSTR_CALL_METHOD_VAR_KW;
            instruction->arg = unum & 0xff;
            instruction->argobjex_cache = MP_OBJ_NEW_SMALL_INT((unum >> 8) & 0xff);
            break;

        case MP_BC_RETURN_VALUE:
            instruction->qstr_opname = MP_QSTR_RETURN_VALUE;
            break;

        case MP_BC_RAISE_VARARGS:
            unum = *ip++;
            instruction->qstr_opname = MP_QSTR_RAISE_VARARGS;
            instruction->arg = unum;
            break;

        case MP_BC_YIELD_VALUE:
            instruction->qstr_opname = MP_QSTR_YIELD_VALUE;
            break;

        case MP_BC_YIELD_FROM:
            instruction->qstr_opname = MP_QSTR_YIELD_FROM;
            break;

        case MP_BC_IMPORT_NAME:
            DECODE_QSTR;
            instruction->qstr_opname = MP_QSTR_IMPORT_NAME;
            instruction->arg = qst;
            instruction->argobj= MP_OBJ_NEW_QSTR(qst);
            break;

        case MP_BC_IMPORT_FROM:
            DECODE_QSTR;
            instruction->qstr_opname = MP_QSTR_IMPORT_FROM;
            instruction->arg = qst;
            instruction->argobj= MP_OBJ_NEW_QSTR(qst);
            break;

        case MP_BC_IMPORT_STAR:
            instruction->qstr_opname = MP_QSTR_IMPORT_STAR;
            break;

        default:
            if (ip[-1] < MP_BC_LOAD_CONST_SMALL_INT_MULTI + 64) {
                instruction->qstr_opname = MP_QSTR_LOAD_CONST_SMALL_INT;
                instruction->arg = (mp_int_t)ip[-1] - MP_BC_LOAD_CONST_SMALL_INT_MULTI - 16;
            } else if (ip[-1] < MP_BC_LOAD_FAST_MULTI + 16) {
                instruction->qstr_opname = MP_QSTR_LOAD_FAST;
                instruction->arg = (mp_uint_t)ip[-1] - MP_BC_LOAD_FAST_MULTI;
            } else if (ip[-1] < MP_BC_STORE_FAST_MULTI + 16) {
                instruction->qstr_opname = MP_QSTR_STORE_FAST;
                instruction->arg = (mp_uint_t)ip[-1] - MP_BC_STORE_FAST_MULTI;
            } else if (ip[-1] < MP_BC_UNARY_OP_MULTI + MP_UNARY_OP_NUM_BYTECODE) {
                instruction->qstr_opname = MP_QSTR_UNARY_OP;
                instruction->arg = (mp_uint_t)ip[-1] - MP_BC_UNARY_OP_MULTI;
            } else if (ip[-1] < MP_BC_BINARY_OP_MULTI + MP_BINARY_OP_NUM_BYTECODE) {
                mp_uint_t op = ip[-1] - MP_BC_BINARY_OP_MULTI;
                instruction->qstr_opname = MP_QSTR_BINARY_OP;
                instruction->arg = op;
            } else {
                printf("code %p, byte code 0x%02x not implemented\n", ip-1, ip[-1]);
                assert(0);
                return ip;
            }
            break;
    }

    return ip;
}

void prof_print_instr(const byte* ip, mp_code_state_t *code_state) {
    mp_dis_instruction_t _instruction, *instruction = &_instruction;
    prof_opcode_decode(ip, code_state->fun_bc->rc->const_table, instruction);
    const mp_raw_code_t *rc = code_state->fun_bc->rc;
    const mp_bytecode_prelude_t *prelude = &rc->prelude;

    mp_uint_t offset = (ip) - prelude->bytecode;
    mp_printf(&mp_plat_print, "instr");
    
    /* long path */ if (1) {
        mp_printf(&mp_plat_print,
            "@0x%p:%q:%q+0x%04x:%d",
            ip,
            prelude->qstr_source_file,
            prelude->qstr_block_name,
            offset,
            prof_bytecode_lineno(rc, offset)
        );
    }
    
    /* bytecode */ if (0) {
        mp_printf(&mp_plat_print,
            " %02x %02x %02x %02x",
            ip[0], ip[1], ip[2], ip[3], ip[4]
        );
    }
    
    mp_printf(&mp_plat_print,
        " 0x%02x %q [%d]",
        *(byte*)(ip),
        instruction->qstr_opname,
        instruction->arg
    );
    if (instruction->argobj != mp_const_none) {
        mp_printf(&mp_plat_print, " $");
        mp_obj_print_helper(&mp_plat_print, instruction->argobj, PRINT_REPR);
    }
    if (instruction->argobjex_cache != mp_const_none) {
        mp_printf(&mp_plat_print, " #");
        mp_obj_print_helper(&mp_plat_print, instruction->argobjex_cache, PRINT_REPR);
    }

    mp_printf(&mp_plat_print, "\n");
}

#endif // PROF_PRINT_INSTR

#endif // MICROPY_PY_SYS_SETTRACE
