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
#include "extmod/mpbthci.h"

#include "host/ble_hs.h"
#include "host/util/util.h"
#include "nimble/ble.h"
#include "nimble/nimble_port.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#if MICROPY_PY_BLUETOOTH_ENABLE_L2CAP_CHANNELS
// We need the definition of "struct ble_l2cap_chan".
// See l2cap_channel_event() for details.
#include "nimble/host/src/ble_l2cap_priv.h"
#endif

#if MICROPY_PY_BLUETOOTH_ENABLE_HCI_CMD || MICROPY_BLUETOOTH_USE_ZEPHYR_STATIC_ADDRESS
// For ble_hs_hci_cmd_tx
#include "nimble/host/src/ble_hs_hci_priv.h"
#endif

#define DEBUG_printf(...) // printf("nimble: " __VA_ARGS__)

#define ERRNO_BLUETOOTH_NOT_ACTIVE MP_ENODEV

static uint8_t nimble_address_mode = BLE_OWN_ADDR_RANDOM;

#define NIMBLE_STARTUP_TIMEOUT 2000

// Any BLE_HS_xxx code not in this table will default to MP_EIO.
static int8_t ble_hs_err_to_errno_table[] = {
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
    [BLE_HS_EBADDATA] = MP_EINVAL,
};

static int ble_hs_err_to_errno(int err);

static ble_uuid_t *create_nimble_uuid(const mp_obj_bluetooth_uuid_t *uuid, ble_uuid_any_t *storage);
static void reverse_addr_byte_order(uint8_t *addr_out, const uint8_t *addr_in);

#if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
static mp_obj_bluetooth_uuid_t create_mp_uuid(const ble_uuid_any_t *uuid);
static ble_addr_t create_nimble_addr(uint8_t addr_type, const uint8_t *addr);
#endif

static void reset_cb(int reason);

static bool has_public_address(void);
static void set_random_address(bool nrpa);

#if MICROPY_PY_BLUETOOTH_ENABLE_PAIRING_BONDING
static int load_irk(void);
#endif

static void sync_cb(void);

#if !MICROPY_BLUETOOTH_NIMBLE_BINDINGS_ONLY
static void ble_hs_shutdown_stop_cb(int status, void *arg);
#endif

// Successfully registered service/char/desc handles.
static void gatts_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);

// Events about a connected central (we're in peripheral role).
static int central_gap_event_cb(struct ble_gap_event *event, void *arg);
#if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
// Events about a connected peripheral (we're in central role).
static int peripheral_gap_event_cb(struct ble_gap_event *event, void *arg);
#endif
// Used by both of the above.
static int commmon_gap_event_cb(struct ble_gap_event *event, void *arg);

#if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
// Scan results.
static int gap_scan_cb(struct ble_gap_event *event, void *arg);
#endif

#if MICROPY_PY_BLUETOOTH_ENABLE_GATT_CLIENT
// Data available (either due to notify/indicate or successful read).
static void gattc_on_data_available(uint8_t event, uint16_t conn_handle, uint16_t value_handle, const struct os_mbuf *om);

// Client discovery callbacks.
static int ble_gattc_service_cb(uint16_t conn_handle, const struct ble_gatt_error *error, const struct ble_gatt_svc *service, void *arg);
static int ble_gattc_characteristic_cb(uint16_t conn_handle, const struct ble_gatt_error *error, const struct ble_gatt_chr *characteristic, void *arg);
static int ble_gattc_descriptor_cb(uint16_t conn_handle, const struct ble_gatt_error *error, uint16_t characteristic_val_handle, const struct ble_gatt_dsc *descriptor, void *arg);

// Client read/write handlers.
static int ble_gattc_attr_read_cb(uint16_t conn_handle, const struct ble_gatt_error *error, struct ble_gatt_attr *attr, void *arg);
static int ble_gattc_attr_write_cb(uint16_t conn_handle, const struct ble_gatt_error *error, struct ble_gatt_attr *attr, void *arg);
#endif

#if MICROPY_PY_BLUETOOTH_ENABLE_PAIRING_BONDING
// Bonding store.
static int ble_secret_store_read(int obj_type, const union ble_store_key *key, union ble_store_value *value);
static int ble_secret_store_write(int obj_type, const union ble_store_value *val);
static int ble_secret_store_delete(int obj_type, const union ble_store_key *key);
#endif

static int ble_hs_err_to_errno(int err) {
    DEBUG_printf("ble_hs_err_to_errno: %d\n", err);
    if (!err) {
        return 0;
    }
    if (err >= 0 && (unsigned)err < MP_ARRAY_SIZE(ble_hs_err_to_errno_table) && ble_hs_err_to_errno_table[err]) {
        // Return an MP_Exxx error code.
        return ble_hs_err_to_errno_table[err];
    } else {
        // Pass through the BLE error code.
        return -err;
    }
}

