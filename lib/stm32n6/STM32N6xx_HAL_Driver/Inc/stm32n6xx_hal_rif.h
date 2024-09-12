/**
  ******************************************************************************
  * @file    stm32n6xx_hal_rif.h
  * @author  MCD Application Team
  * @brief   Header file of RIF HAL module.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32N6xx_HAL_RIF_H
#define STM32N6xx_HAL_RIF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @addtogroup RIF
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup RIF_EXPORTED_CONSTANTS_COMMON RIF exported constants - Common
  * @{
  */

/** @defgroup RIF_COMPARTMENT_ID RIF CID number definition
  * @{
  */
#define RIF_CID_NONE                       0x00000000U
#define RIF_CID_0                          0x00000001U
#define RIF_CID_1                          0x00000002U
#define RIF_CID_2                          0x00000004U
#define RIF_CID_3                          0x00000008U
#define RIF_CID_4                          0x00000010U
#define RIF_CID_5                          0x00000020U
#define RIF_CID_6                          0x00000040U
#define RIF_CID_7                          0x00000080U
/**
  * @}
  */

/** @defgroup RIF_MASTER_INDEX RIF Master index
  * @{
  */
#define RIF_MASTER_INDEX_ETR               0U
#if defined(NPU_PRESENT)
#define RIF_MASTER_INDEX_NPU               1U
#endif /* defined(NPU_PRESENT) */
#define RIF_MASTER_INDEX_SDMMC1            2U
#define RIF_MASTER_INDEX_SDMMC2            3U
#define RIF_MASTER_INDEX_OTG1              4U
#define RIF_MASTER_INDEX_OTG2              5U
#define RIF_MASTER_INDEX_ETH1              6U
#define RIF_MASTER_INDEX_GPU2D             7U
#define RIF_MASTER_INDEX_DMA2D             8U
#define RIF_MASTER_INDEX_DCMIPP            9U
#define RIF_MASTER_INDEX_LTDC1             10U
#define RIF_MASTER_INDEX_LTDC2             11U
#define RIF_MASTER_INDEX_VENC              12U
/**
  * @}
  */

/** @defgroup RIF_PERIPHERAL_INDEX RIF RISUP Peripheral index
  * @{
  */
/** @defgroup RIF_PERIPH_PERIPHERAL_INDEX RIF RISUP Peripheral index
  * @{
  */
#define RIF_RISC_PERIPH_INDEX_SPI1         (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC0_Pos)
#define RIF_RISC_PERIPH_INDEX_SPI2         (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC1_Pos)
#define RIF_RISC_PERIPH_INDEX_SPI3         (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC2_Pos)
#define RIF_RISC_PERIPH_INDEX_SPI4         (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC3_Pos)
#define RIF_RISC_PERIPH_INDEX_SPI5         (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC4_Pos)
#define RIF_RISC_PERIPH_INDEX_SPI6         (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC5_Pos)
#define RIF_RISC_PERIPH_INDEX_SAI1         (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC6_Pos)
#define RIF_RISC_PERIPH_INDEX_SAI2         (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC8_Pos)
#define RIF_RISC_PERIPH_INDEX_I2C1         (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC9_Pos)
#define RIF_RISC_PERIPH_INDEX_I2C2         (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC10_Pos)
#define RIF_RISC_PERIPH_INDEX_I2C3         (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC11_Pos)
#define RIF_RISC_PERIPH_INDEX_I2C4         (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC12_Pos)
#define RIF_RISC_PERIPH_INDEX_I3C1         (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC13_Pos)
#define RIF_RISC_PERIPH_INDEX_I3C2         (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC14_Pos)
#define RIF_RISC_PERIPH_INDEX_USART1       (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC15_Pos)
#define RIF_RISC_PERIPH_INDEX_USART2       (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC16_Pos)
#define RIF_RISC_PERIPH_INDEX_USART3       (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC17_Pos)
#define RIF_RISC_PERIPH_INDEX_UART4        (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC18_Pos)
#define RIF_RISC_PERIPH_INDEX_UART5        (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC19_Pos)
#define RIF_RISC_PERIPH_INDEX_USART6       (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC20_Pos)
#define RIF_RISC_PERIPH_INDEX_UART7        (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC21_Pos)
#define RIF_RISC_PERIPH_INDEX_UART8        (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC22_Pos)
#define RIF_RISC_PERIPH_INDEX_UART9        (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC23_Pos)
#define RIF_RISC_PERIPH_INDEX_USART10      (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC24_Pos)
#define RIF_RISC_PERIPH_INDEX_LPUART1      (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC25_Pos)
#define RIF_RISC_PERIPH_INDEX_FDCAN1       (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC26_Pos)
#define RIF_RISC_PERIPH_INDEX_TIM1         (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC27_Pos)
#define RIF_RISC_PERIPH_INDEX_TIM2         (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC28_Pos)
#define RIF_RISC_PERIPH_INDEX_TIM3         (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC29_Pos)
#define RIF_RISC_PERIPH_INDEX_TIM4         (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC30_Pos)
#define RIF_RISC_PERIPH_INDEX_TIM5         (RIF_PERIPH_REG0 | RIFSC_RISC_SECCFGRx_SEC31_Pos)
#define RIF_RISC_PERIPH_INDEX_TIM6         (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC0_Pos)
#define RIF_RISC_PERIPH_INDEX_TIM7         (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC1_Pos)
#define RIF_RISC_PERIPH_INDEX_TIM8         (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC2_Pos)
#define RIF_RISC_PERIPH_INDEX_TIM9         (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC3_Pos)
#define RIF_RISC_PERIPH_INDEX_TIM10        (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC4_Pos)
#define RIF_RISC_PERIPH_INDEX_TIM11        (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC5_Pos)
#define RIF_RISC_PERIPH_INDEX_TIM12        (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC6_Pos)
#define RIF_RISC_PERIPH_INDEX_TIM13        (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC7_Pos)
#define RIF_RISC_PERIPH_INDEX_TIM14        (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC8_Pos)
#define RIF_RISC_PERIPH_INDEX_TIM15        (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC9_Pos)
#define RIF_RISC_PERIPH_INDEX_TIM16        (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC10_Pos)
#define RIF_RISC_PERIPH_INDEX_TIM17        (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC11_Pos)
#define RIF_RISC_PERIPH_INDEX_TIM18        (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC12_Pos)
#define RIF_RISC_PERIPH_INDEX_GFXTIM       (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC13_Pos)
#define RIF_RISC_PERIPH_INDEX_LPTIM1       (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC14_Pos)
#define RIF_RISC_PERIPH_INDEX_LPTIM2       (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC15_Pos)
#define RIF_RISC_PERIPH_INDEX_LPTIM3       (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC16_Pos)
#define RIF_RISC_PERIPH_INDEX_LPTIM4       (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC17_Pos)
#define RIF_RISC_PERIPH_INDEX_LPTIM5       (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC18_Pos)
#define RIF_RISC_PERIPH_INDEX_ADF1         (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC19_Pos)
#define RIF_RISC_PERIPH_INDEX_MDF1         (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC20_Pos)
#define RIF_RISC_PERIPH_INDEX_SDMMC1       (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC21_Pos)
#define RIF_RISC_PERIPH_INDEX_SDMMC2       (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC22_Pos)
#define RIF_RISC_PERIPH_INDEX_MDIOS        (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC23_Pos)
#define RIF_RISC_PERIPH_INDEX_OTG1HS       (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC24_Pos)
#define RIF_RISC_PERIPH_INDEX_OTG2HS       (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC25_Pos)
#define RIF_RISC_PERIPH_INDEX_UCPD1        (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC26_Pos)
#define RIF_RISC_PERIPH_INDEX_ETH1         (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC28_Pos)
#define RIF_RISC_PERIPH_INDEX_SPDIFRX      (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC29_Pos)
#define RIF_RISC_PERIPH_INDEX_SYSCFG       (RIF_PERIPH_REG1 | RIFSC_RISC_SECCFGRx_SEC30_Pos)
#define RIF_RISC_PERIPH_INDEX_ADC12        (RIF_PERIPH_REG2 | RIFSC_RISC_SECCFGRx_SEC0_Pos)
#define RIF_RISC_PERIPH_INDEX_VREFBUF      (RIF_PERIPH_REG2 | RIFSC_RISC_SECCFGRx_SEC1_Pos)
#define RIF_RISC_PERIPH_INDEX_CRC          (RIF_PERIPH_REG2 | RIFSC_RISC_SECCFGRx_SEC3_Pos)
#define RIF_RISC_PERIPH_INDEX_IWDG         (RIF_PERIPH_REG2 | RIFSC_RISC_SECCFGRx_SEC4_Pos)
#define RIF_RISC_PERIPH_INDEX_WWDG         (RIF_PERIPH_REG2 | RIFSC_RISC_SECCFGRx_SEC5_Pos)
#define RIF_RISC_PERIPH_INDEX_RNG          (RIF_PERIPH_REG2 | RIFSC_RISC_SECCFGRx_SEC12_Pos)
#define RIF_RISC_PERIPH_INDEX_PKA          (RIF_PERIPH_REG2 | RIFSC_RISC_SECCFGRx_SEC13_Pos)
#if defined(CRYP)
#define RIF_RISC_PERIPH_INDEX_SAES         (RIF_PERIPH_REG2 | RIFSC_RISC_SECCFGRx_SEC14_Pos)
#endif /* defined(CRYP) */
#define RIF_RISC_PERIPH_INDEX_HASH         (RIF_PERIPH_REG2 | RIFSC_RISC_SECCFGRx_SEC15_Pos)
#if defined(CRYP)
#define RIF_RISC_PERIPH_INDEX_CRYP         (RIF_PERIPH_REG2 | RIFSC_RISC_SECCFGRx_SEC16_Pos)
#define RIF_RISC_PERIPH_INDEX_MCE1         (RIF_PERIPH_REG2 | RIFSC_RISC_SECCFGRx_SEC17_Pos)
#define RIF_RISC_PERIPH_INDEX_MCE2         (RIF_PERIPH_REG2 | RIFSC_RISC_SECCFGRx_SEC18_Pos)
#define RIF_RISC_PERIPH_INDEX_MCE3         (RIF_PERIPH_REG2 | RIFSC_RISC_SECCFGRx_SEC19_Pos)
#define RIF_RISC_PERIPH_INDEX_MCE4         (RIF_PERIPH_REG2 | RIFSC_RISC_SECCFGRx_SEC20_Pos)
#endif /* defined(CRYP) */
#define RIF_RISC_PERIPH_INDEX_XSPI1        (RIF_PERIPH_REG2 | RIFSC_RISC_SECCFGRx_SEC22_Pos)
#define RIF_RISC_PERIPH_INDEX_XSPI2        (RIF_PERIPH_REG2 | RIFSC_RISC_SECCFGRx_SEC23_Pos)
#define RIF_RISC_PERIPH_INDEX_XSPI3        (RIF_PERIPH_REG2 | RIFSC_RISC_SECCFGRx_SEC24_Pos)
#define RIF_RISC_PERIPH_INDEX_XSPIM        (RIF_PERIPH_REG2 | RIFSC_RISC_SECCFGRx_SEC25_Pos)
#define RIF_RISC_PERIPH_INDEX_FMC          (RIF_PERIPH_REG2 | RIFSC_RISC_SECCFGRx_SEC26_Pos)
#define RIF_RISC_PERIPH_INDEX_CSI          (RIF_PERIPH_REG2 | RIFSC_RISC_SECCFGRx_SEC28_Pos)
#define RIF_RISC_PERIPH_INDEX_DCMIPP       (RIF_PERIPH_REG2 | RIFSC_RISC_SECCFGRx_SEC29_Pos)
#define RIF_RISC_PERIPH_INDEX_DCMI         (RIF_PERIPH_REG2 | RIFSC_RISC_SECCFGRx_SEC30_Pos)
#define RIF_RISC_PERIPH_INDEX_JPEG         (RIF_PERIPH_REG3 | RIFSC_RISC_SECCFGRx_SEC0_Pos)
#define RIF_RISC_PERIPH_INDEX_VENC         (RIF_PERIPH_REG3 | RIFSC_RISC_SECCFGRx_SEC1_Pos)
#define RIF_RISC_PERIPH_INDEX_ICACHE       (RIF_PERIPH_REG3 | RIFSC_RISC_SECCFGRx_SEC2_Pos)
#define RIF_RISC_PERIPH_INDEX_GPU2D        (RIF_PERIPH_REG3 | RIFSC_RISC_SECCFGRx_SEC3_Pos)
#define RIF_RISC_PERIPH_INDEX_GFXMMU       (RIF_PERIPH_REG3 | RIFSC_RISC_SECCFGRx_SEC4_Pos)
#define RIF_RISC_PERIPH_INDEX_DMA2D        (RIF_PERIPH_REG3 | RIFSC_RISC_SECCFGRx_SEC5_Pos)
#define RIF_RISC_PERIPH_INDEX_LTDC         (RIF_PERIPH_REG3 | RIFSC_RISC_SECCFGRx_SEC6_Pos)
#define RIF_RISC_PERIPH_INDEX_LTDCL1       (RIF_PERIPH_REG3 | RIFSC_RISC_SECCFGRx_SEC7_Pos)
#define RIF_RISC_PERIPH_INDEX_LTDCL2       (RIF_PERIPH_REG3 | RIFSC_RISC_SECCFGRx_SEC8_Pos)
#if defined(NPU_PRESENT)
#define RIF_RISC_PERIPH_INDEX_NPU          (RIF_PERIPH_REG3 | RIFSC_RISC_SECCFGRx_SEC10_Pos)
#endif /* defined(NPU_PRESENT) */
/**
  * @}
  */

