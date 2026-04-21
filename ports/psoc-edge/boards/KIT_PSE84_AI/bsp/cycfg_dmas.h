/*******************************************************************************
 * File Name: cycfg_dmas.h
 *
 * Description:
 * DMA configuration
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

#if !defined(CYCFG_DMAS_H)
#define CYCFG_DMAS_H

#include "cycfg_notices.h"
#include "cy_dma.h"
#include "cy_axidmac.h"

#if defined (COMPONENT_MTB_HAL)
#include "mtb_hal.h"
#endif /* defined (COMPONENT_MTB_HAL) */

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

#define CYBSP_DMA_DVP_CAM_CONTROLLER_ENABLED 1U
#define CYBSP_DMA_DVP_CAM_CONTROLLER_HW DW0
#define CYBSP_DMA_DVP_CAM_CONTROLLER_CHANNEL 0U
#define CYBSP_DMA_DVP_CAM_CONTROLLER_IRQ cpuss_interrupts_dw0_0_IRQn
#define CYBSP_DMA_TX_SPI_CONTROLLER_ENABLED 1U
#define CYBSP_DMA_TX_SPI_CONTROLLER_HW DW0
#define CYBSP_DMA_TX_SPI_CONTROLLER_CHANNEL 1U
#define CYBSP_DMA_TX_SPI_CONTROLLER_IRQ cpuss_interrupts_dw0_1_IRQn
#define CYBSP_AXIDMAC_DVP_CAM_CONTROLLER_ENABLED 1U
#define CYBSP_AXIDMAC_DVP_CAM_CONTROLLER_HW SAXI_DMAC
#define CYBSP_AXIDMAC_DVP_CAM_CONTROLLER_CHANNEL 0U
#define CYBSP_AXIDMAC_DVP_CAM_CONTROLLER_IRQ m55appcpuss_interrupts_axidmac_0_IRQn

extern const cy_stc_dma_descriptor_config_t CYBSP_DMA_DVP_CAM_CONTROLLER_Descriptor_0_config;
extern CY_SECTION(".cy_socmem_data") CY_ALIGN(8) cy_stc_dma_descriptor_t CYBSP_DMA_DVP_CAM_CONTROLLER_Descriptor_0;
extern const cy_stc_dma_channel_config_t CYBSP_DMA_DVP_CAM_CONTROLLER_channelConfig;
extern const cy_stc_dma_crc_config_t CYBSP_DMA_DVP_CAM_CONTROLLER_crcConfig;

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_DMA)
extern const mtb_hal_dma_configurator_t CYBSP_DMA_DVP_CAM_CONTROLLER_hal_config;
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_DMA) */

extern const cy_stc_dma_descriptor_config_t CYBSP_DMA_TX_SPI_CONTROLLER_Descriptor_0_config;
extern CY_SECTION(".cy_socmem_data") CY_ALIGN(8) cy_stc_dma_descriptor_t CYBSP_DMA_TX_SPI_CONTROLLER_Descriptor_0;
extern const cy_stc_dma_channel_config_t CYBSP_DMA_TX_SPI_CONTROLLER_channelConfig;
extern const cy_stc_dma_crc_config_t CYBSP_DMA_TX_SPI_CONTROLLER_crcConfig;

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_DMA)
extern const mtb_hal_dma_configurator_t CYBSP_DMA_TX_SPI_CONTROLLER_hal_config;
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_DMA) */

extern const cy_stc_axidmac_descriptor_config_t CYBSP_AXIDMAC_DVP_CAM_CONTROLLER_Descriptor_0_config;
extern CY_SECTION(".cy_socmem_data") CY_ALIGN(8) cy_stc_axidmac_descriptor_t CYBSP_AXIDMAC_DVP_CAM_CONTROLLER_Descriptor_0;
extern const cy_stc_axidmac_channel_config_t CYBSP_AXIDMAC_DVP_CAM_CONTROLLER_channelConfig;

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_DMA)
extern const mtb_hal_dma_configurator_t CYBSP_AXIDMAC_DVP_CAM_CONTROLLER_hal_config;
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_DMA) */

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

#endif /* CYCFG_DMAS_H */
