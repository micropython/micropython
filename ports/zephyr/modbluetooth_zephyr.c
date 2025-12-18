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

#include <zephyr/types.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include "extmod/modbluetooth.h"

#define DEBUG_printf(...) // printk("BLE: " __VA_ARGS__)

#define BLE_HCI_SCAN_ITVL_MIN 0x10
#define BLE_HCI_SCAN_ITVL_MAX 0xffff
#define BLE_HCI_SCAN_WINDOW_MIN 0x10
#define BLE_HCI_SCAN_WINDOW_MAX 0xffff

#define ERRNO_BLUETOOTH_NOT_ACTIVE MP_ENODEV

#define MP_BLUETOOTH_ZEPHYR_MAX_SERVICES            (8)

/* This masks Permission bits from GATT API */
#define GATT_PERM_MASK  (BT_GATT_PERM_READ | \
    BT_GATT_PERM_READ_AUTHEN | \
    BT_GATT_PERM_READ_ENCRYPT | \
    BT_GATT_PERM_WRITE | \
    BT_GATT_PERM_WRITE_AUTHEN | \
    BT_GATT_PERM_WRITE_ENCRYPT | \
    BT_GATT_PERM_PREPARE_WRITE)

#define GATT_PERM_ENC_READ_MASK     (BT_GATT_PERM_READ_ENCRYPT | \
    BT_GATT_PERM_READ_AUTHEN)

#define GATT_PERM_ENC_WRITE_MASK    (BT_GATT_PERM_WRITE_ENCRYPT | \
    BT_GATT_PERM_WRITE_AUTHEN)

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

union uuid_u {
    struct bt_uuid uuid;
    struct bt_uuid_16 u16;
    struct bt_uuid_32 u32;
    struct bt_uuid_128 u128;
};

struct add_characteristic {
    uint8_t properties;
    uint8_t permissions;
    const struct bt_uuid *uuid;
};

struct add_descriptor {
    uint8_t permissions;
    const struct bt_uuid *uuid;
};

typedef struct _mp_bt_zephyr_conn_t {
    struct bt_conn *conn;
    struct _mp_bt_zephyr_conn_t *next;
} mp_bt_zephyr_conn_t;

typedef struct _mp_bluetooth_zephyr_root_pointers_t {
    // list of objects to be tracked by the gc
    mp_obj_t objs_list;

    // Characteristic (and descriptor) value storage.
    mp_gatts_db_t gatts_db;

    // Service definitions.
    size_t n_services;
    struct bt_gatt_service *services[MP_BLUETOOTH_ZEPHYR_MAX_SERVICES];

    // active connections
    mp_bt_zephyr_conn_t *connections;
} mp_bluetooth_zephyr_root_pointers_t;

static int mp_bluetooth_zephyr_ble_state;

#if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
static int mp_bluetooth_zephyr_gap_scan_state;
static struct k_timer mp_bluetooth_zephyr_gap_scan_timer;
static struct bt_le_scan_cb mp_bluetooth_zephyr_gap_scan_cb_struct;
#endif

static struct bt_data bt_ad_data[8];
static size_t bt_ad_len = 0;
static struct bt_data bt_sd_data[8];
static size_t bt_sd_len = 0;

static mp_bt_zephyr_conn_t *mp_bt_zephyr_next_conn;

static mp_bt_zephyr_conn_t *mp_bt_zephyr_find_connection(uint8_t conn_handle);
static void mp_bt_zephyr_insert_connection(mp_bt_zephyr_conn_t *connection);
static void mp_bt_zephyr_remove_connection(uint8_t conn_handle);
static void mp_bt_zephyr_connected(struct bt_conn *connected, uint8_t err);
static void mp_bt_zephyr_disconnected(struct bt_conn *disconn, uint8_t reason);
static struct bt_uuid *create_zephyr_uuid(const mp_obj_bluetooth_uuid_t *uuid);
static void gatt_db_add(const struct bt_gatt_attr *pattern, struct bt_gatt_attr *attr, size_t user_data_len);
static void add_service(const struct bt_uuid *u, struct bt_gatt_attr *attr);
static void add_characteristic(struct add_characteristic *ch, struct bt_gatt_attr *attr_chrc, struct bt_gatt_attr *attr_value);
static void add_ccc(struct bt_gatt_attr *attr, struct bt_gatt_attr *attr_desc);
static void add_cep(const struct bt_gatt_attr *attr_chrc, struct bt_gatt_attr *attr_desc);
static void add_descriptor(struct bt_gatt_attr *chrc, struct add_descriptor *d, struct bt_gatt_attr *attr_desc);
static void mp_bt_zephyr_gatt_indicate_done(struct bt_conn *conn, struct bt_gatt_indicate_params *params, uint8_t err);
static struct bt_gatt_attr *mp_bt_zephyr_find_attr_by_handle(uint16_t value_handle);

