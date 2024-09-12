/**
  ******************************************************************************
  * @file    stm32n6xx_ll_bus.h
  * @author  MCD Application Team
  * @brief   Header file of BUS LL module.
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
  @verbatim
                      ##### RCC Limitations #####
  ==============================================================================
    [..]
      A delay between an RCC peripheral clock enable and the effective peripheral
      enabling should be taken into account in order to manage the peripheral read/write
      from/to registers.
      (+) This delay depends on the peripheral mapping.
        (++) AHB & APB peripherals, 1 dummy read is necessary

    [..]
      Workarounds:
      (#) For AHB & APB peripherals, a dummy read to the peripheral register has been
          inserted in each LL_{BUS}_GRP{x}_EnableClock() function.

  @endverbatim
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32N6xx_LL_BUS_H
#define STM32N6xx_LL_BUS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx.h"

/** @addtogroup STM32N6xx_LL_Driver
  * @{
  */
#if defined(RCC)

/** @defgroup BUS_LL BUS
  * @{
  */

/* Private variables ---------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/** @defgroup BUS_LL_Exported_Constants BUS Exported Constants
  * @{
  */

/** @defgroup BUS_LL_EC_AXI  AXI
  * @{
  */
#define LL_ACLKN                           RCC_BUSENR_ACLKNEN
#define LL_ACLKNC                          RCC_BUSENR_ACLKNCEN
#define LL_AHBM                            RCC_BUSENR_AHBMEN
#define LL_AHB1                            RCC_BUSENR_AHB1EN
#define LL_AHB2                            RCC_BUSENR_AHB2EN
#define LL_AHB3                            RCC_BUSENR_AHB3EN
#define LL_AHB4                            RCC_BUSENR_AHB4EN
#define LL_AHB5                            RCC_BUSENR_AHB5EN
#define LL_APB1                            RCC_BUSENR_APB1EN
#define LL_APB2                            RCC_BUSENR_APB2EN
#define LL_APB3                            RCC_BUSENR_APB3EN
#define LL_APB4                            RCC_BUSENR_APB4EN
#define LL_APB5                            RCC_BUSENR_APB5EN
/**
  * @}
  */

/** @defgroup BUS_LL_EC_MEM  MEM
  * @{
  */
#define LL_MEM_AXISRAM1                    RCC_MEMENR_AXISRAM1EN
#define LL_MEM_AXISRAM2                    RCC_MEMENR_AXISRAM2EN
#define LL_MEM_AXISRAM3                    RCC_MEMENR_AXISRAM3EN
#define LL_MEM_AXISRAM4                    RCC_MEMENR_AXISRAM4EN
#define LL_MEM_AXISRAM5                    RCC_MEMENR_AXISRAM5EN
#define LL_MEM_AXISRAM6                    RCC_MEMENR_AXISRAM6EN
#define LL_MEM_AHBSRAM1                    RCC_MEMENR_AHBSRAM1EN
#define LL_MEM_AHBSRAM2                    RCC_MEMENR_AHBSRAM2EN
#define LL_MEM_BKPSRAM                     RCC_MEMENR_BKPSRAMEN
#define LL_MEM_FLEXRAM                     RCC_MEMENR_FLEXRAMEN
#define LL_MEM_CACHEAXIRAM                 RCC_MEMENR_CACHEAXIRAMEN
#define LL_MEM_VENCRAM                     RCC_MEMENR_VENCRAMEN
#define LL_MEM_BOOTROM                     RCC_MEMENR_BOOTROMEN
/**
  * @}
  */

/** @defgroup BUS_LL_EC_AHB1_GRP1_PERIPH  AHB1 GRP1 PERIPH
  * @{
  */
#define LL_AHB1_GRP1_PERIPH_ADC12          RCC_AHB1ENR_ADC12EN
#define LL_AHB1_GRP1_PERIPH_GPDMA1         RCC_AHB1ENR_GPDMA1EN
#define LL_AHB1_GRP1_PERIPH_ALL            (LL_AHB1_GRP1_PERIPH_ADC12 | LL_AHB1_GRP1_PERIPH_GPDMA1)
/**
  * @}
  */

/** @defgroup BUS_LL_EC_AHB2_GRP1_PERIPH  AHB2 GRP1 PERIPH
  * @{
  */
#define LL_AHB2_GRP1_PERIPH_ADF1           RCC_AHB2ENR_ADF1EN
#define LL_AHB2_GRP1_PERIPH_MDF1           RCC_AHB2ENR_MDF1EN
#define LL_AHB2_GRP1_PERIPH_RAMCFG         RCC_AHB2ENR_RAMCFGEN
#define LL_AHB2_GRP1_PERIPH_ALL            (LL_AHB2_GRP1_PERIPH_ADF1 | LL_AHB2_GRP1_PERIPH_MDF1 | \
                                            LL_AHB2_GRP1_PERIPH_RAMCFG)
/**
  * @}
  */

/** @defgroup BUS_LL_EC_AHB3_GRP1_PERIPH  AHB3 GRP1 PERIPH
  * @{
  */
#if defined(CRYP)
#define LL_AHB3_GRP1_PERIPH_CRYP           RCC_AHB3ENR_CRYPEN
#else
#define LL_AHB3_GRP1_PERIPH_CRYP
#endif /* CRYP */
#define LL_AHB3_GRP1_PERIPH_HASH           RCC_AHB3ENR_HASHEN
#define LL_AHB3_GRP1_PERIPH_IAC            RCC_AHB3ENR_IACEN
#if defined(PKA)
#define LL_AHB3_GRP1_PERIPH_PKA            RCC_AHB3ENR_PKAEN
#else
#define LL_AHB3_GRP1_PERIPH_PKA
#endif /* PKA */
#define LL_AHB3_GRP1_PERIPH_RIFSC          RCC_AHB3ENR_RIFSCEN
#define LL_AHB3_GRP1_PERIPH_RISAF          RCC_AHB3ENR_RISAFEN
#define LL_AHB3_GRP1_PERIPH_RNG            RCC_AHB3ENR_RNGEN
#if defined(SAES)
#define LL_AHB3_GRP1_PERIPH_SAES           RCC_AHB3ENR_SAESEN
#else
#define LL_AHB3_GRP1_PERIPH_SAES
#endif /* SAES */
#define LL_AHB3_GRP1_PERIPH_ALL            (LL_AHB3_GRP1_PERIPH_CRYP  | LL_AHB3_GRP1_PERIPH_HASH  | \
                                            LL_AHB3_GRP1_PERIPH_IAC   | LL_AHB3_GRP1_PERIPH_PKA   | \
                                            LL_AHB3_GRP1_PERIPH_RIFSC | LL_AHB3_GRP1_PERIPH_RISAF | \
                                            LL_AHB3_GRP1_PERIPH_RNG   | LL_AHB3_GRP1_PERIPH_SAES)
/**
  * @}
  */

/** @defgroup BUS_LL_EC_AHB4_GRP1_PERIPH  AHB4 GRP1 PERIPH
  * @{
  */
#define LL_AHB4_GRP1_PERIPH_CRC            RCC_AHB4ENR_CRCEN
#define LL_AHB4_GRP1_PERIPH_GPIOA          RCC_AHB4ENR_GPIOAEN
#define LL_AHB4_GRP1_PERIPH_GPIOB          RCC_AHB4ENR_GPIOBEN
#define LL_AHB4_GRP1_PERIPH_GPIOC          RCC_AHB4ENR_GPIOCEN
#define LL_AHB4_GRP1_PERIPH_GPIOD          RCC_AHB4ENR_GPIODEN
#define LL_AHB4_GRP1_PERIPH_GPIOE          RCC_AHB4ENR_GPIOEEN
#define LL_AHB4_GRP1_PERIPH_GPIOF          RCC_AHB4ENR_GPIOFEN
#define LL_AHB4_GRP1_PERIPH_GPIOG          RCC_AHB4ENR_GPIOGEN
#define LL_AHB4_GRP1_PERIPH_GPIOH          RCC_AHB4ENR_GPIOHEN
#define LL_AHB4_GRP1_PERIPH_GPION          RCC_AHB4ENR_GPIONEN
#define LL_AHB4_GRP1_PERIPH_GPIOO          RCC_AHB4ENR_GPIOOEN
#define LL_AHB4_GRP1_PERIPH_GPIOP          RCC_AHB4ENR_GPIOPEN
#define LL_AHB4_GRP1_PERIPH_GPIOQ          RCC_AHB4ENR_GPIOQEN
#define LL_AHB4_GRP1_PERIPH_PWR            RCC_AHB4ENR_PWREN
#define LL_AHB4_GRP1_PERIPH_ALL            (LL_AHB4_GRP1_PERIPH_CRC   | LL_AHB4_GRP1_PERIPH_GPIOA | \
                                            LL_AHB4_GRP1_PERIPH_GPIOB | LL_AHB4_GRP1_PERIPH_GPIOC | \
                                            LL_AHB4_GRP1_PERIPH_GPIOD | LL_AHB4_GRP1_PERIPH_GPIOE | \
                                            LL_AHB4_GRP1_PERIPH_GPIOF | LL_AHB4_GRP1_PERIPH_GPIOG | \
                                            LL_AHB4_GRP1_PERIPH_GPIOH | LL_AHB4_GRP1_PERIPH_GPION | \
                                            LL_AHB4_GRP1_PERIPH_GPIOO | LL_AHB4_GRP1_PERIPH_GPIOP | \
                                            LL_AHB4_GRP1_PERIPH_GPIOQ | LL_AHB4_GRP1_PERIPH_PWR)
/**
  * @}
  */

/** @defgroup BUS_LL_EC_AHB5_GRP1_PERIPH  AHB5 GRP1 PERIPH
  * @{
  */
#define LL_AHB5_GRP1_PERIPH_DMA2D             RCC_AHB5ENR_DMA2DEN
#define LL_AHB5_GRP1_PERIPH_ETH1              RCC_AHB5ENR_ETH1EN
#define LL_AHB5_GRP1_PERIPH_ETH1MAC           RCC_AHB5ENR_ETH1MACEN
#define LL_AHB5_GRP1_PERIPH_ETH1TX            RCC_AHB5ENR_ETH1TXEN
#define LL_AHB5_GRP1_PERIPH_ETH1RX            RCC_AHB5ENR_ETH1RXEN
#define LL_AHB5_GRP1_PERIPH_FMC               RCC_AHB5ENR_FMCEN
#define LL_AHB5_GRP1_PERIPH_GFXMMU            RCC_AHB5ENR_GFXMMUEN
#define LL_AHB5_GRP1_PERIPH_GPU2D             RCC_AHB5ENR_GPU2DEN
#define LL_AHB5_GRP1_PERIPH_HPDMA1            RCC_AHB5ENR_HPDMA1EN
#define LL_AHB5_GRP1_PERIPH_XSPI1             RCC_AHB5ENR_XSPI1EN
#define LL_AHB5_GRP1_PERIPH_XSPI2             RCC_AHB5ENR_XSPI2EN
#define LL_AHB5_GRP1_PERIPH_XSPI3             RCC_AHB5ENR_XSPI3EN
#define LL_AHB5_GRP1_PERIPH_XSPIM             RCC_AHB5ENR_XSPIMEN
#define LL_AHB5_GRP1_PERIPH_JPEG              RCC_AHB5ENR_JPEGEN
#define LL_AHB5_GRP1_PERIPH_MCE1              RCC_AHB5ENR_MCE1EN
#define LL_AHB5_GRP1_PERIPH_MCE2              RCC_AHB5ENR_MCE2EN
#define LL_AHB5_GRP1_PERIPH_MCE3              RCC_AHB5ENR_MCE3EN
#define LL_AHB5_GRP1_PERIPH_MCE4              RCC_AHB5ENR_MCE4EN
#define LL_AHB5_GRP1_PERIPH_CACHEAXI          RCC_AHB5ENR_CACHEAXIEN
#define LL_AHB5_GRP1_PERIPH_NPU               RCC_AHB5ENR_NPUEN
#define LL_AHB5_GRP1_PERIPH_OTG1              RCC_AHB5ENR_OTG1EN
#define LL_AHB5_GRP1_PERIPH_OTG2              RCC_AHB5ENR_OTG2EN
#define LL_AHB5_GRP1_PERIPH_OTGPHY1           RCC_AHB5ENR_OTGPHY1EN
#define LL_AHB5_GRP1_PERIPH_OTGPHY2           RCC_AHB5ENR_OTGPHY2EN
#define LL_AHB5_GRP1_PERIPH_OTG1PHYCTL        RCC_AHB5RSTR_OTG1PHYCTLRST
#define LL_AHB5_GRP1_PERIPH_OTG2PHYCTL        RCC_AHB5RSTR_OTG2PHYCTLRST
#define LL_AHB5_GRP1_PERIPH_PSSI              RCC_AHB5ENR_PSSIEN
#define LL_AHB5_GRP1_PERIPH_SDMMC1            RCC_AHB5ENR_SDMMC1EN
#define LL_AHB5_GRP1_PERIPH_SDMMC2            RCC_AHB5ENR_SDMMC2EN
#define LL_AHB5_GRP1_PERIPH_ALL            (LL_AHB5_GRP1_PERIPH_DMA2D    | LL_AHB5_GRP1_PERIPH_ETH1    | \
                                            LL_AHB5_GRP1_PERIPH_ETH1MAC  | LL_AHB5_GRP1_PERIPH_ETH1TX  | \
                                            LL_AHB5_GRP1_PERIPH_ETH1RX   | LL_AHB5_GRP1_PERIPH_FMC     | \
                                            LL_AHB5_GRP1_PERIPH_GFXMMU   | LL_AHB5_GRP1_PERIPH_GPU2D   | \
                                            LL_AHB5_GRP1_PERIPH_HPDMA1   | LL_AHB5_GRP1_PERIPH_XSPI1   | \
                                            LL_AHB5_GRP1_PERIPH_XSPI2    | LL_AHB5_GRP1_PERIPH_XSPI3   | \
                                            LL_AHB5_GRP1_PERIPH_XSPIM    | LL_AHB5_GRP1_PERIPH_JPEG    | \
                                            LL_AHB5_GRP1_PERIPH_MCE1     | LL_AHB5_GRP1_PERIPH_MCE2    | \
                                            LL_AHB5_GRP1_PERIPH_MCE3     | LL_AHB5_GRP1_PERIPH_MCE4    | \
                                            LL_AHB5_GRP1_PERIPH_CACHEAXI | LL_AHB5_GRP1_PERIPH_NPU     | \
                                            LL_AHB5_GRP1_PERIPH_OTG1     | LL_AHB5_GRP1_PERIPH_OTG2    | \
                                            LL_AHB5_GRP1_PERIPH_OTGPHY1  | LL_AHB5_GRP1_PERIPH_OTGPHY2 | \
                                            LL_AHB5_GRP1_PERIPH_PSSI     | LL_AHB5_GRP1_PERIPH_SDMMC1  | \
                                            LL_AHB5_GRP1_PERIPH_SDMMC2)
/**
  * @}
  */

/** @defgroup BUS_LL_EC_APB1_GRP1_PERIPH  APB1 GRP1 PERIPH
  * @{
  */
#define LL_APB1_GRP1_PERIPH_I2C1           RCC_APB1ENR1_I2C1EN
#define LL_APB1_GRP1_PERIPH_I2C2           RCC_APB1ENR1_I2C2EN
#define LL_APB1_GRP1_PERIPH_I2C3           RCC_APB1ENR1_I2C3EN
#define LL_APB1_GRP1_PERIPH_I3C1           RCC_APB1ENR1_I3C1EN
#define LL_APB1_GRP1_PERIPH_I3C2           RCC_APB1ENR1_I3C2EN
#define LL_APB1_GRP1_PERIPH_LPTIM1         RCC_APB1ENR1_LPTIM1EN
#define LL_APB1_GRP1_PERIPH_SPDIFRX1       RCC_APB1ENR1_SPDIFRX1EN
#define LL_APB1_GRP1_PERIPH_SPI2           RCC_APB1ENR1_SPI2EN
#define LL_APB1_GRP1_PERIPH_SPI3           RCC_APB1ENR1_SPI3EN
#define LL_APB1_GRP1_PERIPH_TIM2           RCC_APB1ENR1_TIM2EN
#define LL_APB1_GRP1_PERIPH_TIM3           RCC_APB1ENR1_TIM3EN
#define LL_APB1_GRP1_PERIPH_TIM4           RCC_APB1ENR1_TIM4EN
#define LL_APB1_GRP1_PERIPH_TIM5           RCC_APB1ENR1_TIM5EN
#define LL_APB1_GRP1_PERIPH_TIM6           RCC_APB1ENR1_TIM6EN
#define LL_APB1_GRP1_PERIPH_TIM7           RCC_APB1ENR1_TIM7EN
#define LL_APB1_GRP1_PERIPH_TIM10          RCC_APB1ENR1_TIM10EN
#define LL_APB1_GRP1_PERIPH_TIM11          RCC_APB1ENR1_TIM11EN
#define LL_APB1_GRP1_PERIPH_TIM12          RCC_APB1ENR1_TIM12EN
#define LL_APB1_GRP1_PERIPH_TIM13          RCC_APB1ENR1_TIM13EN
#define LL_APB1_GRP1_PERIPH_TIM14          RCC_APB1ENR1_TIM14EN
#define LL_APB1_GRP1_PERIPH_USART2         RCC_APB1ENR1_USART2EN
#define LL_APB1_GRP1_PERIPH_USART3         RCC_APB1ENR1_USART3EN
#define LL_APB1_GRP1_PERIPH_UART4          RCC_APB1ENR1_UART4EN
#define LL_APB1_GRP1_PERIPH_UART5          RCC_APB1ENR1_UART5EN
#define LL_APB1_GRP1_PERIPH_UART7          RCC_APB1ENR1_UART7EN
#define LL_APB1_GRP1_PERIPH_UART8          RCC_APB1ENR1_UART8EN
#define LL_APB1_GRP1_PERIPH_WWDG           RCC_APB1ENR1_WWDGEN
#define LL_APB1_GRP1_PERIPH_ALL            (LL_APB1_GRP1_PERIPH_I2C1     | LL_APB1_GRP1_PERIPH_I2C2   | \
                                            LL_APB1_GRP1_PERIPH_I2C3     | LL_APB1_GRP1_PERIPH_I3C1   | \
                                            LL_APB1_GRP1_PERIPH_I3C2     | LL_APB1_GRP1_PERIPH_LPTIM1 | \
                                            LL_APB1_GRP1_PERIPH_SPDIFRX1 | LL_APB1_GRP1_PERIPH_SPI2   | \
                                            LL_APB1_GRP1_PERIPH_SPI3     | LL_APB1_GRP1_PERIPH_TIM2   | \
                                            LL_APB1_GRP1_PERIPH_TIM3     | LL_APB1_GRP1_PERIPH_TIM4   | \
                                            LL_APB1_GRP1_PERIPH_TIM5     | LL_APB1_GRP1_PERIPH_TIM6   | \
                                            LL_APB1_GRP1_PERIPH_TIM7     | LL_APB1_GRP1_PERIPH_TIM10  | \
                                            LL_APB1_GRP1_PERIPH_TIM11    | LL_APB1_GRP1_PERIPH_TIM12  | \
                                            LL_APB1_GRP1_PERIPH_TIM13    | LL_APB1_GRP1_PERIPH_TIM14  | \
                                            LL_APB1_GRP1_PERIPH_USART2   | LL_APB1_GRP1_PERIPH_USART3 | \
                                            LL_APB1_GRP1_PERIPH_UART4    | LL_APB1_GRP1_PERIPH_UART5  | \
                                            LL_APB1_GRP1_PERIPH_UART7    | LL_APB1_GRP1_PERIPH_UART8  | \
                                            LL_APB1_GRP1_PERIPH_WWDG)
/**
  * @}
  */

/** @defgroup BUS_LL_EC_APB1_GRP2_PERIPH  APB1 GRP2 PERIPH
  * @{
  */
#define LL_APB1_GRP2_PERIPH_FDCAN          RCC_APB1ENR2_FDCANEN
#define LL_APB1_GRP2_PERIPH_MDIOS          RCC_APB1ENR2_MDIOSEN
#define LL_APB1_GRP2_PERIPH_UCPD1          RCC_APB1ENR2_UCPD1EN
#define LL_APB1_GRP2_PERIPH_ALL            (LL_APB1_GRP2_PERIPH_FDCAN | LL_APB1_GRP2_PERIPH_MDIOS | \
                                            LL_APB1_GRP2_PERIPH_UCPD1)
/**
  * @}
  */

/** @defgroup BUS_LL_EC_APB2_GRP1_PERIPH  APB2 GRP1 PERIPH
  * @{
  */
#define LL_APB2_GRP1_PERIPH_SAI1           RCC_APB2ENR_SAI1EN
#define LL_APB2_GRP1_PERIPH_SAI2           RCC_APB2ENR_SAI2EN
#define LL_APB2_GRP1_PERIPH_SPI1           RCC_APB2ENR_SPI1EN
#define LL_APB2_GRP1_PERIPH_SPI4           RCC_APB2ENR_SPI4EN
#define LL_APB2_GRP1_PERIPH_SPI5           RCC_APB2ENR_SPI5EN
#define LL_APB2_GRP1_PERIPH_TIM1           RCC_APB2ENR_TIM1EN
#define LL_APB2_GRP1_PERIPH_TIM8           RCC_APB2ENR_TIM8EN
#define LL_APB2_GRP1_PERIPH_TIM9           RCC_APB2ENR_TIM9EN
#define LL_APB2_GRP1_PERIPH_TIM15          RCC_APB2ENR_TIM15EN
#define LL_APB2_GRP1_PERIPH_TIM16          RCC_APB2ENR_TIM16EN
#define LL_APB2_GRP1_PERIPH_TIM17          RCC_APB2ENR_TIM17EN
#define LL_APB2_GRP1_PERIPH_TIM18          RCC_APB2ENR_TIM18EN
#define LL_APB2_GRP1_PERIPH_USART1         RCC_APB2ENR_USART1EN
#define LL_APB2_GRP1_PERIPH_USART6         RCC_APB2ENR_USART6EN
#define LL_APB2_GRP1_PERIPH_UART9          RCC_APB2ENR_UART9EN
#define LL_APB2_GRP1_PERIPH_USART10        RCC_APB2ENR_USART10EN
#define LL_APB2_GRP1_PERIPH_ALL            (LL_APB2_GRP1_PERIPH_SAI1   | LL_APB2_GRP1_PERIPH_SAI2   | \
                                            LL_APB2_GRP1_PERIPH_SPI1   | LL_APB2_GRP1_PERIPH_SPI4   | \
                                            LL_APB2_GRP1_PERIPH_SPI5   | LL_APB2_GRP1_PERIPH_TIM1   | \
                                            LL_APB2_GRP1_PERIPH_TIM8   | LL_APB2_GRP1_PERIPH_TIM9   | \
                                            LL_APB2_GRP1_PERIPH_TIM15  | LL_APB2_GRP1_PERIPH_TIM16  | \
                                            LL_APB2_GRP1_PERIPH_TIM17  | LL_APB2_GRP1_PERIPH_TIM18  | \
                                            LL_APB2_GRP1_PERIPH_USART1 | LL_APB2_GRP1_PERIPH_USART6 | \
                                            LL_APB2_GRP1_PERIPH_UART9  | LL_APB2_GRP1_PERIPH_USART10)
/**
  * @}
  */

/** @defgroup BUS_LL_EC_APB4_GRP1_PERIPH  APB4 GRP1 PERIPH
  * @{
  */
#define LL_APB4_GRP1_PERIPH_HDP            RCC_APB4ENR1_HDPEN
#define LL_APB4_GRP1_PERIPH_I2C4           RCC_APB4ENR1_I2C4EN
#define LL_APB4_GRP1_PERIPH_LPTIM2         RCC_APB4ENR1_LPTIM2EN
#define LL_APB4_GRP1_PERIPH_LPTIM3         RCC_APB4ENR1_LPTIM3EN
#define LL_APB4_GRP1_PERIPH_LPTIM4         RCC_APB4ENR1_LPTIM4EN
#define LL_APB4_GRP1_PERIPH_LPTIM5         RCC_APB4ENR1_LPTIM5EN
#define LL_APB4_GRP1_PERIPH_LPUART1        RCC_APB4ENR1_LPUART1EN
#define LL_APB4_GRP1_PERIPH_RTC            RCC_APB4ENR1_RTCEN
#define LL_APB4_GRP1_PERIPH_RTCAPB         RCC_APB4ENR1_RTCAPBEN
#define LL_APB4_GRP1_PERIPH_SPI6           RCC_APB4ENR1_SPI6EN
#define LL_APB4_GRP1_PERIPH_VREFBUF        RCC_APB4ENR1_VREFBUFEN
#define LL_APB4_GRP1_PERIPH_ALL            (LL_APB4_GRP1_PERIPH_HDP     | LL_APB4_GRP1_PERIPH_I2C4    | \
                                            LL_APB4_GRP1_PERIPH_LPTIM2  | LL_APB4_GRP1_PERIPH_LPTIM3  | \
                                            LL_APB4_GRP1_PERIPH_LPTIM4  | LL_APB4_GRP1_PERIPH_LPTIM5  | \
                                            LL_APB4_GRP1_PERIPH_LPUART1 | LL_APB4_GRP1_PERIPH_RTC     | \
                                            LL_APB4_GRP1_PERIPH_RTCAPB  | LL_APB4_GRP1_PERIPH_SPI6    | \
                                            LL_APB4_GRP1_PERIPH_VREFBUF)
/**
  * @}
  */

/** @defgroup BUS_LL_EC_APB4_GRP2_PERIPH  APB4 GRP2 PERIPH
  * @{
  */
#define LL_APB4_GRP2_PERIPH_BSEC           RCC_APB4ENR2_BSECEN
#define LL_APB4_GRP2_PERIPH_DTS            RCC_APB4ENR2_DTSEN
#define LL_APB4_GRP2_PERIPH_SYSCFG         RCC_APB4ENR2_SYSCFGEN
#define LL_APB4_GRP2_PERIPH_ALL            (LL_APB4_GRP2_PERIPH_BSEC | LL_APB4_GRP2_PERIPH_DTS | \
                                            LL_APB4_GRP2_PERIPH_SYSCFG)
/**
  * @}
  */

/** @defgroup BUS_LL_EC_APB5_GRP1_PERIPH  APB5 GRP1 PERIPH
  * @{
  */
