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
//  GSM model 
//
// *****************************************************************************


#ifndef BTSTACK_HFP_GSM_MODEL_H
#define BTSTACK_HFP_GSM_MODEL_H

#include "hci.h"
#include "sdp_query_rfcomm.h"
#include "hfp.h"

#if defined __cplusplus
extern "C" {
#endif

/* API_START */
typedef struct {
    uint8_t used_slot;
    hfp_enhanced_call_status_t enhanced_status;
    hfp_enhanced_call_dir_t direction;
    hfp_enhanced_call_mode_t mode;
    hfp_enhanced_call_mpty_t mpty;
    // TODO: sort on drop call, so that index corresponds to table index
    int index;
    uint8_t clip_type;
    char    clip_number[25];
} hfp_gsm_call_t;

hfp_callheld_status_t hfp_gsm_callheld_status(void);
hfp_call_status_t hfp_gsm_call_status(void);
hfp_callsetup_status_t hfp_gsm_callsetup_status(void);

int hfp_gsm_get_number_of_calls(void);
char * hfp_gsm_last_dialed_number(void);
void hfp_gsm_clear_last_dialed_number(void);


hfp_gsm_call_t * hfp_gsm_call(int index);

int hfp_gsm_call_possible(void);

uint8_t hfp_gsm_clip_type(void);
char *  hfp_gsm_clip_number(void);

void hfp_gsm_init(void);

void hfp_gsm_handle_event_with_clip(hfp_ag_call_event_t event, uint8_t type, const char * number);
void hfp_gsm_handle_event_with_call_index(hfp_ag_call_event_t event, uint8_t index);
void hfp_gsm_handle_event_with_call_number(hfp_ag_call_event_t event, const char * number);
void hfp_gsm_handle_event(hfp_ag_call_event_t event);

/* API_END */

#if defined __cplusplus
}
#endif

#endif