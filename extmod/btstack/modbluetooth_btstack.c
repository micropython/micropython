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

#ifndef MICROPY_PY_BLUETOOTH_DEFAULT_GAP_NAME
#define MICROPY_PY_BLUETOOTH_DEFAULT_GAP_NAME "MPY BTSTACK"
#endif

// How long to wait for a controller to init/deinit.
// Some controllers can take up to 5-6 seconds in normal operation.
STATIC const uint32_t BTSTACK_INIT_DEINIT_TIMEOUT_MS = 15000;

// We need to know the attribute handle for the GAP device name (see GAP_DEVICE_NAME_UUID)
// so it can be put into the gatts_db before registering the services, and accessed
// efficiently when requesting an attribute in att_read_callback.  Because this is the
// first characteristic of the first service, it always has a handle value of 3.
STATIC const uint16_t BTSTACK_GAP_DEVICE_NAME_HANDLE = 3;

volatile int mp_bluetooth_btstack_state = MP_BLUETOOTH_BTSTACK_STATE_OFF;

STATIC btstack_packet_callback_registration_t hci_event_callback_registration;

STATIC int btstack_error_to_errno(int err) {
    if (err == ERROR_CODE_SUCCESS) {
        return 0;
    } else if (err == BTSTACK_ACL_BUFFERS_FULL) {
        return MP_ENOMEM;
    } else {
        return MP_EINVAL;
    }
}

STATIC mp_obj_bluetooth_uuid_t create_mp_uuid(uint16_t uuid16, const uint8_t *uuid128) {
    mp_obj_bluetooth_uuid_t result;
    if (uuid16 != 0) {
        result.data[0] = uuid16 & 0xff;
        result.data[1] = (uuid16 >> 8) & 0xff;
        result.type = MP_BLUETOOTH_UUID_TYPE_16;
    } else {
        reverse_128(uuid128, result.data);
        result.type = MP_BLUETOOTH_UUID_TYPE_128;
    }
    return result;
}

