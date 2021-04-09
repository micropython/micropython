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

#ifndef MICROPY_INCLUDED_DEVICES_BLE_HCI_COMMON_HAL_BLEIO_ATT_H
#define MICROPY_INCLUDED_DEVICES_BLE_HCI_COMMON_HAL_BLEIO_ATT_H

#include <stdbool.h>
#include <stdint.h>

#include "hci_include/addr.h"
#include "hci_include/att.h"
#include "hci_include/att_internal.h"

void bleio_att_reset(void);

// FIX void att_set_event_handler(BLEDeviceEvent event, BLEDeviceEventHandler eventHandler);
bool att_address_is_connected(bt_addr_le_t *addr);
bool att_connect_to_address(bt_addr_le_t *addr);
bool att_disconnect(uint16_t conn_handle);
bool att_disconnect_all(void);
bool att_discover_attributes(bt_addr_le_t *addr, const char *service_uuid_filter);
bool att_exchange_mtu(uint16_t conn_handle);
bool att_handle_is_connected(uint16_t handle);
bool att_indicate(uint16_t handle, const uint8_t *value, int length);
bool att_is_connected(void);
bool att_notify(uint16_t handle, const uint8_t *value, int length);
int att_read_req(uint16_t conn_handle, uint16_t handle, uint8_t response_buffer[]);
int att_write_req(uint16_t conn_handle, uint16_t handle, const uint8_t *data, uint8_t data_len, uint8_t response_buffer[]);
uint16_t att_conn_handle(bt_addr_le_t *addr);
uint16_t att_mtu(uint16_t handle);
void att_add_connection(uint16_t handle, uint8_t role, bt_addr_le_t *peer_addr, uint16_t interval, uint16_t latency, uint16_t supervision_timeout, uint8_t master_clock_accuracy);
void att_process_data(uint16_t conn_handle, uint8_t dlen, uint8_t data[]);
void att_remove_connection(uint16_t conn_handle, uint8_t reason);
void att_set_max_mtu(uint16_t max_mtu);
void att_set_timeout(unsigned long timeout);
void att_write_cmd(uint16_t conn_handle, uint16_t handle, const uint8_t *data, uint8_t data_len);

#endif // MICROPY_INCLUDED_DEVICES_BLE_HCI_COMMON_HAL_BLEIO_ATT_H