static struct bt_conn_cb mp_bt_zephyr_conn_callbacks = {
    .connected = mp_bt_zephyr_connected,
    .disconnected = mp_bt_zephyr_disconnected,
};

static mp_bt_zephyr_conn_t *mp_bt_zephyr_find_connection(uint8_t conn_handle) {
    struct bt_conn_info info;
    for (mp_bt_zephyr_conn_t *connection = MP_STATE_PORT(bluetooth_zephyr_root_pointers)->connections; connection != NULL; connection = connection->next) {
        if (connection->conn) {
            bt_conn_get_info(connection->conn, &info);
            if (info.id == conn_handle) {
                return connection;
            }
        }
    }
    return NULL;
}

static void mp_bt_zephyr_insert_connection(mp_bt_zephyr_conn_t *connection) {
    connection->next = MP_STATE_PORT(bluetooth_zephyr_root_pointers)->connections;
    MP_STATE_PORT(bluetooth_zephyr_root_pointers)->connections = connection;
}

static void mp_bt_zephyr_remove_connection(uint8_t conn_handle) {
    struct bt_conn_info info;
    mp_bt_zephyr_conn_t *prev = NULL;
    for (mp_bt_zephyr_conn_t *connection = MP_STATE_PORT(bluetooth_zephyr_root_pointers)->connections; connection != NULL; connection = connection->next) {
        if (connection->conn) {
            bt_conn_get_info(connection->conn, &info);
            if (info.id == conn_handle) {
                // unlink this item and the gc will eventually collect it
                if (prev != NULL) {
                    prev->next = connection->next;
                } else {
                    // move the start pointer
                    MP_STATE_PORT(bluetooth_zephyr_root_pointers)->connections = connection->next;
                }
                break;
            } else {
                prev = connection;
            }
        }
    }
}

static void mp_bt_zephyr_connected(struct bt_conn *conn, uint8_t err) {
    struct bt_conn_info info;
    bt_conn_get_info(conn, &info);

    if (err) {
        uint8_t addr[6] = {0};
        DEBUG_printf("Connection from central failed (err %u)\n", err);
        mp_bluetooth_gap_on_connected_disconnected(MP_BLUETOOTH_IRQ_CENTRAL_DISCONNECT, info.id, 0xff, addr);
    } else {
        DEBUG_printf("Central connected with id %d\n", info.id);
        mp_bt_zephyr_next_conn->conn = bt_conn_ref(conn);
        mp_bluetooth_gap_on_connected_disconnected(MP_BLUETOOTH_IRQ_CENTRAL_CONNECT, info.id, info.le.dst->type, info.le.dst->a.val);
        mp_bt_zephyr_insert_connection(mp_bt_zephyr_next_conn);
    }
}

static void mp_bt_zephyr_disconnected(struct bt_conn *conn, uint8_t reason) {
    struct bt_conn_info info;
    bt_conn_get_info(conn, &info);
    DEBUG_printf("Central disconnected (id %d reason %u)\n", info.id, reason);
    bt_conn_unref(conn);
    mp_bt_zephyr_remove_connection(info.id);
    mp_bluetooth_gap_on_connected_disconnected(MP_BLUETOOTH_IRQ_CENTRAL_DISCONNECT, info.id, info.le.dst->type, info.le.dst->a.val);
}

static int bt_err_to_errno(int err) {
    // Zephyr uses errno codes directly, but they are negative.
    return -err;
}

