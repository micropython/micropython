/* hci_vs.h - Bluetooth Host Control Interface Vendor Specific definitions */

/*
 * Copyright (c) 2017-2018 Nordic Semiconductor ASA
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_BLUETOOTH_HCI_VS_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_HCI_VS_H_

#include <bluetooth/hci.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BT_VS_CMD_BIT_VERSION                       0
#define BT_VS_CMD_BIT_SUP_CMD                       1
#define BT_VS_CMD_BIT_SUP_FEAT                      2
#define BT_VS_CMD_BIT_SET_EVT_MASK                  3
#define BT_VS_CMD_BIT_RESET                         4
#define BT_VS_CMD_BIT_WRITE_BDADDR                  5
#define BT_VS_CMD_BIT_SET_TRACE_ENABLE              6
#define BT_VS_CMD_BIT_READ_BUILD_INFO               7
#define BT_VS_CMD_BIT_READ_STATIC_ADDRS             8
#define BT_VS_CMD_BIT_READ_KEY_ROOTS                9
#define BT_VS_CMD_BIT_READ_CHIP_TEMP               10
#define BT_VS_CMD_BIT_READ_HOST_STACK_CMD          11
#define BT_VS_CMD_BIT_SET_SCAN_REP_ENABLE          12
#define BT_VS_CMD_BIT_WRITE_TX_POWER               13
#define BT_VS_CMD_BIT_READ_TX_POWER                14

#define BT_VS_CMD_SUP_FEAT(cmd)                 BT_LE_FEAT_TEST(cmd, \
						BT_VS_CMD_BIT_SUP_FEAT)
#define BT_VS_CMD_READ_STATIC_ADDRS(cmd)        BT_LE_FEAT_TEST(cmd, \
						BT_VS_CMD_BIT_READ_STATIC_ADDRS)
#define BT_VS_CMD_READ_KEY_ROOTS(cmd)           BT_LE_FEAT_TEST(cmd, \
						BT_VS_CMD_BIT_READ_KEY_ROOTS)

#define BT_HCI_VS_HW_PLAT_INTEL                 0x0001
#define BT_HCI_VS_HW_PLAT_NORDIC                0x0002
#define BT_HCI_VS_HW_PLAT_NXP                   0x0003

#define BT_HCI_VS_HW_VAR_NORDIC_NRF51X          0x0001
#define BT_HCI_VS_HW_VAR_NORDIC_NRF52X          0x0002
#define BT_HCI_VS_HW_VAR_NORDIC_NRF53X          0x0003

#define BT_HCI_VS_FW_VAR_STANDARD_CTLR          0x0001
#define BT_HCI_VS_FW_VAR_VS_CTLR                0x0002
#define BT_HCI_VS_FW_VAR_FW_LOADER              0x0003
#define BT_HCI_VS_FW_VAR_RESCUE_IMG             0x0004
#define BT_HCI_OP_VS_READ_VERSION_INFO		BT_OP(BT_OGF_VS, 0x0001)
struct bt_hci_rp_vs_read_version_info {
	uint8_t  status;
	uint16_t hw_platform;
	uint16_t hw_variant;
	uint8_t  fw_variant;
	uint8_t  fw_version;
	uint16_t fw_revision;
	uint32_t fw_build;
} __packed;

#define BT_HCI_OP_VS_READ_SUPPORTED_COMMANDS	BT_OP(BT_OGF_VS, 0x0002)
struct bt_hci_rp_vs_read_supported_commands {
	uint8_t  status;
	uint8_t  commands[64];
} __packed;

#define BT_HCI_OP_VS_READ_SUPPORTED_FEATURES	BT_OP(BT_OGF_VS, 0x0003)
struct bt_hci_rp_vs_read_supported_features {
	uint8_t  status;
	uint8_t  features[8];
} __packed;

#define BT_HCI_OP_VS_SET_EVENT_MASK             BT_OP(BT_OGF_VS, 0x0004)
struct bt_hci_cp_vs_set_event_mask {
	uint8_t  event_mask[8];
} __packed;

#define BT_HCI_VS_RESET_SOFT                    0x00
#define BT_HCI_VS_RESET_HARD                    0x01
#define BT_HCI_OP_VS_RESET                      BT_OP(BT_OGF_VS, 0x0005)
struct bt_hci_cp_vs_reset {
	uint8_t  type;
} __packed;

#define BT_HCI_OP_VS_WRITE_BD_ADDR              BT_OP(BT_OGF_VS, 0x0006)
struct bt_hci_cp_vs_write_bd_addr {
	bt_addr_t bdaddr;
} __packed;

#define BT_HCI_VS_TRACE_DISABLED                0x00
#define BT_HCI_VS_TRACE_ENABLED                 0x01

#define BT_HCI_VS_TRACE_HCI_EVTS                0x00
#define BT_HCI_VS_TRACE_VDC                     0x01
#define BT_HCI_OP_VS_SET_TRACE_ENABLE           BT_OP(BT_OGF_VS, 0x0007)
struct bt_hci_cp_vs_set_trace_enable {
	uint8_t  enable;
	uint8_t  type;
} __packed;

#define BT_HCI_OP_VS_READ_BUILD_INFO            BT_OP(BT_OGF_VS, 0x0008)
struct bt_hci_rp_vs_read_build_info {
	uint8_t  status;
	uint8_t  info[0];
} __packed;

struct bt_hci_vs_static_addr {
	bt_addr_t bdaddr;
	uint8_t      ir[16];
} __packed;

#define BT_HCI_OP_VS_READ_STATIC_ADDRS          BT_OP(BT_OGF_VS, 0x0009)
struct bt_hci_rp_vs_read_static_addrs {
	uint8_t   status;
	uint8_t   num_addrs;
	struct bt_hci_vs_static_addr a[0];
} __packed;

#define BT_HCI_OP_VS_READ_KEY_HIERARCHY_ROOTS   BT_OP(BT_OGF_VS, 0x000a)
struct bt_hci_rp_vs_read_key_hierarchy_roots {
	uint8_t  status;
	uint8_t  ir[16];
	uint8_t  er[16];
} __packed;

#define BT_HCI_OP_VS_READ_CHIP_TEMP             BT_OP(BT_OGF_VS, 0x000b)
struct bt_hci_rp_vs_read_chip_temp {
	uint8_t  status;
	int8_t  temps;
} __packed;

struct bt_hci_vs_cmd {
	uint16_t vendor_id;
	uint16_t opcode_base;
} __packed;

#define BT_HCI_VS_VID_ANDROID                   0x0001
#define BT_HCI_VS_VID_MICROSOFT                 0x0002
#define BT_HCI_OP_VS_READ_HOST_STACK_CMDS       BT_OP(BT_OGF_VS, 0x000c)
struct bt_hci_rp_vs_read_host_stack_cmds {
	uint8_t   status;
	uint8_t   num_cmds;
	struct bt_hci_vs_cmd c[0];
} __packed;

#define BT_HCI_VS_SCAN_REQ_REPORTS_DISABLED     0x00
#define BT_HCI_VS_SCAN_REQ_REPORTS_ENABLED      0x01
#define BT_HCI_OP_VS_SET_SCAN_REQ_REPORTS       BT_OP(BT_OGF_VS, 0x000d)
struct bt_hci_cp_vs_set_scan_req_reports {
	uint8_t  enable;
} __packed;

#define BT_HCI_VS_LL_HANDLE_TYPE_ADV       0x00
#define BT_HCI_VS_LL_HANDLE_TYPE_SCAN      0x01
#define BT_HCI_VS_LL_HANDLE_TYPE_CONN      0x02
#define BT_HCI_VS_LL_TX_POWER_LEVEL_NO_PREF     0x7F
#define BT_HCI_OP_VS_WRITE_TX_POWER_LEVEL       BT_OP(BT_OGF_VS, 0x000e)
struct bt_hci_cp_vs_write_tx_power_level {
	uint8_t  handle_type;
	uint16_t handle;
	int8_t  tx_power_level;
} __packed;

struct bt_hci_rp_vs_write_tx_power_level {
	uint8_t  status;
	uint8_t  handle_type;
	uint16_t handle;
	int8_t  selected_tx_power;
} __packed;

#define BT_HCI_OP_VS_READ_TX_POWER_LEVEL        BT_OP(BT_OGF_VS, 0x000f)
struct bt_hci_cp_vs_read_tx_power_level {
	uint8_t  handle_type;
	uint16_t handle;
} __packed;

struct bt_hci_rp_vs_read_tx_power_level {
	uint8_t  status;
	uint8_t  handle_type;
	uint16_t handle;
	int8_t  tx_power_level;
} __packed;

#define BT_HCI_OP_VS_READ_USB_TRANSPORT_MODE    BT_OP(BT_OGF_VS, 0x0010)

struct bt_hci_rp_vs_read_usb_transport_mode {
	uint8_t  status;
	uint8_t  num_supported_modes;
	uint8_t  supported_mode[0];
} __packed;

#define BT_HCI_VS_USB_H2_MODE                  0x00
#define BT_HCI_VS_USB_H4_MODE                  0x01

#define BT_HCI_OP_VS_SET_USB_TRANSPORT_MODE    BT_OP(BT_OGF_VS, 0x0011)

struct bt_hci_cp_vs_set_usb_transport_mode {
	uint8_t  mode;
} __packed;

/* Events */

