/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023-2024 Damien P. George
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
#ifndef MICROPY_INCLUDED_WEBASSEMBLY_PROXY_C_H
#define MICROPY_INCLUDED_WEBASSEMBLY_PROXY_C_H

#include "py/obj.h"

// Fixed JsProxy references.
#define MP_OBJ_JSPROXY_REF_GLOBAL_THIS (0)
#define MP_OBJ_JSPROXY_REF_UNDEFINED (1)

// proxy value number of items
#define PVN (3)

typedef struct _mp_obj_jsproxy_t {
    mp_obj_base_t base;
    int ref;
    bool bind_to_self;
} mp_obj_jsproxy_t;

extern const mp_obj_type_t mp_type_jsproxy;
extern const mp_obj_type_t mp_type_JsException;

void external_call_depth_inc(void);
void external_call_depth_dec(void);

void proxy_c_init(void);
mp_obj_t proxy_convert_js_to_mp_obj_cside(uint32_t *value);
void proxy_convert_mp_to_js_obj_cside(mp_obj_t obj, uint32_t *out);
void proxy_convert_mp_to_js_exc_cside(void *exc, uint32_t *out);

mp_obj_t mp_obj_new_jsproxy(int ref);
mp_obj_t mp_obj_get_jsproxy(int ref);
void mp_obj_jsproxy_global_this_attr(qstr attr, mp_obj_t *dest);

static inline bool mp_obj_is_jsproxy(mp_obj_t o) {
    return mp_obj_get_type(o) == &mp_type_jsproxy;
}

static inline int mp_obj_jsproxy_get_ref(mp_obj_t o) {
    mp_obj_jsproxy_t *self = MP_OBJ_TO_PTR(o);
    return self->ref;
}

#endif // MICROPY_INCLUDED_WEBASSEMBLY_PROXY_C_H
