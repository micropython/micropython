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
 *  btstack_memory.h
 *
 *  @brief BTstack memory management via configurable memory pools
 *
 */

#ifndef __BTSTACK_MEMORY_H
#define __BTSTACK_MEMORY_H

#if defined __cplusplus
extern "C" {
#endif

#include "btstack_config.h"
    
// Core
#include "hci.h"
#include "l2cap.h"

// Classic
#include "classic/bnep.h"
#include "classic/hfp.h"
#include "classic/btstack_link_key_db.h"
#include "classic/btstack_link_key_db_memory.h"
#include "classic/rfcomm.h"
#include "classic/sdp_server.h"

// BLE
#ifdef ENABLE_BLE
#include "ble/gatt_client.h"
#include "ble/sm.h"
#endif

/* API_START */

/**
 * @brief Initializes BTstack memory pools.
 */
void btstack_memory_init(void);

/* API_END */

// hci_connection
hci_connection_t * btstack_memory_hci_connection_get(void);
void   btstack_memory_hci_connection_free(hci_connection_t *hci_connection);

// l2cap_service, l2cap_channel
l2cap_service_t * btstack_memory_l2cap_service_get(void);
void   btstack_memory_l2cap_service_free(l2cap_service_t *l2cap_service);
l2cap_channel_t * btstack_memory_l2cap_channel_get(void);
void   btstack_memory_l2cap_channel_free(l2cap_channel_t *l2cap_channel);

// rfcomm_multiplexer, rfcomm_service, rfcomm_channel
rfcomm_multiplexer_t * btstack_memory_rfcomm_multiplexer_get(void);
void   btstack_memory_rfcomm_multiplexer_free(rfcomm_multiplexer_t *rfcomm_multiplexer);
rfcomm_service_t * btstack_memory_rfcomm_service_get(void);
void   btstack_memory_rfcomm_service_free(rfcomm_service_t *rfcomm_service);
rfcomm_channel_t * btstack_memory_rfcomm_channel_get(void);
void   btstack_memory_rfcomm_channel_free(rfcomm_channel_t *rfcomm_channel);

// btstack_link_key_db_memory
btstack_link_key_db_memory_t * btstack_memory_btstack_link_key_db_memory_get(void);
void   btstack_memory_btstack_link_key_db_memory_free(btstack_link_key_db_memory_t *btstack_link_key_db_memory);

// bnep_service, bnep_channel
bnep_service_t * btstack_memory_bnep_service_get(void);
void   btstack_memory_bnep_service_free(bnep_service_t *bnep_service);
bnep_channel_t * btstack_memory_bnep_channel_get(void);
void   btstack_memory_bnep_channel_free(bnep_channel_t *bnep_channel);

// hfp_connection
hfp_connection_t * btstack_memory_hfp_connection_get(void);
void   btstack_memory_hfp_connection_free(hfp_connection_t *hfp_connection);

// service_record_item
service_record_item_t * btstack_memory_service_record_item_get(void);
void   btstack_memory_service_record_item_free(service_record_item_t *service_record_item);

#ifdef ENABLE_BLE
// gatt_client, whitelist_entry, sm_lookup_entry
gatt_client_t * btstack_memory_gatt_client_get(void);
void   btstack_memory_gatt_client_free(gatt_client_t *gatt_client);
whitelist_entry_t * btstack_memory_whitelist_entry_get(void);
void   btstack_memory_whitelist_entry_free(whitelist_entry_t *whitelist_entry);
sm_lookup_entry_t * btstack_memory_sm_lookup_entry_get(void);
void   btstack_memory_sm_lookup_entry_free(sm_lookup_entry_t *sm_lookup_entry);
#endif

#if defined __cplusplus
}
#endif

#endif // __BTSTACK_MEMORY_H

