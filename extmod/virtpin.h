// Copyright (c) 2016 Paul Sokolovsky
// SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_EXTMOD_VIRTPIN_H
#define MICROPY_INCLUDED_EXTMOD_VIRTPIN_H

#include "py/obj.h"
#include "py/proto.h"

#define MP_PIN_READ   (1)
#define MP_PIN_WRITE  (2)
#define MP_PIN_INPUT  (3)
#define MP_PIN_OUTPUT (4)

// Pin protocol
typedef struct _mp_pin_p_t {
    MP_PROTOCOL_HEAD
    mp_uint_t (*ioctl)(mp_obj_t obj, mp_uint_t request, uintptr_t arg, int *errcode);
} mp_pin_p_t;

int mp_virtual_pin_read(mp_obj_t pin);
void mp_virtual_pin_write(mp_obj_t pin, int value);

// If a port exposes a Pin object, it's constructor should be like this
mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);

#endif // MICROPY_INCLUDED_EXTMOD_VIRTPIN_H
