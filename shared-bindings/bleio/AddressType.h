/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Artur Pacholec
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_ADDRESSTYPE_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_ADDRESSTYPE_H

#include "py/obj.h"

typedef enum {
    ADDRESS_PUBLIC,
    ADDRESS_RANDOM_STATIC,
    ADDRESS_RANDOM_PRIVATE_RESOLVABLE,
    ADDRESS_RANDOM_PRIVATE_NON_RESOLVABLE
} bleio_address_type_t;

extern const mp_obj_type_t bleio_addresstype_type;

typedef struct {
    mp_obj_base_t base;
} bleio_addresstype_obj_t;

extern const bleio_addresstype_obj_t bleio_addresstype_public_obj;
extern const bleio_addresstype_obj_t bleio_addresstype_random_static_obj;
extern const bleio_addresstype_obj_t bleio_addresstype_random_private_resolvable_obj;
extern const bleio_addresstype_obj_t bleio_addresstype_random_private_non_resolvable_obj;

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_ADDRESSTYPE_H
