// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

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