#define LL_APB5_GRP1_PERIPH_CSI            RCC_APB5ENR_CSIEN
#define LL_APB5_GRP1_PERIPH_DCMIPP         RCC_APB5ENR_DCMIPPEN
#define LL_APB5_GRP1_PERIPH_GFXTIM         RCC_APB5ENR_GFXTIMEN
#define LL_APB5_GRP1_PERIPH_LTDC           RCC_APB5ENR_LTDCEN
#if defined(VENC)
#define LL_APB5_GRP1_PERIPH_VENC           RCC_APB5ENR_VENCEN
#else
#define LL_APB5_GRP1_PERIPH_VENC
#endif /* VENC */
#define LL_APB5_GRP1_PERIPH_ALL            (LL_APB5_GRP1_PERIPH_CSI    | LL_APB5_GRP1_PERIPH_DCMIPP | \
                                            LL_APB5_GRP1_PERIPH_GFXTIM | LL_APB5_GRP1_PERIPH_LTDC   | \
                                            LL_APB5_GRP1_PERIPH_VENC)
/**
  * @}
  */

/** @defgroup BUS_LL_EC_MISC  MISC
  * @{
  */
#define LL_DBG                             RCC_MISCENR_DBGEN
#define LL_MCO1                            RCC_MISCENR_MCO1EN
#define LL_MCO2                            RCC_MISCENR_MCO2EN
#define LL_XSPIPHYCOMP                     RCC_MISCENR_XSPIPHYCOMPEN
#define LL_XSPIPHY1                        RCC_MISCRSTR_XSPIPHY1RST
#define LL_XSPIPHY2                        RCC_MISCRSTR_XSPIPHY2RST
#define LL_PER                             RCC_MISCENR_PEREN
#define LL_SDMMC1DLL                       RCC_MISCRSTR_SDMMC1DLLRST
#define LL_SDMMC2DLL                       RCC_MISCRSTR_SDMMC2DLLRST
/**
  * @}
  */

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @defgroup BUS_LL_Exported_Functions BUS Exported Functions
  * @{
  */

/** @defgroup BUS_LL_EF_BUS BUS
  * @{
  */

/**
  * @brief  Enable AXI bus clock.
  * @rmtoll BUSENSR      ACLKNENS      LL_BUS_EnableClock\n
  *         BUSENSR      ACLKNCENS     LL_BUS_EnableClock\n
  *         BUSENSR      AHBMENS       LL_BUS_EnableClock\n
  *         BUSENSR      AHB1ENS       LL_BUS_EnableClock\n
  *         BUSENSR      AHB2ENS       LL_BUS_EnableClock\n
  *         BUSENSR      AHB3ENS       LL_BUS_EnableClock\n
  *         BUSENSR      AHB4ENS       LL_BUS_EnableClock\n
  *         BUSENSR      AHB5ENS       LL_BUS_EnableClock\n
  *         BUSENSR      APB1ENS       LL_BUS_EnableClock\n
  *         BUSENSR      APB2ENS       LL_BUS_EnableClock\n
  *         BUSENSR      APB3ENS       LL_BUS_EnableClock\n
  *         BUSENSR      APB4ENS       LL_BUS_EnableClock\n
  *         BUSENSR      APB5ENS       LL_BUS_EnableClock
  * @param  Bus This parameter can be a combination of the following values:
  *         @arg @ref LL_ACLKN
  *         @arg @ref LL_ACLKNC
  *         @arg @ref LL_AHBM
  *         @arg @ref LL_AHB1
  *         @arg @ref LL_AHB2
  *         @arg @ref LL_AHB3
  *         @arg @ref LL_AHB4
  *         @arg @ref LL_AHB5
  *         @arg @ref LL_APB1
  *         @arg @ref LL_APB2
  *         @arg @ref LL_APB3
  *         @arg @ref LL_APB4
  *         @arg @ref LL_APB5
  * @retval None
  */
__STATIC_INLINE void LL_BUS_EnableClock(uint32_t Bus)
{
  __IO uint32_t tmpreg;
  WRITE_REG(RCC->BUSENSR, Bus);
  /* Delay after an RCC bus clock enabling */
  tmpreg = READ_REG(RCC->BUSENR);
  (void)tmpreg;
}

/**
  * @brief  Check if AXI bus clock is enabled or not
  * @rmtoll BUSENR       ACLKNEN       LL_BUS_IsEnabledClock\n
  *         BUSENR       ACLKNCEN      LL_BUS_IsEnabledClock\n
  *         BUSENR       AHBMEN        LL_BUS_IsEnabledClock\n
  *         BUSENR       AHB1EN        LL_BUS_IsEnabledClock\n
  *         BUSENR       AHB2EN        LL_BUS_IsEnabledClock\n
  *         BUSENR       AHB3EN        LL_BUS_IsEnabledClock\n
  *         BUSENR       AHB4EN        LL_BUS_IsEnabledClock\n
  *         BUSENR       AHB5EN        LL_BUS_IsEnabledClock\n
  *         BUSENR       APB1EN        LL_BUS_IsEnabledClock\n
  *         BUSENR       APB2EN        LL_BUS_IsEnabledClock\n
  *         BUSENR       APB3EN        LL_BUS_IsEnabledClock\n
  *         BUSENR       APB4EN        LL_BUS_IsEnabledClock\n
  *         BUSENR       APB5EN        LL_BUS_IsEnabledClock
  * @param  Bus This parameter can be a combination of the following values:
  *         @arg @ref LL_ACLKN
  *         @arg @ref LL_ACLKNC
  *         @arg @ref LL_AHBM
  *         @arg @ref LL_AHB1
  *         @arg @ref LL_AHB2
  *         @arg @ref LL_AHB3
  *         @arg @ref LL_AHB4
  *         @arg @ref LL_AHB5
  *         @arg @ref LL_APB1
  *         @arg @ref LL_APB2
  *         @arg @ref LL_APB3
  *         @arg @ref LL_APB4
  *         @arg @ref LL_APB5
  * @retval uint32_t
  */
__STATIC_INLINE uint32_t LL_BUS_IsEnabledClock(uint32_t Bus)
{
  return ((READ_BIT(RCC->BUSENR, Bus) == Bus) ? 1UL : 0UL);
}

/**
  * @brief  Disable AXI bus clock.
  * @rmtoll BUSENCR      ACLKNENC      LL_BUS_DisableClock\n
  *         BUSENCR      ACLKNCENC     LL_BUS_DisableClock\n
  *         BUSENCR      AHBMENC       LL_BUS_DisableClock\n
  *         BUSENCR      AHB1ENC       LL_BUS_DisableClock\n
  *         BUSENCR      AHB2ENC       LL_BUS_DisableClock\n
  *         BUSENCR      AHB3ENC       LL_BUS_DisableClock\n
  *         BUSENCR      AHB4ENC       LL_BUS_DisableClock\n
  *         BUSENCR      AHB5ENC       LL_BUS_DisableClock\n
  *         BUSENCR      APB1ENC       LL_BUS_DisableClock\n
  *         BUSENCR      APB2ENC       LL_BUS_DisableClock\n
  *         BUSENCR      APB3ENC       LL_BUS_DisableClock\n
  *         BUSENCR      APB4ENC       LL_BUS_DisableClock\n
  *         BUSENCR      APB5ENC       LL_BUS_DisableClock
  * @param  Bus This parameter can be a combination of the following values:
  *         @arg @ref LL_ACLKN
  *         @arg @ref LL_ACLKNC
  *         @arg @ref LL_AHBM
  *         @arg @ref LL_AHB1
  *         @arg @ref LL_AHB2
  *         @arg @ref LL_AHB3
  *         @arg @ref LL_AHB4
  *         @arg @ref LL_AHB5
  *         @arg @ref LL_APB1
  *         @arg @ref LL_APB2
  *         @arg @ref LL_APB3
  *         @arg @ref LL_APB4
  *         @arg @ref LL_APB5
  * @retval None
  */
__STATIC_INLINE void LL_BUS_DisableClock(uint32_t Bus)
{
  WRITE_REG(RCC->BUSENCR, Bus);
}

/**
  * @brief  Enable AXI bus clock during Low Power mode.
  * @rmtoll BUSLPENSR    ACLKNLPENS    LL_BUS_EnableClockLowPower\n
  *         BUSLPENSR    ACLKNCLPENS   LL_BUS_EnableClockLowPower\n
  *         BUSLPENSR    AHBMLPENS     LL_BUS_EnableClockLowPower\n
  *         BUSLPENSR    AHB1LPENS     LL_BUS_EnableClockLowPower\n
  *         BUSLPENSR    AHB2LPENS     LL_BUS_EnableClockLowPower\n
  *         BUSLPENSR    AHB3LPENS     LL_BUS_EnableClockLowPower\n
  *         BUSLPENSR    AHB4LPENS     LL_BUS_EnableClockLowPower\n
  *         BUSLPENSR    AHB5LPENS     LL_BUS_EnableClockLowPower\n
  *         BUSLPENSR    APB1LPENS     LL_BUS_EnableClockLowPower\n
  *         BUSLPENSR    APB2LPENS     LL_BUS_EnableClockLowPower\n
  *         BUSLPENSR    APB3LPENS     LL_BUS_EnableClockLowPower\n
  *         BUSLPENSR    APB4LPENS     LL_BUS_EnableClockLowPower\n
  *         BUSLPENSR    APB5LPENS     LL_BUS_EnableClockLowPower
  * @param  Bus This parameter can be a combination of the following values:
  *         @arg @ref LL_ACLKN
  *         @arg @ref LL_ACLKNC
  *         @arg @ref LL_AHBM
  *         @arg @ref LL_AHB1
  *         @arg @ref LL_AHB2
  *         @arg @ref LL_AHB3
  *         @arg @ref LL_AHB4
  *         @arg @ref LL_AHB5
  *         @arg @ref LL_APB1
  *         @arg @ref LL_APB2
  *         @arg @ref LL_APB3
  *         @arg @ref LL_APB4
  *         @arg @ref LL_APB5
  * @retval None
  */
__STATIC_INLINE void LL_BUS_EnableClockLowPower(uint32_t Bus)
{
  __IO uint32_t tmpreg;
  WRITE_REG(RCC->BUSLPENSR, Bus);
  /* Delay after an RCC bus clock enabling */
  tmpreg = READ_REG(RCC->BUSLPENR);
  (void)tmpreg;
}

/**
  * @brief  Check if AXI bus clock during Low Power mode is enabled or not
  * @rmtoll BUSLPENR       ACLKNLPEN       LL_BUS_IsEnabledClockLowPower\n
  *         BUSLPENR       ACLKNCLPEN      LL_BUS_IsEnabledClockLowPower\n
  *         BUSLPENR       AHBMLPEN        LL_BUS_IsEnabledClockLowPower\n
  *         BUSLPENR       AHB1LPEN        LL_BUS_IsEnabledClockLowPower\n
  *         BUSLPENR       AHB2LPEN        LL_BUS_IsEnabledClockLowPower\n
  *         BUSLPENR       AHB3LPEN        LL_BUS_IsEnabledClockLowPower\n
  *         BUSLPENR       AHB4LPEN        LL_BUS_IsEnabledClockLowPower\n
  *         BUSLPENR       AHB5LPEN        LL_BUS_IsEnabledClockLowPower\n
  *         BUSLPENR       APB1LPEN        LL_BUS_IsEnabledClockLowPower\n
  *         BUSLPENR       APB2LPEN        LL_BUS_IsEnabledClockLowPower\n
  *         BUSLPENR       APB3LPEN        LL_BUS_IsEnabledClockLowPower\n
  *         BUSLPENR       APB4LPEN        LL_BUS_IsEnabledClockLowPower\n
  *         BUSLPENR       APB5LPEN        LL_BUS_IsEnabledClockLowPower
  * @param  Bus This parameter can be a combination of the following values:
  *         @arg @ref LL_ACLKN
  *         @arg @ref LL_ACLKNC
  *         @arg @ref LL_AHBM
  *         @arg @ref LL_AHB1
  *         @arg @ref LL_AHB2
  *         @arg @ref LL_AHB3
  *         @arg @ref LL_AHB4
  *         @arg @ref LL_AHB5
  *         @arg @ref LL_APB1
  *         @arg @ref LL_APB2
  *         @arg @ref LL_APB3
  *         @arg @ref LL_APB4
  *         @arg @ref LL_APB5
  * @retval uint32_t
  */
__STATIC_INLINE uint32_t LL_BUS_IsEnabledClockLowPower(uint32_t Bus)
{
  return ((READ_BIT(RCC->BUSLPENR, Bus) == Bus) ? 1UL : 0UL);
}

/**
  * @brief  Disable AXI bus clock during Low Power mode.
  * @rmtoll BUSLPENCR    ACLKNLPENC    LL_BUS_DisableClockLowPower\n
  *         BUSLPENCR    ACLKNCLPENC   LL_BUS_DisableClockLowPower\n
  *         BUSLPENCR    AHBMLPENC     LL_BUS_DisableClockLowPower\n
  *         BUSLPENCR    AHB1LPENC     LL_BUS_DisableClockLowPower\n
  *         BUSLPENCR    AHB2LPENC     LL_BUS_DisableClockLowPower\n
  *         BUSLPENCR    AHB3LPENC     LL_BUS_DisableClockLowPower\n
  *         BUSLPENCR    AHB4LPENC     LL_BUS_DisableClockLowPower\n
  *         BUSLPENCR    AHB5LPENC     LL_BUS_DisableClockLowPower\n
  *         BUSLPENCR    APB1LPENC     LL_BUS_DisableClockLowPower\n
  *         BUSLPENCR    APB2LPENC     LL_BUS_DisableClockLowPower\n
  *         BUSLPENCR    APB3LPENC     LL_BUS_DisableClockLowPower\n
  *         BUSLPENCR    APB4LPENC     LL_BUS_DisableClockLowPower\n
  *         BUSLPENCR    APB5LPENC     LL_BUS_DisableClockLowPower
  * @param  Bus This parameter can be a combination of the following values:
  *         @arg @ref LL_ACLKN
  *         @arg @ref LL_ACLKNC
  *         @arg @ref LL_AHBM
  *         @arg @ref LL_AHB1
  *         @arg @ref LL_AHB2
  *         @arg @ref LL_AHB3
  *         @arg @ref LL_AHB4
  *         @arg @ref LL_AHB5
  *         @arg @ref LL_APB1
  *         @arg @ref LL_APB2
  *         @arg @ref LL_APB3
  *         @arg @ref LL_APB4
  *         @arg @ref LL_APB5
  * @retval None
  */
__STATIC_INLINE void LL_BUS_DisableClockLowPower(uint32_t Bus)
{
  WRITE_REG(RCC->BUSLPENCR, Bus);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_MEM MEM
  * @{
  */

/**
  * @brief  Enable memories clock.
  * @rmtoll MEMENSR      AXISRAM1ENS   LL_MEM_EnableClock\n
  *         MEMENSR      AXISRAM2ENS   LL_MEM_EnableClock\n
  *         MEMENSR      AXISRAM3ENS   LL_MEM_EnableClock\n
  *         MEMENSR      AXISRAM4ENS   LL_MEM_EnableClock\n
  *         MEMENSR      AXISRAM5ENS   LL_MEM_EnableClock\n
  *         MEMENSR      AXISRAM6ENS   LL_MEM_EnableClock\n
  *         MEMENSR      AHBSRAM1ENS   LL_MEM_EnableClock\n
  *         MEMENSR      AHBSRAM2ENS   LL_MEM_EnableClock\n
  *         MEMENSR      BKPSRAMENS    LL_MEM_EnableClock\n
  *         MEMENSR      FLEXRAMENS    LL_MEM_EnableClock\n
  *         MEMENSR      CACHEAXIRAMENS LL_MEM_EnableClock\n
  *         MEMENSR      VENCRAMENS    LL_MEM_EnableClock
  * @param  Memories This parameter can be a combination of the following values:
  *         @arg @ref LL_MEM_AXISRAM1
  *         @arg @ref LL_MEM_AXISRAM2
  *         @arg @ref LL_MEM_AXISRAM3
  *         @arg @ref LL_MEM_AXISRAM4
  *         @arg @ref LL_MEM_AXISRAM5
  *         @arg @ref LL_MEM_AXISRAM6
  *         @arg @ref LL_MEM_AHBSRAM1
  *         @arg @ref LL_MEM_AHBSRAM2
  *         @arg @ref LL_MEM_BKPSRAM
  *         @arg @ref LL_MEM_FLEXRAM
  *         @arg @ref LL_MEM_CACHEAXIRAM
  *         @arg @ref LL_MEM_VENCRAM
  * @retval None
  */
__STATIC_INLINE void LL_MEM_EnableClock(uint32_t Memories)
{
  __IO uint32_t tmpreg;
  WRITE_REG(RCC->MEMENSR, Memories);
  /* Delay after an RCC memories clock enabling */
  tmpreg = READ_REG(RCC->MEMENR);
  (void)tmpreg;
}

/**
  * @brief  Check if memory clock is enabled or not
  * @rmtoll MEMENR       AXISRAM1EN    LL_MEM_IsEnabledClock\n
  *         MEMENR       AXISRAM2EN    LL_MEM_IsEnabledClock\n
  *         MEMENR       AXISRAM3EN    LL_MEM_IsEnabledClock\n
  *         MEMENR       AXISRAM4EN    LL_MEM_IsEnabledClock\n
  *         MEMENR       AXISRAM5EN    LL_MEM_IsEnabledClock\n
  *         MEMENR       AXISRAM6EN    LL_MEM_IsEnabledClock\n
  *         MEMENR       AHBSRAM1EN    LL_MEM_IsEnabledClock\n
  *         MEMENR       AHBSRAM2EN    LL_MEM_IsEnabledClock\n
  *         MEMENR       BKPSRAMEN     LL_MEM_IsEnabledClock\n
  *         MEMENR       FLEXRAMEN     LL_MEM_IsEnabledClock\n
  *         MEMENR       CACHEAXIRAMEN LL_MEM_IsEnabledClock\n
  *         MEMENR       VENCRAMEN     LL_MEM_IsEnabledClock
  * @param  Memories This parameter can be a combination of the following values:
  *         @arg @ref LL_MEM_AXISRAM1
  *         @arg @ref LL_MEM_AXISRAM2
  *         @arg @ref LL_MEM_AXISRAM3
  *         @arg @ref LL_MEM_AXISRAM4
  *         @arg @ref LL_MEM_AXISRAM5
  *         @arg @ref LL_MEM_AXISRAM6
  *         @arg @ref LL_MEM_AHBSRAM1
  *         @arg @ref LL_MEM_AHBSRAM2
  *         @arg @ref LL_MEM_BKPSRAM
  *         @arg @ref LL_MEM_FLEXRAM
  *         @arg @ref LL_MEM_CACHEAXIRAM
  *         @arg @ref LL_MEM_VENCRAM
  * @retval uint32_t
  */
__STATIC_INLINE uint32_t LL_MEM_IsEnabledClock(uint32_t Memories)
{
  return ((READ_BIT(RCC->MEMENR, Memories) == Memories) ? 1UL : 0UL);
}

/**
  * @brief  Disable memories clock.
  * @rmtoll MEMENCR      AXISRAM1ENC   LL_MEM_DisableClock\n
  *         MEMENCR      AXISRAM2ENC   LL_MEM_DisableClock\n
  *         MEMENCR      AXISRAM3ENC   LL_MEM_DisableClock\n
  *         MEMENCR      AXISRAM4ENC   LL_MEM_DisableClock\n
  *         MEMENCR      AXISRAM5ENC   LL_MEM_DisableClock\n
  *         MEMENCR      AXISRAM6ENC   LL_MEM_DisableClock\n
  *         MEMENCR      AHBSRAM1ENC   LL_MEM_DisableClock\n
  *         MEMENCR      AHBSRAM2ENC   LL_MEM_DisableClock\n
  *         MEMENCR      BKPSRAMENC    LL_MEM_DisableClock\n
  *         MEMENCR      FLEXRAMENC    LL_MEM_DisableClock\n
  *         MEMENCR      CACHEAXIRAMENC LL_MEM_DisableClock\n
  *         MEMENCR      VENCRAMENC    LL_MEM_DisableClock
  * @param  Memories This parameter can be a combination of the following values:
  *         @arg @ref LL_MEM_AXISRAM1
  *         @arg @ref LL_MEM_AXISRAM2
  *         @arg @ref LL_MEM_AXISRAM3
  *         @arg @ref LL_MEM_AXISRAM4
  *         @arg @ref LL_MEM_AXISRAM5
  *         @arg @ref LL_MEM_AXISRAM6
  *         @arg @ref LL_MEM_AHBSRAM1
  *         @arg @ref LL_MEM_AHBSRAM2
  *         @arg @ref LL_MEM_BKPSRAM
  *         @arg @ref LL_MEM_FLEXRAM
  *         @arg @ref LL_MEM_CACHEAXIRAM
  *         @arg @ref LL_MEM_VENCRAM
  * @retval None
  */
__STATIC_INLINE void LL_MEM_DisableClock(uint32_t Memories)
{
  WRITE_REG(RCC->MEMENCR, Memories);
}

/**
  * @brief  Enable memories clock during Low Power mode.
  * @rmtoll MEMLPENSR    AXISRAM1LPENS LL_MEM_EnableClockLowPower\n
  *         MEMLPENSR    AXISRAM2LPENS LL_MEM_EnableClockLowPower\n
  *         MEMLPENSR    AXISRAM3LPENS LL_MEM_EnableClockLowPower\n
  *         MEMLPENSR    AXISRAM4LPENS LL_MEM_EnableClockLowPower\n
  *         MEMLPENSR    AXISRAM5LPENS LL_MEM_EnableClockLowPower\n
  *         MEMLPENSR    AXISRAM6LPENS LL_MEM_EnableClockLowPower\n
  *         MEMLPENSR    AHBSRAM1LPENS LL_MEM_EnableClockLowPower\n
  *         MEMLPENSR    AHBSRAM2LPENS LL_MEM_EnableClockLowPower\n
  *         MEMLPENSR    BKPSRAMLPENS  LL_MEM_EnableClockLowPower\n
  *         MEMLPENSR    FLEXRAMLPENS  LL_MEM_EnableClockLowPower\n
  *         MEMLPENSR    CACHEAXIRAMLPENS LL_MEM_EnableClockLowPower\n
  *         MEMLPENSR    VENCRAMLPENS  LL_MEM_EnableClockLowPower
  * @param  Memories This parameter can be a combination of the following values:
  *         @arg @ref LL_MEM_AXISRAM1
  *         @arg @ref LL_MEM_AXISRAM2
  *         @arg @ref LL_MEM_AXISRAM3
  *         @arg @ref LL_MEM_AXISRAM4
  *         @arg @ref LL_MEM_AXISRAM5
  *         @arg @ref LL_MEM_AXISRAM6
  *         @arg @ref LL_MEM_AHBSRAM1
  *         @arg @ref LL_MEM_AHBSRAM2
  *         @arg @ref LL_MEM_BKPSRAM
  *         @arg @ref LL_MEM_FLEXRAM
  *         @arg @ref LL_MEM_CACHEAXIRAM
  *         @arg @ref LL_MEM_VENCRAM
  * @retval None
  */
__STATIC_INLINE void LL_MEM_EnableClockLowPower(uint32_t Memories)
{
  __IO uint32_t tmpreg;
  WRITE_REG(RCC->MEMLPENSR, Memories);
  /* Delay after an RCC memories clock enabling */
  tmpreg = READ_REG(RCC->MEMLPENR);
  (void)tmpreg;
}

/**
  * @brief  Check if memories clock during Low Power mode is enabled or not .
  * @rmtoll MEMLPENR      AXISRAM1LPEN    LL_MEM_IsEnabledClockLowPower\n
  *         MEMLPENR      AXISRAM2LPEN    LL_MEM_IsEnabledClockLowPower\n
  *         MEMLPENR      AXISRAM3LPEN    LL_MEM_IsEnabledClockLowPower\n
  *         MEMLPENR      AXISRAM4LPEN    LL_MEM_IsEnabledClockLowPower\n
  *         MEMLPENR      AXISRAM5LPEN    LL_MEM_IsEnabledClockLowPower\n
  *         MEMLPENR      AXISRAM6LPEN    LL_MEM_IsEnabledClockLowPower\n
  *         MEMLPENR      AHBSRAM1LPEN    LL_MEM_IsEnabledClockLowPower\n
  *         MEMLPENR      AHBSRAM2LPEN    LL_MEM_IsEnabledClockLowPower\n
  *         MEMLPENR      BKPSRAMLPEN     LL_MEM_IsEnabledClockLowPower\n
  *         MEMLPENR      FLEXRAMLPEN     LL_MEM_IsEnabledClockLowPower\n
  *         MEMLPENR      CACHEAXIRAMLPEN LL_MEM_IsEnabledClockLowPower\n
  *         MEMLPENR      VENCRAMLPEN     LL_MEM_IsEnabledClockLowPower
  * @param  Memories This parameter can be a combination of the following values:
  *         @arg @ref LL_MEM_AXISRAM1
  *         @arg @ref LL_MEM_AXISRAM2
  *         @arg @ref LL_MEM_AXISRAM3
  *         @arg @ref LL_MEM_AXISRAM4
  *         @arg @ref LL_MEM_AXISRAM5
  *         @arg @ref LL_MEM_AXISRAM6
  *         @arg @ref LL_MEM_AHBSRAM1
  *         @arg @ref LL_MEM_AHBSRAM2
  *         @arg @ref LL_MEM_BKPSRAM
  *         @arg @ref LL_MEM_FLEXRAM
  *         @arg @ref LL_MEM_CACHEAXIRAM
  *         @arg @ref LL_MEM_VENCRAM
  * @retval None
  */
__STATIC_INLINE uint32_t LL_MEM_IsEnabledClockLowPower(uint32_t Memories)
{
  return ((READ_BIT(RCC->MEMLPENR, Memories) == Memories) ? 1UL : 0UL);
}

/**
  * @brief  Disable memories clock during Low Power mode.
  * @rmtoll MEMLPENCR    AXISRAM1LPENC LL_MEM_DisableClockLowPower\n
  *         MEMLPENCR    AXISRAM2LPENC LL_MEM_DisableClockLowPower\n
  *         MEMLPENCR    AXISRAM3LPENC LL_MEM_DisableClockLowPower\n
  *         MEMLPENCR    AXISRAM4LPENC LL_MEM_DisableClockLowPower\n
  *         MEMLPENCR    AXISRAM5LPENC LL_MEM_DisableClockLowPower\n
  *         MEMLPENCR    AXISRAM6LPENC LL_MEM_DisableClockLowPower\n
  *         MEMLPENCR    AHBSRAM1LPENC LL_MEM_DisableClockLowPower\n
  *         MEMLPENCR    AHBSRAM2LPENC LL_MEM_DisableClockLowPower\n
  *         MEMLPENCR    BKPSRAMLPENC  LL_MEM_DisableClockLowPower\n
  *         MEMLPENCR    FLEXRAMLPENC  LL_MEM_DisableClockLowPower\n
  *         MEMLPENCR    CACHEAXIRAMLPENC LL_MEM_DisableClockLowPower\n
  *         MEMLPENCR    VENCRAMLPENC  LL_MEM_DisableClockLowPower
  * @param  Memories This parameter can be a combination of the following values:
  *         @arg @ref LL_MEM_AXISRAM1
  *         @arg @ref LL_MEM_AXISRAM2
  *         @arg @ref LL_MEM_AXISRAM3
  *         @arg @ref LL_MEM_AXISRAM4
  *         @arg @ref LL_MEM_AXISRAM5
  *         @arg @ref LL_MEM_AXISRAM6
  *         @arg @ref LL_MEM_AHBSRAM1
  *         @arg @ref LL_MEM_AHBSRAM2
  *         @arg @ref LL_MEM_BKPSRAM
  *         @arg @ref LL_MEM_FLEXRAM
  *         @arg @ref LL_MEM_CACHEAXIRAM
  *         @arg @ref LL_MEM_VENCRAM
  * @retval None
  */
__STATIC_INLINE void LL_MEM_DisableClockLowPower(uint32_t Memories)
{
  WRITE_REG(RCC->MEMLPENCR, Memories);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_AHB1 AHB1
  * @{
  */

/**
  * @brief  Enable AHB1 peripherals clock.
  * @rmtoll AHB1ENSR     GPDMA1ENS     LL_AHB1_GRP1_EnableClock\n
  *         AHB1ENSR     ADC12ENS      LL_AHB1_GRP1_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_GPDMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ADC12
  * @retval None
  */
__STATIC_INLINE void LL_AHB1_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  WRITE_REG(RCC->AHB1ENSR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_REG(RCC->AHB1ENR);
  (void)tmpreg;
}

/**
  * @brief  Check if AHB1 peripheral clock is enabled or not
  * @rmtoll AHB1ENR      GPDMA1EN      LL_AHB1_GRP1_IsEnabledClock\n
  *         AHB1ENR      ADC12EN       LL_AHB1_GRP1_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_GPDMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ADC12
  * @retval uint32_t
  */
__STATIC_INLINE uint32_t LL_AHB1_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC->AHB1ENR, Periphs) == Periphs) ? 1UL : 0UL);
}

/**
  * @brief  Disable AHB1 peripherals clock.
  * @rmtoll AHB1ENCR     GPDMA1ENC     LL_AHB1_GRP1_DisableClock\n
  *         AHB1ENCR     ADC12ENC      LL_AHB1_GRP1_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_GPDMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ADC12
  * @retval None
  */
__STATIC_INLINE void LL_AHB1_GRP1_DisableClock(uint32_t Periphs)
{
  WRITE_REG(RCC->AHB1ENCR, Periphs);
}

/**
  * @brief  Force AHB1 peripherals reset.
  * @rmtoll AHB1RSTSR    GPDMA1RSTS    LL_AHB1_GRP1_ForceReset\n
  *         AHB1RSTSR    ADC12RSTS     LL_AHB1_GRP1_ForceReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_GPDMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ADC12
  * @retval None
  */
__STATIC_INLINE void LL_AHB1_GRP1_ForceReset(uint32_t Periphs)
{
  WRITE_REG(RCC->AHB1RSTSR, Periphs);
}

/**
  * @brief  Release AHB1 peripherals reset.
  * @rmtoll AHB1RSTCR    GPDMA1RSTC    LL_AHB1_GRP1_ReleaseReset\n
  *         AHB1RSTCR    ADC12RSTC     LL_AHB1_GRP1_ReleaseReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_GPDMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ADC12
  * @retval None
  */
__STATIC_INLINE void LL_AHB1_GRP1_ReleaseReset(uint32_t Periphs)
{
  WRITE_REG(RCC->AHB1RSTCR, Periphs);
}

/**
  * @brief  Enable AHB1 peripherals clock during Low Power mode.
  * @rmtoll AHB1LPENSR   GPDMA1LPENS   LL_AHB1_GRP1_EnableClockLowPower\n
  *         AHB1LPENSR   ADC12LPENS    LL_AHB1_GRP1_EnableClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_GPDMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ADC12
  * @retval None
  */
__STATIC_INLINE void LL_AHB1_GRP1_EnableClockLowPower(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  WRITE_REG(RCC->AHB1LPENSR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_REG(RCC->AHB1LPENR);
  (void)tmpreg;
}

/**
  * @brief  Check if AHB1 peripheral clock during Low Power mode is enabled or not .
  * @rmtoll AHB1LPENR    GPDMA1LPEN    LL_AHB1_GRP1_IsEnabledClockLowPower\n
  *         AHB1LPENR    ADC12LPEN     LL_AHB1_GRP1_IsEnabledClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_GPDMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ADC12
  * @retval uint32_t
  */
__STATIC_INLINE uint32_t LL_AHB1_GRP1_IsEnabledClockLowPower(uint32_t Periphs)
{
  return ((READ_BIT(RCC->AHB1LPENR, Periphs) == Periphs) ? 1UL : 0UL);
}

/**
  * @brief  Disable AHB1 peripherals clock during Low Power mode.
  * @rmtoll AHB1LPENCR   GPDMA1LPENC   LL_AHB1_GRP1_DisableClockLowPower\n
  *         AHB1LPENCR   ADC12LPENC    LL_AHB1_GRP1_DisableClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_GPDMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ADC12
  * @retval None
  */
__STATIC_INLINE void LL_AHB1_GRP1_DisableClockLowPower(uint32_t Periphs)
{
  WRITE_REG(RCC->AHB1LPENCR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_AHB2 AHB2
  * @{
  */

/**
  * @brief  Enable AHB2 peripherals clock.
  * @rmtoll AHB2ENSR     RAMCFGENS     LL_AHB2_GRP1_EnableClock\n
  *         AHB2ENSR     MDF1ENS       LL_AHB2_GRP1_EnableClock\n
  *         AHB2ENSR     ADF1ENS       LL_AHB2_GRP1_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_RAMCFG
  *         @arg @ref LL_AHB2_GRP1_PERIPH_MDF1
  *         @arg @ref LL_AHB2_GRP1_PERIPH_ADF1
  * @retval None
  */
__STATIC_INLINE void LL_AHB2_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  WRITE_REG(RCC->AHB2ENSR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_REG(RCC->AHB2ENR);
  (void)tmpreg;
}

/**
  * @brief  Check if AHB2 peripheral clock is enabled or not
  * @rmtoll AHB2ENR      RAMCFGEN      LL_AHB2_GRP1_IsEnabledClock\n
  *         AHB2ENR      MDF1EN        LL_AHB2_GRP1_IsEnabledClock\n
  *         AHB2ENR      ADF1EN        LL_AHB2_GRP1_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_RAMCFG
  *         @arg @ref LL_AHB2_GRP1_PERIPH_MDF1
  *         @arg @ref LL_AHB2_GRP1_PERIPH_ADF1
  * @retval uint32_t
  */
__STATIC_INLINE uint32_t LL_AHB2_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC->AHB2ENR, Periphs) == Periphs) ? 1UL : 0UL);
}

/**
  * @brief  Disable AHB2 peripherals clock.
  * @rmtoll AHB2ENCR     RAMCFGENC     LL_AHB2_GRP1_DisableClock\n
  *         AHB2ENCR     MDF1ENC       LL_AHB2_GRP1_DisableClock\n
  *         AHB2ENCR     ADF1ENC       LL_AHB2_GRP1_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_RAMCFG
  *         @arg @ref LL_AHB2_GRP1_PERIPH_MDF1
  *         @arg @ref LL_AHB2_GRP1_PERIPH_ADF1
  * @retval None
  */
__STATIC_INLINE void LL_AHB2_GRP1_DisableClock(uint32_t Periphs)
{
  WRITE_REG(RCC->AHB2ENCR, Periphs);
}

/**
  * @brief  Force AHB2 peripherals reset.
  * @rmtoll AHB2RSTSR    RAMCFGRSTS     LL_AHB2_GRP1_ForceReset\n
  *         AHB2RSTSR    MDF1RSTS       LL_AHB2_GRP1_ForceReset\n
  *         AHB2RSTSR    ADF1RSTS       LL_AHB2_GRP1_ForceReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_RAMCFG
  *         @arg @ref LL_AHB2_GRP1_PERIPH_MDF1
  *         @arg @ref LL_AHB2_GRP1_PERIPH_ADF1
  * @retval None
  */
__STATIC_INLINE void LL_AHB2_GRP1_ForceReset(uint32_t Periphs)
{
  WRITE_REG(RCC->AHB2RSTSR, Periphs);
}

/**
  * @brief  Release AHB2 peripherals reset.
  * @rmtoll AHB2RSTCR     RAMCFGRSTC    LL_AHB2_GRP1_ReleaseReset\n
  *         AHB2RSTCR     MDF1RSTC      LL_AHB2_GRP1_ReleaseReset\n
  *         AHB2RSTCR     ADF1RSTC      LL_AHB2_GRP1_ReleaseReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_RAMCFG
  *         @arg @ref LL_AHB2_GRP1_PERIPH_MDF1
  *         @arg @ref LL_AHB2_GRP1_PERIPH_ADF1
  * @retval None
  */
__STATIC_INLINE void LL_AHB2_GRP1_ReleaseReset(uint32_t Periphs)
{
  WRITE_REG(RCC->AHB2RSTCR, Periphs);
}

/**
  * @brief  Enable AHB2 peripherals clock during Low Power mode.
  * @rmtoll AHB2LPENSR   RAMCFGLPENC   LL_AHB2_GRP1_EnableClockLowPower\n
  *         AHB2LPENSR   MDF1LPENC     LL_AHB2_GRP1_EnableClockLowPower\n
  *         AHB2LPENSR   ADF1LPENC     LL_AHB2_GRP1_EnableClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_RAMCFG
  *         @arg @ref LL_AHB2_GRP1_PERIPH_MDF1
  *         @arg @ref LL_AHB2_GRP1_PERIPH_ADF1
  * @retval None
  */
__STATIC_INLINE void LL_AHB2_GRP1_EnableClockLowPower(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  WRITE_REG(RCC->AHB2LPENSR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_REG(RCC->AHB2LPENR);
  (void)tmpreg;
}

/**
  * @brief  Check if AHB2 peripheral clock during Low Power mode is enabled or not .
  * @rmtoll AHB2LPENR    RAMCFGLPEN    LL_AHB2_GRP1_IsEnabledClockLowPower\n
  *         AHB2LPENR    MDF1LPEN      LL_AHB2_GRP1_IsEnabledClockLowPower\n
  *         AHB2LPENR    ADF1LPEN      LL_AHB2_GRP1_IsEnabledClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_RAMCFG
  *         @arg @ref LL_AHB2_GRP1_PERIPH_MDF1
  *         @arg @ref LL_AHB2_GRP1_PERIPH_ADF1
  * @retval uint32_t
  */
__STATIC_INLINE uint32_t LL_AHB2_GRP1_IsEnabledClockLowPower(uint32_t Periphs)
{
  return ((READ_BIT(RCC->AHB2LPENR, Periphs) == Periphs) ? 1UL : 0UL);
}

/**
  * @brief  Disable AHB2 peripherals clock during Low Power mode.
  * @rmtoll AHB2LPENCR   RAMCFGLPENC   LL_AHB2_GRP1_DisableClockLowPower\n
  *         AHB2LPENCR   MDF1LPENC     LL_AHB2_GRP1_DisableClockLowPower\n
  *         AHB2LPENCR   ADF1LPENC     LL_AHB2_GRP1_DisableClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_RAMCFG
  *         @arg @ref LL_AHB2_GRP1_PERIPH_MDF1
  *         @arg @ref LL_AHB2_GRP1_PERIPH_ADF1
  * @retval None
  */
__STATIC_INLINE void LL_AHB2_GRP1_DisableClockLowPower(uint32_t Periphs)
{
  WRITE_REG(RCC->AHB2LPENCR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_AHB3 AHB3
  * @{
  */

/**
  * @brief  Enable AHB3 peripherals clock.
  * @rmtoll AHB3ENSR     RNGENS        LL_AHB3_GRP1_EnableClock\n
  *         AHB3ENSR     HASHENS       LL_AHB3_GRP1_EnableClock\n
  *         AHB3ENSR     CRYPENS       LL_AHB3_GRP1_EnableClock\n  (*)
  *         AHB3ENSR     SAESENS       LL_AHB3_GRP1_EnableClock\n  (*)
  *         AHB3ENSR     PKAENS        LL_AHB3_GRP1_EnableClock\n
  *         AHB3ENSR     RIFSCENS      LL_AHB3_GRP1_EnableClock\n
  *         AHB3ENSR     RISAFENS      LL_AHB3_GRP1_EnableClock\n
  *         AHB3ENSR     IACENS        LL_AHB3_GRP1_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB3_GRP1_PERIPH_RNG
  *         @arg @ref LL_AHB3_GRP1_PERIPH_HASH
  *         @arg @ref LL_AHB3_GRP1_PERIPH_CRYP (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_SAES (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_PKA
  *         @arg @ref LL_AHB3_GRP1_PERIPH_RIFSC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_RISAF
  *         @arg @ref LL_AHB3_GRP1_PERIPH_IAC
  *
  *         (*) value not defined in all devices.
  * @retval None
  */
__STATIC_INLINE void LL_AHB3_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  WRITE_REG(RCC->AHB3ENSR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_REG(RCC->AHB3ENR);
  (void)tmpreg;
}

/**
  * @brief  Check if AHB3 peripheral clock is enabled or not
  * @rmtoll AHB3ENR      RNGEN         LL_AHB3_GRP1_IsEnabledClock\n
  *         AHB3ENR      HASHEN        LL_AHB3_GRP1_IsEnabledClock\n
  *         AHB3ENR      CRYPEN        LL_AHB3_GRP1_IsEnabledClock\n  (*)
  *         AHB3ENR      SAESEN        LL_AHB3_GRP1_IsEnabledClock\n  (*)
  *         AHB3ENR      PKAEN         LL_AHB3_GRP1_IsEnabledClock\n
  *         AHB3ENR      RIFSCEN       LL_AHB3_GRP1_IsEnabledClock\n
  *         AHB3ENR      RISAFEN       LL_AHB3_GRP1_IsEnabledClock\n
  *         AHB3ENR      IACEN         LL_AHB3_GRP1_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB3_GRP1_PERIPH_RNG
  *         @arg @ref LL_AHB3_GRP1_PERIPH_HASH
  *         @arg @ref LL_AHB3_GRP1_PERIPH_CRYP (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_SAES (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_PKA
  *         @arg @ref LL_AHB3_GRP1_PERIPH_RIFSC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_RISAF
  *         @arg @ref LL_AHB3_GRP1_PERIPH_IAC
  *
  *         (*) value not defined in all devices.
  * @retval uint32_t
  */
__STATIC_INLINE uint32_t LL_AHB3_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC->AHB3ENR, Periphs) == Periphs) ? 1UL : 0UL);
}

/**
  * @brief  Disable AHB3 peripherals clock.
  * @rmtoll AHB3ENCR     RNGENC        LL_AHB3_GRP1_DisableClock\n
  *         AHB3ENCR     HASHENC       LL_AHB3_GRP1_DisableClock\n
  *         AHB3ENCR     CRYPENC       LL_AHB3_GRP1_DisableClock\n  (*)
  *         AHB3ENCR     SAESENC       LL_AHB3_GRP1_DisableClock\n  (*)
  *         AHB3ENCR     PKAENC        LL_AHB3_GRP1_DisableClock\n
  *         AHB3ENCR     RIFSCENC      LL_AHB3_GRP1_DisableClock\n
  *         AHB3ENCR     RISAFENC      LL_AHB3_GRP1_DisableClock\n
  *         AHB3ENCR     IACENC        LL_AHB3_GRP1_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB3_GRP1_PERIPH_RNG
  *         @arg @ref LL_AHB3_GRP1_PERIPH_HASH
  *         @arg @ref LL_AHB3_GRP1_PERIPH_CRYP (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_SAES (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_PKA
  *         @arg @ref LL_AHB3_GRP1_PERIPH_RIFSC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_RISAF
  *         @arg @ref LL_AHB3_GRP1_PERIPH_IAC
  *
  *         (*) value not defined in all devices.
  * @retval None
  */
__STATIC_INLINE void LL_AHB3_GRP1_DisableClock(uint32_t Periphs)
{
  WRITE_REG(RCC->AHB3ENCR, Periphs);
}

/**
  * @brief  Force AHB3 peripherals reset.
  * @rmtoll AHB3RSTSR    RNGRSTS       LL_AHB3_GRP1_ForceReset\n
  *         AHB3RSTSR    HASHRSTS      LL_AHB3_GRP1_ForceReset\n
  *         AHB3RSTSR    CRYPRSTS      LL_AHB3_GRP1_ForceReset\n  (*)
  *         AHB3RSTSR    SAESRSTS      LL_AHB3_GRP1_ForceReset\n  (*)
  *         AHB3RSTSR    PKARSTS       LL_AHB3_GRP1_ForceReset\n
  *         AHB3RSTSR    IACRSTS       LL_AHB3_GRP1_ForceReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB3_GRP1_PERIPH_RNG
  *         @arg @ref LL_AHB3_GRP1_PERIPH_HASH
  *         @arg @ref LL_AHB3_GRP1_PERIPH_CRYP (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_SAES (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_PKA
  *         @arg @ref LL_AHB3_GRP1_PERIPH_IAC
  *
  *         (*) value not defined in all devices.
  * @retval None
  */
__STATIC_INLINE void LL_AHB3_GRP1_ForceReset(uint32_t Periphs)
{
  WRITE_REG(RCC->AHB3RSTSR, Periphs);
}

/**
  * @brief  Release AHB3 peripherals reset.
  * @rmtoll AHB3RSTCR    RNGRSTC       LL_AHB3_GRP1_ReleaseReset\n
  *         AHB3RSTCR    HASHRSTC      LL_AHB3_GRP1_ReleaseReset\n
  *         AHB3RSTCR    CRYPRSTC      LL_AHB3_GRP1_ReleaseReset\n  (*)
  *         AHB3RSTCR    SAESRSTC      LL_AHB3_GRP1_ReleaseReset\n  (*)
  *         AHB3RSTCR    PKARSTC       LL_AHB3_GRP1_ReleaseReset\n
  *         AHB3RSTCR    IACRSTC       LL_AHB3_GRP1_ReleaseReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB3_GRP1_PERIPH_RNG
  *         @arg @ref LL_AHB3_GRP1_PERIPH_HASH
  *         @arg @ref LL_AHB3_GRP1_PERIPH_CRYP (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_SAES (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_PKA
  *         @arg @ref LL_AHB3_GRP1_PERIPH_IAC
  *
  *         (*) value not defined in all devices.
  * @retval None
  */
__STATIC_INLINE void LL_AHB3_GRP1_ReleaseReset(uint32_t Periphs)
{
  WRITE_REG(RCC->AHB3RSTCR, Periphs);
}

/**
  * @brief  Enable AHB3 peripherals clock during Low Power mode.
  * @rmtoll AHB3LPENSR   RNGLPENS      LL_AHB3_GRP1_EnableClockLowPower\n
  *         AHB3LPENSR   HASHLPENS     LL_AHB3_GRP1_EnableClockLowPower\n
  *         AHB3LPENSR   CRYPLPENS     LL_AHB3_GRP1_EnableClockLowPower\n  (*)
  *         AHB3LPENSR   SAESLPENS     LL_AHB3_GRP1_EnableClockLowPower\n  (*)
  *         AHB3LPENSR   PKALPENS      LL_AHB3_GRP1_EnableClockLowPower\n
  *         AHB3LPENSR   RIFSCLPENS    LL_AHB3_GRP1_EnableClockLowPower\n
  *         AHB3LPENSR   RISAFLPENS    LL_AHB3_GRP1_EnableClockLowPower\n
  *         AHB3LPENSR   IACLPENS      LL_AHB3_GRP1_EnableClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB3_GRP1_PERIPH_RNG
  *         @arg @ref LL_AHB3_GRP1_PERIPH_HASH
  *         @arg @ref LL_AHB3_GRP1_PERIPH_CRYP (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_SAES (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_PKA
  *         @arg @ref LL_AHB3_GRP1_PERIPH_RIFSC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_RISAF
  *         @arg @ref LL_AHB3_GRP1_PERIPH_IAC
  *
  *         (*) value not defined in all devices.
  * @retval None
  */
__STATIC_INLINE void LL_AHB3_GRP1_EnableClockLowPower(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  WRITE_REG(RCC->AHB3LPENSR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_REG(RCC->AHB3LPENR);
  (void)tmpreg;
}

/**
  * @brief  Check if AHB3 peripheral clock during Low Power mode is enabled or not .
  * @rmtoll AHB3LPENR    RNGLPEN       LL_AHB3_GRP1_IsEnabledClockLowPower\n
  *         AHB3LPENR    HASHLPEN      LL_AHB3_GRP1_IsEnabledClockLowPower\n
  *         AHB3LPENR    CRYPLPEN      LL_AHB3_GRP1_IsEnabledClockLowPower\n  (*)
  *         AHB3LPENR    SAESLPEN      LL_AHB3_GRP1_IsEnabledClockLowPower\n  (*)
  *         AHB3LPENR    PKALPEN       LL_AHB3_GRP1_IsEnabledClockLowPower\n
  *         AHB3LPENR    RIFSCLPEN     LL_AHB3_GRP1_IsEnabledClockLowPower\n
  *         AHB3LPENR    RISAFLPEN     LL_AHB3_GRP1_IsEnabledClockLowPower\n
  *         AHB3LPENR    IACLPEN       LL_AHB3_GRP1_IsEnabledClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB3_GRP1_PERIPH_RNG
  *         @arg @ref LL_AHB3_GRP1_PERIPH_HASH
  *         @arg @ref LL_AHB3_GRP1_PERIPH_CRYP (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_SAES (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_PKA
  *         @arg @ref LL_AHB3_GRP1_PERIPH_RIFSC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_RISAF
  *         @arg @ref LL_AHB3_GRP1_PERIPH_IAC
  *
  *         (*) value not defined in all devices.
  * @retval uint32_t
  */
__STATIC_INLINE uint32_t LL_AHB3_GRP1_IsEnabledClockLowPower(uint32_t Periphs)
{
  return ((READ_BIT(RCC->AHB3LPENR, Periphs) == Periphs) ? 1UL : 0UL);
}

/**
  * @brief  Disable AHB3 peripherals clock during Low Power mode.
  * @rmtoll AHB3LPENCR   RNGLPENC      LL_AHB3_GRP1_DisableClockLowPower\n
  *         AHB3LPENCR   HASHLPENC     LL_AHB3_GRP1_DisableClockLowPower\n
  *         AHB3LPENCR   CRYPLPENC     LL_AHB3_GRP1_DisableClockLowPower\n  (*)
  *         AHB3LPENCR   SAESLPENC     LL_AHB3_GRP1_DisableClockLowPower\n  (*)
  *         AHB3LPENCR   PKALPENC      LL_AHB3_GRP1_DisableClockLowPower\n
  *         AHB3LPENCR   RIFSCLPENC    LL_AHB3_GRP1_DisableClockLowPower\n
  *         AHB3LPENCR   RISAFLPENC    LL_AHB3_GRP1_DisableClockLowPower\n
  *         AHB3LPENCR   IACLPENC      LL_AHB3_GRP1_DisableClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB3_GRP1_PERIPH_RNG
  *         @arg @ref LL_AHB3_GRP1_PERIPH_HASH
  *         @arg @ref LL_AHB3_GRP1_PERIPH_CRYP (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_SAES (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_PKA
  *         @arg @ref LL_AHB3_GRP1_PERIPH_RIFSC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_RISAF
  *         @arg @ref LL_AHB3_GRP1_PERIPH_IAC
  *
  *         (*) value not defined in all devices.
  * @retval None
  */
__STATIC_INLINE void LL_AHB3_GRP1_DisableClockLowPower(uint32_t Periphs)
{
  WRITE_REG(RCC->AHB3LPENCR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_AHB4 AHB4
  * @{
  */

/**
  * @brief  Enable AHB4 peripherals clock.
  * @rmtoll AHB4ENSR     GPIOAENS      LL_AHB4_GRP1_EnableClock\n
  *         AHB4ENSR     GPIOBENS      LL_AHB4_GRP1_EnableClock\n
  *         AHB4ENSR     GPIOCENS      LL_AHB4_GRP1_EnableClock\n
  *         AHB4ENSR     GPIODENS      LL_AHB4_GRP1_EnableClock\n
  *         AHB4ENSR     GPIOEENS      LL_AHB4_GRP1_EnableClock\n
  *         AHB4ENSR     GPIOFENS      LL_AHB4_GRP1_EnableClock\n
  *         AHB4ENSR     GPIOGENS      LL_AHB4_GRP1_EnableClock\n
  *         AHB4ENSR     GPIOHENS      LL_AHB4_GRP1_EnableClock\n
  *         AHB4ENSR     GPIONENS      LL_AHB4_GRP1_EnableClock\n
  *         AHB4ENSR     GPIOOENS      LL_AHB4_GRP1_EnableClock\n
  *         AHB4ENSR     GPIOPENS      LL_AHB4_GRP1_EnableClock\n
  *         AHB4ENSR     GPIOQENS      LL_AHB4_GRP1_EnableClock\n
  *         AHB4ENSR     PWRENS        LL_AHB4_GRP1_EnableClock\n
  *         AHB4ENSR     CRCENS        LL_AHB4_GRP1_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOE
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOF
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOG
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOH
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPION
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOO
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOP
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOQ
  *         @arg @ref LL_AHB4_GRP1_PERIPH_PWR
  *         @arg @ref LL_AHB4_GRP1_PERIPH_CRC
  * @retval None
  */
__STATIC_INLINE void LL_AHB4_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  WRITE_REG(RCC->AHB4ENSR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_REG(RCC->AHB4ENR);
  (void)tmpreg;
}

/**
  * @brief  Check if AHB4 peripheral clock is enabled or not
  * @rmtoll AHB4ENR      GPIOAEN       LL_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOBEN       LL_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOCEN       LL_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIODEN       LL_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOEEN       LL_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOFEN       LL_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOGEN       LL_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOHEN       LL_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIONEN       LL_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOOEN       LL_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOPEN       LL_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      GPIOQEN       LL_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      PWREN         LL_AHB4_GRP1_IsEnabledClock\n
  *         AHB4ENR      CRCEN         LL_AHB4_GRP1_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOE
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOF
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOG
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOH
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPION
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOO
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOP
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOQ
  *         @arg @ref LL_AHB4_GRP1_PERIPH_PWR
  *         @arg @ref LL_AHB4_GRP1_PERIPH_CRC
  * @retval uint32_t
  */
__STATIC_INLINE uint32_t LL_AHB4_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC->AHB4ENR, Periphs) == Periphs) ? 1UL : 0UL);
}

/**
  * @brief  Disable AHB4 peripherals clock.
  * @rmtoll AHB4ENCR     GPIOAENC      LL_AHB4_GRP1_DisableClock\n
  *         AHB4ENCR     GPIOBENC      LL_AHB4_GRP1_DisableClock\n
  *         AHB4ENCR     GPIOCENC      LL_AHB4_GRP1_DisableClock\n
  *         AHB4ENCR     GPIODENC      LL_AHB4_GRP1_DisableClock\n
  *         AHB4ENCR     GPIOEENC      LL_AHB4_GRP1_DisableClock\n
  *         AHB4ENCR     GPIOFENC      LL_AHB4_GRP1_DisableClock\n
  *         AHB4ENCR     GPIOGENC      LL_AHB4_GRP1_DisableClock\n
  *         AHB4ENCR     GPIOHENC      LL_AHB4_GRP1_DisableClock\n
  *         AHB4ENCR     GPIONENC      LL_AHB4_GRP1_DisableClock\n
  *         AHB4ENCR     GPIOOENC      LL_AHB4_GRP1_DisableClock\n
  *         AHB4ENCR     GPIOPENC      LL_AHB4_GRP1_DisableClock\n
  *         AHB4ENCR     GPIOQENC      LL_AHB4_GRP1_DisableClock\n
  *         AHB4ENCR     PWRENC        LL_AHB4_GRP1_DisableClock\n
  *         AHB4ENCR     CRCENC        LL_AHB4_GRP1_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOE
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOF
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOG
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOH
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPION
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOO
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOP
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOQ
  *         @arg @ref LL_AHB4_GRP1_PERIPH_PWR
  *         @arg @ref LL_AHB4_GRP1_PERIPH_CRC
  * @retval None
  */
__STATIC_INLINE void LL_AHB4_GRP1_DisableClock(uint32_t Periphs)
{
  WRITE_REG(RCC->AHB4ENCR, Periphs);
}

/**
  * @brief  Force AHB4 peripherals reset.
  * @rmtoll AHB4RSTSR     GPIOARSTS      LL_AHB4_GRP1_ForceReset\n
  *         AHB4RSTSR     GPIOBRSTS      LL_AHB4_GRP1_ForceReset\n
  *         AHB4RSTSR     GPIOCRSTS      LL_AHB4_GRP1_ForceReset\n
  *         AHB4RSTSR     GPIODRSTS      LL_AHB4_GRP1_ForceReset\n
  *         AHB4RSTSR     GPIOERSTS      LL_AHB4_GRP1_ForceReset\n
  *         AHB4RSTSR     GPIOFRSTS      LL_AHB4_GRP1_ForceReset\n
  *         AHB4RSTSR     GPIOGRSTS      LL_AHB4_GRP1_ForceReset\n
  *         AHB4RSTSR     GPIOHRSTS      LL_AHB4_GRP1_ForceReset\n
  *         AHB4RSTSR     GPIONRSTS      LL_AHB4_GRP1_ForceReset\n
  *         AHB4RSTSR     GPIOORSTS      LL_AHB4_GRP1_ForceReset\n
  *         AHB4RSTSR     GPIOPRSTS      LL_AHB4_GRP1_ForceReset\n
  *         AHB4RSTSR     GPIOQRSTS      LL_AHB4_GRP1_ForceReset\n
  *         AHB4RSTSR     CRCRSTS        LL_AHB4_GRP1_ForceReset\n
  *         AHB4RSTSR     BKPRAMRSTS     LL_AHB4_GRP1_ForceReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOE
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOF
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOG
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOH
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPION
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOO
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOP
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOQ
  *         @arg @ref LL_AHB4_GRP1_PERIPH_PWR
  *         @arg @ref LL_AHB4_GRP1_PERIPH_CRC
  * @retval None
  */
__STATIC_INLINE void LL_AHB4_GRP1_ForceReset(uint32_t Periphs)
{
  WRITE_REG(RCC->AHB4RSTSR, Periphs);
}

/**
  * @brief  Release AHB4 peripherals reset.
  * @rmtoll AHB4RSTCR     GPIOARSTC      LL_AHB4_GRP1_ReleaseReset\n
  *         AHB4RSTCR     GPIOBRSTC      LL_AHB4_GRP1_ReleaseReset\n
  *         AHB4RSTCR     GPIOCRSTC      LL_AHB4_GRP1_ReleaseReset\n
  *         AHB4RSTCR     GPIODRSTC      LL_AHB4_GRP1_ReleaseReset\n
  *         AHB4RSTCR     GPIOERSTC      LL_AHB4_GRP1_ReleaseReset\n
  *         AHB4RSTCR     GPIOFRSTC      LL_AHB4_GRP1_ReleaseReset\n
  *         AHB4RSTCR     GPIOGRSTC      LL_AHB4_GRP1_ReleaseReset\n
  *         AHB4RSTCR     GPIOHRSTC      LL_AHB4_GRP1_ReleaseReset\n
  *         AHB4RSTCR     GPIONRSTC      LL_AHB4_GRP1_ReleaseReset\n
  *         AHB4RSTCR     GPIOORSTC      LL_AHB4_GRP1_ReleaseReset\n
  *         AHB4RSTCR     GPIOPRSTC      LL_AHB4_GRP1_ReleaseReset\n
  *         AHB4RSTCR     GPIOQRSTC      LL_AHB4_GRP1_ReleaseReset\n
  *         AHB4RSTCR     CRCRSTC        LL_AHB4_GRP1_ReleaseReset\n
  *         AHB4RSTCR     BKPRAMRSTC     LL_AHB4_GRP1_ReleaseReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOE
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOF
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOG
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOH
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPION
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOO
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOP
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOQ
  *         @arg @ref LL_AHB4_GRP1_PERIPH_PWR
  *         @arg @ref LL_AHB4_GRP1_PERIPH_CRC
  * @retval None
  */
__STATIC_INLINE void LL_AHB4_GRP1_ReleaseReset(uint32_t Periphs)
{
  WRITE_REG(RCC->AHB4RSTCR, Periphs);
}

/**
  * @brief  Enable AHB4 peripherals clock during Low Power mode.
  * @rmtoll AHB4LPENSR    GPIOALPENS     LL_AHB4_GRP1_EnableClockLowPower\n
  *         AHB4LPENSR    GPIOBLPENS     LL_AHB4_GRP1_EnableClockLowPower\n
  *         AHB4LPENSR    GPIOCLPENS     LL_AHB4_GRP1_EnableClockLowPower\n
  *         AHB4LPENSR    GPIODLPENS     LL_AHB4_GRP1_EnableClockLowPower\n
  *         AHB4LPENSR    GPIOELPENS     LL_AHB4_GRP1_EnableClockLowPower\n
  *         AHB4LPENSR    GPIOFLPENS     LL_AHB4_GRP1_EnableClockLowPower\n
  *         AHB4LPENSR    GPIOGLPENS     LL_AHB4_GRP1_EnableClockLowPower\n
  *         AHB4LPENSR    GPIOHLPENS     LL_AHB4_GRP1_EnableClockLowPower\n
  *         AHB4LPENSR    GPIONLPENS     LL_AHB4_GRP1_EnableClockLowPower\n
  *         AHB4LPENSR    GPIOOLPENS     LL_AHB4_GRP1_EnableClockLowPower\n
  *         AHB4LPENSR    GPIOPLPENS     LL_AHB4_GRP1_EnableClockLowPower\n
  *         AHB4LPENSR    GPIOQLPENS     LL_AHB4_GRP1_EnableClockLowPower\n
  *         AHB4LPENSR    CRCLPENS       LL_AHB4_GRP1_EnableClockLowPower\n
  *         AHB4LPENSR    BKPRAMLPENS    LL_AHB4_GRP1_EnableClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOE
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOF
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOG
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOH
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPION
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOO
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOP
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOQ
  *         @arg @ref LL_AHB4_GRP1_PERIPH_PWR
  *         @arg @ref LL_AHB4_GRP1_PERIPH_CRC
  * @retval None
  */
__STATIC_INLINE void LL_AHB4_GRP1_EnableClockLowPower(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  WRITE_REG(RCC->AHB4LPENSR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_REG(RCC->AHB4LPENR);
  (void)tmpreg;
}

/**
  * @brief  Check if AHB4 peripheral clock during Low Power mode is enabled or not .
  * @rmtoll AHB4LPENR     GPIOALPEN      LL_AHB4_GRP1_IsEnabledClockLowPower\n
  *         AHB4LPENR     GPIOBLPEN      LL_AHB4_GRP1_IsEnabledClockLowPower\n
  *         AHB4LPENR     GPIOCLPEN      LL_AHB4_GRP1_IsEnabledClockLowPower\n
  *         AHB4LPENR     GPIODLPEN      LL_AHB4_GRP1_IsEnabledClockLowPower\n
  *         AHB4LPENR     GPIOELPEN      LL_AHB4_GRP1_IsEnabledClockLowPower\n
  *         AHB4LPENR     GPIOFLPEN      LL_AHB4_GRP1_IsEnabledClockLowPower\n
  *         AHB4LPENR     GPIOGLPEN      LL_AHB4_GRP1_IsEnabledClockLowPower\n
  *         AHB4LPENR     GPIOHLPEN      LL_AHB4_GRP1_IsEnabledClockLowPower\n
  *         AHB4LPENR     GPIONLPEN      LL_AHB4_GRP1_IsEnabledClockLowPower\n
  *         AHB4LPENR     GPIOOLPEN      LL_AHB4_GRP1_IsEnabledClockLowPower\n
  *         AHB4LPENR     GPIOPLPEN      LL_AHB4_GRP1_IsEnabledClockLowPower\n
  *         AHB4LPENR     GPIOQLPEN      LL_AHB4_GRP1_IsEnabledClockLowPower\n
  *         AHB4LPENR     PWRLPEN        LL_AHB4_GRP1_IsEnabledClockLowPower\n
  *         AHB4LPENR     CRCLPEN        LL_AHB4_GRP1_IsEnabledClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOE
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOF
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOG
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOH
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPION
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOO
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOP
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOQ
  *         @arg @ref LL_AHB4_GRP1_PERIPH_PWR
  *         @arg @ref LL_AHB4_GRP1_PERIPH_CRC
  * @retval uint32_t
  */
__STATIC_INLINE uint32_t LL_AHB4_GRP1_IsEnabledClockLowPower(uint32_t Periphs)
{
  return ((READ_BIT(RCC->AHB4LPENR, Periphs) == Periphs) ? 1UL : 0UL);
}

/**
  * @brief  Disable AHB4 peripherals clock during Low Power mode.
  * @rmtoll AHB4LPENCR    GPIOALPENC     LL_AHB4_GRP1_DisableClockLowPower\n
  *         AHB4LPENCR    GPIOBLPENC     LL_AHB4_GRP1_DisableClockLowPower\n
  *         AHB4LPENCR    GPIOCLPENC     LL_AHB4_GRP1_DisableClockLowPower\n
  *         AHB4LPENCR    GPIODLPENC     LL_AHB4_GRP1_DisableClockLowPower\n
  *         AHB4LPENCR    GPIOELPENC     LL_AHB4_GRP1_DisableClockLowPower\n
  *         AHB4LPENCR    GPIOFLPENC     LL_AHB4_GRP1_DisableClockLowPower\n
  *         AHB4LPENCR    GPIOGLPENC     LL_AHB4_GRP1_DisableClockLowPower\n
  *         AHB4LPENCR    GPIOHLPENC     LL_AHB4_GRP1_DisableClockLowPower\n
  *         AHB4LPENCR    GPIONLPENC     LL_AHB4_GRP1_DisableClockLowPower\n
  *         AHB4LPENCR    GPIOOLPENC     LL_AHB4_GRP1_DisableClockLowPower\n
  *         AHB4LPENCR    GPIOPLPENC     LL_AHB4_GRP1_DisableClockLowPower\n
  *         AHB4LPENCR    GPIOQLPENC     LL_AHB4_GRP1_DisableClockLowPower\n
  *         AHB4LPENCR    PWRLPENC       LL_AHB4_GRP1_DisableClockLowPower\n
  *         AHB4LPENCR    CRCLPENC       LL_AHB4_GRP1_DisableClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOE
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOF
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOG
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOH
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPION
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOO
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOP
  *         @arg @ref LL_AHB4_GRP1_PERIPH_GPIOQ
  *         @arg @ref LL_AHB4_GRP1_PERIPH_PWR
  *         @arg @ref LL_AHB4_GRP1_PERIPH_CRC
  * @retval None
  */
__STATIC_INLINE void LL_AHB4_GRP1_DisableClockLowPower(uint32_t Periphs)
{
  WRITE_REG(RCC->AHB4LPENCR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_AHB5 AHB5
  * @{
  */

/**
  * @brief  Enable AHB5 peripherals clock.
  * @rmtoll AHB5ENSR     HPDMA1ENS     LL_AHB5_GRP1_EnableClock\n
  *         AHB5ENSR     DMA2DENS      LL_AHB5_GRP1_EnableClock\n
  *         AHB5ENSR     JPEGENS       LL_AHB5_GRP1_EnableClock\n
  *         AHB5ENSR     FMCENS        LL_AHB5_GRP1_EnableClock\n
  *         AHB5ENSR     XSPI1ENS      LL_AHB5_GRP1_EnableClock\n
  *         AHB5ENSR     PSSIENS       LL_AHB5_GRP1_EnableClock\n
  *         AHB5ENSR     SDMMC2ENS     LL_AHB5_GRP1_EnableClock\n
  *         AHB5ENSR     SDMMC1ENS     LL_AHB5_GRP1_EnableClock\n
  *         AHB5ENSR     XSPI2ENS      LL_AHB5_GRP1_EnableClock\n
  *         AHB5ENSR     XSPIMENS      LL_AHB5_GRP1_EnableClock\n
  *         AHB5ENSR     MCE1ENS       LL_AHB5_GRP1_EnableClock\n (*)
  *         AHB5ENSR     MCE2ENS       LL_AHB5_GRP1_EnableClock\n (*)
  *         AHB5ENSR     MCE3ENS       LL_AHB5_GRP1_EnableClock\n (*)
  *         AHB5ENSR     XSPI3ENS      LL_AHB5_GRP1_EnableClock\n
  *         AHB5ENSR     MCE4ENS       LL_AHB5_GRP1_EnableClock\n (*)
  *         AHB5ENSR     GFXMMUENS     LL_AHB5_GRP1_EnableClock\n
  *         AHB5ENSR     GPU2DENS      LL_AHB5_GRP1_EnableClock\n
  *         AHB5ENSR     ETH1MACENS    LL_AHB5_GRP1_EnableClock\n
  *         AHB5ENSR     ETH1TXENS     LL_AHB5_GRP1_EnableClock\n
  *         AHB5ENSR     ETH1RXENS     LL_AHB5_GRP1_EnableClock\n
  *         AHB5ENSR     ETH1ENS       LL_AHB5_GRP1_EnableClock\n
  *         AHB5ENSR     OTG1ENS       LL_AHB5_GRP1_EnableClock\n
  *         AHB5ENSR     OTGPHY1ENS    LL_AHB5_GRP1_EnableClock\n
  *         AHB5ENSR     OTGPHY2ENS    LL_AHB5_GRP1_EnableClock\n
  *         AHB5ENSR     OTG2ENS       LL_AHB5_GRP1_EnableClock\n
  *         AHB5ENSR     CACHEAXIENS   LL_AHB5_GRP1_EnableClock\n
  *         AHB5ENSR     NPUENS        LL_AHB5_GRP1_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB5_GRP1_PERIPH_HPDMA1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_DMA2D
  *         @arg @ref LL_AHB5_GRP1_PERIPH_JPEG
  *         @arg @ref LL_AHB5_GRP1_PERIPH_FMC
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPI1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPI2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPI3
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPIM
  *         @arg @ref LL_AHB5_GRP1_PERIPH_PSSI
  *         @arg @ref LL_AHB5_GRP1_PERIPH_SDMMC1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_SDMMC2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_MCE1 (*)
  *         @arg @ref LL_AHB5_GRP1_PERIPH_MCE2 (*)
  *         @arg @ref LL_AHB5_GRP1_PERIPH_MCE3 (*)
  *         @arg @ref LL_AHB5_GRP1_PERIPH_MCE4 (*)
  *         @arg @ref LL_AHB5_GRP1_PERIPH_GFXMMU
  *         @arg @ref LL_AHB5_GRP1_PERIPH_GPU2D
  *         @arg @ref LL_AHB5_GRP1_PERIPH_ETH1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_ETH1MAC
  *         @arg @ref LL_AHB5_GRP1_PERIPH_ETH1TX
  *         @arg @ref LL_AHB5_GRP1_PERIPH_ETH1RX
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTG1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTG2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTGPHY1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTGPHY2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_CACHEAXI
  *         @arg @ref LL_AHB5_GRP1_PERIPH_NPU
  *
  *         (*) value not defined in all devices.
  * @retval None
  */
__STATIC_INLINE void LL_AHB5_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  WRITE_REG(RCC->AHB5ENSR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_REG(RCC->AHB5ENR);
  (void)tmpreg;
}

/**
  * @brief  Check if AHB5 peripheral clock is enabled or not
  * @rmtoll AHB5ENR      HPDMA1EN      LL_AHB5_GRP1_IsEnabledClock\n
  *         AHB5ENR      DMA2DEN       LL_AHB5_GRP1_IsEnabledClock\n
  *         AHB5ENR      JPEGEN        LL_AHB5_GRP1_IsEnabledClock\n
  *         AHB5ENR      FMCEN         LL_AHB5_GRP1_IsEnabledClock\n
  *         AHB5ENR      XSPI1EN       LL_AHB5_GRP1_IsEnabledClock\n
  *         AHB5ENR      PSSIEN        LL_AHB5_GRP1_IsEnabledClock\n
  *         AHB5ENR      SDMMC2EN      LL_AHB5_GRP1_IsEnabledClock\n
  *         AHB5ENR      SDMMC1EN      LL_AHB5_GRP1_IsEnabledClock\n
  *         AHB5ENR      XSPI2EN       LL_AHB5_GRP1_IsEnabledClock\n
  *         AHB5ENR      XSPIMEN       LL_AHB5_GRP1_IsEnabledClock\n
  *         AHB5ENR      MCE1EN        LL_AHB5_GRP1_IsEnabledClock\n (*)
  *         AHB5ENR      MCE2EN        LL_AHB5_GRP1_IsEnabledClock\n (*)
  *         AHB5ENR      MCE3EN        LL_AHB5_GRP1_IsEnabledClock\n (*)
  *         AHB5ENR      XSPI3EN       LL_AHB5_GRP1_IsEnabledClock\n
  *         AHB5ENR      MCE4EN        LL_AHB5_GRP1_IsEnabledClock\n (*)
  *         AHB5ENR      GFXMMUEN      LL_AHB5_GRP1_IsEnabledClock\n
  *         AHB5ENR      GPU2DEN       LL_AHB5_GRP1_IsEnabledClock\n
  *         AHB5ENR      ETH1MACEN     LL_AHB5_GRP1_IsEnabledClock\n
  *         AHB5ENR      ETH1TXEN      LL_AHB5_GRP1_IsEnabledClock\n
  *         AHB5ENR      ETH1RXEN      LL_AHB5_GRP1_IsEnabledClock\n
  *         AHB5ENR      ETH1EN        LL_AHB5_GRP1_IsEnabledClock\n
  *         AHB5ENR      OTG1EN        LL_AHB5_GRP1_IsEnabledClock\n
  *         AHB5ENR      OTGPHY1EN     LL_AHB5_GRP1_IsEnabledClock\n
  *         AHB5ENR      OTGPHY2EN     LL_AHB5_GRP1_IsEnabledClock\n
  *         AHB5ENR      OTG2EN        LL_AHB5_GRP1_IsEnabledClock\n
  *         AHB5ENR      CACHEAXIEN    LL_AHB5_GRP1_IsEnabledClock\n
  *         AHB5ENR      NPUEN         LL_AHB5_GRP1_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB5_GRP1_PERIPH_HPDMA1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_DMA2D
  *         @arg @ref LL_AHB5_GRP1_PERIPH_JPEG
  *         @arg @ref LL_AHB5_GRP1_PERIPH_FMC
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPI1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPI2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPI3
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPIM
  *         @arg @ref LL_AHB5_GRP1_PERIPH_PSSI
  *         @arg @ref LL_AHB5_GRP1_PERIPH_SDMMC1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_SDMMC2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_MCE1 (*)
  *         @arg @ref LL_AHB5_GRP1_PERIPH_MCE2 (*)
  *         @arg @ref LL_AHB5_GRP1_PERIPH_MCE3 (*)
  *         @arg @ref LL_AHB5_GRP1_PERIPH_MCE4 (*)
  *         @arg @ref LL_AHB5_GRP1_PERIPH_GFXMMU
  *         @arg @ref LL_AHB5_GRP1_PERIPH_GPU2D
  *         @arg @ref LL_AHB5_GRP1_PERIPH_ETH1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_ETH1MAC
  *         @arg @ref LL_AHB5_GRP1_PERIPH_ETH1TX
  *         @arg @ref LL_AHB5_GRP1_PERIPH_ETH1RX
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTG1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTG2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTGPHY1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTGPHY2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_CACHEAXI
  *         @arg @ref LL_AHB5_GRP1_PERIPH_NPU
  *
  *         (*) value not defined in all devices.
  * @retval uint32_t
  */
__STATIC_INLINE uint32_t LL_AHB5_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC->AHB5ENR, Periphs) == Periphs) ? 1UL : 0UL);
}

/**
  * @brief  Disable AHB5 peripherals clock.
  * @rmtoll AHB5ENCR     HPDMA1ENC     LL_AHB5_GRP1_DisableClock\n
  *         AHB5ENCR     DMA2DENC      LL_AHB5_GRP1_DisableClock\n
  *         AHB5ENCR     JPEGENC       LL_AHB5_GRP1_DisableClock\n
  *         AHB5ENCR     FMCENC        LL_AHB5_GRP1_DisableClock\n
  *         AHB5ENCR     XSPI1ENC      LL_AHB5_GRP1_DisableClock\n
  *         AHB5ENCR     PSSIENC       LL_AHB5_GRP1_DisableClock\n
  *         AHB5ENCR     SDMMC2ENC     LL_AHB5_GRP1_DisableClock\n
  *         AHB5ENCR     SDMMC1ENC     LL_AHB5_GRP1_DisableClock\n
  *         AHB5ENCR     XSPI2ENC      LL_AHB5_GRP1_DisableClock\n
  *         AHB5ENCR     XSPIMENC      LL_AHB5_GRP1_DisableClock\n
  *         AHB5ENCR     MCE1ENC       LL_AHB5_GRP1_DisableClock\n (*)
  *         AHB5ENCR     MCE2ENC       LL_AHB5_GRP1_DisableClock\n (*)
  *         AHB5ENCR     MCE3ENC       LL_AHB5_GRP1_DisableClock\n (*)
  *         AHB5ENCR     XSPI3ENC      LL_AHB5_GRP1_DisableClock\n
  *         AHB5ENCR     MCE4ENC       LL_AHB5_GRP1_DisableClock\n (*)
  *         AHB5ENCR     GFXMMUENC     LL_AHB5_GRP1_DisableClock\n
  *         AHB5ENCR     GPU2DENC      LL_AHB5_GRP1_DisableClock\n
  *         AHB5ENCR     ETH1MACENC    LL_AHB5_GRP1_DisableClock\n
  *         AHB5ENCR     ETH1TXENC     LL_AHB5_GRP1_DisableClock\n
  *         AHB5ENCR     ETH1RXENC     LL_AHB5_GRP1_DisableClock\n
  *         AHB5ENCR     ETH1ENC       LL_AHB5_GRP1_DisableClock\n
  *         AHB5ENCR     OTG1ENC       LL_AHB5_GRP1_DisableClock\n
  *         AHB5ENCR     OTGPHY1ENC    LL_AHB5_GRP1_DisableClock\n
  *         AHB5ENCR     OTGPHY2ENC    LL_AHB5_GRP1_DisableClock\n
  *         AHB5ENCR     OTG2ENC       LL_AHB5_GRP1_DisableClock\n
  *         AHB5ENCR     CACHEAXIENC   LL_AHB5_GRP1_DisableClock\n
  *         AHB5ENCR     NPUENC        LL_AHB5_GRP1_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB5_GRP1_PERIPH_HPDMA1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_DMA2D
  *         @arg @ref LL_AHB5_GRP1_PERIPH_JPEG
  *         @arg @ref LL_AHB5_GRP1_PERIPH_FMC
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPI1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPI2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPI3
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPIM
  *         @arg @ref LL_AHB5_GRP1_PERIPH_PSSI
  *         @arg @ref LL_AHB5_GRP1_PERIPH_SDMMC1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_SDMMC2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_MCE1 (*)
  *         @arg @ref LL_AHB5_GRP1_PERIPH_MCE2 (*)
  *         @arg @ref LL_AHB5_GRP1_PERIPH_MCE3 (*)
  *         @arg @ref LL_AHB5_GRP1_PERIPH_MCE4 (*)
  *         @arg @ref LL_AHB5_GRP1_PERIPH_GFXMMU
  *         @arg @ref LL_AHB5_GRP1_PERIPH_GPU2D
  *         @arg @ref LL_AHB5_GRP1_PERIPH_ETH1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_ETH1MAC
  *         @arg @ref LL_AHB5_GRP1_PERIPH_ETH1TX
  *         @arg @ref LL_AHB5_GRP1_PERIPH_ETH1RX
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTG1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTG2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTGPHY1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTGPHY2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_CACHEAXI
  *         @arg @ref LL_AHB5_GRP1_PERIPH_NPU
  *
  *         (*) value not defined in all devices.
  * @retval None
  */
__STATIC_INLINE void LL_AHB5_GRP1_DisableClock(uint32_t Periphs)
{
  WRITE_REG(RCC->AHB5ENCR, Periphs);
}

/**
  * @brief  Force AHB5 peripherals reset.
  * @rmtoll AHB5RSTSR    HPDMA1RSTS      LL_AHB5_GRP1_ForceReset\n
  *         AHB5RSTSR    DMA2DRSTS       LL_AHB5_GRP1_ForceReset\n
  *         AHB5RSTSR    JPEGRSTS        LL_AHB5_GRP1_ForceReset\n
  *         AHB5RSTSR    FMCRSTS         LL_AHB5_GRP1_ForceReset\n
  *         AHB5RSTSR    XSPI1RSTS       LL_AHB5_GRP1_ForceReset\n
  *         AHB5RSTSR    PSSIRSTS        LL_AHB5_GRP1_ForceReset\n
  *         AHB5RSTSR    SDMMC2RSTS      LL_AHB5_GRP1_ForceReset\n
  *         AHB5RSTSR    SDMMC1RSTS      LL_AHB5_GRP1_ForceReset\n
  *         AHB5RSTSR    XSPI2RSTS       LL_AHB5_GRP1_ForceReset\n
  *         AHB5RSTSR    XSPIMRSTS       LL_AHB5_GRP1_ForceReset\n
  *         AHB5RSTSR    XSPI3RSTS       LL_AHB5_GRP1_ForceReset\n
  *         AHB5RSTSR    GFXMMURSTS      LL_AHB5_GRP1_ForceReset\n
  *         AHB5RSTSR    GPU2DRSTS       LL_AHB5_GRP1_ForceReset\n
  *         AHB5RSTSR    OTG1PHYCTLRSTS  LL_AHB5_GRP1_ForceReset\n
  *         AHB5RSTSR    OTG2PHYCTLRSTS  LL_AHB5_GRP1_ForceReset\n
  *         AHB5RSTSR    ETH1RSTS        LL_AHB5_GRP1_ForceReset\n
  *         AHB5RSTSR    OTG1RSTS        LL_AHB5_GRP1_ForceReset\n
  *         AHB5RSTSR    OTGPHY1RSTS     LL_AHB5_GRP1_ForceReset\n
  *         AHB5RSTSR    OTGPHY2RSTS     LL_AHB5_GRP1_ForceReset\n
  *         AHB5RSTSR    OTG2RSTS        LL_AHB5_GRP1_ForceReset\n
  *         AHB5RSTSR    CACHEAXIRSTS    LL_AHB5_GRP1_ForceReset\n
  *         AHB5RSTSR    NPURSTS         LL_AHB5_GRP1_ForceReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB5_GRP1_PERIPH_HPDMA1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_DMA2D
  *         @arg @ref LL_AHB5_GRP1_PERIPH_JPEG
  *         @arg @ref LL_AHB5_GRP1_PERIPH_FMC
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPI1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPI2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPI3
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPIM
  *         @arg @ref LL_AHB5_GRP1_PERIPH_PSSI
  *         @arg @ref LL_AHB5_GRP1_PERIPH_SDMMC1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_SDMMC2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_GFXMMU
  *         @arg @ref LL_AHB5_GRP1_PERIPH_GPU2D
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTG1PHYCTL
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTG2PHYCTL
  *         @arg @ref LL_AHB5_GRP1_PERIPH_ETH1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTG1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTG2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTGPHY1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTGPHY2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_CACHEAXI
  *         @arg @ref LL_AHB5_GRP1_PERIPH_NPU
  * @retval None
  */
__STATIC_INLINE void LL_AHB5_GRP1_ForceReset(uint32_t Periphs)
{
  WRITE_REG(RCC->AHB5RSTSR, Periphs);
}

/**
  * @brief  Release AHB5 peripherals reset.
  * @rmtoll AHB5RSTCR    HPDMA1RSTC      LL_AHB5_GRP1_ReleaseReset\n
  *         AHB5RSTCR    DMA2DRSTC       LL_AHB5_GRP1_ReleaseReset\n
  *         AHB5RSTCR    JPEGRSTC        LL_AHB5_GRP1_ReleaseReset\n
  *         AHB5RSTCR    FMCRSTC         LL_AHB5_GRP1_ReleaseReset\n
  *         AHB5RSTCR    XSPI1RSTC       LL_AHB5_GRP1_ReleaseReset\n
  *         AHB5RSTCR    PSSIRSTC        LL_AHB5_GRP1_ReleaseReset\n
  *         AHB5RSTCR    SDMMC2RSTC      LL_AHB5_GRP1_ReleaseReset\n
  *         AHB5RSTCR    SDMMC1RSTC      LL_AHB5_GRP1_ReleaseReset\n
  *         AHB5RSTCR    XSPI2RSTC       LL_AHB5_GRP1_ReleaseReset\n
  *         AHB5RSTCR    XSPIMRSTC       LL_AHB5_GRP1_ReleaseReset\n
  *         AHB5RSTCR    XSPI3RSTC       LL_AHB5_GRP1_ReleaseReset\n
  *         AHB5RSTCR    GFXMMURSTC      LL_AHB5_GRP1_ReleaseReset\n
  *         AHB5RSTCR    GPU2DRSTC       LL_AHB5_GRP1_ReleaseReset\n
  *         AHB5RSTCR    OTG1PHYCTLRSTC  LL_AHB5_GRP1_ReleaseReset\n
  *         AHB5RSTCR    OTG2PHYCTLRSTC  LL_AHB5_GRP1_ReleaseReset\n
  *         AHB5RSTCR    ETH1RSTC        LL_AHB5_GRP1_ReleaseReset\n
  *         AHB5RSTCR    OTG1RSTC        LL_AHB5_GRP1_ReleaseReset\n
  *         AHB5RSTCR    OTGPHY1RSTC     LL_AHB5_GRP1_ReleaseReset\n
  *         AHB5RSTCR    OTGPHY2RSTC     LL_AHB5_GRP1_ReleaseReset\n
  *         AHB5RSTCR    OTG2RSTC        LL_AHB5_GRP1_ReleaseReset\n
  *         AHB5RSTCR    CACHEAXIRSTC    LL_AHB5_GRP1_ReleaseReset\n
  *         AHB5RSTCR    NPURSTS         LL_AHB5_GRP1_ReleaseReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB5_GRP1_PERIPH_HPDMA1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_DMA2D
  *         @arg @ref LL_AHB5_GRP1_PERIPH_JPEG
  *         @arg @ref LL_AHB5_GRP1_PERIPH_FMC
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPI1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPI2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPI3
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPIM
  *         @arg @ref LL_AHB5_GRP1_PERIPH_PSSI
  *         @arg @ref LL_AHB5_GRP1_PERIPH_SDMMC1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_SDMMC2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_GFXMMU
  *         @arg @ref LL_AHB5_GRP1_PERIPH_GPU2D
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTG1PHYCTL
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTG2PHYCTL
  *         @arg @ref LL_AHB5_GRP1_PERIPH_ETH1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTG1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTG2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTGPHY1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTGPHY2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_CACHEAXI
  *         @arg @ref LL_AHB5_GRP1_PERIPH_NPU
  * @retval None
  */
__STATIC_INLINE void LL_AHB5_GRP1_ReleaseReset(uint32_t Periphs)
{
  WRITE_REG(RCC->AHB5RSTCR, Periphs);
}

/**
  * @brief  Enable AHB5 peripherals clock during Low Power mode.
  * @rmtoll AHB5LPENSR   HPDMA1LPENS   LL_AHB5_GRP1_EnableClockLowPower\n
  *         AHB5LPENSR   DMA2DLPENS    LL_AHB5_GRP1_EnableClockLowPower\n
  *         AHB5LPENSR   JPEGLPENS     LL_AHB5_GRP1_EnableClockLowPower\n
  *         AHB5LPENSR   FMCLPENS      LL_AHB5_GRP1_EnableClockLowPower\n
  *         AHB5LPENSR   XSPI1LPENS    LL_AHB5_GRP1_EnableClockLowPower\n
  *         AHB5LPENSR   PSSILPENS     LL_AHB5_GRP1_EnableClockLowPower\n
  *         AHB5LPENSR   SDMMC2LPENS   LL_AHB5_GRP1_EnableClockLowPower\n
  *         AHB5LPENSR   SDMMC1LPENS   LL_AHB5_GRP1_EnableClockLowPower\n
  *         AHB5LPENSR   XSPI2LPENS    LL_AHB5_GRP1_EnableClockLowPower\n
  *         AHB5LPENSR   XSPIMLPENS    LL_AHB5_GRP1_EnableClockLowPower\n
  *         AHB5LPENSR   MCE1LPENS     LL_AHB5_GRP1_EnableClockLowPower\n (*)
  *         AHB5LPENSR   MCE2LPENS     LL_AHB5_GRP1_EnableClockLowPower\n (*)
  *         AHB5LPENSR   MCE3LPENS     LL_AHB5_GRP1_EnableClockLowPower\n (*)
  *         AHB5LPENSR   XSPI3LPENS    LL_AHB5_GRP1_EnableClockLowPower\n
  *         AHB5LPENSR   MCE4LPENS     LL_AHB5_GRP1_EnableClockLowPower\n (*)
  *         AHB5LPENSR   GFXMMULPENS   LL_AHB5_GRP1_EnableClockLowPower\n
  *         AHB5LPENSR   GPU2DLPENS    LL_AHB5_GRP1_EnableClockLowPower\n
  *         AHB5LPENSR   ETH1MACLPENS  LL_AHB5_GRP1_EnableClockLowPower\n
  *         AHB5LPENSR   ETH1TXLPENS   LL_AHB5_GRP1_EnableClockLowPower\n
  *         AHB5LPENSR   ETH1RXLPENS   LL_AHB5_GRP1_EnableClockLowPower\n
  *         AHB5LPENSR   ETH1LPENS     LL_AHB5_GRP1_EnableClockLowPower\n
  *         AHB5LPENSR   OTG1LPENS     LL_AHB5_GRP1_EnableClockLowPower\n
  *         AHB5LPENSR   OTGPHY1LPENS  LL_AHB5_GRP1_EnableClockLowPower\n
  *         AHB5LPENSR   OTGPHY2LPENS  LL_AHB5_GRP1_EnableClockLowPower\n
  *         AHB5LPENSR   OTG2LPENS     LL_AHB5_GRP1_EnableClockLowPower\n
  *         AHB5LPENSR   CACHEAXILPENS LL_AHB5_GRP1_EnableClockLowPower\n
  *         AHB5LPENSR   NPULPENS      LL_AHB5_GRP1_EnableClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB5_GRP1_PERIPH_HPDMA1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_DMA2D
  *         @arg @ref LL_AHB5_GRP1_PERIPH_JPEG
  *         @arg @ref LL_AHB5_GRP1_PERIPH_FMC
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPI1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPI2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPI3
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPIM
  *         @arg @ref LL_AHB5_GRP1_PERIPH_PSSI
  *         @arg @ref LL_AHB5_GRP1_PERIPH_SDMMC1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_SDMMC2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_MCE1 (*)
  *         @arg @ref LL_AHB5_GRP1_PERIPH_MCE2 (*)
  *         @arg @ref LL_AHB5_GRP1_PERIPH_MCE3 (*)
  *         @arg @ref LL_AHB5_GRP1_PERIPH_MCE4 (*)
  *         @arg @ref LL_AHB5_GRP1_PERIPH_GFXMMU
  *         @arg @ref LL_AHB5_GRP1_PERIPH_GPU2D
  *         @arg @ref LL_AHB5_GRP1_PERIPH_ETH1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_ETH1MAC
  *         @arg @ref LL_AHB5_GRP1_PERIPH_ETH1TX
  *         @arg @ref LL_AHB5_GRP1_PERIPH_ETH1RX
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTG1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTG2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTGPHY1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTGPHY2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_CACHEAXI
  *         @arg @ref LL_AHB5_GRP1_PERIPH_NPU
  *
  *         (*) value not defined in all devices.
  * @retval None
  */
__STATIC_INLINE void LL_AHB5_GRP1_EnableClockLowPower(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  WRITE_REG(RCC->AHB5LPENSR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_REG(RCC->AHB5LPENR);
  (void)tmpreg;
}

/**
  * @brief  Check if AHB5 peripheral clock during Low Power mode is enabled or not .
  * @rmtoll AHB5LPENR    HPDMA1LPEN    LL_AHB5_GRP1_IsEnabledClockLowPower\n
  *         AHB5LPENR    DMA2DLPEN     LL_AHB5_GRP1_IsEnabledClockLowPower\n
  *         AHB5LPENR    JPEGLPEN      LL_AHB5_GRP1_IsEnabledClockLowPower\n
  *         AHB5LPENR    FMCLPEN       LL_AHB5_GRP1_IsEnabledClockLowPower\n
  *         AHB5LPENR    XSPI1LPEN     LL_AHB5_GRP1_IsEnabledClockLowPower\n
  *         AHB5LPENR    PSSILPEN      LL_AHB5_GRP1_IsEnabledClockLowPower\n
  *         AHB5LPENR    SDMMC2LPEN    LL_AHB5_GRP1_IsEnabledClockLowPower\n
  *         AHB5LPENR    SDMMC1LPEN    LL_AHB5_GRP1_IsEnabledClockLowPower\n
  *         AHB5LPENR    XSPI2LPEN     LL_AHB5_GRP1_IsEnabledClockLowPower\n
  *         AHB5LPENR    XSPIMLPEN     LL_AHB5_GRP1_IsEnabledClockLowPower\n
  *         AHB5LPENR    MCE1LPEN      LL_AHB5_GRP1_IsEnabledClockLowPower\n (*)
  *         AHB5LPENR    MCE2LPEN      LL_AHB5_GRP1_IsEnabledClockLowPower\n (*)
  *         AHB5LPENR    MCE3LPEN      LL_AHB5_GRP1_IsEnabledClockLowPower\n (*)
  *         AHB5LPENR    XSPI3LPEN     LL_AHB5_GRP1_IsEnabledClockLowPower\n
  *         AHB5LPENR    MCE4LPEN      LL_AHB5_GRP1_IsEnabledClockLowPower\n (*)
  *         AHB5LPENR    GFXMMULPEN    LL_AHB5_GRP1_IsEnabledClockLowPower\n
  *         AHB5LPENR    GPU2DLPEN     LL_AHB5_GRP1_IsEnabledClockLowPower\n
  *         AHB5LPENR    ETH1MACLPEN   LL_AHB5_GRP1_IsEnabledClockLowPower\n
  *         AHB5LPENR    ETH1TXLPEN    LL_AHB5_GRP1_IsEnabledClockLowPower\n
  *         AHB5LPENR    ETH1RXLPEN    LL_AHB5_GRP1_IsEnabledClockLowPower\n
  *         AHB5LPENR    ETH1LPEN      LL_AHB5_GRP1_IsEnabledClockLowPower\n
  *         AHB5LPENR    OTG1LPEN      LL_AHB5_GRP1_IsEnabledClockLowPower\n
  *         AHB5LPENR    OTGPHY1LPEN   LL_AHB5_GRP1_IsEnabledClockLowPower\n
  *         AHB5LPENR    OTGPHY2LPEN   LL_AHB5_GRP1_IsEnabledClockLowPower\n
  *         AHB5LPENR    OTG2LPEN      LL_AHB5_GRP1_IsEnabledClockLowPower\n
  *         AHB5LPENR    CACHEAXILPEN  LL_AHB5_GRP1_IsEnabledClockLowPower\n
  *         AHB5LPENR    NPULPEN       LL_AHB5_GRP1_IsEnabledClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB5_GRP1_PERIPH_HPDMA1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_DMA2D
  *         @arg @ref LL_AHB5_GRP1_PERIPH_JPEG
  *         @arg @ref LL_AHB5_GRP1_PERIPH_FMC
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPI1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPI2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPI3
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPIM
  *         @arg @ref LL_AHB5_GRP1_PERIPH_PSSI
  *         @arg @ref LL_AHB5_GRP1_PERIPH_SDMMC1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_SDMMC2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_MCE1 (*)
  *         @arg @ref LL_AHB5_GRP1_PERIPH_MCE2 (*)
  *         @arg @ref LL_AHB5_GRP1_PERIPH_MCE3 (*)
  *         @arg @ref LL_AHB5_GRP1_PERIPH_MCE4 (*)
  *         @arg @ref LL_AHB5_GRP1_PERIPH_GFXMMU
  *         @arg @ref LL_AHB5_GRP1_PERIPH_GPU2D
  *         @arg @ref LL_AHB5_GRP1_PERIPH_ETH1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_ETH1MAC
  *         @arg @ref LL_AHB5_GRP1_PERIPH_ETH1TX
  *         @arg @ref LL_AHB5_GRP1_PERIPH_ETH1RX
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTG1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTG2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTGPHY1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTGPHY2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_CACHEAXI
  *         @arg @ref LL_AHB5_GRP1_PERIPH_NPU
  *
  *         (*) value not defined in all devices.
  * @retval uint32_t
  */
__STATIC_INLINE uint32_t LL_AHB5_GRP1_IsEnabledClockLowPower(uint32_t Periphs)
{
  return ((READ_BIT(RCC->AHB5LPENR, Periphs) == Periphs) ? 1UL : 0UL);
}

/**
  * @brief  Disable AHB5 peripherals clock during Low Power mode.
  * @rmtoll AHB5LPENCR   HPDMA1LPENC   LL_AHB5_GRP1_DisableClockLowPower\n
  *         AHB5LPENCR   DMA2DLPENC    LL_AHB5_GRP1_DisableClockLowPower\n
  *         AHB5LPENCR   JPEGLPENC     LL_AHB5_GRP1_DisableClockLowPower\n
  *         AHB5LPENCR   FMCLPENC      LL_AHB5_GRP1_DisableClockLowPower\n
  *         AHB5LPENCR   XSPI1LPENC    LL_AHB5_GRP1_DisableClockLowPower\n
  *         AHB5LPENCR   PSSILPENC     LL_AHB5_GRP1_DisableClockLowPower\n
  *         AHB5LPENCR   SDMMC2LPENC   LL_AHB5_GRP1_DisableClockLowPower\n
  *         AHB5LPENCR   SDMMC1LPENC   LL_AHB5_GRP1_DisableClockLowPower\n
  *         AHB5LPENCR   XSPI2LPENC    LL_AHB5_GRP1_DisableClockLowPower\n
  *         AHB5LPENCR   XSPIMLPENC    LL_AHB5_GRP1_DisableClockLowPower\n
  *         AHB5LPENCR   MCE1LPENC     LL_AHB5_GRP1_DisableClockLowPower\n (*)
  *         AHB5LPENCR   MCE2LPENC     LL_AHB5_GRP1_DisableClockLowPower\n (*)
  *         AHB5LPENCR   MCE3LPENC     LL_AHB5_GRP1_DisableClockLowPower\n (*)
  *         AHB5LPENCR   XSPI3LPENC    LL_AHB5_GRP1_DisableClockLowPower\n
  *         AHB5LPENCR   MCE4LPENC     LL_AHB5_GRP1_DisableClockLowPower\n (*)
  *         AHB5LPENCR   GFXMMULPENC   LL_AHB5_GRP1_DisableClockLowPower\n
  *         AHB5LPENCR   GPU2DLPENC    LL_AHB5_GRP1_DisableClockLowPower\n
  *         AHB5LPENCR   ETH1MACLPENC  LL_AHB5_GRP1_DisableClockLowPower\n
  *         AHB5LPENCR   ETH1TXLPENC   LL_AHB5_GRP1_DisableClockLowPower\n
  *         AHB5LPENCR   ETH1RXLPENC   LL_AHB5_GRP1_DisableClockLowPower\n
  *         AHB5LPENCR   ETH1LPENC     LL_AHB5_GRP1_DisableClockLowPower\n
  *         AHB5LPENCR   OTG1LPENC     LL_AHB5_GRP1_DisableClockLowPower\n
  *         AHB5LPENCR   OTGPHY1LPENC  LL_AHB5_GRP1_DisableClockLowPower\n
  *         AHB5LPENCR   OTGPHY2LPENC  LL_AHB5_GRP1_DisableClockLowPower\n
  *         AHB5LPENCR   OTG2LPENC     LL_AHB5_GRP1_DisableClockLowPower\n
  *         AHB5LPENCR   CACHEAXILPENC LL_AHB5_GRP1_DisableClockLowPower\n
  *         AHB5LPENCR   NPULPENC      LL_AHB5_GRP1_DisableClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB5_GRP1_PERIPH_HPDMA1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_DMA2D
  *         @arg @ref LL_AHB5_GRP1_PERIPH_JPEG
  *         @arg @ref LL_AHB5_GRP1_PERIPH_FMC
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPI1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPI2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPI3
  *         @arg @ref LL_AHB5_GRP1_PERIPH_XSPIM
  *         @arg @ref LL_AHB5_GRP1_PERIPH_PSSI
  *         @arg @ref LL_AHB5_GRP1_PERIPH_SDMMC1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_SDMMC2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_MCE1 (*)
  *         @arg @ref LL_AHB5_GRP1_PERIPH_MCE2 (*)
  *         @arg @ref LL_AHB5_GRP1_PERIPH_MCE3 (*)
  *         @arg @ref LL_AHB5_GRP1_PERIPH_MCE4 (*)
  *         @arg @ref LL_AHB5_GRP1_PERIPH_GFXMMU
  *         @arg @ref LL_AHB5_GRP1_PERIPH_GPU2D
  *         @arg @ref LL_AHB5_GRP1_PERIPH_ETH1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_ETH1MAC
  *         @arg @ref LL_AHB5_GRP1_PERIPH_ETH1TX
  *         @arg @ref LL_AHB5_GRP1_PERIPH_ETH1RX
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTG1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTG2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTGPHY1
  *         @arg @ref LL_AHB5_GRP1_PERIPH_OTGPHY2
  *         @arg @ref LL_AHB5_GRP1_PERIPH_CACHEAXI
  *         @arg @ref LL_AHB5_GRP1_PERIPH_NPU
  *
  *         (*) value not defined in all devices.
  * @retval None
  */
__STATIC_INLINE void LL_AHB5_GRP1_DisableClockLowPower(uint32_t Periphs)
{
  WRITE_REG(RCC->AHB5LPENCR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_APB1 APB1
  * @{
  */

/**
  * @brief  Enable APB1 peripherals clock.
  * @rmtoll APB1ENSR1    TIM2ENS       LL_APB1_GRP1_EnableClock\n
  *         APB1ENSR1    TIM3ENS       LL_APB1_GRP1_EnableClock\n
  *         APB1ENSR1    TIM4ENS       LL_APB1_GRP1_EnableClock\n
  *         APB1ENSR1    TIM5ENS       LL_APB1_GRP1_EnableClock\n
  *         APB1ENSR1    TIM6ENS       LL_APB1_GRP1_EnableClock\n
  *         APB1ENSR1    TIM7ENS       LL_APB1_GRP1_EnableClock\n
  *         APB1ENSR1    TIM12ENS      LL_APB1_GRP1_EnableClock\n
  *         APB1ENSR1    TIM13ENS      LL_APB1_GRP1_EnableClock\n
  *         APB1ENSR1    TIM14ENS      LL_APB1_GRP1_EnableClock\n
  *         APB1ENSR1    LPTIM1ENS     LL_APB1_GRP1_EnableClock\n
  *         APB1ENSR1    WWDGENS       LL_APB1_GRP1_EnableClock\n
  *         APB1ENSR1    TIM10ENS      LL_APB1_GRP1_EnableClock\n
  *         APB1ENSR1    TIM11ENS      LL_APB1_GRP1_EnableClock\n
  *         APB1ENSR1    SPI2ENS       LL_APB1_GRP1_EnableClock\n
  *         APB1ENSR1    SPI3ENS       LL_APB1_GRP1_EnableClock\n
  *         APB1ENSR1    SPDIFRX1ENS   LL_APB1_GRP1_EnableClock\n
  *         APB1ENSR1    USART2ENS     LL_APB1_GRP1_EnableClock\n
  *         APB1ENSR1    USART3ENS     LL_APB1_GRP1_EnableClock\n
  *         APB1ENSR1    UART4ENS      LL_APB1_GRP1_EnableClock\n
  *         APB1ENSR1    UART5ENS      LL_APB1_GRP1_EnableClock\n
  *         APB1ENSR1    I2C1ENS       LL_APB1_GRP1_EnableClock\n
  *         APB1ENSR1    I2C2ENS       LL_APB1_GRP1_EnableClock\n
  *         APB1ENSR1    I2C3ENS       LL_APB1_GRP1_EnableClock\n
  *         APB1ENSR1    I3C1ENS       LL_APB1_GRP1_EnableClock\n
  *         APB1ENSR1    I3C2ENS       LL_APB1_GRP1_EnableClock\n
  *         APB1ENSR1    UART7ENS      LL_APB1_GRP1_EnableClock\n
  *         APB1ENSR1    UART8ENS      LL_APB1_GRP1_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C3
  *         @arg @ref LL_APB1_GRP1_PERIPH_I3C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I3C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_LPTIM1
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPDIFRX1
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM10
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM11
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART7
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART8
  *         @arg @ref LL_APB1_GRP1_PERIPH_WWDG
  * @retval None
  */
__STATIC_INLINE void LL_APB1_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  WRITE_REG(RCC->APB1ENSR1, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_REG(RCC->APB1ENR1);
  (void)tmpreg;
}

/**
  * @brief  Check if APB1 peripheral clock is enabled or not
  * @rmtoll APB1ENR1     TIM2EN        LL_APB1_GRP1_IsEnabledClock\n
  *         APB1ENR1     TIM3EN        LL_APB1_GRP1_IsEnabledClock\n
  *         APB1ENR1     TIM4EN        LL_APB1_GRP1_IsEnabledClock\n
  *         APB1ENR1     TIM5EN        LL_APB1_GRP1_IsEnabledClock\n
  *         APB1ENR1     TIM6EN        LL_APB1_GRP1_IsEnabledClock\n
  *         APB1ENR1     TIM7EN        LL_APB1_GRP1_IsEnabledClock\n
  *         APB1ENR1     TIM12EN       LL_APB1_GRP1_IsEnabledClock\n
  *         APB1ENR1     TIM13EN       LL_APB1_GRP1_IsEnabledClock\n
  *         APB1ENR1     TIM14EN       LL_APB1_GRP1_IsEnabledClock\n
  *         APB1ENR1     LPTIM1EN      LL_APB1_GRP1_IsEnabledClock\n
  *         APB1ENR1     WWDGEN        LL_APB1_GRP1_IsEnabledClock\n
  *         APB1ENR1     TIM10EN       LL_APB1_GRP1_IsEnabledClock\n
  *         APB1ENR1     TIM11EN       LL_APB1_GRP1_IsEnabledClock\n
  *         APB1ENR1     SPI2EN        LL_APB1_GRP1_IsEnabledClock\n
  *         APB1ENR1     SPI3EN        LL_APB1_GRP1_IsEnabledClock\n
  *         APB1ENR1     SPDIFRX1EN    LL_APB1_GRP1_IsEnabledClock\n
  *         APB1ENR1     USART2EN      LL_APB1_GRP1_IsEnabledClock\n
  *         APB1ENR1     USART3EN      LL_APB1_GRP1_IsEnabledClock\n
  *         APB1ENR1     UART4EN       LL_APB1_GRP1_IsEnabledClock\n
  *         APB1ENR1     UART5EN       LL_APB1_GRP1_IsEnabledClock\n
  *         APB1ENR1     I2C1EN        LL_APB1_GRP1_IsEnabledClock\n
  *         APB1ENR1     I2C2EN        LL_APB1_GRP1_IsEnabledClock\n
  *         APB1ENR1     I2C3EN        LL_APB1_GRP1_IsEnabledClock\n
  *         APB1ENR1     I3C1EN        LL_APB1_GRP1_IsEnabledClock\n
  *         APB1ENR1     I3C2EN        LL_APB1_GRP1_IsEnabledClock\n
  *         APB1ENR1     UART7EN       LL_APB1_GRP1_IsEnabledClock\n
  *         APB1ENR1     UART8EN       LL_APB1_GRP1_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C3
  *         @arg @ref LL_APB1_GRP1_PERIPH_I3C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I3C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_LPTIM1
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPDIFRX1
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM10
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM11
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART7
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART8
  *         @arg @ref LL_APB1_GRP1_PERIPH_WWDG
  * @retval uint32_t
  */
__STATIC_INLINE uint32_t LL_APB1_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC->APB1ENR1, Periphs) == Periphs) ? 1UL : 0UL);
}

/**
  * @brief  Disable APB1 peripherals clock.
  * @rmtoll APB1ENCR1    TIM2ENC       LL_APB1_GRP1_DisableClock\n
  *         APB1ENCR1    TIM3ENC       LL_APB1_GRP1_DisableClock\n
  *         APB1ENCR1    TIM4ENC       LL_APB1_GRP1_DisableClock\n
  *         APB1ENCR1    TIM5ENC       LL_APB1_GRP1_DisableClock\n
  *         APB1ENCR1    TIM6ENC       LL_APB1_GRP1_DisableClock\n
  *         APB1ENCR1    TIM7ENC       LL_APB1_GRP1_DisableClock\n
  *         APB1ENCR1    TIM12ENC      LL_APB1_GRP1_DisableClock\n
  *         APB1ENCR1    TIM13ENC      LL_APB1_GRP1_DisableClock\n
  *         APB1ENCR1    TIM14ENC      LL_APB1_GRP1_DisableClock\n
  *         APB1ENCR1    LPTIM1ENC     LL_APB1_GRP1_DisableClock\n
  *         APB1ENCR1    TIM10ENC      LL_APB1_GRP1_DisableClock\n
  *         APB1ENCR1    TIM11ENC      LL_APB1_GRP1_DisableClock\n
  *         APB1ENCR1    SPI2ENC       LL_APB1_GRP1_DisableClock\n
  *         APB1ENCR1    SPI3ENC       LL_APB1_GRP1_DisableClock\n
  *         APB1ENCR1    SPDIFRX1ENC   LL_APB1_GRP1_DisableClock\n
  *         APB1ENCR1    USART2ENC     LL_APB1_GRP1_DisableClock\n
  *         APB1ENCR1    USART3ENC     LL_APB1_GRP1_DisableClock\n
  *         APB1ENCR1    UART4ENC      LL_APB1_GRP1_DisableClock\n
  *         APB1ENCR1    UART5ENC      LL_APB1_GRP1_DisableClock\n
  *         APB1ENCR1    I2C1ENC       LL_APB1_GRP1_DisableClock\n
  *         APB1ENCR1    I2C2ENC       LL_APB1_GRP1_DisableClock\n
  *         APB1ENCR1    I2C3ENC       LL_APB1_GRP1_DisableClock\n
  *         APB1ENCR1    I3C1ENC       LL_APB1_GRP1_DisableClock\n
  *         APB1ENCR1    I3C2ENC       LL_APB1_GRP1_DisableClock\n
  *         APB1ENCR1    UART7ENC      LL_APB1_GRP1_DisableClock\n
  *         APB1ENCR1    UART8ENC      LL_APB1_GRP1_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C3
  *         @arg @ref LL_APB1_GRP1_PERIPH_I3C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I3C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_LPTIM1
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPDIFRX1
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM10
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM11
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART7
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART8
  * @retval None
  */
__STATIC_INLINE void LL_APB1_GRP1_DisableClock(uint32_t Periphs)
{
  WRITE_REG(RCC->APB1ENCR1, Periphs);
}

/**
  * @brief  Force APB1 peripherals reset.
  * @rmtoll APB1RSTSR1   TIM2RSTS      LL_APB1_GRP1_ForceReset\n
  *         APB1RSTSR1   TIM3RSTS      LL_APB1_GRP1_ForceReset\n
  *         APB1RSTSR1   TIM4RSTS      LL_APB1_GRP1_ForceReset\n
  *         APB1RSTSR1   TIM5RSTS      LL_APB1_GRP1_ForceReset\n
  *         APB1RSTSR1   TIM6RSTS      LL_APB1_GRP1_ForceReset\n
  *         APB1RSTSR1   TIM7RSTS      LL_APB1_GRP1_ForceReset\n
  *         APB1RSTSR1   TIM12RSTS     LL_APB1_GRP1_ForceReset\n
  *         APB1RSTSR1   TIM13RSTS     LL_APB1_GRP1_ForceReset\n
  *         APB1RSTSR1   TIM14RSTS     LL_APB1_GRP1_ForceReset\n
  *         APB1RSTSR1   LPTIM1RSTS    LL_APB1_GRP1_ForceReset\n
  *         APB1RSTSR1   TIM10RSTS     LL_APB1_GRP1_ForceReset\n
  *         APB1RSTSR1   TIM11RSTS     LL_APB1_GRP1_ForceReset\n
  *         APB1RSTSR1   SPI2RSTS      LL_APB1_GRP1_ForceReset\n
  *         APB1RSTSR1   SPI3RSTS      LL_APB1_GRP1_ForceReset\n
  *         APB1RSTSR1   SPDIFRX1RSTS  LL_APB1_GRP1_ForceReset\n
  *         APB1RSTSR1   USART2RSTS    LL_APB1_GRP1_ForceReset\n
  *         APB1RSTSR1   USART3RSTS    LL_APB1_GRP1_ForceReset\n
  *         APB1RSTSR1   UART4RSTS     LL_APB1_GRP1_ForceReset\n
  *         APB1RSTSR1   UART5RSTS     LL_APB1_GRP1_ForceReset\n
  *         APB1RSTSR1   I2C1RSTS      LL_APB1_GRP1_ForceReset\n
  *         APB1RSTSR1   I2C2RSTS      LL_APB1_GRP1_ForceReset\n
  *         APB1RSTSR1   I2C3RSTS      LL_APB1_GRP1_ForceReset\n
  *         APB1RSTSR1   I3C1RSTS      LL_APB1_GRP1_ForceReset\n
  *         APB1RSTSR1   I3C2RSTS      LL_APB1_GRP1_ForceReset\n
  *         APB1RSTSR1   UART7RSTS     LL_APB1_GRP1_ForceReset\n
  *         APB1RSTSR1   UART8RSTS     LL_APB1_GRP1_ForceReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C3
  *         @arg @ref LL_APB1_GRP1_PERIPH_I3C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I3C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_LPTIM1
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPDIFRX1
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM10
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM11
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART7
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART8
  * @retval None
  */
__STATIC_INLINE void LL_APB1_GRP1_ForceReset(uint32_t Periphs)
{
  WRITE_REG(RCC->APB1RSTSR1, Periphs);
}

/**
  * @brief  Release APB1 peripherals reset.
  * @rmtoll APB1RSTCR1   TIM2RSTC      LL_APB1_GRP1_ReleaseReset\n
  *         APB1RSTCR1   TIM3RSTC      LL_APB1_GRP1_ReleaseReset\n
  *         APB1RSTCR1   TIM4RSTC      LL_APB1_GRP1_ReleaseReset\n
  *         APB1RSTCR1   TIM5RSTC      LL_APB1_GRP1_ReleaseReset\n
  *         APB1RSTCR1   TIM6RSTC      LL_APB1_GRP1_ReleaseReset\n
  *         APB1RSTCR1   TIM7RSTC      LL_APB1_GRP1_ReleaseReset\n
  *         APB1RSTCR1   TIM12RSTC     LL_APB1_GRP1_ReleaseReset\n
  *         APB1RSTCR1   TIM13RSTC     LL_APB1_GRP1_ReleaseReset\n
  *         APB1RSTCR1   TIM14RSTC     LL_APB1_GRP1_ReleaseReset\n
  *         APB1RSTCR1   LPTIM1RSTC    LL_APB1_GRP1_ReleaseReset\n
  *         APB1RSTCR1   TIM10RSTC     LL_APB1_GRP1_ReleaseReset\n
  *         APB1RSTCR1   TIM11RSTC     LL_APB1_GRP1_ReleaseReset\n
  *         APB1RSTCR1   SPI2RSTC      LL_APB1_GRP1_ReleaseReset\n
  *         APB1RSTCR1   SPI3RSTC      LL_APB1_GRP1_ReleaseReset\n
  *         APB1RSTCR1   SPDIFRX1RSTC  LL_APB1_GRP1_ReleaseReset\n
  *         APB1RSTCR1   USART2RSTC    LL_APB1_GRP1_ReleaseReset\n
  *         APB1RSTCR1   USART3RSTC    LL_APB1_GRP1_ReleaseReset\n
  *         APB1RSTCR1   UART4RSTC     LL_APB1_GRP1_ReleaseReset\n
  *         APB1RSTCR1   UART5RSTC     LL_APB1_GRP1_ReleaseReset\n
  *         APB1RSTCR1   I2C1RSTC      LL_APB1_GRP1_ReleaseReset\n
  *         APB1RSTCR1   I2C2RSTC      LL_APB1_GRP1_ReleaseReset\n
  *         APB1RSTCR1   I2C3RSTC      LL_APB1_GRP1_ReleaseReset\n
  *         APB1RSTCR1   I3C1RSTC      LL_APB1_GRP1_ReleaseReset\n
  *         APB1RSTCR1   I3C2RSTC      LL_APB1_GRP1_ReleaseReset\n
  *         APB1RSTCR1   UART7RSTC     LL_APB1_GRP1_ReleaseReset\n
  *         APB1RSTCR1   UART8RSTC     LL_APB1_GRP1_ReleaseReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C3
  *         @arg @ref LL_APB1_GRP1_PERIPH_I3C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I3C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_LPTIM1
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPDIFRX1
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM10
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM11
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART7
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART8
  * @retval None
  */
__STATIC_INLINE void LL_APB1_GRP1_ReleaseReset(uint32_t Periphs)
{
  WRITE_REG(RCC->APB1RSTCR1, Periphs);
}

/**
  * @brief  Enable APB1 peripherals clock during Low Power mode.
  * @rmtoll APB1LPENSR1  TIM2LPENS     LL_APB1_GRP1_EnableClockLowPower\n
  *         APB1LPENSR1  TIM3LPENS     LL_APB1_GRP1_EnableClockLowPower\n
  *         APB1LPENSR1  TIM4LPENS     LL_APB1_GRP1_EnableClockLowPower\n
  *         APB1LPENSR1  TIM5LPENS     LL_APB1_GRP1_EnableClockLowPower\n
  *         APB1LPENSR1  TIM6LPENS     LL_APB1_GRP1_EnableClockLowPower\n
  *         APB1LPENSR1  TIM7LPENS     LL_APB1_GRP1_EnableClockLowPower\n
  *         APB1LPENSR1  TIM12LPENS    LL_APB1_GRP1_EnableClockLowPower\n
  *         APB1LPENSR1  TIM13LPENS    LL_APB1_GRP1_EnableClockLowPower\n
  *         APB1LPENSR1  TIM14LPENS    LL_APB1_GRP1_EnableClockLowPower\n
  *         APB1LPENSR1  LPTIM1LPENS   LL_APB1_GRP1_EnableClockLowPower\n
  *         APB1LPENSR1  WWDGLPENS     LL_APB1_GRP1_EnableClockLowPower\n
  *         APB1LPENSR1  TIM10LPENS    LL_APB1_GRP1_EnableClockLowPower\n
  *         APB1LPENSR1  TIM11LPENS    LL_APB1_GRP1_EnableClockLowPower\n
  *         APB1LPENSR1  SPI2LPENS     LL_APB1_GRP1_EnableClockLowPower\n
  *         APB1LPENSR1  SPI3LPENS     LL_APB1_GRP1_EnableClockLowPower\n
  *         APB1LPENSR1  SPDIFRX1LPENS LL_APB1_GRP1_EnableClockLowPower\n
  *         APB1LPENSR1  USART2LPENS   LL_APB1_GRP1_EnableClockLowPower\n
  *         APB1LPENSR1  USART3LPENS   LL_APB1_GRP1_EnableClockLowPower\n
  *         APB1LPENSR1  UART4LPENS    LL_APB1_GRP1_EnableClockLowPower\n
  *         APB1LPENSR1  UART5LPENS    LL_APB1_GRP1_EnableClockLowPower\n
  *         APB1LPENSR1  I2C1LPENS     LL_APB1_GRP1_EnableClockLowPower\n
  *         APB1LPENSR1  I2C2LPENS     LL_APB1_GRP1_EnableClockLowPower\n
  *         APB1LPENSR1  I2C3LPENS     LL_APB1_GRP1_EnableClockLowPower\n
  *         APB1LPENSR1  I3C1LPENS     LL_APB1_GRP1_EnableClockLowPower\n
  *         APB1LPENSR1  I3C2LPENS     LL_APB1_GRP1_EnableClockLowPower\n
  *         APB1LPENSR1  UART7LPENS    LL_APB1_GRP1_EnableClockLowPower\n
  *         APB1LPENSR1  UART8LPENS    LL_APB1_GRP1_EnableClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C3
  *         @arg @ref LL_APB1_GRP1_PERIPH_I3C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I3C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_LPTIM1
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPDIFRX1
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM10
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM11
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART7
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART8
  *         @arg @ref LL_APB1_GRP1_PERIPH_WWDG
  * @retval None
  */
__STATIC_INLINE void LL_APB1_GRP1_EnableClockLowPower(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  WRITE_REG(RCC->APB1LPENSR1, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_REG(RCC->APB1LPENR1);
  (void)tmpreg;
}

/**
  * @brief  Check if APB1 peripheral clock during Low Power mode is enabled or not .
  * @rmtoll APB1LPENR1   TIM2LPEN      LL_APB1_GRP1_IsEnabledClockLowPower\n
  *         APB1LPENR1   TIM3LPEN      LL_APB1_GRP1_IsEnabledClockLowPower\n
  *         APB1LPENR1   TIM4LPEN      LL_APB1_GRP1_IsEnabledClockLowPower\n
  *         APB1LPENR1   TIM5LPEN      LL_APB1_GRP1_IsEnabledClockLowPower\n
  *         APB1LPENR1   TIM6LPEN      LL_APB1_GRP1_IsEnabledClockLowPower\n
  *         APB1LPENR1   TIM7LPEN      LL_APB1_GRP1_IsEnabledClockLowPower\n
  *         APB1LPENR1   TIM12LPEN     LL_APB1_GRP1_IsEnabledClockLowPower\n
  *         APB1LPENR1   TIM13LPEN     LL_APB1_GRP1_IsEnabledClockLowPower\n
  *         APB1LPENR1   TIM14LPEN     LL_APB1_GRP1_IsEnabledClockLowPower\n
  *         APB1LPENR1   LPTIM1LPEN    LL_APB1_GRP1_IsEnabledClockLowPower\n
  *         APB1LPENR1   WWDGLPEN      LL_APB1_GRP1_IsEnabledClockLowPower\n
  *         APB1LPENR1   TIM10LPEN     LL_APB1_GRP1_IsEnabledClockLowPower\n
  *         APB1LPENR1   TIM11LPEN     LL_APB1_GRP1_IsEnabledClockLowPower\n
  *         APB1LPENR1   SPI2LPEN      LL_APB1_GRP1_IsEnabledClockLowPower\n
  *         APB1LPENR1   SPI3LPEN      LL_APB1_GRP1_IsEnabledClockLowPower\n
  *         APB1LPENR1   SPDIFRX1LPEN  LL_APB1_GRP1_IsEnabledClockLowPower\n
  *         APB1LPENR1   USART2LPEN    LL_APB1_GRP1_IsEnabledClockLowPower\n
  *         APB1LPENR1   USART3LPEN    LL_APB1_GRP1_IsEnabledClockLowPower\n
  *         APB1LPENR1   UART4LPEN     LL_APB1_GRP1_IsEnabledClockLowPower\n
  *         APB1LPENR1   UART5LPEN     LL_APB1_GRP1_IsEnabledClockLowPower\n
  *         APB1LPENR1   I2C1LPEN      LL_APB1_GRP1_IsEnabledClockLowPower\n
  *         APB1LPENR1   I2C2LPEN      LL_APB1_GRP1_IsEnabledClockLowPower\n
  *         APB1LPENR1   I2C3LPEN      LL_APB1_GRP1_IsEnabledClockLowPower\n
  *         APB1LPENR1   I3C1LPEN      LL_APB1_GRP1_IsEnabledClockLowPower\n
  *         APB1LPENR1   I3C2LPEN      LL_APB1_GRP1_IsEnabledClockLowPower\n
  *         APB1LPENR1   UART7LPEN     LL_APB1_GRP1_IsEnabledClockLowPower\n
  *         APB1LPENR1   UART8LPEN     LL_APB1_GRP1_IsEnabledClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C3
  *         @arg @ref LL_APB1_GRP1_PERIPH_I3C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I3C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_LPTIM1
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPDIFRX1
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM10
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM11
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART7
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART8
  *         @arg @ref LL_APB1_GRP1_PERIPH_WWDG
  * @retval uint32_t
  */
__STATIC_INLINE uint32_t LL_APB1_GRP1_IsEnabledClockLowPower(uint32_t Periphs)
{
  return ((READ_BIT(RCC->APB1LPENR1, Periphs) == Periphs) ? 1UL : 0UL);
}

/**
  * @brief  Disable APB1 peripherals clock during Low Power mode.
  * @rmtoll APB1LPENCR1  TIM2LPENC     LL_APB1_GRP1_DisableClockLowPower\n
  *         APB1LPENCR1  TIM3LPENC     LL_APB1_GRP1_DisableClockLowPower\n
  *         APB1LPENCR1  TIM4LPENC     LL_APB1_GRP1_DisableClockLowPower\n
  *         APB1LPENCR1  TIM5LPENC     LL_APB1_GRP1_DisableClockLowPower\n
  *         APB1LPENCR1  TIM6LPENC     LL_APB1_GRP1_DisableClockLowPower\n
  *         APB1LPENCR1  TIM7LPENC     LL_APB1_GRP1_DisableClockLowPower\n
  *         APB1LPENCR1  TIM12LPENC    LL_APB1_GRP1_DisableClockLowPower\n
  *         APB1LPENCR1  TIM13LPENC    LL_APB1_GRP1_DisableClockLowPower\n
  *         APB1LPENCR1  TIM14LPENC    LL_APB1_GRP1_DisableClockLowPower\n
  *         APB1LPENCR1  LPTIM1LPENC   LL_APB1_GRP1_DisableClockLowPower\n
  *         APB1LPENCR1  WWDGLPENC     LL_APB1_GRP1_DisableClockLowPower\n
  *         APB1LPENCR1  TIM10LPENC    LL_APB1_GRP1_DisableClockLowPower\n
  *         APB1LPENCR1  TIM11LPENC    LL_APB1_GRP1_DisableClockLowPower\n
  *         APB1LPENCR1  SPI2LPENC     LL_APB1_GRP1_DisableClockLowPower\n
  *         APB1LPENCR1  SPI3LPENC     LL_APB1_GRP1_DisableClockLowPower\n
  *         APB1LPENCR1  SPDIFRX1LPENC LL_APB1_GRP1_DisableClockLowPower\n
  *         APB1LPENCR1  USART2LPENC   LL_APB1_GRP1_DisableClockLowPower\n
  *         APB1LPENCR1  USART3LPENC   LL_APB1_GRP1_DisableClockLowPower\n
  *         APB1LPENCR1  UART4LPENC    LL_APB1_GRP1_DisableClockLowPower\n
  *         APB1LPENCR1  UART5LPENC    LL_APB1_GRP1_DisableClockLowPower\n
  *         APB1LPENCR1  I2C1LPENC     LL_APB1_GRP1_DisableClockLowPower\n
  *         APB1LPENCR1  I2C2LPENC     LL_APB1_GRP1_DisableClockLowPower\n
  *         APB1LPENCR1  I2C3LPENC     LL_APB1_GRP1_DisableClockLowPower\n
  *         APB1LPENCR1  I3C1LPENC     LL_APB1_GRP1_DisableClockLowPower\n
  *         APB1LPENCR1  I3C2LPENC     LL_APB1_GRP1_DisableClockLowPower\n
  *         APB1LPENCR1  UART7LPENC    LL_APB1_GRP1_DisableClockLowPower\n
  *         APB1LPENCR1  UART8LPENC    LL_APB1_GRP1_DisableClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C3
  *         @arg @ref LL_APB1_GRP1_PERIPH_I3C1
  *         @arg @ref LL_APB1_GRP1_PERIPH_I3C2
  *         @arg @ref LL_APB1_GRP1_PERIPH_LPTIM1
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPDIFRX1
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM10
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM11
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART7
  *         @arg @ref LL_APB1_GRP1_PERIPH_UART8
  *         @arg @ref LL_APB1_GRP1_PERIPH_WWDG
  * @retval None
  */
__STATIC_INLINE void LL_APB1_GRP1_DisableClockLowPower(uint32_t Periphs)
{
  WRITE_REG(RCC->APB1LPENCR1, Periphs);
}

/**
  * @brief  Enable APB1 peripherals clock.
  * @rmtoll APB1ENSR2    MDIOSENS      LL_APB1_GRP2_EnableClock\n
  *         APB1ENSR2    FDCANENS      LL_APB1_GRP2_EnableClock\n
  *         APB1ENSR2    UCPD1ENS      LL_APB1_GRP2_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP2_PERIPH_FDCAN
  *         @arg @ref LL_APB1_GRP2_PERIPH_MDIOS
  *         @arg @ref LL_APB1_GRP2_PERIPH_UCPD1
  * @retval None
  */
__STATIC_INLINE void LL_APB1_GRP2_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  WRITE_REG(RCC->APB1ENSR2, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_REG(RCC->APB1ENR2);
  (void)tmpreg;
}

/**
  * @brief  Check if APB1 peripheral clock is enabled or not
  * @rmtoll APB1ENR2     MDIOSEN       LL_APB1_GRP2_IsEnabledClock\n
  *         APB1ENR2     FDCANEN       LL_APB1_GRP2_IsEnabledClock\n
  *         APB1ENR2     UCPD1EN       LL_APB1_GRP2_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP2_PERIPH_FDCAN
  *         @arg @ref LL_APB1_GRP2_PERIPH_MDIOS
  *         @arg @ref LL_APB1_GRP2_PERIPH_UCPD1
  * @retval uint32_t
  */
__STATIC_INLINE uint32_t LL_APB1_GRP2_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC->APB1ENR2, Periphs) == Periphs) ? 1UL : 0UL);
}

