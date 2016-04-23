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
 *  RFCOMM.h
 */

#ifndef __RFCOMM_H
#define __RFCOMM_H
 
#include "btstack_util.h"

#include <stdint.h>

#if defined __cplusplus
extern "C" {
#endif
    
#define UNLIMITED_INCOMING_CREDITS 0xff

#define RFCOMM_TEST_DATA_MAX_LEN 4

#define RFCOMM_RLS_STATUS_INVALID 0xff


// private structs
typedef enum {
	RFCOMM_MULTIPLEXER_CLOSED = 1,
	RFCOMM_MULTIPLEXER_W4_CONNECT,  // outgoing
	RFCOMM_MULTIPLEXER_SEND_SABM_0,     //    "
	RFCOMM_MULTIPLEXER_W4_UA_0,     //    "
	RFCOMM_MULTIPLEXER_W4_SABM_0,   // incoming
    RFCOMM_MULTIPLEXER_SEND_UA_0,
	RFCOMM_MULTIPLEXER_OPEN,
    RFCOMM_MULTIPLEXER_SEND_UA_0_AND_DISC
} RFCOMM_MULTIPLEXER_STATE;

typedef enum {
    MULT_EV_READY_TO_SEND = 1,
} RFCOMM_MULTIPLEXER_EVENT;

typedef enum {
	RFCOMM_CHANNEL_CLOSED = 1,
	RFCOMM_CHANNEL_W4_MULTIPLEXER,
	RFCOMM_CHANNEL_SEND_UIH_PN,
    RFCOMM_CHANNEL_W4_PN_RSP,
	RFCOMM_CHANNEL_SEND_SABM_W4_UA,
	RFCOMM_CHANNEL_W4_UA,
    RFCOMM_CHANNEL_INCOMING_SETUP,
    RFCOMM_CHANNEL_DLC_SETUP,
	RFCOMM_CHANNEL_OPEN,
    RFCOMM_CHANNEL_SEND_UA_AFTER_DISC,
    RFCOMM_CHANNEL_SEND_DISC,
    RFCOMM_CHANNEL_W4_UA_AFTER_UA,
    RFCOMM_CHANNEL_SEND_DM,
} RFCOMM_CHANNEL_STATE;

typedef enum {
    RFCOMM_CHANNEL_STATE_VAR_NONE            = 0,
    RFCOMM_CHANNEL_STATE_VAR_CLIENT_ACCEPTED = 1 << 0,
    RFCOMM_CHANNEL_STATE_VAR_RCVD_PN         = 1 << 1,
    RFCOMM_CHANNEL_STATE_VAR_RCVD_RPN        = 1 << 2,
    RFCOMM_CHANNEL_STATE_VAR_RCVD_SABM       = 1 << 3,
    
    RFCOMM_CHANNEL_STATE_VAR_RCVD_MSC_CMD    = 1 << 4,
    RFCOMM_CHANNEL_STATE_VAR_RCVD_MSC_RSP    = 1 << 5,
    RFCOMM_CHANNEL_STATE_VAR_SEND_PN_RSP     = 1 << 6,
    RFCOMM_CHANNEL_STATE_VAR_SEND_RPN_INFO   = 1 << 7,
    
    RFCOMM_CHANNEL_STATE_VAR_SEND_RPN_RSP    = 1 << 8,
    RFCOMM_CHANNEL_STATE_VAR_SEND_UA         = 1 << 9,
    RFCOMM_CHANNEL_STATE_VAR_SEND_MSC_CMD    = 1 << 10,
    RFCOMM_CHANNEL_STATE_VAR_SEND_MSC_RSP    = 1 << 11,
    
    RFCOMM_CHANNEL_STATE_VAR_SEND_CREDITS    = 1 << 12,
    RFCOMM_CHANNEL_STATE_VAR_SENT_MSC_CMD    = 1 << 13,
    RFCOMM_CHANNEL_STATE_VAR_SENT_MSC_RSP    = 1 << 14,
    RFCOMM_CHANNEL_STATE_VAR_SENT_CREDITS    = 1 << 15,
} RFCOMM_CHANNEL_STATE_VAR;

typedef struct rfcomm_rpn_data {
    uint8_t baud_rate;
    uint8_t flags;
    uint8_t flow_control;
    uint8_t xon;
    uint8_t xoff;
    uint8_t parameter_mask_0;   // first byte
    uint8_t parameter_mask_1;   // second byte
} rfcomm_rpn_data_t;

// info regarding potential connections
typedef struct {
    // linked list - assert: first field
    btstack_linked_item_t    item;
	
    // packet handler
    btstack_packet_handler_t packet_handler;

    // server channel
    uint8_t server_channel;
    
    // incoming max frame size
    uint16_t max_frame_size;

    // use incoming flow control
    uint8_t incoming_flow_control;
    
    // initial incoming credits
    uint8_t incoming_initial_credits;
    
    
} rfcomm_service_t;

// info regarding multiplexer
// note: spec mandates single multiplexer per device combination
typedef struct {
    // linked list - assert: first field
    btstack_linked_item_t    item;
    
    btstack_timer_source_t   timer;
    int              timer_active;
    
	RFCOMM_MULTIPLEXER_STATE state;	
    
    uint16_t  l2cap_cid;
    
    uint8_t   fcon; // only send if fcon & 1, send rsp if fcon & 0x80

	bd_addr_t remote_addr;
    hci_con_handle_t con_handle;
    
	uint8_t   outgoing;
    
    // hack to deal with authentication failure only observed by remote side
    uint8_t at_least_one_connection;
    
    uint16_t max_frame_size;
    
    // send DM for DLCI != 0
    uint8_t send_dm_for_dlci;
    
    // non supported command, 0 if not set
    uint8_t nsc_command;

    // test data - limited to RFCOMM_TEST_DATA_MAX_LEN
    uint8_t test_data_len;
    uint8_t test_data[RFCOMM_TEST_DATA_MAX_LEN];

} rfcomm_multiplexer_t;

// info regarding an actual connection
typedef struct {

    // linked list - assert: first field
    btstack_linked_item_t    item;
	
    // packet handler
    btstack_packet_handler_t packet_handler;

    // server channel (see rfcomm_service_t) - NULL => outgoing channel
    rfcomm_service_t * service;

	// muliplexer for this channel
    rfcomm_multiplexer_t *multiplexer;
	
    // RFCOMM Channel ID
    uint16_t rfcomm_cid;
        
    // 
    uint8_t  dlci; 
    
    // credits for outgoing traffic
    uint8_t credits_outgoing;
    
    // number of packets remote will be granted
    uint8_t new_credits_incoming;

    // credits for incoming traffic
    uint8_t credits_incoming;
    
    // use incoming flow control
    uint8_t incoming_flow_control;
    
    // channel state
    RFCOMM_CHANNEL_STATE state;
    
    // state variables used in RFCOMM_CHANNEL_INCOMING
    RFCOMM_CHANNEL_STATE_VAR state_var;
    
    // priority set by incoming side in PN
    uint8_t pn_priority;
    
	// negotiated frame size
    uint16_t max_frame_size;
	
    // local rpn data
    rfcomm_rpn_data_t rpn_data;
    
    // rls line status. RFCOMM_RLS_STATUS_INVALID if not set
    uint8_t rls_line_status;

    // msc modem status.
    uint8_t msc_modem_status;

    //
    uint8_t   waiting_for_can_send_now;
        
} rfcomm_channel_t;

/* API_START */

/** 
 * @brief Set up RFCOMM.
 */
void rfcomm_init(void);

/** 
 * @brief Set security level required for incoming connections, need to be called before registering services.
 */
void rfcomm_set_required_security_level(gap_security_level_t security_level);

/* 
 * @brief Create RFCOMM connection to a given server channel on a remote deivce.
 * This channel will automatically provide enough credits to the remote side.
 * @param addr
 * @param server_channel
 * @param out_cid
 * @result status
 */
uint8_t rfcomm_create_channel(btstack_packet_handler_t packet_handler, bd_addr_t addr, uint8_t server_channel, uint16_t * out_cid);

/* 
 * @brief Create RFCOMM connection to a given server channel on a remote deivce.
 * This channel will use explicit credit management. During channel establishment, an initial  amount of credits is provided.
 * @param addr
 * @param server_channel
 * @param initial_credits
 * @param out_cid
 * @result status
 */
uint8_t rfcomm_create_channel_with_initial_credits(btstack_packet_handler_t packet_handler, bd_addr_t addr, uint8_t server_channel, uint8_t initial_credits, uint16_t * out_cid);

/** 
 * @brief Disconnects RFCOMM channel with given identifier. 
 */
void rfcomm_disconnect(uint16_t rfcomm_cid);

/** 
 * @brief Registers RFCOMM service for a server channel and a maximum frame size, and assigns a packet handler.
 * This channel provides credits automatically to the remote side -> no flow control
 * @param packet handler for all channels of this service
 * @param channel 
 * @param max_frame_size
 */
uint8_t rfcomm_register_service(btstack_packet_handler_t packet_handler, uint8_t channel, uint16_t max_frame_size);

/** 
 * @brief Registers RFCOMM service for a server channel and a maximum frame size, and assigns a packet handler. 
 * This channel will use explicit credit management. During channel establishment, an initial amount of credits is provided.
 * @param packet handler for all channels of this service
 * @param channel 
 * @param max_frame_size
 * @param initial_credits
 */
uint8_t rfcomm_register_service_with_initial_credits(btstack_packet_handler_t packet_handler, uint8_t channel, uint16_t max_frame_size, uint8_t initial_credits);

/** 
 * @brief Unregister RFCOMM service.
 */
void rfcomm_unregister_service(uint8_t service_channel);

/** 
 * @brief Accepts incoming RFCOMM connection.
 */
void rfcomm_accept_connection(uint16_t rfcomm_cid);

/** 
 * @brief Deny incoming RFCOMM connection.
 */
void rfcomm_decline_connection(uint16_t rfcomm_cid);

/** 
 * @brief Grant more incoming credits to the remote side for the given RFCOMM channel identifier.
 */
void rfcomm_grant_credits(uint16_t rfcomm_cid, uint8_t credits);

/** 
 * @brief Checks if RFCOMM can send packet. Returns yes if packet can be sent.
 */
int rfcomm_can_send_packet_now(uint16_t rfcomm_cid);

/** 
 * @brief Sends RFCOMM data packet to the RFCOMM channel with given identifier.
 */
int  rfcomm_send(uint16_t rfcomm_cid, uint8_t *data, uint16_t len);

/** 
 * @brief Sends Local Line Status, see LINE_STATUS_..
 */
int rfcomm_send_local_line_status(uint16_t rfcomm_cid, uint8_t line_status);

/** 
 * @brief Send local modem status. see MODEM_STAUS_..
 */
int rfcomm_send_modem_status(uint16_t rfcomm_cid, uint8_t modem_status);

/** 
 * @brief Configure remote port 
 */
int rfcomm_send_port_configuration(uint16_t rfcomm_cid, rpn_baud_t baud_rate, rpn_data_bits_t data_bits, rpn_stop_bits_t stop_bits, rpn_parity_t parity, rpn_flow_control_t flow_control);

/** 
 * @brief Query remote port 
 */
int rfcomm_query_port_configuration(uint16_t rfcomm_cid);

/** 
 * @brief Query max frame size
 */
uint16_t  rfcomm_get_max_frame_size(uint16_t rfcomm_cid);

/** 
 * @brief Allow to create RFCOMM packet in outgoing buffer.
 * if (rfcomm_can_send_packet_now(cid)){
 *     rfcomm_reserve_packet_buffer();
 *     uint8_t * buffer = rfcomm_get_outgoing_buffer();
 *     uint16_t buffer_size = rfcomm_get_max_frame_size(cid);
 *     // .. setup data in buffer with len
 *     rfcomm_send_prepared(cid, len)
 * }
 */
int       rfcomm_reserve_packet_buffer(void);
uint8_t * rfcomm_get_outgoing_buffer(void);
int       rfcomm_send_prepared(uint16_t rfcomm_cid, uint16_t len);
void      rfcomm_release_packet_buffer(void);

/**
 * CRC8 functions using ETSI TS 101 369 V6.3.0.
 * Only used by RFCOMM
 */
uint8_t crc8_check(uint8_t *data, uint16_t len, uint8_t check_sum);
uint8_t crc8_calc(uint8_t *data, uint16_t len);

/* API_END */

#if defined __cplusplus
}
#endif

#endif // __RFCOMM_H