// Note: modbluetooth UUIDs store their data in LE.
static ble_uuid_t *create_nimble_uuid(const mp_obj_bluetooth_uuid_t *uuid, ble_uuid_any_t *storage) {
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

// modbluetooth (and the layers above it) work in BE for addresses, Nimble works in LE.
static void reverse_addr_byte_order(uint8_t *addr_out, const uint8_t *addr_in) {
    for (int i = 0; i < 6; ++i) {
        addr_out[i] = addr_in[5 - i];
    }
}

#if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE

static mp_obj_bluetooth_uuid_t create_mp_uuid(const ble_uuid_any_t *uuid) {
    mp_obj_bluetooth_uuid_t result;
    result.base.type = &mp_type_bluetooth_uuid;
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

static ble_addr_t create_nimble_addr(uint8_t addr_type, const uint8_t *addr) {
    ble_addr_t addr_nimble;
    addr_nimble.type = addr_type;
    // Incoming addr is from modbluetooth (BE), so copy and convert to LE for Nimble.
    reverse_addr_byte_order(addr_nimble.val, addr);
    return addr_nimble;
}

#endif // MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE

volatile int mp_bluetooth_nimble_ble_state = MP_BLUETOOTH_NIMBLE_BLE_STATE_OFF;

static void reset_cb(int reason) {
    (void)reason;
}

static bool has_public_address(void) {
    return ble_hs_id_copy_addr(BLE_ADDR_PUBLIC, NULL, NULL) == 0;
}

static void set_random_address(bool nrpa) {
    int rc;
    (void)rc;
    ble_addr_t addr;
    #if MICROPY_BLUETOOTH_USE_MP_HAL_GET_MAC_STATIC_ADDRESS
    if (!nrpa) {
        DEBUG_printf("set_random_address: Generating static address using mp_hal_get_mac\n");
        uint8_t hal_mac_addr[6];
        mp_hal_get_mac(MP_HAL_MAC_BDADDR, hal_mac_addr);
        addr = create_nimble_addr(BLE_ADDR_RANDOM, hal_mac_addr);
        // Mark it as STATIC (not RPA or NRPA).
        addr.val[5] |= 0xc0;
    } else
    #elif MICROPY_BLUETOOTH_USE_ZEPHYR_STATIC_ADDRESS
    if (!nrpa) {
        DEBUG_printf("set_random_address: Generating static address from Zephyr controller\n");
        uint8_t buf[23];
        rc = ble_hs_hci_cmd_tx(BLE_HCI_OP(BLE_HCI_OGF_VENDOR, 0x09), NULL, 0, buf, sizeof(buf));
        assert(rc == 0);
        memcpy(addr.val, buf + 1, 6);
    } else
    #endif
    {
        DEBUG_printf("set_random_address: Generating random static address\n");
        rc = ble_hs_id_gen_rnd(nrpa ? 1 : 0, &addr);
        assert(rc == 0);
    }
    rc = ble_hs_id_set_rnd(addr.val);
    assert(rc == 0);
    rc = ble_hs_util_ensure_addr(1);
    assert(rc == 0);
}

#if MICROPY_PY_BLUETOOTH_ENABLE_PAIRING_BONDING
// For ble_hs_pvcy_set_our_irk
#include "nimble/host/src/ble_hs_pvcy_priv.h"
// For ble_hs_hci_util_rand
#include "nimble/host/src/ble_hs_hci_priv.h"
// For ble_hs_misc_restore_irks
#include "nimble/host/src/ble_hs_priv.h"

// Must be distinct to BLE_STORE_OBJ_TYPE_ in ble_store.h.
#define SECRET_TYPE_OUR_IRK 10

static int load_irk(void) {
    // NimBLE unconditionally loads a fixed IRK on startup.
    // See https://github.com/apache/mynewt-nimble/issues/887

    // Dummy key to use for the store.
    // Technically the secret type is enough as there will only be
    // one IRK so the key doesn't matter, but a NULL (None) key means "search".
    const uint8_t key[3] = {'i', 'r', 'k'};

    int rc;
    const uint8_t *irk;
    size_t irk_len;
    if (mp_bluetooth_gap_on_get_secret(SECRET_TYPE_OUR_IRK, 0, key, sizeof(key), &irk, &irk_len) && irk_len == 16) {
        DEBUG_printf("load_irk: Applying IRK from store.\n");
        rc = ble_hs_pvcy_set_our_irk(irk);
        if (rc) {
            return rc;
        }
    } else {
        DEBUG_printf("load_irk: Generating new IRK.\n");
        uint8_t rand_irk[16];
        rc = ble_hs_hci_util_rand(rand_irk, 16);
        if (rc) {
            return rc;
        }
        DEBUG_printf("load_irk: Saving new IRK.\n");
        if (!mp_bluetooth_gap_on_set_secret(SECRET_TYPE_OUR_IRK, key, sizeof(key), rand_irk, 16)) {
            // Code that doesn't implement pairing/bonding won't support set/get secret.
            // So they'll just get the default fixed IRK.
            return 0;
        }
        DEBUG_printf("load_irk: Applying new IRK.\n");
        rc = ble_hs_pvcy_set_our_irk(rand_irk);
        if (rc) {
            return rc;
        }
    }

    // Loading an IRK will clear all peer IRKs, so reload them from the store.
    rc = ble_hs_misc_restore_irks();
    return rc;
}
#endif

static void sync_cb(void) {
    int rc;
    (void)rc;

    DEBUG_printf("sync_cb: state=%d\n", mp_bluetooth_nimble_ble_state);

    if (mp_bluetooth_nimble_ble_state != MP_BLUETOOTH_NIMBLE_BLE_STATE_WAITING_FOR_SYNC) {
        return;
    }

    #if MICROPY_PY_BLUETOOTH_ENABLE_PAIRING_BONDING
    rc = load_irk();
    assert(rc == 0);
    #endif

    if (has_public_address()) {
        nimble_address_mode = BLE_OWN_ADDR_PUBLIC;
    } else {
        nimble_address_mode = BLE_OWN_ADDR_RANDOM;
        set_random_address(false);
    }

    if (MP_BLUETOOTH_DEFAULT_ATTR_LEN > 20) {
        DEBUG_printf("sync_cb: Setting MTU\n");
        rc = ble_att_set_preferred_mtu(MP_BLUETOOTH_DEFAULT_ATTR_LEN + 3);
        assert(rc == 0);
    }

    mp_bluetooth_nimble_ble_state = MP_BLUETOOTH_NIMBLE_BLE_STATE_ACTIVE;
}

static void gatts_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg) {
    if (!mp_bluetooth_is_active()) {
        return;
    }
    switch (ctxt->op) {
        case BLE_GATT_REGISTER_OP_SVC:
            // Called when a service is successfully registered.
            DEBUG_printf("gatts_register_cb: svc uuid=%p handle=%d\n", &ctxt->svc.svc_def->uuid, ctxt->svc.handle);
            break;

        case BLE_GATT_REGISTER_OP_CHR:
            // Called when a characteristic is successfully registered.
            DEBUG_printf("gatts_register_cb: chr uuid=%p def_handle=%d val_handle=%d\n", &ctxt->chr.chr_def->uuid, ctxt->chr.def_handle, ctxt->chr.val_handle);

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
            DEBUG_printf("gatts_register_cb: dsc uuid=%p handle=%d\n", &ctxt->dsc.dsc_def->uuid, ctxt->dsc.handle);

            // See above, safe to alloc.
            mp_bluetooth_gatts_db_create_entry(MP_STATE_PORT(bluetooth_nimble_root_pointers)->gatts_db, ctxt->dsc.handle, MP_BLUETOOTH_DEFAULT_ATTR_LEN);

            // Unlike characteristics, we have to manually provide a way to get the handle back to the register method.
            *((uint16_t *)ctxt->dsc.dsc_def->arg) = ctxt->dsc.handle;
            break;

        default:
            DEBUG_printf("gatts_register_cb: unknown op %d\n", ctxt->op);
            break;
    }
}

static int commmon_gap_event_cb(struct ble_gap_event *event, void *arg) {
    struct ble_gap_conn_desc desc;

    switch (event->type) {
        #if MICROPY_PY_BLUETOOTH_ENABLE_GATT_CLIENT
        case BLE_GAP_EVENT_NOTIFY_RX: {
            uint16_t ev = event->notify_rx.indication == 0 ? MP_BLUETOOTH_IRQ_GATTC_NOTIFY : MP_BLUETOOTH_IRQ_GATTC_INDICATE;
            gattc_on_data_available(ev, event->notify_rx.conn_handle, event->notify_rx.attr_handle, event->notify_rx.om);
            return 0;
        }
        #endif // MICROPY_PY_BLUETOOTH_ENABLE_GATT_CLIENT

        case BLE_GAP_EVENT_CONN_UPDATE: {
            DEBUG_printf("commmon_gap_event_cb: connection update: status=%d\n", event->conn_update.status);
            if (ble_gap_conn_find(event->conn_update.conn_handle, &desc) == 0) {
                mp_bluetooth_gap_on_connection_update(event->conn_update.conn_handle, desc.conn_itvl, desc.conn_latency, desc.supervision_timeout, event->conn_update.status == 0 ? 0 : 1);
            }
            return 0;
        }

        case BLE_GAP_EVENT_MTU: {
            if (event->mtu.channel_id == BLE_L2CAP_CID_ATT) {
                DEBUG_printf("commmon_gap_event_cb: mtu update: conn_handle=%d cid=%d mtu=%d\n", event->mtu.conn_handle, event->mtu.channel_id, event->mtu.value);
                mp_bluetooth_gatts_on_mtu_exchanged(event->mtu.conn_handle, event->mtu.value);
            }
            return 0;
        }

        case BLE_GAP_EVENT_ENC_CHANGE: {
            DEBUG_printf("commmon_gap_event_cb: enc change: status=%d\n", event->enc_change.status);
            #if MICROPY_PY_BLUETOOTH_ENABLE_PAIRING_BONDING
            if (ble_gap_conn_find(event->enc_change.conn_handle, &desc) == 0) {
                mp_bluetooth_gatts_on_encryption_update(event->conn_update.conn_handle,
                    desc.sec_state.encrypted, desc.sec_state.authenticated,
                    desc.sec_state.bonded, desc.sec_state.key_size);
            }
            #endif
            return 0;
        }

        default:
            DEBUG_printf("commmon_gap_event_cb: unknown type %d\n", event->type);
            return 0;
    }
}

static int central_gap_event_cb(struct ble_gap_event *event, void *arg) {
    DEBUG_printf("central_gap_event_cb: type=%d\n", event->type);
    if (!mp_bluetooth_is_active()) {
        return 0;
    }
    struct ble_gap_conn_desc desc;
    uint8_t addr[6] = {0};

    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            DEBUG_printf("central_gap_event_cb: connect: status=%d\n", event->connect.status);
            if (event->connect.status == 0) {
                // Connection established.
                ble_gap_conn_find(event->connect.conn_handle, &desc);
                reverse_addr_byte_order(addr, desc.peer_id_addr.val);
                mp_bluetooth_gap_on_connected_disconnected(MP_BLUETOOTH_IRQ_CENTRAL_CONNECT, event->connect.conn_handle, desc.peer_id_addr.type, addr);
            } else {
                // Connection failed.
                mp_bluetooth_gap_on_connected_disconnected(MP_BLUETOOTH_IRQ_CENTRAL_DISCONNECT, event->connect.conn_handle, 0xff, addr);
            }
            return 0;

        case BLE_GAP_EVENT_DISCONNECT:
            // Disconnect.
            DEBUG_printf("central_gap_event_cb: disconnect: reason=%d\n", event->disconnect.reason);
            reverse_addr_byte_order(addr, event->disconnect.conn.peer_id_addr.val);
            mp_bluetooth_gap_on_connected_disconnected(MP_BLUETOOTH_IRQ_CENTRAL_DISCONNECT, event->disconnect.conn.conn_handle, event->disconnect.conn.peer_id_addr.type, addr);
            return 0;

        case BLE_GAP_EVENT_NOTIFY_TX: {
            DEBUG_printf("central_gap_event_cb: notify_tx: %d %d\n", event->notify_tx.indication, event->notify_tx.status);
            // This event corresponds to either a sent notify/indicate (status == 0), or an indication confirmation (status != 0).
            if (event->notify_tx.indication && event->notify_tx.status != 0) {
                // Map "done/ack" to 0, otherwise pass the status directly.
                mp_bluetooth_gatts_on_indicate_complete(event->notify_tx.conn_handle, event->notify_tx.attr_handle, event->notify_tx.status == BLE_HS_EDONE ? 0 : event->notify_tx.status);
            }
            return 0;
        }

        case BLE_GAP_EVENT_PHY_UPDATE_COMPLETE:
            DEBUG_printf("central_gap_event_cb: phy update: %d\n", event->phy_updated.tx_phy);
            return 0;

        case BLE_GAP_EVENT_REPEAT_PAIRING: {
            // We recognized this peer but the peer doesn't recognize us.
            DEBUG_printf("central_gap_event_cb: repeat pairing: conn_handle=%d\n", event->repeat_pairing.conn_handle);

            // TODO: Consider returning BLE_GAP_REPEAT_PAIRING_IGNORE (and
            // possibly an API to configure this).

            // Delete the old bond.
            int rc = ble_gap_conn_find(event->repeat_pairing.conn_handle, &desc);
            if (rc == 0) {
                ble_store_util_delete_peer(&desc.peer_id_addr);
            }

            // Allow re-pairing.
            return BLE_GAP_REPEAT_PAIRING_RETRY;
        }

        case BLE_GAP_EVENT_PASSKEY_ACTION: {
            DEBUG_printf("central_gap_event_cb: passkey action: conn_handle=%d action=%d num=" UINT_FMT "\n", event->passkey.conn_handle, event->passkey.params.action, (mp_uint_t)event->passkey.params.numcmp);

            #if MICROPY_PY_BLUETOOTH_ENABLE_PAIRING_BONDING
            mp_bluetooth_gap_on_passkey_action(event->passkey.conn_handle, event->passkey.params.action, event->passkey.params.numcmp);
            #endif

            return 0;
        }

        case BLE_GAP_EVENT_SUBSCRIBE: {
            DEBUG_printf("central_gap_event_cb: subscribe: handle=%d, reason=%d notify=%d indicate=%d \n", event->subscribe.attr_handle, event->subscribe.reason, event->subscribe.cur_notify, event->subscribe.cur_indicate);
            return 0;
        }
    }

    return commmon_gap_event_cb(event, arg);
}

#if !MICROPY_BLUETOOTH_NIMBLE_BINDINGS_ONLY

// On ports such as ESP32 where we only implement the bindings, then
// the port must provide these functions.
// But for STM32 / Unix-H4, we provide a default implementation of the
// port-specific functionality.
// TODO: In the future if a port ever needs to customise these functions
// then investigate using MP_WEAK or splitting them out to another .c file.

#include "transport/uart/ble_hci_uart.h"

void mp_bluetooth_nimble_port_hci_init(void) {
    DEBUG_printf("mp_bluetooth_nimble_port_hci_init (nimble default)\n");
    // This calls mp_bluetooth_hci_uart_init (via ble_hci_uart_init --> hal_uart_config --> mp_bluetooth_hci_uart_init).
    ble_hci_uart_init();
    mp_bluetooth_hci_controller_init();
}

void mp_bluetooth_nimble_port_hci_deinit(void) {
    DEBUG_printf("mp_bluetooth_nimble_port_hci_deinit (nimble default)\n");
    mp_bluetooth_hci_controller_deinit();
    mp_bluetooth_hci_uart_deinit();
}

