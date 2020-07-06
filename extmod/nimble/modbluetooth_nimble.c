/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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

#if MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_NIMBLE

#include "extmod/nimble/modbluetooth_nimble.h"
#include "extmod/modbluetooth.h"

#include "host/ble_hs.h"
#include "host/util/util.h"
#include "nimble/ble.h"
#include "nimble/nimble_port.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#ifndef MICROPY_PY_BLUETOOTH_DEFAULT_GAP_NAME
#define MICROPY_PY_BLUETOOTH_DEFAULT_GAP_NAME "MPY NIMBLE"
#endif

#define DEBUG_EVENT_printf(...) // printf(__VA_ARGS__)

#define ERRNO_BLUETOOTH_NOT_ACTIVE MP_ENODEV

// Any BLE_HS_xxx code not in this table will default to MP_EIO.
STATIC int8_t ble_hs_err_to_errno_table[] = {
    [BLE_HS_EAGAIN] = MP_EAGAIN,
    [BLE_HS_EALREADY] = MP_EALREADY,
    [BLE_HS_EINVAL] = MP_EINVAL,
    [BLE_HS_ENOENT] = MP_ENOENT,
    [BLE_HS_ENOMEM] = MP_ENOMEM,
    [BLE_HS_ENOTCONN] = MP_ENOTCONN,
    [BLE_HS_ENOTSUP] = MP_EOPNOTSUPP,
    [BLE_HS_ETIMEOUT] = MP_ETIMEDOUT,
    [BLE_HS_EDONE] = MP_EIO,               // TODO: Maybe should be MP_EISCONN (connect uses this for "already connected").
    [BLE_HS_EBUSY] = MP_EBUSY,
};

STATIC int ble_hs_err_to_errno(int err) {
    if (!err) {
        return 0;
    }
    if (0 <= err && err < MP_ARRAY_SIZE(ble_hs_err_to_errno_table) && ble_hs_err_to_errno_table[err]) {
        return ble_hs_err_to_errno_table[err];
    } else {
        return MP_EIO;
    }
}

// Note: modbluetooth UUIDs store their data in LE.
STATIC ble_uuid_t *create_nimble_uuid(const mp_obj_bluetooth_uuid_t *uuid, ble_uuid_any_t *storage) {
    if (uuid->type == MP_BLUETOOTH_UUID_TYPE_16) {
        ble_uuid16_t *result = storage ? &storage->u16 : m_new(ble_uuid16_t, 1);
        result->u.type = BLE_UUID_TYPE_16;
        result->value = (uuid->data[1] << 8) | uuid->data[0];
        return (ble_uuid_t *)result;
    } else if (uuid->type == MP_BLUETOOTH_UUID_TYPE_32) {
        ble_uuid32_t *result = storage ? &storage->u32 : m_new(ble_uuid32_t, 1);
        result->u.type = BLE_UUID_TYPE_32;
        result->value = (uuid->data[1] << 24) | (uuid->data[1] << 16) | (uuid->data[1] << 8) | uuid->data[0];
        return (ble_uuid_t *)result;
    } else if (uuid->type == MP_BLUETOOTH_UUID_TYPE_128) {
        ble_uuid128_t *result = storage ? &storage->u128 : m_new(ble_uuid128_t, 1);
        result->u.type = BLE_UUID_TYPE_128;
        memcpy(result->value, uuid->data, 16);
        return (ble_uuid_t *)result;
    } else {
        return NULL;
    }
}

#if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE

STATIC mp_obj_bluetooth_uuid_t create_mp_uuid(const ble_uuid_any_t *uuid) {
    mp_obj_bluetooth_uuid_t result;
    switch (uuid->u.type) {
        case BLE_UUID_TYPE_16:
            result.type = MP_BLUETOOTH_UUID_TYPE_16;
            result.data[0] = uuid->u16.value & 0xff;
            result.data[1] = (uuid->u16.value >> 8) & 0xff;
            break;
        case BLE_UUID_TYPE_32:
            result.type = MP_BLUETOOTH_UUID_TYPE_32;
            result.data[0] = uuid->u32.value & 0xff;
            result.data[1] = (uuid->u32.value >> 8) & 0xff;
            result.data[2] = (uuid->u32.value >> 16) & 0xff;
            result.data[3] = (uuid->u32.value >> 24) & 0xff;
            break;
        case BLE_UUID_TYPE_128:
            result.type = MP_BLUETOOTH_UUID_TYPE_128;
            memcpy(result.data, uuid->u128.value, 16);
            break;
        default:
            assert(false);
    }
    return result;
}

// modbluetooth (and the layers above it) work in BE for addresses, Nimble works in LE.
STATIC void reverse_addr_byte_order(uint8_t *addr_out, const uint8_t *addr_in) {
    for (int i = 0; i < 6; ++i) {
        addr_out[i] = addr_in[5 - i];
    }
}

