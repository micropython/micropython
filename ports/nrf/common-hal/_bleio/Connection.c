/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
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

#include "shared-bindings/_bleio/Connection.h"

#include <string.h>
#include <stdio.h>

#include "ble.h"
#include "ble_drv.h"
#include "ble_hci.h"
#include "nrf_soc.h"
#include "lib/utils/interrupt_char.h"
#include "py/gc.h"
#include "py/objlist.h"
#include "py/objstr.h"
#include "py/qstr.h"
#include "py/runtime.h"
#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Adapter.h"
#include "shared-bindings/_bleio/Attribute.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/UUID.h"
#include "supervisor/shared/tick.h"

#include "common-hal/_bleio/bonding.h"

#define BLE_ADV_LENGTH_FIELD_SIZE   1
#define BLE_ADV_AD_TYPE_FIELD_SIZE  1
#define BLE_AD_TYPE_FLAGS_DATA_SIZE 1

static const ble_gap_sec_params_t pairing_sec_params = {
    .bond = 1,
    .mitm = 0,
    .lesc = 0,
    .keypress = 0,
    .oob = 0,
    .io_caps = BLE_GAP_IO_CAPS_NONE,
    .min_key_size = 7,
    .max_key_size = 16,
    .kdist_own = { .enc = 1, .id = 1},
    .kdist_peer = { .enc = 1, .id = 1},
};

#define CONNECTION_DEBUG (1)
#if CONNECTION_DEBUG
  #define CONNECTION_DEBUG_PRINTF(...) printf(__VA_ARGS__)
#else
  #define CONNECTION_DEBUG_PRINTF(...)
#endif

static volatile bool m_discovery_in_process;
static volatile bool m_discovery_successful;

static bleio_service_obj_t *m_char_discovery_service;
static bleio_characteristic_obj_t *m_desc_discovery_characteristic;