// modbluetooth (and the layers above it) work in BE for addresses, Zephyr works in LE.
static void reverse_addr_byte_order(uint8_t *addr_out, const bt_addr_le_t *addr_in) {
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

static mp_obj_t gap_scan_stop(mp_obj_t unused) {
    (void)unused;
    mp_bluetooth_gap_scan_stop();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(gap_scan_stop_obj, gap_scan_stop);

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

    MP_STATE_PORT(bluetooth_zephyr_root_pointers)->connections = NULL;
    mp_bt_zephyr_next_conn = NULL;

    MP_STATE_PORT(bluetooth_zephyr_root_pointers)->objs_list = mp_obj_new_list(0, NULL);

    #if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
    mp_bluetooth_zephyr_gap_scan_state = MP_BLUETOOTH_ZEPHYR_GAP_SCAN_STATE_INACTIVE;
    k_timer_init(&mp_bluetooth_zephyr_gap_scan_timer, gap_scan_cb_timeout, NULL);
    mp_bluetooth_zephyr_gap_scan_cb_struct.recv = gap_scan_cb_recv;
    mp_bluetooth_zephyr_gap_scan_cb_struct.timeout = NULL; // currently not implemented in Zephyr
    bt_le_scan_cb_register(&mp_bluetooth_zephyr_gap_scan_cb_struct);
    #endif

    if (mp_bluetooth_zephyr_ble_state == MP_BLUETOOTH_ZEPHYR_BLE_STATE_OFF) {

        bt_conn_cb_register(&mp_bt_zephyr_conn_callbacks);

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

int mp_bluetooth_deinit(void) {
    DEBUG_printf("mp_bluetooth_deinit %d\n", mp_bluetooth_zephyr_ble_state);
    if (mp_bluetooth_zephyr_ble_state == MP_BLUETOOTH_ZEPHYR_BLE_STATE_OFF
        || mp_bluetooth_zephyr_ble_state == MP_BLUETOOTH_ZEPHYR_BLE_STATE_SUSPENDED) {
        return 0;
    }

    mp_bluetooth_gap_advertise_stop();

    #if CONFIG_BT_GATT_DYNAMIC_DB
    for (size_t i = 0; i < MP_STATE_PORT(bluetooth_zephyr_root_pointers)->n_services; ++i) {
        bt_gatt_service_unregister(MP_STATE_PORT(bluetooth_zephyr_root_pointers)->services[i]);
        MP_STATE_PORT(bluetooth_zephyr_root_pointers)->services[i] = NULL;
    }
    #endif

    #if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
    mp_bluetooth_gap_scan_stop();
    bt_le_scan_cb_unregister(&mp_bluetooth_zephyr_gap_scan_cb_struct);
    #endif

    // There is no way to turn off the BT stack in Zephyr, so just set the
    // state as suspended so it can be correctly reactivated later.
    mp_bluetooth_zephyr_ble_state = MP_BLUETOOTH_ZEPHYR_BLE_STATE_SUSPENDED;

    MP_STATE_PORT(bluetooth_zephyr_root_pointers) = NULL;
    mp_bt_zephyr_next_conn = NULL;
    return 0;
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
    mp_raise_OSError(MP_EOPNOTSUPP);
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
static void mp_bluetooth_prepare_bt_data(const uint8_t *data, size_t len, struct bt_data *bt_data, size_t *bt_len) {
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

    if (adv_data) {
        bt_ad_len = MP_ARRAY_SIZE(bt_ad_data);
        mp_bluetooth_prepare_bt_data(adv_data, adv_data_len, bt_ad_data, &bt_ad_len);
    }

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

    // pre-allocate a new connection structure as we cannot allocate this inside the connection callback
    mp_bt_zephyr_next_conn = m_new0(mp_bt_zephyr_conn_t, 1);
    mp_obj_list_append(MP_STATE_PORT(bluetooth_zephyr_root_pointers)->objs_list, MP_OBJ_FROM_PTR(mp_bt_zephyr_next_conn));

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
    #if CONFIG_BT_GATT_DYNAMIC_DB

    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }

    if (append) {
        // Don't support append yet (modbluetooth.c doesn't support it yet anyway).
        return MP_EOPNOTSUPP;
    }

    // Unregister and unref any previous service definitions.
    for (size_t i = 0; i < MP_STATE_PORT(bluetooth_zephyr_root_pointers)->n_services; ++i) {
        bt_gatt_service_unregister(MP_STATE_PORT(bluetooth_zephyr_root_pointers)->services[i]);
        MP_STATE_PORT(bluetooth_zephyr_root_pointers)->services[i] = NULL;
    }
    MP_STATE_PORT(bluetooth_zephyr_root_pointers)->n_services = 0;

    // Reset the gatt characteristic value db.
    mp_bluetooth_gatts_db_reset(MP_STATE_PORT(bluetooth_zephyr_root_pointers)->gatts_db);
    MP_STATE_PORT(bluetooth_zephyr_root_pointers)->connections = NULL;
    MP_STATE_PORT(bluetooth_zephyr_root_pointers)->objs_list = mp_obj_new_list(0, NULL);
    mp_bt_zephyr_next_conn = NULL;

    return 0;

    #else
    return MP_EOPNOTSUPP;
    #endif
}

int mp_bluetooth_gatts_register_service_end(void) {
    return 0;
}

int mp_bluetooth_gatts_register_service(mp_obj_bluetooth_uuid_t *service_uuid, mp_obj_bluetooth_uuid_t **characteristic_uuids, uint16_t *characteristic_flags, mp_obj_bluetooth_uuid_t **descriptor_uuids, uint16_t *descriptor_flags, uint8_t *num_descriptors, uint16_t *handles, size_t num_characteristics) {
    #if CONFIG_BT_GATT_DYNAMIC_DB
    if (MP_STATE_PORT(bluetooth_zephyr_root_pointers)->n_services >= MP_BLUETOOTH_ZEPHYR_MAX_SERVICES) {
        return MP_E2BIG;
    }

    // first of all allocate the entire memory for all the attributes that this service is composed of
    // 1 for the service itself, 2 for each characteristic (the declaration and the value), and one for each descriptor
    size_t total_descriptors = 0;
    for (size_t i = 0; i < num_characteristics; ++i) {
        total_descriptors += num_descriptors[i];
        // we have to add the CCC manually
        if (characteristic_flags[i] & (MP_BLUETOOTH_CHARACTERISTIC_FLAG_NOTIFY | MP_BLUETOOTH_CHARACTERISTIC_FLAG_INDICATE)) {
            total_descriptors += 1;
        }
    }
    size_t total_attributes = 1 + (num_characteristics * 2) + total_descriptors;

    // allocate one extra so that we can know later where the final attribute is
    struct bt_gatt_attr *svc_attributes = m_new(struct bt_gatt_attr, total_attributes + 1);
    mp_obj_list_append(MP_STATE_PORT(bluetooth_zephyr_root_pointers)->objs_list, MP_OBJ_FROM_PTR(svc_attributes));

    size_t handle_index = 0;
    size_t descriptor_index = 0;
    size_t attr_index = 0;
    // bitfield of the handles we should ignore, should be more than enough for most applications
    uint64_t attrs_to_ignore = 0;
    uint64_t attrs_are_chrs = 0;
    uint64_t chr_has_ccc = 0;

    add_service(create_zephyr_uuid(service_uuid), &svc_attributes[attr_index]);
    attr_index += 1;

    for (size_t i = 0; i < num_characteristics; ++i) {

        struct add_characteristic add_char;
        add_char.uuid = create_zephyr_uuid(characteristic_uuids[i]);
        add_char.permissions = 0;
        add_char.properties = 0;
        if (characteristic_flags[i] & MP_BLUETOOTH_CHARACTERISTIC_FLAG_READ) {
            add_char.permissions |= BT_GATT_PERM_READ;
            add_char.properties |= BT_GATT_CHRC_READ;
        }
        if (characteristic_flags[i] & MP_BLUETOOTH_CHARACTERISTIC_FLAG_NOTIFY) {
            add_char.properties |= BT_GATT_CHRC_NOTIFY;
        }
        if (characteristic_flags[i] & MP_BLUETOOTH_CHARACTERISTIC_FLAG_INDICATE) {
            add_char.properties |= BT_GATT_CHRC_INDICATE;
        }
        if (characteristic_flags[i] & (MP_BLUETOOTH_CHARACTERISTIC_FLAG_WRITE | MP_BLUETOOTH_CHARACTERISTIC_FLAG_WRITE_NO_RESPONSE)) {
            add_char.permissions |= BT_GATT_PERM_WRITE;
            add_char.properties |= (BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP);
        }

        add_characteristic(&add_char, &svc_attributes[attr_index], &svc_attributes[attr_index + 1]);

        struct bt_gatt_attr *curr_char = &svc_attributes[attr_index];
        attrs_are_chrs |= (1 << attr_index);
        if (characteristic_flags[i] & (MP_BLUETOOTH_CHARACTERISTIC_FLAG_NOTIFY | MP_BLUETOOTH_CHARACTERISTIC_FLAG_INDICATE)) {
            chr_has_ccc |= (1 << attr_index);
        }
        attr_index += 1;
        attrs_to_ignore |= (1 << attr_index);   // ignore the value handle
        attr_index += 1;

        if (num_descriptors[i] > 0) {
            for (size_t j = 0; j < num_descriptors[i]; ++j) {

                struct add_descriptor add_desc;
                add_desc.uuid = create_zephyr_uuid(descriptor_uuids[descriptor_index]);
                add_desc.permissions = 0;
                if (descriptor_flags[descriptor_index] & MP_BLUETOOTH_CHARACTERISTIC_FLAG_READ) {
                    add_desc.permissions |= BT_GATT_PERM_READ;
                }
                if (descriptor_flags[descriptor_index] & (MP_BLUETOOTH_CHARACTERISTIC_FLAG_WRITE | MP_BLUETOOTH_CHARACTERISTIC_FLAG_WRITE_NO_RESPONSE)) {
                    add_desc.permissions |= BT_GATT_PERM_WRITE;
                }

                add_descriptor(curr_char, &add_desc, &svc_attributes[attr_index]);
                attr_index += 1;

                descriptor_index++;
            }
        }

        // to support indications and notifications we must add the CCC descriptor manually
        if (characteristic_flags[i] & (MP_BLUETOOTH_CHARACTERISTIC_FLAG_NOTIFY | MP_BLUETOOTH_CHARACTERISTIC_FLAG_INDICATE)) {
            struct add_descriptor add_desc;
            mp_obj_bluetooth_uuid_t ccc_uuid;
            ccc_uuid.base.type = &mp_type_bluetooth_uuid;
            ccc_uuid.data[0] = BT_UUID_GATT_CCC_VAL & 0xff;
            ccc_uuid.data[1] = (BT_UUID_GATT_CCC_VAL >> 8) & 0xff;
            ccc_uuid.type = MP_BLUETOOTH_UUID_TYPE_16;
            add_desc.uuid = create_zephyr_uuid(&ccc_uuid);
            add_desc.permissions = BT_GATT_PERM_READ | BT_GATT_PERM_WRITE;

            attrs_to_ignore |= (1 << attr_index);

            add_descriptor(curr_char, &add_desc, &svc_attributes[attr_index]);
            attr_index += 1;
        }
    }

    struct bt_gatt_service *service = m_new(struct bt_gatt_service, 1);
    mp_obj_list_append(MP_STATE_PORT(bluetooth_zephyr_root_pointers)->objs_list, MP_OBJ_FROM_PTR(service));
    service->attrs = svc_attributes;
    service->attr_count = attr_index;
    // invalidate the last attribute uuid pointer so that we new this is the end of attributes for this service
    svc_attributes[attr_index].uuid = NULL;

    // Note: advertising must be stopped for gatts registration to work

    int err = bt_gatt_service_register(service);
    if (err) {
        return bt_err_to_errno(err);
    }

    // now that the service has been registered, we can assign the handles for the characteristics and the descriptors
    // we are not interested in the handle of the service itself, so we start the loop from index 1
    for (int i = 1; i < total_attributes; i++) {
        // store all the relevant handles (characteristics and descriptors defined in Python)
        if (!((uint64_t)(attrs_to_ignore >> i) & (uint64_t)0x01)) {
            if (svc_attributes[i].user_data == NULL) {
                mp_bluetooth_gatts_db_create_entry(MP_STATE_PORT(bluetooth_zephyr_root_pointers)->gatts_db, svc_attributes[i].handle, MP_BLUETOOTH_DEFAULT_ATTR_LEN);
                mp_bluetooth_gatts_db_entry_t *entry = mp_bluetooth_gatts_db_lookup(MP_STATE_PORT(bluetooth_zephyr_root_pointers)->gatts_db, svc_attributes[i].handle);
                svc_attributes[i].user_data = entry->data;
            } else if (((uint64_t)(attrs_are_chrs >> i) & (uint64_t)0x01)) {
                if (svc_attributes[i + 1].user_data == NULL) {
                    mp_bluetooth_gatts_db_create_entry(MP_STATE_PORT(bluetooth_zephyr_root_pointers)->gatts_db, svc_attributes[i].handle, MP_BLUETOOTH_DEFAULT_ATTR_LEN);
                    mp_bluetooth_gatts_db_entry_t *entry = mp_bluetooth_gatts_db_lookup(MP_STATE_PORT(bluetooth_zephyr_root_pointers)->gatts_db, svc_attributes[i].handle);
                    svc_attributes[i + 1].user_data = entry->data;

                    if (((uint64_t)(chr_has_ccc >> i) & (uint64_t)0x01)) {
                        // create another database entry for the ccc of this characteristic
                        mp_bluetooth_gatts_db_create_entry(MP_STATE_PORT(bluetooth_zephyr_root_pointers)->gatts_db, svc_attributes[i].handle + 2, 1);
                    }
                }
            }
            handles[handle_index++] = svc_attributes[i].handle;
        }
    }

    MP_STATE_PORT(bluetooth_zephyr_root_pointers)->services[MP_STATE_PORT(bluetooth_zephyr_root_pointers)->n_services++] = service;

    return 0;

    #else
    return MP_EOPNOTSUPP;
    #endif
}

int mp_bluetooth_gap_disconnect(uint16_t conn_handle) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    mp_bt_zephyr_conn_t *connection = mp_bt_zephyr_find_connection(conn_handle);
    if (connection) {
        return bt_conn_disconnect(connection->conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
    }
    return MP_ENOENT;
}

int mp_bluetooth_gatts_read(uint16_t value_handle, const uint8_t **value, size_t *value_len) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    return mp_bluetooth_gatts_db_read(MP_STATE_PORT(bluetooth_zephyr_root_pointers)->gatts_db, value_handle, value, value_len);
}

int mp_bluetooth_gatts_write(uint16_t value_handle, const uint8_t *value, size_t value_len, bool send_update) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }

    int err = mp_bluetooth_gatts_db_write(MP_STATE_PORT(bluetooth_zephyr_root_pointers)->gatts_db, value_handle, value, value_len);

    if ((err == 0) && send_update) {
        struct bt_gatt_attr *attr_val = mp_bt_zephyr_find_attr_by_handle(value_handle + 1);
        mp_bluetooth_gatts_db_entry_t *ccc_entry = mp_bluetooth_gatts_db_lookup(MP_STATE_PORT(bluetooth_zephyr_root_pointers)->gatts_db, value_handle + 2);

        if (ccc_entry && (ccc_entry->data[0] == BT_GATT_CCC_NOTIFY)) {
            err = bt_gatt_notify(NULL, attr_val, value, value_len);
        } else if (ccc_entry && (ccc_entry->data[0] == BT_GATT_CCC_INDICATE)) {
            struct bt_gatt_indicate_params params = {
                .uuid = NULL,
                .attr = attr_val,
                .func = mp_bt_zephyr_gatt_indicate_done,
                .destroy = NULL,
                .data = value,
                .len = value_len
            };
            err = bt_gatt_indicate(NULL, &params);
        }
    }
    return err;
}

