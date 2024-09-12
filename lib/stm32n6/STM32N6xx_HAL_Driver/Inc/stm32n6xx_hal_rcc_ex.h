/**
  ******************************************************************************
  * @file    stm32n6xx_hal_rcc_ex.h
  * @author  MCD Application Team
  * @brief   Header file of RCC HAL Extension module.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32N6xx_HAL_RCC_EX_H
#define STM32N6xx_HAL_RCC_EX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @addtogroup RCCEx
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup RCCEx_Exported_Constants RCCEx Exported Constants
  * @{
  */

/** @defgroup RCCEx_Periph_Clock_Selection  Periph Clock Selection
  * @{
  */
#define RCC_PERIPHCLK_ADC         (0x0000000000000001UL)
#define RCC_PERIPHCLK_ADF1        (0x0000000000000002UL)
#define RCC_PERIPHCLK_CKPER       (0x0000000000000004UL)
#define RCC_PERIPHCLK_CSI         (0x0000000000000008UL)
#define RCC_PERIPHCLK_DCMIPP      (0x0000000000000010UL)
#define RCC_PERIPHCLK_ETH1        (0x0000000000000020UL)
#define RCC_PERIPHCLK_ETH1PHY     (0x0000000000000040UL)
#define RCC_PERIPHCLK_ETH1RX      (0x0000000000000080UL)
#define RCC_PERIPHCLK_ETH1TX      (0x0000000000000100UL)
#define RCC_PERIPHCLK_ETH1PTP     (0x0000000000000200UL)
#define RCC_PERIPHCLK_FDCAN       (0x0000000000000400UL)
#define RCC_PERIPHCLK_FMC         (0x0000000000000800UL)
#define RCC_PERIPHCLK_I2C1        (0x0000000000001000UL)
#define RCC_PERIPHCLK_I2C2        (0x0000000000002000UL)
#define RCC_PERIPHCLK_I2C3        (0x0000000000004000UL)
#define RCC_PERIPHCLK_I2C4        (0x0000000000008000UL)
#define RCC_PERIPHCLK_I3C1        (0x0000000000010000UL)
#define RCC_PERIPHCLK_I3C2        (0x0000000000020000UL)
#define RCC_PERIPHCLK_LPTIM1      (0x0000000000040000UL)
#define RCC_PERIPHCLK_LPTIM2      (0x0000000000080000UL)
#define RCC_PERIPHCLK_LPTIM3      (0x0000000000100000UL)
#define RCC_PERIPHCLK_LPTIM4      (0x0000000000200000UL)
#define RCC_PERIPHCLK_LPTIM5      (0x0000000000400000UL)
#define RCC_PERIPHCLK_LPUART1     (0x0000000000800000UL)
#define RCC_PERIPHCLK_LTDC        (0x0000000001000000UL)
#define RCC_PERIPHCLK_MDF1        (0x0000000002000000UL)
#define RCC_PERIPHCLK_PSSI        (0x0000000004000000UL)
#define RCC_PERIPHCLK_RTC         (0x0000000008000000UL)
#define RCC_PERIPHCLK_SAI1        (0x0000000010000000UL)
#define RCC_PERIPHCLK_SAI2        (0x0000000020000000UL)
#define RCC_PERIPHCLK_SDMMC1      (0x0000000040000000UL)
#define RCC_PERIPHCLK_SDMMC2      (0x0000000080000000UL)
#define RCC_PERIPHCLK_SPDIFRX1    (0x0000000100000000UL)
#define RCC_PERIPHCLK_SPI1        (0x0000000200000000UL)
#define RCC_PERIPHCLK_SPI2        (0x0000000400000000UL)
#define RCC_PERIPHCLK_SPI3        (0x0000000800000000UL)
#define RCC_PERIPHCLK_SPI4        (0x0000001000000000UL)
#define RCC_PERIPHCLK_SPI5        (0x0000002000000000UL)
#define RCC_PERIPHCLK_SPI6        (0x0000004000000000UL)
#define RCC_PERIPHCLK_TIM         (0x0000008000000000UL)
#define RCC_PERIPHCLK_USART1      (0x0000010000000000UL)
#define RCC_PERIPHCLK_USART2      (0x0000020000000000UL)
#define RCC_PERIPHCLK_USART3      (0x0000040000000000UL)
#define RCC_PERIPHCLK_UART4       (0x0000080000000000UL)
#define RCC_PERIPHCLK_UART5       (0x0000100000000000UL)
#define RCC_PERIPHCLK_USART6      (0x0000200000000000UL)
#define RCC_PERIPHCLK_UART7       (0x0000400000000000UL)
#define RCC_PERIPHCLK_UART8       (0x0000800000000000UL)
#define RCC_PERIPHCLK_UART9       (0x0001000000000000UL)
#define RCC_PERIPHCLK_USART10     (0x0002000000000000UL)
#define RCC_PERIPHCLK_USBPHY1     (0x0004000000000000UL)
#define RCC_PERIPHCLK_USBOTGHS1   (0x0008000000000000UL)
#define RCC_PERIPHCLK_USBPHY2     (0x0010000000000000UL)
#define RCC_PERIPHCLK_USBOTGHS2   (0x0020000000000000UL)
#define RCC_PERIPHCLK_XSPI1       (0x0040000000000000UL)
#define RCC_PERIPHCLK_XSPI2       (0x0080000000000000UL)
#define RCC_PERIPHCLK_XSPI3       (0x0100000000000000UL)
/**
  * @}
  */


/** @defgroup RCCEx_ADC_Clock_Source  ADC Clock Source
  * @{
  */
#define RCC_ADCCLKSOURCE_HCLK      LL_RCC_ADC_CLKSOURCE_HCLK
#define RCC_ADCCLKSOURCE_CLKP      LL_RCC_ADC_CLKSOURCE_CLKP
#define RCC_ADCCLKSOURCE_IC7       LL_RCC_ADC_CLKSOURCE_IC7
#define RCC_ADCCLKSOURCE_IC8       LL_RCC_ADC_CLKSOURCE_IC8
#define RCC_ADCCLKSOURCE_MSI       LL_RCC_ADC_CLKSOURCE_MSI
#define RCC_ADCCLKSOURCE_HSI       LL_RCC_ADC_CLKSOURCE_HSI
#define RCC_ADCCLKSOURCE_PIN       LL_RCC_ADC_CLKSOURCE_I2S_CKIN
#define RCC_ADCCLKSOURCE_TIMG      LL_RCC_ADC_CLKSOURCE_TIMG
/**
  * @}
  */

/** @defgroup RCCEx_ADF1_Clock_Source  ADF1 Clock Source
  * @{
  */
#define RCC_ADF1CLKSOURCE_HCLK     LL_RCC_ADF1_CLKSOURCE_HCLK
#define RCC_ADF1CLKSOURCE_CLKP     LL_RCC_ADF1_CLKSOURCE_CLKP
#define RCC_ADF1CLKSOURCE_IC7      LL_RCC_ADF1_CLKSOURCE_IC7
#define RCC_ADF1CLKSOURCE_IC8      LL_RCC_ADF1_CLKSOURCE_IC8
#define RCC_ADF1CLKSOURCE_MSI      LL_RCC_ADF1_CLKSOURCE_MSI
#define RCC_ADF1CLKSOURCE_HSI      LL_RCC_ADF1_CLKSOURCE_HSI
#define RCC_ADF1CLKSOURCE_PIN      LL_RCC_ADF1_CLKSOURCE_I2S_CKIN
#define RCC_ADF1CLKSOURCE_TIMG     LL_RCC_ADF1_CLKSOURCE_TIMG
/**
  * @}
  */

/** @defgroup RCCEx_CLKP_Clock_Source  CLKP Clock Source
  * @{
  */
#define RCC_CLKPCLKSOURCE_HSI      LL_RCC_CLKP_CLKSOURCE_HSI
#define RCC_CLKPCLKSOURCE_MSI      LL_RCC_CLKP_CLKSOURCE_MSI
#define RCC_CLKPCLKSOURCE_HSE      LL_RCC_CLKP_CLKSOURCE_HSE
#define RCC_CLKPCLKSOURCE_IC5      LL_RCC_CLKP_CLKSOURCE_IC5
#define RCC_CLKPCLKSOURCE_IC10     LL_RCC_CLKP_CLKSOURCE_IC10
#define RCC_CLKPCLKSOURCE_IC15     LL_RCC_CLKP_CLKSOURCE_IC15
#define RCC_CLKPCLKSOURCE_IC19     LL_RCC_CLKP_CLKSOURCE_IC19
#define RCC_CLKPCLKSOURCE_IC20     LL_RCC_CLKP_CLKSOURCE_IC20
/**
  * @}
  */

/** @defgroup RCCEx_DCMIPP_Clock_Source DCMIPP Clock Source
  * @{
  */
#define RCC_DCMIPPCLKSOURCE_PCLK5  LL_RCC_DCMIPP_CLKSOURCE_PCLK5
#define RCC_DCMIPPCLKSOURCE_CLKP   LL_RCC_DCMIPP_CLKSOURCE_CLKP
#define RCC_DCMIPPCLKSOURCE_IC17   LL_RCC_DCMIPP_CLKSOURCE_IC17
#define RCC_DCMIPPCLKSOURCE_HSI    LL_RCC_DCMIPP_CLKSOURCE_HSI
/**
  * @}
  */

/** @defgroup RCCEx_ETH1_Clock_Source  ETH1 Clock Source
  * @{
  */
#define RCC_ETH1CLKSOURCE_HCLK     LL_RCC_ETH1_CLKSOURCE_HCLK
#define RCC_ETH1CLKSOURCE_CLKP     LL_RCC_ETH1_CLKSOURCE_CLKP
#define RCC_ETH1CLKSOURCE_IC12     LL_RCC_ETH1_CLKSOURCE_IC12
#define RCC_ETH1CLKSOURCE_HSE      LL_RCC_ETH1_CLKSOURCE_HSE
/**
  * @}
  */

/** @defgroup RCCEx_ETH1_PHY_Interface  ETH1 PHY Interface
  * @{
  */
#define RCC_ETH1PHYIF_MII          LL_RCC_ETH1PHY_IF_MII
#define RCC_ETH1PHYIF_RGMII        LL_RCC_ETH1PHY_IF_RGMII
#define RCC_ETH1PHYIF_RMII         LL_RCC_ETH1PHY_IF_RMII
/**
  * @}
  */

/** @defgroup RCCEx_ETH1_RX_Clock_Source  ETH1 RX Clock Source
  * @{
  */
#define RCC_ETH1RXCLKSOURCE_EXT    LL_RCC_ETH1REFRX_CLKSOURCE_EXT
#define RCC_ETH1RXCLKSOURCE_INT    LL_RCC_ETH1REFRX_CLKSOURCE_INT
/**
  * @}
  */

/** @defgroup RCCEx_ETH1_TX_Clock_Source  ETH1 TX Clock Source
  * @{
  */
#define RCC_ETH1TXCLKSOURCE_EXT    LL_RCC_ETH1REFTX_CLKSOURCE_EXT
#define RCC_ETH1TXCLKSOURCE_INT    LL_RCC_ETH1REFTX_CLKSOURCE_INT
/**
  * @}
  */

/** @defgroup RCCEx_ETH1_PTP_Clock_Source  ETH1 PTP Clock Source
  * @{
  */
#define RCC_ETH1PTPCLKSOURCE_HCLK  LL_RCC_ETH1PTP_CLKSOURCE_HCLK
#define RCC_ETH1PTPCLKSOURCE_CLKP  LL_RCC_ETH1PTP_CLKSOURCE_CLKP
#define RCC_ETH1PTPCLKSOURCE_IC13  LL_RCC_ETH1PTP_CLKSOURCE_IC13
#define RCC_ETH1PTPCLKSOURCE_HSE   LL_RCC_ETH1PTP_CLKSOURCE_HSE
/**
  * @}
  */

/** @defgroup RCCEx_FDCAN_Clock_Source  FDCAN Kernel Clock Source
  * @{
  */
#define RCC_FDCANCLKSOURCE_PCLK1   LL_RCC_FDCAN_CLKSOURCE_PCLK1
#define RCC_FDCANCLKSOURCE_CLKP    LL_RCC_FDCAN_CLKSOURCE_CLKP
#define RCC_FDCANCLKSOURCE_IC19    LL_RCC_FDCAN_CLKSOURCE_IC19
#define RCC_FDCANCLKSOURCE_HSE     LL_RCC_FDCAN_CLKSOURCE_HSE
/**
  * @}
  */

/** @defgroup RCCEx_FMC_Clock_Source  FMC Clock Source
  * @{
  */
#define RCC_FMCCLKSOURCE_HCLK      LL_RCC_FMC_CLKSOURCE_HCLK
#define RCC_FMCCLKSOURCE_CLKP      LL_RCC_FMC_CLKSOURCE_CLKP
#define RCC_FMCCLKSOURCE_IC3       LL_RCC_FMC_CLKSOURCE_IC3
#define RCC_FMCCLKSOURCE_IC4       LL_RCC_FMC_CLKSOURCE_IC4
/**
  * @}
  */

/** @defgroup RCCEx_I2C1_Clock_Source  I2C1 Clock Source
  * @{
  */
#define RCC_I2C1CLKSOURCE_PCLK1    LL_RCC_I2C1_CLKSOURCE_PCLK1
#define RCC_I2C1CLKSOURCE_CLKP     LL_RCC_I2C1_CLKSOURCE_CLKP
#define RCC_I2C1CLKSOURCE_IC10     LL_RCC_I2C1_CLKSOURCE_IC10
#define RCC_I2C1CLKSOURCE_IC15     LL_RCC_I2C1_CLKSOURCE_IC15
#define RCC_I2C1CLKSOURCE_MSI      LL_RCC_I2C1_CLKSOURCE_MSI
#define RCC_I2C1CLKSOURCE_HSI      LL_RCC_I2C1_CLKSOURCE_HSI
/**
  * @}
  */

/** @defgroup RCCEx_I2C2_Clock_Source  I2C2 Clock Source
  * @{
  */
#define RCC_I2C2CLKSOURCE_PCLK1    LL_RCC_I2C2_CLKSOURCE_PCLK1
#define RCC_I2C2CLKSOURCE_CLKP     LL_RCC_I2C2_CLKSOURCE_CLKP
#define RCC_I2C2CLKSOURCE_IC10     LL_RCC_I2C2_CLKSOURCE_IC10
#define RCC_I2C2CLKSOURCE_IC15     LL_RCC_I2C2_CLKSOURCE_IC15
#define RCC_I2C2CLKSOURCE_MSI      LL_RCC_I2C2_CLKSOURCE_MSI
#define RCC_I2C2CLKSOURCE_HSI      LL_RCC_I2C2_CLKSOURCE_HSI
/**
  * @}
  */

/** @defgroup RCCEx_I2C3_Clock_Source  I2C3 Clock Source
  * @{
  */
#define RCC_I2C3CLKSOURCE_PCLK1    LL_RCC_I2C3_CLKSOURCE_PCLK1
#define RCC_I2C3CLKSOURCE_CLKP     LL_RCC_I2C3_CLKSOURCE_CLKP
#define RCC_I2C3CLKSOURCE_IC10     LL_RCC_I2C3_CLKSOURCE_IC10
#define RCC_I2C3CLKSOURCE_IC15     LL_RCC_I2C3_CLKSOURCE_IC15
#define RCC_I2C3CLKSOURCE_MSI      LL_RCC_I2C3_CLKSOURCE_MSI
#define RCC_I2C3CLKSOURCE_HSI      LL_RCC_I2C3_CLKSOURCE_HSI
/**
  * @}
  */

/** @defgroup RCCEx_I2C4_Clock_Source  I2C4 Clock Source
  * @{
  */
#define RCC_I2C4CLKSOURCE_PCLK1    LL_RCC_I2C4_CLKSOURCE_PCLK1
#define RCC_I2C4CLKSOURCE_CLKP     LL_RCC_I2C4_CLKSOURCE_CLKP
#define RCC_I2C4CLKSOURCE_IC10     LL_RCC_I2C4_CLKSOURCE_IC10
#define RCC_I2C4CLKSOURCE_IC15     LL_RCC_I2C4_CLKSOURCE_IC15
#define RCC_I2C4CLKSOURCE_MSI      LL_RCC_I2C4_CLKSOURCE_MSI
#define RCC_I2C4CLKSOURCE_HSI      LL_RCC_I2C4_CLKSOURCE_HSI
/**
  * @}
  */

/** @defgroup RCCEx_I3C1_Clock_Source  I3C1 Clock Source
  * @{
  */
#define RCC_I3C1CLKSOURCE_PCLK1    LL_RCC_I3C1_CLKSOURCE_PCLK1
#define RCC_I3C1CLKSOURCE_CLKP     LL_RCC_I3C1_CLKSOURCE_CLKP
#define RCC_I3C1CLKSOURCE_IC10     LL_RCC_I3C1_CLKSOURCE_IC10
#define RCC_I3C1CLKSOURCE_IC15     LL_RCC_I3C1_CLKSOURCE_IC15
#define RCC_I3C1CLKSOURCE_MSI      LL_RCC_I3C1_CLKSOURCE_MSI
#define RCC_I3C1CLKSOURCE_HSI      LL_RCC_I3C1_CLKSOURCE_HSI
/**
  * @}
  */

/** @defgroup RCCEx_I3C2_Clock_Source  I3C2 Clock Source
  * @{
  */
#define RCC_I3C2CLKSOURCE_PCLK1    LL_RCC_I3C2_CLKSOURCE_PCLK1
#define RCC_I3C2CLKSOURCE_CLKP     LL_RCC_I3C2_CLKSOURCE_CLKP
#define RCC_I3C2CLKSOURCE_IC10     LL_RCC_I3C2_CLKSOURCE_IC10
#define RCC_I3C2CLKSOURCE_IC15     LL_RCC_I3C2_CLKSOURCE_IC15
#define RCC_I3C2CLKSOURCE_MSI      LL_RCC_I3C2_CLKSOURCE_MSI
#define RCC_I3C2CLKSOURCE_HSI      LL_RCC_I3C2_CLKSOURCE_HSI
/**
  * @}
  */

/** @defgroup RCCEx_LPTIM1_Clock_Source  LPTIM1 Clock Source
  * @{
  */
#define RCC_LPTIM1CLKSOURCE_PCLK1  LL_RCC_LPTIM1_CLKSOURCE_PCLK1
#define RCC_LPTIM1CLKSOURCE_CLKP   LL_RCC_LPTIM1_CLKSOURCE_CLKP
#define RCC_LPTIM1CLKSOURCE_IC15   LL_RCC_LPTIM1_CLKSOURCE_IC15
#define RCC_LPTIM1CLKSOURCE_LSE    LL_RCC_LPTIM1_CLKSOURCE_LSE
#define RCC_LPTIM1CLKSOURCE_LSI    LL_RCC_LPTIM1_CLKSOURCE_LSI
#define RCC_LPTIM1CLKSOURCE_TIMG   LL_RCC_LPTIM1_CLKSOURCE_TIMG
/**
  * @}
  */

/** @defgroup RCCEx_LPTIM2_Clock_Source  LPTIM2 Clock Source
  * @{
  */
#define RCC_LPTIM2CLKSOURCE_PCLK4  LL_RCC_LPTIM2_CLKSOURCE_PCLK4
#define RCC_LPTIM2CLKSOURCE_CLKP   LL_RCC_LPTIM2_CLKSOURCE_CLKP
#define RCC_LPTIM2CLKSOURCE_IC15   LL_RCC_LPTIM2_CLKSOURCE_IC15
#define RCC_LPTIM2CLKSOURCE_LSE    LL_RCC_LPTIM2_CLKSOURCE_LSE
#define RCC_LPTIM2CLKSOURCE_LSI    LL_RCC_LPTIM2_CLKSOURCE_LSI
#define RCC_LPTIM2CLKSOURCE_TIMG   LL_RCC_LPTIM2_CLKSOURCE_TIMG
/**
  * @}
  */

/** @defgroup RCCEx_LPTIM3_Clock_Source  LPTIM3 Clock Source
  * @{
  */
#define RCC_LPTIM3CLKSOURCE_PCLK4  LL_RCC_LPTIM3_CLKSOURCE_PCLK4
#define RCC_LPTIM3CLKSOURCE_CLKP   LL_RCC_LPTIM3_CLKSOURCE_CLKP
#define RCC_LPTIM3CLKSOURCE_IC15   LL_RCC_LPTIM3_CLKSOURCE_IC15
#define RCC_LPTIM3CLKSOURCE_LSE    LL_RCC_LPTIM3_CLKSOURCE_LSE
#define RCC_LPTIM3CLKSOURCE_LSI    LL_RCC_LPTIM3_CLKSOURCE_LSI
#define RCC_LPTIM3CLKSOURCE_TIMG   LL_RCC_LPTIM3_CLKSOURCE_TIMG
/**
  * @}
  */

/** @defgroup RCCEx_LPTIM4_Clock_Source  LPTIM4 Clock Source
  * @{
  */
#define RCC_LPTIM4CLKSOURCE_PCLK4  LL_RCC_LPTIM4_CLKSOURCE_PCLK4
#define RCC_LPTIM4CLKSOURCE_CLKP   LL_RCC_LPTIM4_CLKSOURCE_CLKP
#define RCC_LPTIM4CLKSOURCE_IC15   LL_RCC_LPTIM4_CLKSOURCE_IC15
#define RCC_LPTIM4CLKSOURCE_LSE    LL_RCC_LPTIM4_CLKSOURCE_LSE
#define RCC_LPTIM4CLKSOURCE_LSI    LL_RCC_LPTIM4_CLKSOURCE_LSI
#define RCC_LPTIM4CLKSOURCE_TIMG   LL_RCC_LPTIM4_CLKSOURCE_TIMG
/**
  * @}
  */

/** @defgroup RCCEx_LPTIM5_Clock_Source  LPTIM5 Clock Source
  * @{
  */
#define RCC_LPTIM5CLKSOURCE_PCLK4  LL_RCC_LPTIM5_CLKSOURCE_PCLK4
#define RCC_LPTIM5CLKSOURCE_CLKP   LL_RCC_LPTIM5_CLKSOURCE_CLKP
#define RCC_LPTIM5CLKSOURCE_IC15   LL_RCC_LPTIM5_CLKSOURCE_IC15
#define RCC_LPTIM5CLKSOURCE_LSE    LL_RCC_LPTIM5_CLKSOURCE_LSE
#define RCC_LPTIM5CLKSOURCE_LSI    LL_RCC_LPTIM5_CLKSOURCE_LSI
#define RCC_LPTIM5CLKSOURCE_TIMG   LL_RCC_LPTIM5_CLKSOURCE_TIMG
/**
  * @}
  */

/** @defgroup RCCEx_LPUART1_Clock_Source  LPUART1 Clock Source
  * @{
  */
#define RCC_LPUART1CLKSOURCE_PCLK4 LL_RCC_LPUART1_CLKSOURCE_PCLK4
#define RCC_LPUART1CLKSOURCE_CLKP  LL_RCC_LPUART1_CLKSOURCE_CLKP
#define RCC_LPUART1CLKSOURCE_IC9   LL_RCC_LPUART1_CLKSOURCE_IC9
#define RCC_LPUART1CLKSOURCE_IC14  LL_RCC_LPUART1_CLKSOURCE_IC14
#define RCC_LPUART1CLKSOURCE_LSE   LL_RCC_LPUART1_CLKSOURCE_LSE
#define RCC_LPUART1CLKSOURCE_MSI   LL_RCC_LPUART1_CLKSOURCE_MSI
#define RCC_LPUART1CLKSOURCE_HSI   LL_RCC_LPUART1_CLKSOURCE_HSI
/**
  * @}
  */

/** @defgroup RCCEx_LTDC_Clock_Source  LTDC Clock Source
  * @{
  */
#define RCC_LTDCCLKSOURCE_PCLK5    LL_RCC_LTDC_CLKSOURCE_PCLK5
#define RCC_LTDCCLKSOURCE_CLKP     LL_RCC_LTDC_CLKSOURCE_CLKP
#define RCC_LTDCCLKSOURCE_IC16     LL_RCC_LTDC_CLKSOURCE_IC16
#define RCC_LTDCCLKSOURCE_HSI      LL_RCC_LTDC_CLKSOURCE_HSI
/**
  * @}
  */

/** @defgroup RCCEx_MDF1_Clock_Source  MDF1 Clock Source
  * @{
  */
#define RCC_MDF1CLKSOURCE_HCLK     LL_RCC_MDF1_CLKSOURCE_HCLK
#define RCC_MDF1CLKSOURCE_CLKP     LL_RCC_MDF1_CLKSOURCE_CLKP
#define RCC_MDF1CLKSOURCE_IC7      LL_RCC_MDF1_CLKSOURCE_IC7
#define RCC_MDF1CLKSOURCE_IC8      LL_RCC_MDF1_CLKSOURCE_IC8
#define RCC_MDF1CLKSOURCE_MSI      LL_RCC_MDF1_CLKSOURCE_MSI
#define RCC_MDF1CLKSOURCE_HSI      LL_RCC_MDF1_CLKSOURCE_HSI
#define RCC_MDF1CLKSOURCE_PIN      LL_RCC_MDF1_CLKSOURCE_I2S_CKIN
#define RCC_MDF1CLKSOURCE_TIMG     LL_RCC_MDF1_CLKSOURCE_TIMG
/**
  * @}
  */

/** @defgroup RCCEx_PSSI_Clock_Source  PSSI Clock Source
  * @{
  */
#define RCC_PSSICLKSOURCE_HCLK     LL_RCC_PSSI_CLKSOURCE_HCLK
#define RCC_PSSICLKSOURCE_CLKP     LL_RCC_PSSI_CLKSOURCE_CLKP
#define RCC_PSSICLKSOURCE_IC20     LL_RCC_PSSI_CLKSOURCE_IC20
#define RCC_PSSICLKSOURCE_HSI      LL_RCC_PSSI_CLKSOURCE_HSI
/**
  * @}
  */

/** @defgroup RCCEx_SAI1_Clock_Source SAI1 Clock Source
  * @{
  */
#define RCC_SAI1CLKSOURCE_PCLK2    LL_RCC_SAI1_CLKSOURCE_PCLK2
#define RCC_SAI1CLKSOURCE_CLKP     LL_RCC_SAI1_CLKSOURCE_CLKP
#define RCC_SAI1CLKSOURCE_IC7      LL_RCC_SAI1_CLKSOURCE_IC7
#define RCC_SAI1CLKSOURCE_IC8      LL_RCC_SAI1_CLKSOURCE_IC8
#define RCC_SAI1CLKSOURCE_MSI      LL_RCC_SAI1_CLKSOURCE_MSI
#define RCC_SAI1CLKSOURCE_HSI      LL_RCC_SAI1_CLKSOURCE_HSI
#define RCC_SAI1CLKSOURCE_PIN      LL_RCC_SAI1_CLKSOURCE_I2S_CKIN
#define RCC_SAI1CLKSOURCE_SPDIFRX1 LL_RCC_SAI1_CLKSOURCE_SPDIFRX1
/**
  * @}
  */