/** @defgroup RIF_RCC_PERIPHERAL_INDEX RIF RCC Security Control Peripheral index
  * @{
  */
#define RIF_RCC_PERIPH_INDEX_GPDMA1        (RIF_PERIPH_REG4 | RIFSC_RISC_SECCFGRx_SEC2_Pos)
#define RIF_RCC_PERIPH_INDEX_HPDMA1        (RIF_PERIPH_REG4 | RIFSC_RISC_SECCFGRx_SEC3_Pos)
#define RIF_RCC_PERIPH_INDEX_RTC           (RIF_PERIPH_REG4 | RIFSC_RISC_SECCFGRx_SEC5_Pos)
#define RIF_RCC_PERIPH_INDEX_AXISRAM1      (RIF_PERIPH_REG4 | RIFSC_RISC_SECCFGRx_SEC12_Pos)
#define RIF_RCC_PERIPH_INDEX_AXISRAM2      (RIF_PERIPH_REG4 | RIFSC_RISC_SECCFGRx_SEC13_Pos)
#define RIF_RCC_PERIPH_INDEX_FLEXRAM       (RIF_PERIPH_REG4 | RIFSC_RISC_SECCFGRx_SEC17_Pos)
#define RIF_RCC_PERIPH_INDEX_CACHEAXIRAM   (RIF_PERIPH_REG4 | RIFSC_RISC_SECCFGRx_SEC18_Pos)
#define RIF_RCC_PERIPH_INDEX_VENCRAM       (RIF_PERIPH_REG4 | RIFSC_RISC_SECCFGRx_SEC19_Pos)
#if defined(NPU_PRESENT)
#define RIF_RCC_PERIPH_INDEX_CACHECONFIG   (RIF_PERIPH_REG4 | RIFSC_RISC_SECCFGRx_SEC25_Pos)
#endif /* defined(NPU_PRESENT) */
#define RIF_RCC_PERIPH_INDEX_AHBRAM1       (RIF_PERIPH_REG4 | RIFSC_RISC_SECCFGRx_SEC27_Pos)
#define RIF_RCC_PERIPH_INDEX_AHBRAM2       (RIF_PERIPH_REG4 | RIFSC_RISC_SECCFGRx_SEC28_Pos)
#define RIF_RCC_PERIPH_INDEX_BKPRAM        (RIF_PERIPH_REG4 | RIFSC_RISC_SECCFGRx_SEC29_Pos)
#define RIF_RCC_PERIPH_INDEX_GPIOA         (RIF_PERIPH_REG4 | RIFSC_RISC_SECCFGRx_SEC31_Pos)
#define RIF_RCC_PERIPH_INDEX_GPIOB         (RIF_PERIPH_REG5 | RIFSC_RISC_SECCFGRx_SEC0_Pos)
#define RIF_RCC_PERIPH_INDEX_GPIOC         (RIF_PERIPH_REG5 | RIFSC_RISC_SECCFGRx_SEC1_Pos)
#define RIF_RCC_PERIPH_INDEX_GPIOD         (RIF_PERIPH_REG5 | RIFSC_RISC_SECCFGRx_SEC2_Pos)
#define RIF_RCC_PERIPH_INDEX_GPIOE         (RIF_PERIPH_REG5 | RIFSC_RISC_SECCFGRx_SEC3_Pos)
#define RIF_RCC_PERIPH_INDEX_GPIOF         (RIF_PERIPH_REG5 | RIFSC_RISC_SECCFGRx_SEC4_Pos)
#define RIF_RCC_PERIPH_INDEX_GPIOG         (RIF_PERIPH_REG5 | RIFSC_RISC_SECCFGRx_SEC5_Pos)
#define RIF_RCC_PERIPH_INDEX_GPIOH         (RIF_PERIPH_REG5 | RIFSC_RISC_SECCFGRx_SEC6_Pos)
#define RIF_RCC_PERIPH_INDEX_GPION         (RIF_PERIPH_REG5 | RIFSC_RISC_SECCFGRx_SEC8_Pos)
#define RIF_RCC_PERIPH_INDEX_GPIOO         (RIF_PERIPH_REG5 | RIFSC_RISC_SECCFGRx_SEC9_Pos)
#define RIF_RCC_PERIPH_INDEX_GPIOP         (RIF_PERIPH_REG5 | RIFSC_RISC_SECCFGRx_SEC10_Pos)
#define RIF_RCC_PERIPH_INDEX_GPIOQ         (RIF_PERIPH_REG5 | RIFSC_RISC_SECCFGRx_SEC11_Pos)
#define RIF_RCC_PERIPH_INDEX_DTS           (RIF_PERIPH_REG5 | RIFSC_RISC_SECCFGRx_SEC13_Pos)
#define RIF_RCC_PERIPH_INDEX_MCO1          (RIF_PERIPH_REG5 | RIFSC_RISC_SECCFGRx_SEC14_Pos)
#define RIF_RCC_PERIPH_INDEX_MCO2          (RIF_PERIPH_REG5 | RIFSC_RISC_SECCFGRx_SEC15_Pos)
#if defined(NPU_PRESENT)
#define RIF_RCC_PERIPH_INDEX_NPURAM0       (RIF_PERIPH_REG5 | RIFSC_RISC_SECCFGRx_SEC17_Pos)
#define RIF_RCC_PERIPH_INDEX_NPURAM1       (RIF_PERIPH_REG5 | RIFSC_RISC_SECCFGRx_SEC18_Pos)
#define RIF_RCC_PERIPH_INDEX_NPURAM2       (RIF_PERIPH_REG5 | RIFSC_RISC_SECCFGRx_SEC19_Pos)
#define RIF_RCC_PERIPH_INDEX_NPURAM3       (RIF_PERIPH_REG5 | RIFSC_RISC_SECCFGRx_SEC20_Pos)
#endif /* defined(NPU_PRESENT) */
#define RIF_RCC_PERIPH_INDEX_XSPIPHYCOMP   (RIF_PERIPH_REG5 | RIFSC_RISC_SECCFGRx_SEC22_Pos)
#define RIF_RCC_PERIPH_INDEX_XSPIPHY1      (RIF_PERIPH_REG5 | RIFSC_RISC_SECCFGRx_SEC23_Pos)
#define RIF_RCC_PERIPH_INDEX_XSPIPHY2      (RIF_PERIPH_REG5 | RIFSC_RISC_SECCFGRx_SEC24_Pos)
#define RIF_RCC_PERIPH_INDEX_HDP           (RIF_PERIPH_REG5 | RIFSC_RISC_SECCFGRx_SEC27_Pos)
#define RIF_RCC_PERIPH_INDEX_RAMCFG        (RIF_PERIPH_REG5 | RIFSC_RISC_SECCFGRx_SEC30_Pos)
/**
  * @}
  */