STATIC ble_addr_t create_nimble_addr(uint8_t addr_type, const uint8_t *addr) {
    ble_addr_t addr_nimble;
    addr_nimble.type = addr_type;
    // Incoming addr is from modbluetooth (BE), so copy and convert to LE for Nimble.
    reverse_addr_byte_order(addr_nimble.val, addr);
    return addr_nimble;
}

#endif // MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE

volatile int mp_bluetooth_nimble_ble_state = MP_BLUETOOTH_NIMBLE_BLE_STATE_OFF;

STATIC void reset_cb(int reason) {
    (void)reason;
}

STATIC void sync_cb(void) {
    int rc;
    ble_addr_t addr;

    rc = ble_hs_util_ensure_addr(0); // prefer public address
    if (rc != 0) {
        // https://mynewt.apache.org/latest/tutorials/ble/eddystone.html#configure-the-nimble-stack-with-an-address
        #if MICROPY_PY_BLUETOOTH_RANDOM_ADDR
        rc = ble_hs_id_gen_rnd(1, &addr);
        assert(rc == 0);
        rc = ble_hs_id_set_rnd(addr.val);
        assert(rc == 0);
        #else
        uint8_t addr_be[6];
        mp_hal_get_mac(MP_HAL_MAC_BDADDR, addr_be);
        reverse_addr_byte_order(addr.val, addr_be);
        // ble_hs_id_set_pub(addr.val);
        rc = ble_hs_id_set_rnd(addr.val);
        assert(rc == 0);
        #endif

        rc = ble_hs_util_ensure_addr(0); // prefer public address
        assert(rc == 0);
    }

    if (MP_BLUETOOTH_DEFAULT_ATTR_LEN > 20) {
        rc = ble_att_set_preferred_mtu(MP_BLUETOOTH_DEFAULT_ATTR_LEN + 3);
        assert(rc == 0);
    }

    ble_svc_gap_device_name_set(MICROPY_PY_BLUETOOTH_DEFAULT_GAP_NAME);

    mp_bluetooth_nimble_ble_state = MP_BLUETOOTH_NIMBLE_BLE_STATE_ACTIVE;
}

STATIC void gatts_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg) {
    if (!mp_bluetooth_is_active()) {
        return;
    }
    switch (ctxt->op) {
        case BLE_GATT_REGISTER_OP_SVC:
            // Called when a service is successfully registered.
            DEBUG_EVENT_printf("gatts_register_cb: svc uuid=%p handle=%d\n", &ctxt->svc.svc_def->uuid, ctxt->svc.handle);
            break;

        case BLE_GATT_REGISTER_OP_CHR:
            // Called when a characteristic is successfully registered.
            DEBUG_EVENT_printf("gatts_register_cb: chr uuid=%p def_handle=%d val_handle=%d\n", &ctxt->chr.chr_def->uuid, ctxt->chr.def_handle, ctxt->chr.val_handle);

            // Note: We will get this event for the default GAP Service, meaning that we allocate storage for the
            // "device name" and "appearance" characteristics, even though we never see the reads for them.
            // TODO: Possibly check if the service UUID is 0x1801 and ignore?

            // Allocate the gatts_db storage for this characteristic.
            // Although this function is a callback, it's called synchronously from ble_hs_sched_start/ble_gatts_start, so safe to allocate.
            mp_bluetooth_gatts_db_create_entry(MP_STATE_PORT(bluetooth_nimble_root_pointers)->gatts_db, ctxt->chr.val_handle, MP_BLUETOOTH_DEFAULT_ATTR_LEN);
            break;

        case BLE_GATT_REGISTER_OP_DSC:
            // Called when a descriptor is successfully registered.
            // Note: This is event is not called for the CCCD.
            DEBUG_EVENT_printf("gatts_register_cb: dsc uuid=%p handle=%d\n", &ctxt->dsc.dsc_def->uuid, ctxt->dsc.handle);

            // See above, safe to alloc.
            mp_bluetooth_gatts_db_create_entry(MP_STATE_PORT(bluetooth_nimble_root_pointers)->gatts_db, ctxt->dsc.handle, MP_BLUETOOTH_DEFAULT_ATTR_LEN);

            // Unlike characteristics, we have to manually provide a way to get the handle back to the register method.
            *((uint16_t *)ctxt->dsc.dsc_def->arg) = ctxt->dsc.handle;
            break;

        default:
            DEBUG_EVENT_printf("gatts_register_cb: unknown op %d\n", ctxt->op);
            break;
    }
}