/** @defgroup RCCEx_SAI2_Clock_Source SAI2 Clock Source
  * @{
  */
#define RCC_SAI2CLKSOURCE_PCLK2    LL_RCC_SAI2_CLKSOURCE_PCLK2
#define RCC_SAI2CLKSOURCE_CLKP     LL_RCC_SAI2_CLKSOURCE_CLKP
#define RCC_SAI2CLKSOURCE_IC7      LL_RCC_SAI2_CLKSOURCE_IC7
#define RCC_SAI2CLKSOURCE_IC8      LL_RCC_SAI2_CLKSOURCE_IC8
#define RCC_SAI2CLKSOURCE_MSI      LL_RCC_SAI2_CLKSOURCE_MSI
#define RCC_SAI2CLKSOURCE_HSI      LL_RCC_SAI2_CLKSOURCE_HSI
#define RCC_SAI2CLKSOURCE_PIN      LL_RCC_SAI2_CLKSOURCE_I2S_CKIN
#define RCC_SAI2CLKSOURCE_SPDIFRX1 LL_RCC_SAI2_CLKSOURCE_SPDIFRX1
/**
  * @}
  */

/** @defgroup RCCEx_SDMMC1_Clock_Source  SDMMC1 Clock Source
  * @{
  */
#define RCC_SDMMC1CLKSOURCE_HCLK   LL_RCC_SDMMC1_CLKSOURCE_HCLK
#define RCC_SDMMC1CLKSOURCE_CLKP   LL_RCC_SDMMC1_CLKSOURCE_CLKP
#define RCC_SDMMC1CLKSOURCE_IC4    LL_RCC_SDMMC1_CLKSOURCE_IC4
#define RCC_SDMMC1CLKSOURCE_IC5    LL_RCC_SDMMC1_CLKSOURCE_IC5
/**
  * @}
  */

/** @defgroup RCCEx_SDMMC2_Clock_Source  SDMMC2 Clock Source
  * @{
  */
#define RCC_SDMMC2CLKSOURCE_HCLK   LL_RCC_SDMMC2_CLKSOURCE_HCLK
#define RCC_SDMMC2CLKSOURCE_CLKP   LL_RCC_SDMMC2_CLKSOURCE_CLKP
#define RCC_SDMMC2CLKSOURCE_IC4    LL_RCC_SDMMC2_CLKSOURCE_IC4
#define RCC_SDMMC2CLKSOURCE_IC5    LL_RCC_SDMMC2_CLKSOURCE_IC5
/**
  * @}
  */

/** @defgroup RCCEx_SPDIFRX1_Clock_Source  SPDIFRX1 Clock Source
  * @{
  */
#define RCC_SPDIFRX1CLKSOURCE_PCLK1 LL_RCC_SPDIFRX1_CLKSOURCE_PCLK1
#define RCC_SPDIFRX1CLKSOURCE_CLKP  LL_RCC_SPDIFRX1_CLKSOURCE_CLKP
#define RCC_SPDIFRX1CLKSOURCE_IC7   LL_RCC_SPDIFRX1_CLKSOURCE_IC7
#define RCC_SPDIFRX1CLKSOURCE_IC8   LL_RCC_SPDIFRX1_CLKSOURCE_IC8
#define RCC_SPDIFRX1CLKSOURCE_MSI   LL_RCC_SPDIFRX1_CLKSOURCE_MSI
#define RCC_SPDIFRX1CLKSOURCE_HSI   LL_RCC_SPDIFRX1_CLKSOURCE_HSI
#define RCC_SPDIFRX1CLKSOURCE_PIN   LL_RCC_SPDIFRX1_CLKSOURCE_I2S_CKIN
/**
  * @}
  */

/** @defgroup RCCEx_SPI1_Clock_Source  SPI1 Clock Source
  * @{
  */
#define RCC_SPI1CLKSOURCE_PCLK2    LL_RCC_SPI1_CLKSOURCE_PCLK2
#define RCC_SPI1CLKSOURCE_CLKP     LL_RCC_SPI1_CLKSOURCE_CLKP
#define RCC_SPI1CLKSOURCE_IC8      LL_RCC_SPI1_CLKSOURCE_IC8
#define RCC_SPI1CLKSOURCE_IC9      LL_RCC_SPI1_CLKSOURCE_IC9
#define RCC_SPI1CLKSOURCE_MSI      LL_RCC_SPI1_CLKSOURCE_MSI
#define RCC_SPI1CLKSOURCE_HSI      LL_RCC_SPI1_CLKSOURCE_HSI
#define RCC_SPI1CLKSOURCE_PIN      LL_RCC_SPI1_CLKSOURCE_I2S_CKIN
/**
  * @}
  */

/** @defgroup RCCEx_SPI2_Clock_Source  SPI2 Clock Source
  * @{
  */
#define RCC_SPI2CLKSOURCE_PCLK1    LL_RCC_SPI2_CLKSOURCE_PCLK1
#define RCC_SPI2CLKSOURCE_CLKP     LL_RCC_SPI2_CLKSOURCE_CLKP
#define RCC_SPI2CLKSOURCE_IC8      LL_RCC_SPI2_CLKSOURCE_IC8
#define RCC_SPI2CLKSOURCE_IC9      LL_RCC_SPI2_CLKSOURCE_IC9
#define RCC_SPI2CLKSOURCE_MSI      LL_RCC_SPI2_CLKSOURCE_MSI
#define RCC_SPI2CLKSOURCE_HSI      LL_RCC_SPI2_CLKSOURCE_HSI
#define RCC_SPI2CLKSOURCE_PIN      LL_RCC_SPI2_CLKSOURCE_I2S_CKIN
/**
  * @}
  */

/** @defgroup RCCEx_SPI3_Clock_Source  SPI3 Clock Source
  * @{
  */
#define RCC_SPI3CLKSOURCE_PCLK1    LL_RCC_SPI3_CLKSOURCE_PCLK1
#define RCC_SPI3CLKSOURCE_CLKP     LL_RCC_SPI3_CLKSOURCE_CLKP
#define RCC_SPI3CLKSOURCE_IC8      LL_RCC_SPI3_CLKSOURCE_IC8
#define RCC_SPI3CLKSOURCE_IC9      LL_RCC_SPI3_CLKSOURCE_IC9
#define RCC_SPI3CLKSOURCE_MSI      LL_RCC_SPI3_CLKSOURCE_MSI
#define RCC_SPI3CLKSOURCE_HSI      LL_RCC_SPI3_CLKSOURCE_HSI
#define RCC_SPI3CLKSOURCE_PIN      LL_RCC_SPI3_CLKSOURCE_I2S_CKIN
/**
  * @}
  */

/** @defgroup RCCEx_SPI4_Clock_Source  SPI4 Clock Source
  * @{
  */
#define RCC_SPI4CLKSOURCE_PCLK2    LL_RCC_SPI4_CLKSOURCE_PCLK2
#define RCC_SPI4CLKSOURCE_CLKP     LL_RCC_SPI4_CLKSOURCE_CLKP
#define RCC_SPI4CLKSOURCE_IC9      LL_RCC_SPI4_CLKSOURCE_IC9
#define RCC_SPI4CLKSOURCE_IC14     LL_RCC_SPI4_CLKSOURCE_IC14
#define RCC_SPI4CLKSOURCE_MSI      LL_RCC_SPI4_CLKSOURCE_MSI
#define RCC_SPI4CLKSOURCE_HSI      LL_RCC_SPI4_CLKSOURCE_HSI
#define RCC_SPI4CLKSOURCE_HSE      LL_RCC_SPI4_CLKSOURCE_HSE
/**
  * @}
  */

/** @defgroup RCCEx_SPI5_Clock_Source  SPI5 Clock Source
  * @{
  */
#define RCC_SPI5CLKSOURCE_PCLK2    LL_RCC_SPI5_CLKSOURCE_PCLK2
#define RCC_SPI5CLKSOURCE_CLKP     LL_RCC_SPI5_CLKSOURCE_CLKP
#define RCC_SPI5CLKSOURCE_IC9      LL_RCC_SPI5_CLKSOURCE_IC9
#define RCC_SPI5CLKSOURCE_IC14     LL_RCC_SPI5_CLKSOURCE_IC14
#define RCC_SPI5CLKSOURCE_MSI      LL_RCC_SPI5_CLKSOURCE_MSI
#define RCC_SPI5CLKSOURCE_HSI      LL_RCC_SPI5_CLKSOURCE_HSI
#define RCC_SPI5CLKSOURCE_HSE      LL_RCC_SPI5_CLKSOURCE_HSE
/**
  * @}
  */

/** @defgroup RCCEx_SPI6_Clock_Source  SPI6 Clock Source
  * @{
  */
#define RCC_SPI6CLKSOURCE_PCLK4    LL_RCC_SPI6_CLKSOURCE_PCLK4
#define RCC_SPI6CLKSOURCE_CLKP     LL_RCC_SPI6_CLKSOURCE_CLKP
#define RCC_SPI6CLKSOURCE_IC8      LL_RCC_SPI6_CLKSOURCE_IC8
#define RCC_SPI6CLKSOURCE_IC9      LL_RCC_SPI6_CLKSOURCE_IC9
#define RCC_SPI6CLKSOURCE_MSI      LL_RCC_SPI6_CLKSOURCE_MSI
#define RCC_SPI6CLKSOURCE_HSI      LL_RCC_SPI6_CLKSOURCE_HSI
#define RCC_SPI6CLKSOURCE_PIN      LL_RCC_SPI6_CLKSOURCE_I2S_CKIN
/**
  * @}
  */

/** @defgroup RCCEx_USART1_Clock_Source  USART1 Clock Source
  * @{
  */
#define RCC_USART1CLKSOURCE_PCLK2  LL_RCC_USART1_CLKSOURCE_PCLK2
#define RCC_USART1CLKSOURCE_CLKP   LL_RCC_USART1_CLKSOURCE_CLKP
#define RCC_USART1CLKSOURCE_IC9    LL_RCC_USART1_CLKSOURCE_IC9
#define RCC_USART1CLKSOURCE_IC14   LL_RCC_USART1_CLKSOURCE_IC14
#define RCC_USART1CLKSOURCE_LSE    LL_RCC_USART1_CLKSOURCE_LSE
#define RCC_USART1CLKSOURCE_MSI    LL_RCC_USART1_CLKSOURCE_MSI
#define RCC_USART1CLKSOURCE_HSI    LL_RCC_USART1_CLKSOURCE_HSI
/**
  * @}
  */

/** @defgroup RCCEx_USART2_Clock_Source  USART2 Clock Source
  * @{
  */
#define RCC_USART2CLKSOURCE_PCLK1  LL_RCC_USART2_CLKSOURCE_PCLK1
#define RCC_USART2CLKSOURCE_CLKP   LL_RCC_USART2_CLKSOURCE_CLKP
#define RCC_USART2CLKSOURCE_IC9    LL_RCC_USART2_CLKSOURCE_IC9
#define RCC_USART2CLKSOURCE_IC14   LL_RCC_USART2_CLKSOURCE_IC14
#define RCC_USART2CLKSOURCE_LSE    LL_RCC_USART2_CLKSOURCE_LSE
#define RCC_USART2CLKSOURCE_MSI    LL_RCC_USART2_CLKSOURCE_MSI
#define RCC_USART2CLKSOURCE_HSI    LL_RCC_USART2_CLKSOURCE_HSI
/**
  * @}
  */

/** @defgroup RCCEx_USART3_Clock_Source  USART3 Clock Source
  * @{
  */
#define RCC_USART3CLKSOURCE_PCLK1  LL_RCC_USART3_CLKSOURCE_PCLK1
#define RCC_USART3CLKSOURCE_CLKP   LL_RCC_USART3_CLKSOURCE_CLKP
#define RCC_USART3CLKSOURCE_IC9    LL_RCC_USART3_CLKSOURCE_IC9
#define RCC_USART3CLKSOURCE_IC14   LL_RCC_USART3_CLKSOURCE_IC14
#define RCC_USART3CLKSOURCE_LSE    LL_RCC_USART3_CLKSOURCE_LSE
#define RCC_USART3CLKSOURCE_MSI    LL_RCC_USART3_CLKSOURCE_MSI
#define RCC_USART3CLKSOURCE_HSI    LL_RCC_USART3_CLKSOURCE_HSI
/**
  * @}
  */

/** @defgroup RCCEx_UART4_Clock_Source  UART4 Clock Source
  * @{
  */
#define RCC_UART4CLKSOURCE_PCLK1   LL_RCC_UART4_CLKSOURCE_PCLK1
#define RCC_UART4CLKSOURCE_CLKP    LL_RCC_UART4_CLKSOURCE_CLKP
#define RCC_UART4CLKSOURCE_IC9     LL_RCC_UART4_CLKSOURCE_IC9
#define RCC_UART4CLKSOURCE_IC14    LL_RCC_UART4_CLKSOURCE_IC14
#define RCC_UART4CLKSOURCE_LSE     LL_RCC_UART4_CLKSOURCE_LSE
#define RCC_UART4CLKSOURCE_MSI     LL_RCC_UART4_CLKSOURCE_MSI
#define RCC_UART4CLKSOURCE_HSI     LL_RCC_UART4_CLKSOURCE_HSI
/**
  * @}
  */

/** @defgroup RCCEx_UART5_Clock_Source  UART5 Clock Source
  * @{
  */
#define RCC_UART5CLKSOURCE_PCLK1   LL_RCC_UART5_CLKSOURCE_PCLK1
#define RCC_UART5CLKSOURCE_CLKP    LL_RCC_UART5_CLKSOURCE_CLKP
#define RCC_UART5CLKSOURCE_IC9     LL_RCC_UART5_CLKSOURCE_IC9
#define RCC_UART5CLKSOURCE_IC14    LL_RCC_UART5_CLKSOURCE_IC14
#define RCC_UART5CLKSOURCE_LSE     LL_RCC_UART5_CLKSOURCE_LSE
#define RCC_UART5CLKSOURCE_MSI     LL_RCC_UART5_CLKSOURCE_MSI
#define RCC_UART5CLKSOURCE_HSI     LL_RCC_UART5_CLKSOURCE_HSI
/**
  * @}
  */

/** @defgroup RCCEx_USART6_Clock_Source  USART6 Clock Source
  * @{
  */
#define RCC_USART6CLKSOURCE_PCLK2  LL_RCC_USART6_CLKSOURCE_PCLK2
#define RCC_USART6CLKSOURCE_CLKP   LL_RCC_USART6_CLKSOURCE_CLKP
#define RCC_USART6CLKSOURCE_IC9    LL_RCC_USART6_CLKSOURCE_IC9
#define RCC_USART6CLKSOURCE_IC14   LL_RCC_USART6_CLKSOURCE_IC14
#define RCC_USART6CLKSOURCE_LSE    LL_RCC_USART6_CLKSOURCE_LSE
#define RCC_USART6CLKSOURCE_MSI    LL_RCC_USART6_CLKSOURCE_MSI
#define RCC_USART6CLKSOURCE_HSI    LL_RCC_USART6_CLKSOURCE_HSI
/**
  * @}
  */

/** @defgroup RCCEx_UART7_Clock_Source  UART7 Clock Source
  * @{
  */
#define RCC_UART7CLKSOURCE_PCLK1   LL_RCC_UART7_CLKSOURCE_PCLK1
#define RCC_UART7CLKSOURCE_CLKP    LL_RCC_UART7_CLKSOURCE_CLKP
#define RCC_UART7CLKSOURCE_IC9     LL_RCC_UART7_CLKSOURCE_IC9
#define RCC_UART7CLKSOURCE_IC14    LL_RCC_UART7_CLKSOURCE_IC14
#define RCC_UART7CLKSOURCE_LSE     LL_RCC_UART7_CLKSOURCE_LSE
#define RCC_UART7CLKSOURCE_MSI     LL_RCC_UART7_CLKSOURCE_MSI
#define RCC_UART7CLKSOURCE_HSI     LL_RCC_UART7_CLKSOURCE_HSI
/**
  * @}
  */

/** @defgroup RCCEx_UART8_Clock_Source  UART8 Clock Source
  * @{
  */
#define RCC_UART8CLKSOURCE_PCLK1   LL_RCC_UART8_CLKSOURCE_PCLK1
#define RCC_UART8CLKSOURCE_CLKP    LL_RCC_UART8_CLKSOURCE_CLKP
#define RCC_UART8CLKSOURCE_IC9     LL_RCC_UART8_CLKSOURCE_IC9
#define RCC_UART8CLKSOURCE_IC14    LL_RCC_UART8_CLKSOURCE_IC14
#define RCC_UART8CLKSOURCE_LSE     LL_RCC_UART8_CLKSOURCE_LSE
#define RCC_UART8CLKSOURCE_MSI     LL_RCC_UART8_CLKSOURCE_MSI
#define RCC_UART8CLKSOURCE_HSI     LL_RCC_UART8_CLKSOURCE_HSI
/**
  * @}
  */

/** @defgroup RCCEx_UART9_Clock_Source  UART9 Clock Source
  * @{
  */
#define RCC_UART9CLKSOURCE_PCLK2   LL_RCC_UART9_CLKSOURCE_PCLK2
#define RCC_UART9CLKSOURCE_CLKP    LL_RCC_UART9_CLKSOURCE_CLKP
#define RCC_UART9CLKSOURCE_IC9     LL_RCC_UART9_CLKSOURCE_IC9
#define RCC_UART9CLKSOURCE_IC14    LL_RCC_UART9_CLKSOURCE_IC14
#define RCC_UART9CLKSOURCE_LSE     LL_RCC_UART9_CLKSOURCE_LSE
#define RCC_UART9CLKSOURCE_MSI     LL_RCC_UART9_CLKSOURCE_MSI
#define RCC_UART9CLKSOURCE_HSI     LL_RCC_UART9_CLKSOURCE_HSI
/**
  * @}
  */

/** @defgroup RCCEx_USART10_Clock_Source  USART10 Clock Source
  * @{
  */
#define RCC_USART10CLKSOURCE_PCLK2 LL_RCC_USART10_CLKSOURCE_PCLK2
#define RCC_USART10CLKSOURCE_CLKP  LL_RCC_USART10_CLKSOURCE_CLKP
#define RCC_USART10CLKSOURCE_IC9   LL_RCC_USART10_CLKSOURCE_IC9
#define RCC_USART10CLKSOURCE_IC14  LL_RCC_USART10_CLKSOURCE_IC14
#define RCC_USART10CLKSOURCE_LSE   LL_RCC_USART10_CLKSOURCE_LSE
#define RCC_USART10CLKSOURCE_MSI   LL_RCC_USART10_CLKSOURCE_MSI
#define RCC_USART10CLKSOURCE_HSI   LL_RCC_USART10_CLKSOURCE_HSI
/**
  * @}
  */

/** @defgroup RCCEx_USBPHY1_Clock_Source  USBPHY1 Clock Source
  * @{
  */
#define RCC_USBPHY1REFCLKSOURCE_OTGPHY1             LL_RCC_OTGPHY1CKREF_CLKSOURCE_OTGPHY1
#define RCC_USBPHY1REFCLKSOURCE_HSE_DIRECT          LL_RCC_OTGPHY1CKREF_CLKSOURCE_HSE_DIV_2_OSC
#define RCC_USBPHY1REFCLKSOURCE_HSE_DIRECT_DIV2     (LL_RCC_OTGPHY1CKREF_CLKSOURCE_HSE_DIV_2_OSC | 0x80000000UL)
/**
  * @}
  */

/** @defgroup RCCEx_USBPHY2_Clock_Source  USBPHY2 Clock Source
  * @{
  */
#define RCC_USBPHY2REFCLKSOURCE_OTGPHY2             LL_RCC_OTGPHY2CKREF_CLKSOURCE_OTGPHY2
#define RCC_USBPHY2REFCLKSOURCE_HSE_DIRECT          LL_RCC_OTGPHY2CKREF_CLKSOURCE_HSE_DIV_2_OSC
#define RCC_USBPHY2REFCLKSOURCE_HSE_DIRECT_DIV2     (LL_RCC_OTGPHY2CKREF_CLKSOURCE_HSE_DIV_2_OSC | 0x80000000UL)
/**
  * @}
  */

/** @defgroup RCCEx_USB_OTGHS1_Clock_Source  USB OTGHS1 Clock Source
  * @{
  */
#define RCC_USBOTGHS1CLKSOURCE_HSE_DIV2             LL_RCC_OTGPHY1_CLKSOURCE_HSE_DIV_2
#define RCC_USBOTGHS1CLKSOURCE_CLKP                 LL_RCC_OTGPHY1_CLKSOURCE_CLKP
#define RCC_USBOTGHS1CLKSOURCE_IC15                 LL_RCC_OTGPHY1_CLKSOURCE_IC15
#define RCC_USBOTGHS1CLKSOURCE_HSE_DIRECT           LL_RCC_OTGPHY1_CLKSOURCE_HSE_DIV_2_OSC
#define RCC_USBOTGHS1CLKSOURCE_HSE_DIRECT_DIV2      (LL_RCC_OTGPHY1_CLKSOURCE_HSE_DIV_2_OSC | 0x80000000UL)
/**
  * @}
  */

/** @defgroup RCCEx_USB_OTGHS2_Clock_Source  USB OTGHS2 Clock Source
  * @{
  */
#define RCC_USBOTGHS2CLKSOURCE_HSE_DIV2             LL_RCC_OTGPHY2_CLKSOURCE_HSE_DIV_2
#define RCC_USBOTGHS2CLKSOURCE_CLKP                 LL_RCC_OTGPHY2_CLKSOURCE_CLKP
#define RCC_USBOTGHS2CLKSOURCE_IC15                 LL_RCC_OTGPHY2_CLKSOURCE_IC15
#define RCC_USBOTGHS2CLKSOURCE_HSE_DIRECT           LL_RCC_OTGPHY2_CLKSOURCE_HSE_DIV_2_OSC
#define RCC_USBOTGHS2CLKSOURCE_HSE_DIRECT_DIV2      (LL_RCC_OTGPHY2_CLKSOURCE_HSE_DIV_2_OSC | 0x80000000UL)
/**
  * @}
  */

/** @defgroup RCCEx_XSPI1_Clock_Source  XSPI1 Clock Source
  * @{
  */
#define RCC_XSPI1CLKSOURCE_HCLK    LL_RCC_XSPI1_CLKSOURCE_HCLK
#define RCC_XSPI1CLKSOURCE_CLKP    LL_RCC_XSPI1_CLKSOURCE_CLKP
#define RCC_XSPI1CLKSOURCE_IC3     LL_RCC_XSPI1_CLKSOURCE_IC3
#define RCC_XSPI1CLKSOURCE_IC4     LL_RCC_XSPI1_CLKSOURCE_IC4
/**
  * @}
  */

/** @defgroup RCCEx_XSPI2_Clock_Source  XSPI2 Clock Source
  * @{
  */
#define RCC_XSPI2CLKSOURCE_HCLK    LL_RCC_XSPI2_CLKSOURCE_HCLK
#define RCC_XSPI2CLKSOURCE_CLKP    LL_RCC_XSPI2_CLKSOURCE_CLKP
#define RCC_XSPI2CLKSOURCE_IC3     LL_RCC_XSPI2_CLKSOURCE_IC3
#define RCC_XSPI2CLKSOURCE_IC4     LL_RCC_XSPI2_CLKSOURCE_IC4
/**
  * @}
  */

/** @defgroup RCCEx_XSPI3_Clock_Source  XSPI3 Clock Source
  * @{
  */
#define RCC_XSPI3CLKSOURCE_HCLK    LL_RCC_XSPI3_CLKSOURCE_HCLK
#define RCC_XSPI3CLKSOURCE_CLKP    LL_RCC_XSPI3_CLKSOURCE_CLKP
#define RCC_XSPI3CLKSOURCE_IC3     LL_RCC_XSPI3_CLKSOURCE_IC3
#define RCC_XSPI3CLKSOURCE_IC4     LL_RCC_XSPI3_CLKSOURCE_IC4
/**
  * @}
  */


/** @defgroup RCCEx_TIM_Prescaler_Selection TIM Prescaler Selection
  * @{
  */
#define RCC_TIMPRES_DIV1           LL_RCC_TIM_PRESCALER_1     /*!< Timers clocks prescaler divide by 1 */
#define RCC_TIMPRES_DIV2           LL_RCC_TIM_PRESCALER_2     /*!< Timers clocks prescaler divide by 2 */
#define RCC_TIMPRES_DIV4           LL_RCC_TIM_PRESCALER_4     /*!< Timers clocks prescaler divide by 4 */
#define RCC_TIMPRES_DIV8           LL_RCC_TIM_PRESCALER_8     /*!< Timers clocks prescaler divide by 8 */
/**
  * @}
  */

/** @defgroup RCCEx_PERIPH_FREQUENCY Peripheral clock frequency
  * @{
  */
#define RCC_PERIPH_FREQUENCY_NO        LL_RCC_PERIPH_FREQUENCY_NO /*!< No clock enabled for the peripheral            */
#define RCC_PERIPH_FREQUENCY_NA        LL_RCC_PERIPH_FREQUENCY_NA /*!< Frequency cannot be provided as external clock */
/**
  * @}
  */

/** @defgroup RCCEx_IC_Selection_Max  Maximum IC Selection
  * @{
  */
#define RCC_IC_MAX_NUMBER              20U
/**
  * @}
  */

/** @defgroup RCCEx_IC_Selection  IC Selection
  * @{
  */
#define RCC_IC1                         0U   /*!< IC1 source is PLL1 output by default */
#define RCC_IC2                         1U   /*!< IC2 source is PLL1 output by default */
#define RCC_IC3                         2U   /*!< IC3 source is PLL1 output by default */
#define RCC_IC4                         3U   /*!< IC4 source is PLL1 output by default */
#define RCC_IC5                         4U   /*!< IC5 source is PLL1 output by default */
#define RCC_IC6                         5U   /*!< IC6 source is PLL1 output by default */
#define RCC_IC7                         6U   /*!< IC7 source is PLL2 output by default */
#define RCC_IC8                         7U   /*!< IC8 source is PLL2 output by default */
#define RCC_IC9                         8U   /*!< IC9 source is PLL2 output by default */
#define RCC_IC10                        9U   /*!< IC10 source is PLL2 output by default */
#define RCC_IC11                       10U   /*!< IC11 source is PLL1 output by default */
#define RCC_IC12                       11U   /*!< IC12 source is PLL3 output by default */
#define RCC_IC13                       12U   /*!< IC13 source is PLL3 output by default */
#define RCC_IC14                       13U   /*!< IC14 source is PLL3 output by default */
#define RCC_IC15                       14U   /*!< IC15 source is PLL3 output by default */
#define RCC_IC16                       15U   /*!< IC16 source is PLL4 output by default */
#define RCC_IC17                       16U   /*!< IC17 source is PLL4 output by default */
#define RCC_IC18                       17U   /*!< IC18 source is PLL4 output by default */
#define RCC_IC19                       18U   /*!< IC19 source is PLL4 output by default */
#define RCC_IC20                       19U   /*!< IC20 source is PLL4 output by default */
/**
  * @}
  */

