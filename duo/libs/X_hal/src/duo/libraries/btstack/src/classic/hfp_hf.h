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
//  HFP Hands-Free (HF) unit
//
// *****************************************************************************


#ifndef __BTSTACK_HFP_HF_H
#define __BTSTACK_HFP_HF_H

#include "hci.h"
#include "classic/sdp_query_rfcomm.h"
#include "classic/hfp.h"

#if defined __cplusplus
extern "C" {
#endif

/* API_START */

/**
 * @brief Create HFP Hands-Free (HF) SDP service record. 
 */
void hfp_hf_create_sdp_record(uint8_t * service, uint32_t service_record_handle, int rfcomm_channel_nr, const char * name, uint16_t supported_features);

/**
 * @brief Intialize HFP Hands-Free (HF) device. 
 * TODO:  move optional params into setters
 */
void hfp_hf_init(uint16_t rfcomm_channel_nr, uint32_t supported_features, uint16_t * indicators, int indicators_nr, uint32_t indicators_status);

void hfp_hf_set_codecs(uint8_t * codecs, int codecs_nr);

void hfp_hf_set_supported_features(uint32_t supported_features);

/**
 * @brief Register callback for the HFP Hands-Free (HF) client. 
 */
void hfp_hf_register_packet_handler(hfp_callback_t callback);

/**
 * @brief Establish RFCOMM connection, and perform service level connection agreement:
 * - exchange of supported features
 * - retrieve Audio Gateway (AG) indicators and their status 
 * - enable indicator status update in the AG
 * - notify the AG about its own available codecs, if possible
 * - retrieve the AG information describing the call hold and multiparty services, if possible
 * - retrieve which HF indicators are enabled on the AG, if possible
 */
void hfp_hf_establish_service_level_connection(bd_addr_t bd_addr);

/**
 * @brief Release the RFCOMM channel and the audio connection between the HF and the AG. 
 * TODO: trigger release of the audio connection
 */
void hfp_hf_release_service_level_connection(bd_addr_t bd_addr);

/**
 * @brief Deactivate/reactivate status update for all indicators in the AG.
 */
void hfp_hf_enable_status_update_for_all_ag_indicators(bd_addr_t bd_addr);

void hfp_hf_disable_status_update_for_all_ag_indicators(bd_addr_t bd_addr);

/**
 * @brief Deactivate/reactivate status update for the individual indicators in the AG using bitmap.
 */
void hfp_hf_set_status_update_for_individual_ag_indicators(bd_addr_t bd_addr, uint32_t indicators_status_bitmap);


/**
 * @brief Find out the name of the currently selected Network operator by AG. 
 * The name is restricted to max 16 characters.
 *
 * TODO: what is the result of this?
 */
void hfp_hf_query_operator_selection(bd_addr_t bd_addr);

/**
 * @brief Enable/disable Extended Audio Gateway Error result codes in the AG.
 * Whenever there is an error relating to the functionality of the AG as a 
 * result of AT command, the AG shall send +CME ERROR:
 * - +CME ERROR: 0  - AG failure
 * - +CME ERROR: 1  - no connection to phone 
 * - +CME ERROR: 3  - operation not allowed 
 * - +CME ERROR: 4  - operation not supported 
 * - +CME ERROR: 5  - PH-SIM PIN required 
 * - +CME ERROR: 10 - SIM not inserted 
 * - +CME ERROR: 11 - SIM PIN required 
 * - +CME ERROR: 12 - SIM PUK required 
 * - +CME ERROR: 13 - SIM failure
 * - +CME ERROR: 14 - SIM busy
 * - +CME ERROR: 16 - incorrect password 
 * - +CME ERROR: 17 - SIM PIN2 required 
 * - +CME ERROR: 18 - SIM PUK2 required 
 * - +CME ERROR: 20 - memory full
 * - +CME ERROR: 21 - invalid index
 * - +CME ERROR: 23 - memory failure
 * - +CME ERROR: 24 - text string too long
 * - +CME ERROR: 25 - invalid characters in text string
 * - +CME ERROR: 26 - dial string too long
 * - +CME ERROR: 27 - invalid characters in dial string
 * - +CME ERROR: 30 - no network service
 * - +CME ERROR: 31 - network Timeout.
 * - +CME ERROR: 32 - network not allowed – Emergency calls only
 */
void hfp_hf_enable_report_extended_audio_gateway_error_result_code(bd_addr_t bd_addr);
void hfp_hf_disable_report_extended_audio_gateway_error_result_code(bd_addr_t bd_addr);

/**
 * @brief 
 */
void hfp_hf_establish_audio_connection(bd_addr_t bd_addr);

/**
 * @brief 
 */
void hfp_hf_release_audio_connection(bd_addr_t bd_addr);

/**
 * @brief 
 */
void hfp_hf_answer_incoming_call(bd_addr_t bd_addr);

/**
 * @brief 
 */
void hfp_hf_reject_call(bd_addr_t bd_addr);

/**
 * @brief
 */
void hfp_hf_user_busy(bd_addr_t addr);

/**
 * @brief
 */
void hfp_hf_end_active_and_accept_other(bd_addr_t addr);

/**
 * @brief
 */
void hfp_hf_swap_calls(bd_addr_t addr);

/**
 * @brief
 */
void hfp_hf_join_held_call(bd_addr_t addr);

/**
 * @brief
 */
void hfp_hf_connect_calls(bd_addr_t addr);

/**
 * @brief 
 */
void hfp_hf_terminate_call(bd_addr_t bd_addr);

/**
 * @brief 
 */
void hfp_hf_dial_number(bd_addr_t bd_addr, char * number);

/**
 * @brief
 * TODO: use int for number instead of string? 
 */
void hfp_hf_dial_memory(bd_addr_t bd_addr, char * number);

/**
 * @brief 
 */
void hfp_hf_redial_last_number(bd_addr_t bd_addr);

/*
 * @brief
 */
void hfp_hf_activate_call_waiting_notification(bd_addr_t bd_addr);

/*
 * @brief
 */
void hfp_hf_deactivate_call_waiting_notification(bd_addr_t bd_addr);

/*
 * @brief
 */
void hfp_hf_activate_calling_line_notification(bd_addr_t bd_addr);

/*
 * @brief
 */
void hfp_hf_deactivate_calling_line_notification(bd_addr_t bd_addr);


/*
 * @brief
 */
void hfp_hf_activate_echo_canceling_and_noise_reduction(bd_addr_t bd_addr);

/*
 * @brief
 */
void hfp_hf_deactivate_echo_canceling_and_noise_reduction(bd_addr_t bd_addr);

/*
 * @brief
 */
void hfp_hf_activate_voice_recognition_notification(bd_addr_t bd_addr);

/*
 * @brief
 */
void hfp_hf_deactivate_voice_recognition_notification(bd_addr_t bd_addr);

/*
 * @brief
 */
void hfp_hf_set_microphone_gain(bd_addr_t bd_addr, int gain);

/*
 * @brief
 */
void hfp_hf_set_speaker_gain(bd_addr_t bd_addr, int gain);

/*
 * @brief
 */
void hfp_hf_send_dtmf_code(bd_addr_t bd_addr, char code);

/*
 * @brief
 */
void hfp_hf_request_phone_number_for_voice_tag(bd_addr_t addr);

/*
 * @brief
 */
void hfp_hf_query_current_call_status(bd_addr_t addr);

/*
 * @brief
 */
void hfp_hf_release_call_with_index(bd_addr_t addr, int index);

/*
 * @brief
 */
void hfp_hf_private_consultation_with_call(bd_addr_t addr, int index);

/*
 * @brief
 */
void hfp_hf_rrh_query_status(bd_addr_t addr);

/*
 * @brief
 */
void hfp_hf_rrh_hold_call(bd_addr_t addr);

/*
 * @brief
 */
void hfp_hf_rrh_accept_held_call(bd_addr_t addr);

/*
 * @brief
 */
void hfp_hf_rrh_reject_held_call(bd_addr_t addr);

/*
 * @brief
 */
void hfp_hf_query_subscriber_number(bd_addr_t addr);

/*
 * @brief
 */
void hfp_hf_set_hf_indicator(bd_addr_t addr, int assigned_number, int value);

/* API_END */

#if defined __cplusplus
}
#endif

#endif // __BTSTACK_HFP_HF_H