/*******************************************************************************
 * File Name: cycfg_peripheral_clocks.h
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

#if !defined(CYCFG_PERIPHERAL_CLOCKS_H)
#define CYCFG_PERIPHERAL_CLOCKS_H

#include "cycfg_notices.h"
#include "cy_sysclk.h"

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

#define peri_0_mmio_0_ENABLED 1U
#define peri_1_mmio_0_ENABLED 1U
#define CYBSP_I2C_CONTROLLER_3V3_CLK_DIV_ENABLED 1U

#if !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE)
#define CYBSP_I2C_CONTROLLER_3V3_CLK_DIV_HW CY_SYSCLK_DIV_8_BIT
#endif /* !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE) */

#define CYBSP_I2C_CONTROLLER_3V3_CLK_DIV_NUM 0U
#define CYBSP_I2C_CONTROLLER_3V3_CLK_DIV_GRP_NUM ((1U << PERI_PCLK_GR_NUM_Pos) | (0U << PERI_PCLK_INST_NUM_Pos))

#if !defined (CYBSP_I2C_CONTROLLER_3V3_CLK_DIV_GRP_NUM)
#define CYBSP_I2C_CONTROLLER_3V3_CLK_DIV_GRP_NUM CYBSP_I2C_CONTROLLER_3V3_CLK_DIV_GRP_NUM
#endif /* !defined (CYBSP_I2C_CONTROLLER_3V3_CLK_DIV_GRP_NUM) */

#define CYBSP_SPI_CONTROLLER_CLK_DIV_ENABLED 1U

#if !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE)
#define CYBSP_SPI_CONTROLLER_CLK_DIV_HW CY_SYSCLK_DIV_8_BIT
#endif /* !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE) */

#define CYBSP_SPI_CONTROLLER_CLK_DIV_NUM 1U
#define CYBSP_SPI_CONTROLLER_CLK_DIV_GRP_NUM ((1U << PERI_PCLK_GR_NUM_Pos) | (0U << PERI_PCLK_INST_NUM_Pos))

#if !defined (CYBSP_SPI_CONTROLLER_CLK_DIV_GRP_NUM)
#define CYBSP_SPI_CONTROLLER_CLK_DIV_GRP_NUM CYBSP_SPI_CONTROLLER_CLK_DIV_GRP_NUM
#endif /* !defined (CYBSP_SPI_CONTROLLER_CLK_DIV_GRP_NUM) */

#define CYBSP_USB_OS_TIMER_CLK_DIV_ENABLED 1U

#if !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE)
#define CYBSP_USB_OS_TIMER_CLK_DIV_HW CY_SYSCLK_DIV_8_BIT
#endif /* !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE) */

#define CYBSP_USB_OS_TIMER_CLK_DIV_NUM 2U
#define CYBSP_USB_OS_TIMER_CLK_DIV_GRP_NUM ((1U << PERI_PCLK_GR_NUM_Pos) | (0U << PERI_PCLK_INST_NUM_Pos))

#if !defined (CYBSP_USB_OS_TIMER_CLK_DIV_GRP_NUM)
#define CYBSP_USB_OS_TIMER_CLK_DIV_GRP_NUM CYBSP_USB_OS_TIMER_CLK_DIV_GRP_NUM
#endif /* !defined (CYBSP_USB_OS_TIMER_CLK_DIV_GRP_NUM) */

#define CYBSP_PWM_DT_CLK_DIV_ENABLED 1U

#if !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE)
#define CYBSP_PWM_DT_CLK_DIV_HW CY_SYSCLK_DIV_8_BIT
#endif /* !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE) */

#define CYBSP_PWM_DT_CLK_DIV_NUM 3U
#define CYBSP_PWM_DT_CLK_DIV_GRP_NUM ((1U << PERI_PCLK_GR_NUM_Pos) | (0U << PERI_PCLK_INST_NUM_Pos))