static void mp_bt_zephyr_gatt_indicate_done(struct bt_conn *conn, struct bt_gatt_indicate_params *params, uint8_t err) {
    struct bt_conn_info info;
    bt_conn_get_info(conn, &info);
    uint16_t chr_handle = params->attr->handle - 1;
    mp_bluetooth_gatts_on_indicate_complete(info.id, chr_handle, err);
}

static ssize_t mp_bt_zephyr_gatts_attr_read(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset) {
    // we receive the value handle, but to look up in the gatts db we need the characteristic handle, and that is is the value handle minus 1
    uint16_t _handle = attr->handle - 1;

    DEBUG_printf("BLE attr read for handle %d\n", attr->handle);

    mp_bluetooth_gatts_db_entry_t *entry = mp_bluetooth_gatts_db_lookup(MP_STATE_PORT(bluetooth_zephyr_root_pointers)->gatts_db, _handle);
    if (!entry) {
        // it could be a descriptor instead
        _handle = attr->handle;
        entry = mp_bluetooth_gatts_db_lookup(MP_STATE_PORT(bluetooth_zephyr_root_pointers)->gatts_db, _handle);
        if (!entry) {
            return BT_GATT_ERR(BT_ATT_ERR_INVALID_HANDLE);
        }
    }

    return bt_gatt_attr_read(conn, attr, buf, len, offset, entry->data, entry->data_len);
}

