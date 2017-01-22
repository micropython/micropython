/**
  ******************************************************************************
  * @file    stm32f7xx_hal_rcc_ex.h
  * @author  MCD Application Team
  * @version V1.1.2
  * @date    23-September-2016
  * @brief   Header file of RCC HAL Extension module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F7xx_HAL_RCC_EX_H
#define __STM32F7xx_HAL_RCC_EX_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal_def.h"

/** @addtogroup STM32F7xx_HAL_Driver
  * @{
  */

/** @addtogroup RCCEx
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup RCCEx_Exported_Types RCCEx Exported Types
  * @{
  */

/**
  * @brief  RCC PLL configuration structure definition
  */
typedef struct
{
  uint32_t PLLState;   /*!< The new state of the PLL.
                            This parameter can be a value of @ref RCC_PLL_Config                      */

  uint32_t PLLSource;  /*!< RCC_PLLSource: PLL entry clock source.
                            This parameter must be a value of @ref RCC_PLL_Clock_Source               */

  uint32_t PLLM;       /*!< PLLM: Division factor for PLL VCO input clock.
                            This parameter must be a number between Min_Data = 2 and Max_Data = 63    */

  uint32_t PLLN;       /*!< PLLN: Multiplication factor for PLL VCO output clock.
                            This parameter must be a number between Min_Data = 50 and Max_Data = 432  */

  uint32_t PLLP;       /*!< PLLP: Division factor for main system clock (SYSCLK).
                            This parameter must be a value of @ref RCC_PLLP_Clock_Divider             */

  uint32_t PLLQ;       /*!< PLLQ: Division factor for OTG FS, SDMMC and RNG clocks.
                            This parameter must be a number between Min_Data = 2 and Max_Data = 15    */
#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
  uint32_t PLLR;       /*!< PLLR: Division factor for DSI clock.
                            This parameter must be a number between Min_Data = 2 and Max_Data = 7    */
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */

}RCC_PLLInitTypeDef;

/**
  * @brief  PLLI2S Clock structure definition
  */
typedef struct
{
  uint32_t PLLI2SN;    /*!< Specifies the multiplication factor for PLLI2S VCO output clock.
                            This parameter must be a number between Min_Data = 50 and Max_Data = 432.
                            This parameter will be used only when PLLI2S is selected as Clock Source I2S or SAI */

  uint32_t PLLI2SR;    /*!< Specifies the division factor for I2S clock.
                            This parameter must be a number between Min_Data = 2 and Max_Data = 7.
                            This parameter will be used only when PLLI2S is selected as Clock Source I2S or SAI */

  uint32_t PLLI2SQ;    /*!< Specifies the division factor for SAI1 clock.
                            This parameter must be a number between Min_Data = 2 and Max_Data = 15.
                            This parameter will be used only when PLLI2S is selected as Clock Source SAI */

#if defined (STM32F745xx) || defined (STM32F746xx) || defined (STM32F756xx) || defined (STM32F765xx) || defined (STM32F767xx) || \
    defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
  uint32_t PLLI2SP;    /*!< Specifies the division factor for SPDIF-RX clock.
                            This parameter must be a value of @ref RCCEx_PLLI2SP_Clock_Divider.
                            This parameter will be used only when PLLI2S is selected as Clock Source SPDIF-RX */
#endif /* STM32F745xx || STM32F746xx || STM32F756xx || STM32F765xx || STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
}RCC_PLLI2SInitTypeDef;

/**
  * @brief  PLLSAI Clock structure definition
  */
typedef struct
{
  uint32_t PLLSAIN;    /*!< Specifies the multiplication factor for PLLI2S VCO output clock.
                            This parameter must be a number between Min_Data = 50 and Max_Data = 432.
                            This parameter will be used only when PLLSAI is selected as Clock Source SAI or LTDC */

  uint32_t PLLSAIQ;    /*!< Specifies the division factor for SAI1 clock.
                            This parameter must be a number between Min_Data = 2 and Max_Data = 15.
                            This parameter will be used only when PLLSAI is selected as Clock Source SAI or LTDC */

#if defined (STM32F745xx) || defined (STM32F746xx) || defined (STM32F756xx) || defined (STM32F765xx) || defined (STM32F767xx) || \
    defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
  uint32_t PLLSAIR;    /*!< specifies the division factor for LTDC clock
                            This parameter must be a number between Min_Data = 2 and Max_Data = 7.
                            This parameter will be used only when PLLSAI is selected as Clock Source LTDC */
#endif /* STM32F745xx || STM32F746xx || STM32F756xx || STM32F765xx || STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */

  uint32_t PLLSAIP;    /*!< Specifies the division factor for 48MHz clock.
                            This parameter must be a value of @ref RCCEx_PLLSAIP_Clock_Divider
                            This parameter will be used only when PLLSAI is disabled */
}RCC_PLLSAIInitTypeDef;

/**
  * @brief  RCC extended clocks structure definition
  */
typedef struct
{
  uint32_t PeriphClockSelection; /*!< The Extended Clock to be configured.
                                      This parameter can be a value of @ref RCCEx_Periph_Clock_Selection */

  RCC_PLLI2SInitTypeDef PLLI2S;  /*!< PLL I2S structure parameters.
                                      This parameter will be used only when PLLI2S is selected as Clock Source I2S or SAI */

  RCC_PLLSAIInitTypeDef PLLSAI;  /*!< PLL SAI structure parameters.
                                      This parameter will be used only when PLLI2S is selected as Clock Source SAI or LTDC */

  uint32_t PLLI2SDivQ;           /*!< Specifies the PLLI2S division factor for SAI1 clock.
                                      This parameter must be a number between Min_Data = 1 and Max_Data = 32
                                      This parameter will be used only when PLLI2S is selected as Clock Source SAI */

  uint32_t PLLSAIDivQ;           /*!< Specifies the PLLI2S division factor for SAI1 clock.
                                      This parameter must be a number between Min_Data = 1 and Max_Data = 32
                                      This parameter will be used only when PLLSAI is selected as Clock Source SAI */

  uint32_t PLLSAIDivR;           /*!< Specifies the PLLSAI division factor for LTDC clock.
                                      This parameter must be one value of @ref RCCEx_PLLSAI_DIVR */

  uint32_t RTCClockSelection;      /*!< Specifies RTC Clock source Selection.
                                        This parameter can be a value of @ref RCC_RTC_Clock_Source */

  uint32_t I2sClockSelection;      /*!< Specifies I2S Clock source Selection.
                                        This parameter can be a value of @ref RCCEx_I2S_Clock_Source */

  uint32_t TIMPresSelection;      /*!< Specifies TIM Clock Prescalers Selection.
                                       This parameter can be a value of @ref RCCEx_TIM_Prescaler_Selection */

  uint32_t Sai1ClockSelection;     /*!< Specifies SAI1 Clock Prescalers Selection
                                        This parameter can be a value of @ref RCCEx_SAI1_Clock_Source */

  uint32_t Sai2ClockSelection;     /*!< Specifies SAI2 Clock Prescalers Selection
                                        This parameter can be a value of @ref RCCEx_SAI2_Clock_Source */

  uint32_t Usart1ClockSelection; /*!< USART1 clock source
                                      This parameter can be a value of @ref RCCEx_USART1_Clock_Source */

  uint32_t Usart2ClockSelection; /*!< USART2 clock source
                                      This parameter can be a value of @ref RCCEx_USART2_Clock_Source */

  uint32_t Usart3ClockSelection; /*!< USART3 clock source
                                      This parameter can be a value of @ref RCCEx_USART3_Clock_Source */

  uint32_t Uart4ClockSelection;  /*!< UART4 clock source
                                      This parameter can be a value of @ref RCCEx_UART4_Clock_Source */

  uint32_t Uart5ClockSelection;  /*!< UART5 clock source
                                      This parameter can be a value of @ref RCCEx_UART5_Clock_Source */

  uint32_t Usart6ClockSelection;  /*!< USART6 clock source
                                      This parameter can be a value of @ref RCCEx_USART6_Clock_Source */

  uint32_t Uart7ClockSelection;  /*!< UART7 clock source
                                      This parameter can be a value of @ref RCCEx_UART7_Clock_Source */

  uint32_t Uart8ClockSelection;  /*!< UART8 clock source
                                      This parameter can be a value of @ref RCCEx_UART8_Clock_Source */

  uint32_t I2c1ClockSelection;   /*!< I2C1 clock source
                                      This parameter can be a value of @ref RCCEx_I2C1_Clock_Source */

  uint32_t I2c2ClockSelection;   /*!< I2C2 clock source
                                      This parameter can be a value of @ref RCCEx_I2C2_Clock_Source */

  uint32_t I2c3ClockSelection;   /*!< I2C3 clock source
                                      This parameter can be a value of @ref RCCEx_I2C3_Clock_Source */

  uint32_t I2c4ClockSelection;   /*!< I2C4 clock source
                                      This parameter can be a value of @ref RCCEx_I2C4_Clock_Source */

  uint32_t Lptim1ClockSelection;   /*!< Specifies LPTIM1 clock source
                                        This parameter can be a value of @ref RCCEx_LPTIM1_Clock_Source */

  uint32_t CecClockSelection;      /*!< CEC clock source
                                        This parameter can be a value of @ref RCCEx_CEC_Clock_Source */

  uint32_t Clk48ClockSelection;    /*!< Specifies 48Mhz clock source used by USB OTG FS, RNG and SDMMC
                                        This parameter can be a value of @ref RCCEx_CLK48_Clock_Source */

  uint32_t Sdmmc1ClockSelection;     /*!< SDMMC1 clock source
                                        This parameter can be a value of @ref RCCEx_SDMMC1_Clock_Source */

#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
  uint32_t Sdmmc2ClockSelection;     /*!< SDMMC2 clock source
                                        This parameter can be a value of @ref RCCEx_SDMMC2_Clock_Source */

  uint32_t Dfsdm1ClockSelection;     /*!< DFSDM1 clock source
                                        This parameter can be a value of @ref RCCEx_DFSDM1_Kernel_Clock_Source */

  uint32_t Dfsdm1AudioClockSelection; /*!< DFSDM1 clock source
                                        This parameter can be a value of @ref RCCEx_DFSDM1_AUDIO_Clock_Source */
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
}RCC_PeriphCLKInitTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup RCCEx_Exported_Constants RCCEx Exported Constants
  * @{
  */

/** @defgroup RCCEx_Periph_Clock_Selection RCC Periph Clock Selection
  * @{
  */
#define RCC_PERIPHCLK_I2S             ((uint32_t)0x00000001U)
#if defined(STM32F746xx) || defined(STM32F756xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define RCC_PERIPHCLK_LTDC            ((uint32_t)0x00000008U)
#endif /* STM32F746xx || STM32F756xx || STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
#define RCC_PERIPHCLK_TIM             ((uint32_t)0x00000010U)
#define RCC_PERIPHCLK_RTC             ((uint32_t)0x00000020U)
#define RCC_PERIPHCLK_USART1          ((uint32_t)0x00000040U)
#define RCC_PERIPHCLK_USART2          ((uint32_t)0x00000080U)
#define RCC_PERIPHCLK_USART3          ((uint32_t)0x00000100U)
#define RCC_PERIPHCLK_UART4           ((uint32_t)0x00000200U)
#define RCC_PERIPHCLK_UART5           ((uint32_t)0x00000400U)
#define RCC_PERIPHCLK_USART6          ((uint32_t)0x00000800U)
#define RCC_PERIPHCLK_UART7           ((uint32_t)0x00001000U)
#define RCC_PERIPHCLK_UART8           ((uint32_t)0x00002000U)
#define RCC_PERIPHCLK_I2C1            ((uint32_t)0x00004000U)
#define RCC_PERIPHCLK_I2C2            ((uint32_t)0x00008000U)
#define RCC_PERIPHCLK_I2C3            ((uint32_t)0x00010000U)
#define RCC_PERIPHCLK_I2C4            ((uint32_t)0x00020000U)
#define RCC_PERIPHCLK_LPTIM1          ((uint32_t)0x00040000U)
#define RCC_PERIPHCLK_SAI1            ((uint32_t)0x00080000U)
#define RCC_PERIPHCLK_SAI2            ((uint32_t)0x00100000U)
#define RCC_PERIPHCLK_CLK48           ((uint32_t)0x00200000U)
#define RCC_PERIPHCLK_CEC             ((uint32_t)0x00400000U)
#define RCC_PERIPHCLK_SDMMC1          ((uint32_t)0x00800000U)
#define RCC_PERIPHCLK_SPDIFRX         ((uint32_t)0x01000000U)
#define RCC_PERIPHCLK_PLLI2S          ((uint32_t)0x02000000U)
#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define RCC_PERIPHCLK_SDMMC2          ((uint32_t)0x04000000U)
#define RCC_PERIPHCLK_DFSDM1           ((uint32_t)0x08000000U)
#define RCC_PERIPHCLK_DFSDM1_AUDIO      ((uint32_t)0x10000000U)
#endif /* STM32F765xx || STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */

/**
  * @}
  */

#if defined (STM32F745xx) || defined (STM32F746xx) || defined (STM32F756xx) || defined (STM32F765xx) || defined (STM32F767xx) || \
    defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
/** @defgroup RCCEx_PLLI2SP_Clock_Divider RCCEx PLLI2SP Clock Divider
  * @{
  */
#define RCC_PLLI2SP_DIV2                  ((uint32_t)0x00000000U)
#define RCC_PLLI2SP_DIV4                  ((uint32_t)0x00000001U)
#define RCC_PLLI2SP_DIV6                  ((uint32_t)0x00000002U)
#define RCC_PLLI2SP_DIV8                  ((uint32_t)0x00000003U)
/**
  * @}
  */
#endif /* STM32F745xx || STM32F746xx || STM32F756xx || STM32F765xx || STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */

/** @defgroup RCCEx_PLLSAIP_Clock_Divider RCCEx PLLSAIP Clock Divider
  * @{
  */
#define RCC_PLLSAIP_DIV2                  ((uint32_t)0x00000000U)
#define RCC_PLLSAIP_DIV4                  ((uint32_t)0x00000001U)
#define RCC_PLLSAIP_DIV6                  ((uint32_t)0x00000002U)
#define RCC_PLLSAIP_DIV8                  ((uint32_t)0x00000003U)
/**
  * @}
  */

/** @defgroup RCCEx_PLLSAI_DIVR RCCEx PLLSAI DIVR
  * @{
  */
#define RCC_PLLSAIDIVR_2                ((uint32_t)0x00000000U)
#define RCC_PLLSAIDIVR_4                RCC_DCKCFGR1_PLLSAIDIVR_0
#define RCC_PLLSAIDIVR_8                RCC_DCKCFGR1_PLLSAIDIVR_1
#define RCC_PLLSAIDIVR_16               RCC_DCKCFGR1_PLLSAIDIVR
/**
  * @}
  */

/** @defgroup RCCEx_I2S_Clock_Source RCCEx I2S Clock Source
  * @{
  */
#define RCC_I2SCLKSOURCE_PLLI2S             ((uint32_t)0x00000000U)
#define RCC_I2SCLKSOURCE_EXT                RCC_CFGR_I2SSRC

/**
  * @}
  */


/** @defgroup RCCEx_SAI1_Clock_Source RCCEx SAI1 Clock Source
  * @{
  */
#define RCC_SAI1CLKSOURCE_PLLSAI             ((uint32_t)0x00000000U)
#define RCC_SAI1CLKSOURCE_PLLI2S             RCC_DCKCFGR1_SAI1SEL_0
#define RCC_SAI1CLKSOURCE_PIN                RCC_DCKCFGR1_SAI1SEL_1
#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define RCC_SAI1CLKSOURCE_PLLSRC             RCC_DCKCFGR1_SAI1SEL
#endif /* STM32F765xx || STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
/**
  * @}
  */

/** @defgroup RCCEx_SAI2_Clock_Source RCCEx SAI2 Clock Source
  * @{
  */
#define RCC_SAI2CLKSOURCE_PLLSAI             ((uint32_t)0x00000000U)
#define RCC_SAI2CLKSOURCE_PLLI2S             RCC_DCKCFGR1_SAI2SEL_0
#define RCC_SAI2CLKSOURCE_PIN                RCC_DCKCFGR1_SAI2SEL_1
#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define RCC_SAI2CLKSOURCE_PLLSRC             RCC_DCKCFGR1_SAI2SEL
#endif /* STM32F765xx || STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
/**
  * @}
  */

/** @defgroup RCCEx_CEC_Clock_Source RCCEx CEC Clock Source
  * @{
  */
#define RCC_CECCLKSOURCE_LSE             ((uint32_t)0x00000000U)
#define RCC_CECCLKSOURCE_HSI             RCC_DCKCFGR2_CECSEL /* CEC clock is HSI/488*/
/**
  * @}
  */

/** @defgroup RCCEx_USART1_Clock_Source RCCEx USART1 Clock Source
  * @{
  */
#define RCC_USART1CLKSOURCE_PCLK2      ((uint32_t)0x00000000U)
#define RCC_USART1CLKSOURCE_SYSCLK     RCC_DCKCFGR2_USART1SEL_0
#define RCC_USART1CLKSOURCE_HSI        RCC_DCKCFGR2_USART1SEL_1
#define RCC_USART1CLKSOURCE_LSE        RCC_DCKCFGR2_USART1SEL
/**
  * @}
  */

/** @defgroup RCCEx_USART2_Clock_Source RCCEx USART2 Clock Source
  * @{
  */
#define RCC_USART2CLKSOURCE_PCLK1       ((uint32_t)0x00000000U)
#define RCC_USART2CLKSOURCE_SYSCLK     RCC_DCKCFGR2_USART2SEL_0
#define RCC_USART2CLKSOURCE_HSI        RCC_DCKCFGR2_USART2SEL_1
#define RCC_USART2CLKSOURCE_LSE        RCC_DCKCFGR2_USART2SEL
/**
  * @}
  */

/** @defgroup RCCEx_USART3_Clock_Source RCCEx USART3 Clock Source
  * @{
  */
#define RCC_USART3CLKSOURCE_PCLK1       ((uint32_t)0x00000000U)
#define RCC_USART3CLKSOURCE_SYSCLK     RCC_DCKCFGR2_USART3SEL_0
#define RCC_USART3CLKSOURCE_HSI        RCC_DCKCFGR2_USART3SEL_1
#define RCC_USART3CLKSOURCE_LSE        RCC_DCKCFGR2_USART3SEL
/**
  * @}
  */

/** @defgroup RCCEx_UART4_Clock_Source RCCEx UART4 Clock Source
  * @{
  */
#define RCC_UART4CLKSOURCE_PCLK1        ((uint32_t)0x00000000U)
#define RCC_UART4CLKSOURCE_SYSCLK       RCC_DCKCFGR2_UART4SEL_0
#define RCC_UART4CLKSOURCE_HSI          RCC_DCKCFGR2_UART4SEL_1
#define RCC_UART4CLKSOURCE_LSE          RCC_DCKCFGR2_UART4SEL
/**
  * @}
  */

/** @defgroup RCCEx_UART5_Clock_Source RCCEx UART5 Clock Source
  * @{
  */
#define RCC_UART5CLKSOURCE_PCLK1        ((uint32_t)0x00000000U)
#define RCC_UART5CLKSOURCE_SYSCLK       RCC_DCKCFGR2_UART5SEL_0
#define RCC_UART5CLKSOURCE_HSI          RCC_DCKCFGR2_UART5SEL_1
#define RCC_UART5CLKSOURCE_LSE          RCC_DCKCFGR2_UART5SEL
/**
  * @}
  */

/** @defgroup RCCEx_USART6_Clock_Source RCCEx USART6 Clock Source
  * @{
  */
#define RCC_USART6CLKSOURCE_PCLK2       ((uint32_t)0x00000000U)
#define RCC_USART6CLKSOURCE_SYSCLK      RCC_DCKCFGR2_USART6SEL_0
#define RCC_USART6CLKSOURCE_HSI         RCC_DCKCFGR2_USART6SEL_1
#define RCC_USART6CLKSOURCE_LSE         RCC_DCKCFGR2_USART6SEL
/**
  * @}
  */

/** @defgroup RCCEx_UART7_Clock_Source RCCEx UART7 Clock Source
  * @{
  */
#define RCC_UART7CLKSOURCE_PCLK1       ((uint32_t)0x00000000U)
#define RCC_UART7CLKSOURCE_SYSCLK      RCC_DCKCFGR2_UART7SEL_0
#define RCC_UART7CLKSOURCE_HSI         RCC_DCKCFGR2_UART7SEL_1
#define RCC_UART7CLKSOURCE_LSE         RCC_DCKCFGR2_UART7SEL
/**
  * @}
  */

/** @defgroup RCCEx_UART8_Clock_Source RCCEx UART8 Clock Source
  * @{
  */
#define RCC_UART8CLKSOURCE_PCLK1        ((uint32_t)0x00000000U)
#define RCC_UART8CLKSOURCE_SYSCLK      RCC_DCKCFGR2_UART8SEL_0
#define RCC_UART8CLKSOURCE_HSI         RCC_DCKCFGR2_UART8SEL_1
#define RCC_UART8CLKSOURCE_LSE         RCC_DCKCFGR2_UART8SEL
/**
  * @}
  */

/** @defgroup RCCEx_I2C1_Clock_Source RCCEx I2C1 Clock Source
  * @{
  */
#define RCC_I2C1CLKSOURCE_PCLK1        ((uint32_t)0x00000000U)
#define RCC_I2C1CLKSOURCE_SYSCLK       RCC_DCKCFGR2_I2C1SEL_0
#define RCC_I2C1CLKSOURCE_HSI          RCC_DCKCFGR2_I2C1SEL_1
/**
  * @}
  */

/** @defgroup RCCEx_I2C2_Clock_Source RCCEx I2C2 Clock Source
  * @{
  */
#define RCC_I2C2CLKSOURCE_PCLK1        ((uint32_t)0x00000000U)
#define RCC_I2C2CLKSOURCE_SYSCLK       RCC_DCKCFGR2_I2C2SEL_0
#define RCC_I2C2CLKSOURCE_HSI          RCC_DCKCFGR2_I2C2SEL_1

/**
  * @}
  */

/** @defgroup RCCEx_I2C3_Clock_Source RCCEx I2C3 Clock Source
  * @{
  */
#define RCC_I2C3CLKSOURCE_PCLK1        ((uint32_t)0x00000000U)
#define RCC_I2C3CLKSOURCE_SYSCLK       RCC_DCKCFGR2_I2C3SEL_0
#define RCC_I2C3CLKSOURCE_HSI          RCC_DCKCFGR2_I2C3SEL_1
/**
  * @}
  */

/** @defgroup RCCEx_I2C4_Clock_Source RCCEx I2C4 Clock Source
  * @{
  */
#define RCC_I2C4CLKSOURCE_PCLK1        ((uint32_t)0x00000000U)
#define RCC_I2C4CLKSOURCE_SYSCLK       RCC_DCKCFGR2_I2C4SEL_0
#define RCC_I2C4CLKSOURCE_HSI          RCC_DCKCFGR2_I2C4SEL_1
/**
  * @}
  */

/** @defgroup RCCEx_LPTIM1_Clock_Source RCCEx LPTIM1 Clock Source
  * @{
  */
#define RCC_LPTIM1CLKSOURCE_PCLK       ((uint32_t)0x00000000U)
#define RCC_LPTIM1CLKSOURCE_LSI        RCC_DCKCFGR2_LPTIM1SEL_0
#define RCC_LPTIM1CLKSOURCE_HSI        RCC_DCKCFGR2_LPTIM1SEL_1
#define RCC_LPTIM1CLKSOURCE_LSE        RCC_DCKCFGR2_LPTIM1SEL

/**
  * @}
  */

/** @defgroup RCCEx_CLK48_Clock_Source RCCEx CLK48 Clock Source
  * @{
  */
#define RCC_CLK48SOURCE_PLL         ((uint32_t)0x00000000U)
#define RCC_CLK48SOURCE_PLLSAIP     RCC_DCKCFGR2_CK48MSEL
/**
  * @}
  */

/** @defgroup RCCEx_TIM_Prescaler_Selection RCCEx TIM Prescaler Selection
  * @{
  */
#define RCC_TIMPRES_DESACTIVATED        ((uint32_t)0x00000000U)
#define RCC_TIMPRES_ACTIVATED           RCC_DCKCFGR1_TIMPRE
/**
  * @}
  */

/** @defgroup RCCEx_SDMMC1_Clock_Source RCCEx SDMMC1 Clock Source
  * @{
  */
#define RCC_SDMMC1CLKSOURCE_CLK48              ((uint32_t)0x00000000U)
#define RCC_SDMMC1CLKSOURCE_SYSCLK             RCC_DCKCFGR2_SDMMC1SEL
/**
  * @}
  */

#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
/** @defgroup RCCEx_SDMMC2_Clock_Source RCCEx SDMMC2 Clock Source
  * @{
  */
#define RCC_SDMMC2CLKSOURCE_CLK48              ((uint32_t)0x00000000U)
#define RCC_SDMMC2CLKSOURCE_SYSCLK             RCC_DCKCFGR2_SDMMC2SEL
/**
  * @}
  */

/** @defgroup RCCEx_DFSDM1_Kernel_Clock_Source  RCCEx DFSDM1 Kernel Clock Source
  * @{
  */
#define RCC_DFSDM1CLKSOURCE_PCLK             ((uint32_t)0x00000000U)
#define RCC_DFSDM1CLKSOURCE_SYSCLK           RCC_DCKCFGR1_DFSDM1SEL
/**
  * @}
  */

/** @defgroup RCCEx_DFSDM1_AUDIO_Clock_Source RCCEx DFSDM1 AUDIO Clock Source
  * @{
  */
#define RCC_DFSDM1AUDIOCLKSOURCE_SAI1        ((uint32_t)0x00000000U)
#define RCC_DFSDM1AUDIOCLKSOURCE_SAI2        RCC_DCKCFGR1_ADFSDM1SEL
/**
  * @}
  */
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */

#if defined (STM32F769xx) || defined (STM32F779xx)
/** @defgroup RCCEx_DSI_Clock_Source  RCC DSI Clock Source
  * @{
  */
#define RCC_DSICLKSOURCE_DSIPHY             ((uint32_t)0x00000000U)
#define RCC_DSICLKSOURCE_PLLR               ((uint32_t)RCC_DCKCFGR2_DSISEL)
/**
  * @}
  */
#endif /* STM32F769xx || STM32F779xx */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup RCCEx_Exported_Macros RCCEx Exported Macros
  * @{
  */
/** @defgroup RCCEx_Peripheral_Clock_Enable_Disable RCCEx_Peripheral_Clock_Enable_Disable
  * @brief  Enables or disables the AHB/APB peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

/** @brief  Enables or disables the AHB1 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */
#define __HAL_RCC_BKPSRAM_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_BKPSRAMEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_BKPSRAMEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_DTCMRAMEN_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DTCMRAMEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DTCMRAMEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_DMA2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA2EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_DMA2D_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA2DEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA2DEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_USB_OTG_HS_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_OTGHSEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_OTGHSEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_USB_OTG_HS_ULPI_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_OTGHSULPIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_OTGHSULPIEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_GPIOA_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_GPIOB_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_GPIOC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_GPIOD_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_GPIOE_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOEEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOEEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_GPIOF_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOFEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOFEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_GPIOG_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOGEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOGEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_GPIOH_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOHEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOHEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_GPIOI_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOIEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_GPIOJ_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOJEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOJEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_GPIOK_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOKEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOKEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_BKPSRAM_CLK_DISABLE()         (RCC->AHB1ENR &= ~(RCC_AHB1ENR_BKPSRAMEN))
#define __HAL_RCC_DTCMRAMEN_CLK_DISABLE()       (RCC->AHB1ENR &= ~(RCC_AHB1ENR_DTCMRAMEN))
#define __HAL_RCC_DMA2_CLK_DISABLE()            (RCC->AHB1ENR &= ~(RCC_AHB1ENR_DMA2EN))
#define __HAL_RCC_DMA2D_CLK_DISABLE()           (RCC->AHB1ENR &= ~(RCC_AHB1ENR_DMA2DEN))
#define __HAL_RCC_USB_OTG_HS_CLK_DISABLE()      (RCC->AHB1ENR &= ~(RCC_AHB1ENR_OTGHSEN))
#define __HAL_RCC_USB_OTG_HS_ULPI_CLK_DISABLE() (RCC->AHB1ENR &= ~(RCC_AHB1ENR_OTGHSULPIEN))
#define __HAL_RCC_GPIOA_CLK_DISABLE()           (RCC->AHB1ENR &= ~(RCC_AHB1ENR_GPIOAEN))
#define __HAL_RCC_GPIOB_CLK_DISABLE()           (RCC->AHB1ENR &= ~(RCC_AHB1ENR_GPIOBEN))
#define __HAL_RCC_GPIOC_CLK_DISABLE()           (RCC->AHB1ENR &= ~(RCC_AHB1ENR_GPIOCEN))
#define __HAL_RCC_GPIOD_CLK_DISABLE()           (RCC->AHB1ENR &= ~(RCC_AHB1ENR_GPIODEN))
#define __HAL_RCC_GPIOE_CLK_DISABLE()           (RCC->AHB1ENR &= ~(RCC_AHB1ENR_GPIOEEN))
#define __HAL_RCC_GPIOF_CLK_DISABLE()           (RCC->AHB1ENR &= ~(RCC_AHB1ENR_GPIOFEN))
#define __HAL_RCC_GPIOG_CLK_DISABLE()           (RCC->AHB1ENR &= ~(RCC_AHB1ENR_GPIOGEN))
#define __HAL_RCC_GPIOH_CLK_DISABLE()           (RCC->AHB1ENR &= ~(RCC_AHB1ENR_GPIOHEN))
#define __HAL_RCC_GPIOI_CLK_DISABLE()           (RCC->AHB1ENR &= ~(RCC_AHB1ENR_GPIOIEN))
#define __HAL_RCC_GPIOJ_CLK_DISABLE()           (RCC->AHB1ENR &= ~(RCC_AHB1ENR_GPIOJEN))
#define __HAL_RCC_GPIOK_CLK_DISABLE()           (RCC->AHB1ENR &= ~(RCC_AHB1ENR_GPIOKEN))
/**
  * @brief  Enable ETHERNET clock.
  */
#define __HAL_RCC_ETHMAC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_ETHMACEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_ETHMACEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_ETHMACTX_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_ETHMACTXEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_ETHMACTXEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_ETHMACRX_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_ETHMACRXEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_ETHMACRXEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_ETHMACPTP_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_ETHMACPTPEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_ETHMACPTPEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_ETH_CLK_ENABLE()       do {                            \
                                     __HAL_RCC_ETHMAC_CLK_ENABLE();      \
                                     __HAL_RCC_ETHMACTX_CLK_ENABLE();    \
                                     __HAL_RCC_ETHMACRX_CLK_ENABLE();    \
                                    } while(0)
/**
  * @brief  Disable ETHERNET clock.
  */
#define __HAL_RCC_ETHMAC_CLK_DISABLE()    (RCC->AHB1ENR &= ~(RCC_AHB1ENR_ETHMACEN))
#define __HAL_RCC_ETHMACTX_CLK_DISABLE()  (RCC->AHB1ENR &= ~(RCC_AHB1ENR_ETHMACTXEN))
#define __HAL_RCC_ETHMACRX_CLK_DISABLE()  (RCC->AHB1ENR &= ~(RCC_AHB1ENR_ETHMACRXEN))
#define __HAL_RCC_ETHMACPTP_CLK_DISABLE() (RCC->AHB1ENR &= ~(RCC_AHB1ENR_ETHMACPTPEN))
#define __HAL_RCC_ETH_CLK_DISABLE()       do {                             \
                                      __HAL_RCC_ETHMACTX_CLK_DISABLE();    \
                                      __HAL_RCC_ETHMACRX_CLK_DISABLE();    \
                                      __HAL_RCC_ETHMAC_CLK_DISABLE();      \
                                     } while(0)

