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

#include "py/profile.h"
#include "py/bc0.h"
#include "py/gc.h"

#if MICROPY_PY_SYS_SETTRACE

#define prof_trace_cb MP_STATE_THREAD(prof_trace_callback)

STATIC uint mp_prof_bytecode_lineno(const mp_raw_code_t *rc, size_t bc) {
    const mp_bytecode_prelude_t *prelude = &rc->prelude;
    return mp_bytecode_get_source_line(prelude->line_info, bc + prelude->opcodes - prelude->locals);
}

void mp_prof_extract_prelude(const byte *bytecode, mp_bytecode_prelude_t *prelude) {
    const byte *ip = bytecode;

    prelude->n_state = mp_decode_uint(&ip);
    prelude->n_exc_stack = mp_decode_uint(&ip);
    prelude->scope_flags = *ip++;
    prelude->n_pos_args = *ip++;
    prelude->n_kwonly_args = *ip++;
    prelude->n_def_pos_args = *ip++;

    const byte *code_info = ip;
    size_t code_info_size = mp_decode_uint(&ip);

    qstr block_name = ip[0] | (ip[1] << 8);
    qstr source_file = ip[2] | (ip[3] << 8);
    ip += 4;
    prelude->qstr_block_name = block_name;
    prelude->qstr_source_file = source_file;

    prelude->line_info = ip;
    prelude->locals = code_info + code_info_size;

    ip = prelude->locals;
    while (*ip++ != 255) {
    }
    prelude->opcodes = ip;
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
    mp_obj_tuple_t *consts = MP_OBJ_TO_PTR(mp_obj_new_tuple(stop - start + 1, NULL));

    size_t const_no = 0;
    for (int i = start; i < stop; ++i) {
        mp_obj_t code = mp_obj_new_code((const mp_raw_code_t*)MP_OBJ_TO_PTR(rc->const_table[i]));
        if (code == MP_OBJ_NULL) {
            m_malloc_fail(sizeof(mp_obj_code_t));
        }
        consts->items[const_no++] = code;
    }
    consts->items[const_no++] = mp_const_none;

    return consts;
}

STATIC mp_obj_t raw_code_lnotab(const mp_raw_code_t *rc) {
    // const mp_bytecode_prelude_t *prelude = &rc->prelude;
    uint start = 0;
    uint stop = rc->fun_data_len - start;

    uint last_lineno = mp_prof_bytecode_lineno(rc, start);
    uint lasti = 0;

    const uint buffer_chunk_size = (stop-start) >> 2; // heuristic magic
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
                (void*)prelude->opcodes,
                rc->fun_data_len - (prelude->opcodes - (const byte*)rc->fun_data)
            );
            break;
        case MP_QSTR_co_consts:
            dest[0] = MP_OBJ_FROM_PTR(code_consts(rc));
            break;
        case MP_QSTR_co_filename:
            dest[0] = MP_OBJ_NEW_QSTR(prelude->qstr_source_file);
            break;
        case MP_QSTR_co_firstlineno:
            dest[0] = MP_OBJ_NEW_SMALL_INT(mp_prof_bytecode_lineno(rc, 0));
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
    mp_obj_code_t *o = m_new_obj_maybe(mp_obj_code_t);
    if (o == NULL) {
        return MP_OBJ_NULL;
    }
    o->base.type = &mp_type_code;
    o->rc = rc;
    o->dict_locals = mp_locals_get(); // this is a wrong! how to do this properly?
    o->lnotab = MP_OBJ_NULL;
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
                dest[0] = MP_OBJ_FROM_PTR(o->code_state->prev_state->frame);
            }
            break;
        case MP_QSTR_f_code:
            dest[0] = MP_OBJ_FROM_PTR(o->code);
            break;
        case MP_QSTR_f_globals:
            dest[0] = MP_OBJ_FROM_PTR(o->code_state->fun_bc->globals);
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
    if (gc_is_locked()) {
        return MP_OBJ_NULL;
    }

    mp_obj_frame_t *o = m_new_obj_maybe(mp_obj_frame_t);
    if (o == NULL) {
        return MP_OBJ_NULL;
    }

    mp_obj_code_t *code = o->code = MP_OBJ_TO_PTR(mp_obj_new_code(code_state->fun_bc->rc));
    if (code == NULL) {
        return MP_OBJ_NULL;
    }

    const mp_raw_code_t *rc = code->rc;
    const mp_bytecode_prelude_t *prelude = &rc->prelude;
    o->code_state = code_state;
    o->base.type = &mp_type_frame;
    o->back = NULL;
    o->code = code;
    o->lasti = code_state->ip - prelude->opcodes;
    o->lineno = mp_prof_bytecode_lineno(rc, o->lasti);
    o->trace_opcodes = false;
    o->callback = MP_OBJ_NULL;

    return MP_OBJ_FROM_PTR(o);
}


/******************************************************************************/
// Trace logic

typedef struct {
    struct _mp_obj_frame_t * frame;
    mp_obj_t event;
    mp_obj_t arg;
} prof_callback_args_t;

