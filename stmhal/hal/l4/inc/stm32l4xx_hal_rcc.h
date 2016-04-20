/**
  ******************************************************************************
  * @file    stm32l4xx_hal_rcc.h
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   Header file of RCC HAL module.
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
#ifndef __STM32L4xx_HAL_RCC_H
#define __STM32L4xx_HAL_RCC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal_def.h"

/** @addtogroup STM32L4xx_HAL_Driver
  * @{
  */

/** @addtogroup RCC
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup RCC_Exported_Types RCC Exported Types
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
                            This parameter must be a number between Min_Data = 0 and Max_Data = 8     */

  uint32_t PLLN;       /*!< PLLN: Multiplication factor for PLL VCO output clock.
                            This parameter must be a number between Min_Data = 8 and Max_Data = 86    */

  uint32_t PLLP;       /*!< PLLP: Division factor for SAI clock.
                            This parameter must be a value of @ref RCC_PLLP_Clock_Divider             */

  uint32_t PLLQ;       /*!< PLLQ: Division factor for SDMMC1, RNG and USB clocks.
                            This parameter must be a value of @ref RCC_PLLQ_Clock_Divider             */

  uint32_t PLLR;       /*!< PLLR: Division for the main system clock.
                            User have to set the PLLR parameter correctly to not exceed max frequency 80MHZ.
                            This parameter must be a value of @ref RCC_PLLR_Clock_Divider             */

}RCC_PLLInitTypeDef;

/**
  * @brief  RCC Internal/External Oscillator (HSE, HSI, MSI, LSE and LSI) configuration structure definition
  */
typedef struct
{
  uint32_t OscillatorType;       /*!< The oscillators to be configured.
                                      This parameter can be a value of @ref RCC_Oscillator_Type                   */

  uint32_t HSEState;             /*!< The new state of the HSE.
                                      This parameter can be a value of @ref RCC_HSE_Config                        */

  uint32_t LSEState;             /*!< The new state of the LSE.
                                      This parameter can be a value of @ref RCC_LSE_Config                        */

  uint32_t HSIState;             /*!< The new state of the HSI.
                                      This parameter can be a value of @ref RCC_HSI_Config                        */

  uint32_t HSICalibrationValue;  /*!< The calibration trimming value (default is RCC_HSICALIBRATION_DEFAULT).
                                      This parameter must be a number between Min_Data = 0x00 and Max_Data = 0x1F */

  uint32_t LSIState;             /*!< The new state of the LSI.
                                      This parameter can be a value of @ref RCC_LSI_Config                        */

  uint32_t MSIState;             /*!< The new state of the MSI.
                                      This parameter can be a value of @ref RCC_MSI_Config */

  uint32_t MSICalibrationValue;  /*!< The calibration trimming value (default is RCC_MSICALIBRATION_DEFAULT).
                                      This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF */

  uint32_t MSIClockRange;        /*!< The MSI frequency range.
                                      This parameter can be a value of @ref RCC_MSI_Clock_Range  */

  RCC_PLLInitTypeDef PLL;        /*!< Main PLL structure parameters                                               */

}RCC_OscInitTypeDef;

/**
  * @brief  RCC System, AHB and APB busses clock configuration structure definition
  */
