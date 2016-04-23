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
// Advertising Data Parser 
//
// *****************************************************************************

#ifndef __AD_PARSER_H
#define __AD_PARSER_H

#include "btstack_config.h"

#if defined __cplusplus
extern "C" {
#endif

/* API_START */

typedef struct ad_context {
     uint8_t * data;
     uint8_t   offset;
     uint8_t   length;
} ad_context_t;

// Advertising or Scan Response data iterator
void ad_iterator_init(ad_context_t *context, uint8_t ad_len, uint8_t * ad_data);
int  ad_iterator_has_more(ad_context_t * context);
void ad_iterator_next(ad_context_t * context);

// Access functions
uint8_t   ad_iterator_get_data_type(ad_context_t * context);
uint8_t   ad_iterator_get_data_len(ad_context_t * context);
uint8_t * ad_iterator_get_data(ad_context_t * context);

// convenience function on complete advertisements
int ad_data_contains_uuid16(uint8_t ad_len, uint8_t * ad_data, uint16_t uuid);
int ad_data_contains_uuid128(uint8_t ad_len, uint8_t * ad_data, uint8_t * uuid128);

/* API_END */

#if defined __cplusplus
}
#endif
#endif // __AD_PARSER_H