/** @defgroup RIF_AWARE_PERIPHERAL_INDEX RIF-aware Peripheral index
  * @{
  */
#define RIF_AWARE_PERIPH_INDEX_CM55        (RIF_PERIPH_REG4 | IAC_IERx_IAIE0_Pos)
#define RIF_AWARE_PERIPH_INDEX_EXTI        (RIF_PERIPH_REG4 | IAC_IERx_IAIE1_Pos)
#define RIF_AWARE_PERIPH_INDEX_GPDMA1      RIF_RCC_PERIPH_INDEX_GPDMA1
#define RIF_AWARE_PERIPH_INDEX_HPDMA1      RIF_RCC_PERIPH_INDEX_HPDMA1
#define RIF_AWARE_PERIPH_INDEX_RTC         RIF_RCC_PERIPH_INDEX_RTC
#define RIF_AWARE_PERIPH_INDEX_TAMP        (RIF_PERIPH_REG4 | IAC_IERx_IAIE6_Pos)
#define RIF_AWARE_PERIPH_INDEX_BSEC        (RIF_PERIPH_REG4 | IAC_IERx_IAIE7_Pos)
#define RIF_AWARE_PERIPH_INDEX_RCC         (RIF_PERIPH_REG4 | IAC_IERx_IAIE8_Pos)
#define RIF_AWARE_PERIPH_INDEX_PWR         (RIF_PERIPH_REG4 | IAC_IERx_IAIE9_Pos)
#define RIF_AWARE_PERIPH_INDEX_IAC         (RIF_PERIPH_REG4 | IAC_IERx_IAIE10_Pos)
#define RIF_AWARE_PERIPH_INDEX_RISAF1      (RIF_PERIPH_REG4 | IAC_IERx_IAIE11_Pos)
#define RIF_AWARE_PERIPH_INDEX_RISAF2      RIF_RCC_PERIPH_INDEX_AXISRAM1
#define RIF_AWARE_PERIPH_INDEX_RISAF3      RIF_RCC_PERIPH_INDEX_AXISRAM2
#if defined(NPU_PRESENT)
#define RIF_AWARE_PERIPH_INDEX_RISAF4      (RIF_PERIPH_REG4 | IAC_IERx_IAIE14_Pos)
#define RIF_AWARE_PERIPH_INDEX_RISAF5      (RIF_PERIPH_REG4 | IAC_IERx_IAIE15_Pos)
#endif /* if defined(NPU_PRESENT) */
#define RIF_AWARE_PERIPH_INDEX_RISAF6      (RIF_PERIPH_REG4 | IAC_IERx_IAIE16_Pos)
#define RIF_AWARE_PERIPH_INDEX_RISAF7      RIF_RCC_PERIPH_INDEX_FLEXRAM
#define RIF_AWARE_PERIPH_INDEX_RISAF8      RIF_RCC_PERIPH_INDEX_CACHEAXIRAM
#define RIF_AWARE_PERIPH_INDEX_RISAF9      RIF_RCC_PERIPH_INDEX_VENCRAM
#define RIF_AWARE_PERIPH_INDEX_RISAF11     (RIF_PERIPH_REG4 | IAC_IERx_IAIE21_Pos)
#define RIF_AWARE_PERIPH_INDEX_RISAF12     (RIF_PERIPH_REG4 | IAC_IERx_IAIE22_Pos)
#define RIF_AWARE_PERIPH_INDEX_RISAF13     (RIF_PERIPH_REG4 | IAC_IERx_IAIE23_Pos)
#define RIF_AWARE_PERIPH_INDEX_RISAF14     (RIF_PERIPH_REG4 | IAC_IERx_IAIE24_Pos)
#if defined(NPU_PRESENT)
#define RIF_AWARE_PERIPH_INDEX_RISAF15     RIF_RCC_PERIPH_INDEX_CACHECONFIG
#endif /* if defined(NPU_PRESENT) */
#define RIF_AWARE_PERIPH_INDEX_RISAF21     RIF_RCC_PERIPH_INDEX_AHBRAM1
#define RIF_AWARE_PERIPH_INDEX_RISAF22     RIF_RCC_PERIPH_INDEX_AHBRAM2
#define RIF_AWARE_PERIPH_INDEX_RISAF23     RIF_RCC_PERIPH_INDEX_BKPRAM
#define RIF_AWARE_PERIPH_INDEX_RIFSC       (RIF_PERIPH_REG4 | IAC_IERx_IAIE30_Pos)
/**
  * @}
  */
/**
  * @}
  */

/** @defgroup RIF_SEC_PRIV RIF Secure Privileged attributes definitions
  * @{
  */
#define RIF_ATTRIBUTE_NSEC                 0x00000000U
#define RIF_ATTRIBUTE_SEC                  0x00000001U
#define RIF_ATTRIBUTE_NPRIV                0x00000000U
#define RIF_ATTRIBUTE_PRIV                 0x00000002U
/**
  * @}
  */

/** @defgroup RIF_ACCESS_TYPE RIF Access type definitions
  * @{
  */
#define RIF_ACCTYPE_READ_FETCH             0U
#define RIF_ACCTYPE_WRITE                  1U
/**
  * @}
  */

/** @defgroup RIF_LOCK_STATE RIF Lock states definitions
  * @{
  */
#define RIF_LOCK_DISABLE                   0U
#define RIF_LOCK_ENABLE                    RIFSC_RIMC_CR_GLOCK
/**
  * @}
  */

/**
  * @}
  */


/** @defgroup RIF_EXPORTED_CONSTANTS_RISAF RIF Exported Constants - RISAF
  * @{
  */

/** @defgroup RIF_RISAF_REGION RISAF Base region definitions
  * @{
  */
#define RISAF_REGION_1                     0U
#define RISAF_REGION_2                     1U
#define RISAF_REGION_3                     2U
#define RISAF_REGION_4                     3U
#define RISAF_REGION_5                     4U
#define RISAF_REGION_6                     5U
#define RISAF_REGION_7                     6U
#define RISAF_REGION_8                     7U
#define RISAF_REGION_9                     8U
#define RISAF_REGION_10                    9U
#define RISAF_REGION_11                    10U
#define RISAF_REGION_12                    11U
#define RISAF_REGION_13                    12U
#define RISAF_REGION_14                    13U
#define RISAF_REGION_15                    14U
/**
  * @}
  */

/** @defgroup RIF_RISAF_GRANULARITY RISAF granularity definitions
  * @{
  */
#if !defined(NPU_PRESENT)
#define RISAF1_GRANULARITY                 0x1000U
#define RISAF2_GRANULARITY                 0x1000U
#define RISAF3_GRANULARITY                 0x1000U
#define RISAF6_GRANULARITY                 0x1000U
#define RISAF7_GRANULARITY                 0x1000U
#define RISAF8_GRANULARITY                 0x1000U
#define RISAF9_GRANULARITY                 0x1000U
#define RISAF11_GRANULARITY                0x1000U
#define RISAF12_GRANULARITY                0x1000U
#define RISAF13_GRANULARITY                0x1000U
#define RISAF14_GRANULARITY                0x1000U
#define RISAF21_GRANULARITY                0x0200U
#define RISAF22_GRANULARITY                0x0200U
#define RISAF23_GRANULARITY                0x0200U
#else
#define RISAF1_GRANULARITY                 0x1000U
#define RISAF2_GRANULARITY                 0x1000U
#define RISAF3_GRANULARITY                 0x1000U
#define RISAF4_GRANULARITY                 0x1000U
#define RISAF5_GRANULARITY                 0x1000U
#define RISAF6_GRANULARITY                 0x1000U
#define RISAF7_GRANULARITY                 0x1000U
#define RISAF8_GRANULARITY                 0x1000U
#define RISAF9_GRANULARITY                 0x1000U
#define RISAF11_GRANULARITY                0x1000U
#define RISAF12_GRANULARITY                0x1000U
#define RISAF13_GRANULARITY                0x1000U
#define RISAF14_GRANULARITY                0x1000U
#define RISAF15_GRANULARITY                0x0004U
#define RISAF21_GRANULARITY                0x0200U
#define RISAF22_GRANULARITY                0x0200U
#define RISAF23_GRANULARITY                0x0200U
#endif /* !defined(NPU_PRESENT) */
/**
  * @}
  */

/** @defgroup RIF_RISAF_ADDSPACE RISAF address space sizes definitions
  * @{
  */
