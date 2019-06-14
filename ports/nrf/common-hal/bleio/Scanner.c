/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
 * Copyright (c) 2018 Artur Pacholec
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

#include "ble_drv.h"
#include "ble_gap.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "shared-bindings/bleio/Adapter.h"
#include "shared-bindings/bleio/ScanEntry.h"
#include "shared-bindings/bleio/Scanner.h"
#include "shared-module/bleio/ScanEntry.h"

static uint8_t m_scan_buffer_data[BLE_GAP_SCAN_BUFFER_MIN];

static ble_data_t m_scan_buffer = {
    m_scan_buffer_data,
    BLE_GAP_SCAN_BUFFER_MIN
};

STATIC void on_ble_evt(ble_evt_t *ble_evt, void *scanner_in) {
    bleio_scanner_obj_t *scanner = (bleio_scanner_obj_t*)scanner_in;
    ble_gap_evt_adv_report_t *report = &ble_evt->evt.gap_evt.params.adv_report;

    if (ble_evt->header.evt_id != BLE_GAP_EVT_ADV_REPORT) {
        return;
    }

    bleio_scanentry_obj_t *entry = m_new_obj(bleio_scanentry_obj_t);
    entry->base.type = &bleio_scanentry_type;
    entry->rssi = report->rssi;

    memcpy(entry->address.bytes, report->data.p_data, NUM_BLEIO_ADDRESS_BYTES);
    entry->address.type = report->peer_addr.addr_type;

    entry->data = mp_obj_new_bytes(report->data.p_data, report->data.len);

    mp_obj_list_append(scanner->adv_reports, entry);

    const uint32_t err_code = sd_ble_gap_scan_start(NULL, &m_scan_buffer);
    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg_varg(translate("Failed to continue scanning, err 0x%04x"), err_code);
    }
}

void common_hal_bleio_scanner_scan(bleio_scanner_obj_t *self, mp_float_t timeout, mp_float_t interval, mp_float_t window) {
    common_hal_bleio_adapter_set_enabled(true);
    ble_drv_add_event_handler(on_ble_evt, self);

    ble_gap_scan_params_t scan_params = {
        .interval = SEC_TO_UNITS(interval, UNIT_0_625_MS),
        .window = SEC_TO_UNITS(window, UNIT_0_625_MS),
        .scan_phys = BLE_GAP_PHY_1MBPS,
    };

    common_hal_bleio_adapter_set_enabled(true);

    uint32_t err_code;
    err_code = sd_ble_gap_scan_start(&scan_params, &m_scan_buffer);

    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg_varg(translate("Failed to start scanning, err 0x%04x"), err_code);
    }

    mp_hal_delay_ms(timeout * 1000);
    sd_ble_gap_scan_stop();
}