/**
  * @brief  Disable APB1 peripherals clock.
  * @rmtoll APB1ENCR2    MDIOSENC      LL_APB1_GRP2_DisableClock\n
  *         APB1ENCR2    FDCANENC      LL_APB1_GRP2_DisableClock\n
  *         APB1ENCR2    UCPD1ENC      LL_APB1_GRP2_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP2_PERIPH_FDCAN
  *         @arg @ref LL_APB1_GRP2_PERIPH_MDIOS
  *         @arg @ref LL_APB1_GRP2_PERIPH_UCPD1
  * @retval None
  */
__STATIC_INLINE void LL_APB1_GRP2_DisableClock(uint32_t Periphs)
{
  WRITE_REG(RCC->APB1ENCR2, Periphs);
}

/**
  * @brief  Force APB1 peripherals reset.
  * @rmtoll APB1RSTSR2   MDIOSRSTS     LL_APB1_GRP2_ForceReset\n
  *         APB1RSTSR2   FDCANRSTS     LL_APB1_GRP2_ForceReset\n
  *         APB1RSTSR2   UCPD1RSTS     LL_APB1_GRP2_ForceReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP2_PERIPH_FDCAN
  *         @arg @ref LL_APB1_GRP2_PERIPH_MDIOS
  *         @arg @ref LL_APB1_GRP2_PERIPH_UCPD1
  * @retval None
  */
