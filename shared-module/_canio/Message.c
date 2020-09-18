/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
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

#include "shared-module/_canio/Message.h"

#include <string.h>

void common_hal_canio_message_construct(canio_message_obj_t *self, int id, void *data, size_t size, bool rtr, bool extended)
{
    self->id = id;
    self->size = size;
    self->rtr = rtr;
    self->extended = extended;
    if (data) {
        memcpy(self->data, data, size);
    } else {
        memset(self->data, 0, size);
    }
}

int common_hal_canio_message_get_id(const canio_message_obj_t *self)
{
    return self->id;
}

void common_hal_canio_message_set_id(canio_message_obj_t *self, int id)
{
    self->id = id;
}


const void *common_hal_canio_message_get_data(const canio_message_obj_t *self)
{
    return self->data;
}

const void common_hal_canio_message_set_data(canio_message_obj_t *self, const void *data, size_t size)
{
    self->rtr = false;
    self->size = size;
    memcpy(self->data, data, size);
}


size_t common_hal_canio_message_get_size(const canio_message_obj_t *self)
{
    return self->size;
}

void common_hal_canio_message_set_size(canio_message_obj_t *self, size_t size)
{
    memset(self->data, 0, size);
    self->size = size;
}


bool common_hal_canio_message_get_rtr(const canio_message_obj_t *self)
{
    return self->rtr;
}

void common_hal_canio_message_set_rtr(canio_message_obj_t *self, bool rtr)
{
    self->rtr = rtr;
    if (rtr) {
        memset(self->data, 0, self->size);
    }
}

bool common_hal_canio_message_get_extended(const canio_message_obj_t *self)
{
    return self->extended;
}

void common_hal_canio_message_set_extended(canio_message_obj_t *self, bool extended)
{
    self->extended = extended;
}