#if !defined (CYBSP_PWM_DT_CLK_DIV_GRP_NUM)
#define CYBSP_PWM_DT_CLK_DIV_GRP_NUM CYBSP_PWM_DT_CLK_DIV_GRP_NUM
#endif /* !defined (CYBSP_PWM_DT_CLK_DIV_GRP_NUM) */

#define CYBSP_I2C_CONTROLLER_CLK_DIV_ENABLED 1U

#if !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE)
#define CYBSP_I2C_CONTROLLER_CLK_DIV_HW CY_SYSCLK_DIV_16_BIT
#endif /* !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE) */

#define CYBSP_I2C_CONTROLLER_CLK_DIV_NUM 0U
#define CYBSP_I2C_CONTROLLER_CLK_DIV_GRP_NUM ((1U << PERI_PCLK_GR_NUM_Pos) | (0U << PERI_PCLK_INST_NUM_Pos))

#if !defined (CYBSP_I2C_CONTROLLER_CLK_DIV_GRP_NUM)
#define CYBSP_I2C_CONTROLLER_CLK_DIV_GRP_NUM CYBSP_I2C_CONTROLLER_CLK_DIV_GRP_NUM
#endif /* !defined (CYBSP_I2C_CONTROLLER_CLK_DIV_GRP_NUM) */

#define CYBSP_DEBUG_UART_CLK_DIV_ENABLED 1U

#if !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE)
#define CYBSP_DEBUG_UART_CLK_DIV_HW CY_SYSCLK_DIV_16_BIT
#endif /* !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE) */

#define CYBSP_DEBUG_UART_CLK_DIV_NUM 1U
#define CYBSP_DEBUG_UART_CLK_DIV_GRP_NUM ((1U << PERI_PCLK_GR_NUM_Pos) | (0U << PERI_PCLK_INST_NUM_Pos))

#if !defined (CYBSP_DEBUG_UART_CLK_DIV_GRP_NUM)
#define CYBSP_DEBUG_UART_CLK_DIV_GRP_NUM CYBSP_DEBUG_UART_CLK_DIV_GRP_NUM
#endif /* !defined (CYBSP_DEBUG_UART_CLK_DIV_GRP_NUM) */

#define CYBSP_GENERAL_PURPOSE_TIMER_CLK_DIV_ENABLED 1U

#if !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE)
#define CYBSP_GENERAL_PURPOSE_TIMER_CLK_DIV_HW CY_SYSCLK_DIV_16_BIT
#endif /* !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE) */

#define CYBSP_GENERAL_PURPOSE_TIMER_CLK_DIV_NUM 2U
#define CYBSP_GENERAL_PURPOSE_TIMER_CLK_DIV_GRP_NUM ((1U << PERI_PCLK_GR_NUM_Pos) | (0U << PERI_PCLK_INST_NUM_Pos))

#if !defined (CYBSP_GENERAL_PURPOSE_TIMER_CLK_DIV_GRP_NUM)
#define CYBSP_GENERAL_PURPOSE_TIMER_CLK_DIV_GRP_NUM CYBSP_GENERAL_PURPOSE_TIMER_CLK_DIV_GRP_NUM
#endif /* !defined (CYBSP_GENERAL_PURPOSE_TIMER_CLK_DIV_GRP_NUM) */

#define CYBSP_PWM_LED_CTRL_CLK_DIV_ENABLED 1U

#if !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE)
#define CYBSP_PWM_LED_CTRL_CLK_DIV_HW CY_SYSCLK_DIV_16_BIT
#endif /* !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE) */

#define CYBSP_PWM_LED_CTRL_CLK_DIV_NUM 3U
#define CYBSP_PWM_LED_CTRL_CLK_DIV_GRP_NUM ((1U << PERI_PCLK_GR_NUM_Pos) | (0U << PERI_PCLK_INST_NUM_Pos))