__STATIC_INLINE void LL_APB1_GRP2_ForceReset(uint32_t Periphs)
{
  WRITE_REG(RCC->APB1RSTSR2, Periphs);
}

/**
  * @brief  Release APB1 peripherals reset.
  * @rmtoll APB1RSTCR2   MDIOSRSTC     LL_APB1_GRP2_ReleaseReset\n
  *         APB1RSTCR2   FDCANRSTC     LL_APB1_GRP2_ReleaseReset\n
  *         APB1RSTCR2   UCPD1RSTC     LL_APB1_GRP2_ReleaseReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP2_PERIPH_FDCAN
  *         @arg @ref LL_APB1_GRP2_PERIPH_MDIOS
  *         @arg @ref LL_APB1_GRP2_PERIPH_UCPD1
  * @retval None
  */
__STATIC_INLINE void LL_APB1_GRP2_ReleaseReset(uint32_t Periphs)
{
  WRITE_REG(RCC->APB1RSTCR2, Periphs);
}

/**
  * @brief  Enable APB1 peripherals clock during Low Power mode.
  * @rmtoll APB1LPENSR2  MDIOSLPENS    LL_APB1_GRP2_EnableClockLowPower\n
  *         APB1LPENSR2  FDCANLPENS    LL_APB1_GRP2_EnableClockLowPower\n
  *         APB1LPENSR2  UCPD1LPENS    LL_APB1_GRP2_EnableClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP2_PERIPH_FDCAN
  *         @arg @ref LL_APB1_GRP2_PERIPH_MDIOS
  *         @arg @ref LL_APB1_GRP2_PERIPH_UCPD1
  * @retval None
  */