void mp_bluetooth_nimble_port_start(void) {
    DEBUG_printf("mp_bluetooth_nimble_port_start (nimble default)\n");
    // By default, assume port is already running its own background task (e.g. SysTick on STM32).
    // ESP32 runs a FreeRTOS task, Unix has a thread.
}

// Called when the host stop procedure has completed.
static void ble_hs_shutdown_stop_cb(int status, void *arg) {
    (void)status;
    (void)arg;
    mp_bluetooth_nimble_ble_state = MP_BLUETOOTH_NIMBLE_BLE_STATE_OFF;
}

static struct ble_hs_stop_listener ble_hs_shutdown_stop_listener;

void mp_bluetooth_nimble_port_shutdown(void) {
    DEBUG_printf("mp_bluetooth_nimble_port_shutdown (nimble default)\n");
    // By default, just call ble_hs_stop directly and wait for the stack to stop.

    mp_bluetooth_nimble_ble_state = MP_BLUETOOTH_NIMBLE_BLE_STATE_STOPPING;

    ble_hs_stop(&ble_hs_shutdown_stop_listener, ble_hs_shutdown_stop_cb, NULL);

    while (mp_bluetooth_nimble_ble_state != MP_BLUETOOTH_NIMBLE_BLE_STATE_OFF) {
        mp_event_wait_indefinite();
    }
}

#endif // !MICROPY_BLUETOOTH_NIMBLE_BINDINGS_ONLY

void nimble_reset_gatts_bss(void) {
    // NimBLE assumes that service registration only ever happens once, so
    // we need to reset service registration state from a previous stack startup.
    // These variables are defined in ble_hs.c and are only ever incremented
    // (during service registration) and never reset.
    // See https://github.com/apache/mynewt-nimble/issues/896
    extern uint16_t ble_hs_max_attrs;
    extern uint16_t ble_hs_max_services;
    extern uint16_t ble_hs_max_client_configs;
    ble_hs_max_attrs = 0;
    ble_hs_max_services = 0;
    ble_hs_max_client_configs = 0;
}

int mp_bluetooth_init(void) {
    DEBUG_printf("mp_bluetooth_init\n");
    // Clean up if necessary.
    mp_bluetooth_deinit();

    nimble_reset_gatts_bss();

    mp_bluetooth_nimble_ble_state = MP_BLUETOOTH_NIMBLE_BLE_STATE_STARTING;

    MP_STATE_PORT(bluetooth_nimble_root_pointers) = m_new0(mp_bluetooth_nimble_root_pointers_t, 1);
    mp_bluetooth_gatts_db_create(&MP_STATE_PORT(bluetooth_nimble_root_pointers)->gatts_db);

    #if !MICROPY_BLUETOOTH_NIMBLE_BINDINGS_ONLY
    // Dereference any previous NimBLE mallocs.
    MP_STATE_PORT(bluetooth_nimble_memory) = NULL;
    #endif

    // Allow port (ESP32) to override NimBLE's HCI init.
    // Otherwise default implementation above calls ble_hci_uart_init().
    mp_bluetooth_nimble_port_hci_init();

    // Static initialization is complete, can start processing events.
    mp_bluetooth_nimble_ble_state = MP_BLUETOOTH_NIMBLE_BLE_STATE_WAITING_FOR_SYNC;

    // Initialise NimBLE memory and data structures.
    DEBUG_printf("mp_bluetooth_init: nimble_port_init\n");
    nimble_port_init();

    ble_hs_cfg.reset_cb = reset_cb;
    ble_hs_cfg.sync_cb = sync_cb;
    ble_hs_cfg.gatts_register_cb = gatts_register_cb;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    #if MICROPY_PY_BLUETOOTH_ENABLE_PAIRING_BONDING
    ble_hs_cfg.sm_our_key_dist = BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID | BLE_SM_PAIR_KEY_DIST_SIGN;
    ble_hs_cfg.sm_their_key_dist = BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID | BLE_SM_PAIR_KEY_DIST_SIGN;
    ble_hs_cfg.store_read_cb = ble_secret_store_read;
    ble_hs_cfg.store_write_cb = ble_secret_store_write;
    ble_hs_cfg.store_delete_cb = ble_secret_store_delete;
    #endif // MICROPY_PY_BLUETOOTH_ENABLE_PAIRING_BONDING

    // Make sure that the HCI UART and event handling task is running.
    mp_bluetooth_nimble_port_start();

    // Run the scheduler while we wait for stack startup.
    // On non-ringbuffer builds (NimBLE on STM32/Unix) this will also poll the UART and run the event queue.
    mp_uint_t timeout_start_ticks_ms = mp_hal_ticks_ms();
    while (mp_bluetooth_nimble_ble_state != MP_BLUETOOTH_NIMBLE_BLE_STATE_ACTIVE) {
        uint32_t elapsed = mp_hal_ticks_ms() - timeout_start_ticks_ms;
        if (elapsed > NIMBLE_STARTUP_TIMEOUT) {
            break;
        }
        mp_event_wait_ms(NIMBLE_STARTUP_TIMEOUT - elapsed);
    }

    if (mp_bluetooth_nimble_ble_state != MP_BLUETOOTH_NIMBLE_BLE_STATE_ACTIVE) {
        mp_bluetooth_deinit();
        return MP_ETIMEDOUT;
    }

    DEBUG_printf("mp_bluetooth_init: starting services\n");

    // By default, just register the default gap/gatt service.
    ble_svc_gap_init();
    ble_svc_gatt_init();
    // The preceding two calls allocate service definitions on the heap,
    // then we must now call gatts_start to register those services
    // and free the heap memory.
    // Otherwise it will be realloc'ed on the next stack startup.
    ble_gatts_start();

    DEBUG_printf("mp_bluetooth_init: ready\n");

    return 0;
}

void mp_bluetooth_deinit(void) {
    DEBUG_printf("mp_bluetooth_deinit %d\n", mp_bluetooth_nimble_ble_state);
    if (mp_bluetooth_nimble_ble_state == MP_BLUETOOTH_NIMBLE_BLE_STATE_OFF) {
        return;
    }

    // Must call ble_hs_stop() in a port-specific way to stop the background
    // task. Default implementation provided above.
    if (mp_bluetooth_nimble_ble_state == MP_BLUETOOTH_NIMBLE_BLE_STATE_ACTIVE) {
        mp_bluetooth_gap_advertise_stop();
        #if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
        mp_bluetooth_gap_scan_stop();
        #endif

        DEBUG_printf("mp_bluetooth_deinit: starting port shutdown\n");

        mp_bluetooth_nimble_port_shutdown();
        assert(mp_bluetooth_nimble_ble_state == MP_BLUETOOTH_NIMBLE_BLE_STATE_OFF);
    } else {
        mp_bluetooth_nimble_ble_state = MP_BLUETOOTH_NIMBLE_BLE_STATE_OFF;
    }

    // Shutdown the HCI controller.
    mp_bluetooth_nimble_port_hci_deinit();

    MP_STATE_PORT(bluetooth_nimble_root_pointers) = NULL;

    #if !MICROPY_BLUETOOTH_NIMBLE_BINDINGS_ONLY
    // Dereference any previous NimBLE mallocs.
    MP_STATE_PORT(bluetooth_nimble_memory) = NULL;
    #endif

    DEBUG_printf("mp_bluetooth_deinit: shut down\n");
}

bool mp_bluetooth_is_active(void) {
    return mp_bluetooth_nimble_ble_state == MP_BLUETOOTH_NIMBLE_BLE_STATE_ACTIVE;
}

void mp_bluetooth_get_current_address(uint8_t *addr_type, uint8_t *addr) {
    if (!mp_bluetooth_is_active()) {
        mp_raise_OSError(ERRNO_BLUETOOTH_NOT_ACTIVE);
    }

    uint8_t addr_le[6];

    switch (nimble_address_mode) {
        case BLE_OWN_ADDR_PUBLIC:
            *addr_type = BLE_ADDR_PUBLIC;
            break;
        case BLE_OWN_ADDR_RANDOM:
            *addr_type = BLE_ADDR_RANDOM;
            break;
        case BLE_OWN_ADDR_RPA_PUBLIC_DEFAULT:
        case BLE_OWN_ADDR_RPA_RANDOM_DEFAULT:
        default:
            // TODO: If RPA/NRPA in use, get the current value.
            // Is this even possible in NimBLE?
            mp_raise_OSError(MP_EINVAL);
    }

    int rc = ble_hs_id_copy_addr(*addr_type, addr_le, NULL);
    if (rc != 0) {
        mp_raise_OSError(MP_EINVAL);
    }
    reverse_addr_byte_order(addr, addr_le);
}

void mp_bluetooth_set_address_mode(uint8_t addr_mode) {
    if (!mp_bluetooth_is_active()) {
        mp_raise_OSError(ERRNO_BLUETOOTH_NOT_ACTIVE);
    }
    switch (addr_mode) {
        case MP_BLUETOOTH_ADDRESS_MODE_PUBLIC:
            if (!has_public_address()) {
                // No public address available.
                mp_raise_OSError(MP_EINVAL);
            }
            nimble_address_mode = BLE_OWN_ADDR_PUBLIC;
            break;
        case MP_BLUETOOTH_ADDRESS_MODE_RANDOM:
            // Generate an static random address.
            set_random_address(false);
            nimble_address_mode = BLE_OWN_ADDR_RANDOM;
            break;
        case MP_BLUETOOTH_ADDRESS_MODE_RPA:
            if (has_public_address()) {
                nimble_address_mode = BLE_OWN_ADDR_RPA_PUBLIC_DEFAULT;
            } else {
                // Generate an static random address to use as the identity address.
                set_random_address(false);
                nimble_address_mode = BLE_OWN_ADDR_RPA_RANDOM_DEFAULT;
            }
            break;
        case MP_BLUETOOTH_ADDRESS_MODE_NRPA:
            // Generate an NRPA.
            set_random_address(true);
            // In NimBLE, NRPA is treated like a static random address that happens to be an NRPA.
            nimble_address_mode = BLE_OWN_ADDR_RANDOM;
            break;
    }
}