/** @brief  Enable or disable the AHB2 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */
#define __HAL_RCC_DCMI_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_DCMIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_DCMIEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#if defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_JPEG_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_JPEGEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_JPEGEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)
#define __HAL_RCC_JPEG_CLK_DISABLE()  (RCC->AHB2ENR &= ~(RCC_AHB2ENR_JPEGEN))
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */

#define __HAL_RCC_RNG_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_RNGEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_RNGEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_USB_OTG_FS_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_OTGFSEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_OTGFSEN);\
                                        UNUSED(tmpreg); \
                                        __HAL_RCC_SYSCFG_CLK_ENABLE();\
                                      } while(0)

#define __HAL_RCC_DCMI_CLK_DISABLE()  (RCC->AHB2ENR &= ~(RCC_AHB2ENR_DCMIEN))
#define __HAL_RCC_RNG_CLK_DISABLE()   (RCC->AHB2ENR &= ~(RCC_AHB2ENR_RNGEN))

#define __HAL_RCC_USB_OTG_FS_CLK_DISABLE() (RCC->AHB2ENR &= ~(RCC_AHB2ENR_OTGFSEN))
#if defined(STM32F756xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_CRYP_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_CRYPEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_CRYPEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_HASH_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_HASHEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_HASHEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_CRYP_CLK_DISABLE()  (RCC->AHB2ENR &= ~(RCC_AHB2ENR_CRYPEN))
#define __HAL_RCC_HASH_CLK_DISABLE()  (RCC->AHB2ENR &= ~(RCC_AHB2ENR_HASHEN))
#endif /* STM32F756x || STM32F777xx || STM32F779xx */

/** @brief  Enables or disables the AHB3 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */
#define __HAL_RCC_FMC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_QSPI_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB3ENR, RCC_AHB3ENR_QSPIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_QSPIEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_FMC_CLK_DISABLE()   (RCC->AHB3ENR &= ~(RCC_AHB3ENR_FMCEN))
#define __HAL_RCC_QSPI_CLK_DISABLE()  (RCC->AHB3ENR &= ~(RCC_AHB3ENR_QSPIEN))

/** @brief  Enable or disable the Low Speed APB (APB1) peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */
#define __HAL_RCC_TIM2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM2EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_TIM3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM3EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_TIM4_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM4EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM4EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_TIM5_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM5EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM5EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_TIM6_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM6EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM6EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_TIM7_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM7EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM7EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_TIM12_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM12EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM12EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_TIM13_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM13EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM13EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_TIM14_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM14EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM14EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_LPTIM1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_LPTIM1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_LPTIM1EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_RTC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_RTCEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_RTCEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_CAN3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_CAN3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_CAN3EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */

#define __HAL_RCC_SPI2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_SPI2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_SPI2EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_SPI3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_SPI3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_SPI3EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_SPDIFRX_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_SPDIFRXEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_SPDIFRXEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_USART2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_USART2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_USART2EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_USART3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_USART3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_USART3EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_UART4_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_UART4EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_UART4EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_UART5_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_UART5EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_UART5EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_I2C1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_I2C2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C2EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_I2C3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C3EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_I2C4_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C4EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C4EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_CAN1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_CAN1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_CAN1EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_CAN2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_CAN2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_CAN2EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_CEC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_CECEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_CECEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_DAC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_DACEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_DACEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_UART7_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_UART7EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_UART7EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_UART8_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_UART8EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_UART8EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_TIM2_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_TIM2EN))
#define __HAL_RCC_TIM3_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_TIM3EN))
#define __HAL_RCC_TIM4_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_TIM4EN))
#define __HAL_RCC_TIM5_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_TIM5EN))
#define __HAL_RCC_TIM6_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_TIM6EN))
#define __HAL_RCC_TIM7_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_TIM7EN))
#define __HAL_RCC_TIM12_CLK_DISABLE()  (RCC->APB1ENR &= ~(RCC_APB1ENR_TIM12EN))
#define __HAL_RCC_TIM13_CLK_DISABLE()  (RCC->APB1ENR &= ~(RCC_APB1ENR_TIM13EN))
#define __HAL_RCC_TIM14_CLK_DISABLE()  (RCC->APB1ENR &= ~(RCC_APB1ENR_TIM14EN))
#define __HAL_RCC_LPTIM1_CLK_DISABLE() (RCC->APB1ENR &= ~(RCC_APB1ENR_LPTIM1EN))
#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_RTC_CLK_DISABLE()    (RCC->APB1ENR &= ~(RCC_APB1ENR_RTCEN))
#define __HAL_RCC_CAN3_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_CAN3EN))
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
#define __HAL_RCC_SPI2_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_SPI2EN))
#define __HAL_RCC_SPI3_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_SPI3EN))
#define __HAL_RCC_SPDIFRX_CLK_DISABLE()(RCC->APB1ENR &= ~(RCC_APB1ENR_SPDIFRXEN))
#define __HAL_RCC_USART2_CLK_DISABLE() (RCC->APB1ENR &= ~(RCC_APB1ENR_USART2EN))
#define __HAL_RCC_USART3_CLK_DISABLE() (RCC->APB1ENR &= ~(RCC_APB1ENR_USART3EN))
#define __HAL_RCC_UART4_CLK_DISABLE()  (RCC->APB1ENR &= ~(RCC_APB1ENR_UART4EN))
#define __HAL_RCC_UART5_CLK_DISABLE()  (RCC->APB1ENR &= ~(RCC_APB1ENR_UART5EN))
#define __HAL_RCC_I2C1_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_I2C1EN))
#define __HAL_RCC_I2C2_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_I2C2EN))
#define __HAL_RCC_I2C3_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_I2C3EN))
#define __HAL_RCC_I2C4_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_I2C4EN))
#define __HAL_RCC_CAN1_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_CAN1EN))
#define __HAL_RCC_CAN2_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_CAN2EN))
#define __HAL_RCC_CEC_CLK_DISABLE()    (RCC->APB1ENR &= ~(RCC_APB1ENR_CECEN))
#define __HAL_RCC_DAC_CLK_DISABLE()    (RCC->APB1ENR &= ~(RCC_APB1ENR_DACEN))
#define __HAL_RCC_UART7_CLK_DISABLE()  (RCC->APB1ENR &= ~(RCC_APB1ENR_UART7EN))
#define __HAL_RCC_UART8_CLK_DISABLE()  (RCC->APB1ENR &= ~(RCC_APB1ENR_UART8EN))

/** @brief  Enable or disable the High Speed APB (APB2) peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */
#define __HAL_RCC_TIM1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM1EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_TIM8_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM8EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM8EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_USART1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_USART6_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART6EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_USART6EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_SDMMC2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SDMMC2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SDMMC2EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */

#define __HAL_RCC_ADC1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_ADC1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_ADC1EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_ADC2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_ADC2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_ADC2EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_ADC3_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_ADC3EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_ADC3EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_SDMMC1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SDMMC1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SDMMC1EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_SPI1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI1EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_SPI4_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI4EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI4EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_TIM9_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM9EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM9EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_TIM10_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM10EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM10EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_TIM11_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM11EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM11EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_SPI5_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI5EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI5EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_SPI6_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI6EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI6EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_SAI1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SAI1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SAI1EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_SAI2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SAI2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SAI2EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#if defined (STM32F746xx) || defined (STM32F756xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_LTDC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_LTDCEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_LTDCEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)
#endif /* STM32F746xx || STM32F756xx || STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */

#if defined (STM32F769xx) || defined (STM32F779xx)
#define __HAL_RCC_DSI_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_DSIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_DSIEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)
#endif /* STM32F769xx || STM32F779xx */

#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_DFSDM1_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_DFSDM1EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_DFSDM1EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_MDIO_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_MDIOEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_MDIOEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */

#define __HAL_RCC_TIM1_CLK_DISABLE()   (RCC->APB2ENR &= ~(RCC_APB2ENR_TIM1EN))
#define __HAL_RCC_TIM8_CLK_DISABLE()   (RCC->APB2ENR &= ~(RCC_APB2ENR_TIM8EN))
#define __HAL_RCC_USART1_CLK_DISABLE() (RCC->APB2ENR &= ~(RCC_APB2ENR_USART1EN))
#define __HAL_RCC_USART6_CLK_DISABLE() (RCC->APB2ENR &= ~(RCC_APB2ENR_USART6EN))
#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_SDMMC2_CLK_DISABLE() (RCC->APB2ENR &= ~(RCC_APB2ENR_SDMMC2EN))
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
#define __HAL_RCC_ADC1_CLK_DISABLE()   (RCC->APB2ENR &= ~(RCC_APB2ENR_ADC1EN))
#define __HAL_RCC_ADC2_CLK_DISABLE()   (RCC->APB2ENR &= ~(RCC_APB2ENR_ADC2EN))
#define __HAL_RCC_ADC3_CLK_DISABLE()   (RCC->APB2ENR &= ~(RCC_APB2ENR_ADC3EN))
#define __HAL_RCC_SDMMC1_CLK_DISABLE() (RCC->APB2ENR &= ~(RCC_APB2ENR_SDMMC1EN))
#define __HAL_RCC_SPI1_CLK_DISABLE()   (RCC->APB2ENR &= ~(RCC_APB2ENR_SPI1EN))
#define __HAL_RCC_SPI4_CLK_DISABLE()   (RCC->APB2ENR &= ~(RCC_APB2ENR_SPI4EN))
#define __HAL_RCC_TIM9_CLK_DISABLE()   (RCC->APB2ENR &= ~(RCC_APB2ENR_TIM9EN))
#define __HAL_RCC_TIM10_CLK_DISABLE()  (RCC->APB2ENR &= ~(RCC_APB2ENR_TIM10EN))
#define __HAL_RCC_TIM11_CLK_DISABLE()  (RCC->APB2ENR &= ~(RCC_APB2ENR_TIM11EN))
#define __HAL_RCC_SPI5_CLK_DISABLE()   (RCC->APB2ENR &= ~(RCC_APB2ENR_SPI5EN))
#define __HAL_RCC_SPI6_CLK_DISABLE()   (RCC->APB2ENR &= ~(RCC_APB2ENR_SPI6EN))
#define __HAL_RCC_SAI1_CLK_DISABLE()   (RCC->APB2ENR &= ~(RCC_APB2ENR_SAI1EN))
#define __HAL_RCC_SAI2_CLK_DISABLE()   (RCC->APB2ENR &= ~(RCC_APB2ENR_SAI2EN))
#if defined (STM32F746xx) || defined (STM32F756xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_LTDC_CLK_DISABLE()   (RCC->APB2ENR &= ~(RCC_APB2ENR_LTDCEN))
#endif /* STM32F746xx || STM32F756xx || STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
#if defined (STM32F769xx) || defined (STM32F779xx)
#define __HAL_RCC_DSI_CLK_DISABLE()    (RCC->APB2ENR &= ~(RCC_APB2ENR_DSIEN))
#endif /* STM32F769xx || STM32F779xx */
#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_DFSDM1_CLK_DISABLE()  (RCC->APB2ENR &= ~(RCC_APB2ENR_DFSDM1EN))
#define __HAL_RCC_MDIO_CLK_DISABLE()  (RCC->APB2ENR &= ~(RCC_APB2ENR_MDIOEN))
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */

/**
  * @}
  */


/** @defgroup RCCEx_Peripheral_Clock_Enable_Disable_Status Peripheral Clock Enable Disable Status
  * @brief  Get the enable or disable status of the AHB/APB peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

/** @brief  Get the enable or disable status of the AHB1 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */
#define __HAL_RCC_BKPSRAM_IS_CLK_ENABLED()          ((RCC->AHB1ENR & (RCC_AHB1ENR_BKPSRAMEN)) != RESET)
#define __HAL_RCC_DTCMRAMEN_IS_CLK_ENABLED()        ((RCC->AHB1ENR & (RCC_AHB1ENR_DTCMRAMEN)) != RESET)
#define __HAL_RCC_DMA2_IS_CLK_ENABLED()             ((RCC->AHB1ENR & (RCC_AHB1ENR_DMA2EN)) != RESET)
#define __HAL_RCC_DMA2D_IS_CLK_ENABLED()            ((RCC->AHB1ENR & (RCC_AHB1ENR_DMA2DEN)) != RESET)
#define __HAL_RCC_USB_OTG_HS_IS_CLK_ENABLED()       ((RCC->AHB1ENR & (RCC_AHB1ENR_OTGHSEN)) != RESET)
#define __HAL_RCC_USB_OTG_HS_ULPI_IS_CLK_ENABLED()  ((RCC->AHB1ENR & (RCC_AHB1ENR_OTGHSULPIEN)) != RESET)
#define __HAL_RCC_GPIOA_IS_CLK_ENABLED()            ((RCC->AHB1ENR & (RCC_AHB1ENR_GPIOAEN)) != RESET)
#define __HAL_RCC_GPIOB_IS_CLK_ENABLED()            ((RCC->AHB1ENR & (RCC_AHB1ENR_GPIOBEN)) != RESET)
#define __HAL_RCC_GPIOC_IS_CLK_ENABLED()            ((RCC->AHB1ENR & (RCC_AHB1ENR_GPIOCEN)) != RESET)
#define __HAL_RCC_GPIOD_IS_CLK_ENABLED()            ((RCC->AHB1ENR & (RCC_AHB1ENR_GPIODEN)) != RESET)
#define __HAL_RCC_GPIOE_IS_CLK_ENABLED()            ((RCC->AHB1ENR & (RCC_AHB1ENR_GPIOEEN)) != RESET)
#define __HAL_RCC_GPIOF_IS_CLK_ENABLED()            ((RCC->AHB1ENR & (RCC_AHB1ENR_GPIOFEN)) != RESET)
#define __HAL_RCC_GPIOG_IS_CLK_ENABLED()            ((RCC->AHB1ENR & (RCC_AHB1ENR_GPIOGEN)) != RESET)
#define __HAL_RCC_GPIOH_IS_CLK_ENABLED()            ((RCC->AHB1ENR & (RCC_AHB1ENR_GPIOHEN)) != RESET)
#define __HAL_RCC_GPIOI_IS_CLK_ENABLED()            ((RCC->AHB1ENR & (RCC_AHB1ENR_GPIOIEN)) != RESET)
#define __HAL_RCC_GPIOJ_IS_CLK_ENABLED()            ((RCC->AHB1ENR & (RCC_AHB1ENR_GPIOJEN)) != RESET)
#define __HAL_RCC_GPIOK_IS_CLK_ENABLED()            ((RCC->AHB1ENR & (RCC_AHB1ENR_GPIOKEN)) != RESET)

#define __HAL_RCC_BKPSRAM_IS_CLK_DISABLED()         ((RCC->AHB1ENR & (RCC_AHB1ENR_BKPSRAMEN)) == RESET)
#define __HAL_RCC_DTCMRAMEN_IS_CLK_DISABLED()       ((RCC->AHB1ENR & (RCC_AHB1ENR_DTCMRAMEN)) == RESET)
#define __HAL_RCC_DMA2_IS_CLK_DISABLED()            ((RCC->AHB1ENR & (RCC_AHB1ENR_DMA2EN)) == RESET)
#define __HAL_RCC_DMA2D_IS_CLK_DISABLED()           ((RCC->AHB1ENR & (RCC_AHB1ENR_DMA2DEN)) == RESET)
#define __HAL_RCC_USB_OTG_HS_IS_CLK_DISABLED()      ((RCC->AHB1ENR & (RCC_AHB1ENR_OTGHSEN)) == RESET)
#define __HAL_RCC_USB_OTG_HS_ULPI_IS_CLK_DISABLED() ((RCC->AHB1ENR & (RCC_AHB1ENR_OTGHSULPIEN)) == RESET)
#define __HAL_RCC_GPIOA_IS_CLK_DISABLED()           ((RCC->AHB1ENR & (RCC_AHB1ENR_GPIOAEN)) == RESET)
#define __HAL_RCC_GPIOB_IS_CLK_DISABLED()           ((RCC->AHB1ENR & (RCC_AHB1ENR_GPIOBEN)) == RESET)
#define __HAL_RCC_GPIOC_IS_CLK_DISABLED()           ((RCC->AHB1ENR & (RCC_AHB1ENR_GPIOCEN)) == RESET)
#define __HAL_RCC_GPIOD_IS_CLK_DISABLED()           ((RCC->AHB1ENR & (RCC_AHB1ENR_GPIODEN)) == RESET)
#define __HAL_RCC_GPIOE_IS_CLK_DISABLED()           ((RCC->AHB1ENR & (RCC_AHB1ENR_GPIOEEN)) == RESET)
#define __HAL_RCC_GPIOF_IS_CLK_DISABLED()           ((RCC->AHB1ENR & (RCC_AHB1ENR_GPIOFEN)) == RESET)
#define __HAL_RCC_GPIOG_IS_CLK_DISABLED()           ((RCC->AHB1ENR & (RCC_AHB1ENR_GPIOGEN)) == RESET)
#define __HAL_RCC_GPIOH_IS_CLK_DISABLED()           ((RCC->AHB1ENR & (RCC_AHB1ENR_GPIOHEN)) == RESET)
#define __HAL_RCC_GPIOI_IS_CLK_DISABLED()           ((RCC->AHB1ENR & (RCC_AHB1ENR_GPIOIEN)) == RESET)
#define __HAL_RCC_GPIOJ_IS_CLK_DISABLED()           ((RCC->AHB1ENR & (RCC_AHB1ENR_GPIOJEN)) == RESET)
#define __HAL_RCC_GPIOK_IS_CLK_DISABLED()           ((RCC->AHB1ENR & (RCC_AHB1ENR_GPIOKEN)) == RESET)
/**
  * @brief  Enable ETHERNET clock.
  */
#define __HAL_RCC_ETHMAC_IS_CLK_ENABLED()     ((RCC->AHB1ENR & (RCC_AHB1ENR_ETHMACEN)) != RESET)
#define __HAL_RCC_ETHMACTX_IS_CLK_ENABLED()   ((RCC->AHB1ENR & (RCC_AHB1ENR_ETHMACTXEN)) != RESET)
#define __HAL_RCC_ETHMACRX_IS_CLK_ENABLED()   ((RCC->AHB1ENR & (RCC_AHB1ENR_ETHMACRXEN)) != RESET)
#define __HAL_RCC_ETHMACPTP_IS_CLK_ENABLED()  ((RCC->AHB1ENR & (RCC_AHB1ENR_ETHMACPTPEN)) != RESET)
#define __HAL_RCC_ETH_IS_CLK_ENABLED()        (__HAL_RCC_ETHMAC_IS_CLK_ENABLED()   && \
                                               __HAL_RCC_ETHMACTX_IS_CLK_ENABLED() && \
                                               __HAL_RCC_ETHMACRX_IS_CLK_ENABLED())

/**
  * @brief  Disable ETHERNET clock.
  */
#define __HAL_RCC_ETHMAC_IS_CLK_DISABLED()    ((RCC->AHB1ENR & (RCC_AHB1ENR_ETHMACEN)) == RESET)
#define __HAL_RCC_ETHMACTX_IS_CLK_DISABLED()  ((RCC->AHB1ENR & (RCC_AHB1ENR_ETHMACTXEN)) == RESET)
#define __HAL_RCC_ETHMACRX_IS_CLK_DISABLED()  ((RCC->AHB1ENR & (RCC_AHB1ENR_ETHMACRXEN)) == RESET)
#define __HAL_RCC_ETHMACPTP_IS_CLK_DISABLED() ((RCC->AHB1ENR & (RCC_AHB1ENR_ETHMACPTPEN)) == RESET)
#define __HAL_RCC_ETH_IS_CLK_DISABLED()        (__HAL_RCC_ETHMAC_IS_CLK_DISABLED()   && \
                                                __HAL_RCC_ETHMACTX_IS_CLK_DISABLED() && \
                                                __HAL_RCC_ETHMACRX_IS_CLK_DISABLED())

/** @brief  Get the enable or disable status of the AHB2 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */
#define __HAL_RCC_DCMI_IS_CLK_ENABLED()        ((RCC->AHB2ENR & (RCC_AHB2ENR_DCMIEN)) != RESET)
#define __HAL_RCC_RNG_IS_CLK_ENABLED()         ((RCC->AHB2ENR & (RCC_AHB2ENR_RNGEN)) != RESET)
#define __HAL_RCC_USB_OTG_FS_IS_CLK_ENABLED()  ((RCC->AHB2ENR & (RCC_AHB2ENR_OTGFSEN)) != RESET)

#define __HAL_RCC_DCMI_IS_CLK_DISABLED()       ((RCC->AHB2ENR & (RCC_AHB2ENR_DCMIEN)) == RESET)
#define __HAL_RCC_RNG_IS_CLK_DISABLED()        ((RCC->AHB2ENR & (RCC_AHB2ENR_RNGEN)) == RESET)
#define __HAL_RCC_USB_IS_OTG_FS_CLK_DISABLED() ((RCC->AHB2ENR & (RCC_AHB2ENR_OTGFSEN)) == RESET)

#if defined(STM32F756xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_CRYP_IS_CLK_ENABLED()   ((RCC->AHB2ENR & (RCC_AHB2ENR_CRYPEN)) != RESET)
#define __HAL_RCC_HASH_IS_CLK_ENABLED()   ((RCC->AHB2ENR & (RCC_AHB2ENR_HASHEN)) != RESET)
#define __HAL_RCC_CRYP_IS_CLK_DISABLED()  ((RCC->AHB2ENR & (RCC_AHB2ENR_CRYPEN)) == RESET)
#define __HAL_RCC_HASH_IS_CLK_DISABLED()  ((RCC->AHB2ENR & (RCC_AHB2ENR_HASHEN)) == RESET)
#endif /* STM32F756xx || STM32F777xx || STM32F779xx */

#if defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_JPEG_IS_CLK_ENABLED()        ((RCC->AHB2ENR & (RCC_AHB2ENR_JPEGEN)) != RESET)
#define __HAL_RCC_JPEG_IS_CLK_DISABLED()       ((RCC->AHB2ENR & (RCC_AHB2ENR_JPEGEN)) == RESET)
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */

/** @brief  Get the enable or disable status of the AHB3 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */
#define __HAL_RCC_FMC_IS_CLK_ENABLED()   ((RCC->AHB3ENR & (RCC_AHB3ENR_FMCEN)) != RESET)
#define __HAL_RCC_QSPI_IS_CLK_ENABLED()  ((RCC->AHB3ENR & (RCC_AHB3ENR_QSPIEN)) != RESET)

#define __HAL_RCC_FMC_IS_CLK_DISABLED()   ((RCC->AHB3ENR & (RCC_AHB3ENR_FMCEN)) == RESET)
#define __HAL_RCC_QSPI_IS_CLK_DISABLED()  ((RCC->AHB3ENR & (RCC_AHB3ENR_QSPIEN)) == RESET)

