/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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

#include "extmod/modbluetooth.h"

#include "host/ble_hs.h"
#include "host/util/util.h"
#include "nimble/ble.h"
#include "nimble/nimble_port.h"
#include "services/gap/ble_svc_gap.h"
#include "transport/uart/ble_hci_uart.h"

STATIC int8_t ble_hs_err_to_errno_table[] = {
    [BLE_HS_EAGAIN]             = MP_EAGAIN,
    [BLE_HS_EALREADY]           = MP_EALREADY,
    [BLE_HS_EINVAL]             = MP_EINVAL,
    [BLE_HS_EMSGSIZE]           = MP_EIO,
    [BLE_HS_ENOENT]             = MP_ENOENT,
    [BLE_HS_ENOMEM]             = MP_ENOMEM,
    [BLE_HS_ENOTCONN]           = MP_ENOTCONN,
    [BLE_HS_ENOTSUP]            = MP_EOPNOTSUPP,
    [BLE_HS_EAPP]               = MP_EIO,
    [BLE_HS_EBADDATA]           = MP_EIO,
    [BLE_HS_EOS]                = MP_EIO,
    [BLE_HS_ECONTROLLER]        = MP_EIO,
    [BLE_HS_ETIMEOUT]           = MP_ETIMEDOUT,
    [BLE_HS_EDONE]              = MP_EIO,
    [BLE_HS_EBUSY]              = MP_EBUSY,
    [BLE_HS_EREJECT]            = MP_EIO,
    [BLE_HS_EUNKNOWN]           = MP_EIO,
    [BLE_HS_EROLE]              = MP_EIO,
    [BLE_HS_ETIMEOUT_HCI]       = MP_EIO,
    [BLE_HS_ENOMEM_EVT]         = MP_EIO,
    [BLE_HS_ENOADDR]            = MP_EIO,
    [BLE_HS_ENOTSYNCED]         = MP_EIO,
    [BLE_HS_EAUTHEN]            = MP_EIO,
    [BLE_HS_EAUTHOR]            = MP_EIO,
    [BLE_HS_EENCRYPT]           = MP_EIO,
    [BLE_HS_EENCRYPT_KEY_SZ]    = MP_EIO,
    [BLE_HS_ESTORE_CAP]         = MP_EIO,
    [BLE_HS_ESTORE_FAIL]        = MP_EIO,
    [BLE_HS_EPREEMPTED]         = MP_EIO,
    [BLE_HS_EDISABLED]          = MP_EIO,
};

STATIC int ble_hs_err_to_errno(int err) {
    if (0 <= err && err < MP_ARRAY_SIZE(ble_hs_err_to_errno_table)) {
        return ble_hs_err_to_errno_table[err];
    } else {
        return MP_EIO;
    }
}

/******************************************************************************/
// RUN LOOP

enum {
    BLE_STATE_OFF,
    BLE_STATE_STARTING,
    BLE_STATE_ACTIVE,
};

static volatile int ble_state = BLE_STATE_OFF;

extern void nimble_uart_process(void);
extern void os_eventq_run_all(void);
extern void os_callout_process(void);

// hook for network poller to run this periodically
void nimble_poll(void) {
    if (ble_state == BLE_STATE_OFF) {
        return;
    }

    nimble_uart_process();
    os_callout_process();
    os_eventq_run_all();
}

/******************************************************************************/
// BINDINGS

STATIC uint16_t active_conn_handle[MP_BT_MAX_CONNECTED_DEVICES];

STATIC void reset_cb(int reason) {
    (void)reason;
}

STATIC void sync_cb(void) {
    ble_hs_util_ensure_addr(0); // prefer public address
    ble_svc_gap_device_name_set("PYBD");

    ble_state = BLE_STATE_ACTIVE;
}

STATIC void gatts_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg) {
    switch (ctxt->op) {
        case BLE_GATT_REGISTER_OP_SVC:
            printf("gatts_register_cb: svc uuid=%p handle=%d\n", &ctxt->svc.svc_def->uuid, ctxt->svc.handle);
            break;

        case BLE_GATT_REGISTER_OP_CHR:
            printf("gatts_register_cb: chr uuid=%p def_handle=%d val_handle=%d\n", &ctxt->chr.chr_def->uuid, ctxt->chr.def_handle, ctxt->chr.val_handle);
            break;

        case BLE_GATT_REGISTER_OP_DSC:
            printf("gatts_register_cb: dsc uuid=%p handle=%d\n", &ctxt->dsc.dsc_def->uuid, ctxt->dsc.handle);
            break;
    }
}