STATIC int gap_event_cb(struct ble_gap_event *event, void *arg) {
    DEBUG_EVENT_printf("gap_event_cb: type=%d\n", event->type);
    if (!mp_bluetooth_is_active()) {
        return 0;
    }
    struct ble_gap_conn_desc desc;
    uint8_t addr[6] = {0};

    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            if (event->connect.status == 0) {
                // Connection established.
                ble_gap_conn_find(event->connect.conn_handle, &desc);
                reverse_addr_byte_order(addr, desc.peer_id_addr.val);
                mp_bluetooth_gap_on_connected_disconnected(MP_BLUETOOTH_IRQ_CENTRAL_CONNECT, event->connect.conn_handle, desc.peer_id_addr.type, addr);
            } else {
                // Connection failed.
                mp_bluetooth_gap_on_connected_disconnected(MP_BLUETOOTH_IRQ_CENTRAL_DISCONNECT, event->connect.conn_handle, 0xff, addr);
            }
            break;

        case BLE_GAP_EVENT_DISCONNECT:
            // Disconnect.
            reverse_addr_byte_order(addr, event->disconnect.conn.peer_id_addr.val);
            mp_bluetooth_gap_on_connected_disconnected(MP_BLUETOOTH_IRQ_CENTRAL_DISCONNECT, event->disconnect.conn.conn_handle, event->disconnect.conn.peer_id_addr.type, addr);
            break;
    }

    return 0;
}

int mp_bluetooth_init(void) {
    DEBUG_EVENT_printf("mp_bluetooth_init\n");
    // Clean up if necessary.
    mp_bluetooth_deinit();

    ble_hs_cfg.reset_cb = reset_cb;
    ble_hs_cfg.sync_cb = sync_cb;
    ble_hs_cfg.gatts_register_cb = gatts_register_cb;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    MP_STATE_PORT(bluetooth_nimble_root_pointers) = m_new0(mp_bluetooth_nimble_root_pointers_t, 1);
    mp_bluetooth_gatts_db_create(&MP_STATE_PORT(bluetooth_nimble_root_pointers)->gatts_db);

    mp_bluetooth_nimble_ble_state = MP_BLUETOOTH_NIMBLE_BLE_STATE_STARTING;

    mp_bluetooth_nimble_port_preinit();
    nimble_port_init();
    mp_bluetooth_nimble_port_postinit();

    // By default, just register the default gap/gatt service.
    ble_svc_gap_init();
    ble_svc_gatt_init();

    mp_bluetooth_nimble_port_start();

    // Wait for sync callback
    while (mp_bluetooth_nimble_ble_state != MP_BLUETOOTH_NIMBLE_BLE_STATE_ACTIVE) {
        MICROPY_EVENT_POLL_HOOK
    }
    DEBUG_EVENT_printf("mp_bluetooth_init: ready\n");

    return 0;
}

// Called when the host stop procedure has completed.
STATIC void ble_hs_shutdown_stop_cb(int status, void *arg) {
    mp_bluetooth_nimble_ble_state = MP_BLUETOOTH_NIMBLE_BLE_STATE_OFF;
}

STATIC struct ble_hs_stop_listener ble_hs_shutdown_stop_listener;

void mp_bluetooth_deinit(void) {
    DEBUG_EVENT_printf("mp_bluetooth_deinit\n");
    if (mp_bluetooth_nimble_ble_state == MP_BLUETOOTH_NIMBLE_BLE_STATE_OFF) {
        return;
    }

    mp_bluetooth_gap_advertise_stop();
    #if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
    mp_bluetooth_gap_scan_stop();
    #endif

    mp_bluetooth_nimble_ble_state = MP_BLUETOOTH_NIMBLE_BLE_STATE_STOPPING;

    ble_hs_stop(&ble_hs_shutdown_stop_listener, ble_hs_shutdown_stop_cb, NULL);

    while (mp_bluetooth_nimble_ble_state != MP_BLUETOOTH_NIMBLE_BLE_STATE_OFF) {
        MICROPY_EVENT_POLL_HOOK
    }

    mp_bluetooth_nimble_port_deinit();

    MP_STATE_PORT(bluetooth_nimble_root_pointers) = NULL;
    DEBUG_EVENT_printf("mp_bluetooth_deinit: shut down\n");
}

bool mp_bluetooth_is_active(void) {
    return mp_bluetooth_nimble_ble_state == MP_BLUETOOTH_NIMBLE_BLE_STATE_ACTIVE;
}

void mp_bluetooth_get_device_addr(uint8_t *addr) {
    #if MICROPY_PY_BLUETOOTH_RANDOM_ADDR
    uint8_t addr_le[6];
    int rc = ble_hs_id_copy_addr(BLE_ADDR_RANDOM, addr_le, NULL);
    if (rc != 0) {
        // Even with MICROPY_PY_BLUETOOTH_RANDOM_ADDR enabled the public address may
        // be used instead, in which case there is no random address.
        ble_hs_id_copy_addr(BLE_ADDR_PUBLIC, addr_le, NULL);
    }
    reverse_addr_byte_order(addr, addr_le);
    #else
    mp_hal_get_mac(MP_HAL_MAC_BDADDR, addr);
    #endif
}

size_t mp_bluetooth_gap_get_device_name(const uint8_t **buf) {
    const char *name = ble_svc_gap_device_name();
    *buf = (const uint8_t *)name;
    return strlen(name);
}

