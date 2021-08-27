// CircuitPython: Adapted from Zephyr include file.

/* hci.h - Bluetooth Host Control Interface definitions */

/*
 * Copyright 2020 Dan Halbert for Adafruit Industries
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_BLUETOOTH_HCI_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_HCI_H_

#include <stdbool.h>
// for __packed
#include <sys/cdefs.h>

#include "addr.h"

// ESP32S2 build environment defines this already.
#ifndef BIT
#define BIT(n)  (1UL << (n))
#endif

/* Special own address types for LL privacy (used in adv & scan parameters) */
#define BT_HCI_OWN_ADDR_RPA_OR_PUBLIC  0x02
#define BT_HCI_OWN_ADDR_RPA_OR_RANDOM  0x03
#define BT_HCI_OWN_ADDR_RPA_MASK       0x02

#define BT_HCI_PEER_ADDR_RPA_UNRESOLVED 0xfe
#define BT_HCI_PEER_ADDR_ANONYMOUS      0xff

#define BT_ENC_KEY_SIZE_MIN                     0x07
#define BT_ENC_KEY_SIZE_MAX                     0x10

struct bt_hci_evt_hdr {
    uint8_t evt;
    uint8_t len;
} __packed;
#define BT_HCI_EVT_HDR_SIZE             2

#define BT_ACL_START_NO_FLUSH           0x00
#define BT_ACL_CONT                     0x01
#define BT_ACL_START                    0x02
#define BT_ACL_COMPLETE                 0x03

#define BT_ACL_POINT_TO_POINT           0x00
#define BT_ACL_BROADCAST                0x01

#define bt_acl_handle(h)                ((h) & BIT_MASK(12))
#define bt_acl_flags(h)                 ((h) >> 12)
#define bt_acl_flags_pb(f)              ((f) & BIT_MASK(2))
#define bt_acl_flags_bc(f)              ((f) >> 2)
#define bt_acl_handle_pack(h, f)        ((h) | ((f) << 12))

struct bt_hci_acl_hdr {
    uint16_t handle;
    uint16_t len;
} __packed;
#define BT_HCI_ACL_HDR_SIZE             4

struct bt_hci_cmd_hdr {
    uint16_t opcode;
    uint8_t param_len;
} __packed;
#define BT_HCI_CMD_HDR_SIZE             3

/* Supported Commands */
#define BT_CMD_TEST(cmd, octet, bit)            (cmd[octet] & BIT(bit))
#define BT_CMD_LE_STATES(cmd)                   BT_CMD_TEST(cmd, 28, 3)

#define BT_FEAT_TEST(feat, page, octet, bit)    (feat[page][octet] & BIT(bit))

#define BT_FEAT_BREDR(feat)                     !BT_FEAT_TEST(feat, 0, 4, 5)
#define BT_FEAT_LE(feat)                        BT_FEAT_TEST(feat, 0, 4, 6)
#define BT_FEAT_EXT_FEATURES(feat)              BT_FEAT_TEST(feat, 0, 7, 7)
#define BT_FEAT_HOST_SSP(feat)                  BT_FEAT_TEST(feat, 1, 0, 0)
#define BT_FEAT_SC(feat)                        BT_FEAT_TEST(feat, 2, 1, 0)

#define BT_FEAT_LMP_ESCO_CAPABLE(feat)          BT_FEAT_TEST(feat, 0, 3, 7)
#define BT_FEAT_HV2_PKT(feat)                   BT_FEAT_TEST(feat, 0, 1, 4)
#define BT_FEAT_HV3_PKT(feat)                   BT_FEAT_TEST(feat, 0, 1, 5)
#define BT_FEAT_EV4_PKT(feat)                   BT_FEAT_TEST(feat, 0, 4, 0)
#define BT_FEAT_EV5_PKT(feat)                   BT_FEAT_TEST(feat, 0, 4, 1)
#define BT_FEAT_2EV3_PKT(feat)                  BT_FEAT_TEST(feat, 0, 5, 5)
#define BT_FEAT_3EV3_PKT(feat)                  BT_FEAT_TEST(feat, 0, 5, 6)
#define BT_FEAT_3SLOT_PKT(feat)                 BT_FEAT_TEST(feat, 0, 5, 7)

/* LE features */
#define BT_LE_FEAT_BIT_ENC                      0
#define BT_LE_FEAT_BIT_CONN_PARAM_REQ           1
#define BT_LE_FEAT_BIT_EXT_REJ_IND              2
#define BT_LE_FEAT_BIT_SLAVE_FEAT_REQ           3
#define BT_LE_FEAT_BIT_PING                     4
#define BT_LE_FEAT_BIT_DLE                      5
#define BT_LE_FEAT_BIT_PRIVACY                  6
#define BT_LE_FEAT_BIT_EXT_SCAN                 7
#define BT_LE_FEAT_BIT_PHY_2M                   8
#define BT_LE_FEAT_BIT_SMI_TX                   9
#define BT_LE_FEAT_BIT_SMI_RX                   10
#define BT_LE_FEAT_BIT_PHY_CODED                11
#define BT_LE_FEAT_BIT_EXT_ADV                  12
#define BT_LE_FEAT_BIT_PER_ADV                  13
#define BT_LE_FEAT_BIT_CHAN_SEL_ALGO_2          14
#define BT_LE_FEAT_BIT_PWR_CLASS_1              15
#define BT_LE_FEAT_BIT_MIN_USED_CHAN_PROC       16
#define BT_LE_FEAT_BIT_CONN_CTE_REQ             17
#define BT_LE_FEAT_BIT_CONN_CTE_RESP            18
#define BT_LE_FEAT_BIT_CONNECTIONLESS_CTE_TX    19
#define BT_LE_FEAT_BIT_CONNECTIONLESS_CTE_RX    20
#define BT_LE_FEAT_BIT_ANT_SWITCH_TX_AOD        21
#define BT_LE_FEAT_BIT_ANT_SWITCH_RX_AOA        22
#define BT_LE_FEAT_BIT_RX_CTE                   23
#define BT_LE_FEAT_BIT_PERIODIC_SYNC_XFER_SEND  24
#define BT_LE_FEAT_BIT_PERIODIC_SYNC_XFER_RECV  25
#define BT_LE_FEAT_BIT_SCA_UPDATE               26
#define BT_LE_FEAT_BIT_REMOTE_PUB_KEY_VALIDATE  27
#define BT_LE_FEAT_BIT_CIS_MASTER               28
#define BT_LE_FEAT_BIT_CIS_SLAVE                29
#define BT_LE_FEAT_BIT_ISO_BROADCASTER          30
#define BT_LE_FEAT_BIT_SYNC_RECEIVER            31
#define BT_LE_FEAT_BIT_ISO_CHANNELS             32
#define BT_LE_FEAT_BIT_PWR_CTRL_REQ             33
#define BT_LE_FEAT_BIT_PWR_CHG_IND              34
#define BT_LE_FEAT_BIT_PATH_LOSS_MONITOR        35

#define BT_LE_FEAT_TEST(feat, n)                (feat[(n) >> 3] & \
    BIT((n) & 7))

#define BT_FEAT_LE_ENCR(feat)                   BT_LE_FEAT_TEST(feat, \
    BT_LE_FEAT_BIT_ENC)
#define BT_FEAT_LE_CONN_PARAM_REQ_PROC(feat)    BT_LE_FEAT_TEST(feat, \
    BT_LE_FEAT_BIT_CONN_PARAM_REQ)
#define BT_FEAT_LE_SLAVE_FEATURE_XCHG(feat)     BT_LE_FEAT_TEST(feat, \
    BT_LE_FEAT_BIT_SLAVE_FEAT_REQ)
#define BT_FEAT_LE_DLE(feat)                    BT_LE_FEAT_TEST(feat, \
    BT_LE_FEAT_BIT_DLE)
#define BT_FEAT_LE_PHY_2M(feat)                 BT_LE_FEAT_TEST(feat, \
    BT_LE_FEAT_BIT_PHY_2M)
#define BT_FEAT_LE_PHY_CODED(feat)              BT_LE_FEAT_TEST(feat, \
    BT_LE_FEAT_BIT_PHY_CODED)
#define BT_FEAT_LE_PRIVACY(feat)                BT_LE_FEAT_TEST(feat, \
    BT_LE_FEAT_BIT_PRIVACY)
#define BT_FEAT_LE_EXT_ADV(feat)                BT_LE_FEAT_TEST(feat, \
    BT_LE_FEAT_BIT_EXT_ADV)

/* LE States */
#define BT_LE_STATES_SLAVE_CONN_ADV(states)     (states & 0x0000004000000000)

/* Bonding/authentication types */
#define BT_HCI_NO_BONDING                       0x00
#define BT_HCI_NO_BONDING_MITM                  0x01
#define BT_HCI_DEDICATED_BONDING                0x02
#define BT_HCI_DEDICATED_BONDING_MITM           0x03
#define BT_HCI_GENERAL_BONDING                  0x04
#define BT_HCI_GENERAL_BONDING_MITM             0x05

/*
 * MITM protection is enabled in SSP authentication requirements octet when
 * LSB bit is set.
 */
#define BT_MITM                                 0x01

/* I/O capabilities */
#define BT_IO_DISPLAY_ONLY                      0x00
#define BT_IO_DISPLAY_YESNO                     0x01
#define BT_IO_KEYBOARD_ONLY                     0x02
#define BT_IO_NO_INPUT_OUTPUT                   0x03

/* SCO packet types */
#define HCI_PKT_TYPE_HV1                        0x0020
#define HCI_PKT_TYPE_HV2                        0x0040
#define HCI_PKT_TYPE_HV3                        0x0080

