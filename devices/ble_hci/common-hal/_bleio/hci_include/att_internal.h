// CircuitPython: Adapted from Zephyr include file.

/* att_internal.h - Attribute protocol handling */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <stdbool.h>
// for __packed
#include <sys/cdefs.h>

#define BT_EATT_PSM             0x27
#define BT_ATT_DEFAULT_LE_MTU   23
#define BT_ATT_TIMEOUT          K_SECONDS(30)

// FIX #if BT_L2CAP_RX_MTU < CONFIG_BT_L2CAP_TX_MTU
// #define BT_ATT_MTU BT_L2CAP_RX_MTU
// #else
// #define BT_ATT_MTU CONFIG_BT_L2CAP_TX_MTU
// #endif

struct bt_att_hdr {
    uint8_t code;
} __packed;

#define BT_ATT_OP_ERROR_RSP                     0x01
struct bt_att_error_rsp {
    uint8_t request;
    uint16_t handle;
    uint8_t error;
} __packed;

#define BT_ATT_OP_MTU_REQ                       0x02
struct bt_att_exchange_mtu_req {
    uint16_t mtu;
} __packed;

#define BT_ATT_OP_MTU_RSP                       0x03
struct bt_att_exchange_mtu_rsp {
    uint16_t mtu;
} __packed;

/* Find Information Request */
#define BT_ATT_OP_FIND_INFO_REQ                 0x04
struct bt_att_find_info_req {
    uint16_t start_handle;
    uint16_t end_handle;
} __packed;

/* Format field values for BT_ATT_OP_FIND_INFO_RSP */
#define BT_ATT_INFO_16                          0x01
#define BT_ATT_INFO_128                         0x02

struct bt_att_info_16 {
    uint16_t handle;
    uint16_t uuid;
} __packed;

struct bt_att_info_128 {
    uint16_t handle;
    uint8_t uuid[16];
} __packed;

/* Find Information Response */
#define BT_ATT_OP_FIND_INFO_RSP                 0x05
struct bt_att_find_info_rsp {
    uint8_t format;
    uint8_t info[];
} __packed;

/* Find By Type Value Request */
#define BT_ATT_OP_FIND_TYPE_REQ                 0x06
struct bt_att_find_type_req {
    uint16_t start_handle;
    uint16_t end_handle;
    uint16_t type;
    uint8_t value[];
} __packed;

struct bt_att_handle_group {
    uint16_t start_handle;
    uint16_t end_handle;
} __packed;

/* Find By Type Value Response */
#define BT_ATT_OP_FIND_TYPE_RSP                 0x07
struct bt_att_find_type_rsp {
    uint8_t _dummy[0];
    struct bt_att_handle_group list[];
} __packed;

/* Read By Type Request */
#define BT_ATT_OP_READ_TYPE_REQ                 0x08
struct bt_att_read_type_req {
    uint16_t start_handle;
    uint16_t end_handle;
    uint8_t uuid[];
} __packed;

struct bt_att_data {
    uint16_t handle;
    uint8_t value[];
} __packed;

/* Read By Type Response */
#define BT_ATT_OP_READ_TYPE_RSP                 0x09
struct bt_att_read_type_rsp {
    uint8_t len;
    struct bt_att_data data[];
} __packed;

/* Read Request */
#define BT_ATT_OP_READ_REQ                      0x0a
struct bt_att_read_req {
    uint16_t handle;
} __packed;

/* Read Response */
#define BT_ATT_OP_READ_RSP                      0x0b
struct bt_att_read_rsp {
    uint8_t _dummy[0];
    uint8_t value[];
} __packed;

/* Read Blob Request */
#define BT_ATT_OP_READ_BLOB_REQ                 0x0c
struct bt_att_read_blob_req {
    uint16_t handle;
    uint16_t offset;
} __packed;

/* Read Blob Response */
#define BT_ATT_OP_READ_BLOB_RSP                 0x0d
struct bt_att_read_blob_rsp {
    uint8_t _dummy[0];
    uint8_t value[];
} __packed;

