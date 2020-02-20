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

#define DEBUG_EVENT_printf(...) //printf(__VA_ARGS__)

volatile int mp_bluetooth_btstack_state = MP_BLUETOOTH_BTSTACK_STATE_OFF;

static btstack_packet_callback_registration_t hci_event_callback_registration;

STATIC int btstack_error_to_errno(int err) {
    if (err == ERROR_CODE_SUCCESS) {
        return 0;
    } else if (err == BTSTACK_ACL_BUFFERS_FULL) {
        return MP_ENOMEM;
    } else {
        return MP_EINVAL;
    }
}

STATIC void att_server_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
    DEBUG_EVENT_printf("att_server_packet_handler: packet_type: %u, channel: %u, packet: %p, size: %u\n", packet_type, channel, packet, size);
    if (packet_type == HCI_EVENT_PACKET) {
        uint8_t event_type = hci_event_packet_get_type(packet);
        if (event_type == ATT_EVENT_CONNECTED) {
            DEBUG_EVENT_printf("  --> att connected\n");
            uint16_t conn_handle = att_event_connected_get_handle(packet);
            uint8_t addr_type = att_event_connected_get_address_type(packet);
            bd_addr_t addr;
            att_event_connected_get_address(packet, addr);
            mp_bluetooth_gap_on_connected_disconnected(MP_BLUETOOTH_IRQ_CENTRAL_CONNECT, conn_handle, addr_type, addr);
        } else if (event_type == ATT_EVENT_DISCONNECTED) {
            DEBUG_EVENT_printf("  --> att disconnected\n");
            uint16_t conn_handle = att_event_disconnected_get_handle(packet);
            bd_addr_t addr = {0};
            mp_bluetooth_gap_on_connected_disconnected(MP_BLUETOOTH_IRQ_CENTRAL_DISCONNECT, conn_handle, 0, addr);
        } else if (event_type == HCI_EVENT_LE_META) {
            DEBUG_EVENT_printf("  --> att le meta\n");
        } else if (event_type == HCI_EVENT_DISCONNECTION_COMPLETE) {
            DEBUG_EVENT_printf("  --> att disconnect complete\n");
        } else {
            DEBUG_EVENT_printf("  --> att type: unknown (%u)\n", event_type);
        }
    }
}

STATIC void hci_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
    DEBUG_EVENT_printf("hci_packet_handler: packet_type: %u, channel: %u, packet: %p, size: %u\n", packet_type, channel, packet, size);
    if (packet_type == HCI_EVENT_PACKET) {
        uint8_t event_type = hci_event_packet_get_type(packet);
        if (event_type == HCI_EVENT_TRANSPORT_PACKET_SENT || event_type == HCI_EVENT_COMMAND_COMPLETE) {
        } else if (event_type == BTSTACK_EVENT_NR_CONNECTIONS_CHANGED) {
            DEBUG_EVENT_printf("  --> hci type: # conns changed\n");
        } else if (event_type == HCI_EVENT_VENDOR_SPECIFIC) {
            DEBUG_EVENT_printf("  --> hci type: vendor specific\n");
        } else {
            DEBUG_EVENT_printf("  --> hci type: unknown (%u)\n", event_type);
        }
    }
}

int mp_bluetooth_init(void) {
    DEBUG_EVENT_printf("mp_bluetooth_init\n");
    btstack_memory_init();

    MP_STATE_PORT(bluetooth_btstack_root_pointers) = m_new0(mp_bluetooth_btstack_root_pointers_t, 1);
    mp_bluetooth_gatts_db_create(&MP_STATE_PORT(bluetooth_btstack_root_pointers)->gatts_db);

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

    // Register for HCI events.
    memset(&hci_event_callback_registration, 0, sizeof(btstack_packet_callback_registration_t));
    hci_event_callback_registration.callback = &hci_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    // Register for ATT event.
    att_server_register_packet_handler(&att_server_packet_handler);

    return 0;
}

void mp_bluetooth_deinit(void) {
    DEBUG_EVENT_printf("mp_bluetooth_deinit\n");
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
    DEBUG_EVENT_printf("mp_bluetooth_gap_advertise_start\n");
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
    DEBUG_EVENT_printf("mp_bluetooth_gap_advertise_stop\n");
    gap_advertisements_enable(false);
    MP_STATE_PORT(bluetooth_btstack_root_pointers)->adv_data_alloc = 0;
    MP_STATE_PORT(bluetooth_btstack_root_pointers)->adv_data = NULL;
}

