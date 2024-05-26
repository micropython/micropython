// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// BLE security modes: 0x<level><mode>
typedef enum {
    SECURITY_MODE_NO_ACCESS = 0x00,
    SECURITY_MODE_OPEN = 0x11,
    SECURITY_MODE_ENC_NO_MITM = 0x21,
    SECURITY_MODE_ENC_WITH_MITM = 0x31,
    SECURITY_MODE_LESC_ENC_WITH_MITM = 0x41,
    SECURITY_MODE_SIGNED_NO_MITM = 0x12,
    SECURITY_MODE_SIGNED_WITH_MITM = 0x22,
} bleio_attribute_security_mode_t;
