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

#pragma once

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    int id;
    uint8_t data[8];
    size_t size:4;
    bool rtr:1;
    bool extended:1;
} canio_message_obj_t;

void common_hal_canio_message_construct(canio_message_obj_t *self, int id, void *data, size_t size, bool rtr, bool extended);
bool common_hal_canio_message_rtr_get(const canio_message_obj_t *self);
bool common_hal_canio_message_extended_get(const canio_message_obj_t *self);
int common_hal_canio_message_id_get(const canio_message_obj_t *self);
const void *common_hal_canio_message_data_get(const canio_message_obj_t *self);
size_t common_hal_canio_message_size_get(const canio_message_obj_t *self);
void common_hal_canio_message_rtr_set(canio_message_obj_t *self, bool rtr);
void common_hal_canio_message_extended_set(canio_message_obj_t *self, bool extended);
void common_hal_canio_message_id_set(canio_message_obj_t *self, int id);
void common_hal_canio_message_data_set(canio_message_obj_t *self, const void *data, size_t size);
void common_hal_canio_message_size_set(canio_message_obj_t *self, size_t size);