int mp_bluetooth_gatts_register_service_begin(bool append) {
    DEBUG_EVENT_printf("mp_bluetooth_gatts_register_service_begin\n");
    if (!append) {
        // This will reset the DB.
        // Becase the DB is statically allocated, there's no problem with just re-initing it.
        // Note this would be a memory leak if we enabled HAVE_MALLOC (there's no API to free the existing db).
        att_db_util_init();

        att_db_util_add_service_uuid16(GAP_SERVICE_UUID);
        att_db_util_add_characteristic_uuid16(GAP_DEVICE_NAME_UUID, ATT_PROPERTY_READ, ATT_SECURITY_NONE, ATT_SECURITY_NONE, (uint8_t *)"MPY BTSTACK", 11);

        att_db_util_add_service_uuid16(0x1801);
        att_db_util_add_characteristic_uuid16(0x2a05, ATT_PROPERTY_READ, ATT_SECURITY_NONE, ATT_SECURITY_NONE, NULL, 0);
    }

    return 0;
}

STATIC uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, uint8_t *buffer, uint16_t buffer_size) {
    DEBUG_EVENT_printf("btstack: att_read_callback (handle: %u, offset: %u, buffer: %p, size: %u)\n", att_handle, offset, buffer, buffer_size);
    mp_bluetooth_gatts_db_entry_t *entry = mp_bluetooth_gatts_db_lookup(MP_STATE_PORT(bluetooth_btstack_root_pointers)->gatts_db, att_handle);
    if (!entry) {
        DEBUG_EVENT_printf("btstack: att_read_callback handle not found\n");
        return 0; // TODO: Find status code for not-found.
    }

    return att_read_callback_handle_blob(entry->data, entry->data_len, offset, buffer, buffer_size);
}

STATIC int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size) {
    DEBUG_EVENT_printf("btstack: att_write_callback (handle: %u, mode: %u, offset: %u, buffer: %p, size: %u)\n", att_handle, transaction_mode, offset, buffer, buffer_size);
    mp_bluetooth_gatts_db_entry_t *entry = mp_bluetooth_gatts_db_lookup(MP_STATE_PORT(bluetooth_btstack_root_pointers)->gatts_db, att_handle);
    if (!entry) {
        DEBUG_EVENT_printf("btstack: att_write_callback handle not found\n");
        return 0; // TODO: Find status code for not-found.
    }

    // TODO: Use `offset` arg.
    size_t append_offset = 0;
    if (entry->append) {
        append_offset = entry->data_len;
    }
    entry->data_len = MIN(entry->data_alloc, buffer_size + append_offset);
    memcpy(entry->data + append_offset, buffer, entry->data_len - append_offset);

    mp_bluetooth_gatts_on_write(connection_handle, att_handle);

    return 0;
}

STATIC inline uint16_t get_uuid16(const mp_obj_bluetooth_uuid_t *uuid) {
    return (uuid->data[1] << 8) | uuid->data[0];
}

