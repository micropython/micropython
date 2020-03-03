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

#ifndef MICROPY_INCLUDED_PY_PROFILING_H
#define MICROPY_INCLUDED_PY_PROFILING_H

#include "py/emitglue.h"

#if MICROPY_PY_SYS_SETTRACE

#define mp_prof_is_executing MP_STATE_THREAD(prof_callback_is_executing)

typedef struct _mp_obj_code_t {
    mp_obj_base_t base;
    const mp_raw_code_t *rc;
    mp_obj_dict_t *dict_locals;
    mp_obj_t lnotab;
} mp_obj_code_t;

typedef struct _mp_obj_frame_t {
    mp_obj_base_t base;
    const mp_code_state_t *code_state;
    struct _mp_obj_frame_t *back;
    mp_obj_t callback;
    mp_obj_code_t *code;
    mp_uint_t lasti;
    mp_uint_t lineno;
    bool trace_opcodes;
} mp_obj_frame_t;

void mp_prof_extract_prelude(const byte *bytecode, mp_bytecode_prelude_t *prelude);

mp_obj_t mp_obj_new_code(const mp_raw_code_t *rc);
mp_obj_t mp_obj_new_frame(const mp_code_state_t *code_state);

// This is the implementation for the sys.settrace
mp_obj_t mp_prof_settrace(mp_obj_t callback);

mp_obj_t mp_prof_frame_enter(mp_code_state_t *code_state);
mp_obj_t mp_prof_frame_update(const mp_code_state_t *code_state);

// For every VM instruction tick this function deduces events from the state
mp_obj_t mp_prof_instr_tick(mp_code_state_t *code_state, bool is_exception);

// This section is for debugging the settrace feature itself, and is not intended
// to be included in production/release builds.
#define MICROPY_PROF_INSTR_DEBUG_PRINT_ENABLE 0
#if MICROPY_PROF_INSTR_DEBUG_PRINT_ENABLE
void mp_prof_print_instr(const byte *ip, mp_code_state_t *code_state);
#define MP_PROF_INSTR_DEBUG_PRINT(current_ip) mp_prof_print_instr((current_ip), code_state)
#else
#define MP_PROF_INSTR_DEBUG_PRINT(current_ip)
#endif

#endif // MICROPY_PY_SYS_SETTRACE
#endif // MICROPY_INCLUDED_PY_PROFILING_H