/** @brief  Get the enable or disable status of the APB1 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */
#define __HAL_RCC_TIM2_IS_CLK_ENABLED()    ((RCC->APB1ENR & (RCC_APB1ENR_TIM2EN)) != RESET)
#define __HAL_RCC_TIM3_IS_CLK_ENABLED()    ((RCC->APB1ENR & (RCC_APB1ENR_TIM3EN)) != RESET)
#define __HAL_RCC_TIM4_IS_CLK_ENABLED()    ((RCC->APB1ENR & (RCC_APB1ENR_TIM4EN)) != RESET)
#define __HAL_RCC_TIM5_IS_CLK_ENABLED()    ((RCC->APB1ENR & (RCC_APB1ENR_TIM5EN)) != RESET)
#define __HAL_RCC_TIM6_IS_CLK_ENABLED()    ((RCC->APB1ENR & (RCC_APB1ENR_TIM6EN)) != RESET)
#define __HAL_RCC_TIM7_IS_CLK_ENABLED()    ((RCC->APB1ENR & (RCC_APB1ENR_TIM7EN)) != RESET)
#define __HAL_RCC_TIM12_IS_CLK_ENABLED()   ((RCC->APB1ENR & (RCC_APB1ENR_TIM12EN)) != RESET)
#define __HAL_RCC_TIM13_IS_CLK_ENABLED()   ((RCC->APB1ENR & (RCC_APB1ENR_TIM13EN)) != RESET)
#define __HAL_RCC_TIM14_IS_CLK_ENABLED()   ((RCC->APB1ENR & (RCC_APB1ENR_TIM14EN)) != RESET)
#define __HAL_RCC_LPTIM1_IS_CLK_ENABLED()  ((RCC->APB1ENR & (RCC_APB1ENR_LPTIM1EN)) != RESET)
#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_RTC_IS_CLK_ENABLED()     ((RCC->APB1ENR & (RCC_APB1ENR_RTCEN)) != RESET)
#define __HAL_RCC_CAN3_IS_CLK_ENABLED()    ((RCC->APB1ENR & (RCC_APB1ENR_CAN3EN)) != RESET)
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
#define __HAL_RCC_SPI2_IS_CLK_ENABLED()    ((RCC->APB1ENR & (RCC_APB1ENR_SPI2EN)) != RESET)
#define __HAL_RCC_SPI3_IS_CLK_ENABLED()    ((RCC->APB1ENR & (RCC_APB1ENR_SPI3EN)) != RESET)
#define __HAL_RCC_SPDIFRX_IS_CLK_ENABLED() ((RCC->APB1ENR & (RCC_APB1ENR_SPDIFRXEN)) != RESET)
#define __HAL_RCC_USART2_IS_CLK_ENABLED()  ((RCC->APB1ENR & (RCC_APB1ENR_USART2EN)) != RESET)
#define __HAL_RCC_USART3_IS_CLK_ENABLED()  ((RCC->APB1ENR & (RCC_APB1ENR_USART3EN)) != RESET)
#define __HAL_RCC_UART4_IS_CLK_ENABLED()   ((RCC->APB1ENR & (RCC_APB1ENR_UART4EN)) != RESET)
#define __HAL_RCC_UART5_IS_CLK_ENABLED()   ((RCC->APB1ENR & (RCC_APB1ENR_UART5EN)) != RESET)
#define __HAL_RCC_I2C1_IS_CLK_ENABLED()    ((RCC->APB1ENR & (RCC_APB1ENR_I2C1EN)) != RESET)
#define __HAL_RCC_I2C2_IS_CLK_ENABLED()    ((RCC->APB1ENR & (RCC_APB1ENR_I2C2EN)) != RESET)
#define __HAL_RCC_I2C3_IS_CLK_ENABLED()    ((RCC->APB1ENR & (RCC_APB1ENR_I2C3EN)) != RESET)
#define __HAL_RCC_I2C4_IS_CLK_ENABLED()    ((RCC->APB1ENR & (RCC_APB1ENR_I2C4EN)) != RESET)
#define __HAL_RCC_CAN1_IS_CLK_ENABLED()    ((RCC->APB1ENR & (RCC_APB1ENR_CAN1EN)) != RESET)
#define __HAL_RCC_CAN2_IS_CLK_ENABLED()    ((RCC->APB1ENR & (RCC_APB1ENR_CAN2EN)) != RESET)
#define __HAL_RCC_CEC_IS_CLK_ENABLED()     ((RCC->APB1ENR & (RCC_APB1ENR_CECEN)) != RESET)
#define __HAL_RCC_DAC_IS_CLK_ENABLED()     ((RCC->APB1ENR & (RCC_APB1ENR_DACEN)) != RESET)
#define __HAL_RCC_UART7_IS_CLK_ENABLED()   ((RCC->APB1ENR & (RCC_APB1ENR_UART7EN)) != RESET)
#define __HAL_RCC_UART8_IS_CLK_ENABLED()   ((RCC->APB1ENR & (RCC_APB1ENR_UART8EN)) != RESET)

#define __HAL_RCC_TIM2_IS_CLK_DISABLED()   ((RCC->APB1ENR & (RCC_APB1ENR_TIM2EN)) == RESET)
#define __HAL_RCC_TIM3_IS_CLK_DISABLED()   ((RCC->APB1ENR & (RCC_APB1ENR_TIM3EN)) == RESET)
#define __HAL_RCC_TIM4_IS_CLK_DISABLED()   ((RCC->APB1ENR & (RCC_APB1ENR_TIM4EN)) == RESET)
#define __HAL_RCC_TIM5_IS_CLK_DISABLED()   ((RCC->APB1ENR & (RCC_APB1ENR_TIM5EN)) == RESET)
#define __HAL_RCC_TIM6_IS_CLK_DISABLED()   ((RCC->APB1ENR & (RCC_APB1ENR_TIM6EN)) == RESET)
#define __HAL_RCC_TIM7_IS_CLK_DISABLED()   ((RCC->APB1ENR & (RCC_APB1ENR_TIM7EN)) == RESET)
#define __HAL_RCC_TIM12_IS_CLK_DISABLED()  ((RCC->APB1ENR & (RCC_APB1ENR_TIM12EN)) == RESET)
#define __HAL_RCC_TIM13_IS_CLK_DISABLED()  ((RCC->APB1ENR & (RCC_APB1ENR_TIM13EN)) == RESET)
#define __HAL_RCC_TIM14_IS_CLK_DISABLED()  ((RCC->APB1ENR & (RCC_APB1ENR_TIM14EN)) == RESET)
#define __HAL_RCC_LPTIM1_IS_CLK_DISABLED() ((RCC->APB1ENR & (RCC_APB1ENR_LPTIM1EN)) == RESET)
#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_RTC_IS_CLK_DISABLED()    ((RCC->APB1ENR & (RCC_APB1ENR_RTCEN)) == RESET)
#define __HAL_RCC_CAN3_IS_CLK_DISABLED()   ((RCC->APB1ENR & (RCC_APB1ENR_CAN3EN)) == RESET)
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
#define __HAL_RCC_SPI2_IS_CLK_DISABLED()   ((RCC->APB1ENR & (RCC_APB1ENR_SPI2EN)) == RESET)
#define __HAL_RCC_SPI3_IS_CLK_DISABLED()   ((RCC->APB1ENR & (RCC_APB1ENR_SPI3EN)) == RESET)
#define __HAL_RCC_SPDIFRX_IS_CLK_DISABLED()((RCC->APB1ENR & (RCC_APB1ENR_SPDIFRXEN)) == RESET)
#define __HAL_RCC_USART2_IS_CLK_DISABLED() ((RCC->APB1ENR & (RCC_APB1ENR_USART2EN)) == RESET)
#define __HAL_RCC_USART3_IS_CLK_DISABLED() ((RCC->APB1ENR & (RCC_APB1ENR_USART3EN)) == RESET)
#define __HAL_RCC_UART4_IS_CLK_DISABLED()  ((RCC->APB1ENR & (RCC_APB1ENR_UART4EN)) == RESET)
#define __HAL_RCC_UART5_IS_CLK_DISABLED()  ((RCC->APB1ENR & (RCC_APB1ENR_UART5EN)) == RESET)
#define __HAL_RCC_I2C1_IS_CLK_DISABLED()   ((RCC->APB1ENR & (RCC_APB1ENR_I2C1EN)) == RESET)
#define __HAL_RCC_I2C2_IS_CLK_DISABLED()   ((RCC->APB1ENR & (RCC_APB1ENR_I2C2EN)) == RESET)
#define __HAL_RCC_I2C3_IS_CLK_DISABLED()   ((RCC->APB1ENR & (RCC_APB1ENR_I2C3EN)) == RESET)
#define __HAL_RCC_I2C4_IS_CLK_DISABLED()   ((RCC->APB1ENR & (RCC_APB1ENR_I2C4EN)) == RESET)
#define __HAL_RCC_CAN1_IS_CLK_DISABLED()   ((RCC->APB1ENR & (RCC_APB1ENR_CAN1EN)) == RESET)
#define __HAL_RCC_CAN2_IS_CLK_DISABLED()   ((RCC->APB1ENR & (RCC_APB1ENR_CAN2EN)) == RESET)
#define __HAL_RCC_CEC_IS_CLK_DISABLED()    ((RCC->APB1ENR & (RCC_APB1ENR_CECEN)) == RESET)
#define __HAL_RCC_DAC_IS_CLK_DISABLED()    ((RCC->APB1ENR & (RCC_APB1ENR_DACEN)) == RESET)
#define __HAL_RCC_UART7_IS_CLK_DISABLED()  ((RCC->APB1ENR & (RCC_APB1ENR_UART7EN)) == RESET)
#define __HAL_RCC_UART8_IS_CLK_DISABLED()  ((RCC->APB1ENR & (RCC_APB1ENR_UART8EN)) == RESET)

/** @brief  Get the enable or disable status of the APB2 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */
#define __HAL_RCC_TIM1_IS_CLK_ENABLED()    ((RCC->APB2ENR & (RCC_APB2ENR_TIM1EN)) != RESET)
#define __HAL_RCC_TIM8_IS_CLK_ENABLED()    ((RCC->APB2ENR & (RCC_APB2ENR_TIM8EN)) != RESET)
#define __HAL_RCC_USART1_IS_CLK_ENABLED()  ((RCC->APB2ENR & (RCC_APB2ENR_USART1EN)) != RESET)
#define __HAL_RCC_USART6_IS_CLK_ENABLED()  ((RCC->APB2ENR & (RCC_APB2ENR_USART6EN)) != RESET)
#define __HAL_RCC_ADC1_IS_CLK_ENABLED()    ((RCC->APB2ENR & (RCC_APB2ENR_ADC1EN)) != RESET)
#define __HAL_RCC_ADC2_IS_CLK_ENABLED()    ((RCC->APB2ENR & (RCC_APB2ENR_ADC2EN)) != RESET)
#define __HAL_RCC_ADC3_IS_CLK_ENABLED()    ((RCC->APB2ENR & (RCC_APB2ENR_ADC3EN)) != RESET)
#define __HAL_RCC_SDMMC1_IS_CLK_ENABLED()  ((RCC->APB2ENR & (RCC_APB2ENR_SDMMC1EN)) != RESET)
#define __HAL_RCC_SPI1_IS_CLK_ENABLED()    ((RCC->APB2ENR & (RCC_APB2ENR_SPI1EN)) != RESET)
#define __HAL_RCC_SPI4_IS_CLK_ENABLED()    ((RCC->APB2ENR & (RCC_APB2ENR_SPI4EN)) != RESET)
#define __HAL_RCC_TIM9_IS_CLK_ENABLED()    ((RCC->APB2ENR & (RCC_APB2ENR_TIM9EN)) != RESET)
#define __HAL_RCC_TIM10_IS_CLK_ENABLED()   ((RCC->APB2ENR & (RCC_APB2ENR_TIM10EN)) != RESET)
#define __HAL_RCC_TIM11_IS_CLK_ENABLED()   ((RCC->APB2ENR & (RCC_APB2ENR_TIM11EN)) != RESET)
#define __HAL_RCC_SPI5_IS_CLK_ENABLED()    ((RCC->APB2ENR & (RCC_APB2ENR_SPI5EN)) != RESET)
#define __HAL_RCC_SPI6_IS_CLK_ENABLED()    ((RCC->APB2ENR & (RCC_APB2ENR_SPI6EN)) != RESET)
#define __HAL_RCC_SAI1_IS_CLK_ENABLED()    ((RCC->APB2ENR & (RCC_APB2ENR_SAI1EN)) != RESET)
#define __HAL_RCC_SAI2_IS_CLK_ENABLED()    ((RCC->APB2ENR & (RCC_APB2ENR_SAI2EN)) != RESET)
#if defined (STM32F746xx) || defined (STM32F756xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_LTDC_IS_CLK_ENABLED()    ((RCC->APB2ENR & (RCC_APB2ENR_LTDCEN)) != RESET)
#endif /* STM32F746xx || STM32F756xx || STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
#if defined (STM32F769xx) || defined (STM32F779xx)
#define __HAL_RCC_DSI_IS_CLK_ENABLED()     ((RCC->APB2ENR & (RCC_APB2ENR_DSIEN)) != RESET)
#endif /* STM32F769xx || STM32F779xx */
#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_SDMMC2_IS_CLK_ENABLED()  ((RCC->APB2ENR & (RCC_APB2ENR_SDMMC2EN)) != RESET)
#define __HAL_RCC_DFSDM1_IS_CLK_ENABLED()   ((RCC->APB2ENR & (RCC_APB2ENR_DFSDM1EN)) != RESET)
#define __HAL_RCC_MDIO_IS_CLK_ENABLED()    ((RCC->APB2ENR & (RCC_APB2ENR_MDIOEN)) != RESET)
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
#define __HAL_RCC_TIM1_IS_CLK_DISABLED()   ((RCC->APB2ENR & (RCC_APB2ENR_TIM1EN)) == RESET)
#define __HAL_RCC_TIM8_IS_CLK_DISABLED()   ((RCC->APB2ENR & (RCC_APB2ENR_TIM8EN)) == RESET)
#define __HAL_RCC_USART1_IS_CLK_DISABLED() ((RCC->APB2ENR & (RCC_APB2ENR_USART1EN)) == RESET)
#define __HAL_RCC_USART6_IS_CLK_DISABLED() ((RCC->APB2ENR & (RCC_APB2ENR_USART6EN)) == RESET)
#define __HAL_RCC_ADC1_IS_CLK_DISABLED()   ((RCC->APB2ENR & (RCC_APB2ENR_ADC1EN)) == RESET)
#define __HAL_RCC_ADC2_IS_CLK_DISABLED()   ((RCC->APB2ENR & (RCC_APB2ENR_ADC2EN)) == RESET)
#define __HAL_RCC_ADC3_IS_CLK_DISABLED()   ((RCC->APB2ENR & (RCC_APB2ENR_ADC3EN)) == RESET)
#define __HAL_RCC_SDMMC1_IS_CLK_DISABLED() ((RCC->APB2ENR & (RCC_APB2ENR_SDMMC1EN)) == RESET)
#define __HAL_RCC_SPI1_IS_CLK_DISABLED()   ((RCC->APB2ENR & (RCC_APB2ENR_SPI1EN)) == RESET)
#define __HAL_RCC_SPI4_IS_CLK_DISABLED()   ((RCC->APB2ENR & (RCC_APB2ENR_SPI4EN)) == RESET)
#define __HAL_RCC_TIM9_IS_CLK_DISABLED()   ((RCC->APB2ENR & (RCC_APB2ENR_TIM9EN)) == RESET)
#define __HAL_RCC_TIM10_IS_CLK_DISABLED()  ((RCC->APB2ENR & (RCC_APB2ENR_TIM10EN)) == RESET)
#define __HAL_RCC_TIM11_IS_CLK_DISABLED()  ((RCC->APB2ENR & (RCC_APB2ENR_TIM11EN)) == RESET)
#define __HAL_RCC_SPI5_IS_CLK_DISABLED()   ((RCC->APB2ENR & (RCC_APB2ENR_SPI5EN)) == RESET)
#define __HAL_RCC_SPI6_IS_CLK_DISABLED()   ((RCC->APB2ENR & (RCC_APB2ENR_SPI6EN)) == RESET)
#define __HAL_RCC_SAI1_IS_CLK_DISABLED()   ((RCC->APB2ENR & (RCC_APB2ENR_SAI1EN)) == RESET)
#define __HAL_RCC_SAI2_IS_CLK_DISABLED()   ((RCC->APB2ENR & (RCC_APB2ENR_SAI2EN)) == RESET)
#if defined (STM32F746xx) || defined (STM32F756xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_LTDC_IS_CLK_DISABLED()   ((RCC->APB2ENR & (RCC_APB2ENR_LTDCEN)) == RESET)
#endif /* STM32F746xx || STM32F756xx || STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
#if defined (STM32F769xx) || defined (STM32F779xx)
#define __HAL_RCC_DSI_IS_CLK_DISABLED()    ((RCC->APB2ENR & (RCC_APB2ENR_DSIEN)) == RESET)
#endif /* STM32F769xx || STM32F779xx */
#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_SDMMC2_IS_CLK_DISABLED() ((RCC->APB2ENR & (RCC_APB2ENR_SDMMC2EN)) == RESET)
#define __HAL_RCC_DFSDM1_IS_CLK_DISABLED()  ((RCC->APB2ENR & (RCC_APB2ENR_DFSDM1EN)) == RESET)
#define __HAL_RCC_MDIO_IS_CLK_DISABLED()   ((RCC->APB2ENR & (RCC_APB2ENR_MDIOEN)) == RESET)
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
/**
  * @}
  */

/** @defgroup RCCEx_Force_Release_Peripheral_Reset RCCEx Force Release Peripheral Reset
  * @brief  Forces or releases AHB/APB peripheral reset.
  * @{
  */

/** @brief  Force or release AHB1 peripheral reset.
  */
#define __HAL_RCC_DMA2_FORCE_RESET()    (RCC->AHB1RSTR |= (RCC_AHB1RSTR_DMA2RST))
#define __HAL_RCC_DMA2D_FORCE_RESET()    (RCC->AHB1RSTR |= (RCC_AHB1RSTR_DMA2DRST))
#define __HAL_RCC_ETHMAC_FORCE_RESET()   (RCC->AHB1RSTR |= (RCC_AHB1RSTR_ETHMACRST))
#define __HAL_RCC_USB_OTG_HS_FORCE_RESET()    (RCC->AHB1RSTR |= (RCC_AHB1RSTR_OTGHRST))
#define __HAL_RCC_GPIOA_FORCE_RESET()   (RCC->AHB1RSTR |= (RCC_AHB1RSTR_GPIOARST))
#define __HAL_RCC_GPIOB_FORCE_RESET()   (RCC->AHB1RSTR |= (RCC_AHB1RSTR_GPIOBRST))
#define __HAL_RCC_GPIOC_FORCE_RESET()   (RCC->AHB1RSTR |= (RCC_AHB1RSTR_GPIOCRST))
#define __HAL_RCC_GPIOD_FORCE_RESET()   (RCC->AHB1RSTR |= (RCC_AHB1RSTR_GPIODRST))
#define __HAL_RCC_GPIOE_FORCE_RESET()   (RCC->AHB1RSTR |= (RCC_AHB1RSTR_GPIOERST))
#define __HAL_RCC_GPIOF_FORCE_RESET()   (RCC->AHB1RSTR |= (RCC_AHB1RSTR_GPIOFRST))
#define __HAL_RCC_GPIOG_FORCE_RESET()   (RCC->AHB1RSTR |= (RCC_AHB1RSTR_GPIOGRST))
#define __HAL_RCC_GPIOH_FORCE_RESET()   (RCC->AHB1RSTR |= (RCC_AHB1RSTR_GPIOHRST))
#define __HAL_RCC_GPIOI_FORCE_RESET()   (RCC->AHB1RSTR |= (RCC_AHB1RSTR_GPIOIRST))
#define __HAL_RCC_GPIOJ_FORCE_RESET()   (RCC->AHB1RSTR |= (RCC_AHB1RSTR_GPIOJRST))
#define __HAL_RCC_GPIOK_FORCE_RESET()   (RCC->AHB1RSTR |= (RCC_AHB1RSTR_GPIOKRST))

#define __HAL_RCC_DMA2_RELEASE_RESET()  (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_DMA2RST))
#define __HAL_RCC_DMA2D_RELEASE_RESET()  (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_DMA2DRST))
#define __HAL_RCC_ETHMAC_RELEASE_RESET() (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_ETHMACRST))
#define __HAL_RCC_USB_OTG_HS_RELEASE_RESET()  (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_OTGHRST))
#define __HAL_RCC_GPIOA_RELEASE_RESET() (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_GPIOARST))
#define __HAL_RCC_GPIOB_RELEASE_RESET() (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_GPIOBRST))
#define __HAL_RCC_GPIOC_RELEASE_RESET() (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_GPIOCRST))
#define __HAL_RCC_GPIOD_RELEASE_RESET() (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_GPIODRST))
#define __HAL_RCC_GPIOE_RELEASE_RESET() (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_GPIOERST))
#define __HAL_RCC_GPIOF_RELEASE_RESET() (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_GPIOFRST))
#define __HAL_RCC_GPIOG_RELEASE_RESET() (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_GPIOGRST))
#define __HAL_RCC_GPIOH_RELEASE_RESET() (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_GPIOHRST))
#define __HAL_RCC_GPIOI_RELEASE_RESET() (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_GPIOIRST))
#define __HAL_RCC_GPIOJ_RELEASE_RESET() (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_GPIOJRST))
#define __HAL_RCC_GPIOK_RELEASE_RESET() (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_GPIOKRST))

/** @brief  Force or release AHB2 peripheral reset.
  */
#define __HAL_RCC_AHB2_FORCE_RESET()    (RCC->AHB2RSTR = 0xFFFFFFFFU)
#define __HAL_RCC_DCMI_FORCE_RESET()   (RCC->AHB2RSTR |= (RCC_AHB2RSTR_DCMIRST))
#define __HAL_RCC_RNG_FORCE_RESET()    (RCC->AHB2RSTR |= (RCC_AHB2RSTR_RNGRST))
#define __HAL_RCC_USB_OTG_FS_FORCE_RESET()   (RCC->AHB2RSTR |= (RCC_AHB2RSTR_OTGFSRST))

#define __HAL_RCC_AHB2_RELEASE_RESET()  (RCC->AHB2RSTR = 0x00U)
#define __HAL_RCC_DCMI_RELEASE_RESET() (RCC->AHB2RSTR &= ~(RCC_AHB2RSTR_DCMIRST))
#define __HAL_RCC_RNG_RELEASE_RESET()  (RCC->AHB2RSTR &= ~(RCC_AHB2RSTR_RNGRST))
#define __HAL_RCC_USB_OTG_FS_RELEASE_RESET() (RCC->AHB2RSTR &= ~(RCC_AHB2RSTR_OTGFSRST))

#if defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_JPEG_FORCE_RESET()   (RCC->AHB2RSTR |= (RCC_AHB2RSTR_JPEGRST))
#define __HAL_RCC_JPEG_RELEASE_RESET() (RCC->AHB2RSTR &= ~(RCC_AHB2RSTR_JPEGRST))
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */

#if defined(STM32F756xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_CRYP_FORCE_RESET()   (RCC->AHB2RSTR |= (RCC_AHB2RSTR_CRYPRST))
#define __HAL_RCC_HASH_FORCE_RESET()   (RCC->AHB2RSTR |= (RCC_AHB2RSTR_HASHRST))
#define __HAL_RCC_CRYP_RELEASE_RESET() (RCC->AHB2RSTR &= ~(RCC_AHB2RSTR_CRYPRST))
#define __HAL_RCC_HASH_RELEASE_RESET() (RCC->AHB2RSTR &= ~(RCC_AHB2RSTR_HASHRST))
#endif /* STM32F756xx || STM32F777xx || STM32F779xx */

/** @brief  Force or release AHB3 peripheral reset
  */
#define __HAL_RCC_AHB3_FORCE_RESET()   (RCC->AHB3RSTR = 0xFFFFFFFFU)
#define __HAL_RCC_FMC_FORCE_RESET()    (RCC->AHB3RSTR |= (RCC_AHB3RSTR_FMCRST))
#define __HAL_RCC_QSPI_FORCE_RESET()   (RCC->AHB3RSTR |= (RCC_AHB3RSTR_QSPIRST))

#define __HAL_RCC_AHB3_RELEASE_RESET() (RCC->AHB3RSTR = 0x00U)
#define __HAL_RCC_FMC_RELEASE_RESET()  (RCC->AHB3RSTR &= ~(RCC_AHB3RSTR_FMCRST))
#define __HAL_RCC_QSPI_RELEASE_RESET() (RCC->AHB3RSTR &= ~(RCC_AHB3RSTR_QSPIRST))

/** @brief  Force or release APB1 peripheral reset.
  */
#define __HAL_RCC_TIM2_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_TIM2RST))
#define __HAL_RCC_TIM3_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_TIM3RST))
#define __HAL_RCC_TIM4_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_TIM4RST))
#define __HAL_RCC_TIM5_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_TIM5RST))
#define __HAL_RCC_TIM6_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_TIM6RST))
#define __HAL_RCC_TIM7_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_TIM7RST))
#define __HAL_RCC_TIM12_FORCE_RESET()    (RCC->APB1RSTR |= (RCC_APB1RSTR_TIM12RST))
#define __HAL_RCC_TIM13_FORCE_RESET()    (RCC->APB1RSTR |= (RCC_APB1RSTR_TIM13RST))
#define __HAL_RCC_TIM14_FORCE_RESET()    (RCC->APB1RSTR |= (RCC_APB1RSTR_TIM14RST))
#define __HAL_RCC_LPTIM1_FORCE_RESET()   (RCC->APB1RSTR |= (RCC_APB1RSTR_LPTIM1RST))
#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_CAN3_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_CAN3RST))
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
#define __HAL_RCC_SPI2_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_SPI2RST))
#define __HAL_RCC_SPI3_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_SPI3RST))
#define __HAL_RCC_SPDIFRX_FORCE_RESET()  (RCC->APB1RSTR |= (RCC_APB1RSTR_SPDIFRXRST))
#define __HAL_RCC_USART2_FORCE_RESET()   (RCC->APB1RSTR |= (RCC_APB1RSTR_USART2RST))
#define __HAL_RCC_USART3_FORCE_RESET()   (RCC->APB1RSTR |= (RCC_APB1RSTR_USART3RST))
#define __HAL_RCC_UART4_FORCE_RESET()    (RCC->APB1RSTR |= (RCC_APB1RSTR_UART4RST))
#define __HAL_RCC_UART5_FORCE_RESET()    (RCC->APB1RSTR |= (RCC_APB1RSTR_UART5RST))
#define __HAL_RCC_I2C1_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_I2C1RST))
#define __HAL_RCC_I2C2_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_I2C2RST))
#define __HAL_RCC_I2C3_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_I2C3RST))
#define __HAL_RCC_I2C4_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_I2C4RST))
#define __HAL_RCC_CAN1_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_CAN1RST))
#define __HAL_RCC_CAN2_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_CAN2RST))
#define __HAL_RCC_CEC_FORCE_RESET()      (RCC->APB1RSTR |= (RCC_APB1RSTR_CECRST))
#define __HAL_RCC_DAC_FORCE_RESET()      (RCC->APB1RSTR |= (RCC_APB1RSTR_DACRST))
#define __HAL_RCC_UART7_FORCE_RESET()    (RCC->APB1RSTR |= (RCC_APB1RSTR_UART7RST))
#define __HAL_RCC_UART8_FORCE_RESET()    (RCC->APB1RSTR |= (RCC_APB1RSTR_UART8RST))

