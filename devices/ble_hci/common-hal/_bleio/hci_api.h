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

#ifndef MICROPY_INCLUDED_DEVICES_BLE_HCI_COMMON_HAL_BLEIO_HCI_H
#define MICROPY_INCLUDED_DEVICES_BLE_HCI_COMMON_HAL_BLEIO_HCI_H

#include <stdint.h>

#include "common-hal/_bleio/hci_include/hci.h"

#include "common-hal/_bleio/Adapter.h"

// An hci_result_t is one of the HCI_x values below,
// or is it > 0 and is an HCI command status value (see hci_include/hci_err.h)
typedef int hci_result_t;
#define HCI_OK (0)
#define HCI_NO_RESPONSE (-1)
#define HCI_ERR_RESPONSE (-2)
#define HCI_READ_TIMEOUT (-3)
#define HCI_WRITE_TIMEOUT (-4)
#define HCI_READ_ERROR (-5)
#define HCI_WRITE_ERROR (-6)

void hci_init(bleio_adapter_obj_t *adapter_in);

hci_result_t hci_disconnect(uint16_t handle);

hci_result_t hci_le_cancel_conn(void);
hci_result_t hci_le_conn_update(uint16_t handle, uint16_t conn_interval_min, uint16_t conn_interval_max, uint16_t conn_latency, uint16_t supervision_timeout);
hci_result_t hci_le_create_conn(uint16_t scan_interval, uint16_t scan_window, uint8_t filter_policy, bt_addr_le_t *peer_addr, uint8_t own_addr_type, uint16_t conn_interval_min, uint16_t conn_interval_max, uint16_t conn_latency, uint16_t supervision_timeout, uint16_t min_ce_len, uint16_t max_ce_len);

hci_result_t hci_le_read_maximum_advertising_data_length(int *max_adv_data_len);

hci_result_t hci_le_set_advertise_enable(uint8_t enable);
hci_result_t hci_le_set_advertising_data(uint8_t length, uint8_t data[]);
hci_result_t hci_le_set_advertising_parameters(uint16_t min_interval, uint16_t max_interval, uint8_t type, uint8_t own_addr_type, bt_addr_le_t *direct_addr, uint8_t channel_map, uint8_t filter_policy);
hci_result_t hci_le_set_random_address(uint8_t addr[6]);
hci_result_t hci_le_set_scan_enable(uint8_t enable, uint8_t filter_dup);
hci_result_t hci_le_set_scan_parameters(uint8_t scan_type, uint16_t interval, uint16_t window, uint8_t addr_type, uint8_t filter_policy);
hci_result_t hci_le_set_scan_response_data(uint8_t length, uint8_t data[]);

hci_result_t hci_poll_for_incoming_pkt(void);

hci_result_t hci_read_bd_addr(bt_addr_t *addr);
hci_result_t hci_read_le_buffer_size(uint16_t *le_max_len, uint8_t *le_max_num);
hci_result_t hci_read_local_version(uint8_t *hci_version, uint16_t *hci_revision, uint8_t *lmp_version, uint16_t *manufacturer, uint16_t *lmp_subversion);
hci_result_t hci_read_rssi(uint16_t handle, int *rssi);

hci_result_t hci_reset(void);

hci_result_t hci_set_evt_mask(uint64_t event_mask);

#endif // MICROPY_INCLUDED_DEVICES_BLE_HCI_COMMON_HAL_BLEIO_HCI_H