typedef struct
{
  uint32_t ClockType;             /*!< The clock to be configured.
                                       This parameter can be a value of @ref RCC_System_Clock_Type      */

  uint32_t SYSCLKSource;          /*!< The clock source used as system clock (SYSCLK).
                                       This parameter can be a value of @ref RCC_System_Clock_Source    */

  uint32_t AHBCLKDivider;         /*!< The AHB clock (HCLK) divider. This clock is derived from the system clock (SYSCLK).
                                       This parameter can be a value of @ref RCC_AHB_Clock_Source       */

  uint32_t APB1CLKDivider;        /*!< The APB1 clock (PCLK1) divider. This clock is derived from the AHB clock (HCLK).
                                       This parameter can be a value of @ref RCC_APB1_APB2_Clock_Source */

  uint32_t APB2CLKDivider;        /*!< The APB2 clock (PCLK2) divider. This clock is derived from the AHB clock (HCLK).
                                       This parameter can be a value of @ref RCC_APB1_APB2_Clock_Source */

}RCC_ClkInitTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup RCC_Exported_Constants RCC Exported Constants
  * @{
  */

/** @defgroup RCC_Timeout_Value Timeout Values
  * @{
  */
#define RCC_DBP_TIMEOUT_VALUE          ((uint32_t)2U)    /* 2 ms (minimum Tick + 1) */
#define RCC_LSE_TIMEOUT_VALUE          LSE_STARTUP_TIMEOUT
/**
  * @}
  */

/** @defgroup RCC_Oscillator_Type Oscillator Type
  * @{
  */
#define RCC_OSCILLATORTYPE_NONE        ((uint32_t)0x00000000U)   /*!< Oscillator configuration unchanged */
#define RCC_OSCILLATORTYPE_HSE         ((uint32_t)0x00000001U)   /*!< HSE to configure */
#define RCC_OSCILLATORTYPE_HSI         ((uint32_t)0x00000002U)   /*!< HSI to configure */
#define RCC_OSCILLATORTYPE_LSE         ((uint32_t)0x00000004U)   /*!< LSE to configure */
#define RCC_OSCILLATORTYPE_LSI         ((uint32_t)0x00000008U)   /*!< LSI to configure */
#define RCC_OSCILLATORTYPE_MSI         ((uint32_t)0x00000010U)   /*!< MSI to configure */
/**
  * @}
  */

/** @defgroup RCC_HSE_Config HSE Config
  * @{
  */
#define RCC_HSE_OFF                    ((uint32_t)0x00000000U)                     /*!< HSE clock deactivation */
#define RCC_HSE_ON                     RCC_CR_HSEON                               /*!< HSE clock activation */
#define RCC_HSE_BYPASS                 ((uint32_t)(RCC_CR_HSEBYP | RCC_CR_HSEON)) /*!< External clock source for HSE clock */
/**
  * @}
  */

/** @defgroup RCC_LSE_Config LSE Config
  * @{
  */
#define RCC_LSE_OFF                    ((uint32_t)0x00000000U)                         /*!< LSE clock deactivation */
#define RCC_LSE_ON                     RCC_BDCR_LSEON                                 /*!< LSE clock activation */
#define RCC_LSE_BYPASS                 ((uint32_t)(RCC_BDCR_LSEBYP | RCC_BDCR_LSEON)) /*!< External clock source for LSE clock */
/**
  * @}
  */

/** @defgroup RCC_HSI_Config HSI Config
  * @{
  */
#define RCC_HSI_OFF                    ((uint32_t)0x00000000U) /*!< HSI clock deactivation */
#define RCC_HSI_ON                     RCC_CR_HSION           /*!< HSI clock activation */

#define RCC_HSICALIBRATION_DEFAULT     ((uint32_t)16)   /*!< Default HSI calibration trimming value */
/**
  * @}
  */

/** @defgroup RCC_LSI_Config LSI Config
  * @{
  */
#define RCC_LSI_OFF                    ((uint32_t)0x00000000U) /*!< LSI clock deactivation */
#define RCC_LSI_ON                     RCC_CSR_LSION          /*!< LSI clock activation */
/**
  * @}
  */

/** @defgroup RCC_MSI_Config MSI Config
  * @{
  */
#define RCC_MSI_OFF                    ((uint32_t)0x00000000U) /*!< MSI clock deactivation */
#define RCC_MSI_ON                     RCC_CR_MSION           /*!< MSI clock activation */

#define RCC_MSICALIBRATION_DEFAULT     ((uint32_t)0)   /*!< Default MSI calibration trimming value */
/**
  * @}
  */

/** @defgroup RCC_PLL_Config PLL Config
  * @{
  */
#define RCC_PLL_NONE                   ((uint32_t)0x00000000U) /*!< PLL configuration unchanged */
#define RCC_PLL_OFF                    ((uint32_t)0x00000001U) /*!< PLL deactivation */
#define RCC_PLL_ON                     ((uint32_t)0x00000002U) /*!< PLL activation */
/**
  * @}
  */

/** @defgroup RCC_PLLP_Clock_Divider PLLP Clock Divider
  * @{
  */
#define RCC_PLLP_DIV7                  ((uint32_t)0x00000007U) /*!< PLLP division factor = 7  */
#define RCC_PLLP_DIV17                 ((uint32_t)0x00000011U) /*!< PLLP division factor = 17 */
/**
  * @}
  */

/** @defgroup RCC_PLLQ_Clock_Divider PLLQ Clock Divider
  * @{
  */
#define RCC_PLLQ_DIV2                  ((uint32_t)0x00000002U) /*!< PLLQ division factor = 2 */
#define RCC_PLLQ_DIV4                  ((uint32_t)0x00000004U) /*!< PLLQ division factor = 4 */
#define RCC_PLLQ_DIV6                  ((uint32_t)0x00000006U) /*!< PLLQ division factor = 6 */
#define RCC_PLLQ_DIV8                  ((uint32_t)0x00000008U) /*!< PLLQ division factor = 8 */
/**
  * @}
  */

/** @defgroup RCC_PLLR_Clock_Divider PLLR Clock Divider
  * @{
  */
#define RCC_PLLR_DIV2                  ((uint32_t)0x00000002U) /*!< PLLR division factor = 2 */
#define RCC_PLLR_DIV4                  ((uint32_t)0x00000004U) /*!< PLLR division factor = 4 */
#define RCC_PLLR_DIV6                  ((uint32_t)0x00000006U) /*!< PLLR division factor = 6 */
#define RCC_PLLR_DIV8                  ((uint32_t)0x00000008U) /*!< PLLR division factor = 8 */
/**
  * @}
  */

/** @defgroup RCC_PLL_Clock_Source PLL Clock Source
  * @{
  */
#define RCC_PLLSOURCE_NONE             ((uint32_t)0x00000000U)  /*!< No clock selected as PLL entry clock source  */
#define RCC_PLLSOURCE_MSI              RCC_PLLCFGR_PLLSRC_MSI  /*!< MSI clock selected as PLL entry clock source */
#define RCC_PLLSOURCE_HSI              RCC_PLLCFGR_PLLSRC_HSI  /*!< HSI clock selected as PLL entry clock source */
#define RCC_PLLSOURCE_HSE              RCC_PLLCFGR_PLLSRC_HSE  /*!< HSE clock selected as PLL entry clock source */
/**
  * @}
  */

/** @defgroup RCC_PLL_Clock_Output PLL Clock Output
  * @{
  */
#define RCC_PLL_SAI3CLK                RCC_PLLCFGR_PLLPEN      /*!< PLLSAI3CLK selection from main PLL */
#define RCC_PLL_48M1CLK                RCC_PLLCFGR_PLLQEN      /*!< PLL48M1CLK selection from main PLL */
#define RCC_PLL_SYSCLK                 RCC_PLLCFGR_PLLREN      /*!< PLLCLK selection from main PLL */
/**
  * @}
  */

/** @defgroup RCC_PLLSAI1_Clock_Output PLLSAI1 Clock Output
  * @{
  */
#define RCC_PLLSAI1_SAI1CLK            RCC_PLLSAI1CFGR_PLLSAI1PEN /*!< PLLSAI1CLK selection from PLLSAI1 */
#define RCC_PLLSAI1_48M2CLK            RCC_PLLSAI1CFGR_PLLSAI1QEN /*!< PLL48M2CLK selection from PLLSAI1 */
#define RCC_PLLSAI1_ADC1CLK            RCC_PLLSAI1CFGR_PLLSAI1REN /*!< PLLADC1CLK selection from PLLSAI1 */
/**
  * @}
  */

/** @defgroup RCC_PLLSAI2_Clock_Output PLLSAI2 Clock Output
  * @{
  */
#define RCC_PLLSAI2_SAI2CLK            RCC_PLLSAI2CFGR_PLLSAI2PEN /*!< PLLSAI2CLK selection from PLLSAI2 */
#define RCC_PLLSAI2_ADC2CLK            RCC_PLLSAI2CFGR_PLLSAI2REN /*!< PLLADC2CLK selection from PLLSAI2 */
/**
  * @}
  */

/** @defgroup RCC_MSI_Clock_Range MSI Clock Range
  * @{
  */
#define RCC_MSIRANGE_0                 RCC_CR_MSIRANGE_0  /*!< MSI = 100 KHz  */
#define RCC_MSIRANGE_1                 RCC_CR_MSIRANGE_1  /*!< MSI = 200 KHz  */
#define RCC_MSIRANGE_2                 RCC_CR_MSIRANGE_2  /*!< MSI = 400 KHz  */
#define RCC_MSIRANGE_3                 RCC_CR_MSIRANGE_3  /*!< MSI = 800 KHz  */
#define RCC_MSIRANGE_4                 RCC_CR_MSIRANGE_4  /*!< MSI = 1 MHz    */
#define RCC_MSIRANGE_5                 RCC_CR_MSIRANGE_5  /*!< MSI = 2 MHz    */
#define RCC_MSIRANGE_6                 RCC_CR_MSIRANGE_6  /*!< MSI = 4 MHz    */
#define RCC_MSIRANGE_7                 RCC_CR_MSIRANGE_7  /*!< MSI = 8 MHz    */
#define RCC_MSIRANGE_8                 RCC_CR_MSIRANGE_8  /*!< MSI = 16 MHz   */
#define RCC_MSIRANGE_9                 RCC_CR_MSIRANGE_9  /*!< MSI = 24 MHz   */
#define RCC_MSIRANGE_10                RCC_CR_MSIRANGE_10 /*!< MSI = 32 MHz   */
#define RCC_MSIRANGE_11                RCC_CR_MSIRANGE_11 /*!< MSI = 48 MHz   */
/**
  * @}
  */

/** @defgroup RCC_System_Clock_Type System Clock Type
  * @{
  */
#define RCC_CLOCKTYPE_SYSCLK           ((uint32_t)0x00000001U)  /*!< SYSCLK to configure */
#define RCC_CLOCKTYPE_HCLK             ((uint32_t)0x00000002U)  /*!< HCLK to configure */
#define RCC_CLOCKTYPE_PCLK1            ((uint32_t)0x00000004U)  /*!< PCLK1 to configure */
#define RCC_CLOCKTYPE_PCLK2            ((uint32_t)0x00000008U)  /*!< PCLK2 to configure */
/**
  * @}
  */

/** @defgroup RCC_System_Clock_Source System Clock Source
  * @{
  */
#define RCC_SYSCLKSOURCE_MSI           RCC_CFGR_SW_MSI    /*!< MSI selection as system clock */
#define RCC_SYSCLKSOURCE_HSI           RCC_CFGR_SW_HSI    /*!< HSI selection as system clock */
#define RCC_SYSCLKSOURCE_HSE           RCC_CFGR_SW_HSE    /*!< HSE selection as system clock */
#define RCC_SYSCLKSOURCE_PLLCLK        RCC_CFGR_SW_PLL    /*!< PLL selection as system clock */
/**
  * @}
  */

/** @defgroup RCC_System_Clock_Source_Status System Clock Source Status
  * @{
  */
#define RCC_SYSCLKSOURCE_STATUS_MSI    RCC_CFGR_SWS_MSI   /*!< MSI used as system clock */
#define RCC_SYSCLKSOURCE_STATUS_HSI    RCC_CFGR_SWS_HSI   /*!< HSI used as system clock */
#define RCC_SYSCLKSOURCE_STATUS_HSE    RCC_CFGR_SWS_HSE   /*!< HSE used as system clock */
#define RCC_SYSCLKSOURCE_STATUS_PLLCLK RCC_CFGR_SWS_PLL   /*!< PLL used as system clock */
/**
  * @}
  */

/** @defgroup RCC_AHB_Clock_Source AHB Clock Source
  * @{
  */
#define RCC_SYSCLK_DIV1                RCC_CFGR_HPRE_DIV1   /*!< SYSCLK not divided */
#define RCC_SYSCLK_DIV2                RCC_CFGR_HPRE_DIV2   /*!< SYSCLK divided by 2 */
#define RCC_SYSCLK_DIV4                RCC_CFGR_HPRE_DIV4   /*!< SYSCLK divided by 4 */
#define RCC_SYSCLK_DIV8                RCC_CFGR_HPRE_DIV8   /*!< SYSCLK divided by 8 */
#define RCC_SYSCLK_DIV16               RCC_CFGR_HPRE_DIV16  /*!< SYSCLK divided by 16 */
#define RCC_SYSCLK_DIV64               RCC_CFGR_HPRE_DIV64  /*!< SYSCLK divided by 64 */
#define RCC_SYSCLK_DIV128              RCC_CFGR_HPRE_DIV128 /*!< SYSCLK divided by 128 */
#define RCC_SYSCLK_DIV256              RCC_CFGR_HPRE_DIV256 /*!< SYSCLK divided by 256 */
#define RCC_SYSCLK_DIV512              RCC_CFGR_HPRE_DIV512 /*!< SYSCLK divided by 512 */
/**
  * @}
  */

/** @defgroup RCC_APB1_APB2_Clock_Source APB1 APB2 Clock Source
  * @{
  */
#define RCC_HCLK_DIV1                  RCC_CFGR_PPRE1_DIV1  /*!< HCLK not divided */
#define RCC_HCLK_DIV2                  RCC_CFGR_PPRE1_DIV2  /*!< HCLK divided by 2 */
#define RCC_HCLK_DIV4                  RCC_CFGR_PPRE1_DIV4  /*!< HCLK divided by 4 */
#define RCC_HCLK_DIV8                  RCC_CFGR_PPRE1_DIV8  /*!< HCLK divided by 8 */
#define RCC_HCLK_DIV16                 RCC_CFGR_PPRE1_DIV16 /*!< HCLK divided by 16 */
/**
  * @}
  */

/** @defgroup RCC_RTC_Clock_Source RTC Clock Source
  * @{
  */
#define RCC_RTCCLKSOURCE_LSE           RCC_BDCR_RTCSEL_0  /*!< LSE oscillator clock used as RTC clock */
#define RCC_RTCCLKSOURCE_LSI           RCC_BDCR_RTCSEL_1  /*!< LSI oscillator clock used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV32     RCC_BDCR_RTCSEL    /*!< HSE oscillator clock divided by 32 used as RTC clock */
/**
  * @}
  */

/** @defgroup RCC_MCO_Index MCO Index
  * @{
  */
#define RCC_MCO1                       ((uint32_t)0x00000000U)
#define RCC_MCO                        RCC_MCO1               /*!< MCO1 to be compliant with other families with 2 MCOs*/
/**
  * @}
  */

/** @defgroup RCC_MCO1_Clock_Source MCO1 Clock Source
  * @{
  */
#define RCC_MCO1SOURCE_NOCLOCK         ((uint32_t)0x00000000U)                 /*!< MCO1 output disabled, no clock on MCO1 */
#define RCC_MCO1SOURCE_SYSCLK          RCC_CFGR_MCOSEL_0                      /*!< SYSCLK selection as MCO1 source */
#define RCC_MCO1SOURCE_MSI             RCC_CFGR_MCOSEL_1                      /*!< MSI selection as MCO1 source */
#define RCC_MCO1SOURCE_HSI             (RCC_CFGR_MCOSEL_0| RCC_CFGR_MCOSEL_1) /*!< HSI selection as MCO1 source */
#define RCC_MCO1SOURCE_HSE             RCC_CFGR_MCOSEL_2                      /*!< HSE selection as MCO1 source */
#define RCC_MCO1SOURCE_PLLCLK          (RCC_CFGR_MCOSEL_0|RCC_CFGR_MCOSEL_2)  /*!< PLLCLK selection as MCO1 source */
#define RCC_MCO1SOURCE_LSI             (RCC_CFGR_MCOSEL_1|RCC_CFGR_MCOSEL_2)  /*!< LSI selection as MCO1 source */
#define RCC_MCO1SOURCE_LSE             (RCC_CFGR_MCOSEL_0|RCC_CFGR_MCOSEL_1|RCC_CFGR_MCOSEL_2) /*!< LSE selection as MCO1 source */
/**
  * @}
  */

/** @defgroup RCC_MCOx_Clock_Prescaler MCO1 Clock Prescaler
  * @{
  */
#define RCC_MCODIV_1                   RCC_CFGR_MCOPRE_DIV1     /*!< MCO not divided  */      
#define RCC_MCODIV_2                   RCC_CFGR_MCOPRE_DIV2     /*!< MCO divided by 2 */
#define RCC_MCODIV_4                   RCC_CFGR_MCOPRE_DIV4     /*!< MCO divided by 4 */
#define RCC_MCODIV_8                   RCC_CFGR_MCOPRE_DIV8     /*!< MCO divided by 8 */
#define RCC_MCODIV_16                  RCC_CFGR_MCOPRE_DIV16    /*!< MCO divided by 16 */
/**
  * @}
  */

/** @defgroup RCC_Interrupt Interrupts
  * @{
  */
#define RCC_IT_LSIRDY                  RCC_CIFR_LSIRDYF      /*!< LSI Ready Interrupt flag */
#define RCC_IT_LSERDY                  RCC_CIFR_LSERDYF      /*!< LSE Ready Interrupt flag */
#define RCC_IT_MSIRDY                  RCC_CIFR_MSIRDYF      /*!< MSI Ready Interrupt flag */
#define RCC_IT_HSIRDY                  RCC_CIFR_HSIRDYF      /*!< HSI16 Ready Interrupt flag */
#define RCC_IT_HSERDY                  RCC_CIFR_HSERDYF      /*!< HSE Ready Interrupt flag */
#define RCC_IT_PLLRDY                  RCC_CIFR_PLLRDYF      /*!< PLL Ready Interrupt flag */
#define RCC_IT_PLLSAI1RDY              RCC_CIFR_PLLSAI1RDYF  /*!< PLLSAI1 Ready Interrupt flag */
#define RCC_IT_PLLSAI2RDY              RCC_CIFR_PLLSAI2RDYF  /*!< PLLSAI2 Ready Interrupt flag */
#define RCC_IT_CSS                     RCC_CIFR_CSSF        /*!< Clock Security System Interrupt flag */
#define RCC_IT_LSECSS                  RCC_CIFR_LSECSSF     /*!< LSE Clock Security System Interrupt flag */
/**
  * @}
  */

/** @defgroup RCC_Flag Flags
  *        Elements values convention: XXXYYYYYb
  *           - YYYYY  : Flag position in the register
  *           - XXX  : Register index
  *                 - 001: CR register
  *                 - 010: BDCR register
  *                 - 011: CSR register
  * @{
  */
/* Flags in the CR register */
#define RCC_FLAG_MSIRDY                ((uint32_t)((CR_REG_INDEX << 5U) | POSITION_VAL(RCC_CR_MSIRDY))) /*!< MSI Ready flag */
#define RCC_FLAG_HSIRDY                ((uint32_t)((CR_REG_INDEX << 5U) | POSITION_VAL(RCC_CR_HSIRDY))) /*!< HSI Ready flag */
#define RCC_FLAG_HSERDY                ((uint32_t)((CR_REG_INDEX << 5U) | POSITION_VAL(RCC_CR_HSERDY))) /*!< HSE Ready flag */
#define RCC_FLAG_PLLRDY                ((uint32_t)((CR_REG_INDEX << 5U) | POSITION_VAL(RCC_CR_PLLRDY))) /*!< PLL Ready flag */
#define RCC_FLAG_PLLSAI1RDY            ((uint32_t)((CR_REG_INDEX << 5U) | POSITION_VAL(RCC_CR_PLLSAI1RDY))) /*!< PLLSAI1 Ready flag */
#define RCC_FLAG_PLLSAI2RDY            ((uint32_t)((CR_REG_INDEX << 5U) | POSITION_VAL(RCC_CR_PLLSAI2RDY))) /*!< PLLSAI2 Ready flag */

/* Flags in the BDCR register */
#define RCC_FLAG_LSERDY                ((uint32_t)((BDCR_REG_INDEX << 5U) | POSITION_VAL(RCC_BDCR_LSERDY))) /*!< LSE Ready flag */
#define RCC_FLAG_LSECSSD               ((uint32_t)((BDCR_REG_INDEX << 5U) | POSITION_VAL(RCC_BDCR_LSECSSD))) /*!< LSE Clock Security System Interrupt flag */

/* Flags in the CSR register */
#define RCC_FLAG_LSIRDY                ((uint32_t)((CSR_REG_INDEX << 5U) | POSITION_VAL(RCC_CSR_LSIRDY)))   /*!< LSI Ready flag */
#define RCC_FLAG_RMVF                  ((uint32_t)((CSR_REG_INDEX << 5U) | POSITION_VAL(RCC_CSR_RMVF)))     /*!< Remove reset flag */
#define RCC_FLAG_FWRST                 ((uint32_t)((CSR_REG_INDEX << 5U) | POSITION_VAL(RCC_CSR_FWRSTF)))   /*!< Firewall reset flag */
#define RCC_FLAG_OBLRST                ((uint32_t)((CSR_REG_INDEX << 5U) | POSITION_VAL(RCC_CSR_OBLRSTF)))  /*!< Option Byte Loader reset flag */
#define RCC_FLAG_PINRST                ((uint32_t)((CSR_REG_INDEX << 5U) | POSITION_VAL(RCC_CSR_PINRSTF)))  /*!< PIN reset flag */
#define RCC_FLAG_BORRST                ((uint32_t)((CSR_REG_INDEX << 5U) | POSITION_VAL(RCC_CSR_BORRSTF)))  /*!< BOR reset flag */
#define RCC_FLAG_SFTRST                ((uint32_t)((CSR_REG_INDEX << 5U) | POSITION_VAL(RCC_CSR_SFTRSTF)))  /*!< Software Reset flag */
#define RCC_FLAG_IWDGRST               ((uint32_t)((CSR_REG_INDEX << 5U) | POSITION_VAL(RCC_CSR_IWDGRSTF))) /*!< Independent Watchdog reset flag */
#define RCC_FLAG_WWDGRST               ((uint32_t)((CSR_REG_INDEX << 5U) | POSITION_VAL(RCC_CSR_WWDGRSTF))) /*!< Window watchdog reset flag */
#define RCC_FLAG_LPWRRST               ((uint32_t)((CSR_REG_INDEX << 5U) | POSITION_VAL(RCC_CSR_LPWRRSTF))) /*!< Low-Power reset flag */
/**
  * @}
  */

/** @defgroup RCC_LSEDrive_Config LSE Drive Config
  * @{
  */
#define RCC_LSEDRIVE_LOW                 ((uint32_t)0x00000000U) /*!< LSE low drive capability */
#define RCC_LSEDRIVE_MEDIUMLOW           RCC_BDCR_LSEDRV_0      /*!< LSE medium low drive capability */
#define RCC_LSEDRIVE_MEDIUMHIGH          RCC_BDCR_LSEDRV_1      /*!< LSE medium high drive capability */ 
#define RCC_LSEDRIVE_HIGH                RCC_BDCR_LSEDRV        /*!< LSE high drive capability */
/**
  * @}
  */

/** @defgroup RCC_Stop_WakeUpClock Wake-Up from STOP Clock 
  * @{
  */
#define RCC_STOP_WAKEUPCLOCK_MSI       ((uint32_t)0x00000000U)  /*!< MSI selection after wake-up from STOP */
#define RCC_STOP_WAKEUPCLOCK_HSI       RCC_CFGR_STOPWUCK       /*!< HSI selection after wake-up from STOP */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/

/** @defgroup RCC_Exported_Macros RCC Exported Macros
  * @{
  */

/** @defgroup RCC_AHB1_Peripheral_Clock_Enable_Disable AHB1 Peripheral Clock Enable Disable
  * @brief  Enable or disable the AHB1 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_DMA1_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA1EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA1EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_DMA2_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA2EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA2EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_FLASH_CLK_ENABLE()           do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_FLASHEN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_FLASHEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_CRC_CLK_ENABLE()             do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_CRCEN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_CRCEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_TSC_CLK_ENABLE()             do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_TSCEN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_TSCEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_DMA1_CLK_DISABLE()           CLEAR_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA1EN)

#define __HAL_RCC_DMA2_CLK_DISABLE()           CLEAR_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA2EN)

#define __HAL_RCC_FLASH_CLK_DISABLE()          CLEAR_BIT(RCC->AHB1ENR, RCC_AHB1ENR_FLASHEN)

#define __HAL_RCC_CRC_CLK_DISABLE()            CLEAR_BIT(RCC->AHB1ENR, RCC_AHB1ENR_CRCEN)

#define __HAL_RCC_TSC_CLK_DISABLE()            CLEAR_BIT(RCC->AHB1ENR, RCC_AHB1ENR_TSCEN)

/**
  * @}
  */

/** @defgroup RCC_AHB2_Peripheral_Clock_Enable_Disable AHB2 Peripheral Clock Enable Disable
  * @brief  Enable or disable the AHB2 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_GPIOA_CLK_ENABLE()           do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_GPIOB_CLK_ENABLE()           do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOBEN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOBEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_GPIOC_CLK_ENABLE()           do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOCEN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOCEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_GPIOD_CLK_ENABLE()           do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIODEN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIODEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_GPIOE_CLK_ENABLE()           do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOEEN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOEEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_GPIOF_CLK_ENABLE()           do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOFEN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOFEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_GPIOG_CLK_ENABLE()           do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOGEN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOGEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_GPIOH_CLK_ENABLE()           do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOHEN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOHEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#if defined(USB_OTG_FS)
#define __HAL_RCC_USB_OTG_FS_CLK_ENABLE()      do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_OTGFSEN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_OTGFSEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)
#endif /* USB_OTG_FS */

#define __HAL_RCC_ADC_CLK_ENABLE()             do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_ADCEN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_ADCEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#if defined(AES)
#define __HAL_RCC_AES_CLK_ENABLE()             do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_AESEN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_AESEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)
#endif /* AES */

#define __HAL_RCC_RNG_CLK_ENABLE()             do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_RNGEN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_RNGEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_GPIOA_CLK_DISABLE()          CLEAR_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN)

#define __HAL_RCC_GPIOB_CLK_DISABLE()          CLEAR_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOBEN)

#define __HAL_RCC_GPIOC_CLK_DISABLE()          CLEAR_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOCEN)

#define __HAL_RCC_GPIOD_CLK_DISABLE()          CLEAR_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIODEN)

#define __HAL_RCC_GPIOE_CLK_DISABLE()          CLEAR_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOEEN)

#define __HAL_RCC_GPIOF_CLK_DISABLE()          CLEAR_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOFEN)

#define __HAL_RCC_GPIOG_CLK_DISABLE()          CLEAR_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOGEN)

#define __HAL_RCC_GPIOH_CLK_DISABLE()          CLEAR_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOHEN)

#if defined(USB_OTG_FS)
#define __HAL_RCC_USB_OTG_FS_CLK_DISABLE()     CLEAR_BIT(RCC->AHB2ENR, RCC_AHB2ENR_OTGFSEN);
#endif /* USB_OTG_FS */

#define __HAL_RCC_ADC_CLK_DISABLE()            CLEAR_BIT(RCC->AHB2ENR, RCC_AHB2ENR_ADCEN)

#if defined(AES)
#define __HAL_RCC_AES_CLK_DISABLE()            CLEAR_BIT(RCC->AHB2ENR, RCC_AHB2ENR_AESEN);
#endif /* AES */

#define __HAL_RCC_RNG_CLK_DISABLE()            CLEAR_BIT(RCC->AHB2ENR, RCC_AHB2ENR_RNGEN)

/**
  * @}
  */

/** @defgroup RCC_AHB3_Clock_Enable_Disable AHB3 Peripheral Clock Enable Disable
  * @brief  Enable or disable the AHB3 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_FMC_CLK_ENABLE()             do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_QSPI_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->AHB3ENR, RCC_AHB3ENR_QSPIEN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_QSPIEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_FMC_CLK_DISABLE()            CLEAR_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN)

#define __HAL_RCC_QSPI_CLK_DISABLE()           CLEAR_BIT(RCC->AHB3ENR, RCC_AHB3ENR_QSPIEN)

/**
  * @}
  */

/** @defgroup RCC_APB1_Clock_Enable_Disable APB1 Peripheral Clock Enable Disable
  * @brief  Enable or disable the APB1 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_TIM2_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM2EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM2EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_TIM3_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM3EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM3EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_TIM4_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM4EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM4EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_TIM5_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM5EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM5EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_TIM6_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM6EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM6EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_TIM7_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM7EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM7EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#if defined(LCD)
#define __HAL_RCC_LCD_CLK_ENABLE()             do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_LCDEN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_LCDEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)
#endif /* LCD */

#define __HAL_RCC_WWDG_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_WWDGEN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_WWDGEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_SPI2_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_SPI2EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_SPI2EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_SPI3_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_SPI3EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_SPI3EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_USART2_CLK_ENABLE()          do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_USART2EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_USART2EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_USART3_CLK_ENABLE()          do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_USART3EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_USART3EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_UART4_CLK_ENABLE()           do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_UART4EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_UART4EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_UART5_CLK_ENABLE()           do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_UART5EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_UART5EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_I2C1_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_I2C1EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_I2C1EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_I2C2_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_I2C2EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_I2C2EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_I2C3_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_I2C3EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_I2C3EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_CAN1_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_CAN1EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_CAN1EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_PWR_CLK_ENABLE()             do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_PWREN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_PWREN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_DAC1_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_DAC1EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_DAC1EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_OPAMP_CLK_ENABLE()           do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_OPAMPEN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_OPAMPEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_LPTIM1_CLK_ENABLE()          do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_LPTIM1EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_LPTIM1EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_LPUART1_CLK_ENABLE()         do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB1ENR2, RCC_APB1ENR2_LPUART1EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB1ENR2, RCC_APB1ENR2_LPUART1EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_SWPMI1_CLK_ENABLE()          do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB1ENR2, RCC_APB1ENR2_SWPMI1EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB1ENR2, RCC_APB1ENR2_SWPMI1EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_LPTIM2_CLK_ENABLE()          do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB1ENR2, RCC_APB1ENR2_LPTIM2EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB1ENR2, RCC_APB1ENR2_LPTIM2EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_TIM2_CLK_DISABLE()           CLEAR_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM2EN)

#define __HAL_RCC_TIM3_CLK_DISABLE()           CLEAR_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM3EN)

#define __HAL_RCC_TIM4_CLK_DISABLE()           CLEAR_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM4EN)

#define __HAL_RCC_TIM5_CLK_DISABLE()           CLEAR_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM5EN)

#define __HAL_RCC_TIM6_CLK_DISABLE()           CLEAR_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM6EN)

#define __HAL_RCC_TIM7_CLK_DISABLE()           CLEAR_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM7EN)

#if defined(LCD)
#define __HAL_RCC_LCD_CLK_DISABLE()            CLEAR_BIT(RCC->APB1ENR1, RCC_APB1ENR1_LCDEN);
#endif /* LCD */

#define __HAL_RCC_SPI2_CLK_DISABLE()           CLEAR_BIT(RCC->APB1ENR1, RCC_APB1ENR1_SPI2EN)

#define __HAL_RCC_SPI3_CLK_DISABLE()           CLEAR_BIT(RCC->APB1ENR1, RCC_APB1ENR1_SPI3EN)

#define __HAL_RCC_USART2_CLK_DISABLE()         CLEAR_BIT(RCC->APB1ENR1, RCC_APB1ENR1_USART2EN)

#define __HAL_RCC_USART3_CLK_DISABLE()         CLEAR_BIT(RCC->APB1ENR1, RCC_APB1ENR1_USART3EN)

#define __HAL_RCC_UART4_CLK_DISABLE()          CLEAR_BIT(RCC->APB1ENR1, RCC_APB1ENR1_UART4EN)

#define __HAL_RCC_UART5_CLK_DISABLE()          CLEAR_BIT(RCC->APB1ENR1, RCC_APB1ENR1_UART5EN)

#define __HAL_RCC_I2C1_CLK_DISABLE()           CLEAR_BIT(RCC->APB1ENR1, RCC_APB1ENR1_I2C1EN)

#define __HAL_RCC_I2C2_CLK_DISABLE()           CLEAR_BIT(RCC->APB1ENR1, RCC_APB1ENR1_I2C2EN)

#define __HAL_RCC_I2C3_CLK_DISABLE()           CLEAR_BIT(RCC->APB1ENR1, RCC_APB1ENR1_I2C3EN)

#define __HAL_RCC_CAN1_CLK_DISABLE()           CLEAR_BIT(RCC->APB1ENR1, RCC_APB1ENR1_CAN1EN)

#define __HAL_RCC_PWR_CLK_DISABLE()            CLEAR_BIT(RCC->APB1ENR1, RCC_APB1ENR1_PWREN)

#define __HAL_RCC_DAC1_CLK_DISABLE()           CLEAR_BIT(RCC->APB1ENR1, RCC_APB1ENR1_DAC1EN)

#define __HAL_RCC_OPAMP_CLK_DISABLE()          CLEAR_BIT(RCC->APB1ENR1, RCC_APB1ENR1_OPAMPEN)

#define __HAL_RCC_LPTIM1_CLK_DISABLE()         CLEAR_BIT(RCC->APB1ENR1, RCC_APB1ENR1_LPTIM1EN)

#define __HAL_RCC_LPUART1_CLK_DISABLE()        CLEAR_BIT(RCC->APB1ENR2, RCC_APB1ENR2_LPUART1EN)

#define __HAL_RCC_SWPMI1_CLK_DISABLE()         CLEAR_BIT(RCC->APB1ENR2, RCC_APB1ENR2_SWPMI1EN)

#define __HAL_RCC_LPTIM2_CLK_DISABLE()         CLEAR_BIT(RCC->APB1ENR2, RCC_APB1ENR2_LPTIM2EN)

/**
  * @}
  */

/** @defgroup RCC_APB2_Clock_Enable_Disable APB2 Peripheral Clock Enable Disable
  * @brief  Enable or disable the APB2 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_SYSCFG_CLK_ENABLE()          do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_FIREWALL_CLK_ENABLE()        do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB2ENR, RCC_APB2ENR_FWEN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_FWEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_SDMMC1_CLK_ENABLE()          do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SDMMC1EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SDMMC1EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_TIM1_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM1EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM1EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_SPI1_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI1EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI1EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_TIM8_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM8EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM8EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_USART1_CLK_ENABLE()          do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)


#define __HAL_RCC_TIM15_CLK_ENABLE()           do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM15EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM15EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_TIM16_CLK_ENABLE()           do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM16EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM16EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_TIM17_CLK_ENABLE()           do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM17EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM17EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_SAI1_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SAI1EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SAI1EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_SAI2_CLK_ENABLE()            do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SAI2EN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SAI2EN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_DFSDM_CLK_ENABLE()           do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB2ENR, RCC_APB2ENR_DFSDMEN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_DFSDMEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_SYSCFG_CLK_DISABLE()         CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN)

#define __HAL_RCC_SDMMC1_CLK_DISABLE()         CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_SDMMC1EN)

#define __HAL_RCC_TIM1_CLK_DISABLE()           CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM1EN)

#define __HAL_RCC_SPI1_CLK_DISABLE()           CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI1EN)

#define __HAL_RCC_TIM8_CLK_DISABLE()           CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM8EN)

#define __HAL_RCC_USART1_CLK_DISABLE()         CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN)

#define __HAL_RCC_TIM15_CLK_DISABLE()          CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM15EN)

#define __HAL_RCC_TIM16_CLK_DISABLE()          CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM16EN)

#define __HAL_RCC_TIM17_CLK_DISABLE()          CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM17EN)

#define __HAL_RCC_SAI1_CLK_DISABLE()           CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_SAI1EN)

#define __HAL_RCC_SAI2_CLK_DISABLE()           CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_SAI2EN)

#define __HAL_RCC_DFSDM_CLK_DISABLE()          CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_DFSDMEN)

/**
  * @}
  */

/** @defgroup RCC_AHB1_Peripheral_Clock_Enable_Disable_Status AHB1 Peripheral Clock Enabled or Disabled Status
  * @brief  Check whether the AHB1 peripheral clock is enabled or not.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_DMA1_IS_CLK_ENABLED()        (READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA1EN) != RESET)

#define __HAL_RCC_DMA2_IS_CLK_ENABLED()        (READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA2EN) != RESET)

#define __HAL_RCC_FLASH_IS_CLK_ENABLED()       (READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_FLASHEN) != RESET)

#define __HAL_RCC_CRC_IS_CLK_ENABLED()         (READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_CRCEN) != RESET)

#define __HAL_RCC_TSC_IS_CLK_ENABLED()         (READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_TSCEN) != RESET)

#define __HAL_RCC_DMA1_IS_CLK_DISABLED()       (READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA1EN) == RESET)

#define __HAL_RCC_DMA2_IS_CLK_DISABLED()       (READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA2EN) == RESET)

#define __HAL_RCC_FLASH_IS_CLK_DISABLED()      (READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_FLASHEN) == RESET)

#define __HAL_RCC_CRC_IS_CLK_DISABLED()        (READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_CRCEN) == RESET)

#define __HAL_RCC_TSC_IS_CLK_DISABLED()        (READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_TSCEN) == RESET)

/**
  * @}
  */

/** @defgroup RCC_AHB2_Clock_Enable_Disable_Status AHB2 Peripheral Clock Enabled or Disabled Status
  * @brief  Check whether the AHB2 peripheral clock is enabled or not.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_GPIOA_IS_CLK_ENABLED()       (READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN) != RESET)

#define __HAL_RCC_GPIOB_IS_CLK_ENABLED()       (READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOCEN) != RESET)

#define __HAL_RCC_GPIOC_IS_CLK_ENABLED()       (READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOCEN) != RESET)

#define __HAL_RCC_GPIOD_IS_CLK_ENABLED()       (READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIODEN) != RESET)

#define __HAL_RCC_GPIOE_IS_CLK_ENABLED()       (READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOEEN) != RESET)

#define __HAL_RCC_GPIOF_IS_CLK_ENABLED()       (READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOFEN) != RESET)

#define __HAL_RCC_GPIOG_IS_CLK_ENABLED()       (READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOGEN) != RESET)

#define __HAL_RCC_GPIOH_IS_CLK_ENABLED()       (READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOHEN) != RESET)

#if defined(USB_OTG_FS)
#define __HAL_RCC_USB_OTG_FS_IS_CLK_ENABLED()  (READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_OTGFSEN) != RESET)
#endif /* USB_OTG_FS */

#define __HAL_RCC_ADC_IS_CLK_ENABLED()         (READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_ADCEN) != RESET)

#if defined(AES)
#define __HAL_RCC_AES_IS_CLK_ENABLED()         (READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_AESEN) != RESET)
#endif /* AES */

#define __HAL_RCC_RNG_IS_CLK_ENABLED()         (READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_RNGEN) != RESET)

#define __HAL_RCC_GPIOA_IS_CLK_DISABLED()      (READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN) == RESET)

#define __HAL_RCC_GPIOB_IS_CLK_DISABLED()      (READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOBEN) == RESET)

#define __HAL_RCC_GPIOC_IS_CLK_DISABLED()      (READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOCEN) == RESET)

#define __HAL_RCC_GPIOD_IS_CLK_DISABLED()      (READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIODEN) == RESET)

#define __HAL_RCC_GPIOE_IS_CLK_DISABLED()      (READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOEEN) == RESET)

#define __HAL_RCC_GPIOF_IS_CLK_DISABLED()      (READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOFEN) == RESET)

#define __HAL_RCC_GPIOG_IS_CLK_DISABLED()      (READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOGEN) == RESET)

#define __HAL_RCC_GPIOH_IS_CLK_DISABLED()      (READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOHEN) == RESET)

#if defined(USB_OTG_FS)
#define __HAL_RCC_USB_OTG_FS_IS_CLK_DISABLED() (READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_OTGFSEN) == RESET)
#endif /* USB_OTG_FS */

#define __HAL_RCC_ADC_IS_CLK_DISABLED()        (READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_ADCEN) == RESET)

#if defined(AES)
#define __HAL_RCC_AES_IS_CLK_DISABLED()        (READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_AESEN) == RESET)
#endif /* AES */

#define __HAL_RCC_RNG_IS_CLK_DISABLED()        (READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_RNGEN) == RESET)

/**
  * @}
  */

/** @defgroup RCC_AHB3_Clock_Enable_Disable_Status AHB3 Peripheral Clock Enabled or Disabled Status
  * @brief  Check whether the AHB3 peripheral clock is enabled or not.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_FMC_IS_CLK_ENABLED()         (READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN) != RESET)

#define __HAL_RCC_QSPI_IS_CLK_ENABLED()        (READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_QSPIEN) != RESET)

#define __HAL_RCC_FMC_IS_CLK_DISABLED()        (READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN) == RESET)

#define __HAL_RCC_QSPI_IS_CLK_DISABLED()       (READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_QSPIEN) == RESET)

/**
  * @}
  */

/** @defgroup RCC_APB1_Clock_Enable_Disable_Status APB1 Peripheral Clock Enabled or Disabled Status
  * @brief  Check whether the APB1 peripheral clock is enabled or not.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_TIM2_IS_CLK_ENABLED()        (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM2EN) != RESET)

#define __HAL_RCC_TIM3_IS_CLK_ENABLED()        (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM3EN) != RESET)

#define __HAL_RCC_TIM4_IS_CLK_ENABLED()        (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM4EN) != RESET)

#define __HAL_RCC_TIM5_IS_CLK_ENABLED()        (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM5EN) != RESET)

#define __HAL_RCC_TIM6_IS_CLK_ENABLED()        (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM6EN) != RESET)

#define __HAL_RCC_TIM7_IS_CLK_ENABLED()        (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM7EN) != RESET)

#if defined(LCD)
#define __HAL_RCC_LCD_IS_CLK_ENABLED()         (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_LCDEN) != RESET)
#endif /* LCD */

#define __HAL_RCC_WWDG_IS_CLK_ENABLED()        (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_WWDGEN) != RESET)

#define __HAL_RCC_SPI2_IS_CLK_ENABLED()        (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_SPI2EN) != RESET)

#define __HAL_RCC_SPI3_IS_CLK_ENABLED()        (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_SPI3EN) != RESET)

#define __HAL_RCC_USART2_IS_CLK_ENABLED()      (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_USART2EN) != RESET)

#define __HAL_RCC_USART3_IS_CLK_ENABLED()      (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_USART3EN) != RESET)

#define __HAL_RCC_UART4_IS_CLK_ENABLED()       (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_UART4EN) != RESET)

#define __HAL_RCC_UART5_IS_CLK_ENABLED()       (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_UART5EN) != RESET)

#define __HAL_RCC_I2C1_IS_CLK_ENABLED()        (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_I2C1EN) != RESET)

#define __HAL_RCC_I2C2_IS_CLK_ENABLED()        (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_I2C2EN) != RESET)

#define __HAL_RCC_I2C3_IS_CLK_ENABLED()        (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_I2C3EN) != RESET)

#define __HAL_RCC_CAN1_IS_CLK_ENABLED()        (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_CAN1EN) != RESET)

#define __HAL_RCC_PWR_IS_CLK_ENABLED()         (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_PWREN) != RESET)

#define __HAL_RCC_DAC1_IS_CLK_ENABLED()        (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_DAC1EN) != RESET)

#define __HAL_RCC_OPAMP_IS_CLK_ENABLED()       (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_OPAMPEN) != RESET)

#define __HAL_RCC_LPTIM1_IS_CLK_ENABLED()      (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_LPTIM1EN) != RESET)

#define __HAL_RCC_LPUART1_IS_CLK_ENABLED()     (READ_BIT(RCC->APB1ENR2, RCC_APB1ENR2_LPUART1EN) != RESET)

#define __HAL_RCC_SWPMI1_IS_CLK_ENABLED()      (READ_BIT(RCC->APB1ENR2, RCC_APB1ENR2_SWPMI1EN) != RESET)

#define __HAL_RCC_LPTIM2_IS_CLK_ENABLED()      (READ_BIT(RCC->APB1ENR2, RCC_APB1ENR2_LPTIM2EN) != RESET)

#define __HAL_RCC_TIM2_IS_CLK_DISABLED()       (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM2EN) == RESET)

#define __HAL_RCC_TIM3_IS_CLK_DISABLED()       (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM3EN) == RESET)

#define __HAL_RCC_TIM4_IS_CLK_DISABLED()       (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM4EN) == RESET)

#define __HAL_RCC_TIM5_IS_CLK_DISABLED()       (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM5EN) == RESET)

#define __HAL_RCC_TIM6_IS_CLK_DISABLED()       (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM6EN) == RESET)

#define __HAL_RCC_TIM7_IS_CLK_DISABLED()       (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM7EN) == RESET)

#if defined(LCD)
#define __HAL_RCC_LCD_IS_CLK_DISABLED()        (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_LCDEN) == RESET)
#endif /* LCD */

#define __HAL_RCC_WWDG_IS_CLK_DISABLED()       (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_WWDGEN) == RESET)

#define __HAL_RCC_SPI2_IS_CLK_DISABLED()       (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_SPI2EN) == RESET)

#define __HAL_RCC_SPI3_IS_CLK_DISABLED()       (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_SPI3EN) == RESET)

#define __HAL_RCC_USART2_IS_CLK_DISABLED()     (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_USART2EN) == RESET)

#define __HAL_RCC_USART3_IS_CLK_DISABLED()     (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_USART3EN) == RESET)

#define __HAL_RCC_UART4_IS_CLK_DISABLED()      (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_UART4EN) == RESET)

#define __HAL_RCC_UART5_IS_CLK_DISABLED()      (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_UART5EN) == RESET)

#define __HAL_RCC_I2C1_IS_CLK_DISABLED()       (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_I2C1EN) == RESET)

#define __HAL_RCC_I2C2_IS_CLK_DISABLED()       (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_I2C2EN) == RESET)

#define __HAL_RCC_I2C3_IS_CLK_DISABLED()       (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_I2C3EN) == RESET)

#define __HAL_RCC_CAN1_IS_CLK_DISABLED()       (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_CAN1EN) == RESET)

#define __HAL_RCC_PWR_IS_CLK_DISABLED()        (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_PWREN) == RESET)

#define __HAL_RCC_DAC1_IS_CLK_DISABLED()       (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_DAC1EN) == RESET)

#define __HAL_RCC_OPAMP_IS_CLK_DISABLED()      (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_OPAMPEN) == RESET)

#define __HAL_RCC_LPTIM1_IS_CLK_DISABLED()     (READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_LPTIM1EN) == RESET)

#define __HAL_RCC_LPUART1_IS_CLK_DISABLED()    (READ_BIT(RCC->APB1ENR2, RCC_APB1ENR2_LPUART1EN) == RESET)

#define __HAL_RCC_SWPMI1_IS_CLK_DISABLED()     (READ_BIT(RCC->APB1ENR2, RCC_APB1ENR2_SWPMI1EN) == RESET)

#define __HAL_RCC_LPTIM2_IS_CLK_DISABLED()     (READ_BIT(RCC->APB1ENR2, RCC_APB1ENR2_LPTIM2EN) == RESET)

/**
  * @}
  */

/** @defgroup RCC_APB2_Clock_Enable_Disable_Status APB2 Peripheral Clock Enabled or Disabled Status
  * @brief  Check whether the APB2 peripheral clock is enabled or not.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_SYSCFG_IS_CLK_ENABLED()      (READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN) != RESET)

#define __HAL_RCC_FIREWALL_IS_CLK_ENABLED()    (READ_BIT(RCC->APB2ENR, RCC_APB2ENR_FWEN) != RESET)

#define __HAL_RCC_SDMMC1_IS_CLK_ENABLED()      (READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SDMMC1EN) != RESET)

#define __HAL_RCC_TIM1_IS_CLK_ENABLED()        (READ_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM1EN) != RESET)

#define __HAL_RCC_SPI1_IS_CLK_ENABLED()        (READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI1EN) != RESET)

#define __HAL_RCC_TIM8_IS_CLK_ENABLED()        (READ_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM8EN) != RESET)

#define __HAL_RCC_USART1_IS_CLK_ENABLED()      (READ_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN) != RESET)

#define __HAL_RCC_TIM15_IS_CLK_ENABLED()       (READ_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM15EN) != RESET)

#define __HAL_RCC_TIM16_IS_CLK_ENABLED()       (READ_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM16EN) != RESET)

#define __HAL_RCC_TIM17_IS_CLK_ENABLED()       (READ_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM17EN) != RESET)

#define __HAL_RCC_SAI1_IS_CLK_ENABLED()        (READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SAI1EN) != RESET)

#define __HAL_RCC_SAI2_IS_CLK_ENABLED()        (READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SAI2EN) != RESET)

#define __HAL_RCC_DFSDM_IS_CLK_ENABLED()       (READ_BIT(RCC->APB2ENR, RCC_APB2ENR_DFSDMEN) != RESET)

#define __HAL_RCC_SYSCFG_IS_CLK_DISABLED()     (READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN) == RESET)

#define __HAL_RCC_SDMMC1_IS_CLK_DISABLED()     (READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SDMMC1EN) == RESET)

#define __HAL_RCC_TIM1_IS_CLK_DISABLED()       (READ_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM1EN) == RESET)

#define __HAL_RCC_SPI1_IS_CLK_DISABLED()       (READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI1EN) == RESET)

#define __HAL_RCC_TIM8_IS_CLK_DISABLED()       (READ_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM8EN) == RESET)

#define __HAL_RCC_USART1_IS_CLK_DISABLED()     (READ_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN) == RESET)

#define __HAL_RCC_TIM15_IS_CLK_DISABLED()      (READ_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM15EN) == RESET)

#define __HAL_RCC_TIM16_IS_CLK_DISABLED()      (READ_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM16EN) == RESET)

#define __HAL_RCC_TIM17_IS_CLK_DISABLED()      (READ_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM17EN) == RESET)

#define __HAL_RCC_SAI1_IS_CLK_DISABLED()       (READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SAI1EN) == RESET)

#define __HAL_RCC_SAI2_IS_CLK_DISABLED()       (READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SAI2EN) == RESET)

#define __HAL_RCC_DFSDM_IS_CLK_DISABLED()      (READ_BIT(RCC->APB2ENR, RCC_APB2ENR_DFSDMEN) == RESET)

/**
  * @}
  */

/** @defgroup RCC_AHB1_Force_Release_Reset AHB1 Peripheral Force Release Reset
  * @brief  Force or release AHB1 peripheral reset.
  * @{
  */
#define __HAL_RCC_AHB1_FORCE_RESET()           WRITE_REG(RCC->AHB1RSTR, 0xFFFFFFFFU)

#define __HAL_RCC_DMA1_FORCE_RESET()           SET_BIT(RCC->AHB1RSTR, RCC_AHB1RSTR_DMA1RST)

#define __HAL_RCC_DMA2_FORCE_RESET()           SET_BIT(RCC->AHB1RSTR, RCC_AHB1RSTR_DMA2RST)

#define __HAL_RCC_FLASH_FORCE_RESET()          SET_BIT(RCC->AHB1RSTR, RCC_AHB1RSTR_FLASHRST)

#define __HAL_RCC_CRC_FORCE_RESET()            SET_BIT(RCC->AHB1RSTR, RCC_AHB1RSTR_CRCRST)

#define __HAL_RCC_TSC_FORCE_RESET()            SET_BIT(RCC->AHB1RSTR, RCC_AHB1RSTR_TSCRST)

#define __HAL_RCC_AHB1_RELEASE_RESET()         WRITE_REG(RCC->AHB1RSTR, 0x00000000U)

#define __HAL_RCC_DMA1_RELEASE_RESET()         CLEAR_BIT(RCC->AHB1RSTR, RCC_AHB1RSTR_DMA1RST)

#define __HAL_RCC_DMA2_RELEASE_RESET()         CLEAR_BIT(RCC->AHB1RSTR, RCC_AHB1RSTR_DMA2RST)

#define __HAL_RCC_FLASH_RELEASE_RESET()        CLEAR_BIT(RCC->AHB1RSTR, RCC_AHB1RSTR_FLASHRST)

#define __HAL_RCC_CRC_RELEASE_RESET()          CLEAR_BIT(RCC->AHB1RSTR, RCC_AHB1RSTR_CRCRST)

#define __HAL_RCC_TSC_RELEASE_RESET()          CLEAR_BIT(RCC->AHB1RSTR, RCC_AHB1RSTR_TSCRST)

/**
  * @}
  */

/** @defgroup RCC_AHB2_Force_Release_Reset AHB2 Peripheral Force Release Reset
  * @brief  Force or release AHB2 peripheral reset.
  * @{
  */
#define __HAL_RCC_AHB2_FORCE_RESET()           WRITE_REG(RCC->AHB2RSTR, 0xFFFFFFFFU)

#define __HAL_RCC_GPIOA_FORCE_RESET()          SET_BIT(RCC->AHB2RSTR, RCC_AHB2RSTR_GPIOARST)

#define __HAL_RCC_GPIOB_FORCE_RESET()          SET_BIT(RCC->AHB2RSTR, RCC_AHB2RSTR_GPIOBRST)

#define __HAL_RCC_GPIOC_FORCE_RESET()          SET_BIT(RCC->AHB2RSTR, RCC_AHB2RSTR_GPIOCRST)

#define __HAL_RCC_GPIOD_FORCE_RESET()          SET_BIT(RCC->AHB2RSTR, RCC_AHB2RSTR_GPIODRST)

#define __HAL_RCC_GPIOE_FORCE_RESET()          SET_BIT(RCC->AHB2RSTR, RCC_AHB2RSTR_GPIOERST)

#define __HAL_RCC_GPIOF_FORCE_RESET()          SET_BIT(RCC->AHB2RSTR, RCC_AHB2RSTR_GPIOFRST)

#define __HAL_RCC_GPIOG_FORCE_RESET()          SET_BIT(RCC->AHB2RSTR, RCC_AHB2RSTR_GPIOGRST)

#define __HAL_RCC_GPIOH_FORCE_RESET()          SET_BIT(RCC->AHB2RSTR, RCC_AHB2RSTR_GPIOHRST)

#if defined(USB_OTG_FS)
#define __HAL_RCC_USB_OTG_FS_FORCE_RESET()     SET_BIT(RCC->AHB2RSTR, RCC_AHB2RSTR_OTGFSRST)
#endif /* USB_OTG_FS */

#define __HAL_RCC_ADC_FORCE_RESET()            SET_BIT(RCC->AHB2RSTR, RCC_AHB2RSTR_ADCRST)

#if defined(AES)
#define __HAL_RCC_AES_FORCE_RESET()            SET_BIT(RCC->AHB2RSTR, RCC_AHB2RSTR_AESRST)
#endif /* AES */

#define __HAL_RCC_RNG_FORCE_RESET()            SET_BIT(RCC->AHB2RSTR, RCC_AHB2RSTR_RNGRST)

#define __HAL_RCC_AHB2_RELEASE_RESET()         WRITE_REG(RCC->AHB2RSTR, 0x00000000U)

#define __HAL_RCC_GPIOA_RELEASE_RESET()        CLEAR_BIT(RCC->AHB2RSTR, RCC_AHB2RSTR_GPIOARST)

#define __HAL_RCC_GPIOB_RELEASE_RESET()        CLEAR_BIT(RCC->AHB2RSTR, RCC_AHB2RSTR_GPIOBRST)

#define __HAL_RCC_GPIOC_RELEASE_RESET()        CLEAR_BIT(RCC->AHB2RSTR, RCC_AHB2RSTR_GPIOCRST)

#define __HAL_RCC_GPIOD_RELEASE_RESET()        CLEAR_BIT(RCC->AHB2RSTR, RCC_AHB2RSTR_GPIODRST)

#define __HAL_RCC_GPIOE_RELEASE_RESET()        CLEAR_BIT(RCC->AHB2RSTR, RCC_AHB2RSTR_GPIOERST)

#define __HAL_RCC_GPIOF_RELEASE_RESET()        CLEAR_BIT(RCC->AHB2RSTR, RCC_AHB2RSTR_GPIOFRST)

#define __HAL_RCC_GPIOG_RELEASE_RESET()        CLEAR_BIT(RCC->AHB2RSTR, RCC_AHB2RSTR_GPIOGRST)

#define __HAL_RCC_GPIOH_RELEASE_RESET()        CLEAR_BIT(RCC->AHB2RSTR, RCC_AHB2RSTR_GPIOHRST)

#if defined(USB_OTG_FS)
#define __HAL_RCC_USB_OTG_FS_RELEASE_RESET()   CLEAR_BIT(RCC->AHB2RSTR, RCC_AHB2RSTR_OTGFSRST)
#endif /* USB_OTG_FS */

#define __HAL_RCC_ADC_RELEASE_RESET()          CLEAR_BIT(RCC->AHB2RSTR, RCC_AHB2RSTR_ADCRST)

#if defined(AES)
#define __HAL_RCC_AES_RELEASE_RESET()          CLEAR_BIT(RCC->AHB2RSTR, RCC_AHB2RSTR_AESRST)
#endif /* AES */

#define __HAL_RCC_RNG_RELEASE_RESET()          CLEAR_BIT(RCC->AHB2RSTR, RCC_AHB2RSTR_RNGRST)

/**
  * @}
  */

/** @defgroup RCC_AHB3_Force_Release_Reset AHB3 Peripheral Force Release Reset
  * @brief  Force or release AHB3 peripheral reset.
  * @{
  */
#define __HAL_RCC_AHB3_FORCE_RESET()           WRITE_REG(RCC->AHB3RSTR, 0xFFFFFFFFU)

#define __HAL_RCC_FMC_FORCE_RESET()            SET_BIT(RCC->AHB3RSTR, RCC_AHB3RSTR_FMCRST)

#define __HAL_RCC_QSPI_FORCE_RESET()           SET_BIT(RCC->AHB3RSTR, RCC_AHB3RSTR_QSPIRST)

#define __HAL_RCC_AHB3_RELEASE_RESET()         WRITE_REG(RCC->AHB3RSTR, 0x00000000U)

#define __HAL_RCC_FMC_RELEASE_RESET()          CLEAR_BIT(RCC->AHB3RSTR, RCC_AHB3RSTR_FMCRST)

#define __HAL_RCC_QSPI_RELEASE_RESET()         CLEAR_BIT(RCC->AHB3RSTR, RCC_AHB3RSTR_QSPIRST)

/**
  * @}
  */

/** @defgroup RCC_APB1_Force_Release_Reset APB1 Peripheral Force Release Reset
  * @brief  Force or release APB1 peripheral reset.
  * @{
  */
#define __HAL_RCC_APB1_FORCE_RESET()           WRITE_REG(RCC->APB1RSTR1, 0xFFFFFFFFU)

#define __HAL_RCC_TIM2_FORCE_RESET()           SET_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_TIM2RST)

#define __HAL_RCC_TIM3_FORCE_RESET()           SET_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_TIM3RST)

#define __HAL_RCC_TIM4_FORCE_RESET()           SET_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_TIM4RST)

#define __HAL_RCC_TIM5_FORCE_RESET()           SET_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_TIM5RST)

#define __HAL_RCC_TIM6_FORCE_RESET()           SET_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_TIM6RST)

#define __HAL_RCC_TIM7_FORCE_RESET()           SET_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_TIM7RST)

#if defined(LCD)
#define __HAL_RCC_LCD_FORCE_RESET()            SET_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_LCDRST)
#endif /* LCD */

#define __HAL_RCC_SPI2_FORCE_RESET()           SET_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_SPI2RST)

#define __HAL_RCC_SPI3_FORCE_RESET()           SET_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_SPI3RST)

#define __HAL_RCC_USART2_FORCE_RESET()         SET_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_USART2RST)

#define __HAL_RCC_USART3_FORCE_RESET()         SET_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_USART3RST)

#define __HAL_RCC_UART4_FORCE_RESET()          SET_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_UART4RST)

#define __HAL_RCC_UART5_FORCE_RESET()          SET_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_UART5RST)

#define __HAL_RCC_I2C1_FORCE_RESET()           SET_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_I2C1RST)

#define __HAL_RCC_I2C2_FORCE_RESET()           SET_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_I2C2RST)

#define __HAL_RCC_I2C3_FORCE_RESET()           SET_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_I2C3RST)

#define __HAL_RCC_CAN1_FORCE_RESET()           SET_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_CAN1RST)

#define __HAL_RCC_PWR_FORCE_RESET()            SET_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_PWRRST)

#define __HAL_RCC_DAC1_FORCE_RESET()           SET_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_DAC1RST)

#define __HAL_RCC_OPAMP_FORCE_RESET()          SET_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_OPAMPRST)

#define __HAL_RCC_LPTIM1_FORCE_RESET()         SET_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_LPTIM1RST)

#define __HAL_RCC_LPUART1_FORCE_RESET()        SET_BIT(RCC->APB1RSTR2, RCC_APB1RSTR2_LPUART1RST)

#define __HAL_RCC_SWPMI1_FORCE_RESET()         SET_BIT(RCC->APB1RSTR2, RCC_APB1RSTR2_SWPMI1RST)

#define __HAL_RCC_LPTIM2_FORCE_RESET()         SET_BIT(RCC->APB1RSTR2, RCC_APB1RSTR2_LPTIM2RST)

#define __HAL_RCC_APB1_RELEASE_RESET()         WRITE_REG(RCC->APB1RSTR1, 0x00000000U)

#define __HAL_RCC_TIM2_RELEASE_RESET()         CLEAR_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_TIM2RST)

#define __HAL_RCC_TIM3_RELEASE_RESET()         CLEAR_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_TIM3RST)

#define __HAL_RCC_TIM4_RELEASE_RESET()         CLEAR_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_TIM4RST)

#define __HAL_RCC_TIM5_RELEASE_RESET()         CLEAR_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_TIM5RST)

#define __HAL_RCC_TIM6_RELEASE_RESET()         CLEAR_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_TIM6RST)

#define __HAL_RCC_TIM7_RELEASE_RESET()         CLEAR_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_TIM7RST)

#if defined(LCD)
#define __HAL_RCC_LCD_RELEASE_RESET()          CLEAR_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_LCDRST)
#endif /* LCD */

#define __HAL_RCC_SPI2_RELEASE_RESET()         CLEAR_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_SPI2RST)

#define __HAL_RCC_SPI3_RELEASE_RESET()         CLEAR_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_SPI3RST)

#define __HAL_RCC_USART2_RELEASE_RESET()       CLEAR_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_USART2RST)

#define __HAL_RCC_USART3_RELEASE_RESET()       CLEAR_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_USART3RST)

#define __HAL_RCC_UART4_RELEASE_RESET()        CLEAR_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_UART4RST)

#define __HAL_RCC_UART5_RELEASE_RESET()        CLEAR_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_UART5RST)

#define __HAL_RCC_I2C1_RELEASE_RESET()         CLEAR_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_I2C1RST)

#define __HAL_RCC_I2C2_RELEASE_RESET()         CLEAR_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_I2C2RST)

#define __HAL_RCC_I2C3_RELEASE_RESET()         CLEAR_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_I2C3RST)

#define __HAL_RCC_CAN1_RELEASE_RESET()         CLEAR_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_CAN1RST)

#define __HAL_RCC_PWR_RELEASE_RESET()          CLEAR_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_PWRRST)

#define __HAL_RCC_DAC1_RELEASE_RESET()         CLEAR_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_DAC1RST)

#define __HAL_RCC_OPAMP_RELEASE_RESET()        CLEAR_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_OPAMPRST)

#define __HAL_RCC_LPTIM1_RELEASE_RESET()       CLEAR_BIT(RCC->APB1RSTR1, RCC_APB1RSTR1_LPTIM1RST)

#define __HAL_RCC_LPUART1_RELEASE_RESET()      CLEAR_BIT(RCC->APB1RSTR2, RCC_APB1RSTR2_LPUART1RST)

#define __HAL_RCC_SWPMI1_RELEASE_RESET()       CLEAR_BIT(RCC->APB1RSTR2, RCC_APB1RSTR2_SWPMI1RST)

#define __HAL_RCC_LPTIM2_RELEASE_RESET()       CLEAR_BIT(RCC->APB1RSTR2, RCC_APB1RSTR2_LPTIM2RST)

/**
  * @}
  */

/** @defgroup RCC_APB2_Force_Release_Reset APB2 Peripheral Force Release Reset
  * @brief  Force or release APB2 peripheral reset.
  * @{
  */
#define __HAL_RCC_APB2_FORCE_RESET()           WRITE_REG(RCC->APB2RSTR, 0xFFFFFFFFU)

#define __HAL_RCC_SYSCFG_FORCE_RESET()         SET_BIT(RCC->APB2RSTR, RCC_APB2RSTR_SYSCFGRST)

#define __HAL_RCC_SDMMC1_FORCE_RESET()         SET_BIT(RCC->APB2RSTR, RCC_APB2RSTR_SDMMC1RST)

#define __HAL_RCC_TIM1_FORCE_RESET()           SET_BIT(RCC->APB2RSTR, RCC_APB2RSTR_TIM1RST)

#define __HAL_RCC_SPI1_FORCE_RESET()           SET_BIT(RCC->APB2RSTR, RCC_APB2RSTR_SPI1RST)

#define __HAL_RCC_TIM8_FORCE_RESET()           SET_BIT(RCC->APB2RSTR, RCC_APB2RSTR_TIM8RST)

#define __HAL_RCC_USART1_FORCE_RESET()         SET_BIT(RCC->APB2RSTR, RCC_APB2RSTR_USART1RST)

#define __HAL_RCC_TIM15_FORCE_RESET()          SET_BIT(RCC->APB2RSTR, RCC_APB2RSTR_TIM15RST)

#define __HAL_RCC_TIM16_FORCE_RESET()          SET_BIT(RCC->APB2RSTR, RCC_APB2RSTR_TIM16RST)

#define __HAL_RCC_TIM17_FORCE_RESET()          SET_BIT(RCC->APB2RSTR, RCC_APB2RSTR_TIM17RST)

#define __HAL_RCC_SAI1_FORCE_RESET()           SET_BIT(RCC->APB2RSTR, RCC_APB2RSTR_SAI1RST)

#define __HAL_RCC_SAI2_FORCE_RESET()           SET_BIT(RCC->APB2RSTR, RCC_APB2RSTR_SAI2RST)

#define __HAL_RCC_DFSDM_FORCE_RESET()          SET_BIT(RCC->APB2RSTR, RCC_APB2RSTR_DFSDMRST)

#define __HAL_RCC_APB2_RELEASE_RESET()         WRITE_REG(RCC->APB2RSTR, 0x00000000U)

#define __HAL_RCC_SYSCFG_RELEASE_RESET()       CLEAR_BIT(RCC->APB2RSTR, RCC_APB2RSTR_SYSCFGRST)

#define __HAL_RCC_SDMMC1_RELEASE_RESET()       CLEAR_BIT(RCC->APB2RSTR, RCC_APB2RSTR_SDMMC1RST)

#define __HAL_RCC_TIM1_RELEASE_RESET()         CLEAR_BIT(RCC->APB2RSTR, RCC_APB2RSTR_TIM1RST)

#define __HAL_RCC_SPI1_RELEASE_RESET()         CLEAR_BIT(RCC->APB2RSTR, RCC_APB2RSTR_SPI1RST)

#define __HAL_RCC_TIM8_RELEASE_RESET()         CLEAR_BIT(RCC->APB2RSTR, RCC_APB2RSTR_TIM8RST)

#define __HAL_RCC_USART1_RELEASE_RESET()       CLEAR_BIT(RCC->APB2RSTR, RCC_APB2RSTR_USART1RST)

#define __HAL_RCC_TIM15_RELEASE_RESET()        CLEAR_BIT(RCC->APB2RSTR, RCC_APB2RSTR_TIM15RST)

#define __HAL_RCC_TIM16_RELEASE_RESET()        CLEAR_BIT(RCC->APB2RSTR, RCC_APB2RSTR_TIM16RST)

#define __HAL_RCC_TIM17_RELEASE_RESET()        CLEAR_BIT(RCC->APB2RSTR, RCC_APB2RSTR_TIM17RST)

#define __HAL_RCC_SAI1_RELEASE_RESET()         CLEAR_BIT(RCC->APB2RSTR, RCC_APB2RSTR_SAI1RST)

#define __HAL_RCC_SAI2_RELEASE_RESET()         CLEAR_BIT(RCC->APB2RSTR, RCC_APB2RSTR_SAI2RST)

#define __HAL_RCC_DFSDM_RELEASE_RESET()        CLEAR_BIT(RCC->APB2RSTR, RCC_APB2RSTR_DFSDMRST)

/**
  * @}
  */

/** @defgroup RCC_AHB1_Clock_Sleep_Enable_Disable AHB1 Peripheral Clock Sleep Enable Disable
  * @brief  Enable or disable the AHB1 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  * @{
  */

#define __HAL_RCC_DMA1_CLK_SLEEP_ENABLE()      SET_BIT(RCC->AHB1SMENR, RCC_AHB1SMENR_DMA1SMEN)

#define __HAL_RCC_DMA2_CLK_SLEEP_ENABLE()      SET_BIT(RCC->AHB1SMENR, RCC_AHB1SMENR_DMA2SMEN)

#define __HAL_RCC_FLASH_CLK_SLEEP_ENABLE()     SET_BIT(RCC->AHB1SMENR, RCC_AHB1SMENR_FLASHSMEN)

#define __HAL_RCC_SRAM1_CLK_SLEEP_ENABLE()     SET_BIT(RCC->AHB1SMENR, RCC_AHB1SMENR_SRAM1SMEN)

#define __HAL_RCC_CRC_CLK_SLEEP_ENABLE()       SET_BIT(RCC->AHB1SMENR, RCC_AHB1SMENR_CRCSMEN)

#define __HAL_RCC_TSC_CLK_SLEEP_ENABLE()       SET_BIT(RCC->AHB1SMENR, RCC_AHB1SMENR_TSCSMEN)

#define __HAL_RCC_DMA1_CLK_SLEEP_DISABLE()     CLEAR_BIT(RCC->AHB1SMENR, RCC_AHB1SMENR_DMA1SMEN)

#define __HAL_RCC_DMA2_CLK_SLEEP_DISABLE()     CLEAR_BIT(RCC->AHB1SMENR, RCC_AHB1SMENR_DMA2SMEN)

#define __HAL_RCC_FLASH_CLK_SLEEP_DISABLE()    CLEAR_BIT(RCC->AHB1SMENR, RCC_AHB1SMENR_FLASHSMEN)

#define __HAL_RCC_SRAM1_CLK_SLEEP_DISABLE()    CLEAR_BIT(RCC->AHB1SMENR, RCC_AHB1SMENR_SRAM1SMEN)

#define __HAL_RCC_CRC_CLK_SLEEP_DISABLE()      CLEAR_BIT(RCC->AHB1SMENR, RCC_AHB1SMENR_CRCSMEN)

#define __HAL_RCC_TSC_CLK_SLEEP_DISABLE()      CLEAR_BIT(RCC->AHB1SMENR, RCC_AHB1SMENR_TSCSMEN)

/**
  * @}
  */

/** @defgroup RCC_AHB2_Clock_Sleep_Enable_Disable AHB2 Peripheral Clock Sleep Enable Disable
  * @brief  Enable or disable the AHB2 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  * @{
  */

#define __HAL_RCC_GPIOA_CLK_SLEEP_ENABLE()     SET_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIOASMEN)

#define __HAL_RCC_GPIOB_CLK_SLEEP_ENABLE()     SET_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIOBSMEN)

#define __HAL_RCC_GPIOC_CLK_SLEEP_ENABLE()     SET_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIOCSMEN)

#define __HAL_RCC_GPIOD_CLK_SLEEP_ENABLE()     SET_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIODSMEN)

#define __HAL_RCC_GPIOE_CLK_SLEEP_ENABLE()     SET_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIOESMEN)

#define __HAL_RCC_GPIOF_CLK_SLEEP_ENABLE()     SET_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIOFSMEN)

#define __HAL_RCC_GPIOG_CLK_SLEEP_ENABLE()     SET_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIOGSMEN)

#define __HAL_RCC_GPIOH_CLK_SLEEP_ENABLE()     SET_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIOHSMEN)

#define __HAL_RCC_SRAM2_CLK_SLEEP_ENABLE()     SET_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_SRAM2SMEN)

#if defined(USB_OTG_FS)
#define __HAL_RCC_USB_OTG_FS_CLK_SLEEP_ENABLE()  SET_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_OTGFSSMEN)
#endif /* USB_OTG_FS */

#define __HAL_RCC_ADC_CLK_SLEEP_ENABLE()       SET_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_ADCSMEN)

#if defined(AES)
#define __HAL_RCC_AES_CLK_SLEEP_ENABLE()       SET_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_AESSMEN)
#endif /* AES */

#define __HAL_RCC_RNG_CLK_SLEEP_ENABLE()       SET_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_RNGSMEN)

#define __HAL_RCC_GPIOA_CLK_SLEEP_DISABLE()    CLEAR_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIOASMEN)

#define __HAL_RCC_GPIOB_CLK_SLEEP_DISABLE()    CLEAR_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIOBSMEN)

#define __HAL_RCC_GPIOC_CLK_SLEEP_DISABLE()    CLEAR_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIOCSMEN)

#define __HAL_RCC_GPIOD_CLK_SLEEP_DISABLE()    CLEAR_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIODSMEN)

#define __HAL_RCC_GPIOE_CLK_SLEEP_DISABLE()    CLEAR_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIOESMEN)

#define __HAL_RCC_GPIOF_CLK_SLEEP_DISABLE()    CLEAR_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIOFSMEN)

#define __HAL_RCC_GPIOG_CLK_SLEEP_DISABLE()    CLEAR_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIOGSMEN)

#define __HAL_RCC_GPIOH_CLK_SLEEP_DISABLE()    CLEAR_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIOHSMEN)

#define __HAL_RCC_SRAM2_CLK_SLEEP_DISABLE()    CLEAR_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_SRAM2SMEN)

#if defined(USB_OTG_FS)
#define __HAL_RCC_USB_OTG_FS_CLK_SLEEP_DISABLE() CLEAR_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_OTGFSSMEN)
#endif /* USB_OTG_FS */

#define __HAL_RCC_ADC_CLK_SLEEP_DISABLE()      CLEAR_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_ADCSMEN)

#if defined(AES)
#define __HAL_RCC_AES_CLK_SLEEP_DISABLE()      CLEAR_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_AESSMEN)
#endif /* AES */

#define __HAL_RCC_RNG_CLK_SLEEP_DISABLE()      CLEAR_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_RNGSMEN)

/**
  * @}
  */

/** @defgroup RCC_AHB3_Clock_Sleep_Enable_Disable AHB3 Peripheral Clock Sleep Enable Disable
  * @brief  Enable or disable the AHB3 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  * @{
  */

#define __HAL_RCC_QSPI_CLK_SLEEP_ENABLE()      SET_BIT(RCC->AHB3SMENR, RCC_AHB3SMENR_QSPISMEN)

#define __HAL_RCC_FMC_CLK_SLEEP_ENABLE()       SET_BIT(RCC->AHB3SMENR, RCC_AHB3SMENR_FMCSMEN)

#define __HAL_RCC_QSPI_CLK_SLEEP_DISABLE()     CLEAR_BIT(RCC->AHB3SMENR, RCC_AHB3SMENR_QSPISMEN)

#define __HAL_RCC_FMC_CLK_SLEEP_DISABLE()      CLEAR_BIT(RCC->AHB3SMENR, RCC_AHB3SMENR_FMCSMEN)

/**
  * @}
  */

/** @defgroup RCC_APB1_Clock_Sleep_Enable_Disable APB1 Peripheral Clock Sleep Enable Disable
  * @brief  Enable or disable the APB1 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  * @{
  */

#define __HAL_RCC_TIM2_CLK_SLEEP_ENABLE()      SET_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_TIM2SMEN)

#define __HAL_RCC_TIM3_CLK_SLEEP_ENABLE()      SET_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_TIM3SMEN)

#define __HAL_RCC_TIM4_CLK_SLEEP_ENABLE()      SET_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_TIM4SMEN)

#define __HAL_RCC_TIM5_CLK_SLEEP_ENABLE()      SET_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_TIM5SMEN)

#define __HAL_RCC_TIM6_CLK_SLEEP_ENABLE()      SET_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_TIM6SMEN)

#define __HAL_RCC_TIM7_CLK_SLEEP_ENABLE()      SET_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_TIM7SMEN)

#if defined(LCD)
#define __HAL_RCC_LCD_CLK_SLEEP_ENABLE()       SET_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_LCDSMEN)
#endif /* LCD */

#define __HAL_RCC_WWDG_CLK_SLEEP_ENABLE()      SET_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_WWDGSMEN)

#define __HAL_RCC_SPI2_CLK_SLEEP_ENABLE()      SET_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_SPI2SMEN)

#define __HAL_RCC_SPI3_CLK_SLEEP_ENABLE()      SET_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_SPI3SMEN)

#define __HAL_RCC_USART2_CLK_SLEEP_ENABLE()    SET_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_USART2SMEN)

#define __HAL_RCC_USART3_CLK_SLEEP_ENABLE()    SET_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_USART3SMEN)

#define __HAL_RCC_UART4_CLK_SLEEP_ENABLE()     SET_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_UART4SMEN)

#define __HAL_RCC_UART5_CLK_SLEEP_ENABLE()     SET_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_UART5SMEN)

#define __HAL_RCC_I2C1_CLK_SLEEP_ENABLE()      SET_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_I2C1SMEN)

#define __HAL_RCC_I2C2_CLK_SLEEP_ENABLE()      SET_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_I2C2SMEN)

#define __HAL_RCC_I2C3_CLK_SLEEP_ENABLE()      SET_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_I2C3SMEN)

#define __HAL_RCC_CAN1_CLK_SLEEP_ENABLE()      SET_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_CAN1SMEN)

#define __HAL_RCC_PWR_CLK_SLEEP_ENABLE()       SET_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_PWRSMEN)

#define __HAL_RCC_DAC1_CLK_SLEEP_ENABLE()      SET_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_DAC1SMEN)

#define __HAL_RCC_OPAMP_CLK_SLEEP_ENABLE()     SET_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_OPAMPSMEN)

#define __HAL_RCC_LPTIM1_CLK_SLEEP_ENABLE()    SET_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_LPTIM1SMEN)

#define __HAL_RCC_LPUART1_CLK_SLEEP_ENABLE()   SET_BIT(RCC->APB1SMENR2, RCC_APB1SMENR2_LPUART1SMEN)

#define __HAL_RCC_SWPMI1_CLK_SLEEP_ENABLE()    SET_BIT(RCC->APB1SMENR2, RCC_APB1SMENR2_SWPMI1SMEN)

#define __HAL_RCC_LPTIM2_CLK_SLEEP_ENABLE()    SET_BIT(RCC->APB1SMENR2, RCC_APB1SMENR2_LPTIM2SMEN)

#define __HAL_RCC_TIM2_CLK_SLEEP_DISABLE()     CLEAR_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_TIM2SMEN)

#define __HAL_RCC_TIM3_CLK_SLEEP_DISABLE()     CLEAR_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_TIM3SMEN)

#define __HAL_RCC_TIM4_CLK_SLEEP_DISABLE()     CLEAR_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_TIM4SMEN)

#define __HAL_RCC_TIM5_CLK_SLEEP_DISABLE()     CLEAR_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_TIM5SMEN)

#define __HAL_RCC_TIM6_CLK_SLEEP_DISABLE()     CLEAR_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_TIM6SMEN)

#define __HAL_RCC_TIM7_CLK_SLEEP_DISABLE()     CLEAR_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_TIM7SMEN)

#if defined(LCD)
#define __HAL_RCC_LCD_CLK_SLEEP_DISABLE()      CLEAR_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_LCDSMEN)
#endif /* LCD */

#define __HAL_RCC_WWDG_CLK_SLEEP_DISABLE()     CLEAR_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_WWDGSMEN)

#define __HAL_RCC_SPI2_CLK_SLEEP_DISABLE()     CLEAR_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_SPI2SMEN)

#define __HAL_RCC_SPI3_CLK_SLEEP_DISABLE()     CLEAR_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_SPI3SMEN)

#define __HAL_RCC_USART2_CLK_SLEEP_DISABLE()   CLEAR_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_USART2SMEN)

#define __HAL_RCC_USART3_CLK_SLEEP_DISABLE()   CLEAR_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_USART3SMEN)

#define __HAL_RCC_UART4_CLK_SLEEP_DISABLE()    CLEAR_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_UART4SMEN)

#define __HAL_RCC_UART5_CLK_SLEEP_DISABLE()    CLEAR_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_UART5SMEN)

#define __HAL_RCC_I2C1_CLK_SLEEP_DISABLE()     CLEAR_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_I2C1SMEN)

#define __HAL_RCC_I2C2_CLK_SLEEP_DISABLE()     CLEAR_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_I2C2SMEN)

#define __HAL_RCC_I2C3_CLK_SLEEP_DISABLE()     CLEAR_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_I2C3SMEN)

#define __HAL_RCC_CAN1_CLK_SLEEP_DISABLE()     CLEAR_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_CAN1SMEN)

#define __HAL_RCC_PWR_CLK_SLEEP_DISABLE()      CLEAR_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_PWRSMEN)

#define __HAL_RCC_DAC1_CLK_SLEEP_DISABLE()     CLEAR_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_DAC1SMEN)

#define __HAL_RCC_OPAMP_CLK_SLEEP_DISABLE()    CLEAR_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_OPAMPSMEN)

#define __HAL_RCC_LPTIM1_CLK_SLEEP_DISABLE()   CLEAR_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_LPTIM1SMEN)

#define __HAL_RCC_LPUART1_CLK_SLEEP_DISABLE()  CLEAR_BIT(RCC->APB1SMENR2, RCC_APB1SMENR2_LPUART1SMEN)

#define __HAL_RCC_SWPMI1_CLK_SLEEP_DISABLE()   CLEAR_BIT(RCC->APB1SMENR2, RCC_APB1SMENR2_SWPMI1SMEN)

#define __HAL_RCC_LPTIM2_CLK_SLEEP_DISABLE()   CLEAR_BIT(RCC->APB1SMENR2, RCC_APB1SMENR2_LPTIM2SMEN)

/**
  * @}
  */

/** @defgroup RCC_APB2_Clock_Sleep_Enable_Disable APB2 Peripheral Clock Sleep Enable Disable
  * @brief  Enable or disable the APB2 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  * @{
  */

#define __HAL_RCC_SYSCFG_CLK_SLEEP_ENABLE()    SET_BIT(RCC->APB2SMENR, RCC_APB2SMENR_SYSCFGSMEN)

#define __HAL_RCC_SDMMC1_CLK_SLEEP_ENABLE()    SET_BIT(RCC->APB2SMENR, RCC_APB2SMENR_SDMMC1SMEN)

#define __HAL_RCC_TIM1_CLK_SLEEP_ENABLE()      SET_BIT(RCC->APB2SMENR, RCC_APB2SMENR_TIM1SMEN)

#define __HAL_RCC_SPI1_CLK_SLEEP_ENABLE()      SET_BIT(RCC->APB2SMENR, RCC_APB2SMENR_SPI1SMEN)

#define __HAL_RCC_TIM8_CLK_SLEEP_ENABLE()      SET_BIT(RCC->APB2SMENR, RCC_APB2SMENR_TIM8SMEN)

#define __HAL_RCC_USART1_CLK_SLEEP_ENABLE()    SET_BIT(RCC->APB2SMENR, RCC_APB2SMENR_USART1SMEN)

#define __HAL_RCC_TIM15_CLK_SLEEP_ENABLE()     SET_BIT(RCC->APB2SMENR, RCC_APB2SMENR_TIM15SMEN)

#define __HAL_RCC_TIM16_CLK_SLEEP_ENABLE()     SET_BIT(RCC->APB2SMENR, RCC_APB2SMENR_TIM16SMEN)

#define __HAL_RCC_TIM17_CLK_SLEEP_ENABLE()     SET_BIT(RCC->APB2SMENR, RCC_APB2SMENR_TIM17SMEN)

#define __HAL_RCC_SAI1_CLK_SLEEP_ENABLE()      SET_BIT(RCC->APB2SMENR, RCC_APB2SMENR_SAI1SMEN)

#define __HAL_RCC_SAI2_CLK_SLEEP_ENABLE()      SET_BIT(RCC->APB2SMENR, RCC_APB2SMENR_SAI2SMEN)

#define __HAL_RCC_DFSDM_CLK_SLEEP_ENABLE()     SET_BIT(RCC->APB2SMENR, RCC_APB2SMENR_DFSDMSMEN)

#define __HAL_RCC_SYSCFG_CLK_SLEEP_DISABLE()   CLEAR_BIT(RCC->APB2SMENR, RCC_APB2SMENR_SYSCFGSMEN)

#define __HAL_RCC_SDMMC1_CLK_SLEEP_DISABLE()   CLEAR_BIT(RCC->APB2SMENR, RCC_APB2SMENR_SDMMC1SMEN)

#define __HAL_RCC_TIM1_CLK_SLEEP_DISABLE()     CLEAR_BIT(RCC->APB2SMENR, RCC_APB2SMENR_TIM1SMEN)

#define __HAL_RCC_SPI1_CLK_SLEEP_DISABLE()     CLEAR_BIT(RCC->APB2SMENR, RCC_APB2SMENR_SPI1SMEN)

#define __HAL_RCC_TIM8_CLK_SLEEP_DISABLE()     CLEAR_BIT(RCC->APB2SMENR, RCC_APB2SMENR_TIM8SMEN)

#define __HAL_RCC_USART1_CLK_SLEEP_DISABLE()   CLEAR_BIT(RCC->APB2SMENR, RCC_APB2SMENR_USART1SMEN)

#define __HAL_RCC_TIM15_CLK_SLEEP_DISABLE()    CLEAR_BIT(RCC->APB2SMENR, RCC_APB2SMENR_TIM15SMEN)

#define __HAL_RCC_TIM16_CLK_SLEEP_DISABLE()    CLEAR_BIT(RCC->APB2SMENR, RCC_APB2SMENR_TIM16SMEN)

#define __HAL_RCC_TIM17_CLK_SLEEP_DISABLE()    CLEAR_BIT(RCC->APB2SMENR, RCC_APB2SMENR_TIM17SMEN)

#define __HAL_RCC_SAI1_CLK_SLEEP_DISABLE()     CLEAR_BIT(RCC->APB2SMENR, RCC_APB2SMENR_SAI1SMEN)

#define __HAL_RCC_SAI2_CLK_SLEEP_DISABLE()     CLEAR_BIT(RCC->APB2SMENR, RCC_APB2SMENR_SAI2SMEN)

#define __HAL_RCC_DFSDM_CLK_SLEEP_DISABLE()    CLEAR_BIT(RCC->APB2SMENR, RCC_APB2SMENR_DFSDMSMEN)

/**
  * @}
  */

/** @defgroup RCC_AHB1_Clock_Sleep_Enable_Disable_Status AHB1 Peripheral Clock Sleep Enabled or Disabled Status
  * @brief  Check whether the AHB1 peripheral clock during Low Power (Sleep) mode is enabled or not.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  * @{
  */

#define __HAL_RCC_DMA1_IS_CLK_SLEEP_ENABLED()    (READ_BIT(RCC->AHB1SMENR, RCC_AHB1SMENR_DMA1SMEN) != RESET)

#define __HAL_RCC_DMA2_IS_CLK_SLEEP_ENABLED()    (READ_BIT(RCC->AHB1SMENR, RCC_AHB1SMENR_DMA2SMEN) != RESET)

#define __HAL_RCC_FLASH_IS_CLK_SLEEP_ENABLED()   (READ_BIT(RCC->AHB1SMENR, RCC_AHB1SMENR_FLASHSMEN) != RESET)

#define __HAL_RCC_SRAM1_IS_CLK_SLEEP_ENABLED()   (READ_BIT(RCC->AHB1SMENR, RCC_AHB1SMENR_SRAM1SMEN) != RESET)

#define __HAL_RCC_CRC_IS_CLK_SLEEP_ENABLED()     (READ_BIT(RCC->AHB1SMENR, RCC_AHB1SMENR_CRCSMEN) != RESET)

#define __HAL_RCC_TSC_IS_CLK_SLEEP_ENABLED()     (READ_BIT(RCC->AHB1SMENR, RCC_AHB1SMENR_TSCSMEN) != RESET)

#define __HAL_RCC_DMA1_IS_CLK_SLEEP_DISABLED()   (READ_BIT(RCC->AHB1SMENR, RCC_AHB1SMENR_DMA1SMEN) == RESET)

#define __HAL_RCC_DMA2_IS_CLK_SLEEP_DISABLED()   (READ_BIT(RCC->AHB1SMENR, RCC_AHB1SMENR_DMA2SMEN) == RESET)

#define __HAL_RCC_FLASH_IS_CLK_SLEEP_DISABLED()  (READ_BIT(RCC->AHB1SMENR, RCC_AHB1SMENR_FLASHSMEN) == RESET)

#define __HAL_RCC_SRAM1_IS_CLK_SLEEP_DISABLED()  (READ_BIT(RCC->AHB1SMENR, RCC_AHB1SMENR_SRAM1SMEN) == RESET)

#define __HAL_RCC_CRC_IS_CLK_SLEEP_DISABLED()    (READ_BIT(RCC->AHB1SMENR, RCC_AHB1SMENR_CRCSMEN) == RESET)

#define __HAL_RCC_TSC_IS_CLK_SLEEP_DISABLED()    (READ_BIT(RCC->AHB1SMENR, RCC_AHB1SMENR_TSCSMEN) == RESET)

/**
  * @}
  */

/** @defgroup RCC_AHB2_Clock_Sleep_Enable_Disable_Status AHB2 Peripheral Clock Sleep Enabled or Disabled Status
  * @brief  Check whether the AHB2 peripheral clock during Low Power (Sleep) mode is enabled or not.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  * @{
  */

#define __HAL_RCC_GPIOA_IS_CLK_SLEEP_ENABLED()   (READ_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIOASMEN) != RESET)

#define __HAL_RCC_GPIOB_IS_CLK_SLEEP_ENABLED()   (READ_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIOBSMEN) != RESET)

#define __HAL_RCC_GPIOC_IS_CLK_SLEEP_ENABLED()   (READ_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIOCSMEN) != RESET)

#define __HAL_RCC_GPIOD_IS_CLK_SLEEP_ENABLED()   (READ_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIODSMEN) != RESET)

#define __HAL_RCC_GPIOE_IS_CLK_SLEEP_ENABLED()   (READ_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIOESMEN) != RESET)

#define __HAL_RCC_GPIOF_IS_CLK_SLEEP_ENABLED()   (READ_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIOFSMEN) != RESET)

#define __HAL_RCC_GPIOG_IS_CLK_SLEEP_ENABLED()   (READ_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIOGSMEN) != RESET)

#define __HAL_RCC_GPIOH_IS_CLK_SLEEP_ENABLED()   (READ_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIOHSMEN) != RESET)

#define __HAL_RCC_SRAM2_IS_CLK_SLEEP_ENABLED()   (READ_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_SRAM2SMEN) != RESET)

#if defined(USB_OTG_FS)
#define __HAL_RCC_USB_OTG_FS_IS_CLK_SLEEP_ENABLED()  (READ_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_OTGFSSMEN) != RESET)
#endif /* USB_OTG_FS */

#define __HAL_RCC_ADC_IS_CLK_SLEEP_ENABLED()     (READ_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_ADCSMEN) != RESET)

#if defined(AES)
#define __HAL_RCC_AES_IS_CLK_SLEEP_ENABLED()     (READ_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_AESSMEN) != RESET)
#endif /* AES */

#define __HAL_RCC_RNG_IS_CLK_SLEEP_ENABLED()     (READ_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_RNGSMEN) != RESET)

#define __HAL_RCC_GPIOA_IS_CLK_SLEEP_DISABLED()  (READ_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIOASMEN) == RESET)

#define __HAL_RCC_GPIOB_IS_CLK_SLEEP_DISABLED()  (READ_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIOBSMEN) == RESET)

#define __HAL_RCC_GPIOC_IS_CLK_SLEEP_DISABLED()  (READ_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIOCSMEN) == RESET)

#define __HAL_RCC_GPIOD_IS_CLK_SLEEP_DISABLED()  (READ_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIODSMEN) == RESET)

#define __HAL_RCC_GPIOE_IS_CLK_SLEEP_DISABLED()  (READ_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIOESMEN) == RESET)

#define __HAL_RCC_GPIOF_IS_CLK_SLEEP_DISABLED()  (READ_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIOFSMEN) == RESET)

#define __HAL_RCC_GPIOG_IS_CLK_SLEEP_DISABLED()  (READ_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIOGSMEN) == RESET)

#define __HAL_RCC_GPIOH_IS_CLK_SLEEP_DISABLED()  (READ_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_GPIOHSMEN) == RESET)

#define __HAL_RCC_SRAM2_IS_CLK_SLEEP_DISABLED()  (READ_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_SRAM2SMEN) == RESET)

#if defined(USB_OTG_FS)
#define __HAL_RCC_USB_OTG_FS_IS_CLK_SLEEP_DISABLED() (READ_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_OTGFSSMEN) == RESET)
#endif /* USB_OTG_FS */

#define __HAL_RCC_ADC_IS_CLK_SLEEP_DISABLED()    (READ_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_ADCSMEN) == RESET)

#if defined(AES)
#define __HAL_RCC_AES_IS_CLK_SLEEP_DISABLED()    (READ_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_AESSMEN) == RESET)
#endif /* AES */

#define __HAL_RCC_RNG_IS_CLK_SLEEP_DISABLED()    (READ_BIT(RCC->AHB2SMENR, RCC_AHB2SMENR_RNGSMEN) == RESET)

/**
  * @}
  */

/** @defgroup RCC_AHB3_Clock_Sleep_Enable_Disable_Status AHB3 Peripheral Clock Sleep Enabled or Disabled Status
  * @brief  Check whether the AHB3 peripheral clock during Low Power (Sleep) mode is enabled or not.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  * @{
  */

#define __HAL_RCC_QSPI_IS_CLK_SLEEP_ENABLED()    (READ_BIT(RCC->AHB3SMENR, RCC_AHB3SMENR_QSPISMEN) != RESET)

#define __HAL_RCC_FMC_IS_CLK_SLEEP_ENABLED()     (READ_BIT(RCC->AHB3SMENR, RCC_AHB3SMENR_FMCSMEN) != RESET)

#define __HAL_RCC_QSPI_IS_CLK_SLEEP_DISABLED()   (READ_BIT(RCC->AHB3SMENR, RCC_AHB3SMENR_QSPISMEN) == RESET)

#define __HAL_RCC_FMC_IS_CLK_SLEEP_DISABLED()    (READ_BIT(RCC->AHB3SMENR, RCC_AHB3SMENR_FMCSMEN) == RESET)

/**
  * @}
  */

/** @defgroup RCC_APB1_Clock_Sleep_Enable_Disable_Status APB1 Peripheral Clock Sleep Enabled or Disabled Status
  * @brief  Check whether the APB1 peripheral clock during Low Power (Sleep) mode is enabled or not.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  * @{
  */

#define __HAL_RCC_TIM2_IS_CLK_SLEEP_ENABLED()      (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_TIM2SMEN) != RESET)

#define __HAL_RCC_TIM3_IS_CLK_SLEEP_ENABLED()      (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_TIM3SMEN) != RESET)

#define __HAL_RCC_TIM4_IS_CLK_SLEEP_ENABLED()      (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_TIM4SMEN) != RESET)

#define __HAL_RCC_TIM5_IS_CLK_SLEEP_ENABLED()      (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_TIM5SMEN) != RESET)

#define __HAL_RCC_TIM6_IS_CLK_SLEEP_ENABLED()      (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_TIM6SMEN) != RESET)

#define __HAL_RCC_TIM7_IS_CLK_SLEEP_ENABLED()      (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_TIM7SMEN) != RESET)

#if defined(LCD)
#define __HAL_RCC_LCD_IS_CLK_SLEEP_ENABLED()       (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_LCDSMEN) != RESET)
#endif /* LCD */

#define __HAL_RCC_WWDG_IS_CLK_SLEEP_ENABLED()      (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_WWDGSMEN) != RESET)

#define __HAL_RCC_SPI2_IS_CLK_SLEEP_ENABLED()      (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_SPI2SMEN) != RESET)

#define __HAL_RCC_SPI3_IS_CLK_SLEEP_ENABLED()      (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_SPI3SMEN) != RESET)

#define __HAL_RCC_USART2_IS_CLK_SLEEP_ENABLED()    (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_USART2SMEN) != RESET)

#define __HAL_RCC_USART3_IS_CLK_SLEEP_ENABLED()    (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_USART3SMEN) != RESET)

#define __HAL_RCC_UART4_IS_CLK_SLEEP_ENABLED()     (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_UART4SMEN) != RESET)

#define __HAL_RCC_UART5_IS_CLK_SLEEP_ENABLED()     (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_UART5SMEN) != RESET)

#define __HAL_RCC_I2C1_IS_CLK_SLEEP_ENABLED()      (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_I2C1SMEN) != RESET)

#define __HAL_RCC_I2C2_IS_CLK_SLEEP_ENABLED()      (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_I2C2SMEN) != RESET)

#define __HAL_RCC_I2C3_IS_CLK_SLEEP_ENABLED()      (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_I2C3SMEN) != RESET)

#define __HAL_RCC_CAN1_IS_CLK_SLEEP_ENABLED()      (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_CAN1SMEN) != RESET)

#define __HAL_RCC_PWR_IS_CLK_SLEEP_ENABLED()       (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_PWRSMEN) != RESET)

#define __HAL_RCC_DAC1_IS_CLK_SLEEP_ENABLED()      (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_DAC1SMEN) != RESET)

#define __HAL_RCC_OPAMP_IS_CLK_SLEEP_ENABLED()     (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_OPAMPSMEN) != RESET)

#define __HAL_RCC_LPTIM1_IS_CLK_SLEEP_ENABLED()    (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_LPTIM1SMEN) != RESET)

#define __HAL_RCC_LPUART1_IS_CLK_SLEEP_ENABLED()   (READ_BIT(RCC->APB1SMENR2, RCC_APB1SMENR2_LPUART1SMEN) != RESET)

#define __HAL_RCC_SWPMI1_IS_CLK_SLEEP_ENABLED()    (READ_BIT(RCC->APB1SMENR2, RCC_APB1SMENR2_SWPMI1SMEN) != RESET)

#define __HAL_RCC_LPTIM2_IS_CLK_SLEEP_ENABLED()    (READ_BIT(RCC->APB1SMENR2, RCC_APB1SMENR2_LPTIM2SMEN) != RESET)

#define __HAL_RCC_TIM2_IS_CLK_SLEEP_DISABLED()     (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_TIM2SMEN) == RESET)

#define __HAL_RCC_TIM3_IS_CLK_SLEEP_DISABLED()     (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_TIM3SMEN) == RESET)

#define __HAL_RCC_TIM4_IS_CLK_SLEEP_DISABLED()     (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_TIM4SMEN) == RESET)

#define __HAL_RCC_TIM5_IS_CLK_SLEEP_DISABLED()     (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_TIM5SMEN) == RESET)

#define __HAL_RCC_TIM6_IS_CLK_SLEEP_DISABLED()     (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_TIM6SMEN) == RESET)

#define __HAL_RCC_TIM7_IS_CLK_SLEEP_DISABLED()     (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_TIM7SMEN) == RESET)

#if defined(LCD)
#define __HAL_RCC_LCD_IS_CLK_SLEEP_DISABLED()      (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_LCDSMEN) == RESET)
#endif /* LCD */

#define __HAL_RCC_WWDG_IS_CLK_SLEEP_DISABLED()     (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_WWDGSMEN) == RESET)

#define __HAL_RCC_SPI2_IS_CLK_SLEEP_DISABLED()     (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_SPI2SMEN) == RESET)

#define __HAL_RCC_SPI3_IS_CLK_SLEEP_DISABLED()     (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_SPI3SMEN) == RESET)

#define __HAL_RCC_USART2_IS_CLK_SLEEP_DISABLED()   (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_USART2SMEN) == RESET)

#define __HAL_RCC_USART3_IS_CLK_SLEEP_DISABLED()   (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_USART3SMEN) == RESET)

#define __HAL_RCC_UART4_IS_CLK_SLEEP_DISABLED()    (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_UART4SMEN) == RESET)

#define __HAL_RCC_UART5_IS_CLK_SLEEP_DISABLED()    (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_UART5SMEN) == RESET)

#define __HAL_RCC_I2C1_IS_CLK_SLEEP_DISABLED()     (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_I2C1SMEN) == RESET)

#define __HAL_RCC_I2C2_IS_CLK_SLEEP_DISABLED()     (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_I2C2SMEN) == RESET)

#define __HAL_RCC_I2C3_IS_CLK_SLEEP_DISABLED()     (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_I2C3SMEN) == RESET)

#define __HAL_RCC_CAN1_IS_CLK_SLEEP_DISABLED()     (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_CAN1SMEN) == RESET)

#define __HAL_RCC_PWR_IS_CLK_SLEEP_DISABLED()      (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_PWRSMEN) == RESET)

#define __HAL_RCC_DAC1_IS_CLK_SLEEP_DISABLED()     (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_DAC1SMEN) == RESET)

#define __HAL_RCC_OPAMP_IS_CLK_SLEEP_DISABLED()    (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_OPAMPSMEN) == RESET)

#define __HAL_RCC_LPTIM1_IS_CLK_SLEEP_DISABLED()   (READ_BIT(RCC->APB1SMENR1, RCC_APB1SMENR1_LPTIM1SMEN) == RESET)

#define __HAL_RCC_LPUART1_IS_CLK_SLEEP_DISABLED()  (READ_BIT(RCC->APB1SMENR2, RCC_APB1SMENR2_LPUART1SMEN) == RESET)

#define __HAL_RCC_SWPMI1_IS_CLK_SLEEP_DISABLED()   (READ_BIT(RCC->APB1SMENR2, RCC_APB1SMENR2_SWPMI1SMEN) == RESET)

#define __HAL_RCC_LPTIM2_IS_CLK_SLEEP_DISABLED()   (READ_BIT(RCC->APB1SMENR2, RCC_APB1SMENR2_LPTIM2SMEN) == RESET)

/**
  * @}
  */

/** @defgroup RCC_APB2_Clock_Sleep_Enable_Disable_Status APB2 Peripheral Clock Sleep Enabled or Disabled Status
  * @brief  Check whether the APB2 peripheral clock during Low Power (Sleep) mode is enabled or not.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  * @{
  */

#define __HAL_RCC_SYSCFG_IS_CLK_SLEEP_ENABLED()    (READ_BIT(RCC->APB2SMENR, RCC_APB2SMENR_SYSCFGSMEN) != RESET)

#define __HAL_RCC_SDMMC1_IS_CLK_SLEEP_ENABLED()    (READ_BIT(RCC->APB2SMENR, RCC_APB2SMENR_SDMMC1SMEN) != RESET)

#define __HAL_RCC_TIM1_IS_CLK_SLEEP_ENABLED()      (READ_BIT(RCC->APB2SMENR, RCC_APB2SMENR_TIM1SMEN) != RESET)

#define __HAL_RCC_SPI1_IS_CLK_SLEEP_ENABLED()      (READ_BIT(RCC->APB2SMENR, RCC_APB2SMENR_SPI1SMEN) != RESET)

#define __HAL_RCC_TIM8_IS_CLK_SLEEP_ENABLED()      (READ_BIT(RCC->APB2SMENR, RCC_APB2SMENR_TIM8SMEN) != RESET)

#define __HAL_RCC_USART1_IS_CLK_SLEEP_ENABLED()    (READ_BIT(RCC->APB2SMENR, RCC_APB2SMENR_USART1SMEN) != RESET)

#define __HAL_RCC_TIM15_IS_CLK_SLEEP_ENABLED()     (READ_BIT(RCC->APB2SMENR, RCC_APB2SMENR_TIM15SMEN) != RESET)

#define __HAL_RCC_TIM16_IS_CLK_SLEEP_ENABLED()     (READ_BIT(RCC->APB2SMENR, RCC_APB2SMENR_TIM16SMEN) != RESET)

#define __HAL_RCC_TIM17_IS_CLK_SLEEP_ENABLED()     (READ_BIT(RCC->APB2SMENR, RCC_APB2SMENR_TIM17SMEN) != RESET)

#define __HAL_RCC_SAI1_IS_CLK_SLEEP_ENABLED()      (READ_BIT(RCC->APB2SMENR, RCC_APB2SMENR_SAI1SMEN) != RESET)

#define __HAL_RCC_SAI2_IS_CLK_SLEEP_ENABLED()      (READ_BIT(RCC->APB2SMENR, RCC_APB2SMENR_SAI2SMEN) != RESET)

#define __HAL_RCC_DFSDM_IS_CLK_SLEEP_ENABLED()     (READ_BIT(RCC->APB2SMENR, RCC_APB2SMENR_DFSDMSMEN) != RESET)

#define __HAL_RCC_SYSCFG_IS_CLK_SLEEP_DISABLED()   (READ_BIT(RCC->APB2SMENR, RCC_APB2SMENR_SYSCFGSMEN) == RESET)

#define __HAL_RCC_SDMMC1_IS_CLK_SLEEP_DISABLED()   (READ_BIT(RCC->APB2SMENR, RCC_APB2SMENR_SDMMC1SMEN) == RESET)

#define __HAL_RCC_TIM1_IS_CLK_SLEEP_DISABLED()     (READ_BIT(RCC->APB2SMENR, RCC_APB2SMENR_TIM1SMEN) == RESET)

#define __HAL_RCC_SPI1_IS_CLK_SLEEP_DISABLED()     (READ_BIT(RCC->APB2SMENR, RCC_APB2SMENR_SPI1SMEN) == RESET)

#define __HAL_RCC_TIM8_IS_CLK_SLEEP_DISABLED()     (READ_BIT(RCC->APB2SMENR, RCC_APB2SMENR_TIM8SMEN) == RESET)

#define __HAL_RCC_USART1_IS_CLK_SLEEP_DISABLED()   (READ_BIT(RCC->APB2SMENR, RCC_APB2SMENR_USART1SMEN) == RESET)

#define __HAL_RCC_TIM15_IS_CLK_SLEEP_DISABLED()    (READ_BIT(RCC->APB2SMENR, RCC_APB2SMENR_TIM15SMEN) == RESET)

#define __HAL_RCC_TIM16_IS_CLK_SLEEP_DISABLED()    (READ_BIT(RCC->APB2SMENR, RCC_APB2SMENR_TIM16SMEN) == RESET)

#define __HAL_RCC_TIM17_IS_CLK_SLEEP_DISABLED()    (READ_BIT(RCC->APB2SMENR, RCC_APB2SMENR_TIM17SMEN) == RESET)

#define __HAL_RCC_SAI1_IS_CLK_SLEEP_DISABLED()     (READ_BIT(RCC->APB2SMENR, RCC_APB2SMENR_SAI1SMEN) == RESET)

#define __HAL_RCC_SAI2_IS_CLK_SLEEP_DISABLED()     (READ_BIT(RCC->APB2SMENR, RCC_APB2SMENR_SAI2SMEN) == RESET)

#define __HAL_RCC_DFSDM_IS_CLK_SLEEP_DISABLED()    (READ_BIT(RCC->APB2SMENR, RCC_APB2SMENR_DFSDMSMEN) == RESET)

/**
  * @}
  */

/** @defgroup RCC_Backup_Domain_Reset RCC Backup Domain Reset
  * @{   
  */ 

/** @brief  Macros to force or release the Backup domain reset.
  * @note   This function resets the RTC peripheral (including the backup registers)
  *         and the RTC clock source selection in RCC_CSR register.
  * @note   The BKPSRAM is not affected by this reset.
  * @retval None
  */
#define __HAL_RCC_BACKUPRESET_FORCE()   SET_BIT(RCC->BDCR, RCC_BDCR_BDRST)

#define __HAL_RCC_BACKUPRESET_RELEASE() CLEAR_BIT(RCC->BDCR, RCC_BDCR_BDRST)

/**
  * @}
  */

/** @defgroup RCC_RTC_Clock_Configuration RCC RTC Clock Configuration
  * @{   
  */ 

/** @brief  Macros to enable or disable the RTC clock.
  * @note   As the RTC is in the Backup domain and write access is denied to
  *         this domain after reset, you have to enable write access using
  *         HAL_PWR_EnableBkUpAccess() function before to configure the RTC
  *         (to be done once after reset).
  * @note   These macros must be used after the RTC clock source was selected.
  * @retval None
  */
#define __HAL_RCC_RTC_ENABLE()         SET_BIT(RCC->BDCR, RCC_BDCR_RTCEN)

#define __HAL_RCC_RTC_DISABLE()        CLEAR_BIT(RCC->BDCR, RCC_BDCR_RTCEN)

/**
  * @}
  */

/** @brief  Macros to enable or disable the Internal High Speed 16MHz oscillator (HSI).
  * @note   The HSI is stopped by hardware when entering STOP and STANDBY modes.
  *         It is used (enabled by hardware) as system clock source after startup
  *         from Reset, wakeup from STOP and STANDBY mode, or in case of failure
  *         of the HSE used directly or indirectly as system clock (if the Clock
  *         Security System CSS is enabled).
  * @note   HSI can not be stopped if it is used as system clock source. In this case,
  *         you have to select another source of the system clock then stop the HSI.
  * @note   After enabling the HSI, the application software should wait on HSIRDY
  *         flag to be set indicating that HSI clock is stable and can be used as
  *         system clock source.
  *         This parameter can be: ENABLE or DISABLE.
  * @note   When the HSI is stopped, HSIRDY flag goes low after 6 HSI oscillator
  *         clock cycles.
  * @retval None
  */
#define __HAL_RCC_HSI_ENABLE()  SET_BIT(RCC->CR, RCC_CR_HSION)

#define __HAL_RCC_HSI_DISABLE() CLEAR_BIT(RCC->CR, RCC_CR_HSION)

/** @brief  Macro to adjust the Internal High Speed 16MHz oscillator (HSI) calibration value.
  * @note   The calibration is used to compensate for the variations in voltage
  *         and temperature that influence the frequency of the internal HSI RC.
  * @param  __HSICALIBRATIONVALUE__: specifies the calibration trimming value
  *         (default is RCC_HSICALIBRATION_DEFAULT).
  *         This parameter must be a number between 0 and 31.
  * @retval None
  */
#define __HAL_RCC_HSI_CALIBRATIONVALUE_ADJUST(__HSICALIBRATIONVALUE__) \
                  MODIFY_REG(RCC->ICSCR, RCC_ICSCR_HSITRIM, (uint32_t)(__HSICALIBRATIONVALUE__) << POSITION_VAL(RCC_ICSCR_HSITRIM))

/**
  * @brief    Macros to enable or disable the wakeup the Internal High Speed oscillator (HSI)
  *           in parallel to the Internal Multi Speed oscillator (MSI) used at system wakeup.
  * @note     The enable of this function has not effect on the HSION bit.
  *           This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
#define __HAL_RCC_HSIAUTOMATIC_START_ENABLE()   SET_BIT(RCC->CR, RCC_CR_HSIASFS)

#define __HAL_RCC_HSIAUTOMATIC_START_DISABLE()  CLEAR_BIT(RCC->CR, RCC_CR_HSIASFS)

/**
  * @brief    Macros to enable or disable the force of the Internal High Speed oscillator (HSI)
  *           in STOP mode to be quickly available as kernel clock for USARTs and I2Cs.
  * @note     Keeping the HSI ON in STOP mode allows to avoid slowing down the communication
  *           speed because of the HSI startup time.
  * @note     The enable of this function has not effect on the HSION bit.
  *           This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
#define __HAL_RCC_HSISTOP_ENABLE()     SET_BIT(RCC->CR, RCC_CR_HSIKERON)

#define __HAL_RCC_HSISTOP_DISABLE()    CLEAR_BIT(RCC->CR, RCC_CR_HSIKERON)

/**
  * @brief  Macros to enable or disable the Internal Multi Speed oscillator (MSI).
  * @note     The MSI is stopped by hardware when entering STOP and STANDBY modes.
  *           It is used (enabled by hardware) as system clock source after
  *           startup from Reset, wakeup from STOP and STANDBY mode, or in case
  *           of failure of the HSE used directly or indirectly as system clock
  *           (if the Clock Security System CSS is enabled).
  * @note     MSI can not be stopped if it is used as system clock source.
  *           In this case, you have to select another source of the system
  *           clock then stop the MSI.
  * @note     After enabling the MSI, the application software should wait on
  *           MSIRDY flag to be set indicating that MSI clock is stable and can
  *           be used as system clock source.
  * @note   When the MSI is stopped, MSIRDY flag goes low after 6 MSI oscillator
  *         clock cycles.
  * @retval None
  */
#define __HAL_RCC_MSI_ENABLE()  SET_BIT(RCC->CR, RCC_CR_MSION)

#define __HAL_RCC_MSI_DISABLE() CLEAR_BIT(RCC->CR, RCC_CR_MSION)

/** @brief  Macro Adjusts the Internal Multi Speed oscillator (MSI) calibration value.
  * @note   The calibration is used to compensate for the variations in voltage
  *         and temperature that influence the frequency of the internal MSI RC.
  *         Refer to the Application Note AN3300 for more details on how to
  *         calibrate the MSI.
  * @param  __MSICALIBRATIONVALUE__: specifies the calibration trimming value
  *         (default is RCC_MSICALIBRATION_DEFAULT).
  *         This parameter must be a number between 0 and 255.
  * @retval None
  */
#define __HAL_RCC_MSI_CALIBRATIONVALUE_ADJUST(__MSICALIBRATIONVALUE__) \
                  MODIFY_REG(RCC->ICSCR, RCC_ICSCR_MSITRIM, (uint32_t)(__MSICALIBRATIONVALUE__) << 8)

/**
  * @brief  Macro configures the Internal Multi Speed oscillator (MSI) clock range in run mode
  * @note     After restart from Reset , the MSI clock is around 4 MHz.
  *           After stop the startup clock can be MSI (at any of its possible
  *           frequencies, the one that was used before entering stop mode) or HSI.
  *          After Standby its frequency can be selected between 4 possible values
  *          (1, 2, 4 or 8 MHz).
  * @note     MSIRANGE can be modified when MSI is OFF (MSION=0) or when MSI is ready
  *          (MSIRDY=1).
  * @note    The MSI clock range after reset can be modified on the fly.
  * @param  __MSIRANGEVALUE__: specifies the MSI clock range.
  *         This parameter must be one of the following values:
  *            @arg @ref RCC_MSIRANGE_0  MSI clock is around 100 KHz
  *            @arg @ref RCC_MSIRANGE_1  MSI clock is around 200 KHz
  *            @arg @ref RCC_MSIRANGE_2  MSI clock is around 400 KHz
  *            @arg @ref RCC_MSIRANGE_3  MSI clock is around 800 KHz
  *            @arg @ref RCC_MSIRANGE_4  MSI clock is around 1 MHz
  *            @arg @ref RCC_MSIRANGE_5  MSI clock is around 2MHz
  *            @arg @ref RCC_MSIRANGE_6  MSI clock is around 4MHz (default after Reset)
  *            @arg @ref RCC_MSIRANGE_7  MSI clock is around 8  MHz
  *            @arg @ref RCC_MSIRANGE_8  MSI clock is around 16 MHz
  *            @arg @ref RCC_MSIRANGE_9  MSI clock is around 24 MHz
  *            @arg @ref RCC_MSIRANGE_10  MSI clock is around 32 MHz
  *            @arg @ref RCC_MSIRANGE_11  MSI clock is around 48 MHz
  * @retval None
  */
#define __HAL_RCC_MSI_RANGE_CONFIG(__MSIRANGEVALUE__) \
                  do {                                                         \
                    SET_BIT(RCC->CR, RCC_CR_MSIRGSEL);                         \
                    MODIFY_REG(RCC->CR, RCC_CR_MSIRANGE, (__MSIRANGEVALUE__)); \
                  } while(0)

/**
  * @brief  Macro configures the Internal Multi Speed oscillator (MSI) clock range after Standby mode
  *         After Standby its frequency can be selected between 4 possible values (1, 2, 4 or 8 MHz).
  * @param  __MSIRANGEVALUE__: specifies the MSI clock range.
  *         This parameter must be one of the following values:
  *            @arg @ref RCC_MSIRANGE_4  MSI clock is around 1 MHz
  *            @arg @ref RCC_MSIRANGE_5  MSI clock is around 2MHz
  *            @arg @ref RCC_MSIRANGE_6  MSI clock is around 4MHz (default after Reset)
  *            @arg @ref RCC_MSIRANGE_7  MSI clock is around 8  MHz
  * @retval None
  */
#define __HAL_RCC_MSI_STANDBY_RANGE_CONFIG(__MSIRANGEVALUE__) \
                  MODIFY_REG(RCC->CSR, RCC_CSR_MSISRANGE, (__MSIRANGEVALUE__) << 4U)

/** @brief  Macro to get the Internal Multi Speed oscillator (MSI) clock range in run mode
  * @retval MSI clock range.
  *         This parameter must be one of the following values:
  *            @arg @ref RCC_MSIRANGE_0  MSI clock is around 100 KHz
  *            @arg @ref RCC_MSIRANGE_1  MSI clock is around 200 KHz
  *            @arg @ref RCC_MSIRANGE_2  MSI clock is around 400 KHz
  *            @arg @ref RCC_MSIRANGE_3  MSI clock is around 800 KHz
  *            @arg @ref RCC_MSIRANGE_4  MSI clock is around 1 MHz
  *            @arg @ref RCC_MSIRANGE_5  MSI clock is around 2MHz
  *            @arg @ref RCC_MSIRANGE_6  MSI clock is around 4MHz (default after Reset)
  *            @arg @ref RCC_MSIRANGE_7  MSI clock is around 8  MHz
  *            @arg @ref RCC_MSIRANGE_8  MSI clock is around 16 MHz
  *            @arg @ref RCC_MSIRANGE_9  MSI clock is around 24 MHz
  *            @arg @ref RCC_MSIRANGE_10  MSI clock is around 32 MHz
  *            @arg @ref RCC_MSIRANGE_11  MSI clock is around 48 MHz
  */
#define __HAL_RCC_GET_MSI_RANGE()                                              \
                  ((READ_BIT(RCC->CR, RCC_CR_MSIRGSEL) != RESET) ?             \
                   (uint32_t)(READ_BIT(RCC->CR, RCC_CR_MSIRANGE)) :            \
                   (uint32_t)(READ_BIT(RCC->CSR, RCC_CSR_MSISRANGE) >> 4))

/** @brief  Macros to enable or disable the Internal Low Speed oscillator (LSI).
  * @note   After enabling the LSI, the application software should wait on
  *         LSIRDY flag to be set indicating that LSI clock is stable and can
  *         be used to clock the IWDG and/or the RTC.
  * @note   LSI can not be disabled if the IWDG is running.
  * @note   When the LSI is stopped, LSIRDY flag goes low after 6 LSI oscillator
  *         clock cycles.
  * @retval None
  */
#define __HAL_RCC_LSI_ENABLE()         SET_BIT(RCC->CSR, RCC_CSR_LSION)

#define __HAL_RCC_LSI_DISABLE()        CLEAR_BIT(RCC->CSR, RCC_CSR_LSION)

/**
  * @brief  Macro to configure the External High Speed oscillator (HSE).
  * @note   Transition HSE Bypass to HSE On and HSE On to HSE Bypass are not
  *         supported by this macro. User should request a transition to HSE Off
  *         first and then HSE On or HSE Bypass.
  * @note   After enabling the HSE (RCC_HSE_ON or RCC_HSE_Bypass), the application
  *         software should wait on HSERDY flag to be set indicating that HSE clock
  *         is stable and can be used to clock the PLL and/or system clock.
  * @note   HSE state can not be changed if it is used directly or through the
  *         PLL as system clock. In this case, you have to select another source
  *         of the system clock then change the HSE state (ex. disable it).
  * @note   The HSE is stopped by hardware when entering STOP and STANDBY modes.
  * @note   This function reset the CSSON bit, so if the clock security system(CSS)
  *         was previously enabled you have to enable it again after calling this
  *         function.
  * @param  __STATE__: specifies the new state of the HSE.
  *         This parameter can be one of the following values:
  *            @arg @ref RCC_HSE_OFF  Turn OFF the HSE oscillator, HSERDY flag goes low after
  *                              6 HSE oscillator clock cycles.
  *            @arg @ref RCC_HSE_ON  Turn ON the HSE oscillator.
  *            @arg @ref RCC_HSE_BYPASS  HSE oscillator bypassed with external clock.
  * @retval None
  */
#define __HAL_RCC_HSE_CONFIG(__STATE__)                      \
                    do {                                     \
                      if((__STATE__) == RCC_HSE_ON)          \
                      {                                      \
                        SET_BIT(RCC->CR, RCC_CR_HSEON);      \
                      }                                      \
                      else if((__STATE__) == RCC_HSE_BYPASS) \
                      {                                      \
                        CLEAR_BIT(RCC->CR, RCC_CR_HSEON);    \
                        SET_BIT(RCC->CR, RCC_CR_HSEBYP);     \
                        SET_BIT(RCC->CR, RCC_CR_HSEON);      \
                      }                                      \
                      else                                   \
                      {                                      \
                        CLEAR_BIT(RCC->CR, RCC_CR_HSEON);    \
                        CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);   \
                      }                                      \
                    } while(0)

