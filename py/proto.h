// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// CIRCUITPY-CHANGE
//
// In CircuitPython, we have multiple different 'protocol' types (rather than multiplexing through ioctl).
// The standard (read/write/ioctl) protocol is indicated by the "0" protocol, while others must MP_PROTO_IMPLEMENT(QSTR_...).
// While not used in CircuitPython or tested, the ability to skip including the protocol field in a protocol object is included at the source level

#ifdef MICROPY_UNSAFE_PROTO
#define MP_PROTOCOL_HEAD /* NOTHING */
#define MP_PROTO_IMPLEMENT(name) /* NOTHING */
static inline void *mp_proto_get(uint16_t name, mp_const_obj_type_t obj) {
    return MP_OBJ_TYPE_GET_SLOT_OR_NULL(mp_obj_get_type(obj), protocol);
}
#else
#define MP_PROTOCOL_HEAD \
    uint16_t name;
#define MP_PROTO_IMPLEMENT(n) .name = n,
const void *mp_proto_get(uint16_t name, mp_const_obj_t obj);
const void *mp_proto_get_or_throw(uint16_t name, mp_const_obj_t obj);
#endif
