/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2021 Damien P. George
 * Copyright (c) 2019-2020 Jim Mussared
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

#if MICROPY_PY_BLUETOOTH

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include "extmod/modbluetooth.h"

#define DEBUG_printf(...) // printk("BLE: " __VA_ARGS__)

#define BLE_HCI_SCAN_ITVL_MIN 0x10
#define BLE_HCI_SCAN_ITVL_MAX 0xffff
#define BLE_HCI_SCAN_WINDOW_MIN 0x10
#define BLE_HCI_SCAN_WINDOW_MAX 0xffff

#define ERRNO_BLUETOOTH_NOT_ACTIVE MP_ENODEV

enum {
    MP_BLUETOOTH_ZEPHYR_BLE_STATE_OFF,
    MP_BLUETOOTH_ZEPHYR_BLE_STATE_ACTIVE,
    MP_BLUETOOTH_ZEPHYR_BLE_STATE_SUSPENDED,
};

enum {
    MP_BLUETOOTH_ZEPHYR_GAP_SCAN_STATE_INACTIVE,
    MP_BLUETOOTH_ZEPHYR_GAP_SCAN_STATE_DEACTIVATING,
    MP_BLUETOOTH_ZEPHYR_GAP_SCAN_STATE_ACTIVE,
};

typedef struct _mp_bluetooth_zephyr_root_pointers_t {
    // Characteristic (and descriptor) value storage.
    mp_gatts_db_t gatts_db;
} mp_bluetooth_zephyr_root_pointers_t;

STATIC int mp_bluetooth_zephyr_ble_state;

#if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
STATIC int mp_bluetooth_zephyr_gap_scan_state;
STATIC struct k_timer mp_bluetooth_zephyr_gap_scan_timer;
STATIC struct bt_le_scan_cb mp_bluetooth_zephyr_gap_scan_cb_struct;
#endif

STATIC int bt_err_to_errno(int err) {
    // Zephyr uses errno codes directly, but they are negative.
    return -err;
}

// modbluetooth (and the layers above it) work in BE for addresses, Zephyr works in LE.
STATIC void reverse_addr_byte_order(uint8_t *addr_out, const bt_addr_le_t *addr_in) {
    for (int i = 0; i < 6; ++i) {
        addr_out[i] = addr_in->a.val[5 - i];
    }
}

#if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
void gap_scan_cb_recv(const struct bt_le_scan_recv_info *info, struct net_buf_simple *buf) {
    DEBUG_printf("gap_scan_cb_recv: adv_type=%d\n", info->adv_type);

    if (!mp_bluetooth_is_active()) {
        return;
    }

    if (mp_bluetooth_zephyr_gap_scan_state != MP_BLUETOOTH_ZEPHYR_GAP_SCAN_STATE_ACTIVE) {
        return;
    }

    uint8_t addr[6];
    reverse_addr_byte_order(addr, info->addr);
    mp_bluetooth_gap_on_scan_result(info->addr->type, addr, info->adv_type, info->rssi, buf->data, buf->len);
}

