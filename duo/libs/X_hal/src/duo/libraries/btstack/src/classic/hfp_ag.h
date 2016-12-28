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
// HFP Audio Gateway (AG) unit
//
// *****************************************************************************


#ifndef btstack_hfp_ag_h
#define btstack_hfp_ag_h

#include "hci.h"
#include "classic/sdp_query_rfcomm.h"
#include "classic/hfp.h"
#include "classic/hfp_gsm_model.h"

#if defined __cplusplus
extern "C" {
#endif

/* API_START */
typedef struct {
    uint8_t type;
    const char * number;
} hfp_phone_number_t;

/**
 * @brief Create HFP Audio Gateway (AG) SDP service record. 
 */
void hfp_ag_create_sdp_record(uint8_t * service, uint32_t service_record_handle, int rfcomm_channel_nr, const char * name, uint8_t ability_to_reject_call, uint16_t supported_features);;

/**
 * @brief Intialize HFP Audio Gateway (AG) device. 
 * TODO:  move optional params into setters
 */
void hfp_ag_init(uint16_t rfcomm_channel_nr, uint32_t supported_features, 
    uint8_t * codecs, int codecs_nr, 
    hfp_ag_indicator_t * ag_indicators, int ag_indicators_nr,
    hfp_generic_status_indicator_t * hf_indicators, int hf_indicators_nr,
    const char *call_hold_services[], int call_hold_services_nr);

/**
 * @brief Register callback for the HFP Audio Gateway (AG) client. 
 */
void hfp_ag_register_packet_handler(hfp_callback_t callback);

/**
 * @brief Enable in-band ring tone
 */
void hfp_ag_set_use_in_band_ring_tone(int use_in_band_ring_tone);


// actions used by local device / user

/**
 * @brief Establish RFCOMM connection, and perform service level connection agreement:
 * - exchange of supported features
 * - report Audio Gateway (AG) indicators and their status 
 * - enable indicator status update in the AG
 * - accept the information about available codecs in the Hands-Free (HF), if sent
 * - report own information describing the call hold and multiparty services, if possible
 * - report which HF indicators are enabled on the AG, if possible
 */
void hfp_ag_establish_service_level_connection(bd_addr_t bd_addr);

/**
 * @brief Release the RFCOMM channel and the audio connection between the HF and the AG. 
 * TODO: trigger release of the audio connection ??
 */
void hfp_ag_release_service_level_connection(bd_addr_t bd_addr);

/**
 * @brief 
 */
void hfp_ag_establish_audio_connection(bd_addr_t bd_addr);

/**
 * @brief 
 */
void hfp_ag_release_audio_connection(bd_addr_t bd_addr);

/**
 * @brief 
 */
void hfp_ag_answer_incoming_call(void);

/**
 * @brief 
 */
void hfp_ag_join_held_call(void);

/**
 * @brief 
 */
void hfp_ag_terminate_call(void);

/*
 * @brief
 */
void hfp_ag_hold_incoming_call(void);

/*
 * @brief
 */
void hfp_ag_accept_held_incoming_call(void);

/*
 * @brief
 */
void hfp_ag_reject_held_incoming_call(void);

/*
 * @brief
 */
void hfp_ag_set_microphone_gain(bd_addr_t bd_addr, int gain);

/*
 * @brief
 */
void hfp_ag_set_speaker_gain(bd_addr_t bd_addr, int gain);

/*
 * @brief
 */
void hfp_ag_set_battery_level(int level);

/*
 * @brief
 */
void hfp_ag_clear_last_dialed_number(void);


// Voice Recognition

/*
 * @brief
 */
void hfp_ag_activate_voice_recognition(bd_addr_t bd_addr, int activate);

/*
 * @brief
 */
void hfp_ag_send_phone_number_for_voice_tag(bd_addr_t bd_addr, const char * number);

/*
 * @brief
 */
void hfp_ag_reject_phone_number_for_voice_tag(bd_addr_t bd_addr);


// Cellular Actions

/**
 * @brief 
 */
void hfp_ag_incoming_call(void);

/**
 * @brief number is stored.
 */
void hfp_ag_set_clip(uint8_t type, const char * number);

/**
 * @brief 
 */
void hfp_ag_outgoing_call_rejected(void);

/**
 * @brief 
 */
void hfp_ag_outgoing_call_accepted(void);

/**
 * @brief 
 */
void hfp_ag_outgoing_call_ringing(void);

/**
 * @brief 
 */
void hfp_ag_outgoing_call_established(void);

/**
 * @brief 
 */
void hfp_ag_call_dropped(void);
/*
 * @brief
 */
void hfp_ag_set_registration_status(int status);

/*
 * @brief
 */
void hfp_ag_set_signal_strength(int strength);

/*
 * @brief
 */
void hfp_ag_set_roaming_status(int status);

/*
 * @brief
 */
void hfp_ag_set_subcriber_number_information(hfp_phone_number_t * numbers, int numbers_count);

/*
 * @brief Called by cellular unit after a DTMF code was transmitted, so that the next one can be emitted
 */
void hfp_ag_send_dtmf_code_done(bd_addr_t bd_addr);

/**
 * @brief Report Extended Audio Gateway Error result codes in the AG.
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
void hfp_ag_report_extended_audio_gateway_error_result_code(bd_addr_t bd_addr, hfp_cme_error_t error);

/* API_END */


#if defined __cplusplus
}
#endif

#endif