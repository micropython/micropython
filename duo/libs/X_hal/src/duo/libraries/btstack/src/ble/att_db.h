/*
 * Copyright (C) 2014 BlueKitchen GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 4. Any redistribution, use, or modification is done solely for
 *    personal benefit and not for any commercial purpose or for
 *    monetary gain.
 *
 * THIS SOFTWARE IS PROVIDED BY BLUEKITCHEN GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MATTHIAS
 * RINGWALD OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Please inquire about commercial licensing options at 
 * contact@bluekitchen-gmbh.com
 *
 */


#ifndef __ATT_H
#define __ATT_H

#include <stdint.h>
#include "bluetooth.h"
 
#if defined __cplusplus
extern "C" {
#endif

// custom BTstack error codes
#define ATT_ERROR_HCI_DISCONNECT_RECEIVED          0x1f

// custom BTstack ATT error coders
#define ATT_ERROR_DATA_MISMATCH                    0x7e
#define ATT_ERROR_TIMEOUT                          0x7F
    
typedef struct att_connection {
    hci_con_handle_t con_handle;
    uint16_t mtu;       // initialized to ATT_DEFAULT_MTU (23), negotiated during MTU exchange
    uint16_t max_mtu;   // local maximal L2CAP_MTU, set to l2cap_max_le_mtu()
    uint8_t  encryption_key_size;
    uint8_t  authenticated;
    uint8_t  authorized;
} att_connection_t;

// ATT Client Read Callback for Dynamic Data
// - if buffer == NULL, don't copy data, just return size of value
// - if buffer != NULL, copy data and return number bytes copied
// @param con_handle of hci le connection
// @param attribute_handle to be read
// @param offset defines start of attribute value
// @param buffer 
// @param buffer_size
typedef uint16_t (*att_read_callback_t)(hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size);

// ATT Client Write Callback for Dynamic Data
// @param con_handle of hci le connection
// @param attribute_handle to be written
// @param transaction - ATT_TRANSACTION_MODE_NONE for regular writes, ATT_TRANSACTION_MODE_ACTIVE for prepared writes and ATT_TRANSACTION_MODE_EXECUTE
// @param offset into the value - used for queued writes and long attributes
// @param buffer 
// @param buffer_size
// @param signature used for signed write commmands
// @returns 0 if write was ok, ATT_ERROR_PREPARE_QUEUE_FULL if no space in queue, ATT_ERROR_INVALID_OFFSET if offset is larger than max buffer
typedef int (*att_write_callback_t)(hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size);

// MARK: ATT Operations

/*
 * @brief setup ATT database
 */
void att_set_db(uint8_t const * db);

/*
 * @brief set callback for read of dynamic attributes
 * @param callback
 */
void att_set_read_callback(att_read_callback_t callback);

/*
 * @brief set callback for write of dynamic attributes
 * @param callback
 */
void att_set_write_callback(att_write_callback_t callback);

/*
 * @brief debug helper, dump ATT database to stdout using log_info
 */
void att_dump_attributes(void);

/*
 * @brief process ATT request against database and put response into response buffer
 * @param att_connection used for mtu and security properties
 * @param request_buffer, request_len: ATT request from clinet
 * @param response_buffer for result
 * @returns len of data in response buffer. 0 = no response
 */
uint16_t att_handle_request(att_connection_t * att_connection,
                            uint8_t * request_buffer,
                            uint16_t request_len,
                            uint8_t * response_buffer);

/*
 * @brief setup value notification in response buffer for a given handle and value
 * @param att_connection
 * @param attribute_handle
 * @param value
 * @param value_len
 * @param response_buffer for notification
 */
uint16_t att_prepare_handle_value_notification(att_connection_t * att_connection,
                                               uint16_t attribute_handle,
                                               uint8_t *value,
                                               uint16_t value_len, 
                                               uint8_t * response_buffer);

/*
 * @brief setup value indication in response buffer for a given handle and value
 * @param att_connection
 * @param attribute_handle
 * @param value
 * @param value_len
 * @param response_buffer for indication
 */
uint16_t att_prepare_handle_value_indication(att_connection_t * att_connection,
                                             uint16_t attribute_handle,
                                             uint8_t *value,
                                             uint16_t value_len, 
                                             uint8_t * response_buffer);

/*
 * @brief transcation queue of prepared writes, e.g., after disconnect
 */
void att_clear_transaction_queue(att_connection_t * att_connection);

// experimental client API
uint16_t att_uuid_for_handle(uint16_t attribute_handle);

#if defined __cplusplus
}
#endif

#endif // __ATT_H