int mp_bluetooth_gap_set_device_name(const uint8_t *buf, size_t len) {
    char tmp_buf[MYNEWT_VAL(BLE_SVC_GAP_DEVICE_NAME_MAX_LENGTH) + 1];
    if (len + 1 > sizeof(tmp_buf)) {
        return MP_EINVAL;
    }
    memcpy(tmp_buf, buf, len);
    tmp_buf[len] = '\0';
    return ble_hs_err_to_errno(ble_svc_gap_device_name_set(tmp_buf));
}

int mp_bluetooth_gap_advertise_start(bool connectable, int32_t interval_us, const uint8_t *adv_data, size_t adv_data_len, const uint8_t *sr_data, size_t sr_data_len) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }

    mp_bluetooth_gap_advertise_stop();

    int ret;
    if (adv_data) {
        ret = ble_gap_adv_set_data(adv_data, adv_data_len);
        if (ret != 0) {
            return ble_hs_err_to_errno(ret);
        }
    }

    if (sr_data) {
        ret = ble_gap_adv_rsp_set_data(sr_data, sr_data_len);
        if (ret != 0) {
            return ble_hs_err_to_errno(ret);
        }
    }

    struct ble_gap_adv_params adv_params = {
        .conn_mode = connectable ? BLE_GAP_CONN_MODE_UND : BLE_GAP_CONN_MODE_NON,
        .disc_mode = BLE_GAP_DISC_MODE_GEN,
        .itvl_min = interval_us / BLE_HCI_ADV_ITVL, // convert to 625us units.
        .itvl_max = interval_us / BLE_HCI_ADV_ITVL,
        .channel_map = 7, // all 3 channels.
    };

    ret = ble_gap_adv_start(BLE_OWN_ADDR_PUBLIC, NULL, BLE_HS_FOREVER, &adv_params, gap_event_cb, NULL);
    if (ret == 0) {
        return 0;
    }
    ret = ble_gap_adv_start(BLE_OWN_ADDR_RPA_PUBLIC_DEFAULT, NULL, BLE_HS_FOREVER, &adv_params, gap_event_cb, NULL);
    if (ret == 0) {
        return 0;
    }
    ret = ble_gap_adv_start(BLE_OWN_ADDR_RPA_RANDOM_DEFAULT, NULL, BLE_HS_FOREVER, &adv_params, gap_event_cb, NULL);
    if (ret == 0) {
        return 0;
    }
    ret = ble_gap_adv_start(BLE_OWN_ADDR_RANDOM, NULL, BLE_HS_FOREVER, &adv_params, gap_event_cb, NULL);
    if (ret == 0) {
        return 0;
    }
    DEBUG_EVENT_printf("ble_gap_adv_start: %d\n", ret);

    return ble_hs_err_to_errno(ret);
}

void mp_bluetooth_gap_advertise_stop(void) {
    if (ble_gap_adv_active()) {
        ble_gap_adv_stop();
    }
}

static int characteristic_access_cb(uint16_t conn_handle, uint16_t value_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    DEBUG_EVENT_printf("characteristic_access_cb: conn_handle=%u value_handle=%u op=%u\n", conn_handle, value_handle, ctxt->op);
    if (!mp_bluetooth_is_active()) {
        return 0;
    }
    mp_bluetooth_gatts_db_entry_t *entry;
    switch (ctxt->op) {
        case BLE_GATT_ACCESS_OP_READ_CHR:
        case BLE_GATT_ACCESS_OP_READ_DSC:
            #if MICROPY_PY_BLUETOOTH_GATTS_ON_READ_CALLBACK
            // Allow Python code to override (by using gatts_write), or deny (by returning false) the read.
            if (!mp_bluetooth_gatts_on_read_request(conn_handle, value_handle)) {
                return BLE_ATT_ERR_READ_NOT_PERMITTED;
            }
            #endif

            entry = mp_bluetooth_gatts_db_lookup(MP_STATE_PORT(bluetooth_nimble_root_pointers)->gatts_db, value_handle);
            if (!entry) {
                return BLE_ATT_ERR_ATTR_NOT_FOUND;
            }

            os_mbuf_append(ctxt->om, entry->data, entry->data_len);

            return 0;
        case BLE_GATT_ACCESS_OP_WRITE_CHR:
        case BLE_GATT_ACCESS_OP_WRITE_DSC:
            entry = mp_bluetooth_gatts_db_lookup(MP_STATE_PORT(bluetooth_nimble_root_pointers)->gatts_db, value_handle);
            if (!entry) {
                return BLE_ATT_ERR_ATTR_NOT_FOUND;
            }

            size_t offset = 0;
            if (entry->append) {
                offset = entry->data_len;
            }
            entry->data_len = MIN(entry->data_alloc, OS_MBUF_PKTLEN(ctxt->om) + offset);
            os_mbuf_copydata(ctxt->om, 0, entry->data_len - offset, entry->data + offset);

            mp_bluetooth_gatts_on_write(conn_handle, value_handle);

            return 0;
    }
    return BLE_ATT_ERR_UNLIKELY;
}

