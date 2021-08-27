// CircuitPython: Adapted from Zephyr include file.

/** @file
 *  @brief Internal APIs for Bluetooth L2CAP handling.
 */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
// for __packed
#include <string.h>

enum l2cap_conn_list_action {
    BT_L2CAP_CHAN_LOOKUP,
    BT_L2CAP_CHAN_DETACH,
};

#define BT_L2CAP_CID_BR_SIG             0x0001
#define BT_L2CAP_CID_ATT                0x0004
#define BT_L2CAP_CID_LE_SIG             0x0005
#define BT_L2CAP_CID_SMP                0x0006
#define BT_L2CAP_CID_BR_SMP             0x0007

#define BT_L2CAP_PSM_RFCOMM             0x0003

struct bt_l2cap_hdr {
    uint16_t len;
    uint16_t cid;
} __packed;

struct bt_l2cap_sig_hdr {
    uint8_t code;
    uint8_t ident;
    uint16_t len;
} __packed;

#define BT_L2CAP_REJ_NOT_UNDERSTOOD     0x0000
#define BT_L2CAP_REJ_MTU_EXCEEDED       0x0001
#define BT_L2CAP_REJ_INVALID_CID        0x0002

#define BT_L2CAP_CMD_REJECT             0x01
struct bt_l2cap_cmd_reject {
    uint16_t reason;
    uint8_t data[];
} __packed;

struct bt_l2cap_cmd_reject_cid_data {
    uint16_t scid;
    uint16_t dcid;
} __packed;

#define BT_L2CAP_CONN_REQ               0x02
struct bt_l2cap_conn_req {
    uint16_t psm;
    uint16_t scid;
} __packed;

/* command statuses in reposnse */
#define BT_L2CAP_CS_NO_INFO             0x0000
#define BT_L2CAP_CS_AUTHEN_PEND         0x0001

/* valid results in conn response on BR/EDR */
#define BT_L2CAP_BR_SUCCESS             0x0000
#define BT_L2CAP_BR_PENDING             0x0001
#define BT_L2CAP_BR_ERR_PSM_NOT_SUPP    0x0002
#define BT_L2CAP_BR_ERR_SEC_BLOCK       0x0003
#define BT_L2CAP_BR_ERR_NO_RESOURCES    0x0004
#define BT_L2CAP_BR_ERR_INVALID_SCID    0x0006
#define BT_L2CAP_BR_ERR_SCID_IN_USE     0x0007

#define BT_L2CAP_CONN_RSP               0x03
struct bt_l2cap_conn_rsp {
    uint16_t dcid;
    uint16_t scid;
    uint16_t result;
    uint16_t status;
} __packed;

#define BT_L2CAP_CONF_SUCCESS           0x0000
#define BT_L2CAP_CONF_UNACCEPT          0x0001
#define BT_L2CAP_CONF_REJECT            0x0002

#define BT_L2CAP_CONF_REQ               0x04
struct bt_l2cap_conf_req {
    uint16_t dcid;
    uint16_t flags;
    uint8_t data[];
} __packed;

#define BT_L2CAP_CONF_RSP               0x05
struct bt_l2cap_conf_rsp {
    uint16_t scid;
    uint16_t flags;
    uint16_t result;
    uint8_t data[];
} __packed;

/* Option type used by MTU config request data */
#define BT_L2CAP_CONF_OPT_MTU           0x01
/* Options bits selecting most significant bit (hint) in type field */
#define BT_L2CAP_CONF_HINT              0x80
#define BT_L2CAP_CONF_MASK              0x7f

struct bt_l2cap_conf_opt {
    uint8_t type;
    uint8_t len;
    uint8_t data[];
} __packed;

#define BT_L2CAP_DISCONN_REQ            0x06
struct bt_l2cap_disconn_req {
    uint16_t dcid;
    uint16_t scid;
} __packed;

#define BT_L2CAP_DISCONN_RSP            0x07
struct bt_l2cap_disconn_rsp {
    uint16_t dcid;
    uint16_t scid;
} __packed;

