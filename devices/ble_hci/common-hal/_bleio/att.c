// Derived from ArduinoBLE.
// Copyright 2020 Dan Halbert for Adafruit Industries

/*
  This file is part of the ArduinoBLE library.
  Copyright (c) 2018 Arduino SA. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "hci.h"
#include "att.h"

// Zephyr include files to define HCI communication values and structs.
//#include "hci_include/hci.h"
//#include "hci_include/hci_err.h"
#include "hci_include/l2cap_internal.h"

#include "py/obj.h"
#include "common-hal/_bleio/Adapter.h"
#include "common-hal/_bleio/Attribute.h"
#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "supervisor/shared/tick.h"

STATIC uint16_t max_mtu = BT_ATT_DEFAULT_LE_MTU;  // 23
STATIC unsigned long timeout = 5000;

STATIC volatile bool confirm;

STATIC uint16_t long_write_handle = BLE_GATT_HANDLE_INVALID;
STATIC uint8_t* long_write_value = NULL;
STATIC uint16_t long_write_value_length = 0;

// When we send a request, fill this struct with info about the expected response.
// We check this against the actual received response.
STATIC struct {
    uint16_t conn_handle;       // Expected handle.
    uint8_t opcode;             // Expected RSP opcode.
    uint8_t* buffer;            // Pointer to response packet
    uint8_t length;             // Length of response packet.
} expected_rsp;

//FIX BLEDeviceEventHandler event_handlers[2];


STATIC void send_error(uint16_t conn_handle, uint8_t opcode, uint16_t handle, uint8_t code) {
    struct __packed {
        struct bt_att_hdr h;
        struct bt_att_error_rsp r;
    } rsp = { {
            .code = BT_ATT_OP_ERROR_RSP,
        }, {
            .request = opcode,
        }
    };

    hci_send_acl_pkt(conn_handle, BT_L2CAP_CID_ATT, sizeof(rsp), (uint8_t *) &rsp);
}

STATIC void send_req(uint16_t conn_handle, size_t request_length, uint8_t* request_buffer) {
    hci_send_acl_pkt(conn_handle, BT_L2CAP_CID_ATT, request_length, request_buffer);
}

STATIC int send_req_wait_for_rsp(uint16_t conn_handle, size_t request_length, uint8_t* request_buffer, uint8_t response_buffer[]) {
    // We expect a particular kind of response after this request.
    expected_rsp.conn_handle = conn_handle;
    // The response opcode is the request opcode + 1.
    expected_rsp.opcode = request_buffer[0] + 1;
    expected_rsp.buffer = response_buffer;
    expected_rsp.length = 0;

    send_req(conn_handle, request_length, request_buffer);

    if (response_buffer == NULL) {
        // not expecting a response.
        return 0;
    }

    for (uint64_t start = supervisor_ticks_ms64(); supervisor_ticks_ms64() - start < timeout;) {
        hci_poll_for_incoming_pkt();

        if (!att_handle_is_connected(conn_handle)) {
            break;
        }

        if (expected_rsp.length != 0) {
            expected_rsp.conn_handle = 0xffff;
            return expected_rsp.length;
        }
    }

    expected_rsp.conn_handle = 0xffff;
    return 0;
}

// If a response matches what is in expected_rsp, copy the rest of it into the buffer.
STATIC void check_and_save_expected_rsp(uint16_t conn_handle, uint8_t opcode, uint8_t dlen, uint8_t data[]) {
    if (conn_handle == expected_rsp.conn_handle && expected_rsp.opcode == opcode) {
        expected_rsp.buffer[0] = opcode;
        memcpy(&expected_rsp.buffer[1], data, dlen);
        expected_rsp.length = dlen + 1;
    }
}

void att_init(void) {
    max_mtu = BT_ATT_DEFAULT_LE_MTU;
    timeout = 5000;
    long_write_handle = BLE_GATT_HANDLE_INVALID;
    long_write_value = NULL;
    long_write_value_length = 0;

    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        bleio_connections[i].conn_handle = BLE_CONN_HANDLE_INVALID;
        bleio_connections[i].role = 0x00;
        bleio_connections[i].addr.type = 0;
        memset(bleio_connections[i].addr.a.val, 0, sizeof_field(bt_addr_t, val));
        bleio_connections[i].mtu = BT_ATT_DEFAULT_LE_MTU;
    }

    //FIX memset(event_handlers, 0x00, sizeof(event_handlers));
}

bool att_connect_to_address(bt_addr_le_t *addr) {
    //FIX
    if (hci_le_create_conn(0x0060, 0x0030, 0x00, addr, 0x00,
                           0x0006, 0x000c, 0x0000, 0x00c8, 0x0004, 0x0006) != 0) {
        return false;
    }

    bool is_connected = false;

    for (uint64_t start = supervisor_ticks_ms64(); supervisor_ticks_ms64() - start < timeout;) {
        hci_poll_for_incoming_pkt();

        is_connected = att_address_is_connected(addr);

        if (is_connected) {
            break;
        }
    }

    if (!is_connected) {
        hci_le_cancel_conn();
    }

    return is_connected;
}

bool att_disconnect_from_address(bt_addr_le_t *addr) {
    uint16_t conn_handle = att_conn_handle(addr);
    if (conn_handle == 0xffff) {
        return false;
    }

    hci_disconnect(conn_handle);

    hci_poll_for_incoming_pkt_timeout(timeout);

    if (!att_handle_is_connected(conn_handle)) {
        return true;
    }

    return false;
}

//FIX
// STATIC bool discover_services(uint16_t conn_handle, BLERemoteDevice* device, const char* serviceUuidFilter) {
//     uint16_t reqStart_handle = 0x0001;
//     uint16_t reqEnd_handle = 0xffff;

//     uint8_t response_buffer[max_mtu];

//     BLEUuid serviceUuid(serviceUuidFilter);

//     while (reqEnd_handle == 0xffff) {
//         int respLength = readByGroupReq(conn_handle, reqStart_handle, reqEnd_handle, BLE_TYPE_PRIMARY_SERVICE, response_buffer);

//         if (respLength == 0) {
//             return false;
//         }

//         if (response_buffer[0] == BT_ATT_OP_READ_BY_GROUP_RSP) {
//             uint16_t lengthPerService = response_buffer[1];
//             uint8_t uuidLen = lengthPerService - 4;

//             for (size_t i = 2; i < respLength; i += lengthPerService) {
//                 struct __attribute__ ((packed)) RawService {
//                     uint16_t start_handle;
//                     uint16_t end_handle;
//                     uint8_t uuid[16];
//                 } *rawService = (RawService*)&response_buffer[i];

//                 if (serviceUuidFilter == NULL ||
//                     (uuidLen == serviceUuid.length() && memcmp(rawService->uuid, serviceUuid.data(), uuidLen) == 0)) {

//                     BLERemoteService* service = new BLERemoteService(rawService->uuid, uuidLen,
//                                                                      rawService->start_handle,
//                                                                      rawService->end_handle);

//                     if (service == NULL) {
//                         return false;
//                     }

//                     device->addService(service);

//                 }

//                 reqStart_handle = rawService->end_handle + 1;

//                 if (reqStart_handle == BLE_GATT_HANDLE_INVALID) {
//                     reqEnd_handle = BLE_GATT_HANDLE_INVALID;
//                 }
//             }
//         } else {
//             reqEnd_handle = BLE_GATT_HANDLE_INVALID;
//         }
//     }

//     return true;
// }

// STATIC bool discover_characteristics(uint16_t conn_handle, BLERemoteDevice* device) {
//     uint16_t reqStart_handle = 0x0001;
//     uint16_t reqEnd_handle = 0xffff;

//     uint8_t response_buffer[max_mtu];

//     int serviceCount = device->serviceCount();

//     for (size_t i = 0; i < serviceCount; i++) {
//         BLERemoteService* service = device->service(i);

//         reqStart_handle = service->start_handle();
//         reqEnd_handle = service->end_handle();

//         while (1) {
//             int respLength = readByTypeReq(conn_handle, reqStart_handle, reqEnd_handle, BLE_TYPE_CHARACTERISTIC, response_buffer);

//             if (respLength == 0) {
//                 return false;
//             }

//             if (response_buffer[0] == BT_ATT_OP_READ_BY_TYPE_RSP) {
//                 uint16_t lengthPerCharacteristic = response_buffer[1];
//                 uint8_t uuidLen = lengthPerCharacteristic - 5;

//                 for (size_t i = 2; i < respLength; i += lengthPerCharacteristic) {
//                     struct __attribute__ ((packed)) RawCharacteristic {
//                         uint16_t start_handle;
//                         uint8_t properties;
//                         uint16_t value_handle;
//                         uint8_t uuid[16];
//                     } *rawCharacteristic = (RawCharacteristic*)&response_buffer[i];

//                     BLERemoteCharacteristic* characteristic = new BLERemoteCharacteristic(rawCharacteristic->uuid, uuidLen,
//                                                                                           conn_handle,
//                                                                                           rawCharacteristic->start_handle,
//                                                                                           rawCharacteristic->properties,
//                                                                                           rawCharacteristic->value_handle);

//                     if (characteristic == NULL) {
//                         return false;
//                     }

//                     service->addCharacteristic(characteristic);

//                     reqStart_handle = rawCharacteristic->value_handle + 1;
//                 }
//             } else {
//                 break;
//             }
//         }
//     }

//     return true;
// }

// STATIC bool discover_descriptors(uint16_t conn_handle, BLERemoteDevice* device) {
//     uint16_t reqStart_handle = 0x0001;
//     uint16_t reqEnd_handle = 0xffff;

//     uint8_t response_buffer[max_mtu];

//     int serviceCount = device->serviceCount();

//     for (size_t i = 0; i < serviceCount; i++) {
//         BLERemoteService* service = device->service(i);

//         uint16_t serviceEnd_handle = service->end_handle();

//         int characteristicCount = service->characteristicCount();

//         for (int j = 0; j < characteristicCount; j++) {
//             BLERemoteCharacteristic* characteristic = service->characteristic(j);
//             BLERemoteCharacteristic* nextCharacteristic = (j == (characteristicCount - 1)) ? NULL : service->characteristic(j);

//             reqStart_handle = characteristic->value_handle() + 1;
//             reqEnd_handle = nextCharacteristic ? nextCharacteristic->value_handle() : serviceEnd_handle;

//             if (reqStart_handle > reqEnd_handle) {
//                 continue;
//             }

//             while (1) {
//                 int respLength = findInfoReq(conn_handle, reqStart_handle, reqEnd_handle, response_buffer);

//                 if (respLength == 0) {
//                     return false;
//                 }

//                 if (response_buffer[0] == BT_ATT_OP_FIND_INFO_RSP) {
//                     uint16_t lengthPerDescriptor = response_buffer[1] * 4;
//                     uint8_t uuidLen = 2;

//                     for (size_t i = 2; i < respLength; i += lengthPerDescriptor) {
//                         struct __attribute__ ((packed)) RawDescriptor {
//                             uint16_t handle;
//                             uint8_t uuid[16];
//                         } *rawDescriptor = (RawDescriptor*)&response_buffer[i];

//                         BLERemoteDescriptor* descriptor = new BLERemoteDescriptor(rawDescriptor->uuid, uuidLen,
//                                                                                   conn_handle,
//                                                                                   rawDescriptor->handle);

//                         if (descriptor == NULL) {
//                             return false;
//                         }

//                         characteristic->addDescriptor(descriptor);

//                         reqStart_handle = rawDescriptor->handle + 1;
//                     }
//                 } else {
//                     break;
//                 }
//             }
//         }
//     }

//     return true;
// }

bool att_discover_attributes(bt_addr_le_t *addr, const char* service_uuid_filter) {
    uint16_t conn_handle = att_conn_handle(addr);
    if (conn_handle == 0xffff) {
        return false;
    }

    // send MTU request
    if (!att_exchange_mtu(conn_handle)) {
        return false;
    }

    // find the device entry for the peeer
    //FIX BLERemoteDevice* device = NULL;

    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
    //     if (bleio_connections[i].conn_handle == conn_handle) {
    //         //FIX if (bleio_connections[i].device == NULL) {
    //             //FIX
    //             //bleio_connections[i].device = new BLERemoteDevice();
    //         //}

    //         //device = bleio_connections[i].device;

    //         break;
    //     }
    // }

    // //FIX if (device == NULL) {
    // //     return false;
    // // }

    // if (service_uuid_filter == NULL) {
    //     // clear existing services
    //     //FIX device->clear_services();
    // } else {
    //     //FIX int service_count = device->service_count();

    //     for (size_t i = 0; i < service_count; i++) {
    //         //FIX BLERemoteService* service = device->service(i);

    //         if (strcasecmp(service->uuid(), service_uuid_filter) == 0) {
    //             // found an existing service with same UUID
    //             return true;
    //         }
    //     }
    }

    // discover services
    //FIX
    // if (!att_discover_services(conn_handle, device, service_uuid_filter)) {
    //     return false;
    // }

    // // discover characteristics
    // if (!discover_characteristics(conn_handle, device)) {
    //     return false;
    // }

    // // discover descriptors396
    // if (!discover_descriptors(conn_handle, device)) {
    //     return false;
    // }

    return true;
}

void att_set_max_mtu(uint16_t max_mtu_in) {
    max_mtu = max_mtu_in;
}

void att_set_timeout(unsigned long timeout_in) {
    timeout = timeout_in;
}

void att_add_connection(uint16_t handle, uint8_t role, bt_addr_le_t *peer_addr, uint16_t interval, uint16_t latency, uint16_t supervision_timeout, uint8_t master_clock_accuracy) {
    (void) interval;
    (void) latency;
    (void) supervision_timeout;
    (void) master_clock_accuracy;

    int peer_index = -1;

    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        if (bleio_connections[i].conn_handle == 0xffff) {
            peer_index = i;
            break;
        }
    }

    if (peer_index == -1) {
        // bail, no space
        return;
    }

    bleio_connections[peer_index].conn_handle = handle;
    bleio_connections[peer_index].role = role;
    bleio_connections[peer_index].mtu = BT_ATT_DEFAULT_LE_MTU;
    memcpy(&bleio_connections[peer_index].addr, peer_addr, sizeof(bleio_connections[peer_index].addr));

    //FIX if (event_handlers[BLEConnected]) {
    //     event_handlers[BLEConnected](BLEDevice(peer_bdaddr_type, peer_bdaddr));
    // }
}


void att_remove_connection(uint16_t handle, uint8_t reason) {
    (void) reason;
    int peer_index = -1;
    int peer_count = 0;

    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        if (bleio_connections[i].conn_handle == handle) {
            peer_index = i;
        }

        if (bleio_connections[i].conn_handle != 0xffff) {
            peer_count++;
        }
    }

    if (peer_index == -1) {
        // bail not found
        return;
    }

    //FIX BLEDevice bleDevice(bleio_connections[peer_index].address_type, bleio_connections[peer_index].address);

    if (peer_count == 1) {
        //FIX
        // clear CCCD values on disconnect
        // for (uint16_t i = 0; i < GATT.attributeCount(); i++) {
        //     BLELocalAttribute* attribute = GATT.attribute(i);

        //     if (attribute->type() == BLE_TYPE_CHARACTERISTIC) {
        //         BLELocalCharacteristic* characteristic = (BLELocalCharacteristic*)attribute;

        //         characteristic->writeCccdValue(bleDevice, 0x0000);
        //     }
        // }

        long_write_handle = BLE_GATT_HANDLE_INVALID;
        long_write_value_length = 0;
    }

    //FIX
    // if (event_handlers[BLEDisconnected]) {
    //     event_handlers[BLEDisconnected](bleDevice);
    // }

    bleio_connections[peer_index].conn_handle = 0xffff;
    bleio_connections[peer_index].role = 0x00;
    memset(&bleio_connections[peer_index].addr, 0x00, sizeof(bleio_connections[peer_index].addr));
    bleio_connections[peer_index].mtu = BT_ATT_DEFAULT_LE_MTU;

    //FIX if (bleio_connections[peer_index].device) {
        //FIX delete bleio_connections[peer_index].device;
    // }
    //FIX bleio_connections[peer_index].device = NULL;
}

uint16_t att_conn_handle(bt_addr_le_t *addr) {
    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        if (bleio_connections[i].addr.type == addr->type &&
            memcmp(&bleio_connections[i].addr.a.val, addr->a.val, sizeof(addr->a.val)) == 0) {
            return bleio_connections[i].conn_handle;
        }
    }

    return 0xffff;
}

//FIX
// BLERemoteDevice* att_device(uint8_t address_type, const uint8_t address[6]) {
//     for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
//        if (bleio_connections[i].addr.type == addr->type &&
//            memcmp(&bleio_connections[i].addr.a.val, addr->a.val, sizeof(addr->a.val)) == 0) {
//         }
//     }

//     return NULL;
// }

bool att_is_connected(void) {
    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        if (bleio_connections[i].conn_handle != 0xffff) {
            return true;
        }
    }

    return false;
}

bool att_address_is_connected(bt_addr_le_t *addr) {
    return (att_conn_handle(addr) != 0xffff);
}

bool att_handle_is_connected(uint16_t handle) {
    hci_poll_for_incoming_pkt();

    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        if (bleio_connections[i].conn_handle == handle) {
            return true;
        }
    }

    return false;
}

uint16_t att_mtu(uint16_t handle) {
    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        if (bleio_connections[i].conn_handle == handle) {
            return bleio_connections[i].mtu;
        }
    }

    return BT_ATT_DEFAULT_LE_MTU;
}

bool att_disconnect_all(void) {
    int num_disconnects = 0;

    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        if (bleio_connections[i].conn_handle == 0xffff) {
            continue;
        }

        if (hci_disconnect(bleio_connections[i].conn_handle) != 0) {
            continue;
        }

        num_disconnects++;

        bleio_connections[i].conn_handle = 0xffff;
        bleio_connections[i].role = 0x00;
        bleio_connections[i].addr.type = 0;
        memset(bleio_connections[i].addr.a.val, 0, sizeof(bleio_connections[i].addr.a.val));
        bleio_connections[i].mtu = BT_ATT_DEFAULT_LE_MTU;

        //FIX
        // if (bleio_connections[i].device) {
        //     delete bleio_connections[i].device;
        // }
        // bleio_connections[i].device = NULL;
    }

    return (num_disconnects > 0);
}

// FIX
// BLEDevice att_central() {
//     for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
//         if (bleio_connections[i].conn_handle == 0xffff || bleio_connections[i].role != 0x01) {
//             continue;
//         }

//         return BLEDevice(bleio_connections[i].address_type, bleio_connections[i].address);
//     }

//     return BLEDevice();
// }

bool att_notify(uint16_t handle, const uint8_t* value, int length) {
    int num_notifications = 0;

    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        if (bleio_connections[i].conn_handle == 0xffff) {
            continue;
        }

        typedef struct __packed {
            struct bt_att_hdr hdr;
            struct bt_att_notify ntf;
        } notify_t;

        size_t allowed_length = MIN((uint16_t)(bleio_connections[i].mtu - sizeof(notify_t)), (uint16_t)length);

        uint8_t notify_bytes[sizeof(notify_t) + allowed_length];
        notify_t *notify = (notify_t *) notify_bytes;
        notify->hdr.code = BT_ATT_OP_NOTIFY;;
        notify->ntf.handle = handle;
        memcpy(notify->ntf.value, value, allowed_length);
        hci_send_acl_pkt(bleio_connections[i].conn_handle, BT_L2CAP_CID_ATT,
                         sizeof(notify_bytes), notify_bytes);

        num_notifications++;
    }

    return (num_notifications > 0);
}

bool att_indicate(uint16_t handle, const uint8_t* value, int length) {
    int num_indications = 0;

    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        if (bleio_connections[i].conn_handle == 0xffff) {
            continue;
        }

        typedef struct __packed {
            struct bt_att_hdr hdr;
            struct bt_att_indicate ind;
        } indicate_t;

        size_t allowed_length = MIN((uint16_t)(bleio_connections[i].mtu - sizeof(indicate_t)), (uint16_t)length);

        uint8_t indicate_bytes[sizeof(indicate_t) + allowed_length];
        indicate_t *indicate = (indicate_t *) indicate_bytes;
        indicate->hdr.code = BT_ATT_OP_INDICATE;;
        indicate->ind.handle = handle;
        memcpy(indicate->ind.value, value, allowed_length);

        confirm = false;

        hci_send_acl_pkt(bleio_connections[i].conn_handle, BT_L2CAP_CID_ATT,
                         sizeof(indicate_bytes), indicate_bytes);

        while (!confirm) {
            hci_poll_for_incoming_pkt();

            if (!att_address_is_connected(&bleio_connections[i].addr)) {
                break;
            }
        }

        num_indications++;
    }

    return (num_indications > 0);
}

STATIC void process_error(uint16_t conn_handle, uint8_t dlen, uint8_t data[]) {
    struct bt_att_error_rsp *rsp = (struct bt_att_error_rsp *) data;
    if (dlen != sizeof(struct bt_att_error_rsp)) {
        // Incorrect size; ignore.
        return;
    }

    // expected_rsp.opcode is an RSP opcode. Does it match the REQ opcode in this response?
    if (expected_rsp.conn_handle == conn_handle && (expected_rsp.opcode - 1) == rsp->request) {
        expected_rsp.buffer[0] = BT_ATT_OP_ERROR_RSP;
        memcpy(&expected_rsp.buffer[1], data, dlen);
        expected_rsp.length = dlen + 1;
    }
}

STATIC void process_mtu_req(uint16_t conn_handle, uint8_t dlen, uint8_t data[]) {
    struct bt_att_exchange_mtu_req *req = (struct bt_att_exchange_mtu_req *) data;
    if (dlen != sizeof(req)) {
        send_error(conn_handle, BT_ATT_OP_MTU_REQ, BLE_GATT_HANDLE_INVALID, BT_ATT_ERR_INVALID_PDU);
        return;
    }

    uint16_t mtu = req->mtu;

    if (mtu > max_mtu) {
        mtu = max_mtu;
    }

    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        if (bleio_connections[i].conn_handle == conn_handle) {
            bleio_connections[i].mtu = mtu;
            break;
        }
    }

    struct __packed {
        struct bt_att_hdr h;
        struct bt_att_exchange_mtu_req r;
    } rsp = { {
            .code = BT_ATT_OP_MTU_RSP,
        }, {
            .mtu = mtu,
        }
    };

    hci_send_acl_pkt(conn_handle, BT_L2CAP_CID_ATT, sizeof(rsp), (uint8_t *) &rsp);
}

STATIC void process_mtu_rsp(uint16_t conn_handle, uint8_t dlen, uint8_t data[]) {
    if (dlen != sizeof(struct bt_att_exchange_mtu_rsp)) {
        return;
    }

    struct bt_att_exchange_mtu_rsp *rsp = (struct bt_att_exchange_mtu_rsp *) data;

    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        if (bleio_connections[i].conn_handle == conn_handle) {
            bleio_connections[i].mtu = rsp->mtu;
            break;
        }
    }

    check_and_save_expected_rsp(conn_handle, BT_ATT_OP_MTU_RSP, dlen, data);
}

STATIC void process_find_info_req(uint16_t conn_handle, uint16_t mtu, uint8_t dlen, uint8_t data[]) {
    struct bt_att_find_info_req *req = (struct bt_att_find_info_req *) data;

    if (dlen != sizeof(struct bt_att_find_info_req)) {
        send_error(conn_handle, BT_ATT_OP_FIND_INFO_REQ, req->start_handle, BT_ATT_ERR_INVALID_PDU);
        return;
    }

    //FIX
    // uint8_t response[mtu];
    // uint16_t response_length;

    // response[0] = BT_ATT_OP_FIND_INFO_RSP;
    // response[1] = 0x00;
    // response_length = 2;

    // for (uint16_t i = (req->start_handle - 1); i < GATT.attributeCount() && i <= (req->end_handle - 1); i++) {
    //     BLELocalAttribute* attribute = GATT.attribute(i);
    //     uint16_t handle = (i + 1);
    //     bool is_value_handle = (attribute->type() == BLE_TYPE_CHARACTERISTIC) && (((BLELocalCharacteristic*)attribute)->valueHandle() == handle);
    //     int uuid_len = is_value_handle ? 2 : attribute->uuid_length();
    //     size_t info_type = (uuidLen == 2) ? 0x01 : 0x02;

    //     if (response[1] == 0) {
    //         response[1] = info_type;
    //     }

    //     if (response[1] != info_type) {
    //         // different type
    //         break;
    //     }

    //     // add the handle
    //     memcpy(&response[response_length], &handle, sizeof(handle));
    //     response_length += sizeof(handle);

    //     if (is_value_handle || attribute->type() == BLE_TYPE_DESCRIPTOR) {
    //         // add the UUID
    //         memcpy(&response[response_length], attribute->uuid_data(), uuid_len);
    //         response_length += uuidLen;
    //     } else {
    //         // add the type
    //         uint16_t type = attribute->type();

    //         memcpy(&response[response_length], &type, sizeof(type));
    //         response_length += sizeof(type);
    //     }

    //     if ((response_length + (2 + uuidLen)) > mtu) {
    //         break;
    //     }
    // }

    // if (response_length == 2) {
    //     send_error(conn_handle, BT_ATT_OP_FIND_INFO_REQ, findInfoReq->start_handle, BT_ATT_ERR_ATTR_NOT_FOUND);
    // } else {
    //     hci_send_acl_pkt(conn_handle, BT_L2CAP_CID_ATT, response_length, response);
    // }
}

int att_find_info_req(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle, uint8_t response_buffer[]) {
    struct __packed req {
        struct bt_att_hdr h;
        struct bt_att_find_info_req r;
    } req = { {
            .code = BT_ATT_OP_FIND_INFO_REQ,
        }, {
            .start_handle = start_handle,
            .end_handle = end_handle,
        }
    };

    return send_req_wait_for_rsp(conn_handle, sizeof(req), (uint8_t *) &req, response_buffer);
}

STATIC void process_find_info_rsp(uint16_t conn_handle, uint8_t dlen, uint8_t data[]) {
    if (dlen < 2) {
        return; // invalid, drop
    }

    check_and_save_expected_rsp(conn_handle, BT_ATT_OP_FIND_INFO_RSP, dlen, data);
}

STATIC void process_find_by_type_req(uint16_t conn_handle, uint16_t mtu, uint8_t dlen, uint8_t data[]) {
    struct bt_att_find_type_req *req = (struct bt_att_find_type_req *) data;

    if (dlen < sizeof(struct bt_att_find_type_req)) {
        send_error(conn_handle, BT_ATT_OP_FIND_TYPE_RSP, req->start_handle, BT_ATT_ERR_INVALID_PDU);
        return;
    }

    uint8_t response[mtu];
    uint16_t response_length;

    response[0] = BT_ATT_OP_FIND_TYPE_RSP;
    response_length = 1;

    //FIX
    // if (find_by_type_req->type == BLE_TYPE_PRIMARY_SERVICE) {
    //     for (uint16_t i = (find_by_type_req->start_handle - 1); i < GATT.attributeCount() && i <= (find_by_type_req->end_handle - 1); i++) {
    //         BLELocalAttribute* attribute = GATT.attribute(i);

    //         if ((attribute->type() == find_by_type_req->type) && (attribute->uuidLength() == value_length) && memcmp(attribute->uuidData(), value, value_length) == 0) {
    //             BLELocalService* service = (BLELocalService*)attribute;

    //             // add the start handle
    //             uint16_t start_handle = service->start_handle();
    //             memcpy(&response[response_length], &start_handle, sizeof(start_handle));
    //             response_length += sizeof(start_handle);

    //             // add the end handle
    //             uint16_t end_handle = service->end_handle();
    //             memcpy(&response[response_length], &end_handle, sizeof(end_handle));
    //             response_length += sizeof(end_handle);
    //         }

    //         if ((response_length + 4) > mtu) {
    //             break;
    //         }
    //     }
    // }

    if (response_length == 1) {
        send_error(conn_handle, BT_ATT_OP_FIND_TYPE_RSP, req->start_handle, BT_ATT_ERR_ATTRIBUTE_NOT_FOUND);
    } else {
        hci_send_acl_pkt(conn_handle, BT_L2CAP_CID_ATT, response_length, response);
    }
}

void process_read_by_group_req(uint16_t conn_handle, uint16_t mtu, uint8_t dlen, uint8_t data[]) {
    struct bt_att_read_group_req *req = (struct bt_att_read_group_req *) data;
    uint16_t uuid = req->uuid[0] | (req->uuid[1] << 8);

    if (dlen != sizeof(struct bt_att_find_type_req) ||
        (uuid != BLE_TYPE_PRIMARY_SERVICE &&
         uuid != BLE_TYPE_SECONDARY_SERVICE)) {
        send_error(conn_handle, BT_ATT_OP_READ_GROUP_REQ, req->start_handle, BT_ATT_ERR_UNSUPPORTED_GROUP_TYPE);
        return;
    }

    uint8_t response[mtu];
    uint16_t response_length;

    response[0] = BT_ATT_OP_READ_GROUP_RSP;
    response[1] = 0x00;
    response_length = 2;

    // FIX
    // for (uint16_t i = (readByGroupReq->start_handle - 1); i < GATT.attributeCount() && i <= (readByGroupReq->end_handle - 1); i++) {
        //FIX
        // BLELocalAttribute* attribute = GATT.attribute(i);

        // if (readByGroupReq->uuid != attribute->type()) {
        //     // not the type
        //     continue;
        // }

        // int uuidLen = attribute->uuidLength();
        // size_t infoSize = (uuidLen == 2) ? 6 : 20;

        // if (response[1] == 0) {
        //     response[1] = infoSize;
        // }

        // if (response[1] != infoSize) {
        //     // different size
        //     break;
        // }

        // BLELocalService* service = (BLELocalService*)attribute;

        // // add the start handle
        // uint16_t start_handle = service->start_handle();
        // memcpy(&response[response_length], &start_handle, sizeof(start_handle));
        // response_length += sizeof(start_handle);

        // // add the end handle
        // uint16_t end_handle = service->end_handle();
        // memcpy(&response[response_length], &end_handle, sizeof(end_handle));
        // response_length += sizeof(end_handle);

        // // add the UUID
        // memcpy(&response[response_length], service->uuidData(), uuidLen);
        // response_length += uuidLen;

        // if ((response_length + infoSize) > mtu) {
        //     break;
        // }
    // }

    if (response_length == 2) {
        send_error(conn_handle, BT_ATT_OP_READ_GROUP_REQ, req->start_handle, BT_ATT_ERR_ATTRIBUTE_NOT_FOUND);
    } else {
        hci_send_acl_pkt(conn_handle, BT_L2CAP_CID_ATT, response_length, response);
    }
}

int att_read_by_group_req(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle, uint16_t uuid, uint8_t response_buffer[]) {
    struct __packed {
        struct bt_att_hdr h;
        struct bt_att_read_group_req r;
    } req = { {
            .code = BT_ATT_OP_ERROR_RSP,
        }, {
            .start_handle = start_handle,
            .end_handle = end_handle,
        }
    };
    req.r.uuid[0] = uuid & 0xff;
    req.r.uuid[1] = uuid >> 8;


    return send_req_wait_for_rsp(conn_handle, sizeof(req), (uint8_t *) &req, response_buffer);
}

STATIC void process_read_by_group_rsp(uint16_t conn_handle, uint8_t dlen, uint8_t data[]) {
    if (dlen < 2) {
        return; // invalid, drop
    }

    check_and_save_expected_rsp(conn_handle, BT_ATT_OP_READ_GROUP_RSP, dlen, data);
}

STATIC void process_read_or_read_blob_req(uint16_t conn_handle, uint16_t mtu, uint8_t opcode, uint8_t dlen, uint8_t data[]) {
    uint16_t handle;
    uint16_t offset = 0;
    uint8_t response_opcode;

    if (opcode == BT_ATT_OP_READ_REQ) {
        if (dlen != sizeof(struct bt_att_read_req)) {
            send_error(conn_handle, BT_ATT_OP_READ_REQ, BLE_GATT_HANDLE_INVALID, BT_ATT_ERR_INVALID_PDU);
            return;
        }

        struct bt_att_read_req *req = (struct bt_att_read_req *) data;
        handle = req->handle;
        response_opcode = BT_ATT_OP_READ_RSP;

    } else {
        if (dlen != sizeof(struct bt_att_read_blob_req)) {
            send_error(conn_handle, BT_ATT_OP_READ_BLOB_REQ, BLE_GATT_HANDLE_INVALID, BT_ATT_ERR_INVALID_PDU);
            return;
        }

        struct bt_att_read_blob_req *req = (struct bt_att_read_blob_req *) data;
        handle = req->handle;
        offset = req->offset;
        response_opcode = BT_ATT_OP_READ_BLOB_RSP;
    }

    //FIX
    (void) offset;
    (void) handle;
    //FIX if ((uint16_t)(handle - 1) > GATT.attributeCount()) {
    //     send_error(conn_handle, opcode, handle, BT_ATT_ERR_ATTR_NOT_FOUND);
    //     return;
    // }

    uint8_t response[mtu];
    uint16_t response_length;

    response[0] = response_opcode;
    response_length = 1;

    //FIX BLELocalAttribute* attribute = GATT.attribute(handle - 1);
    // enum BLEAttributeType attributeType = attribute->type();

    // if (attributeType == BLE_TYPE_PRIMARY_SERVICE) {
    //     if (offset) {
    //         send_error(conn_handle, BT_ATT_ERR_ATTR_NOT_LONG, handle, BT_ATT_ERR_INVALID_PDU);
    //         return;
    //     }

    //     BLELocalService* service = (BLELocalService*)attribute;

    //     // add the UUID
    //     uint8_t uuidLen = service->uuidLength();
    //     memcpy(&response[response_length], service->uuidData(), uuidLen);
    //     response_length += uuidLen;
    // } else if (attributeType == BLE_TYPE_CHARACTERISTIC) {
    //     BLELocalCharacteristic* characteristic = (BLELocalCharacteristic*)attribute;

    //     if (characteristic->handle() == handle) {
    //         if (offset) {
    //             send_error(conn_handle, opcode, handle, BT_ATT_ERR_ATTR_NOT_LONG);
    //             return;
    //         }

    //         // add the properties
    //         response[response_length++] = characteristic->properties();

    //         // add the value handle
    //         uint16_t value_handle = characteristic->value_handle();
    //         memcpy(&response[response_length], &value_handle, sizeof(value_handle));
    //         response_length += sizeof(value_handle);

    //         // add the UUID
    //         uint8_t uuidLen = characteristic->uuidLength();
    //         memcpy(&response[response_length], characteristic->uuidData(), uuidLen);
    //         response_length += uuidLen;
    //     } else {
    //         if ((characteristic->properties() & BLERead) == 0) {
    //             send_error(conn_handle, opcode, handle, BT_ATT_ERR_READ_NOT_PERM);
    //             return;
    //         }

    //         uint16_t value_length = characteristic->value_length();

    //         if (offset >= value_length) {
    //             send_error(conn_handle, opcode, handle, BT_ATT_ERR_INVALID_OFFSET);
    //             return;
    //         }

    //         value_length = min(mtu - response_length, value_length - offset);

    //         for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
    //             if (bleio_connections[i].conn_handle == conn_handle) {
    //                 // FIX characteristic->readValue(BLEDevice(bleio_connections[i].address_type, bleio_connections[i].address), offset, &response[response_length], value_length);
    //                 response_length += value_length;
    //             }
    //         }
    //     }
    // } else if (attributeType == BLE_TYPE_DESCRIPTOR) {
    //     BLELocalDescriptor* descriptor = (BLELocalDescriptor*)attribute;

    //     uint16_t value_length = descriptor->valueSize();

    //     if (offset >= value_length) {
    //         send_error(conn_handle, opcode, handle, BT_ATT_ERR_INVALID_OFFSET);
    //         return;
    //     }

    //     value_length = min(mtu - response_length, value_length - offset);

    //     memcpy(&response[response_length], descriptor->value() + offset, value_length);
    //     response_length += value_length;
    // }

    hci_send_acl_pkt(conn_handle, BT_L2CAP_CID_ATT, response_length, response);
}

STATIC void process_read_rsp(uint16_t conn_handle, uint8_t dlen, uint8_t data[]) {
    check_and_save_expected_rsp(conn_handle, BT_ATT_OP_READ_RSP, dlen, data);
}

STATIC void process_read_by_type_req(uint16_t conn_handle, uint16_t mtu, uint8_t dlen, uint8_t data[]) {
    struct bt_att_read_type_req *req = (struct bt_att_read_type_req *) data;

    if (dlen != sizeof(struct bt_att_read_type_req)) {
        send_error(conn_handle, BT_ATT_OP_READ_TYPE_REQ, req->start_handle, BT_ATT_ERR_INVALID_PDU);
        return;
    }

    // uint8_t response[mtu];
    // uint16_t response_length;

    // response[0] = BT_ATT_OP_READ_TYPE_RSP;
    // response[1] = 0x00;
    // response_length = 2;

    // for (uint16_t i = (req->start_handle - 1); i < GATT.attributeCount() && i <= (req->end_handle - 1); i++) {
    //     BLELocalAttribute* attribute = GATT.attribute(i);
    //     uint16_t handle = (i + 1);

    //     if (attribute->type() == readByTypeReq->uuid) {
    //         if (attribute->type() == BLE_TYPE_CHARACTERISTIC) {
    //             BLELocalCharacteristic* characteristic = (BLELocalCharacteristic*)attribute;

    //             if (characteristic->value_handle() == handle) {
    //                 // value handle, skip
    //                 continue;
    //             }

    //             int uuidLen = attribute->uuidLength();
    //             int typeSize = (uuidLen == 2) ? 7 : 21;

    //             if (response[1] == 0) {
    //                 response[1] = typeSize;
    //             }

    //             if (response[1] != typeSize) {
    //                 // all done, wrong size
    //                 break;
    //             }

    //             // add the handle
    //             memcpy(&response[response_length], &handle, sizeof(handle));
    //             response_length += sizeof(handle);

    //             // add the properties
    //             response[response_length++] = characteristic->properties();

    //             // add the value handle
    //             uint16_t value_handle = (handle + 1);
    //             memcpy(&response[response_length], &value_handle, sizeof(value_handle));
    //             response_length += sizeof(value_handle);

    //             // add the UUID
    //             memcpy(&response[response_length], characteristic->uuidData(), uuidLen);
    //             response_length += uuidLen;

    //             // skip the next handle, it's a value handle
    //             i++;

    //             if ((response_length + typeSize) > mtu) {
    //                 break;
    //             }
    //         } else if (attribute->type() == 0x2902) {
    //             BLELocalDescriptor* descriptor = (BLELocalDescriptor*)attribute;

    //             // add the handle
    //             memcpy(&response[response_length], &handle, sizeof(handle));
    //             response_length += sizeof(handle);

    //             // add the value
    //             int valueSize = min((uint16_t)(mtu - response_length), (uint16_t)descriptor->valueSize());
    //             memcpy(&response[response_length], descriptor->value(), valueSize);
    //             response_length += valueSize;

    //             response[1] = 2 + valueSize;

    //             break; // all done
    //         }
    //     } else if (attribute->type() == BLE_TYPE_CHARACTERISTIC && attribute->uuidLength() == 2 && memcmp(&readByTypeReq->uuid, attribute->uuidData(), 2) == 0) {
    //         BLELocalCharacteristic* characteristic = (BLELocalCharacteristic*)attribute;

    //         // add the handle
    //         memcpy(&response[response_length], &handle, sizeof(handle));
    //         response_length += sizeof(handle);

    //         // add the value
    //         int value_length = min((uint16_t)(mtu - response_length), (uint16_t)characteristic->value_length());
    //         memcpy(&response[response_length], characteristic->value(), value_length);
    //         response_length += value_length;

    //         response[1] = 2 + value_length;

    //         break; // all done
    //     }
    // }

    // if (response_length == 2) {
    //     send_error(conn_handle, BT_ATT_OP_READ_BY_TYPE_REQ, readByTypeReq->start_handle, BT_ATT_ERR_ATTR_NOT_FOUND);
    // } else {
    //     hci_send_acl_pkt(conn_handle, BT_L2CAP_CID_ATT, response_length, response);
    // }
}

int att_read_by_type_req(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle, uint16_t type, uint8_t response_buffer[]) {
    struct __packed {
        struct bt_att_hdr h;
        struct bt_att_read_type_req r;
    } req = { {
            .code = BT_ATT_OP_READ_TYPE_REQ,
        }, {
            .start_handle = start_handle,
            .end_handle = end_handle,
        }
    };
    req.r.uuid[0] = type & 0xff;
    req.r.uuid[1] = type >> 8;

    return send_req_wait_for_rsp(conn_handle, sizeof(req), (uint8_t *) &req, response_buffer);
}

STATIC void process_read_by_type_rsp(uint16_t conn_handle, uint8_t dlen, uint8_t data[]) {
    if (dlen < 1) {
        return; // invalid, drop
    }

    check_and_save_expected_rsp(conn_handle, BT_ATT_OP_READ_TYPE_RSP, dlen, data);
}

// Handles BT_ATT_OP_WRITE_REQ or BT_ATT_OP_WRITE_
STATIC void process_write_req_or_cmd(uint16_t conn_handle, uint16_t mtu, uint8_t op, uint8_t dlen, uint8_t data[]) {
    // struct bt_att_write_cmd is identical, so don't bother to split code paths based on opcode.
    //FIX REMOVE this later struct bt_att_write_req *req = (struct bt_att_write_req *) data;
    bool with_response = (op == BT_ATT_OP_WRITE_REQ);

    if (dlen < sizeof(struct bt_att_write_req)) {
        if (with_response) {
            send_error(conn_handle, BT_ATT_OP_WRITE_REQ, BLE_GATT_HANDLE_INVALID, BT_ATT_ERR_INVALID_PDU);
        }
        return;
    }

    //FIX why cast?
    // if ((uint16_t)(req->handle - 1) > GATT.attributeCount()) {
    //     if (with_response) {
    //         send_error(conn_handle, BT_ATT_OP_WRITE_REQ, handle, BT_ATT_ERR_ATTR_NOT_FOUND);
    //     }
    //     return;
    // }

    // uint8_t value_length = dlen - sizeof(req->handle);
    // uint8_t* value = &data[sizeof(req->handle)];

    // BLELocalAttribute* attribute = GATT.attribute(req->handle - 1);

    // if (attribute->type() == BLE_TYPE_CHARACTERISTIC) {
    //     BLELocalCharacteristic* characteristic = (BLELocalCharacteristic*)attribute;

    //     if (req->handle != characteristic->value_handle() ||
    //         withResponse ? ((characteristic->properties() & BLEWrite) == 0) :
    //         ((characteristic->properties() & BLEWriteWithoutResponse) == 0)) {
    //         if (withResponse) {
    //             send_error(conn_handle, BT_ATT_OP_WRITE_REQ, handle, BT_ATT_ERR_WRITE_NOT_PERM);
    //         }
    //         return;
    //     }

    //     for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
    //         if (bleio_connections[i].conn_handle == conn_handle) {
    //             // FIX characteristic->writeValue(BLEDevice(bleio_connections[i].address_type, bleio_connections[i].address), value, value_length);
    //             break;
    //         }
    //     }
    // } else if (attribute->type() == BLE_TYPE_DESCRIPTOR) {
    //     BLELocalDescriptor* descriptor = (BLELocalDescriptor*)attribute;

    //     // only CCCD's are writable
    //     if (descriptor->uuidLength() != 2 || *((uint16_t*)(descriptor->uuidData())) != 0x2902) {
    //         if (withResponse) {
    //             send_error(conn_handle, BT_ATT_OP_WRITE_REQ, handle, BT_ATT_ERR_WRITE_NOT_PERM);
    //         }
    //         return;
    //     }

    //     // get the previous handle, should be the characteristic for the CCCD
    //     attribute = GATT.attribute(handle - 2);

    //     if (attribute->type() != BLE_TYPE_CHARACTERISTIC) {
    //         if (withResponse) {
    //             send_error(conn_handle, BT_ATT_OP_WRITE_REQ, handle, BT_ATT_ERR_WRITE_NOT_PERM);
    //         }
    //         return;
    //     }

    //     BLELocalCharacteristic* characteristic = (BLELocalCharacteristic*)attribute;

    //     for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
    //         if (bleio_connections[i].conn_handle == conn_handle) {
    //             //FIX characteristic->writeCccdValue(BLEDevice(bleio_connections[i].address_type, bleio_connections[i].address), *((uint16_t*)value));
    //             break;
    //         }
    //     }
    // } else {
    //     if (withResponse) {
    //         send_error(conn_handle, BT_ATT_OP_WRITE_REQ, handle, BT_ATT_ERR_WRITE_NOT_PERM);
    //     }
    //     return;
    // }

    if (with_response) {
        uint8_t response[mtu];
        uint16_t response_length;

        response[0] = BT_ATT_OP_WRITE_RSP;
        response_length = 1;

        hci_send_acl_pkt(conn_handle, BT_L2CAP_CID_ATT, response_length, response);
    }
}

STATIC void process_write_rsp(uint16_t conn_handle, uint8_t dlen, uint8_t data[]) {
    if (dlen != 0) {
        return; // drop
    }

    check_and_save_expected_rsp(conn_handle, BT_ATT_OP_WRITE_RSP, dlen, data);
}

STATIC void process_prepare_write_req(uint16_t conn_handle, uint16_t mtu, uint8_t dlen, uint8_t data[]) {
    struct bt_att_prepare_write_req *req = (struct bt_att_prepare_write_req *) data;

    if (dlen < sizeof(struct bt_att_prepare_write_req)) {
        send_error(conn_handle, BT_ATT_OP_PREPARE_WRITE_REQ, BLE_GATT_HANDLE_INVALID, BT_ATT_ERR_INVALID_PDU);
        return;
    }

    uint16_t handle = req->handle;
    uint16_t offset = req->offset;
    (void) offset;

    if (handle > bleio_adapter_max_attribute_handle(&common_hal_bleio_adapter_obj)) {
        send_error(conn_handle, BT_ATT_OP_PREPARE_WRITE_REQ, handle, BT_ATT_ERR_ATTRIBUTE_NOT_FOUND);
        return;
    }

    mp_obj_t *attribute = bleio_adapter_get_attribute(&common_hal_bleio_adapter_obj, handle);

    if (!MP_OBJ_IS_TYPE(attribute, &bleio_characteristic_type)) {
        send_error(conn_handle, BT_ATT_OP_PREPARE_WRITE_REQ, handle, BT_ATT_ERR_ATTRIBUTE_NOT_LONG);
        return;
    }

    bleio_characteristic_obj_t* characteristic = MP_OBJ_TO_PTR(attribute);

    if (handle != characteristic->handle) {
        send_error(conn_handle, BT_ATT_OP_PREPARE_WRITE_REQ, handle, BT_ATT_ERR_ATTRIBUTE_NOT_LONG);
        return;
    }

    if (characteristic->props & CHAR_PROP_WRITE) {
        send_error(conn_handle, BT_ATT_OP_PREPARE_WRITE_REQ, handle, BT_ATT_ERR_WRITE_NOT_PERMITTED);
        return;
    }

    //FIX if (long_write_handle == BLE_GATT_HANDLE_INVALID)
    //     int valueSize = characteristic->valueSize();

    //     long_write_value = (uint8_t*)realloc(long_write_value, valueSize);
    //     long_write_value_length = 0;
    //     long_write_handle = handle;

    //     memset(long_write_value, 0x00, valueSize);
    // } else if (long_write_handle != handle) {
    //     send_error(conn_handle, BT_ATT_OP_PREPARE_WRITE_REQ, handle, BT_ATT_ERR_UNLIKELY);
    //     return;
    // }

    // uint8_t value_length = dlen - sizeof(struct bt_att_prepare_write_req);
    // uint8_t* value = &data[sizeof(struct bt_att_prepare_write_req)];

    // if ((offset != long_write_value_length) || ((offset + value_length) > (uint16_t)characteristic->valueSize())) {
    //     send_error(conn_handle, BT_ATT_OP_PREPARE_WRITE_REQ, handle, BT_ATT_ERR_INVALID_OFFSET);
    //     return;
    // }

    // memcpy(long_write_value + offset, value, value_length);
    // long_write_value_length += value_length;

    // uint8_t response[mtu];
    // uint16_t response_length;

    // response[0] = BT_ATT_OP_PREP_WRITE_RSP;
    // memcpy(&response[1], data, dlen);
    // response_length = dlen + 1;

    // hci_send_acl_pkt(conn_handle, BT_L2CAP_CID_ATT, response_length, response);
}

STATIC void process_exec_write_req(uint16_t conn_handle, uint16_t mtu, uint8_t dlen, uint8_t data[]) {
    struct bt_att_exec_write_req *req = (struct bt_att_exec_write_req *) data;

    if (dlen != sizeof(struct bt_att_exec_write_req)) {
        send_error(conn_handle, BT_ATT_OP_EXEC_WRITE_REQ, BLE_GATT_HANDLE_INVALID, BT_ATT_ERR_INVALID_PDU);
        return;
    }

    if (long_write_handle && (req->flags & 0x01)) {
        //FIX BLELocalCharacteristic* characteristic = (BLELocalCharacteristic*)GATT.attribute(long_write_handle - 1);

        for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
            if (bleio_connections[i].conn_handle == conn_handle) {
                //FIX characteristic->writeValue(BLEDevice(bleio_connections[i].address_type, bleio_connections[i].address), long_write_value, long_write_value_length);
                break;
            }
        }
    }

    long_write_handle = BLE_GATT_HANDLE_INVALID;
    long_write_value_length = 0;

    uint8_t response[mtu];
    uint16_t response_length;

    response[0] = BT_ATT_OP_EXEC_WRITE_RSP;
    response_length = 1;

    hci_send_acl_pkt(conn_handle, BT_L2CAP_CID_ATT, response_length, response);
}

STATIC void process_handle_notify_or_indicate(uint16_t conn_handle, uint8_t opcode, uint8_t dlen, uint8_t data[]) {
    if (dlen < 2) {
        return; // drop
    }

    // struct bt_att_notify and bt_att_indicate are identical.
    //FIXunused struct bt_att_notify *req = (struct bt_att_notify *) data;

    //FIXunused uint8_t handle = req->handle;

    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        if (bleio_connections[i].conn_handle != conn_handle) {
            continue;
        }

        //FIX BLERemoteDevice* device = bleio_connections[i].device;

        // if (!device) {
        //     break;
        // }

        // int serviceCount = device->serviceCount();

        // for (size_t i = 0; i < serviceCount; i++) {
        //     BLERemoteService* s = device->service(i);

        //     if (s->start_handle() < handle && s->end_handle() >= handle) {
        //         int characteristicCount = s->characteristicCount();

        //         for (int j = 0; j < characteristicCount; j++) {
        //             BLERemoteCharacteristic* c = s->characteristic(j);

        //             if (c->value_handle() == handle) {
        //                 //FIX c->writeValue(BLEDevice(bleio_connections[i].address_type, bleio_connections[i].address), &data[2], dlen - 2);
        //             }
        //         }

        //         break;
        //     }
        // }
    }

    if (opcode == BT_ATT_OP_INDICATE) {
        // send CONFIRM for INDICATE

        uint8_t op_confirm = BT_ATT_OP_CONFIRM;

        hci_send_acl_pkt(conn_handle, BT_L2CAP_CID_ATT, sizeof(op_confirm), &op_confirm);
    }
}

STATIC void process_handle_confirm(uint16_t conn_handle, uint8_t dlen, uint8_t data[]) {
    (void) conn_handle;
    (void) dlen;
    (void) data;

    confirm = true;
}

bool att_exchange_mtu(uint16_t conn_handle) {
    uint8_t response_buffer[max_mtu];
    struct bt_att_exchange_mtu_req req = {
        .mtu = max_mtu,
    };
    return send_req_wait_for_rsp(conn_handle, sizeof(req), (uint8_t *) &req, response_buffer);
}



//FIX void att_set_event_handler(BLEDeviceEvent event, BLEDeviceEventHandler event_handler) {
//     if (event < (sizeof(event_handlers) / (sizeof(event_handlers[0])))) {
//         event_handlers[event] = event_handler;
//     }
// }

int att_read_req(uint16_t conn_handle, uint16_t handle, uint8_t response_buffer[]) {
    struct __packed {
        struct bt_att_hdr h;
        struct bt_att_read_req r;
    } req = { {
            .code = BT_ATT_OP_READ_REQ,
        }, {
            .handle = handle,
        }
    };

    return send_req_wait_for_rsp(conn_handle, sizeof(req), (uint8_t *) &req, response_buffer);
}

int att_write_req(uint16_t conn_handle, uint16_t handle, const uint8_t* data, uint8_t data_len, uint8_t response_buffer[]) {
    typedef struct __packed {
        struct bt_att_hdr h;
        struct bt_att_write_req r;
    } req_t;

    uint8_t req_bytes[sizeof(req_t) + data_len];
    req_t *req = (req_t *) req_bytes;
    req->h.code = BT_ATT_OP_WRITE_REQ;
    req->r.handle = handle;
    memcpy(req->r.value, data, data_len);

    return send_req_wait_for_rsp(conn_handle, sizeof(req_bytes), req_bytes, response_buffer);
}

void att_write_cmd(uint16_t conn_handle, uint16_t handle, const uint8_t* data, uint8_t data_len) {
    typedef struct __packed {
        struct bt_att_hdr h;
        struct bt_att_write_cmd r;
    } cmd_t;

    uint8_t cmd_bytes[sizeof(cmd_t) + data_len];
    cmd_t *cmd = (cmd_t *) cmd_bytes;
    cmd->h.code = BT_ATT_OP_WRITE_CMD;
    cmd->r.handle = handle;
    memcpy(cmd->r.value, data, data_len);

    return send_req(conn_handle, sizeof(cmd_bytes), cmd_bytes);
}

void att_process_data(uint16_t conn_handle, uint8_t dlen, uint8_t data[]) {
    // Opcode is a single byte at the front of the data.
    uint8_t opcode = data[0];

    // Skip over opcode.
    dlen--;
    data++;

    uint16_t mtu = att_mtu(conn_handle);

    switch (opcode) {
        case BT_ATT_OP_ERROR_RSP:
            process_error(conn_handle, dlen, data);
            break;

        case BT_ATT_OP_MTU_REQ:
            process_mtu_req(conn_handle, dlen, data);
            break;

        case BT_ATT_OP_MTU_RSP:
            process_mtu_rsp(conn_handle, dlen, data);
            break;

        case BT_ATT_OP_FIND_INFO_REQ:
            process_find_info_req(conn_handle, mtu, dlen, data);
            break;

        case BT_ATT_OP_FIND_INFO_RSP:
            process_find_info_rsp(conn_handle, dlen, data);
            break;

        case BT_ATT_OP_FIND_TYPE_REQ:
            process_find_by_type_req(conn_handle, mtu, dlen, data);
            break;

        case BT_ATT_OP_READ_TYPE_REQ:
            process_read_by_type_req(conn_handle, mtu, dlen, data);
            break;

        case BT_ATT_OP_READ_TYPE_RSP:
            process_read_by_type_rsp(conn_handle, dlen, data);
            break;

        case BT_ATT_OP_READ_GROUP_REQ:
            process_read_by_group_req(conn_handle, mtu, dlen, data);
            break;

        case BT_ATT_OP_READ_GROUP_RSP:
            process_read_by_group_rsp(conn_handle, dlen, data);
            break;

        case BT_ATT_OP_READ_REQ:
        case BT_ATT_OP_READ_BLOB_REQ:
            process_read_or_read_blob_req(conn_handle, mtu, opcode, dlen, data);
            break;

        case BT_ATT_OP_READ_RSP:
            process_read_rsp(conn_handle, dlen, data);
            break;

        case BT_ATT_OP_WRITE_REQ:
        case BT_ATT_OP_WRITE_CMD:
            process_write_req_or_cmd(conn_handle, mtu, opcode, dlen, data);
            break;

        case BT_ATT_OP_WRITE_RSP:
            process_write_rsp(conn_handle, dlen, data);
            break;

        case BT_ATT_OP_PREPARE_WRITE_REQ:
            process_prepare_write_req(conn_handle, mtu, dlen, data);
            break;

        case BT_ATT_OP_EXEC_WRITE_REQ:
            process_exec_write_req(conn_handle, mtu, dlen, data);
            break;

        case BT_ATT_OP_NOTIFY:
        case BT_ATT_OP_INDICATE:
            process_handle_notify_or_indicate(conn_handle, opcode, dlen, data);
            break;

        case BT_ATT_OP_CONFIRM:
            process_handle_confirm(conn_handle, dlen, data);
            break;

        case BT_ATT_OP_READ_MULT_REQ:
        case BT_ATT_OP_SIGNED_WRITE_CMD:
        default:
            send_error(conn_handle, opcode, 0x00, BT_ATT_ERR_NOT_SUPPORTED);
            break;
    }
}
