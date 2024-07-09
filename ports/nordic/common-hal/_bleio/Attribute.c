// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/_bleio/Attribute.h"

// Convert a _bleio security mode to a ble_gap_conn_sec_mode_t setting.
void bleio_attribute_gatts_set_security_mode(ble_gap_conn_sec_mode_t *perm, bleio_attribute_security_mode_t security_mode) {
    switch (security_mode) {
        case SECURITY_MODE_NO_ACCESS:
            BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(perm);
            break;

        case SECURITY_MODE_OPEN:
            BLE_GAP_CONN_SEC_MODE_SET_OPEN(perm);
            break;

        case SECURITY_MODE_ENC_NO_MITM:
            BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(perm);
            break;

        case SECURITY_MODE_ENC_WITH_MITM:
            BLE_GAP_CONN_SEC_MODE_SET_ENC_WITH_MITM(perm);
            break;

        case SECURITY_MODE_LESC_ENC_WITH_MITM:
            BLE_GAP_CONN_SEC_MODE_SET_LESC_ENC_WITH_MITM(perm);
            break;

        case SECURITY_MODE_SIGNED_NO_MITM:
            BLE_GAP_CONN_SEC_MODE_SET_SIGNED_NO_MITM(perm);
            break;

        case SECURITY_MODE_SIGNED_WITH_MITM:
            BLE_GAP_CONN_SEC_MODE_SET_SIGNED_WITH_MITM(perm);
            break;
    }
}