STATIC void btstack_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
    (void)channel;
    (void)size;
    DEBUG_EVENT_printf("btstack_packet_handler(packet_type=%u, channel=%u, packet=%p, size=%u)\n", packet_type, channel, packet, size);
    if (packet_type != HCI_EVENT_PACKET) {
        return;
    }

    uint8_t event_type = hci_event_packet_get_type(packet);
    if (event_type == ATT_EVENT_CONNECTED) {
        DEBUG_EVENT_printf("  --> att connected\n");
    } else if (event_type == ATT_EVENT_DISCONNECTED) {
        DEBUG_EVENT_printf("  --> att disconnected\n");
    } else if (event_type == HCI_EVENT_LE_META) {
        DEBUG_EVENT_printf("  --> hci le meta\n");
        if (hci_event_le_meta_get_subevent_code(packet) == HCI_SUBEVENT_LE_CONNECTION_COMPLETE) {
            uint16_t conn_handle = hci_subevent_le_connection_complete_get_connection_handle(packet);
            uint8_t addr_type = hci_subevent_le_connection_complete_get_peer_address_type(packet);
            bd_addr_t addr;
            hci_subevent_le_connection_complete_get_peer_address(packet, addr);
            uint16_t irq_event;
            if (hci_subevent_le_connection_complete_get_role(packet) == 0) {
                // Master role.
                irq_event = MP_BLUETOOTH_IRQ_PERIPHERAL_CONNECT;
            } else {
                // Slave role.
                irq_event = MP_BLUETOOTH_IRQ_CENTRAL_CONNECT;
            }
            mp_bluetooth_gap_on_connected_disconnected(irq_event, conn_handle, addr_type, addr);
        }
    } else if (event_type == BTSTACK_EVENT_STATE) {
        uint8_t state = btstack_event_state_get_state(packet);
        DEBUG_EVENT_printf("  --> btstack event state 0x%02x\n", state);
        if (state == HCI_STATE_WORKING) {
            // Signal that initialisation has completed.
            mp_bluetooth_btstack_state = MP_BLUETOOTH_BTSTACK_STATE_ACTIVE;
        } else if (state == HCI_STATE_OFF) {
            // Signal that de-initialisation has completed.
            mp_bluetooth_btstack_state = MP_BLUETOOTH_BTSTACK_STATE_OFF;
        }
    } else if (event_type == HCI_EVENT_TRANSPORT_PACKET_SENT) {
        DEBUG_EVENT_printf("  --> hci transport packet set\n");
    } else if (event_type == HCI_EVENT_COMMAND_COMPLETE) {
        DEBUG_EVENT_printf("  --> hci command complete\n");
    } else if (event_type == HCI_EVENT_COMMAND_STATUS) {
        DEBUG_EVENT_printf("  --> hci command status\n");
    } else if (event_type == HCI_EVENT_NUMBER_OF_COMPLETED_PACKETS) {
        DEBUG_EVENT_printf("  --> hci number of completed packets\n");
    } else if (event_type == BTSTACK_EVENT_NR_CONNECTIONS_CHANGED) {
        DEBUG_EVENT_printf("  --> btstack # conns changed\n");
    } else if (event_type == HCI_EVENT_VENDOR_SPECIFIC) {
        DEBUG_EVENT_printf("  --> hci vendor specific\n");
    } else if (event_type == GAP_EVENT_ADVERTISING_REPORT) {
        DEBUG_EVENT_printf("  --> gap advertising report\n");
        bd_addr_t address;
        gap_event_advertising_report_get_address(packet, address);
        uint8_t adv_event_type = gap_event_advertising_report_get_advertising_event_type(packet);
        uint8_t address_type = gap_event_advertising_report_get_address_type(packet);
        int8_t rssi = gap_event_advertising_report_get_rssi(packet);
        uint8_t length = gap_event_advertising_report_get_data_length(packet);
        const uint8_t *data = gap_event_advertising_report_get_data(packet);
        mp_bluetooth_gap_on_scan_result(address_type, address, adv_event_type, rssi, data, length);
    } else if (event_type == HCI_EVENT_DISCONNECTION_COMPLETE) {
        DEBUG_EVENT_printf("  --> hci disconnect complete\n");
        uint16_t conn_handle = hci_event_disconnection_complete_get_connection_handle(packet);
        const hci_connection_t *conn = hci_connection_for_handle(conn_handle);
        uint16_t irq_event;
        if (conn == NULL || conn->role == 0) {
            // Master role.
            irq_event = MP_BLUETOOTH_IRQ_PERIPHERAL_DISCONNECT;
        } else {
            // Slave role.
            irq_event = MP_BLUETOOTH_IRQ_CENTRAL_DISCONNECT;
        }
        uint8_t addr[6] = {0};
        mp_bluetooth_gap_on_connected_disconnected(irq_event, conn_handle, 0xff, addr);
    #if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
    } else if (event_type == GATT_EVENT_QUERY_COMPLETE) {
        DEBUG_EVENT_printf("  --> gatt query complete\n");
    } else if (event_type == GATT_EVENT_SERVICE_QUERY_RESULT) {
        DEBUG_EVENT_printf("  --> gatt service query result\n");
        uint16_t conn_handle = gatt_event_service_query_result_get_handle(packet);
        gatt_client_service_t service;
        gatt_event_service_query_result_get_service(packet, &service);
        mp_obj_bluetooth_uuid_t service_uuid = create_mp_uuid(service.uuid16, service.uuid128);
        mp_bluetooth_gattc_on_primary_service_result(conn_handle, service.start_group_handle, service.end_group_handle, &service_uuid);
    } else if (event_type == GATT_EVENT_CHARACTERISTIC_QUERY_RESULT) {
        DEBUG_EVENT_printf("  --> gatt characteristic query result\n");
        uint16_t conn_handle = gatt_event_characteristic_query_result_get_handle(packet);
        gatt_client_characteristic_t characteristic;
        gatt_event_characteristic_query_result_get_characteristic(packet, &characteristic);
        mp_obj_bluetooth_uuid_t characteristic_uuid = create_mp_uuid(characteristic.uuid16, characteristic.uuid128);
        mp_bluetooth_gattc_on_characteristic_result(conn_handle, characteristic.start_handle, characteristic.value_handle, characteristic.properties, &characteristic_uuid);
    } else if (event_type == GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT) {
        DEBUG_EVENT_printf("  --> gatt descriptor query result\n");
        uint16_t conn_handle = gatt_event_all_characteristic_descriptors_query_result_get_handle(packet);
        gatt_client_characteristic_descriptor_t descriptor;
        gatt_event_all_characteristic_descriptors_query_result_get_characteristic_descriptor(packet, &descriptor);
        mp_obj_bluetooth_uuid_t descriptor_uuid = create_mp_uuid(descriptor.uuid16, descriptor.uuid128);
        mp_bluetooth_gattc_on_descriptor_result(conn_handle, descriptor.handle, &descriptor_uuid);
    } else if (event_type == GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT) {
        DEBUG_EVENT_printf("  --> gatt characteristic value query result\n");
        uint16_t conn_handle = gatt_event_characteristic_value_query_result_get_handle(packet);
        uint16_t value_handle = gatt_event_characteristic_value_query_result_get_value_handle(packet);
        uint16_t len = gatt_event_characteristic_value_query_result_get_value_length(packet);
        const uint8_t *data = gatt_event_characteristic_value_query_result_get_value(packet);
        mp_uint_t atomic_state;
        len = mp_bluetooth_gattc_on_data_available_start(MP_BLUETOOTH_IRQ_GATTC_READ_RESULT, conn_handle, value_handle, len, &atomic_state);
        mp_bluetooth_gattc_on_data_available_chunk(data, len);
        mp_bluetooth_gattc_on_data_available_end(atomic_state);
    } else if (event_type == GATT_EVENT_NOTIFICATION) {
        DEBUG_EVENT_printf("  --> gatt notification\n");
        uint16_t conn_handle = gatt_event_notification_get_handle(packet);
        uint16_t value_handle = gatt_event_notification_get_value_handle(packet);
        uint16_t len = gatt_event_notification_get_value_length(packet);
        const uint8_t *data = gatt_event_notification_get_value(packet);
        mp_uint_t atomic_state;
        len = mp_bluetooth_gattc_on_data_available_start(MP_BLUETOOTH_IRQ_GATTC_NOTIFY, conn_handle, value_handle, len, &atomic_state);
        mp_bluetooth_gattc_on_data_available_chunk(data, len);
        mp_bluetooth_gattc_on_data_available_end(atomic_state);
    } else if (event_type == GATT_EVENT_INDICATION) {
        DEBUG_EVENT_printf("  --> gatt indication\n");
        uint16_t conn_handle = gatt_event_indication_get_handle(packet);
        uint16_t value_handle = gatt_event_indication_get_value_handle(packet);
        uint16_t len = gatt_event_indication_get_value_length(packet);
        const uint8_t *data = gatt_event_indication_get_value(packet);
        mp_uint_t atomic_state;
        len = mp_bluetooth_gattc_on_data_available_start(MP_BLUETOOTH_IRQ_GATTC_INDICATE, conn_handle, value_handle, len, &atomic_state);
        mp_bluetooth_gattc_on_data_available_chunk(data, len);
        mp_bluetooth_gattc_on_data_available_end(atomic_state);
    #endif
    } else {
        DEBUG_EVENT_printf("  --> hci event type: unknown (0x%02x)\n", event_type);
    }
}

