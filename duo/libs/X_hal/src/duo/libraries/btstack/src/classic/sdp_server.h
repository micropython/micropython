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
#ifndef __SDP_H
#define __SDP_H

#include <stdint.h>
#include "btstack_linked_list.h"

#include "btstack_config.h"

#if defined __cplusplus
extern "C" {
#endif
    
typedef struct {
    // linked list - assert: first field
    btstack_linked_item_t   item;

    uint32_t        service_record_handle;
    uint8_t *       service_record;
} service_record_item_t;

int sdp_handle_service_search_request(uint8_t * packet, uint16_t remote_mtu);
int sdp_handle_service_attribute_request(uint8_t * packet, uint16_t remote_mtu);
int sdp_handle_service_search_attribute_request(uint8_t * packet, uint16_t remote_mtu);

/* API_START */

/** 
 * @brief Set up SDP.
 */
void sdp_init(void);

/**
 * @brief Register Service Record with database using ServiceRecordHandle stored in record
 * @pre AttributeIDs are in ascending order
 * @pre ServiceRecordHandle is first attribute and valid
 * @param record is not copied!
 * @result status
 */
uint8_t sdp_register_service(const uint8_t * record);

/** 
 * @brief Unregister service record internally.
 */
void sdp_unregister_service(uint32_t service_record_handle);

/* API_END */

// used by daemon

/**
 * @brief Finds an unused valid service record handle
 * @result handle
 */
uint32_t sdp_create_service_record_handle(void);

/**
 * @brief gets record for handle
 * @result record
 */

uint8_t * sdp_get_record_for_handle(uint32_t handle);

/**
 * @brief gets service record handle from record
 * @resutl service record handle or 0
 */
uint32_t sdp_get_service_record_handle(const uint8_t * record);

#if defined __cplusplus
}
#endif
#endif // __SDP_H