int mp_bluetooth_gatts_register_service_begin(bool append) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    int ret = ble_gatts_reset();
    if (ret != 0) {
        return ble_hs_err_to_errno(ret);
    }

    // Reset the gatt characteristic value db.
    mp_bluetooth_gatts_db_reset(MP_STATE_PORT(bluetooth_nimble_root_pointers)->gatts_db);

    // By default, just register the default gap/gatt service.
    ble_svc_gap_init();
    ble_svc_gatt_init();

    if (!append) {
        // Unref any previous service definitions.
        for (int i = 0; i < MP_STATE_PORT(bluetooth_nimble_root_pointers)->n_services; ++i) {
            MP_STATE_PORT(bluetooth_nimble_root_pointers)->services[i] = NULL;
        }
        MP_STATE_PORT(bluetooth_nimble_root_pointers)->n_services = 0;
    }

    return 0;
}

int mp_bluetooth_gatts_register_service_end() {
    int ret = ble_gatts_start();
    if (ret != 0) {
        return ble_hs_err_to_errno(ret);
    }

    return 0;
}

int mp_bluetooth_gatts_register_service(mp_obj_bluetooth_uuid_t *service_uuid, mp_obj_bluetooth_uuid_t **characteristic_uuids, uint8_t *characteristic_flags, mp_obj_bluetooth_uuid_t **descriptor_uuids, uint8_t *descriptor_flags, uint8_t *num_descriptors, uint16_t *handles, size_t num_characteristics) {
    if (MP_STATE_PORT(bluetooth_nimble_root_pointers)->n_services == MP_BLUETOOTH_NIMBLE_MAX_SERVICES) {
        return MP_E2BIG;
    }
    size_t handle_index = 0;
    size_t descriptor_index = 0;

    struct ble_gatt_chr_def *characteristics = m_new(struct ble_gatt_chr_def, num_characteristics + 1);
    for (size_t i = 0; i < num_characteristics; ++i) {
        characteristics[i].uuid = create_nimble_uuid(characteristic_uuids[i], NULL);
        characteristics[i].access_cb = characteristic_access_cb;
        characteristics[i].arg = NULL;
        characteristics[i].flags = characteristic_flags[i];
        characteristics[i].min_key_size = 0;
        characteristics[i].val_handle = &handles[handle_index];
        ++handle_index;

        if (num_descriptors[i] == 0) {
            characteristics[i].descriptors = NULL;
        } else {
            struct ble_gatt_dsc_def *descriptors = m_new(struct ble_gatt_dsc_def, num_descriptors[i] + 1);

            for (size_t j = 0; j < num_descriptors[i]; ++j) {
                descriptors[j].uuid = create_nimble_uuid(descriptor_uuids[descriptor_index], NULL);
                descriptors[j].access_cb = characteristic_access_cb;
                descriptors[j].att_flags = descriptor_flags[descriptor_index];
                descriptors[j].min_key_size = 0;
                // Unlike characteristic, Nimble doesn't provide an automatic way to remember the handle, so use the arg.
                descriptors[j].arg = &handles[handle_index];
                ++descriptor_index;
                ++handle_index;
            }
            descriptors[num_descriptors[i]].uuid = NULL; // no more descriptors

            characteristics[i].descriptors = descriptors;
        }
    }
    characteristics[num_characteristics].uuid = NULL; // no more characteristics

    struct ble_gatt_svc_def *service = m_new(struct ble_gatt_svc_def, 2);
    service[0].type = BLE_GATT_SVC_TYPE_PRIMARY;
    service[0].uuid = create_nimble_uuid(service_uuid, NULL);
    service[0].includes = NULL;
    service[0].characteristics = characteristics;
    service[1].type = 0; // no more services

    MP_STATE_PORT(bluetooth_nimble_root_pointers)->services[MP_STATE_PORT(bluetooth_nimble_root_pointers)->n_services++] = service;

    // Note: advertising must be stopped for gatts registration to work

    int ret = ble_gatts_count_cfg(service);
    if (ret != 0) {
        return ble_hs_err_to_errno(ret);
    }

    ret = ble_gatts_add_svcs(service);
    if (ret != 0) {
        return ble_hs_err_to_errno(ret);
    }

    return 0;
}

int mp_bluetooth_gap_disconnect(uint16_t conn_handle) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    return ble_hs_err_to_errno(ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM));
}

int mp_bluetooth_gatts_read(uint16_t value_handle, uint8_t **value, size_t *value_len) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    return mp_bluetooth_gatts_db_read(MP_STATE_PORT(bluetooth_nimble_root_pointers)->gatts_db, value_handle, value, value_len);
}

int mp_bluetooth_gatts_write(uint16_t value_handle, const uint8_t *value, size_t value_len) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    return mp_bluetooth_gatts_db_write(MP_STATE_PORT(bluetooth_nimble_root_pointers)->gatts_db, value_handle, value, value_len);
}

// TODO: Could use ble_gatts_chr_updated to send to all subscribed centrals.