/**
  * @brief  Macro to configure the External Low Speed oscillator (LSE).
  * @note   Transitions LSE Bypass to LSE On and LSE On to LSE Bypass are not
  *         supported by this macro. User should request a transition to LSE Off 
  *         first and then LSE On or LSE Bypass.  
  * @note   As the LSE is in the Backup domain and write access is denied to
  *         this domain after reset, you have to enable write access using
  *         HAL_PWR_EnableBkUpAccess() function before to configure the LSE
  *         (to be done once after reset).
  * @note   After enabling the LSE (RCC_LSE_ON or RCC_LSE_BYPASS), the application
  *         software should wait on LSERDY flag to be set indicating that LSE clock
  *         is stable and can be used to clock the RTC.
  * @param  __STATE__: specifies the new state of the LSE.
  *         This parameter can be one of the following values:
  *            @arg @ref RCC_LSE_OFF  Turn OFF the LSE oscillator, LSERDY flag goes low after
  *                              6 LSE oscillator clock cycles.
  *            @arg @ref RCC_LSE_ON  Turn ON the LSE oscillator.
  *            @arg @ref RCC_LSE_BYPASS  LSE oscillator bypassed with external clock.
  * @retval None
  */
#define __HAL_RCC_LSE_CONFIG(__STATE__)                        \
                    do {                                       \
                      if((__STATE__) == RCC_LSE_ON)            \
                      {                                        \
                        SET_BIT(RCC->BDCR, RCC_BDCR_LSEON);    \
                      }                                        \
                      else if((__STATE__) == RCC_LSE_OFF)      \
                      {                                        \
                        CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEON);  \
                        CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEBYP); \
                      }                                        \
                      else if((__STATE__) == RCC_LSE_BYPASS)   \
                      {                                        \
                        CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEON);  \
                        SET_BIT(RCC->BDCR, RCC_BDCR_LSEBYP);   \
                        SET_BIT(RCC->BDCR, RCC_BDCR_LSEON);    \
                      }                                        \
                      else                                     \
                      {                                        \
                        CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEON);  \
                        CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEBYP); \
                      }                                        \
                    } while(0)