struct bt_hci_evt_vs {
	uint8_t  subevent;
} __packed;

#define BT_HCI_EVT_VS_FATAL_ERROR              0x02
struct bt_hci_evt_vs_fatal_error {
	uint64_t pc;
	uint8_t  err_info[0];
} __packed;

#define BT_HCI_VS_TRACE_LMP_TX                 0x01
#define BT_HCI_VS_TRACE_LMP_RX                 0x02
#define BT_HCI_VS_TRACE_LLCP_TX                0x03
#define BT_HCI_VS_TRACE_LLCP_RX                0x04
#define BT_HCI_VS_TRACE_LE_CONN_IND            0x05
#define BT_HCI_EVT_VS_TRACE_INFO               0x03
struct bt_hci_evt_vs_trace_info {
	uint8_t  type;
	uint8_t  data[0];
} __packed;

#define BT_HCI_EVT_VS_SCAN_REQ_RX              0x04
struct bt_hci_evt_vs_scan_req_rx {
	bt_addr_le_t addr;
	int8_t         rssi;
} __packed;

/* Event mask bits */

#define BT_EVT_MASK_VS_FATAL_ERROR             BT_EVT_BIT(1)
#define BT_EVT_MASK_VS_TRACE_INFO              BT_EVT_BIT(2)
#define BT_EVT_MASK_VS_SCAN_REQ_RX             BT_EVT_BIT(3)

