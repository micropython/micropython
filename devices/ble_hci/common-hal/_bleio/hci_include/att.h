// CircuitPython: Adapted from Zephyr include file.
/** @file
 *  @brief Attribute Protocol handling.
 */

/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_BLUETOOTH_ATT_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_ATT_H_

/* Error codes for Error response PDU */
#define BT_ATT_ERR_INVALID_HANDLE               0x01
#define BT_ATT_ERR_READ_NOT_PERMITTED           0x02
#define BT_ATT_ERR_WRITE_NOT_PERMITTED          0x03
#define BT_ATT_ERR_INVALID_PDU                  0x04
#define BT_ATT_ERR_AUTHENTICATION               0x05
#define BT_ATT_ERR_NOT_SUPPORTED                0x06
#define BT_ATT_ERR_INVALID_OFFSET               0x07
#define BT_ATT_ERR_AUTHORIZATION                0x08
#define BT_ATT_ERR_PREPARE_QUEUE_FULL           0x09
#define BT_ATT_ERR_ATTRIBUTE_NOT_FOUND          0x0a
#define BT_ATT_ERR_ATTRIBUTE_NOT_LONG           0x0b
#define BT_ATT_ERR_ENCRYPTION_KEY_SIZE          0x0c
#define BT_ATT_ERR_INVALID_ATTRIBUTE_LEN        0x0d
#define BT_ATT_ERR_UNLIKELY                     0x0e
#define BT_ATT_ERR_INSUFFICIENT_ENCRYPTION      0x0f
#define BT_ATT_ERR_UNSUPPORTED_GROUP_TYPE       0x10
#define BT_ATT_ERR_INSUFFICIENT_RESOURCES       0x11
#define BT_ATT_ERR_DB_OUT_OF_SYNC               0x12
#define BT_ATT_ERR_VALUE_NOT_ALLOWED            0x13

/* Common Profile Error Codes (from CSS) */
#define BT_ATT_ERR_WRITE_REQ_REJECTED           0xfc
#define BT_ATT_ERR_CCC_IMPROPER_CONF            0xfd
#define BT_ATT_ERR_PROCEDURE_IN_PROGRESS        0xfe
#define BT_ATT_ERR_OUT_OF_RANGE                 0xff

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_ATT_H_ */
