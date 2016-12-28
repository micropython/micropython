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
 *  hci_cmd.h
 *
 *  Created by Matthias Ringwald on 7/23/09.
 */

#ifndef __HCI_CMDS_H
#define __HCI_CMDS_H

#include "bluetooth.h"
#include "btstack_defines.h"

#include <stdint.h>
#include <stdarg.h>

#if defined __cplusplus
extern "C" {
#endif

/**
 *  Hardware state of Bluetooth controller 
 */
typedef enum {
    HCI_POWER_OFF = 0,
    HCI_POWER_ON,
    HCI_POWER_SLEEP
} HCI_POWER_MODE;

/**
 * State of BTstack 
 */
typedef enum {
    HCI_STATE_OFF = 0,
    HCI_STATE_INITIALIZING,
    HCI_STATE_WORKING,
    HCI_STATE_HALTING,
    HCI_STATE_SLEEPING,
    HCI_STATE_FALLING_ASLEEP
} HCI_STATE;

/** 
 * compact HCI Command packet description
 */
 typedef struct {
    uint16_t    opcode;
    const char *format;
} hci_cmd_t;


// HCI Commands - see hci_cmd.c for info on parameters
extern const hci_cmd_t hci_accept_connection_request;
extern const hci_cmd_t hci_accept_synchronous_connection;
extern const hci_cmd_t hci_authentication_requested;
extern const hci_cmd_t hci_change_connection_link_key;
extern const hci_cmd_t hci_change_connection_packet_type;
extern const hci_cmd_t hci_create_connection;
extern const hci_cmd_t hci_create_connection_cancel;
extern const hci_cmd_t hci_delete_stored_link_key;
extern const hci_cmd_t hci_enhanced_setup_synchronous_connection;
extern const hci_cmd_t hci_enhanced_accept_synchronous_connection;
extern const hci_cmd_t hci_disconnect;
extern const hci_cmd_t hci_host_buffer_size;
extern const hci_cmd_t hci_inquiry;
extern const hci_cmd_t hci_io_capability_request_reply;
extern const hci_cmd_t hci_io_capability_request_negative_reply;
extern const hci_cmd_t hci_inquiry_cancel;
extern const hci_cmd_t hci_link_key_request_negative_reply;
extern const hci_cmd_t hci_link_key_request_reply;
extern const hci_cmd_t hci_pin_code_request_reply;
extern const hci_cmd_t hci_pin_code_request_negative_reply;
extern const hci_cmd_t hci_qos_setup;
extern const hci_cmd_t hci_read_bd_addr;
extern const hci_cmd_t hci_read_buffer_size;
extern const hci_cmd_t hci_read_le_host_supported;
extern const hci_cmd_t hci_read_link_policy_settings;
extern const hci_cmd_t hci_read_link_supervision_timeout;
extern const hci_cmd_t hci_read_local_version_information;
extern const hci_cmd_t hci_read_local_supported_commands;
extern const hci_cmd_t hci_read_local_supported_features;
extern const hci_cmd_t hci_read_num_broadcast_retransmissions;
extern const hci_cmd_t hci_read_remote_supported_features_command;
extern const hci_cmd_t hci_read_rssi;
extern const hci_cmd_t hci_reject_connection_request;
extern const hci_cmd_t hci_remote_name_request;
extern const hci_cmd_t hci_remote_name_request_cancel;
extern const hci_cmd_t hci_remote_oob_data_request_negative_reply;
extern const hci_cmd_t hci_reset;
extern const hci_cmd_t hci_role_discovery;
extern const hci_cmd_t hci_set_event_mask;
extern const hci_cmd_t hci_set_connection_encryption;
extern const hci_cmd_t hci_setup_synchronous_connection;
extern const hci_cmd_t hci_sniff_mode;
extern const hci_cmd_t hci_switch_role_command;
extern const hci_cmd_t hci_user_confirmation_request_negative_reply;
extern const hci_cmd_t hci_user_confirmation_request_reply;
extern const hci_cmd_t hci_user_passkey_request_negative_reply;
extern const hci_cmd_t hci_user_passkey_request_reply;
extern const hci_cmd_t hci_write_authentication_enable;
extern const hci_cmd_t hci_write_class_of_device;
extern const hci_cmd_t hci_write_extended_inquiry_response;
extern const hci_cmd_t hci_write_inquiry_mode;
extern const hci_cmd_t hci_write_le_host_supported;
extern const hci_cmd_t hci_write_link_policy_settings;
extern const hci_cmd_t hci_write_link_supervision_timeout;
extern const hci_cmd_t hci_write_local_name;
extern const hci_cmd_t hci_write_num_broadcast_retransmissions;
extern const hci_cmd_t hci_write_page_timeout;
extern const hci_cmd_t hci_write_scan_enable;
extern const hci_cmd_t hci_write_simple_pairing_mode;
extern const hci_cmd_t hci_write_synchronous_flow_control_enable;
extern const hci_cmd_t hci_read_loopback_mode;
extern const hci_cmd_t hci_write_loopback_mode;

extern const hci_cmd_t hci_le_add_device_to_white_list;
extern const hci_cmd_t hci_le_clear_white_list;
extern const hci_cmd_t hci_le_connection_update;
extern const hci_cmd_t hci_le_create_connection;
extern const hci_cmd_t hci_le_create_connection_cancel;
extern const hci_cmd_t hci_le_encrypt;
extern const hci_cmd_t hci_le_long_term_key_negative_reply;
extern const hci_cmd_t hci_le_long_term_key_request_reply;
extern const hci_cmd_t hci_le_rand;
extern const hci_cmd_t hci_le_read_advertising_channel_tx_power;
extern const hci_cmd_t hci_le_read_buffer_size ;
extern const hci_cmd_t hci_le_read_channel_map;
extern const hci_cmd_t hci_le_read_remote_used_features;
extern const hci_cmd_t hci_le_read_supported_features;
extern const hci_cmd_t hci_le_read_supported_states;
extern const hci_cmd_t hci_le_read_white_list_size;
extern const hci_cmd_t hci_le_receiver_test;
extern const hci_cmd_t hci_le_remove_device_from_white_list;
extern const hci_cmd_t hci_le_set_advertise_enable;
extern const hci_cmd_t hci_le_set_advertising_data;
extern const hci_cmd_t hci_le_set_advertising_parameters;
extern const hci_cmd_t hci_le_set_event_mask;
extern const hci_cmd_t hci_le_set_host_channel_classification;
extern const hci_cmd_t hci_le_set_random_address;
extern const hci_cmd_t hci_le_set_scan_enable;
extern const hci_cmd_t hci_le_set_scan_parameters;
extern const hci_cmd_t hci_le_set_scan_response_data;
extern const hci_cmd_t hci_le_start_encryption;
extern const hci_cmd_t hci_le_test_end;
extern const hci_cmd_t hci_le_transmitter_test;


/**
 * construct HCI Command based on template
 *
 * Format:
 *   1,2,3,4: one to four byte value
 *   H: HCI connection handle
 *   B: Bluetooth Baseband Address (BD_ADDR)
 *   D: 8 byte data block
 *   E: Extended Inquiry Result
 *   N: Name up to 248 chars, \0 terminated
 *   P: 16 byte Pairing code
 *   A: 31 bytes advertising data
 *   S: Service Record (Data Element Sequence)
 */
 uint16_t hci_cmd_create_from_template(uint8_t *hci_cmd_buffer, const hci_cmd_t *cmd, va_list argptr);

    
#if defined __cplusplus
}
#endif

#endif // __HCI_CMDS_H
