/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_ADDRESS_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_ADDRESS_H

#include "py/objtype.h"
#include "shared-module/_bleio/Address.h"

#define BLEIO_ADDRESS_TYPE_PUBLIC (0)
#define BLEIO_ADDRESS_TYPE_RANDOM_STATIC (1)
#define BLEIO_ADDRESS_TYPE_RANDOM_PRIVATE_RESOLVABLE (2)
#define BLEIO_ADDRESS_TYPE_RANDOM_PRIVATE_NON_RESOLVABLE (3)

#define BLEIO_ADDRESS_TYPE_MIN BLEIO_ADDRESS_TYPE_PUBLIC
#define BLEIO_ADDRESS_TYPE_MAX BLEIO_ADDRESS_TYPE_RANDOM_PRIVATE_NON_RESOLVABLE

extern const mp_obj_type_t bleio_address_type;

extern void common_hal_bleio_address_construct(bleio_address_obj_t *self, uint8_t *bytes, uint8_t address_type);
extern mp_obj_t common_hal_bleio_address_get_address_bytes(bleio_address_obj_t *self);
extern uint8_t common_hal_bleio_address_get_type(bleio_address_obj_t *self);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_ADDRESS_H