int mp_bluetooth_gatts_register_service(mp_obj_bluetooth_uuid_t *service_uuid, mp_obj_bluetooth_uuid_t **characteristic_uuids, uint8_t *characteristic_flags, mp_obj_bluetooth_uuid_t **descriptor_uuids, uint8_t *descriptor_flags, uint8_t *num_descriptors, uint16_t *handles, size_t num_characteristics) {
    DEBUG_EVENT_printf("mp_bluetooth_gatts_register_service\n");
    // Note: btstack expects BE UUIDs (which it immediately convertes to LE).
    // So we have to convert all our modbluetooth LE UUIDs to BE just for the att_db_util_add_* methods (using get_uuid16 above, and reverse_128 from btstackutil.h).

    // TODO: btstack's att_db_util_add_* methods have no bounds checking or validation.
    // Need some way to prevent additional services being added if we're out of space in the static buffer.

    if (service_uuid->type == MP_BLUETOOTH_UUID_TYPE_16) {
        att_db_util_add_service_uuid16(get_uuid16(service_uuid));
    } else if (service_uuid->type == MP_BLUETOOTH_UUID_TYPE_128) {
        uint8_t buffer[16];
        reverse_128(service_uuid->data, buffer);
        att_db_util_add_service_uuid128(buffer);
    } else {
        return MP_EINVAL;
    }

    size_t handle_index = 0;
    size_t descriptor_index = 0;
    static uint8_t cccb_buf[2] = {0};

    for (size_t i = 0; i < num_characteristics; ++i) {
        uint16_t props = characteristic_flags[i] | ATT_PROPERTY_DYNAMIC;
        uint16_t read_permission = ATT_SECURITY_NONE;
        uint16_t write_permission = ATT_SECURITY_NONE;
        if (characteristic_uuids[i]->type == MP_BLUETOOTH_UUID_TYPE_16) {
            handles[handle_index] = att_db_util_add_characteristic_uuid16(get_uuid16(characteristic_uuids[i]), props, read_permission, write_permission, NULL, 0);
        } else if (characteristic_uuids[i]->type == MP_BLUETOOTH_UUID_TYPE_128) {
            uint8_t buffer[16];
            reverse_128(characteristic_uuids[i]->data, buffer);
            handles[handle_index] = att_db_util_add_characteristic_uuid128(buffer, props, read_permission, write_permission, NULL, 0);
        } else {
            return MP_EINVAL;
        }
        mp_bluetooth_gatts_db_create_entry(MP_STATE_PORT(bluetooth_btstack_root_pointers)->gatts_db, handles[handle_index], MP_BLUETOOTH_DEFAULT_ATTR_LEN);
        // If a NOTIFY or INDICATE characteristic is added, then we need to manage a value for the CCCB.
        if (props & (ATT_PROPERTY_NOTIFY | ATT_PROPERTY_INDICATE)) {
            // btstack creates the CCCB as the next handle.
            mp_bluetooth_gatts_db_create_entry(MP_STATE_PORT(bluetooth_btstack_root_pointers)->gatts_db, handles[handle_index] + 1, MP_BLUETOOTH_CCCB_LEN);
            mp_bluetooth_gatts_db_write(MP_STATE_PORT(bluetooth_btstack_root_pointers)->gatts_db, handles[handle_index] + 1, cccb_buf, sizeof(cccb_buf));
        }
        DEBUG_EVENT_printf("Registered char with handle %u\n", handles[handle_index]);
        ++handle_index;

        for (size_t j = 0; j < num_descriptors[i]; ++j) {
            uint16_t props = descriptor_flags[descriptor_index] | ATT_PROPERTY_DYNAMIC;
            uint16_t read_permission = ATT_SECURITY_NONE;
            uint16_t write_permission = ATT_SECURITY_NONE;

            if (descriptor_uuids[descriptor_index]->type == MP_BLUETOOTH_UUID_TYPE_16) {
                handles[handle_index] = att_db_util_add_descriptor_uuid16(get_uuid16(descriptor_uuids[descriptor_index]), props, read_permission, write_permission, NULL, 0);
            } else if (descriptor_uuids[descriptor_index]->type == MP_BLUETOOTH_UUID_TYPE_128) {
                uint8_t buffer[16];
                reverse_128(descriptor_uuids[descriptor_index]->data, buffer);
                handles[handle_index] = att_db_util_add_descriptor_uuid128(buffer, props, read_permission, write_permission, NULL, 0);
            } else {
                return MP_EINVAL;
            }
            mp_bluetooth_gatts_db_create_entry(MP_STATE_PORT(bluetooth_btstack_root_pointers)->gatts_db, handles[handle_index], MP_BLUETOOTH_DEFAULT_ATTR_LEN);
            DEBUG_EVENT_printf("Registered desc with handle %u\n", handles[handle_index]);
            ++descriptor_index;
            ++handle_index;
        }
    }

    return 0;
}

int mp_bluetooth_gatts_register_service_end(void) {
    DEBUG_EVENT_printf("mp_bluetooth_gatts_register_service_end\n");
    att_server_init(att_db_util_get_address(), &att_read_callback, &att_write_callback);
    return 0;
}

