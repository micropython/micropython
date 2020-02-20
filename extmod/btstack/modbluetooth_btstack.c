/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Damien P. George
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

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"

#if MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_BTSTACK

#include "extmod/btstack/modbluetooth_btstack.h"
#include "extmod/modbluetooth.h"

#include "lib/btstack/src/btstack.h"

volatile int mp_bluetooth_btstack_state = MP_BLUETOOTH_BTSTACK_STATE_OFF;

int mp_bluetooth_init(void) {
    btstack_memory_init();

    MP_STATE_PORT(bluetooth_btstack_root_pointers) = m_new0(mp_bluetooth_btstack_root_pointers_t, 1);

    mp_bluetooth_btstack_port_init();
    mp_bluetooth_btstack_state = MP_BLUETOOTH_BTSTACK_STATE_STARTING;

    l2cap_init();
    le_device_db_init();
    sm_init();

    // Set blank ER/IR keys to suppress BTstack warning.
    // TODO handle this correctly.
    sm_key_t dummy_key;
    memset(dummy_key, 0, sizeof(dummy_key));
    sm_set_er(dummy_key);
    sm_set_ir(dummy_key);

    mp_bluetooth_btstack_port_start();
    mp_bluetooth_btstack_state = MP_BLUETOOTH_BTSTACK_STATE_ACTIVE;
    return 0;
}

void mp_bluetooth_deinit(void) {
    mp_bluetooth_btstack_port_deinit();
    mp_bluetooth_btstack_state = MP_BLUETOOTH_BTSTACK_STATE_OFF;

    MP_STATE_PORT(bluetooth_btstack_root_pointers) = NULL;
}

bool mp_bluetooth_is_enabled(void) {
    return mp_bluetooth_btstack_state == MP_BLUETOOTH_BTSTACK_STATE_ACTIVE;
}

void mp_bluetooth_get_device_addr(uint8_t *addr) {
    mp_hal_get_mac(MP_HAL_MAC_BDADDR, addr);
}

int mp_bluetooth_gap_advertise_start(bool connectable, int32_t interval_us, const uint8_t *adv_data, size_t adv_data_len, const uint8_t *sr_data, size_t sr_data_len) {
    uint16_t adv_int_min = interval_us / 625;
    uint16_t adv_int_max = interval_us / 625;
    uint8_t adv_type = connectable ? 0 : 2;
    bd_addr_t null_addr = {0};

    uint8_t direct_address_type = 0;
    uint8_t channel_map = 0x07; // Use all three broadcast channels.
    uint8_t filter_policy = 0x00; // None.

    gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, direct_address_type, null_addr, channel_map, filter_policy);

    // Copy the adv_data and sr_data into a persistent buffer (which is findable via the btstack root pointers).
    size_t total_bytes = adv_data_len + sr_data_len;
    if (total_bytes > MP_STATE_PORT(bluetooth_btstack_root_pointers)->adv_data_alloc) {
        // Resize if necessary.
        MP_STATE_PORT(bluetooth_btstack_root_pointers)->adv_data = m_new(uint8_t, total_bytes);
        MP_STATE_PORT(bluetooth_btstack_root_pointers)->adv_data_alloc = total_bytes;
    }
    uint8_t *data = MP_STATE_PORT(bluetooth_btstack_root_pointers)->adv_data;

    if (adv_data) {
        memcpy(data, (uint8_t *)adv_data, adv_data_len);
        gap_advertisements_set_data(adv_data_len, data);
        data += adv_data_len;
    }
    if (sr_data) {
        memcpy(data, (uint8_t *)sr_data, sr_data_len);
        gap_scan_response_set_data(sr_data_len, data);
    }

    gap_advertisements_enable(true);
    return 0;
}

void mp_bluetooth_gap_advertise_stop(void) {
    gap_advertisements_enable(false);
    MP_STATE_PORT(bluetooth_btstack_root_pointers)->adv_data_alloc = 0;
    MP_STATE_PORT(bluetooth_btstack_root_pointers)->adv_data = NULL;
}

int mp_bluetooth_gatts_register_service_begin(bool append) {
    return 0;
}

int mp_bluetooth_gatts_register_service(mp_obj_bluetooth_uuid_t *service_uuid, mp_obj_bluetooth_uuid_t **characteristic_uuids, uint8_t *characteristic_flags, mp_obj_bluetooth_uuid_t **descriptor_uuids, uint8_t *descriptor_flags, uint8_t *num_descriptors, uint16_t *handles, size_t num_characteristics) {
    return 0;
}

int mp_bluetooth_gatts_register_service_end(void) {
    return 0;
}

int mp_bluetooth_gatts_read(uint16_t value_handle, uint8_t **value, size_t *value_len) {
    return 0;
}

int mp_bluetooth_gatts_write(uint16_t value_handle, const uint8_t *value, size_t value_len) {
    return 0;
}

int mp_bluetooth_gatts_notify(uint16_t conn_handle, uint16_t value_handle) {
    return 0;
}

int mp_bluetooth_gatts_notify_send(uint16_t conn_handle, uint16_t value_handle, const uint8_t *value, size_t *value_len) {
    return 0;
}

int mp_bluetooth_gatts_indicate(uint16_t conn_handle, uint16_t value_handle) {
    return 0;
}

int mp_bluetooth_gatts_set_buffer(uint16_t value_handle, size_t len, bool append) {
    return 0;
}

int mp_bluetooth_gap_disconnect(uint16_t conn_handle) {
    return 0;
}

#if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
int mp_bluetooth_gap_scan_start(int32_t duration_ms, int32_t interval_us, int32_t window_us) {
    return 0;
}

int mp_bluetooth_gap_scan_stop(void) {
    return 0;
}

int mp_bluetooth_gap_peripheral_connect(uint8_t addr_type, const uint8_t *addr, int32_t duration_ms) {
    return 0;
}

int mp_bluetooth_gattc_discover_primary_services(uint16_t conn_handle) {
    return 0;
}

int mp_bluetooth_gattc_discover_characteristics(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle) {
    return 0;
}

int mp_bluetooth_gattc_discover_descriptors(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle) {
    return 0;
}

int mp_bluetooth_gattc_read(uint16_t conn_handle, uint16_t value_handle) {
    return 0;
}

int mp_bluetooth_gattc_write(uint16_t conn_handle, uint16_t value_handle, const uint8_t *value, size_t *value_len, unsigned int mode) {
    return 0;
}
#endif

#endif // MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_BTSTACK
