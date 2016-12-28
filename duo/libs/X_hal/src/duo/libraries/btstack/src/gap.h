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

#ifndef __GAP_H
#define __GAP_H

#if defined __cplusplus
extern "C" {
#endif

#include "btstack_defines.h"
#include "btstack_util.h"
	
typedef enum {

	// MITM protection not required
	// No encryption required
	// No user interaction required
	LEVEL_0 = 0,

	// MITM protection not required
	// No encryption required
	// Minimal user interaction desired
	LEVEL_1,

	// MITM protection not required
	// Encryption required
	LEVEL_2,

	// MITM protection required
	// Encryption required
	// User interaction acceptable
	LEVEL_3,

	// MITM protection required
	// Encryption required
	// 128-bit equivalent strength for link and encryption keys required (P-192 is not enough)
	// User interaction acceptable
	LEVEL_4,	
} gap_security_level_t;

typedef enum {
	GAP_SECURITY_NONE,
	GAP_SECUIRTY_ENCRYPTED,		// SSP: JUST WORKS
	GAP_SECURITY_AUTHENTICATED, // SSP: numeric comparison, passkey, OOB 
	// GAP_SECURITY_AUTHORIZED
} gap_security_state;

typedef enum {
	GAP_CONNECTION_INVALID,
	GAP_CONNECTION_ACL,
	GAP_CONNECTION_SCO,
	GAP_CONNECTION_LE
} gap_connection_type_t;

typedef struct le_connection_parameter_range{
    uint16_t le_conn_interval_min;
    uint16_t le_conn_interval_max;
    uint16_t le_conn_latency_min;
    uint16_t le_conn_latency_max;
    uint16_t le_supervision_timeout_min;
    uint16_t le_supervision_timeout_max;
} le_connection_parameter_range_t;

typedef enum {
    GAP_RANDOM_ADDRESS_TYPE_OFF = 0,
    GAP_RANDOM_ADDRESS_NON_RESOLVABLE,
    GAP_RANDOM_ADDRESS_RESOLVABLE,
} gap_random_address_type_t;

/* API_START */

// Classic + LE

/**
 * @brief Disconnect connection with handle
 * @param handle
 */
uint8_t gap_disconnect(hci_con_handle_t handle);

/**
 * @brief Get connection type
 * @param con_handle
 * @result connection_type
 */
gap_connection_type_t gap_get_connection_type(hci_con_handle_t connection_handle);

// Classic

/** 
 * @brief Sets local name.
 * @note has to be done before stack starts up
 * @param name is not copied, make sure memory is accessible during stack startup
 */
void gap_set_local_name(const char * local_name);

/**
 * @brief Enable/disable bonding. Default is enabled.
 * @param enabled
 */
void gap_set_bondable_mode(int enabled);

/**  
 * @brief Get bondable mode.
 * @return 1 if bondable
 */
int gap_get_bondable_mode(void);

/* Configure Secure Simple Pairing */

/**
 * @brief Enable will enable SSP during init.
 */
void gap_ssp_set_enable(int enable);

/**
 * @brief Set IO Capability. BTstack will return capability to SSP requests
 */
void gap_ssp_set_io_capability(int ssp_io_capability);

/**
 * @brief Set Authentication Requirements using during SSP
 */
void gap_ssp_set_authentication_requirement(int authentication_requirement);

/**
 * @brief If set, BTstack will confirm a numeric comparison and enter '000000' if requested.
 */
void gap_ssp_set_auto_accept(int auto_accept);

/**
 * @brief Start dedicated bonding with device. Disconnect after bonding.
 * @param device
 * @param request MITM protection
 * @return error, if max num acl connections active
 * @result GAP_DEDICATED_BONDING_COMPLETE
 */
int gap_dedicated_bonding(bd_addr_t device, int mitm_protection_required);

gap_security_level_t gap_security_level_for_link_key_type(link_key_type_t link_key_type);
gap_security_level_t gap_security_level(hci_con_handle_t con_handle);

void gap_request_security_level(hci_con_handle_t con_handle, gap_security_level_t level);

int  gap_mitm_protection_required_for_security_level(gap_security_level_t level);

// LE

/**
 * @brief Set parameters for LE Scan
 */
void gap_set_scan_parameters(uint8_t scan_type, uint16_t scan_interval, uint16_t scan_window);

/**
 * @brief Start LE Scan 
 */
void gap_start_scan(void);

/**
 * @brief Stop LE Scan
 */
void gap_stop_scan(void);

/**
 * @brief Enable privacy by using random addresses
 * @param random_address_type to use (incl. OFF)
 */
void gap_random_address_set_mode(gap_random_address_type_t random_address_type);

/**
 * @brief Get privacy mode
 */
gap_random_address_type_t gap_random_address_get_mode(void);

/**
 * @brief Sets update period for random address
 * @param period_ms in ms
 */
 void gap_random_address_set_update_period(int period_ms);

/** 
 * @brief Sets a fixed random address for advertising
 * @param addr
 * @note Sets random address mode to type off
 */
void gap_random_address_set(bd_addr_t addr);

/**
 * @brief Set Advertisement Data
 * @param advertising_data_length
 * @param advertising_data (max 31 octets)
 * @note data is not copied, pointer has to stay valid
 */
void gap_advertisements_set_data(uint8_t advertising_data_length, uint8_t * advertising_data);

/**
 * @brief Set Advertisement Paramters
 * @param adv_int_min
 * @param adv_int_max
 * @param adv_type
 * @param direct_address_type
 * @param direct_address
 * @param channel_map
 * @param filter_policy
 * @note own_address_type is used from gap_random_address_set_mode
 */
void gap_advertisements_set_params(uint16_t adv_int_min, uint16_t adv_int_max, uint8_t adv_type,
	uint8_t direct_address_typ, bd_addr_t direct_address, uint8_t channel_map, uint8_t filter_policy);

/** 
 * @brief Enable/Disable Advertisements
 * @param enabled
 */
void gap_advertisements_enable(int enabled);
/**
 * @brief Request an update of the connection parameter for a given LE connection
 * @param handle
 * @param conn_interval_min (unit: 1.25ms)
 * @param conn_interval_max (unit: 1.25ms)
 * @param conn_latency
 * @param supervision_timeout (unit: 10ms)
 * @returns 0 if ok
 */
int gap_request_connection_parameter_update(hci_con_handle_t con_handle, uint16_t conn_interval_min,
	uint16_t conn_interval_max, uint16_t conn_latency, uint16_t supervision_timeout);

/**
 * @brief Updates the connection parameters for a given LE connection
 * @param handle
 * @param conn_interval_min (unit: 1.25ms)
 * @param conn_interval_max (unit: 1.25ms)
 * @param conn_latency
 * @param supervision_timeout (unit: 10ms)
 * @returns 0 if ok
 */
int gap_update_connection_parameters(hci_con_handle_t con_handle, uint16_t conn_interval_min,
	uint16_t conn_interval_max, uint16_t conn_latency, uint16_t supervision_timeout);

/**
 * @brief Set accepted connection parameter range
 * @param range
 */
void gap_get_connection_parameter_range(le_connection_parameter_range_t range);

/**
 * @brief Get accepted connection parameter range
 * @param range
 */
void gap_set_connection_parameter_range(le_connection_parameter_range_t range);

/**
 * @brief Connect to remote LE device
 */
uint8_t gap_connect(bd_addr_t addr, bd_addr_type_t addr_type);

/**
 * @brief Cancel connection process initiated by gap_connect
 */
uint8_t gap_connect_cancel(void);

/**
 * @brief Auto Connection Establishment - Start Connecting to device
 * @param address_typ
 * @param address
 * @returns 0 if ok
 */
int gap_auto_connection_start(bd_addr_type_t address_typ, bd_addr_t address);

/**
 * @brief Auto Connection Establishment - Stop Connecting to device
 * @param address_typ
 * @param address
 * @returns 0 if ok
 */
int gap_auto_connection_stop(bd_addr_type_t address_typ, bd_addr_t address);

/**
 * @brief Auto Connection Establishment - Stop everything
 * @note  Convenience function to stop all active auto connection attempts
 */
void gap_auto_connection_stop_all(void);

// Classic

/**
 * @brief Override page scan mode. Page scan mode enabled by l2cap when services are registered
 * @note Might be used to reduce power consumption while Bluetooth module stays powered but no (new)
 *       connections are expected
 */
void gap_connectable_control(uint8_t enable);

/**
 * @brief Allows to control if device is discoverable. OFF by default.
 */
void gap_discoverable_control(uint8_t enable);

/**
 * @brief Gets local address.
 */
void gap_local_bd_addr(bd_addr_t address_buffer);

/**
 * @brief Deletes link key for remote device with baseband address.
 */
void gap_drop_link_key_for_bd_addr(bd_addr_t addr);

// LE

/**
 * @brief Get addr type and address used in advertisement packets.
 */
void gap_advertisements_get_address(uint8_t * addr_type, bd_addr_t addr);


/* API_END*/

#if defined __cplusplus
}
#endif

#endif // __GAP_H