#if MICROPY_PY_BLUETOOTH_ENABLE_PAIRING_BONDING
void mp_bluetooth_set_bonding(bool enabled) {
    ble_hs_cfg.sm_bonding = enabled;
}

void mp_bluetooth_set_mitm_protection(bool enabled) {
    ble_hs_cfg.sm_mitm = enabled;
}

void mp_bluetooth_set_le_secure(bool enabled) {
    ble_hs_cfg.sm_sc = enabled;
}

void mp_bluetooth_set_io_capability(uint8_t capability) {
    ble_hs_cfg.sm_io_cap = capability;
}
#endif // MICROPY_PY_BLUETOOTH_ENABLE_PAIRING_BONDING

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

    ret = ble_gap_adv_start(nimble_address_mode, NULL, BLE_HS_FOREVER, &adv_params, central_gap_event_cb, NULL);
    if (ret == 0) {
        return 0;
    }
    DEBUG_printf("ble_gap_adv_start: %d\n", ret);

    return ble_hs_err_to_errno(ret);
}

void mp_bluetooth_gap_advertise_stop(void) {
    if (ble_gap_adv_active()) {
        ble_gap_adv_stop();
    }
}

static int characteristic_access_cb(uint16_t conn_handle, uint16_t value_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    DEBUG_printf("characteristic_access_cb: conn_handle=%u value_handle=%u op=%u\n", conn_handle, value_handle, ctxt->op);
    if (!mp_bluetooth_is_active()) {
        return 0;
    }
    mp_bluetooth_gatts_db_entry_t *entry;
    switch (ctxt->op) {
        case BLE_GATT_ACCESS_OP_READ_CHR:
        case BLE_GATT_ACCESS_OP_READ_DSC: {
            DEBUG_printf("write for %d %d (op=%d)\n", conn_handle, value_handle, ctxt->op);
            // Allow Python code to override (by using gatts_write), or deny (by returning false) the read.
            // Note this will be a no-op if the ringbuffer implementation is being used (i.e. the stack isn't
            // run in the scheduler). The ringbuffer is not used on STM32 and Unix-H4 only.
            int req = mp_bluetooth_gatts_on_read_request(conn_handle, value_handle);
            if (req) {
                return req;
            }

            entry = mp_bluetooth_gatts_db_lookup(MP_STATE_PORT(bluetooth_nimble_root_pointers)->gatts_db, value_handle);
            if (!entry) {
                return BLE_ATT_ERR_ATTR_NOT_FOUND;
            }

            if (os_mbuf_append(ctxt->om, entry->data, entry->data_len)) {
                return BLE_ATT_ERR_INSUFFICIENT_RES;
            }

            return 0;
        }
        case BLE_GATT_ACCESS_OP_WRITE_CHR:
        case BLE_GATT_ACCESS_OP_WRITE_DSC:
            DEBUG_printf("write for %d %d (op=%d)\n", conn_handle, value_handle, ctxt->op);
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

            // TODO: Consider failing with BLE_ATT_ERR_INSUFFICIENT_RES if the buffer is full.

            mp_bluetooth_gatts_on_write(conn_handle, value_handle);

            return 0;
    }
    return BLE_ATT_ERR_UNLIKELY;
}

int mp_bluetooth_gatts_register_service_begin(bool append) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }

    if (append) {
        // Don't support append yet (modbluetooth.c doesn't support it yet anyway).
        // TODO: This should be possible with NimBLE.
        return MP_EOPNOTSUPP;
    }

    nimble_reset_gatts_bss();

    int ret = ble_gatts_reset();
    if (ret != 0) {
        return ble_hs_err_to_errno(ret);
    }

    // Reset the gatt characteristic value db.
    mp_bluetooth_gatts_db_reset(MP_STATE_PORT(bluetooth_nimble_root_pointers)->gatts_db);

    // By default, just register the default gap/gatt service.
    ble_svc_gap_init();
    ble_svc_gatt_init();

    // Unref any previous service definitions.
    for (size_t i = 0; i < MP_STATE_PORT(bluetooth_nimble_root_pointers)->n_services; ++i) {
        MP_STATE_PORT(bluetooth_nimble_root_pointers)->services[i] = NULL;
    }
    MP_STATE_PORT(bluetooth_nimble_root_pointers)->n_services = 0;

    return 0;
}

int mp_bluetooth_gatts_register_service_end(void) {
    int ret = ble_gatts_start();
    if (ret != 0) {
        return ble_hs_err_to_errno(ret);
    }

    return 0;
}

int mp_bluetooth_gatts_register_service(mp_obj_bluetooth_uuid_t *service_uuid, mp_obj_bluetooth_uuid_t **characteristic_uuids, uint16_t *characteristic_flags, mp_obj_bluetooth_uuid_t **descriptor_uuids, uint16_t *descriptor_flags, uint8_t *num_descriptors, uint16_t *handles, size_t num_characteristics) {
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
        // NimBLE flags match the MP_BLUETOOTH_CHARACTERISTIC_FLAG_ ones exactly (including the security/privacy options).
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
                // NimBLE doesn't support security/privacy options on descriptors.
                uint8_t desc_att_flags = 0;
                if (descriptor_flags[descriptor_index] & MP_BLUETOOTH_CHARACTERISTIC_FLAG_READ) {
                    desc_att_flags |= BLE_ATT_F_READ;
                }
                if (descriptor_flags[descriptor_index] & (MP_BLUETOOTH_CHARACTERISTIC_FLAG_WRITE | MP_BLUETOOTH_CHARACTERISTIC_FLAG_WRITE_NO_RESPONSE)) {
                    desc_att_flags |= BLE_ATT_F_WRITE;
                }
                descriptors[j].att_flags = desc_att_flags;
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

int mp_bluetooth_gatts_read(uint16_t value_handle, const uint8_t **value, size_t *value_len) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    return mp_bluetooth_gatts_db_read(MP_STATE_PORT(bluetooth_nimble_root_pointers)->gatts_db, value_handle, value, value_len);
}

int mp_bluetooth_gatts_write(uint16_t value_handle, const uint8_t *value, size_t value_len, bool send_update) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    int err = mp_bluetooth_gatts_db_write(MP_STATE_PORT(bluetooth_nimble_root_pointers)->gatts_db, value_handle, value, value_len);
    if (err == 0 && send_update) {
        ble_gatts_chr_updated(value_handle);
    }
    return err;
}

int mp_bluetooth_gatts_notify_indicate(uint16_t conn_handle, uint16_t value_handle, int gatts_op, const uint8_t *value, size_t value_len) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }

    int err = BLE_HS_EINVAL;

    // NULL om in the _custom methods means "use DB value" (NimBLE will call
    // back into mp_bluetooth_gatts_read for us).
    struct os_mbuf *om = NULL;

    if (value) {
        om = ble_hs_mbuf_from_flat(value, value_len);
        if (om == NULL) {
            return MP_ENOMEM;
        }
    }

    // Note: Confusingly, Nimble's notify/notify_custom and indicate/indicate_custom
    // are "gattc" functions (even though they're used by peripherals, i.e. gatt servers).
    // See https://www.mail-archive.com/dev@mynewt.apache.org/msg01293.html

    switch (gatts_op) {
        case MP_BLUETOOTH_GATTS_OP_NOTIFY:
            err = ble_gattc_notify_custom(conn_handle, value_handle, om);
            break;
        case MP_BLUETOOTH_GATTS_OP_INDICATE:
            // This will raise BLE_GAP_EVENT_NOTIFY_TX with a status when it is
            // acknowledged (or timeout/error).
            err = ble_gattc_indicate_custom(conn_handle, value_handle, om);
            break;
    }

    return ble_hs_err_to_errno(err);
}

int mp_bluetooth_gatts_set_buffer(uint16_t value_handle, size_t len, bool append) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    return mp_bluetooth_gatts_db_resize(MP_STATE_PORT(bluetooth_nimble_root_pointers)->gatts_db, value_handle, len, append);
}

int mp_bluetooth_get_preferred_mtu(void) {
    if (!mp_bluetooth_is_active()) {
        mp_raise_OSError(ERRNO_BLUETOOTH_NOT_ACTIVE);
    }
    return ble_att_preferred_mtu();
}

int mp_bluetooth_set_preferred_mtu(uint16_t mtu) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    if (ble_att_set_preferred_mtu(mtu)) {
        return MP_EINVAL;
    }
    return 0;
}

#if MICROPY_PY_BLUETOOTH_ENABLE_PAIRING_BONDING
int mp_bluetooth_gap_pair(uint16_t conn_handle) {
    DEBUG_printf("mp_bluetooth_gap_pair: conn_handle=%d\n", conn_handle);
    return ble_hs_err_to_errno(ble_gap_security_initiate(conn_handle));
}

int mp_bluetooth_gap_passkey(uint16_t conn_handle, uint8_t action, mp_int_t passkey) {
    struct ble_sm_io io = {0};

    switch (action) {
        case MP_BLUETOOTH_PASSKEY_ACTION_INPUT: {
            io.passkey = passkey;
            break;
        }
        case MP_BLUETOOTH_PASSKEY_ACTION_DISPLAY: {
            io.passkey = passkey;
            break;
        }
        case MP_BLUETOOTH_PASSKEY_ACTION_NUMERIC_COMPARISON: {
            io.numcmp_accept = passkey != 0;
            break;
        }
        default: {
            return MP_EINVAL;
        }
    }

    io.action = action;

    DEBUG_printf("mp_bluetooth_gap_passkey: injecting IO: conn_handle=%d, action=%d, passkey=" UINT_FMT ", numcmp_accept=%d\n", conn_handle, io.action, (mp_uint_t)io.passkey, io.numcmp_accept);
    return ble_hs_err_to_errno(ble_sm_inject_io(conn_handle, &io));
}
#endif // MICROPY_PY_BLUETOOTH_ENABLE_PAIRING_BONDING

#if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE

