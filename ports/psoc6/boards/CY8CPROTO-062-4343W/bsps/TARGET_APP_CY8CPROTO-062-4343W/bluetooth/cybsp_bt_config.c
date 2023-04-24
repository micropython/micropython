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

#if defined(COMPONENT_WICED_BLE) || defined(COMPONENT_WICED_DUALMODE)

#include "cybsp_bt_config.h"
#include "cycfg_connectivity_bt.h"
#include "wiced_bt_dev.h"

// Not all boards use all of these pins. Any that arn't defined we will fallback on No Connects.
#if !defined(CYBSP_BT_POWER)
    #define CYBSP_BT_POWER              (NC)
#endif
#if !defined(CYCFG_BT_DEV_WAKE_GPIO)
    #define CYCFG_BT_DEV_WAKE_GPIO      (NC)
#endif
#if !defined(CYCFG_BT_HOST_WAKE_GPIO)
    #define CYCFG_BT_HOST_WAKE_GPIO     (NC)
#endif
#if !defined(CYBSP_BT_UART_TX)
    #define CYBSP_BT_UART_TX            (NC)
#endif
#if !defined(CYBSP_BT_UART_RX)
    #define CYBSP_BT_UART_RX            (NC)
#endif
#if !defined(CYBSP_BT_UART_RTS)
    #define CYBSP_BT_UART_RTS           (NC)
#endif
#if !defined(CYBSP_BT_UART_CTS)
    #define CYBSP_BT_UART_CTS           (NC)
#endif

const cybt_platform_config_t cybsp_bt_platform_cfg =
{
    .hci_config =
    {
        .hci_transport = CYBT_HCI_UART,

        .hci =
        {
            .hci_uart =
            {
                .uart_tx_pin = CYBSP_BT_UART_TX,
                .uart_rx_pin = CYBSP_BT_UART_RX,
                .uart_rts_pin = CYBSP_BT_UART_RTS,
                .uart_cts_pin = CYBSP_BT_UART_CTS,

                .baud_rate_for_fw_download = CYBSP_BT_PLATFORM_CFG_BAUD_DOWNLOAD,
                .baud_rate_for_feature = CYBSP_BT_PLATFORM_CFG_BAUD_FEATURE,

                .data_bits = CYBSP_BT_PLATFORM_CFG_BITS_DATA,
                .stop_bits = CYBSP_BT_PLATFORM_CFG_BITS_STOP,
                .parity = CYHAL_UART_PARITY_NONE,
                .flow_control = true
            }
        }
    },

    .controller_config =
    {
        .bt_power_pin = CYBSP_BT_POWER,
        .sleep_mode =
        {
            .sleep_mode_enabled = CYCFG_BT_LP_ENABLED,
            .device_wakeup_pin = CYCFG_BT_DEV_WAKE_GPIO,
            .host_wakeup_pin = CYCFG_BT_HOST_WAKE_GPIO,
            .device_wake_polarity = CYCFG_BT_DEV_WAKE_POLARITY,
            .host_wake_polarity = CYCFG_BT_HOST_WAKE_IRQ_EVENT
        }
    },

    .task_mem_pool_size = CYBSP_BT_PLATFORM_CFG_MEM_POOL_BYTES
};

#endif /* defined(COMPONENT_WICED_BLE) || defined(COMPONENT_WICED_DUALMODE) */