__STATIC_INLINE void LL_APB1_GRP2_EnableClockLowPower(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  WRITE_REG(RCC->APB1LPENSR2, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_REG(RCC->APB1LPENR2);
  (void)tmpreg;
}

/**
  * @brief  Check if APB1 peripheral clock during Low Power mode is enabled or not .
  * @rmtoll APB1LPENR2   MDIOSLPEN     LL_APB1_GRP2_IsEnabledClockLowPower\n
  *         APB1LPENR2   FDCANLPEN     LL_APB1_GRP2_IsEnabledClockLowPower\n
  *         APB1LPENR2   UCPD1LPEN     LL_APB1_GRP2_IsEnabledClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP2_PERIPH_FDCAN
  *         @arg @ref LL_APB1_GRP2_PERIPH_MDIOS
  *         @arg @ref LL_APB1_GRP2_PERIPH_UCPD1
  * @retval uint32_t
  */
__STATIC_INLINE uint32_t LL_APB1_GRP2_IsEnabledClockLowPower(uint32_t Periphs)
{
  return ((READ_BIT(RCC->APB1LPENR2, Periphs) == Periphs) ? 1UL : 0UL);
}

/**
  * @brief  Disable APB1 peripherals clock during Low Power mode.
  * @rmtoll APB1LPENCR2  MDIOSLPENC    LL_APB1_GRP2_DisableClockLowPower\n
  *         APB1LPENCR2  FDCANLPENC    LL_APB1_GRP2_DisableClockLowPower\n
  *         APB1LPENCR2  UCPD1LPENC    LL_APB1_GRP2_DisableClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP2_PERIPH_FDCAN
  *         @arg @ref LL_APB1_GRP2_PERIPH_MDIOS
  *         @arg @ref LL_APB1_GRP2_PERIPH_UCPD1
  * @retval None
  */
__STATIC_INLINE void LL_APB1_GRP2_DisableClockLowPower(uint32_t Periphs)
{
  WRITE_REG(RCC->APB1LPENCR2, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_APB2 APB2
  * @{
  */

/**
  * @brief  Enable APB2 peripherals clock.
  * @rmtoll APB2ENSR     TIM1ENS       LL_APB2_GRP1_EnableClock\n
  *         APB2ENSR     TIM8ENS       LL_APB2_GRP1_EnableClock\n
  *         APB2ENSR     USART1ENS     LL_APB2_GRP1_EnableClock\n
  *         APB2ENSR     USART6ENS     LL_APB2_GRP1_EnableClock\n
  *         APB2ENSR     UART9ENS      LL_APB2_GRP1_EnableClock\n
  *         APB2ENSR     USART10ENS    LL_APB2_GRP1_EnableClock\n
  *         APB2ENSR     SPI1ENS       LL_APB2_GRP1_EnableClock\n
  *         APB2ENSR     SPI4ENS       LL_APB2_GRP1_EnableClock\n
  *         APB2ENSR     TIM18ENS      LL_APB2_GRP1_EnableClock\n
  *         APB2ENSR     TIM15ENS      LL_APB2_GRP1_EnableClock\n
  *         APB2ENSR     TIM16ENS      LL_APB2_GRP1_EnableClock\n
  *         APB2ENSR     TIM17ENS      LL_APB2_GRP1_EnableClock\n
  *         APB2ENSR     TIM9ENS       LL_APB2_GRP1_EnableClock\n
  *         APB2ENSR     SPI5ENS       LL_APB2_GRP1_EnableClock\n
  *         APB2ENSR     SAI1ENS       LL_APB2_GRP1_EnableClock\n
  *         APB2ENSR     SAI2ENS       LL_APB2_GRP1_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI2
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI4
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI5
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM9
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM15
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM16
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM17
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM18
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART6
  *         @arg @ref LL_APB2_GRP1_PERIPH_UART9
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART10
  * @retval None
  */
__STATIC_INLINE void LL_APB2_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  WRITE_REG(RCC->APB2ENSR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_REG(RCC->APB2ENR);
  (void)tmpreg;
}

/**
  * @brief  Check if APB2 peripheral clock is enabled or not
  * @rmtoll APB2ENR      TIM1EN        LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      TIM8EN        LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      USART1EN      LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      USART6EN      LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      UART9EN       LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      USART10EN     LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      SPI1EN        LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      SPI4EN        LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      TIM18EN       LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      TIM15EN       LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      TIM16EN       LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      TIM17EN       LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      TIM9EN        LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      SPI5EN        LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      SAI1EN        LL_APB2_GRP1_IsEnabledClock\n
  *         APB2ENR      SAI2EN        LL_APB2_GRP1_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI2
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI4
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI5
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM9
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM15
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM16
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM17
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM18
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART6
  *         @arg @ref LL_APB2_GRP1_PERIPH_UART9
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART10
  * @retval uint32_t
  */
__STATIC_INLINE uint32_t LL_APB2_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC->APB2ENR, Periphs) == Periphs) ? 1UL : 0UL);
}

