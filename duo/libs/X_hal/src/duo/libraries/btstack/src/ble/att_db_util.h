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

/**
 * Helper to construct ATT DB at runtime 
 * (BTstack GATT Compiler is not used)
 */

#ifndef __ATT_DB_UTIL
#define __ATT_DB_UTIL

#include "btstack_config.h"
#
#ifdef HAVE_MALLOC
#else
#endif

#include <stdint.h>

#if defined __cplusplus
extern "C" {
#endif

/* API_START */

/**
 * @brief Init ATT DB storage
 */
void att_db_util_init(void);

/**
 * @brief Add primary service for 16-bit UUID
 */
void att_db_util_add_service_uuid16(uint16_t udid16);

/**
 * @brief Add primary service for 128-bit UUID
 */
void att_db_util_add_service_uuid128(uint8_t * udid128);

/**
 * @brief Add Characteristic with 16-bit UUID, properties, and data
 * @returns attribute value handle
 * @see ATT_PROPERTY_* in ble/att_db.h
 */
uint16_t att_db_util_add_characteristic_uuid16(uint16_t   udid16,  uint16_t properties, uint8_t * data, uint16_t data_len);

/**
 * @brief Add Characteristic with 128-bit UUID, properties, and data
 * @returns attribute value handle
 * @see ATT_PROPERTY_* in ble/att_db.h
 */
uint16_t att_db_util_add_characteristic_uuid128(uint8_t * udid128, uint16_t properties, uint8_t * data, uint16_t data_len);

/** 
 * @brief Get address of constructed ATT DB
 */
uint8_t * att_db_util_get_address(void);

/**
 * @brief Get size of constructed ATT DB 
 */
uint16_t att_db_util_get_size(void);

/* API_END */

#if defined __cplusplus
}
#endif
#endif