static int gap_scan_cb(struct ble_gap_event *event, void *arg) {
    DEBUG_printf("gap_scan_cb: event=%d type=%d\n", event->type, event->type == BLE_GAP_EVENT_DISC ? event->disc.event_type : -1);
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

int mp_bluetooth_gap_scan_start(int32_t duration_ms, int32_t interval_us, int32_t window_us, bool active_scan) {
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
        .passive = active_scan ? 0 : 1,
        .filter_duplicates = 0,
    };
    int err = ble_gap_disc(nimble_address_mode, duration_ms, &discover_params, gap_scan_cb, NULL);
    return ble_hs_err_to_errno(err);
}

int mp_bluetooth_gap_scan_stop(void) {
    DEBUG_printf("mp_bluetooth_gap_scan_stop\n");
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
static int peripheral_gap_event_cb(struct ble_gap_event *event, void *arg) {
    DEBUG_printf("peripheral_gap_event_cb: event=%d\n", event->type);
    if (!mp_bluetooth_is_active()) {
        return 0;
    }
    struct ble_gap_conn_desc desc;
    uint8_t addr[6] = {0};

    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            DEBUG_printf("peripheral_gap_event_cb: status=%d\n", event->connect.status);
            if (event->connect.status == 0) {
                // Connection established.
                ble_gap_conn_find(event->connect.conn_handle, &desc);
                reverse_addr_byte_order(addr, desc.peer_id_addr.val);
                mp_bluetooth_gap_on_connected_disconnected(MP_BLUETOOTH_IRQ_PERIPHERAL_CONNECT, event->connect.conn_handle, desc.peer_id_addr.type, addr);
            } else {
                // Connection failed.
                mp_bluetooth_gap_on_connected_disconnected(MP_BLUETOOTH_IRQ_PERIPHERAL_DISCONNECT, event->connect.conn_handle, 0xff, addr);
            }
            return 0;

        case BLE_GAP_EVENT_DISCONNECT:
            // Disconnect.
            DEBUG_printf("peripheral_gap_event_cb: reason=%d\n", event->disconnect.reason);
            reverse_addr_byte_order(addr, event->disconnect.conn.peer_id_addr.val);
            mp_bluetooth_gap_on_connected_disconnected(MP_BLUETOOTH_IRQ_PERIPHERAL_DISCONNECT, event->disconnect.conn.conn_handle, event->disconnect.conn.peer_id_addr.type, addr);
            return 0;
    }

    return commmon_gap_event_cb(event, arg);
}

int mp_bluetooth_gap_peripheral_connect(uint8_t addr_type, const uint8_t *addr, int32_t duration_ms, int32_t min_conn_interval_us, int32_t max_conn_interval_us) {
    DEBUG_printf("mp_bluetooth_gap_peripheral_connect\n");
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    if (ble_gap_disc_active()) {
        mp_bluetooth_gap_scan_stop();
    }

    uint16_t conn_interval_min = min_conn_interval_us ? min_conn_interval_us / BLE_HCI_CONN_ITVL : BLE_GAP_INITIAL_CONN_ITVL_MIN;
    uint16_t conn_interval_max = max_conn_interval_us ? max_conn_interval_us / BLE_HCI_CONN_ITVL : BLE_GAP_INITIAL_CONN_ITVL_MAX;

    const struct ble_gap_conn_params params = {
        .scan_itvl = 0x0010,
        .scan_window = 0x0010,
        .itvl_min = conn_interval_min,
        .itvl_max = conn_interval_max,
        .latency = BLE_GAP_INITIAL_CONN_LATENCY,
        .supervision_timeout = BLE_GAP_INITIAL_SUPERVISION_TIMEOUT,
        .min_ce_len = BLE_GAP_INITIAL_CONN_MIN_CE_LEN,
        .max_ce_len = BLE_GAP_INITIAL_CONN_MAX_CE_LEN,
    };

    ble_addr_t addr_nimble = create_nimble_addr(addr_type, addr);
    int err = ble_gap_connect(nimble_address_mode, &addr_nimble, duration_ms, &params, &peripheral_gap_event_cb, NULL);
    return ble_hs_err_to_errno(err);
}

int mp_bluetooth_gap_peripheral_connect_cancel(void) {
    DEBUG_printf("mp_bluetooth_gap_peripheral_connect_cancel\n");
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    int err = ble_gap_conn_cancel();
    return ble_hs_err_to_errno(err);
}

static int ble_gattc_service_cb(uint16_t conn_handle, const struct ble_gatt_error *error, const struct ble_gatt_svc *service, void *arg) {
    DEBUG_printf("ble_gattc_service_cb: conn_handle=%d status=%d start_handle=%d\n", conn_handle, error->status, service ? service->start_handle : -1);
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

#endif // MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE

#if MICROPY_PY_BLUETOOTH_ENABLE_GATT_CLIENT

static void gattc_on_data_available(uint8_t event, uint16_t conn_handle, uint16_t value_handle, const struct os_mbuf *om) {
    // When the HCI data for an ATT payload arrives, the L2CAP channel will
    // buffer it into its receive buffer. We set BLE_L2CAP_JOIN_RX_FRAGS=1 in
    // syscfg.h so it should be rare that the mbuf is fragmented, but we do need
    // to be able to handle it. We pass all the fragments up to modbluetooth.c
    // which will create a temporary buffer on the MicroPython heap if necessary
    // to re-assemble them.

    // Count how many links are in the mbuf chain.
    size_t n = 0;
    const struct os_mbuf *elem = om;
    while (elem) {
        n += 1;
        elem = SLIST_NEXT(elem, om_next);
    }

    // Grab data pointers and lengths for each of the links.
    const uint8_t **data = mp_local_alloc(sizeof(uint8_t *) * n);
    uint16_t *data_len = mp_local_alloc(sizeof(uint16_t) * n);
    for (size_t i = 0; i < n; ++i) {
        data[i] = OS_MBUF_DATA(om, const uint8_t *);
        data_len[i] = om->om_len;
        om = SLIST_NEXT(om, om_next);
    }

    // Pass all the fragments together.
    mp_bluetooth_gattc_on_data_available(event, conn_handle, value_handle, data, data_len, n);

    mp_local_free(data_len);
    mp_local_free(data);
}

int mp_bluetooth_gattc_discover_primary_services(uint16_t conn_handle, const mp_obj_bluetooth_uuid_t *uuid) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    int err;
    if (uuid) {
        ble_uuid_any_t nimble_uuid;
        create_nimble_uuid(uuid, &nimble_uuid);
        err = ble_gattc_disc_svc_by_uuid(conn_handle, &nimble_uuid.u, &ble_gattc_service_cb, NULL);
    } else {
        err = ble_gattc_disc_all_svcs(conn_handle, &ble_gattc_service_cb, NULL);
    }
    return ble_hs_err_to_errno(err);
}

static bool match_char_uuid(const mp_obj_bluetooth_uuid_t *filter_uuid, const ble_uuid_any_t *result_uuid) {
    if (!filter_uuid) {
        return true;
    }
    ble_uuid_any_t filter_uuid_nimble;
    create_nimble_uuid(filter_uuid, &filter_uuid_nimble);
    return ble_uuid_cmp(&result_uuid->u, &filter_uuid_nimble.u) == 0;
}

static int ble_gattc_characteristic_cb(uint16_t conn_handle, const struct ble_gatt_error *error, const struct ble_gatt_chr *characteristic, void *arg) {
    DEBUG_printf("ble_gattc_characteristic_cb: conn_handle=%d status=%d def_handle=%d val_handle=%d\n", conn_handle, error->status, characteristic ? characteristic->def_handle : -1, characteristic ? characteristic->val_handle : -1);
    if (!mp_bluetooth_is_active()) {
        return 0;
    }

    mp_bluetooth_nimble_pending_characteristic_t *pending = &MP_STATE_PORT(bluetooth_nimble_root_pointers)->pending_char_result;
    if (pending->ready) {
        // If there's a pending characteristic, we now know what it's end handle is, report it up to modbluetooth.
        pending->ready = 0;

        // The end handle will either be the end of the query range (there are
        // no more results), or one before the current result's definition
        // handle.
        uint16_t end_handle = MP_STATE_PORT(bluetooth_nimble_root_pointers)->char_disc_end_handle;
        if (error->status == 0) {
            end_handle = characteristic->def_handle - 1;
        }

        // Assume same conn_handle because we're limiting to a single active discovery.
        mp_bluetooth_gattc_on_characteristic_result(conn_handle, pending->value_handle, end_handle, pending->properties, &pending->uuid);
    }

    if (error->status == 0) {
        // If there's no filter, or the filter matches, then save this result.
        if (match_char_uuid(MP_STATE_PORT(bluetooth_nimble_root_pointers)->char_filter_uuid, &characteristic->uuid)) {
            pending->value_handle = characteristic->val_handle;
            pending->properties = characteristic->properties;
            pending->uuid = create_mp_uuid(&characteristic->uuid);
            pending->ready = 1;
        }
    } else {
        // Finished (or failed). Allow another characteristic discovery to start.
        MP_STATE_PORT(bluetooth_nimble_root_pointers)->char_disc_end_handle = 0;

        // Report completion.
        mp_bluetooth_gattc_on_discover_complete(MP_BLUETOOTH_IRQ_GATTC_CHARACTERISTIC_DONE, conn_handle, error->status == BLE_HS_EDONE ? 0 : error->status);
    }
    return 0;
}

int mp_bluetooth_gattc_discover_characteristics(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle, const mp_obj_bluetooth_uuid_t *uuid) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }

    // The implementation of characteristic discovery queries for all
    // characteristics, and then UUID filtering is applied by NimBLE on each
    // characteristic. Unfortunately, each characteristic result does not
    // include its end handle, so you need to know the next characteristic
    // before you can raise the previous one to modbluetooth. But if we let
    // NimBLE do the filtering, then we don't necessarily see the next one.
    // So we make NimBLE return all results and do the filtering here instead.

    if (MP_STATE_PORT(bluetooth_nimble_root_pointers)->char_disc_end_handle) {
        // Only allow a single discovery (otherwise we'd need to track a
        // pending characteristic per conn handle).
        return MP_EBUSY;
    }

    // Set the uuid filter (if any). This needs to be a root pointer,
    // otherwise we'd use ble_gattc_disc_all_chrs's arg param.
    MP_STATE_PORT(bluetooth_nimble_root_pointers)->char_filter_uuid = uuid;

    int err = ble_gattc_disc_all_chrs(conn_handle, start_handle, end_handle, &ble_gattc_characteristic_cb, NULL);
    if (!err) {
        // Lock out concurrent characteristic discovery.
        MP_STATE_PORT(bluetooth_nimble_root_pointers)->char_disc_end_handle = end_handle;
    }
    return ble_hs_err_to_errno(err);
}