int mp_bluetooth_gatts_notify(uint16_t conn_handle, uint16_t value_handle) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    // Confusingly, notify/notify_custom/indicate are "gattc" function (even though they're used by peripherals (i.e. gatt servers)).
    // See https://www.mail-archive.com/dev@mynewt.apache.org/msg01293.html
    return ble_hs_err_to_errno(ble_gattc_notify(conn_handle, value_handle));
}

int mp_bluetooth_gatts_notify_send(uint16_t conn_handle, uint16_t value_handle, const uint8_t *value, size_t *value_len) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    struct os_mbuf *om = ble_hs_mbuf_from_flat(value, *value_len);
    if (om == NULL) {
        return -1;
    }
    // TODO: check that notify_custom takes ownership of om, if not os_mbuf_free_chain(om).
    return ble_hs_err_to_errno(ble_gattc_notify_custom(conn_handle, value_handle, om));
}

int mp_bluetooth_gatts_indicate(uint16_t conn_handle, uint16_t value_handle) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    return ble_hs_err_to_errno(ble_gattc_indicate(conn_handle, value_handle));
}

int mp_bluetooth_gatts_set_buffer(uint16_t value_handle, size_t len, bool append) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    return mp_bluetooth_gatts_db_resize(MP_STATE_PORT(bluetooth_nimble_root_pointers)->gatts_db, value_handle, len, append);
}

#if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE

STATIC void gattc_on_data_available(uint8_t event, uint16_t conn_handle, uint16_t value_handle, const struct os_mbuf *om) {
    size_t len = OS_MBUF_PKTLEN(om);
    mp_uint_t atomic_state;
    len = mp_bluetooth_gattc_on_data_available_start(event, conn_handle, value_handle, len, &atomic_state);
    while (len > 0 && om != NULL) {
        size_t n = MIN(om->om_len, len);
        mp_bluetooth_gattc_on_data_available_chunk(OS_MBUF_DATA(om, const uint8_t *), n);
        len -= n;
        om = SLIST_NEXT(om, om_next);
    }
    mp_bluetooth_gattc_on_data_available_end(atomic_state);
}

STATIC int gap_scan_cb(struct ble_gap_event *event, void *arg) {
    DEBUG_EVENT_printf("gap_scan_cb: event=%d type=%d\n", event->type, event->type == BLE_GAP_EVENT_DISC ? event->disc.event_type : -1);
    if (!mp_bluetooth_is_active()) {
        return 0;
    }

    if (event->type == BLE_GAP_EVENT_DISC_COMPLETE) {
        mp_bluetooth_gap_on_scan_complete();
        return 0;
    }

    if (event->type != BLE_GAP_EVENT_DISC) {
        return 0;
    }

    uint8_t addr[6];
    reverse_addr_byte_order(addr, event->disc.addr.val);
    mp_bluetooth_gap_on_scan_result(event->disc.addr.type, addr, event->disc.event_type, event->disc.rssi, event->disc.data, event->disc.length_data);

    return 0;
}

int mp_bluetooth_gap_scan_start(int32_t duration_ms, int32_t interval_us, int32_t window_us) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    if (duration_ms == 0) {
        duration_ms = BLE_HS_FOREVER;
    }
    struct ble_gap_disc_params discover_params = {
        .itvl = MAX(BLE_HCI_SCAN_ITVL_MIN, MIN(BLE_HCI_SCAN_ITVL_MAX, interval_us / BLE_HCI_SCAN_ITVL)),
        .window = MAX(BLE_HCI_SCAN_WINDOW_MIN, MIN(BLE_HCI_SCAN_WINDOW_MAX, window_us / BLE_HCI_SCAN_ITVL)),
        .filter_policy = BLE_HCI_CONN_FILT_NO_WL,
        .limited = 0,
        .passive = 1,  // TODO: Handle BLE_HCI_ADV_RPT_EVTYPE_SCAN_RSP in gap_scan_cb above.
        .filter_duplicates = 0,
    };
    int err = ble_gap_disc(BLE_OWN_ADDR_PUBLIC, duration_ms, &discover_params, gap_scan_cb, NULL);
    return ble_hs_err_to_errno(err);
}

int mp_bluetooth_gap_scan_stop(void) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    if (!ble_gap_disc_active()) {
        return 0;
    }
    int err = ble_gap_disc_cancel();
    if (err == 0) {
        mp_bluetooth_gap_on_scan_complete();
        return 0;
    }
    return ble_hs_err_to_errno(err);
}