#define __HAL_RCC_TIM2_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM2RST))
#define __HAL_RCC_TIM3_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM3RST))
#define __HAL_RCC_TIM4_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM4RST))
#define __HAL_RCC_TIM5_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM5RST))
#define __HAL_RCC_TIM6_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM6RST))
#define __HAL_RCC_TIM7_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM7RST))
#define __HAL_RCC_TIM12_RELEASE_RESET()  (RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM12RST))
#define __HAL_RCC_TIM13_RELEASE_RESET()  (RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM13RST))
#define __HAL_RCC_TIM14_RELEASE_RESET()  (RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM14RST))
#define __HAL_RCC_LPTIM1_RELEASE_RESET() (RCC->APB1RSTR &= ~(RCC_APB1RSTR_LPTIM1RST))
#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_CAN3_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_CAN3RST))
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
#define __HAL_RCC_SPI2_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_SPI2RST))
#define __HAL_RCC_SPI3_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_SPI3RST))
#define __HAL_RCC_SPDIFRX_RELEASE_RESET()(RCC->APB1RSTR &= ~(RCC_APB1RSTR_SPDIFRXRST))
#define __HAL_RCC_USART2_RELEASE_RESET() (RCC->APB1RSTR &= ~(RCC_APB1RSTR_USART2RST))
#define __HAL_RCC_USART3_RELEASE_RESET() (RCC->APB1RSTR &= ~(RCC_APB1RSTR_USART3RST))
#define __HAL_RCC_UART4_RELEASE_RESET()  (RCC->APB1RSTR &= ~(RCC_APB1RSTR_UART4RST))
#define __HAL_RCC_UART5_RELEASE_RESET()  (RCC->APB1RSTR &= ~(RCC_APB1RSTR_UART5RST))
#define __HAL_RCC_I2C1_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_I2C1RST))
#define __HAL_RCC_I2C2_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_I2C2RST))
#define __HAL_RCC_I2C3_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_I2C3RST))
#define __HAL_RCC_I2C4_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_I2C4RST))
#define __HAL_RCC_CAN1_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_CAN1RST))
#define __HAL_RCC_CAN2_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_CAN2RST))
#define __HAL_RCC_CEC_RELEASE_RESET()    (RCC->APB1RSTR &= ~(RCC_APB1RSTR_CECRST))
#define __HAL_RCC_DAC_RELEASE_RESET()    (RCC->APB1RSTR &= ~(RCC_APB1RSTR_DACRST))
#define __HAL_RCC_UART7_RELEASE_RESET()  (RCC->APB1RSTR &= ~(RCC_APB1RSTR_UART7RST))
#define __HAL_RCC_UART8_RELEASE_RESET()  (RCC->APB1RSTR &= ~(RCC_APB1RSTR_UART8RST))

/** @brief  Force or release APB2 peripheral reset.
  */
#define __HAL_RCC_TIM1_FORCE_RESET()     (RCC->APB2RSTR |= (RCC_APB2RSTR_TIM1RST))
#define __HAL_RCC_TIM8_FORCE_RESET()     (RCC->APB2RSTR |= (RCC_APB2RSTR_TIM8RST))
#define __HAL_RCC_USART1_FORCE_RESET()   (RCC->APB2RSTR |= (RCC_APB2RSTR_USART1RST))
#define __HAL_RCC_USART6_FORCE_RESET()   (RCC->APB2RSTR |= (RCC_APB2RSTR_USART6RST))
#define __HAL_RCC_ADC_FORCE_RESET()      (RCC->APB2RSTR |= (RCC_APB2RSTR_ADCRST))
#define __HAL_RCC_SDMMC1_FORCE_RESET()   (RCC->APB2RSTR |= (RCC_APB2RSTR_SDMMC1RST))
#define __HAL_RCC_SPI1_FORCE_RESET()     (RCC->APB2RSTR |= (RCC_APB2RSTR_SPI1RST))
#define __HAL_RCC_SPI4_FORCE_RESET()     (RCC->APB2RSTR |= (RCC_APB2RSTR_SPI4RST))
#define __HAL_RCC_TIM9_FORCE_RESET()     (RCC->APB2RSTR |= (RCC_APB2RSTR_TIM9RST))
#define __HAL_RCC_TIM10_FORCE_RESET()    (RCC->APB2RSTR |= (RCC_APB2RSTR_TIM10RST))
#define __HAL_RCC_TIM11_FORCE_RESET()    (RCC->APB2RSTR |= (RCC_APB2RSTR_TIM11RST))
#define __HAL_RCC_SPI5_FORCE_RESET()     (RCC->APB2RSTR |= (RCC_APB2RSTR_SPI5RST))
#define __HAL_RCC_SPI6_FORCE_RESET()     (RCC->APB2RSTR |= (RCC_APB2RSTR_SPI6RST))
#define __HAL_RCC_SAI1_FORCE_RESET()     (RCC->APB2RSTR |= (RCC_APB2RSTR_SAI1RST))
#define __HAL_RCC_SAI2_FORCE_RESET()     (RCC->APB2RSTR |= (RCC_APB2RSTR_SAI2RST))
#if defined (STM32F746xx) || defined (STM32F756xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_LTDC_FORCE_RESET()     (RCC->APB2RSTR |= (RCC_APB2RSTR_LTDCRST))
#endif /* STM32F746xx || STM32F756xx || STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */

#define __HAL_RCC_TIM1_RELEASE_RESET()   (RCC->APB2RSTR &= ~(RCC_APB2RSTR_TIM1RST))
#define __HAL_RCC_TIM8_RELEASE_RESET()   (RCC->APB2RSTR &= ~(RCC_APB2RSTR_TIM8RST))
#define __HAL_RCC_USART1_RELEASE_RESET() (RCC->APB2RSTR &= ~(RCC_APB2RSTR_USART1RST))
#define __HAL_RCC_USART6_RELEASE_RESET() (RCC->APB2RSTR &= ~(RCC_APB2RSTR_USART6RST))
#define __HAL_RCC_ADC_RELEASE_RESET()    (RCC->APB2RSTR &= ~(RCC_APB2RSTR_ADCRST))
#define __HAL_RCC_SDMMC1_RELEASE_RESET() (RCC->APB2RSTR &= ~(RCC_APB2RSTR_SDMMC1RST))
#define __HAL_RCC_SPI1_RELEASE_RESET()   (RCC->APB2RSTR &= ~(RCC_APB2RSTR_SPI1RST))
#define __HAL_RCC_SPI4_RELEASE_RESET()   (RCC->APB2RSTR &= ~(RCC_APB2RSTR_SPI4RST))
#define __HAL_RCC_TIM9_RELEASE_RESET()   (RCC->APB2RSTR &= ~(RCC_APB2RSTR_TIM9RST))
#define __HAL_RCC_TIM10_RELEASE_RESET()  (RCC->APB2RSTR &= ~(RCC_APB2RSTR_TIM10RST))
#define __HAL_RCC_TIM11_RELEASE_RESET()  (RCC->APB2RSTR &= ~(RCC_APB2RSTR_TIM11RST))
#define __HAL_RCC_SPI5_RELEASE_RESET()   (RCC->APB2RSTR &= ~(RCC_APB2RSTR_SPI5RST))
#define __HAL_RCC_SPI6_RELEASE_RESET()   (RCC->APB2RSTR &= ~(RCC_APB2RSTR_SPI6RST))
#define __HAL_RCC_SAI1_RELEASE_RESET()   (RCC->APB2RSTR &= ~(RCC_APB2RSTR_SAI1RST))
#define __HAL_RCC_SAI2_RELEASE_RESET()   (RCC->APB2RSTR &= ~(RCC_APB2RSTR_SAI2RST))
#if defined (STM32F746xx) || defined (STM32F756xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_LTDC_RELEASE_RESET()   (RCC->APB2RSTR &= ~(RCC_APB2RSTR_LTDCRST))
#endif /* STM32F746xx || STM32F756xx || STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */

#if defined (STM32F769xx) || defined (STM32F779xx)
#define __HAL_RCC_DSI_FORCE_RESET()      (RCC->APB2RSTR |= (RCC_APB2RSTR_DSIRST))
#define __HAL_RCC_DSI_RELEASE_RESET()    (RCC->APB2RSTR &= ~(RCC_APB2RSTR_DSIRST))
#endif /* STM32F769xx || STM32F779xx */

#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_SDMMC2_FORCE_RESET()   (RCC->APB2RSTR |= (RCC_APB2RSTR_SDMMC2RST))
#define __HAL_RCC_DFSDM1_FORCE_RESET()    (RCC->APB2RSTR |= (RCC_APB2RSTR_DFSDM1RST))
#define __HAL_RCC_MDIO_FORCE_RESET()    (RCC->APB2RSTR |= (RCC_APB2RSTR_MDIORST))

#define __HAL_RCC_SDMMC2_RELEASE_RESET() (RCC->APB2RSTR &= ~(RCC_APB2RSTR_SDMMC2RST))
#define __HAL_RCC_DFSDM1_RELEASE_RESET()  (RCC->APB2RSTR &= ~(RCC_APB2RSTR_DFSDM1RST))
#define __HAL_RCC_MDIO_RELEASE_RESET()  (RCC->APB2RSTR &= ~(RCC_APB2RSTR_MDIORST))
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
/**
  * @}
  */

/** @defgroup RCCEx_Peripheral_Clock_Sleep_Enable_Disable RCCEx Peripheral Clock Sleep Enable Disable
  * @brief  Enables or disables the AHB/APB peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  * @{
  */

/** @brief  Enable or disable the AHB1 peripheral clock during Low Power (Sleep) mode.
  */
#define __HAL_RCC_FLITF_CLK_SLEEP_ENABLE()      (RCC->AHB1LPENR |= (RCC_AHB1LPENR_FLITFLPEN))
#define __HAL_RCC_AXI_CLK_SLEEP_ENABLE()        (RCC->AHB1LPENR |= (RCC_AHB1LPENR_AXILPEN))
#define __HAL_RCC_SRAM1_CLK_SLEEP_ENABLE()      (RCC->AHB1LPENR |= (RCC_AHB1LPENR_SRAM1LPEN))
#define __HAL_RCC_SRAM2_CLK_SLEEP_ENABLE()      (RCC->AHB1LPENR |= (RCC_AHB1LPENR_SRAM2LPEN))
#define __HAL_RCC_BKPSRAM_CLK_SLEEP_ENABLE()    (RCC->AHB1LPENR |= (RCC_AHB1LPENR_BKPSRAMLPEN))
#define __HAL_RCC_DTCM_CLK_SLEEP_ENABLE()       (RCC->AHB1LPENR |= (RCC_AHB1LPENR_DTCMLPEN))
#define __HAL_RCC_DMA2_CLK_SLEEP_ENABLE()       (RCC->AHB1LPENR |= (RCC_AHB1LPENR_DMA2LPEN))
#define __HAL_RCC_DMA2D_CLK_SLEEP_ENABLE()      (RCC->AHB1LPENR |= (RCC_AHB1LPENR_DMA2DLPEN))
#define __HAL_RCC_ETHMAC_CLK_SLEEP_ENABLE()     (RCC->AHB1LPENR |= (RCC_AHB1LPENR_ETHMACLPEN))
#define __HAL_RCC_ETHMACTX_CLK_SLEEP_ENABLE()   (RCC->AHB1LPENR |= (RCC_AHB1LPENR_ETHMACTXLPEN))
#define __HAL_RCC_ETHMACRX_CLK_SLEEP_ENABLE()   (RCC->AHB1LPENR |= (RCC_AHB1LPENR_ETHMACRXLPEN))
#define __HAL_RCC_ETHMACPTP_CLK_SLEEP_ENABLE()  (RCC->AHB1LPENR |= (RCC_AHB1LPENR_ETHMACPTPLPEN))
#define __HAL_RCC_USB_OTG_HS_CLK_SLEEP_ENABLE()      (RCC->AHB1LPENR |= (RCC_AHB1LPENR_OTGHSLPEN))
#define __HAL_RCC_USB_OTG_HS_ULPI_CLK_SLEEP_ENABLE() (RCC->AHB1LPENR |= (RCC_AHB1LPENR_OTGHSULPILPEN))
#define __HAL_RCC_GPIOA_CLK_SLEEP_ENABLE()      (RCC->AHB1LPENR |= (RCC_AHB1LPENR_GPIOALPEN))
#define __HAL_RCC_GPIOB_CLK_SLEEP_ENABLE()      (RCC->AHB1LPENR |= (RCC_AHB1LPENR_GPIOBLPEN))
#define __HAL_RCC_GPIOC_CLK_SLEEP_ENABLE()      (RCC->AHB1LPENR |= (RCC_AHB1LPENR_GPIOCLPEN))
#define __HAL_RCC_GPIOD_CLK_SLEEP_ENABLE()      (RCC->AHB1LPENR |= (RCC_AHB1LPENR_GPIODLPEN))
#define __HAL_RCC_GPIOE_CLK_SLEEP_ENABLE()      (RCC->AHB1LPENR |= (RCC_AHB1LPENR_GPIOELPEN))
#define __HAL_RCC_GPIOF_CLK_SLEEP_ENABLE()      (RCC->AHB1LPENR |= (RCC_AHB1LPENR_GPIOFLPEN))
#define __HAL_RCC_GPIOG_CLK_SLEEP_ENABLE()      (RCC->AHB1LPENR |= (RCC_AHB1LPENR_GPIOGLPEN))
#define __HAL_RCC_GPIOH_CLK_SLEEP_ENABLE()      (RCC->AHB1LPENR |= (RCC_AHB1LPENR_GPIOHLPEN))
#define __HAL_RCC_GPIOI_CLK_SLEEP_ENABLE()      (RCC->AHB1LPENR |= (RCC_AHB1LPENR_GPIOILPEN))
#define __HAL_RCC_GPIOJ_CLK_SLEEP_ENABLE()      (RCC->AHB1LPENR |= (RCC_AHB1LPENR_GPIOJLPEN))
#define __HAL_RCC_GPIOK_CLK_SLEEP_ENABLE()      (RCC->AHB1LPENR |= (RCC_AHB1LPENR_GPIOKLPEN))

#define __HAL_RCC_FLITF_CLK_SLEEP_DISABLE()     (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_FLITFLPEN))
#define __HAL_RCC_AXI_CLK_SLEEP_DISABLE()       (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_AXILPEN))
#define __HAL_RCC_SRAM1_CLK_SLEEP_DISABLE()     (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_SRAM1LPEN))
#define __HAL_RCC_SRAM2_CLK_SLEEP_DISABLE()     (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_SRAM2LPEN))
#define __HAL_RCC_BKPSRAM_CLK_SLEEP_DISABLE()   (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_BKPSRAMLPEN))
#define __HAL_RCC_DTCM_CLK_SLEEP_DISABLE()      (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_DTCMLPEN))
#define __HAL_RCC_DMA2_CLK_SLEEP_DISABLE()      (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_DMA2LPEN))
#define __HAL_RCC_DMA2D_CLK_SLEEP_DISABLE()     (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_DMA2DLPEN))
#define __HAL_RCC_ETHMAC_CLK_SLEEP_DISABLE()    (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_ETHMACLPEN))
#define __HAL_RCC_ETHMACTX_CLK_SLEEP_DISABLE()  (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_ETHMACTXLPEN))
#define __HAL_RCC_ETHMACRX_CLK_SLEEP_DISABLE()  (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_ETHMACRXLPEN))
#define __HAL_RCC_ETHMACPTP_CLK_SLEEP_DISABLE() (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_ETHMACPTPLPEN))
#define __HAL_RCC_USB_OTG_HS_CLK_SLEEP_DISABLE()      (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_OTGHSLPEN))
#define __HAL_RCC_USB_OTG_HS_ULPI_CLK_SLEEP_DISABLE() (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_OTGHSULPILPEN))
#define __HAL_RCC_GPIOA_CLK_SLEEP_DISABLE()     (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_GPIOALPEN))
#define __HAL_RCC_GPIOB_CLK_SLEEP_DISABLE()     (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_GPIOBLPEN))
#define __HAL_RCC_GPIOC_CLK_SLEEP_DISABLE()     (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_GPIOCLPEN))
#define __HAL_RCC_GPIOD_CLK_SLEEP_DISABLE()     (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_GPIODLPEN))
#define __HAL_RCC_GPIOE_CLK_SLEEP_DISABLE()     (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_GPIOELPEN))
#define __HAL_RCC_GPIOF_CLK_SLEEP_DISABLE()     (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_GPIOFLPEN))
#define __HAL_RCC_GPIOG_CLK_SLEEP_DISABLE()     (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_GPIOGLPEN))
#define __HAL_RCC_GPIOH_CLK_SLEEP_DISABLE()     (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_GPIOHLPEN))
#define __HAL_RCC_GPIOI_CLK_SLEEP_DISABLE()     (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_GPIOILPEN))
#define __HAL_RCC_GPIOJ_CLK_SLEEP_DISABLE()     (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_GPIOJLPEN))
#define __HAL_RCC_GPIOK_CLK_SLEEP_DISABLE()     (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_GPIOKLPEN))

/** @brief  Enable or disable the AHB2 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  */
#define __HAL_RCC_DCMI_CLK_SLEEP_ENABLE()        (RCC->AHB2LPENR |= (RCC_AHB2LPENR_DCMILPEN))
#define __HAL_RCC_DCMI_CLK_SLEEP_DISABLE()       (RCC->AHB2LPENR &= ~(RCC_AHB2LPENR_DCMILPEN))

#if defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_JPEG_CLK_SLEEP_ENABLE()        (RCC->AHB2LPENR |= (RCC_AHB2LPENR_JPEGLPEN))
#define __HAL_RCC_JPEG_CLK_SLEEP_DISABLE()       (RCC->AHB2LPENR &= ~(RCC_AHB2LPENR_JPEGLPEN))
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */

#define __HAL_RCC_RNG_CLK_SLEEP_ENABLE()         (RCC->AHB2LPENR |= (RCC_AHB2LPENR_RNGLPEN))
#define __HAL_RCC_RNG_CLK_SLEEP_DISABLE()        (RCC->AHB2LPENR &= ~(RCC_AHB2LPENR_RNGLPEN))

#define __HAL_RCC_USB_OTG_FS_CLK_SLEEP_ENABLE()  (RCC->AHB2LPENR |= (RCC_AHB2LPENR_OTGFSLPEN))
#define __HAL_RCC_USB_OTG_FS_CLK_SLEEP_DISABLE() (RCC->AHB2LPENR &= ~(RCC_AHB2LPENR_OTGFSLPEN))

#if defined(STM32F756xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_CRYP_CLK_SLEEP_ENABLE()        (RCC->AHB2LPENR |= (RCC_AHB2LPENR_CRYPLPEN))
#define __HAL_RCC_HASH_CLK_SLEEP_ENABLE()        (RCC->AHB2LPENR |= (RCC_AHB2LPENR_HASHLPEN))

#define __HAL_RCC_CRYP_CLK_SLEEP_DISABLE()       (RCC->AHB2LPENR &= ~(RCC_AHB2LPENR_CRYPLPEN))
#define __HAL_RCC_HASH_CLK_SLEEP_DISABLE()       (RCC->AHB2LPENR &= ~(RCC_AHB2LPENR_HASHLPEN))
#endif /* STM32F756xx || STM32F777xx || STM32F779xx */

/** @brief  Enable or disable the AHB3 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  */
#define __HAL_RCC_FMC_CLK_SLEEP_ENABLE()  (RCC->AHB3LPENR |= (RCC_AHB3LPENR_FMCLPEN))
#define __HAL_RCC_FMC_CLK_SLEEP_DISABLE() (RCC->AHB3LPENR &= ~(RCC_AHB3LPENR_FMCLPEN))

#define __HAL_RCC_QSPI_CLK_SLEEP_ENABLE()  (RCC->AHB3LPENR |= (RCC_AHB3LPENR_QSPILPEN))
#define __HAL_RCC_QSPI_CLK_SLEEP_DISABLE() (RCC->AHB3LPENR &= ~(RCC_AHB3LPENR_QSPILPEN))

/** @brief  Enable or disable the APB1 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  */
#define __HAL_RCC_TIM2_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_TIM2LPEN))
#define __HAL_RCC_TIM3_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_TIM3LPEN))
#define __HAL_RCC_TIM4_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_TIM4LPEN))
#define __HAL_RCC_TIM5_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_TIM5LPEN))
#define __HAL_RCC_TIM6_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_TIM6LPEN))
#define __HAL_RCC_TIM7_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_TIM7LPEN))
#define __HAL_RCC_TIM12_CLK_SLEEP_ENABLE()   (RCC->APB1LPENR |= (RCC_APB1LPENR_TIM12LPEN))
#define __HAL_RCC_TIM13_CLK_SLEEP_ENABLE()   (RCC->APB1LPENR |= (RCC_APB1LPENR_TIM13LPEN))
#define __HAL_RCC_TIM14_CLK_SLEEP_ENABLE()   (RCC->APB1LPENR |= (RCC_APB1LPENR_TIM14LPEN))
#define __HAL_RCC_LPTIM1_CLK_SLEEP_ENABLE()  (RCC->APB1LPENR |= (RCC_APB1LPENR_LPTIM1LPEN))
#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_RTC_CLK_SLEEP_ENABLE()     (RCC->APB1LPENR |= (RCC_APB1LPENR_RTCLPEN))
#define __HAL_RCC_CAN3_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_CAN3LPEN))
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
#define __HAL_RCC_SPI2_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_SPI2LPEN))
#define __HAL_RCC_SPI3_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_SPI3LPEN))
#define __HAL_RCC_SPDIFRX_CLK_SLEEP_ENABLE() (RCC->APB1LPENR |= (RCC_APB1LPENR_SPDIFRXLPEN))
#define __HAL_RCC_USART2_CLK_SLEEP_ENABLE()  (RCC->APB1LPENR |= (RCC_APB1LPENR_USART2LPEN))
#define __HAL_RCC_USART3_CLK_SLEEP_ENABLE()  (RCC->APB1LPENR |= (RCC_APB1LPENR_USART3LPEN))
#define __HAL_RCC_UART4_CLK_SLEEP_ENABLE()   (RCC->APB1LPENR |= (RCC_APB1LPENR_UART4LPEN))
#define __HAL_RCC_UART5_CLK_SLEEP_ENABLE()   (RCC->APB1LPENR |= (RCC_APB1LPENR_UART5LPEN))
#define __HAL_RCC_I2C1_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_I2C1LPEN))
#define __HAL_RCC_I2C2_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_I2C2LPEN))
#define __HAL_RCC_I2C3_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_I2C3LPEN))
#define __HAL_RCC_I2C4_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_I2C4LPEN))
#define __HAL_RCC_CAN1_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_CAN1LPEN))
#define __HAL_RCC_CAN2_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_CAN2LPEN))
#define __HAL_RCC_CEC_CLK_SLEEP_ENABLE()     (RCC->APB1LPENR |= (RCC_APB1LPENR_CECLPEN))
#define __HAL_RCC_DAC_CLK_SLEEP_ENABLE()     (RCC->APB1LPENR |= (RCC_APB1LPENR_DACLPEN))
#define __HAL_RCC_UART7_CLK_SLEEP_ENABLE()   (RCC->APB1LPENR |= (RCC_APB1LPENR_UART7LPEN))
#define __HAL_RCC_UART8_CLK_SLEEP_ENABLE()   (RCC->APB1LPENR |= (RCC_APB1LPENR_UART8LPEN))

#define __HAL_RCC_TIM2_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_TIM2LPEN))
#define __HAL_RCC_TIM3_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_TIM3LPEN))
#define __HAL_RCC_TIM4_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_TIM4LPEN))
#define __HAL_RCC_TIM5_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_TIM5LPEN))
#define __HAL_RCC_TIM6_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_TIM6LPEN))
#define __HAL_RCC_TIM7_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_TIM7LPEN))
#define __HAL_RCC_TIM12_CLK_SLEEP_DISABLE()  (RCC->APB1LPENR &= ~(RCC_APB1LPENR_TIM12LPEN))
#define __HAL_RCC_TIM13_CLK_SLEEP_DISABLE()  (RCC->APB1LPENR &= ~(RCC_APB1LPENR_TIM13LPEN))
#define __HAL_RCC_TIM14_CLK_SLEEP_DISABLE()  (RCC->APB1LPENR &= ~(RCC_APB1LPENR_TIM14LPEN))
#define __HAL_RCC_LPTIM1_CLK_SLEEP_DISABLE() (RCC->APB1LPENR &= ~(RCC_APB1LPENR_LPTIM1LPEN))
#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_RTC_CLK_SLEEP_DISABLE()    (RCC->APB1LPENR &= ~(RCC_APB1LPENR_RTCLPEN))
#define __HAL_RCC_CAN3_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_CAN3LPEN))
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
#define __HAL_RCC_SPI2_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_SPI2LPEN))
#define __HAL_RCC_SPI3_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_SPI3LPEN))
#define __HAL_RCC_SPDIFRX_CLK_SLEEP_DISABLE()(RCC->APB1LPENR &= ~(RCC_APB1LPENR_SPDIFRXLPEN))
#define __HAL_RCC_USART2_CLK_SLEEP_DISABLE() (RCC->APB1LPENR &= ~(RCC_APB1LPENR_USART2LPEN))
#define __HAL_RCC_USART3_CLK_SLEEP_DISABLE() (RCC->APB1LPENR &= ~(RCC_APB1LPENR_USART3LPEN))
#define __HAL_RCC_UART4_CLK_SLEEP_DISABLE()  (RCC->APB1LPENR &= ~(RCC_APB1LPENR_UART4LPEN))
#define __HAL_RCC_UART5_CLK_SLEEP_DISABLE()  (RCC->APB1LPENR &= ~(RCC_APB1LPENR_UART5LPEN))
#define __HAL_RCC_I2C1_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_I2C1LPEN))
#define __HAL_RCC_I2C2_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_I2C2LPEN))
#define __HAL_RCC_I2C3_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_I2C3LPEN))
#define __HAL_RCC_I2C4_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_I2C4LPEN))
#define __HAL_RCC_CAN1_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_CAN1LPEN))
#define __HAL_RCC_CAN2_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_CAN2LPEN))
#define __HAL_RCC_CEC_CLK_SLEEP_DISABLE()    (RCC->APB1LPENR &= ~(RCC_APB1LPENR_CECLPEN))
#define __HAL_RCC_DAC_CLK_SLEEP_DISABLE()    (RCC->APB1LPENR &= ~(RCC_APB1LPENR_DACLPEN))
#define __HAL_RCC_UART7_CLK_SLEEP_DISABLE()  (RCC->APB1LPENR &= ~(RCC_APB1LPENR_UART7LPEN))
#define __HAL_RCC_UART8_CLK_SLEEP_DISABLE()  (RCC->APB1LPENR &= ~(RCC_APB1LPENR_UART8LPEN))

