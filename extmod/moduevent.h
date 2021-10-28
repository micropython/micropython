/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Damien P. George
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
#ifndef MICROPY_INCLUDED_EXTMOD_MODUEVENT_H
#define MICROPY_INCLUDED_EXTMOD_MODUEVENT_H

#include "py/obj.h"

#define MICROPY_ALLOC_UEVENT_POLL_INIT (4)
#define MICROPY_ALLOC_UEVENT_POLL_INC (4)

#define UEVENT_READ (1)
#define UEVENT_WRITE (2)

typedef struct _mp_obj_poll_t mp_obj_poll_t;

typedef struct _mp_obj_poll_entry_base_t {
    mp_obj_base_t base;
    mp_obj_poll_t *poller;
    mp_obj_t obj;
    uint16_t user_flags;
    uint16_t user_method_name; // a qstr
    mp_obj_t user_method_obj;
    mp_obj_t user_data;
} mp_obj_poll_entry_base_t;

extern const mp_obj_type_t mp_type_poll;
extern const mp_obj_type_t mp_type_poll_entry;

// These should all be provided by a specific uevent implementation.
mp_obj_t mp_uevent_poll_iternext(mp_obj_t self_in);
mp_obj_t mp_uevent_poll_register(size_t n_args, const mp_obj_t *args);
mp_obj_t mp_uevent_poll_unregister(size_t n_args, const mp_obj_t *args);
mp_obj_t mp_uevent_poll_poll_ms(size_t n_args, const mp_obj_t *args);
mp_obj_t mp_uevent_poll(void);

#endif // MICROPY_INCLUDED_EXTMOD_MODUEVENT_H
