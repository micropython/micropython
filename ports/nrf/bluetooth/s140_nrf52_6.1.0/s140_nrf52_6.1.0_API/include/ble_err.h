/*
 * Copyright (c) 2012 - 2018, Nordic Semiconductor ASA
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
  @addtogroup BLE_COMMON
  @{
  @addtogroup  nrf_error
  @{
    @ingroup BLE_COMMON
  @}

  @defgroup ble_err General error codes
  @{

  @brief General error code definitions for the BLE API.

  @ingroup BLE_COMMON
*/
#ifndef NRF_BLE_ERR_H__
#define NRF_BLE_ERR_H__

#include "nrf_error.h"

#ifdef __cplusplus
extern "C" {
#endif

/* @defgroup BLE_ERRORS Error Codes
 * @{ */
#define BLE_ERROR_NOT_ENABLED            (NRF_ERROR_STK_BASE_NUM+0x001) /**< @ref sd_ble_enable has not been called. */
#define BLE_ERROR_INVALID_CONN_HANDLE    (NRF_ERROR_STK_BASE_NUM+0x002) /**< Invalid connection handle. */
#define BLE_ERROR_INVALID_ATTR_HANDLE    (NRF_ERROR_STK_BASE_NUM+0x003) /**< Invalid attribute handle. */
#define BLE_ERROR_INVALID_ADV_HANDLE     (NRF_ERROR_STK_BASE_NUM+0x004) /**< Invalid advertising handle. */
#define BLE_ERROR_INVALID_ROLE           (NRF_ERROR_STK_BASE_NUM+0x005) /**< Invalid role. */
#define BLE_ERROR_BLOCKED_BY_OTHER_LINKS (NRF_ERROR_STK_BASE_NUM+0x006) /**< The attempt to change link settings failed due to the scheduling of other links. */
/** @} */


/** @defgroup BLE_ERROR_SUBRANGES Module specific error code subranges
 *  @brief Assignment of subranges for module specific error codes.
 *  @note For specific error codes, see ble_<module>.h or ble_error_<module>.h.
 * @{ */
#define NRF_L2CAP_ERR_BASE             (NRF_ERROR_STK_BASE_NUM+0x100) /**< L2CAP specific errors. */
#define NRF_GAP_ERR_BASE               (NRF_ERROR_STK_BASE_NUM+0x200) /**< GAP specific errors. */
#define NRF_GATTC_ERR_BASE             (NRF_ERROR_STK_BASE_NUM+0x300) /**< GATT client specific errors. */
#define NRF_GATTS_ERR_BASE             (NRF_ERROR_STK_BASE_NUM+0x400) /**< GATT server specific errors. */
/** @} */

#ifdef __cplusplus
}
#endif
#endif


/**
  @}
  @}
*/