#if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
STATIC void btstack_packet_handler_write_with_response(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
    (void)channel;
    (void)size;
    DEBUG_EVENT_printf("btstack_packet_handler_write_with_response(packet_type=%u, channel=%u, packet=%p, size=%u)\n", packet_type, channel, packet, size);
    if (packet_type != HCI_EVENT_PACKET) {
        return;
    }

    uint8_t event_type = hci_event_packet_get_type(packet);
    if (event_type == GATT_EVENT_QUERY_COMPLETE) {
        DEBUG_EVENT_printf("  --> gatt query complete\n");
        uint16_t conn_handle = gatt_event_query_complete_get_handle(packet);
        uint8_t status = gatt_event_query_complete_get_att_status(packet);
        // TODO there is no value_handle to pass here
        mp_bluetooth_gattc_on_write_status(conn_handle, 0, status);
    }
}
#endif

STATIC btstack_timer_source_t btstack_init_deinit_timeout;

STATIC void btstack_init_deinit_timeout_handler(btstack_timer_source_t *ds) {
    (void)ds;

    // Stop waiting for initialisation.
    // This signals both the loops in mp_bluetooth_init and mp_bluetooth_deinit,
    // as well as ports that run a polling loop.
    mp_bluetooth_btstack_state = MP_BLUETOOTH_BTSTACK_STATE_TIMEOUT;
}

