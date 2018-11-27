/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_DESCRIPTOR_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_DESCRIPTOR_H

#include "common-hal/bleio/Descriptor.h"
#include "common-hal/bleio/UUID.h"

extern const mp_obj_type_t bleio_descriptor_type;

extern void common_hal_bleio_descriptor_construct(bleio_descriptor_obj_t *self, bleio_uuid_obj_t *uuid);
extern mp_int_t common_hal_bleio_descriptor_get_handle(bleio_descriptor_obj_t *self);
extern mp_obj_t common_hal_bleio_descriptor_get_uuid(bleio_descriptor_obj_t *self);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_DESCRIPTOR_H