STATIC mp_obj_t gap_scan_stop(mp_obj_t unused) {
    (void)unused;
    mp_bluetooth_gap_scan_stop();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(gap_scan_stop_obj, gap_scan_stop);

void gap_scan_cb_timeout(struct k_timer *timer_id) {
    DEBUG_printf("gap_scan_cb_timeout\n");
    // Cannot call bt_le_scan_stop from a timer callback because this callback may be
    // preempting the BT stack.  So schedule it to be called from the main thread.
    while (!mp_sched_schedule(MP_OBJ_FROM_PTR(&gap_scan_stop_obj), mp_const_none)) {
        k_yield();
    }
    // Indicate scanning has stopped so that no more scan result events are generated
    // (they may still come in until bt_le_scan_stop is called by gap_scan_stop).
    mp_bluetooth_zephyr_gap_scan_state = MP_BLUETOOTH_ZEPHYR_GAP_SCAN_STATE_DEACTIVATING;
}
#endif

int mp_bluetooth_init(void) {
    DEBUG_printf("mp_bluetooth_init\n");

    // Clean up if necessary.
    mp_bluetooth_deinit();

    // Allocate memory for state.
    MP_STATE_PORT(bluetooth_zephyr_root_pointers) = m_new0(mp_bluetooth_zephyr_root_pointers_t, 1);
    mp_bluetooth_gatts_db_create(&MP_STATE_PORT(bluetooth_zephyr_root_pointers)->gatts_db);

    #if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
    mp_bluetooth_zephyr_gap_scan_state = MP_BLUETOOTH_ZEPHYR_GAP_SCAN_STATE_INACTIVE;
    k_timer_init(&mp_bluetooth_zephyr_gap_scan_timer, gap_scan_cb_timeout, NULL);
    mp_bluetooth_zephyr_gap_scan_cb_struct.recv = gap_scan_cb_recv;
    mp_bluetooth_zephyr_gap_scan_cb_struct.timeout = NULL; // currently not implemented in Zephyr
    bt_le_scan_cb_register(&mp_bluetooth_zephyr_gap_scan_cb_struct);
    #endif

    if (mp_bluetooth_zephyr_ble_state == MP_BLUETOOTH_ZEPHYR_BLE_STATE_OFF) {
        // bt_enable can only be called once.
        int ret = bt_enable(NULL);
        if (ret) {
            return bt_err_to_errno(ret);
        }
    }

    mp_bluetooth_zephyr_ble_state = MP_BLUETOOTH_ZEPHYR_BLE_STATE_ACTIVE;

    DEBUG_printf("mp_bluetooth_init: ready\n");

    return 0;
}

void mp_bluetooth_deinit(void) {
    DEBUG_printf("mp_bluetooth_deinit %d\n", mp_bluetooth_zephyr_ble_state);
    if (mp_bluetooth_zephyr_ble_state == MP_BLUETOOTH_ZEPHYR_BLE_STATE_OFF
        || mp_bluetooth_zephyr_ble_state == MP_BLUETOOTH_ZEPHYR_BLE_STATE_SUSPENDED) {
        return;
    }

    mp_bluetooth_gap_advertise_stop();

    #if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
    mp_bluetooth_gap_scan_stop();
    bt_le_scan_cb_unregister(&mp_bluetooth_zephyr_gap_scan_cb_struct);
    #endif

    // There is no way to turn off the BT stack in Zephyr, so just set the
    // state as suspended so it can be correctly reactivated later.
    mp_bluetooth_zephyr_ble_state = MP_BLUETOOTH_ZEPHYR_BLE_STATE_SUSPENDED;

    MP_STATE_PORT(bluetooth_zephyr_root_pointers) = NULL;
}

bool mp_bluetooth_is_active(void) {
    return mp_bluetooth_zephyr_ble_state == MP_BLUETOOTH_ZEPHYR_BLE_STATE_ACTIVE;
}

void mp_bluetooth_get_current_address(uint8_t *addr_type, uint8_t *addr) {
    if (!mp_bluetooth_is_active()) {
        mp_raise_OSError(ERRNO_BLUETOOTH_NOT_ACTIVE);
    }
    bt_addr_le_t le_addr;
    size_t count = 1;
    bt_id_get(&le_addr, &count);
    if (count == 0) {
        mp_raise_OSError(EIO);
    }
    reverse_addr_byte_order(addr, &le_addr);
    *addr_type = le_addr.type;
}

void mp_bluetooth_set_address_mode(uint8_t addr_mode) {
    // TODO: implement
}

size_t mp_bluetooth_gap_get_device_name(const uint8_t **buf) {
    const char *name = bt_get_name();
    *buf = (const uint8_t *)name;
    return strlen(name);
}

int mp_bluetooth_gap_set_device_name(const uint8_t *buf, size_t len) {
    char tmp_buf[CONFIG_BT_DEVICE_NAME_MAX + 1];
    if (len + 1 > sizeof(tmp_buf)) {
        return MP_EINVAL;
    }
    memcpy(tmp_buf, buf, len);
    tmp_buf[len] = '\0';
    return bt_err_to_errno(bt_set_name(tmp_buf));
}

// Zephyr takes advertising/scan data as an array of (type, len, payload) packets,
// and this function constructs such an array from raw advertising/scan data.
STATIC void mp_bluetooth_prepare_bt_data(const uint8_t *data, size_t len, struct bt_data *bt_data, size_t *bt_len) {
    size_t i = 0;
    const uint8_t *d = data;
    while (d < data + len && i < *bt_len) {
        bt_data[i].type = d[1];
        bt_data[i].data_len = d[0] - 1;
        bt_data[i].data = &d[2];
        i += 1;
        d += 1 + d[0];
    }
    *bt_len = i;
}

int mp_bluetooth_gap_advertise_start(bool connectable, int32_t interval_us, const uint8_t *adv_data, size_t adv_data_len, const uint8_t *sr_data, size_t sr_data_len) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }

    mp_bluetooth_gap_advertise_stop();

    struct bt_data bt_ad_data[8];
    size_t bt_ad_len = 0;
    if (adv_data) {
        bt_ad_len = MP_ARRAY_SIZE(bt_ad_data);
        mp_bluetooth_prepare_bt_data(adv_data, adv_data_len, bt_ad_data, &bt_ad_len);
    }

    struct bt_data bt_sd_data[8];
    size_t bt_sd_len = 0;
    if (sr_data) {
        bt_sd_len = MP_ARRAY_SIZE(bt_sd_data);
        mp_bluetooth_prepare_bt_data(sr_data, sr_data_len, bt_sd_data, &bt_sd_len);
    }

    struct bt_le_adv_param param = {
        .id = 0,
        .sid = 0,
        .secondary_max_skip = 0,
        .options = (connectable ? BT_LE_ADV_OPT_CONNECTABLE : 0)
            | BT_LE_ADV_OPT_ONE_TIME
            | BT_LE_ADV_OPT_USE_IDENTITY
            | BT_LE_ADV_OPT_SCANNABLE,
        .interval_min = interval_us / 625,
        .interval_max = interval_us / 625 + 1, // min/max cannot be the same value
        .peer = NULL,
    };

    return bt_err_to_errno(bt_le_adv_start(&param, bt_ad_data, bt_ad_len, bt_sd_data, bt_sd_len));
}