int mp_bluetooth_gatts_read(uint16_t value_handle, uint8_t **value, size_t *value_len) {
    DEBUG_EVENT_printf("mp_bluetooth_gatts_read\n");
    return mp_bluetooth_gatts_db_read(MP_STATE_PORT(bluetooth_btstack_root_pointers)->gatts_db, value_handle, value, value_len);
}

int mp_bluetooth_gatts_write(uint16_t value_handle, const uint8_t *value, size_t value_len) {
    DEBUG_EVENT_printf("mp_bluetooth_gatts_write\n");
    return mp_bluetooth_gatts_db_write(MP_STATE_PORT(bluetooth_btstack_root_pointers)->gatts_db, value_handle, value, value_len);
}

int mp_bluetooth_gatts_notify(uint16_t conn_handle, uint16_t value_handle) {
    DEBUG_EVENT_printf("mp_bluetooth_gatts_notify\n");
    // Note: btstack doesn't appear to support sending a notification without a value, so include the stored value.
    uint8_t *data = NULL;
    size_t len = 0;
    mp_bluetooth_gatts_db_read(MP_STATE_PORT(bluetooth_btstack_root_pointers)->gatts_db, value_handle, &data, &len);
    return mp_bluetooth_gatts_notify_send(conn_handle, value_handle, data, &len);
}

int mp_bluetooth_gatts_notify_send(uint16_t conn_handle, uint16_t value_handle, const uint8_t *value, size_t *value_len) {
    DEBUG_EVENT_printf("mp_bluetooth_gatts_notify_send\n");
    // TODO: We need to use att_server_request_to_send_notification here as the stack may not be ready to send a notification.
    int err = att_server_notify(conn_handle, value_handle, value, *value_len);
    return btstack_error_to_errno(err);
}

int mp_bluetooth_gatts_indicate(uint16_t conn_handle, uint16_t value_handle) {
    DEBUG_EVENT_printf("mp_bluetooth_gatts_indicate\n");
    return btstack_error_to_errno(att_server_indicate(conn_handle, value_handle, NULL, 0));
}

int mp_bluetooth_gatts_set_buffer(uint16_t value_handle, size_t len, bool append) {
    DEBUG_EVENT_printf("mp_bluetooth_gatts_set_buffer\n");
    return mp_bluetooth_gatts_db_resize(MP_STATE_PORT(bluetooth_btstack_root_pointers)->gatts_db, value_handle, len, append);
}

int mp_bluetooth_gap_disconnect(uint16_t conn_handle) {
    DEBUG_EVENT_printf("mp_bluetooth_gap_disconnect\n");
    gap_disconnect(conn_handle);
    return 0;
}

#if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
int mp_bluetooth_gap_scan_start(int32_t duration_ms, int32_t interval_us, int32_t window_us) {
    DEBUG_EVENT_printf("mp_bluetooth_gap_scan_start\n");
    return 0;
}

int mp_bluetooth_gap_scan_stop(void) {
    DEBUG_EVENT_printf("mp_bluetooth_gap_scan_stop\n");
    return 0;
}

int mp_bluetooth_gap_peripheral_connect(uint8_t addr_type, const uint8_t *addr, int32_t duration_ms) {
    DEBUG_EVENT_printf("mp_bluetooth_gap_peripheral_connect\n");
    return 0;
}

int mp_bluetooth_gattc_discover_primary_services(uint16_t conn_handle) {
    DEBUG_EVENT_printf("mp_bluetooth_gattc_discover_primary_services\n");
    return 0;
}

int mp_bluetooth_gattc_discover_characteristics(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle) {
    DEBUG_EVENT_printf("mp_bluetooth_gattc_discover_characteristics\n");
    return 0;
}

int mp_bluetooth_gattc_discover_descriptors(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle) {
    DEBUG_EVENT_printf("mp_bluetooth_gattc_discover_descriptors\n");
    return 0;
}

int mp_bluetooth_gattc_read(uint16_t conn_handle, uint16_t value_handle) {
    DEBUG_EVENT_printf("mp_bluetooth_gattc_read\n");
    return 0;
}

int mp_bluetooth_gattc_write(uint16_t conn_handle, uint16_t value_handle, const uint8_t *value, size_t *value_len, unsigned int mode) {
    DEBUG_EVENT_printf("mp_bluetooth_gattc_write\n");
    return 0;
}
#endif

#endif // MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_BTSTACK
