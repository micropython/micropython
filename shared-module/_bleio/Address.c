// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/objstr.h"
#include "shared-bindings/_bleio/Address.h"
#include "shared-module/_bleio/Address.h"

void common_hal_bleio_address_construct(bleio_address_obj_t *self, uint8_t *bytes, uint8_t address_type) {
    self->bytes = mp_obj_new_bytes(bytes, NUM_BLEIO_ADDRESS_BYTES);
    self->type = address_type;
}

mp_obj_t common_hal_bleio_address_get_address_bytes(bleio_address_obj_t *self) {
    return self->bytes;
}

uint8_t common_hal_bleio_address_get_type(bleio_address_obj_t *self) {
    return self->type;
}