/** @brief  Enable or disable the APB2 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  */
#define __HAL_RCC_TIM1_CLK_SLEEP_ENABLE()    (RCC->APB2LPENR |= (RCC_APB2LPENR_TIM1LPEN))
#define __HAL_RCC_TIM8_CLK_SLEEP_ENABLE()    (RCC->APB2LPENR |= (RCC_APB2LPENR_TIM8LPEN))
#define __HAL_RCC_USART1_CLK_SLEEP_ENABLE()  (RCC->APB2LPENR |= (RCC_APB2LPENR_USART1LPEN))
#define __HAL_RCC_USART6_CLK_SLEEP_ENABLE()  (RCC->APB2LPENR |= (RCC_APB2LPENR_USART6LPEN))
#define __HAL_RCC_ADC1_CLK_SLEEP_ENABLE()    (RCC->APB2LPENR |= (RCC_APB2LPENR_ADC1LPEN))
#define __HAL_RCC_ADC2_CLK_SLEEP_ENABLE()    (RCC->APB2LPENR |= (RCC_APB2LPENR_ADC2LPEN))
#define __HAL_RCC_ADC3_CLK_SLEEP_ENABLE()    (RCC->APB2LPENR |= (RCC_APB2LPENR_ADC3LPEN))
#define __HAL_RCC_SDMMC1_CLK_SLEEP_ENABLE()  (RCC->APB2LPENR |= (RCC_APB2LPENR_SDMMC1LPEN))
#define __HAL_RCC_SPI1_CLK_SLEEP_ENABLE()    (RCC->APB2LPENR |= (RCC_APB2LPENR_SPI1LPEN))
#define __HAL_RCC_SPI4_CLK_SLEEP_ENABLE()    (RCC->APB2LPENR |= (RCC_APB2LPENR_SPI4LPEN))
#define __HAL_RCC_TIM9_CLK_SLEEP_ENABLE()    (RCC->APB2LPENR |= (RCC_APB2LPENR_TIM9LPEN))
#define __HAL_RCC_TIM10_CLK_SLEEP_ENABLE()   (RCC->APB2LPENR |= (RCC_APB2LPENR_TIM10LPEN))
#define __HAL_RCC_TIM11_CLK_SLEEP_ENABLE()   (RCC->APB2LPENR |= (RCC_APB2LPENR_TIM11LPEN))
#define __HAL_RCC_SPI5_CLK_SLEEP_ENABLE()    (RCC->APB2LPENR |= (RCC_APB2LPENR_SPI5LPEN))
#define __HAL_RCC_SPI6_CLK_SLEEP_ENABLE()    (RCC->APB2LPENR |= (RCC_APB2LPENR_SPI6LPEN))
#define __HAL_RCC_SAI1_CLK_SLEEP_ENABLE()    (RCC->APB2LPENR |= (RCC_APB2LPENR_SAI1LPEN))
#define __HAL_RCC_SAI2_CLK_SLEEP_ENABLE()    (RCC->APB2LPENR |= (RCC_APB2LPENR_SAI2LPEN))
#if defined (STM32F746xx) || defined (STM32F756xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_LTDC_CLK_SLEEP_ENABLE()    (RCC->APB2LPENR |= (RCC_APB2LPENR_LTDCLPEN))
#endif /* STM32F746xx || STM32F756xx || STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */

#define __HAL_RCC_TIM1_CLK_SLEEP_DISABLE()   (RCC->APB2LPENR &= ~(RCC_APB2LPENR_TIM1LPEN))
#define __HAL_RCC_TIM8_CLK_SLEEP_DISABLE()   (RCC->APB2LPENR &= ~(RCC_APB2LPENR_TIM8LPEN))
#define __HAL_RCC_USART1_CLK_SLEEP_DISABLE() (RCC->APB2LPENR &= ~(RCC_APB2LPENR_USART1LPEN))
#define __HAL_RCC_USART6_CLK_SLEEP_DISABLE() (RCC->APB2LPENR &= ~(RCC_APB2LPENR_USART6LPEN))
#define __HAL_RCC_ADC1_CLK_SLEEP_DISABLE()   (RCC->APB2LPENR &= ~(RCC_APB2LPENR_ADC1LPEN))
#define __HAL_RCC_ADC2_CLK_SLEEP_DISABLE()   (RCC->APB2LPENR &= ~(RCC_APB2LPENR_ADC2LPEN))
#define __HAL_RCC_ADC3_CLK_SLEEP_DISABLE()   (RCC->APB2LPENR &= ~(RCC_APB2LPENR_ADC3LPEN))
#define __HAL_RCC_SDMMC1_CLK_SLEEP_DISABLE()  (RCC->APB2LPENR &= ~(RCC_APB2LPENR_SDMMC1LPEN))
#define __HAL_RCC_SPI1_CLK_SLEEP_DISABLE()   (RCC->APB2LPENR &= ~(RCC_APB2LPENR_SPI1LPEN))
#define __HAL_RCC_SPI4_CLK_SLEEP_DISABLE()   (RCC->APB2LPENR &= ~(RCC_APB2LPENR_SPI4LPEN))
#define __HAL_RCC_TIM9_CLK_SLEEP_DISABLE()   (RCC->APB2LPENR &= ~(RCC_APB2LPENR_TIM9LPEN))
#define __HAL_RCC_TIM10_CLK_SLEEP_DISABLE()  (RCC->APB2LPENR &= ~(RCC_APB2LPENR_TIM10LPEN))
#define __HAL_RCC_TIM11_CLK_SLEEP_DISABLE()  (RCC->APB2LPENR &= ~(RCC_APB2LPENR_TIM11LPEN))
#define __HAL_RCC_SPI5_CLK_SLEEP_DISABLE()   (RCC->APB2LPENR &= ~(RCC_APB2LPENR_SPI5LPEN))
#define __HAL_RCC_SPI6_CLK_SLEEP_DISABLE()   (RCC->APB2LPENR &= ~(RCC_APB2LPENR_SPI6LPEN))
#define __HAL_RCC_SAI1_CLK_SLEEP_DISABLE()   (RCC->APB2LPENR &= ~(RCC_APB2LPENR_SAI1LPEN))
#define __HAL_RCC_SAI2_CLK_SLEEP_DISABLE()   (RCC->APB2LPENR &= ~(RCC_APB2LPENR_SAI2LPEN))
#if defined (STM32F746xx) || defined (STM32F756xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_LTDC_CLK_SLEEP_DISABLE()   (RCC->APB2LPENR &= ~(RCC_APB2LPENR_LTDCLPEN))
#endif /* STM32F746xx || STM32F756xx || STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
#if defined (STM32F769xx) || defined (STM32F779xx)
#define __HAL_RCC_DSI_CLK_SLEEP_ENABLE()     (RCC->APB2LPENR |= (RCC_APB2LPENR_DSILPEN))
#define __HAL_RCC_DSI_CLK_SLEEP_DISABLE()    (RCC->APB2LPENR &= ~(RCC_APB2LPENR_DSILPEN))
#endif /* STM32F769xx || STM32F779xx */
#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_SDMMC2_CLK_SLEEP_ENABLE()  (RCC->APB2LPENR |= (RCC_APB2LPENR_SDMMC2LPEN))
#define __HAL_RCC_DFSDM1_CLK_SLEEP_ENABLE()   (RCC->APB2LPENR |= (RCC_APB2LPENR_DFSDM1LPEN))
#define __HAL_RCC_MDIO_CLK_SLEEP_ENABLE()    (RCC->APB2LPENR |= (RCC_APB2LPENR_MDIOLPEN))

#define __HAL_RCC_SDMMC2_CLK_SLEEP_DISABLE() (RCC->APB2LPENR &= ~(RCC_APB2LPENR_SDMMC2LPEN))
#define __HAL_RCC_DFSDM1_CLK_SLEEP_DISABLE()  (RCC->APB2LPENR &= ~(RCC_APB2LPENR_DFSDM1LPEN))
#define __HAL_RCC_MDIO_CLK_SLEEP_DISABLE()   (RCC->APB2LPENR &= ~(RCC_APB2LPENR_MDIOLPEN))
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
/**
  * @}
  */

/** @defgroup RCC_Clock_Sleep_Enable_Disable_Status AHB/APB Peripheral Clock Sleep Enable Disable Status
  * @brief  Get the enable or disable status of the AHB/APB peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  * @{
  */

/** @brief  Get the enable or disable status of the AHB1 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  */
#define __HAL_RCC_FLITF_IS_CLK_SLEEP_ENABLED()      ((RCC->AHB1LPENR & (RCC_AHB1LPENR_FLITFLPEN)) != RESET)
#define __HAL_RCC_AXI_IS_CLK_SLEEP_ENABLED()        ((RCC->AHB1LPENR & (RCC_AHB1LPENR_AXILPEN)) != RESET)
#define __HAL_RCC_SRAM1_IS_CLK_SLEEP_ENABLED()      ((RCC->AHB1LPENR & (RCC_AHB1LPENR_SRAM1LPEN)) != RESET)
#define __HAL_RCC_SRAM2_IS_CLK_SLEEP_ENABLED()      ((RCC->AHB1LPENR & (RCC_AHB1LPENR_SRAM2LPEN)) != RESET)
#define __HAL_RCC_BKPSRAM_IS_CLK_SLEEP_ENABLED()    ((RCC->AHB1LPENR & (RCC_AHB1LPENR_BKPSRAMLPEN)) != RESET)
#define __HAL_RCC_DTCM_IS_CLK_SLEEP_ENABLED()       ((RCC->AHB1LPENR & (RCC_AHB1LPENR_DTCMLPEN)) != RESET)
#define __HAL_RCC_DMA2_IS_CLK_SLEEP_ENABLED()       ((RCC->AHB1LPENR & (RCC_AHB1LPENR_DMA2LPEN)) != RESET)
#define __HAL_RCC_DMA2D_IS_CLK_SLEEP_ENABLED()      ((RCC->AHB1LPENR & (RCC_AHB1LPENR_DMA2DLPEN)) != RESET)
#define __HAL_RCC_ETHMAC_IS_CLK_SLEEP_ENABLED()     ((RCC->AHB1LPENR & (RCC_AHB1LPENR_ETHMACLPEN)) != RESET)
#define __HAL_RCC_ETHMACTX_IS_CLK_SLEEP_ENABLED()   ((RCC->AHB1LPENR & (RCC_AHB1LPENR_ETHMACTXLPEN)) != RESET)
#define __HAL_RCC_ETHMACRX_IS_CLK_SLEEP_ENABLED()   ((RCC->AHB1LPENR & (RCC_AHB1LPENR_ETHMACRXLPEN)) != RESET)
#define __HAL_RCC_ETHMACPTP_IS_CLK_SLEEP_ENABLED()  ((RCC->AHB1LPENR & (RCC_AHB1LPENR_ETHMACPTPLPEN)) != RESET)
#define __HAL_RCC_USB_OTG_HS_IS_CLK_SLEEP_ENABLED() ((RCC->AHB1LPENR & (RCC_AHB1LPENR_OTGHSLPEN)) != RESET)
#define __HAL_RCC_USB_OTG_HS_ULPI_IS_CLK_SLEEP_ENABLED() ((RCC->AHB1LPENR & (RCC_AHB1LPENR_OTGHSULPILPEN)) != RESET)
#define __HAL_RCC_GPIOA_IS_CLK_SLEEP_ENABLED()      ((RCC->AHB1LPENR & (RCC_AHB1LPENR_GPIOALPEN)) != RESET)
#define __HAL_RCC_GPIOB_IS_CLK_SLEEP_ENABLED()      ((RCC->AHB1LPENR & (RCC_AHB1LPENR_GPIOBLPEN)) != RESET)
#define __HAL_RCC_GPIOC_IS_CLK_SLEEP_ENABLED()      ((RCC->AHB1LPENR & (RCC_AHB1LPENR_GPIOCLPEN)) != RESET)
#define __HAL_RCC_GPIOD_IS_CLK_SLEEP_ENABLED()      ((RCC->AHB1LPENR & (RCC_AHB1LPENR_GPIODLPEN)) != RESET)
#define __HAL_RCC_GPIOE_IS_CLK_SLEEP_ENABLED()      ((RCC->AHB1LPENR & (RCC_AHB1LPENR_GPIOELPEN)) != RESET)
#define __HAL_RCC_GPIOF_IS_CLK_SLEEP_ENABLED()      ((RCC->AHB1LPENR & (RCC_AHB1LPENR_GPIOFLPEN)) != RESET)
#define __HAL_RCC_GPIOG_IS_CLK_SLEEP_ENABLED()      ((RCC->AHB1LPENR & (RCC_AHB1LPENR_GPIOGLPEN)) != RESET)
#define __HAL_RCC_GPIOH_IS_CLK_SLEEP_ENABLED()      ((RCC->AHB1LPENR & (RCC_AHB1LPENR_GPIOHLPEN)) != RESET)
#define __HAL_RCC_GPIOI_IS_CLK_SLEEP_ENABLED()      ((RCC->AHB1LPENR & (RCC_AHB1LPENR_GPIOILPEN)) != RESET)
#define __HAL_RCC_GPIOJ_IS_CLK_SLEEP_ENABLED()      ((RCC->AHB1LPENR & (RCC_AHB1LPENR_GPIOJLPEN)) != RESET)
#define __HAL_RCC_GPIOK_IS_CLK_SLEEP_ENABLED()      ((RCC->AHB1LPENR & (RCC_AHB1LPENR_GPIOKLPEN)) != RESET)

#define __HAL_RCC_FLITF_IS_CLK_SLEEP_DISABLED()     ((RCC->AHB1LPENR & (RCC_AHB1LPENR_FLITFLPEN)) == RESET)
#define __HAL_RCC_AXI_IS_CLK_SLEEP_DISABLED()       ((RCC->AHB1LPENR & (RCC_AHB1LPENR_AXILPEN)) == RESET)
#define __HAL_RCC_SRAM1_IS_CLK_SLEEP_DISABLED()     ((RCC->AHB1LPENR & (RCC_AHB1LPENR_SRAM1LPEN)) == RESET)
#define __HAL_RCC_SRAM2_IS_CLK_SLEEP_DISABLED()     ((RCC->AHB1LPENR & (RCC_AHB1LPENR_SRAM2LPEN)) == RESET)
#define __HAL_RCC_BKPSRAM_IS_CLK_SLEEP_DISABLED()   ((RCC->AHB1LPENR & (RCC_AHB1LPENR_BKPSRAMLPEN)) == RESET)
#define __HAL_RCC_DTCM_IS_CLK_SLEEP_DISABLED()      ((RCC->AHB1LPENR & (RCC_AHB1LPENR_DTCMLPEN)) == RESET)
#define __HAL_RCC_DMA2_IS_CLK_SLEEP_DISABLED()      ((RCC->AHB1LPENR & (RCC_AHB1LPENR_DMA2LPEN)) == RESET)
#define __HAL_RCC_DMA2D_IS_CLK_SLEEP_DISABLED()     ((RCC->AHB1LPENR & (RCC_AHB1LPENR_DMA2DLPEN)) == RESET)
#define __HAL_RCC_ETHMAC_IS_CLK_SLEEP_DISABLED()    ((RCC->AHB1LPENR & (RCC_AHB1LPENR_ETHMACLPEN)) == RESET)
#define __HAL_RCC_ETHMACTX_IS_CLK_SLEEP_DISABLED()  ((RCC->AHB1LPENR & (RCC_AHB1LPENR_ETHMACTXLPEN)) == RESET)
#define __HAL_RCC_ETHMACRX_IS_CLK_SLEEP_DISABLED()  ((RCC->AHB1LPENR & (RCC_AHB1LPENR_ETHMACRXLPEN)) == RESET)
#define __HAL_RCC_ETHMACPTP_IS_CLK_SLEEP_DISABLED() ((RCC->AHB1LPENR & (RCC_AHB1LPENR_ETHMACPTPLPEN)) == RESET)
#define __HAL_RCC_USB_OTG_HS_IS_CLK_SLEEP_DISABLED() ((RCC->AHB1LPENR & (RCC_AHB1LPENR_OTGHSLPEN)) == RESET)
#define __HAL_RCC_USB_OTG_HS_ULPI_IS_CLK_SLEEP_DISABLED() ((RCC->AHB1LPENR & (RCC_AHB1LPENR_OTGHSULPILPEN)) == RESET)
#define __HAL_RCC_GPIOA_IS_CLK_SLEEP_DISABLED()     ((RCC->AHB1LPENR & (RCC_AHB1LPENR_GPIOALPEN)) == RESET)
#define __HAL_RCC_GPIOB_IS_CLK_SLEEP_DISABLED()     ((RCC->AHB1LPENR & (RCC_AHB1LPENR_GPIOBLPEN)) == RESET)
#define __HAL_RCC_GPIOC_IS_CLK_SLEEP_DISABLED()     ((RCC->AHB1LPENR & (RCC_AHB1LPENR_GPIOCLPEN)) == RESET)
#define __HAL_RCC_GPIOD_IS_CLK_SLEEP_DISABLED()     ((RCC->AHB1LPENR & (RCC_AHB1LPENR_GPIODLPEN)) == RESET)
#define __HAL_RCC_GPIOE_IS_CLK_SLEEP_DISABLED()     ((RCC->AHB1LPENR & (RCC_AHB1LPENR_GPIOELPEN)) == RESET)
#define __HAL_RCC_GPIOF_IS_CLK_SLEEP_DISABLED()     ((RCC->AHB1LPENR & (RCC_AHB1LPENR_GPIOFLPEN)) == RESET)
#define __HAL_RCC_GPIOG_IS_CLK_SLEEP_DISABLED()     ((RCC->AHB1LPENR & (RCC_AHB1LPENR_GPIOGLPEN)) == RESET)
#define __HAL_RCC_GPIOH_IS_CLK_SLEEP_DISABLED()     ((RCC->AHB1LPENR & (RCC_AHB1LPENR_GPIOHLPEN)) == RESET)
#define __HAL_RCC_GPIOI_IS_CLK_SLEEP_DISABLED()     ((RCC->AHB1LPENR & (RCC_AHB1LPENR_GPIOILPEN)) == RESET)
#define __HAL_RCC_GPIOJ_IS_CLK_SLEEP_DISABLED()     ((RCC->AHB1LPENR & (RCC_AHB1LPENR_GPIOJLPEN)) == RESET)
#define __HAL_RCC_GPIOK_IS_CLK_SLEEP_DISABLED()     ((RCC->AHB1LPENR & (RCC_AHB1LPENR_GPIOKLPEN)) == RESET)

/** @brief  Get the enable or disable status of the AHB2 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  */
#define __HAL_RCC_DCMI_IS_CLK_SLEEP_ENABLED()        ((RCC->AHB2LPENR & (RCC_AHB2LPENR_DCMILPEN)) != RESET)
#define __HAL_RCC_DCMI_IS_CLK_SLEEP_DISABLED()       ((RCC->AHB2LPENR & (RCC_AHB2LPENR_DCMILPEN)) == RESET)

#if defined(STM32F767xx) || defined(STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_JPEG_IS_CLK_SLEEP_ENABLED()        ((RCC->AHB2LPENR & (RCC_AHB2LPENR_JPEGLPEN)) != RESET)
#define __HAL_RCC_JPEG_IS_CLK_SLEEP_DISABLED()       ((RCC->AHB2LPENR & (RCC_AHB2LPENR_JPEGLPEN)) == RESET)
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */

#define __HAL_RCC_RNG_IS_CLK_SLEEP_ENABLED()         ((RCC->AHB2LPENR & (RCC_AHB2LPENR_RNGLPEN)) != RESET)
#define __HAL_RCC_RNG_IS_CLK_SLEEP_DISABLED()        ((RCC->AHB2LPENR & (RCC_AHB2LPENR_RNGLPEN)) == RESET)

#define __HAL_RCC_USB_OTG_FS_IS_CLK_SLEEP_ENABLED()  ((RCC->AHB2LPENR & (RCC_AHB2LPENR_OTGFSLPEN)) != RESET)
#define __HAL_RCC_USB_OTG_FS_IS_CLK_SLEEP_DISABLED() ((RCC->AHB2LPENR & (RCC_AHB2LPENR_OTGFSLPEN)) == RESET)

#if defined(STM32F756xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_CRYP_IS_CLK_SLEEP_ENABLED()        ((RCC->AHB2LPENR & (RCC_AHB2LPENR_CRYPLPEN)) != RESET)
#define __HAL_RCC_HASH_IS_CLK_SLEEP_ENABLED()        ((RCC->AHB2LPENR & (RCC_AHB2LPENR_HASHLPEN)) != RESET)

#define __HAL_RCC_CRYP_IS_CLK_SLEEP_DISABLED()       ((RCC->AHB2LPENR & (RCC_AHB2LPENR_CRYPLPEN)) == RESET)
#define __HAL_RCC_HASH_IS_CLK_SLEEP_DISABLED()       ((RCC->AHB2LPENR & (RCC_AHB2LPENR_HASHLPEN)) == RESET)
#endif /* STM32F756xx || STM32F777xx || STM32F779xx */

/** @brief  Get the enable or disable status of the AHB3 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  */
#define __HAL_RCC_FMC_IS_CLK_SLEEP_ENABLED()  ((RCC->AHB3LPENR & (RCC_AHB3LPENR_FMCLPEN)) != RESET)
#define __HAL_RCC_FMC_IS_CLK_SLEEP_DISABLED() ((RCC->AHB3LPENR & (RCC_AHB3LPENR_FMCLPEN)) == RESET)

#define __HAL_RCC_QSPI_IS_CLK_SLEEP_ENABLED()  ((RCC->AHB3LPENR & (RCC_AHB3LPENR_QSPILPEN)) != RESET)
#define __HAL_RCC_QSPI_IS_CLK_SLEEP_DISABLED() ((RCC->AHB3LPENR & (RCC_AHB3LPENR_QSPILPEN)) == RESET)

/** @brief  Get the enable or disable status of the APB1 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  */
#define __HAL_RCC_TIM2_IS_CLK_SLEEP_ENABLED()    ((RCC->APB1LPENR & (RCC_APB1LPENR_TIM2LPEN)) != RESET)
#define __HAL_RCC_TIM3_IS_CLK_SLEEP_ENABLED()    ((RCC->APB1LPENR & (RCC_APB1LPENR_TIM3LPEN)) != RESET)
#define __HAL_RCC_TIM4_IS_CLK_SLEEP_ENABLED()    ((RCC->APB1LPENR & (RCC_APB1LPENR_TIM4LPEN)) != RESET)
#define __HAL_RCC_TIM5_IS_CLK_SLEEP_ENABLED()    ((RCC->APB1LPENR & (RCC_APB1LPENR_TIM5LPEN)) != RESET)
#define __HAL_RCC_TIM6_IS_CLK_SLEEP_ENABLED()    ((RCC->APB1LPENR & (RCC_APB1LPENR_TIM6LPEN)) != RESET)
#define __HAL_RCC_TIM7_IS_CLK_SLEEP_ENABLED()    ((RCC->APB1LPENR & (RCC_APB1LPENR_TIM7LPEN)) != RESET)
#define __HAL_RCC_TIM12_IS_CLK_SLEEP_ENABLED()   ((RCC->APB1LPENR & (RCC_APB1LPENR_TIM12LPEN)) != RESET)
#define __HAL_RCC_TIM13_IS_CLK_SLEEP_ENABLED()   ((RCC->APB1LPENR & (RCC_APB1LPENR_TIM13LPEN)) != RESET)
#define __HAL_RCC_TIM14_IS_CLK_SLEEP_ENABLED()   ((RCC->APB1LPENR & (RCC_APB1LPENR_TIM14LPEN)) != RESET)
#define __HAL_RCC_LPTIM1_IS_CLK_SLEEP_ENABLED()  ((RCC->APB1LPENR & (RCC_APB1LPENR_LPTIM1LPEN)) != RESET)
#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_RTC_IS_CLK_SLEEP_ENABLED()     ((RCC->APB1LPENR & (RCC_APB1LPENR_RTCLPEN)) != RESET)
#define __HAL_RCC_CAN3_IS_CLK_SLEEP_ENABLED()    ((RCC->APB1LPENR & (RCC_APB1LPENR_CAN3LPEN)) != RESET)
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
#define __HAL_RCC_SPI2_IS_CLK_SLEEP_ENABLED()    ((RCC->APB1LPENR & (RCC_APB1LPENR_SPI2LPEN)) != RESET)
#define __HAL_RCC_SPI3_IS_CLK_SLEEP_ENABLED()    ((RCC->APB1LPENR & (RCC_APB1LPENR_SPI3LPEN)) != RESET)
#define __HAL_RCC_SPDIFRX_IS_CLK_SLEEP_ENABLED() ((RCC->APB1LPENR & (RCC_APB1LPENR_SPDIFRXLPEN)) != RESET)
#define __HAL_RCC_USART2_IS_CLK_SLEEP_ENABLED()  ((RCC->APB1LPENR & (RCC_APB1LPENR_USART2LPEN)) != RESET)
#define __HAL_RCC_USART3_IS_CLK_SLEEP_ENABLED()  ((RCC->APB1LPENR & (RCC_APB1LPENR_USART3LPEN)) != RESET)
#define __HAL_RCC_UART4_IS_CLK_SLEEP_ENABLED()   ((RCC->APB1LPENR & (RCC_APB1LPENR_UART4LPEN)) != RESET)
#define __HAL_RCC_UART5_IS_CLK_SLEEP_ENABLED()   ((RCC->APB1LPENR & (RCC_APB1LPENR_UART5LPEN)) != RESET)
#define __HAL_RCC_I2C1_IS_CLK_SLEEP_ENABLED()    ((RCC->APB1LPENR & (RCC_APB1LPENR_I2C1LPEN)) != RESET)
#define __HAL_RCC_I2C2_IS_CLK_SLEEP_ENABLED()    ((RCC->APB1LPENR & (RCC_APB1LPENR_I2C2LPEN)) != RESET)
#define __HAL_RCC_I2C3_IS_CLK_SLEEP_ENABLED()    ((RCC->APB1LPENR & (RCC_APB1LPENR_I2C3LPEN)) != RESET)
#define __HAL_RCC_I2C4_IS_CLK_SLEEP_ENABLED()    ((RCC->APB1LPENR & (RCC_APB1LPENR_I2C4LPEN)) != RESET)
#define __HAL_RCC_CAN1_IS_CLK_SLEEP_ENABLED()    ((RCC->APB1LPENR & (RCC_APB1LPENR_CAN1LPEN)) != RESET)
#define __HAL_RCC_CAN2_IS_CLK_SLEEP_ENABLED()    ((RCC->APB1LPENR & (RCC_APB1LPENR_CAN2LPEN)) != RESET)
#define __HAL_RCC_CEC_IS_CLK_SLEEP_ENABLED()     ((RCC->APB1LPENR & (RCC_APB1LPENR_CECLPEN)) != RESET)
#define __HAL_RCC_DAC_IS_CLK_SLEEP_ENABLED()     ((RCC->APB1LPENR & (RCC_APB1LPENR_DACLPEN)) != RESET)
#define __HAL_RCC_UART7_IS_CLK_SLEEP_ENABLED()   ((RCC->APB1LPENR & (RCC_APB1LPENR_UART7LPEN)) != RESET)
#define __HAL_RCC_UART8_IS_CLK_SLEEP_ENABLED()   ((RCC->APB1LPENR & (RCC_APB1LPENR_UART8LPEN)) != RESET)