/** @defgroup RCCEx_EXTI_LINE_LSECSS  LSE CSS external interrupt line
  * @{
  */
#define RCC_EXTI_LINE_LSECSS           EXTI_IMR3_IM65        /*!< LSE CSS failure interrupt connected to direct EXTI line 65 (Tamper) */
/**
  * @}
  */

/** @defgroup RCC_PLL_Selection  PLL selection
  * @{
  */
#define RCC_PLL1           0U
#define RCC_PLL2           1U
#define RCC_PLL3           2U
#define RCC_PLL4           3U
/**
  * @}
  */

/** @defgroup RCC_PLL_SSCGSpread_Mode  PLL SSCG spread mode selection
  * @{
  */
#define RCC_PLL_SSCG_CENTER            0U                  /*!< PLL SSCG center spread modulation */
#define RCC_PLL_SSCG_DOWN              1U                  /*!< PLL SSCG down spread modulation */
/**
  * @}
  */

/**
  * @}
  */


/* Exported types ------------------------------------------------------------*/
/** @defgroup RCCEx_Exported_Types RCCEx Exported Types
  * @{
  */

/**
  * @brief  RCC extended clocks structure definition
  */
typedef struct
{
  uint64_t PeriphClockSelection;   /*!< The Extended Clock to be configured.
                                        This parameter can be a value of @ref RCCEx_Periph_Clock_Selection */

  RCC_ICInitTypeDef ICSelection[RCC_IC_MAX_NUMBER]; /*!< ICx structure parameters.
                                        This parameter shall be used when ICx is selected as kernel clock source
                                        for some peripherals */

  uint32_t FmcClockSelection;      /*!< Specifies FMC clock source.
                                        This parameter can be a value of @ref RCCEx_FMC_Clock_Source    */

  uint32_t Xspi1ClockSelection;    /*!< Specifies XSPI1 clock source.
                                        This parameter can be a value of @ref RCCEx_XSPI1_Clock_Source  */

  uint32_t Xspi2ClockSelection;    /*!< Specifies XSPI2 clock source.
                                        This parameter can be a value of @ref RCCEx_XSPI2_Clock_Source  */

  uint32_t Xspi3ClockSelection;    /*!< Specifies XSPI3 clock source.
                                        This parameter can be a value of @ref RCCEx_XSPI3_Clock_Source  */

  uint32_t CkperClockSelection;   /*!< Specifies CKPER clock source.
                                        This parameter can be a value of @ref RCCEx_CLKP_Clock_Source   */

  uint32_t AdcClockSelection;      /*!< Specifies ADC interface clock source.
                                        This parameter can be a value of @ref RCCEx_ADC_Clock_Source     */

  uint32_t AdcDivider;             /*!< Specifies ADC clock divider.
                                        This parameter can be a value between 1 to 256 */

  uint32_t Adf1ClockSelection;     /*!< Specifies ADF1 Clock clock source.
                                        This parameter can be a value of @ref RCCEx_ADF1_Clock_Source    */

  uint32_t DcmippClockSelection;   /*!< Specifies DCMIPP clock source.
                                        This parameter can be a value of @ref RCCEx_DCMIPP_Clock_Source  */

  uint32_t Eth1ClockSelection;     /*!< Specifies ETH1 clock source.
                                        This parameter can be a value of @ref RCCEx_ETH1_Clock_Source    */

  uint32_t Eth1PhyInterfaceSelection;  /*!< Specifies ETH1 PHY interface.
                                        This parameter can be a value of @ref RCCEx_ETH1_PHY_Interface */

  uint32_t Eth1RxClockSelection;    /*!< Specifies ETH1 RX clock source.
                                        This parameter can be a value of @ref RCCEx_ETH1_RX_Clock_Source */

  uint32_t Eth1TxClockSelection;    /*!< Specifies ETH1 TX clock source.
                                        This parameter can be a value of @ref RCCEx_ETH1_TX_Clock_Source */

  uint32_t Eth1PtpClockSelection;   /*!< Specifies ETH1 PTP clock source.
                                        This parameter can be a value of @ref RCCEx_ETH1_PTP_Clock_Source */

  uint32_t Eth1PtpDivider;          /*!< Specifies ETH1 PTP clock divider.
                                        This parameter can be a value between 1 to 16 */

  uint32_t FdcanClockSelection;    /*!< Specifies FDCAN kernel clock source.
                                        This parameter can be a value of @ref RCCEx_FDCAN_Clock_Source   */

  uint32_t I2c1ClockSelection;     /*!< Specifies I2C1 clock source.
                                        This parameter can be a value of @ref RCCEx_I2C1_Clock_Source    */

  uint32_t I2c2ClockSelection;     /*!< Specifies I2C2 clock source.
                                        This parameter can be a value of @ref RCCEx_I2C2_Clock_Source    */

  uint32_t I2c3ClockSelection;     /*!< Specifies I2C3 clock source.
                                        This parameter can be a value of @ref RCCEx_I2C3_Clock_Source    */

  uint32_t I2c4ClockSelection;     /*!< Specifies I2C4 clock source.
                                        This parameter can be a value of @ref RCCEx_I2C4_Clock_Source    */

  uint32_t I3c1ClockSelection;     /*!< Specifies I3C1 clock source.
                                        This parameter can be a value of @ref RCCEx_I3C1_Clock_Source    */

  uint32_t I3c2ClockSelection;     /*!< Specifies I3C2 clock source.
                                        This parameter can be a value of @ref RCCEx_I3C2_Clock_Source    */

  uint32_t Lptim1ClockSelection;   /*!< Specifies LPTIM1 clock source.
                                        This parameter can be a value of @ref RCCEx_LPTIM1_Clock_Source  */

  uint32_t Lptim2ClockSelection;   /*!< Specifies LPTIM2 clock source.
                                        This parameter can be a value of @ref RCCEx_LPTIM2_Clock_Source  */

  uint32_t Lptim3ClockSelection;   /*!< Specifies LPTIM3 clock source.
                                        This parameter can be a value of @ref RCCEx_LPTIM3_Clock_Source  */

  uint32_t Lptim4ClockSelection;   /*!< Specifies LPTIM4 clock source.
                                        This parameter can be a value of @ref RCCEx_LPTIM4_Clock_Source  */

  uint32_t Lptim5ClockSelection;   /*!< Specifies LPTIM5 clock source.
                                        This parameter can be a value of @ref RCCEx_LPTIM5_Clock_Source  */

  uint32_t Lpuart1ClockSelection;  /*!< Specifies LPUART1 clock source.
                                        This parameter can be a value of @ref RCCEx_LPUART1_Clock_Source */

  uint32_t LtdcClockSelection;     /*!< Specifies LPUART1 clock source.
                                        This parameter can be a value of @ref RCCEx_LTDC_Clock_Source */

  uint32_t Mdf1ClockSelection;     /*!< Specifies MDF1 Clock clock source.
                                        This parameter can be a value of @ref RCCEx_MDF1_Clock_Source  */

  uint32_t PssiClockSelection;     /*!< Specifies PSSI clock source.
                                        This parameter can be a value of @ref RCCEx_PSSI_Clock_Source   */

  uint32_t Sai1ClockSelection;     /*!< Specifies SAI1 clock source.
                                        This parameter can be a value of @ref RCCEx_SAI1_Clock_Source    */

  uint32_t Sai2ClockSelection;     /*!< Specifies SAI2 clock source.
                                        This parameter can be a value of @ref RCCEx_SAI2_Clock_Source    */

  uint32_t Sdmmc1ClockSelection;   /*!< Specifies SDMMC1 clock source.
                                        This parameter can be a value of @ref RCCEx_SDMMC1_Clock_Source  */

  uint32_t Sdmmc2ClockSelection;   /*!< Specifies SDMMC2 clock source.
                                        This parameter can be a value of @ref RCCEx_SDMMC2_Clock_Source  */

  uint32_t Spi1ClockSelection;     /*!< Specifies SPI1 clock source.
                                        This parameter can be a value of @ref RCCEx_SPI1_Clock_Source    */

  uint32_t Spi2ClockSelection;     /*!< Specifies SPI2 clock source.
                                        This parameter can be a value of @ref RCCEx_SPI2_Clock_Source    */

  uint32_t Spi3ClockSelection;     /*!< Specifies SPI3 clock source.
                                        This parameter can be a value of @ref RCCEx_SPI3_Clock_Source    */

  uint32_t Spi4ClockSelection;     /*!< Specifies SPI4 clock source.
                                        This parameter can be a value of @ref RCCEx_SPI4_Clock_Source    */

  uint32_t Spi5ClockSelection;     /*!< Specifies SPI5 clock source.
                                        This parameter can be a value of @ref RCCEx_SPI5_Clock_Source    */

  uint32_t Spi6ClockSelection;     /*!< Specifies SPI6 clock source.
                                        This parameter can be a value of @ref RCCEx_SPI6_Clock_Source    */

  uint32_t Spdifrx1ClockSelection; /*!< Specifies SPDIFRX1 Clock clock source.
                                        This parameter can be a value of @ref RCCEx_SPDIFRX1_Clock_Source */

  uint32_t Usart1ClockSelection;   /*!< Specifies USART1 clock source.
                                        This parameter can be a value of @ref RCCEx_USART1_Clock_Source  */

  uint32_t Usart2ClockSelection;   /*!< Specifies USART2 clock source.
                                        This parameter can be a value of @ref RCCEx_USART2_Clock_Source  */

  uint32_t Usart3ClockSelection;   /*!< Specifies USART3 clock source.
                                        This parameter can be a value of @ref RCCEx_USART3_Clock_Source  */

  uint32_t Uart4ClockSelection;    /*!< Specifies UART4 clock source.
                                        This parameter can be a value of @ref RCCEx_UART4_Clock_Source  */

  uint32_t Uart5ClockSelection;    /*!< Specifies UART5 clock source.
                                        This parameter can be a value of @ref RCCEx_UART5_Clock_Source  */

  uint32_t Usart6ClockSelection;   /*!< Specifies USART6 clock source.
                                        This parameter can be a value of @ref RCCEx_USART6_Clock_Source  */

  uint32_t Uart7ClockSelection;    /*!< Specifies UART7 clock source.
                                        This parameter can be a value of @ref RCCEx_UART7_Clock_Source  */

  uint32_t Uart8ClockSelection;    /*!< Specifies UART8 clock source.
                                        This parameter can be a value of @ref RCCEx_UART8_Clock_Source  */

  uint32_t Uart9ClockSelection;    /*!< Specifies UART9 clock source.
                                        This parameter can be a value of @ref RCCEx_UART9_Clock_Source  */

  uint32_t Usart10ClockSelection;  /*!< Specifies USART10 clock source.
                                        This parameter can be a value of @ref RCCEx_USART10_Clock_Source  */

  uint32_t UsbPhy1ClockSelection;  /*!< Specifies USBPHY1 clock source.
                                        This parameter can be a value of @ref RCCEx_USBPHY1_Clock_Source */

  uint32_t UsbOtgHs1ClockSelection; /*!< Specifies USB OTG HS1 clock source.
                                        This parameter can be a value of @ref RCCEx_USB_OTGHS1_Clock_Source */

  uint32_t UsbPhy2ClockSelection;  /*!< Specifies USBPHY2 clock source.
                                        This parameter can be a value of @ref RCCEx_USBPHY2_Clock_Source */

  uint32_t UsbOtgHs2ClockSelection; /*!< Specifies USB OTG HS2 clock source.
                                        This parameter can be a value of @ref RCCEx_USB_OTGHS2_Clock_Source */

  uint32_t RTCClockSelection;      /*!< Specifies RTC Clock clock source.
                                        This parameter can be a value of @ref RCC_RTC_Clock_Source       */

  uint32_t TIMPresSelection;       /*!< Specifies TIM Clock Prescalers Selection.
                                        This parameter can be a value of @ref RCCEx_TIM_Prescaler_Selection */
} RCC_PeriphCLKInitTypeDef;

/**
  * @brief  RCC Extended PLL configuration structure definition
  *         (allow to configure the PLL in SSCG mode)
  */
typedef struct
{
  uint32_t PLLModDiv;                /*!< Modulation division frequency.
                                      This parameter must be a number between Min_Data = 0 and Max_Data = 0xF */
  uint32_t PLLModSpreadDepth;        /*!< Modulation spread spectrum depth.
                                      This parameter must be a number between Min_Data = 0 and Max_Data = 0x1F */
  uint32_t PLLModSpreadMode;         /*!< Modulation spread spectrum mode.
                                      This parameter must be a value of @ref RCC_PLL_SSCGSpread_Mode */
} RCC_PLLSSCGInitTypeDef;
/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/
/** @defgroup RCCEx_Exported_Macros  RCCEx Exported Macros
  * @{
  */

/**
  * @brief  Macro to configure the PLL1 SSCG Spread Depth
  * @note   This configuration cannot be requested when the PLL1 has been enabled.
  *
  * @param  __PLLMODSPR__ specifies the SSCG modulation spread depth.
  *                         It should be a value between 0 and 0x1F.
  * @retval None
  */
#define  __HAL_RCC_PLL1_MODSPR_CONFIG(__PLLMODSPR__) \
  MODIFY_REG(RCC->PLL1CFGR3, RCC_PLL1CFGR3_PLL1MODSPR, (uint32_t)(__PLLMODSPR__) << RCC_PLL1CFGR3_PLL1MODSPR_Pos)

/**
  * @brief  Macro to configure the PLL1 SSCG Spread Mode
  * @note   This configuration cannot be requested when the PLL1 has been enabled.
  *
  * @param  __PLLMODSPRDW__ specifies the SSCG modulation spread mode.
  *                         It should be a value of @ref RCC_PLL_SSCGSpread_Mode
  * @retval None
  */
#define  __HAL_RCC_PLL1_MODSPRDW_CONFIG(__PLLMODSPRDW__) \
  MODIFY_REG(RCC->PLL1CFGR3, RCC_PLL1CFGR3_PLL1MODSPRDW, (uint32_t)(__PLLMODSPRDW__) << RCC_PLL1CFGR3_PLL1MODSPRDW_Pos)


/**
  * @brief  Macro to configure the PLL2 SSCG Spread Depth
  * @note   This configuration cannot be requested when the PLL2 has been enabled.
  *
  * @param  __PLLMODSPR__ specifies the SSCG modulation spread depth.
  *                         It should be a value between 0 and 0x1F.
  * @retval None
  */
#define  __HAL_RCC_PLL2_MODSPR_CONFIG(__PLLMODSPR__) \
  MODIFY_REG(RCC->PLL2CFGR3, RCC_PLL2CFGR3_PLL2MODSPR, (uint32_t)(__PLLMODSPR__) << RCC_PLL2CFGR3_PLL2MODSPR_Pos)

/**
  * @brief  Macro to configure the PLL2 SSCG Spread Mode
  * @note   This configuration cannot be requested when the PLL2 has been enabled.
  *
  * @param  __PLLMODSPRDW__ specifies the SSCG modulation spread mode.
  *                         It should be a value of @ref RCC_PLL_SSCGSpread_Mode
  * @retval None
  */
#define  __HAL_RCC_PLL2_MODSPRDW_CONFIG(__PLLMODSPRDW__) \
  MODIFY_REG(RCC->PLL2CFGR3, RCC_PLL2CFGR3_PLL2MODSPRDW, (uint32_t)(__PLLMODSPRDW__) << RCC_PLL2CFGR3_PLL2MODSPRDW_Pos)


/**
  * @brief  Macro to configure the PLL3 SSCG Spread Depth
  * @note   This configuration cannot be requested when the PLL3 has been enabled.
  *
  * @param  __PLLMODSPR__ specifies the SSCG modulation spread depth.
  *                         It should be a value between 0 and 0x1F.
  * @retval None
  */
#define  __HAL_RCC_PLL3_MODSPR_CONFIG(__PLLMODSPR__) \
  MODIFY_REG(RCC->PLL3CFGR3, RCC_PLL3CFGR3_PLL3MODSPR, (uint32_t)(__PLLMODSPR__) << RCC_PLL3CFGR3_PLL3MODSPR_Pos)

/**
  * @brief  Macro to configure the PLL3 SSCG Spread Mode
  * @note   This configuration cannot be requested when the PLL3 has been enabled.
  *
  * @param  __PLLMODSPRDW__ specifies the SSCG modulation spread mode.
  *                         It should be a value of @ref RCC_PLL_SSCGSpread_Mode
  * @retval None
  */
#define  __HAL_RCC_PLL3_MODSPRDW_CONFIG(__PLLMODSPRDW__) \
  MODIFY_REG(RCC->PLL3CFGR3, RCC_PLL3CFGR3_PLL3MODSPRDW, (uint32_t)(__PLLMODSPRDW__) << RCC_PLL3CFGR3_PLL3MODSPRDW_Pos)


/**
  * @brief  Macro to configure the PLL4 SSCG Spread Depth
  * @note   This configuration cannot be requested when the PLL4 has been enabled.
  *
  * @param  __PLLMODSPR__ specifies the SSCG modulation spread depth.
  *                         It should be a value between 0 and 0x1F.
  * @retval None
  */
#define  __HAL_RCC_PLL4_MODSPR_CONFIG(__PLLMODSPR__) \
  MODIFY_REG(RCC->PLL4CFGR3, RCC_PLL4CFGR3_PLL4MODSPR, (uint32_t)(__PLLMODSPR__) << RCC_PLL4CFGR3_PLL4MODSPR_Pos)

/**
  * @brief  Macro to configure the PLL4 SSCG Spread Mode
  * @note   This configuration cannot be requested when the PLL4 has been enabled.
  *
  * @param  __PLLMODSPRDW__ specifies the SSCG modulation spread mode.
  *                         It should be a value of @ref RCC_PLL_SSCGSpread_Mode
  * @retval None
  */
#define  __HAL_RCC_PLL4_MODSPRDW_CONFIG(__PLLMODSPRDW__) \
  MODIFY_REG(RCC->PLL4CFGR3, RCC_PLL4CFGR3_PLL4MODSPRDW, (uint32_t)(__PLLMODSPRDW__) << RCC_PLL4CFGR3_PLL4MODSPRDW_Pos)


/** @brief  Macro to configure the ADC clock
  * @param  __ADC_CLKSOURCE__ specifies the ADC  clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_ADCCLKSOURCE_HCLK   HCLK Clock selected as ADC clock
  *            @arg RCC_ADCCLKSOURCE_CLKP   CLKP selected as ADC clock
  *            @arg RCC_ADCCLKSOURCE_IC7    IC7 selected as ADC clock
  *            @arg RCC_ADCCLKSOURCE_IC8    IC8 selected as ADC clock
  *            @arg RCC_ADCCLKSOURCE_MSI    MSI selected as ADC clock
  *            @arg RCC_ADCCLKSOURCE_HSI    HSI selected as ADC clock
  *            @arg RCC_ADCCLKSOURCE_PIN    External I2S_CKIN selected as ADC clock
  *            @arg RCC_ADCCLKSOURCE_TIMG   TIMG selected as ADC clock
  */
#define __HAL_RCC_ADC_CONFIG(__ADC_CLKSOURCE__) \
  LL_RCC_SetADCClockSource((__ADC_CLKSOURCE__))

/** @brief  Macro to get the ADC clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_ADCCLKSOURCE_HCLK   HCLK Clock selected as ADC clock
  *            @arg RCC_ADCCLKSOURCE_CLKP   CLKP selected as ADC clock
  *            @arg RCC_ADCCLKSOURCE_IC7    IC7 selected as ADC clock
  *            @arg RCC_ADCCLKSOURCE_IC8    IC8 selected as ADC clock
  *            @arg RCC_ADCCLKSOURCE_MSI    MSI selected as ADC clock
  *            @arg RCC_ADCCLKSOURCE_HSI    HSI selected as ADC clock
  *            @arg RCC_ADCCLKSOURCE_PIN    External I2S_CKIN selected as ADC clock
  *            @arg RCC_ADCCLKSOURCE_TIMG   TIMG selected as ADC clock
  */
#define __HAL_RCC_GET_ADC_SOURCE() LL_RCC_GetADCClockSource(LL_RCC_ADC_CLKSOURCE)

/** @brief  Macro to configure the ADC clock divider.
  * @param  __ADC_DIVIDER__ specifies  clock divider for ADC.
  *         This parameter can be a value between 1 and 256.
  */
#define __HAL_RCC_ADC_DIVIDER_CONFIG(__ADC_DIVIDER__) LL_RCC_SetADCPrescaler((__ADC_DIVIDER__) - 1U)

/** @brief  Macro to get the ADC clock divider.
  * @retval The divider can be a value between 1 and 256.
  */
#define __HAL_RCC_GET_ADC_DIVIDER() (LL_RCC_GetADCPrescaler() + 1U)

/** @brief  Macro to configure the ADF1 clock
  * @param  __ADF1_CLKSOURCE__ specifies the ADF1  clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_ADF1CLKSOURCE_HCLK   HCLK Clock selected as ADF1 clock
  *            @arg RCC_ADF1CLKSOURCE_CLKP   CLKP selected as ADF1 clock
  *            @arg RCC_ADF1CLKSOURCE_IC7    IC7 selected as ADF1 clock
  *            @arg RCC_ADF1CLKSOURCE_IC8    IC8 selected as ADF1 clock
  *            @arg RCC_ADF1CLKSOURCE_MSI    MSI selected as ADF1 clock
  *            @arg RCC_ADF1CLKSOURCE_HSI    HSI selected as ADF1 clock
  *            @arg RCC_ADF1CLKSOURCE_PIN    External I2S_CKIN selected as ADF1 clock
  *            @arg RCC_ADF1CLKSOURCE_TIMG   TIMG selected as ADF1 clock
  */
#define __HAL_RCC_ADF1_CONFIG(__ADF1_CLKSOURCE__) \
  LL_RCC_SetADFClockSource((__ADF1_CLKSOURCE__))

/** @brief  Macro to get the ADF1 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_ADF1CLKSOURCE_HCLK   HCLK Clock selected as ADF1 clock
  *            @arg RCC_ADF1CLKSOURCE_CLKP   CLKP selected as ADF1 clock
  *            @arg RCC_ADF1CLKSOURCE_IC7    IC7 selected as ADF1 clock
  *            @arg RCC_ADF1CLKSOURCE_IC8    IC8 selected as ADF1 clock
  *            @arg RCC_ADF1CLKSOURCE_MSI    MSI selected as ADF1 clock
  *            @arg RCC_ADF1CLKSOURCE_HSI    HSI selected as ADF1 clock
  *            @arg RCC_ADF1CLKSOURCE_PIN    External I2S_CKIN selected as ADF1 clock
  *            @arg RCC_ADF1CLKSOURCE_TIMG   TIMG selected as ADF1 clock
  */
#define __HAL_RCC_GET_ADF1_SOURCE() LL_RCC_GetADFClockSource(LL_RCC_ADF1_CLKSOURCE)

/** @brief  Macro to configure the CLKP clock for peripheral
  * @param  __CLKP_CLKSOURCE__ specifies the clock for peripheral
  *         This parameter can be one of the following values:
  *            @arg RCC_CLKPCLKSOURCE_HSI     HSI selected as CLKP clock
  *            @arg RCC_CLKPCLKSOURCE_MSI     MSI selected as CLKP clock
  *            @arg RCC_CLKPCLKSOURCE_HSE     HSE selected as CLKP clock
  *            @arg RCC_CLKPCLKSOURCE_IC5     IC5 selected as CLKP clock
  *            @arg RCC_CLKPCLKSOURCE_IC10    IC10 selected as CLKP clock
  *            @arg RCC_CLKPCLKSOURCE_IC15    IC15 selected as CLKP clock
  *            @arg RCC_CLKPCLKSOURCE_IC19    IC19 selected as CLKP clock
  *            @arg RCC_CLKPCLKSOURCE_IC20    IC20 selected as CLKP clock
  */
#define __HAL_RCC_CLKP_CONFIG(__CLKP_CLKSOURCE__) \
  LL_RCC_SetCLKPClockSource((__CLKP_CLKSOURCE__))

/** @brief  Macro to get the Oscillator clock for peripheral source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_CLKPCLKSOURCE_HSI     HSI selected as CLKP clock
  *            @arg RCC_CLKPCLKSOURCE_MSI     MSI selected as CLKP clock
  *            @arg RCC_CLKPCLKSOURCE_HSE     HSE selected as CLKP clock
  *            @arg RCC_CLKPCLKSOURCE_IC5     IC5 selected as CLKP clock
  *            @arg RCC_CLKPCLKSOURCE_IC10    IC10 selected as CLKP clock
  *            @arg RCC_CLKPCLKSOURCE_IC15    IC15 selected as CLKP clock
  *            @arg RCC_CLKPCLKSOURCE_IC19    IC19 selected as CLKP clock
  *            @arg RCC_CLKPCLKSOURCE_IC20    IC20 selected as CLKP clock
  */
#define __HAL_RCC_GET_CLKP_SOURCE() LL_RCC_GetCLKPClockSource(LL_RCC_CLKP_CLKSOURCE)

/** @brief  Macro to configure the DCMIPP clock source.
  *
  * @param  __DCMIPP_CLKSOURCE__ specifies the DCMIPP clock source.
  *            @arg RCC_DCMIPPCLKSOURCE_PCLK5 PCLK5 selected as DCMIPP clock
  *            @arg RCC_DCMIPPCLKSOURCE_CLKP  CLKP selected as DCMIPP clock
  *            @arg RCC_DCMIPPCLKSOURCE_IC17  IC17 selected as DCMIPP clock
  *            @arg RCC_DCMIPPCLKSOURCE_HSI   HSI selected as DCMIPP clock
  */
#define __HAL_RCC_DCMIPP_CONFIG(__DCMIPP_CLKSOURCE__) \
  LL_RCC_SetDCMIPPClockSource((__DCMIPP_CLKSOURCE__))