// Central role: GAP events for a connected peripheral.
STATIC int peripheral_gap_event_cb(struct ble_gap_event *event, void *arg) {
    DEBUG_EVENT_printf("peripheral_gap_event_cb: event=%d\n", event->type);
    if (!mp_bluetooth_is_active()) {
        return 0;
    }
    struct ble_gap_conn_desc desc;
    uint8_t addr[6] = {0};

    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            if (event->connect.status == 0) {
                // Connection established.
                ble_gap_conn_find(event->connect.conn_handle, &desc);
                reverse_addr_byte_order(addr, desc.peer_id_addr.val);
                mp_bluetooth_gap_on_connected_disconnected(MP_BLUETOOTH_IRQ_PERIPHERAL_CONNECT, event->connect.conn_handle, desc.peer_id_addr.type, addr);
            } else {
                // Connection failed.
                mp_bluetooth_gap_on_connected_disconnected(MP_BLUETOOTH_IRQ_PERIPHERAL_DISCONNECT, event->connect.conn_handle, 0xff, addr);
            }
            break;

        case BLE_GAP_EVENT_DISCONNECT:
            // Disconnect.
            reverse_addr_byte_order(addr, event->disconnect.conn.peer_id_addr.val);
            mp_bluetooth_gap_on_connected_disconnected(MP_BLUETOOTH_IRQ_PERIPHERAL_DISCONNECT, event->disconnect.conn.conn_handle, event->disconnect.conn.peer_id_addr.type, addr);

            break;

        case BLE_GAP_EVENT_NOTIFY_RX: {
            uint16_t ev = event->notify_rx.indication == 0 ? MP_BLUETOOTH_IRQ_GATTC_NOTIFY : MP_BLUETOOTH_IRQ_GATTC_INDICATE;
            gattc_on_data_available(ev, event->notify_rx.conn_handle, event->notify_rx.attr_handle, event->notify_rx.om);
            break;
        }

        case BLE_GAP_EVENT_CONN_UPDATE:
            // TODO
            break;

        case BLE_GAP_EVENT_CONN_UPDATE_REQ:
            // TODO
            break;

        default:
            break;
    }
    return 0;
}

int mp_bluetooth_gap_peripheral_connect(uint8_t addr_type, const uint8_t *addr, int32_t duration_ms) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    if (ble_gap_disc_active()) {
        mp_bluetooth_gap_scan_stop();
    }

    // TODO: This is the same as ble_gap_conn_params_dflt (i.e. passing NULL).
    STATIC const struct ble_gap_conn_params params = {
        .scan_itvl = 0x0010,
        .scan_window = 0x0010,
        .itvl_min = BLE_GAP_INITIAL_CONN_ITVL_MIN,
        .itvl_max = BLE_GAP_INITIAL_CONN_ITVL_MAX,
        .latency = BLE_GAP_INITIAL_CONN_LATENCY,
        .supervision_timeout = BLE_GAP_INITIAL_SUPERVISION_TIMEOUT,
        .min_ce_len = BLE_GAP_INITIAL_CONN_MIN_CE_LEN,
        .max_ce_len = BLE_GAP_INITIAL_CONN_MAX_CE_LEN,
    };

    ble_addr_t addr_nimble = create_nimble_addr(addr_type, addr);
    int err = ble_gap_connect(BLE_OWN_ADDR_PUBLIC, &addr_nimble, duration_ms, &params, &peripheral_gap_event_cb, NULL);
    return ble_hs_err_to_errno(err);
}

STATIC int peripheral_discover_service_cb(uint16_t conn_handle, const struct ble_gatt_error *error, const struct ble_gatt_svc *service, void *arg) {
    DEBUG_EVENT_printf("peripheral_discover_service_cb: conn_handle=%d status=%d start_handle=%d\n", conn_handle, error->status, service ? service->start_handle : -1);
    if (!mp_bluetooth_is_active()) {
        return 0;
    }
    if (error->status == 0) {
        mp_obj_bluetooth_uuid_t service_uuid = create_mp_uuid(&service->uuid);
        mp_bluetooth_gattc_on_primary_service_result(conn_handle, service->start_handle, service->end_handle, &service_uuid);
    } else {
        mp_bluetooth_gattc_on_discover_complete(MP_BLUETOOTH_IRQ_GATTC_SERVICE_DONE, conn_handle, error->status == BLE_HS_EDONE ? 0 : error->status);
    }
    return 0;
}

int mp_bluetooth_gattc_discover_primary_services(uint16_t conn_handle, const mp_obj_bluetooth_uuid_t *uuid) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    int err;
    if (uuid) {
        ble_uuid_any_t nimble_uuid;
        create_nimble_uuid(uuid, &nimble_uuid);
        err = ble_gattc_disc_svc_by_uuid(conn_handle, &nimble_uuid.u, &peripheral_discover_service_cb, NULL);
    } else {
        err = ble_gattc_disc_all_svcs(conn_handle, &peripheral_discover_service_cb, NULL);
    }
    return ble_hs_err_to_errno(err);
}

STATIC int ble_gatt_characteristic_cb(uint16_t conn_handle, const struct ble_gatt_error *error, const struct ble_gatt_chr *characteristic, void *arg) {
    DEBUG_EVENT_printf("ble_gatt_characteristic_cb: conn_handle=%d status=%d def_handle=%d val_handle=%d\n", conn_handle, error->status, characteristic ? characteristic->def_handle : -1, characteristic ? characteristic->val_handle : -1);
    if (!mp_bluetooth_is_active()) {
        return 0;
    }
    if (error->status == 0) {
        mp_obj_bluetooth_uuid_t characteristic_uuid = create_mp_uuid(&characteristic->uuid);
        mp_bluetooth_gattc_on_characteristic_result(conn_handle, characteristic->def_handle, characteristic->val_handle, characteristic->properties, &characteristic_uuid);
    } else {
        mp_bluetooth_gattc_on_discover_complete(MP_BLUETOOTH_IRQ_GATTC_CHARACTERISTIC_DONE, conn_handle, error->status == BLE_HS_EDONE ? 0 : error->status);
    }
    return 0;
}