/** @brief  Macros to configure the RTC clock (RTCCLK).
  * @note   As the RTC clock configuration bits are in the Backup domain and write
  *         access is denied to this domain after reset, you have to enable write
  *         access using the Power Backup Access macro before to configure
  *         the RTC clock source (to be done once after reset).
  * @note   Once the RTC clock is configured it cannot be changed unless the
  *         Backup domain is reset using __HAL_RCC_BACKUPRESET_FORCE() macro, or by
  *         a Power On Reset (POR).
  *
  * @param  __RTC_CLKSOURCE__: specifies the RTC clock source.
  *         This parameter can be one of the following values:
  *            @arg @ref RCC_RTCCLKSOURCE_LSE  LSE selected as RTC clock.
  *            @arg @ref RCC_RTCCLKSOURCE_LSI  LSI selected as RTC clock.
  *            @arg @ref RCC_RTCCLKSOURCE_HSE_DIV32  HSE clock divided by 32 selected
  *
  * @note   If the LSE or LSI is used as RTC clock source, the RTC continues to
  *         work in STOP and STANDBY modes, and can be used as wakeup source.
  *         However, when the HSE clock is used as RTC clock source, the RTC
  *         cannot be used in STOP and STANDBY modes.
  * @note   The maximum input clock frequency for RTC is 1MHz (when using HSE as
  *         RTC clock source).
  * @retval None
  */