static int ble_gattc_descriptor_cb(uint16_t conn_handle, const struct ble_gatt_error *error, uint16_t characteristic_val_handle, const struct ble_gatt_dsc *descriptor, void *arg) {
    DEBUG_printf("ble_gattc_descriptor_cb: conn_handle=%d status=%d chr_handle=%d dsc_handle=%d\n", conn_handle, error->status, characteristic_val_handle, descriptor ? descriptor->handle : -1);
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
    int err = ble_gattc_disc_all_dscs(conn_handle, start_handle, end_handle, &ble_gattc_descriptor_cb, NULL);
    return ble_hs_err_to_errno(err);
}

static int ble_gattc_attr_read_cb(uint16_t conn_handle, const struct ble_gatt_error *error, struct ble_gatt_attr *attr, void *arg) {
    uint16_t handle = attr ? attr->handle : (error ? error->att_handle : 0xffff);
    DEBUG_printf("ble_gattc_attr_read_cb: conn_handle=%d status=%d handle=%d\n", conn_handle, error->status, handle);
    if (!mp_bluetooth_is_active()) {
        return 0;
    }
    if (error->status == 0) {
        gattc_on_data_available(MP_BLUETOOTH_IRQ_GATTC_READ_RESULT, conn_handle, attr->handle, attr->om);
    }
    mp_bluetooth_gattc_on_read_write_status(MP_BLUETOOTH_IRQ_GATTC_READ_DONE, conn_handle, handle, error->status);
    return 0;
}

// Initiate read of a value from the remote peripheral.
int mp_bluetooth_gattc_read(uint16_t conn_handle, uint16_t value_handle) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    int err = ble_gattc_read(conn_handle, value_handle, &ble_gattc_attr_read_cb, NULL);
    return ble_hs_err_to_errno(err);
}

static int ble_gattc_attr_write_cb(uint16_t conn_handle, const struct ble_gatt_error *error, struct ble_gatt_attr *attr, void *arg) {
    uint16_t handle = attr ? attr->handle : (error ? error->att_handle : 0xffff);
    DEBUG_printf("ble_gattc_attr_write_cb: conn_handle=%d status=%d handle=%d\n", conn_handle, error->status, handle);
    if (!mp_bluetooth_is_active()) {
        return 0;
    }
    mp_bluetooth_gattc_on_read_write_status(MP_BLUETOOTH_IRQ_GATTC_WRITE_DONE, conn_handle, handle, error->status);
    return 0;
}

// Write the value to the remote peripheral.
int mp_bluetooth_gattc_write(uint16_t conn_handle, uint16_t value_handle, const uint8_t *value, size_t value_len, unsigned int mode) {
    if (!mp_bluetooth_is_active()) {
        return ERRNO_BLUETOOTH_NOT_ACTIVE;
    }
    int err;
    if (mode == MP_BLUETOOTH_WRITE_MODE_NO_RESPONSE) {
        err = ble_gattc_write_no_rsp_flat(conn_handle, value_handle, value, value_len);
    } else if (mode == MP_BLUETOOTH_WRITE_MODE_WITH_RESPONSE) {
        err = ble_gattc_write_flat(conn_handle, value_handle, value, value_len, &ble_gattc_attr_write_cb, NULL);
    } else {
        err = BLE_HS_EINVAL;
    }
    return ble_hs_err_to_errno(err);
}

int mp_bluetooth_gattc_exchange_mtu(uint16_t conn_handle) {
    DEBUG_printf("mp_bluetooth_exchange_mtu: conn_handle=%d mtu=%d\n", conn_handle, ble_att_preferred_mtu());

    // Using NULL callback (we'll get notified in gap_event_cb instead).
    return ble_hs_err_to_errno(ble_gattc_exchange_mtu(conn_handle, NULL, NULL));
}

#endif // MICROPY_PY_BLUETOOTH_ENABLE_GATT_CLIENT

#if MICROPY_PY_BLUETOOTH_ENABLE_L2CAP_CHANNELS
static void unstall_l2cap_channel(void);
#endif

void mp_bluetooth_nimble_sent_hci_packet(void) {
    #if MICROPY_PY_BLUETOOTH_ENABLE_L2CAP_CHANNELS
    if (os_msys_num_free() >= os_msys_count() * 3 / 4) {
        unstall_l2cap_channel();
    }
    #endif
}

#if MICROPY_PY_BLUETOOTH_ENABLE_L2CAP_CHANNELS

// Fortunately NimBLE uses mbuf chains correctly with L2CAP COC (rather than
// accessing the mbuf internals directly), so we can use a small block size to
// avoid excessive fragmentation and rely on them chaining together for larger
// payloads.
#define L2CAP_BUF_BLOCK_SIZE (128)

// This gives us enough room to have one MTU-size transmit buffer and two
// MTU-sized receive buffers. Note that we use the local MTU to calculate
// the buffer size. This means that if the peer MTU is larger, then
// there might not be enough space in the pool to send a full peer-MTU
// sized payload and mp_bluetooth_l2cap_send will return ENOMEM.
#define L2CAP_BUF_SIZE_MTUS_PER_CHANNEL (3)

typedef struct _mp_bluetooth_nimble_l2cap_channel_t {
    struct ble_l2cap_chan *chan;
    struct os_mbuf_pool sdu_mbuf_pool;
    struct os_mempool sdu_mempool;
    struct os_mbuf *rx_pending;
    bool irq_in_progress;
    bool mem_stalled;
    uint16_t mtu;
    os_membuf_t sdu_mem[];
} mp_bluetooth_nimble_l2cap_channel_t;

static void destroy_l2cap_channel();
static int l2cap_channel_event(struct ble_l2cap_event *event, void *arg);
static mp_bluetooth_nimble_l2cap_channel_t *get_l2cap_channel_for_conn_cid(uint16_t conn_handle, uint16_t cid);
static int create_l2cap_channel(uint16_t mtu, mp_bluetooth_nimble_l2cap_channel_t **out);

static void destroy_l2cap_channel() {
    // Only free the l2cap channel if we're the one that initiated the connection.
    // Listeners continue listening on the same channel.
    if (!MP_STATE_PORT(bluetooth_nimble_root_pointers)->l2cap_listening) {
        MP_STATE_PORT(bluetooth_nimble_root_pointers)->l2cap_chan = NULL;
    }
}

static void unstall_l2cap_channel(void) {
    // Whenever we send an HCI packet and the sys mempool is now less than 1/4 full,
    // we can un-stall the L2CAP channel if it was marked as "mem_stalled" by
    // mp_bluetooth_l2cap_send. (This happens if the pool is half-empty).
    mp_bluetooth_nimble_l2cap_channel_t *chan = MP_STATE_PORT(bluetooth_nimble_root_pointers)->l2cap_chan;
    if (!chan || !chan->mem_stalled) {
        return;
    }
    DEBUG_printf("unstall_l2cap_channel: count %d, free: %d\n", os_msys_count(), os_msys_num_free());
    chan->mem_stalled = false;
    mp_bluetooth_on_l2cap_send_ready(chan->chan->conn_handle, chan->chan->scid, 0);
}

