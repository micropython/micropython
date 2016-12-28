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
 *  pan.h
 *
 *  Created by Milanka Ringwald on 10/16/14.
 */

#ifndef __PAN_H
#define __PAN_H

#include <stdint.h>
#include "bluetooth.h"
 
#if defined __cplusplus
extern "C" {
#endif

/* API_START */

/** 
 * @brief Creates SDP record for PANU BNEP service in provided empty buffer.
 * @note Make sure the buffer is big enough.
 *
 * @param service is an empty buffer to store service record
 * @param service_record_handle for new service
 * @param network_packet_types array of types terminated by a 0x0000 entry
 * @param name if NULL, the default service name will be assigned
 * @param description if NULL, the default service description will be assigned
 * @param security_desc 
 */
void pan_create_panu_service(uint8_t *service, uint32_t service_record_handle, uint16_t * network_packet_types, const char *name,
	const char *description, security_description_t security_desc);

/** 
 * @brief Creates SDP record for GN BNEP service in provided empty buffer.
 * @note Make sure the buffer is big enough.
 *
 * @param service is an empty buffer to store service record
 * @param service_record_handle for new service
 * @param network_packet_types array of types terminated by a 0x0000 entry
 * @param name if NULL, the default service name will be assigned
 * @param description if NULL, the default service description will be assigned
 * @param security_desc 
 * @param IPv4Subnet is optional subnet definition, e.g. "10.0.0.0/8"
 * @param IPv6Subnet is optional subnet definition given in the standard IETF format with the absolute attribute IDs
 */
void pan_create_gn_service(uint8_t *service, uint32_t service_record_handle, uint16_t * network_packet_types, const char *name,
	const char *description, security_description_t security_desc, const char *IPv4Subnet,
	const char *IPv6Subnet);

/** 
 * @brief Creates SDP record for NAP BNEP service in provided empty buffer.
 * @note Make sure the buffer is big enough.
 *
 * @param service is an empty buffer to store service record
 * @param service_record_handle for new service
 * @param name if NULL, the default service name will be assigned
 * @param network_packet_types array of types terminated by a 0x0000 entry
 * @param description if NULL, the default service description will be assigned
 * @param security_desc 
 * @param net_access_type type of available network access
 * @param max_net_access_rate based on net_access_type measured in byte/s
 * @param IPv4Subnet is optional subnet definition, e.g. "10.0.0.0/8"
 * @param IPv6Subnet is optional subnet definition given in the standard IETF format with the absolute attribute IDs
 */
void pan_create_nap_service(uint8_t *service, uint32_t service_record_handle, uint16_t * network_packet_types, const char *name,
	const char *description, security_description_t security_desc, net_access_type_t net_access_type,
	uint32_t max_net_access_rate, const char *IPv4Subnet, const char *IPv6Subnet);

/* API_END */

#if defined __cplusplus
}
#endif
#endif // __PAN_H
