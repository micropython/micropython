// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-module/canio/Message.h"
#include "shared-bindings/canio/Message.h"

#include <string.h>

void common_hal_canio_message_construct(canio_message_obj_t *self, int id, void *data, size_t size, bool extended) {
    self->id = id;
    self->size = size;
    self->extended = extended;
    if (data) {
        memcpy(self->data, data, size);
    }
}

int common_hal_canio_message_get_id(const canio_message_obj_t *self) {
    return self->id;
}

void common_hal_canio_message_set_id(canio_message_obj_t *self, int id) {
    self->id = id;
}


const void *common_hal_canio_message_get_data(const canio_message_obj_t *self) {
    return self->data;
}

const void common_hal_canio_message_set_data(canio_message_obj_t *self, const void *data, size_t size) {
    self->size = size;
    memcpy(self->data, data, size);
}


size_t common_hal_canio_message_get_length(const canio_message_obj_t *self) {
    return self->size;
}

bool common_hal_canio_message_get_extended(const canio_message_obj_t *self) {
    return self->extended;
}

void common_hal_canio_message_set_extended(canio_message_obj_t *self, bool extended) {
    self->extended = extended;
}