#if !defined(NPU_PRESENT)
#define RISAF1_LIMIT_ADDRESS_SPACE_SIZE         0x3FFFFFFFU
#define RISAF2_LIMIT_ADDRESS_SPACE_SIZE         0x000FFFFFU
#define RISAF3_LIMIT_ADDRESS_SPACE_SIZE         0x000FFFFFU
#define RISAF6_LIMIT_ADDRESS_SPACE_SIZE         0xFFFFFFFFU
#define RISAF7_LIMIT_ADDRESS_SPACE_SIZE         0x0007FFFFU
#define RISAF8_LIMIT_ADDRESS_SPACE_SIZE         0x0003FFFFU
#define RISAF9_LIMIT_ADDRESS_SPACE_SIZE         0x0001FFFFU
#define RISAF11_LIMIT_ADDRESS_SPACE_SIZE        0x0FFFFFFFU
#define RISAF12_LIMIT_ADDRESS_SPACE_SIZE        0x0FFFFFFFU
#define RISAF13_LIMIT_ADDRESS_SPACE_SIZE        0x0FFFFFFFU
#define RISAF14_LIMIT_ADDRESS_SPACE_SIZE        0x0FFFFFFFU
#define RISAF21_LIMIT_ADDRESS_SPACE_SIZE        0x00001FFFU
#define RISAF22_LIMIT_ADDRESS_SPACE_SIZE        0x00001FFFU
#define RISAF23_LIMIT_ADDRESS_SPACE_SIZE        0x00000FFFU
#else
#define RISAF1_LIMIT_ADDRESS_SPACE_SIZE         0x3FFFFFFFU
#define RISAF2_LIMIT_ADDRESS_SPACE_SIZE         0x000FFFFFU
#define RISAF3_LIMIT_ADDRESS_SPACE_SIZE         0x000FFFFFU
#define RISAF4_LIMIT_ADDRESS_SPACE_SIZE         0xFFFFFFFFU
#define RISAF5_LIMIT_ADDRESS_SPACE_SIZE         0xFFFFFFFFU
#define RISAF6_LIMIT_ADDRESS_SPACE_SIZE         0xFFFFFFFFU
#define RISAF7_LIMIT_ADDRESS_SPACE_SIZE         0x0007FFFFU
#define RISAF8_LIMIT_ADDRESS_SPACE_SIZE         0x0003FFFFU
#define RISAF9_LIMIT_ADDRESS_SPACE_SIZE         0x0001FFFFU
#define RISAF11_LIMIT_ADDRESS_SPACE_SIZE        0x0FFFFFFFU
#define RISAF12_LIMIT_ADDRESS_SPACE_SIZE        0x0FFFFFFFU
#define RISAF13_LIMIT_ADDRESS_SPACE_SIZE        0x0FFFFFFFU
#define RISAF14_LIMIT_ADDRESS_SPACE_SIZE        0x0FFFFFFFU
#define RISAF15_LIMIT_ADDRESS_SPACE_SIZE        0x00000FFFU
#define RISAF21_LIMIT_ADDRESS_SPACE_SIZE        0x00001FFFU
#define RISAF22_LIMIT_ADDRESS_SPACE_SIZE        0x00001FFFU
#define RISAF23_LIMIT_ADDRESS_SPACE_SIZE        0x00000FFFU
#endif /* !defined(NPU_PRESENT) */
/**
  * @}
  */

/** @defgroup RIF_RISAF_SUBREGION RIF subregion definitions
  * @{
  */
#define RISAF_SUBREGION_A                  0U
#define RISAF_SUBREGION_B                  1U
/**
  * @}
  */

/** @defgroup RIF_RISAF_FILTERING_MODE RISAF Filtering mode definitions
  * @{
  */
#define RISAF_FILTER_DISABLE               0U
#define RISAF_FILTER_ENABLE                RISAF_REGx_CFGR_BREN
/**
  * @}
  */

/** @defgroup RIF_RISAF_DELEGATION_MODE RISAF Delegation mode definitions
  * @{
  */
#define RISAF_DELEGATION_DISABLE           0U
#define RISAF_DELEGATION_ENABLE            RISAF_REGx_zNESTR_DCEN
/**
  * @}
  */

/** @defgroup RIF_RISAF_READ_WRITE RISAF Read/Write definitions
  * @{
  */
#define RISAF_READ_DISABLE                 0U
#define RISAF_READ_ENABLE                  RISAF_REGx_zCFGR_RDEN
#define RISAF_WRITE_DISABLE                0U
#define RISAF_WRITE_ENABLE                 RISAF_REGx_zCFGR_WREN
/**
  * @}
  */

/** @defgroup RIF_RISAF_ILLEGAL_ACCESS RISAF Illegal Access definitions
  * @{
  */
#define RISAF_ILLEGAL_ACCESS_NONE          0U
#define RISAF_ILLEGAL_CONFIGURATION_ACCESS RISAF_IASR_CAEF
#define RISAF_ILLEGAL_ACCESS               RISAF_IASR_IAEF
/**
  * @}
  */

/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup RIF_Exported_Types RIF Exported Types
  * @{
  */

/**
  * @brief  RIFSC Master CID attributes configuration structure
  */
typedef struct
{
  uint32_t MasterCID; /*!< One of @ref RIF_COMPARTMENT_ID        */
  uint32_t SecPriv;   /*!< A combination of @ref RIF_SEC_PRIV    */
} RIMC_MasterConfig_t;

/**
  * @brief  RISAF base region configuration structure
  */
typedef struct
{
  uint32_t Filtering;       /*!< One of @ref RIF_RISAF_FILTERING_MODE      */
  uint32_t Secure;          /*!< One of @ref RIF_SEC_PRIV                  */
  uint32_t PrivWhitelist;   /*!< A combination of @ref RIF_COMPARTMENT_ID  */
  uint32_t ReadWhitelist;   /*!< A combination of @ref RIF_COMPARTMENT_ID  */
  uint32_t WriteWhitelist;  /*!< A combination of @ref RIF_COMPARTMENT_ID  */
  uint32_t StartAddress;    /*!< Base region start, address offset to the base address of the protected address space */
  uint32_t EndAddress;      /*!< Base region end, address offset to the base address of the protected address space   */
} RISAF_BaseRegionConfig_t;

/**
  * @brief  RISAF subregion configuration structure
  */
typedef struct
{
  uint32_t Filtering;    /*!< One of @ref RIF_RISAF_FILTERING_MODE           */
  uint32_t CID;          /*!< One of @ref RIF_COMPARTMENT_ID                 */
  uint32_t SecPriv;      /*!< A combination of @ref RIF_SEC_PRIV             */
  uint32_t ReadWrite;    /*!< A combination of @ref RIF_RISAF_READ_WRITE     */
  uint32_t Lock;         /*!< One of @ref RIF_LOCK_STATE                     */
  uint32_t StartAddress; /*!< Subregion start, address offset to the base address of the protected address space */
  uint32_t EndAddress;   /*!< Subregion end, address offset to the base address of the protected address space   */
} RISAF_SubRegionConfig_t;

/**
  * @brief  RISAF delegation structure
  */
typedef struct
{
  uint32_t Delegation;   /*!< Enable or Disable               */
  uint32_t DelegatedCID; /*!< One of @ref RIF_COMPARTMENT_ID  */
} RISAF_DelegationConfig_t;

/**
  * @brief  RISAF illegal access detection structure
  */
/**
  * @brief  RISAF illegal access detection data structure
  */
typedef struct
{
  uint32_t CID;        /*!< One of @ref RIF_COMPARTMENT_ID      */
  uint32_t SecPriv;    /*!< A combination of @ref RIF_SEC_PRIV  */
  uint32_t AccessType; /*!< One of @ref RIF_ACCESS_TYPE         */
  uint32_t Address;
} RISAF_IllegalAccessData_t;

typedef struct
{
  uint32_t ErrorType; /*!< One of @ref RIF_RISAF_ILLEGAL_ACCESS      */
  RISAF_IllegalAccessData_t Data;
} RISAF_IllegalAccess_t;

/**
  * @}
  */

/* Private constants ---------------------------------------------------------*/
/** @defgroup RCC_Private_Constants RCC Private Constants
  * @{
  */

/** @defgroup RIF_REG_INDEX RIF register index definition
  * @{
  */
/* Composition definition for Peripheral identifier parameter (PeriphId) used in
 * RIF RISC and IAC related functions.
 * Bitmap Definition
  *     bits[31:28] define the register index a peripheral belongs to, value from 0 to 5.
  *     bits[4:0]   define the bit position within the register, value from 0 to 31.
 */
#define RIF_PERIPH_REG_SHIFT               28U
#define RIF_PERIPH_REG                     0xF0000000U
#define RIF_PERIPH_REG0                    0x00000000U
#define RIF_PERIPH_REG1                    0x10000000U
#define RIF_PERIPH_REG2                    0x20000000U
#define RIF_PERIPH_REG3                    0x30000000U
#define RIF_PERIPH_REG4                    0x40000000U
#define RIF_PERIPH_REG5                    0x50000000U
#define RIF_PERIPH_BIT_POSITION            0x0000001FU
/**
  * @}
  */

/** @defgroup RIF_MASK RIF register masks
  * @{
  */
#define RIF_CID_MASK                       0x000000FFU
#define RIF_ATTRIBUTE_MASK                 0x00000003U
#define RISAF_READ_WRITE_MASK              (RISAF_READ_ENABLE | RISAF_WRITE_ENABLE)
/**
  * @}
  */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup RIF_Private_Macros_Common RIF Private Macros - Common
  * @{
  */
#define IS_RIF_CID(__CID__)  (((uint32_t)(__CID__) & ~RIF_CID_MASK) == 0x00U)

#define IS_RIF_SINGLE_CID(__CID__)   (((__CID__) == RIF_CID_0)  || \
                                      ((__CID__) == RIF_CID_1)  || \
                                      ((__CID__) == RIF_CID_2)  || \
                                      ((__CID__) == RIF_CID_3)  || \
                                      ((__CID__) == RIF_CID_4)  || \
                                      ((__CID__) == RIF_CID_5)  || \
                                      ((__CID__) == RIF_CID_6)  || \
                                      ((__CID__) == RIF_CID_7))

#define IS_RIF_MASTER_CID(__CID__)  (((uint32_t)(__CID__) != RIF_CID_7) && \
                                     (((uint32_t)(__CID__) & ~RIF_CID_MASK) == 0x00U))


