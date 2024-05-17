// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_BLE_HCI_COMMON_HAL_ATTRIBUTE_H
#define MICROPY_INCLUDED_BLE_HCI_COMMON_HAL_ATTRIBUTE_H

#include "shared-module/_bleio/Attribute.h"
#include "shared-bindings/_bleio/UUID.h"

bleio_uuid_obj_t *bleio_attribute_get_uuid(mp_obj_t *attribute);

#endif // MICROPY_INCLUDED_BLE_HCI_COMMON_HAL_ATTRIBUTE_H