/** @brief  Macro to get the DCMIPP clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_DCMIPPCLKSOURCE_PCLK5 PCLK5 selected as DCMIPP clock
  *            @arg RCC_DCMIPPCLKSOURCE_CLKP  CLKP selected as DCMIPP clock
  *            @arg RCC_DCMIPPCLKSOURCE_IC17  IC17 elected as DCMIPP clock
  *            @arg RCC_DCMIPPCLKSOURCE_HSI   HSI selected as DCMIPP clock
  */
#define __HAL_RCC_GET_DCMIPP_SOURCE() LL_RCC_GetDCMIPPClockSource(LL_RCC_DCMIPP_CLKSOURCE)

/** @brief  Macro to configure the ETH1 kernel clock source.
  * @param  __ETH1_CLKSOURCE__ specifies  clock source for ETH1
  *         This parameter can be one of the following values:
  *            @arg RCC_ETH1CLKSOURCE_HCLK    HCLK selected as ETH1 clock
  *            @arg RCC_ETH1CLKSOURCE_CLKP    CLKP selected as ETH1 clock
  *            @arg RCC_ETH1CLKSOURCE_IC12    IC12 selected as ETH1 clock
  *            @arg RCC_ETH1CLKSOURCE_HSE     HSE selected as ETH1 clock
  */
#define __HAL_RCC_ETH1_CONFIG(__ETH1_CLKSOURCE__) \
  LL_RCC_SetETHClockSource(__ETH1_CLKSOURCE__)

/** @brief  Macro to get the ETH1 kernel clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_ETH1CLKSOURCE_HCLK    HCLK selected as ETH1 clock
  *            @arg RCC_ETH1CLKSOURCE_CLKP    CLKP selected as ETH1 clock
  *            @arg RCC_ETH1CLKSOURCE_IC12    IC12 selected as ETH1 clock
  *            @arg RCC_ETH1CLKSOURCE_HSE     HSE selected as ETH1 clock
  */
#define __HAL_RCC_GET_ETH1_SOURCE() LL_RCC_GetETHClockSource(LL_RCC_ETH1_CLKSOURCE)

/** @brief  Macro to configure the ETH1 PHY interface.
  * @note   This configuration must be done while ETH1 is under reset,
  *         and before enabling the ETH1 clocks
  * @param  __ETH1PHY_IF__ specifies  interface for ETH1 PHY
  *         This parameter can be one of the following values:
  *            @arg RCC_ETH1PHYIF_MII   MII selected as ETH1 PHY interface
  *            @arg RCC_ETH1PHYIF_RGMII RGMII selected as ETH1 PHY interface
  *            @arg RCC_ETH1PHYIF_RMII  RMII selected as ETH1 PHY interface
  */
#define __HAL_RCC_ETH1PHY_CONFIG(__ETH1PHY_IF__) \
  LL_RCC_SetETHPHYInterface(__ETH1PHY_IF__)

/** @brief  Macro to get the ETH PHY clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_ETH1PHYIF_MII   MII selected as ETH1 PHY interface
  *            @arg RCC_ETH1PHYIF_RGMII RGMII selected as ETH1 PHY interface
  *            @arg RCC_ETH1PHYIF_RMII  RMII selected as ETH1 PHY interface
  */
#define __HAL_RCC_GET_ETH1PHY_INTERFACE() LL_RCC_GetETHPHYInterface(LL_RCC_ETH1PHY_IF)

/** @brief  Macro to configure the ETH1 reference Rx clock source.
  * @param  __ETH1RX_CLKSOURCE__ specifies  clock source for ETH1 reference Rx
  *         This parameter can be one of the following values:
  *            @arg RCC_ETH1RXCLKSOURCE_EXT    External clock selected as ETH1 reference Rx clock
  *            @arg RCC_ETH1RXCLKSOURCE_INT    Internal kernel selected as ETH1 reference Rx clock
  */
#define __HAL_RCC_ETH1RX_CONFIG(__ETH1RX_CLKSOURCE__) \
  LL_RCC_SetETHREFRXClockSource(__ETH1RX_CLKSOURCE__)

/** @brief  Macro to get the ETH1 reference Rx clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_ETH1RXCLKSOURCE_EXT    External clock selected as ETH1 reference Rx clock
  *            @arg RCC_ETH1RXCLKSOURCE_INT    Internal kernel selected as ETH1 reference Rx clock
  */
#define __HAL_RCC_GET_ETH1RX_SOURCE() LL_RCC_GetETHREFRXClockSource(LL_RCC_ETH1REFRX_CLKSOURCE)

/** @brief  Macro to configure the ETH1 reference Tx RGMII 125MHz clock source.
  * @param  __ETH1TX_CLKSOURCE__ specifies  clock source for ETH1 reference Tx
  *         This parameter can be one of the following values:
  *            @arg RCC_ETH1TXCLKSOURCE_EXT    External clock selected as ETH1 reference Tx clock
  *            @arg RCC_ETH1TXCLKSOURCE_INT    Internal kernel selected as ETH1 reference Tx clock
  */
#define __HAL_RCC_ETH1TX_CONFIG(__ETH1TX_CLKSOURCE__) \
  LL_RCC_SetETHREFTXClockSource(__ETH1TX_CLKSOURCE__)

/** @brief  Macro to get the ETH1 reference Tx RGMII 125MHz clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_ETH1TXCLKSOURCE_EXT    External clock selected as ETH1 reference Tx clock
  *            @arg RCC_ETH1TXCLKSOURCE_INT    Internal kernel selected as ETH1 reference Tx clock
  */
#define __HAL_RCC_GET_ETH1TX_SOURCE() LL_RCC_GetETHREFTXClockSource(LL_RCC_ETH1REFTX_CLKSOURCE)

/** @brief  Macro to configure the ETH1 PTP clock source.
  * @param  __ETH1PTP_CLKSOURCE__ specifies  clock source for ETH1 PTP
  *         This parameter can be one of the following values:
  *            @arg RCC_ETH1PTPCLKSOURCE_HCLK    HCLK selected as ETH1 PTP clock
  *            @arg RCC_ETH1PTPCLKSOURCE_CLKP    CLKP selected as ETH1 PTP clock
  *            @arg RCC_ETH1PTPCLKSOURCE_IC13    IC13 selected as ETH1 PTP clock
  *            @arg RCC_ETH1PTPCLKSOURCE_HSE     HSE selected as ETH1 PTP clock
  */
#define __HAL_RCC_ETH1PTP_CONFIG(__ETH1PTP_CLKSOURCE__) \
  LL_RCC_SetETHPTPClockSource(__ETH1PTP_CLKSOURCE__)

/** @brief  Macro to get the ETH1 PTP clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_ETH1PTPCLKSOURCE_HCLK    HCLK selected as ETH1 PTP clock
  *            @arg RCC_ETH1PTPCLKSOURCE_CLKP    CLKP selected as ETH1 PTP clock
  *            @arg RCC_ETH1PTPCLKSOURCE_IC13    IC13 selected as ETH1 PTP clock
  *            @arg RCC_ETH1PTPCLKSOURCE_HSE     HSE selected as ETH1 PTP clock
  */
#define __HAL_RCC_GET_ETH1PTP_SOURCE() LL_RCC_GetETHPTPClockSource(LL_RCC_ETH1PTP_CLKSOURCE)

/** @brief  Macro to configure the ETH1 PTP divider.
  * @param  __ETH1PTP_DIVIDER__ specifies  clock divider for ETH1 PTP
  *         This parameter can be a value between 1 and 16.
  */
#define __HAL_RCC_ETH1PTP_DIVIDER_CONFIG(__ETH1PTP_DIVIDER__) \
  LL_RCC_SetETH1PTPDivider(((__ETH1PTP_DIVIDER__) - 1U) << RCC_CCIPR2_ETH1PTPDIV_Pos)

/** @brief  Macro to get the ETH1 PTP divider.
  * @retval The divider can be a value between 1 and 16.
  */
#define __HAL_RCC_GET_ETH1PTP_DIVIDER() \
  ((LL_RCC_GetETH1PTPDivider() >> RCC_CCIPR2_ETH1PTPDIV_Pos) + 1U)

/** @brief  Macro to configure the FDCAN kernel clock source.
  * @param  __FDCAN_CLKSOURCE__ specifies  clock source for FDCAN kernel
  *         This parameter can be one of the following values:
  *            @arg RCC_FDCANCLKSOURCE_PCLK1  PCLK1 selected as FDCAN kernel clock (default)
  *            @arg RCC_FDCANCLKSOURCE_CLKP   CLKP selected as FDCAN kernel clock
  *            @arg RCC_FDCANCLKSOURCE_IC19   IC19 selected as FDCAN kernel clock
  *            @arg RCC_FDCANCLKSOURCE_HSE    HSE selected as FDCAN kernel clock
  */
#define __HAL_RCC_FDCAN_CONFIG(__FDCAN_CLKSOURCE__) \
  LL_RCC_SetFDCANClockSource((__FDCAN_CLKSOURCE__))

/** @brief  Macro to get the FDCAN kernel clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_FDCANCLKSOURCE_PCLK1  PCLK1 selected as FDCAN kernel clock (default)
  *            @arg RCC_FDCANCLKSOURCE_CLKP   CLKP selected as FDCAN kernel clock
  *            @arg RCC_FDCANCLKSOURCE_IC19   IC19 selected as FDCAN kernel clock
  *            @arg RCC_FDCANCLKSOURCE_HSE    HSE selected as FDCAN kernel clock
  */
#define __HAL_RCC_GET_FDCAN_SOURCE() LL_RCC_GetFDCANClockSource(LL_RCC_FDCAN_CLKSOURCE)

/** @brief  Macro to configure the FMC clock source.
  *
  * @param  __FMC_CLKSOURCE__ specifies the FMC clock source.
  *            @arg RCC_FMCCLKSOURCE_HCLK  HCLK Clock selected as FMC clock
  *            @arg RCC_FMCCLKSOURCE_CLKP  CLKP Clock selected as FMC clock
  *            @arg RCC_FMCCLKSOURCE_IC3   IC3 Clock selected as FMC clock
  *            @arg RCC_FMCCLKSOURCE_IC4   IC4 selected as FMC clock
  */
#define __HAL_RCC_FMC_CONFIG(__FMC_CLKSOURCE__) \
  LL_RCC_SetFMCClockSource((__FMC_CLKSOURCE__))

/** @brief  Macro to get the FMC clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_FMCCLKSOURCE_HCLK  HCLK Clock selected as FMC clock
  *            @arg RCC_FMCCLKSOURCE_CLKP  CLKP Clock selected as FMC clock
  *            @arg RCC_FMCCLKSOURCE_IC3   IC3 Clock selected as FMC clock
  *            @arg RCC_FMCCLKSOURCE_IC4   IC4 selected as FMC clock
  */
#define __HAL_RCC_GET_FMC_SOURCE() LL_RCC_GetFMCClockSource(LL_RCC_FMC_CLKSOURCE)

/** @brief  Macro to configure the I2C1 clock source.
  * @param  __I2C1_CLKSOURCE__ specifies the I2C1 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_I2C1CLKSOURCE_PCLK1    PCLK1 selected as I2C1 clock
  *            @arg RCC_I2C1CLKSOURCE_CLKP     CKLP selected as I2C1 clock
  *            @arg RCC_I2C1CLKSOURCE_IC10     IC10 selected as I2C1 clock
  *            @arg RCC_I2C1CLKSOURCE_IC15     IC15 selected as I2C1 clock
  *            @arg RCC_I2C1CLKSOURCE_MSI      MSI selected as I2C1 clock
  *            @arg RCC_I2C1CLKSOURCE_HSI      HSI selected as I2C1 clock
  */
#define __HAL_RCC_I2C1_CONFIG(__I2C1_CLKSOURCE__) \
  LL_RCC_SetI2CClockSource((__I2C1_CLKSOURCE__))

/** @brief  Macro to get the I2C1 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_I2C1CLKSOURCE_PCLK1    PCLK1 selected as I2C1 clock
  *            @arg RCC_I2C1CLKSOURCE_CLKP     CKLP selected as I2C1 clock
  *            @arg RCC_I2C1CLKSOURCE_IC10     IC10 selected as I2C1 clock
  *            @arg RCC_I2C1CLKSOURCE_IC15     IC15 selected as I2C1 clock
  *            @arg RCC_I2C1CLKSOURCE_MSI      MSI selected as I2C1 clock
  *            @arg RCC_I2C1CLKSOURCE_HSI      HSI selected as I2C1 clock
  */
#define __HAL_RCC_GET_I2C1_SOURCE() LL_RCC_GetI2CClockSource(LL_RCC_I2C1_CLKSOURCE)

/** @brief  Macro to configure the I2C2 clock source.
  * @param  __I2C2_CLKSOURCE__ specifies the I2C2 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_I2C2CLKSOURCE_PCLK1    PCLK1 selected as I2C2 clock
  *            @arg RCC_I2C2CLKSOURCE_CLKP     CKLP selected as I2C2 clock
  *            @arg RCC_I2C2CLKSOURCE_IC10     IC10 selected as I2C2 clock
  *            @arg RCC_I2C2CLKSOURCE_IC15     IC15 selected as I2C2 clock
  *            @arg RCC_I2C2CLKSOURCE_MSI      MSI selected as I2C2 clock
  *            @arg RCC_I2C2CLKSOURCE_HSI      HSI selected as I2C2 clock
  */
#define __HAL_RCC_I2C2_CONFIG(__I2C2_CLKSOURCE__) \
  LL_RCC_SetI2CClockSource((__I2C2_CLKSOURCE__))

/** @brief  Macro to get the I2C2 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_I2C2CLKSOURCE_PCLK1    PCLK1 selected as I2C2 clock
  *            @arg RCC_I2C2CLKSOURCE_CLKP     CKLP selected as I2C2 clock
  *            @arg RCC_I2C2CLKSOURCE_IC10     IC10 selected as I2C2 clock
  *            @arg RCC_I2C2CLKSOURCE_IC15     IC15 selected as I2C2 clock
  *            @arg RCC_I2C2CLKSOURCE_MSI      MSI selected as I2C2 clock
  *            @arg RCC_I2C2CLKSOURCE_HSI      HSI selected as I2C2 clock
  */
#define __HAL_RCC_GET_I2C2_SOURCE() LL_RCC_GetI2CClockSource(LL_RCC_I2C2_CLKSOURCE)

/** @brief  Macro to configure the I2C3 clock source.
  * @param  __I2C3_CLKSOURCE__ specifies the I2C3 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_I2C3CLKSOURCE_PCLK1    PCLK1 selected as I2C3 clock
  *            @arg RCC_I2C3CLKSOURCE_CLKP     CKLP selected as I2C3 clock
  *            @arg RCC_I2C3CLKSOURCE_IC10     IC10 selected as I2C3 clock
  *            @arg RCC_I2C3CLKSOURCE_IC15     IC15 selected as I2C3 clock
  *            @arg RCC_I2C3CLKSOURCE_MSI      MSI selected as I2C3 clock
  *            @arg RCC_I2C3CLKSOURCE_HSI      HSI selected as I2C3 clock
  */
#define __HAL_RCC_I2C3_CONFIG(__I2C3_CLKSOURCE__) \
  LL_RCC_SetI2CClockSource((__I2C3_CLKSOURCE__))

/** @brief  Macro to get the I2C3 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_I2C3CLKSOURCE_PCLK1    PCLK1 selected as I2C3 clock
  *            @arg RCC_I2C3CLKSOURCE_CLKP     CKLP selected as I2C3 clock
  *            @arg RCC_I2C3CLKSOURCE_IC10     IC10 selected as I2C3 clock
  *            @arg RCC_I2C3CLKSOURCE_IC15     IC15 selected as I2C3 clock
  *            @arg RCC_I2C3CLKSOURCE_MSI      MSI selected as I2C3 clock
  *            @arg RCC_I2C3CLKSOURCE_HSI      HSI selected as I2C3 clock
  */
#define __HAL_RCC_GET_I2C3_SOURCE() LL_RCC_GetI2CClockSource(LL_RCC_I2C3_CLKSOURCE)

/** @brief  Macro to configure the I2C4 clock source.
  * @param  __I2C4_CLKSOURCE__ specifies the I2C4 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_I2C4CLKSOURCE_PCLK1    PCLK1 selected as I2C4 clock
  *            @arg RCC_I2C4CLKSOURCE_CLKP     CKLP selected as I2C4 clock
  *            @arg RCC_I2C4CLKSOURCE_IC10     IC10 selected as I2C4 clock
  *            @arg RCC_I2C4CLKSOURCE_IC15     IC15 selected as I2C4 clock
  *            @arg RCC_I2C4CLKSOURCE_MSI      MSI selected as I2C4 clock
  *            @arg RCC_I2C4CLKSOURCE_HSI      HSI selected as I2C4 clock
  */
#define __HAL_RCC_I2C4_CONFIG(__I2C4_CLKSOURCE__) \
  LL_RCC_SetI2CClockSource((__I2C4_CLKSOURCE__))

/** @brief  Macro to get the I2C4 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_I2C4CLKSOURCE_PCLK1    PCLK1 selected as I2C4 clock
  *            @arg RCC_I2C4CLKSOURCE_CLKP     CKLP selected as I2C4 clock
  *            @arg RCC_I2C4CLKSOURCE_IC10     IC10 selected as I2C4 clock
  *            @arg RCC_I2C4CLKSOURCE_IC15     IC15 selected as I2C4 clock
  *            @arg RCC_I2C4CLKSOURCE_MSI      MSI selected as I2C4 clock
  *            @arg RCC_I2C4CLKSOURCE_HSI      HSI selected as I2C4 clock
  */
#define __HAL_RCC_GET_I2C4_SOURCE() LL_RCC_GetI2CClockSource(LL_RCC_I2C4_CLKSOURCE)

/** @brief  Macro to configure the I3C1 clock source.
  * @param  __I3C1_CLKSOURCE__ specifies the I3C1 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_I3C1CLKSOURCE_PCLK1    PCLK1 selected as I3C1 clock
  *            @arg RCC_I3C1CLKSOURCE_CLKP     CKLP selected as I3C1 clock
  *            @arg RCC_I3C1CLKSOURCE_IC10     IC10 selected as I3C1 clock
  *            @arg RCC_I3C1CLKSOURCE_IC15     IC15 selected as I3C1 clock
  *            @arg RCC_I3C1CLKSOURCE_MSI      MSI selected as I3C1 clock
  *            @arg RCC_I3C1CLKSOURCE_HSI      HSI selected as I3C1 clock
  */
#define __HAL_RCC_I3C1_CONFIG(__I3C1_CLKSOURCE__) \
  LL_RCC_SetI3CClockSource((__I3C1_CLKSOURCE__))

/** @brief  Macro to get the I3C1 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_I3C1CLKSOURCE_PCLK1    PCLK1 selected as I3C1 clock
  *            @arg RCC_I3C1CLKSOURCE_CLKP     CKLP selected as I3C1 clock
  *            @arg RCC_I3C1CLKSOURCE_IC10     IC10 selected as I3C1 clock
  *            @arg RCC_I3C1CLKSOURCE_IC15     IC15 selected as I3C1 clock
  *            @arg RCC_I3C1CLKSOURCE_MSI      MSI selected as I3C1 clock
  *            @arg RCC_I3C1CLKSOURCE_HSI      HSI selected as I3C1 clock
  */
#define __HAL_RCC_GET_I3C1_SOURCE() LL_RCC_GetI3CClockSource(LL_RCC_I3C1_CLKSOURCE)

/** @brief  Macro to configure the I3C2 clock source.
  * @param  __I3C2_CLKSOURCE__ specifies the I3C2 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_I3C2CLKSOURCE_PCLK1    PCLK1 selected as I3C2 clock
  *            @arg RCC_I3C2CLKSOURCE_CLKP     CKLP selected as I3C2 clock
  *            @arg RCC_I3C2CLKSOURCE_IC10     IC10 selected as I3C2 clock
  *            @arg RCC_I3C2CLKSOURCE_IC15     IC15 selected as I3C2 clock
  *            @arg RCC_I3C2CLKSOURCE_MSI      MSI selected as I3C2 clock
  *            @arg RCC_I3C2CLKSOURCE_HSI      HSI selected as I3C2 clock
  */
#define __HAL_RCC_I3C2_CONFIG(__I3C2_CLKSOURCE__) \
  LL_RCC_SetI3CClockSource((__I3C2_CLKSOURCE__))

/** @brief  Macro to get the I3C2 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_I3C2CLKSOURCE_PCLK1    PCLK1 selected as I3C2 clock
  *            @arg RCC_I3C2CLKSOURCE_CLKP     CKLP selected as I3C2 clock
  *            @arg RCC_I3C2CLKSOURCE_IC10     IC10 selected as I3C2 clock
  *            @arg RCC_I3C2CLKSOURCE_IC15     IC15 selected as I3C2 clock
  *            @arg RCC_I3C2CLKSOURCE_MSI      MSI selected as I3C2 clock
  *            @arg RCC_I3C2CLKSOURCE_HSI      HSI selected as I3C2 clock
  */
#define __HAL_RCC_GET_I3C2_SOURCE() LL_RCC_GetI3CClockSource(LL_RCC_I3C2_CLKSOURCE)

/** @brief  Macro to configure the LPTIM1 clock source.
  * @param  __LPTIM1_CLKSOURCE__ specifies the LPTIM1 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_LPTIM1CLKSOURCE_PCLK1  PCLK1 clock selected as LPTIM1 clock
  *            @arg RCC_LPTIM1CLKSOURCE_CLKP   CLKP clock selected as LPTIM1 clock
  *            @arg RCC_LPTIM1CLKSOURCE_IC15   IC15 clock selected as LPTIM1 clock
  *            @arg RCC_LPTIM1CLKSOURCE_LSE    LSE clock selected as LPTIM1 clock
  *            @arg RCC_LPTIM1CLKSOURCE_LSI    LSI clock selected as LPTIM1 clock
  *            @arg RCC_LPTIM1CLKSOURCE_TIMG   TIMG clock selected as LPTIM1 clock
  */
#define __HAL_RCC_LPTIM1_CONFIG(__LPTIM1_CLKSOURCE__) \
  LL_RCC_SetLPTIMClockSource((__LPTIM1_CLKSOURCE__))

/** @brief  Macro to get the LPTIM1 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_LPTIM1CLKSOURCE_PCLK1  PCLK1 clock selected as LPTIM1 clock
  *            @arg RCC_LPTIM1CLKSOURCE_CLKP   CLKP clock selected as LPTIM1 clock
  *            @arg RCC_LPTIM1CLKSOURCE_IC15   IC15 clock selected as LPTIM1 clock
  *            @arg RCC_LPTIM1CLKSOURCE_LSE    LSE clock selected as LPTIM1 clock
  *            @arg RCC_LPTIM1CLKSOURCE_LSI    LSI clock selected as LPTIM1 clock
  *            @arg RCC_LPTIM1CLKSOURCE_TIMG   TIMG clock selected as LPTIM1 clock
  */
#define __HAL_RCC_GET_LPTIM1_SOURCE() LL_RCC_GetLPTIMClockSource(LL_RCC_LPTIM1_CLKSOURCE)

/** @brief  Macro to configure the LPTIM2 clock source.
  * @param  __LPTIM2_CLKSOURCE__ specifies the LPTIM2 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_LPTIM2CLKSOURCE_PCLK4  PCLK4 clock selected as LPTIM2 clock
  *            @arg RCC_LPTIM2CLKSOURCE_CLKP   CLKP clock selected as LPTIM2 clock
  *            @arg RCC_LPTIM2CLKSOURCE_IC15   IC15 clock selected as LPTIM2 clock
  *            @arg RCC_LPTIM2CLKSOURCE_LSE    LSE clock selected as LPTIM2 clock
  *            @arg RCC_LPTIM2CLKSOURCE_LSI    LSI clock selected as LPTIM2 clock
  *            @arg RCC_LPTIM2CLKSOURCE_TIMG   TIMG clock selected as LPTIM2 clock
  */
#define __HAL_RCC_LPTIM2_CONFIG(__LPTIM2_CLKSOURCE__) \
  LL_RCC_SetLPTIMClockSource((__LPTIM2_CLKSOURCE__))

/** @brief  Macro to get the LPTIM2 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_LPTIM2CLKSOURCE_PCLK4  PCLK4 clock selected as LPTIM2 clock
  *            @arg RCC_LPTIM2CLKSOURCE_CLKP   CLKP clock selected as LPTIM2 clock
  *            @arg RCC_LPTIM2CLKSOURCE_IC15   IC15 clock selected as LPTIM2 clock
  *            @arg RCC_LPTIM2CLKSOURCE_LSE    LSE clock selected as LPTIM2 clock
  *            @arg RCC_LPTIM2CLKSOURCE_LSI    LSI clock selected as LPTIM2 clock
  *            @arg RCC_LPTIM2CLKSOURCE_TIMG   TIMG clock selected as LPTIM2 clock
  */
#define __HAL_RCC_GET_LPTIM2_SOURCE() LL_RCC_GetLPTIMClockSource(LL_RCC_LPTIM2_CLKSOURCE)

/** @brief  Macro to configure the LPTIM3 clock source.
  * @param  __LPTIM3_CLKSOURCE__ specifies the LPTIM3 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_LPTIM3CLKSOURCE_PCLK4  PCLK4 clock selected as LPTIM3 clock
  *            @arg RCC_LPTIM3CLKSOURCE_CLKP   CLKP clock selected as LPTIM3 clock
  *            @arg RCC_LPTIM3CLKSOURCE_IC15   IC15 clock selected as LPTIM3 clock
  *            @arg RCC_LPTIM3CLKSOURCE_LSE    LSE clock selected as LPTIM3 clock
  *            @arg RCC_LPTIM3CLKSOURCE_LSI    LSI clock selected as LPTIM3 clock
  *            @arg RCC_LPTIM3CLKSOURCE_TIMG   TIMG clock selected as LPTIM3 clock
  */
#define __HAL_RCC_LPTIM3_CONFIG(__LPTIM3_CLKSOURCE__) \
  LL_RCC_SetLPTIMClockSource((__LPTIM3_CLKSOURCE__))

/** @brief  Macro to get the LPTIM3 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_LPTIM3CLKSOURCE_PCLK4  PCLK4 clock selected as LPTIM3 clock
  *            @arg RCC_LPTIM3CLKSOURCE_CLKP   CLKP clock selected as LPTIM3 clock
  *            @arg RCC_LPTIM3CLKSOURCE_IC15   IC15 clock selected as LPTIM3 clock
  *            @arg RCC_LPTIM3CLKSOURCE_LSE    LSE clock selected as LPTIM3 clock
  *            @arg RCC_LPTIM3CLKSOURCE_LSI    LSI clock selected as LPTIM3 clock
  *            @arg RCC_LPTIM3CLKSOURCE_TIMG   TIMG clock selected as LPTIM3 clock
  */