void mp_bluetooth_gap_advertise_stop(void) {
    // Note: bt_le_adv_stop returns 0 if adv is already stopped.
    int ret = bt_le_adv_stop();
    if (ret != 0) {
        mp_raise_OSError(bt_err_to_errno(ret));
    }
}

int mp_bluetooth_gatts_register_service_begin(bool append) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }

    if (append) {
        // Don't support append yet (modbluetooth.c doesn't support it yet anyway).
        return MP_EOPNOTSUPP;
    }

    // Reset the gatt characteristic value db.
    mp_bluetooth_gatts_db_reset(MP_STATE_PORT(bluetooth_zephyr_root_pointers)->gatts_db);

    return MP_EOPNOTSUPP;
}

int mp_bluetooth_gatts_register_service_end(void) {
    return MP_EOPNOTSUPP;
}

int mp_bluetooth_gatts_register_service(mp_obj_bluetooth_uuid_t *service_uuid, mp_obj_bluetooth_uuid_t **characteristic_uuids, uint16_t *characteristic_flags, mp_obj_bluetooth_uuid_t **descriptor_uuids, uint16_t *descriptor_flags, uint8_t *num_descriptors, uint16_t *handles, size_t num_characteristics) {
    return MP_EOPNOTSUPP;
}

int mp_bluetooth_gap_disconnect(uint16_t conn_handle) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    return MP_EOPNOTSUPP;
}

int mp_bluetooth_gatts_read(uint16_t value_handle, uint8_t **value, size_t *value_len) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    return mp_bluetooth_gatts_db_read(MP_STATE_PORT(bluetooth_zephyr_root_pointers)->gatts_db, value_handle, value, value_len);
}

