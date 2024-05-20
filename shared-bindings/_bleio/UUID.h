// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/_bleio/UUID.h"

void bleio_uuid_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);

extern const mp_obj_type_t bleio_uuid_type;

extern void common_hal_bleio_uuid_construct(bleio_uuid_obj_t *self, mp_int_t uuid16, const uint8_t uuid128[16]);
extern uint32_t common_hal_bleio_uuid_get_uuid16(bleio_uuid_obj_t *self);
extern void common_hal_bleio_uuid_get_uuid128(bleio_uuid_obj_t *self, uint8_t uuid128[16]);
extern uint32_t common_hal_bleio_uuid_get_size(bleio_uuid_obj_t *self);

void common_hal_bleio_uuid_pack_into(bleio_uuid_obj_t *self, uint8_t *buf);
