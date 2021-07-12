/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Jeff Epler for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_PY_PROTO_H
#define MICROPY_INCLUDED_PY_PROTO_H

#ifdef MICROPY_UNSAFE_PROTO
#define MP_PROTOCOL_HEAD /* NOTHING */
#define MP_PROTO_IMPLEMENT(name) /* NOTHING */
static inline void *mp_proto_get(uint16_t name, mp_const_obj_type_t obj) {
    return mp_type_get_protocol_slot(mp_obj_get_type(obj));
}
#else
#define MP_PROTOCOL_HEAD \
    uint16_t name; // The name of this protocol, a qstr
#define MP_PROTO_IMPLEMENT(n) .name = n,
const void *mp_proto_get(uint16_t name, mp_const_obj_t obj);
const void *mp_proto_get_or_throw(uint16_t name, mp_const_obj_t obj);
#endif

#endif