int mp_bluetooth_init(void) {
    DEBUG_EVENT_printf("mp_bluetooth_init\n");

    if (mp_bluetooth_btstack_state == MP_BLUETOOTH_BTSTACK_STATE_ACTIVE) {
        return 0;
    }

    // Clean up if necessary.
    mp_bluetooth_deinit();

    btstack_memory_init();

    MP_STATE_PORT(bluetooth_btstack_root_pointers) = m_new0(mp_bluetooth_btstack_root_pointers_t, 1);
    mp_bluetooth_gatts_db_create(&MP_STATE_PORT(bluetooth_btstack_root_pointers)->gatts_db);

    // Set the default GAP device name.
    const char *gap_name = MICROPY_PY_BLUETOOTH_DEFAULT_GAP_NAME;
    size_t gap_len = strlen(gap_name);
    mp_bluetooth_gatts_db_create_entry(MP_STATE_PORT(bluetooth_btstack_root_pointers)->gatts_db, BTSTACK_GAP_DEVICE_NAME_HANDLE, gap_len);
    mp_bluetooth_gap_set_device_name((const uint8_t *)gap_name, gap_len);

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

    #if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
    gatt_client_init();
    #endif

    // Register for HCI events.
    hci_event_callback_registration.callback = &btstack_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    // Set a timeout for HCI initialisation.
    btstack_run_loop_set_timer(&btstack_init_deinit_timeout, BTSTACK_INIT_DEINIT_TIMEOUT_MS);
    btstack_run_loop_set_timer_handler(&btstack_init_deinit_timeout, btstack_init_deinit_timeout_handler);
    btstack_run_loop_add_timer(&btstack_init_deinit_timeout);

    // Either the HCI event will set state to ACTIVE, or the timeout will set it to TIMEOUT.
    mp_bluetooth_btstack_port_start();
    while (mp_bluetooth_btstack_state == MP_BLUETOOTH_BTSTACK_STATE_STARTING) {
        MICROPY_EVENT_POLL_HOOK
    }
    btstack_run_loop_remove_timer(&btstack_init_deinit_timeout);

    // Check for timeout.
    if (mp_bluetooth_btstack_state != MP_BLUETOOTH_BTSTACK_STATE_ACTIVE) {
        // Required to stop the polling loop.
        mp_bluetooth_btstack_state = MP_BLUETOOTH_BTSTACK_STATE_OFF;
        // Attempt a shutdown (may not do anything).
        mp_bluetooth_btstack_port_deinit();

        // Clean up.
        MP_STATE_PORT(bluetooth_btstack_root_pointers) = NULL;
        return MP_ETIMEDOUT;
    }

    #if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
    // Enable GATT_EVENT_NOTIFICATION/GATT_EVENT_INDICATION for all connections and handles.
    gatt_client_listen_for_characteristic_value_updates(&MP_STATE_PORT(bluetooth_btstack_root_pointers)->notification, &btstack_packet_handler, GATT_CLIENT_ANY_CONNECTION, NULL);
    #endif

    return 0;
}

void mp_bluetooth_deinit(void) {
    DEBUG_EVENT_printf("mp_bluetooth_deinit\n");

    // Nothing to do if not initialised.
    if (!MP_STATE_PORT(bluetooth_btstack_root_pointers)) {
        return;
    }

    mp_bluetooth_gap_advertise_stop();

    #if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
    // Remove our registration for notify/indicate.
    gatt_client_stop_listening_for_characteristic_value_updates(&MP_STATE_PORT(bluetooth_btstack_root_pointers)->notification);
    #endif

    // Set a timer that will forcibly set the state to TIMEOUT, which will stop the loop below.
    btstack_run_loop_set_timer(&btstack_init_deinit_timeout, BTSTACK_INIT_DEINIT_TIMEOUT_MS);
    btstack_run_loop_add_timer(&btstack_init_deinit_timeout);

    // This should result in a clean shutdown, which will set the state to OFF.
    // On Unix this is blocking (it joins on the poll thread), on other ports the loop below will wait unil
    // either timeout or clean shutdown.
    mp_bluetooth_btstack_port_deinit();
    while (mp_bluetooth_btstack_state == MP_BLUETOOTH_BTSTACK_STATE_ACTIVE) {
        MICROPY_EVENT_POLL_HOOK
    }
    btstack_run_loop_remove_timer(&btstack_init_deinit_timeout);

    mp_bluetooth_btstack_state = MP_BLUETOOTH_BTSTACK_STATE_OFF;
    MP_STATE_PORT(bluetooth_btstack_root_pointers) = NULL;
}