int mp_bluetooth_gattc_discover_characteristics(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle, const mp_obj_bluetooth_uuid_t *uuid) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    int err;
    if (uuid) {
        ble_uuid_any_t nimble_uuid;
        create_nimble_uuid(uuid, &nimble_uuid);
        err = ble_gattc_disc_chrs_by_uuid(conn_handle, start_handle, end_handle, &nimble_uuid.u, &ble_gatt_characteristic_cb, NULL);
    } else {
        err = ble_gattc_disc_all_chrs(conn_handle, start_handle, end_handle, &ble_gatt_characteristic_cb, NULL);
    }
    return ble_hs_err_to_errno(err);
}

STATIC int ble_gatt_descriptor_cb(uint16_t conn_handle, const struct ble_gatt_error *error, uint16_t characteristic_val_handle, const struct ble_gatt_dsc *descriptor, void *arg) {
    DEBUG_EVENT_printf("ble_gatt_descriptor_cb: conn_handle=%d status=%d chr_handle=%d dsc_handle=%d\n", conn_handle, error->status, characteristic_val_handle, descriptor ? descriptor->handle : -1);
    if (!mp_bluetooth_is_active()) {
        return 0;
    }
    if (error->status == 0) {
        mp_obj_bluetooth_uuid_t descriptor_uuid = create_mp_uuid(&descriptor->uuid);
        mp_bluetooth_gattc_on_descriptor_result(conn_handle, descriptor->handle, &descriptor_uuid);
    } else {
        mp_bluetooth_gattc_on_discover_complete(MP_BLUETOOTH_IRQ_GATTC_DESCRIPTOR_DONE, conn_handle, error->status == BLE_HS_EDONE ? 0 : error->status);
    }
    return 0;
}

int mp_bluetooth_gattc_discover_descriptors(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    int err = ble_gattc_disc_all_dscs(conn_handle, start_handle, end_handle, &ble_gatt_descriptor_cb, NULL);
    return ble_hs_err_to_errno(err);
}

STATIC int ble_gatt_attr_read_cb(uint16_t conn_handle, const struct ble_gatt_error *error, struct ble_gatt_attr *attr, void *arg) {
    DEBUG_EVENT_printf("ble_gatt_attr_read_cb: conn_handle=%d status=%d handle=%d\n", conn_handle, error->status, attr ? attr->handle : -1);
    if (!mp_bluetooth_is_active()) {
        return 0;
    }
    if (error->status == 0) {
        gattc_on_data_available(MP_BLUETOOTH_IRQ_GATTC_READ_RESULT, conn_handle, attr->handle, attr->om);
    }
    mp_bluetooth_gattc_on_read_write_status(MP_BLUETOOTH_IRQ_GATTC_READ_DONE, conn_handle, attr->handle, error->status);
    return 0;
}

// Initiate read of a value from the remote peripheral.
int mp_bluetooth_gattc_read(uint16_t conn_handle, uint16_t value_handle) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    int err = ble_gattc_read(conn_handle, value_handle, &ble_gatt_attr_read_cb, NULL);
    return ble_hs_err_to_errno(err);
}

STATIC int ble_gatt_attr_write_cb(uint16_t conn_handle, const struct ble_gatt_error *error, struct ble_gatt_attr *attr, void *arg) {
    DEBUG_EVENT_printf("ble_gatt_attr_write_cb: conn_handle=%d status=%d handle=%d\n", conn_handle, error->status, attr ? attr->handle : -1);
    if (!mp_bluetooth_is_active()) {
        return 0;
    }
    mp_bluetooth_gattc_on_read_write_status(MP_BLUETOOTH_IRQ_GATTC_WRITE_DONE, conn_handle, attr->handle, error->status);
    return 0;
}

// Write the value to the remote peripheral.
int mp_bluetooth_gattc_write(uint16_t conn_handle, uint16_t value_handle, const uint8_t *value, size_t *value_len, unsigned int mode) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    int err;
    if (mode == MP_BLUETOOTH_WRITE_MODE_NO_RESPONSE) {
        err = ble_gattc_write_no_rsp_flat(conn_handle, value_handle, value, *value_len);
    } else if (mode == MP_BLUETOOTH_WRITE_MODE_WITH_RESPONSE) {
        err = ble_gattc_write_flat(conn_handle, value_handle, value, *value_len, &ble_gatt_attr_write_cb, NULL);
    } else {
        err = BLE_HS_EINVAL;
    }
    return ble_hs_err_to_errno(err);
}

#endif // MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE

#endif // MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_NIMBLE
