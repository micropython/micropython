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
#ifndef MICROPY_INCLUDED_PY_OBJCODE_H
#define MICROPY_INCLUDED_PY_OBJCODE_H

#include "py/bc.h"

#if MICROPY_PY_BUILTINS_CODE == MICROPY_PY_BUILTINS_CODE_NONE

// Code object not implemented at this configuration level.

#elif MICROPY_PY_BUILTINS_CODE <= MICROPY_PY_BUILTINS_CODE_MINIMUM

typedef struct _mp_obj_code_t {
    mp_obj_base_t base;
    mp_obj_t module_fun;
} mp_obj_code_t;

static inline mp_obj_t mp_obj_new_code(mp_obj_t module_fun) {
    mp_obj_code_t *code = mp_obj_malloc(mp_obj_code_t, &mp_type_code);
    code->module_fun = module_fun;
    return MP_OBJ_FROM_PTR(code);
}

#elif MICROPY_PY_BUILTINS_CODE <= MICROPY_PY_BUILTINS_CODE_BASIC

typedef struct _mp_obj_code_t {
    mp_obj_base_t base;
    mp_module_constants_t constants;
    const void *proto_fun;
} mp_obj_code_t;

static inline mp_obj_t mp_obj_new_code(const mp_module_constants_t constants, const void *proto_fun) {
    mp_obj_code_t *code = mp_obj_malloc(mp_obj_code_t, &mp_type_code);
    code->constants = constants;
    code->proto_fun = proto_fun;
    return MP_OBJ_FROM_PTR(code);
}

static inline const mp_module_constants_t *mp_code_get_constants(mp_obj_code_t *self) {
    return &self->constants;
}

static inline const void *mp_code_get_proto_fun(mp_obj_code_t *self) {
    return self->proto_fun;
}

#elif MICROPY_PY_BUILTINS_CODE <= MICROPY_PY_BUILTINS_CODE_FULL

#include "py/emitglue.h"

#define MP_CODE_QSTR_MAP(context, idx) ((qstr)(context->constants.qstr_table[idx]))

typedef struct _mp_obj_code_t {
    mp_obj_base_t base;
    const mp_module_context_t *context;
    const mp_raw_code_t *rc;
    mp_obj_dict_t *dict_locals;
    #if !MICROPY_PREVIEW_VERSION_2
    mp_obj_t lnotab;
    #endif
} mp_obj_code_t;

mp_obj_t mp_obj_new_code(const mp_module_context_t *context, const mp_raw_code_t *rc, bool result_required);

static inline const mp_module_constants_t *mp_code_get_constants(mp_obj_code_t *self) {
    return &self->context->constants;
}

static inline const void *mp_code_get_proto_fun(mp_obj_code_t *self) {
    // A mp_raw_code_t is always a proto_fun (but not the other way around).
    return self->rc;
}

#endif

#endif // MICROPY_INCLUDED_PY_OBJCODE_H