#define __HAL_RCC_GET_LPTIM3_SOURCE() LL_RCC_GetLPTIMClockSource(LL_RCC_LPTIM3_CLKSOURCE)

/** @brief  Macro to configure the LPTIM4 clock source.
  * @param  __LPTIM4_CLKSOURCE__ specifies the LPTIM4 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_LPTIM4CLKSOURCE_PCLK4  PCLK4 clock selected as LPTIM4 clock
  *            @arg RCC_LPTIM4CLKSOURCE_CLKP   CLKP clock selected as LPTIM4 clock
  *            @arg RCC_LPTIM4CLKSOURCE_IC15   IC15 clock selected as LPTIM4 clock
  *            @arg RCC_LPTIM4CLKSOURCE_LSE    LSE clock selected as LPTIM4 clock
  *            @arg RCC_LPTIM4CLKSOURCE_LSI    LSI clock selected as LPTIM4 clock
  *            @arg RCC_LPTIM4CLKSOURCE_TIMG   TIMG clock selected as LPTIM4 clock
  */
#define __HAL_RCC_LPTIM4_CONFIG(__LPTIM4_CLKSOURCE__) \
  LL_RCC_SetLPTIMClockSource((__LPTIM4_CLKSOURCE__))

/** @brief  Macro to get the LPTIM4 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_LPTIM4CLKSOURCE_PCLK4  PCLK4 clock selected as LPTIM4 clock
  *            @arg RCC_LPTIM4CLKSOURCE_CLKP   CLKP clock selected as LPTIM4 clock
  *            @arg RCC_LPTIM4CLKSOURCE_IC15   IC15 clock selected as LPTIM4 clock
  *            @arg RCC_LPTIM4CLKSOURCE_LSE    LSE clock selected as LPTIM4 clock
  *            @arg RCC_LPTIM4CLKSOURCE_LSI    LSI clock selected as LPTIM4 clock
  *            @arg RCC_LPTIM4CLKSOURCE_TIMG   TIMG clock selected as LPTIM4 clock
  */
#define __HAL_RCC_GET_LPTIM4_SOURCE() LL_RCC_GetLPTIMClockSource(LL_RCC_LPTIM4_CLKSOURCE)

/** @brief  Macro to configure the LPTIM5 clock source.
  * @param  __LPTIM5_CLKSOURCE__ specifies the LPTIM5 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_LPTIM5CLKSOURCE_PCLK4  PCLK4 clock selected as LPTIM5 clock
  *            @arg RCC_LPTIM5CLKSOURCE_CLKP   CLKP clock selected as LPTIM5 clock
  *            @arg RCC_LPTIM5CLKSOURCE_IC15   IC15 clock selected as LPTIM5 clock
  *            @arg RCC_LPTIM5CLKSOURCE_LSE    LSE clock selected as LPTIM5 clock
  *            @arg RCC_LPTIM5CLKSOURCE_LSI    LSI clock selected as LPTIM5 clock
  *            @arg RCC_LPTIM5CLKSOURCE_TIMG   TIMG clock selected as LPTIM5 clock
  */
#define __HAL_RCC_LPTIM5_CONFIG(__LPTIM5_CLKSOURCE__) \
  LL_RCC_SetLPTIMClockSource((__LPTIM5_CLKSOURCE__))

/** @brief  Macro to get the LPTIM5 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_LPTIM5CLKSOURCE_PCLK4  PCLK4 clock selected as LPTIM5 clock
  *            @arg RCC_LPTIM5CLKSOURCE_CLKP   CLKP clock selected as LPTIM5 clock
  *            @arg RCC_LPTIM5CLKSOURCE_IC15   IC15 clock selected as LPTIM5 clock
  *            @arg RCC_LPTIM5CLKSOURCE_LSE    LSE clock selected as LPTIM5 clock
  *            @arg RCC_LPTIM5CLKSOURCE_LSI    LSI clock selected as LPTIM5 clock
  *            @arg RCC_LPTIM5CLKSOURCE_TIMG   TIMG clock selected as LPTIM5 clock
  */
#define __HAL_RCC_GET_LPTIM5_SOURCE() LL_RCC_GetLPTIMClockSource(LL_RCC_LPTIM5_CLKSOURCE)

/** @brief  Macro to configure the LPUART1 clock (LPUART1CLK).
  * @param  __LPUART1_CLKSOURCE__ specifies the LPUART1 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_LPUART1CLKSOURCE_PCLK4 PCLK4 Clock selected as LPUART1 clock
  *            @arg RCC_LPUART1CLKSOURCE_CLKP  CLKP selected as LPUART1 clock
  *            @arg RCC_LPUART1CLKSOURCE_IC9   IC9 selected as LPUART1 clock
  *            @arg RCC_LPUART1CLKSOURCE_IC14  IC14 selected as LPUART1 clock
  *            @arg RCC_LPUART1CLKSOURCE_LSE   LSE selected as LPUART1 clock
  *            @arg RCC_LPUART1CLKSOURCE_MSI   MSI selected as LPUART1 clock
  *            @arg RCC_LPUART1CLKSOURCE_HSI   HSI selected as LPUART1 clock
  */
#define __HAL_RCC_LPUART1_CONFIG(__LPUART1_CLKSOURCE__) \
  LL_RCC_SetLPUARTClockSource((__LPUART1_CLKSOURCE__))

/** @brief  Macro to get the LPUART1 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_LPUART1CLKSOURCE_PCLK4 PCLK4 Clock selected as LPUART1 clock
  *            @arg RCC_LPUART1CLKSOURCE_CLKP  CLKP selected as LPUART1 clock
  *            @arg RCC_LPUART1CLKSOURCE_IC9   IC9 selected as LPUART1 clock
  *            @arg RCC_LPUART1CLKSOURCE_IC14  IC14 selected as LPUART1 clock
  *            @arg RCC_LPUART1CLKSOURCE_LSE   LSE selected as LPUART1 clock
  *            @arg RCC_LPUART1CLKSOURCE_MSI   MSI selected as LPUART1 clock
  *            @arg RCC_LPUART1CLKSOURCE_HSI   HSI selected as LPUART1 clock
  */
#define __HAL_RCC_GET_LPUART1_SOURCE() LL_RCC_GetLPUARTClockSource(LL_RCC_LPUART1_CLKSOURCE)

/** @brief  Macro to configure the LTDC clock (LTDCCLK).
  * @param  __LTDC_CLKSOURCE__ specifies the LTDC clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_LTDCCLKSOURCE_PCLK5 PCLK5 Clock selected as LTDC clock
  *            @arg RCC_LTDCCLKSOURCE_CLKP  CLKP selected as LTDC clock
  *            @arg RCC_LTDCCLKSOURCE_IC16  IC14 selected as LTDC clock
  *            @arg RCC_LTDCCLKSOURCE_HSI   HSI selected as LTDC clock
  */
#define __HAL_RCC_LTDC_CONFIG(__LTDC_CLKSOURCE__) \
  LL_RCC_SetLTDCClockSource((__LTDC_CLKSOURCE__))

/** @brief  Macro to get the LTDC clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_LTDCCLKSOURCE_PCLK5 PCLK5 Clock selected as LTDC clock
  *            @arg RCC_LTDCCLKSOURCE_CLKP  CLKP selected as LTDC clock
  *            @arg RCC_LTDCCLKSOURCE_IC16  IC14 selected as LTDC clock
  *            @arg RCC_LTDCCLKSOURCE_HSI   HSI selected as LTDC clock
  */
#define __HAL_RCC_GET_LTDC_SOURCE() LL_RCC_GetLTDCClockSource(LL_RCC_LTDC_CLKSOURCE)

/** @brief  Macro to configure the MDF1 clock
  * @param  __MDF1_CLKSOURCE__ specifies the MDF1  clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_MDF1CLKSOURCE_HCLK   HCLK Clock selected as MDF1 clock
  *            @arg RCC_MDF1CLKSOURCE_CLKP   CLKP selected as MDF1 clock
  *            @arg RCC_MDF1CLKSOURCE_IC7    IC7 selected as MDF1 clock
  *            @arg RCC_MDF1CLKSOURCE_IC8    IC8 selected as MDF1 clock
  *            @arg RCC_MDF1CLKSOURCE_MSI    MSI selected as MDF1 clock
  *            @arg RCC_MDF1CLKSOURCE_HSI    HSI selected as MDF1 clock
  *            @arg RCC_MDF1CLKSOURCE_PIN    External I2S_CKIN selected as MDF1 clock
  *            @arg RCC_MDF1CLKSOURCE_TIMG   TIMG selected as MDF1 clock
  */
#define __HAL_RCC_MDF1_CONFIG(__MDF1_CLKSOURCE__) \
  LL_RCC_SetMDFClockSource((__MDF1_CLKSOURCE__))

/** @brief  Macro to get the MDF1 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_MDF1CLKSOURCE_HCLK   HCLK Clock selected as MDF1 clock
  *            @arg RCC_MDF1CLKSOURCE_CLKP   CLKP selected as MDF1 clock
  *            @arg RCC_MDF1CLKSOURCE_IC7    IC7 selected as MDF1 clock
  *            @arg RCC_MDF1CLKSOURCE_IC8    IC8 selected as MDF1 clock
  *            @arg RCC_MDF1CLKSOURCE_MSI    MSI selected as MDF1 clock
  *            @arg RCC_MDF1CLKSOURCE_HSI    HSI selected as MDF1 clock
  *            @arg RCC_MDF1CLKSOURCE_PIN    External I2S_CKIN selected as MDF1 clock
  *            @arg RCC_MDF1CLKSOURCE_TIMG   TIMG selected as MDF1 clock
  */
#define __HAL_RCC_GET_MDF1_SOURCE() LL_RCC_GetMDFClockSource(LL_RCC_MDF1_CLKSOURCE)

/**
  * @brief  Macro to configure the PSSI clock source.
  * @param  __PSSI_CLKSOURCE__ defines the PSSI clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_PSSICLKSOURCE_HCLK  HCLK selected as PSSI clock
  *            @arg RCC_PSSICLKSOURCE_CLKP  Peripheral clock CLKP selected as PSSI clock
  *            @arg RCC_PSSICLKSOURCE_IC20  IC20 selected as PSSI clock
  *            @arg RCC_PSSICLKSOURCE_HSI   HSI selected as PSSI clock
  * @retval None
  */
#define __HAL_RCC_PSSI_CONFIG(__PSSI_CLKSOURCE__) \
  LL_RCC_SetPSSIClockSource((__PSSI_CLKSOURCE__))

/** @brief  Macro to get the PSSI clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_PSSICLKSOURCE_HCLK  HCLK selected as PSSI clock
  *            @arg RCC_PSSICLKSOURCE_CLKP  Peripheral clock CLKP selected as PSSI clock
  *            @arg RCC_PSSICLKSOURCE_IC20  IC20 selected as PSSI clock
  *            @arg RCC_PSSICLKSOURCE_HSI   HSI selected as PSSI clock
  */
#define __HAL_RCC_GET_PSSI_SOURCE() LL_RCC_GetPSSIClockSource(LL_RCC_PSSI_CLKSOURCE)

/**
  * @brief  Macro to configure the SAI1 clock source.
  * @param  __SAI1_CLKSOURCE__ defines the SAI1 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_SAI1CLKSOURCE_PCLK2  PCLK2 Clock selected as SAI1 clock
  *            @arg RCC_SAI1CLKSOURCE_CLKP   Peripheral clock CLKP selected as SAI1 clock
  *            @arg RCC_SAI1CLKSOURCE_IC7    IC7 selected as SAI1 clock
  *            @arg RCC_SAI1CLKSOURCE_IC8    IC8 selected as SAI1 clock
  *            @arg RCC_SAI1CLKSOURCE_MSI    MSI selected as SAI1 clock
  *            @arg RCC_SAI1CLKSOURCE_HSI    HSI selected as SAI1 clock
  *            @arg RCC_SAI1CLKSOURCE_PIN    External I2S_CKIN selected as SAI1 clock
  *            @arg RCC_SAI1CLKSOURCE_SPDIFRX1 SPDIFRX1 selected as SAI1 clock
  * @retval None
  */
#define __HAL_RCC_SAI1_CONFIG(__SAI1_CLKSOURCE__) \
  LL_RCC_SetSAIClockSource((__SAI1_CLKSOURCE__))

/** @brief  Macro to get the SAI1 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_SAI1CLKSOURCE_PCLK2  PCLK2 Clock selected as SAI1 clock
  *            @arg RCC_SAI1CLKSOURCE_CLKP   Peripheral clock CLKP selected as SAI1 clock
  *            @arg RCC_SAI1CLKSOURCE_IC7    IC7 selected as SAI1 clock
  *            @arg RCC_SAI1CLKSOURCE_IC8    IC8 selected as SAI1 clock
  *            @arg RCC_SAI1CLKSOURCE_MSI    MSI selected as SAI1 clock
  *            @arg RCC_SAI1CLKSOURCE_HSI    HSI selected as SAI1 clock
  *            @arg RCC_SAI1CLKSOURCE_PIN    External I2S_CKIN selected as SAI1 clock
  *            @arg RCC_SAI1CLKSOURCE_SPDIFRX1 SPDIFRX1 selected as SAI1 clock
  */
#define __HAL_RCC_GET_SAI1_SOURCE() LL_RCC_GetSAIClockSource(LL_RCC_SAI1_CLKSOURCE)

/**
  * @brief  Macro to configure the SAI2 clock source.
  * @param  __SAI2_CLKSOURCE__ defines the SAI2 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_SAI2CLKSOURCE_PCLK2  PCLK2 Clock selected as SAI2 clock
  *            @arg RCC_SAI2CLKSOURCE_CLKP   Peripheral clock CLKP selected as SAI2 clock
  *            @arg RCC_SAI2CLKSOURCE_IC7    IC7 selected as SAI2 clock
  *            @arg RCC_SAI2CLKSOURCE_IC8    IC8 selected as SAI2 clock
  *            @arg RCC_SAI2CLKSOURCE_MSI    MSI selected as SAI2 clock
  *            @arg RCC_SAI2CLKSOURCE_HSI    HSI selected as SAI2 clock
  *            @arg RCC_SAI2CLKSOURCE_PIN    External I2S_CKIN selected as SAI2 clock
  *            @arg RCC_SAI2CLKSOURCE_SPDIFRX1 SPDIFRX1 selected as SAI2 clock
  * @retval None
  */
#define __HAL_RCC_SAI2_CONFIG(__SAI2_CLKSOURCE__) \
  LL_RCC_SetSAIClockSource((__SAI2_CLKSOURCE__))

/** @brief  Macro to get the SAI2 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_SAI2CLKSOURCE_PCLK2  PCLK2 Clock selected as SAI2 clock
  *            @arg RCC_SAI2CLKSOURCE_CLKP   Peripheral clock CLKP selected as SAI2 clock
  *            @arg RCC_SAI2CLKSOURCE_IC7    IC7 selected as SAI2 clock
  *            @arg RCC_SAI2CLKSOURCE_IC8    IC8 selected as SAI2 clock
  *            @arg RCC_SAI2CLKSOURCE_MSI    MSI selected as SAI2 clock
  *            @arg RCC_SAI2CLKSOURCE_HSI    HSI selected as SAI2 clock
  *            @arg RCC_SAI2CLKSOURCE_PIN    External I2S_CKIN selected as SAI2 clock
  *            @arg RCC_SAI2CLKSOURCE_SPDIFRX1 SPDIFRX1 selected as SAI2 clock
  */
#define __HAL_RCC_GET_SAI2_SOURCE() LL_RCC_GetSAIClockSource(LL_RCC_SAI2_CLKSOURCE)

/** @brief  Macro to configure the SDMMC1 kernel clock source.
  * @param  __SDMMC1_CLKSOURCE__ specifies  clock source  for SDMMC1
  *        This parameter can be one of the following values:
  *            @arg RCC_SDMMC1CLKSOURCE_HCLK   HCLK Clock selected as SDMMC1 kernel clock
  *            @arg RCC_SDMMC1CLKSOURCE_CLKP   Peripheral clock CLKP selected as SDMMC1 kernel clock
  *            @arg RCC_SDMMC1CLKSOURCE_IC4    IC4 selected as SDMMC1 kernel clock
  *            @arg RCC_SDMMC1CLKSOURCE_IC5    IC5 selected as SDMMC1 kernel clock
  */
#define __HAL_RCC_SDMMC1_CONFIG(__SDMMC1_CLKSOURCE__) \
  LL_RCC_SetSDMMCClockSource((__SDMMC1_CLKSOURCE__))

/** @brief  Macro to get the SDMMC1 kernel clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_SDMMC1CLKSOURCE_HCLK   HCLK Clock selected as SDMMC1 kernel clock
  *            @arg RCC_SDMMC1CLKSOURCE_CLKP   Peripheral clock CLKP selected as SDMMC1 kernel clock
  *            @arg RCC_SDMMC1CLKSOURCE_IC4    IC4 selected as SDMMC1 kernel clock
  *            @arg RCC_SDMMC1CLKSOURCE_IC5    IC5 selected as SDMMC1 kernel clock
  */
#define __HAL_RCC_GET_SDMMC1_SOURCE() LL_RCC_GetSDMMCClockSource(LL_RCC_SDMMC1_CLKSOURCE)

/** @brief  Macro to configure the SDMMC2 kernel clock source.
  * @param  __SDMMC2_CLKSOURCE__ specifies  clock source  for SDMMC2
  *        This parameter can be one of the following values:
  *            @arg RCC_SDMMC2CLKSOURCE_HCLK   HCLK Clock selected as SDMMC2 kernel clock
  *            @arg RCC_SDMMC2CLKSOURCE_CLKP   Peripheral clock CLKP selected as SDMMC2 kernel clock
  *            @arg RCC_SDMMC2CLKSOURCE_IC4    IC4 selected as SDMMC2 kernel clock
  *            @arg RCC_SDMMC2CLKSOURCE_IC5    IC5 selected as SDMMC2 kernel clock
  */
#define __HAL_RCC_SDMMC2_CONFIG(__SDMMC2_CLKSOURCE__) \
  LL_RCC_SetSDMMCClockSource((__SDMMC2_CLKSOURCE__))

/** @brief  Macro to get the SDMMC2 kernel clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_SDMMC2CLKSOURCE_HCLK   HCLK Clock selected as SDMMC2 kernel clock
  *            @arg RCC_SDMMC2CLKSOURCE_CLKP   Peripheral clock CLKP selected as SDMMC2 kernel clock
  *            @arg RCC_SDMMC2CLKSOURCE_IC4    IC4 selected as SDMMC2 kernel clock
  *            @arg RCC_SDMMC2CLKSOURCE_IC5    IC5 selected as SDMMC2 kernel clock
  */
#define __HAL_RCC_GET_SDMMC2_SOURCE() LL_RCC_GetSDMMCClockSource(LL_RCC_SDMMC2_CLKSOURCE)

/**
  * @brief  Macro to Configure the SPDIFRX1 clock source.
  * @param  __SPDIFRX_CLKSOURCE__ defines the SPDIFRX1 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_SPDIFRX1CLKSOURCE_PCLK1  PCLK1 Clock selected as SPDIFRX1 clock
  *            @arg RCC_SPDIFRX1CLKSOURCE_CLKP   Peripheral clock CLKP selected as SPDIFRX1 clock
  *            @arg RCC_SPDIFRX1CLKSOURCE_IC7    IC7 selected as SPDIFRX1 clock
  *            @arg RCC_SPDIFRX1CLKSOURCE_IC8    IC8 selected as SPDIFRX1 clock
  *            @arg RCC_SPDIFRX1CLKSOURCE_MSI    MSI selected as SPDIFRX1 clock
  *            @arg RCC_SPDIFRX1CLKSOURCE_HSI    HSI selected as SPDIFRX1 clock
  *            @arg RCC_SPDIFRX1CLKSOURCE_PIN    External I2S_CKIN selected as SPDIFRX1 clock
  * @retval None
  */
#define __HAL_RCC_SPDIFRX1_CONFIG(__SPDIFRX_CLKSOURCE__ ) \
  LL_RCC_SetSPDIFRXClockSource((__SPDIFRX_CLKSOURCE__))

/**
  * @brief  Macro to get the SPDIFRX1 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_SPDIFRX1CLKSOURCE_PCLK1  PCLK1 Clock selected as SPDIFRX1 clock
  *            @arg RCC_SPDIFRX1CLKSOURCE_CLKP   Peripheral clock CLKP selected as SPDIFRX1 clock
  *            @arg RCC_SPDIFRX1CLKSOURCE_IC7    IC7 selected as SPDIFRX1 clock
  *            @arg RCC_SPDIFRX1CLKSOURCE_IC8    IC8 selected as SPDIFRX1 clock
  *            @arg RCC_SPDIFRX1CLKSOURCE_MSI    MSI selected as SPDIFRX1 clock
  *            @arg RCC_SPDIFRX1CLKSOURCE_HSI    HSI selected as SPDIFRX1 clock
  *            @arg RCC_SPDIFRX1CLKSOURCE_PIN    External I2S_CKIN selected as SPDIFRX1 clock
  * @retval None
  */
#define __HAL_RCC_GET_SPDIFRX1_SOURCE() LL_RCC_GetSPDIFRXClockSource(LL_RCC_SPDIFRX1_CLKSOURCE)

/**
  * @brief  Macro to configure the SPI1 clock source.
  * @param  __SPI1_CLKSOURCE__ defines the SPI1 clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_SPI1CLKSOURCE_PCLK2  PCLK2 selected as SPI1 clock
  *            @arg RCC_SPI1CLKSOURCE_CLKP   Peripheral clock CKLP selected as SPI1 clock
  *            @arg RCC_SPI1CLKSOURCE_IC8    IC8 selected as SPI1 clock
  *            @arg RCC_SPI1CLKSOURCE_IC9    IC9 selected as SPI1 clock
  *            @arg RCC_SPI1CLKSOURCE_MSI    MSI selected as SPI1 clock
  *            @arg RCC_SPI1CLKSOURCE_HSI    HSI selected as SPI1 clock
  *            @arg RCC_SPI1CLKSOURCE_PIN    External I2S_CKIN selected as SPI1 clock
  * @retval None
  */
#define __HAL_RCC_SPI1_CONFIG(__SPI1_CLKSOURCE__ ) \
  LL_RCC_SetSPIClockSource((__SPI1_CLKSOURCE__))

/** @brief  Macro to get the SPI1 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_SPI1CLKSOURCE_PCLK2  PCLK2 selected as SPI1 clock
  *            @arg RCC_SPI1CLKSOURCE_CLKP   Peripheral clock CKLP selected as SPI1 clock
  *            @arg RCC_SPI1CLKSOURCE_IC8    IC8 selected as SPI1 clock
  *            @arg RCC_SPI1CLKSOURCE_IC9    IC9 selected as SPI1 clock
  *            @arg RCC_SPI1CLKSOURCE_MSI    MSI selected as SPI1 clock
  *            @arg RCC_SPI1CLKSOURCE_HSI    HSI selected as SPI1 clock
  *            @arg RCC_SPI1CLKSOURCE_PIN    External I2S_CKIN selected as SPI1 clock
  */
#define __HAL_RCC_GET_SPI1_SOURCE() LL_RCC_GetSPIClockSource(LL_RCC_SPI1_CLKSOURCE)

/**
  * @brief  Macro to configure the SPI2 clock source.
  * @param  __SPI2_CLKSOURCE__ defines the SPI2 clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_SPI2CLKSOURCE_PCLK1  PCLK1 selected as SPI2 clock
  *            @arg RCC_SPI2CLKSOURCE_CLKP   Peripheral clock CKLP selected as SPI2 clock
  *            @arg RCC_SPI2CLKSOURCE_IC8    IC8 selected as SPI2 clock
  *            @arg RCC_SPI2CLKSOURCE_IC9    IC9 selected as SPI2 clock
  *            @arg RCC_SPI2CLKSOURCE_MSI    MSI selected as SPI2 clock
  *            @arg RCC_SPI2CLKSOURCE_HSI    HSI selected as SPI2 clock
  *            @arg RCC_SPI2CLKSOURCE_PIN    External I2S_CKIN selected as SPI2 clock
  * @retval None
  */
#define __HAL_RCC_SPI2_CONFIG(__SPI2_CLKSOURCE__ ) \
  LL_RCC_SetSPIClockSource((__SPI2_CLKSOURCE__))

/** @brief  Macro to get the SPI2 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_SPI2CLKSOURCE_PCLK1  PCLK1 selected as SPI2 clock
  *            @arg RCC_SPI2CLKSOURCE_CLKP   Peripheral clock CKLP selected as SPI2 clock
  *            @arg RCC_SPI2CLKSOURCE_IC8    IC8 selected as SPI2 clock
  *            @arg RCC_SPI2CLKSOURCE_IC9    IC9 selected as SPI2 clock
  *            @arg RCC_SPI2CLKSOURCE_MSI    MSI selected as SPI2 clock
  *            @arg RCC_SPI2CLKSOURCE_HSI    HSI selected as SPI2 clock
  *            @arg RCC_SPI2CLKSOURCE_PIN    External I2S_CKIN selected as SPI2 clock
  */
#define __HAL_RCC_GET_SPI2_SOURCE() LL_RCC_GetSPIClockSource(LL_RCC_SPI2_CLKSOURCE)

/**
  * @brief  Macro to configure the SPI3 clock source.
  * @param  __SPI3_CLKSOURCE__ defines the SPI3 clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_SPI3CLKSOURCE_PCLK1  PCLK1 selected as SPI3 clock
  *            @arg RCC_SPI3CLKSOURCE_CLKP   Peripheral clock CKLP selected as SPI3 clock
  *            @arg RCC_SPI3CLKSOURCE_IC8    IC8 selected as SPI3 clock
  *            @arg RCC_SPI3CLKSOURCE_IC9    IC9 selected as SPI3 clock
  *            @arg RCC_SPI3CLKSOURCE_MSI    MSI selected as SPI3 clock
  *            @arg RCC_SPI3CLKSOURCE_HSI    HSI selected as SPI3 clock
  *            @arg RCC_SPI3CLKSOURCE_PIN    External I2S_CKIN selected as SPI3 clock
  * @retval None
  */
#define __HAL_RCC_SPI3_CONFIG(__SPI3_CLKSOURCE__ ) \
  LL_RCC_SetSPIClockSource((__SPI3_CLKSOURCE__))

