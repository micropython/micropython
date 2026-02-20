/*******************************************************************************
 * File Name: cycfg_connectivity_bt.h
 *
 * Description:
 * Connectivity BT configuration
 * This file was automatically generated and should not be modified.
 * Configurator Backend 3.60.0
 * device-db 4.34.0.9502
 * mtb-dsl-pse8xxgp 1.2.0.895
 *
 *******************************************************************************
 * Copyright 2026 Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.
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
 ******************************************************************************/

#if !defined(CYCFG_CONNECTIVITY_BT_H)
#define CYCFG_CONNECTIVITY_BT_H

#include "cycfg_notices.h"
#include "cycfg_pins.h"

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

#define bt_0_power_0_ENABLED 1U
#define CYCFG_BT_LP_ENABLED (1u)
#define CYCFG_BT_WAKE_EVENT_ACTIVE_LOW (0)
#define CYCFG_BT_WAKE_EVENT_ACTIVE_HIGH (1)
#define CYCFG_BT_HOST_WAKE_GPIO CYBSP_BT_HOST_WAKE_HAL_PORT_PIN
#define CYCFG_BT_HOST_WAKE_IRQ_EVENT CYCFG_BT_WAKE_EVENT_ACTIVE_HIGH
#define CYCFG_BT_DEV_WAKE_GPIO CYBSP_BT_DEVICE_WAKE_HAL_PORT_PIN
#define CYCFG_BT_DEV_WAKE_POLARITY CYCFG_BT_WAKE_EVENT_ACTIVE_LOW
#define CYCFG_BT_HOST_WAKE_PORT_NUM CYBSP_BT_HOST_WAKE_HAL_PORT_PIN
#define CYCFG_BT_HOST_WAKE_PIN CYCFG_BT_WAKE_EVENT_ACTIVE_HIGH
#define CYCFG_BT_DEVICE_WAKE_PORT_NUM CYBSP_BT_HOST_WAKE_HAL_PORT_PIN
#define CYCFG_BT_DEVICE_WAKE_PIN CYCFG_BT_WAKE_EVENT_ACTIVE_HIGH

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

#endif /* CYCFG_CONNECTIVITY_BT_H */