#if defined(NPU_PRESENT)
#define IS_RIF_MASTER_INDEX(__INDEX__)  (((__INDEX__) == RIF_MASTER_INDEX_ETR)    || \
                                         ((__INDEX__) == RIF_MASTER_INDEX_NPU)    || \
                                         ((__INDEX__) == RIF_MASTER_INDEX_SDMMC1) || \
                                         ((__INDEX__) == RIF_MASTER_INDEX_SDMMC2) || \
                                         ((__INDEX__) == RIF_MASTER_INDEX_OTG1)   || \
                                         ((__INDEX__) == RIF_MASTER_INDEX_OTG2)   || \
                                         ((__INDEX__) == RIF_MASTER_INDEX_ETH1)   || \
                                         ((__INDEX__) == RIF_MASTER_INDEX_GPU2D)  || \
                                         ((__INDEX__) == RIF_MASTER_INDEX_DMA2D)  || \
                                         ((__INDEX__) == RIF_MASTER_INDEX_DCMIPP) || \
                                         ((__INDEX__) == RIF_MASTER_INDEX_LTDC1)  || \
                                         ((__INDEX__) == RIF_MASTER_INDEX_LTDC2)  || \
                                         ((__INDEX__) == RIF_MASTER_INDEX_VENC))
#else
#define IS_RIF_MASTER_INDEX(__INDEX__)  (((__INDEX__) == RIF_MASTER_INDEX_ETR)    || \
                                         ((__INDEX__) == RIF_MASTER_INDEX_SDMMC1) || \
                                         ((__INDEX__) == RIF_MASTER_INDEX_SDMMC2) || \
                                         ((__INDEX__) == RIF_MASTER_INDEX_OTG1)   || \
                                         ((__INDEX__) == RIF_MASTER_INDEX_OTG2)   || \
                                         ((__INDEX__) == RIF_MASTER_INDEX_ETH1)   || \
                                         ((__INDEX__) == RIF_MASTER_INDEX_GPU2D)  || \
                                         ((__INDEX__) == RIF_MASTER_INDEX_DMA2D)  || \
                                         ((__INDEX__) == RIF_MASTER_INDEX_DCMIPP) || \
                                         ((__INDEX__) == RIF_MASTER_INDEX_LTDC1)  || \
                                         ((__INDEX__) == RIF_MASTER_INDEX_LTDC2)  || \
                                         ((__INDEX__) == RIF_MASTER_INDEX_VENC))
#endif /* defined(NPU_PRESENT) */

#if !defined(NPU_PRESENT) && !defined(CRYP)
#define IS_RIF_RISC_PERIPH_INDEX(__INDEX__)  (((__INDEX__) == RIF_RISC_PERIPH_INDEX_SPI1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SPI2)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SPI3)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SPI4)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SPI5)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SPI6)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SAI1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SAI2)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_I2C1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_I2C2)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_I2C3)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_I2C4)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_I3C1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_I3C2)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_USART1)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_USART2)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_USART3)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_UART4)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_UART5)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_USART6)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_UART7)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_UART8)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_UART9)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_USART10)       || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LPUART1)       || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_FDCAN1)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM2)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM3)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM4)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM5)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM6)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM7)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM8)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM9)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM10)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM11)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM12)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM13)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM14)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM15)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM16)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM17)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM18)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_GFXTIM)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LPTIM1)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LPTIM2)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LPTIM3)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LPTIM4)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LPTIM5)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_ADF1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_MDF1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SDMMC1)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SDMMC2)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_MDIOS)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_OTG1HS)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_OTG2HS)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_UCPD1)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_ETH1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SPDIFRX)       || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SYSCFG)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_ADC12)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_VREFBUF)       || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_CRC)           || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_IWDG)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_WWDG)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_RNG)           || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_PKA)           || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_HASH)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_XSPI1)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_XSPI2)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_XSPI3)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_XSPIM)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_FMC)           || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_CSI)           || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_DCMIPP)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_DCMI)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_JPEG)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_VENC)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_ICACHE)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_GPU2D)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_GFXMMU)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_DMA2D)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LTDC)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LTDCL1)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LTDCL2))

#define IS_RIF_RCC_PERIPH_INDEX(__INDEX__)   (((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPDMA1)         || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_HPDMA1)         || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_RTC)            || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_AXISRAM1)       || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_AXISRAM2)       || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_FLEXRAM)        || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_CACHEAXIRAM)    || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_VENCRAM)        || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_AHBRAM1)        || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_AHBRAM2)        || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_BKPRAM)         || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOA)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOB)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOC)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOD)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOE)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOF)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOG)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOH)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPION)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOO)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOP)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOQ)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_DTS)            || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_MCO1)           || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_MCO2)           || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_XSPIPHYCOMP)    || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_XSPIPHY1)       || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_XSPIPHY2)       || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_HDP)            || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_RAMCFG))

#define IS_RIF_AWARE_PERIPH_INDEX(__INDEX__) (((__INDEX__) == RIF_AWARE_PERIPH_INDEX_CM55)         || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_EXTI)         || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_GPDMA1)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_HPDMA1)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RTC)          || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_TAMP)         || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_BSEC)         || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RCC)          || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_PWR)          || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_IAC)          || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF1)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF2)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF3)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF6)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF7)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF8)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF9)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF11)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF12)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF13)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF14)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF21)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF22)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF23)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RIFSC))
#endif /* !defined(NPU_PRESENT) && !defined(CRYP) */

#if defined(NPU_PRESENT) && !defined(CRYP)
#define IS_RIF_RISC_PERIPH_INDEX(__INDEX__)  (((__INDEX__) == RIF_RISC_PERIPH_INDEX_SPI1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SPI2)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SPI3)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SPI4)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SPI5)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SPI6)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SAI1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SAI2)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_I2C1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_I2C2)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_I2C3)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_I2C4)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_I3C1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_I3C2)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_USART1)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_USART2)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_USART3)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_UART4)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_UART5)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_USART6)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_UART7)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_UART8)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_UART9)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_USART10)       || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LPUART1)       || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_FDCAN1)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM2)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM3)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM4)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM5)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM6)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM7)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM8)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM9)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM10)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM11)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM12)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM13)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM14)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM15)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM16)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM17)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM18)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_GFXTIM)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LPTIM1)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LPTIM2)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LPTIM3)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LPTIM4)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LPTIM5)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_ADF1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_MDF1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SDMMC1)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SDMMC2)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_MDIOS)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_OTG1HS)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_OTG2HS)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_UCPD1)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_ETH1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SPDIFRX)       || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SYSCFG)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_ADC12)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_VREFBUF)       || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_CRC)           || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_IWDG)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_WWDG)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_RNG)           || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_PKA)           || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_HASH)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_XSPI1)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_XSPI2)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_XSPI3)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_XSPIM)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_FMC)           || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_CSI)           || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_DCMIPP)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_DCMI)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_JPEG)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_VENC)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_ICACHE)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_GPU2D)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_GFXMMU)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_DMA2D)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LTDC)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LTDCL1)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LTDCL2)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_NPU))

#define IS_RIF_RCC_PERIPH_INDEX(__INDEX__)   (((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPDMA1)         || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_HPDMA1)         || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_RTC)            || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_AXISRAM1)       || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_AXISRAM2)       || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_FLEXRAM)        || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_CACHEAXIRAM)    || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_VENCRAM)        || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_CACHECONFIG)    || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_AHBRAM1)        || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_AHBRAM2)        || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_BKPRAM)         || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOA)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOB)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOC)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOD)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOE)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOF)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOG)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOH)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPION)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOO)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOP)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOQ)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_DTS)            || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_MCO1)           || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_MCO2)           || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_NPURAM0)        || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_NPURAM1)        || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_NPURAM2)        || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_NPURAM3)        || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_XSPIPHYCOMP)    || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_XSPIPHY1)       || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_XSPIPHY2)       || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_HDP)            || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_RAMCFG))

#define IS_RIF_AWARE_PERIPH_INDEX(__INDEX__) (((__INDEX__) == RIF_AWARE_PERIPH_INDEX_CM55)         || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_EXTI)         || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_GPDMA1)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_HPDMA1)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RTC)          || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_TAMP)         || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_BSEC)         || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RCC)          || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_PWR)          || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_IAC)          || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF1)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF2)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF3)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF4)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF5)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF6)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF7)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF8)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF9)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF11)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF12)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF13)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF14)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF15)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF21)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF22)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF23)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RIFSC))
#endif /* defined(NPU_PRESENT) && !defined(CRYP) */

#if !defined(NPU_PRESENT) && defined(CRYP)
#define IS_RIF_RISC_PERIPH_INDEX(__INDEX__)  (((__INDEX__) == RIF_RISC_PERIPH_INDEX_SPI1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SPI2)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SPI3)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SPI4)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SPI5)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SPI6)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SAI1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SAI2)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_I2C1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_I2C2)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_I2C3)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_I2C4)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_I3C1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_I3C2)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_USART1)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_USART2)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_USART3)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_UART4)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_UART5)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_USART6)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_UART7)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_UART8)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_UART9)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_USART10)       || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LPUART1)       || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_FDCAN1)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM2)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM3)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM4)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM5)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM6)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM7)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM8)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM9)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM10)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM11)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM12)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM13)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM14)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM15)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM16)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM17)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM18)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_GFXTIM)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LPTIM1)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LPTIM2)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LPTIM3)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LPTIM4)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LPTIM5)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_ADF1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_MDF1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SDMMC1)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SDMMC2)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_MDIOS)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_OTG1HS)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_OTG2HS)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_UCPD1)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_ETH1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SPDIFRX)       || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SYSCFG)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_ADC12)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_VREFBUF)       || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_CRC)           || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_IWDG)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_WWDG)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_RNG)           || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_PKA)           || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SAES)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_HASH)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_CRYP)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_MCE1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_MCE2)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_MCE3)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_MCE4)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_XSPI1)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_XSPI2)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_XSPI3)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_XSPIM)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_FMC)           || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_CSI)           || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_DCMIPP)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_DCMI)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_JPEG)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_VENC)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_ICACHE)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_GPU2D)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_GFXMMU)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_DMA2D)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LTDC)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LTDCL1)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LTDCL2))