/**
  * @brief  Disable APB2 peripherals clock.
  * @rmtoll APB2ENCR     TIM1ENC       LL_APB2_GRP1_DisableClock\n
  *         APB2ENCR     TIM8ENC       LL_APB2_GRP1_DisableClock\n
  *         APB2ENCR     USART1ENC     LL_APB2_GRP1_DisableClock\n
  *         APB2ENCR     USART6ENC     LL_APB2_GRP1_DisableClock\n
  *         APB2ENCR     UART9ENC      LL_APB2_GRP1_DisableClock\n
  *         APB2ENCR     USART10ENC    LL_APB2_GRP1_DisableClock\n
  *         APB2ENCR     SPI1ENC       LL_APB2_GRP1_DisableClock\n
  *         APB2ENCR     SPI4ENC       LL_APB2_GRP1_DisableClock\n
  *         APB2ENCR     TIM18ENC      LL_APB2_GRP1_DisableClock\n
  *         APB2ENCR     TIM15ENC      LL_APB2_GRP1_DisableClock\n
  *         APB2ENCR     TIM16ENC      LL_APB2_GRP1_DisableClock\n
  *         APB2ENCR     TIM17ENC      LL_APB2_GRP1_DisableClock\n
  *         APB2ENCR     TIM9ENC       LL_APB2_GRP1_DisableClock\n
  *         APB2ENCR     SPI5ENC       LL_APB2_GRP1_DisableClock\n
  *         APB2ENCR     SAI1ENC       LL_APB2_GRP1_DisableClock\n
  *         APB2ENCR     SAI2ENC       LL_APB2_GRP1_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI2
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI4
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI5
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM9
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM15
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM16
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM17
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM18
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART6
  *         @arg @ref LL_APB2_GRP1_PERIPH_UART9
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART10
  * @retval None
  */
