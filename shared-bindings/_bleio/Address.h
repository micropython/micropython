// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/objtype.h"
#include "shared-module/_bleio/Address.h"

#define BLEIO_ADDRESS_TYPE_PUBLIC (0)
#define BLEIO_ADDRESS_TYPE_RANDOM_STATIC (1)
#define BLEIO_ADDRESS_TYPE_RANDOM_PRIVATE_RESOLVABLE (2)
#define BLEIO_ADDRESS_TYPE_RANDOM_PRIVATE_NON_RESOLVABLE (3)

#define BLEIO_ADDRESS_TYPE_MIN BLEIO_ADDRESS_TYPE_PUBLIC
#define BLEIO_ADDRESS_TYPE_MAX BLEIO_ADDRESS_TYPE_RANDOM_PRIVATE_NON_RESOLVABLE

extern const mp_obj_type_t bleio_address_type;

extern void common_hal_bleio_address_construct(bleio_address_obj_t *self, uint8_t *bytes, uint8_t address_type);
extern mp_obj_t common_hal_bleio_address_get_address_bytes(bleio_address_obj_t *self);
extern uint8_t common_hal_bleio_address_get_type(bleio_address_obj_t *self);