#define __HAL_RCC_RTC_CONFIG(__RTC_CLKSOURCE__)  \
                  MODIFY_REG( RCC->BDCR, RCC_BDCR_RTCSEL, (__RTC_CLKSOURCE__))


/** @brief  Macro to get the RTC clock source.
  * @retval The returned value can be one of the following:
  *            @arg @ref RCC_RTCCLKSOURCE_LSE  LSE selected as RTC clock.
  *            @arg @ref RCC_RTCCLKSOURCE_LSI  LSI selected as RTC clock.
  *            @arg @ref RCC_RTCCLKSOURCE_HSE_DIV32  HSE clock divided by 32 selected
  */
#define  __HAL_RCC_GET_RTC_SOURCE() ((uint32_t)(READ_BIT(RCC->BDCR, RCC_BDCR_RTCSEL)))

/** @brief  Macros to enable or disable the main PLL.
  * @note   After enabling the main PLL, the application software should wait on
  *         PLLRDY flag to be set indicating that PLL clock is stable and can
  *         be used as system clock source.
  * @note   The main PLL can not be disabled if it is used as system clock source
  * @note   The main PLL is disabled by hardware when entering STOP and STANDBY modes.
  * @retval None
  */
#define __HAL_RCC_PLL_ENABLE()         SET_BIT(RCC->CR, RCC_CR_PLLON)

