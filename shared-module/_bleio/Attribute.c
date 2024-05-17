// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/runtime.h"
#include "shared-bindings/_bleio/Attribute.h"

void common_hal_bleio_attribute_security_mode_check_valid(bleio_attribute_security_mode_t security_mode) {
    switch (security_mode) {
        case SECURITY_MODE_NO_ACCESS:
        case SECURITY_MODE_OPEN:
        case SECURITY_MODE_ENC_NO_MITM:
        case SECURITY_MODE_ENC_WITH_MITM:
        case SECURITY_MODE_LESC_ENC_WITH_MITM:
        case SECURITY_MODE_SIGNED_NO_MITM:
        case SECURITY_MODE_SIGNED_WITH_MITM:
            break;
        default:
            mp_arg_error_invalid(MP_QSTR_security_mode);
            break;
    }
}