static int l2cap_channel_event(struct ble_l2cap_event *event, void *arg) {
    DEBUG_printf("l2cap_channel_event: type=%d\n", event->type);
    mp_bluetooth_nimble_l2cap_channel_t *chan = (mp_bluetooth_nimble_l2cap_channel_t *)arg;
    struct ble_l2cap_chan_info info;

    switch (event->type) {
        case BLE_L2CAP_EVENT_COC_CONNECTED: {
            DEBUG_printf("l2cap_channel_event: connect: conn_handle=%d status=%d\n", event->connect.conn_handle, event->connect.status);
            chan->chan = event->connect.chan;

            ble_l2cap_get_chan_info(event->connect.chan, &info);
            if (event->connect.status == 0) {
                mp_bluetooth_on_l2cap_connect(event->connect.conn_handle, info.scid, info.psm, info.our_coc_mtu, info.peer_coc_mtu);
            } else {
                mp_bluetooth_on_l2cap_disconnect(event->connect.conn_handle, info.scid, info.psm, event->connect.status);
                destroy_l2cap_channel();
            }
            break;
        }
        case BLE_L2CAP_EVENT_COC_DISCONNECTED: {
            DEBUG_printf("l2cap_channel_event: disconnect: conn_handle=%d\n", event->disconnect.conn_handle);
            ble_l2cap_get_chan_info(event->disconnect.chan, &info);
            mp_bluetooth_on_l2cap_disconnect(event->disconnect.conn_handle, info.scid, info.psm, 0);
            destroy_l2cap_channel();
            break;
        }
        case BLE_L2CAP_EVENT_COC_ACCEPT: {
            DEBUG_printf("l2cap_channel_event: accept: conn_handle=%d peer_sdu_size=%d\n", event->accept.conn_handle, event->accept.peer_sdu_size);
            chan->chan = event->accept.chan;
            ble_l2cap_get_chan_info(event->accept.chan, &info);
            int ret = mp_bluetooth_on_l2cap_accept(event->accept.conn_handle, info.scid, info.psm, info.our_coc_mtu, info.peer_coc_mtu);
            if (ret != 0) {
                return ret;
            }
            struct os_mbuf *sdu_rx = os_mbuf_get_pkthdr(&chan->sdu_mbuf_pool, 0);
            assert(sdu_rx);
            return ble_l2cap_recv_ready(chan->chan, sdu_rx);
        }
        case BLE_L2CAP_EVENT_COC_DATA_RECEIVED: {
            DEBUG_printf("l2cap_channel_event: receive: conn_handle=%d len=%d\n", event->receive.conn_handle, OS_MBUF_PKTLEN(event->receive.sdu_rx));

            if (chan->rx_pending) {
                // Ideally this doesn't happen, as the sender should not get
                // any more credits to send more data until we call
                // ble_l2cap_recv_ready. However there might be multiple
                // in-flight packets if the sender was able to send more than
                // one before stalling.
                DEBUG_printf("l2cap_channel_event: receive: appending to rx pending\n");
                // Note: os_mbuf_concat will just join the two together, so
                // sdu_rx is now "owned" by rx_pending.
                os_mbuf_concat(chan->rx_pending, event->receive.sdu_rx);
            } else {
                // Normal case is when the first payload arrives since calling
                // ble_l2cap_recv_ready.
                DEBUG_printf("l2cap_event: receive: new payload\n");
                // Take ownership of sdu_rx.
                chan->rx_pending = event->receive.sdu_rx;
            }

            struct os_mbuf *sdu_rx = os_mbuf_get_pkthdr(&chan->sdu_mbuf_pool, 0);
            assert(sdu_rx);

            // ble_l2cap_coc_rx_fn invokes this event handler when a complete payload arrives.
            // However, it NULLs chan->chan->coc_rx.sdu before doing so, expecting that
            // ble_l2cap_recv_ready will be called to give it a new mbuf.
            // This means that if another payload arrives before we call ble_l2cap_recv_ready
            // then ble_l2cap_coc_rx_fn will NULL-deref coc_rx.sdu.

            // Because we're not yet ready to grant new credits to the channel, we can't call
            // ble_l2cap_recv_ready yet, so instead we just give it a new mbuf. This requires
            // ble_l2cap_priv.h for the definition of chan->chan (i.e. struct ble_l2cap_chan).
            chan->chan->coc_rx.sdu = sdu_rx;

            ble_l2cap_get_chan_info(event->receive.chan, &info);

            // Don't allow granting more credits until after the IRQ is handled.
            chan->irq_in_progress = true;

            mp_bluetooth_on_l2cap_recv(event->receive.conn_handle, info.scid);
            chan->irq_in_progress = false;

            // If all data has been consumed by the IRQ handler, then now allow
            // more credits. If the IRQ handler doesn't consume all available data
            // then rx_pending will be still set.
            if (!chan->rx_pending) {
                struct os_mbuf *sdu_rx = chan->chan->coc_rx.sdu;
                assert(sdu_rx);
                if (sdu_rx) {
                    ble_l2cap_recv_ready(chan->chan, sdu_rx);
                }
            }
            break;
        }
        case BLE_L2CAP_EVENT_COC_TX_UNSTALLED: {
            DEBUG_printf("l2cap_channel_event: tx_unstalled: conn_handle=%d status=%d\n", event->tx_unstalled.conn_handle, event->tx_unstalled.status);
            assert(event->tx_unstalled.conn_handle == chan->chan->conn_handle);
            // Don't un-stall if we're still waiting for room in the sys pool.
            if (!chan->mem_stalled) {
                ble_l2cap_get_chan_info(event->receive.chan, &info);
                // Map status to {0,1} (i.e. "sent everything", or "partial send").
                mp_bluetooth_on_l2cap_send_ready(event->tx_unstalled.conn_handle, info.scid, event->tx_unstalled.status == 0 ? 0 : 1);
            }
            break;
        }
        case BLE_L2CAP_EVENT_COC_RECONFIG_COMPLETED: {
            DEBUG_printf("l2cap_channel_event: reconfig_completed: conn_handle=%d\n", event->reconfigured.conn_handle);
            break;
        }
        case BLE_L2CAP_EVENT_COC_PEER_RECONFIGURED: {
            DEBUG_printf("l2cap_channel_event: peer_reconfigured: conn_handle=%d\n", event->reconfigured.conn_handle);
            break;
        }
        default: {
            DEBUG_printf("l2cap_channel_event: unknown event\n");
            break;
        }
    }

    return 0;
}

static mp_bluetooth_nimble_l2cap_channel_t *get_l2cap_channel_for_conn_cid(uint16_t conn_handle, uint16_t cid) {
    // TODO: Support more than one concurrent L2CAP channel. At the moment we
    // just verify that the cid refers to the current channel.
    mp_bluetooth_nimble_l2cap_channel_t *chan = MP_STATE_PORT(bluetooth_nimble_root_pointers)->l2cap_chan;

    if (!chan) {
        return NULL;
    }

    struct ble_l2cap_chan_info info;
    ble_l2cap_get_chan_info(chan->chan, &info);

    if (info.scid != cid || ble_l2cap_get_conn_handle(chan->chan) != conn_handle) {
        return NULL;
    }

    return chan;
}

static int create_l2cap_channel(uint16_t mtu, mp_bluetooth_nimble_l2cap_channel_t **out) {
    if (MP_STATE_PORT(bluetooth_nimble_root_pointers)->l2cap_chan) {
        // Only one L2CAP channel allowed.
        // Additionally, if we're listening, then no connections may be initiated.
        DEBUG_printf("create_l2cap_channel: channel already in use\n");
        return MP_EALREADY;
    }

    // We want the TX and RX buffers to share a pool that is some multiple of
    // the MTU size. Figure out how many blocks per MTU (rounding up), then
    // multiply that by the "MTUs per channel" (set to 3 above).
    const size_t buf_blocks = MP_CEIL_DIVIDE(mtu, L2CAP_BUF_BLOCK_SIZE) * L2CAP_BUF_SIZE_MTUS_PER_CHANNEL;

    mp_bluetooth_nimble_l2cap_channel_t *chan = m_new_obj_var(mp_bluetooth_nimble_l2cap_channel_t, sdu_mem, uint8_t, OS_MEMPOOL_SIZE(buf_blocks, L2CAP_BUF_BLOCK_SIZE) * sizeof(os_membuf_t));
    MP_STATE_PORT(bluetooth_nimble_root_pointers)->l2cap_chan = chan;

    // Will be set in BLE_L2CAP_EVENT_COC_CONNECTED or BLE_L2CAP_EVENT_COC_ACCEPT.
    chan->chan = NULL;

    chan->mtu = mtu;
    chan->rx_pending = NULL;
    chan->irq_in_progress = false;

    int err = os_mempool_init(&chan->sdu_mempool, buf_blocks, L2CAP_BUF_BLOCK_SIZE, chan->sdu_mem, "l2cap_sdu_pool");
    if (err != 0) {
        DEBUG_printf("mp_bluetooth_l2cap_connect: os_mempool_init failed %d\n", err);
        return MP_ENOMEM;
    }

    err = os_mbuf_pool_init(&chan->sdu_mbuf_pool, &chan->sdu_mempool, L2CAP_BUF_BLOCK_SIZE, buf_blocks);
    if (err != 0) {
        DEBUG_printf("mp_bluetooth_l2cap_connect: os_mbuf_pool_init failed %d\n", err);
        return MP_ENOMEM;
    }

    *out = chan;
    return 0;
}

int mp_bluetooth_l2cap_listen(uint16_t psm, uint16_t mtu) {
    DEBUG_printf("mp_bluetooth_l2cap_listen: psm=%d, mtu=%d\n", psm, mtu);

    mp_bluetooth_nimble_l2cap_channel_t *chan;
    int err = create_l2cap_channel(mtu, &chan);
    if (err != 0) {
        return err;
    }

    MP_STATE_PORT(bluetooth_nimble_root_pointers)->l2cap_listening = true;

    return ble_hs_err_to_errno(ble_l2cap_create_server(psm, mtu, &l2cap_channel_event, chan));
}

int mp_bluetooth_l2cap_connect(uint16_t conn_handle, uint16_t psm, uint16_t mtu) {
    DEBUG_printf("mp_bluetooth_l2cap_connect: conn_handle=%d, psm=%d, mtu=%d\n", conn_handle, psm, mtu);

    mp_bluetooth_nimble_l2cap_channel_t *chan;
    int err = create_l2cap_channel(mtu, &chan);
    if (err != 0) {
        return err;
    }

    struct os_mbuf *sdu_rx = os_mbuf_get_pkthdr(&chan->sdu_mbuf_pool, 0);
    assert(sdu_rx);
    return ble_hs_err_to_errno(ble_l2cap_connect(conn_handle, psm, mtu, sdu_rx, &l2cap_channel_event, chan));
}

int mp_bluetooth_l2cap_disconnect(uint16_t conn_handle, uint16_t cid) {
    DEBUG_printf("mp_bluetooth_l2cap_disconnect: conn_handle=%d, cid=%d\n", conn_handle, cid);
    mp_bluetooth_nimble_l2cap_channel_t *chan = get_l2cap_channel_for_conn_cid(conn_handle, cid);
    if (!chan) {
        return MP_EINVAL;
    }
    return ble_hs_err_to_errno(ble_l2cap_disconnect(chan->chan));
}

int mp_bluetooth_l2cap_send(uint16_t conn_handle, uint16_t cid, const uint8_t *buf, size_t len, bool *stalled) {
    DEBUG_printf("mp_bluetooth_l2cap_send: conn_handle=%d, cid=%d, len=%d\n", conn_handle, cid, (int)len);

    mp_bluetooth_nimble_l2cap_channel_t *chan = get_l2cap_channel_for_conn_cid(conn_handle, cid);
    if (!chan) {
        return MP_EINVAL;
    }

    struct ble_l2cap_chan_info info;
    ble_l2cap_get_chan_info(chan->chan, &info);
    if (len > info.peer_coc_mtu) {
        // This is verified by ble_l2cap_send anyway, but this lets us
        // avoid copying a too-large buffer into an mbuf.
        return MP_EINVAL;
    }

    if (len > (L2CAP_BUF_SIZE_MTUS_PER_CHANNEL - 1) * info.our_coc_mtu) {
        // Always ensure there's at least one local MTU of space left in the buffer
        // for the RX buffer.
        return MP_EINVAL;
    }

    // Grab an mbuf from the pool, and append the incoming buffer to it.
    struct os_mbuf *sdu_tx = os_mbuf_get_pkthdr(&chan->sdu_mbuf_pool, 0);
    if (sdu_tx == NULL) {
        return MP_ENOMEM;
    }
    int err = os_mbuf_append(sdu_tx, buf, len);
    if (err) {
        os_mbuf_free_chain(sdu_tx);
        return MP_ENOMEM;
    }

    *stalled = false;

    err = ble_l2cap_send(chan->chan, sdu_tx);
    if (err == BLE_HS_ESTALLED) {
        // Stalled means that this one will still send but any future ones
        // will fail until we receive an un-stalled event.
        DEBUG_printf("mp_bluetooth_l2cap_send: credit stall\n");
        *stalled = true;
        err = 0;
    } else {
        if (err) {
            // Anything except stalled means it won't attempt to send,
            // so free the mbuf (we're failing the op entirely).
            os_mbuf_free_chain(sdu_tx);
        }
    }

    if (os_msys_num_free() <= os_msys_count() / 2) {
        // If the sys mempool is less than half-full, then back off sending more
        // on this channel.
        DEBUG_printf("mp_bluetooth_l2cap_send: forcing mem stall: count %d, free: %d\n", os_msys_count(), os_msys_num_free());
        chan->mem_stalled = true;
        *stalled = true;
    }

    // Other error codes such as BLE_HS_EBUSY (we're stalled) or BLE_HS_EBADDATA (bigger than MTU).
    return ble_hs_err_to_errno(err);
}

