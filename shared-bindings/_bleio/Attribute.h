// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "common-hal/_bleio/Attribute.h"
#include "shared-module/_bleio/Attribute.h"

extern const mp_obj_type_t bleio_attribute_type;

extern void common_hal_bleio_attribute_security_mode_check_valid(bleio_attribute_security_mode_t security_mode);