static ssize_t mp_bt_zephyr_gatts_attr_write(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags) {
    struct bt_conn_info info;
    bt_conn_get_info(conn, &info);

    DEBUG_printf("BLE attr write for handle %d\n", attr->handle);

    // the characteristic handle is the value handle minus 1
    uint16_t _handle = attr->handle - 1;

    mp_bluetooth_gatts_db_entry_t *entry = mp_bluetooth_gatts_db_lookup(MP_STATE_PORT(bluetooth_zephyr_root_pointers)->gatts_db, _handle);
    if (!entry) {
        // it could be a descriptor instead
        _handle = attr->handle;
        entry = mp_bluetooth_gatts_db_lookup(MP_STATE_PORT(bluetooth_zephyr_root_pointers)->gatts_db, _handle);
        if (!entry) {
            return BT_GATT_ERR(BT_ATT_ERR_INVALID_HANDLE);
        }
    }

    // Don't write anything if prepare flag is set
    if (flags & BT_GATT_WRITE_FLAG_PREPARE) {
        return 0;
    }

    if (offset > entry->data_alloc) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    if ((offset + len) > entry->data_alloc) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }

    if (entry->append) {
        offset = entry->data_len;
    }

    // copy the data into the buffer in the gatts database
    memcpy(&entry->data[offset], buf, len);
    entry->data_len = offset + len;

    mp_bluetooth_gatts_on_write(info.id, _handle);

    return len;
}