int mp_bluetooth_l2cap_recvinto(uint16_t conn_handle, uint16_t cid, uint8_t *buf, size_t *len) {
    mp_bluetooth_nimble_l2cap_channel_t *chan = get_l2cap_channel_for_conn_cid(conn_handle, cid);
    if (!chan) {
        return MP_EINVAL;
    }

    MICROPY_PY_BLUETOOTH_ENTER
    if (chan->rx_pending) {
        size_t avail = OS_MBUF_PKTLEN(chan->rx_pending);

        if (buf == NULL) {
            // Can use this to implement a poll - just find out how much is available.
            *len = avail;
        } else {
            // Have dest buffer and data available.
            // Figure out how much we should copy.
            *len = min(*len, avail);

            // Extract the required number of bytes.
            os_mbuf_copydata(chan->rx_pending, 0, *len, buf);

            if (*len == avail) {
                // That's all that's available -- free this mbuf and re-enable receiving.
                os_mbuf_free_chain(chan->rx_pending);
                chan->rx_pending = NULL;

                // If we're in the call stack of the l2cap_channel_event handler, then don't
                // re-enable receiving yet (as we need to complete the rest of IRQ handler first).
                if (!chan->irq_in_progress) {
                    // We've already given the channel a new mbuf in l2cap_channel_event above, so
                    // reuse that mbuf in the call to ble_l2cap_recv_ready. This will just
                    // give the channel more credits.
                    struct os_mbuf *sdu_rx = chan->chan->coc_rx.sdu;
                    assert(sdu_rx);
                    if (sdu_rx) {
                        ble_l2cap_recv_ready(chan->chan, sdu_rx);
                    }
                }
            } else {
                // Trim the used bytes from the start of the mbuf.
                // Positive argument means "trim this many from head".
                os_mbuf_adj(chan->rx_pending, *len);
                // Clean up any empty mbufs at the head.
                chan->rx_pending = os_mbuf_trim_front(chan->rx_pending);
            }
        }
    } else {
        // No pending data.
        *len = 0;
    }

    MICROPY_PY_BLUETOOTH_EXIT
    return 0;
}

#endif // MICROPY_PY_BLUETOOTH_ENABLE_L2CAP_CHANNELS

#if MICROPY_PY_BLUETOOTH_ENABLE_HCI_CMD

int mp_bluetooth_hci_cmd(uint16_t ogf, uint16_t ocf, const uint8_t *req, size_t req_len, uint8_t *resp, size_t resp_len, uint8_t *status) {
    int rc = ble_hs_hci_cmd_tx(BLE_HCI_OP(ogf, ocf), req, req_len, resp, resp_len);
    if (rc < BLE_HS_ERR_HCI_BASE || rc >= BLE_HS_ERR_HCI_BASE + 0x100) {
        // The controller didn't handle the command (e.g. HCI timeout).
        return ble_hs_err_to_errno(rc);
    } else {
        // The command executed, but had an error (i.e. invalid parameter).
        *status = rc - BLE_HS_ERR_HCI_BASE;
        return 0;
    }
}

#endif // MICROPY_PY_BLUETOOTH_ENABLE_HCI_CMD

#if MICROPY_PY_BLUETOOTH_ENABLE_PAIRING_BONDING

static int ble_secret_store_read(int obj_type, const union ble_store_key *key, union ble_store_value *value) {
    DEBUG_printf("ble_secret_store_read: %d\n", obj_type);
    const uint8_t *key_data;
    size_t key_data_len;

    switch (obj_type) {
        case BLE_STORE_OBJ_TYPE_PEER_SEC: {
            if (ble_addr_cmp(&key->sec.peer_addr, BLE_ADDR_ANY)) {
                // <type=peer,addr,*> (single)
                // Find the entry for this specific peer.
                assert(key->sec.idx == 0);
                assert(!key->sec.ediv_rand_present);
                key_data = (const uint8_t *)&key->sec.peer_addr;
                key_data_len = sizeof(ble_addr_t);
            } else {
                // <type=peer,*> (with index)
                // Iterate all known peers.
                assert(!key->sec.ediv_rand_present);
                key_data = NULL;
                key_data_len = 0;
            }
            break;
        }
        case BLE_STORE_OBJ_TYPE_OUR_SEC: {
            // <type=our,addr,ediv_rand>
            // Find our secret for this remote device, matching this ediv/rand key.
            assert(ble_addr_cmp(&key->sec.peer_addr, BLE_ADDR_ANY)); // Must have address.
            assert(key->sec.idx == 0);
            assert(key->sec.ediv_rand_present);
            key_data = (const uint8_t *)&key->sec.peer_addr;
            key_data_len = sizeof(ble_addr_t);
            break;
        }
        case BLE_STORE_OBJ_TYPE_CCCD: {
            // TODO: Implement CCCD persistence.
            DEBUG_printf("ble_secret_store_read: CCCD not supported.\n");
            return -1;
        }
        default:
            return BLE_HS_ENOTSUP;
    }

    const uint8_t *value_data;
    size_t value_data_len;
    if (!mp_bluetooth_gap_on_get_secret(obj_type, key->sec.idx, key_data, key_data_len, &value_data, &value_data_len)) {
        DEBUG_printf("ble_secret_store_read: Key not found: type=%d, index=%u, key=0x%p, len=" UINT_FMT "\n", obj_type, key->sec.idx, key_data, key_data_len);
        return BLE_HS_ENOENT;
    }

    if (value_data_len != sizeof(struct ble_store_value_sec)) {
        DEBUG_printf("ble_secret_store_read: Invalid key data: actual=" UINT_FMT " expected=" UINT_FMT "\n", value_data_len, sizeof(struct ble_store_value_sec));
        return BLE_HS_ENOENT;
    }

    memcpy((uint8_t *)&value->sec, value_data, sizeof(struct ble_store_value_sec));

    DEBUG_printf("ble_secret_store_read: found secret\n");

    if (obj_type == BLE_STORE_OBJ_TYPE_OUR_SEC) {
        // TODO: Verify ediv_rand matches.
    }

    return 0;
}

static int ble_secret_store_write(int obj_type, const union ble_store_value *val) {
    DEBUG_printf("ble_secret_store_write: %d\n", obj_type);
    switch (obj_type) {
        case BLE_STORE_OBJ_TYPE_PEER_SEC:
        case BLE_STORE_OBJ_TYPE_OUR_SEC: {
            // <type=peer,addr,edivrand>

            struct ble_store_key_sec key_sec;
            const struct ble_store_value_sec *value_sec = &val->sec;
            ble_store_key_from_value_sec(&key_sec, value_sec);

            assert(ble_addr_cmp(&key_sec.peer_addr, BLE_ADDR_ANY)); // Must have address.
            assert(key_sec.ediv_rand_present);

            if (!mp_bluetooth_gap_on_set_secret(obj_type, (const uint8_t *)&key_sec.peer_addr, sizeof(ble_addr_t), (const uint8_t *)value_sec, sizeof(struct ble_store_value_sec))) {
                DEBUG_printf("Failed to write key: type=%d\n", obj_type);
                return BLE_HS_ESTORE_CAP;
            }

            DEBUG_printf("ble_secret_store_write: wrote secret\n");

            return 0;
        }
        case BLE_STORE_OBJ_TYPE_CCCD: {
            // TODO: Implement CCCD persistence.
            DEBUG_printf("ble_secret_store_write: CCCD not supported.\n");
            // Just pretend we wrote it.
            return 0;
        }
        default:
            return BLE_HS_ENOTSUP;
    }
}

static int ble_secret_store_delete(int obj_type, const union ble_store_key *key) {
    DEBUG_printf("ble_secret_store_delete: %d\n", obj_type);
    switch (obj_type) {
        case BLE_STORE_OBJ_TYPE_PEER_SEC:
        case BLE_STORE_OBJ_TYPE_OUR_SEC: {
            // <type=peer,addr,*>

            assert(ble_addr_cmp(&key->sec.peer_addr, BLE_ADDR_ANY)); // Must have address.
            // ediv_rand is optional (will not be present for delete).

            if (!mp_bluetooth_gap_on_set_secret(obj_type, (const uint8_t *)&key->sec.peer_addr, sizeof(ble_addr_t), NULL, 0)) {
                DEBUG_printf("Failed to delete key: type=%d\n", obj_type);
                return BLE_HS_ENOENT;
            }

            DEBUG_printf("ble_secret_store_delete: deleted secret\n");

            return 0;
        }
        case BLE_STORE_OBJ_TYPE_CCCD: {
            // TODO: Implement CCCD persistence.
            DEBUG_printf("ble_secret_store_delete: CCCD not supported.\n");
            // Just pretend it wasn't there.
            return BLE_HS_ENOENT;
        }
        default:
            return BLE_HS_ENOTSUP;
    }
}

#endif // MICROPY_PY_BLUETOOTH_ENABLE_PAIRING_BONDING

#if !MICROPY_BLUETOOTH_NIMBLE_BINDINGS_ONLY
MP_REGISTER_ROOT_POINTER(struct _mp_bluetooth_nimble_malloc_t *bluetooth_nimble_memory);
#endif
MP_REGISTER_ROOT_POINTER(struct _mp_bluetooth_nimble_root_pointers_t *bluetooth_nimble_root_pointers);

#endif // MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_NIMBLE