bool mp_bluetooth_is_active(void) {
    return mp_bluetooth_btstack_state == MP_BLUETOOTH_BTSTACK_STATE_ACTIVE;
}

void mp_bluetooth_get_device_addr(uint8_t *addr) {
    mp_hal_get_mac(MP_HAL_MAC_BDADDR, addr);
}

size_t mp_bluetooth_gap_get_device_name(const uint8_t **buf) {
    uint8_t *value = NULL;
    size_t value_len = 0;
    mp_bluetooth_gatts_db_read(MP_STATE_PORT(bluetooth_btstack_root_pointers)->gatts_db, BTSTACK_GAP_DEVICE_NAME_HANDLE, &value, &value_len);
    *buf = value;
    return value_len;
}

int mp_bluetooth_gap_set_device_name(const uint8_t *buf, size_t len) {
    mp_bluetooth_gatts_db_write(MP_STATE_PORT(bluetooth_btstack_root_pointers)->gatts_db, BTSTACK_GAP_DEVICE_NAME_HANDLE, buf, len);
    return 0;
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
        uint16_t handle = att_db_util_add_characteristic_uuid16(GAP_DEVICE_NAME_UUID, ATT_PROPERTY_READ | ATT_PROPERTY_DYNAMIC, ATT_SECURITY_NONE, ATT_SECURITY_NONE, NULL, 0);
        assert(handle == BTSTACK_GAP_DEVICE_NAME_HANDLE);
        (void)handle;

        att_db_util_add_service_uuid16(0x1801);
        att_db_util_add_characteristic_uuid16(0x2a05, ATT_PROPERTY_READ, ATT_SECURITY_NONE, ATT_SECURITY_NONE, NULL, 0);
    }

    return 0;
}

STATIC uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, uint8_t *buffer, uint16_t buffer_size) {
    (void)connection_handle;
    DEBUG_EVENT_printf("btstack: att_read_callback (handle: %u, offset: %u, buffer: %p, size: %u)\n", att_handle, offset, buffer, buffer_size);
    mp_bluetooth_gatts_db_entry_t *entry = mp_bluetooth_gatts_db_lookup(MP_STATE_PORT(bluetooth_btstack_root_pointers)->gatts_db, att_handle);
    if (!entry) {
        DEBUG_EVENT_printf("btstack: att_read_callback handle not found\n");
        return 0; // TODO: Find status code for not-found.
    }

    return att_read_callback_handle_blob(entry->data, entry->data_len, offset, buffer, buffer_size);
}