STATIC int gap_event_cb(struct ble_gap_event *event, void *arg) {
    printf("gap_event_cb: type=%d\n", event->type);

    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            printf("  connect: handle=%u\n", event->connect.conn_handle);
            if (event->connect.status == 0) {
                // Connection established
                for (size_t i = 0; i < MP_BT_MAX_CONNECTED_DEVICES; ++i) {
                    if (active_conn_handle[i] == MP_BT_INVALID_CONN_HANDLE) {
                        active_conn_handle[i] = event->connect.conn_handle;
                        break;
                    }
                }
            } else {
                // Connection failed
                for (size_t i = 0; i < MP_BT_MAX_CONNECTED_DEVICES; ++i) {
                    if (active_conn_handle[i] == event->connect.conn_handle) {
                        active_conn_handle[i] = MP_BT_INVALID_CONN_HANDLE;
                        break;
                    }
                }
            }
            break;

        case BLE_GAP_EVENT_DISCONNECT:
            printf("  disconnect: handle=%u\n", event->disconnect.conn.conn_handle);
            for (size_t i = 0; i < MP_BT_MAX_CONNECTED_DEVICES; ++i) {
                if (active_conn_handle[i] == event->disconnect.conn.conn_handle) {
                    active_conn_handle[i] = MP_BT_INVALID_CONN_HANDLE;
                    break;
                }
            }
            break;
    }

    return 0;
}

int mp_bt_enable(void) {
    if (ble_state != BLE_STATE_OFF) {
        return 0;
    }

    for (size_t i = 0; i < MP_BT_MAX_CONNECTED_DEVICES; ++i) {
        active_conn_handle[i] = MP_BT_INVALID_CONN_HANDLE;
    }

    ble_state = BLE_STATE_STARTING;

    ble_hs_cfg.reset_cb = reset_cb;
    ble_hs_cfg.sync_cb = sync_cb;
    ble_hs_cfg.gatts_register_cb = gatts_register_cb;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    ble_hci_uart_init();
    nimble_port_init();
    ble_hs_sched_start();

    // Wait for sync callback
    while (ble_state != BLE_STATE_ACTIVE) {
        MICROPY_EVENT_POLL_HOOK
    }

    return 0;
}

void mp_bt_disable(void) {
    ble_state = BLE_STATE_OFF;
    mp_hal_pin_low(pyb_pin_BT_REG_ON);
}

bool mp_bt_is_enabled(void) {
    return ble_state == BLE_STATE_ACTIVE;
}

void mp_bt_get_address(uint8_t *address) {
    mp_hal_get_mac(MP_HAL_MAC_BDADDR, address);
    // TODO: need to convert MSB/LSB?
}

int mp_bt_advertise_start(mp_bt_adv_type_t type, uint16_t interval, const uint8_t *adv_data, size_t adv_data_len, const uint8_t *sr_data, size_t sr_data_len) {
    int ret;

    mp_bt_advertise_stop();

    if (adv_data != NULL) {
        ret = ble_gap_adv_set_data(adv_data, adv_data_len);
        if (ret != 0) {
            printf("ble_gap_adv_set_data: fail with %u\n", ret);
            return ble_hs_err_to_errno(ret);
        }
    }

    if (sr_data != NULL) {
        ret = ble_gap_adv_rsp_set_data(sr_data, sr_data_len);
        if (ret != 0) {
            printf("ble_gap_adv_rsp_set_data: fail with %u\n", ret);
            return ble_hs_err_to_errno(ret);
        }
    }

    struct ble_gap_adv_params adv_params = {
        .conn_mode = type,
        .disc_mode = BLE_GAP_DISC_MODE_GEN,
        .itvl_min = interval,
        .itvl_max = interval,
        .channel_map = 7, // all 3 channels
    };

    ret = ble_gap_adv_start(BLE_OWN_ADDR_PUBLIC, NULL, BLE_HS_FOREVER, &adv_params, gap_event_cb, NULL);
    if (ret != 0) {
        printf("ble_gap_adv_start: fail with %u\n", ret);
        return ble_hs_err_to_errno(ret);
    }

    return 0;
}

void mp_bt_advertise_stop(void) {
    if (ble_gap_adv_active()) {
        ble_gap_adv_stop();
    }
}

static int chr_access_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    printf("chr_access_cb: conn_handle=%u attr_handle=%u\n", conn_handle, attr_handle);
    if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
        for (struct os_mbuf *om = ctxt->om; om; om = SLIST_NEXT(om, om_next)) {
            printf("write_chr: data='%.*s'\n", om->om_len, om->om_data);
        }
        return 0;
    }
    return BLE_ATT_ERR_UNLIKELY;
}

