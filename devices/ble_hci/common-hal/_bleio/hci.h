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
#include "common-hal/_bleio/hci_include/hci_err.h"

// Incomplete forward declaration to get around mutually-dependent include files.
typedef struct _bleio_adapter_obj_t bleio_adapter_obj_t;

// An hci_result_t is one of the HCI_x values below,
// or it is > 0 and is an HCI command status value (see hci_include/hci_err.h)
typedef int hci_result_t;
#define HCI_OK                 (0)
#define HCI_RESPONSE_TIMEOUT  (-1)
#define HCI_WRITE_TIMEOUT     (-2)
#define HCI_READ_ERROR        (-3)
#define HCI_WRITE_ERROR       (-4)
#define HCI_ATT_ERROR         (-5)
#define HCI_PACKET_SIZE_ERROR (-6)

extern void bleio_hci_reset(void);

void hci_check_error(hci_result_t result);

hci_result_t hci_disconnect(uint16_t handle);

hci_result_t hci_le_cancel_conn(void);
hci_result_t hci_le_conn_update(uint16_t handle, uint16_t conn_interval_min, uint16_t conn_interval_max, uint16_t conn_latency, uint16_t supervision_timeout);
hci_result_t hci_le_create_conn(uint16_t scan_interval, uint16_t scan_window, uint8_t filter_policy, bt_addr_le_t *peer_addr, uint8_t own_addr_type, uint16_t conn_interval_min, uint16_t conn_interval_max, uint16_t conn_latency, uint16_t supervision_timeout, uint16_t min_ce_len, uint16_t max_ce_len);

hci_result_t hci_le_read_buffer_size(uint16_t *le_max_len, uint8_t *le_max_num);
hci_result_t hci_le_read_maximum_advertising_data_length(uint16_t *max_adv_data_len);
hci_result_t hci_le_read_local_supported_features(uint8_t features[8]);

hci_result_t hci_le_set_advertising_data(uint8_t length, uint8_t data[]);
hci_result_t hci_le_set_advertising_enable(uint8_t enable);
hci_result_t hci_le_set_advertising_parameters(uint16_t min_interval, uint16_t max_interval, uint8_t type, uint8_t own_addr_type, bt_addr_le_t *direct_addr, uint8_t channel_map, uint8_t filter_policy);

hci_result_t hci_le_set_extended_advertising_data(uint8_t handle, uint8_t op, uint8_t frag_pref, uint8_t len, uint8_t data[]);
hci_result_t hci_le_set_extended_advertising_enable(uint8_t enable, uint8_t set_num, uint8_t handle[], uint16_t duration[], uint8_t max_ext_adv_evts[]);
hci_result_t hci_le_set_extended_advertising_parameters(uint8_t handle, uint16_t props, uint32_t prim_min_interval, uint32_t prim_max_interval, uint8_t prim_channel_map, uint8_t own_addr_type, bt_addr_le_t *peer_addr, uint8_t filter_policy, int8_t tx_power, uint8_t prim_adv_phy, uint8_t sec_adv_max_skip, uint8_t sec_adv_phy, uint8_t sid, uint8_t scan_req_notify_enable);

hci_result_t hci_le_set_random_address(uint8_t addr[6]);
hci_result_t hci_le_set_scan_enable(uint8_t enable, uint8_t filter_dup);
hci_result_t hci_le_set_scan_parameters(uint8_t scan_type, uint16_t interval, uint16_t window, uint8_t addr_type, uint8_t filter_policy);
hci_result_t hci_le_set_scan_response_data(uint8_t length, uint8_t data[]);

hci_result_t hci_poll_for_incoming_pkt(void);

hci_result_t hci_read_bd_addr(bt_addr_t *addr);
hci_result_t hci_read_buffer_size(uint16_t *acl_max_len, uint8_t *sco_max_len, uint16_t *acl_max_num, uint16_t *sco_max_num);
hci_result_t hci_read_local_version(uint8_t *hci_version, uint16_t *hci_revision, uint8_t *lmp_version, uint16_t *manufacturer, uint16_t *lmp_subversion);
hci_result_t hci_read_rssi(uint16_t handle, int *rssi);

hci_result_t hci_reset(void);

hci_result_t hci_send_acl_pkt(uint16_t handle, uint8_t cid, uint16_t data_len, uint8_t *data);
hci_result_t hci_set_event_mask(uint64_t event_mask);

#endif // MICROPY_INCLUDED_DEVICES_BLE_HCI_COMMON_HAL_BLEIO_HCI_H
