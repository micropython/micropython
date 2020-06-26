/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

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