__STATIC_INLINE void LL_APB2_GRP1_DisableClock(uint32_t Periphs)
{
  WRITE_REG(RCC->APB2ENCR, Periphs);
}

/**
  * @brief  Force APB2 peripherals reset.
  * @rmtoll APB2RSTSR    TIM1RSTS      LL_APB2_GRP1_ForceReset\n
  *         APB2RSTSR    TIM8RSTS      LL_APB2_GRP1_ForceReset\n
  *         APB2RSTSR    USART1RSTS    LL_APB2_GRP1_ForceReset\n
  *         APB2RSTSR    USART6RSTS    LL_APB2_GRP1_ForceReset\n
  *         APB2RSTSR    UART9RSTS     LL_APB2_GRP1_ForceReset\n
  *         APB2RSTSR    USART10RSTS   LL_APB2_GRP1_ForceReset\n
  *         APB2RSTSR    SPI1RSTS      LL_APB2_GRP1_ForceReset\n
  *         APB2RSTSR    SPI4RSTS      LL_APB2_GRP1_ForceReset\n
  *         APB2RSTSR    TIM18RSTS     LL_APB2_GRP1_ForceReset\n
  *         APB2RSTSR    TIM15RSTS     LL_APB2_GRP1_ForceReset\n
  *         APB2RSTSR    TIM16RSTS     LL_APB2_GRP1_ForceReset\n
  *         APB2RSTSR    TIM17RSTS     LL_APB2_GRP1_ForceReset\n
  *         APB2RSTSR    TIM9RSTS      LL_APB2_GRP1_ForceReset\n
  *         APB2RSTSR    SPI5RSTS      LL_APB2_GRP1_ForceReset\n
  *         APB2RSTSR    SAI1RSTS      LL_APB2_GRP1_ForceReset\n
  *         APB2RSTSR    SAI2RSTS      LL_APB2_GRP1_ForceReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI2
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI4
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI5
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM9
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM15
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM16
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM17
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM18
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART6
  *         @arg @ref LL_APB2_GRP1_PERIPH_UART9
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART10
  * @retval None
  */
__STATIC_INLINE void LL_APB2_GRP1_ForceReset(uint32_t Periphs)
{
  WRITE_REG(RCC->APB2RSTSR, Periphs);
}

/**
  * @brief  Release APB2 peripherals reset.
  * @rmtoll APB2RSTCR    TIM1RSTC      LL_APB2_GRP1_ReleaseReset\n
  *         APB2RSTCR    TIM8RSTC      LL_APB2_GRP1_ReleaseReset\n
  *         APB2RSTCR    USART1RSTC    LL_APB2_GRP1_ReleaseReset\n
  *         APB2RSTCR    USART6RSTC    LL_APB2_GRP1_ReleaseReset\n
  *         APB2RSTCR    UART9RSTC     LL_APB2_GRP1_ReleaseReset\n
  *         APB2RSTCR    USART10RSTC   LL_APB2_GRP1_ReleaseReset\n
  *         APB2RSTCR    SPI1RSTC      LL_APB2_GRP1_ReleaseReset\n
  *         APB2RSTCR    SPI4RSTC      LL_APB2_GRP1_ReleaseReset\n
  *         APB2RSTCR    TIM18RSTC     LL_APB2_GRP1_ReleaseReset\n
  *         APB2RSTCR    TIM15RSTC     LL_APB2_GRP1_ReleaseReset\n
  *         APB2RSTCR    TIM16RSTC     LL_APB2_GRP1_ReleaseReset\n
  *         APB2RSTCR    TIM17RSTC     LL_APB2_GRP1_ReleaseReset\n
  *         APB2RSTCR    TIM9RSTC      LL_APB2_GRP1_ReleaseReset\n
  *         APB2RSTCR    SPI5RSTC      LL_APB2_GRP1_ReleaseReset\n
  *         APB2RSTCR    SAI1RSTC      LL_APB2_GRP1_ReleaseReset\n
  *         APB2RSTCR    SAI2RSTC      LL_APB2_GRP1_ReleaseReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI2
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI4
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI5
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM9
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM15
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM16
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM17
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM18
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART6
  *         @arg @ref LL_APB2_GRP1_PERIPH_UART9
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART10
  * @retval None
  */
__STATIC_INLINE void LL_APB2_GRP1_ReleaseReset(uint32_t Periphs)
{
  WRITE_REG(RCC->APB2RSTCR, Periphs);
}

/**
  * @brief  Enable APB2 peripherals clock during Low Power mode.
  * @rmtoll APB2LPENSR   TIM1LPENS     LL_APB2_GRP1_EnableClockLowPower\n
  *         APB2LPENSR   TIM8LPENS     LL_APB2_GRP1_EnableClockLowPower\n
  *         APB2LPENSR   USART1LPENS   LL_APB2_GRP1_EnableClockLowPower\n
  *         APB2LPENSR   USART6LPENS   LL_APB2_GRP1_EnableClockLowPower\n
  *         APB2LPENSR   UART9LPENS    LL_APB2_GRP1_EnableClockLowPower\n
  *         APB2LPENSR   USART10LPENS  LL_APB2_GRP1_EnableClockLowPower\n
  *         APB2LPENSR   SPI1LPENS     LL_APB2_GRP1_EnableClockLowPower\n
  *         APB2LPENSR   SPI4LPENS     LL_APB2_GRP1_EnableClockLowPower\n
  *         APB2LPENSR   TIM18LPENS    LL_APB2_GRP1_EnableClockLowPower\n
  *         APB2LPENSR   TIM15LPENS    LL_APB2_GRP1_EnableClockLowPower\n
  *         APB2LPENSR   TIM16LPENS    LL_APB2_GRP1_EnableClockLowPower\n
  *         APB2LPENSR   TIM17LPENS    LL_APB2_GRP1_EnableClockLowPower\n
  *         APB2LPENSR   TIM9LPENS     LL_APB2_GRP1_EnableClockLowPower\n
  *         APB2LPENSR   SPI5LPENS     LL_APB2_GRP1_EnableClockLowPower\n
  *         APB2LPENSR   SAI1LPENS     LL_APB2_GRP1_EnableClockLowPower\n
  *         APB2LPENSR   SAI2LPENS     LL_APB2_GRP1_EnableClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI2
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI4
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI5
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM9
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM15
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM16
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM17
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM18
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART6
  *         @arg @ref LL_APB2_GRP1_PERIPH_UART9
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART10
  * @retval None
  */
__STATIC_INLINE void LL_APB2_GRP1_EnableClockLowPower(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  WRITE_REG(RCC->APB2LPENSR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_REG(RCC->APB2LPENR);
  (void)tmpreg;
}

/**
  * @brief  Check if APB2 peripheral clock during Low Power mode is enabled or not .
  * @rmtoll APB2LPENR    TIM1LPEN      LL_APB2_GRP1_IsEnabledClockLowPower\n
  *         APB2LPENR    TIM8LPEN      LL_APB2_GRP1_IsEnabledClockLowPower\n
  *         APB2LPENR    USART1LPEN    LL_APB2_GRP1_IsEnabledClockLowPower\n
  *         APB2LPENR    USART6LPEN    LL_APB2_GRP1_IsEnabledClockLowPower\n
  *         APB2LPENR    UART9LPEN     LL_APB2_GRP1_IsEnabledClockLowPower\n
  *         APB2LPENR    USART10LPEN   LL_APB2_GRP1_IsEnabledClockLowPower\n
  *         APB2LPENR    SPI1LPEN      LL_APB2_GRP1_IsEnabledClockLowPower\n
  *         APB2LPENR    SPI4LPEN      LL_APB2_GRP1_IsEnabledClockLowPower\n
  *         APB2LPENR    TIM18LPEN     LL_APB2_GRP1_IsEnabledClockLowPower\n
  *         APB2LPENR    TIM15LPEN     LL_APB2_GRP1_IsEnabledClockLowPower\n
  *         APB2LPENR    TIM16LPEN     LL_APB2_GRP1_IsEnabledClockLowPower\n
  *         APB2LPENR    TIM17LPEN     LL_APB2_GRP1_IsEnabledClockLowPower\n
  *         APB2LPENR    TIM9LPEN      LL_APB2_GRP1_IsEnabledClockLowPower\n
  *         APB2LPENR    SPI5LPEN      LL_APB2_GRP1_IsEnabledClockLowPower\n
  *         APB2LPENR    SAI1LPEN      LL_APB2_GRP1_IsEnabledClockLowPower\n
  *         APB2LPENR    SAI2LPEN      LL_APB2_GRP1_IsEnabledClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI2
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI4
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI5
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM9
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM15
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM16
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM17
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM18
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART6
  *         @arg @ref LL_APB2_GRP1_PERIPH_UART9
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART10
  * @retval uint32_t
  */
__STATIC_INLINE uint32_t LL_APB2_GRP1_IsEnabledClockLowPower(uint32_t Periphs)
{
  return ((READ_BIT(RCC->APB2LPENR, Periphs) == Periphs) ? 1UL : 0UL);
}

/**
  * @brief  Disable APB2 peripherals clock during Low Power mode.
  * @rmtoll APB2LPENCR   TIM1LPENC     LL_APB2_GRP1_DisableClockLowPower\n
  *         APB2LPENCR   TIM8LPENC     LL_APB2_GRP1_DisableClockLowPower\n
  *         APB2LPENCR   USART1LPENC   LL_APB2_GRP1_DisableClockLowPower\n
  *         APB2LPENCR   USART6LPENC   LL_APB2_GRP1_DisableClockLowPower\n
  *         APB2LPENCR   UART9LPENC    LL_APB2_GRP1_DisableClockLowPower\n
  *         APB2LPENCR   USART10LPENC  LL_APB2_GRP1_DisableClockLowPower\n
  *         APB2LPENCR   SPI1LPENC     LL_APB2_GRP1_DisableClockLowPower\n
  *         APB2LPENCR   SPI4LPENC     LL_APB2_GRP1_DisableClockLowPower\n
  *         APB2LPENCR   TIM18LPENC    LL_APB2_GRP1_DisableClockLowPower\n
  *         APB2LPENCR   TIM15LPENC    LL_APB2_GRP1_DisableClockLowPower\n
  *         APB2LPENCR   TIM16LPENC    LL_APB2_GRP1_DisableClockLowPower\n
  *         APB2LPENCR   TIM17LPENC    LL_APB2_GRP1_DisableClockLowPower\n
  *         APB2LPENCR   TIM9LPENC     LL_APB2_GRP1_DisableClockLowPower\n
  *         APB2LPENCR   SPI5LPENC     LL_APB2_GRP1_DisableClockLowPower\n
  *         APB2LPENCR   SAI1LPENC     LL_APB2_GRP1_DisableClockLowPower\n
  *         APB2LPENCR   SAI2LPENC     LL_APB2_GRP1_DisableClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SAI2
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI4
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI5
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM9
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM15
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM16
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM17
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM18
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART6
  *         @arg @ref LL_APB2_GRP1_PERIPH_UART9
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART10
  * @retval None
  */
__STATIC_INLINE void LL_APB2_GRP1_DisableClockLowPower(uint32_t Periphs)
{
  WRITE_REG(RCC->APB2LPENCR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_APB4 APB4
  * @{
  */

/**
  * @brief  Enable APB4 peripherals clock.
  * @rmtoll APB4ENSR1    HDPENS        LL_APB4_GRP1_EnableClock\n
  *         APB4ENSR1    LPUART1ENS    LL_APB4_GRP1_EnableClock\n
  *         APB4ENSR1    SPI6ENS       LL_APB4_GRP1_EnableClock\n
  *         APB4ENSR1    I2C4ENS       LL_APB4_GRP1_EnableClock\n
  *         APB4ENSR1    LPTIM2ENS     LL_APB4_GRP1_EnableClock\n
  *         APB4ENSR1    LPTIM3ENS     LL_APB4_GRP1_EnableClock\n
  *         APB4ENSR1    LPTIM4ENS     LL_APB4_GRP1_EnableClock\n
  *         APB4ENSR1    LPTIM5ENS     LL_APB4_GRP1_EnableClock\n
  *         APB4ENSR1    VREFBUFENS    LL_APB4_GRP1_EnableClock\n
  *         APB4ENSR1    RTCENS        LL_APB4_GRP1_EnableClock\n
  *         APB4ENSR1    RTCAPBENS     LL_APB4_GRP1_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP1_PERIPH_HDP
  *         @arg @ref LL_APB4_GRP1_PERIPH_I2C4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM2
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM3
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM5
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPUART1
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTC
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTCAPB
  *         @arg @ref LL_APB4_GRP1_PERIPH_SPI6
  *         @arg @ref LL_APB4_GRP1_PERIPH_VREFBUF
  * @retval None
  */
__STATIC_INLINE void LL_APB4_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  WRITE_REG(RCC->APB4ENSR1, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_REG(RCC->APB4ENR1);
  (void)tmpreg;
}

/**
  * @brief  Check if APB4 peripheral clock is enabled or not
  * @rmtoll APB4ENR1     HDPEN         LL_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR1     LPUART1EN     LL_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR1     SPI6EN        LL_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR1     I2C4EN        LL_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR1     LPTIM2EN      LL_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR1     LPTIM3EN      LL_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR1     LPTIM4EN      LL_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR1     LPTIM5EN      LL_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR1     VREFBUFEN     LL_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR1     RTCEN         LL_APB4_GRP1_IsEnabledClock\n
  *         APB4ENR1     RTCAPBEN      LL_APB4_GRP1_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP1_PERIPH_HDP
  *         @arg @ref LL_APB4_GRP1_PERIPH_I2C4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM2
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM3
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM5
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPUART1
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTC
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTCAPB
  *         @arg @ref LL_APB4_GRP1_PERIPH_SPI6
  *         @arg @ref LL_APB4_GRP1_PERIPH_VREFBUF
  * @retval uint32_t
  */
__STATIC_INLINE uint32_t LL_APB4_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC->APB4ENR1, Periphs) == Periphs) ? 1UL : 0UL);
}

/**
  * @brief  Disable APB4 peripherals clock.
  * @rmtoll APB4ENCR1    HDPENC        LL_APB4_GRP1_DisableClock\n
  *         APB4ENCR1    LPUART1ENC    LL_APB4_GRP1_DisableClock\n
  *         APB4ENCR1    SPI6ENC       LL_APB4_GRP1_DisableClock\n
  *         APB4ENCR1    I2C4ENC       LL_APB4_GRP1_DisableClock\n
  *         APB4ENCR1    LPTIM2ENC     LL_APB4_GRP1_DisableClock\n
  *         APB4ENCR1    LPTIM3ENC     LL_APB4_GRP1_DisableClock\n
  *         APB4ENCR1    LPTIM4ENC     LL_APB4_GRP1_DisableClock\n
  *         APB4ENCR1    LPTIM5ENC     LL_APB4_GRP1_DisableClock\n
  *         APB4ENCR1    VREFBUFENC    LL_APB4_GRP1_DisableClock\n
  *         APB4ENCR1    RTCENC        LL_APB4_GRP1_DisableClock\n
  *         APB4ENCR1    RTCAPBENC     LL_APB4_GRP1_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP1_PERIPH_HDP
  *         @arg @ref LL_APB4_GRP1_PERIPH_I2C4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM2
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM3
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM5
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPUART1
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTC
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTCAPB
  *         @arg @ref LL_APB4_GRP1_PERIPH_SPI6
  *         @arg @ref LL_APB4_GRP1_PERIPH_VREFBUF
  * @retval None
  */
__STATIC_INLINE void LL_APB4_GRP1_DisableClock(uint32_t Periphs)
{
  WRITE_REG(RCC->APB4ENCR1, Periphs);
}

/**
  * @brief  Force APB4 peripherals reset.
  * @rmtoll APB4RSTSR1   HDPRSTS       LL_APB4_GRP1_ForceReset\n
  *         APB4RSTSR1   LPUART1RSTS   LL_APB4_GRP1_ForceReset\n
  *         APB4RSTSR1   SPI6RSTS      LL_APB4_GRP1_ForceReset\n
  *         APB4RSTSR1   I2C4RSTS      LL_APB4_GRP1_ForceReset\n
  *         APB4RSTSR1   LPTIM2RSTS    LL_APB4_GRP1_ForceReset\n
  *         APB4RSTSR1   LPTIM3RSTS    LL_APB4_GRP1_ForceReset\n
  *         APB4RSTSR1   LPTIM4RSTS    LL_APB4_GRP1_ForceReset\n
  *         APB4RSTSR1   LPTIM5RSTS    LL_APB4_GRP1_ForceReset\n
  *         APB4RSTSR1   VREFBUFRSTS   LL_APB4_GRP1_ForceReset\n
  *         APB4RSTSR1   RTCAPBRSTS    LL_APB4_GRP1_ForceReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP1_PERIPH_HDP
  *         @arg @ref LL_APB4_GRP1_PERIPH_I2C4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM2
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM3
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM5
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPUART1
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTCAPB
  *         @arg @ref LL_APB4_GRP1_PERIPH_SPI6
  *         @arg @ref LL_APB4_GRP1_PERIPH_VREFBUF
  * @retval None
  */
__STATIC_INLINE void LL_APB4_GRP1_ForceReset(uint32_t Periphs)
{
  WRITE_REG(RCC->APB4RSTSR1, Periphs);
}

/**
  * @brief  Release APB4 peripherals reset.
  * @rmtoll APB4RSTCR1   HDPRSTC       LL_APB4_GRP1_ReleaseReset\n
  *         APB4RSTCR1   LPUART1RSTC   LL_APB4_GRP1_ReleaseReset\n
  *         APB4RSTCR1   SPI6RSTC      LL_APB4_GRP1_ReleaseReset\n
  *         APB4RSTCR1   I2C4RSTC      LL_APB4_GRP1_ReleaseReset\n
  *         APB4RSTCR1   LPTIM2RSTC    LL_APB4_GRP1_ReleaseReset\n
  *         APB4RSTCR1   LPTIM3RSTC    LL_APB4_GRP1_ReleaseReset\n
  *         APB4RSTCR1   LPTIM4RSTC    LL_APB4_GRP1_ReleaseReset\n
  *         APB4RSTCR1   LPTIM5RSTC    LL_APB4_GRP1_ReleaseReset\n
  *         APB4RSTCR1   VREFBUFRSTC   LL_APB4_GRP1_ReleaseReset\n
  *         APB4RSTCR1   RTCAPBRSTC    LL_APB4_GRP1_ReleaseReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP1_PERIPH_HDP
  *         @arg @ref LL_APB4_GRP1_PERIPH_I2C4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM2
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM3
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM5
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPUART1
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTCAPB
  *         @arg @ref LL_APB4_GRP1_PERIPH_SPI6
  *         @arg @ref LL_APB4_GRP1_PERIPH_VREFBUF
  * @retval None
  */
__STATIC_INLINE void LL_APB4_GRP1_ReleaseReset(uint32_t Periphs)
{
  WRITE_REG(RCC->APB4RSTCR1, Periphs);
}

/**
  * @brief  Enable APB4 peripherals clock during Low Power mode.
  * @rmtoll APB4LPENSR1  HDPLPENS      LL_APB4_GRP1_EnableClockLowPower\n
  *         APB4LPENSR1  LPUART1LPENS  LL_APB4_GRP1_EnableClockLowPower\n
  *         APB4LPENSR1  SPI6LPENS     LL_APB4_GRP1_EnableClockLowPower\n
  *         APB4LPENSR1  I2C4LPENS     LL_APB4_GRP1_EnableClockLowPower\n
  *         APB4LPENSR1  LPTIM2LPENS   LL_APB4_GRP1_EnableClockLowPower\n
  *         APB4LPENSR1  LPTIM3LPENS   LL_APB4_GRP1_EnableClockLowPower\n
  *         APB4LPENSR1  LPTIM4LPENS   LL_APB4_GRP1_EnableClockLowPower\n
  *         APB4LPENSR1  LPTIM5LPENS   LL_APB4_GRP1_EnableClockLowPower\n
  *         APB4LPENSR1  VREFBUFLPENS  LL_APB4_GRP1_EnableClockLowPower\n
  *         APB4LPENSR1  RTCLPENS      LL_APB4_GRP1_EnableClockLowPower\n
  *         APB4LPENSR1  RTCAPBLPENS   LL_APB4_GRP1_EnableClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP1_PERIPH_HDP
  *         @arg @ref LL_APB4_GRP1_PERIPH_I2C4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM2
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM3
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM5
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPUART1
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTC
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTCAPB
  *         @arg @ref LL_APB4_GRP1_PERIPH_SPI6
  *         @arg @ref LL_APB4_GRP1_PERIPH_VREFBUF
  * @retval None
  */