static struct bt_gatt_attr *mp_bt_zephyr_find_attr_by_handle(uint16_t value_handle) {
    for (int i = 0; i < MP_STATE_PORT(bluetooth_zephyr_root_pointers)->n_services; i++) {
        int j = 0;
        while (MP_STATE_PORT(bluetooth_zephyr_root_pointers)->services[i]->attrs[j].uuid != NULL) {
            if (MP_STATE_PORT(bluetooth_zephyr_root_pointers)->services[i]->attrs[j].handle == value_handle) {
                return &MP_STATE_PORT(bluetooth_zephyr_root_pointers)->services[i]->attrs[j];
            }
            j++;
        }
    }
    return NULL;
}

int mp_bluetooth_gatts_notify_indicate(uint16_t conn_handle, uint16_t value_handle, int gatts_op, const uint8_t *value, size_t value_len) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }

    int err = MP_ENOENT;
    mp_bt_zephyr_conn_t *connection = mp_bt_zephyr_find_connection(conn_handle);

    if (connection) {
        struct bt_gatt_attr *attr_val = mp_bt_zephyr_find_attr_by_handle(value_handle + 1);

        if (attr_val) {
            switch (gatts_op) {
                case MP_BLUETOOTH_GATTS_OP_NOTIFY: {
                    err = bt_gatt_notify(connection->conn, attr_val, value, value_len);
                    break;
                }
                case MP_BLUETOOTH_GATTS_OP_INDICATE: {
                    struct bt_gatt_indicate_params params = {
                        .uuid = NULL,
                        .attr = attr_val,
                        .func = mp_bt_zephyr_gatt_indicate_done,
                        .destroy = NULL,
                        .data = value,
                        .len = value_len
                    };
                    err = bt_gatt_indicate(connection->conn, &params);
                    break;
                }
            }
        }
    }

    return err;
}