#if !defined (CYBSP_PWM_LED_CTRL_CLK_DIV_GRP_NUM)
#define CYBSP_PWM_LED_CTRL_CLK_DIV_GRP_NUM CYBSP_PWM_LED_CTRL_CLK_DIV_GRP_NUM
#endif /* !defined (CYBSP_PWM_LED_CTRL_CLK_DIV_GRP_NUM) */

#define CYBSP_BT_UART_CLK_DIV_ENABLED 1U

#if !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE)
#define CYBSP_BT_UART_CLK_DIV_HW CY_SYSCLK_DIV_16_5_BIT
#endif /* !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE) */

#define CYBSP_BT_UART_CLK_DIV_NUM 0U
#define CYBSP_BT_UART_CLK_DIV_GRP_NUM ((1U << PERI_PCLK_GR_NUM_Pos) | (0U << PERI_PCLK_INST_NUM_Pos))

#if !defined (CYBSP_BT_UART_CLK_DIV_GRP_NUM)
#define CYBSP_BT_UART_CLK_DIV_GRP_NUM CYBSP_BT_UART_CLK_DIV_GRP_NUM
#endif /* !defined (CYBSP_BT_UART_CLK_DIV_GRP_NUM) */

#define CYBSP_PWM_DVP_CAM_CTRL_CLK_DIV_ENABLED 1U

#if !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE)
#define CYBSP_PWM_DVP_CAM_CTRL_CLK_DIV_HW CY_SYSCLK_DIV_16_5_BIT
#endif /* !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE) */

#define CYBSP_PWM_DVP_CAM_CTRL_CLK_DIV_NUM 1U
#define CYBSP_PWM_DVP_CAM_CTRL_CLK_DIV_GRP_NUM ((1U << PERI_PCLK_GR_NUM_Pos) | (0U << PERI_PCLK_INST_NUM_Pos))

#if !defined (CYBSP_PWM_DVP_CAM_CTRL_CLK_DIV_GRP_NUM)
#define CYBSP_PWM_DVP_CAM_CTRL_CLK_DIV_GRP_NUM CYBSP_PWM_DVP_CAM_CTRL_CLK_DIV_GRP_NUM
#endif /* !defined (CYBSP_PWM_DVP_CAM_CTRL_CLK_DIV_GRP_NUM) */

#define CYBSP_SMART_IO_CLK_DIV_ENABLED 1U

#if !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE)
#define CYBSP_SMART_IO_CLK_DIV_HW CY_SYSCLK_DIV_24_5_BIT
#endif /* !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE) */

#define CYBSP_SMART_IO_CLK_DIV_NUM 0U
#define CYBSP_SMART_IO_CLK_DIV_GRP_NUM ((1U << PERI_PCLK_GR_NUM_Pos) | (0U << PERI_PCLK_INST_NUM_Pos))

#if !defined (CYBSP_SMART_IO_CLK_DIV_GRP_NUM)
#define CYBSP_SMART_IO_CLK_DIV_GRP_NUM CYBSP_SMART_IO_CLK_DIV_GRP_NUM
#endif /* !defined (CYBSP_SMART_IO_CLK_DIV_GRP_NUM) */

#define CYBSP_TRACE_CLK_DIV_ENABLED 1U

#if !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE)
#define CYBSP_TRACE_CLK_DIV_HW CY_SYSCLK_DIV_16_5_BIT
#endif /* !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE) */

#define CYBSP_TRACE_CLK_DIV_NUM 0U
#define CYBSP_TRACE_CLK_DIV_GRP_NUM ((7U << PERI_PCLK_GR_NUM_Pos) | (0U << PERI_PCLK_INST_NUM_Pos))

#if !defined (CYBSP_TRACE_CLK_DIV_GRP_NUM)
#define CYBSP_TRACE_CLK_DIV_GRP_NUM CYBSP_TRACE_CLK_DIV_GRP_NUM
#endif /* !defined (CYBSP_TRACE_CLK_DIV_GRP_NUM) */