#define __HAL_RCC_PLL_DISABLE()        CLEAR_BIT(RCC->CR, RCC_CR_PLLON)

/** @brief  Macro to configure the PLL clock source.
  * @note   This function must be used only when the main PLL is disabled.
  * @param  __PLLSOURCE__: specifies the PLL entry clock source.
  *         This parameter can be one of the following values:
  *            @arg @ref RCC_PLLSOURCE_NONE  No clock selected as PLL clock entry
  *            @arg @ref RCC_PLLSOURCE_MSI  MSI oscillator clock selected as PLL clock entry
  *            @arg @ref RCC_PLLSOURCE_HSI  HSI oscillator clock selected as PLL clock entry
  *            @arg @ref RCC_PLLSOURCE_HSE  HSE oscillator clock selected as PLL clock entry
  * @note   This clock source is common for the main PLL and audio PLL (PLLSAI1 and PLLSAI2).
  * @retval None
  *      
  */
#define __HAL_RCC_PLL_PLLSOURCE_CONFIG(__PLLSOURCE__) \
                  MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC, (__PLLSOURCE__))

/** @brief  Macro to configure the PLL multiplication factor.
  * @note   This function must be used only when the main PLL is disabled.
  * @param  __PLLM__: specifies the division factor for PLL VCO input clock
  *         This parameter must be a number between Min_Data = 1 and Max_Data = 8.
  * @note   You have to set the PLLM parameter correctly to ensure that the VCO input
  *         frequency ranges from 4 to 16 MHz. It is recommended to select a frequency
  *         of 16 MHz to limit PLL jitter.
  * @retval None
  *      
  */