int mp_bluetooth_gatts_write(uint16_t value_handle, const uint8_t *value, size_t value_len, bool send_update) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    if (send_update) {
        return MP_EOPNOTSUPP;
    }
    return mp_bluetooth_gatts_db_write(MP_STATE_PORT(bluetooth_zephyr_root_pointers)->gatts_db, value_handle, value, value_len);
}

int mp_bluetooth_gatts_notify(uint16_t conn_handle, uint16_t value_handle) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    return MP_EOPNOTSUPP;
}

int mp_bluetooth_gatts_notify_send(uint16_t conn_handle, uint16_t value_handle, const uint8_t *value, size_t value_len) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    return MP_EOPNOTSUPP;
}

int mp_bluetooth_gatts_indicate(uint16_t conn_handle, uint16_t value_handle) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    return MP_EOPNOTSUPP;
}

int mp_bluetooth_gatts_set_buffer(uint16_t value_handle, size_t len, bool append) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    return MP_EOPNOTSUPP;
}

int mp_bluetooth_get_preferred_mtu(void) {
    if (!mp_bluetooth_is_active()) {
        mp_raise_OSError(ERRNO_BLUETOOTH_NOT_ACTIVE);
    }
    mp_raise_OSError(MP_EOPNOTSUPP);
}

int mp_bluetooth_set_preferred_mtu(uint16_t mtu) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    return MP_EOPNOTSUPP;
}

#if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE

int mp_bluetooth_gap_scan_start(int32_t duration_ms, int32_t interval_us, int32_t window_us, bool active_scan) {
    // Stop any ongoing GAP scan.
    int ret = mp_bluetooth_gap_scan_stop();
    if (ret) {
        return ret;
    }

    struct bt_le_scan_param param = {
        .type = active_scan ? BT_HCI_LE_SCAN_ACTIVE : BT_HCI_LE_SCAN_PASSIVE,
        .options = BT_LE_SCAN_OPT_NONE,
        .interval = MAX(BLE_HCI_SCAN_ITVL_MIN, MIN(BLE_HCI_SCAN_ITVL_MAX, interval_us / 625)),
        .window = MAX(BLE_HCI_SCAN_WINDOW_MIN, MIN(BLE_HCI_SCAN_WINDOW_MAX, window_us / 625)),
    };
    k_timer_start(&mp_bluetooth_zephyr_gap_scan_timer, K_MSEC(duration_ms), K_NO_WAIT);
    mp_bluetooth_zephyr_gap_scan_state = MP_BLUETOOTH_ZEPHYR_GAP_SCAN_STATE_ACTIVE;
    int err = bt_le_scan_start(&param, NULL);
    return bt_err_to_errno(err);
}

int mp_bluetooth_gap_scan_stop(void) {
    DEBUG_printf("mp_bluetooth_gap_scan_stop\n");
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    if (mp_bluetooth_zephyr_gap_scan_state == MP_BLUETOOTH_ZEPHYR_GAP_SCAN_STATE_INACTIVE) {
        // Already stopped.
        return 0;
    }
    mp_bluetooth_zephyr_gap_scan_state = MP_BLUETOOTH_ZEPHYR_GAP_SCAN_STATE_INACTIVE;
    k_timer_stop(&mp_bluetooth_zephyr_gap_scan_timer);
    int err = bt_le_scan_stop();
    if (err == 0) {
        mp_bluetooth_gap_on_scan_complete();
        return 0;
    }
    return bt_err_to_errno(err);
}

int mp_bluetooth_gap_peripheral_connect(uint8_t addr_type, const uint8_t *addr, int32_t duration_ms, int32_t min_conn_interval_us, int32_t max_conn_interval_us) {
    DEBUG_printf("mp_bluetooth_gap_peripheral_connect\n");
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    return MP_EOPNOTSUPP;
}

#endif // MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE

#endif // MICROPY_PY_BLUETOOTH
