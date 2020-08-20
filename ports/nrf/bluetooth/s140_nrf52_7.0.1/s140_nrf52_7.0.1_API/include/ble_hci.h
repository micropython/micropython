/*
 * Copyright (c) 2012 - 2017, Nordic Semiconductor ASA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
  @addtogroup BLE_COMMON
  @{
*/


#ifndef BLE_HCI_H__
#define BLE_HCI_H__
#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup BLE_HCI_STATUS_CODES Bluetooth status codes
 * @{ */

#define BLE_HCI_STATUS_CODE_SUCCESS                        0x00   /**< Success. */
#define BLE_HCI_STATUS_CODE_UNKNOWN_BTLE_COMMAND           0x01   /**< Unknown BLE Command. */
#define BLE_HCI_STATUS_CODE_UNKNOWN_CONNECTION_IDENTIFIER  0x02   /**< Unknown Connection Identifier. */
/*0x03 Hardware Failure
0x04 Page Timeout
*/
#define BLE_HCI_AUTHENTICATION_FAILURE                     0x05   /**< Authentication Failure. */
#define BLE_HCI_STATUS_CODE_PIN_OR_KEY_MISSING             0x06   /**< Pin or Key missing. */
#define BLE_HCI_MEMORY_CAPACITY_EXCEEDED                   0x07   /**< Memory Capacity Exceeded. */
#define BLE_HCI_CONNECTION_TIMEOUT                         0x08   /**< Connection Timeout. */
/*0x09 Connection Limit Exceeded
0x0A Synchronous Connection Limit To A Device Exceeded
0x0B ACL Connection Already Exists*/
#define BLE_HCI_STATUS_CODE_COMMAND_DISALLOWED             0x0C   /**< Command Disallowed. */
/*0x0D Connection Rejected due to Limited Resources
0x0E Connection Rejected Due To Security Reasons
0x0F Connection Rejected due to Unacceptable BD_ADDR
0x10 Connection Accept Timeout Exceeded
0x11 Unsupported Feature or Parameter Value*/
#define BLE_HCI_STATUS_CODE_INVALID_BTLE_COMMAND_PARAMETERS 0x12  /**< Invalid BLE Command Parameters. */
#define BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION           0x13  /**< Remote User Terminated Connection. */
#define BLE_HCI_REMOTE_DEV_TERMINATION_DUE_TO_LOW_RESOURCES 0x14  /**< Remote Device Terminated Connection due to low resources.*/
#define BLE_HCI_REMOTE_DEV_TERMINATION_DUE_TO_POWER_OFF     0x15  /**< Remote Device Terminated Connection due to power off. */
#define BLE_HCI_LOCAL_HOST_TERMINATED_CONNECTION            0x16  /**< Local Host Terminated Connection. */
/*
0x17 Repeated Attempts
0x18 Pairing Not Allowed
0x19 Unknown LMP PDU
*/
#define BLE_HCI_UNSUPPORTED_REMOTE_FEATURE 0x1A                   /**< Unsupported Remote Feature. */
/*
0x1B SCO Offset Rejected
0x1C SCO Interval Rejected
0x1D SCO Air Mode Rejected*/
#define BLE_HCI_STATUS_CODE_INVALID_LMP_PARAMETERS     0x1E       /**< Invalid LMP Parameters. */
#define BLE_HCI_STATUS_CODE_UNSPECIFIED_ERROR          0x1F       /**< Unspecified Error. */
/*0x20 Unsupported LMP Parameter Value
0x21 Role Change Not Allowed
*/
#define BLE_HCI_STATUS_CODE_LMP_RESPONSE_TIMEOUT            0x22       /**< LMP Response Timeout. */
#define BLE_HCI_STATUS_CODE_LMP_ERROR_TRANSACTION_COLLISION 0x23  /**< LMP Error Transaction Collision/LL Procedure Collision. */
#define BLE_HCI_STATUS_CODE_LMP_PDU_NOT_ALLOWED             0x24       /**< LMP PDU Not Allowed. */
/*0x25 Encryption Mode Not Acceptable
0x26 Link Key Can Not be Changed
0x27 Requested QoS Not Supported
*/
#define BLE_HCI_INSTANT_PASSED                         0x28       /**< Instant Passed. */
#define BLE_HCI_PAIRING_WITH_UNIT_KEY_UNSUPPORTED      0x29       /**< Pairing with Unit Key Unsupported. */
#define BLE_HCI_DIFFERENT_TRANSACTION_COLLISION        0x2A       /**< Different Transaction Collision. */
/*
0x2B Reserved
0x2C QoS Unacceptable Parameter
0x2D QoS Rejected
0x2E Channel Classification Not Supported
0x2F Insufficient Security
*/
#define BLE_HCI_PARAMETER_OUT_OF_MANDATORY_RANGE       0x30            /**< Parameter Out Of Mandatory Range. */
/*
0x31 Reserved
0x32 Role Switch Pending
0x33 Reserved
0x34 Reserved Slot Violation
0x35 Role Switch Failed
0x36 Extended Inquiry Response Too Large
0x37 Secure Simple Pairing Not Supported By Host.
0x38 Host Busy - Pairing
0x39 Connection Rejected due to No Suitable Channel Found*/
#define BLE_HCI_CONTROLLER_BUSY                        0x3A       /**< Controller Busy. */
#define BLE_HCI_CONN_INTERVAL_UNACCEPTABLE             0x3B       /**< Connection Interval Unacceptable. */
#define BLE_HCI_DIRECTED_ADVERTISER_TIMEOUT            0x3C       /**< Directed Advertisement Timeout. */
#define BLE_HCI_CONN_TERMINATED_DUE_TO_MIC_FAILURE     0x3D       /**< Connection Terminated due to MIC Failure. */
#define BLE_HCI_CONN_FAILED_TO_BE_ESTABLISHED          0x3E       /**< Connection Failed to be Established. */

/** @} */


#ifdef __cplusplus
}
#endif
#endif // BLE_HCI_H__

/** @} */