STATIC int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size) {
    (void)offset;
    (void)transaction_mode;
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
            props = descriptor_flags[descriptor_index] | ATT_PROPERTY_DYNAMIC;
            read_permission = ATT_SECURITY_NONE;
            write_permission = ATT_SECURITY_NONE;

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
STATIC btstack_timer_source_t scan_duration_timeout;

STATIC void hci_initialization_timeout_handler(btstack_timer_source_t *ds) {
    (void)ds;
    mp_bluetooth_gap_scan_stop();
}

int mp_bluetooth_gap_scan_start(int32_t duration_ms, int32_t interval_us, int32_t window_us) {
    DEBUG_EVENT_printf("mp_bluetooth_gap_scan_start\n");

    btstack_run_loop_set_timer(&scan_duration_timeout, duration_ms);
    btstack_run_loop_set_timer_handler(&scan_duration_timeout, hci_initialization_timeout_handler);
    btstack_run_loop_add_timer(&scan_duration_timeout);

    // 0 = passive scan (we don't handle scan response).
    gap_set_scan_parameters(0, interval_us / 625, window_us / 625);
    gap_start_scan();

    return 0;
}

int mp_bluetooth_gap_scan_stop(void) {
    DEBUG_EVENT_printf("mp_bluetooth_gap_scan_stop\n");
    btstack_run_loop_remove_timer(&scan_duration_timeout);
    gap_stop_scan();
    mp_bluetooth_gap_on_scan_complete();
    return 0;
}

int mp_bluetooth_gap_peripheral_connect(uint8_t addr_type, const uint8_t *addr, int32_t duration_ms) {
    DEBUG_EVENT_printf("mp_bluetooth_gap_peripheral_connect\n");

    uint16_t conn_scan_interval = 60000 / 625;
    uint16_t conn_scan_window = 30000 / 625;
    uint16_t conn_interval_min = 10000 / 1250;
    uint16_t conn_interval_max = 30000 / 1250;
    uint16_t conn_latency = 4;
    uint16_t supervision_timeout = duration_ms / 10; // default = 720
    uint16_t min_ce_length = 10000 / 625;
    uint16_t max_ce_length = 30000 / 625;

    gap_set_connection_parameters(conn_scan_interval, conn_scan_window, conn_interval_min, conn_interval_max, conn_latency, supervision_timeout, min_ce_length, max_ce_length);

    bd_addr_t btstack_addr;
    memcpy(btstack_addr, addr, BD_ADDR_LEN);
    return btstack_error_to_errno(gap_connect(btstack_addr, addr_type));
}

int mp_bluetooth_gattc_discover_primary_services(uint16_t conn_handle) {
    DEBUG_EVENT_printf("mp_bluetooth_gattc_discover_primary_services\n");
    return btstack_error_to_errno(gatt_client_discover_primary_services(&btstack_packet_handler, conn_handle));
}

int mp_bluetooth_gattc_discover_characteristics(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle) {
    DEBUG_EVENT_printf("mp_bluetooth_gattc_discover_characteristics\n");

    gatt_client_service_t service = {
        // Only start/end handles needed for gatt_client_discover_characteristics_for_service.
        .start_group_handle = start_handle,
        .end_group_handle = end_handle,
        .uuid16 = 0,
        .uuid128 = {0},
    };
    return btstack_error_to_errno(gatt_client_discover_characteristics_for_service(&btstack_packet_handler, conn_handle, &service));
}

int mp_bluetooth_gattc_discover_descriptors(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle) {
    DEBUG_EVENT_printf("mp_bluetooth_gattc_discover_descriptors\n");
    gatt_client_characteristic_t characteristic = {
        // Only start/end handles needed for gatt_client_discover_characteristic_descriptors.
        .start_handle = start_handle,
        .value_handle = 0,
        .end_handle = end_handle,
        .properties = 0,
        .uuid16 = 0,
        .uuid128 = {0},
    };
    return btstack_error_to_errno(gatt_client_discover_characteristic_descriptors(&btstack_packet_handler, conn_handle, &characteristic));
}

int mp_bluetooth_gattc_read(uint16_t conn_handle, uint16_t value_handle) {
    DEBUG_EVENT_printf("mp_bluetooth_gattc_read\n");
    return btstack_error_to_errno(gatt_client_read_value_of_characteristic_using_value_handle(&btstack_packet_handler, conn_handle, value_handle));
}

int mp_bluetooth_gattc_write(uint16_t conn_handle, uint16_t value_handle, const uint8_t *value, size_t *value_len, unsigned int mode) {
    DEBUG_EVENT_printf("mp_bluetooth_gattc_write\n");

    // TODO the below gatt_client functions do not copy the data and require it to be valid
    // until the write is done, so there should be some kind of buffering done here.

    if (mode == MP_BLUETOOTH_WRITE_MODE_NO_RESPONSE) {
        // TODO need to call gatt_client_request_can_write_without_response_event then do
        // the actual write on the callback from that.
        return btstack_error_to_errno(gatt_client_write_value_of_characteristic_without_response(conn_handle, value_handle, *value_len, (uint8_t *)value));
    }
    if (mode == MP_BLUETOOTH_WRITE_MODE_WITH_RESPONSE) {
        return btstack_error_to_errno(gatt_client_write_value_of_characteristic(&btstack_packet_handler_write_with_response, conn_handle, value_handle, *value_len, (uint8_t *)value));
    }

    return MP_EINVAL;
}
#endif // MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE

#endif // MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_BTSTACK
