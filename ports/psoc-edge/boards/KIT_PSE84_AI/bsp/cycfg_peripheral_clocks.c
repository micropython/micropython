/*******************************************************************************
 * File Name: cycfg_peripheral_clocks.c
 *
 * Description:
 * Peripheral clock divider configuration
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

#include "cycfg_peripheral_clocks.h"

void init_cycfg_peripheral_clocks(void)
{
    Cy_SysClk_PeriGroupSetDivider((0 << 8) | 2, 1U);
    Cy_SysClk_PeriGroupSetDivider((1 << 8) | 1, 1U);
    Cy_SysClk_PeriGroupSetDivider((1 << 8) | 3, 3U);
    Cy_SysClk_PeriPclkDisableDivider((en_clk_dst_t)CYBSP_I2C_CONTROLLER_3V3_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_8_BIT, 0U);
    Cy_SysClk_PeriPclkSetDivider((en_clk_dst_t)CYBSP_I2C_CONTROLLER_3V3_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_8_BIT, 0U, 31U);
    Cy_SysClk_PeriPclkEnableDivider((en_clk_dst_t)CYBSP_I2C_CONTROLLER_3V3_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_8_BIT, 0U);
    Cy_SysClk_PeriPclkDisableDivider((en_clk_dst_t)CYBSP_SPI_CONTROLLER_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_8_BIT, 1U);
    Cy_SysClk_PeriPclkSetDivider((en_clk_dst_t)CYBSP_SPI_CONTROLLER_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_8_BIT, 1U, 0U);
    Cy_SysClk_PeriPclkEnableDivider((en_clk_dst_t)CYBSP_SPI_CONTROLLER_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_8_BIT, 1U);
    Cy_SysClk_PeriPclkDisableDivider((en_clk_dst_t)CYBSP_USB_OS_TIMER_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_8_BIT, 2U);
    Cy_SysClk_PeriPclkSetDivider((en_clk_dst_t)CYBSP_USB_OS_TIMER_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_8_BIT, 2U, 0U);
    Cy_SysClk_PeriPclkEnableDivider((en_clk_dst_t)CYBSP_USB_OS_TIMER_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_8_BIT, 2U);
    Cy_SysClk_PeriPclkDisableDivider((en_clk_dst_t)CYBSP_PWM_DT_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_8_BIT, 3U);
    Cy_SysClk_PeriPclkSetDivider((en_clk_dst_t)CYBSP_PWM_DT_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_8_BIT, 3U, 249U);
    Cy_SysClk_PeriPclkEnableDivider((en_clk_dst_t)CYBSP_PWM_DT_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_8_BIT, 3U);
    Cy_SysClk_PeriPclkDisableDivider((en_clk_dst_t)CYBSP_I2C_CONTROLLER_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_16_BIT, 0U);
    Cy_SysClk_PeriPclkSetDivider((en_clk_dst_t)CYBSP_I2C_CONTROLLER_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_16_BIT, 0U, 9U);
    Cy_SysClk_PeriPclkEnableDivider((en_clk_dst_t)CYBSP_I2C_CONTROLLER_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_16_BIT, 0U);
    Cy_SysClk_PeriPclkDisableDivider((en_clk_dst_t)CYBSP_DEBUG_UART_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_16_BIT, 1U);
    Cy_SysClk_PeriPclkSetDivider((en_clk_dst_t)CYBSP_DEBUG_UART_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_16_BIT, 1U, 86U);
    Cy_SysClk_PeriPclkEnableDivider((en_clk_dst_t)CYBSP_DEBUG_UART_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_16_BIT, 1U);
    Cy_SysClk_PeriPclkDisableDivider((en_clk_dst_t)CYBSP_GENERAL_PURPOSE_TIMER_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_16_BIT, 2U);
    Cy_SysClk_PeriPclkSetDivider((en_clk_dst_t)CYBSP_GENERAL_PURPOSE_TIMER_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_16_BIT, 2U, 9999U);
    Cy_SysClk_PeriPclkEnableDivider((en_clk_dst_t)CYBSP_GENERAL_PURPOSE_TIMER_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_16_BIT, 2U);
    Cy_SysClk_PeriPclkDisableDivider((en_clk_dst_t)CYBSP_PWM_LED_CTRL_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_16_BIT, 3U);
    Cy_SysClk_PeriPclkSetDivider((en_clk_dst_t)CYBSP_PWM_LED_CTRL_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_16_BIT, 3U, 49999U);
    Cy_SysClk_PeriPclkEnableDivider((en_clk_dst_t)CYBSP_PWM_LED_CTRL_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_16_BIT, 3U);
    Cy_SysClk_PeriPclkDisableDivider((en_clk_dst_t)CYBSP_BT_UART_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_16_5_BIT, 0U);
    Cy_SysClk_PeriPclkSetFracDivider((en_clk_dst_t)CYBSP_BT_UART_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_16_5_BIT, 0U, 107U, 16U);
    Cy_SysClk_PeriPclkEnableDivider((en_clk_dst_t)CYBSP_BT_UART_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_16_5_BIT, 0U);
    Cy_SysClk_PeriPclkDisableDivider((en_clk_dst_t)CYBSP_PWM_DVP_CAM_CTRL_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_16_5_BIT, 1U);
    Cy_SysClk_PeriPclkSetFracDivider((en_clk_dst_t)CYBSP_PWM_DVP_CAM_CTRL_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_16_5_BIT, 1U, 3U, 5U);
    Cy_SysClk_PeriPclkEnableDivider((en_clk_dst_t)CYBSP_PWM_DVP_CAM_CTRL_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_16_5_BIT, 1U);
    Cy_SysClk_PeriPclkDisableDivider((en_clk_dst_t)CYBSP_SMART_IO_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_24_5_BIT, 0U);
    Cy_SysClk_PeriPclkSetFracDivider((en_clk_dst_t)CYBSP_SMART_IO_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_24_5_BIT, 0U, 505049U, 0U);
    Cy_SysClk_PeriPclkEnableDivider((en_clk_dst_t)CYBSP_SMART_IO_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_24_5_BIT, 0U);
    Cy_SysClk_PeriPclkDisableDivider((en_clk_dst_t)CYBSP_TRACE_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_16_5_BIT, 0U);
    Cy_SysClk_PeriPclkSetFracDivider((en_clk_dst_t)CYBSP_TRACE_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_16_5_BIT, 0U, 0U, 0U);
    Cy_SysClk_PeriPclkEnableDivider((en_clk_dst_t)CYBSP_TRACE_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_16_5_BIT, 0U);
    Cy_SysClk_PeriPclkDisableDivider((en_clk_dst_t)CYBSP_I3C_CONTROLLER_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_8_BIT, 0U);
    Cy_SysClk_PeriPclkSetDivider((en_clk_dst_t)CYBSP_I3C_CONTROLLER_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_8_BIT, 0U, 7U);
    Cy_SysClk_PeriPclkEnableDivider((en_clk_dst_t)CYBSP_I3C_CONTROLLER_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_8_BIT, 0U);
    Cy_SysClk_PeriPclkDisableDivider((en_clk_dst_t)CYBSP_TDM_CONTROLLER_0_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_16_5_BIT, 0U);
    Cy_SysClk_PeriPclkSetFracDivider((en_clk_dst_t)CYBSP_TDM_CONTROLLER_0_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_16_5_BIT, 0U, 23U, 0U);
    Cy_SysClk_PeriPclkEnableDivider((en_clk_dst_t)CYBSP_TDM_CONTROLLER_0_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_16_5_BIT, 0U);
    Cy_SysClk_PeriPclkDisableDivider((en_clk_dst_t)CYBSP_PDM_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_16_5_BIT, 1U);
    Cy_SysClk_PeriPclkSetFracDivider((en_clk_dst_t)CYBSP_PDM_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_16_5_BIT, 1U, 3U, 0U);
    Cy_SysClk_PeriPclkEnableDivider((en_clk_dst_t)CYBSP_PDM_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_16_5_BIT, 1U);
    Cy_SysClk_PeriPclkDisableDivider((en_clk_dst_t)CYBSP_SDHC_0_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_8_BIT, 0U);
    Cy_SysClk_PeriPclkSetDivider((en_clk_dst_t)CYBSP_SDHC_0_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_8_BIT, 0U, 1U);
    Cy_SysClk_PeriPclkEnableDivider((en_clk_dst_t)CYBSP_SDHC_0_CLK_DIV_GRP_NUM, CY_SYSCLK_DIV_8_BIT, 0U);
}