STATIC mp_obj_t mp_prof_callback_invoke(mp_obj_t callback, prof_callback_args_t *args) {
    assert(mp_obj_is_callable(callback));

    mp_prof_is_executing = true;

    mp_obj_t a[3] = {MP_OBJ_FROM_PTR(args->frame), args->event, args->arg};
    mp_obj_t top = mp_call_function_n_kw(callback, 3, 0, a);

    mp_prof_is_executing = false;

    if (MP_STATE_VM(mp_pending_exception) != MP_OBJ_NULL) {
        mp_obj_t obj = MP_STATE_VM(mp_pending_exception);
        MP_STATE_VM(mp_pending_exception) = MP_OBJ_NULL;
        nlr_raise(obj);
    }
    return top;
}

mp_obj_t mp_prof_settrace(mp_obj_t callback) {
    if (mp_obj_is_callable(callback)) {
        prof_trace_cb = callback;
    } else {
        prof_trace_cb = MP_OBJ_NULL;
    }
    return mp_const_none;
}

mp_obj_t mp_prof_frame_enter(mp_code_state_t *code_state) {
    assert(!mp_prof_is_executing);

    mp_obj_frame_t *frame = MP_OBJ_TO_PTR(mp_obj_new_frame(code_state));
    if (frame == NULL) {
        // Couldn't allocate a frame object
        return MP_OBJ_NULL;
    }

    if (code_state->prev_state && code_state->frame == NULL) {
        // We are entering not-yet-traced frame
        // which means it's a CALL event (not a GENERATOR)
        // so set the function definition line.
        const mp_raw_code_t *rc = code_state->fun_bc->rc;
        frame->lineno = rc->line_of_definition;
        if (!rc->line_of_definition) {
            frame->lineno = mp_prof_bytecode_lineno(rc, 0);
        }
    }
    code_state->frame = frame;

    if (!prof_trace_cb) {
        return MP_OBJ_NULL;
    }

    mp_obj_t top;
    prof_callback_args_t _args, *args=&_args;
    args->frame = code_state->frame;

    // SETTRACE event CALL
    args->event = MP_OBJ_NEW_QSTR(MP_QSTR_call);
    args->arg = mp_const_none;
    top = mp_prof_callback_invoke(prof_trace_cb, args);

    code_state->frame->callback = mp_obj_is_callable(top) ? top : MP_OBJ_NULL;

    // Invalidate the last executed line number so the LINE trace can trigger after this CALL.
    frame->lineno = 0;

    return top;
}

mp_obj_t mp_prof_frame_update(const mp_code_state_t *code_state) {
    mp_obj_frame_t *frame = code_state->frame;
    if (frame == NULL) {
        // Frame was not allocated (eg because there was no memory available)
        return MP_OBJ_NULL;
    }

    mp_obj_frame_t *o = frame;
    mp_obj_code_t *code = o->code;
    const mp_raw_code_t *rc = code->rc;
    const mp_bytecode_prelude_t *prelude = &rc->prelude;

    assert(o->code_state == code_state);

    o->lasti = code_state->ip - prelude->opcodes;
    o->lineno = mp_prof_bytecode_lineno(rc, o->lasti);

    return MP_OBJ_FROM_PTR(o);
}

mp_obj_t mp_prof_instr_tick(mp_code_state_t *code_state, bool is_exception) {
    // Detect execution recursion
    assert(!mp_prof_is_executing);
    assert(code_state->frame);
    assert(mp_obj_get_type(code_state->frame) == &mp_type_frame);

    // Detect data recursion
    assert(code_state != code_state->prev_state);

    mp_obj_t top = mp_const_none;
    mp_obj_t callback = code_state->frame->callback;

    prof_callback_args_t _args, *args=&_args;
    args->frame = code_state->frame;
    args->event = mp_const_none;
    args->arg = mp_const_none;

    // Call event's are handled inside mp_prof_frame_enter

    // SETTRACE event EXCEPTION
    if (is_exception) {
        args->event = MP_OBJ_NEW_QSTR(MP_QSTR_exception);
        top = mp_prof_callback_invoke(callback, args);
        return top;
    }

    // SETTRACE event LINE
    const mp_raw_code_t *rc = code_state->fun_bc->rc;
    const mp_bytecode_prelude_t *prelude = &rc->prelude;
    size_t prev_line_no = args->frame->lineno;
    size_t current_line_no = mp_prof_bytecode_lineno(rc, code_state->ip - prelude->opcodes);
    if (prev_line_no != current_line_no) {
        args->frame->lineno = current_line_no;
        args->event = MP_OBJ_NEW_QSTR(MP_QSTR_line);
        top = mp_prof_callback_invoke(callback, args);
    }

    // SETTRACE event RETURN
    const byte *ip = code_state->ip;
    if (*ip == MP_BC_RETURN_VALUE || *ip == MP_BC_YIELD_VALUE) {
        args->event = MP_OBJ_NEW_QSTR(MP_QSTR_return);
        top = mp_prof_callback_invoke(callback, args);
        if (code_state->prev_state && *ip == MP_BC_RETURN_VALUE) {
            code_state->frame->callback = MP_OBJ_NULL;
        }
    }

    // SETTRACE event OPCODE
    // TODO: frame.f_trace_opcodes=True
    if (false) {
        args->event = MP_OBJ_NEW_QSTR(MP_QSTR_opcode);
    }

    return top;
}

#endif // MICROPY_PY_SYS_SETTRACE