int mp_bluetooth_gatts_set_buffer(uint16_t value_handle, size_t len, bool append) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    return mp_bluetooth_gatts_db_resize(MP_STATE_PORT(bluetooth_zephyr_root_pointers)->gatts_db, value_handle, len, append);
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

int mp_bluetooth_gap_peripheral_connect_cancel(void) {
    DEBUG_printf("mp_bluetooth_gap_peripheral_connect_cancel\n");
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    return MP_EOPNOTSUPP;
}

#endif // MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE

// Note: modbluetooth UUIDs store their data in LE.
static struct bt_uuid *create_zephyr_uuid(const mp_obj_bluetooth_uuid_t *uuid) {
    struct bt_uuid *result = (struct bt_uuid *)m_new(union uuid_u, 1);
    mp_obj_list_append(MP_STATE_PORT(bluetooth_zephyr_root_pointers)->objs_list, MP_OBJ_FROM_PTR(result));
    if (uuid->type == MP_BLUETOOTH_UUID_TYPE_16) {
        bt_uuid_create(result, uuid->data, 2);
    } else if (uuid->type == MP_BLUETOOTH_UUID_TYPE_32) {
        bt_uuid_create(result, uuid->data, 4);
    } else {    //  MP_BLUETOOTH_UUID_TYPE_128
        bt_uuid_create(result, uuid->data, 16);
    }
    return result;
}

static void gatt_db_add(const struct bt_gatt_attr *pattern, struct bt_gatt_attr *attr, size_t user_data_len) {
    const union uuid_u *u = CONTAINER_OF(pattern->uuid, union uuid_u, uuid);
    size_t uuid_size = sizeof(u->u16);

    if (u->uuid.type == BT_UUID_TYPE_32) {
        uuid_size = sizeof(u->u32);
    } else if (u->uuid.type == BT_UUID_TYPE_128) {
        uuid_size = sizeof(u->u128);
    }

    memcpy(attr, pattern, sizeof(*attr));

    // Store the UUID.
    attr->uuid = (const struct bt_uuid *)m_new(union uuid_u, 1);
    mp_obj_list_append(MP_STATE_PORT(bluetooth_zephyr_root_pointers)->objs_list, MP_OBJ_FROM_PTR(attr->uuid));
    memcpy((void *)attr->uuid, &u->uuid, uuid_size);

    // Copy user_data to the buffer.
    if (user_data_len) {
        attr->user_data = m_new(uint8_t, user_data_len);
        mp_obj_list_append(MP_STATE_PORT(bluetooth_zephyr_root_pointers)->objs_list, MP_OBJ_FROM_PTR(attr->user_data));
        memcpy(attr->user_data, pattern->user_data, user_data_len);
    }
}

