/***********************************************************************************************//**
 * \copyright
 * Copyright 2020-2022 Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **************************************************************************************************/

/**
 * \addtogroup group_bsp_bt Bluetooth Configuration Structure
 * \{
 * Basic configuration structure for the Bluetooth interface on this board.
 */
#pragma once

#if defined(COMPONENT_WICED_BLE) || defined(COMPONENT_WICED_DUALMODE)

#include "cybt_platform_config.h"
#include "cycfg_pins.h"

#if defined(__cplusplus)
extern "C" {
#endif

#if !defined(CYBSP_BT_PLATFORM_CFG_BAUD_DOWNLOAD)
/** If not already defined, the baud rate to download data at. */
#define CYBSP_BT_PLATFORM_CFG_BAUD_DOWNLOAD     (115200)
#endif

#if !defined(CYBSP_BT_PLATFORM_CFG_BAUD_FEATURE)
/** If not already defined, the baud rate for general operation. */
#define CYBSP_BT_PLATFORM_CFG_BAUD_FEATURE      (115200)
#endif

#if !defined(CYBSP_BT_PLATFORM_CFG_BITS_DATA)
/** If not already defined, the number of data bits to transmit. */
#define CYBSP_BT_PLATFORM_CFG_BITS_DATA         (8)
#endif

#if !defined(CYBSP_BT_PLATFORM_CFG_BITS_STOP)
/** If not already defined, the number of stop bits to transmit. */
#define CYBSP_BT_PLATFORM_CFG_BITS_STOP         (1)
#endif

#if !defined(CYBSP_BT_PLATFORM_CFG_MEM_POOL_BYTES)
/** If not already defined, the number of bytes to allocated for the task memory pool. */
#define CYBSP_BT_PLATFORM_CFG_MEM_POOL_BYTES    (2048)
#endif

/** Bluetooth platform configuration settings for the board. */
extern const cybt_platform_config_t cybsp_bt_platform_cfg;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* defined(COMPONENT_WICED_BLE) || defined(COMPONENT_WICED_DUALMODE) */

/** \} group_bsp_bt */