int mp_bt_add_service(mp_bt_service_t *service, size_t num_characteristics, mp_bt_characteristic_t **characteristics) {
    void *svc_data = m_malloc(2 * sizeof(struct ble_gatt_svc_def) + (num_characteristics + 1) * sizeof(struct ble_gatt_chr_def));

    struct ble_gatt_chr_def *chr = (void*)((uint8_t*)svc_data + 2 * sizeof(struct ble_gatt_svc_def));
    for (size_t i = 0; i < num_characteristics; ++i) {
        chr[i].uuid = &characteristics[i]->uuid.u;
        chr[i].access_cb = chr_access_cb;
        chr[i].arg = NULL;
        chr[i].descriptors = NULL;
        chr[i].flags = characteristics[i]->flags;
        chr[i].min_key_size = 0;
        chr[i].val_handle = &characteristics[i]->value_handle;
        characteristics[i]->value_handle = 0xffff; // will be set at registration time
    }
    chr[num_characteristics].uuid = NULL; // no more characteristic

    struct ble_gatt_svc_def *svc = svc_data;
    svc[0].type = BLE_GATT_SVC_TYPE_PRIMARY;
    svc[0].uuid = &service->uuid.u;
    svc[0].includes = NULL;
    svc[0].characteristics = chr;
    svc[1].type = 0; // no more services
    service->handle = svc_data; // to prevent GC

    // Note: advertising must be stopped for gatts registration to work

    int ret;

    ret = ble_gatts_reset();
    if (ret != 0) {
        printf("ble_gatts_reset: fail with %d\n", ret);
        return ble_hs_err_to_errno(ret);
    }

    ret = ble_gatts_count_cfg(svc);
    if (ret != 0) {
        printf("ble_gatts_count_cfg: fail with %d\n", ret);
        return ble_hs_err_to_errno(ret);
    }

    ret = ble_gatts_add_svcs(svc);
    if (ret != 0) {
        printf("ble_gatts_add_svcs: fail with %d\n", ret);
        return ble_hs_err_to_errno(ret);
    }

    ret = ble_gatts_start();
    if (ret != 0) {
        printf("ble_gatts_start: fail with %d\n", ret);
        return ble_hs_err_to_errno(ret);
    }

    return 0;
}

int mp_bt_characteristic_value_set(mp_bt_characteristic_t *characteristic, const void *value, size_t value_len) {
    printf("mp_bt_characteristic_value_set: value_handle=%u\n", characteristic->value_handle);
    //ENTER();
    for (size_t conn = 0; conn < MP_BT_MAX_CONNECTED_DEVICES; ++conn) {
        size_t len = value_len;
        const uint8_t *buf = value;
        while (len && active_conn_handle[conn] != MP_BT_INVALID_CONN_HANDLE) {
            size_t l = len < 20 ? len : 20;
            struct os_mbuf *om = ble_hs_mbuf_from_flat(buf, l);
            if (om == NULL) {
                break;
            }
            ble_gattc_notify_custom(active_conn_handle[conn], characteristic->value_handle, om);
            len -= l;
            buf += l;
            if (len) {
                //EXIT();
                mp_hal_delay_ms(100);
                //REENTER();
            }
        }
    }
    //EXIT();
    return 0;
}

int mp_bt_characteristic_value_notify(mp_bt_characteristic_t *characteristic, uint16_t conn_handle, const void *value, size_t value_len) {
    printf("mp_bt_characteristic_value_notify\n");
    return 0;
}

int mp_bt_characteristic_value_get(mp_bt_characteristic_t *characteristic, void *value, size_t *value_len) {
    printf("mp_bt_characteristic_value_get\n");
    return 0;
}

int mp_bt_device_disconnect(uint16_t conn_handle) {
    printf("mp_bt_device_disconnect\n");
    return 0;
}

void mp_bt_parse_uuid(mp_obj_t obj, mp_bt_uuid_t *uuid) {
    if (mp_obj_is_int(obj)) {
        mp_uint_t value = mp_obj_get_int_truncated(obj);
        if (value <= 0xffff) {
            uuid->u.type = BLE_UUID_TYPE_16;
            uuid->u16.value = value;
        } else {
            uuid->u.type = BLE_UUID_TYPE_32;
            uuid->u32.value = value;
        }
    } else if (mp_obj_is_str(obj)) {
        uuid->u.type = BLE_UUID_TYPE_128;
        mp_bt_parse_uuid_str(obj, &uuid->u128.value[0]);
    } else {
        mp_raise_ValueError("can't parse UUID");
    }
}

mp_obj_t mp_bt_format_uuid(mp_bt_uuid_t *uuid) {
    switch (uuid->u.type) {
        case BLE_UUID_TYPE_16:
            return MP_OBJ_NEW_SMALL_INT(uuid->u16.value);
        case BLE_UUID_TYPE_32:
            return mp_obj_new_int(uuid->u32.value);
        case BLE_UUID_TYPE_128:
            return mp_bt_format_uuid_str(uuid->u128.value);
        default:
            return mp_const_none;
    }
}

#endif // MICROPY_PY_BLUETOOTH