bool connection_on_ble_evt(ble_evt_t *ble_evt, void *self_in) {
    bleio_connection_internal_t *self = (bleio_connection_internal_t *)self_in;

    if (BLE_GAP_EVT_BASE <= ble_evt->header.evt_id && ble_evt->header.evt_id <= BLE_GAP_EVT_LAST &&
        ble_evt->evt.gap_evt.conn_handle != self->conn_handle) {
        return false;
    }
    if (BLE_GATTS_EVT_BASE <= ble_evt->header.evt_id && ble_evt->header.evt_id <= BLE_GATTS_EVT_LAST &&
        ble_evt->evt.gatts_evt.conn_handle != self->conn_handle) {
        return false;
    }

    switch (ble_evt->header.evt_id) {
        case BLE_GAP_EVT_DISCONNECTED:
            // Adapter.c does the work for this event.
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST: {
            ble_gap_phys_t const phys = {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            sd_ble_gap_phy_update(ble_evt->evt.gap_evt.conn_handle, &phys);
            break;
        }

        case BLE_GAP_EVT_PHY_UPDATE: { // 0x22
            break;
        }

        case BLE_GAP_EVT_DATA_LENGTH_UPDATE_REQUEST:
            // SoftDevice will respond to a length update request.
            sd_ble_gap_data_length_update(self->conn_handle, NULL, NULL);
            break;

        case BLE_GAP_EVT_DATA_LENGTH_UPDATE: { // 0x24
            break;
        }

        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST: {
            ble_gatts_evt_exchange_mtu_request_t *request =
                &ble_evt->evt.gatts_evt.params.exchange_mtu_request;

            uint16_t new_mtu = BLE_GATTS_VAR_ATTR_LEN_MAX;
            if (request->client_rx_mtu < new_mtu) {
                new_mtu = request->client_rx_mtu;
            }
            if (new_mtu < BLE_GATT_ATT_MTU_DEFAULT) {
                new_mtu = BLE_GATT_ATT_MTU_DEFAULT;
            }
            if (self->mtu > 0) {
                new_mtu = self->mtu;
            }

            self->mtu = new_mtu;
            sd_ble_gatts_exchange_mtu_reply(self->conn_handle, new_mtu);
            break;
        }


        case BLE_GATTC_EVT_EXCHANGE_MTU_RSP: {
            ble_gattc_evt_exchange_mtu_rsp_t *response =
                &ble_evt->evt.gattc_evt.params.exchange_mtu_rsp;

            self->mtu = response->server_rx_mtu;
            break;
        }

        case BLE_GATTS_EVT_WRITE:
            // A client wrote a value.
            // If we are bonded and it's a CCCD (UUID 0x2902), store the CCCD value.
            if (self->conn_handle != BLE_CONN_HANDLE_INVALID &&
                self->pair_status == PAIR_PAIRED &&
                ble_evt->evt.gatts_evt.params.write.uuid.type == BLE_UUID_TYPE_BLE &&
                ble_evt->evt.gatts_evt.params.write.uuid.uuid == 0x2902) {
                //
                // Save sys_attr data (CCCD state) in bonding area at
                // next opportunity, but also remember time of this
                // request, so we can consolidate closely-spaced requests.
                self->do_bond_cccds = true;
                self->do_bond_cccds_request_time = supervisor_ticks_ms64();
            }
            // Return false so other handlers get this event as well.
            return false;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            sd_ble_gatts_sys_attr_set(self->conn_handle, NULL, 0, 0);
            break;

            #if CIRCUITPY_VERBOSE_BLE
        // Use read authorization to snoop on all reads when doing verbose debugging.
        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST: {

            ble_gatts_evt_rw_authorize_request_t *request =
                &ble_evt->evt.gatts_evt.params.authorize_request;

            mp_printf(&mp_plat_print, "Read %x offset %d ", request->request.read.handle, request->request.read.offset);
            uint8_t value_bytes[22];
            ble_gatts_value_t value;
            value.offset = request->request.read.offset;
            value.len = 22;
            value.p_value = value_bytes;

            sd_ble_gatts_value_get(self->conn_handle, request->request.read.handle, &value);
            size_t len = value.len;
            if (len > 22) {
                len = 22;
            }
            for (uint8_t i = 0; i < len; i++) {
                mp_printf(&mp_plat_print, " %02x", value_bytes[i]);
            }
            mp_printf(&mp_plat_print, "\n");
            ble_gatts_rw_authorize_reply_params_t reply;
            reply.type = request->type;
            reply.params.read.gatt_status = BLE_GATT_STATUS_SUCCESS;
            reply.params.read.update = false;
            reply.params.read.offset = request->request.read.offset;
            sd_ble_gatts_rw_authorize_reply(self->conn_handle, &reply);
            break;
        }
            #endif

        case BLE_GATTS_EVT_HVN_TX_COMPLETE: // Capture this for now. 0x55
            break;
        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST: {
            self->conn_params_updating = true;
            ble_gap_evt_conn_param_update_request_t *request =
                &ble_evt->evt.gap_evt.params.conn_param_update_request;
            sd_ble_gap_conn_param_update(self->conn_handle, &request->conn_params);
            break;
        }
        case BLE_GAP_EVT_CONN_PARAM_UPDATE: { // 0x12
            ble_gap_evt_conn_param_update_t *result =
                &ble_evt->evt.gap_evt.params.conn_param_update;

            #if CIRCUITPY_VERBOSE_BLE
            ble_gap_conn_params_t *cp = &ble_evt->evt.gap_evt.params.conn_param_update.conn_params;
            mp_printf(&mp_plat_print, "conn params updated: min_ci %d max_ci %d s_l %d sup_timeout %d\n", cp->min_conn_interval, cp->max_conn_interval, cp->slave_latency, cp->conn_sup_timeout);
            #endif

            memcpy(&self->conn_params, &result->conn_params, sizeof(ble_gap_conn_params_t));
            self->conn_params_updating = false;
            break;
        }
        case BLE_GAP_EVT_SEC_PARAMS_REQUEST: {
            // First time pairing.
            // 1. Either we or peer initiate the process
            // 2. Peer asks for security parameters using BLE_GAP_EVT_SEC_PARAMS_REQUEST.
            // 3. Pair Key exchange ("just works" implemented now; TODO: out-of-band key pairing)
            // 4. Connection is secured: BLE_GAP_EVT_CONN_SEC_UPDATE
            // 5. Long-term Keys exchanged: BLE_GAP_EVT_AUTH_STATUS

            bonding_clear_keys(&self->bonding_keys);
            self->ediv = EDIV_INVALID;
            ble_gap_sec_keyset_t keyset = {
                .keys_own = {
                    .p_enc_key = &self->bonding_keys.own_enc,
                    .p_id_key = NULL,
                    .p_sign_key = NULL,
                    .p_pk = NULL
                },

                .keys_peer = {
                    .p_enc_key = &self->bonding_keys.peer_enc,
                    .p_id_key = &self->bonding_keys.peer_id,
                    .p_sign_key = NULL,
                    .p_pk = NULL
                }
            };

            sd_ble_gap_sec_params_reply(self->conn_handle, BLE_GAP_SEC_STATUS_SUCCESS,
                self->is_central ? NULL : &pairing_sec_params,
                &keyset);
            break;
        }

        case BLE_GAP_EVT_LESC_DHKEY_REQUEST:
            // TODO for LESC pairing:
            // sd_ble_gap_lesc_dhkey_reply(...);
            break;

        case BLE_GAP_EVT_AUTH_STATUS: { // 0x19
            // Key exchange completed.
            ble_gap_evt_auth_status_t *status = &ble_evt->evt.gap_evt.params.auth_status;
            self->sec_status = status->auth_status;
            if (status->auth_status == BLE_GAP_SEC_STATUS_SUCCESS) {
                self->ediv = self->bonding_keys.own_enc.master_id.ediv;
                self->pair_status = PAIR_PAIRED;
                // Save keys in bonding area at next opportunity.
                self->do_bond_keys = true;
            } else {
                // Inform busy-waiter pairing has failed.
                self->pair_status = PAIR_NOT_PAIRED;
            }
            break;
        }

        case BLE_GAP_EVT_SEC_INFO_REQUEST: { // 0x14
            // Peer asks for the stored keys.
            // - load key and return if bonded previously.
            // - Else return NULL --> Initiate key exchange
            ble_gap_evt_sec_info_request_t *sec_info_request = &ble_evt->evt.gap_evt.params.sec_info_request;
            (void)sec_info_request;
            if (bonding_load_keys(self->is_central, sec_info_request->master_id.ediv, &self->bonding_keys)) {
                sd_ble_gap_sec_info_reply(
                    self->conn_handle,
                    &self->bonding_keys.own_enc.enc_info,
                    &self->bonding_keys.peer_id.id_info,
                    NULL);
                self->ediv = self->bonding_keys.own_enc.master_id.ediv;
            } else {
                // We don't have stored keys. Ask for keys.
                sd_ble_gap_sec_info_reply(self->conn_handle, NULL, NULL, NULL);
            }
            break;
        }

        case BLE_GAP_EVT_CONN_SEC_UPDATE: { // 0x1a
            // We get this both on first-time pairing and on subsequent pairings using stored keys.
            ble_gap_conn_sec_t *conn_sec = &ble_evt->evt.gap_evt.params.conn_sec_update.conn_sec;
            if (conn_sec->sec_mode.sm <= 1 && conn_sec->sec_mode.lv <= 1) {
                // Security setup did not succeed:
                // mode 0, level 0 means no access
                // mode 1, level 1 means open link
                // mode >=1 and/or level >=1 means encryption is set up
                self->pair_status = PAIR_NOT_PAIRED;
            } else {
                if (bonding_load_cccd_info(self->is_central, self->conn_handle, self->ediv)) {
                    // Did an sd_ble_gatts_sys_attr_set() with the stored sys_attr values.
                    // Indicate ATTR table change because we may have reloaded since the peer last
                    // connected.
                    sd_ble_gatts_service_changed(self->conn_handle, 0xC, 0xFFFF);
                } else {
                    // No matching bonding found, so use fresh system attributes.
                    sd_ble_gatts_sys_attr_set(self->conn_handle, NULL, 0, 0);
                }
                self->pair_status = PAIR_PAIRED;
            }
            break;
        }

        default:
            return false;
    }
    return true;
}

void bleio_connection_clear(bleio_connection_internal_t *self) {
    self->remote_service_list = NULL;
    self->conn_handle = BLE_CONN_HANDLE_INVALID;
    self->pair_status = PAIR_NOT_PAIRED;
    self->is_central = false;
    bonding_clear_keys(&self->bonding_keys);
}

bool common_hal_bleio_connection_get_paired(bleio_connection_obj_t *self) {
    if (self->connection == NULL) {
        return false;
    }
    return self->connection->pair_status == PAIR_PAIRED;
}

bool common_hal_bleio_connection_get_connected(bleio_connection_obj_t *self) {
    if (self->connection == NULL) {
        return false;
    }
    return self->connection->conn_handle != BLE_CONN_HANDLE_INVALID;
}

void common_hal_bleio_connection_disconnect(bleio_connection_internal_t *self) {
    sd_ble_gap_disconnect(self->conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
}

void common_hal_bleio_connection_pair(bleio_connection_internal_t *self, bool bond) {
    // We may already be trying to pair if we just reconnected to a peer we're
    // bonded with.
    while (self->pair_status == PAIR_WAITING && !mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
    }
    if (self->pair_status == PAIR_PAIRED) {
        return;
    }
    self->pair_status = PAIR_WAITING;

    check_nrf_error(sd_ble_gap_authenticate(self->conn_handle, &pairing_sec_params));

    while (self->pair_status == PAIR_WAITING && !mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
    }
    if (mp_hal_is_interrupted()) {
        return;
    }
    check_sec_status(self->sec_status);
}

mp_float_t common_hal_bleio_connection_get_connection_interval(bleio_connection_internal_t *self) {
    while (self->conn_params_updating && !mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
    }
    return 1.25f * self->conn_params.min_conn_interval;
}

// Return the current negotiated MTU length, minus overhead.
mp_int_t common_hal_bleio_connection_get_max_packet_length(bleio_connection_internal_t *self) {
    return (self->mtu == 0 ? BLE_GATT_ATT_MTU_DEFAULT : self->mtu) - 3;
}

void common_hal_bleio_connection_set_connection_interval(bleio_connection_internal_t *self, mp_float_t new_interval) {
    self->conn_params_updating = true;
    uint16_t interval = new_interval / 1.25f;
    self->conn_params.min_conn_interval = interval;
    self->conn_params.max_conn_interval = interval;
    uint32_t status = NRF_ERROR_BUSY;
    while (status == NRF_ERROR_BUSY) {
        status = sd_ble_gap_conn_param_update(self->conn_handle, &self->conn_params);
        RUN_BACKGROUND_TASKS;
    }
    check_nrf_error(status);
}

// service_uuid may be NULL, to discover all services.
STATIC bool discover_next_services(bleio_connection_internal_t *connection, uint16_t start_handle, ble_uuid_t *service_uuid) {
    m_discovery_successful = false;
    m_discovery_in_process = true;

    uint32_t nrf_err = NRF_ERROR_BUSY;
    while (nrf_err == NRF_ERROR_BUSY) {
        nrf_err = sd_ble_gattc_primary_services_discover(connection->conn_handle, start_handle, service_uuid);
    }
    check_nrf_error(nrf_err);

    // Wait for a discovery event.
    while (m_discovery_in_process) {
        MICROPY_VM_HOOK_LOOP;
    }
    return m_discovery_successful;
}

STATIC bool discover_next_characteristics(bleio_connection_internal_t *connection, bleio_service_obj_t *service, uint16_t start_handle) {
    m_char_discovery_service = service;

    ble_gattc_handle_range_t handle_range;
    handle_range.start_handle = start_handle;
    handle_range.end_handle = service->end_handle;

    m_discovery_successful = false;
    m_discovery_in_process = true;

    uint32_t err_code = sd_ble_gattc_characteristics_discover(connection->conn_handle, &handle_range);
    if (err_code != NRF_SUCCESS) {
        return false;
    }

    // Wait for a discovery event.
    while (m_discovery_in_process) {
        MICROPY_VM_HOOK_LOOP;
    }
    return m_discovery_successful;
}

STATIC bool discover_next_descriptors(bleio_connection_internal_t *connection, bleio_characteristic_obj_t *characteristic, uint16_t start_handle, uint16_t end_handle) {
    m_desc_discovery_characteristic = characteristic;

    ble_gattc_handle_range_t handle_range;
    handle_range.start_handle = start_handle;
    handle_range.end_handle = end_handle;

    m_discovery_successful = false;
    m_discovery_in_process = true;

    uint32_t err_code = sd_ble_gattc_descriptors_discover(connection->conn_handle, &handle_range);
    if (err_code != NRF_SUCCESS) {
        return false;
    }

    // Wait for a discovery event.
    while (m_discovery_in_process) {
        MICROPY_VM_HOOK_LOOP;
    }
    return m_discovery_successful;
}

STATIC void on_primary_srv_discovery_rsp(ble_gattc_evt_prim_srvc_disc_rsp_t *response, bleio_connection_internal_t *connection) {
    for (size_t i = 0; i < response->count; ++i) {
        ble_gattc_service_t *gattc_service = &response->services[i];

        bleio_service_obj_t *service = m_new_obj(bleio_service_obj_t);
        service->base.type = &bleio_service_type;

        // Initialize several fields at once.
        bleio_service_from_connection(service, bleio_connection_new_from_internal(connection));

        service->is_remote = true;
        service->start_handle = gattc_service->handle_range.start_handle;
        service->end_handle = gattc_service->handle_range.end_handle;
        service->handle = gattc_service->handle_range.start_handle;

        if (gattc_service->uuid.type != BLE_UUID_TYPE_UNKNOWN) {
            // Known service UUID.
            bleio_uuid_obj_t *uuid = m_new_obj(bleio_uuid_obj_t);
            uuid->base.type = &bleio_uuid_type;
            bleio_uuid_construct_from_nrf_ble_uuid(uuid, &gattc_service->uuid);
            service->uuid = uuid;
        } else {
            // The discovery response contained a 128-bit UUID that has not yet been registered with the
            // softdevice via sd_ble_uuid_vs_add(). We need to fetch the 128-bit value and register it.
            // For now, just set the UUID to NULL.
            service->uuid = NULL;
        }

        mp_obj_list_append(MP_OBJ_FROM_PTR(connection->remote_service_list),
            MP_OBJ_FROM_PTR(service));
    }

    if (response->count > 0) {
        m_discovery_successful = true;
    }
    m_discovery_in_process = false;
}

STATIC void on_char_discovery_rsp(ble_gattc_evt_char_disc_rsp_t *response, bleio_connection_internal_t *connection) {
    for (size_t i = 0; i < response->count; ++i) {
        ble_gattc_char_t *gattc_char = &response->chars[i];

        bleio_characteristic_obj_t *characteristic = m_new_obj(bleio_characteristic_obj_t);
        characteristic->base.type = &bleio_characteristic_type;

        bleio_uuid_obj_t *uuid = NULL;

        if (gattc_char->uuid.type != BLE_UUID_TYPE_UNKNOWN) {
            // Known characteristic UUID.
            uuid = m_new_obj(bleio_uuid_obj_t);
            uuid->base.type = &bleio_uuid_type;
            bleio_uuid_construct_from_nrf_ble_uuid(uuid, &gattc_char->uuid);
        } else {
            // The discovery response contained a 128-bit UUID that has not yet been registered with the
            // softdevice via sd_ble_uuid_vs_add(). We need to fetch the 128-bit value and register it.
            // For now, just leave the UUID as NULL.
        }

        bleio_characteristic_properties_t props =
            (gattc_char->char_props.broadcast ? CHAR_PROP_BROADCAST : 0) |
            (gattc_char->char_props.indicate ? CHAR_PROP_INDICATE : 0) |
            (gattc_char->char_props.notify ? CHAR_PROP_NOTIFY : 0) |
            (gattc_char->char_props.read ? CHAR_PROP_READ : 0) |
            (gattc_char->char_props.write ? CHAR_PROP_WRITE : 0) |
            (gattc_char->char_props.write_wo_resp ? CHAR_PROP_WRITE_NO_RESPONSE : 0);

        // Call common_hal_bleio_characteristic_construct() to initalize some fields and set up evt handler.
        common_hal_bleio_characteristic_construct(
            characteristic, m_char_discovery_service, gattc_char->handle_value, uuid,
            props, SECURITY_MODE_OPEN, SECURITY_MODE_OPEN,
            GATT_MAX_DATA_LENGTH, false,   // max_length, fixed_length: values don't matter for gattc
            mp_const_empty_bytes,
            NULL);

        mp_obj_list_append(MP_OBJ_FROM_PTR(m_char_discovery_service->characteristic_list),
            MP_OBJ_FROM_PTR(characteristic));
    }

    if (response->count > 0) {
        m_discovery_successful = true;
    }
    m_discovery_in_process = false;
}

STATIC void on_desc_discovery_rsp(ble_gattc_evt_desc_disc_rsp_t *response, bleio_connection_internal_t *connection) {
    for (size_t i = 0; i < response->count; ++i) {
        ble_gattc_desc_t *gattc_desc = &response->descs[i];

        // Remember handles for certain well-known descriptors.
        switch (gattc_desc->uuid.uuid) {
            case BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG:
                m_desc_discovery_characteristic->cccd_handle = gattc_desc->handle;
                break;

            case BLE_UUID_DESCRIPTOR_SERVER_CHAR_CONFIG:
                m_desc_discovery_characteristic->sccd_handle = gattc_desc->handle;
                break;

            case BLE_UUID_DESCRIPTOR_CHAR_USER_DESC:
                m_desc_discovery_characteristic->user_desc_handle = gattc_desc->handle;
                break;

            default:
                // TODO: sd_ble_gattc_descriptors_discover() can return things that are not descriptors,
                // so ignore those.
                // https://devzone.nordicsemi.com/f/nordic-q-a/49500/sd_ble_gattc_descriptors_discover-is-returning-attributes-that-are-not-descriptors
                break;
        }

        bleio_descriptor_obj_t *descriptor = m_new_obj(bleio_descriptor_obj_t);
        descriptor->base.type = &bleio_descriptor_type;

        bleio_uuid_obj_t *uuid = NULL;

        if (gattc_desc->uuid.type != BLE_UUID_TYPE_UNKNOWN) {
            // Known descriptor UUID.
            uuid = m_new_obj(bleio_uuid_obj_t);
            uuid->base.type = &bleio_uuid_type;
            bleio_uuid_construct_from_nrf_ble_uuid(uuid, &gattc_desc->uuid);
        } else {
            // The discovery response contained a 128-bit UUID that has not yet been registered with the
            // softdevice via sd_ble_uuid_vs_add(). We need to fetch the 128-bit value and register it.
            // For now, just leave the UUID as NULL.
        }

        common_hal_bleio_descriptor_construct(
            descriptor, m_desc_discovery_characteristic, uuid,
            SECURITY_MODE_OPEN, SECURITY_MODE_OPEN,
            GATT_MAX_DATA_LENGTH, false, mp_const_empty_bytes);
        descriptor->handle = gattc_desc->handle;

        mp_obj_list_append(MP_OBJ_FROM_PTR(m_desc_discovery_characteristic->descriptor_list),
            MP_OBJ_FROM_PTR(descriptor));
    }

    if (response->count > 0) {
        m_discovery_successful = true;
    }
    m_discovery_in_process = false;
}

STATIC bool discovery_on_ble_evt(ble_evt_t *ble_evt, mp_obj_t payload) {
    bleio_connection_internal_t *connection = MP_OBJ_TO_PTR(payload);
    switch (ble_evt->header.evt_id) {
        case BLE_GAP_EVT_DISCONNECTED:
            m_discovery_successful = false;
            m_discovery_in_process = false;
            break;

        case BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP:
            on_primary_srv_discovery_rsp(&ble_evt->evt.gattc_evt.params.prim_srvc_disc_rsp, connection);
            break;

        case BLE_GATTC_EVT_CHAR_DISC_RSP:
            on_char_discovery_rsp(&ble_evt->evt.gattc_evt.params.char_disc_rsp, connection);
            break;

        case BLE_GATTC_EVT_DESC_DISC_RSP:
            on_desc_discovery_rsp(&ble_evt->evt.gattc_evt.params.desc_disc_rsp, connection);
            break;

        default:
            // CONNECTION_DEBUG_PRINTF(&mp_plat_print, "Unhandled discovery event: 0x%04x\n", ble_evt->header.evt_id);
            return false;
            break;
    }
    return true;
}

STATIC void discover_remote_services(bleio_connection_internal_t *self, mp_obj_t service_uuids_whitelist) {
    ble_drv_add_event_handler(discovery_on_ble_evt, self);

    // Start over with an empty list.
    self->remote_service_list = mp_obj_new_list(0, NULL);

    if (service_uuids_whitelist == mp_const_none) {
        // List of service UUID's not given, so discover all available services.

        uint16_t next_service_start_handle = BLE_GATT_HANDLE_START;

        while (discover_next_services(self, next_service_start_handle, MP_OBJ_NULL)) {
            // discover_next_services() appends to remote_services_list.

            // Get the most recently discovered service, and then ask for services
            // whose handles start after the last attribute handle inside that service.
            // There must be at least one if discover_next_services() returned true.
            const bleio_service_obj_t *service =
                self->remote_service_list->items[self->remote_service_list->len - 1];
            next_service_start_handle = service->end_handle + 1;
        }
    } else {
        mp_obj_iter_buf_t iter_buf;
        mp_obj_t iterable = mp_getiter(service_uuids_whitelist, &iter_buf);
        mp_obj_t uuid_obj;
        while ((uuid_obj = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            if (!mp_obj_is_type(uuid_obj, &bleio_uuid_type)) {
                mp_raise_TypeError(translate("non-UUID found in service_uuids_whitelist"));
            }
            bleio_uuid_obj_t *uuid = MP_OBJ_TO_PTR(uuid_obj);

            ble_uuid_t nrf_uuid;
            bleio_uuid_convert_to_nrf_ble_uuid(uuid, &nrf_uuid);

            // Service might or might not be discovered; that's ok. Caller has to check
            // Central.remote_services to find out.
            // We only need to call this once for each service to discover.
            discover_next_services(self, BLE_GATT_HANDLE_START, &nrf_uuid);
        }
    }


    for (size_t i = 0; i < self->remote_service_list->len; i++) {
        bleio_service_obj_t *service = MP_OBJ_TO_PTR(self->remote_service_list->items[i]);
        // Skip the service if it had an unknown (unregistered) UUID.
        if (service->uuid == NULL) {
            continue;
        }

        uint16_t next_char_start_handle = service->start_handle;

        // Stop when we go past the end of the range of handles for this service or
        // discovery call returns nothing.
        // discover_next_characteristics() appends to the characteristic_list.
        while (next_char_start_handle <= service->end_handle &&
               discover_next_characteristics(self, service, next_char_start_handle)) {

            // Get the most recently discovered characteristic, and then ask for characteristics
            // whose handles start after the last attribute handle inside that characteristic.
            // There must be at least one if discover_next_characteristics() returned true.
            const bleio_characteristic_obj_t *characteristic =
                MP_OBJ_TO_PTR(service->characteristic_list->items[service->characteristic_list->len - 1]);

            next_char_start_handle = characteristic->handle + 1;
        }

        // Got characteristics for this service. Now discover descriptors for each characteristic.
        size_t char_list_len = service->characteristic_list->len;
        for (size_t char_idx = 0; char_idx < char_list_len; ++char_idx) {
            bleio_characteristic_obj_t *characteristic =
                MP_OBJ_TO_PTR(service->characteristic_list->items[char_idx]);
            const bool last_characteristic = char_idx == char_list_len - 1;
            bleio_characteristic_obj_t *next_characteristic = last_characteristic
                ? NULL
                : MP_OBJ_TO_PTR(service->characteristic_list->items[char_idx + 1]);

            // Skip the characteristic if it had an unknown (unregistered) UUID.
            if (characteristic->uuid == NULL) {
                continue;
            }

            uint16_t next_desc_start_handle = characteristic->handle + 1;

            // Don't run past the end of this service or the beginning of the next characteristic.
            uint16_t next_desc_end_handle = next_characteristic == NULL
                ? service->end_handle
                : next_characteristic->handle - 1;

            // Stop when we go past the end of the range of handles for this service or
            // discovery call returns nothing.
            // discover_next_descriptors() appends to the descriptor_list.
            while (next_desc_start_handle <= service->end_handle &&
                   next_desc_start_handle <= next_desc_end_handle &&
                   discover_next_descriptors(self, characteristic,
                       next_desc_start_handle, next_desc_end_handle)) {
                // Get the most recently discovered descriptor, and then ask for descriptors
                // whose handles start after that descriptor's handle.
                // There must be at least one if discover_next_descriptors() returned true.
                const bleio_descriptor_obj_t *descriptor =
                    characteristic->descriptor_list->items[characteristic->descriptor_list->len - 1];
                next_desc_start_handle = descriptor->handle + 1;
            }
        }
    }

    // This event handler is no longer needed.
    ble_drv_remove_event_handler(discovery_on_ble_evt, self);
}

mp_obj_tuple_t *common_hal_bleio_connection_discover_remote_services(bleio_connection_obj_t *self, mp_obj_t service_uuids_whitelist) {
    discover_remote_services(self->connection, service_uuids_whitelist);
    bleio_connection_ensure_connected(self);
    // Convert to a tuple and then clear the list so the callee will take ownership.
    mp_obj_tuple_t *services_tuple =
        mp_obj_new_tuple(self->connection->remote_service_list->len,
            self->connection->remote_service_list->items);
    mp_obj_list_clear(MP_OBJ_FROM_PTR(self->connection->remote_service_list));

    return services_tuple;
}

uint16_t bleio_connection_get_conn_handle(bleio_connection_obj_t *self) {
    if (self == NULL || self->connection == NULL) {
        return BLE_CONN_HANDLE_INVALID;
    }
    return self->connection->conn_handle;
}

mp_obj_t bleio_connection_new_from_internal(bleio_connection_internal_t *internal) {
    if (internal->connection_obj != mp_const_none) {
        return internal->connection_obj;
    }
    bleio_connection_obj_t *connection = m_new_obj(bleio_connection_obj_t);
    connection->base.type = &bleio_connection_type;
    connection->connection = internal;
    internal->connection_obj = connection;

    return MP_OBJ_FROM_PTR(connection);
}

// Find the connection that uses the given conn_handle. Return NULL if not found.
bleio_connection_internal_t *bleio_conn_handle_to_connection(uint16_t conn_handle) {
    bleio_connection_internal_t *connection;
    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        connection = &bleio_connections[i];
        if (connection->conn_handle == conn_handle) {
            return connection;
        }
    }

    return NULL;
}