__STATIC_INLINE void LL_APB4_GRP1_EnableClockLowPower(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  WRITE_REG(RCC->APB4LPENSR1, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_REG(RCC->APB4LPENR1);
  (void)tmpreg;
}

/**
  * @brief  Check if APB4 peripheral clock during Low Power mode is enabled or not .
  * @rmtoll APB4LPENR1   HDPLPEN       LL_APB4_GRP1_IsEnabledClockLowPower\n
  *         APB4LPENR1   LPUART1LPEN   LL_APB4_GRP1_IsEnabledClockLowPower\n
  *         APB4LPENR1   SPI6LPEN      LL_APB4_GRP1_IsEnabledClockLowPower\n
  *         APB4LPENR1   I2C4LPEN      LL_APB4_GRP1_IsEnabledClockLowPower\n
  *         APB4LPENR1   LPTIM2LPEN    LL_APB4_GRP1_IsEnabledClockLowPower\n
  *         APB4LPENR1   LPTIM3LPEN    LL_APB4_GRP1_IsEnabledClockLowPower\n
  *         APB4LPENR1   LPTIM4LPEN    LL_APB4_GRP1_IsEnabledClockLowPower\n
  *         APB4LPENR1   LPTIM5LPEN    LL_APB4_GRP1_IsEnabledClockLowPower\n
  *         APB4LPENR1   VREFBUFLPEN   LL_APB4_GRP1_IsEnabledClockLowPower\n
  *         APB4LPENR1   RTCLPEN       LL_APB4_GRP1_IsEnabledClockLowPower\n
  *         APB4LPENR1   RTCAPBLPEN    LL_APB4_GRP1_IsEnabledClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP1_PERIPH_HDP
  *         @arg @ref LL_APB4_GRP1_PERIPH_I2C4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM2
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM3
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM5
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPUART1
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTC
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTCAPB
  *         @arg @ref LL_APB4_GRP1_PERIPH_SPI6
  *         @arg @ref LL_APB4_GRP1_PERIPH_VREFBUF
  * @retval uint32_t
  */
__STATIC_INLINE uint32_t LL_APB4_GRP1_IsEnabledClockLowPower(uint32_t Periphs)
{
  return ((READ_BIT(RCC->APB4LPENR1, Periphs) == Periphs) ? 1UL : 0UL);
}

/**
  * @brief  Disable APB4 peripherals clock during Low Power mode.
  * @rmtoll APB4LPENCR1  HDPLPENC      LL_APB4_GRP1_DisableClockLowPower\n
  *         APB4LPENCR1  LPUART1LPENC  LL_APB4_GRP1_DisableClockLowPower\n
  *         APB4LPENCR1  SPI6LPENC     LL_APB4_GRP1_DisableClockLowPower\n
  *         APB4LPENCR1  I2C4LPENC     LL_APB4_GRP1_DisableClockLowPower\n
  *         APB4LPENCR1  LPTIM2LPENC   LL_APB4_GRP1_DisableClockLowPower\n
  *         APB4LPENCR1  LPTIM3LPENC   LL_APB4_GRP1_DisableClockLowPower\n
  *         APB4LPENCR1  LPTIM4LPENC   LL_APB4_GRP1_DisableClockLowPower\n
  *         APB4LPENCR1  LPTIM5LPENC   LL_APB4_GRP1_DisableClockLowPower\n
  *         APB4LPENCR1  VREFBUFLPENC  LL_APB4_GRP1_DisableClockLowPower\n
  *         APB4LPENCR1  RTCLPENC      LL_APB4_GRP1_DisableClockLowPower\n
  *         APB4LPENCR1  RTCAPBLPENC   LL_APB4_GRP1_DisableClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP1_PERIPH_HDP
  *         @arg @ref LL_APB4_GRP1_PERIPH_I2C4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM2
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM3
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM4
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPTIM5
  *         @arg @ref LL_APB4_GRP1_PERIPH_LPUART1
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTC
  *         @arg @ref LL_APB4_GRP1_PERIPH_RTCAPB
  *         @arg @ref LL_APB4_GRP1_PERIPH_SPI6
  *         @arg @ref LL_APB4_GRP1_PERIPH_VREFBUF
  * @retval None
  */
__STATIC_INLINE void LL_APB4_GRP1_DisableClockLowPower(uint32_t Periphs)
{
  WRITE_REG(RCC->APB4LPENCR1, Periphs);
}

/**
  * @brief  Enable APB4 peripherals clock.
  * @rmtoll APB4ENSR2    SYSCFGENS     LL_APB4_GRP2_EnableClock\n
  *         APB4ENSR2    BSECENS       LL_APB4_GRP2_EnableClock\n
  *         APB4ENSR2    DTSENS        LL_APB4_GRP2_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP2_PERIPH_BSEC
  *         @arg @ref LL_APB4_GRP2_PERIPH_SYSCFG
  *         @arg @ref LL_APB4_GRP2_PERIPH_DTS
  * @retval None
  */
__STATIC_INLINE void LL_APB4_GRP2_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  WRITE_REG(RCC->APB4ENSR2, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_REG(RCC->APB4ENR2);
  (void)tmpreg;
}

/**
  * @brief  Check if APB4 peripheral clock is enabled or not
  * @rmtoll APB4ENR2     SYSCFGEN      LL_APB4_GRP2_IsEnabledClock\n
  *         APB4ENR2     BSECEN        LL_APB4_GRP2_IsEnabledClock\n
  *         APB4ENR2     DTSEN         LL_APB4_GRP2_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP2_PERIPH_BSEC
  *         @arg @ref LL_APB4_GRP2_PERIPH_SYSCFG
  *         @arg @ref LL_APB4_GRP2_PERIPH_DTS
  * @retval uint32_t
  */
__STATIC_INLINE uint32_t LL_APB4_GRP2_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC->APB4ENR2, Periphs) == Periphs) ? 1UL : 0UL);
}

/**
  * @brief  Disable APB4 peripherals clock.
  * @rmtoll APB4ENCR2    SYSCFGENC     LL_APB4_GRP2_DisableClock\n
  *         APB4ENCR2    BSECENC       LL_APB4_GRP2_DisableClock\n
  *         APB4ENCR2    DTSENC        LL_APB4_GRP2_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP2_PERIPH_BSEC
  *         @arg @ref LL_APB4_GRP2_PERIPH_SYSCFG
  *         @arg @ref LL_APB4_GRP2_PERIPH_DTS
  * @retval None
  */
__STATIC_INLINE void LL_APB4_GRP2_DisableClock(uint32_t Periphs)
{
  WRITE_REG(RCC->APB4ENCR2, Periphs);
}

/**
  * @brief  Force APB4 peripherals reset.
  * @rmtoll APB4RSTR2   SYSCFGRSTS    LL_APB4_GRP2_ForceReset\n
  *         APB4RSTR2   DTSRSTS       LL_APB4_GRP2_ForceReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP2_PERIPH_SYSCFG
  *         @arg @ref LL_APB4_GRP2_PERIPH_DTS
  * @retval None
  */
__STATIC_INLINE void LL_APB4_GRP2_ForceReset(uint32_t Periphs)
{
  WRITE_REG(RCC->APB4RSTSR2, Periphs);
}

/**
  * @brief  Release APB4 peripherals reset.
  * @rmtoll APB4RSTCR2   SYSCFGRSTC    LL_APB4_GRP2_ReleaseReset\n
  *         APB4RSTCR2   DTSRSTC       LL_APB4_GRP2_ReleaseReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP2_PERIPH_SYSCFG
  *         @arg @ref LL_APB4_GRP2_PERIPH_DTS
  * @retval None
  */
__STATIC_INLINE void LL_APB4_GRP2_ReleaseReset(uint32_t Periphs)
{
  WRITE_REG(RCC->APB4RSTCR2, Periphs);
}

/**
  * @brief  Enable APB4 peripherals clock during Low Power mode.
  * @rmtoll APB4LPENSR2  SYSCFGLPENS   LL_APB4_GRP2_EnableClockLowPower\n
  *         APB4LPENSR2  BSECLPENS     LL_APB4_GRP2_EnableClockLowPower\n
  *         APB4LPENSR2  DTSLPENS      LL_APB4_GRP2_EnableClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP2_PERIPH_BSEC
  *         @arg @ref LL_APB4_GRP2_PERIPH_SYSCFG
  *         @arg @ref LL_APB4_GRP2_PERIPH_DTS
  * @retval None
  */
__STATIC_INLINE void LL_APB4_GRP2_EnableClockLowPower(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  WRITE_REG(RCC->APB4LPENSR2, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_REG(RCC->APB4LPENR2);
  (void)tmpreg;
}

/**
  * @brief  Check if APB4 peripheral clock during Low Power mode is enabled or not .
  * @rmtoll APB4LPENR2   SYSCFGLPEN    LL_APB4_GRP2_IsEnabledClockLowPower\n
  *         APB4LPENR2   BSECLPEN      LL_APB4_GRP2_IsEnabledClockLowPower\n
  *         APB4LPENR2   DTSLPEN       LL_APB4_GRP2_IsEnabledClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP2_PERIPH_BSEC
  *         @arg @ref LL_APB4_GRP2_PERIPH_SYSCFG
  *         @arg @ref LL_APB4_GRP2_PERIPH_DTS
  * @retval uint32_t
  */
__STATIC_INLINE uint32_t LL_APB4_GRP2_IsEnabledClockLowPower(uint32_t Periphs)
{
  return ((READ_BIT(RCC->APB4LPENR2, Periphs) == Periphs) ? 1UL : 0UL);
}

/**
  * @brief  Disable APB4 peripherals clock during Low Power mode.
  * @rmtoll APB4LPENCR2  SYSCFGLPENC   LL_APB4_GRP2_DisableClockLowPower\n
  *         APB4LPENCR2  BSECLPENC     LL_APB4_GRP2_DisableClockLowPower\n
  *         APB4LPENCR2  DTSLPENC      LL_APB4_GRP2_DisableClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB4_GRP2_PERIPH_BSEC
  *         @arg @ref LL_APB4_GRP2_PERIPH_SYSCFG
  *         @arg @ref LL_APB4_GRP2_PERIPH_DTS
  * @retval None
  */
__STATIC_INLINE void LL_APB4_GRP2_DisableClockLowPower(uint32_t Periphs)
{
  WRITE_REG(RCC->APB4LPENCR2, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_APB5 APB5
  * @{
  */

/**
  * @brief  Enable APB5 peripherals clock.
  * @rmtoll APB5ENSR     LTDCENS       LL_APB5_GRP1_EnableClock\n
  *         APB5ENSR     DCMIPPENS     LL_APB5_GRP1_EnableClock\n
  *         APB5ENSR     GFXTIMENS     LL_APB5_GRP1_EnableClock\n
  *         APB5ENSR     VENCENS       LL_APB5_GRP1_EnableClock\n (*)
  *         APB5ENSR     CSIENS        LL_APB5_GRP1_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB5_GRP1_PERIPH_CSI
  *         @arg @ref LL_APB5_GRP1_PERIPH_DCMIPP
  *         @arg @ref LL_APB5_GRP1_PERIPH_GFXTIM
  *         @arg @ref LL_APB5_GRP1_PERIPH_LTDC
  *         @arg @ref LL_APB5_GRP1_PERIPH_VENC
  * @retval None
  */
__STATIC_INLINE void LL_APB5_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  WRITE_REG(RCC->APB5ENSR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_REG(RCC->APB5ENR);
  (void)tmpreg;
}

/**
  * @brief  Check if APB5 peripheral clock is enabled or not
  * @rmtoll APB5ENR      LTDCEN        LL_APB5_GRP1_IsEnabledClock\n
  *         APB5ENR      DCMIPPEN      LL_APB5_GRP1_IsEnabledClock\n
  *         APB5ENR      GFXTIMEN      LL_APB5_GRP1_IsEnabledClock\n
  *         APB5ENR      VENCEN        LL_APB5_GRP1_IsEnabledClock\n (*)
  *         APB5ENR      CSIEN         LL_APB5_GRP1_IsEnabledClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB5_GRP1_PERIPH_CSI
  *         @arg @ref LL_APB5_GRP1_PERIPH_DCMIPP
  *         @arg @ref LL_APB5_GRP1_PERIPH_GFXTIM
  *         @arg @ref LL_APB5_GRP1_PERIPH_LTDC
  *         @arg @ref LL_APB5_GRP1_PERIPH_VENC
  * @retval uint32_t
  */
__STATIC_INLINE uint32_t LL_APB5_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC->APB5ENR, Periphs) == Periphs) ? 1UL : 0UL);
}

/**
  * @brief  Disable APB5 peripherals clock.
  * @rmtoll APB5ENCR     LTDCENC       LL_APB5_GRP1_DisableClock\n
  *         APB5ENCR     DCMIPPENC     LL_APB5_GRP1_DisableClock\n
  *         APB5ENCR     GFXTIMENC     LL_APB5_GRP1_DisableClock\n
  *         APB5ENCR     VENCENC       LL_APB5_GRP1_DisableClock\n (*)
  *         APB5ENCR     CSIENC        LL_APB5_GRP1_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB5_GRP1_PERIPH_CSI
  *         @arg @ref LL_APB5_GRP1_PERIPH_DCMIPP
  *         @arg @ref LL_APB5_GRP1_PERIPH_GFXTIM
  *         @arg @ref LL_APB5_GRP1_PERIPH_LTDC
  *         @arg @ref LL_APB5_GRP1_PERIPH_VENC
  * @retval None
  */
__STATIC_INLINE void LL_APB5_GRP1_DisableClock(uint32_t Periphs)
{
  WRITE_REG(RCC->APB5ENCR, Periphs);
}

/**
  * @brief  Force APB5 peripherals reset.
  * @rmtoll APB5RSTSR     LTDCRSTS       LL_APB5_GRP1_ForceReset\n
  *         APB5RSTSR     DCMIPPRSTS     LL_APB5_GRP1_ForceReset\n
  *         APB5RSTSR     GFXTIMRSTS     LL_APB5_GRP1_ForceReset\n
  *         APB5RSTSR     VENCRSTS       LL_APB5_GRP1_ForceReset\n (*)
  *         APB5RSTSR     CSIRSTS        LL_APB5_GRP1_ForceReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB5_GRP1_PERIPH_CSI
  *         @arg @ref LL_APB5_GRP1_PERIPH_DCMIPP
  *         @arg @ref LL_APB5_GRP1_PERIPH_GFXTIM
  *         @arg @ref LL_APB5_GRP1_PERIPH_LTDC
  *         @arg @ref LL_APB5_GRP1_PERIPH_VENC
  * @retval None
  */
__STATIC_INLINE void LL_APB5_GRP1_ForceReset(uint32_t Periphs)
{
  WRITE_REG(RCC->APB5RSTSR, Periphs);
}

/**
  * @brief  Release APB5 peripherals reset.
  * @rmtoll APB5RSTCR     LTDCRSTC       LL_APB5_GRP1_ReleaseReset\n
  *         APB5RSTCR     DCMIPPRSTC     LL_APB5_GRP1_ReleaseReset\n
  *         APB5RSTCR     GFXTIMRSTC     LL_APB5_GRP1_ReleaseReset\n
  *         APB5RSTCR     VENCRSTC       LL_APB5_GRP1_ReleaseReset\n (*)
  *         APB5RSTCR     CSIRSTC        LL_APB5_GRP1_ReleaseReset
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB5_GRP1_PERIPH_CSI
  *         @arg @ref LL_APB5_GRP1_PERIPH_DCMIPP
  *         @arg @ref LL_APB5_GRP1_PERIPH_GFXTIM
  *         @arg @ref LL_APB5_GRP1_PERIPH_LTDC
  *         @arg @ref LL_APB5_GRP1_PERIPH_VENC
  * @retval None
  */
__STATIC_INLINE void LL_APB5_GRP1_ReleaseReset(uint32_t Periphs)
{
  WRITE_REG(RCC->APB5RSTCR, Periphs);
}

/**
  * @brief  Enable APB5 peripherals clock during Low Power mode.
  * @rmtoll APB5LPENSR    LTDCLPENS      LL_APB5_GRP1_EnableClockLowPower\n
  *         APB5LPENSR    DCMIPPLPENS    LL_APB5_GRP1_EnableClockLowPower\n
  *         APB5LPENSR    GFXTIMLPENS    LL_APB5_GRP1_EnableClockLowPower\n
  *         APB5LPENSR    VENCLPENS      LL_APB5_GRP1_EnableClockLowPower\n (*)
  *         APB5LPENSR    CSILPENS       LL_APB5_GRP1_EnableClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB5_GRP1_PERIPH_CSI
  *         @arg @ref LL_APB5_GRP1_PERIPH_DCMIPP
  *         @arg @ref LL_APB5_GRP1_PERIPH_GFXTIM
  *         @arg @ref LL_APB5_GRP1_PERIPH_LTDC
  *         @arg @ref LL_APB5_GRP1_PERIPH_VENC
  * @retval None
  */
__STATIC_INLINE void LL_APB5_GRP1_EnableClockLowPower(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  WRITE_REG(RCC->APB5LPENSR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_REG(RCC->APB5LPENR);
  (void)tmpreg;
}

/**
  * @brief  Check if APB5 peripheral clock during Low Power mode is enabled or not .
  * @rmtoll APB5LPENR     LTDCLPEN       LL_APB5_GRP1_IsEnabledClockLowPower\n
  *         APB5LPENR     DCMIPPLPEN     LL_APB5_GRP1_IsEnabledClockLowPower\n
  *         APB5LPENR     GFXTIMLPEN     LL_APB5_GRP1_IsEnabledClockLowPower\n
  *         APB5LPENR     VENCLPEN       LL_APB5_GRP1_IsEnabledClockLowPower\n (*)
  *         APB5LPENR     CSILPEN        LL_APB5_GRP1_IsEnabledClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB5_GRP1_PERIPH_CSI
  *         @arg @ref LL_APB5_GRP1_PERIPH_DCMIPP
  *         @arg @ref LL_APB5_GRP1_PERIPH_GFXTIM
  *         @arg @ref LL_APB5_GRP1_PERIPH_LTDC
  *         @arg @ref LL_APB5_GRP1_PERIPH_VENC
  * @retval uint32_t
  */
__STATIC_INLINE uint32_t LL_APB5_GRP1_IsEnabledClockLowPower(uint32_t Periphs)
{
  return ((READ_BIT(RCC->APB5LPENR, Periphs) == Periphs) ? 1UL : 0UL);
}

/**
  * @brief  Disable APB5 peripherals clock during Low Power mode.
  * @rmtoll APB5LPENCR    LTDCLPENC      LL_APB5_GRP1_DisableClockLowPower\n
  *         APB5LPENCR    DCMIPPLPENC    LL_APB5_GRP1_DisableClockLowPower\n
  *         APB5LPENCR    GFXTIMLPENC    LL_APB5_GRP1_DisableClockLowPower\n
  *         APB5LPENCR    VENCLPENC      LL_APB5_GRP1_DisableClockLowPower\n (*)
  *         APB5LPENCR    CSILPENC       LL_APB5_GRP1_DisableClockLowPower
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB5_GRP1_PERIPH_CSI
  *         @arg @ref LL_APB5_GRP1_PERIPH_DCMIPP
  *         @arg @ref LL_APB5_GRP1_PERIPH_GFXTIM
  *         @arg @ref LL_APB5_GRP1_PERIPH_LTDC
  *         @arg @ref LL_APB5_GRP1_PERIPH_VENC
  * @retval None
  */
__STATIC_INLINE void LL_APB5_GRP1_DisableClockLowPower(uint32_t Periphs)
{
  WRITE_REG(RCC->APB5LPENCR, Periphs);
}

/**
  * @}
  */

/** @defgroup BUS_LL_EF_MISC MISC
  * @{
  */

/**
  * @brief  Enable miscellaneous clock.
  * @rmtoll MISCENSR     DBGENS        LL_MISC_EnableClock\n
  *         MISCENSR     MCO1ENS       LL_MISC_EnableClock\n
  *         MISCENSR     MCO2ENS       LL_MISC_EnableClock\n
  *         MISCENSR     XSPIPHYCOMPENS LL_MISC_EnableClock\n
  *         MISCENSR     PERENS        LL_MISC_EnableClock
  * @param  Misc This parameter can be a combination of the following values:
  *         @arg @ref LL_DBG
  *         @arg @ref LL_MCO1
  *         @arg @ref LL_MCO2
  *         @arg @ref LL_XSPIPHYCOMP
  *         @arg @ref LL_PER
  * @retval None
  */
__STATIC_INLINE void LL_MISC_EnableClock(uint32_t Misc)
{
  __IO uint32_t tmpreg;
  WRITE_REG(RCC->MISCENSR, Misc);
  /* Delay after an RCC miscellaneous clock enabling */
  tmpreg = READ_REG(RCC->MISCENR);
  (void)tmpreg;
}

/**
  * @brief  Check if miscellaneous clock is enabled or not
  * @rmtoll MISCENR      DBGEN         LL_MISC_IsEnabledClock\n
  *         MISCENR      MCO1EN        LL_MISC_IsEnabledClock\n
  *         MISCENR      MCO2EN        LL_MISC_IsEnabledClock\n
  *         MISCENR      XSPIPHYCOMPEN LL_MISC_IsEnabledClock\n
  *         MISCENR      PEREN         LL_MISC_IsEnabledClock
  * @param  Misc This parameter can be a combination of the following values:
  *         @arg @ref LL_DBG
  *         @arg @ref LL_MCO1
  *         @arg @ref LL_MCO2
  *         @arg @ref LL_XSPIPHYCOMP
  *         @arg @ref LL_PER
  * @retval uint32_t
  */
__STATIC_INLINE uint32_t LL_MISC_IsEnabledClock(uint32_t Misc)
{
  return ((READ_BIT(RCC->MISCENR, Misc) == Misc) ? 1UL : 0UL);
}

/**
  * @brief  Disable miscellaneous clock.
  * @rmtoll MISCENCR     DBGENC        LL_MISC_DisableClock\n
  *         MISCENCR     MCO1ENC       LL_MISC_DisableClock\n
  *         MISCENCR     MCO2ENC       LL_MISC_DisableClock\n
  *         MISCENCR     XSPIPHYCOMPENC LL_MISC_DisableClock\n
  *         MISCENCR     PERENC        LL_MISC_DisableClock
  * @param  Misc This parameter can be a combination of the following values:
  *         @arg @ref LL_DBG
  *         @arg @ref LL_MCO1
  *         @arg @ref LL_MCO2
  *         @arg @ref LL_XSPIPHYCOMP
  *         @arg @ref LL_PER
  * @retval None
  */
__STATIC_INLINE void LL_MISC_DisableClock(uint32_t Misc)
{
  WRITE_REG(RCC->MISCENCR, Misc);
}

/**
  * @brief  Force miscellaneous reset.
  * @rmtoll MISCRSTSR    DBGRSTS       LL_MISC_ForceReset\n
  *         MISCRSTSR    XSPIPHY1RSTS  LL_MISC_ForceReset\n
  *         MISCRSTSR    XSPIPHY2RSTS  LL_MISC_ForceReset\n
  *         MISCRSTSR    SDMMC1DLLRSTS LL_MISC_ForceReset\n
  *         MISCRSTSR    SDMMC2DLLRSTS LL_MISC_ForceReset
  * @param  Misc This parameter can be a combination of the following values:
  *         @arg @ref LL_DBG
  *         @arg @ref LL_XSPIPHY1
  *         @arg @ref LL_XSPIPHY2
  *         @arg @ref LL_SDMMC1DLL
  *         @arg @ref LL_SDMMC2DLL
  * @retval None
  */
__STATIC_INLINE void LL_MISC_ForceReset(uint32_t Misc)
{
  WRITE_REG(RCC->MISCRSTSR, Misc);
}

/**
  * @brief  Release miscellaneous reset.
  * @rmtoll MISCRSTCR    DBGRSTC       LL_MISC_ReleaseReset\n
  *         MISCRSTCR    XSPIPHY1RSTC  LL_MISC_ReleaseReset\n
  *         MISCRSTCR    XSPIPHY2RSTC  LL_MISC_ReleaseReset\n
  *         MISCRSTCR    SDMMC1DLLRSTC LL_MISC_ReleaseReset\n
  *         MISCRSTCR    SDMMC2DLLRSTC LL_MISC_ReleaseReset
  * @param  Misc This parameter can be a combination of the following values:
  *         @arg @ref LL_DBG
  *         @arg @ref LL_XSPIPHY1
  *         @arg @ref LL_XSPIPHY2
  *         @arg @ref LL_SDMMC1DLL
  *         @arg @ref LL_SDMMC2DLL
  * @retval None
  */
__STATIC_INLINE void LL_MISC_ReleaseReset(uint32_t Misc)
{
  WRITE_REG(RCC->MISCRSTCR, Misc);
}

/**
  * @brief  Enable bus clock during Low Power mode.
  * @rmtoll MISCLPENSR   DBGLPENS      LL_MISC_EnableClockLowPower\n
  *         MISCLPENSR   XSPIPHYCOMPLPENS LL_MISC_EnableClockLowPower\n
  *         MISCLPENSR   PERLPENS      LL_MISC_EnableClockLowPower
  * @param  Misc This parameter can be a combination of the following values:
  *         @arg @ref LL_DBG
  *         @arg @ref LL_XSPIPHYCOMP
  *         @arg @ref LL_PER
  * @retval None
  */
__STATIC_INLINE void LL_MISC_EnableClockLowPower(uint32_t Misc)
{
  __IO uint32_t tmpreg;
  WRITE_REG(RCC->MISCLPENSR, Misc);
  /* Delay after an RCC miscellaneous clock enabling */
  tmpreg = READ_REG(RCC->MISCLPENR);
  (void)tmpreg;
}

/**
  * @brief  Disable bus clock during Low Power mode.
  * @rmtoll MISCLPENCR   DBGLPENC      LL_MISC_DisableClockLowPower\n
  *         MISCLPENCR   XSPIPHYCOMPLPENC LL_MISC_DisableClockLowPower\n
  *         MISCLPENCR   PERLPENC      LL_MISC_DisableClockLowPower
  * @param  Misc This parameter can be a combination of the following values:
  *         @arg @ref LL_DBG
  *         @arg @ref LL_XSPIPHYCOMP
  *         @arg @ref LL_PER
  * @retval None
  */
__STATIC_INLINE void LL_MISC_DisableClockLowPower(uint32_t Misc)
{
  WRITE_REG(RCC->MISCLPENCR, Misc);
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif /* defined(RCC) */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32N6xx_LL_BUS_H */