#define IS_RIF_RCC_PERIPH_INDEX(__INDEX__)   (((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPDMA1)         || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_HPDMA1)         || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_RTC)            || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_AXISRAM1)       || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_AXISRAM2)       || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_FLEXRAM)        || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_CACHEAXIRAM)    || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_VENCRAM)        || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_AHBRAM1)        || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_AHBRAM2)        || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_BKPRAM)         || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOA)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOB)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOC)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOD)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOE)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOF)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOG)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOH)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPION)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOO)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOP)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOQ)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_DTS)            || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_MCO1)           || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_MCO2)           || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_XSPIPHYCOMP)    || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_XSPIPHY1)       || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_XSPIPHY2)       || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_HDP)            || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_RAMCFG))

#define IS_RIF_AWARE_PERIPH_INDEX(__INDEX__) (((__INDEX__) == RIF_AWARE_PERIPH_INDEX_CM55)         || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_EXTI)         || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_GPDMA1)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_HPDMA1)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RTC)          || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_TAMP)         || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_BSEC)         || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RCC)          || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_PWR)          || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_IAC)          || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF1)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF2)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF3)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF6)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF7)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF8)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF9)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF11)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF12)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF13)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF14)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF21)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF22)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF23)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RIFSC))
#endif /* !defined(NPU_PRESENT) && defined(CRYP) */

#if defined(NPU_PRESENT) && defined(CRYP)
#define IS_RIF_RISC_PERIPH_INDEX(__INDEX__)  (((__INDEX__) == RIF_RISC_PERIPH_INDEX_SPI1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SPI2)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SPI3)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SPI4)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SPI5)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SPI6)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SAI1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SAI2)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_I2C1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_I2C2)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_I2C3)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_I2C4)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_I3C1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_I3C2)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_USART1)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_USART2)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_USART3)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_UART4)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_UART5)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_USART6)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_UART7)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_UART8)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_UART9)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_USART10)       || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LPUART1)       || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_FDCAN1)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM2)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM3)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM4)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM5)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM6)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM7)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM8)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM9)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM10)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM11)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM12)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM13)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM14)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM15)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM16)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM17)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_TIM18)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_GFXTIM)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LPTIM1)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LPTIM2)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LPTIM3)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LPTIM4)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LPTIM5)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_ADF1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_MDF1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SDMMC1)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SDMMC2)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_MDIOS)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_OTG1HS)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_OTG2HS)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_UCPD1)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_ETH1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SPDIFRX)       || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SYSCFG)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_ADC12)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_VREFBUF)       || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_CRC)           || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_IWDG)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_WWDG)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_RNG)           || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_PKA)           || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_SAES)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_HASH)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_CRYP)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_MCE1)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_MCE2)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_MCE3)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_MCE4)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_XSPI1)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_XSPI2)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_XSPI3)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_XSPIM)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_FMC)           || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_CSI)           || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_DCMIPP)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_DCMI)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_JPEG)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_VENC)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_ICACHE)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_GPU2D)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_GFXMMU)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_DMA2D)         || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LTDC)          || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LTDCL1)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_LTDCL2)        || \
                                              ((__INDEX__) == RIF_RISC_PERIPH_INDEX_NPU))

#define IS_RIF_RCC_PERIPH_INDEX(__INDEX__)   (((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPDMA1)         || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_HPDMA1)         || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_RTC)            || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_AXISRAM1)       || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_AXISRAM2)       || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_FLEXRAM)        || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_CACHEAXIRAM)    || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_VENCRAM)        || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_CACHECONFIG)    || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_AHBRAM1)        || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_AHBRAM2)        || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_BKPRAM)         || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOA)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOB)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOC)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOD)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOE)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOF)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOG)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOH)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPION)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOO)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOP)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_GPIOQ)          || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_DTS)            || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_MCO1)           || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_MCO2)           || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_NPURAM0)        || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_NPURAM1)        || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_NPURAM2)        || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_NPURAM3)        || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_XSPIPHYCOMP)    || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_XSPIPHY1)       || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_XSPIPHY2)       || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_HDP)            || \
                                              ((__INDEX__) == RIF_RCC_PERIPH_INDEX_RAMCFG))

#define IS_RIF_AWARE_PERIPH_INDEX(__INDEX__) (((__INDEX__) == RIF_AWARE_PERIPH_INDEX_CM55)         || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_EXTI)         || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_GPDMA1)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_HPDMA1)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RTC)          || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_TAMP)         || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_BSEC)         || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RCC)          || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_PWR)          || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_IAC)          || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF1)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF2)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF3)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF4)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF5)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF6)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF7)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF8)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF9)       || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF11)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF12)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF13)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF14)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF15)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF21)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF22)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RISAF23)      || \
                                              ((__INDEX__) == RIF_AWARE_PERIPH_INDEX_RIFSC))
#endif /* defined(NPU_PRESENT) && defined(CRYP) */

#define IS_RIF_SEC_PRIV_ATTRIBUTE(__ATTRIBUTE__)  (((uint32_t)(__ATTRIBUTE__) & ~RIF_ATTRIBUTE_MASK) == 0x00U)

#define IS_RIF_ACCESS_TYPE(__ACCTYPE__)  (((__ACCTYPE__) == RIF_ACCTYPE_READ_FETCH) || \
                                          ((__ACCTYPE__) == RIF_ACCTYPE_WRITE))

#define IS_RIF_LOCK_STATE(__LOCK__)  (((__LOCK__) == RIF_LOCK_DISABLE) || \
                                      ((__LOCK__) == RIF_LOCK_ENABLE))
/**
  * @}
  */

/** @defgroup RIF_Private_Macros_RISAF RIF Private Macros - RISAF
  * @{
  */

#if !defined(NPU_PRESENT)
#define IS_RISAF_INSTANCE(__INSTANCE__) (((__INSTANCE__) == RISAF1)   || \
                                         ((__INSTANCE__) == RISAF2)   || \
                                         ((__INSTANCE__) == RISAF3)   || \
                                         ((__INSTANCE__) == RISAF6)   || \
                                         ((__INSTANCE__) == RISAF7)   || \
                                         ((__INSTANCE__) == RISAF8)   || \
                                         ((__INSTANCE__) == RISAF9)   || \
                                         ((__INSTANCE__) == RISAF11)  || \
                                         ((__INSTANCE__) == RISAF12)  || \
                                         ((__INSTANCE__) == RISAF13)  || \
                                         ((__INSTANCE__) == RISAF14)  || \
                                         ((__INSTANCE__) == RISAF21)  || \
                                         ((__INSTANCE__) == RISAF22)  || \
                                         ((__INSTANCE__) == RISAF23))
#else
#define IS_RISAF_INSTANCE(__INSTANCE__) (((__INSTANCE__) == RISAF1)   || \
                                         ((__INSTANCE__) == RISAF2)   || \
                                         ((__INSTANCE__) == RISAF3)   || \
                                         ((__INSTANCE__) == RISAF4)   || \
                                         ((__INSTANCE__) == RISAF5)   || \
                                         ((__INSTANCE__) == RISAF6)   || \
                                         ((__INSTANCE__) == RISAF7)   || \
                                         ((__INSTANCE__) == RISAF8)   || \
                                         ((__INSTANCE__) == RISAF9)   || \
                                         ((__INSTANCE__) == RISAF11)  || \
                                         ((__INSTANCE__) == RISAF12)  || \
                                         ((__INSTANCE__) == RISAF13)  || \
                                         ((__INSTANCE__) == RISAF14)  || \
                                         ((__INSTANCE__) == RISAF15)  || \
                                         ((__INSTANCE__) == RISAF21)  || \
                                         ((__INSTANCE__) == RISAF22)  || \
                                         ((__INSTANCE__) == RISAF23))
#endif /* !defined(NPU_PRESENT) */

#define IS_RISAF_REGION(__REGION__)  (((__REGION__) == RISAF_REGION_1)  || \
                                      ((__REGION__) == RISAF_REGION_2)  || \
                                      ((__REGION__) == RISAF_REGION_3)  || \
                                      ((__REGION__) == RISAF_REGION_4)  || \
                                      ((__REGION__) == RISAF_REGION_5)  || \
                                      ((__REGION__) == RISAF_REGION_6)  || \
                                      ((__REGION__) == RISAF_REGION_7)  || \
                                      ((__REGION__) == RISAF_REGION_8)  || \
                                      ((__REGION__) == RISAF_REGION_9)  || \
                                      ((__REGION__) == RISAF_REGION_10) || \
                                      ((__REGION__) == RISAF_REGION_11) || \
                                      ((__REGION__) == RISAF_REGION_12) || \
                                      ((__REGION__) == RISAF_REGION_13) || \
                                      ((__REGION__) == RISAF_REGION_14) || \
                                      ((__REGION__) == RISAF_REGION_15))