/** @brief  Macro to get the SPI3 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_SPI3CLKSOURCE_PCLK1  PCLK1 selected as SPI3 clock
  *            @arg RCC_SPI3CLKSOURCE_CLKP   Peripheral clock CKLP selected as SPI3 clock
  *            @arg RCC_SPI3CLKSOURCE_IC8    IC8 selected as SPI3 clock
  *            @arg RCC_SPI3CLKSOURCE_IC9    IC9 selected as SPI3 clock
  *            @arg RCC_SPI3CLKSOURCE_MSI    MSI selected as SPI3 clock
  *            @arg RCC_SPI3CLKSOURCE_HSI    HSI selected as SPI3 clock
  *            @arg RCC_SPI3CLKSOURCE_PIN    External I2S_CKIN selected as SPI3 clock
  */
#define __HAL_RCC_GET_SPI3_SOURCE() LL_RCC_GetSPIClockSource(LL_RCC_SPI3_CLKSOURCE)

/**
  * @brief  Macro to configure the SPI4 clock source.
  * @param  __SPI4_CLKSOURCE__ defines the SPI4 clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_SPI4CLKSOURCE_PCLK2  PCLK2 selected as SPI4 clock
  *            @arg RCC_SPI4CLKSOURCE_CLKP   Peripheral clock CKLP selected as SPI4 clock
  *            @arg RCC_SPI4CLKSOURCE_IC8    IC8 selected as SPI4 clock
  *            @arg RCC_SPI4CLKSOURCE_IC9    IC9 selected as SPI4 clock
  *            @arg RCC_SPI4CLKSOURCE_MSI    MSI selected as SPI4 clock
  *            @arg RCC_SPI4CLKSOURCE_HSI    HSI selected as SPI4 clock
  *            @arg RCC_SPI4CLKSOURCE_PIN    External I2S_CKIN selected as SPI4 clock
  * @retval None
  */
#define __HAL_RCC_SPI4_CONFIG(__SPI4_CLKSOURCE__ ) \
  LL_RCC_SetSPIClockSource((__SPI4_CLKSOURCE__))

/** @brief  Macro to get the SPI4 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_SPI4CLKSOURCE_PCLK2  PCLK2 selected as SPI4 clock
  *            @arg RCC_SPI4CLKSOURCE_CLKP   Peripheral clock CKLP selected as SPI4 clock
  *            @arg RCC_SPI4CLKSOURCE_IC8    IC8 selected as SPI4 clock
  *            @arg RCC_SPI4CLKSOURCE_IC9    IC9 selected as SPI4 clock
  *            @arg RCC_SPI4CLKSOURCE_MSI    MSI selected as SPI4 clock
  *            @arg RCC_SPI4CLKSOURCE_HSI    HSI selected as SPI4 clock
  *            @arg RCC_SPI4CLKSOURCE_PIN    External I2S_CKIN selected as SPI4 clock
  */
#define __HAL_RCC_GET_SPI4_SOURCE() LL_RCC_GetSPIClockSource(LL_RCC_SPI4_CLKSOURCE)

/**
  * @brief  Macro to configure the SPI5 clock source.
  * @param  __SPI5_CLKSOURCE__ defines the SPI5 clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_SPI5CLKSOURCE_PCLK2  PCLK2 selected as SPI5 clock
  *            @arg RCC_SPI5CLKSOURCE_CLKP   Peripheral clock CKLP selected as SPI5 clock
  *            @arg RCC_SPI5CLKSOURCE_IC8    IC8 selected as SPI5 clock
  *            @arg RCC_SPI5CLKSOURCE_IC9    IC9 selected as SPI5 clock
  *            @arg RCC_SPI5CLKSOURCE_MSI    MSI selected as SPI5 clock
  *            @arg RCC_SPI5CLKSOURCE_HSI    HSI selected as SPI5 clock
  *            @arg RCC_SPI5CLKSOURCE_PIN    External I2S_CKIN selected as SPI5 clock
  * @retval None
  */
#define __HAL_RCC_SPI5_CONFIG(__SPI5_CLKSOURCE__ ) \
  LL_RCC_SetSPIClockSource((__SPI5_CLKSOURCE__))

/** @brief  Macro to get the SPI5 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_SPI5CLKSOURCE_PCLK2  PCLK2 selected as SPI5 clock
  *            @arg RCC_SPI5CLKSOURCE_CLKP   Peripheral clock CKLP selected as SPI5 clock
  *            @arg RCC_SPI5CLKSOURCE_IC8    IC8 selected as SPI5 clock
  *            @arg RCC_SPI5CLKSOURCE_IC9    IC9 selected as SPI5 clock
  *            @arg RCC_SPI5CLKSOURCE_MSI    MSI selected as SPI5 clock
  *            @arg RCC_SPI5CLKSOURCE_HSI    HSI selected as SPI5 clock
  *            @arg RCC_SPI5CLKSOURCE_PIN    External I2S_CKIN selected as SPI5 clock
  */
#define __HAL_RCC_GET_SPI5_SOURCE() LL_RCC_GetSPIClockSource(LL_RCC_SPI5_CLKSOURCE)

/**
  * @brief  Macro to configure the SPI6 clock source.
  * @param  __SPI6_CLKSOURCE__ defines the SPI6 clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_SPI6CLKSOURCE_PCLK4  PCLK4 selected as SPI6 clock
  *            @arg RCC_SPI6CLKSOURCE_CLKP   Peripheral clock CKLP selected as SPI6 clock
  *            @arg RCC_SPI6CLKSOURCE_IC8    IC8 selected as SPI6 clock
  *            @arg RCC_SPI6CLKSOURCE_IC9    IC9 selected as SPI6 clock
  *            @arg RCC_SPI6CLKSOURCE_MSI    MSI selected as SPI6 clock
  *            @arg RCC_SPI6CLKSOURCE_HSI    HSI selected as SPI6 clock
  *            @arg RCC_SPI6CLKSOURCE_PIN    External I2S_CKIN selected as SPI6 clock
  * @retval None
  */
#define __HAL_RCC_SPI6_CONFIG(__SPI6_CLKSOURCE__ ) \
  LL_RCC_SetSPIClockSource((__SPI6_CLKSOURCE__))

/** @brief  Macro to get the SPI6 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_SPI6CLKSOURCE_PCLK4  PCLK4 selected as SPI6 clock
  *            @arg RCC_SPI6CLKSOURCE_CLKP   Peripheral clock CKLP selected as SPI6 clock
  *            @arg RCC_SPI6CLKSOURCE_IC8    IC8 selected as SPI6 clock
  *            @arg RCC_SPI6CLKSOURCE_IC9    IC9 selected as SPI6 clock
  *            @arg RCC_SPI6CLKSOURCE_MSI    MSI selected as SPI6 clock
  *            @arg RCC_SPI6CLKSOURCE_HSI    HSI selected as SPI6 clock
  *            @arg RCC_SPI6CLKSOURCE_PIN    External I2S_CKIN selected as SPI6 clock
  */
#define __HAL_RCC_GET_SPI6_SOURCE() LL_RCC_GetSPIClockSource(LL_RCC_SPI6_CLKSOURCE)

/** @brief  Macro to configure the USART1 clock source.
  * @param  __USART1_CLKSOURCE__ specifies the USART1 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_USART1CLKSOURCE_PCLK2  PCLK2 selected as USART1 clock
  *            @arg RCC_USART1CLKSOURCE_CLKP   Peripheral clock CKLP selected as USART1 clock
  *            @arg RCC_USART1CLKSOURCE_IC9    IC9 selected as USART1 clock
  *            @arg RCC_USART1CLKSOURCE_IC14   IC14 selected as USART1 clock
  *            @arg RCC_USART1CLKSOURCE_LSE    LSE selected as USART1 clock
  *            @arg RCC_USART1CLKSOURCE_MSI    MSI selected as USART1 clock
  *            @arg RCC_USART1CLKSOURCE_HSI    HSI selected as USART1 clock
  */
#define __HAL_RCC_USART1_CONFIG(__USART1_CLKSOURCE__) \
  LL_RCC_SetUSARTClockSource((__USART1_CLKSOURCE__))

/** @brief  Macro to get the USART1 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_USART1CLKSOURCE_PCLK2  PCLK2 selected as USART1 clock
  *            @arg RCC_USART1CLKSOURCE_CLKP   Peripheral clock CKLP selected as USART1 clock
  *            @arg RCC_USART1CLKSOURCE_IC9    IC9 selected as USART1 clock
  *            @arg RCC_USART1CLKSOURCE_IC14   IC14 selected as USART1 clock
  *            @arg RCC_USART1CLKSOURCE_LSE    LSE selected as USART1 clock
  *            @arg RCC_USART1CLKSOURCE_MSI    MSI selected as USART1 clock
  *            @arg RCC_USART1CLKSOURCE_HSI    HSI selected as USART1 clock
  */
#define __HAL_RCC_GET_USART1_SOURCE() LL_RCC_GetUSARTClockSource(LL_RCC_USART1_CLKSOURCE)

/** @brief  Macro to configure the USART2 clock source.
  * @param  __USART2_CLKSOURCE__ specifies the USART2 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_USART2CLKSOURCE_PCLK1  PCLK1 selected as USART2 clock
  *            @arg RCC_USART2CLKSOURCE_CLKP   Peripheral clock CKLP selected as USART2 clock
  *            @arg RCC_USART2CLKSOURCE_IC9    IC9 selected as USART2 clock
  *            @arg RCC_USART2CLKSOURCE_IC14   IC14 selected as USART2 clock
  *            @arg RCC_USART2CLKSOURCE_LSE    LSE selected as USART2 clock
  *            @arg RCC_USART2CLKSOURCE_MSI    MSI selected as USART2 clock
  *            @arg RCC_USART2CLKSOURCE_HSI    HSI selected as USART2 clock
  */
#define __HAL_RCC_USART2_CONFIG(__USART2_CLKSOURCE__) \
  LL_RCC_SetUSARTClockSource((__USART2_CLKSOURCE__))

/** @brief  Macro to get the USART2 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_USART2CLKSOURCE_PCLK1  PCLK1 selected as USART2 clock
  *            @arg RCC_USART2CLKSOURCE_CLKP   Peripheral clock CKLP selected as USART2 clock
  *            @arg RCC_USART2CLKSOURCE_IC9    IC9 selected as USART2 clock
  *            @arg RCC_USART2CLKSOURCE_IC14   IC14 selected as USART2 clock
  *            @arg RCC_USART2CLKSOURCE_LSE    LSE selected as USART2 clock
  *            @arg RCC_USART2CLKSOURCE_MSI    MSI selected as USART2 clock
  *            @arg RCC_USART2CLKSOURCE_HSI    HSI selected as USART2 clock
  */
#define __HAL_RCC_GET_USART2_SOURCE() LL_RCC_GetUSARTClockSource(LL_RCC_USART2_CLKSOURCE)

/** @brief  Macro to configure the USART3 clock source.
  * @param  __USART3_CLKSOURCE__ specifies the USART3 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_USART3CLKSOURCE_PCLK1  PCLK1 selected as USART3 clock
  *            @arg RCC_USART3CLKSOURCE_CLKP   Peripheral clock CKLP selected as USART3 clock
  *            @arg RCC_USART3CLKSOURCE_IC9    IC9 selected as USART3 clock
  *            @arg RCC_USART3CLKSOURCE_IC14   IC14 selected as USART3 clock
  *            @arg RCC_USART3CLKSOURCE_LSE    LSE selected as USART3 clock
  *            @arg RCC_USART3CLKSOURCE_MSI    MSI selected as USART3 clock
  *            @arg RCC_USART3CLKSOURCE_HSI    HSI selected as USART3 clock
  */
#define __HAL_RCC_USART3_CONFIG(__USART3_CLKSOURCE__) \
  LL_RCC_SetUSARTClockSource((__USART3_CLKSOURCE__))

/** @brief  Macro to get the USART3 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_USART3CLKSOURCE_PCLK1  PCLK1 selected as USART3 clock
  *            @arg RCC_USART3CLKSOURCE_CLKP   Peripheral clock CKLP selected as USART3 clock
  *            @arg RCC_USART3CLKSOURCE_IC9    IC9 selected as USART3 clock
  *            @arg RCC_USART3CLKSOURCE_IC14   IC14 selected as USART3 clock
  *            @arg RCC_USART3CLKSOURCE_LSE    LSE selected as USART3 clock
  *            @arg RCC_USART3CLKSOURCE_MSI    MSI selected as USART3 clock
  *            @arg RCC_USART3CLKSOURCE_HSI    HSI selected as USART3 clock
  */
#define __HAL_RCC_GET_USART3_SOURCE() LL_RCC_GetUSARTClockSource(LL_RCC_USART3_CLKSOURCE)

/** @brief  Macro to configure the UART4 clock source.
  * @param  __UART4_CLKSOURCE__ specifies the UART4 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_UART4CLKSOURCE_PCLK1  PCLK1 selected as UART4 clock
  *            @arg RCC_UART4CLKSOURCE_CLKP   Peripheral clock CKLP selected as UART4 clock
  *            @arg RCC_UART4CLKSOURCE_IC9    IC9 selected as UART4 clock
  *            @arg RCC_UART4CLKSOURCE_IC14   IC14 selected as UART4 clock
  *            @arg RCC_UART4CLKSOURCE_LSE    LSE selected as UART4 clock
  *            @arg RCC_UART4CLKSOURCE_MSI    MSI selected as UART4 clock
  *            @arg RCC_UART4CLKSOURCE_HSI    HSI selected as UART4 clock
  */
#define __HAL_RCC_UART4_CONFIG(__UART4_CLKSOURCE__) \
  LL_RCC_SetUSARTClockSource((__UART4_CLKSOURCE__))

/** @brief  Macro to get the UART4 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_UART4CLKSOURCE_PCLK1  PCLK1 selected as UART4 clock
  *            @arg RCC_UART4CLKSOURCE_CLKP   Peripheral clock CKLP selected as UART4 clock
  *            @arg RCC_UART4CLKSOURCE_IC9    IC9 selected as UART4 clock
  *            @arg RCC_UART4CLKSOURCE_IC14   IC14 selected as UART4 clock
  *            @arg RCC_UART4CLKSOURCE_LSE    LSE selected as UART4 clock
  *            @arg RCC_UART4CLKSOURCE_MSI    MSI selected as UART4 clock
  *            @arg RCC_UART4CLKSOURCE_HSI    HSI selected as UART4 clock
  */
#define __HAL_RCC_GET_UART4_SOURCE() LL_RCC_GetUSARTClockSource(LL_RCC_UART4_CLKSOURCE)

/** @brief  Macro to configure the UART5 clock source.
  * @param  __UART5_CLKSOURCE__ specifies the UART5 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_UART5CLKSOURCE_PCLK1  PCLK1 selected as UART5 clock
  *            @arg RCC_UART5CLKSOURCE_CLKP   Peripheral clock CKLP selected as UART5 clock
  *            @arg RCC_UART5CLKSOURCE_IC9    IC9 selected as UART5 clock
  *            @arg RCC_UART5CLKSOURCE_IC14   IC14 selected as UART5 clock
  *            @arg RCC_UART5CLKSOURCE_LSE    LSE selected as UART5 clock
  *            @arg RCC_UART5CLKSOURCE_MSI    MSI selected as UART5 clock
  *            @arg RCC_UART5CLKSOURCE_HSI    HSI selected as UART5 clock
  */
#define __HAL_RCC_UART5_CONFIG(__UART5_CLKSOURCE__) \
  LL_RCC_SetUSARTClockSource((__UART5_CLKSOURCE__))

/** @brief  Macro to get the UART5 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_UART5CLKSOURCE_PCLK1  PCLK1 selected as UART5 clock
  *            @arg RCC_UART5CLKSOURCE_CLKP   Peripheral clock CKLP selected as UART5 clock
  *            @arg RCC_UART5CLKSOURCE_IC9    IC9 selected as UART5 clock
  *            @arg RCC_UART5CLKSOURCE_IC14   IC14 selected as UART5 clock
  *            @arg RCC_UART5CLKSOURCE_LSE    LSE selected as UART5 clock
  *            @arg RCC_UART5CLKSOURCE_MSI    MSI selected as UART5 clock
  *            @arg RCC_UART5CLKSOURCE_HSI    HSI selected as UART5 clock
  */
#define __HAL_RCC_GET_UART5_SOURCE() LL_RCC_GetUSARTClockSource(LL_RCC_UART5_CLKSOURCE)

/** @brief  Macro to configure the USART6 clock source.
  * @param  __USART6_CLKSOURCE__ specifies the USART6 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_USART6CLKSOURCE_PCLK2  PCLK2 selected as USART6 clock
  *            @arg RCC_USART6CLKSOURCE_CLKP   Peripheral clock CKLP selected as USART6 clock
  *            @arg RCC_USART6CLKSOURCE_IC9    IC9 selected as USART6 clock
  *            @arg RCC_USART6CLKSOURCE_IC14   IC14 selected as USART6 clock
  *            @arg RCC_USART6CLKSOURCE_LSE    LSE selected as USART6 clock
  *            @arg RCC_USART6CLKSOURCE_MSI    MSI selected as USART6 clock
  *            @arg RCC_USART6CLKSOURCE_HSI    HSI selected as USART6 clock
  */
#define __HAL_RCC_USART6_CONFIG(__USART6_CLKSOURCE__) \
  LL_RCC_SetUSARTClockSource((__USART6_CLKSOURCE__))

/** @brief  Macro to get the USART6 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_USART6CLKSOURCE_PCLK2  PCLK2 selected as USART6 clock
  *            @arg RCC_USART6CLKSOURCE_CLKP   Peripheral clock CKLP selected as USART6 clock
  *            @arg RCC_USART6CLKSOURCE_IC9    IC9 selected as USART6 clock
  *            @arg RCC_USART6CLKSOURCE_IC14   IC14 selected as USART6 clock
  *            @arg RCC_USART6CLKSOURCE_LSE    LSE selected as USART6 clock
  *            @arg RCC_USART6CLKSOURCE_MSI    MSI selected as USART6 clock
  *            @arg RCC_USART6CLKSOURCE_HSI    HSI selected as USART6 clock
  */
#define __HAL_RCC_GET_USART6_SOURCE() LL_RCC_GetUSARTClockSource(LL_RCC_USART6_CLKSOURCE)

/** @brief  Macro to configure the UART7 clock source.
  * @param  __UART7_CLKSOURCE__ specifies the UART7 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_UART7CLKSOURCE_PCLK1  PCLK1 selected as UART7 clock
  *            @arg RCC_UART7CLKSOURCE_CLKP   Peripheral clock CKLP selected as UART7 clock
  *            @arg RCC_UART7CLKSOURCE_IC9    IC9 selected as UART7 clock
  *            @arg RCC_UART7CLKSOURCE_IC14   IC14 selected as UART7 clock
  *            @arg RCC_UART7CLKSOURCE_LSE    LSE selected as UART7 clock
  *            @arg RCC_UART7CLKSOURCE_MSI    MSI selected as UART7 clock
  *            @arg RCC_UART7CLKSOURCE_HSI    HSI selected as UART7 clock
  */
#define __HAL_RCC_UART7_CONFIG(__UART7_CLKSOURCE__) \
  LL_RCC_SetUSARTClockSource((__UART7_CLKSOURCE__))

/** @brief  Macro to get the UART7 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_UART7CLKSOURCE_PCLK1  PCLK1 selected as UART7 clock
  *            @arg RCC_UART7CLKSOURCE_CLKP   Peripheral clock CKLP selected as UART7 clock
  *            @arg RCC_UART7CLKSOURCE_IC9    IC9 selected as UART7 clock
  *            @arg RCC_UART7CLKSOURCE_IC14   IC14 selected as UART7 clock
  *            @arg RCC_UART7CLKSOURCE_LSE    LSE selected as UART7 clock
  *            @arg RCC_UART7CLKSOURCE_MSI    MSI selected as UART7 clock
  *            @arg RCC_UART7CLKSOURCE_HSI    HSI selected as UART7 clock
  */
#define __HAL_RCC_GET_UART7_SOURCE() LL_RCC_GetUSARTClockSource(LL_RCC_UART7_CLKSOURCE)

/** @brief  Macro to configure the UART8 clock source.
  * @param  __UART8_CLKSOURCE__ specifies the UART8 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_UART8CLKSOURCE_PCLK1  PCLK1 selected as UART8 clock
  *            @arg RCC_UART8CLKSOURCE_CLKP   Peripheral clock CKLP selected as UART8 clock
  *            @arg RCC_UART8CLKSOURCE_IC9    IC9 selected as UART8 clock
  *            @arg RCC_UART8CLKSOURCE_IC14   IC14 selected as UART8 clock
  *            @arg RCC_UART8CLKSOURCE_LSE    LSE selected as UART8 clock
  *            @arg RCC_UART8CLKSOURCE_MSI    MSI selected as UART8 clock
  *            @arg RCC_UART8CLKSOURCE_HSI    HSI selected as UART8 clock
  */
#define __HAL_RCC_UART8_CONFIG(__UART8_CLKSOURCE__) \
  LL_RCC_SetUSARTClockSource((__UART8_CLKSOURCE__))

/** @brief  Macro to get the UART8 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_UART8CLKSOURCE_PCLK1  PCLK1 selected as UART8 clock
  *            @arg RCC_UART8CLKSOURCE_CLKP   Peripheral clock CKLP selected as UART8 clock
  *            @arg RCC_UART8CLKSOURCE_IC9    IC9 selected as UART8 clock
  *            @arg RCC_UART8CLKSOURCE_IC14   IC14 selected as UART8 clock
  *            @arg RCC_UART8CLKSOURCE_LSE    LSE selected as UART8 clock
  *            @arg RCC_UART8CLKSOURCE_MSI    MSI selected as UART8 clock
  *            @arg RCC_UART8CLKSOURCE_HSI    HSI selected as UART8 clock
  */
#define __HAL_RCC_GET_UART8_SOURCE() LL_RCC_GetUSARTClockSource(LL_RCC_UART8_CLKSOURCE)

/** @brief  Macro to configure the UART9 clock source.
  * @param  __UART9_CLKSOURCE__ specifies the UART9 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_UART9CLKSOURCE_PCLK2  PCLK2 selected as UART9 clock
  *            @arg RCC_UART9CLKSOURCE_CLKP   Peripheral clock CKLP selected as UART9 clock
  *            @arg RCC_UART9CLKSOURCE_IC9    IC9 selected as UART9 clock
  *            @arg RCC_UART9CLKSOURCE_IC14   IC14 selected as UART9 clock
  *            @arg RCC_UART9CLKSOURCE_LSE    LSE selected as UART9 clock
  *            @arg RCC_UART9CLKSOURCE_MSI    MSI selected as UART9 clock
  *            @arg RCC_UART9CLKSOURCE_HSI    HSI selected as UART9 clock
  */
#define __HAL_RCC_UART9_CONFIG(__UART9_CLKSOURCE__) \
  LL_RCC_SetUSARTClockSource((__UART9_CLKSOURCE__))

/** @brief  Macro to get the UART9 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_UART9CLKSOURCE_PCLK2  PCLK2 selected as UART9 clock
  *            @arg RCC_UART9CLKSOURCE_CLKP   Peripheral clock CKLP selected as UART9 clock
  *            @arg RCC_UART9CLKSOURCE_IC9    IC9 selected as UART9 clock
  *            @arg RCC_UART9CLKSOURCE_IC14   IC14 selected as UART9 clock
  *            @arg RCC_UART9CLKSOURCE_LSE    LSE selected as UART9 clock
  *            @arg RCC_UART9CLKSOURCE_MSI    MSI selected as UART9 clock
  *            @arg RCC_UART9CLKSOURCE_HSI    HSI selected as UART9 clock
  */
#define __HAL_RCC_GET_UART9_SOURCE() LL_RCC_GetUSARTClockSource(LL_RCC_UART9_CLKSOURCE)

/** @brief  Macro to configure the USART10 clock source.
  * @param  __USART10_CLKSOURCE__ specifies the USART10 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_USART10CLKSOURCE_PCLK2  PCLK2 selected as USART10 clock
  *            @arg RCC_USART10CLKSOURCE_CLKP   Peripheral clock CKLP selected as USART10 clock
  *            @arg RCC_USART10CLKSOURCE_IC9    IC9 selected as USART10 clock
  *            @arg RCC_USART10CLKSOURCE_IC14   IC14 selected as USART10 clock
  *            @arg RCC_USART10CLKSOURCE_LSE    LSE selected as USART10 clock
  *            @arg RCC_USART10CLKSOURCE_MSI    MSI selected as USART10 clock
  *            @arg RCC_USART10CLKSOURCE_HSI    HSI selected as USART10 clock
  */
#define __HAL_RCC_USART10_CONFIG(__USART10_CLKSOURCE__) \
  LL_RCC_SetUSARTClockSource((__USART10_CLKSOURCE__))

/** @brief  Macro to get the USART10 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_USART10CLKSOURCE_PCLK2  PCLK2 selected as USART10 clock
  *            @arg RCC_USART10CLKSOURCE_CLKP   Peripheral clock CKLP selected as USART10 clock
  *            @arg RCC_USART10CLKSOURCE_IC9    IC9 selected as USART10 clock
  *            @arg RCC_USART10CLKSOURCE_IC14   IC14 selected as USART10 clock
  *            @arg RCC_USART10CLKSOURCE_LSE    LSE selected as USART10 clock
  *            @arg RCC_USART10CLKSOURCE_MSI    MSI selected as USART10 clock
  *            @arg RCC_USART10CLKSOURCE_HSI    HSI selected as USART10 clock
  */
#define __HAL_RCC_GET_USART10_SOURCE() LL_RCC_GetUSARTClockSource(LL_RCC_USART10_CLKSOURCE)

/** @brief  Macro to configure the XSPI1 clock source.
  *
  * @param  __XSPI1_CLKSOURCE__ specifies the XSPI1 clock source.
  *            @arg RCC_XSPI1CLKSOURCE_HCLK  HCLK selected as XSPI1 clock
  *            @arg RCC_XSPI1CLKSOURCE_CLKP Peripheral clock CKLP selected as XSPI1 clock
  *            @arg RCC_XSPI1CLKSOURCE_IC3 IC3 selected as XSPI1 clock
  *            @arg RCC_XSPI1CLKSOURCE_IC4 IC4 selected as XSPI1 clock
  */
#define __HAL_RCC_XSPI1_CONFIG(__XSPI1_CLKSOURCE__) \
  LL_RCC_SetXSPIClockSource((__XSPI1_CLKSOURCE__))

/** @brief  Macro to get the XSPI1 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_XSPI1CLKSOURCE_HCLK  HCLK selected as XSPI1 clock
  *            @arg RCC_XSPI1CLKSOURCE_CLKP Peripheral clock CKLP selected as XSPI1 clock
  *            @arg RCC_XSPI1CLKSOURCE_IC3 IC3 selected as XSPI1 clock
  *            @arg RCC_XSPI1CLKSOURCE_IC4 IC4 selected as XSPI1 clock
  */
