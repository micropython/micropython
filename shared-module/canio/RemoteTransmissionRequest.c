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

#include "shared-module/canio/RemoteTransmissionRequest.h"
#include "shared-bindings/canio/RemoteTransmissionRequest.h"

#include <string.h>

void common_hal_canio_remote_transmission_request_construct(canio_remote_transmission_request_obj_t *self, int id, size_t size, bool extended) {
    self->id = id;
    self->size = size;
    self->extended = extended;
}

int common_hal_canio_remote_transmission_request_get_id(const canio_remote_transmission_request_obj_t *self) {
    return self->id;
}

void common_hal_canio_remote_transmission_request_set_id(canio_remote_transmission_request_obj_t *self, int id) {
    self->id = id;
}

size_t common_hal_canio_remote_transmission_request_get_length(const canio_remote_transmission_request_obj_t *self) {
    return self->size;
}

void common_hal_canio_remote_transmission_request_set_length(canio_remote_transmission_request_obj_t *self, size_t size) {
    self->size = size;
}

bool common_hal_canio_remote_transmission_request_get_extended(const canio_remote_transmission_request_obj_t *self) {
    return self->extended;
}

void common_hal_canio_remote_transmission_request_set_extended(canio_remote_transmission_request_obj_t *self, bool extended) {
    self->extended = extended;
}