#if !defined(NPU_PRESENT)
#define IS_RISAF_MAX_REGION(__INSTANCE__, __REGION__)    (((__INSTANCE__) == RISAF1)  ? ((__REGION__) <= RISAF_REGION_7)  : \
                                                          ((__INSTANCE__) == RISAF2)  ? ((__REGION__) <= RISAF_REGION_7)  : \
                                                          ((__INSTANCE__) == RISAF3)  ? ((__REGION__) <= RISAF_REGION_7)  : \
                                                          ((__INSTANCE__) == RISAF6)  ? ((__REGION__) <= RISAF_REGION_11) : \
                                                          ((__INSTANCE__) == RISAF7)  ? ((__REGION__) <= RISAF_REGION_7)  : \
                                                          ((__INSTANCE__) == RISAF8)  ? ((__REGION__) <= RISAF_REGION_7)  : \
                                                          ((__INSTANCE__) == RISAF9)  ? ((__REGION__) <= RISAF_REGION_7)  : \
                                                          ((__INSTANCE__) == RISAF11) ? ((__REGION__) <= RISAF_REGION_7)  : \
                                                          ((__INSTANCE__) == RISAF12) ? ((__REGION__) <= RISAF_REGION_7)  : \
                                                          ((__INSTANCE__) == RISAF13) ? ((__REGION__) <= RISAF_REGION_7)  : \
                                                          ((__INSTANCE__) == RISAF14) ? ((__REGION__) <= RISAF_REGION_7)  : \
                                                          ((__INSTANCE__) == RISAF21) ? ((__REGION__) <= RISAF_REGION_7)  : \
                                                          ((__INSTANCE__) == RISAF22) ? ((__REGION__) <= RISAF_REGION_7)  : \
                                                          ((__REGION__) <= RISAF_REGION_3))
#else
#define IS_RISAF_MAX_REGION(__INSTANCE__, __REGION__)    (((__INSTANCE__) == RISAF1)  ? ((__REGION__) <= RISAF_REGION_7)  : \
                                                          ((__INSTANCE__) == RISAF2)  ? ((__REGION__) <= RISAF_REGION_7)  : \
                                                          ((__INSTANCE__) == RISAF3)  ? ((__REGION__) <= RISAF_REGION_7)  : \
                                                          ((__INSTANCE__) == RISAF4)  ? ((__REGION__) <= RISAF_REGION_11) : \
                                                          ((__INSTANCE__) == RISAF5)  ? ((__REGION__) <= RISAF_REGION_11) : \
                                                          ((__INSTANCE__) == RISAF6)  ? ((__REGION__) <= RISAF_REGION_11) : \
                                                          ((__INSTANCE__) == RISAF7)  ? ((__REGION__) <= RISAF_REGION_7)  : \
                                                          ((__INSTANCE__) == RISAF8)  ? ((__REGION__) <= RISAF_REGION_7)  : \
                                                          ((__INSTANCE__) == RISAF9)  ? ((__REGION__) <= RISAF_REGION_7)  : \
                                                          ((__INSTANCE__) == RISAF11) ? ((__REGION__) <= RISAF_REGION_7)  : \
                                                          ((__INSTANCE__) == RISAF12) ? ((__REGION__) <= RISAF_REGION_7)  : \
                                                          ((__INSTANCE__) == RISAF13) ? ((__REGION__) <= RISAF_REGION_7)  : \
                                                          ((__INSTANCE__) == RISAF14) ? ((__REGION__) <= RISAF_REGION_7)  : \
                                                          ((__INSTANCE__) == RISAF15) ? ((__REGION__) <= RISAF_REGION_2)  : \
                                                          ((__INSTANCE__) == RISAF21) ? ((__REGION__) <= RISAF_REGION_7)  : \
                                                          ((__INSTANCE__) == RISAF22) ? ((__REGION__) <= RISAF_REGION_7)  : \
                                                          ((__REGION__) <= RISAF_REGION_3))
#endif /* !defined(NPU_PRESENT) */

#if !defined(NPU_PRESENT)
#define IS_RISAF_GRANULARITY(__INSTANCE__, __ADDRESS__)  ((((__INSTANCE__) == RISAF1)  ? ((__ADDRESS__) % RISAF1_GRANULARITY)  : \
                                                           ((__INSTANCE__) == RISAF2)  ? ((__ADDRESS__) % RISAF2_GRANULARITY)  : \
                                                           ((__INSTANCE__) == RISAF3)  ? ((__ADDRESS__) % RISAF3_GRANULARITY)  : \
                                                           ((__INSTANCE__) == RISAF6)  ? ((__ADDRESS__) % RISAF6_GRANULARITY)  : \
                                                           ((__INSTANCE__) == RISAF7)  ? ((__ADDRESS__) % RISAF7_GRANULARITY)  : \
                                                           ((__INSTANCE__) == RISAF8)  ? ((__ADDRESS__) % RISAF8_GRANULARITY)  : \
                                                           ((__INSTANCE__) == RISAF9)  ? ((__ADDRESS__) % RISAF9_GRANULARITY)  : \
                                                           ((__INSTANCE__) == RISAF11) ? ((__ADDRESS__) % RISAF11_GRANULARITY) : \
                                                           ((__INSTANCE__) == RISAF12) ? ((__ADDRESS__) % RISAF12_GRANULARITY) : \
                                                           ((__INSTANCE__) == RISAF13) ? ((__ADDRESS__) % RISAF13_GRANULARITY) : \
                                                           ((__INSTANCE__) == RISAF14) ? ((__ADDRESS__) % RISAF14_GRANULARITY) : \
                                                           ((__INSTANCE__) == RISAF21) ? ((__ADDRESS__) % RISAF21_GRANULARITY) : \
                                                           ((__INSTANCE__) == RISAF22) ? ((__ADDRESS__) % RISAF22_GRANULARITY) : \
                                                           ((__ADDRESS__) % RISAF23_GRANULARITY)) == 0x00U)
#else
#define IS_RISAF_GRANULARITY(__INSTANCE__, __ADDRESS__)  ((((__INSTANCE__) == RISAF1)  ? ((__ADDRESS__) % RISAF1_GRANULARITY)  : \
                                                           ((__INSTANCE__) == RISAF2)  ? ((__ADDRESS__) % RISAF2_GRANULARITY)  : \
                                                           ((__INSTANCE__) == RISAF3)  ? ((__ADDRESS__) % RISAF3_GRANULARITY)  : \
                                                           ((__INSTANCE__) == RISAF4)  ? ((__ADDRESS__) % RISAF4_GRANULARITY)  : \
                                                           ((__INSTANCE__) == RISAF5)  ? ((__ADDRESS__) % RISAF5_GRANULARITY)  : \
                                                           ((__INSTANCE__) == RISAF6)  ? ((__ADDRESS__) % RISAF6_GRANULARITY)  : \
                                                           ((__INSTANCE__) == RISAF7)  ? ((__ADDRESS__) % RISAF7_GRANULARITY)  : \
                                                           ((__INSTANCE__) == RISAF8)  ? ((__ADDRESS__) % RISAF8_GRANULARITY)  : \
                                                           ((__INSTANCE__) == RISAF9)  ? ((__ADDRESS__) % RISAF9_GRANULARITY)  : \
                                                           ((__INSTANCE__) == RISAF11) ? ((__ADDRESS__) % RISAF11_GRANULARITY) : \
                                                           ((__INSTANCE__) == RISAF12) ? ((__ADDRESS__) % RISAF12_GRANULARITY) : \
                                                           ((__INSTANCE__) == RISAF13) ? ((__ADDRESS__) % RISAF13_GRANULARITY) : \
                                                           ((__INSTANCE__) == RISAF14) ? ((__ADDRESS__) % RISAF14_GRANULARITY) : \
                                                           ((__INSTANCE__) == RISAF15) ? ((__ADDRESS__) % RISAF15_GRANULARITY) : \
                                                           ((__INSTANCE__) == RISAF21) ? ((__ADDRESS__) % RISAF21_GRANULARITY) : \
                                                           ((__INSTANCE__) == RISAF22) ? ((__ADDRESS__) % RISAF22_GRANULARITY) : \
                                                           ((__ADDRESS__) % RISAF23_GRANULARITY)) == 0x00U)
#endif /* !defined(NPU_PRESENT) */

#if !defined(NPU_PRESENT)
#define IS_RISAF_LIMIT_ADDRESS_SPACE_SIZE(__INSTANCE__, __ADDRESS__)   ((((__INSTANCE__) == RISAF1)  && ((__ADDRESS__) < (RISAF1_LIMIT_ADDRESS_SPACE_SIZE + 1U)))  || \
                                                                        (((__INSTANCE__) == RISAF2)  && ((__ADDRESS__) < (RISAF2_LIMIT_ADDRESS_SPACE_SIZE + 1U)))  || \
                                                                        (((__INSTANCE__) == RISAF3)  && ((__ADDRESS__) < (RISAF3_LIMIT_ADDRESS_SPACE_SIZE + 1U)))  || \
                                                                        ((__INSTANCE__) == RISAF6)  || \
                                                                        (((__INSTANCE__) == RISAF7)  && ((__ADDRESS__) < (RISAF7_LIMIT_ADDRESS_SPACE_SIZE + 1U)))  || \
                                                                        (((__INSTANCE__) == RISAF8)  && ((__ADDRESS__) < (RISAF8_LIMIT_ADDRESS_SPACE_SIZE + 1U)))  || \
                                                                        (((__INSTANCE__) == RISAF9)  && ((__ADDRESS__) < (RISAF9_LIMIT_ADDRESS_SPACE_SIZE + 1U)))  || \
                                                                        (((__INSTANCE__) == RISAF11) && ((__ADDRESS__) < (RISAF11_LIMIT_ADDRESS_SPACE_SIZE + 1U))) || \
                                                                        (((__INSTANCE__) == RISAF12) && ((__ADDRESS__) < (RISAF12_LIMIT_ADDRESS_SPACE_SIZE + 1U))) || \
                                                                        (((__INSTANCE__) == RISAF13) && ((__ADDRESS__) < (RISAF13_LIMIT_ADDRESS_SPACE_SIZE + 1U))) || \
                                                                        (((__INSTANCE__) == RISAF14) && ((__ADDRESS__) < (RISAF14_LIMIT_ADDRESS_SPACE_SIZE + 1U))) || \
                                                                        (((__INSTANCE__) == RISAF21) && ((__ADDRESS__) < (RISAF21_LIMIT_ADDRESS_SPACE_SIZE + 1U))) || \
                                                                        (((__INSTANCE__) == RISAF22) && ((__ADDRESS__) < (RISAF22_LIMIT_ADDRESS_SPACE_SIZE + 1U))) || \
                                                                        (((__INSTANCE__) == RISAF23) && ((__ADDRESS__) < (RISAF23_LIMIT_ADDRESS_SPACE_SIZE + 1U))))
