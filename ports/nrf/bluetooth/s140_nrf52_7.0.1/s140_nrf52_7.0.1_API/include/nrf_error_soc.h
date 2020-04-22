/*
 * Copyright (c) 2012 - 2017, Nordic Semiconductor ASA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
  @addtogroup nrf_soc_api
  @{
  @defgroup nrf_soc_error SoC Library Error Codes
  @{

  @brief Error definitions for the SoC library

*/

/* Header guard */
#ifndef NRF_ERROR_SOC_H__
#define NRF_ERROR_SOC_H__

#include "nrf_error.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Mutex Errors */
#define NRF_ERROR_SOC_MUTEX_ALREADY_TAKEN                 (NRF_ERROR_SOC_BASE_NUM + 0)  ///< Mutex already taken

/* NVIC errors */
#define NRF_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE        (NRF_ERROR_SOC_BASE_NUM + 1)  ///< NVIC interrupt not available
#define NRF_ERROR_SOC_NVIC_INTERRUPT_PRIORITY_NOT_ALLOWED (NRF_ERROR_SOC_BASE_NUM + 2)  ///< NVIC interrupt priority not allowed
#define NRF_ERROR_SOC_NVIC_SHOULD_NOT_RETURN              (NRF_ERROR_SOC_BASE_NUM + 3)  ///< NVIC should not return

/* Power errors */
#define NRF_ERROR_SOC_POWER_MODE_UNKNOWN                  (NRF_ERROR_SOC_BASE_NUM + 4)  ///< Power mode unknown
#define NRF_ERROR_SOC_POWER_POF_THRESHOLD_UNKNOWN         (NRF_ERROR_SOC_BASE_NUM + 5)  ///< Power POF threshold unknown
#define NRF_ERROR_SOC_POWER_OFF_SHOULD_NOT_RETURN         (NRF_ERROR_SOC_BASE_NUM + 6)  ///< Power off should not return

/* Rand errors */
#define NRF_ERROR_SOC_RAND_NOT_ENOUGH_VALUES              (NRF_ERROR_SOC_BASE_NUM + 7)  ///< RAND not enough values

/* PPI errors */
#define NRF_ERROR_SOC_PPI_INVALID_CHANNEL                 (NRF_ERROR_SOC_BASE_NUM + 8)  ///< Invalid PPI Channel
#define NRF_ERROR_SOC_PPI_INVALID_GROUP                   (NRF_ERROR_SOC_BASE_NUM + 9)  ///< Invalid PPI Group

#ifdef __cplusplus
}
#endif
#endif // NRF_ERROR_SOC_H__
/**
  @}
  @}
*/