#define CYBSP_I3C_CONTROLLER_CLK_DIV_ENABLED 1U

#if !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE)
#define CYBSP_I3C_CONTROLLER_CLK_DIV_HW CY_SYSCLK_DIV_8_BIT
#endif /* !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE) */

#define CYBSP_I3C_CONTROLLER_CLK_DIV_NUM 0U
#define CYBSP_I3C_CONTROLLER_CLK_DIV_GRP_NUM ((9U << PERI_PCLK_GR_NUM_Pos) | (0U << PERI_PCLK_INST_NUM_Pos))

#if !defined (CYBSP_I3C_CONTROLLER_CLK_DIV_GRP_NUM)
#define CYBSP_I3C_CONTROLLER_CLK_DIV_GRP_NUM CYBSP_I3C_CONTROLLER_CLK_DIV_GRP_NUM
#endif /* !defined (CYBSP_I3C_CONTROLLER_CLK_DIV_GRP_NUM) */

#define CYBSP_TDM_CONTROLLER_0_CLK_DIV_ENABLED 1U

#if !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE)
#define CYBSP_TDM_CONTROLLER_0_CLK_DIV_HW CY_SYSCLK_DIV_16_5_BIT
#endif /* !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE) */

#define CYBSP_TDM_CONTROLLER_0_CLK_DIV_NUM 0U
#define CYBSP_TDM_CONTROLLER_0_CLK_DIV_GRP_NUM ((1U << PERI_PCLK_GR_NUM_Pos) | (1U << PERI_PCLK_INST_NUM_Pos))

#if !defined (CYBSP_TDM_CONTROLLER_0_CLK_DIV_GRP_NUM)
#define CYBSP_TDM_CONTROLLER_0_CLK_DIV_GRP_NUM CYBSP_TDM_CONTROLLER_0_CLK_DIV_GRP_NUM
#endif /* !defined (CYBSP_TDM_CONTROLLER_0_CLK_DIV_GRP_NUM) */

#define CYBSP_PDM_CLK_DIV_ENABLED 1U

#if !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE)
#define CYBSP_PDM_CLK_DIV_HW CY_SYSCLK_DIV_16_5_BIT
#endif /* !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE) */

#define CYBSP_PDM_CLK_DIV_NUM 1U
#define CYBSP_PDM_CLK_DIV_GRP_NUM ((1U << PERI_PCLK_GR_NUM_Pos) | (1U << PERI_PCLK_INST_NUM_Pos))

#if !defined (CYBSP_PDM_CLK_DIV_GRP_NUM)
#define CYBSP_PDM_CLK_DIV_GRP_NUM CYBSP_PDM_CLK_DIV_GRP_NUM
#endif /* !defined (CYBSP_PDM_CLK_DIV_GRP_NUM) */

#define CYBSP_SDHC_0_CLK_DIV_ENABLED 1U

#if !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE)
#define CYBSP_SDHC_0_CLK_DIV_HW CY_SYSCLK_DIV_8_BIT
#endif /* !defined (CY_USING_HAL) && !defined (CY_USING_HAL_LITE) */

#define CYBSP_SDHC_0_CLK_DIV_NUM 0U
#define CYBSP_SDHC_0_CLK_DIV_GRP_NUM ((2U << PERI_PCLK_GR_NUM_Pos) | (1U << PERI_PCLK_INST_NUM_Pos))

#if !defined (CYBSP_SDHC_0_CLK_DIV_GRP_NUM)
#define CYBSP_SDHC_0_CLK_DIV_GRP_NUM CYBSP_SDHC_0_CLK_DIV_GRP_NUM
#endif /* !defined (CYBSP_SDHC_0_CLK_DIV_GRP_NUM) */

void init_cycfg_peripheral_clocks(void);

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

#endif /* CYCFG_PERIPHERAL_CLOCKS_H */
