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

// *****************************************************************************
//
// HSP Audio Gateway
//
// *****************************************************************************


#ifndef btstack_hsp_ag_h
#define btstack_hsp_ag_h

#include "hci.h"
#include "classic/sdp_query_rfcomm.h"

#if defined __cplusplus
extern "C" {
#endif

/* API_START */

/**
 * @brief Packet handler for HSP Audio Gateway (AG) events. 
 * 
 * The HSP AG event has type HCI_EVENT_HSP_META with following subtypes:  
 * - HSP_SUBEVENT_AUDIO_CONNECTION_COMPLETE    
 * - HSP_SUBEVENT_AUDIO_DISCONNECTION_COMPLETE                       
 * - HSP_SUBEVENT_MICROPHONE_GAIN_CHANGED      
 * - HSP_SUBEVENT_SPEAKER_GAIN_CHANGED         
 * - HSP_SUBEVENT_HS_COMMAND      
 *
 * @param event See include/btstack/hci_cmds.h
 * @param event_size
 */
typedef void (*hsp_ag_callback_t)(uint8_t * event, uint16_t event_size);

/**
 * @brief Set up HSP AG.
 * @param rfcomm_channel_nr
 */
void hsp_ag_init(uint8_t rfcomm_channel_nr);

/**
 * @brief Create HSP Audio Gateway (AG) SDP service record. 
 * @param service Empty buffer in which a new service record will be stored.
 * @param service_record_handle
 * @param rfcomm_channel_nr
 * @param name
 */
void hsp_ag_create_sdp_record(uint8_t * service, uint32_t service_record_handle, int rfcomm_channel_nr, const char * name);

/**
 * @brief Register packet handler to receive HSP AG events.
 * @param callback 
 */
void hsp_ag_register_packet_handler(hsp_ag_callback_t callback);

/**
 * @brief Connect to HSP Headset
 *
 * Perform SDP query for an RFCOMM service on a remote device, 
 * and establish an RFCOMM connection if such service is found. The reception of the  
 * HSP_SUBEVENT_AUDIO_CONNECTION_COMPLETE or 
 * HSP_SUBEVENT_AUDIO_DISCONNECTION_COMPLETE event
 * indicate if the connection is successfully established or not. 
 *
 * @param bd_addr
 */
void hsp_ag_connect(bd_addr_t bd_addr);

/**
 * @brief Disconnect from HSP Headset.
 *
 * Releases the RFCOMM channel.
 * @param bd_addr
 */
void hsp_ag_disconnect(void);

/**
 * @brief Set microphone gain. 
 * @param gain Valid range: [0,15]
 */
void hsp_ag_set_microphone_gain(uint8_t gain);

/**
 * @brief Set speaker gain. 
 * @param gain Valid range: [0,15]
 */
void hsp_ag_set_speaker_gain(uint8_t gain);

/**
 * @brief Start ringing because of incoming call.
 */
void hsp_ag_start_ringing(void);

/**
 * @brief Stop ringing (e.g. call was terminated).
 */
void hsp_ag_stop_ringing(void);

/**
 * @brief Enable custom AT commands.
 * 
 * Custom commands are disabled by default. 
 * When enabled, custom AT commands are received via the HSP_SUBEVENT_HS_COMMAND.
 * @param enable
 */
void hsp_ag_enable_custom_commands(int enable);

/**
 * @brief Send a custom AT command to HSP Headset.
 *
 * On HSP_SUBEVENT_AG_INDICATION, the client needs to respond
 * with this function with the result to the custom command.
 * @param result 
 */
int hsp_ag_send_result(char * result);

/* API_END */

#if defined __cplusplus
}
#endif

#endif