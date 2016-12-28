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
 *  sdp_client.h
 */

#ifndef __SDP_CLIENT_H
#define __SDP_CLIENT_H

#include "btstack_config.h"

#include "btstack_util.h"

#if defined __cplusplus
extern "C" {
#endif

/* API_START */

typedef struct de_state {
    uint8_t  in_state_GET_DE_HEADER_LENGTH ;
    uint32_t addon_header_bytes;
    uint32_t de_size;
    uint32_t de_offset;
} de_state_t; 

void de_state_init(de_state_t * state);
int  de_state_size(uint8_t eventByte, de_state_t *de_state);

/** 
 * @brief Checks if the SDP Client is ready
 * @return 1 when no query is active
 */
int sdp_client_ready(void);

/** 
 * @brief Queries the SDP service of the remote device given a service search pattern and a list of attribute IDs. 
 * The remote data is handled by the SDP parser. The SDP parser delivers attribute values and done event via the callback.
 * @param callback for attributes values and done event
 * @param remote address
 * @param des_service_search_pattern 
 * @param des_attribute_id_list
 */
void sdp_client_query(btstack_packet_handler_t callback, bd_addr_t remote, const uint8_t * des_service_search_pattern, const uint8_t * des_attribute_id_list);

/*
 * @brief Searches SDP records on a remote device for all services with a given UUID.
 * @note calls sdp_client_query with service search pattern based on uuid16
 */
void sdp_client_query_uuid16(btstack_packet_handler_t callback, bd_addr_t remote, uint16_t uuid16);

/*
 * @brief Searches SDP records on a remote device for all services with a given UUID.
 * @note calls sdp_client_query with service search pattern based on uuid128
 */
void sdp_client_query_uuid128(btstack_packet_handler_t callback, bd_addr_t remote, const uint8_t* uuid128);


/** 
 * @brief Retrieves all attribute IDs of a SDP record specified by its service record handle and a list of attribute IDs. 
 * The remote data is handled by the SDP parser. The SDP parser delivers attribute values and done event via the callback.
 * @note only provided if ENABLE_SDP_EXTRA_QUERIES is defined
 * @param callback for attributes values and done event 
 * @param remote address
 * @param search_service_record_handle 
 * @param des_attributeIDList
 */
void sdp_client_service_attribute_search(btstack_packet_handler_t callback, bd_addr_t remote, uint32_t search_service_record_handle, const uint8_t * des_attributeIDList);

/** 
 * @brief Query the list of SDP records that match a given service search pattern. 
 * The remote data is handled by the SDP parser. The SDP parser delivers attribute values and done event via the callback.
 * @note only provided if ENABLE_SDP_EXTRA_QUERIES is defined
 * @param callback for attributes values and done event
 * @param remote address
 * @param des_service_search_pattern 
 */
void sdp_client_service_search(btstack_packet_handler_t callback, bd_addr_t remote, const uint8_t * des_service_search_pattern);


/* API_END */

#if defined __cplusplus
}
#endif

#endif // __SDP_CLIENT_H