#define __HAL_RCC_GET_XSPI1_SOURCE() LL_RCC_GetXSPIClockSource(LL_RCC_XSPI1_CLKSOURCE)

/** @brief  Macro to configure the XSPI2 clock source.
  *
  * @param  __XSPI2_CLKSOURCE__ specifies the XSPI2 clock source.
  *            @arg RCC_XSPI2CLKSOURCE_HCLK  HCLK selected as XSPI2 clock
  *            @arg RCC_XSPI2CLKSOURCE_CLKP Peripheral clock CKLP selected as XSPI2 clock
  *            @arg RCC_XSPI2CLKSOURCE_IC3 IC3 selected as XSPI2 clock
  *            @arg RCC_XSPI2CLKSOURCE_IC4 IC4 selected as XSPI2 clock
  */
#define __HAL_RCC_XSPI2_CONFIG(__XSPI2_CLKSOURCE__) \
  LL_RCC_SetXSPIClockSource((__XSPI2_CLKSOURCE__))

/** @brief  Macro to get the XSPI2 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_XSPI2CLKSOURCE_HCLK  HCLK selected as XSPI2 clock
  *            @arg RCC_XSPI2CLKSOURCE_CLKP Peripheral clock CKLP selected as XSPI2 clock
  *            @arg RCC_XSPI2CLKSOURCE_IC3 IC3 selected as XSPI2 clock
  *            @arg RCC_XSPI2CLKSOURCE_IC4 IC4 selected as XSPI2 clock
  */
#define __HAL_RCC_GET_XSPI2_SOURCE() LL_RCC_GetXSPIClockSource(LL_RCC_XSPI2_CLKSOURCE)

/** @brief  Macro to configure the XSPI3 clock source.
  *
  * @param  __XSPI3_CLKSOURCE__ specifies the XSPI3 clock source.
  *            @arg RCC_XSPI3CLKSOURCE_HCLK  HCLK selected as XSPI3 clock
  *            @arg RCC_XSPI3CLKSOURCE_CLKP Peripheral clock CKLP selected as XSPI3 clock
  *            @arg RCC_XSPI3CLKSOURCE_IC3 IC3 selected as XSPI3 clock
  *            @arg RCC_XSPI3CLKSOURCE_IC4 IC4 selected as XSPI3 clock
  */
#define __HAL_RCC_XSPI3_CONFIG(__XSPI3_CLKSOURCE__) \
  LL_RCC_SetXSPIClockSource((__XSPI3_CLKSOURCE__))

/** @brief  Macro to get the XSPI3 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_XSPI3CLKSOURCE_HCLK  HCLK selected as XSPI3 clock
  *            @arg RCC_XSPI3CLKSOURCE_CLKP Peripheral clock CKLP selected as XSPI3 clock
  *            @arg RCC_XSPI3CLKSOURCE_IC3 IC3 selected as XSPI3 clock
  *            @arg RCC_XSPI3CLKSOURCE_IC4 IC4 selected as XSPI3 clock
  */
#define __HAL_RCC_GET_XSPI3_SOURCE() LL_RCC_GetXSPIClockSource(LL_RCC_XSPI3_CLKSOURCE)

/** @brief  Macro to configure the Timers clocks prescaler
  * @param  __PRESC__  specifies the Timers clocks prescaler selection
  *         This parameter can be one of the following values:
  *            @arg RCC_TIMPRES_DIV1 The Timers kernels clocks prescaler is 1
  *            @arg RCC_TIMPRES_DIV2 The Timers kernels clocks prescaler is 2
  *            @arg RCC_TIMPRES_DIV4 The Timers kernels clocks prescaler is 4
  *            @arg RCC_TIMPRES_DIV8 The Timers kernels clocks prescaler is 8
  */
#define __HAL_RCC_TIMCLKPRESCALER_CONFIG(__PRESC__) LL_RCC_SetTIMPrescaler((__PRESC__))

/** @brief  Macro to get the Timers clocks prescaler.
  * @retval Timers clocks prescaler can be one of the following values:
  *            @arg RCC_TIMPRES_DIV1 The Timers kernels clocks prescaler is 1
  *            @arg RCC_TIMPRES_DIV2 The Timers kernels clocks prescaler is 2
  *            @arg RCC_TIMPRES_DIV4 The Timers kernels clocks prescaler is 4
  *            @arg RCC_TIMPRES_DIV8 The Timers kernels clocks prescaler is 8
  */
#define __HAL_RCC_GET_TIMCLKPRESCALER() LL_RCC_GetTIMPrescaler()

/** @brief  Macro to configure the USBPHY1 clock.
  * @param  __USBPHY1_CLKSOURCE__ specifies the USBPHY1 clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_USBPHY1REFCLKSOURCE_OTGPHY1         USB OTGPHY1 kernel clock selected as USBPHY1 clock
  *            @arg RCC_USBPHY1REFCLKSOURCE_HSE_DIRECT      HSE from oscillator selected as USBPHY1 clock
  *            @arg RCC_USBPHY1REFCLKSOURCE_HSE_DIRECT_DIV2 HSE from oscillator divided by 2 selected as USBPHY1 clock
  */
#define __HAL_RCC_USBPHY1_CONFIG(__USBPHY1_CLKSOURCE__)                               \
  do                                                                                  \
  {                                                                                   \
    LL_RCC_SetOTGPHYCKREFClockSource((__USBPHY1_CLKSOURCE__) & 0x7FFFFFFFUL);         \
    if(((__USBPHY1_CLKSOURCE__)>>31U) == 1UL) {LL_RCC_HSE_SelectHSEDiv2AsDiv2Clock();} \
      else {LL_RCC_HSE_SelectHSEAsDiv2Clock();}                                         \
  } while (0)


/** @brief  Macro to get the USBPHY1 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_USBPHY1REFCLKSOURCE_OTGPHY1         USB OTGPHY1 kernel clock selected as USBPHY1 clock
  *            @arg RCC_USBPHY1REFCLKSOURCE_HSE_DIRECT      HSE from oscillator selected as USBPHY1 clock
  *            @arg RCC_USBPHY1REFCLKSOURCE_HSE_DIRECT_DIV2 HSE from oscillator divided by 2 selected as USBPHY1 clock
  */
#define __HAL_RCC_GET_USBPHY1_SOURCE()                       \
  (LL_RCC_GetOTGPHYCKREFClockSource(LL_RCC_OTGPHY1CKREF_CLKSOURCE) | (LL_RCC_HSE_IsSelectedHSEDiv2AsDiv2Clock()<<31U))

/** @brief  Macro to configure the USBPHY2 clock.
  * @param  __USBPHY2_CLKSOURCE__ specifies the USBPHY2 clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_USBPHY2REFCLKSOURCE_OTGPHY2         USB OTGPHY2 kernel clock selected as USBPHY2 clock
  *            @arg RCC_USBPHY2REFCLKSOURCE_HSE_DIRECT      HSE from oscillator selected as USBPHY2 clock
  *            @arg RCC_USBPHY2REFCLKSOURCE_HSE_DIRECT_DIV2 HSE from oscillator divided by 2 selected as USBPHY2 clock
  */
#define __HAL_RCC_USBPHY2_CONFIG(__USBPHY2_CLKSOURCE__)                                 \
  do                                                                                    \
  {                                                                                     \
    LL_RCC_SetOTGPHYCKREFClockSource((__USBPHY2_CLKSOURCE__) & 0x7FFFFFFFUL);           \
    if(((__USBPHY2_CLKSOURCE__)>>31U) == 1UL) {LL_RCC_HSE_SelectHSEDiv2AsDiv2Clock();} \
      else {LL_RCC_HSE_SelectHSEAsDiv2Clock();}                                         \
  } while (0)

/** @brief  Macro to get the USBPHY2 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_USBPHY2REFCLKSOURCE_OTGPHY2         USB OTGPHY2 kernel clock selected as USBPHY2 clock
  *            @arg RCC_USBPHY2REFCLKSOURCE_HSE_DIRECT      HSE from oscillator selected as USBPHY2 clock
  *            @arg RCC_USBPHY2REFCLKSOURCE_HSE_DIRECT_DIV2 HSE from oscillator divided by 2 selected as USBPHY2 clock
  */
#define __HAL_RCC_GET_USBPHY2_SOURCE()                       \
  (LL_RCC_GetOTGPHYCKREFClockSource(LL_RCC_OTGPHY2CKREF_CLKSOURCE) | (LL_RCC_HSE_IsSelectedHSEDiv2AsDiv2Clock()<<31U))

/** @brief  Macro to configure the USB OTGHS1 clock.
  * @param  __USBOTGHS1_CLKSOURCE__ specifies the USB OTGHS1 clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_USBOTGHS1CLKSOURCE_HSE_DIV2      HSE divided by 2 selected as USBOTGHS1 clock
  *            @arg RCC_USBOTGHS1CLKSOURCE_CLKP          Peripheral clock CKLP selected as USBOTGHS1 clock
  *            @arg RCC_USBOTGHS1CLKSOURCE_IC15          IC15 selected as USBOTGHS1 clock
  *            @arg RCC_USBOTGHS1CLKSOURCE_HSE_DIV_2_OSC HSE divided by 2 from oscillator selected as USBOTGHS1 clock
  */
#define __HAL_RCC_USBOTGHS1_CONFIG(__USBOTGHS1_CLKSOURCE__)                             \
  do                                                                                    \
  {                                                                                     \
    LL_RCC_SetOTGPHYClockSource((__USBOTGHS1_CLKSOURCE__) & 0x7FFFFFFFUL);              \
    if(((__USBOTGHS1_CLKSOURCE__)>>31U) == 1UL) {LL_RCC_HSE_SelectHSEDiv2AsDiv2Clock();} \
      else {LL_RCC_HSE_SelectHSEAsDiv2Clock();}                                           \
  } while (0)

/** @brief  Macro to get the USB OTGHS1 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_USBOTGHS1CLKSOURCE_HSE_DIV2      HSE divided by 2 selected as USBOTGHS1 clock
  *            @arg RCC_USBOTGHS1CLKSOURCE_CLKP          Peripheral clock CKLP selected as USBOTGHS1 clock
  *            @arg RCC_USBOTGHS1CLKSOURCE_IC15          IC15 selected as USBOTGHS1 clock
  *            @arg RCC_USBOTGHS1CLKSOURCE_HSE_DIV_2_OSC HSE divided by 2 from oscillator selected as USBOTGHS1 clock
  */
#define __HAL_RCC_GET_USBOTGHS1_SOURCE()                       \
  (LL_RCC_GetOTGPHYClockSource(LL_RCC_OTGPHY1_CLKSOURCE) | (LL_RCC_HSE_IsSelectedHSEDiv2AsDiv2Clock()<<31U))

/** @brief  Macro to configure the USB OTGHS2 clock.
  * @param  __USBOTGHS2_CLKSOURCE__ specifies the USB OTGHS2 clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_USBOTGHS2CLKSOURCE_HSE_DIV2      HSE divided by 2 selected as USBOTGHS2 clock
  *            @arg RCC_USBOTGHS2CLKSOURCE_CLKP          Peripheral clock CKLP selected as USBOTGHS2 clock
  *            @arg RCC_USBOTGHS2CLKSOURCE_IC15          IC15 selected as USBOTGHS2 clock
  *            @arg RCC_USBOTGHS2CLKSOURCE_HSE_DIV_2_OSC HSE divided by 2 from oscillator selected as USBOTGHS2 clock
  */
#define __HAL_RCC_USBOTGHS2_CONFIG(__USBOTGHS2_CLKSOURCE__)                             \
  do                                                                                    \
  {                                                                                     \
    LL_RCC_SetOTGPHYClockSource((__USBOTGHS2_CLKSOURCE__) & 0x7FFFFFFFUL);              \
    if(((__USBOTGHS2_CLKSOURCE__)>>31U) == 1UL) {LL_RCC_HSE_SelectHSEDiv2AsDiv2Clock();} \
      else {LL_RCC_HSE_SelectHSEAsDiv2Clock();}                                           \
  } while (0)

/** @brief  Macro to get the USB OTGHS2 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_USBOTGHS2CLKSOURCE_HSE_DIV2      HSE divided by 2 selected as USBOTGHS2 clock
  *            @arg RCC_USBOTGHS2CLKSOURCE_CLKP          Peripheral clock CKLP selected as USBOTGHS2 clock
  *            @arg RCC_USBOTGHS2C LKSOURCE_IC15          IC15 selected as USBOTGHS2 clock
  *            @arg RCC_USBOTGHS2CLKSOURCE_HSE_DIV_2_OSC HSE divided by 2 from oscillator selected as USBOTGHS2 clock
  */
#define __HAL_RCC_GET_USBOTGHS2_SOURCE()                       \
  (LL_RCC_GetOTGPHYClockSource(LL_RCC_OTGPHY2_CLKSOURCE) | (LL_RCC_HSE_IsSelectedHSEDiv2AsDiv2Clock()<<31U))

/**
  * @brief Enable the RCC LSE CSS Extended Interrupt Line.
  * @retval None
  */
#define __HAL_RCC_LSECSS_EXTI_ENABLE_IT()      SET_BIT(EXTI->IMR3, RCC_EXTI_LINE_LSECSS)

/**
  * @brief Disable the RCC LSE CSS Extended Interrupt Line.
  * @retval None
  */
#define __HAL_RCC_LSECSS_EXTI_DISABLE_IT()     CLEAR_BIT(EXTI->IMR3, RCC_EXTI_LINE_LSECSS)

/**
  * @brief Enable the RCC LSE CSS Event Line.
  * @retval None.
  */
#define __HAL_RCC_LSECSS_EXTI_ENABLE_EVENT()   SET_BIT(EXTI->EMR3, RCC_EXTI_LINE_LSECSS)

/**
  * @brief Disable the RCC LSE CSS Event Line.
  * @retval None.
  */
#define __HAL_RCC_LSECSS_EXTI_DISABLE_EVENT()  CLEAR_BIT(EXTI->EMR3, RCC_EXTI_LINE_LSECSS)

/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/
/** @addtogroup RCCEx_Exported_Functions
  * @{
  */

/** @addtogroup RCCEx_Exported_Functions_Group1
  * @{
  */
HAL_StatusTypeDef HAL_RCCEx_PeriphCLKConfig(const RCC_PeriphCLKInitTypeDef  *PeriphClkInit);
void HAL_RCCEx_GetPeriphCLKConfig(RCC_PeriphCLKInitTypeDef  *PeriphClkInit);
uint32_t HAL_RCCEx_GetPeriphCLKFreq(uint64_t PeriphClk);
uint32_t HAL_RCCEx_GetPLL1CLKFreq(void);
uint32_t HAL_RCCEx_GetPLL2CLKFreq(void);
uint32_t HAL_RCCEx_GetPLL3CLKFreq(void);
uint32_t HAL_RCCEx_GetPLL4CLKFreq(void);
/**
  * @}
  */

/** @addtogroup RCCEx_Exported_Functions_Group2
  * @{
  */
void     HAL_RCCEx_WakeUpStopCLKConfig(uint32_t WakeUpClk);
void     HAL_RCCEx_EnableLSECSS(void);
void     HAL_RCCEx_DisableLSECSS(void);
void     HAL_RCCEx_EnableLSECSS_IT(void);
void     HAL_RCCEx_DisableLSECSS_IT(void);
void     HAL_RCCEx_ReArmLSECSS(void);
void     HAL_RCCEx_LSECSS_IRQHandler(void);
void     HAL_RCCEx_LSECSS_Callback(void);
HAL_StatusTypeDef HAL_RCCEx_PLLSSCGConfig(uint32_t PLLnumber, const RCC_PLLInitTypeDef *pPLLInit,
                                          const RCC_PLLSSCGInitTypeDef *pPLLSSCGInit);

/**
  * @}
  */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @addtogroup RCCEx_Private_Macros
  * @{
  */

/** @defgroup RCCEx_IS_RCC_Definitions Private macros to check input parameters
  * @{
  */
#define IS_RCC_PERIPHCLOCK(__SELECTION__)  \
  ((((__SELECTION__) & RCC_PERIPHCLK_FMC)         == RCC_PERIPHCLK_FMC)         || \
   (((__SELECTION__) & RCC_PERIPHCLK_XSPI1)       == RCC_PERIPHCLK_XSPI1)       || \
   (((__SELECTION__) & RCC_PERIPHCLK_XSPI2)       == RCC_PERIPHCLK_XSPI2)       || \
   (((__SELECTION__) & RCC_PERIPHCLK_XSPI3)       == RCC_PERIPHCLK_XSPI3)       || \
   (((__SELECTION__) & RCC_PERIPHCLK_CKPER)       == RCC_PERIPHCLK_CKPER)       || \
   (((__SELECTION__) & RCC_PERIPHCLK_ADC)         == RCC_PERIPHCLK_ADC)         || \
   (((__SELECTION__) & RCC_PERIPHCLK_ADF1)        == RCC_PERIPHCLK_ADF1)        || \
   (((__SELECTION__) & RCC_PERIPHCLK_CSI)         == RCC_PERIPHCLK_CSI)         || \
   (((__SELECTION__) & RCC_PERIPHCLK_DCMIPP)      == RCC_PERIPHCLK_DCMIPP)      || \
   (((__SELECTION__) & RCC_PERIPHCLK_ETH1)        == RCC_PERIPHCLK_ETH1)        || \
   (((__SELECTION__) & RCC_PERIPHCLK_ETH1PHY)     == RCC_PERIPHCLK_ETH1PHY)     || \
   (((__SELECTION__) & RCC_PERIPHCLK_ETH1RX)      == RCC_PERIPHCLK_ETH1RX)      || \
   (((__SELECTION__) & RCC_PERIPHCLK_ETH1TX)      == RCC_PERIPHCLK_ETH1TX)      || \
   (((__SELECTION__) & RCC_PERIPHCLK_ETH1PTP)     == RCC_PERIPHCLK_ETH1PTP)     || \
   (((__SELECTION__) & RCC_PERIPHCLK_FDCAN)       == RCC_PERIPHCLK_FDCAN)       || \
   (((__SELECTION__) & RCC_PERIPHCLK_I2C1)        == RCC_PERIPHCLK_I2C1)        || \
   (((__SELECTION__) & RCC_PERIPHCLK_I2C2)        == RCC_PERIPHCLK_I2C2)        || \
   (((__SELECTION__) & RCC_PERIPHCLK_I2C3)        == RCC_PERIPHCLK_I2C3)        || \
   (((__SELECTION__) & RCC_PERIPHCLK_I2C4)        == RCC_PERIPHCLK_I2C4)        || \
   (((__SELECTION__) & RCC_PERIPHCLK_I3C1)        == RCC_PERIPHCLK_I3C1)        || \
   (((__SELECTION__) & RCC_PERIPHCLK_I3C2)        == RCC_PERIPHCLK_I3C2)        || \
   (((__SELECTION__) & RCC_PERIPHCLK_LPTIM1)      == RCC_PERIPHCLK_LPTIM1)      || \
   (((__SELECTION__) & RCC_PERIPHCLK_LPTIM2)      == RCC_PERIPHCLK_LPTIM2)      || \
   (((__SELECTION__) & RCC_PERIPHCLK_LPTIM3)      == RCC_PERIPHCLK_LPTIM3)      || \
   (((__SELECTION__) & RCC_PERIPHCLK_LPTIM4)      == RCC_PERIPHCLK_LPTIM4)      || \
   (((__SELECTION__) & RCC_PERIPHCLK_LPTIM5)      == RCC_PERIPHCLK_LPTIM5)      || \
   (((__SELECTION__) & RCC_PERIPHCLK_LPUART1)     == RCC_PERIPHCLK_LPUART1)     || \
   (((__SELECTION__) & RCC_PERIPHCLK_LTDC)        == RCC_PERIPHCLK_LTDC)        || \
   (((__SELECTION__) & RCC_PERIPHCLK_MDF1)        == RCC_PERIPHCLK_MDF1)        || \
   (((__SELECTION__) & RCC_PERIPHCLK_PSSI)        == RCC_PERIPHCLK_PSSI)        || \
   (((__SELECTION__) & RCC_PERIPHCLK_RTC)         == RCC_PERIPHCLK_RTC)         || \
   (((__SELECTION__) & RCC_PERIPHCLK_SAI1)        == RCC_PERIPHCLK_SAI1)        || \
   (((__SELECTION__) & RCC_PERIPHCLK_SAI2)        == RCC_PERIPHCLK_SAI2)        || \
   (((__SELECTION__) & RCC_PERIPHCLK_SDMMC1)      == RCC_PERIPHCLK_SDMMC1)      || \
   (((__SELECTION__) & RCC_PERIPHCLK_SDMMC2)      == RCC_PERIPHCLK_SDMMC2)      || \
   (((__SELECTION__) & RCC_PERIPHCLK_SPDIFRX1)    == RCC_PERIPHCLK_SPDIFRX1)    || \
   (((__SELECTION__) & RCC_PERIPHCLK_SPI1)        == RCC_PERIPHCLK_SPI1)        || \
   (((__SELECTION__) & RCC_PERIPHCLK_SPI2)        == RCC_PERIPHCLK_SPI2)        || \
   (((__SELECTION__) & RCC_PERIPHCLK_SPI3)        == RCC_PERIPHCLK_SPI3)        || \
   (((__SELECTION__) & RCC_PERIPHCLK_SPI4)        == RCC_PERIPHCLK_SPI4)        || \
   (((__SELECTION__) & RCC_PERIPHCLK_SPI5)        == RCC_PERIPHCLK_SPI5)        || \
   (((__SELECTION__) & RCC_PERIPHCLK_SPI6)        == RCC_PERIPHCLK_SPI6)        || \
   (((__SELECTION__) & RCC_PERIPHCLK_TIM)         == RCC_PERIPHCLK_TIM)         || \
   (((__SELECTION__) & RCC_PERIPHCLK_USART1)      == RCC_PERIPHCLK_USART1)      || \
   (((__SELECTION__) & RCC_PERIPHCLK_USART2)      == RCC_PERIPHCLK_USART2)      || \
   (((__SELECTION__) & RCC_PERIPHCLK_USART3)      == RCC_PERIPHCLK_USART3)      || \
   (((__SELECTION__) & RCC_PERIPHCLK_UART4)       == RCC_PERIPHCLK_UART4)       || \
   (((__SELECTION__) & RCC_PERIPHCLK_UART5)       == RCC_PERIPHCLK_UART5)       || \
   (((__SELECTION__) & RCC_PERIPHCLK_USART6)      == RCC_PERIPHCLK_USART6)      || \
   (((__SELECTION__) & RCC_PERIPHCLK_UART7)       == RCC_PERIPHCLK_UART7)       || \
   (((__SELECTION__) & RCC_PERIPHCLK_UART8)       == RCC_PERIPHCLK_UART8)       || \
   (((__SELECTION__) & RCC_PERIPHCLK_UART9)       == RCC_PERIPHCLK_UART9)       || \
   (((__SELECTION__) & RCC_PERIPHCLK_USART10)     == RCC_PERIPHCLK_USART10)     || \
   (((__SELECTION__) & RCC_PERIPHCLK_USBPHY1)     == RCC_PERIPHCLK_USBPHY1)     || \
   (((__SELECTION__) & RCC_PERIPHCLK_USBPHY2)     == RCC_PERIPHCLK_USBPHY2)     || \
   (((__SELECTION__) & RCC_PERIPHCLK_USBOTGHS1)   == RCC_PERIPHCLK_USBOTGHS1)   || \
   (((__SELECTION__) & RCC_PERIPHCLK_USBOTGHS2)   == RCC_PERIPHCLK_USBOTGHS2))

#define IS_RCC_ADCCLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_ADCCLKSOURCE_HCLK) || \
   ((__SOURCE__) == RCC_ADCCLKSOURCE_CLKP) || \
   ((__SOURCE__) == RCC_ADCCLKSOURCE_IC7)  || \
   ((__SOURCE__) == RCC_ADCCLKSOURCE_IC8)  || \
   ((__SOURCE__) == RCC_ADCCLKSOURCE_MSI)  || \
   ((__SOURCE__) == RCC_ADCCLKSOURCE_HSI)  || \
   ((__SOURCE__) == RCC_ADCCLKSOURCE_PIN)  || \
   ((__SOURCE__) == RCC_ADCCLKSOURCE_TIMG))

#define IS_RCC_ADCDIVIDER(__VALUE__) \
  ((1U <= (__VALUE__)) && ((__VALUE__) <= 256U))

#define IS_RCC_ADF1CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_ADF1CLKSOURCE_HCLK) || \
   ((__SOURCE__) == RCC_ADF1CLKSOURCE_CLKP) || \
   ((__SOURCE__) == RCC_ADF1CLKSOURCE_IC7)  || \
   ((__SOURCE__) == RCC_ADF1CLKSOURCE_IC8)  || \
   ((__SOURCE__) == RCC_ADF1CLKSOURCE_MSI)  || \
   ((__SOURCE__) == RCC_ADF1CLKSOURCE_HSI)  || \
   ((__SOURCE__) == RCC_ADF1CLKSOURCE_PIN)  || \
   ((__SOURCE__) == RCC_ADF1CLKSOURCE_TIMG))

#define IS_RCC_CKPERCLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_CLKPCLKSOURCE_HSI) || \
   ((__SOURCE__) == RCC_CLKPCLKSOURCE_MSI) || \
   ((__SOURCE__) == RCC_CLKPCLKSOURCE_HSE) || \
   ((__SOURCE__) == RCC_CLKPCLKSOURCE_IC19) || \
   ((__SOURCE__) == RCC_CLKPCLKSOURCE_IC5) || \
   ((__SOURCE__) == RCC_CLKPCLKSOURCE_IC10) || \
   ((__SOURCE__) == RCC_CLKPCLKSOURCE_IC15) || \
   ((__SOURCE__) == RCC_CLKPCLKSOURCE_IC20))

#define IS_RCC_DCMIPPCLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_DCMIPPCLKSOURCE_PCLK5) || \
   ((__SOURCE__) == RCC_DCMIPPCLKSOURCE_CLKP)  || \
   ((__SOURCE__) == RCC_DCMIPPCLKSOURCE_IC17)  || \
   ((__SOURCE__) == RCC_DCMIPPCLKSOURCE_HSI))

#define IS_RCC_ETH1CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_ETH1CLKSOURCE_HCLK) || \
   ((__SOURCE__) == RCC_ETH1CLKSOURCE_CLKP) || \
   ((__SOURCE__) == RCC_ETH1CLKSOURCE_IC12) || \
   ((__SOURCE__) == RCC_ETH1CLKSOURCE_HSE))