/* Mesh HCI commands */
#define BT_HCI_MESH_REVISION                   0x01

#define BT_HCI_OP_VS_MESH                      BT_OP(BT_OGF_VS, 0x0042)
#define BT_HCI_MESH_EVT_PREFIX                 0xF0

struct bt_hci_cp_mesh {
	uint8_t         opcode;
} __packed;

#define BT_HCI_OC_MESH_GET_OPTS                0x00
struct bt_hci_rp_mesh_get_opts {
	uint8_t      status;
	uint8_t      opcode;
	uint8_t      revision;
	uint8_t      ch_map;
	int8_t      min_tx_power;
	int8_t      max_tx_power;
	uint8_t      max_scan_filter;
	uint8_t      max_filter_pattern;
	uint8_t      max_adv_slot;
	uint8_t      max_tx_window;
	uint8_t      evt_prefix_len;
	uint8_t      evt_prefix;
} __packed;

#define BT_HCI_MESH_PATTERN_LEN_MAX            0x0f

#define BT_HCI_OC_MESH_SET_SCAN_FILTER         0x01
struct bt_hci_mesh_pattern {
	uint8_t pattern_len;
	uint8_t pattern[0];
} __packed;

struct bt_hci_cp_mesh_set_scan_filter {
	uint8_t      scan_filter;
	uint8_t      filter_dup;
	uint8_t      num_patterns;
	struct    bt_hci_mesh_pattern patterns[0];
} __packed;
struct bt_hci_rp_mesh_set_scan_filter {
	uint8_t      status;
	uint8_t      opcode;
	uint8_t      scan_filter;
} __packed;