#define __HAL_RCC_PLL_PLLM_CONFIG(__PLLM__) \
                  MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLM, ((__PLLM__) - 1) << 4U)

/**
  * @brief  Macro to configure the main PLL clock source, multiplication and division factors.
  * @note   This function must be used only when the main PLL is disabled.
  *
  * @param  __PLLSOURCE__: specifies the PLL entry clock source.
  *          This parameter can be one of the following values:
  *            @arg @ref RCC_PLLSOURCE_NONE  No clock selected as PLL clock entry
  *            @arg @ref RCC_PLLSOURCE_MSI  MSI oscillator clock selected as PLL clock entry
  *            @arg @ref RCC_PLLSOURCE_HSI  HSI oscillator clock selected as PLL clock entry
  *            @arg @ref RCC_PLLSOURCE_HSE  HSE oscillator clock selected as PLL clock entry
  * @note   This clock source is common for the main PLL and audio PLL (PLLSAI1 and PLLSAI2).
  *
  * @param  __PLLM__: specifies the division factor for PLL VCO input clock.
  *          This parameter must be a number between 1 and 8.
  * @note   You have to set the PLLM parameter correctly to ensure that the VCO input
  *         frequency ranges from 4 to 16 MHz. It is recommended to select a frequency
  *         of 16 MHz to limit PLL jitter.
  *
  * @param  __PLLN__: specifies the multiplication factor for PLL VCO output clock.
  *          This parameter must be a number between 8 and 86.
  * @note   You have to set the PLLN parameter correctly to ensure that the VCO
  *         output frequency is between 64 and 344 MHz.
  *
  * @param  __PLLP__: specifies the division factor for SAI clock.
  *          This parameter must be a number in the range (7 or 17).
  *
  * @param  __PLLQ__: specifies the division factor for OTG FS, SDMMC1 and RNG clocks.
  *          This parameter must be in the range (2, 4, 6 or 8).
  * @note   If the USB OTG FS is used in your application, you have to set the
  *         PLLQ parameter correctly to have 48 MHz clock for the USB. However,
  *         the SDMMC1 and RNG need a frequency lower than or equal to 48 MHz to work
  *         correctly.
  * @param  __PLLR__: specifies the division factor for the main system clock.
  * @note   You have to set the PLLR parameter correctly to not exceed 80MHZ.
  *          This parameter must be in the range (2, 4, 6 or 8).
  * @retval None
  */
#define __HAL_RCC_PLL_CONFIG(__PLLSOURCE__, __PLLM__, __PLLN__, __PLLP__, __PLLQ__,__PLLR__ ) \
                  (RCC->PLLCFGR = (uint32_t)(((__PLLM__) - 1U) << 4U) | (uint32_t)((__PLLN__) << 8U) | (uint32_t)(((__PLLP__) >> 4U ) << 17U) | \
                   (uint32_t)(__PLLSOURCE__) | (uint32_t)((((__PLLQ__) >> 1U) - 1U) << 21U) | (uint32_t)((((__PLLR__) >> 1U) - 1U) << 25U))

/** @brief  Macro to get the oscillator used as PLL clock source.
  * @retval The oscillator used as PLL clock source. The returned value can be one
  *         of the following:
  *              - RCC_PLLSOURCE_NONE: No oscillator is used as PLL clock source.
  *              - RCC_PLLSOURCE_MSI: MSI oscillator is used as PLL clock source.
  *              - RCC_PLLSOURCE_HSI: HSI oscillator is used as PLL clock source.
  *              - RCC_PLLSOURCE_HSE: HSE oscillator is used as PLL clock source.
  */
#define __HAL_RCC_GET_PLL_OSCSOURCE() ((uint32_t)(RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC))

/**
  * @brief  Enable or disable each clock output (RCC_PLL_SYSCLK, RCC_PLL_48M1CLK, RCC_PLL_SAI3CLK)
  * @note   Enabling/disabling clock outputs RCC_PLL_SAI3CLK and RCC_PLL_48M1CLK can be done at anytime 
  *         without the need to stop the PLL in order to save power. But RCC_PLL_SYSCLK cannot  
  *         be stopped if used as System Clock.
  * @param  __PLLCLOCKOUT__: specifies the PLL clock to be output.
  *          This parameter can be one or a combination of the following values:
  *            @arg @ref RCC_PLL_SAI3CLK  This clock is used to generate an accurate clock to achieve
  *                                   high-quality audio performance on SAI interface in case.
  *            @arg @ref RCC_PLL_48M1CLK  This Clock is used to generate the clock for the USB OTG FS (48 MHz),
  *                                   the random analog generator (<=48 MHz) and the SDMMC1 (<= 48 MHz).
  *            @arg @ref RCC_PLL_SYSCLK  This Clock is used to generate the high speed system clock (up to 80MHz)
  * @retval None
  */
#define __HAL_RCC_PLLCLKOUT_ENABLE(__PLLCLOCKOUT__)   SET_BIT(RCC->PLLCFGR, (__PLLCLOCKOUT__))

#define __HAL_RCC_PLLCLKOUT_DISABLE(__PLLCLOCKOUT__)  CLEAR_BIT(RCC->PLLCFGR, (__PLLCLOCKOUT__))

/**
  * @brief  Get clock output enable status (RCC_PLL_SYSCLK, RCC_PLL_48M1CLK, RCC_PLL_SAI3CLK)
  * @param  __PLLCLOCKOUT__: specifies the output PLL clock to be checked.
  *          This parameter can be one of the following values:
  *            @arg @ref RCC_PLL_SAI3CLK  This clock is used to generate an accurate clock to achieve
  *                                   high-quality audio performance on SAI interface in case.
  *            @arg @ref RCC_PLL_48M1CLK  This Clock is used to generate the clock for the USB OTG FS (48 MHz),
  *                                   the random analog generator (<=48 MHz) and the SDMMC1 (<= 48 MHz).
  *            @arg @ref RCC_PLL_SYSCLK  This Clock is used to generate the high speed system clock (up to 80MHz)
  * @retval SET / RESET
  */
#define __HAL_RCC_GET_PLLCLKOUT_CONFIG(__PLLCLOCKOUT__)  READ_BIT(RCC->PLLCFGR, (__PLLCLOCKOUT__))

/**
  * @brief  Macro to configure the system clock source.
  * @param  __SYSCLKSOURCE__: specifies the system clock source.
  *          This parameter can be one of the following values:
  *              - RCC_SYSCLKSOURCE_MSI: MSI oscillator is used as system clock source.
  *              - RCC_SYSCLKSOURCE_HSI: HSI oscillator is used as system clock source.
  *              - RCC_SYSCLKSOURCE_HSE: HSE oscillator is used as system clock source.
  *              - RCC_SYSCLKSOURCE_PLLCLK: PLL output is used as system clock source.
  * @retval None
  */
#define __HAL_RCC_SYSCLK_CONFIG(__SYSCLKSOURCE__) \
                  MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, (__SYSCLKSOURCE__))

/** @brief  Macro to get the clock source used as system clock.
  * @retval The clock source used as system clock. The returned value can be one
  *         of the following:
  *              - RCC_SYSCLKSOURCE_STATUS_MSI: MSI used as system clock.
  *              - RCC_SYSCLKSOURCE_STATUS_HSI: HSI used as system clock.
  *              - RCC_SYSCLKSOURCE_STATUS_HSE: HSE used as system clock.
  *              - RCC_SYSCLKSOURCE_STATUS_PLLCLK: PLL used as system clock.
  */
#define __HAL_RCC_GET_SYSCLK_SOURCE() ((uint32_t)(RCC->CFGR & RCC_CFGR_SWS))

/**
  * @brief  Macro to configure the External Low Speed oscillator (LSE) drive capability.
  * @note   As the LSE is in the Backup domain and write access is denied to
  *         this domain after reset, you have to enable write access using
  *         HAL_PWR_EnableBkUpAccess() function before to configure the LSE
  *         (to be done once after reset).
  * @param  __LSEDRIVE__: specifies the new state of the LSE drive capability.
  *          This parameter can be one of the following values:
  *            @arg @ref RCC_LSEDRIVE_LOW  LSE oscillator low drive capability.
  *            @arg @ref RCC_LSEDRIVE_MEDIUMLOW  LSE oscillator medium low drive capability.
  *            @arg @ref RCC_LSEDRIVE_MEDIUMHIGH  LSE oscillator medium high drive capability.
  *            @arg @ref RCC_LSEDRIVE_HIGH  LSE oscillator high drive capability.
  * @retval None
  */
#define __HAL_RCC_LSEDRIVE_CONFIG(__LSEDRIVE__) \
                  MODIFY_REG(RCC->BDCR, RCC_BDCR_LSEDRV, (uint32_t)(__LSEDRIVE__))

/**
  * @brief  Macro to configure the wake up from stop clock.
  * @param  __STOPWUCLK__: specifies the clock source used after wake up from stop.
  *         This parameter can be one of the following values:
  *            @arg @ref RCC_STOP_WAKEUPCLOCK_MSI  MSI selected as system clock source
  *            @arg @ref RCC_STOP_WAKEUPCLOCK_HSI  HSI selected as system clock source
  * @retval None
  */
#define __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(__STOPWUCLK__) \
                  MODIFY_REG(RCC->CFGR, RCC_CFGR_STOPWUCK, (__STOPWUCLK__))


/** @brief  Macro to configure the MCO clock.
  * @param  __MCOCLKSOURCE__ specifies the MCO clock source.
  *          This parameter can be one of the following values:
  *            @arg @ref RCC_MCO1SOURCE_NOCLOCK  MCO output disabled
  *            @arg @ref RCC_MCO1SOURCE_SYSCLK  System  clock selected as MCO source
  *            @arg @ref RCC_MCO1SOURCE_MSI  MSI clock selected as MCO source
  *            @arg @ref RCC_MCO1SOURCE_HSI  HSI clock selected as MCO source
  *            @arg @ref RCC_MCO1SOURCE_HSE  HSE clock selected as MCO sourcee
  *            @arg @ref RCC_MCO1SOURCE_PLLCLK  Main PLL clock selected as MCO source
  *            @arg @ref RCC_MCO1SOURCE_LSI  LSI clock selected as MCO source
  *            @arg @ref RCC_MCO1SOURCE_LSE  LSE clock selected as MCO source
  * @param  __MCODIV__ specifies the MCO clock prescaler.
  *          This parameter can be one of the following values:
  *            @arg @ref RCC_MCODIV_1   MCO clock source is divided by 1
  *            @arg @ref RCC_MCODIV_2   MCO clock source is divided by 2
  *            @arg @ref RCC_MCODIV_4   MCO clock source is divided by 4
  *            @arg @ref RCC_MCODIV_8   MCO clock source is divided by 8
  *            @arg @ref RCC_MCODIV_16  MCO clock source is divided by 16
  */
