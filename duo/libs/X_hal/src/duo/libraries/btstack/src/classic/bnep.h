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
 * bnep.h
 * Author: Ole Reinhardt <ole.reinhardt@kernelconcepts.de>
 *
 */

#ifndef __BNEP_H
#define __BNEP_H
 
#include "btstack_util.h"
#include "btstack_run_loop.h"
#include "gap.h"
 
#include <stdint.h>

#if defined __cplusplus
extern "C" {
#endif

#define MAX_BNEP_NETFILTER                              8
#define MAX_BNEP_MULTICAST_FILTER                       8
#define MAX_BNEP_NETFILTER_OUT                          421
#define MAX_BNEP_MULTICAST_FILTER_OUT                   140

typedef enum {
	BNEP_CHANNEL_STATE_CLOSED = 1,
    BNEP_CHANNEL_STATE_WAIT_FOR_CONNECTION_REQUEST,
    BNEP_CHANNEL_STATE_WAIT_FOR_CONNECTION_RESPONSE,
	BNEP_CHANNEL_STATE_CONNECTED,
} BNEP_CHANNEL_STATE;

typedef enum {
    BNEP_CHANNEL_STATE_VAR_NONE                            = 0,
    BNEP_CHANNEL_STATE_VAR_SND_NOT_UNDERSTOOD              = 1 << 0,
    BNEP_CHANNEL_STATE_VAR_SND_CONNECTION_REQUEST          = 1 << 1,
    BNEP_CHANNEL_STATE_VAR_SND_CONNECTION_RESPONSE         = 1 << 2,
    BNEP_CHANNEL_STATE_VAR_SND_FILTER_NET_TYPE_SET         = 1 << 3,
    BNEP_CHANNEL_STATE_VAR_SND_FILTER_NET_TYPE_RESPONSE    = 1 << 4,
    BNEP_CHANNEL_STATE_VAR_SND_FILTER_MULTI_ADDR_SET       = 1 << 5,
    BNEP_CHANNEL_STATE_VAR_SND_FILTER_MULTI_ADDR_RESPONSE  = 1 << 6,
} BNEP_CHANNEL_STATE_VAR;

typedef enum {
    BNEP_CH_EVT_READY_TO_SEND,
} BNEP_CHANNEL_EVENT;

typedef struct bnep_channel_event {
    BNEP_CHANNEL_EVENT type;
} bnep_channel_event_t;

/* network protocol type filter */
typedef struct {
	uint16_t	        range_start;
	uint16_t	        range_end;
} bnep_net_filter_t;

/* multicast address filter */
typedef struct {
	uint8_t		        addr_start[ETHER_ADDR_LEN];
	uint8_t		        addr_end[ETHER_ADDR_LEN];
} bnep_multi_filter_t;


// info regarding multiplexer
// note: spec mandates single multplexer per device combination
typedef struct {
    // linked list - assert: first field
    btstack_linked_item_t      item;

    BNEP_CHANNEL_STATE state;	          // Channel state

    BNEP_CHANNEL_STATE_VAR state_var;     // State flag variable. Needed for asynchronous packet sending

    uint16_t           max_frame_size;    // incomming max. frame size   
    void              *connection;        // client connection
    bd_addr_t          local_addr;        // locale drvice address
	bd_addr_t          remote_addr;       // remote device address
    uint16_t           l2cap_cid;         // l2cap channel id
    hci_con_handle_t   con_handle;        // hci connection handle

    uint16_t           uuid_source;       // Source UUID
    uint16_t           uuid_dest;         // Destination UUID

    uint8_t            last_control_type; // type of last control package
    uint16_t           response_code;     // response code of last action (temp. storage for state machine)

    bnep_net_filter_t  net_filter[MAX_BNEP_NETFILTER];              // network protocol filter, define fixed size for now
    uint16_t           net_filter_count;

    bnep_net_filter_t *net_filter_out;                              // outgoint network protocol filter, must be statically allocated in the application
    uint16_t           net_filter_out_count;
    
    bnep_multi_filter_t  multicast_filter[MAX_BNEP_MULTICAST_FILTER]; // multicast address filter, define fixed size for now
    uint16_t             multicast_filter_count;
    
    bnep_multi_filter_t *multicast_filter_out;                        // outgoing multicast address filter, must be statically allocated in the application
    uint16_t             multicast_filter_out_count;


    btstack_timer_source_t     timer;             // Timeout timer
    int                timer_active;      // Is a timer running?
    int                retry_count;       // number of retries for CONTROL SETUP MSG
    // l2cap packet handler
    btstack_packet_handler_t packet_handler;
} bnep_channel_t;

/* Internal BNEP service descriptor */
typedef struct {
    btstack_linked_item_t    item;           // linked list - assert: first field
    uint16_t         service_uuid;   // Service class: PANU, NAP, GN
    uint16_t         max_frame_size; // incomming max. frame size
    
    // internal connection
    btstack_packet_handler_t packet_handler;
} bnep_service_t;


void bnep_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);

/* API_START */

/**
 * @brief Set up BNEP.
 */
void bnep_init(void);

/**
 * @brief Check if a data packet can be send out.
 */
int bnep_can_send_packet_now(uint16_t bnep_cid);

/**
 * @brief Send a data packet.
 */
int bnep_send(uint16_t bnep_cid, uint8_t *packet, uint16_t len);

/**
 * @brief Set the network protocol filter.
 */
int bnep_set_net_type_filter(uint16_t bnep_cid, bnep_net_filter_t *filter, uint16_t len);

/**
 * @brief Set the multicast address filter.
 */
int bnep_set_multicast_filter(uint16_t bnep_cid, bnep_multi_filter_t *filter, uint16_t len);

/**
 * @brief Set security level required for incoming connections, need to be called before registering services.
 */
void bnep_set_required_security_level(gap_security_level_t security_level);

/**
 * @brief Register packet handler. 
 */
void bnep_register_packet_handler(void (*handler)(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size));

/**
 * @brief Creates BNEP connection (channel) to a given server on a remote device with baseband address. A new baseband connection will be initiated if necessary. 
 */
int bnep_connect(bd_addr_t addr, uint16_t l2cap_psm, uint16_t uuid_src, uint16_t uuid_dest);

/**
 * @brief Disconnects BNEP channel with given identifier. 
 */
void bnep_disconnect(bd_addr_t addr);

/**
 * @brief Registers BNEP service, set a maximum frame size and assigns a packet handler. On embedded systems, use NULL for connection parameter. 
 */
uint8_t bnep_register_service(uint16_t service_uuid, uint16_t max_frame_size);

/**
 * @brief Unregister BNEP service.
 */
void bnep_unregister_service(uint16_t service_uuid);
/* API_END */

#if defined __cplusplus
}
#endif

#endif // __BNEP_H