/* Read Multiple Request */
#define BT_ATT_READ_MULT_MIN_LEN_REQ            0x04

#define BT_ATT_OP_READ_MULT_REQ                 0x0e
struct bt_att_read_mult_req {
    uint8_t _dummy[0];
    uint16_t handles[];
} __packed;

/* Read Multiple Respose */
#define BT_ATT_OP_READ_MULT_RSP                 0x0f
struct bt_att_read_mult_rsp {
    uint8_t _dummy[0];
    uint8_t value[];
} __packed;

/* Read by Group Type Request */
#define BT_ATT_OP_READ_GROUP_REQ                0x10
struct bt_att_read_group_req {
    uint16_t start_handle;
    uint16_t end_handle;
    uint8_t uuid[];
} __packed;

struct bt_att_group_data {
    uint16_t start_handle;
    uint16_t end_handle;
    uint8_t value[];
} __packed;

/* Read by Group Type Response */
#define BT_ATT_OP_READ_GROUP_RSP                0x11
struct bt_att_read_group_rsp {
    uint8_t len;
    struct bt_att_group_data data[];
} __packed;

/* Write Request */
#define BT_ATT_OP_WRITE_REQ                     0x12
struct bt_att_write_req {
    uint16_t handle;
    uint8_t value[];
} __packed;

/* Write Response */
#define BT_ATT_OP_WRITE_RSP                     0x13

/* Prepare Write Request */
#define BT_ATT_OP_PREPARE_WRITE_REQ             0x16
struct bt_att_prepare_write_req {
    uint16_t handle;
    uint16_t offset;
    uint8_t value[];
} __packed;

/* Prepare Write Respond */
#define BT_ATT_OP_PREPARE_WRITE_RSP             0x17
struct bt_att_prepare_write_rsp {
    uint16_t handle;
    uint16_t offset;
    uint8_t value[];
} __packed;

/* Execute Write Request */
#define BT_ATT_FLAG_CANCEL                      0x00
#define BT_ATT_FLAG_EXEC                        0x01

#define BT_ATT_OP_EXEC_WRITE_REQ                0x18
struct bt_att_exec_write_req {
    uint8_t flags;
} __packed;

/* Execute Write Response */
#define BT_ATT_OP_EXEC_WRITE_RSP                0x19

/* Handle Value Notification */
#define BT_ATT_OP_NOTIFY                        0x1b
struct bt_att_notify {
    uint16_t handle;
    uint8_t value[];
} __packed;

/* Handle Value Indication */
#define BT_ATT_OP_INDICATE                      0x1d
struct bt_att_indicate {
    uint16_t handle;
    uint8_t value[];
} __packed;

/* Handle Value Confirm */
#define BT_ATT_OP_CONFIRM                       0x1e

struct bt_att_signature {
    uint8_t value[12];
} __packed;

#define BT_ATT_OP_READ_MULT_VL_REQ              0x20
struct bt_att_read_mult_vl_req {
    uint8_t _dummy[0];
    uint16_t handles[];
} __packed;

/* Read Multiple Respose */
#define BT_ATT_OP_READ_MULT_VL_RSP              0x21
struct bt_att_read_mult_vl_rsp {
    uint16_t len;
    uint8_t value[];
} __packed;

/* Handle Multiple Value Notification */
#define BT_ATT_OP_NOTIFY_MULT                   0x23
struct bt_att_notify_mult {
    uint16_t handle;
    uint16_t len;
    uint8_t value[];
} __packed;

/* Write Command */
#define BT_ATT_OP_WRITE_CMD                     0x52
struct bt_att_write_cmd {
    uint16_t handle;
    uint8_t value[];
} __packed;

/* Signed Write Command */
#define BT_ATT_OP_SIGNED_WRITE_CMD              0xd2
struct bt_att_signed_write_cmd {
    uint16_t handle;
    uint8_t value[];
} __packed;
