/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
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

#include <string.h>

#include "ble.h"
#include "ble_drv.h"
#include "ble_hci.h"
#include "nrf_soc.h"
#include "py/runtime.h"

#include "common-hal/bleio/Broadcaster.h"
#include "shared-bindings/bleio/Adapter.h"
#include "shared-bindings/bleio/Broadcaster.h"

static uint8_t m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;

void common_hal_bleio_broadcaster_construct(bleio_broadcaster_obj_t *self, mp_float_t interval) {
    common_hal_bleio_adapter_set_enabled(true);   // TODO -- Do this somewhere else maybe bleio __init__
    const mp_float_t min = BLE_GAP_ADV_INTERVAL_MIN * ADV_INTERVAL_UNIT_FLOAT_SECS;
    const mp_float_t max = BLE_GAP_ADV_INTERVAL_MAX * ADV_INTERVAL_UNIT_FLOAT_SECS;

    if (interval < min || interval > max) {
        // Would like to print range using the constants above, but vargs would convert to double.
        mp_raise_ValueError(translate("interval not in range 0.0020 to 10.24"));
    }
    self->interval = interval;
}


void common_hal_bleio_broadcaster_start_advertising(bleio_broadcaster_obj_t *self, mp_buffer_info_t *data) {
    uint32_t err_code;

    if (data->len >= BLE_GAP_ADV_SET_DATA_SIZE_MAX) {
        mp_raise_ValueError(translate("Data too large for advertisement packet"));
    }
    memcpy(self->adv_data, data->buf, data->len);

    ble_gap_adv_params_t m_adv_params = {
        .interval = (uint32_t) (self->interval / ADV_INTERVAL_UNIT_FLOAT_SECS),
        .properties.type = BLE_GAP_ADV_TYPE_NONCONNECTABLE_SCANNABLE_UNDIRECTED,
        .duration = BLE_GAP_ADV_TIMEOUT_GENERAL_UNLIMITED,
        .filter_policy = BLE_GAP_ADV_FP_ANY,
        .primary_phy = BLE_GAP_PHY_1MBPS,
    };

    common_hal_bleio_broadcaster_stop_advertising(self);

    const ble_gap_adv_data_t ble_gap_adv_data = {
        .adv_data.p_data = self->adv_data,
        .adv_data.len = data->len,
    };

    err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, &ble_gap_adv_data, &m_adv_params);
    if (err_code == NRF_SUCCESS) {
        err_code = sd_ble_gap_adv_start(m_adv_handle, BLE_CONN_CFG_TAG_CUSTOM);
    }

    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg_varg(translate("Failed to start advertising, err 0x%04x"), err_code);
    }
}

void common_hal_bleio_broadcaster_stop_advertising(bleio_broadcaster_obj_t *self) {

    if (m_adv_handle == BLE_GAP_ADV_SET_HANDLE_NOT_SET) {
        return;
    }

    const uint32_t err_code = sd_ble_gap_adv_stop(m_adv_handle);

    if ((err_code != NRF_SUCCESS) && (err_code != NRF_ERROR_INVALID_STATE)) {
        mp_raise_OSError_msg_varg(translate("Failed to stop advertising, err 0x%04x"), err_code);
    }
}