#define __HAL_RCC_MCO1_CONFIG(__MCOCLKSOURCE__, __MCODIV__) \
                 MODIFY_REG(RCC->CFGR, (RCC_CFGR_MCOSEL | RCC_CFGR_MCOPRE), ((__MCOCLKSOURCE__) | (__MCODIV__)))

/** @defgroup RCC_Flags_Interrupts_Management Flags Interrupts Management
  * @brief macros to manage the specified RCC Flags and interrupts.
  * @{
  */

/** @brief  Enable RCC interrupt (Perform Byte access to RCC_CIR[14:8] bits to enable
  *         the selected interrupts).
  * @param  __INTERRUPT__: specifies the RCC interrupt sources to be enabled.
  *         This parameter can be any combination of the following values:
  *            @arg @ref RCC_IT_LSIRDY  LSI ready interrupt
  *            @arg @ref RCC_IT_LSERDY  LSE ready interrupt
  *            @arg @ref RCC_IT_MSIRDY  HSI ready interrupt
  *            @arg @ref RCC_IT_HSIRDY  HSI ready interrupt
  *            @arg @ref RCC_IT_HSERDY  HSE ready interrupt
  *            @arg @ref RCC_IT_PLLRDY  Main PLL ready interrupt
  *            @arg @ref RCC_IT_PLLSAI1RDY  PLLSAI1 ready interrupt
  *            @arg @ref RCC_IT_PLLSAI2RDY  PLLSAI2 ready interrupt
  *            @arg @ref RCC_IT_LSECSS  LSE Clock security system interrupt
  * @retval None
  */
#define __HAL_RCC_ENABLE_IT(__INTERRUPT__) SET_BIT(RCC->CIER, (__INTERRUPT__))

/** @brief Disable RCC interrupt (Perform Byte access to RCC_CIR[14:8] bits to disable
  *        the selected interrupts).
  * @param  __INTERRUPT__: specifies the RCC interrupt sources to be disabled.
  *         This parameter can be any combination of the following values:
  *            @arg @ref RCC_IT_LSIRDY  LSI ready interrupt
  *            @arg @ref RCC_IT_LSERDY  LSE ready interrupt
  *            @arg @ref RCC_IT_MSIRDY  HSI ready interrupt
  *            @arg @ref RCC_IT_HSIRDY  HSI ready interrupt
  *            @arg @ref RCC_IT_HSERDY  HSE ready interrupt
  *            @arg @ref RCC_IT_PLLRDY  Main PLL ready interrupt
  *            @arg @ref RCC_IT_PLLSAI1RDY  PLLSAI1 ready interrupt
  *            @arg @ref RCC_IT_PLLSAI2RDY  PLLSAI2 ready interrupt
  *            @arg @ref RCC_IT_LSECSS  LSE Clock security system interrupt
  * @retval None
  */
#define __HAL_RCC_DISABLE_IT(__INTERRUPT__) CLEAR_BIT(RCC->CIER, (__INTERRUPT__))

/** @brief  Clear the RCC's interrupt pending bits (Perform Byte access to RCC_CIR[23:16]
  *         bits to clear the selected interrupt pending bits.
  * @param  __INTERRUPT__: specifies the interrupt pending bit to clear.
  *         This parameter can be any combination of the following values:
  *            @arg @ref RCC_IT_LSIRDY  LSI ready interrupt
  *            @arg @ref RCC_IT_LSERDY  LSE ready interrupt
  *            @arg @ref RCC_IT_MSIRDY  MSI ready interrupt
  *            @arg @ref RCC_IT_HSIRDY  HSI ready interrupt
  *            @arg @ref RCC_IT_HSERDY  HSE ready interrupt
  *            @arg @ref RCC_IT_PLLRDY  Main PLL ready interrupt
  *            @arg @ref RCC_IT_PLLSAI1RDY  PLLSAI1 ready interrupt
  *            @arg @ref RCC_IT_PLLSAI2RDY  PLLSAI2 ready interrupt
  *            @arg @ref RCC_IT_CSS  HSE Clock security system interrupt
  *            @arg @ref RCC_IT_LSECSS  LSE Clock security system interrupt
  * @retval None
  */
#define __HAL_RCC_CLEAR_IT(__INTERRUPT__) (RCC->CICR = (__INTERRUPT__))

/** @brief  Check whether the RCC interrupt has occurred or not.
  * @param  __INTERRUPT__: specifies the RCC interrupt source to check.
  *         This parameter can be one of the following values:
  *            @arg @ref RCC_IT_LSIRDY  LSI ready interrupt
  *            @arg @ref RCC_IT_LSERDY  LSE ready interrupt
  *            @arg @ref RCC_IT_MSIRDY  MSI ready interrupt
  *            @arg @ref RCC_IT_HSIRDY  HSI ready interrupt
  *            @arg @ref RCC_IT_HSERDY  HSE ready interrupt
  *            @arg @ref RCC_IT_PLLRDY  Main PLL ready interrupt
  *            @arg @ref RCC_IT_PLLSAI1RDY  PLLSAI1 ready interrupt
  *            @arg @ref RCC_IT_PLLSAI2RDY  PLLSAI2 ready interrupt
  *            @arg @ref RCC_IT_CSS  HSE Clock security system interrupt
  *            @arg @ref RCC_IT_LSECSS  LSE Clock security system interrupt
  * @retval The new state of __INTERRUPT__ (TRUE or FALSE).
  */
#define __HAL_RCC_GET_IT(__INTERRUPT__) ((RCC->CIFR & (__INTERRUPT__)) == (__INTERRUPT__))

/** @brief Set RMVF bit to clear the reset flags.
  *        The reset flags are: RCC_FLAG_FWRRST, RCC_FLAG_OBLRST, RCC_FLAG_PINRST, RCC_FLAG_BORRST,
  *        RCC_FLAG_SFTRST, RCC_FLAG_IWDGRST, RCC_FLAG_WWDGRST and RCC_FLAG_LPWRRST.
  * @retval None
 */
#define __HAL_RCC_CLEAR_RESET_FLAGS() (RCC->CSR |= RCC_CSR_RMVF)

/** @brief  Check whether the selected RCC flag is set or not.
  * @param  __FLAG__: specifies the flag to check.
  *         This parameter can be one of the following values:
  *            @arg @ref RCC_FLAG_MSIRDY  MSI oscillator clock ready
  *            @arg @ref RCC_FLAG_HSIRDY  HSI oscillator clock ready
  *            @arg @ref RCC_FLAG_HSERDY  HSE oscillator clock ready
  *            @arg @ref RCC_FLAG_PLLRDY  Main PLL clock ready
  *            @arg @ref RCC_FLAG_PLLSAI1RDY  PLLSAI1 clock ready
  *            @arg @ref RCC_FLAG_PLLSAI2RDY  PLLSAI2 clock ready
  *            @arg @ref RCC_FLAG_LSERDY  LSE oscillator clock ready
  *            @arg @ref RCC_FLAG_LSECSSD  Clock security system failure on LSE oscillator detection
  *            @arg @ref RCC_FLAG_LSIRDY  LSI oscillator clock ready
  *            @arg @ref RCC_FLAG_BORRST  BOR reset
  *            @arg @ref RCC_FLAG_OBLRST  OBLRST reset
  *            @arg @ref RCC_FLAG_PINRST  Pin reset
  *            @arg @ref RCC_FLAG_FWRST  FIREWALL reset
  *            @arg @ref RCC_FLAG_RMVF  Remove reset Flag
  *            @arg @ref RCC_FLAG_SFTRST  Software reset
  *            @arg @ref RCC_FLAG_IWDGRST  Independent Watchdog reset
  *            @arg @ref RCC_FLAG_WWDGRST  Window Watchdog reset
  *            @arg @ref RCC_FLAG_LPWRRST  Low Power reset
  * @retval The new state of __FLAG__ (TRUE or FALSE).
  */
#define __HAL_RCC_GET_FLAG(__FLAG__) (((((((__FLAG__) >> 5U) == 1U) ? RCC->CR :                    \
                                        ((((__FLAG__) >> 5U) == 2U) ? RCC->BDCR :                  \
                                        ((((__FLAG__) >> 5U) == 3U) ? RCC->CSR : RCC->CIFR))) &    \
                                          ((uint32_t)1 << ((__FLAG__) & RCC_FLAG_MASK))) != RESET) \
                                            ? 1U : 0U)

/**
  * @}
  */

/**
  * @}
  */
  
/* Private constants ---------------------------------------------------------*/
/** @defgroup RCC_Private_Constants RCC Private Constants
  * @{
  */
/* Defines used for Flags */
#define CR_REG_INDEX              ((uint32_t)1U)
#define BDCR_REG_INDEX            ((uint32_t)2U)
#define CSR_REG_INDEX             ((uint32_t)3U)

#define RCC_FLAG_MASK             ((uint32_t)0x1FU)
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @addtogroup RCC_Private_Macros
  * @{
  */

#define IS_RCC_OSCILLATORTYPE(__OSCILLATOR__) (((__OSCILLATOR__) == RCC_OSCILLATORTYPE_NONE)                           || \
                                               (((__OSCILLATOR__) & RCC_OSCILLATORTYPE_HSE) == RCC_OSCILLATORTYPE_HSE) || \
                                               (((__OSCILLATOR__) & RCC_OSCILLATORTYPE_HSI) == RCC_OSCILLATORTYPE_HSI) || \
                                               (((__OSCILLATOR__) & RCC_OSCILLATORTYPE_MSI) == RCC_OSCILLATORTYPE_MSI) || \
                                               (((__OSCILLATOR__) & RCC_OSCILLATORTYPE_LSI) == RCC_OSCILLATORTYPE_LSI) || \
                                               (((__OSCILLATOR__) & RCC_OSCILLATORTYPE_LSE) == RCC_OSCILLATORTYPE_LSE))

#define IS_RCC_HSE(__HSE__)  (((__HSE__) == RCC_HSE_OFF) || ((__HSE__) == RCC_HSE_ON) || \
                              ((__HSE__) == RCC_HSE_BYPASS))

#define IS_RCC_LSE(__LSE__)  (((__LSE__) == RCC_LSE_OFF) || ((__LSE__) == RCC_LSE_ON) || \
                              ((__LSE__) == RCC_LSE_BYPASS))

#define IS_RCC_HSI(__HSI__)  (((__HSI__) == RCC_HSI_OFF) || ((__HSI__) == RCC_HSI_ON))

#define IS_RCC_HSI_CALIBRATION_VALUE(__VALUE__) ((__VALUE__) <= (uint32_t)31U)

#define IS_RCC_LSI(__LSI__)  (((__LSI__) == RCC_LSI_OFF) || ((__LSI__) == RCC_LSI_ON))

#define IS_RCC_MSI(__MSI__)  (((__MSI__) == RCC_MSI_OFF) || ((__MSI__) == RCC_MSI_ON))

#define IS_RCC_MSICALIBRATION_VALUE(__VALUE__) ((__VALUE__) <= (uint32_t)255U)

#define IS_RCC_PLL(__PLL__) (((__PLL__) == RCC_PLL_NONE) ||((__PLL__) == RCC_PLL_OFF) || \
                             ((__PLL__) == RCC_PLL_ON))

#define IS_RCC_PLLSOURCE(__SOURCE__) (((__SOURCE__) == RCC_PLLSOURCE_NONE) || \
                                      ((__SOURCE__) == RCC_PLLSOURCE_MSI)  || \
                                      ((__SOURCE__) == RCC_PLLSOURCE_HSI)  || \
                                      ((__SOURCE__) == RCC_PLLSOURCE_HSE))

#define IS_RCC_PLLM_VALUE(__VALUE__) ((__VALUE__) <= 8U)

#define IS_RCC_PLLN_VALUE(__VALUE__) ((8U <= (__VALUE__)) && ((__VALUE__) <= 86U))

#define IS_RCC_PLLP_VALUE(__VALUE__) (((__VALUE__) == 7U) || ((__VALUE__) == 17U))

#define IS_RCC_PLLQ_VALUE(__VALUE__) (((__VALUE__) == 2U) || ((__VALUE__) == 4U) || \
                                      ((__VALUE__) == 6U) || ((__VALUE__) == 8U))

#define IS_RCC_PLLR_VALUE(__VALUE__) (((__VALUE__) == 2U) || ((__VALUE__) == 4U) || \
                                      ((__VALUE__) == 6U) || ((__VALUE__) == 8U))

#define IS_RCC_PLLSAI1CLOCKOUT_VALUE(__VALUE__) (((((__VALUE__) & RCC_PLLSAI1_SAI1CLK) == RCC_PLLSAI1_SAI1CLK)  || \
                                                  (((__VALUE__) & RCC_PLLSAI1_48M2CLK) == RCC_PLLSAI1_48M2CLK)  || \
                                                  (((__VALUE__) & RCC_PLLSAI1_ADC1CLK) == RCC_PLLSAI1_ADC1CLK)) && \
                                                 (((__VALUE__) & ~(RCC_PLLSAI1_SAI1CLK|RCC_PLLSAI1_48M2CLK|RCC_PLLSAI1_ADC1CLK)) == 0U))

#define IS_RCC_PLLSAI2CLOCKOUT_VALUE(__VALUE__) (((((__VALUE__) & RCC_PLLSAI2_SAI2CLK) == RCC_PLLSAI2_SAI2CLK ) || \
                                                  (((__VALUE__) & RCC_PLLSAI2_ADC2CLK) == RCC_PLLSAI2_ADC2CLK)) && \
                                                 (((__VALUE__) & ~(RCC_PLLSAI2_SAI2CLK|RCC_PLLSAI2_ADC2CLK)) == 0U))   

#define IS_RCC_MSI_CLOCK_RANGE(__RANGE__) (((__RANGE__) == RCC_MSIRANGE_0)  || \
                                           ((__RANGE__) == RCC_MSIRANGE_1)  || \
                                           ((__RANGE__) == RCC_MSIRANGE_2)  || \
                                           ((__RANGE__) == RCC_MSIRANGE_3)  || \
                                           ((__RANGE__) == RCC_MSIRANGE_4)  || \
                                           ((__RANGE__) == RCC_MSIRANGE_5)  || \
                                           ((__RANGE__) == RCC_MSIRANGE_6)  || \
                                           ((__RANGE__) == RCC_MSIRANGE_7)  || \
                                           ((__RANGE__) == RCC_MSIRANGE_8)  || \
                                           ((__RANGE__) == RCC_MSIRANGE_9)  || \
                                           ((__RANGE__) == RCC_MSIRANGE_10) || \
                                           ((__RANGE__) == RCC_MSIRANGE_11))

#define IS_RCC_MSI_STANDBY_CLOCK_RANGE(__RANGE__) (((__RANGE__) == RCC_MSIRANGE_4)  || \
                                                   ((__RANGE__) == RCC_MSIRANGE_5)  || \
                                                   ((__RANGE__) == RCC_MSIRANGE_6)  || \
                                                   ((__RANGE__) == RCC_MSIRANGE_7))

#define IS_RCC_CLOCKTYPE(__CLK__)  ((1U <= (__CLK__)) && ((__CLK__) <= 15U))

#define IS_RCC_SYSCLKSOURCE(__SOURCE__) (((__SOURCE__) == RCC_SYSCLKSOURCE_MSI) || \
                                         ((__SOURCE__) == RCC_SYSCLKSOURCE_HSI) || \
                                         ((__SOURCE__) == RCC_SYSCLKSOURCE_HSE) || \
                                         ((__SOURCE__) == RCC_SYSCLKSOURCE_PLLCLK))

#define IS_RCC_HCLK(__HCLK__) (((__HCLK__) == RCC_SYSCLK_DIV1)   || ((__HCLK__) == RCC_SYSCLK_DIV2)   || \
                               ((__HCLK__) == RCC_SYSCLK_DIV4)   || ((__HCLK__) == RCC_SYSCLK_DIV8)   || \
                               ((__HCLK__) == RCC_SYSCLK_DIV16)  || ((__HCLK__) == RCC_SYSCLK_DIV64)  || \
                               ((__HCLK__) == RCC_SYSCLK_DIV128) || ((__HCLK__) == RCC_SYSCLK_DIV256) || \
                               ((__HCLK__) == RCC_SYSCLK_DIV512))

#define IS_RCC_PCLK(__PCLK__) (((__PCLK__) == RCC_HCLK_DIV1) || ((__PCLK__) == RCC_HCLK_DIV2) || \
                               ((__PCLK__) == RCC_HCLK_DIV4) || ((__PCLK__) == RCC_HCLK_DIV8) || \
                               ((__PCLK__) == RCC_HCLK_DIV16))

#define IS_RCC_RTCCLKSOURCE(__SOURCE__) (((__SOURCE__) == RCC_RTCCLKSOURCE_LSE) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_LSI) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV32))

#define IS_RCC_MCO(__MCOX__) ((__MCOX__) == RCC_MCO1)

#define IS_RCC_MCO1SOURCE(__SOURCE__) (((__SOURCE__) == RCC_MCO1SOURCE_NOCLOCK) || \
                                       ((__SOURCE__) == RCC_MCO1SOURCE_SYSCLK) || \
                                       ((__SOURCE__) == RCC_MCO1SOURCE_MSI) || \
                                       ((__SOURCE__) == RCC_MCO1SOURCE_HSI) || \
                                       ((__SOURCE__) == RCC_MCO1SOURCE_HSE) || \
                                       ((__SOURCE__) == RCC_MCO1SOURCE_PLLCLK) || \
                                       ((__SOURCE__) == RCC_MCO1SOURCE_LSI) || \
                                       ((__SOURCE__) == RCC_MCO1SOURCE_LSE))

#define IS_RCC_MCODIV(__DIV__) (((__DIV__) == RCC_MCODIV_1) || ((__DIV__) == RCC_MCODIV_2) || \
                                ((__DIV__) == RCC_MCODIV_4) || ((__DIV__) == RCC_MCODIV_8) || \
                                ((__DIV__) == RCC_MCODIV_16))

#define IS_RCC_LSE_DRIVE(__DRIVE__) (((__DRIVE__) == RCC_LSEDRIVE_LOW)        || \
                                     ((__DRIVE__) == RCC_LSEDRIVE_MEDIUMLOW)  || \
                                     ((__DRIVE__) == RCC_LSEDRIVE_MEDIUMHIGH) || \
                                     ((__DRIVE__) == RCC_LSEDRIVE_HIGH))

#define IS_RCC_STOP_WAKEUPCLOCK(__SOURCE__) (((__SOURCE__) == RCC_STOP_WAKEUPCLOCK_MSI) || \
                                             ((__SOURCE__) == RCC_STOP_WAKEUPCLOCK_HSI))
/**
  * @}
  */

/* Include RCC HAL Extended module */
#include "stm32l4xx_hal_rcc_ex.h"

/* Exported functions --------------------------------------------------------*/
/** @addtogroup RCC_Exported_Functions
  * @{
  */


/** @addtogroup RCC_Exported_Functions_Group1
  * @{
  */

/* Initialization and de-initialization functions  ******************************/
void              HAL_RCC_DeInit(void);
HAL_StatusTypeDef HAL_RCC_OscConfig(RCC_OscInitTypeDef *RCC_OscInitStruct);
HAL_StatusTypeDef HAL_RCC_ClockConfig(RCC_ClkInitTypeDef *RCC_ClkInitStruct, uint32_t FLatency);

/**
  * @}
  */

/** @addtogroup RCC_Exported_Functions_Group2
  * @{
  */

/* Peripheral Control functions  ************************************************/
void              HAL_RCC_MCOConfig(uint32_t RCC_MCOx, uint32_t RCC_MCOSource, uint32_t RCC_MCODiv);
void              HAL_RCC_EnableCSS(void);
uint32_t          HAL_RCC_GetSysClockFreq(void);
uint32_t          HAL_RCC_GetHCLKFreq(void);
uint32_t          HAL_RCC_GetPCLK1Freq(void);
uint32_t          HAL_RCC_GetPCLK2Freq(void);
void              HAL_RCC_GetOscConfig(RCC_OscInitTypeDef *RCC_OscInitStruct);
void              HAL_RCC_GetClockConfig(RCC_ClkInitTypeDef *RCC_ClkInitStruct, uint32_t *pFLatency);
/* CSS NMI IRQ handler */
void              HAL_RCC_NMI_IRQHandler(void);
/* User Callbacks in non blocking mode (IT mode) */
void              HAL_RCC_CSSCallback(void);

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

#endif /* __STM32L4xx_HAL_RCC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
