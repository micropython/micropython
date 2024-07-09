// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// These are not the Bluetooth spec values. They are what is used by the Nordic SoftDevice.
// The bit values are in different positions.

typedef enum {
    CHAR_PROP_NONE =              0,
    CHAR_PROP_BROADCAST =         1u << 0,
    CHAR_PROP_INDICATE =          1u << 1,
    CHAR_PROP_NOTIFY =            1u << 2,
    CHAR_PROP_READ =              1u << 3,
    CHAR_PROP_WRITE =             1u << 4,
    CHAR_PROP_WRITE_NO_RESPONSE = 1u << 5,
    CHAR_PROP_ALL =               (CHAR_PROP_BROADCAST | CHAR_PROP_INDICATE | CHAR_PROP_NOTIFY |
        CHAR_PROP_READ | CHAR_PROP_WRITE | CHAR_PROP_WRITE_NO_RESPONSE)
} bleio_characteristic_properties_enum_t;
typedef uint8_t bleio_characteristic_properties_t;

// Bluetooth spec property values
#define BT_GATT_CHRC_BROADCAST                  0x01
#define BT_GATT_CHRC_READ                       0x02
#define BT_GATT_CHRC_WRITE_WITHOUT_RESP         0x04
#define BT_GATT_CHRC_WRITE                      0x08
#define BT_GATT_CHRC_NOTIFY                     0x10
#define BT_GATT_CHRC_INDICATE                   0x20
#define BT_GATT_CHRC_AUTH                       0x40
#define BT_GATT_CHRC_EXT_PROP                   0x80