/* eSCO packet types */
#define HCI_PKT_TYPE_ESCO_HV1                   0x0001
#define HCI_PKT_TYPE_ESCO_HV2                   0x0002
#define HCI_PKT_TYPE_ESCO_HV3                   0x0004
#define HCI_PKT_TYPE_ESCO_EV3                   0x0008
#define HCI_PKT_TYPE_ESCO_EV4                   0x0010
#define HCI_PKT_TYPE_ESCO_EV5                   0x0020
#define HCI_PKT_TYPE_ESCO_2EV3                  0x0040
#define HCI_PKT_TYPE_ESCO_3EV3                  0x0080
#define HCI_PKT_TYPE_ESCO_2EV5                  0x0100
#define HCI_PKT_TYPE_ESCO_3EV5                  0x0200


#define ESCO_PKT_MASK                           (HCI_PKT_TYPE_ESCO_HV1 | \
    HCI_PKT_TYPE_ESCO_HV2 | \
    HCI_PKT_TYPE_ESCO_HV3)
#define SCO_PKT_MASK                            (HCI_PKT_TYPE_HV1 | \
    HCI_PKT_TYPE_HV2 | \
    HCI_PKT_TYPE_HV3)
#define EDR_ESCO_PKT_MASK                       (HCI_PKT_TYPE_ESCO_2EV3 | \
    HCI_PKT_TYPE_ESCO_3EV3 | \
    HCI_PKT_TYPE_ESCO_2EV5 | \
    HCI_PKT_TYPE_ESCO_3EV5)

/* HCI BR/EDR link types */
#define BT_HCI_SCO                              0x00
#define BT_HCI_ACL                              0x01
#define BT_HCI_ESCO                             0x02

/* OpCode Group Fields */
#define BT_OGF_LINK_CTRL                        0x01
#define BT_OGF_BASEBAND                         0x03
#define BT_OGF_INFO                             0x04
#define BT_OGF_STATUS                           0x05
#define BT_OGF_LE                               0x08
#define BT_OGF_VS                               0x3f

/* Construct OpCode from OGF and OCF */
#define BT_OP(ogf, ocf)                         ((ocf) | ((ogf) << 10))

/* Invalid opcode */
#define BT_OP_NOP                               0x0000

/* Obtain OGF from OpCode */
#define BT_OGF(opcode)                          (((opcode) >> 10) & BIT_MASK(6))
/* Obtain OCF from OpCode */
#define BT_OCF(opcode)                          ((opcode) & BIT_MASK(10))

#define BT_HCI_OP_INQUIRY                       BT_OP(BT_OGF_LINK_CTRL, 0x0001)
struct bt_hci_op_inquiry {
    uint8_t lap[3];
    uint8_t length;
    uint8_t num_rsp;
} __packed;

#define BT_HCI_OP_INQUIRY_CANCEL                BT_OP(BT_OGF_LINK_CTRL, 0x0002)

#define BT_HCI_OP_CONNECT                       BT_OP(BT_OGF_LINK_CTRL, 0x0005)
struct bt_hci_cp_connect {
    bt_addr_t bdaddr;
    uint16_t packet_type;
    uint8_t pscan_rep_mode;
    uint8_t reserved;
    uint16_t clock_offset;
    uint8_t allow_role_switch;
} __packed;

#define BT_HCI_OP_DISCONNECT                    BT_OP(BT_OGF_LINK_CTRL, 0x0006)
struct bt_hci_cp_disconnect {
    uint16_t handle;
    uint8_t reason;
} __packed;