static void add_service(const struct bt_uuid *u, struct bt_gatt_attr *attr) {
    union uuid_u *uuid = (union uuid_u *)u;

    size_t uuid_size = sizeof(uuid->u16);

    if (uuid->uuid.type == BT_UUID_TYPE_32) {
        uuid_size = sizeof(uuid->u32);
    } else if (uuid->uuid.type == BT_UUID_TYPE_128) {
        uuid_size = sizeof(uuid->u128);
    }

    gatt_db_add(&(struct bt_gatt_attr)BT_GATT_PRIMARY_SERVICE(&uuid->uuid), attr, uuid_size);
}

static void add_characteristic(struct add_characteristic *ch, struct bt_gatt_attr *attr_chrc, struct bt_gatt_attr *attr_value) {
    struct bt_gatt_chrc *chrc_data;

    // Add Characteristic Declaration
    gatt_db_add(&(struct bt_gatt_attr)
        BT_GATT_ATTRIBUTE(BT_UUID_GATT_CHRC,
            BT_GATT_PERM_READ,
            bt_gatt_attr_read_chrc, NULL,
            (&(struct bt_gatt_chrc) {})), attr_chrc, sizeof(*chrc_data));

    // Allow prepare writes
    ch->permissions |= BT_GATT_PERM_PREPARE_WRITE;

    // Add Characteristic Value
    gatt_db_add(&(struct bt_gatt_attr)
        BT_GATT_ATTRIBUTE(ch->uuid,
            ch->permissions & GATT_PERM_MASK,
            mp_bt_zephyr_gatts_attr_read, mp_bt_zephyr_gatts_attr_write, NULL), attr_value, 0);

    chrc_data = attr_chrc->user_data;
    chrc_data->properties = ch->properties;
    chrc_data->uuid = attr_value->uuid;
}

static void ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value) {
    mp_bluetooth_gatts_db_entry_t *entry = mp_bluetooth_gatts_db_lookup(MP_STATE_PORT(bluetooth_zephyr_root_pointers)->gatts_db, attr->handle);
    entry->data[0] = value;
}

static struct bt_gatt_attr ccc_definition = BT_GATT_CCC(ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE);

static void add_ccc(struct bt_gatt_attr *attr, struct bt_gatt_attr *attr_desc) {
    struct bt_gatt_chrc *chrc = attr->user_data;

    // Check characteristic properties
    if (!(chrc->properties & (BT_GATT_CHRC_NOTIFY | BT_GATT_CHRC_INDICATE))) {
        mp_raise_OSError(MP_EINVAL);
    }

    // Add CCC descriptor to GATT database
    gatt_db_add(&ccc_definition, attr_desc, 0);
}

static void add_cep(const struct bt_gatt_attr *attr_chrc, struct bt_gatt_attr *attr_desc) {
    struct bt_gatt_chrc *chrc = attr_chrc->user_data;
    struct bt_gatt_cep cep_value;

    // Extended Properties bit shall be set
    if (!(chrc->properties & BT_GATT_CHRC_EXT_PROP)) {
        mp_raise_OSError(MP_EINVAL);
    }

    cep_value.properties = 0x0000;

    // Add CEP descriptor to GATT database
    gatt_db_add(&(struct bt_gatt_attr)BT_GATT_CEP(&cep_value), attr_desc, sizeof(cep_value));
}

static void add_descriptor(struct bt_gatt_attr *chrc, struct add_descriptor *d, struct bt_gatt_attr *attr_desc) {
    if (!bt_uuid_cmp(d->uuid, BT_UUID_GATT_CEP)) {
        add_cep(chrc, attr_desc);
    } else if (!bt_uuid_cmp(d->uuid, BT_UUID_GATT_CCC)) {
        add_ccc(chrc, attr_desc);
    } else {
        // Allow prepare writes
        d->permissions |= BT_GATT_PERM_PREPARE_WRITE;

        gatt_db_add(&(struct bt_gatt_attr)
            BT_GATT_DESCRIPTOR(d->uuid,
                d->permissions & GATT_PERM_MASK,
                mp_bt_zephyr_gatts_attr_read, mp_bt_zephyr_gatts_attr_write, NULL), attr_desc, 0);
    }
}

MP_REGISTER_ROOT_POINTER(struct _mp_bluetooth_zephyr_root_pointers_t *bluetooth_zephyr_root_pointers);

#endif // MICROPY_PY_BLUETOOTH
