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

/*
 *  l2cap.h
 *
 *  Logical Link Control and Adaption Protocol (L2CAP)
 *
 *  Created by Matthias Ringwald on 5/16/09.
 */

#ifndef __L2CAP_H
#define __L2CAP_H

#include "hci.h"
#include "l2cap_signaling.h"
#include "btstack_util.h"
#include "bluetooth.h"

#if defined __cplusplus
extern "C" {
#endif
    
// check L2CAP MTU
#if (L2CAP_MINIMAL_MTU + L2CAP_HEADER_SIZE) > HCI_ACL_PAYLOAD_SIZE
#error "HCI_ACL_PAYLOAD_SIZE too small for minimal L2CAP MTU of 48 bytes"
#endif    
    
// private structs
typedef enum {
    L2CAP_STATE_CLOSED = 1,           // no baseband
    L2CAP_STATE_WILL_SEND_CREATE_CONNECTION,
    L2CAP_STATE_WAIT_CONNECTION_COMPLETE,
    L2CAP_STATE_WAIT_REMOTE_SUPPORTED_FEATURES,
    L2CAP_STATE_WAIT_INCOMING_SECURITY_LEVEL_UPDATE,
    L2CAP_STATE_WAIT_OUTGOING_SECURITY_LEVEL_UPDATE,
    L2CAP_STATE_WAIT_CLIENT_ACCEPT_OR_REJECT,
    L2CAP_STATE_WAIT_CONNECT_RSP, // from peer
    L2CAP_STATE_CONFIG,
    L2CAP_STATE_OPEN,
    L2CAP_STATE_WAIT_DISCONNECT,  // from application
    L2CAP_STATE_WILL_SEND_CONNECTION_REQUEST,
    L2CAP_STATE_WILL_SEND_CONNECTION_RESPONSE_INSUFFICIENT_SECURITY,
    L2CAP_STATE_WILL_SEND_CONNECTION_RESPONSE_DECLINE,
    L2CAP_STATE_WILL_SEND_CONNECTION_RESPONSE_ACCEPT,   
    L2CAP_STATE_WILL_SEND_DISCONNECT_REQUEST,
    L2CAP_STATE_WILL_SEND_DISCONNECT_RESPONSE,
    L2CAP_STATE_INVALID,
} L2CAP_STATE;

typedef enum {
    L2CAP_CHANNEL_STATE_VAR_NONE                  = 0,
    L2CAP_CHANNEL_STATE_VAR_RCVD_CONF_REQ         = 1 << 0,
    L2CAP_CHANNEL_STATE_VAR_RCVD_CONF_RSP         = 1 << 1,
    L2CAP_CHANNEL_STATE_VAR_SEND_CONF_REQ         = 1 << 2,
    L2CAP_CHANNEL_STATE_VAR_SEND_CONF_RSP         = 1 << 3,
    L2CAP_CHANNEL_STATE_VAR_SENT_CONF_REQ         = 1 << 4,
    L2CAP_CHANNEL_STATE_VAR_SENT_CONF_RSP         = 1 << 5,
    L2CAP_CHANNEL_STATE_VAR_SEND_CONF_RSP_MTU     = 1 << 6,   // in CONF RSP, add MTU field
    L2CAP_CHANNEL_STATE_VAR_SEND_CONF_RSP_CONT    = 1 << 7,   // in CONF RSP, set CONTINUE flag
    L2CAP_CHANNEL_STATE_VAR_SEND_CONF_RSP_INVALID = 1 << 8,   // in CONF RSP, send UNKNOWN OPTIONS
    L2CAP_CHANNEL_STATE_VAR_SEND_CMD_REJ_UNKNOWN  = 1 << 9,   // send CMD_REJ with reason unknown
    L2CAP_CHANNEL_STATE_VAR_SEND_CONN_RESP_PEND   = 1 << 10,  // send Connection Respond with pending
} L2CAP_CHANNEL_STATE_VAR;

// info regarding an actual connection
typedef struct {
    // linked list - assert: first field
    btstack_linked_item_t    item;
    
    // packet handler
    btstack_packet_handler_t packet_handler;

    // timer
    btstack_timer_source_t rtx; // also used for ertx

    L2CAP_STATE state;
    L2CAP_CHANNEL_STATE_VAR state_var;

    // info
    bd_addr_t address;
    hci_con_handle_t con_handle;
    
    uint8_t   remote_sig_id;    // used by other side, needed for delayed response
    uint8_t   local_sig_id;     // own signaling identifier
    
    uint16_t  local_cid;
    uint16_t  remote_cid;
    
    uint16_t  local_mtu;
    uint16_t  remote_mtu;
    
    uint16_t  flush_timeout;    // default 0xffff

    uint16_t  psm;
    
    gap_security_level_t required_security_level;

    uint8_t   reason; // used in decline internal
    uint8_t   waiting_for_can_send_now;
    
} l2cap_channel_t;

// info regarding potential connections
typedef struct {
    // linked list - assert: first field
    btstack_linked_item_t    item;
    
    // service id
    uint16_t  psm;
    
    // incoming MTU
    uint16_t mtu;

    // incoming MPS
    uint16_t mps;
    
    // internal connection
    btstack_packet_handler_t packet_handler;

    // required security level
    gap_security_level_t required_security_level;    
} l2cap_service_t;


typedef struct l2cap_signaling_response {
    hci_con_handle_t handle;
    uint8_t  sig_id;
    uint8_t  code;
    uint16_t data; // infoType for INFORMATION REQUEST, result for CONNECTION request and command unknown
} l2cap_signaling_response_t;
    

void l2cap_register_fixed_channel(btstack_packet_handler_t packet_handler, uint16_t channel_id);
int  l2cap_can_send_fixed_channel_packet_now(hci_con_handle_t con_handle, uint16_t channel_id);
int  l2cap_send_connectionless(hci_con_handle_t con_handle, uint16_t cid, uint8_t *data, uint16_t len);
int  l2cap_send_prepared_connectionless(hci_con_handle_t con_handle, uint16_t cid, uint16_t len);

// PTS Testing
int l2cap_send_echo_request(hci_con_handle_t con_handle, uint8_t *data, uint16_t len);
void l2cap_require_security_level_2_for_outgoing_sdp(void);

/* API_START */

/** 
 * @brief Set up L2CAP and register L2CAP with HCI layer.
 */
void l2cap_init(void);

/** 
 * @brief Registers packet handler for LE Connection Parameter Update events
 */
void l2cap_register_packet_handler(void (*handler)(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size));

/** 
 * @brief Get max MTU for Classic connections based on btstack configuration
 */
uint16_t l2cap_max_mtu(void);

/** 
 * @brief Get max MTU for LE connections based on btstack configuration
 */
uint16_t l2cap_max_le_mtu(void);

/** 
 * @brief Creates L2CAP channel to the PSM of a remote device with baseband address. A new baseband connection will be initiated if necessary.
 * @param packet_handler
 * @param address
 * @param psm
 * @param mtu
 * @param local_cid
 * @param status
 */
uint8_t l2cap_create_channel(btstack_packet_handler_t packet_handler, bd_addr_t address, uint16_t psm, uint16_t mtu, uint16_t * out_local_cid);

/** 
 * @brief Disconnects L2CAP channel with given identifier. 
 */
void l2cap_disconnect(uint16_t local_cid, uint8_t reason);

/** 
 * @brief Queries the maximal transfer unit (MTU) for L2CAP channel with given identifier. 
 */
uint16_t l2cap_get_remote_mtu_for_local_cid(uint16_t local_cid);

/** 
 * @brief Sends L2CAP data packet to the channel with given identifier.
 */
int l2cap_send(uint16_t local_cid, uint8_t *data, uint16_t len);

/** 
 * @brief Registers L2CAP service with given PSM and MTU, and assigns a packet handler.
 */
uint8_t l2cap_register_service(btstack_packet_handler_t packet_handler, uint16_t psm, uint16_t mtu, gap_security_level_t security_level);

/** 
 * @brief Unregisters L2CAP service with given PSM.  On embedded systems, use NULL for connection parameter.
 */
void l2cap_unregister_service(uint16_t psm);

/** 
 * @brief Accepts incoming L2CAP connection.
 */
void l2cap_accept_connection(uint16_t local_cid);

/** 
 * @brief Deny incoming L2CAP connection.
 */
void l2cap_decline_connection(uint16_t local_cid, uint8_t reason);

/** 
 * @brief Check if outgoing buffer is available and that there's space on the Bluetooth module
 */
int  l2cap_can_send_packet_now(uint16_t local_cid);    

/** 
 * @brief Check if there's space on the Bluetooth module
 */
int  l2cap_can_send_prepared_packet_now(uint16_t local_cid);

/** 
 * @brief Reserve outgoing buffer
 */
int  l2cap_reserve_packet_buffer(void);

/** 
 * @brief Get outgoing buffer and prepare data.
 */
uint8_t *l2cap_get_outgoing_buffer(void);

/** 
 * @brief Send L2CAP packet prepared in outgoing buffer to channel
 */
int l2cap_send_prepared(uint16_t local_cid, uint16_t len);

/** 
 * @brief Release outgoing buffer (only needed if l2cap_send_prepared is not called)
 */
void l2cap_release_packet_buffer(void);

/* API_END */

#if 0

// to be implemented soon
/**
 * @brief Regster L2CAP LE Credit Based Flow Control Mode service
 * @param
 */
void l2cap_le_register_service(btstack_packet_handler_t packet_handler, uint16_t psm,
    uint16_t mtu, uint16_t mps, uint16_t initial_credits, gap_security_level_t security_level);
void l2cap_le_unregister_service(uint16_t psm);
#endif

#if defined __cplusplus
}
#endif

#endif // __L2CAP_H