#define __HAL_RCC_TIM2_IS_CLK_SLEEP_DISABLED()   ((RCC->APB1LPENR & (RCC_APB1LPENR_TIM2LPEN)) == RESET)
#define __HAL_RCC_TIM3_IS_CLK_SLEEP_DISABLED()   ((RCC->APB1LPENR & (RCC_APB1LPENR_TIM3LPEN)) == RESET)
#define __HAL_RCC_TIM4_IS_CLK_SLEEP_DISABLED()   ((RCC->APB1LPENR & (RCC_APB1LPENR_TIM4LPEN)) == RESET)
#define __HAL_RCC_TIM5_IS_CLK_SLEEP_DISABLED()   ((RCC->APB1LPENR & (RCC_APB1LPENR_TIM5LPEN)) == RESET)
#define __HAL_RCC_TIM6_IS_CLK_SLEEP_DISABLED()   ((RCC->APB1LPENR & (RCC_APB1LPENR_TIM6LPEN)) == RESET)
#define __HAL_RCC_TIM7_IS_CLK_SLEEP_DISABLED()   ((RCC->APB1LPENR & (RCC_APB1LPENR_TIM7LPEN)) == RESET)
#define __HAL_RCC_TIM12_IS_CLK_SLEEP_DISABLED()  ((RCC->APB1LPENR & (RCC_APB1LPENR_TIM12LPEN)) == RESET)
#define __HAL_RCC_TIM13_IS_CLK_SLEEP_DISABLED()  ((RCC->APB1LPENR & (RCC_APB1LPENR_TIM13LPEN)) == RESET)
#define __HAL_RCC_TIM14_IS_CLK_SLEEP_DISABLED()  ((RCC->APB1LPENR & (RCC_APB1LPENR_TIM14LPEN)) == RESET)
#define __HAL_RCC_LPTIM1_IS_CLK_SLEEP_DISABLED() ((RCC->APB1LPENR & (RCC_APB1LPENR_LPTIM1LPEN)) == RESET)
#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_RTC_IS_CLK_SLEEP_DISABLED()    ((RCC->APB1LPENR & (RCC_APB1LPENR_RTCLPEN)) == RESET)
#define __HAL_RCC_CAN3_IS_CLK_SLEEP_DISABLED()   ((RCC->APB1LPENR & (RCC_APB1LPENR_CAN3LPEN)) == RESET)
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
#define __HAL_RCC_SPI2_IS_CLK_SLEEP_DISABLED()   ((RCC->APB1LPENR & (RCC_APB1LPENR_SPI2LPEN)) == RESET)
#define __HAL_RCC_SPI3_IS_CLK_SLEEP_DISABLED()   ((RCC->APB1LPENR & (RCC_APB1LPENR_SPI3LPEN)) == RESET)
#define __HAL_RCC_SPDIFRX_IS_CLK_SLEEP_DISABLED()((RCC->APB1LPENR & (RCC_APB1LPENR_SPDIFRXLPEN)) == RESET)
#define __HAL_RCC_USART2_IS_CLK_SLEEP_DISABLED() ((RCC->APB1LPENR & (RCC_APB1LPENR_USART2LPEN)) == RESET)
#define __HAL_RCC_USART3_IS_CLK_SLEEP_DISABLED() ((RCC->APB1LPENR & (RCC_APB1LPENR_USART3LPEN)) == RESET)
#define __HAL_RCC_UART4_IS_CLK_SLEEP_DISABLED()  ((RCC->APB1LPENR & (RCC_APB1LPENR_UART4LPEN)) == RESET)
#define __HAL_RCC_UART5_IS_CLK_SLEEP_DISABLED()  ((RCC->APB1LPENR & (RCC_APB1LPENR_UART5LPEN)) == RESET)
#define __HAL_RCC_I2C1_IS_CLK_SLEEP_DISABLED()   ((RCC->APB1LPENR & (RCC_APB1LPENR_I2C1LPEN)) == RESET)
#define __HAL_RCC_I2C2_IS_CLK_SLEEP_DISABLED()   ((RCC->APB1LPENR & (RCC_APB1LPENR_I2C2LPEN)) == RESET)
#define __HAL_RCC_I2C3_IS_CLK_SLEEP_DISABLED()   ((RCC->APB1LPENR & (RCC_APB1LPENR_I2C3LPEN)) == RESET)
#define __HAL_RCC_I2C4_IS_CLK_SLEEP_DISABLED()   ((RCC->APB1LPENR & (RCC_APB1LPENR_I2C4LPEN)) == RESET)
#define __HAL_RCC_CAN1_IS_CLK_SLEEP_DISABLED()   ((RCC->APB1LPENR & (RCC_APB1LPENR_CAN1LPEN)) == RESET)
#define __HAL_RCC_CAN2_IS_CLK_SLEEP_DISABLED()   ((RCC->APB1LPENR & (RCC_APB1LPENR_CAN2LPEN)) == RESET)
#define __HAL_RCC_CEC_IS_CLK_SLEEP_DISABLED()    ((RCC->APB1LPENR & (RCC_APB1LPENR_CECLPEN)) == RESET)
#define __HAL_RCC_DAC_IS_CLK_SLEEP_DISABLED()    ((RCC->APB1LPENR & (RCC_APB1LPENR_DACLPEN)) == RESET)
#define __HAL_RCC_UART7_IS_CLK_SLEEP_DISABLED()  ((RCC->APB1LPENR & (RCC_APB1LPENR_UART7LPEN)) == RESET)
#define __HAL_RCC_UART8_IS_CLK_SLEEP_DISABLED()  ((RCC->APB1LPENR & (RCC_APB1LPENR_UART8LPEN)) == RESET)

/** @brief  Get the enable or disable status of the APB2 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  */
#define __HAL_RCC_TIM1_IS_CLK_SLEEP_ENABLED()    ((RCC->APB2LPENR & (RCC_APB2LPENR_TIM1LPEN)) != RESET)
#define __HAL_RCC_TIM8_IS_CLK_SLEEP_ENABLED()    ((RCC->APB2LPENR & (RCC_APB2LPENR_TIM8LPEN)) != RESET)
#define __HAL_RCC_USART1_IS_CLK_SLEEP_ENABLED()  ((RCC->APB2LPENR & (RCC_APB2LPENR_USART1LPEN)) != RESET)
#define __HAL_RCC_USART6_IS_CLK_SLEEP_ENABLED()  ((RCC->APB2LPENR & (RCC_APB2LPENR_USART6LPEN)) != RESET)
#define __HAL_RCC_ADC1_IS_CLK_SLEEP_ENABLED()    ((RCC->APB2LPENR & (RCC_APB2LPENR_ADC1LPEN)) != RESET)
#define __HAL_RCC_ADC2_IS_CLK_SLEEP_ENABLED()    ((RCC->APB2LPENR & (RCC_APB2LPENR_ADC2LPEN)) != RESET)
#define __HAL_RCC_ADC3_IS_CLK_SLEEP_ENABLED()    ((RCC->APB2LPENR & (RCC_APB2LPENR_ADC3LPEN)) != RESET)
#define __HAL_RCC_SDMMC1_IS_CLK_SLEEP_ENABLED()  ((RCC->APB2LPENR & (RCC_APB2LPENR_SDMMC1LPEN)) != RESET)
#define __HAL_RCC_SPI1_IS_CLK_SLEEP_ENABLED()    ((RCC->APB2LPENR & (RCC_APB2LPENR_SPI1LPEN)) != RESET)
#define __HAL_RCC_SPI4_IS_CLK_SLEEP_ENABLED()    ((RCC->APB2LPENR & (RCC_APB2LPENR_SPI4LPEN)) != RESET)
#define __HAL_RCC_TIM9_IS_CLK_SLEEP_ENABLED()    ((RCC->APB2LPENR & (RCC_APB2LPENR_TIM9LPEN)) != RESET)
#define __HAL_RCC_TIM10_IS_CLK_SLEEP_ENABLED()   ((RCC->APB2LPENR & (RCC_APB2LPENR_TIM10LPEN)) != RESET)
#define __HAL_RCC_TIM11_IS_CLK_SLEEP_ENABLED()   ((RCC->APB2LPENR & (RCC_APB2LPENR_TIM11LPEN)) != RESET)
#define __HAL_RCC_SPI5_IS_CLK_SLEEP_ENABLED()    ((RCC->APB2LPENR & (RCC_APB2LPENR_SPI5LPEN)) != RESET)
#define __HAL_RCC_SPI6_IS_CLK_SLEEP_ENABLED()    ((RCC->APB2LPENR & (RCC_APB2LPENR_SPI6LPEN)) != RESET)
#define __HAL_RCC_SAI1_IS_CLK_SLEEP_ENABLED()    ((RCC->APB2LPENR & (RCC_APB2LPENR_SAI1LPEN)) != RESET)
#define __HAL_RCC_SAI2_IS_CLK_SLEEP_ENABLED()    ((RCC->APB2LPENR & (RCC_APB2LPENR_SAI2LPEN)) != RESET)
#if defined (STM32F746xx) || defined (STM32F756xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_LTDC_IS_CLK_SLEEP_ENABLED()    ((RCC->APB2LPENR & (RCC_APB2LPENR_LTDCLPEN)) != RESET)
#endif /* STM32F746xx || STM32F756xx || STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
#if defined (STM32F769xx) || defined (STM32F779xx)
#define __HAL_RCC_DSI_IS_CLK_SLEEP_ENABLED()     ((RCC->APB2LPENR & (RCC_APB2LPENR_DSILPEN)) != RESET)
#endif /* STM32F769xx || STM32F779xx */
#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_SDMMC2_IS_CLK_SLEEP_ENABLED()  ((RCC->APB2LPENR & (RCC_APB2LPENR_SDMMC2LPEN)) != RESET)
#define __HAL_RCC_DFSDM1_IS_CLK_SLEEP_ENABLED()   ((RCC->APB2LPENR & (RCC_APB2LPENR_DFSDM1LPEN)) != RESET)
#define __HAL_RCC_MDIO_IS_CLK_SLEEP_ENABLED()    ((RCC->APB2LPENR & (RCC_APB2LPENR_MDIOLPEN)) != RESET)
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */

#define __HAL_RCC_TIM1_IS_CLK_SLEEP_DISABLED()   ((RCC->APB2LPENR & (RCC_APB2LPENR_TIM1LPEN)) == RESET)
#define __HAL_RCC_TIM8_IS_CLK_SLEEP_DISABLED()   ((RCC->APB2LPENR & (RCC_APB2LPENR_TIM8LPEN)) == RESET)
#define __HAL_RCC_USART1_IS_CLK_SLEEP_DISABLED() ((RCC->APB2LPENR & (RCC_APB2LPENR_USART1LPEN)) == RESET)
#define __HAL_RCC_USART6_IS_CLK_SLEEP_DISABLED() ((RCC->APB2LPENR & (RCC_APB2LPENR_USART6LPEN)) == RESET)
#define __HAL_RCC_ADC1_IS_CLK_SLEEP_DISABLED()   ((RCC->APB2LPENR & (RCC_APB2LPENR_ADC1LPEN)) == RESET)
#define __HAL_RCC_ADC2_IS_CLK_SLEEP_DISABLED()   ((RCC->APB2LPENR & (RCC_APB2LPENR_ADC2LPEN)) == RESET)
#define __HAL_RCC_ADC3_IS_CLK_SLEEP_DISABLED()   ((RCC->APB2LPENR & (RCC_APB2LPENR_ADC3LPEN)) == RESET)
#define __HAL_RCC_SDMMC1_IS_CLK_SLEEP_DISABLED() ((RCC->APB2LPENR & (RCC_APB2LPENR_SDMMC1LPEN)) == RESET)
#define __HAL_RCC_SPI1_IS_CLK_SLEEP_DISABLED()   ((RCC->APB2LPENR & (RCC_APB2LPENR_SPI1LPEN)) == RESET)
#define __HAL_RCC_SPI4_IS_CLK_SLEEP_DISABLED()   ((RCC->APB2LPENR & (RCC_APB2LPENR_SPI4LPEN)) == RESET)
#define __HAL_RCC_TIM9_IS_CLK_SLEEP_DISABLED()   ((RCC->APB2LPENR & (RCC_APB2LPENR_TIM9LPEN)) == RESET)
#define __HAL_RCC_TIM10_IS_CLK_SLEEP_DISABLED()  ((RCC->APB2LPENR & (RCC_APB2LPENR_TIM10LPEN)) == RESET)
#define __HAL_RCC_TIM11_IS_CLK_SLEEP_DISABLED()  ((RCC->APB2LPENR & (RCC_APB2LPENR_TIM11LPEN)) == RESET)
#define __HAL_RCC_SPI5_IS_CLK_SLEEP_DISABLED()   ((RCC->APB2LPENR & (RCC_APB2LPENR_SPI5LPEN)) == RESET)
#define __HAL_RCC_SPI6_IS_CLK_SLEEP_DISABLED()   ((RCC->APB2LPENR & (RCC_APB2LPENR_SPI6LPEN)) == RESET)
#define __HAL_RCC_SAI1_IS_CLK_SLEEP_DISABLED()   ((RCC->APB2LPENR & (RCC_APB2LPENR_SAI1LPEN)) == RESET)
#define __HAL_RCC_SAI2_IS_CLK_SLEEP_DISABLED()   ((RCC->APB2LPENR & (RCC_APB2LPENR_SAI2LPEN)) == RESET)
#if defined (STM32F746xx) || defined (STM32F756xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_LTDC_IS_CLK_SLEEP_DISABLED()   ((RCC->APB2LPENR & (RCC_APB2LPENR_LTDCLPEN)) == RESET)
#endif /* STM32F746xx || STM32F756xx || STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
#if defined (STM32F769xx) || defined (STM32F779xx)
#define __HAL_RCC_DSI_IS_CLK_SLEEP_DISABLED()     ((RCC->APB2LPENR & (RCC_APB2LPENR_DSILPEN)) == RESET)
#endif /* STM32F769xx || STM32F779xx */
#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define __HAL_RCC_SDMMC2_IS_CLK_SLEEP_DISABLED()  ((RCC->APB2LPENR & (RCC_APB2LPENR_SDMMC2LPEN)) == RESET)
#define __HAL_RCC_DFSDM1_IS_CLK_SLEEP_DISABLED()   ((RCC->APB2LPENR & (RCC_APB2LPENR_DFSDM1LPEN)) == RESET)
#define __HAL_RCC_MDIO_IS_CLK_SLEEP_DISABLED()    ((RCC->APB2LPENR & (RCC_APB2LPENR_MDIOLPEN)) == RESET)
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
/**
  * @}
  */

/*------------------------------- PLL Configuration --------------------------*/
#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
/** @brief  Macro to configure the main PLL clock source, multiplication and division factors.
  * @note   This function must be used only when the main PLL is disabled.
  * @param  __RCC_PLLSource__: specifies the PLL entry clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_PLLSOURCE_HSI: HSI oscillator clock selected as PLL clock entry
  *            @arg RCC_PLLSOURCE_HSE: HSE oscillator clock selected as PLL clock entry
  * @note   This clock source (RCC_PLLSource) is common for the main PLL and PLLI2S.
  * @param  __PLLM__: specifies the division factor for PLL VCO input clock
  *         This parameter must be a number between Min_Data = 2 and Max_Data = 63.
  * @note   You have to set the PLLM parameter correctly to ensure that the VCO input
  *         frequency ranges from 1 to 2 MHz. It is recommended to select a frequency
  *         of 2 MHz to limit PLL jitter.
  * @param  __PLLN__: specifies the multiplication factor for PLL VCO output clock
  *         This parameter must be a number between Min_Data = 50 and Max_Data = 432.
  * @note   You have to set the PLLN parameter correctly to ensure that the VCO
  *         output frequency is between 100 and 432 MHz.
  * @param  __PLLP__: specifies the division factor for main system clock (SYSCLK)
  *         This parameter must be a number in the range {2, 4, 6, or 8}.
  * @note   You have to set the PLLP parameter correctly to not exceed 216 MHz on
  *         the System clock frequency.
  * @param  __PLLQ__: specifies the division factor for OTG FS, SDMMC and RNG clocks
  *         This parameter must be a number between Min_Data = 2 and Max_Data = 15.
  * @note   If the USB OTG FS is used in your application, you have to set the
  *         PLLQ parameter correctly to have 48 MHz clock for the USB. However,
  *         the SDMMC and RNG need a frequency lower than or equal to 48 MHz to work
  *         correctly.
  * @param  __PLLR__: specifies the division factor for DSI clock
  *         This parameter must be a number between Min_Data = 2 and Max_Data = 7.
  */
#define __HAL_RCC_PLL_CONFIG(__RCC_PLLSource__, __PLLM__, __PLLN__, __PLLP__, __PLLQ__,__PLLR__)  \
                            (RCC->PLLCFGR = ((__RCC_PLLSource__) | (__PLLM__)                   | \
                            ((__PLLN__) << POSITION_VAL(RCC_PLLCFGR_PLLN))                      | \
                            ((((__PLLP__) >> 1) -1) << POSITION_VAL(RCC_PLLCFGR_PLLP))          | \
                            ((__PLLQ__) << POSITION_VAL(RCC_PLLCFGR_PLLQ))                      | \
                            ((__PLLR__) << POSITION_VAL(RCC_PLLCFGR_PLLR))))
#else
/** @brief  Macro to configure the main PLL clock source, multiplication and division factors.
  * @note   This function must be used only when the main PLL is disabled.
  * @param  __RCC_PLLSource__: specifies the PLL entry clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_PLLSOURCE_HSI: HSI oscillator clock selected as PLL clock entry
  *            @arg RCC_PLLSOURCE_HSE: HSE oscillator clock selected as PLL clock entry
  * @note   This clock source (RCC_PLLSource) is common for the main PLL and PLLI2S.
  * @param  __PLLM__: specifies the division factor for PLL VCO input clock
  *         This parameter must be a number between Min_Data = 2 and Max_Data = 63.
  * @note   You have to set the PLLM parameter correctly to ensure that the VCO input
  *         frequency ranges from 1 to 2 MHz. It is recommended to select a frequency
  *         of 2 MHz to limit PLL jitter.
  * @param  __PLLN__: specifies the multiplication factor for PLL VCO output clock
  *         This parameter must be a number between Min_Data = 50 and Max_Data = 432.
  * @note   You have to set the PLLN parameter correctly to ensure that the VCO
  *         output frequency is between 100 and 432 MHz.
  * @param  __PLLP__: specifies the division factor for main system clock (SYSCLK)
  *         This parameter must be a number in the range {2, 4, 6, or 8}.
  * @note   You have to set the PLLP parameter correctly to not exceed 216 MHz on
  *         the System clock frequency.
  * @param  __PLLQ__: specifies the division factor for OTG FS, SDMMC and RNG clocks
  *         This parameter must be a number between Min_Data = 2 and Max_Data = 15.
  * @note   If the USB OTG FS is used in your application, you have to set the
  *         PLLQ parameter correctly to have 48 MHz clock for the USB. However,
  *         the SDMMC and RNG need a frequency lower than or equal to 48 MHz to work
  *         correctly.
  */
#define __HAL_RCC_PLL_CONFIG(__RCC_PLLSource__, __PLLM__, __PLLN__, __PLLP__, __PLLQ__)     \
                            (RCC->PLLCFGR = (0x20000000 | (__RCC_PLLSource__) | (__PLLM__)| \
                            ((__PLLN__) << POSITION_VAL(RCC_PLLCFGR_PLLN))                | \
                            ((((__PLLP__) >> 1) -1) << POSITION_VAL(RCC_PLLCFGR_PLLP))    | \
                            ((__PLLQ__) << POSITION_VAL(RCC_PLLCFGR_PLLQ))))
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
/*---------------------------------------------------------------------------------------------*/

/** @brief  Macro to configure the Timers clocks prescalers
  * @param  __PRESC__ : specifies the Timers clocks prescalers selection
  *         This parameter can be one of the following values:
  *            @arg RCC_TIMPRES_DESACTIVATED: The Timers kernels clocks prescaler is
  *                 equal to HPRE if PPREx is corresponding to division by 1 or 2,
  *                 else it is equal to [(HPRE * PPREx) / 2] if PPREx is corresponding to
  *                 division by 4 or more.
  *            @arg RCC_TIMPRES_ACTIVATED: The Timers kernels clocks prescaler is
  *                 equal to HPRE if PPREx is corresponding to division by 1, 2 or 4,
  *                 else it is equal to [(HPRE * PPREx) / 4] if PPREx is corresponding
  *                 to division by 8 or more.
  */
#define __HAL_RCC_TIMCLKPRESCALER(__PRESC__) do {RCC->DCKCFGR1 &= ~(RCC_DCKCFGR1_TIMPRE);\
                                                 RCC->DCKCFGR1 |= (__PRESC__);           \
                                                }while(0)

/** @brief Macros to Enable or Disable the PLLISAI.
  * @note  The PLLSAI is disabled by hardware when entering STOP and STANDBY modes.
  */
#define __HAL_RCC_PLLSAI_ENABLE() (RCC->CR |= (RCC_CR_PLLSAION))
#define __HAL_RCC_PLLSAI_DISABLE() (RCC->CR &= ~(RCC_CR_PLLSAION))

/** @brief  Macro to configure the PLLSAI clock multiplication and division factors.
  * @note   This function must be used only when the PLLSAI is disabled.
  * @note   PLLSAI clock source is common with the main PLL (configured in
  *         RCC_PLLConfig function )
  * @param  __PLLSAIN__: specifies the multiplication factor for PLLSAI VCO output clock.
  *         This parameter must be a number between Min_Data = 50 and Max_Data = 432.
  * @note   You have to set the PLLSAIN parameter correctly to ensure that the VCO
  *         output frequency is between Min_Data = 100 and Max_Data = 432 MHz.
  * @param  __PLLSAIP__: specifies the division factor for USB, RNG, SDMMC clocks
  *         This parameter can be a value of @ref RCCEx_PLLSAIP_Clock_Divider.
  * @param  __PLLSAIQ__: specifies the division factor for SAI clock
  *         This parameter must be a number between Min_Data = 2 and Max_Data = 15.
  * @param  __PLLSAIR__: specifies the division factor for LTDC clock
  *         This parameter must be a number between Min_Data = 2 and Max_Data = 7.
  */
#define __HAL_RCC_PLLSAI_CONFIG(__PLLSAIN__, __PLLSAIP__, __PLLSAIQ__, __PLLSAIR__)                        \
                               (RCC->PLLSAICFGR = ((__PLLSAIN__) << POSITION_VAL(RCC_PLLSAICFGR_PLLSAIN)) |\
                               ((__PLLSAIP__) << POSITION_VAL(RCC_PLLSAICFGR_PLLSAIP))                    |\
                               ((__PLLSAIQ__) << POSITION_VAL(RCC_PLLSAICFGR_PLLSAIQ))                    |\
                               ((__PLLSAIR__) << POSITION_VAL(RCC_PLLSAICFGR_PLLSAIR)))

/** @brief  Macro to configure the PLLI2S clock multiplication and division factors.
  * @note   This macro must be used only when the PLLI2S is disabled.
  * @note   PLLI2S clock source is common with the main PLL (configured in
  *         HAL_RCC_ClockConfig() API)
  * @param  __PLLI2SN__: specifies the multiplication factor for PLLI2S VCO output clock.
  *         This parameter must be a number between Min_Data = 50 and Max_Data = 432.
  * @note   You have to set the PLLI2SN parameter correctly to ensure that the VCO
  *         output frequency is between Min_Data = 100 and Max_Data = 432 MHz.
  * @param  __PLLI2SP__: specifies the division factor for SPDDIF-RX clock.
  *         This parameter can be a value of @ref RCCEx_PLLI2SP_Clock_Divider.
  * @param  __PLLI2SQ__: specifies the division factor for SAI clock.
  *         This parameter must be a number between Min_Data = 2 and Max_Data = 15.
  * @param  __PLLI2SR__: specifies the division factor for I2S clock
  *         This parameter must be a number between Min_Data = 2 and Max_Data = 7.
  * @note   You have to set the PLLI2SR parameter correctly to not exceed 192 MHz
  *         on the I2S clock frequency.
  */
#define __HAL_RCC_PLLI2S_CONFIG(__PLLI2SN__, __PLLI2SP__, __PLLI2SQ__, __PLLI2SR__)                        \
                               (RCC->PLLI2SCFGR = ((__PLLI2SN__) << POSITION_VAL(RCC_PLLI2SCFGR_PLLI2SN)) |\
                               ((__PLLI2SP__) << POSITION_VAL(RCC_PLLI2SCFGR_PLLI2SP))                    |\
                               ((__PLLI2SQ__) << POSITION_VAL(RCC_PLLI2SCFGR_PLLI2SQ))                    |\
                               ((__PLLI2SR__) << POSITION_VAL(RCC_PLLI2SCFGR_PLLI2SR)))

/** @brief  Macro to configure the SAI clock Divider coming from PLLI2S.
  * @note   This function must be called before enabling the PLLI2S.
  * @param  __PLLI2SDivQ__: specifies the PLLI2S division factor for SAI1 clock .
  *          This parameter must be a number between 1 and 32.
  *          SAI1 clock frequency = f(PLLI2SQ) / __PLLI2SDivQ__
  */
#define __HAL_RCC_PLLI2S_PLLSAICLKDIVQ_CONFIG(__PLLI2SDivQ__) (MODIFY_REG(RCC->DCKCFGR1, RCC_DCKCFGR1_PLLI2SDIVQ, (__PLLI2SDivQ__)-1))

/** @brief  Macro to configure the SAI clock Divider coming from PLLSAI.
  * @note   This function must be called before enabling the PLLSAI.
  * @param  __PLLSAIDivQ__: specifies the PLLSAI division factor for SAI1 clock .
  *         This parameter must be a number between Min_Data = 1 and Max_Data = 32.
  *         SAI1 clock frequency = f(PLLSAIQ) / __PLLSAIDivQ__
  */
#define __HAL_RCC_PLLSAI_PLLSAICLKDIVQ_CONFIG(__PLLSAIDivQ__) (MODIFY_REG(RCC->DCKCFGR1, RCC_DCKCFGR1_PLLSAIDIVQ, ((__PLLSAIDivQ__)-1)<<8))

/** @brief  Macro to configure the LTDC clock Divider coming from PLLSAI.
  *
  * @note   This function must be called before enabling the PLLSAI.
  * @param  __PLLSAIDivR__: specifies the PLLSAI division factor for LTDC clock .
  *          This parameter can be a value of @ref RCCEx_PLLSAI_DIVR.
  *          LTDC clock frequency = f(PLLSAIR) / __PLLSAIDivR__
  */
#define __HAL_RCC_PLLSAI_PLLSAICLKDIVR_CONFIG(__PLLSAIDivR__)\
                            MODIFY_REG(RCC->DCKCFGR1, RCC_DCKCFGR1_PLLSAIDIVR, (uint32_t)(__PLLSAIDivR__))

/** @brief  Macro to configure SAI1 clock source selection.
  * @note   This function must be called before enabling PLLSAI, PLLI2S and
  *         the SAI clock.
  * @param  __SOURCE__: specifies the SAI1 clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_SAI1CLKSOURCE_PLLI2S: PLLI2S_Q clock divided by PLLI2SDIVQ used
  *                                           as SAI1 clock.
  *            @arg RCC_SAI1CLKSOURCE_PLLSAI: PLLISAI_Q clock divided by PLLSAIDIVQ used
  *                                           as SAI1 clock.
  *            @arg RCC_SAI1CLKSOURCE_PIN: External clock mapped on the I2S_CKIN pin
  *                                        used as SAI1 clock.
  *            @arg RCC_SAI1CLKSOURCE_PLLSRC: HSI or HSE depending from PLL Source clock
  *                                           used as SAI1 clock.
  * @note      The RCC_SAI1CLKSOURCE_PLLSRC value is only available with STM32F767/769/777/779xx Devices
  */
#define __HAL_RCC_SAI1_CONFIG(__SOURCE__)\
                             MODIFY_REG(RCC->DCKCFGR1, RCC_DCKCFGR1_SAI1SEL, (uint32_t)(__SOURCE__))