#define IS_RCC_ETH1PHYIF(__SOURCE__) \
  (((__SOURCE__) == RCC_ETH1PHYIF_MII)   || \
   ((__SOURCE__) == RCC_ETH1PHYIF_RGMII) || \
   ((__SOURCE__) == RCC_ETH1PHYIF_RMII))

#define IS_RCC_ETH1RXCLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_ETH1RXCLKSOURCE_EXT)   || \
   ((__SOURCE__) == RCC_ETH1RXCLKSOURCE_INT))

#define IS_RCC_ETH1TXCLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_ETH1TXCLKSOURCE_EXT)   || \
   ((__SOURCE__) == RCC_ETH1TXCLKSOURCE_INT))

#define IS_RCC_ETH1PTPCLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_ETH1PTPCLKSOURCE_HCLK) || \
   ((__SOURCE__) == RCC_ETH1PTPCLKSOURCE_CLKP) || \
   ((__SOURCE__) == RCC_ETH1PTPCLKSOURCE_IC13) || \
   ((__SOURCE__) == RCC_ETH1PTPCLKSOURCE_HSE))

#define IS_RCC_ETH1PTPDIVIDER(__VALUE__) \
  ((1U <= (__VALUE__)) && ((__VALUE__) <= 16U))

#define IS_RCC_FDCANCLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_FDCANCLKSOURCE_PCLK1) || \
   ((__SOURCE__) == RCC_FDCANCLKSOURCE_CLKP)  || \
   ((__SOURCE__) == RCC_FDCANCLKSOURCE_IC19)  || \
   ((__SOURCE__) == RCC_FDCANCLKSOURCE_HSE))

#define IS_RCC_FMCCLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_FMCCLKSOURCE_HCLK)  || \
   ((__SOURCE__) == RCC_FMCCLKSOURCE_CLKP) || \
   ((__SOURCE__) == RCC_FMCCLKSOURCE_IC3) || \
   ((__SOURCE__) == RCC_FMCCLKSOURCE_IC4))

#define IS_RCC_I2C1CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_I2C1CLKSOURCE_PCLK1) || \
   ((__SOURCE__) == RCC_I2C1CLKSOURCE_CLKP) || \
   ((__SOURCE__) == RCC_I2C1CLKSOURCE_IC10) || \
   ((__SOURCE__) == RCC_I2C1CLKSOURCE_IC15) || \
   ((__SOURCE__) == RCC_I2C1CLKSOURCE_MSI)   || \
   ((__SOURCE__) == RCC_I2C1CLKSOURCE_HSI))

#define IS_RCC_I2C2CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_I2C2CLKSOURCE_PCLK1) || \
   ((__SOURCE__) == RCC_I2C2CLKSOURCE_CLKP) || \
   ((__SOURCE__) == RCC_I2C2CLKSOURCE_IC10) || \
   ((__SOURCE__) == RCC_I2C2CLKSOURCE_IC15) || \
   ((__SOURCE__) == RCC_I2C2CLKSOURCE_MSI)   || \
   ((__SOURCE__) == RCC_I2C2CLKSOURCE_HSI))

#define IS_RCC_I2C3CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_I2C3CLKSOURCE_PCLK1) || \
   ((__SOURCE__) == RCC_I2C3CLKSOURCE_CLKP) || \
   ((__SOURCE__) == RCC_I2C3CLKSOURCE_IC10) || \
   ((__SOURCE__) == RCC_I2C3CLKSOURCE_IC15) || \
   ((__SOURCE__) == RCC_I2C3CLKSOURCE_MSI)   || \
   ((__SOURCE__) == RCC_I2C3CLKSOURCE_HSI))

#define IS_RCC_I2C4CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_I2C4CLKSOURCE_PCLK1) || \
   ((__SOURCE__) == RCC_I2C4CLKSOURCE_CLKP) || \
   ((__SOURCE__) == RCC_I2C4CLKSOURCE_IC10) || \
   ((__SOURCE__) == RCC_I2C4CLKSOURCE_IC15) || \
   ((__SOURCE__) == RCC_I2C4CLKSOURCE_MSI)   || \
   ((__SOURCE__) == RCC_I2C4CLKSOURCE_HSI))

#define IS_RCC_I3C1CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_I3C1CLKSOURCE_PCLK1) || \
   ((__SOURCE__) == RCC_I3C1CLKSOURCE_CLKP) || \
   ((__SOURCE__) == RCC_I3C1CLKSOURCE_IC10) || \
   ((__SOURCE__) == RCC_I3C1CLKSOURCE_IC15) || \
   ((__SOURCE__) == RCC_I3C1CLKSOURCE_MSI)   || \
   ((__SOURCE__) == RCC_I3C1CLKSOURCE_HSI))

#define IS_RCC_I3C2CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_I3C2CLKSOURCE_PCLK1) || \
   ((__SOURCE__) == RCC_I3C2CLKSOURCE_CLKP) || \
   ((__SOURCE__) == RCC_I3C2CLKSOURCE_IC10) || \
   ((__SOURCE__) == RCC_I3C2CLKSOURCE_IC15) || \
   ((__SOURCE__) == RCC_I3C2CLKSOURCE_MSI)   || \
   ((__SOURCE__) == RCC_I3C2CLKSOURCE_HSI))

#define IS_RCC_LPTIM1CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_LPTIM1CLKSOURCE_PCLK1) || \
   ((__SOURCE__) == RCC_LPTIM1CLKSOURCE_CLKP) || \
   ((__SOURCE__) == RCC_LPTIM1CLKSOURCE_IC15) || \
   ((__SOURCE__) == RCC_LPTIM1CLKSOURCE_LSE)   || \
   ((__SOURCE__) == RCC_LPTIM1CLKSOURCE_LSI)   || \
   ((__SOURCE__) == RCC_LPTIM1CLKSOURCE_TIMG))

#define IS_RCC_LPTIM2CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_LPTIM2CLKSOURCE_PCLK4) || \
   ((__SOURCE__) == RCC_LPTIM2CLKSOURCE_CLKP) || \
   ((__SOURCE__) == RCC_LPTIM2CLKSOURCE_IC15) || \
   ((__SOURCE__) == RCC_LPTIM2CLKSOURCE_LSE)   || \
   ((__SOURCE__) == RCC_LPTIM2CLKSOURCE_LSI)   || \
   ((__SOURCE__) == RCC_LPTIM2CLKSOURCE_TIMG))

#define IS_RCC_LPTIM3CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_LPTIM3CLKSOURCE_PCLK4) || \
   ((__SOURCE__) == RCC_LPTIM3CLKSOURCE_CLKP) || \
   ((__SOURCE__) == RCC_LPTIM3CLKSOURCE_IC15) || \
   ((__SOURCE__) == RCC_LPTIM3CLKSOURCE_LSE)   || \
   ((__SOURCE__) == RCC_LPTIM3CLKSOURCE_LSI)   || \
   ((__SOURCE__) == RCC_LPTIM3CLKSOURCE_TIMG))

#define IS_RCC_LPTIM4CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_LPTIM4CLKSOURCE_PCLK4) || \
   ((__SOURCE__) == RCC_LPTIM4CLKSOURCE_CLKP) || \
   ((__SOURCE__) == RCC_LPTIM4CLKSOURCE_IC15) || \
   ((__SOURCE__) == RCC_LPTIM4CLKSOURCE_LSE)   || \
   ((__SOURCE__) == RCC_LPTIM4CLKSOURCE_LSI)   || \
   ((__SOURCE__) == RCC_LPTIM4CLKSOURCE_TIMG))

#define IS_RCC_LPTIM5CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_LPTIM5CLKSOURCE_PCLK4) || \
   ((__SOURCE__) == RCC_LPTIM5CLKSOURCE_CLKP) || \
   ((__SOURCE__) == RCC_LPTIM5CLKSOURCE_IC15) || \
   ((__SOURCE__) == RCC_LPTIM5CLKSOURCE_LSE)   || \
   ((__SOURCE__) == RCC_LPTIM5CLKSOURCE_LSI)   || \
   ((__SOURCE__) == RCC_LPTIM5CLKSOURCE_TIMG))

#define IS_RCC_LPUART1CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_LPUART1CLKSOURCE_PCLK4) || \
   ((__SOURCE__) == RCC_LPUART1CLKSOURCE_CLKP)  || \
   ((__SOURCE__) == RCC_LPUART1CLKSOURCE_IC9)   || \
   ((__SOURCE__) == RCC_LPUART1CLKSOURCE_IC14)  || \
   ((__SOURCE__) == RCC_LPUART1CLKSOURCE_LSE)   || \
   ((__SOURCE__) == RCC_LPUART1CLKSOURCE_MSI)   || \
   ((__SOURCE__) == RCC_LPUART1CLKSOURCE_HSI))

#define IS_RCC_LTDCCLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_LTDCCLKSOURCE_PCLK5) || \
   ((__SOURCE__) == RCC_LTDCCLKSOURCE_CLKP)  || \
   ((__SOURCE__) == RCC_LTDCCLKSOURCE_IC16)  || \
   ((__SOURCE__) == RCC_LTDCCLKSOURCE_HSI))

#define IS_RCC_MDF1CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_MDF1CLKSOURCE_HCLK) || \
   ((__SOURCE__) == RCC_MDF1CLKSOURCE_CLKP) || \
   ((__SOURCE__) == RCC_MDF1CLKSOURCE_IC7)  || \
   ((__SOURCE__) == RCC_MDF1CLKSOURCE_IC8)  || \
   ((__SOURCE__) == RCC_MDF1CLKSOURCE_MSI)  || \
   ((__SOURCE__) == RCC_MDF1CLKSOURCE_HSI)  || \
   ((__SOURCE__) == RCC_MDF1CLKSOURCE_PIN)  || \
   ((__SOURCE__) == RCC_MDF1CLKSOURCE_TIMG))

#define IS_RCC_PSSICLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_PSSICLKSOURCE_HCLK) || \
   ((__SOURCE__) == RCC_PSSICLKSOURCE_CLKP) || \
   ((__SOURCE__) == RCC_PSSICLKSOURCE_IC20) || \
   ((__SOURCE__) == RCC_PSSICLKSOURCE_HSI))

#define IS_RCC_SAI1CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_SAI1CLKSOURCE_PCLK2) || \
   ((__SOURCE__) == RCC_SAI1CLKSOURCE_CLKP)  || \
   ((__SOURCE__) == RCC_SAI1CLKSOURCE_IC7)   || \
   ((__SOURCE__) == RCC_SAI1CLKSOURCE_IC8)   || \
   ((__SOURCE__) == RCC_SAI1CLKSOURCE_MSI)   || \
   ((__SOURCE__) == RCC_SAI1CLKSOURCE_HSI)   || \
   ((__SOURCE__) == RCC_SAI1CLKSOURCE_PIN)   || \
   ((__SOURCE__) == RCC_SAI1CLKSOURCE_SPDIFRX1))

#define IS_RCC_SAI2CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_SAI2CLKSOURCE_PCLK2) || \
   ((__SOURCE__) == RCC_SAI2CLKSOURCE_CLKP)  || \
   ((__SOURCE__) == RCC_SAI2CLKSOURCE_IC7)   || \
   ((__SOURCE__) == RCC_SAI2CLKSOURCE_IC8)   || \
   ((__SOURCE__) == RCC_SAI2CLKSOURCE_MSI)   || \
   ((__SOURCE__) == RCC_SAI2CLKSOURCE_HSI)   || \
   ((__SOURCE__) == RCC_SAI2CLKSOURCE_PIN)   || \
   ((__SOURCE__) == RCC_SAI2CLKSOURCE_SPDIFRX1))

#define IS_RCC_SDMMC1CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_SDMMC1CLKSOURCE_HCLK) || \
   ((__SOURCE__) == RCC_SDMMC1CLKSOURCE_CLKP) || \
   ((__SOURCE__) == RCC_SDMMC1CLKSOURCE_IC4)  || \
   ((__SOURCE__) == RCC_SDMMC1CLKSOURCE_IC5))

#define IS_RCC_SDMMC2CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_SDMMC2CLKSOURCE_HCLK) || \
   ((__SOURCE__) == RCC_SDMMC2CLKSOURCE_CLKP) || \
   ((__SOURCE__) == RCC_SDMMC2CLKSOURCE_IC4)  || \
   ((__SOURCE__) == RCC_SDMMC2CLKSOURCE_IC5))

#define IS_RCC_SPDIFRX1CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_SPDIFRX1CLKSOURCE_PCLK1) || \
   ((__SOURCE__) == RCC_SPDIFRX1CLKSOURCE_CLKP)  || \
   ((__SOURCE__) == RCC_SPDIFRX1CLKSOURCE_IC7)   || \
   ((__SOURCE__) == RCC_SPDIFRX1CLKSOURCE_IC8)   || \
   ((__SOURCE__) == RCC_SPDIFRX1CLKSOURCE_MSI)   || \
   ((__SOURCE__) == RCC_SPDIFRX1CLKSOURCE_HSI)   || \
   ((__SOURCE__) == RCC_SPDIFRX1CLKSOURCE_PIN))

#define IS_RCC_SPI1CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_SPI1CLKSOURCE_PCLK2) || \
   ((__SOURCE__) == RCC_SPI1CLKSOURCE_CLKP)  || \
   ((__SOURCE__) == RCC_SPI1CLKSOURCE_IC8)   || \
   ((__SOURCE__) == RCC_SPI1CLKSOURCE_IC9)   || \
   ((__SOURCE__) == RCC_SPI1CLKSOURCE_MSI)   || \
   ((__SOURCE__) == RCC_SPI1CLKSOURCE_HSI)   || \
   ((__SOURCE__) == RCC_SPI1CLKSOURCE_PIN))

#define IS_RCC_SPI2CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_SPI2CLKSOURCE_PCLK1) || \
   ((__SOURCE__) == RCC_SPI2CLKSOURCE_CLKP)  || \
   ((__SOURCE__) == RCC_SPI2CLKSOURCE_IC8)   || \
   ((__SOURCE__) == RCC_SPI2CLKSOURCE_IC9)   || \
   ((__SOURCE__) == RCC_SPI2CLKSOURCE_MSI)   || \
   ((__SOURCE__) == RCC_SPI2CLKSOURCE_HSI)   || \
   ((__SOURCE__) == RCC_SPI2CLKSOURCE_PIN))

#define IS_RCC_SPI3CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_SPI3CLKSOURCE_PCLK1) || \
   ((__SOURCE__) == RCC_SPI3CLKSOURCE_CLKP)  || \
   ((__SOURCE__) == RCC_SPI3CLKSOURCE_IC8)   || \
   ((__SOURCE__) == RCC_SPI3CLKSOURCE_IC9)   || \
   ((__SOURCE__) == RCC_SPI3CLKSOURCE_MSI)   || \
   ((__SOURCE__) == RCC_SPI3CLKSOURCE_HSI)   || \
   ((__SOURCE__) == RCC_SPI3CLKSOURCE_PIN))

#define IS_RCC_SPI4CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_SPI4CLKSOURCE_PCLK2) || \
   ((__SOURCE__) == RCC_SPI4CLKSOURCE_CLKP)  || \
   ((__SOURCE__) == RCC_SPI4CLKSOURCE_IC9)   || \
   ((__SOURCE__) == RCC_SPI4CLKSOURCE_IC14)  || \
   ((__SOURCE__) == RCC_SPI4CLKSOURCE_MSI)   || \
   ((__SOURCE__) == RCC_SPI4CLKSOURCE_HSI)   || \
   ((__SOURCE__) == RCC_SPI4CLKSOURCE_HSE))

#define IS_RCC_SPI5CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_SPI5CLKSOURCE_PCLK2) || \
   ((__SOURCE__) == RCC_SPI5CLKSOURCE_CLKP)  || \
   ((__SOURCE__) == RCC_SPI5CLKSOURCE_IC9)   || \
   ((__SOURCE__) == RCC_SPI5CLKSOURCE_IC14)  || \
   ((__SOURCE__) == RCC_SPI5CLKSOURCE_MSI)   || \
   ((__SOURCE__) == RCC_SPI5CLKSOURCE_HSI)   || \
   ((__SOURCE__) == RCC_SPI5CLKSOURCE_HSE))

#define IS_RCC_SPI6CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_SPI6CLKSOURCE_PCLK4) || \
   ((__SOURCE__) == RCC_SPI6CLKSOURCE_CLKP)  || \
   ((__SOURCE__) == RCC_SPI6CLKSOURCE_IC8)   || \
   ((__SOURCE__) == RCC_SPI6CLKSOURCE_IC9)   || \
   ((__SOURCE__) == RCC_SPI6CLKSOURCE_MSI)   || \
   ((__SOURCE__) == RCC_SPI6CLKSOURCE_HSI)   || \
   ((__SOURCE__) == RCC_SPI6CLKSOURCE_PIN))

#define IS_RCC_TIMPRES(__VALUE__)  \
  (((__VALUE__) == RCC_TIMPRES_DIV1) || \
   ((__VALUE__) == RCC_TIMPRES_DIV2) || \
   ((__VALUE__) == RCC_TIMPRES_DIV4) || \
   ((__VALUE__) == RCC_TIMPRES_DIV8))

#define IS_RCC_USART1CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_USART1CLKSOURCE_PCLK2) || \
   ((__SOURCE__) == RCC_USART1CLKSOURCE_CLKP)  || \
   ((__SOURCE__) == RCC_USART1CLKSOURCE_IC9)   || \
   ((__SOURCE__) == RCC_USART1CLKSOURCE_IC14)  || \
   ((__SOURCE__) == RCC_USART1CLKSOURCE_LSE)   || \
   ((__SOURCE__) == RCC_USART1CLKSOURCE_MSI)   || \
   ((__SOURCE__) == RCC_USART1CLKSOURCE_HSI))

#define IS_RCC_USART2CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_USART2CLKSOURCE_PCLK1) || \
   ((__SOURCE__) == RCC_USART2CLKSOURCE_CLKP)  || \
   ((__SOURCE__) == RCC_USART2CLKSOURCE_IC9)   || \
   ((__SOURCE__) == RCC_USART2CLKSOURCE_IC14)  || \
   ((__SOURCE__) == RCC_USART2CLKSOURCE_LSE)   || \
   ((__SOURCE__) == RCC_USART2CLKSOURCE_MSI)   || \
   ((__SOURCE__) == RCC_USART2CLKSOURCE_HSI))

#define IS_RCC_USART3CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_USART3CLKSOURCE_PCLK1) || \
   ((__SOURCE__) == RCC_USART3CLKSOURCE_CLKP)  || \
   ((__SOURCE__) == RCC_USART3CLKSOURCE_IC9)   || \
   ((__SOURCE__) == RCC_USART3CLKSOURCE_IC14)  || \
   ((__SOURCE__) == RCC_USART3CLKSOURCE_LSE)   || \
   ((__SOURCE__) == RCC_USART3CLKSOURCE_MSI)   || \
   ((__SOURCE__) == RCC_USART3CLKSOURCE_HSI))

#define IS_RCC_UART4CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_UART4CLKSOURCE_PCLK1) || \
   ((__SOURCE__) == RCC_UART4CLKSOURCE_CLKP)  || \
   ((__SOURCE__) == RCC_UART4CLKSOURCE_IC9)   || \
   ((__SOURCE__) == RCC_UART4CLKSOURCE_IC14)  || \
   ((__SOURCE__) == RCC_UART4CLKSOURCE_LSE)   || \
   ((__SOURCE__) == RCC_UART4CLKSOURCE_MSI)   || \
   ((__SOURCE__) == RCC_UART4CLKSOURCE_HSI))

#define IS_RCC_UART5CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_UART5CLKSOURCE_PCLK1) || \
   ((__SOURCE__) == RCC_UART5CLKSOURCE_CLKP)  || \
   ((__SOURCE__) == RCC_UART5CLKSOURCE_IC9)   || \
   ((__SOURCE__) == RCC_UART5CLKSOURCE_IC14)  || \
   ((__SOURCE__) == RCC_UART5CLKSOURCE_LSE)   || \
   ((__SOURCE__) == RCC_UART5CLKSOURCE_MSI)   || \
   ((__SOURCE__) == RCC_UART5CLKSOURCE_HSI))

#define IS_RCC_USART6CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_USART6CLKSOURCE_PCLK2) || \
   ((__SOURCE__) == RCC_USART6CLKSOURCE_CLKP)  || \
   ((__SOURCE__) == RCC_USART6CLKSOURCE_IC9)   || \
   ((__SOURCE__) == RCC_USART6CLKSOURCE_IC14)  || \
   ((__SOURCE__) == RCC_USART6CLKSOURCE_LSE)   || \
   ((__SOURCE__) == RCC_USART6CLKSOURCE_MSI)   || \
   ((__SOURCE__) == RCC_USART6CLKSOURCE_HSI))

#define IS_RCC_UART7CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_UART7CLKSOURCE_PCLK1) || \
   ((__SOURCE__) == RCC_UART7CLKSOURCE_CLKP)  || \
   ((__SOURCE__) == RCC_UART7CLKSOURCE_IC9)   || \
   ((__SOURCE__) == RCC_UART7CLKSOURCE_IC14)  || \
   ((__SOURCE__) == RCC_UART7CLKSOURCE_LSE)   || \
   ((__SOURCE__) == RCC_UART7CLKSOURCE_MSI)   || \
   ((__SOURCE__) == RCC_UART7CLKSOURCE_HSI))

#define IS_RCC_UART8CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_UART8CLKSOURCE_PCLK1) || \
   ((__SOURCE__) == RCC_UART8CLKSOURCE_CLKP)  || \
   ((__SOURCE__) == RCC_UART8CLKSOURCE_IC9)   || \
   ((__SOURCE__) == RCC_UART8CLKSOURCE_IC14)  || \
   ((__SOURCE__) == RCC_UART8CLKSOURCE_LSE)   || \
   ((__SOURCE__) == RCC_UART8CLKSOURCE_MSI)   || \
   ((__SOURCE__) == RCC_UART8CLKSOURCE_HSI))

#define IS_RCC_UART9CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_UART9CLKSOURCE_PCLK2) || \
   ((__SOURCE__) == RCC_UART9CLKSOURCE_CLKP)  || \
   ((__SOURCE__) == RCC_UART9CLKSOURCE_IC9)   || \
   ((__SOURCE__) == RCC_UART9CLKSOURCE_IC14)  || \
   ((__SOURCE__) == RCC_UART9CLKSOURCE_LSE)   || \
   ((__SOURCE__) == RCC_UART9CLKSOURCE_MSI)   || \
   ((__SOURCE__) == RCC_UART9CLKSOURCE_HSI))

#define IS_RCC_USART10CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_USART10CLKSOURCE_PCLK2) || \
   ((__SOURCE__) == RCC_USART10CLKSOURCE_CLKP)  || \
   ((__SOURCE__) == RCC_USART10CLKSOURCE_IC9)   || \
   ((__SOURCE__) == RCC_USART10CLKSOURCE_IC14)  || \
   ((__SOURCE__) == RCC_USART10CLKSOURCE_LSE)   || \
   ((__SOURCE__) == RCC_USART10CLKSOURCE_MSI)   || \
   ((__SOURCE__) == RCC_USART10CLKSOURCE_HSI))

#define IS_RCC_USBPHY1CLKSOURCE(__SOURCE__)                  \
  (((__SOURCE__) == RCC_USBPHY1REFCLKSOURCE_OTGPHY1)      || \
   ((__SOURCE__) == RCC_USBPHY1REFCLKSOURCE_HSE_DIRECT)   || \
   ((__SOURCE__) == RCC_USBPHY1REFCLKSOURCE_HSE_DIRECT_DIV2))

#define IS_RCC_USBPHY2CLKSOURCE(__SOURCE__)                  \
  (((__SOURCE__) == RCC_USBPHY2REFCLKSOURCE_OTGPHY2)      || \
   ((__SOURCE__) == RCC_USBPHY2REFCLKSOURCE_HSE_DIRECT)   || \
   ((__SOURCE__) == RCC_USBPHY2REFCLKSOURCE_HSE_DIRECT_DIV2))

#define IS_RCC_USBOTGHS1CLKSOURCE(__SOURCE__)                \
  (((__SOURCE__) == RCC_USBOTGHS1CLKSOURCE_HSE_DIV2)      || \
   ((__SOURCE__) == RCC_USBOTGHS1CLKSOURCE_CLKP)          || \
   ((__SOURCE__) == RCC_USBOTGHS1CLKSOURCE_IC15)          || \
   ((__SOURCE__) == RCC_USBOTGHS1CLKSOURCE_HSE_DIRECT)    || \
   ((__SOURCE__) == RCC_USBOTGHS1CLKSOURCE_HSE_DIRECT_DIV2))

#define IS_RCC_USBOTGHS2CLKSOURCE(__SOURCE__)                \
  (((__SOURCE__) == RCC_USBOTGHS2CLKSOURCE_HSE_DIV2)      || \
   ((__SOURCE__) == RCC_USBOTGHS2CLKSOURCE_CLKP)          || \
   ((__SOURCE__) == RCC_USBOTGHS2CLKSOURCE_IC15)          || \
   ((__SOURCE__) == RCC_USBOTGHS2CLKSOURCE_HSE_DIRECT)    || \
   ((__SOURCE__) == RCC_USBOTGHS2CLKSOURCE_HSE_DIRECT_DIV2))

#define IS_RCC_XSPI1CLKSOURCE(__SOURCE__)       \
  (((__SOURCE__) == RCC_XSPI1CLKSOURCE_HCLK) || \
   ((__SOURCE__) == RCC_XSPI1CLKSOURCE_CLKP) || \
   ((__SOURCE__) == RCC_XSPI1CLKSOURCE_IC3)  || \
   ((__SOURCE__) == RCC_XSPI1CLKSOURCE_IC4))

#define IS_RCC_XSPI2CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_XSPI2CLKSOURCE_HCLK) || \
   ((__SOURCE__) == RCC_XSPI2CLKSOURCE_CLKP) || \
   ((__SOURCE__) == RCC_XSPI2CLKSOURCE_IC3)  || \
   ((__SOURCE__) == RCC_XSPI2CLKSOURCE_IC4))

#define IS_RCC_XSPI3CLKSOURCE(__SOURCE__) \
  (((__SOURCE__) == RCC_XSPI3CLKSOURCE_HCLK) || \
   ((__SOURCE__) == RCC_XSPI3CLKSOURCE_CLKP) || \
   ((__SOURCE__) == RCC_XSPI3CLKSOURCE_IC3)  || \
   ((__SOURCE__) == RCC_XSPI3CLKSOURCE_IC4))

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

#endif /* STM32N6xx_HAL_RCC_EX_H */