#define BT_L2CAP_INFO_FEAT_MASK         0x0002
#define BT_L2CAP_INFO_FIXED_CHAN        0x0003

#define BT_L2CAP_INFO_REQ               0x0a
struct bt_l2cap_info_req {
    uint16_t type;
} __packed;

/* info result */
#define BT_L2CAP_INFO_SUCCESS           0x0000
#define BT_L2CAP_INFO_NOTSUPP           0x0001

#define BT_L2CAP_INFO_RSP               0x0b
struct bt_l2cap_info_rsp {
    uint16_t type;
    uint16_t result;
    uint8_t data[];
} __packed;

#define BT_L2CAP_CONN_PARAM_REQ         0x12
struct bt_l2cap_conn_param_req {
    uint16_t min_interval;
    uint16_t max_interval;
    uint16_t latency;
    uint16_t timeout;
} __packed;

#define BT_L2CAP_CONN_PARAM_ACCEPTED    0x0000
#define BT_L2CAP_CONN_PARAM_REJECTED    0x0001

#define BT_L2CAP_CONN_PARAM_RSP         0x13
struct bt_l2cap_conn_param_rsp {
    uint16_t result;
} __packed;

#define BT_L2CAP_LE_CONN_REQ            0x14
struct bt_l2cap_le_conn_req {
    uint16_t psm;
    uint16_t scid;
    uint16_t mtu;
    uint16_t mps;
    uint16_t credits;
} __packed;

/* valid results in conn response on LE */
#define BT_L2CAP_LE_SUCCESS             0x0000
#define BT_L2CAP_LE_ERR_PSM_NOT_SUPP    0x0002
#define BT_L2CAP_LE_ERR_NO_RESOURCES    0x0004
#define BT_L2CAP_LE_ERR_AUTHENTICATION  0x0005
#define BT_L2CAP_LE_ERR_AUTHORIZATION   0x0006
#define BT_L2CAP_LE_ERR_KEY_SIZE        0x0007
#define BT_L2CAP_LE_ERR_ENCRYPTION      0x0008
#define BT_L2CAP_LE_ERR_INVALID_SCID    0x0009
#define BT_L2CAP_LE_ERR_SCID_IN_USE     0x000A
#define BT_L2CAP_LE_ERR_UNACCEPT_PARAMS 0x000B
#define BT_L2CAP_LE_ERR_INVALID_PARAMS  0x000C

#define BT_L2CAP_LE_CONN_RSP            0x15
struct bt_l2cap_le_conn_rsp {
    uint16_t dcid;
    uint16_t mtu;
    uint16_t mps;
    uint16_t credits;
    uint16_t result;
} __packed;

#define BT_L2CAP_LE_CREDITS             0x16
struct bt_l2cap_le_credits {
    uint16_t cid;
    uint16_t credits;
} __packed;

#define BT_L2CAP_ECRED_CONN_REQ         0x17
struct bt_l2cap_ecred_conn_req {
    uint16_t psm;
    uint16_t mtu;
    uint16_t mps;
    uint16_t credits;
    uint16_t scid[];
} __packed;

#define BT_L2CAP_ECRED_CONN_RSP         0x18
struct bt_l2cap_ecred_conn_rsp {
    uint16_t mtu;
    uint16_t mps;
    uint16_t credits;
    uint16_t result;
    uint16_t dcid[];
} __packed;

#define BT_L2CAP_ECRED_RECONF_REQ       0x19
struct bt_l2cap_ecred_reconf_req {
    uint16_t mtu;
    uint16_t mps;
    uint16_t scid[];
} __packed;

#define BT_L2CAP_RECONF_SUCCESS         0x0000
#define BT_L2CAP_RECONF_INVALID_MTU     0x0001
#define BT_L2CAP_RECONF_INVALID_MPS     0x0002

#define BT_L2CAP_ECRED_RECONF_RSP       0x1a
struct bt_l2cap_ecred_reconf_rsp {
    uint16_t result;
} __packed;