/** @brief  Macro to get the SAI1 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_SAI1CLKSOURCE_PLLI2S: PLLI2S_Q clock divided by PLLI2SDIVQ used
  *                                           as SAI1 clock.
  *            @arg RCC_SAI1CLKSOURCE_PLLSAI: PLLISAI_Q clock divided by PLLSAIDIVQ used
  *                                           as SAI1 clock.
  *            @arg RCC_SAI1CLKSOURCE_PIN: External clock mapped on the I2S_CKIN pin
  *                                        used as SAI1 clock.
  *            @arg RCC_SAI1CLKSOURCE_PLLSRC: HSI or HSE depending from PLL Source clock
  *                                           used as SAI1 clock.
  * @note      The RCC_SAI1CLKSOURCE_PLLSRC value is only available with STM32F767/769/777/779xx Devices
  */
#define __HAL_RCC_GET_SAI1_SOURCE() ((uint32_t)(READ_BIT(RCC->DCKCFGR1, RCC_DCKCFGR1_SAI1SEL)))


/** @brief  Macro to configure SAI2 clock source selection.
  * @note   This function must be called before enabling PLLSAI, PLLI2S and
  *         the SAI clock.
  * @param  __SOURCE__: specifies the SAI2 clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_SAI2CLKSOURCE_PLLI2S: PLLI2S_Q clock divided by PLLI2SDIVQ used
  *                                           as SAI2 clock.
  *            @arg RCC_SAI2CLKSOURCE_PLLSAI: PLLISAI_Q clock divided by PLLSAIDIVQ used
  *                                           as SAI2 clock.
  *            @arg RCC_SAI2CLKSOURCE_PIN: External clock mapped on the I2S_CKIN pin
  *                                        used as SAI2 clock.
  *            @arg RCC_SAI2CLKSOURCE_PLLSRC: HSI or HSE depending from PLL Source clock
  *                                           used as SAI2 clock.
  * @note      The RCC_SAI2CLKSOURCE_PLLSRC value is only available with STM32F767/769/777/779xx Devices
  */
#define __HAL_RCC_SAI2_CONFIG(__SOURCE__)\
                            MODIFY_REG(RCC->DCKCFGR1, RCC_DCKCFGR1_SAI2SEL, (uint32_t)(__SOURCE__))


/** @brief  Macro to get the SAI2 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_SAI2CLKSOURCE_PLLI2S: PLLI2S_Q clock divided by PLLI2SDIVQ used
  *                                           as SAI2 clock.
  *            @arg RCC_SAI2CLKSOURCE_PLLSAI: PLLISAI_Q clock divided by PLLSAIDIVQ used
  *                                           as SAI2 clock.
  *            @arg RCC_SAI2CLKSOURCE_PIN: External clock mapped on the I2S_CKIN pin
  *                                        used as SAI2 clock.
  *            @arg RCC_SAI2CLKSOURCE_PLLSRC: HSI or HSE depending from PLL Source clock
  *                                           used as SAI2 clock.
  * @note      The RCC_SAI2CLKSOURCE_PLLSRC value is only available with STM32F767/769/777/779xx Devices
  */
#define __HAL_RCC_GET_SAI2_SOURCE() ((uint32_t)(READ_BIT(RCC->DCKCFGR1, RCC_DCKCFGR1_SAI2SEL)))


/** @brief Enable PLLSAI_RDY interrupt.
  */
#define __HAL_RCC_PLLSAI_ENABLE_IT() (RCC->CIR |= (RCC_CIR_PLLSAIRDYIE))

/** @brief Disable PLLSAI_RDY interrupt.
  */
#define __HAL_RCC_PLLSAI_DISABLE_IT() (RCC->CIR &= ~(RCC_CIR_PLLSAIRDYIE))

/** @brief Clear the PLLSAI RDY interrupt pending bits.
  */
#define __HAL_RCC_PLLSAI_CLEAR_IT() (RCC->CIR |= (RCC_CIR_PLLSAIRDYF))

/** @brief Check the PLLSAI RDY interrupt has occurred or not.
  * @retval The new state (TRUE or FALSE).
  */
#define __HAL_RCC_PLLSAI_GET_IT() ((RCC->CIR & (RCC_CIR_PLLSAIRDYIE)) == (RCC_CIR_PLLSAIRDYIE))

/** @brief  Check PLLSAI RDY flag is set or not.
  * @retval The new state (TRUE or FALSE).
  */
#define __HAL_RCC_PLLSAI_GET_FLAG() ((RCC->CR & (RCC_CR_PLLSAIRDY)) == (RCC_CR_PLLSAIRDY))

/** @brief  Macro to Get I2S clock source selection.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_I2SCLKSOURCE_PLLI2S: PLLI2S VCO output clock divided by PLLI2SR used as I2S clock.
  *            @arg RCC_I2SCLKSOURCE_EXT: External clock mapped on the I2S_CKIN pin used as I2S clock source
  */
#define __HAL_RCC_GET_I2SCLKSOURCE() (READ_BIT(RCC->CFGR, RCC_CFGR_I2SSRC))

/** @brief  Macro to configure the I2C1 clock (I2C1CLK).
  *
  * @param  __I2C1_CLKSOURCE__: specifies the I2C1 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_I2C1CLKSOURCE_PCLK1: PCLK1 selected as I2C1 clock
  *            @arg RCC_I2C1CLKSOURCE_HSI: HSI selected as I2C1 clock
  *            @arg RCC_I2C1CLKSOURCE_SYSCLK: System Clock selected as I2C1 clock
  */
#define __HAL_RCC_I2C1_CONFIG(__I2C1_CLKSOURCE__) \
                  MODIFY_REG(RCC->DCKCFGR2, RCC_DCKCFGR2_I2C1SEL, (uint32_t)(__I2C1_CLKSOURCE__))

/** @brief  Macro to get the I2C1 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_I2C1CLKSOURCE_PCLK1: PCLK1 selected as I2C1 clock
  *            @arg RCC_I2C1CLKSOURCE_HSI: HSI selected as I2C1 clock
  *            @arg RCC_I2C1CLKSOURCE_SYSCLK: System Clock selected as I2C1 clock
  */
#define __HAL_RCC_GET_I2C1_SOURCE() ((uint32_t)(READ_BIT(RCC->DCKCFGR2, RCC_DCKCFGR2_I2C1SEL)))

/** @brief  Macro to configure the I2C2 clock (I2C2CLK).
  *
  * @param  __I2C2_CLKSOURCE__: specifies the I2C2 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_I2C2CLKSOURCE_PCLK1: PCLK1 selected as I2C2 clock
  *            @arg RCC_I2C2CLKSOURCE_HSI: HSI selected as I2C2 clock
  *            @arg RCC_I2C2CLKSOURCE_SYSCLK: System Clock selected as I2C2 clock
  */
#define __HAL_RCC_I2C2_CONFIG(__I2C2_CLKSOURCE__) \
                  MODIFY_REG(RCC->DCKCFGR2, RCC_DCKCFGR2_I2C2SEL, (uint32_t)(__I2C2_CLKSOURCE__))

/** @brief  Macro to get the I2C2 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_I2C2CLKSOURCE_PCLK1: PCLK1 selected as I2C2 clock
  *            @arg RCC_I2C2CLKSOURCE_HSI: HSI selected as I2C2 clock
  *            @arg RCC_I2C2CLKSOURCE_SYSCLK: System Clock selected as I2C2 clock
  */
#define __HAL_RCC_GET_I2C2_SOURCE() ((uint32_t)(READ_BIT(RCC->DCKCFGR2, RCC_DCKCFGR2_I2C2SEL)))

/** @brief  Macro to configure the I2C3 clock (I2C3CLK).
  *
  * @param  __I2C3_CLKSOURCE__: specifies the I2C3 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_I2C3CLKSOURCE_PCLK1: PCLK1 selected as I2C3 clock
  *            @arg RCC_I2C3CLKSOURCE_HSI: HSI selected as I2C3 clock
  *            @arg RCC_I2C3CLKSOURCE_SYSCLK: System Clock selected as I2C3 clock
  */
#define __HAL_RCC_I2C3_CONFIG(__I2C3_CLKSOURCE__) \
                  MODIFY_REG(RCC->DCKCFGR2, RCC_DCKCFGR2_I2C3SEL, (uint32_t)(__I2C3_CLKSOURCE__))

/** @brief  macro to get the I2C3 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_I2C3CLKSOURCE_PCLK1: PCLK1 selected as I2C3 clock
  *            @arg RCC_I2C3CLKSOURCE_HSI: HSI selected as I2C3 clock
  *            @arg RCC_I2C3CLKSOURCE_SYSCLK: System Clock selected as I2C3 clock
  */
#define __HAL_RCC_GET_I2C3_SOURCE() ((uint32_t)(READ_BIT(RCC->DCKCFGR2, RCC_DCKCFGR2_I2C3SEL)))

/** @brief  Macro to configure the I2C4 clock (I2C4CLK).
  *
  * @param  __I2C4_CLKSOURCE__: specifies the I2C4 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_I2C4CLKSOURCE_PCLK1: PCLK1 selected as I2C4 clock
  *            @arg RCC_I2C4CLKSOURCE_HSI: HSI selected as I2C4 clock
  *            @arg RCC_I2C4CLKSOURCE_SYSCLK: System Clock selected as I2C4 clock
  */
#define __HAL_RCC_I2C4_CONFIG(__I2C4_CLKSOURCE__) \
                  MODIFY_REG(RCC->DCKCFGR2, RCC_DCKCFGR2_I2C4SEL, (uint32_t)(__I2C4_CLKSOURCE__))

/** @brief  macro to get the I2C4 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_I2C4CLKSOURCE_PCLK1: PCLK1 selected as I2C4 clock
  *            @arg RCC_I2C4CLKSOURCE_HSI: HSI selected as I2C4 clock
  *            @arg RCC_I2C4CLKSOURCE_SYSCLK: System Clock selected as I2C4 clock
  */
#define __HAL_RCC_GET_I2C4_SOURCE() ((uint32_t)(READ_BIT(RCC->DCKCFGR2, RCC_DCKCFGR2_I2C4SEL)))

/** @brief  Macro to configure the USART1 clock (USART1CLK).
  *
  * @param  __USART1_CLKSOURCE__: specifies the USART1 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_USART1CLKSOURCE_PCLK2: PCLK2 selected as USART1 clock
  *            @arg RCC_USART1CLKSOURCE_HSI: HSI selected as USART1 clock
  *            @arg RCC_USART1CLKSOURCE_SYSCLK: System Clock selected as USART1 clock
  *            @arg RCC_USART1CLKSOURCE_LSE: LSE selected as USART1 clock
  */
#define __HAL_RCC_USART1_CONFIG(__USART1_CLKSOURCE__) \
                  MODIFY_REG(RCC->DCKCFGR2, RCC_DCKCFGR2_USART1SEL, (uint32_t)(__USART1_CLKSOURCE__))

/** @brief  macro to get the USART1 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_USART1CLKSOURCE_PCLK2: PCLK2 selected as USART1 clock
  *            @arg RCC_USART1CLKSOURCE_HSI: HSI selected as USART1 clock
  *            @arg RCC_USART1CLKSOURCE_SYSCLK: System Clock selected as USART1 clock
  *            @arg RCC_USART1CLKSOURCE_LSE: LSE selected as USART1 clock
  */
#define __HAL_RCC_GET_USART1_SOURCE() ((uint32_t)(READ_BIT(RCC->DCKCFGR2, RCC_DCKCFGR2_USART1SEL)))

/** @brief  Macro to configure the USART2 clock (USART2CLK).
  *
  * @param  __USART2_CLKSOURCE__: specifies the USART2 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_USART2CLKSOURCE_PCLK1: PCLK1 selected as USART2 clock
  *            @arg RCC_USART2CLKSOURCE_HSI: HSI selected as USART2 clock
  *            @arg RCC_USART2CLKSOURCE_SYSCLK: System Clock selected as USART2 clock
  *            @arg RCC_USART2CLKSOURCE_LSE: LSE selected as USART2 clock
  */
#define __HAL_RCC_USART2_CONFIG(__USART2_CLKSOURCE__) \
                  MODIFY_REG(RCC->DCKCFGR2, RCC_DCKCFGR2_USART2SEL, (uint32_t)(__USART2_CLKSOURCE__))

/** @brief  macro to get the USART2 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_USART2CLKSOURCE_PCLK1: PCLK1 selected as USART2 clock
  *            @arg RCC_USART2CLKSOURCE_HSI: HSI selected as USART2 clock
  *            @arg RCC_USART2CLKSOURCE_SYSCLK: System Clock selected as USART2 clock
  *            @arg RCC_USART2CLKSOURCE_LSE: LSE selected as USART2 clock
  */
#define __HAL_RCC_GET_USART2_SOURCE() ((uint32_t)(READ_BIT(RCC->DCKCFGR2, RCC_DCKCFGR2_USART2SEL)))

/** @brief  Macro to configure the USART3 clock (USART3CLK).
  *
  * @param  __USART3_CLKSOURCE__: specifies the USART3 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_USART3CLKSOURCE_PCLK1: PCLK1 selected as USART3 clock
  *            @arg RCC_USART3CLKSOURCE_HSI: HSI selected as USART3 clock
  *            @arg RCC_USART3CLKSOURCE_SYSCLK: System Clock selected as USART3 clock
  *            @arg RCC_USART3CLKSOURCE_LSE: LSE selected as USART3 clock
  */
#define __HAL_RCC_USART3_CONFIG(__USART3_CLKSOURCE__) \
                  MODIFY_REG(RCC->DCKCFGR2, RCC_DCKCFGR2_USART3SEL, (uint32_t)(__USART3_CLKSOURCE__))

/** @brief  macro to get the USART3 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_USART3CLKSOURCE_PCLK1: PCLK1 selected as USART3 clock
  *            @arg RCC_USART3CLKSOURCE_HSI: HSI selected as USART3 clock
  *            @arg RCC_USART3CLKSOURCE_SYSCLK: System Clock selected as USART3 clock
  *            @arg RCC_USART3CLKSOURCE_LSE: LSE selected as USART3 clock
  */
#define __HAL_RCC_GET_USART3_SOURCE() ((uint32_t)(READ_BIT(RCC->DCKCFGR2, RCC_DCKCFGR2_USART3SEL)))

 /** @brief  Macro to configure the UART4 clock (UART4CLK).
  *
  * @param  __UART4_CLKSOURCE__: specifies the UART4 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_UART4CLKSOURCE_PCLK1: PCLK1 selected as UART4 clock
  *            @arg RCC_UART4CLKSOURCE_HSI: HSI selected as UART4 clock
  *            @arg RCC_UART4CLKSOURCE_SYSCLK: System Clock selected as UART4 clock
  *            @arg RCC_UART4CLKSOURCE_LSE: LSE selected as UART4 clock
  */
#define __HAL_RCC_UART4_CONFIG(__UART4_CLKSOURCE__) \
                  MODIFY_REG(RCC->DCKCFGR2, RCC_DCKCFGR2_UART4SEL, (uint32_t)(__UART4_CLKSOURCE__))

/** @brief  macro to get the UART4 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_UART4CLKSOURCE_PCLK1: PCLK1 selected as UART4 clock
  *            @arg RCC_UART4CLKSOURCE_HSI: HSI selected as UART4 clock
  *            @arg RCC_UART4CLKSOURCE_SYSCLK: System Clock selected as UART4 clock
  *            @arg RCC_UART4CLKSOURCE_LSE: LSE selected as UART4 clock
  */
#define __HAL_RCC_GET_UART4_SOURCE() ((uint32_t)(READ_BIT(RCC->DCKCFGR2, RCC_DCKCFGR2_UART4SEL)))

 /** @brief  Macro to configure the UART5 clock (UART5CLK).
  *
  * @param  __UART5_CLKSOURCE__: specifies the UART5 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_UART5CLKSOURCE_PCLK1: PCLK1 selected as UART5 clock
  *            @arg RCC_UART5CLKSOURCE_HSI: HSI selected as UART5 clock
  *            @arg RCC_UART5CLKSOURCE_SYSCLK: System Clock selected as UART5 clock
  *            @arg RCC_UART5CLKSOURCE_LSE: LSE selected as UART5 clock
  */
#define __HAL_RCC_UART5_CONFIG(__UART5_CLKSOURCE__) \
                  MODIFY_REG(RCC->DCKCFGR2, RCC_DCKCFGR2_UART5SEL, (uint32_t)(__UART5_CLKSOURCE__))

/** @brief  macro to get the UART5 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_UART5CLKSOURCE_PCLK1: PCLK1 selected as UART5 clock
  *            @arg RCC_UART5CLKSOURCE_HSI: HSI selected as UART5 clock
  *            @arg RCC_UART5CLKSOURCE_SYSCLK: System Clock selected as UART5 clock
  *            @arg RCC_UART5CLKSOURCE_LSE: LSE selected as UART5 clock
  */
#define __HAL_RCC_GET_UART5_SOURCE() ((uint32_t)(READ_BIT(RCC->DCKCFGR2, RCC_DCKCFGR2_UART5SEL)))

 /** @brief  Macro to configure the USART6 clock (USART6CLK).
  *
  * @param  __USART6_CLKSOURCE__: specifies the USART6 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_USART6CLKSOURCE_PCLK1: PCLK1 selected as USART6 clock
  *            @arg RCC_USART6CLKSOURCE_HSI: HSI selected as USART6 clock
  *            @arg RCC_USART6CLKSOURCE_SYSCLK: System Clock selected as USART6 clock
  *            @arg RCC_USART6CLKSOURCE_LSE: LSE selected as USART6 clock
  */
#define __HAL_RCC_USART6_CONFIG(__USART6_CLKSOURCE__) \
                  MODIFY_REG(RCC->DCKCFGR2, RCC_DCKCFGR2_USART6SEL, (uint32_t)(__USART6_CLKSOURCE__))

/** @brief  macro to get the USART6 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_USART6CLKSOURCE_PCLK1: PCLK1 selected as USART6 clock
  *            @arg RCC_USART6CLKSOURCE_HSI: HSI selected as USART6 clock
  *            @arg RCC_USART6CLKSOURCE_SYSCLK: System Clock selected as USART6 clock
  *            @arg RCC_USART6CLKSOURCE_LSE: LSE selected as USART6 clock
  */
#define __HAL_RCC_GET_USART6_SOURCE() ((uint32_t)(READ_BIT(RCC->DCKCFGR2, RCC_DCKCFGR2_USART6SEL)))

 /** @brief  Macro to configure the UART7 clock (UART7CLK).
  *
  * @param  __UART7_CLKSOURCE__: specifies the UART7 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_UART7CLKSOURCE_PCLK1: PCLK1 selected as UART7 clock
  *            @arg RCC_UART7CLKSOURCE_HSI: HSI selected as UART7 clock
  *            @arg RCC_UART7CLKSOURCE_SYSCLK: System Clock selected as UART7 clock
  *            @arg RCC_UART7CLKSOURCE_LSE: LSE selected as UART7 clock
  */
#define __HAL_RCC_UART7_CONFIG(__UART7_CLKSOURCE__) \
                  MODIFY_REG(RCC->DCKCFGR2, RCC_DCKCFGR2_UART7SEL, (uint32_t)(__UART7_CLKSOURCE__))

/** @brief  macro to get the UART7 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_UART7CLKSOURCE_PCLK1: PCLK1 selected as UART7 clock
  *            @arg RCC_UART7CLKSOURCE_HSI: HSI selected as UART7 clock
  *            @arg RCC_UART7CLKSOURCE_SYSCLK: System Clock selected as UART7 clock
  *            @arg RCC_UART7CLKSOURCE_LSE: LSE selected as UART7 clock
  */
#define __HAL_RCC_GET_UART7_SOURCE() ((uint32_t)(READ_BIT(RCC->DCKCFGR2, RCC_DCKCFGR2_UART7SEL)))

/** @brief  Macro to configure the UART8 clock (UART8CLK).
  *
  * @param  __UART8_CLKSOURCE__: specifies the UART8 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_UART8CLKSOURCE_PCLK1: PCLK1 selected as UART8 clock
  *            @arg RCC_UART8CLKSOURCE_HSI: HSI selected as UART8 clock
  *            @arg RCC_UART8CLKSOURCE_SYSCLK: System Clock selected as UART8 clock
  *            @arg RCC_UART8CLKSOURCE_LSE: LSE selected as UART8 clock
  */
#define __HAL_RCC_UART8_CONFIG(__UART8_CLKSOURCE__) \
                  MODIFY_REG(RCC->DCKCFGR2, RCC_DCKCFGR2_UART8SEL, (uint32_t)(__UART8_CLKSOURCE__))

/** @brief  macro to get the UART8 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_UART8CLKSOURCE_PCLK1: PCLK1 selected as UART8 clock
  *            @arg RCC_UART8CLKSOURCE_HSI: HSI selected as UART8 clock
  *            @arg RCC_UART8CLKSOURCE_SYSCLK: System Clock selected as UART8 clock
  *            @arg RCC_UART8CLKSOURCE_LSE: LSE selected as UART8 clock
  */
#define __HAL_RCC_GET_UART8_SOURCE() ((uint32_t)(READ_BIT(RCC->DCKCFGR2, RCC_DCKCFGR2_UART8SEL)))

/** @brief  Macro to configure the LPTIM1 clock (LPTIM1CLK).
  *
  * @param  __LPTIM1_CLKSOURCE__: specifies the LPTIM1 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_LPTIM1CLKSOURCE_PCLK: PCLK selected as LPTIM1 clock
  *            @arg RCC_LPTIM1CLKSOURCE_HSI: HSI selected as LPTIM1 clock
  *            @arg RCC_LPTIM1CLKSOURCE_LSI: LSI selected as LPTIM1 clock
  *            @arg RCC_LPTIM1CLKSOURCE_LSE: LSE selected as LPTIM1 clock
  */
#define __HAL_RCC_LPTIM1_CONFIG(__LPTIM1_CLKSOURCE__) \
                  MODIFY_REG(RCC->DCKCFGR2, RCC_DCKCFGR2_LPTIM1SEL, (uint32_t)(__LPTIM1_CLKSOURCE__))

/** @brief  macro to get the LPTIM1 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_LPTIM1CLKSOURCE_PCLK: PCLK selected as LPTIM1 clock
  *            @arg RCC_LPTIM1CLKSOURCE_HSI: HSI selected as LPTIM1 clock
  *            @arg RCC_LPTIM1CLKSOURCE_LSI: LSI selected as LPTIM1 clock
  *            @arg RCC_LPTIM1CLKSOURCE_LSE: LSE selected as LPTIM1 clock
  */
#define __HAL_RCC_GET_LPTIM1_SOURCE() ((uint32_t)(READ_BIT(RCC->DCKCFGR2, RCC_DCKCFGR2_LPTIM1SEL)))

/** @brief  Macro to configure the CEC clock (CECCLK).
  *
  * @param  __CEC_CLKSOURCE__: specifies the CEC clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_CECCLKSOURCE_LSE: LSE selected as CEC clock
  *            @arg RCC_CECCLKSOURCE_HSI: HSI divided by 488 selected as CEC clock
  */
#define __HAL_RCC_CEC_CONFIG(__CEC_CLKSOURCE__) \
                  MODIFY_REG(RCC->DCKCFGR2, RCC_DCKCFGR2_CECSEL, (uint32_t)(__CEC_CLKSOURCE__))

/** @brief  macro to get the CEC clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_CECCLKSOURCE_LSE: LSE selected as CEC clock
  *            @arg RCC_CECCLKSOURCE_HSI: HSI selected as CEC clock
  */
#define __HAL_RCC_GET_CEC_SOURCE() ((uint32_t)(READ_BIT(RCC->DCKCFGR2, RCC_DCKCFGR2_CECSEL)))

/** @brief  Macro to configure the CLK48 source (CLK48CLK).
  *
  * @param  __CLK48_SOURCE__: specifies the CLK48 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_CLK48SOURCE_PLL: PLL selected as CLK48 source
  *            @arg RCC_CLK48SOURCE_PLLSAIP: PLLSAIP selected as CLK48 source
  */
#define __HAL_RCC_CLK48_CONFIG(__CLK48_SOURCE__) \
                  MODIFY_REG(RCC->DCKCFGR2, RCC_DCKCFGR2_CK48MSEL, (uint32_t)(__CLK48_SOURCE__))

/** @brief  macro to get the CLK48 source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_CLK48SOURCE_PLL: PLL used as CLK48 source
  *            @arg RCC_CLK48SOURCE_PLLSAIP: PLLSAIP used as CLK48 source
  */
#define __HAL_RCC_GET_CLK48_SOURCE() ((uint32_t)(READ_BIT(RCC->DCKCFGR2, RCC_DCKCFGR2_CK48MSEL)))

/** @brief  Macro to configure the SDMMC1 clock (SDMMC1CLK).
  *
  * @param  __SDMMC1_CLKSOURCE__: specifies the SDMMC1 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_SDMMC1CLKSOURCE_CLK48: CLK48 selected as SDMMC clock
  *            @arg RCC_SDMMC1CLKSOURCE_SYSCLK: SYSCLK selected as SDMMC clock
  */
#define __HAL_RCC_SDMMC1_CONFIG(__SDMMC1_CLKSOURCE__) \
                  MODIFY_REG(RCC->DCKCFGR2, RCC_DCKCFGR2_SDMMC1SEL, (uint32_t)(__SDMMC1_CLKSOURCE__))

/** @brief  macro to get the SDMMC1 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_SDMMC1CLKSOURCE_CLK48: CLK48 selected as SDMMC1 clock
  *            @arg RCC_SDMMC1CLKSOURCE_SYSCLK: SYSCLK selected as SDMMC1 clock
  */
#define __HAL_RCC_GET_SDMMC1_SOURCE() ((uint32_t)(READ_BIT(RCC->DCKCFGR2, RCC_DCKCFGR2_SDMMC1SEL)))

#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
/** @brief  Macro to configure the SDMMC2 clock (SDMMC2CLK).
  * @param  __SDMMC2_CLKSOURCE__: specifies the SDMMC2 clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_SDMMC2CLKSOURCE_CLK48: CLK48 selected as SDMMC2 clock
  *            @arg RCC_SDMMC2CLKSOURCE_SYSCLK: SYSCLK selected as SDMMC2 clock
  */
#define __HAL_RCC_SDMMC2_CONFIG(__SDMMC2_CLKSOURCE__) \
                  MODIFY_REG(RCC->DCKCFGR2, RCC_DCKCFGR2_SDMMC2SEL, (uint32_t)(__SDMMC2_CLKSOURCE__))

/** @brief  macro to get the SDMMC2 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_SDMMC2CLKSOURCE_CLK48: CLK48 selected as SDMMC2 clock
  *            @arg RCC_SDMMC2CLKSOURCE_SYSCLK: SYSCLK selected as SDMMC2 clock
  */
#define __HAL_RCC_GET_SDMMC2_SOURCE() ((uint32_t)(READ_BIT(RCC->DCKCFGR2, RCC_DCKCFGR2_SDMMC2SEL)))

/** @brief  Macro to configure the DFSDM1 clock
  * @param  __DFSDM1_CLKSOURCE__: specifies the DFSDM1  clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_DFSDM1CLKSOURCE_PCLK: PCLK2 Clock selected as DFSDM clock
  *            @arg RCC_DFSDMCLKSOURCE_SYSCLK: System Clock selected as DFSDM clock
  */
#define __HAL_RCC_DFSDM1_CONFIG(__DFSDM1_CLKSOURCE__) \
                  MODIFY_REG(RCC->DCKCFGR1, RCC_DCKCFGR1_DFSDM1SEL, (uint32_t)(__DFSDM1_CLKSOURCE__))