#define BT_HCI_OC_MESH_ADVERTISE               0x02
struct bt_hci_cp_mesh_advertise {
	uint8_t      adv_slot;
	uint8_t      own_addr_type;
	bt_addr_t random_addr;
	uint8_t      ch_map;
	int8_t      tx_power;
	uint8_t      min_tx_delay;
	uint8_t      max_tx_delay;
	uint8_t      retx_count;
	uint8_t      retx_interval;
	uint8_t      scan_delay;
	uint16_t     scan_duration;
	uint8_t      scan_filter;
	uint8_t      data_len;
	uint8_t      data[31];
} __packed;
struct bt_hci_rp_mesh_advertise {
	uint8_t      status;
	uint8_t      opcode;
	uint8_t      adv_slot;
} __packed;

#define BT_HCI_OC_MESH_ADVERTISE_TIMED         0x03
struct bt_hci_cp_mesh_advertise_timed {
	uint8_t      adv_slot;
	uint8_t      own_addr_type;
	bt_addr_t random_addr;
	uint8_t      ch_map;
	int8_t      tx_power;
	uint8_t      retx_count;
	uint8_t      retx_interval;
	uint32_t     instant;
	uint16_t     tx_delay;
	uint16_t     tx_window;
	uint8_t      data_len;
	uint8_t      data[31];
} __packed;
struct bt_hci_rp_mesh_advertise_timed {
	uint8_t      status;
	uint8_t      opcode;
	uint8_t      adv_slot;
} __packed;

#define BT_HCI_OC_MESH_ADVERTISE_CANCEL        0x04
struct bt_hci_cp_mesh_advertise_cancel {
	uint8_t      adv_slot;
} __packed;
struct bt_hci_rp_mesh_advertise_cancel {
	uint8_t      status;
	uint8_t      opcode;
	uint8_t      adv_slot;
} __packed;

#define BT_HCI_OC_MESH_SET_SCANNING            0x05
struct bt_hci_cp_mesh_set_scanning {
	uint8_t      enable;
	uint8_t      ch_map;
	uint8_t      scan_filter;
} __packed;
struct bt_hci_rp_mesh_set_scanning {
	uint8_t      status;
	uint8_t      opcode;
} __packed;

/* Events */
struct bt_hci_evt_mesh {
	uint8_t  prefix;
	uint8_t  subevent;
} __packed;

#define BT_HCI_EVT_MESH_ADV_COMPLETE           0x00
struct bt_hci_evt_mesh_adv_complete {
	uint8_t         adv_slot;
} __packed;

#define BT_HCI_EVT_MESH_SCANNING_REPORT        0x01
struct bt_hci_evt_mesh_scan_report {
	bt_addr_le_t addr;
	uint8_t         chan;
	int8_t         rssi;
	uint32_t        instant;
	uint8_t         data_len;
	uint8_t         data[0];
} __packed;
struct bt_hci_evt_mesh_scanning_report {
	uint8_t num_reports;
	struct bt_hci_evt_mesh_scan_report reports[0];
} __packed;

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_HCI_VS_H_ */
