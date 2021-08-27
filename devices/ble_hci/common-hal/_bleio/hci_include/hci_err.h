/** @file
 *  @brief Bluetooth Host Control Interface status codes.
 */

/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_BLUETOOTH_HCI_STATUS_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_HCI_STATUS_H_

#ifdef __cplusplus
extern "C" {
#endif

/** HCI Error Codes, BT Core Spec v5.2 [Vol 1, Part F]. */
#define BT_HCI_ERR_SUCCESS                      0x00
#define BT_HCI_ERR_UNKNOWN_CMD                  0x01
#define BT_HCI_ERR_UNKNOWN_CONN_ID              0x02
#define BT_HCI_ERR_HW_FAILURE                   0x03
#define BT_HCI_ERR_PAGE_TIMEOUT                 0x04
#define BT_HCI_ERR_AUTH_FAIL                    0x05
#define BT_HCI_ERR_PIN_OR_KEY_MISSING           0x06
#define BT_HCI_ERR_MEM_CAPACITY_EXCEEDED        0x07
#define BT_HCI_ERR_CONN_TIMEOUT                 0x08
#define BT_HCI_ERR_CONN_LIMIT_EXCEEDED          0x09
#define BT_HCI_ERR_SYNC_CONN_LIMIT_EXCEEDED     0x0a
#define BT_HCI_ERR_CONN_ALREADY_EXISTS          0x0b
#define BT_HCI_ERR_CMD_DISALLOWED               0x0c
#define BT_HCI_ERR_INSUFFICIENT_RESOURCES       0x0d
#define BT_HCI_ERR_INSUFFICIENT_SECURITY        0x0e
#define BT_HCI_ERR_BD_ADDR_UNACCEPTABLE         0x0f
#define BT_HCI_ERR_CONN_ACCEPT_TIMEOUT          0x10
#define BT_HCI_ERR_UNSUPP_FEATURE_PARAM_VAL     0x11
#define BT_HCI_ERR_INVALID_PARAM                0x12
#define BT_HCI_ERR_REMOTE_USER_TERM_CONN        0x13
#define BT_HCI_ERR_REMOTE_LOW_RESOURCES         0x14
#define BT_HCI_ERR_REMOTE_POWER_OFF             0x15
#define BT_HCI_ERR_LOCALHOST_TERM_CONN          0x16
#define BT_HCI_ERR_REPEATED_ATTEMPTS            0x17
#define BT_HCI_ERR_PAIRING_NOT_ALLOWED          0x18
#define BT_HCI_ERR_UNKNOWN_LMP_PDU              0x19
#define BT_HCI_ERR_UNSUPP_REMOTE_FEATURE        0x1a
#define BT_HCI_ERR_SCO_OFFSET_REJECTED          0x1b
#define BT_HCI_ERR_SCO_INTERVAL_REJECTED        0x1c
#define BT_HCI_ERR_SCO_AIR_MODE_REJECTED        0x1d
#define BT_HCI_ERR_INVALID_LL_PARAM             0x1e
#define BT_HCI_ERR_UNSPECIFIED                  0x1f
#define BT_HCI_ERR_UNSUPP_LL_PARAM_VAL          0x20
#define BT_HCI_ERR_ROLE_CHANGE_NOT_ALLOWED      0x21
#define BT_HCI_ERR_LL_RESP_TIMEOUT              0x22
#define BT_HCI_ERR_LL_PROC_COLLISION            0x23
#define BT_HCI_ERR_LMP_PDU_NOT_ALLOWED          0x24
#define BT_HCI_ERR_ENC_MODE_NOT_ACCEPTABLE      0x25
#define BT_HCI_ERR_LINK_KEY_CANNOT_BE_CHANGED   0x26
#define BT_HCI_ERR_REQUESTED_QOS_NOT_SUPPORTED  0x27
#define BT_HCI_ERR_INSTANT_PASSED               0x28
#define BT_HCI_ERR_PAIRING_NOT_SUPPORTED        0x29
#define BT_HCI_ERR_DIFF_TRANS_COLLISION         0x2a
#define BT_HCI_ERR_QOS_UNACCEPTABLE_PARAM       0x2c
#define BT_HCI_ERR_QOS_REJECTED                 0x2d
#define BT_HCI_ERR_CHAN_ASSESS_NOT_SUPPORTED    0x2e
#define BT_HCI_ERR_INSUFF_SECURITY              0x2f
#define BT_HCI_ERR_PARAM_OUT_OF_MANDATORY_RANGE 0x30
#define BT_HCI_ERR_ROLE_SWITCH_PENDING          0x32
#define BT_HCI_ERR_RESERVED_SLOT_VIOLATION      0x34
#define BT_HCI_ERR_ROLE_SWITCH_FAILED           0x35
#define BT_HCI_ERR_EXT_INQ_RESP_TOO_LARGE       0x36
#define BT_HCI_ERR_SIMPLE_PAIR_NOT_SUPP_BY_HOST 0x37
#define BT_HCI_ERR_HOST_BUSY_PAIRING            0x38
#define BT_HCI_ERR_CONN_REJECTED_DUE_TO_NO_CHAN 0x39
#define BT_HCI_ERR_CONTROLLER_BUSY              0x3a
#define BT_HCI_ERR_UNACCEPT_CONN_PARAM          0x3b
#define BT_HCI_ERR_ADV_TIMEOUT                  0x3c
#define BT_HCI_ERR_TERM_DUE_TO_MIC_FAIL         0x3d
#define BT_HCI_ERR_CONN_FAIL_TO_ESTAB           0x3e
#define BT_HCI_ERR_MAC_CONN_FAILED              0x3f
#define BT_HCI_ERR_CLOCK_ADJUST_REJECTED        0x40
#define BT_HCI_ERR_SUBMAP_NOT_DEFINED           0x41
#define BT_HCI_ERR_UNKNOWN_ADV_IDENTIFIER       0x42
#define BT_HCI_ERR_LIMIT_REACHED                0x43
#define BT_HCI_ERR_OP_CANCELLED_BY_HOST         0x44
#define BT_HCI_ERR_PACKET_TOO_LONG              0x45

#define BT_HCI_ERR_AUTHENTICATION_FAIL __DEPRECATED_MACRO BT_HCI_ERR_AUTH_FAIL

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_HCI_STATUS_H_ */