#define BT_HCI_OP_CONNECT_CANCEL                BT_OP(BT_OGF_LINK_CTRL, 0x0008)
struct bt_hci_cp_connect_cancel {
    bt_addr_t bdaddr;
} __packed;
struct bt_hci_rp_connect_cancel {
    uint8_t status;
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_OP_ACCEPT_CONN_REQ               BT_OP(BT_OGF_LINK_CTRL, 0x0009)
struct bt_hci_cp_accept_conn_req {
    bt_addr_t bdaddr;
    uint8_t role;
} __packed;

#define BT_HCI_OP_SETUP_SYNC_CONN               BT_OP(BT_OGF_LINK_CTRL, 0x0028)
struct bt_hci_cp_setup_sync_conn {
    uint16_t handle;
    uint32_t tx_bandwidth;
    uint32_t rx_bandwidth;
    uint16_t max_latency;
    uint16_t content_format;
    uint8_t retrans_effort;
    uint16_t pkt_type;
} __packed;

#define BT_HCI_OP_ACCEPT_SYNC_CONN_REQ          BT_OP(BT_OGF_LINK_CTRL, 0x0029)
struct bt_hci_cp_accept_sync_conn_req {
    bt_addr_t bdaddr;
    uint32_t tx_bandwidth;
    uint32_t rx_bandwidth;
    uint16_t max_latency;
    uint16_t content_format;
    uint8_t retrans_effort;
    uint16_t pkt_type;
} __packed;

#define BT_HCI_OP_REJECT_CONN_REQ               BT_OP(BT_OGF_LINK_CTRL, 0x000a)
struct bt_hci_cp_reject_conn_req {
    bt_addr_t bdaddr;
    uint8_t reason;
} __packed;

#define BT_HCI_OP_LINK_KEY_REPLY                BT_OP(BT_OGF_LINK_CTRL, 0x000b)
struct bt_hci_cp_link_key_reply {
    bt_addr_t bdaddr;
    uint8_t link_key[16];
} __packed;

#define BT_HCI_OP_LINK_KEY_NEG_REPLY            BT_OP(BT_OGF_LINK_CTRL, 0x000c)
struct bt_hci_cp_link_key_neg_reply {
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_OP_PIN_CODE_REPLY                BT_OP(BT_OGF_LINK_CTRL, 0x000d)
struct bt_hci_cp_pin_code_reply {
    bt_addr_t bdaddr;
    uint8_t pin_len;
    uint8_t pin_code[16];
} __packed;
struct bt_hci_rp_pin_code_reply {
    uint8_t status;
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_OP_PIN_CODE_NEG_REPLY            BT_OP(BT_OGF_LINK_CTRL, 0x000e)
struct bt_hci_cp_pin_code_neg_reply {
    bt_addr_t bdaddr;
} __packed;
struct bt_hci_rp_pin_code_neg_reply {
    uint8_t status;
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_OP_AUTH_REQUESTED                BT_OP(BT_OGF_LINK_CTRL, 0x0011)
struct bt_hci_cp_auth_requested {
    uint16_t handle;
} __packed;

#define BT_HCI_OP_SET_CONN_ENCRYPT              BT_OP(BT_OGF_LINK_CTRL, 0x0013)
struct bt_hci_cp_set_conn_encrypt {
    uint16_t handle;
    uint8_t encrypt;
} __packed;

#define BT_HCI_OP_REMOTE_NAME_REQUEST           BT_OP(BT_OGF_LINK_CTRL, 0x0019)
struct bt_hci_cp_remote_name_request {
    bt_addr_t bdaddr;
    uint8_t pscan_rep_mode;
    uint8_t reserved;
    uint16_t clock_offset;
} __packed;

#define BT_HCI_OP_REMOTE_NAME_CANCEL            BT_OP(BT_OGF_LINK_CTRL, 0x001a)
struct bt_hci_cp_remote_name_cancel {
    bt_addr_t bdaddr;
} __packed;
struct bt_hci_rp_remote_name_cancel {
    uint8_t status;
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_OP_READ_REMOTE_FEATURES          BT_OP(BT_OGF_LINK_CTRL, 0x001b)
struct bt_hci_cp_read_remote_features {
    uint16_t handle;
} __packed;

#define BT_HCI_OP_READ_REMOTE_EXT_FEATURES      BT_OP(BT_OGF_LINK_CTRL, 0x001c)
struct bt_hci_cp_read_remote_ext_features {
    uint16_t handle;
    uint8_t page;
} __packed;

#define BT_HCI_OP_READ_REMOTE_VERSION_INFO      BT_OP(BT_OGF_LINK_CTRL, 0x001d)
struct bt_hci_cp_read_remote_version_info {
    uint16_t handle;
} __packed;

#define BT_HCI_OP_IO_CAPABILITY_REPLY           BT_OP(BT_OGF_LINK_CTRL, 0x002b)
struct bt_hci_cp_io_capability_reply {
    bt_addr_t bdaddr;
    uint8_t capability;
    uint8_t oob_data;
    uint8_t authentication;
} __packed;

#define BT_HCI_OP_USER_CONFIRM_REPLY            BT_OP(BT_OGF_LINK_CTRL, 0x002c)
#define BT_HCI_OP_USER_CONFIRM_NEG_REPLY        BT_OP(BT_OGF_LINK_CTRL, 0x002d)
struct bt_hci_cp_user_confirm_reply {
    bt_addr_t bdaddr;
} __packed;
struct bt_hci_rp_user_confirm_reply {
    uint8_t status;
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_OP_USER_PASSKEY_REPLY            BT_OP(BT_OGF_LINK_CTRL, 0x002e)
struct bt_hci_cp_user_passkey_reply {
    bt_addr_t bdaddr;
    uint32_t passkey;
} __packed;

#define BT_HCI_OP_USER_PASSKEY_NEG_REPLY        BT_OP(BT_OGF_LINK_CTRL, 0x002f)
struct bt_hci_cp_user_passkey_neg_reply {
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_OP_IO_CAPABILITY_NEG_REPLY       BT_OP(BT_OGF_LINK_CTRL, 0x0034)
struct bt_hci_cp_io_capability_neg_reply {
    bt_addr_t bdaddr;
    uint8_t reason;
} __packed;

#define BT_HCI_OP_SET_EVENT_MASK                BT_OP(BT_OGF_BASEBAND, 0x0001)
struct bt_hci_cp_set_event_mask {
    uint8_t events[8];
} __packed;

#define BT_HCI_OP_RESET                         BT_OP(BT_OGF_BASEBAND, 0x0003)

#define BT_HCI_OP_WRITE_LOCAL_NAME              BT_OP(BT_OGF_BASEBAND, 0x0013)
struct bt_hci_write_local_name {
    uint8_t local_name[248];
} __packed;

#define BT_HCI_OP_WRITE_PAGE_TIMEOUT            BT_OP(BT_OGF_BASEBAND, 0x0018)

#define BT_HCI_OP_WRITE_SCAN_ENABLE             BT_OP(BT_OGF_BASEBAND, 0x001a)
#define BT_BREDR_SCAN_DISABLED                  0x00
#define BT_BREDR_SCAN_INQUIRY                   0x01
#define BT_BREDR_SCAN_PAGE                      0x02

#define BT_TX_POWER_LEVEL_CURRENT               0x00
#define BT_TX_POWER_LEVEL_MAX                   0x01
#define BT_HCI_OP_READ_TX_POWER_LEVEL           BT_OP(BT_OGF_BASEBAND, 0x002d)
struct bt_hci_cp_read_tx_power_level {
    uint16_t handle;
    uint8_t type;
} __packed;

struct bt_hci_rp_read_tx_power_level {
    uint8_t status;
    uint16_t handle;
    int8_t tx_power_level;
} __packed;

#define BT_HCI_CTL_TO_HOST_FLOW_DISABLE         0x00
#define BT_HCI_CTL_TO_HOST_FLOW_ENABLE          0x01
#define BT_HCI_OP_SET_CTL_TO_HOST_FLOW          BT_OP(BT_OGF_BASEBAND, 0x0031)
struct bt_hci_cp_set_ctl_to_host_flow {
    uint8_t flow_enable;
} __packed;

#define BT_HCI_OP_HOST_BUFFER_SIZE              BT_OP(BT_OGF_BASEBAND, 0x0033)
struct bt_hci_cp_host_buffer_size {
    uint16_t acl_mtu;
    uint8_t sco_mtu;
    uint16_t acl_pkts;
    uint16_t sco_pkts;
} __packed;

struct bt_hci_handle_count {
    uint16_t handle;
    uint16_t count;
} __packed;

#define BT_HCI_OP_HOST_NUM_COMPLETED_PACKETS    BT_OP(BT_OGF_BASEBAND, 0x0035)
struct bt_hci_cp_host_num_completed_packets {
    uint8_t num_handles;
    struct bt_hci_handle_count h[];
} __packed;

#define BT_HCI_OP_WRITE_INQUIRY_MODE            BT_OP(BT_OGF_BASEBAND, 0x0045)
struct bt_hci_cp_write_inquiry_mode {
    uint8_t mode;
} __packed;

#define BT_HCI_OP_WRITE_SSP_MODE                BT_OP(BT_OGF_BASEBAND, 0x0056)
struct bt_hci_cp_write_ssp_mode {
    uint8_t mode;
} __packed;

#define BT_HCI_OP_SET_EVENT_MASK_PAGE_2         BT_OP(BT_OGF_BASEBAND, 0x0063)
struct bt_hci_cp_set_event_mask_page_2 {
    uint8_t events_page_2[8];
} __packed;

#define BT_HCI_OP_LE_WRITE_LE_HOST_SUPP         BT_OP(BT_OGF_BASEBAND, 0x006d)
struct bt_hci_cp_write_le_host_supp {
    uint8_t le;
    uint8_t simul;
} __packed;

#define BT_HCI_OP_WRITE_SC_HOST_SUPP            BT_OP(BT_OGF_BASEBAND, 0x007a)
struct bt_hci_cp_write_sc_host_supp {
    uint8_t sc_support;
} __packed;

#define BT_HCI_OP_READ_AUTH_PAYLOAD_TIMEOUT     BT_OP(BT_OGF_BASEBAND, 0x007b)
struct bt_hci_cp_read_auth_payload_timeout {
    uint16_t handle;
} __packed;

struct bt_hci_rp_read_auth_payload_timeout {
    uint8_t status;
    uint16_t handle;
    uint16_t auth_payload_timeout;
} __packed;

#define BT_HCI_OP_WRITE_AUTH_PAYLOAD_TIMEOUT    BT_OP(BT_OGF_BASEBAND, 0x007c)
struct bt_hci_cp_write_auth_payload_timeout {
    uint16_t handle;
    uint16_t auth_payload_timeout;
} __packed;

struct bt_hci_rp_write_auth_payload_timeout {
    uint8_t status;
    uint16_t handle;
} __packed;

/* HCI version from Assigned Numbers */
#define BT_HCI_VERSION_1_0B                     0
#define BT_HCI_VERSION_1_1                      1
#define BT_HCI_VERSION_1_2                      2
#define BT_HCI_VERSION_2_0                      3
#define BT_HCI_VERSION_2_1                      4
#define BT_HCI_VERSION_3_0                      5
#define BT_HCI_VERSION_4_0                      6
#define BT_HCI_VERSION_4_1                      7
#define BT_HCI_VERSION_4_2                      8
#define BT_HCI_VERSION_5_0                      9
#define BT_HCI_VERSION_5_1                      10
#define BT_HCI_VERSION_5_2                      11

#define BT_HCI_OP_READ_LOCAL_VERSION_INFO       BT_OP(BT_OGF_INFO, 0x0001)
struct bt_hci_rp_read_local_version_info {
    uint8_t status;
    uint8_t hci_version;
    uint16_t hci_revision;
    uint8_t lmp_version;
    uint16_t manufacturer;
    uint16_t lmp_subversion;
} __packed;

#define BT_HCI_OP_READ_SUPPORTED_COMMANDS       BT_OP(BT_OGF_INFO, 0x0002)
struct bt_hci_rp_read_supported_commands {
    uint8_t status;
    uint8_t commands[64];
} __packed;

#define BT_HCI_OP_READ_LOCAL_EXT_FEATURES       BT_OP(BT_OGF_INFO, 0x0004)
struct bt_hci_cp_read_local_ext_features {
    uint8_t page;
};
struct bt_hci_rp_read_local_ext_features {
    uint8_t status;
    uint8_t page;
    uint8_t max_page;
    uint8_t ext_features[8];
} __packed;

#define BT_HCI_OP_READ_LOCAL_FEATURES           BT_OP(BT_OGF_INFO, 0x0003)
struct bt_hci_rp_read_local_features {
    uint8_t status;
    uint8_t features[8];
} __packed;

#define BT_HCI_OP_READ_BUFFER_SIZE              BT_OP(BT_OGF_INFO, 0x0005)
struct bt_hci_rp_read_buffer_size {
    uint8_t status;
    uint16_t acl_max_len;
    uint8_t sco_max_len;
    uint16_t acl_max_num;
    uint16_t sco_max_num;
} __packed;

#define BT_HCI_OP_READ_BD_ADDR                  BT_OP(BT_OGF_INFO, 0x0009)
struct bt_hci_rp_read_bd_addr {
    uint8_t status;
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_OP_READ_RSSI                     BT_OP(BT_OGF_STATUS, 0x0005)
struct bt_hci_cp_read_rssi {
    uint16_t handle;
} __packed;
struct bt_hci_rp_read_rssi {
    uint8_t status;
    uint16_t handle;
    int8_t rssi;
} __packed;

#define BT_HCI_ENCRYPTION_KEY_SIZE_MIN          7
#define BT_HCI_ENCRYPTION_KEY_SIZE_MAX          16

#define BT_HCI_OP_READ_ENCRYPTION_KEY_SIZE      BT_OP(BT_OGF_STATUS, 0x0008)
struct bt_hci_cp_read_encryption_key_size {
    uint16_t handle;
} __packed;
struct bt_hci_rp_read_encryption_key_size {
    uint8_t status;
    uint16_t handle;
    uint8_t key_size;
} __packed;

/* BLE */

#define BT_HCI_OP_LE_SET_EVENT_MASK             BT_OP(BT_OGF_LE, 0x0001)
struct bt_hci_cp_le_set_event_mask {
    uint8_t events[8];
} __packed;

#define BT_HCI_OP_LE_READ_BUFFER_SIZE           BT_OP(BT_OGF_LE, 0x0002)
struct bt_hci_rp_le_read_buffer_size {
    uint8_t status;
    uint16_t le_max_len;
    uint8_t le_max_num;
} __packed;

#define BT_HCI_OP_LE_READ_LOCAL_FEATURES        BT_OP(BT_OGF_LE, 0x0003)
struct bt_hci_rp_le_read_local_features {
    uint8_t status;
    uint8_t features[8];
} __packed;

#define BT_HCI_OP_LE_SET_RANDOM_ADDRESS         BT_OP(BT_OGF_LE, 0x0005)
struct bt_hci_cp_le_set_random_address {
    bt_addr_t bdaddr;
} __packed;

/* LE Advertising Types (LE Advertising Parameters Set)*/
#define BT_LE_ADV_IND                  (__DEPRECATED_MACRO 0x00)
#define BT_LE_ADV_DIRECT_IND           (__DEPRECATED_MACRO 0x01)
#define BT_LE_ADV_SCAN_IND             (__DEPRECATED_MACRO 0x02)
#define BT_LE_ADV_NONCONN_IND          (__DEPRECATED_MACRO 0x03)
#define BT_LE_ADV_DIRECT_IND_LOW_DUTY  (__DEPRECATED_MACRO 0x04)
/* LE Advertising PDU Types. */
#define BT_LE_ADV_SCAN_RSP             (__DEPRECATED_MACRO 0x04)

#define BT_HCI_ADV_IND                          0x00
#define BT_HCI_ADV_DIRECT_IND                   0x01
#define BT_HCI_ADV_SCAN_IND                     0x02
#define BT_HCI_ADV_NONCONN_IND                  0x03
#define BT_HCI_ADV_DIRECT_IND_LOW_DUTY          0x04
#define BT_HCI_ADV_SCAN_RSP                     0x04

#define BT_LE_ADV_FP_NO_WHITELIST               0x00
#define BT_LE_ADV_FP_WHITELIST_SCAN_REQ         0x01
#define BT_LE_ADV_FP_WHITELIST_CONN_IND         0x02
#define BT_LE_ADV_FP_WHITELIST_BOTH             0x03

#define BT_HCI_OP_LE_SET_ADV_PARAM              BT_OP(BT_OGF_LE, 0x0006)
struct bt_hci_cp_le_set_adv_param {
    uint16_t min_interval;
    uint16_t max_interval;
    uint8_t type;
    uint8_t own_addr_type;
    bt_addr_le_t direct_addr;
    uint8_t channel_map;
    uint8_t filter_policy;
} __packed;

#define BT_HCI_OP_LE_READ_ADV_CHAN_TX_POWER     BT_OP(BT_OGF_LE, 0x0007)
struct bt_hci_rp_le_read_chan_tx_power {
    uint8_t status;
    int8_t tx_power_level;
} __packed;

#define BT_HCI_OP_LE_SET_ADV_DATA               BT_OP(BT_OGF_LE, 0x0008)
struct bt_hci_cp_le_set_adv_data {
    uint8_t len;
    uint8_t data[31];
} __packed;

#define BT_HCI_OP_LE_SET_SCAN_RSP_DATA          BT_OP(BT_OGF_LE, 0x0009)
struct bt_hci_cp_le_set_scan_rsp_data {
    uint8_t len;
    uint8_t data[31];
} __packed;

#define BT_HCI_LE_ADV_DISABLE                   0x00
#define BT_HCI_LE_ADV_ENABLE                    0x01

#define BT_HCI_OP_LE_SET_ADV_ENABLE             BT_OP(BT_OGF_LE, 0x000a)
struct bt_hci_cp_le_set_adv_enable {
    uint8_t enable;
} __packed;

/* Scan types */
#define BT_HCI_OP_LE_SET_SCAN_PARAM             BT_OP(BT_OGF_LE, 0x000b)
#define BT_HCI_LE_SCAN_PASSIVE                  0x00
#define BT_HCI_LE_SCAN_ACTIVE                   0x01

#define BT_HCI_LE_SCAN_FP_NO_WHITELIST          0x00
#define BT_HCI_LE_SCAN_FP_USE_WHITELIST         0x01

struct bt_hci_cp_le_set_scan_param {
    uint8_t scan_type;
    uint16_t interval;
    uint16_t window;
    uint8_t addr_type;
    uint8_t filter_policy;
} __packed;

#define BT_HCI_OP_LE_SET_SCAN_ENABLE            BT_OP(BT_OGF_LE, 0x000c)

#define BT_HCI_LE_SCAN_DISABLE                  0x00
#define BT_HCI_LE_SCAN_ENABLE                   0x01

#define BT_HCI_LE_SCAN_FILTER_DUP_DISABLE       0x00
#define BT_HCI_LE_SCAN_FILTER_DUP_ENABLE        0x01

struct bt_hci_cp_le_set_scan_enable {
    uint8_t enable;
    uint8_t filter_dup;
} __packed;

#define BT_HCI_OP_LE_CREATE_CONN                BT_OP(BT_OGF_LE, 0x000d)

#define BT_HCI_LE_CREATE_CONN_FP_DIRECT         0x00
#define BT_HCI_LE_CREATE_CONN_FP_WHITELIST      0x01

struct bt_hci_cp_le_create_conn {
    uint16_t scan_interval;
    uint16_t scan_window;
    uint8_t filter_policy;
    bt_addr_le_t peer_addr;
    uint8_t own_addr_type;
    uint16_t conn_interval_min;
    uint16_t conn_interval_max;
    uint16_t conn_latency;
    uint16_t supervision_timeout;
    uint16_t min_ce_len;
    uint16_t max_ce_len;
} __packed;

#define BT_HCI_OP_LE_CREATE_CONN_CANCEL         BT_OP(BT_OGF_LE, 0x000e)

#define BT_HCI_OP_LE_READ_WL_SIZE               BT_OP(BT_OGF_LE, 0x000f)
struct bt_hci_rp_le_read_wl_size {
    uint8_t status;
    uint8_t wl_size;
} __packed;

#define BT_HCI_OP_LE_CLEAR_WL                   BT_OP(BT_OGF_LE, 0x0010)

#define BT_HCI_OP_LE_ADD_DEV_TO_WL              BT_OP(BT_OGF_LE, 0x0011)
struct bt_hci_cp_le_add_dev_to_wl {
    bt_addr_le_t addr;
} __packed;

#define BT_HCI_OP_LE_REM_DEV_FROM_WL            BT_OP(BT_OGF_LE, 0x0012)
struct bt_hci_cp_le_rem_dev_from_wl {
    bt_addr_le_t addr;
} __packed;

#define BT_HCI_OP_LE_CONN_UPDATE                BT_OP(BT_OGF_LE, 0x0013)
struct hci_cp_le_conn_update {
    uint16_t handle;
    uint16_t conn_interval_min;
    uint16_t conn_interval_max;
    uint16_t conn_latency;
    uint16_t supervision_timeout;
    uint16_t min_ce_len;
    uint16_t max_ce_len;
} __packed;

#define BT_HCI_OP_LE_SET_HOST_CHAN_CLASSIF      BT_OP(BT_OGF_LE, 0x0014)
struct bt_hci_cp_le_set_host_chan_classif {
    uint8_t ch_map[5];
} __packed;

#define BT_HCI_OP_LE_READ_CHAN_MAP              BT_OP(BT_OGF_LE, 0x0015)
struct bt_hci_cp_le_read_chan_map {
    uint16_t handle;
} __packed;
struct bt_hci_rp_le_read_chan_map {
    uint8_t status;
    uint16_t handle;
    uint8_t ch_map[5];
} __packed;

#define BT_HCI_OP_LE_READ_REMOTE_FEATURES       BT_OP(BT_OGF_LE, 0x0016)
struct bt_hci_cp_le_read_remote_features {
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_ENCRYPT                    BT_OP(BT_OGF_LE, 0x0017)
struct bt_hci_cp_le_encrypt {
    uint8_t key[16];
    uint8_t plaintext[16];
} __packed;
struct bt_hci_rp_le_encrypt {
    uint8_t status;
    uint8_t enc_data[16];
} __packed;

#define BT_HCI_OP_LE_RAND                       BT_OP(BT_OGF_LE, 0x0018)
struct bt_hci_rp_le_rand {
    uint8_t status;
    uint8_t rand[8];
} __packed;

#define BT_HCI_OP_LE_START_ENCRYPTION           BT_OP(BT_OGF_LE, 0x0019)
struct bt_hci_cp_le_start_encryption {
    uint16_t handle;
    uint64_t rand;
    uint16_t ediv;
    uint8_t ltk[16];
} __packed;

#define BT_HCI_OP_LE_LTK_REQ_REPLY              BT_OP(BT_OGF_LE, 0x001a)
struct bt_hci_cp_le_ltk_req_reply {
    uint16_t handle;
    uint8_t ltk[16];
} __packed;
struct bt_hci_rp_le_ltk_req_reply {
    uint8_t status;
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_LTK_REQ_NEG_REPLY          BT_OP(BT_OGF_LE, 0x001b)
struct bt_hci_cp_le_ltk_req_neg_reply {
    uint16_t handle;
} __packed;
struct bt_hci_rp_le_ltk_req_neg_reply {
    uint8_t status;
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_READ_SUPP_STATES           BT_OP(BT_OGF_LE, 0x001c)
struct bt_hci_rp_le_read_supp_states {
    uint8_t status;
    uint8_t le_states[8];
} __packed;

#define BT_HCI_OP_LE_RX_TEST                    BT_OP(BT_OGF_LE, 0x001d)
struct bt_hci_cp_le_rx_test {
    uint8_t rx_ch;
} __packed;

#define BT_HCI_OP_LE_TX_TEST                    BT_OP(BT_OGF_LE, 0x001e)
struct bt_hci_cp_le_tx_test {
    uint8_t tx_ch;
    uint8_t test_data_len;
    uint8_t pkt_payload;
} __packed;

#define BT_HCI_OP_LE_TEST_END                   BT_OP(BT_OGF_LE, 0x001f)
struct bt_hci_rp_le_test_end {
    uint8_t status;
    uint16_t rx_pkt_count;
} __packed;

#define BT_HCI_OP_LE_CONN_PARAM_REQ_REPLY       BT_OP(BT_OGF_LE, 0x0020)
struct bt_hci_cp_le_conn_param_req_reply {
    uint16_t handle;
    uint16_t interval_min;
    uint16_t interval_max;
    uint16_t latency;
    uint16_t timeout;
    uint16_t min_ce_len;
    uint16_t max_ce_len;
} __packed;
struct bt_hci_rp_le_conn_param_req_reply {
    uint8_t status;
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_CONN_PARAM_REQ_NEG_REPLY   BT_OP(BT_OGF_LE, 0x0021)
struct bt_hci_cp_le_conn_param_req_neg_reply {
    uint16_t handle;
    uint8_t reason;
} __packed;
struct bt_hci_rp_le_conn_param_req_neg_reply {
    uint8_t status;
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_SET_DATA_LEN               BT_OP(BT_OGF_LE, 0x0022)
struct bt_hci_cp_le_set_data_len {
    uint16_t handle;
    uint16_t tx_octets;
    uint16_t tx_time;
} __packed;
struct bt_hci_rp_le_set_data_len {
    uint8_t status;
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_READ_DEFAULT_DATA_LEN      BT_OP(BT_OGF_LE, 0x0023)
struct bt_hci_rp_le_read_default_data_len {
    uint8_t status;
    uint16_t max_tx_octets;
    uint16_t max_tx_time;
} __packed;

#define BT_HCI_OP_LE_WRITE_DEFAULT_DATA_LEN     BT_OP(BT_OGF_LE, 0x0024)
struct bt_hci_cp_le_write_default_data_len {
    uint16_t max_tx_octets;
    uint16_t max_tx_time;
} __packed;

#define BT_HCI_OP_LE_P256_PUBLIC_KEY            BT_OP(BT_OGF_LE, 0x0025)

#define BT_HCI_OP_LE_GENERATE_DHKEY             BT_OP(BT_OGF_LE, 0x0026)
struct bt_hci_cp_le_generate_dhkey {
    uint8_t key[64];
} __packed;

#define BT_HCI_OP_LE_ADD_DEV_TO_RL              BT_OP(BT_OGF_LE, 0x0027)
struct bt_hci_cp_le_add_dev_to_rl {
    bt_addr_le_t peer_id_addr;
    uint8_t peer_irk[16];
    uint8_t local_irk[16];
} __packed;

#define BT_HCI_OP_LE_REM_DEV_FROM_RL            BT_OP(BT_OGF_LE, 0x0028)
struct bt_hci_cp_le_rem_dev_from_rl {
    bt_addr_le_t peer_id_addr;
} __packed;

#define BT_HCI_OP_LE_CLEAR_RL                   BT_OP(BT_OGF_LE, 0x0029)

#define BT_HCI_OP_LE_READ_RL_SIZE               BT_OP(BT_OGF_LE, 0x002a)
struct bt_hci_rp_le_read_rl_size {
    uint8_t status;
    uint8_t rl_size;
} __packed;

#define BT_HCI_OP_LE_READ_PEER_RPA              BT_OP(BT_OGF_LE, 0x002b)
struct bt_hci_cp_le_read_peer_rpa {
    bt_addr_le_t peer_id_addr;
} __packed;
struct bt_hci_rp_le_read_peer_rpa {
    uint8_t status;
    bt_addr_t peer_rpa;
} __packed;

#define BT_HCI_OP_LE_READ_LOCAL_RPA             BT_OP(BT_OGF_LE, 0x002c)
struct bt_hci_cp_le_read_local_rpa {
    bt_addr_le_t peer_id_addr;
} __packed;
struct bt_hci_rp_le_read_local_rpa {
    uint8_t status;
    bt_addr_t local_rpa;
} __packed;

#define BT_HCI_ADDR_RES_DISABLE                 0x00
#define BT_HCI_ADDR_RES_ENABLE                  0x01

#define BT_HCI_OP_LE_SET_ADDR_RES_ENABLE        BT_OP(BT_OGF_LE, 0x002d)
struct bt_hci_cp_le_set_addr_res_enable {
    uint8_t enable;
} __packed;

#define BT_HCI_OP_LE_SET_RPA_TIMEOUT            BT_OP(BT_OGF_LE, 0x002e)
struct bt_hci_cp_le_set_rpa_timeout {
    uint16_t rpa_timeout;
} __packed;

#define BT_HCI_OP_LE_READ_MAX_DATA_LEN          BT_OP(BT_OGF_LE, 0x002f)
struct bt_hci_rp_le_read_max_data_len {
    uint8_t status;
    uint16_t max_tx_octets;
    uint16_t max_tx_time;
    uint16_t max_rx_octets;
    uint16_t max_rx_time;
} __packed;

#define BT_HCI_LE_PHY_1M                        0x01
#define BT_HCI_LE_PHY_2M                        0x02
#define BT_HCI_LE_PHY_CODED                     0x03

#define BT_HCI_OP_LE_READ_PHY                   BT_OP(BT_OGF_LE, 0x0030)
struct bt_hci_cp_le_read_phy {
    uint16_t handle;
} __packed;
struct bt_hci_rp_le_read_phy {
    uint8_t status;
    uint16_t handle;
    uint8_t tx_phy;
    uint8_t rx_phy;
} __packed;

#define BT_HCI_LE_PHY_TX_ANY                    BIT(0)
#define BT_HCI_LE_PHY_RX_ANY                    BIT(1)

#define BT_HCI_LE_PHY_PREFER_1M                 BIT(0)
#define BT_HCI_LE_PHY_PREFER_2M                 BIT(1)
#define BT_HCI_LE_PHY_PREFER_CODED              BIT(2)

#define BT_HCI_OP_LE_SET_DEFAULT_PHY            BT_OP(BT_OGF_LE, 0x0031)
struct bt_hci_cp_le_set_default_phy {
    uint8_t all_phys;
    uint8_t tx_phys;
    uint8_t rx_phys;
} __packed;

#define BT_HCI_LE_PHY_CODED_ANY                 0x00
#define BT_HCI_LE_PHY_CODED_S2                  0x01
#define BT_HCI_LE_PHY_CODED_S8                  0x02

#define BT_HCI_OP_LE_SET_PHY                    BT_OP(BT_OGF_LE, 0x0032)
struct bt_hci_cp_le_set_phy {
    uint16_t handle;
    uint8_t all_phys;
    uint8_t tx_phys;
    uint8_t rx_phys;
    uint16_t phy_opts;
} __packed;

#define BT_HCI_LE_MOD_INDEX_STANDARD            0x00
#define BT_HCI_LE_MOD_INDEX_STABLE              0x01

#define BT_HCI_OP_LE_ENH_RX_TEST                BT_OP(BT_OGF_LE, 0x0033)
struct bt_hci_cp_le_enh_rx_test {
    uint8_t rx_ch;
    uint8_t phy;
    uint8_t mod_index;
} __packed;

/* Extends BT_HCI_LE_PHY */
#define BT_HCI_LE_TX_PHY_CODED_S8               0x03
#define BT_HCI_LE_TX_PHY_CODED_S2               0x04

#define BT_HCI_OP_LE_ENH_TX_TEST                BT_OP(BT_OGF_LE, 0x0034)
struct bt_hci_cp_le_enh_tx_test {
    uint8_t tx_ch;
    uint8_t test_data_len;
    uint8_t pkt_payload;
    uint8_t phy;
} __packed;

#define BT_HCI_OP_LE_SET_ADV_SET_RANDOM_ADDR    BT_OP(BT_OGF_LE, 0x0035)
struct bt_hci_cp_le_set_adv_set_random_addr {
    uint8_t handle;
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_LE_ADV_PROP_CONN                 BIT(0)
#define BT_HCI_LE_ADV_PROP_SCAN                 BIT(1)
#define BT_HCI_LE_ADV_PROP_DIRECT               BIT(2)
#define BT_HCI_LE_ADV_PROP_HI_DC_CONN           BIT(3)
#define BT_HCI_LE_ADV_PROP_LEGACY               BIT(4)
#define BT_HCI_LE_ADV_PROP_ANON                 BIT(5)
#define BT_HCI_LE_ADV_PROP_TX_POWER             BIT(6)

#define BT_HCI_LE_ADV_SCAN_REQ_ENABLE  1
#define BT_HCI_LE_ADV_SCAN_REQ_DISABLE 0

#define BT_HCI_LE_ADV_TX_POWER_NO_PREF 0x7F

#define BT_HCI_OP_LE_SET_EXT_ADV_PARAM          BT_OP(BT_OGF_LE, 0x0036)
struct bt_hci_cp_le_set_ext_adv_param {
    uint8_t handle;
    uint16_t props;
    uint8_t prim_min_interval[3];
    uint8_t prim_max_interval[3];
    uint8_t prim_channel_map;
    uint8_t own_addr_type;
    bt_addr_le_t peer_addr;
    uint8_t filter_policy;
    int8_t tx_power;
    uint8_t prim_adv_phy;
    uint8_t sec_adv_max_skip;
    uint8_t sec_adv_phy;
    uint8_t sid;
    uint8_t scan_req_notify_enable;
} __packed;
struct bt_hci_rp_le_set_ext_adv_param {
    uint8_t status;
    int8_t tx_power;
} __packed;

#define BT_HCI_LE_EXT_ADV_OP_INTERM_FRAG        0x00
#define BT_HCI_LE_EXT_ADV_OP_FIRST_FRAG         0x01
#define BT_HCI_LE_EXT_ADV_OP_LAST_FRAG          0x02
#define BT_HCI_LE_EXT_ADV_OP_COMPLETE_DATA      0x03
#define BT_HCI_LE_EXT_ADV_OP_UNCHANGED_DATA     0x04

#define BT_HCI_LE_EXT_ADV_FRAG_ENABLED          0x00
#define BT_HCI_LE_EXT_ADV_FRAG_DISABLED         0x01

#define BT_HCI_LE_EXT_ADV_FRAG_MAX_LEN          251

#define BT_HCI_OP_LE_SET_EXT_ADV_DATA           BT_OP(BT_OGF_LE, 0x0037)
struct bt_hci_cp_le_set_ext_adv_data {
    uint8_t handle;
    uint8_t op;
    uint8_t frag_pref;
    uint8_t len;
    uint8_t data[251];
} __packed;

#define BT_HCI_OP_LE_SET_EXT_SCAN_RSP_DATA      BT_OP(BT_OGF_LE, 0x0038)
struct bt_hci_cp_le_set_ext_scan_rsp_data {
    uint8_t handle;
    uint8_t op;
    uint8_t frag_pref;
    uint8_t len;
    uint8_t data[251];
} __packed;

#define BT_HCI_OP_LE_SET_EXT_ADV_ENABLE         BT_OP(BT_OGF_LE, 0x0039)
struct bt_hci_ext_adv_set {
    uint8_t handle;
    uint16_t duration;
    uint8_t max_ext_adv_evts;
} __packed;

struct bt_hci_cp_le_set_ext_adv_enable {
    uint8_t enable;
    uint8_t set_num;
    struct bt_hci_ext_adv_set s[];
} __packed;

#define BT_HCI_OP_LE_READ_MAX_ADV_DATA_LEN      BT_OP(BT_OGF_LE, 0x003a)
struct bt_hci_rp_le_read_max_adv_data_len {
    uint8_t status;
    uint16_t max_adv_data_len;
} __packed;

#define BT_HCI_OP_LE_READ_NUM_ADV_SETS          BT_OP(BT_OGF_LE, 0x003b)
struct bt_hci_rp_le_read_num_adv_sets {
    uint8_t status;
    uint8_t num_sets;
} __packed;

#define BT_HCI_OP_LE_REMOVE_ADV_SET             BT_OP(BT_OGF_LE, 0x003c)
struct bt_hci_cp_le_remove_adv_set {
    uint8_t handle;
} __packed;

#define BT_HCI_OP_CLEAR_ADV_SETS                BT_OP(BT_OGF_LE, 0x003d)

#define BT_HCI_OP_LE_SET_PER_ADV_PARAM          BT_OP(BT_OGF_LE, 0x003e)
struct bt_hci_cp_le_set_per_adv_param {
    uint8_t handle;
    uint16_t min_interval;
    uint16_t max_interval;
    uint16_t props;
} __packed;

#define BT_HCI_OP_LE_SET_PER_ADV_DATA           BT_OP(BT_OGF_LE, 0x003f)
struct bt_hci_cp_le_set_per_adv_data {
    uint8_t handle;
    uint8_t op;
    uint8_t len;
    uint8_t data[251];
} __packed;

#define BT_HCI_OP_LE_SET_PER_ADV_ENABLE         BT_OP(BT_OGF_LE, 0x0040)
struct bt_hci_cp_le_set_per_adv_enable {
    uint8_t enable;
    uint8_t handle;
} __packed;

#define BT_HCI_OP_LE_SET_EXT_SCAN_PARAM         BT_OP(BT_OGF_LE, 0x0041)
struct bt_hci_ext_scan_phy {
    uint8_t type;
    uint16_t interval;
    uint16_t window;
} __packed;

#define BT_HCI_LE_EXT_SCAN_PHY_1M               BIT(0)
#define BT_HCI_LE_EXT_SCAN_PHY_2M               BIT(1)
#define BT_HCI_LE_EXT_SCAN_PHY_CODED            BIT(2)

struct bt_hci_cp_le_set_ext_scan_param {
    uint8_t own_addr_type;
    uint8_t filter_policy;
    uint8_t phys;
    struct bt_hci_ext_scan_phy p[];
} __packed;

/* Extends BT_HCI_LE_SCAN_FILTER_DUP */
#define BT_HCI_LE_EXT_SCAN_FILTER_DUP_ENABLE_RESET  0x02

#define BT_HCI_OP_LE_SET_EXT_SCAN_ENABLE        BT_OP(BT_OGF_LE, 0x0042)
struct bt_hci_cp_le_set_ext_scan_enable {
    uint8_t enable;
    uint8_t filter_dup;
    uint16_t duration;
    uint16_t period;
} __packed;

#define BT_HCI_OP_LE_EXT_CREATE_CONN            BT_OP(BT_OGF_LE, 0x0043)
struct bt_hci_ext_conn_phy {
    uint16_t scan_interval;
    uint16_t scan_window;
    uint16_t conn_interval_min;
    uint16_t conn_interval_max;
    uint16_t conn_latency;
    uint16_t supervision_timeout;
    uint16_t min_ce_len;
    uint16_t max_ce_len;
} __packed;

struct bt_hci_cp_le_ext_create_conn {
    uint8_t filter_policy;
    uint8_t own_addr_type;
    bt_addr_le_t peer_addr;
    uint8_t phys;
    struct bt_hci_ext_conn_phy p[];
} __packed;

#define BT_HCI_OP_LE_PER_ADV_CREATE_SYNC        BT_OP(BT_OGF_LE, 0x0044)
struct bt_hci_cp_le_per_adv_create_sync {
    uint8_t filter_policy;
    uint8_t sid;
    bt_addr_le_t addr;
    uint16_t skip;
    uint16_t sync_timeout;
    uint8_t unused;
} __packed;

#define BT_HCI_OP_LE_PER_ADV_CREATE_SYNC_CANCEL BT_OP(BT_OGF_LE, 0x0045)

#define BT_HCI_OP_LE_PER_ADV_TERMINATE_SYNC     BT_OP(BT_OGF_LE, 0x0046)
struct bt_hci_cp_le_per_adv_terminate_sync {
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_ADD_DEV_TO_PER_ADV_LIST    BT_OP(BT_OGF_LE, 0x0047)
struct bt_hci_cp_le_add_dev_to_per_adv_list {
    bt_addr_le_t addr;
    uint8_t sid;
} __packed;

#define BT_HCI_OP_LE_REM_DEV_FROM_PER_ADV_LIST  BT_OP(BT_OGF_LE, 0x0048)
struct bt_hci_cp_le_rem_dev_from_per_adv_list {
    bt_addr_le_t addr;
    uint8_t sid;
} __packed;

#define BT_HCI_OP_LE_CLEAR_PER_ADV_LIST         BT_OP(BT_OGF_LE, 0x0049)

#define BT_HCI_OP_LE_READ_PER_ADV_LIST_SIZE     BT_OP(BT_OGF_LE, 0x004a)
struct bt_hci_rp_le_read_per_adv_list_size {
    uint8_t status;
    uint8_t list_size;
} __packed;

#define BT_HCI_OP_LE_READ_TX_POWER              BT_OP(BT_OGF_LE, 0x004b)
struct bt_hci_rp_le_read_tx_power {
    uint8_t status;
    int8_t min_tx_power;
    int8_t max_tx_power;
} __packed;

#define BT_HCI_OP_LE_READ_RF_PATH_COMP          BT_OP(BT_OGF_LE, 0x004c)
struct bt_hci_rp_le_read_rf_path_comp {
    uint8_t status;
    int16_t tx_path_comp;
    int16_t rx_path_comp;
} __packed;

#define BT_HCI_OP_LE_WRITE_RF_PATH_COMP         BT_OP(BT_OGF_LE, 0x004d)
struct bt_hci_cp_le_write_rf_path_comp {
    int16_t tx_path_comp;
    int16_t rx_path_comp;
} __packed;

#define BT_HCI_LE_PRIVACY_MODE_NETWORK          0x00
#define BT_HCI_LE_PRIVACY_MODE_DEVICE           0x01

#define BT_HCI_OP_LE_SET_PRIVACY_MODE           BT_OP(BT_OGF_LE, 0x004e)
struct bt_hci_cp_le_set_privacy_mode {
    bt_addr_le_t id_addr;
    uint8_t mode;
} __packed;

/* Event definitions */

#define BT_HCI_EVT_UNKNOWN                      0x00
#define BT_HCI_EVT_VENDOR                       0xff

#define BT_HCI_EVT_INQUIRY_COMPLETE             0x01
struct bt_hci_evt_inquiry_complete {
    uint8_t status;
} __packed;

#define BT_HCI_EVT_CONN_COMPLETE                0x03
struct bt_hci_evt_conn_complete {
    uint8_t status;
    uint16_t handle;
    bt_addr_t bdaddr;
    uint8_t link_type;
    uint8_t encr_enabled;
} __packed;

#define BT_HCI_EVT_CONN_REQUEST                 0x04
struct bt_hci_evt_conn_request {
    bt_addr_t bdaddr;
    uint8_t dev_class[3];
    uint8_t link_type;
} __packed;

#define BT_HCI_EVT_DISCONN_COMPLETE             0x05
struct bt_hci_evt_disconn_complete {
    uint8_t status;
    uint16_t handle;
    uint8_t reason;
} __packed;

#define BT_HCI_EVT_AUTH_COMPLETE                0x06
struct bt_hci_evt_auth_complete {
    uint8_t status;
    uint16_t handle;
} __packed;

#define BT_HCI_EVT_REMOTE_NAME_REQ_COMPLETE     0x07
struct bt_hci_evt_remote_name_req_complete {
    uint8_t status;
    bt_addr_t bdaddr;
    uint8_t name[248];
} __packed;

#define BT_HCI_EVT_ENCRYPT_CHANGE               0x08
struct bt_hci_evt_encrypt_change {
    uint8_t status;
    uint16_t handle;
    uint8_t encrypt;
} __packed;

#define BT_HCI_EVT_REMOTE_FEATURES              0x0b
struct bt_hci_evt_remote_features {
    uint8_t status;
    uint16_t handle;
    uint8_t features[8];
} __packed;

#define BT_HCI_EVT_REMOTE_VERSION_INFO          0x0c
struct bt_hci_evt_remote_version_info {
    uint8_t status;
    uint16_t handle;
    uint8_t version;
    uint16_t manufacturer;
    uint16_t subversion;
} __packed;

#define BT_HCI_EVT_CMD_COMPLETE                 0x0e
struct bt_hci_evt_cmd_complete {
    uint8_t ncmd;
    uint16_t opcode;
} __packed;

struct bt_hci_evt_cc_status {
    uint8_t status;
} __packed;

#define BT_HCI_EVT_CMD_STATUS                   0x0f
struct bt_hci_evt_cmd_status {
    uint8_t status;
    uint8_t ncmd;
    uint16_t opcode;
} __packed;

#define BT_HCI_EVT_ROLE_CHANGE                  0x12
struct bt_hci_evt_role_change {
    uint8_t status;
    bt_addr_t bdaddr;
    uint8_t role;
} __packed;

#define BT_HCI_EVT_NUM_COMPLETED_PACKETS        0x13
struct bt_hci_evt_num_completed_packets {
    uint8_t num_handles;
    struct bt_hci_handle_count h[];
} __packed;

#define BT_HCI_EVT_PIN_CODE_REQ                 0x16
struct bt_hci_evt_pin_code_req {
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_EVT_LINK_KEY_REQ                 0x17
struct bt_hci_evt_link_key_req {
    bt_addr_t bdaddr;
} __packed;

/* Link Key types */
#define BT_LK_COMBINATION                       0x00
#define BT_LK_LOCAL_UNIT                        0x01
#define BT_LK_REMOTE_UNIT                       0x02
#define BT_LK_DEBUG_COMBINATION                 0x03
#define BT_LK_UNAUTH_COMBINATION_P192           0x04
#define BT_LK_AUTH_COMBINATION_P192             0x05
#define BT_LK_CHANGED_COMBINATION               0x06
#define BT_LK_UNAUTH_COMBINATION_P256           0x07
#define BT_LK_AUTH_COMBINATION_P256             0x08

#define BT_HCI_EVT_LINK_KEY_NOTIFY              0x18
struct bt_hci_evt_link_key_notify {
    bt_addr_t bdaddr;
    uint8_t link_key[16];
    uint8_t key_type;
} __packed;

/* Overflow link types */
#define BT_OVERFLOW_LINK_SYNCH                  0x00
#define BT_OVERFLOW_LINK_ACL                    0x01

#define BT_HCI_EVT_DATA_BUF_OVERFLOW            0x1a
struct bt_hci_evt_data_buf_overflow {
    uint8_t link_type;
} __packed;

#define BT_HCI_EVT_INQUIRY_RESULT_WITH_RSSI     0x22
struct bt_hci_evt_inquiry_result_with_rssi {
    bt_addr_t addr;
    uint8_t pscan_rep_mode;
    uint8_t reserved;
    uint8_t cod[3];
    uint16_t clock_offset;
    int8_t rssi;
} __packed;

#define BT_HCI_EVT_REMOTE_EXT_FEATURES          0x23
struct bt_hci_evt_remote_ext_features {
    uint8_t status;
    uint16_t handle;
    uint8_t page;
    uint8_t max_page;
    uint8_t features[8];
} __packed;

#define BT_HCI_EVT_SYNC_CONN_COMPLETE           0x2c
struct bt_hci_evt_sync_conn_complete {
    uint8_t status;
    uint16_t handle;
    bt_addr_t bdaddr;
    uint8_t link_type;
    uint8_t tx_interval;
    uint8_t retansmission_window;
    uint16_t rx_pkt_length;
    uint16_t tx_pkt_length;
    uint8_t air_mode;
} __packed;

#define BT_HCI_EVT_EXTENDED_INQUIRY_RESULT      0x2f
struct bt_hci_evt_extended_inquiry_result {
    uint8_t num_reports;
    bt_addr_t addr;
    uint8_t pscan_rep_mode;
    uint8_t reserved;
    uint8_t cod[3];
    uint16_t clock_offset;
    int8_t rssi;
    uint8_t eir[240];
} __packed;

#define BT_HCI_EVT_ENCRYPT_KEY_REFRESH_COMPLETE 0x30
struct bt_hci_evt_encrypt_key_refresh_complete {
    uint8_t status;
    uint16_t handle;
} __packed;

#define BT_HCI_EVT_IO_CAPA_REQ                  0x31
struct bt_hci_evt_io_capa_req {
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_EVT_IO_CAPA_RESP                 0x32
struct bt_hci_evt_io_capa_resp {
    bt_addr_t bdaddr;
    uint8_t capability;
    uint8_t oob_data;
    uint8_t authentication;
} __packed;

#define BT_HCI_EVT_USER_CONFIRM_REQ             0x33
struct bt_hci_evt_user_confirm_req {
    bt_addr_t bdaddr;
    uint32_t passkey;
} __packed;

#define BT_HCI_EVT_USER_PASSKEY_REQ             0x34
struct bt_hci_evt_user_passkey_req {
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_EVT_SSP_COMPLETE                 0x36
struct bt_hci_evt_ssp_complete {
    uint8_t status;
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_EVT_USER_PASSKEY_NOTIFY          0x3b
struct bt_hci_evt_user_passkey_notify {
    bt_addr_t bdaddr;
    uint32_t passkey;
} __packed;

#define BT_HCI_EVT_LE_META_EVENT                0x3e
struct bt_hci_evt_le_meta_event {
    uint8_t subevent;
} __packed;

#define BT_HCI_EVT_AUTH_PAYLOAD_TIMEOUT_EXP     0x57
struct bt_hci_evt_auth_payload_timeout_exp {
    uint16_t handle;
} __packed;

#define BT_HCI_ROLE_MASTER                      0x00
#define BT_HCI_ROLE_SLAVE                       0x01

#define BT_HCI_EVT_LE_CONN_COMPLETE             0x01
struct bt_hci_evt_le_conn_complete {
    uint8_t status;
    uint16_t handle;
    uint8_t role;
    bt_addr_le_t peer_addr;
    uint16_t interval;
    uint16_t latency;
    uint16_t supv_timeout;
    uint8_t clock_accuracy;
} __packed;

#define BT_HCI_EVT_LE_ADVERTISING_REPORT        0x02
struct bt_hci_evt_le_advertising_info {
    uint8_t evt_type;
    bt_addr_le_t addr;
    uint8_t length;
    uint8_t data[];
} __packed;
struct bt_hci_evt_le_advertising_report {
    uint8_t num_reports;
    struct bt_hci_evt_le_advertising_info adv_info[];
} __packed;

#define BT_HCI_EVT_LE_CONN_UPDATE_COMPLETE      0x03
struct bt_hci_evt_le_conn_update_complete {
    uint8_t status;
    uint16_t handle;
    uint16_t interval;
    uint16_t latency;
    uint16_t supv_timeout;
} __packed;

#define BT_HCI_EV_LE_REMOTE_FEAT_COMPLETE       0x04
struct bt_hci_evt_le_remote_feat_complete {
    uint8_t status;
    uint16_t handle;
    uint8_t features[8];
} __packed;

#define BT_HCI_EVT_LE_LTK_REQUEST               0x05
struct bt_hci_evt_le_ltk_request {
    uint16_t handle;
    uint64_t rand;
    uint16_t ediv;
} __packed;

#define BT_HCI_EVT_LE_CONN_PARAM_REQ            0x06
struct bt_hci_evt_le_conn_param_req {
    uint16_t handle;
    uint16_t interval_min;
    uint16_t interval_max;
    uint16_t latency;
    uint16_t timeout;
} __packed;

#define BT_HCI_EVT_LE_DATA_LEN_CHANGE           0x07
struct bt_hci_evt_le_data_len_change {
    uint16_t handle;
    uint16_t max_tx_octets;
    uint16_t max_tx_time;
    uint16_t max_rx_octets;
    uint16_t max_rx_time;
} __packed;

#define BT_HCI_EVT_LE_P256_PUBLIC_KEY_COMPLETE  0x08
struct bt_hci_evt_le_p256_public_key_complete {
    uint8_t status;
    uint8_t key[64];
} __packed;

#define BT_HCI_EVT_LE_GENERATE_DHKEY_COMPLETE   0x09
struct bt_hci_evt_le_generate_dhkey_complete {
    uint8_t status;
    uint8_t dhkey[32];
} __packed;

#define BT_HCI_EVT_LE_ENH_CONN_COMPLETE         0x0a
struct bt_hci_evt_le_enh_conn_complete {
    uint8_t status;
    uint16_t handle;
    uint8_t role;
    bt_addr_le_t peer_addr;
    bt_addr_t local_rpa;
    bt_addr_t peer_rpa;
    uint16_t interval;
    uint16_t latency;
    uint16_t supv_timeout;
    uint8_t clock_accuracy;
} __packed;

#define BT_HCI_EVT_LE_DIRECT_ADV_REPORT         0x0b
struct bt_hci_evt_le_direct_adv_info {
    uint8_t evt_type;
    bt_addr_le_t addr;
    bt_addr_le_t dir_addr;
    int8_t rssi;
} __packed;
struct bt_hci_evt_le_direct_adv_report {
    uint8_t num_reports;
    struct bt_hci_evt_le_direct_adv_info direct_adv_info[];
} __packed;

#define BT_HCI_EVT_LE_PHY_UPDATE_COMPLETE       0x0c
struct bt_hci_evt_le_phy_update_complete {
    uint8_t status;
    uint16_t handle;
    uint8_t tx_phy;
    uint8_t rx_phy;
} __packed;

#define BT_HCI_EVT_LE_EXT_ADVERTISING_REPORT    0x0d

#define BT_HCI_LE_ADV_EVT_TYPE_CONN                 BIT(0)
#define BT_HCI_LE_ADV_EVT_TYPE_SCAN                 BIT(1)
#define BT_HCI_LE_ADV_EVT_TYPE_DIRECT               BIT(2)
#define BT_HCI_LE_ADV_EVT_TYPE_SCAN_RSP             BIT(3)
#define BT_HCI_LE_ADV_EVT_TYPE_LEGACY               BIT(4)

#define BT_HCI_LE_ADV_EVT_TYPE_DATA_STATUS(ev_type) (((ev_type) >> 5) & 0x03)
#define BT_HCI_LE_ADV_EVT_TYPE_DATA_STATUS_COMPLETE   0
#define BT_HCI_LE_ADV_EVT_TYPE_DATA_STATUS_PARTIAL    1
#define BT_HCI_LE_ADV_EVT_TYPE_DATA_STATUS_INCOMPLETE 2

struct bt_hci_evt_le_ext_advertising_info {
    uint16_t evt_type;
    bt_addr_le_t addr;
    uint8_t prim_phy;
    uint8_t sec_phy;
    uint8_t sid;
    int8_t tx_power;
    int8_t rssi;
    uint16_t interval;
    bt_addr_le_t direct_addr;
    uint8_t length;
    uint8_t data[];
} __packed;
struct bt_hci_evt_le_ext_advertising_report {
    uint8_t num_reports;
    struct bt_hci_evt_le_ext_advertising_info adv_info[];
} __packed;

#define BT_HCI_EVT_LE_PER_ADV_SYNC_ESTABLISHED  0x0e
struct bt_hci_evt_le_per_adv_sync_established {
    uint8_t status;
    uint16_t handle;
    uint8_t sid;
    bt_addr_le_t adv_addr;
    uint8_t phy;
    uint16_t interval;
    uint8_t clock_accuracy;
} __packed;

#define BT_HCI_EVT_LE_PER_ADVERTISING_REPORT    0x0f
struct bt_hci_evt_le_per_advertising_report {
    uint16_t handle;
    int8_t tx_power;
    int8_t rssi;
    uint8_t unused;
    uint8_t data_status;
    uint8_t length;
    uint8_t data[];
} __packed;

#define BT_HCI_EVT_LE_PER_ADV_SYNC_LOST         0x10
struct bt_hci_evt_le_per_adv_sync_lost {
    uint16_t handle;
} __packed;

#define BT_HCI_EVT_LE_SCAN_TIMEOUT              0x11

#define BT_HCI_EVT_LE_ADV_SET_TERMINATED        0x12
struct bt_hci_evt_le_adv_set_terminated {
    uint8_t status;
    uint8_t adv_handle;
    uint16_t conn_handle;
    uint8_t num_completed_ext_adv_evts;
} __packed;

#define BT_HCI_EVT_LE_SCAN_REQ_RECEIVED         0x13
struct bt_hci_evt_le_scan_req_received {
    uint8_t handle;
    bt_addr_le_t addr;
} __packed;

#define BT_HCI_LE_CHAN_SEL_ALGO_1               0x00
#define BT_HCI_LE_CHAN_SEL_ALGO_2               0x01

#define BT_HCI_EVT_LE_CHAN_SEL_ALGO             0x14
struct bt_hci_evt_le_chan_sel_algo {
    uint16_t handle;
    uint8_t chan_sel_algo;
} __packed;

/* Event mask bits */

#define BT_EVT_BIT(n) (1ULL << (n))

#define BT_EVT_MASK_INQUIRY_COMPLETE             BT_EVT_BIT(0)
#define BT_EVT_MASK_CONN_COMPLETE                BT_EVT_BIT(2)
#define BT_EVT_MASK_CONN_REQUEST                 BT_EVT_BIT(3)
#define BT_EVT_MASK_DISCONN_COMPLETE             BT_EVT_BIT(4)
#define BT_EVT_MASK_AUTH_COMPLETE                BT_EVT_BIT(5)
#define BT_EVT_MASK_REMOTE_NAME_REQ_COMPLETE     BT_EVT_BIT(6)
#define BT_EVT_MASK_ENCRYPT_CHANGE               BT_EVT_BIT(7)
#define BT_EVT_MASK_REMOTE_FEATURES              BT_EVT_BIT(10)
#define BT_EVT_MASK_REMOTE_VERSION_INFO          BT_EVT_BIT(11)
#define BT_EVT_MASK_HARDWARE_ERROR               BT_EVT_BIT(15)
#define BT_EVT_MASK_ROLE_CHANGE                  BT_EVT_BIT(17)
#define BT_EVT_MASK_PIN_CODE_REQ                 BT_EVT_BIT(21)
#define BT_EVT_MASK_LINK_KEY_REQ                 BT_EVT_BIT(22)
#define BT_EVT_MASK_LINK_KEY_NOTIFY              BT_EVT_BIT(23)
#define BT_EVT_MASK_DATA_BUFFER_OVERFLOW         BT_EVT_BIT(25)
#define BT_EVT_MASK_INQUIRY_RESULT_WITH_RSSI     BT_EVT_BIT(33)
#define BT_EVT_MASK_REMOTE_EXT_FEATURES          BT_EVT_BIT(34)
#define BT_EVT_MASK_SYNC_CONN_COMPLETE           BT_EVT_BIT(43)
#define BT_EVT_MASK_EXTENDED_INQUIRY_RESULT      BT_EVT_BIT(46)
#define BT_EVT_MASK_ENCRYPT_KEY_REFRESH_COMPLETE BT_EVT_BIT(47)
#define BT_EVT_MASK_IO_CAPA_REQ                  BT_EVT_BIT(48)
#define BT_EVT_MASK_IO_CAPA_RESP                 BT_EVT_BIT(49)
#define BT_EVT_MASK_USER_CONFIRM_REQ             BT_EVT_BIT(50)
#define BT_EVT_MASK_USER_PASSKEY_REQ             BT_EVT_BIT(51)
#define BT_EVT_MASK_SSP_COMPLETE                 BT_EVT_BIT(53)
#define BT_EVT_MASK_USER_PASSKEY_NOTIFY          BT_EVT_BIT(58)
#define BT_EVT_MASK_LE_META_EVENT                BT_EVT_BIT(61)

/* Page 2 */
#define BT_EVT_MASK_PHY_LINK_COMPLETE            BT_EVT_BIT(0)
#define BT_EVT_MASK_CH_SELECTED_COMPLETE         BT_EVT_BIT(1)
#define BT_EVT_MASK_DISCONN_PHY_LINK_COMPLETE    BT_EVT_BIT(2)
#define BT_EVT_MASK_PHY_LINK_LOSS_EARLY_WARN     BT_EVT_BIT(3)
#define BT_EVT_MASK_PHY_LINK_RECOVERY            BT_EVT_BIT(4)
#define BT_EVT_MASK_LOG_LINK_COMPLETE            BT_EVT_BIT(5)
#define BT_EVT_MASK_DISCONN_LOG_LINK_COMPLETE    BT_EVT_BIT(6)
#define BT_EVT_MASK_FLOW_SPEC_MODIFY_COMPLETE    BT_EVT_BIT(7)
#define BT_EVT_MASK_NUM_COMPLETE_DATA_BLOCKS     BT_EVT_BIT(8)
#define BT_EVT_MASK_AMP_START_TEST               BT_EVT_BIT(9)
#define BT_EVT_MASK_AMP_TEST_END                 BT_EVT_BIT(10)
#define BT_EVT_MASK_AMP_RX_REPORT                BT_EVT_BIT(11)
#define BT_EVT_MASK_AMP_SR_MODE_CHANGE_COMPLETE  BT_EVT_BIT(12)
#define BT_EVT_MASK_AMP_STATUS_CHANGE            BT_EVT_BIT(13)
#define BT_EVT_MASK_TRIGG_CLOCK_CAPTURE          BT_EVT_BIT(14)
#define BT_EVT_MASK_SYNCH_TRAIN_COMPLETE         BT_EVT_BIT(15)
#define BT_EVT_MASK_SYNCH_TRAIN_RX               BT_EVT_BIT(16)
#define BT_EVT_MASK_CL_SLAVE_BC_RX               BT_EVT_BIT(17)
#define BT_EVT_MASK_CL_SLAVE_BC_TIMEOUT          BT_EVT_BIT(18)
#define BT_EVT_MASK_TRUNC_PAGE_COMPLETE          BT_EVT_BIT(19)
#define BT_EVT_MASK_SLAVE_PAGE_RSP_TIMEOUT       BT_EVT_BIT(20)
#define BT_EVT_MASK_CL_SLAVE_BC_CH_MAP_CHANGE    BT_EVT_BIT(21)
#define BT_EVT_MASK_INQUIRY_RSP_NOT              BT_EVT_BIT(22)
#define BT_EVT_MASK_AUTH_PAYLOAD_TIMEOUT_EXP     BT_EVT_BIT(23)
#define BT_EVT_MASK_SAM_STATUS_CHANGE            BT_EVT_BIT(24)

#define BT_EVT_MASK_LE_CONN_COMPLETE             BT_EVT_BIT(0)
#define BT_EVT_MASK_LE_ADVERTISING_REPORT        BT_EVT_BIT(1)
#define BT_EVT_MASK_LE_CONN_UPDATE_COMPLETE      BT_EVT_BIT(2)
#define BT_EVT_MASK_LE_REMOTE_FEAT_COMPLETE      BT_EVT_BIT(3)
#define BT_EVT_MASK_LE_LTK_REQUEST               BT_EVT_BIT(4)
#define BT_EVT_MASK_LE_CONN_PARAM_REQ            BT_EVT_BIT(5)
#define BT_EVT_MASK_LE_DATA_LEN_CHANGE           BT_EVT_BIT(6)
#define BT_EVT_MASK_LE_P256_PUBLIC_KEY_COMPLETE  BT_EVT_BIT(7)
#define BT_EVT_MASK_LE_GENERATE_DHKEY_COMPLETE   BT_EVT_BIT(8)
#define BT_EVT_MASK_LE_ENH_CONN_COMPLETE         BT_EVT_BIT(9)
#define BT_EVT_MASK_LE_DIRECT_ADV_REPORT         BT_EVT_BIT(10)
#define BT_EVT_MASK_LE_PHY_UPDATE_COMPLETE       BT_EVT_BIT(11)
#define BT_EVT_MASK_LE_EXT_ADVERTISING_REPORT    BT_EVT_BIT(12)
#define BT_EVT_MASK_LE_PER_ADV_SYNC_ESTABLISHED  BT_EVT_BIT(13)
#define BT_EVT_MASK_LE_PER_ADVERTISING_REPORT    BT_EVT_BIT(14)
#define BT_EVT_MASK_LE_PER_ADV_SYNC_LOST         BT_EVT_BIT(15)
#define BT_EVT_MASK_LE_SCAN_TIMEOUT              BT_EVT_BIT(16)
#define BT_EVT_MASK_LE_ADV_SET_TERMINATED        BT_EVT_BIT(17)
#define BT_EVT_MASK_LE_SCAN_REQ_RECEIVED         BT_EVT_BIT(18)
#define BT_EVT_MASK_LE_CHAN_SEL_ALGO             BT_EVT_BIT(19)

//

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_HCI_H_ */
