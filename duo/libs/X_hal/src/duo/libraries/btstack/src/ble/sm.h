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

#ifndef __SM_H
#define __SM_H

 #if defined __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "btstack_util.h"
#include "btstack_defines.h"
#include "hci.h"

typedef struct {
    btstack_linked_item_t  item;
    bd_addr_t      address;
    bd_addr_type_t address_type;
} sm_lookup_entry_t;

/* API_START */

/**
 * @brief Initializes the Security Manager, connects to L2CAP
 */
void sm_init(void);

/**
 * @brief Set secret ER key for key generation as described in Core V4.0, Vol 3, Part G, 5.2.2 
 * @param er
 */
void sm_set_er(sm_key_t er);

/**
 * @brief Set secret IR key for key generation as described in Core V4.0, Vol 3, Part G, 5.2.2 
 */
void sm_set_ir(sm_key_t ir);

/**
 *
 * @brief Registers OOB Data Callback. The callback should set the oob_data and return 1 if OOB data is availble
 * @param get_oob_data_callback
 */
void sm_register_oob_data_callback( int (*get_oob_data_callback)(uint8_t addres_type, bd_addr_t addr, uint8_t * oob_data));

/**
 * @brief Add event packet handler. 
 */
void sm_add_event_handler(btstack_packet_callback_registration_t * callback_handler);

/**
 * @brief Limit the STK generation methods. Bonding is stopped if the resulting one isn't in the list
 * @param OR combination of SM_STK_GENERATION_METHOD_ 
 */
void sm_set_accepted_stk_generation_methods(uint8_t accepted_stk_generation_methods);

/**
 * @brief Set the accepted encryption key size range. Bonding is stopped if the result isn't within the range
 * @param min_size (default 7)
 * @param max_size (default 16)
 */
void sm_set_encryption_key_size_range(uint8_t min_size, uint8_t max_size);

/**
 * @brief Sets the requested authentication requirements, bonding yes/no, MITM yes/no
 * @param OR combination of SM_AUTHREQ_ flags
 */
void sm_set_authentication_requirements(uint8_t auth_req);

/**
 * @brief Sets the available IO Capabilities
 * @param IO_CAPABILITY_
 */
void sm_set_io_capabilities(io_capability_t io_capability);

/**
 * @brief Let Peripheral request an encrypted connection right after connecting
 * @note Not used normally. Bonding is triggered by access to protected attributes in ATT Server
 */
void sm_set_request_security(int enable);

/** 
 * @brief Trigger Security Request
 * @note Not used normally. Bonding is triggered by access to protected attributes in ATT Server
 */
void sm_send_security_request(hci_con_handle_t con_handle);

/**
 * @brief Decline bonding triggered by event before
 * @param addr_type and address
 */
void sm_bonding_decline(hci_con_handle_t con_handle);

/**
 * @brief Confirm Just Works bonding 
 * @param addr_type and address
 */
void sm_just_works_confirm(hci_con_handle_t con_handle);

/**
 * @brief Reports passkey input by user
 * @param addr_type and address
 * @param passkey in [0..999999]
 */
void sm_passkey_input(hci_con_handle_t con_handle, uint32_t passkey);

/**
 *
 * @brief Get encryption key size.
 * @param addr_type and address
 * @return 0 if not encrypted, 7-16 otherwise
 */
int sm_encryption_key_size(hci_con_handle_t con_handle);

/**
 * @brief Get authentication property.
 * @param addr_type and address
 * @return 1 if bonded with OOB/Passkey (AND MITM protection)
 */
int sm_authenticated(hci_con_handle_t con_handle);

/**
 * @brief Queries authorization state.
 * @param addr_type and address
 * @return authorization_state for the current session
 */
authorization_state_t sm_authorization_state(hci_con_handle_t con_handle);

/**
 * @brief Used by att_server.c to request user authorization.
 * @param addr_type and address
 */
void sm_request_pairing(hci_con_handle_t con_handle);

/**
 * @brief Report user authorization decline.
 * @param addr_type and address
 */
void sm_authorization_decline(hci_con_handle_t con_handle);

/**
 * @brief Report user authorization grant.
 * @param addr_type and address
 */
void sm_authorization_grant(hci_con_handle_t con_handle);

/**
 * @brief Support for signed writes, used by att_server.
 * @note Message is in little endian to allows passing in ATT PDU without flipping. 
 * @note calculated hash in done_callback is big endian
 */
int  sm_cmac_ready(void);
void sm_cmac_start(sm_key_t k, uint8_t opcode, uint16_t attribute_handle, uint16_t message_len, uint8_t * message, uint32_t sign_counter, void (*done_handler)(uint8_t hash[8]));

/*
 * @brief Match address against bonded devices
 * @return 0 if successfully added to lookup queue
 * @note Triggers SM_IDENTITY_RESOLVING_* events
 */
int sm_address_resolution_lookup(uint8_t addr_type, bd_addr_t addr);

/**
 * @brief Identify device in LE Device DB.
 * @param handle
 * @return index from le_device_db or -1 if not found/identified
 */
int sm_le_device_index(hci_con_handle_t con_handle );
/* API_END */

// PTS testing
void sm_test_set_irk(sm_key_t irk);
void sm_test_use_fixed_local_csrk(void);

#if defined __cplusplus
}
#endif

#endif // __SM_H