/** @brief  Macro to get the DFSDM1 clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_DFSDM1CLKSOURCE_PCLK:  PCLK2 Clock selected as DFSDM1 clock
  *            @arg RCC_DFSDM1CLKSOURCE_SYSCLK:   System Clock selected as DFSDM1 clock
  */
#define __HAL_RCC_GET_DFSDM1_SOURCE() ((uint32_t)(READ_BIT(RCC->DCKCFGR1, RCC_DCKCFGR1_DFSDM1SEL)))

/** @brief  Macro to configure the DFSDM1 Audio clock
  * @param  __DFSDM1AUDIO_CLKSOURCE__: specifies the DFSDM1 Audio clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_DFSDM1AUDIOCLKSOURCE_SAI1:  SAI1 Clock selected as DFSDM1 Audio clock
  *            @arg RCC_DFSDM1AUDIOCLKSOURCE_SAI2:  SAI2 Clock selected as DFSDM1 Audio clock
  */
#define __HAL_RCC_DFSDM1AUDIO_CONFIG(__DFSDM1AUDIO_CLKSOURCE__) \
                  MODIFY_REG(RCC->DCKCFGR1, RCC_DCKCFGR1_ADFSDM1SEL, (uint32_t)(__DFSDM1AUDIO_CLKSOURCE__))

/** @brief  Macro to get the DFSDM1 Audio clock source.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_DFSDM1AUDIOCLKSOURCE_SAI1:  SAI1 Clock selected as DFSDM1 Audio clock
  *            @arg RCC_DFSDM1AUDIOCLKSOURCE_SAI2:  SAI2 Clock selected as DFSDM1 Audio clock
  */
#define __HAL_RCC_GET_DFSDM1AUDIO_SOURCE() ((uint32_t)(READ_BIT(RCC->DCKCFGR1, RCC_DCKCFGR1_ADFSDM1SEL)))
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */

#if defined (STM32F769xx) || defined (STM32F779xx)
/** @brief  Macro to configure the DSI clock.
  * @param  __DSI_CLKSOURCE__: specifies the DSI clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_DSICLKSOURCE_PLLR: PLLR output used as DSI clock.
  *            @arg RCC_DSICLKSOURCE_DSIPHY: DSI-PHY output used as DSI clock.
  */
#define __HAL_RCC_DSI_CONFIG(__DSI_CLKSOURCE__) (MODIFY_REG(RCC->DCKCFGR2, RCC_DCKCFGR2_DSISEL, (uint32_t)(__DSI_CLKSOURCE__)))

/** @brief  Macro to Get the DSI clock.
  * @retval The clock source can be one of the following values:
  *            @arg RCC_DSICLKSOURCE_PLLR: PLLR output used as DSI clock.
  *            @arg RCC_DSICLKSOURCE_DSIPHY: DSI-PHY output used as DSI clock.
  */
#define __HAL_RCC_GET_DSI_SOURCE() (READ_BIT(RCC->DCKCFGR2, RCC_DCKCFGR2_DSISEL))
#endif /* STM32F769xx || STM32F779xx */
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup RCCEx_Exported_Functions_Group1
  * @{
  */
HAL_StatusTypeDef HAL_RCCEx_PeriphCLKConfig(RCC_PeriphCLKInitTypeDef  *PeriphClkInit);
void HAL_RCCEx_GetPeriphCLKConfig(RCC_PeriphCLKInitTypeDef  *PeriphClkInit);
uint32_t HAL_RCCEx_GetPeriphCLKFreq(uint32_t PeriphClk);

/**
  * @}
  */
/* Private macros ------------------------------------------------------------*/
/** @addtogroup RCCEx_Private_Macros RCCEx Private Macros
  * @{
  */
/** @defgroup RCCEx_IS_RCC_Definitions RCC Private macros to check input parameters
  * @{
  */
#if defined(STM32F756xx) || defined(STM32F746xx)
#define IS_RCC_PERIPHCLOCK(SELECTION)  \
               ((((SELECTION) & RCC_PERIPHCLK_I2S)         == RCC_PERIPHCLK_I2S)     || \
                (((SELECTION) & RCC_PERIPHCLK_LTDC)        == RCC_PERIPHCLK_LTDC)    || \
                (((SELECTION) & RCC_PERIPHCLK_TIM)         == RCC_PERIPHCLK_TIM)     || \
                (((SELECTION) & RCC_PERIPHCLK_USART1)      == RCC_PERIPHCLK_USART1)  || \
                (((SELECTION) & RCC_PERIPHCLK_USART2)      == RCC_PERIPHCLK_USART2)  || \
                (((SELECTION) & RCC_PERIPHCLK_USART3)      == RCC_PERIPHCLK_USART3)  || \
                (((SELECTION) & RCC_PERIPHCLK_UART4)       == RCC_PERIPHCLK_UART4)   || \
                (((SELECTION) & RCC_PERIPHCLK_UART5)       == RCC_PERIPHCLK_UART5)   || \
                (((SELECTION) & RCC_PERIPHCLK_USART6)      == RCC_PERIPHCLK_USART6)  || \
                (((SELECTION) & RCC_PERIPHCLK_UART7)       == RCC_PERIPHCLK_UART7)   || \
                (((SELECTION) & RCC_PERIPHCLK_UART8)       == RCC_PERIPHCLK_UART8)   || \
                (((SELECTION) & RCC_PERIPHCLK_I2C1)        == RCC_PERIPHCLK_I2C1)    || \
                (((SELECTION) & RCC_PERIPHCLK_I2C2)        == RCC_PERIPHCLK_I2C2)    || \
                (((SELECTION) & RCC_PERIPHCLK_I2C3)        == RCC_PERIPHCLK_I2C3)    || \
                (((SELECTION) & RCC_PERIPHCLK_I2C4)        == RCC_PERIPHCLK_I2C4)    || \
                (((SELECTION) & RCC_PERIPHCLK_LPTIM1)      == RCC_PERIPHCLK_LPTIM1)  || \
                (((SELECTION) & RCC_PERIPHCLK_SAI1)        == RCC_PERIPHCLK_SAI1)    || \
                (((SELECTION) & RCC_PERIPHCLK_SAI2)        == RCC_PERIPHCLK_SAI2)    || \
                (((SELECTION) & RCC_PERIPHCLK_CLK48)       == RCC_PERIPHCLK_CLK48)   || \
                (((SELECTION) & RCC_PERIPHCLK_CEC)         == RCC_PERIPHCLK_CEC)     || \
                (((SELECTION) & RCC_PERIPHCLK_SDMMC1)      == RCC_PERIPHCLK_SDMMC1)  || \
                (((SELECTION) & RCC_PERIPHCLK_SPDIFRX)     == RCC_PERIPHCLK_SPDIFRX) || \
                (((SELECTION) & RCC_PERIPHCLK_RTC)         == RCC_PERIPHCLK_RTC))
#elif defined(STM32F745xx)
#define IS_RCC_PERIPHCLOCK(SELECTION)  \
               ((((SELECTION) & RCC_PERIPHCLK_I2S)         == RCC_PERIPHCLK_I2S)     || \
                (((SELECTION) & RCC_PERIPHCLK_TIM)         == RCC_PERIPHCLK_TIM)     || \
                (((SELECTION) & RCC_PERIPHCLK_USART1)      == RCC_PERIPHCLK_USART1)  || \
                (((SELECTION) & RCC_PERIPHCLK_USART2)      == RCC_PERIPHCLK_USART2)  || \
                (((SELECTION) & RCC_PERIPHCLK_USART3)      == RCC_PERIPHCLK_USART3)  || \
                (((SELECTION) & RCC_PERIPHCLK_UART4)       == RCC_PERIPHCLK_UART4)   || \
                (((SELECTION) & RCC_PERIPHCLK_UART5)       == RCC_PERIPHCLK_UART5)   || \
                (((SELECTION) & RCC_PERIPHCLK_USART6)      == RCC_PERIPHCLK_USART6)  || \
                (((SELECTION) & RCC_PERIPHCLK_UART7)       == RCC_PERIPHCLK_UART7)   || \
                (((SELECTION) & RCC_PERIPHCLK_UART8)       == RCC_PERIPHCLK_UART8)   || \
                (((SELECTION) & RCC_PERIPHCLK_I2C1)        == RCC_PERIPHCLK_I2C1)    || \
                (((SELECTION) & RCC_PERIPHCLK_I2C2)        == RCC_PERIPHCLK_I2C2)    || \
                (((SELECTION) & RCC_PERIPHCLK_I2C3)        == RCC_PERIPHCLK_I2C3)    || \
                (((SELECTION) & RCC_PERIPHCLK_I2C4)        == RCC_PERIPHCLK_I2C4)    || \
                (((SELECTION) & RCC_PERIPHCLK_LPTIM1)      == RCC_PERIPHCLK_LPTIM1)  || \
                (((SELECTION) & RCC_PERIPHCLK_SAI1)        == RCC_PERIPHCLK_SAI1)    || \
                (((SELECTION) & RCC_PERIPHCLK_SAI2)        == RCC_PERIPHCLK_SAI2)    || \
                (((SELECTION) & RCC_PERIPHCLK_CLK48)       == RCC_PERIPHCLK_CLK48)   || \
                (((SELECTION) & RCC_PERIPHCLK_CEC)         == RCC_PERIPHCLK_CEC)     || \
                (((SELECTION) & RCC_PERIPHCLK_SDMMC1)      == RCC_PERIPHCLK_SDMMC1)  || \
                (((SELECTION) & RCC_PERIPHCLK_SPDIFRX)     == RCC_PERIPHCLK_SPDIFRX) || \
                (((SELECTION) & RCC_PERIPHCLK_RTC)         == RCC_PERIPHCLK_RTC))
#elif defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define IS_RCC_PERIPHCLOCK(SELECTION)  \
               ((((SELECTION) & RCC_PERIPHCLK_I2S)         == RCC_PERIPHCLK_I2S)     || \
                (((SELECTION) & RCC_PERIPHCLK_LTDC)        == RCC_PERIPHCLK_LTDC)    || \
                (((SELECTION) & RCC_PERIPHCLK_TIM)         == RCC_PERIPHCLK_TIM)     || \
                (((SELECTION) & RCC_PERIPHCLK_USART1)      == RCC_PERIPHCLK_USART1)  || \
                (((SELECTION) & RCC_PERIPHCLK_USART2)      == RCC_PERIPHCLK_USART2)  || \
                (((SELECTION) & RCC_PERIPHCLK_USART3)      == RCC_PERIPHCLK_USART3)  || \
                (((SELECTION) & RCC_PERIPHCLK_UART4)       == RCC_PERIPHCLK_UART4)   || \
                (((SELECTION) & RCC_PERIPHCLK_UART5)       == RCC_PERIPHCLK_UART5)   || \
                (((SELECTION) & RCC_PERIPHCLK_USART6)      == RCC_PERIPHCLK_USART6)  || \
                (((SELECTION) & RCC_PERIPHCLK_UART7)       == RCC_PERIPHCLK_UART7)   || \
                (((SELECTION) & RCC_PERIPHCLK_UART8)       == RCC_PERIPHCLK_UART8)   || \
                (((SELECTION) & RCC_PERIPHCLK_I2C1)        == RCC_PERIPHCLK_I2C1)    || \
                (((SELECTION) & RCC_PERIPHCLK_I2C2)        == RCC_PERIPHCLK_I2C2)    || \
                (((SELECTION) & RCC_PERIPHCLK_I2C3)        == RCC_PERIPHCLK_I2C3)    || \
                (((SELECTION) & RCC_PERIPHCLK_I2C4)        == RCC_PERIPHCLK_I2C4)    || \
                (((SELECTION) & RCC_PERIPHCLK_LPTIM1)      == RCC_PERIPHCLK_LPTIM1)  || \
                (((SELECTION) & RCC_PERIPHCLK_SAI1)        == RCC_PERIPHCLK_SAI1)    || \
                (((SELECTION) & RCC_PERIPHCLK_SAI2)        == RCC_PERIPHCLK_SAI2)    || \
                (((SELECTION) & RCC_PERIPHCLK_CLK48)       == RCC_PERIPHCLK_CLK48)   || \
                (((SELECTION) & RCC_PERIPHCLK_CEC)         == RCC_PERIPHCLK_CEC)     || \
                (((SELECTION) & RCC_PERIPHCLK_SDMMC1)      == RCC_PERIPHCLK_SDMMC1)  || \
                (((SELECTION) & RCC_PERIPHCLK_SDMMC2)      == RCC_PERIPHCLK_SDMMC2)  || \
                (((SELECTION) & RCC_PERIPHCLK_DFSDM1)       == RCC_PERIPHCLK_DFSDM1)   || \
                (((SELECTION) & RCC_PERIPHCLK_DFSDM1_AUDIO) == RCC_PERIPHCLK_DFSDM1_AUDIO) || \
                (((SELECTION) & RCC_PERIPHCLK_SPDIFRX)     == RCC_PERIPHCLK_SPDIFRX) || \
                (((SELECTION) & RCC_PERIPHCLK_RTC)         == RCC_PERIPHCLK_RTC))
#elif defined (STM32F765xx)
#define IS_RCC_PERIPHCLOCK(SELECTION)  \
               ((((SELECTION) & RCC_PERIPHCLK_I2S)         == RCC_PERIPHCLK_I2S)     || \
                (((SELECTION) & RCC_PERIPHCLK_TIM)         == RCC_PERIPHCLK_TIM)     || \
                (((SELECTION) & RCC_PERIPHCLK_USART1)      == RCC_PERIPHCLK_USART1)  || \
                (((SELECTION) & RCC_PERIPHCLK_USART2)      == RCC_PERIPHCLK_USART2)  || \
                (((SELECTION) & RCC_PERIPHCLK_USART3)      == RCC_PERIPHCLK_USART3)  || \
                (((SELECTION) & RCC_PERIPHCLK_UART4)       == RCC_PERIPHCLK_UART4)   || \
                (((SELECTION) & RCC_PERIPHCLK_UART5)       == RCC_PERIPHCLK_UART5)   || \
                (((SELECTION) & RCC_PERIPHCLK_USART6)      == RCC_PERIPHCLK_USART6)  || \
                (((SELECTION) & RCC_PERIPHCLK_UART7)       == RCC_PERIPHCLK_UART7)   || \
                (((SELECTION) & RCC_PERIPHCLK_UART8)       == RCC_PERIPHCLK_UART8)   || \
                (((SELECTION) & RCC_PERIPHCLK_I2C1)        == RCC_PERIPHCLK_I2C1)    || \
                (((SELECTION) & RCC_PERIPHCLK_I2C2)        == RCC_PERIPHCLK_I2C2)    || \
                (((SELECTION) & RCC_PERIPHCLK_I2C3)        == RCC_PERIPHCLK_I2C3)    || \
                (((SELECTION) & RCC_PERIPHCLK_I2C4)        == RCC_PERIPHCLK_I2C4)    || \
                (((SELECTION) & RCC_PERIPHCLK_LPTIM1)      == RCC_PERIPHCLK_LPTIM1)  || \
                (((SELECTION) & RCC_PERIPHCLK_SAI1)        == RCC_PERIPHCLK_SAI1)    || \
                (((SELECTION) & RCC_PERIPHCLK_SAI2)        == RCC_PERIPHCLK_SAI2)    || \
                (((SELECTION) & RCC_PERIPHCLK_CLK48)       == RCC_PERIPHCLK_CLK48)   || \
                (((SELECTION) & RCC_PERIPHCLK_CEC)         == RCC_PERIPHCLK_CEC)     || \
                (((SELECTION) & RCC_PERIPHCLK_SDMMC1)      == RCC_PERIPHCLK_SDMMC1)  || \
                (((SELECTION) & RCC_PERIPHCLK_SDMMC2)      == RCC_PERIPHCLK_SDMMC2)  || \
                (((SELECTION) & RCC_PERIPHCLK_DFSDM1)       == RCC_PERIPHCLK_DFSDM1)   || \
                (((SELECTION) & RCC_PERIPHCLK_DFSDM1_AUDIO) == RCC_PERIPHCLK_DFSDM1_AUDIO) || \
                (((SELECTION) & RCC_PERIPHCLK_SPDIFRX)     == RCC_PERIPHCLK_SPDIFRX) || \
                (((SELECTION) & RCC_PERIPHCLK_RTC)         == RCC_PERIPHCLK_RTC))
#endif /* STM32F746xx || STM32F756xx */
#define IS_RCC_PLLI2SN_VALUE(VALUE) ((50 <= (VALUE)) && ((VALUE) <= 432))
#define IS_RCC_PLLI2SP_VALUE(VALUE) (((VALUE) == RCC_PLLI2SP_DIV2) ||\
                                     ((VALUE) == RCC_PLLI2SP_DIV4) ||\
                                     ((VALUE) == RCC_PLLI2SP_DIV6) ||\
                                     ((VALUE) == RCC_PLLI2SP_DIV8))
#define IS_RCC_PLLI2SQ_VALUE(VALUE) ((2 <= (VALUE)) && ((VALUE) <= 15))
#define IS_RCC_PLLI2SR_VALUE(VALUE) ((2 <= (VALUE)) && ((VALUE) <= 7))

#define IS_RCC_PLLSAIN_VALUE(VALUE) ((50 <= (VALUE)) && ((VALUE) <= 432))
#define IS_RCC_PLLSAIP_VALUE(VALUE) (((VALUE) == RCC_PLLSAIP_DIV2) ||\
                                     ((VALUE) == RCC_PLLSAIP_DIV4) ||\
                                     ((VALUE) == RCC_PLLSAIP_DIV6) ||\
                                     ((VALUE) == RCC_PLLSAIP_DIV8))
#define IS_RCC_PLLSAIQ_VALUE(VALUE) ((2 <= (VALUE)) && ((VALUE) <= 15))
#define IS_RCC_PLLSAIR_VALUE(VALUE) ((2 <= (VALUE)) && ((VALUE) <= 7))

#define IS_RCC_PLLSAI_DIVQ_VALUE(VALUE) ((1 <= (VALUE)) && ((VALUE) <= 32))

#define IS_RCC_PLLI2S_DIVQ_VALUE(VALUE) ((1 <= (VALUE)) && ((VALUE) <= 32))

#define IS_RCC_PLLSAI_DIVR_VALUE(VALUE) (((VALUE) == RCC_PLLSAIDIVR_2) ||\
                                         ((VALUE) == RCC_PLLSAIDIVR_4) ||\
                                         ((VALUE) == RCC_PLLSAIDIVR_8) ||\
                                         ((VALUE) == RCC_PLLSAIDIVR_16))
#define IS_RCC_I2SCLKSOURCE(SOURCE)  (((SOURCE) == RCC_I2SCLKSOURCE_PLLI2S) || \
                                      ((SOURCE) == RCC_I2SCLKSOURCE_EXT))

#define IS_RCC_SDMMC1CLKSOURCE(SOURCE) (((SOURCE) == RCC_SDMMC1CLKSOURCE_SYSCLK) || \
                                        ((SOURCE) == RCC_SDMMC1CLKSOURCE_CLK48))

#define IS_RCC_CECCLKSOURCE(SOURCE)  (((SOURCE) == RCC_CECCLKSOURCE_HSI) || \
                                      ((SOURCE) == RCC_CECCLKSOURCE_LSE))
#define IS_RCC_USART1CLKSOURCE(SOURCE)  \
               (((SOURCE) == RCC_USART1CLKSOURCE_PCLK2)  || \
                ((SOURCE) == RCC_USART1CLKSOURCE_SYSCLK) || \
                ((SOURCE) == RCC_USART1CLKSOURCE_LSE)    || \
                ((SOURCE) == RCC_USART1CLKSOURCE_HSI))

#define IS_RCC_USART2CLKSOURCE(SOURCE)  \
               (((SOURCE) == RCC_USART2CLKSOURCE_PCLK1)  || \
                ((SOURCE) == RCC_USART2CLKSOURCE_SYSCLK) || \
                ((SOURCE) == RCC_USART2CLKSOURCE_LSE)    || \
                ((SOURCE) == RCC_USART2CLKSOURCE_HSI))
#define IS_RCC_USART3CLKSOURCE(SOURCE)  \
               (((SOURCE) == RCC_USART3CLKSOURCE_PCLK1)  || \
                ((SOURCE) == RCC_USART3CLKSOURCE_SYSCLK) || \
                ((SOURCE) == RCC_USART3CLKSOURCE_LSE)    || \
                ((SOURCE) == RCC_USART3CLKSOURCE_HSI))

#define IS_RCC_UART4CLKSOURCE(SOURCE)  \
               (((SOURCE) == RCC_UART4CLKSOURCE_PCLK1)  || \
                ((SOURCE) == RCC_UART4CLKSOURCE_SYSCLK) || \
                ((SOURCE) == RCC_UART4CLKSOURCE_LSE)    || \
                ((SOURCE) == RCC_UART4CLKSOURCE_HSI))

#define IS_RCC_UART5CLKSOURCE(SOURCE)  \
               (((SOURCE) == RCC_UART5CLKSOURCE_PCLK1)  || \
                ((SOURCE) == RCC_UART5CLKSOURCE_SYSCLK) || \
                ((SOURCE) == RCC_UART5CLKSOURCE_LSE)    || \
                ((SOURCE) == RCC_UART5CLKSOURCE_HSI))

#define IS_RCC_USART6CLKSOURCE(SOURCE)  \
               (((SOURCE) == RCC_USART6CLKSOURCE_PCLK2)  || \
                ((SOURCE) == RCC_USART6CLKSOURCE_SYSCLK) || \
                ((SOURCE) == RCC_USART6CLKSOURCE_LSE)    || \
                ((SOURCE) == RCC_USART6CLKSOURCE_HSI))

#define IS_RCC_UART7CLKSOURCE(SOURCE)  \
               (((SOURCE) == RCC_UART7CLKSOURCE_PCLK1)  || \
                ((SOURCE) == RCC_UART7CLKSOURCE_SYSCLK) || \
                ((SOURCE) == RCC_UART7CLKSOURCE_LSE)    || \
                ((SOURCE) == RCC_UART7CLKSOURCE_HSI))

#define IS_RCC_UART8CLKSOURCE(SOURCE)  \
               (((SOURCE) == RCC_UART8CLKSOURCE_PCLK1)  || \
                ((SOURCE) == RCC_UART8CLKSOURCE_SYSCLK) || \
                ((SOURCE) == RCC_UART8CLKSOURCE_LSE)    || \
                ((SOURCE) == RCC_UART8CLKSOURCE_HSI))
#define IS_RCC_I2C1CLKSOURCE(SOURCE)   \
               (((SOURCE) == RCC_I2C1CLKSOURCE_PCLK1) || \
                ((SOURCE) == RCC_I2C1CLKSOURCE_SYSCLK)|| \
                ((SOURCE) == RCC_I2C1CLKSOURCE_HSI))
#define IS_RCC_I2C2CLKSOURCE(SOURCE)   \
               (((SOURCE) == RCC_I2C2CLKSOURCE_PCLK1) || \
                ((SOURCE) == RCC_I2C2CLKSOURCE_SYSCLK)|| \
                ((SOURCE) == RCC_I2C2CLKSOURCE_HSI))

#define IS_RCC_I2C3CLKSOURCE(SOURCE)   \
               (((SOURCE) == RCC_I2C3CLKSOURCE_PCLK1) || \
                ((SOURCE) == RCC_I2C3CLKSOURCE_SYSCLK)|| \
                ((SOURCE) == RCC_I2C3CLKSOURCE_HSI))
#define IS_RCC_I2C4CLKSOURCE(SOURCE)   \
               (((SOURCE) == RCC_I2C4CLKSOURCE_PCLK1) || \
                ((SOURCE) == RCC_I2C4CLKSOURCE_SYSCLK)|| \
                ((SOURCE) == RCC_I2C4CLKSOURCE_HSI))
#define IS_RCC_LPTIM1CLK(SOURCE)  \
               (((SOURCE) == RCC_LPTIM1CLKSOURCE_PCLK) || \
                ((SOURCE) == RCC_LPTIM1CLKSOURCE_LSI)  || \
                ((SOURCE) == RCC_LPTIM1CLKSOURCE_HSI)  || \
                ((SOURCE) == RCC_LPTIM1CLKSOURCE_LSE))
#define IS_RCC_CLK48SOURCE(SOURCE)  \
               (((SOURCE) == RCC_CLK48SOURCE_PLLSAIP) || \
                ((SOURCE) == RCC_CLK48SOURCE_PLL))
#define IS_RCC_TIMPRES(VALUE)  \
               (((VALUE) == RCC_TIMPRES_DESACTIVATED) || \
                ((VALUE) == RCC_TIMPRES_ACTIVATED))

#if defined (STM32F745xx) || defined (STM32F746xx) || defined (STM32F756xx)
#define IS_RCC_SAI1CLKSOURCE(SOURCE)  (((SOURCE) == RCC_SAI1CLKSOURCE_PLLSAI) || \
                                       ((SOURCE) == RCC_SAI1CLKSOURCE_PLLI2S) || \
                                       ((SOURCE) == RCC_SAI1CLKSOURCE_PIN))
#define IS_RCC_SAI2CLKSOURCE(SOURCE)  (((SOURCE) == RCC_SAI2CLKSOURCE_PLLSAI) || \
                                       ((SOURCE) == RCC_SAI2CLKSOURCE_PLLI2S) || \
                                       ((SOURCE) == RCC_SAI2CLKSOURCE_PIN))
#endif /* STM32F745xx || STM32F746xx || STM32F756xx */

#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define IS_RCC_PLLR_VALUE(VALUE)            ((2 <= (VALUE)) && ((VALUE) <= 7))

#define IS_RCC_SAI1CLKSOURCE(SOURCE)  (((SOURCE) == RCC_SAI1CLKSOURCE_PLLSAI) || \
                                       ((SOURCE) == RCC_SAI1CLKSOURCE_PLLI2S) || \
                                       ((SOURCE) == RCC_SAI1CLKSOURCE_PIN)    || \
                                       ((SOURCE) == RCC_SAI1CLKSOURCE_PLLSRC))

#define IS_RCC_SAI2CLKSOURCE(SOURCE)  (((SOURCE) == RCC_SAI2CLKSOURCE_PLLSAI) || \
                                       ((SOURCE) == RCC_SAI2CLKSOURCE_PLLI2S) || \
                                       ((SOURCE) == RCC_SAI2CLKSOURCE_PIN)    || \
                                       ((SOURCE) == RCC_SAI2CLKSOURCE_PLLSRC))

#define IS_RCC_SDMMC2CLKSOURCE(SOURCE)  (((SOURCE) == RCC_SDMMC2CLKSOURCE_SYSCLK) || \
                                         ((SOURCE) == RCC_SDMMC2CLKSOURCE_CLK48))

#define IS_RCC_DFSDM1CLKSOURCE(SOURCE)  (((SOURCE) == RCC_DFSDM1CLKSOURCE_PCLK) || \
                                        ((SOURCE) == RCC_DFSDM1CLKSOURCE_SYSCLK))

#define IS_RCC_DFSDM1AUDIOCLKSOURCE(SOURCE)  (((SOURCE) == RCC_DFSDM1AUDIOCLKSOURCE_SAI1) || \
                                             ((SOURCE) == RCC_DFSDM1AUDIOCLKSOURCE_SAI2))
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */

#if defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define IS_RCC_DSIBYTELANECLKSOURCE(SOURCE) (((SOURCE) == RCC_DSICLKSOURCE_PLLR)  ||\
                                             ((SOURCE) == RCC_DSICLKSOURCE_DSIPHY))
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */

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

#endif /* __STM32F7xx_HAL_RCC_EX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
