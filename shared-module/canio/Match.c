// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-module/canio/Match.h"
#include "shared-bindings/canio/Match.h"

void common_hal_canio_match_construct(canio_match_obj_t *self, int id, int mask, bool extended) {
    self->id = id;
    self->mask = mask;
    self->extended = extended;
}

int common_hal_canio_match_get_id(const canio_match_obj_t *self) {
    return self->id;
}
int common_hal_canio_match_get_mask(const canio_match_obj_t *self) {
    return self->mask;
}
bool common_hal_canio_match_get_extended(const canio_match_obj_t *self) {
    return self->extended;
}