#else
#define IS_RISAF_LIMIT_ADDRESS_SPACE_SIZE(__INSTANCE__, __ADDRESS__)   ((((__INSTANCE__) == RISAF1)  && ((__ADDRESS__) < (RISAF1_LIMIT_ADDRESS_SPACE_SIZE + 1U)))  || \
                                                                        (((__INSTANCE__) == RISAF2)  && ((__ADDRESS__) < (RISAF2_LIMIT_ADDRESS_SPACE_SIZE + 1U)))  || \
                                                                        (((__INSTANCE__) == RISAF3)  && ((__ADDRESS__) < (RISAF3_LIMIT_ADDRESS_SPACE_SIZE + 1U)))  || \
                                                                        ((__INSTANCE__) == RISAF4)  || \
                                                                        ((__INSTANCE__) == RISAF5)  || \
                                                                        ((__INSTANCE__) == RISAF6)  || \
                                                                        (((__INSTANCE__) == RISAF7)  && ((__ADDRESS__) < (RISAF7_LIMIT_ADDRESS_SPACE_SIZE + 1U)))  || \
                                                                        (((__INSTANCE__) == RISAF8)  && ((__ADDRESS__) < (RISAF8_LIMIT_ADDRESS_SPACE_SIZE + 1U)))  || \
                                                                        (((__INSTANCE__) == RISAF9)  && ((__ADDRESS__) < (RISAF9_LIMIT_ADDRESS_SPACE_SIZE + 1U)))  || \
                                                                        (((__INSTANCE__) == RISAF11) && ((__ADDRESS__) < (RISAF11_LIMIT_ADDRESS_SPACE_SIZE + 1U))) || \
                                                                        (((__INSTANCE__) == RISAF12) && ((__ADDRESS__) < (RISAF12_LIMIT_ADDRESS_SPACE_SIZE + 1U))) || \
                                                                        (((__INSTANCE__) == RISAF13) && ((__ADDRESS__) < (RISAF13_LIMIT_ADDRESS_SPACE_SIZE + 1U))) || \
                                                                        (((__INSTANCE__) == RISAF14) && ((__ADDRESS__) < (RISAF14_LIMIT_ADDRESS_SPACE_SIZE + 1U))) || \
                                                                        (((__INSTANCE__) == RISAF15) && ((__ADDRESS__) < (RISAF15_LIMIT_ADDRESS_SPACE_SIZE + 1U))) || \
                                                                        (((__INSTANCE__) == RISAF21) && ((__ADDRESS__) < (RISAF21_LIMIT_ADDRESS_SPACE_SIZE + 1U))) || \
                                                                        (((__INSTANCE__) == RISAF22) && ((__ADDRESS__) < (RISAF22_LIMIT_ADDRESS_SPACE_SIZE + 1U))) || \
                                                                        (((__INSTANCE__) == RISAF23) && ((__ADDRESS__) < (RISAF23_LIMIT_ADDRESS_SPACE_SIZE + 1U))))
#endif /* !defined(NPU_PRESENT) */

#define IS_RISAF_SUBREGION(__SUBREGION__)  (((__SUBREGION__) == RISAF_SUBREGION_A) || \
                                            ((__SUBREGION__) == RISAF_SUBREGION_B))

#define IS_RISAF_FILTERING(__FILTERING__)  (((__FILTERING__) == RISAF_FILTER_DISABLE) || \
                                            ((__FILTERING__) == RISAF_FILTER_ENABLE))

#define IS_RISAF_DELEGATION(__DELEGATION__)  (((__DELEGATION__) == RISAF_DELEGATION_DISABLE) || \
                                              ((__DELEGATION__) == RISAF_DELEGATION_ENABLE))


#define IS_RISAF_READ_WRITE(__RW__)  (((uint32_t)(__RW__) == RISAF_READ_DISABLE) || \
                                      (((uint32_t)(__RW__) & ~RISAF_READ_WRITE_MASK) == 0x00U))
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup RIF_Exported_Functions RIF Exported Functions
  * @{
  */
/** @defgroup RIF_Exported_Functions_Group1 RIMC Configuration functions
  * @{
  */
#if defined(CPU_AS_TRUSTED_DOMAIN)
void     HAL_RIF_RIMC_Lock(void);
#endif /* CPU_AS_TRUSTED_DOMAIN */
uint32_t HAL_RIF_RIMC_GetLock(void);
#if defined(CPU_AS_TRUSTED_DOMAIN)
void     HAL_RIF_RIMC_SetDebugAccessPortCID(uint32_t CID);
#endif /* CPU_AS_TRUSTED_DOMAIN */
uint32_t HAL_RIF_RIMC_GetDebugAccessPortCID(void);
#if defined(CPU_AS_TRUSTED_DOMAIN)
void     HAL_RIF_RIMC_ConfigMasterAttributes(uint32_t MasterId, const RIMC_MasterConfig_t *pConfig);
void     HAL_RIF_RIMC_GetConfigMasterAttributes(uint32_t MasterId, RIMC_MasterConfig_t *pConfig);
#endif /* CPU_AS_TRUSTED_DOMAIN */
/**
  * @}
  */

/** @defgroup RIF_Exported_Functions_Group2 RIFSC Configuration functions
  * @{
  */
#if defined(CPU_AS_TRUSTED_DOMAIN)
void       HAL_RIF_RISC_Lock(void);
#endif /* CPU_AS_TRUSTED_DOMAIN */
uint32_t   HAL_RIF_RISC_GetLock(void);
void       HAL_RIF_RISC_SetSlaveSecureAttributes(uint32_t PeriphId, uint32_t SecPriv);
uint32_t   HAL_RIF_RISC_GetSlaveSecureAttributes(uint32_t PeriphId);
void       HAL_RIF_RISC_SlaveConfigLock(uint32_t PeriphId);
uint32_t   HAL_RIF_RISC_GetSlaveConfigLock(uint32_t PeriphId);
/**
  * @}
  */

/** @defgroup RIF_Exported_Functions_Group4 RISAF Configuration functions
  * @{
  */
#if defined(CPU_AS_TRUSTED_DOMAIN)
void      HAL_RIF_RISAF_Lock(RISAF_TypeDef *RISAFx);
#endif /* CPU_AS_TRUSTED_DOMAIN */
uint32_t  HAL_RIF_RISAF_GetLock(const RISAF_TypeDef *RISAFx);
#if defined(CPU_AS_TRUSTED_DOMAIN)
void      HAL_RIF_RISAF_ConfigBaseRegion(RISAF_TypeDef *RISAFx, uint32_t Region,
                                         const RISAF_BaseRegionConfig_t *pConfig);
#endif /* CPU_AS_TRUSTED_DOMAIN */
void      HAL_RIF_RISAF_GetConfigBaseRegion(const RISAF_TypeDef *RISAFx, uint32_t Region,
                                            RISAF_BaseRegionConfig_t *pConfig);
#if defined(CPU_AS_TRUSTED_DOMAIN)
void HAL_RIF_RISAF_ConfigSubRegionDelegation(RISAF_TypeDef *RISAFx, uint32_t Region, uint32_t SubRegion,
                                             const RISAF_DelegationConfig_t *pConfig);
#endif /* CPU_AS_TRUSTED_DOMAIN */
void      HAL_RIF_RISAF_GetConfigSubRegionDelegation(const RISAF_TypeDef *RISAFx, uint32_t Region, uint32_t SubRegion,
                                                     RISAF_DelegationConfig_t *pConfig);
#if defined(CPU_AS_TRUSTED_DOMAIN)
void      HAL_RIF_RISAF_ConfigSubRegion(RISAF_TypeDef *RISAFx, uint32_t Region, uint32_t SubRegion,
                                        const RISAF_SubRegionConfig_t *pConfig);
#endif /* CPU_AS_TRUSTED_DOMAIN */
void      HAL_RIF_RISAF_GetConfigSubRegion(const RISAF_TypeDef *RISAFx, uint32_t Region, uint32_t SubRegion,
                                           RISAF_SubRegionConfig_t *pConfig);
#if defined(CPU_AS_TRUSTED_DOMAIN)
void      HAL_RIF_RISAF_GetIllegalAccess(RISAF_TypeDef *RISAFx, RISAF_IllegalAccess_t *IllegalAccess);
#endif /* CPU_AS_TRUSTED_DOMAIN */
/**
  * @}
  */

/** @defgroup RIF_Exported_Functions_Group6 IAC Configuration functions
  * @{
  */
#if defined(CPU_AS_TRUSTED_DOMAIN) && defined(CPU_IN_SECURE_STATE)
void      HAL_RIF_IAC_EnableIT(uint32_t PeriphId);
void      HAL_RIF_IAC_DisableIT(uint32_t PeriphId);
uint32_t  HAL_RIF_IAC_GetFlag(uint32_t PeriphId);
void      HAL_RIF_IAC_ClearFlag(uint32_t PeriphId);
void      HAL_RIF_IRQHandler(void);
void      HAL_RIF_ILA_Callback(uint32_t PeriphId);
#endif /* CPU_AS_TRUSTED_DOMAIN && CPU_IN_SECURE_STATE */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32N6xx_HAL_RIF_H */

