/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Paul Sokolovsky
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

#include "extmod/virtpin.h"

int mp_virtual_pin_read(mp_obj_t pin) {
    mp_obj_base_t *s = (mp_obj_base_t *)MP_OBJ_TO_PTR(pin);
    mp_pin_p_t *pin_p = (mp_pin_p_t *)MP_OBJ_TYPE_GET_SLOT(s->type, protocol);
    return pin_p->ioctl(pin, MP_PIN_READ, 0, NULL);
}

void mp_virtual_pin_write(mp_obj_t pin, int value) {
    mp_obj_base_t *s = (mp_obj_base_t *)MP_OBJ_TO_PTR(pin);
    mp_pin_p_t *pin_p = (mp_pin_p_t *)MP_OBJ_TYPE_GET_SLOT(s->type, protocol);
    pin_p->ioctl(pin, MP_PIN_WRITE, value, NULL);
}
