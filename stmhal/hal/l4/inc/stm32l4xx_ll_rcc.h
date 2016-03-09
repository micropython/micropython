/**
  ******************************************************************************
  * @file    stm32l4xx_ll_rcc.h
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   Header file of RCC LL module.
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
#ifndef __STM32L4xx_LL_RCC_H
#define __STM32L4xx_LL_RCC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx.h"

/** @addtogroup STM32L4xx_LL_Driver
  * @{
  */

#if defined(RCC)

/** @defgroup RCC_LL RCC
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** @defgroup RCC_LL_Private_Variables RCC Private Variables
  * @{
  */

static const uint8_t aRCC_APBAHBPrescTable[8] = {0, 0, 0, 0, 1, 2, 3, 4};

/**
  * @}
  */

/* Private constants ---------------------------------------------------------*/
/** @defgroup RCC_LL_Private_Constants RCC Private Constants
  * @{
  */
/* Defines used for the bit position in the register and perform offsets*/
#define RCC_POSITION_HPRE       (uint32_t)POSITION_VAL(RCC_CFGR_HPRE)
#define RCC_POSITION_PPRE1      (uint32_t)POSITION_VAL(RCC_CFGR_PPRE1)
#define RCC_POSITION_PPRE2      (uint32_t)POSITION_VAL(RCC_CFGR_PPRE2)
#define RCC_POSITION_HSICAL     (uint32_t)POSITION_VAL(RCC_ICSCR_HSICAL)
#define RCC_POSITION_HSITRIM    (uint32_t)POSITION_VAL(RCC_ICSCR_HSITRIM)
#define RCC_POSITION_MSICAL     (uint32_t)POSITION_VAL(RCC_ICSCR_MSICAL)
#define RCC_POSITION_MSITRIM    (uint32_t)POSITION_VAL(RCC_ICSCR_MSITRIM)
#define RCC_POSITION_PLLN       (uint32_t)POSITION_VAL(RCC_PLLCFGR_PLLN)
#define RCC_POSITION_PLLM       (uint32_t)POSITION_VAL(RCC_PLLCFGR_PLLM)
#define RCC_POSITION_PLLR       (uint32_t)POSITION_VAL(RCC_PLLCFGR_PLLR)
#define RCC_POSITION_PLLP       (uint32_t)POSITION_VAL(RCC_PLLCFGR_PLLP)
#define RCC_POSITION_PLLQ       (uint32_t)POSITION_VAL(RCC_PLLCFGR_PLLQ)
#define RCC_POSITION_PLLSAI1N   (uint32_t)POSITION_VAL(RCC_PLLSAI1CFGR_PLLSAI1N)
#define RCC_POSITION_PLLSAI1P   (uint32_t)POSITION_VAL(RCC_PLLSAI1CFGR_PLLSAI1P)
#define RCC_POSITION_PLLSAI1Q   (uint32_t)POSITION_VAL(RCC_PLLSAI1CFGR_PLLSAI1Q)
#define RCC_POSITION_PLLSAI1R   (uint32_t)POSITION_VAL(RCC_PLLSAI1CFGR_PLLSAI1R)
#if defined(RCC_PLLSAI2_SUPPORT)
#define RCC_POSITION_PLLSAI2N   (uint32_t)POSITION_VAL(RCC_PLLSAI2CFGR_PLLSAI2N)
#define RCC_POSITION_PLLSAI2P   (uint32_t)POSITION_VAL(RCC_PLLSAI2CFGR_PLLSAI2P)
#define RCC_POSITION_PLLSAI2R   (uint32_t)POSITION_VAL(RCC_PLLSAI2CFGR_PLLSAI2R)
#endif /* RCC_PLLSAI2_SUPPORT */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
#if defined(USE_FULL_LL_DRIVER)
/** @defgroup RCC_LL_Private_Macros RCC Private Macros
  * @{
  */
/**
  * @}
  */
#endif /*USE_FULL_LL_DRIVER*/
/* Exported types ------------------------------------------------------------*/
#if defined(USE_FULL_LL_DRIVER)
/** @defgroup RCC_LL_Exported_Types RCC Exported Types
  * @{
  */

/** @defgroup LL_ES_CLOCK_FREQ Clocks Frequency Structure
  * @{
  */

/**
  * @brief  RCC Clocks Frequency Structure
  */
typedef struct
{
  uint32_t SYSCLK_Frequency;        /*!< SYSCLK clock frequency */
  uint32_t HCLK_Frequency;          /*!< HCLK clock frequency */
  uint32_t PCLK1_Frequency;         /*!< PCLK1 clock frequency */
  uint32_t PCLK2_Frequency;         /*!< PCLK2 clock frequency */
} LL_RCC_ClocksTypeDef;

/**
  * @}
  */

/**
  * @}
  */
#endif /* USE_FULL_LL_DRIVER */

/* Exported constants --------------------------------------------------------*/
/** @defgroup RCC_LL_Exported_Constants RCC Exported Constants
  * @{
  */

/** @defgroup RCC_LL_EC_OSC_VALUES Oscillator Values adaptation
  * @brief    Defines used to adapt values of different oscillators
  * @note     These values could be modified in the user environment according to 
  *           HW set-up.
  * @{
  */
#if !defined  (HSE_VALUE)
#define HSE_VALUE    ((uint32_t)8000000)  /*!< Value of the HSE oscillator in Hz */
#endif /* HSE_VALUE */

#if !defined  (HSI_VALUE)
#define HSI_VALUE    ((uint32_t)16000000) /*!< Value of the HSI oscillator in Hz */
#endif /* HSI_VALUE */

#if !defined  (LSE_VALUE)
#define LSE_VALUE    ((uint32_t)32768)    /*!< Value of the LSE oscillator in Hz */
#endif /* LSE_VALUE */

#if !defined  (LSI_VALUE)
#define LSI_VALUE    ((uint32_t)32000)    /*!< Value of the LSI oscillator in Hz */
#endif /* LSI_VALUE */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_CLEAR_FLAG Clear Flags Defines
  * @brief    Flags defines which can be used with LL_RCC_WriteReg function
  * @{
  */
#define LL_RCC_CICR_LSIRDYC                RCC_CICR_LSIRDYC     /*!< LSI Ready Interrupt Clear */
#define LL_RCC_CICR_LSERDYC                RCC_CICR_LSERDYC     /*!< LSE Ready Interrupt Clear */
#define LL_RCC_CICR_MSIRDYC                RCC_CICR_MSIRDYC     /*!< MSI Ready Interrupt Clear */
#define LL_RCC_CICR_HSIRDYC                RCC_CICR_HSIRDYC     /*!< HSI Ready Interrupt Clear */
#define LL_RCC_CICR_HSERDYC                RCC_CICR_HSERDYC     /*!< HSE Ready Interrupt Clear */
#define LL_RCC_CICR_PLLRDYC                RCC_CICR_PLLRDYC     /*!< PLL Ready Interrupt Clear */
#define LL_RCC_CICR_PLLSAI1RDYC            RCC_CICR_PLLSAI1RDYC /*!< PLLSAI1 Ready Interrupt Clear */
#if defined(RCC_PLLSAI2_SUPPORT)
#define LL_RCC_CICR_PLLSAI2RDYC            RCC_CICR_PLLSAI2RDYC /*!< PLLSAI2 Ready Interrupt Clear */
#endif /* RCC_PLLSAI2_SUPPORT */
#define LL_RCC_CICR_LSECSSC                RCC_CICR_LSECSSC     /*!< LSE Clock Security System Interrupt Clear */
#define LL_RCC_CICR_CSSC                   RCC_CICR_CSSC        /*!< Clock Security System Interrupt Clear */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_GET_FLAG Get Flags Defines
  * @brief    Flags defines which can be used with LL_RCC_ReadReg function
  * @{
  */
#define LL_RCC_CIFR_LSIRDYF                RCC_CIFR_LSIRDYF     /*!< LSI Ready Interrupt flag */
#define LL_RCC_CIFR_LSERDYF                RCC_CIFR_LSERDYF     /*!< LSE Ready Interrupt flag */
#define LL_RCC_CIFR_MSIRDYF                RCC_CIFR_MSIRDYF     /*!< MSI Ready Interrupt flag */
#define LL_RCC_CIFR_HSIRDYF                RCC_CIFR_HSIRDYF     /*!< HSI Ready Interrupt flag */
#define LL_RCC_CIFR_HSERDYF                RCC_CIFR_HSERDYF     /*!< HSE Ready Interrupt flag */
#define LL_RCC_CIFR_PLLRDYF                RCC_CIFR_PLLRDYF     /*!< PLL Ready Interrupt flag */
#define LL_RCC_CIFR_PLLSAI1RDYF            RCC_CIFR_PLLSAI1RDYF /*!< PLLSAI1 Ready Interrupt flag */
#if defined(RCC_PLLSAI2_SUPPORT)
#define LL_RCC_CIFR_PLLSAI2RDYF            RCC_CIFR_PLLSAI2RDYF /*!< PLLSAI2 Ready Interrupt flag */
#endif /* RCC_PLLSAI2_SUPPORT */
#define LL_RCC_CIFR_LSECSSF                RCC_CIFR_LSECSSF     /*!< LSE Clock Security System Interrupt flag */
#define LL_RCC_CIFR_CSSF                   RCC_CIFR_CSSF        /*!< Clock Security System Interrupt flag */
#define LL_RCC_CSR_FWRSTF                  RCC_CSR_FWRSTF     /*!< Firewall reset flag */
#define LL_RCC_CSR_LPWRRSTF                RCC_CSR_LPWRRSTF   /*!< Low-Power reset flag */
#define LL_RCC_CSR_OBLRSTF                 RCC_CSR_OBLRSTF    /*!< OBL reset flag */
#define LL_RCC_CSR_PINRSTF                 RCC_CSR_PINRSTF    /*!< PIN reset flag */
#define LL_RCC_CSR_SFTRSTF                 RCC_CSR_SFTRSTF    /*!< Software Reset flag */
#define LL_RCC_CSR_IWDGRSTF                RCC_CSR_IWDGRSTF   /*!< Independent Watchdog reset flag */
#define LL_RCC_CSR_WWDGRSTF                RCC_CSR_WWDGRSTF   /*!< Window watchdog reset flag */
#define LL_RCC_CSR_BORRSTF                 RCC_CSR_BORRSTF    /*!< BOR reset flag */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_IT IT Defines
  * @brief    IT defines which can be used with LL_RCC_ReadReg and  LL_RCC_WriteReg functions
  * @{
  */
#define LL_RCC_CIER_LSIRDYIE               RCC_CIER_LSIRDYIE      /*!< LSI Ready Interrupt Enable */
#define LL_RCC_CIER_LSERDYIE               RCC_CIER_LSERDYIE      /*!< LSE Ready Interrupt Enable */
#define LL_RCC_CIER_MSIRDYIE               RCC_CIER_MSIRDYIE      /*!< MSI Ready Interrupt Enable */
#define LL_RCC_CIER_HSIRDYIE               RCC_CIER_HSIRDYIE      /*!< HSI Ready Interrupt Enable */
#define LL_RCC_CIER_HSERDYIE               RCC_CIER_HSERDYIE      /*!< HSE Ready Interrupt Enable */
#define LL_RCC_CIER_PLLRDYIE               RCC_CIER_PLLRDYIE      /*!< PLL Ready Interrupt Enable */
#define LL_RCC_CIER_PLLSAI1RDYIE           RCC_CIER_PLLSAI1RDYIE  /*!< PLLSAI1 Ready Interrupt Enable */
#if defined(RCC_PLLSAI2_SUPPORT)
#define LL_RCC_CIER_PLLSAI2RDYIE           RCC_CIER_PLLSAI2RDYIE  /*!< PLLSAI2 Ready Interrupt Enable */
#endif /* RCC_PLLSAI2_SUPPORT */
#define LL_RCC_CIER_LSECSSIE               RCC_CIER_LSECSSIE      /*!< LSE CSS Interrupt Enable */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_LSEDRIVE  LSE oscillator drive capability
  * @{
  */
#define LL_RCC_LSEDRIVE_LOW                ((uint32_t)0x00000000)  /*!< Xtal mode lower driving capability */
#define LL_RCC_LSEDRIVE_MEDIUMLOW          RCC_BDCR_LSEDRV_0       /*!< Xtal mode medium low driving capability */
#define LL_RCC_LSEDRIVE_MEDIUMHIGH         RCC_BDCR_LSEDRV_1       /*!< Xtal mode medium high driving capability */
#define LL_RCC_LSEDRIVE_HIGH               RCC_BDCR_LSEDRV         /*!< Xtal mode higher driving capability */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_MSIRANGE  MSI clock ranges
  * @{
  */
#define LL_RCC_MSIRANGE_0                  RCC_CR_MSIRANGE_0  /*!< MSI = 100 KHz  */
#define LL_RCC_MSIRANGE_1                  RCC_CR_MSIRANGE_1  /*!< MSI = 200 KHz  */
#define LL_RCC_MSIRANGE_2                  RCC_CR_MSIRANGE_2  /*!< MSI = 400 KHz  */
#define LL_RCC_MSIRANGE_3                  RCC_CR_MSIRANGE_3  /*!< MSI = 800 KHz  */
#define LL_RCC_MSIRANGE_4                  RCC_CR_MSIRANGE_4  /*!< MSI = 1 MHz    */
#define LL_RCC_MSIRANGE_5                  RCC_CR_MSIRANGE_5  /*!< MSI = 2 MHz    */
#define LL_RCC_MSIRANGE_6                  RCC_CR_MSIRANGE_6  /*!< MSI = 4 MHz    */
#define LL_RCC_MSIRANGE_7                  RCC_CR_MSIRANGE_7  /*!< MSI = 8 MHz    */
#define LL_RCC_MSIRANGE_8                  RCC_CR_MSIRANGE_8  /*!< MSI = 16 MHz   */
#define LL_RCC_MSIRANGE_9                  RCC_CR_MSIRANGE_9  /*!< MSI = 24 MHz   */
#define LL_RCC_MSIRANGE_10                 RCC_CR_MSIRANGE_10 /*!< MSI = 32 MHz   */
#define LL_RCC_MSIRANGE_11                 RCC_CR_MSIRANGE_11 /*!< MSI = 48 MHz   */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_MSISRANGE  MSI range after Standby mode
  * @{
  */
#define LL_RCC_MSISRANGE_4                 RCC_CSR_MSISRANGE_1  /*!< MSI = 1 MHz    */
#define LL_RCC_MSISRANGE_5                 RCC_CSR_MSISRANGE_2  /*!< MSI = 2 MHz    */
#define LL_RCC_MSISRANGE_6                 RCC_CSR_MSISRANGE_4  /*!< MSI = 4 MHz    */
#define LL_RCC_MSISRANGE_7                 RCC_CSR_MSISRANGE_8  /*!< MSI = 8 MHz    */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_LSCO_CLKSOURCE  LSCO Selection
  * @{
  */
#define LL_RCC_LSCO_CLKSOURCE_LSI          (uint32_t)0x00000000  /*!< LSI selection for low speed clock  */
#define LL_RCC_LSCO_CLKSOURCE_LSE          RCC_BDCR_LSCOSEL      /*!< LSE selection for low speed clock  */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_SYS_CLKSOURCE  System clock switch
  * @{
  */
#define LL_RCC_SYS_CLKSOURCE_MSI           RCC_CFGR_SW_MSI    /*!< MSI selection as system clock */
#define LL_RCC_SYS_CLKSOURCE_HSI           RCC_CFGR_SW_HSI    /*!< HSI selection as system clock */
#define LL_RCC_SYS_CLKSOURCE_HSE           RCC_CFGR_SW_HSE    /*!< HSE selection as system clock */
#define LL_RCC_SYS_CLKSOURCE_PLL           RCC_CFGR_SW_PLL    /*!< PLL selection as system clock */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_SYS_CLKSOURCE_STATUS  System clock switch status
  * @{
  */
#define LL_RCC_SYS_CLKSOURCE_STATUS_MSI    RCC_CFGR_SWS_MSI   /*!< MSI used as system clock */
#define LL_RCC_SYS_CLKSOURCE_STATUS_HSI    RCC_CFGR_SWS_HSI   /*!< HSI used as system clock */
#define LL_RCC_SYS_CLKSOURCE_STATUS_HSE    RCC_CFGR_SWS_HSE   /*!< HSE used as system clock */
#define LL_RCC_SYS_CLKSOURCE_STATUS_PLL    RCC_CFGR_SWS_PLL   /*!< PLL used as system clock */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_SYSCLK_DIV  AHB prescaler
  * @{
  */
#define LL_RCC_SYSCLK_DIV_1                RCC_CFGR_HPRE_DIV1   /*!< SYSCLK not divided */
#define LL_RCC_SYSCLK_DIV_2                RCC_CFGR_HPRE_DIV2   /*!< SYSCLK divided by 2 */
#define LL_RCC_SYSCLK_DIV_4                RCC_CFGR_HPRE_DIV4   /*!< SYSCLK divided by 4 */
#define LL_RCC_SYSCLK_DIV_8                RCC_CFGR_HPRE_DIV8   /*!< SYSCLK divided by 8 */
#define LL_RCC_SYSCLK_DIV_16               RCC_CFGR_HPRE_DIV16  /*!< SYSCLK divided by 16 */
#define LL_RCC_SYSCLK_DIV_64               RCC_CFGR_HPRE_DIV64  /*!< SYSCLK divided by 64 */
#define LL_RCC_SYSCLK_DIV_128              RCC_CFGR_HPRE_DIV128 /*!< SYSCLK divided by 128 */
#define LL_RCC_SYSCLK_DIV_256              RCC_CFGR_HPRE_DIV256 /*!< SYSCLK divided by 256 */
#define LL_RCC_SYSCLK_DIV_512              RCC_CFGR_HPRE_DIV512 /*!< SYSCLK divided by 512 */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_APB1_DIV  APB low-speed prescaler (APB1)
  * @{
  */
#define LL_RCC_APB1_DIV_1                  RCC_CFGR_PPRE1_DIV1  /*!< HCLK not divided */
#define LL_RCC_APB1_DIV_2                  RCC_CFGR_PPRE1_DIV2  /*!< HCLK divided by 2 */
#define LL_RCC_APB1_DIV_4                  RCC_CFGR_PPRE1_DIV4  /*!< HCLK divided by 4 */
#define LL_RCC_APB1_DIV_8                  RCC_CFGR_PPRE1_DIV8  /*!< HCLK divided by 8 */
#define LL_RCC_APB1_DIV_16                 RCC_CFGR_PPRE1_DIV16 /*!< HCLK divided by 16 */
/**
  * @}
  */
/** @defgroup RCC_LL_EC_APB2_DIV  APB high-speed prescaler (APB2)
  * @{
  */
#define LL_RCC_APB2_DIV_1                  RCC_CFGR_PPRE2_DIV1  /*!< HCLK not divided */
#define LL_RCC_APB2_DIV_2                  RCC_CFGR_PPRE2_DIV2  /*!< HCLK divided by 2 */
#define LL_RCC_APB2_DIV_4                  RCC_CFGR_PPRE2_DIV4  /*!< HCLK divided by 4 */
#define LL_RCC_APB2_DIV_8                  RCC_CFGR_PPRE2_DIV8  /*!< HCLK divided by 8 */
#define LL_RCC_APB2_DIV_16                 RCC_CFGR_PPRE2_DIV16 /*!< HCLK divided by 16 */
/**
  * @}
  */
/** @defgroup RCC_LL_EC_STOP_WAKEUPCLOCK  Wakeup from Stop and CSS backup clock selection
  * @{
  */
#define LL_RCC_STOP_WAKEUPCLOCK_MSI        ((uint32_t)0x00000000)  /*!< MSI selection after wake-up from STOP */
#define LL_RCC_STOP_WAKEUPCLOCK_HSI        RCC_CFGR_STOPWUCK       /*!< HSI selection after wake-up from STOP */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_MCO1SOURCE  MCO1 SOURCE selection
  * @{
  */
#define LL_RCC_MCO1SOURCE_NOCLOCK          ((uint32_t)0x00000000)                 /*!< MCO output disabled, no clock on MCO */
#define LL_RCC_MCO1SOURCE_SYSCLK           RCC_CFGR_MCOSEL_0                      /*!< SYSCLK selection as MCO1 source */
#define LL_RCC_MCO1SOURCE_MSI              RCC_CFGR_MCOSEL_1                      /*!< MSI selection as MCO1 source */
#define LL_RCC_MCO1SOURCE_HSI              (RCC_CFGR_MCOSEL_0| RCC_CFGR_MCOSEL_1) /*!< HSI16 selection as MCO1 source */
#define LL_RCC_MCO1SOURCE_HSE              RCC_CFGR_MCOSEL_2                      /*!< HSE selection as MCO1 source */
#define LL_RCC_MCO1SOURCE_PLLCLK           (RCC_CFGR_MCOSEL_0|RCC_CFGR_MCOSEL_2)  /*!< Main PLL selection as MCO1 source */
#define LL_RCC_MCO1SOURCE_LSI              (RCC_CFGR_MCOSEL_1|RCC_CFGR_MCOSEL_2)  /*!< LSI selection as MCO1 source */
#define LL_RCC_MCO1SOURCE_LSE              (RCC_CFGR_MCOSEL_0|RCC_CFGR_MCOSEL_1|RCC_CFGR_MCOSEL_2) /*!< LSE selection as MCO1 source */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_MCO1_DIV  MCO1 prescaler
  * @{
  */
#define LL_RCC_MCO1_DIV_1                  RCC_CFGR_MCOPRE_DIV1       /*!< MCO not divided */
#define LL_RCC_MCO1_DIV_2                  RCC_CFGR_MCOPRE_DIV2       /*!< MCO divided by 2 */
#define LL_RCC_MCO1_DIV_4                  RCC_CFGR_MCOPRE_DIV4       /*!< MCO divided by 4 */
#define LL_RCC_MCO1_DIV_8                  RCC_CFGR_MCOPRE_DIV8       /*!< MCO divided by 8 */
#define LL_RCC_MCO1_DIV_16                 RCC_CFGR_MCOPRE_DIV16      /*!< MCO divided by 16 */
/**
  * @}
  */

#if defined(USE_FULL_LL_DRIVER)
/** @defgroup RCC_LL_EC_PERIPH_FREQUENCY Peripheral clock frequency
  * @{
  */
#define LL_RCC_PERIPH_FREQUENCY_NO         (uint32_t)0x00000000       /*!< No clock enabled for the peripheral            */
#define LL_RCC_PERIPH_FREQUENCY_NA    (uint32_t)0xFFFFFFFFU       /*!< Frequency cannot be provided as external clock */
/**
  * @}
  */
#endif /* USE_FULL_LL_DRIVER */

/** @defgroup RCC_LL_EC_USART1_CLKSOURCE  Peripheral USART clock source selection
  * @{
  */
#define LL_RCC_USART1_CLKSOURCE_PCLK2      (uint32_t)((RCC_CCIPR_USART1SEL << 16) | 0x00000000)            /*!< PCLK2 clock used as USART1 clock source */
#define LL_RCC_USART1_CLKSOURCE_SYSCLK     (uint32_t)((RCC_CCIPR_USART1SEL << 16) | RCC_CCIPR_USART1SEL_0) /*!< SYSCLK clock used as USART1 clock source */
#define LL_RCC_USART1_CLKSOURCE_HSI        (uint32_t)((RCC_CCIPR_USART1SEL << 16) | RCC_CCIPR_USART1SEL_1) /*!< HSI clock used as USART1 clock source */
#define LL_RCC_USART1_CLKSOURCE_LSE        (uint32_t)((RCC_CCIPR_USART1SEL << 16) | RCC_CCIPR_USART1SEL)   /*!< LSE clock used as USART1 clock source */
#define LL_RCC_USART2_CLKSOURCE_PCLK1      (uint32_t)((RCC_CCIPR_USART2SEL << 16) | 0x00000000)            /*!< PCLK1 clock used as USART2 clock source */
#define LL_RCC_USART2_CLKSOURCE_SYSCLK     (uint32_t)((RCC_CCIPR_USART2SEL << 16) | RCC_CCIPR_USART2SEL_0) /*!< SYSCLK clock used as USART2 clock source */
#define LL_RCC_USART2_CLKSOURCE_HSI        (uint32_t)((RCC_CCIPR_USART2SEL << 16) | RCC_CCIPR_USART2SEL_1) /*!< HSI clock used as USART2 clock source */
#define LL_RCC_USART2_CLKSOURCE_LSE        (uint32_t)((RCC_CCIPR_USART2SEL << 16) | RCC_CCIPR_USART2SEL)   /*!< LSE clock used as USART2 clock source */
#define LL_RCC_USART3_CLKSOURCE_PCLK1      (uint32_t)((RCC_CCIPR_USART3SEL << 16) | 0x00000000)            /*!< PCLK1 clock used as USART3 clock source */
#define LL_RCC_USART3_CLKSOURCE_SYSCLK     (uint32_t)((RCC_CCIPR_USART3SEL << 16) | RCC_CCIPR_USART3SEL_0) /*!< SYSCLK clock used as USART3 clock source */
#define LL_RCC_USART3_CLKSOURCE_HSI        (uint32_t)((RCC_CCIPR_USART3SEL << 16) | RCC_CCIPR_USART3SEL_1) /*!< HSI clock used as USART3 clock source */
#define LL_RCC_USART3_CLKSOURCE_LSE        (uint32_t)((RCC_CCIPR_USART3SEL << 16) | RCC_CCIPR_USART3SEL)   /*!< LSE clock used as USART3 clock source */
/**
  * @}
  */

#if defined(UART4) || defined(UART5)
/** @defgroup RCC_LL_EC_UART4_CLKSOURCE  Peripheral UART clock source selection
  * @{
  */
#if defined(UART4)
#define LL_RCC_UART4_CLKSOURCE_PCLK1       (uint32_t)((RCC_CCIPR_UART4SEL << 16) | 0x00000000)            /*!< PCLK1 clock used as UART4 clock source */
#define LL_RCC_UART4_CLKSOURCE_SYSCLK      (uint32_t)((RCC_CCIPR_UART4SEL << 16) | RCC_CCIPR_UART4SEL_0)  /*!< SYSCLK clock used as UART4 clock source */
#define LL_RCC_UART4_CLKSOURCE_HSI         (uint32_t)((RCC_CCIPR_UART4SEL << 16) | RCC_CCIPR_UART4SEL_1)  /*!< HSI clock used as UART4 clock source */
#define LL_RCC_UART4_CLKSOURCE_LSE         (uint32_t)((RCC_CCIPR_UART4SEL << 16) | RCC_CCIPR_UART4SEL)    /*!< LSE clock used as UART4 clock source */
#endif /* UART4 */
#if defined(UART5)
#define LL_RCC_UART5_CLKSOURCE_PCLK1       (uint32_t)((RCC_CCIPR_UART5SEL << 16) | 0x00000000)            /*!< PCLK1 clock used as UART5 clock source */
#define LL_RCC_UART5_CLKSOURCE_SYSCLK      (uint32_t)((RCC_CCIPR_UART5SEL << 16) | RCC_CCIPR_UART5SEL_0)  /*!< SYSCLK clock used as UART5 clock source */
#define LL_RCC_UART5_CLKSOURCE_HSI         (uint32_t)((RCC_CCIPR_UART5SEL << 16) | RCC_CCIPR_UART5SEL_1)  /*!< HSI clock used as UART5 clock source */
#define LL_RCC_UART5_CLKSOURCE_LSE         (uint32_t)((RCC_CCIPR_UART5SEL << 16) | RCC_CCIPR_UART5SEL)    /*!< LSE clock used as UART5 clock source */
#endif /* UART5 */
/**
  * @}
  */
#endif /* UART4 || UART5 */

/** @defgroup RCC_LL_EC_LPUART1_CLKSOURCE  Peripheral LPUART clock source selection
  * @{
  */
#define LL_RCC_LPUART1_CLKSOURCE_PCLK1     (uint32_t)0x00000000            /*!< PCLK1 clock used as LPUART1 clock source */
#define LL_RCC_LPUART1_CLKSOURCE_SYSCLK    RCC_CCIPR_LPUART1SEL_0          /*!< SYSCLK clock used as LPUART1 clock source */
#define LL_RCC_LPUART1_CLKSOURCE_HSI       RCC_CCIPR_LPUART1SEL_1          /*!< HSI clock used as LPUART1 clock source */
#define LL_RCC_LPUART1_CLKSOURCE_LSE       RCC_CCIPR_LPUART1SEL            /*!< LSE clock used as LPUART1 clock source */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_I2C1_CLKSOURCE  Peripheral I2C clock source selection
  * @{
  */
#define LL_RCC_I2C1_CLKSOURCE_PCLK1        (uint32_t)((RCC_CCIPR_I2C1SEL << 4) | (0x00000000 >> 4))          /*!< PCLK1 clock used as I2C1 clock source */
#define LL_RCC_I2C1_CLKSOURCE_SYSCLK       (uint32_t)((RCC_CCIPR_I2C1SEL << 4) | (RCC_CCIPR_I2C1SEL_0 >> 4)) /*!< SYSCLK clock used as I2C1 clock source */
#define LL_RCC_I2C1_CLKSOURCE_HSI          (uint32_t)((RCC_CCIPR_I2C1SEL << 4) | (RCC_CCIPR_I2C1SEL_1 >> 4)) /*!< HSI clock used as I2C1 clock source */
#define LL_RCC_I2C2_CLKSOURCE_PCLK1        (uint32_t)((RCC_CCIPR_I2C2SEL << 4) | (0x00000000 >> 4))          /*!< PCLK1 clock used as I2C2 clock source */
#define LL_RCC_I2C2_CLKSOURCE_SYSCLK       (uint32_t)((RCC_CCIPR_I2C2SEL << 4) | (RCC_CCIPR_I2C2SEL_0 >> 4)) /*!< SYSCLK clock used as I2C2 clock source */
#define LL_RCC_I2C2_CLKSOURCE_HSI          (uint32_t)((RCC_CCIPR_I2C2SEL << 4) | (RCC_CCIPR_I2C2SEL_1 >> 4)) /*!< HSI clock used as I2C2 clock source */
#define LL_RCC_I2C3_CLKSOURCE_PCLK1        (uint32_t)((RCC_CCIPR_I2C3SEL << 4) | (0x00000000 >> 4))          /*!< PCLK1 clock used as I2C3 clock source */
#define LL_RCC_I2C3_CLKSOURCE_SYSCLK       (uint32_t)((RCC_CCIPR_I2C3SEL << 4) | (RCC_CCIPR_I2C3SEL_0 >> 4)) /*!< SYSCLK clock used as I2C3 clock source */
#define LL_RCC_I2C3_CLKSOURCE_HSI          (uint32_t)((RCC_CCIPR_I2C3SEL << 4) | (RCC_CCIPR_I2C3SEL_1 >> 4)) /*!< HSI clock used as I2C3 clock source */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_LPTIM1_CLKSOURCE  Peripheral LPTIM clock source selection
  * @{
  */
#define LL_RCC_LPTIM1_CLKSOURCE_PCLK1      (uint32_t)(RCC_CCIPR_LPTIM1SEL | (0x00000000 >> 16))            /*!< PCLK1 clock used as LPTIM1 clock source */
#define LL_RCC_LPTIM1_CLKSOURCE_LSI        (uint32_t)(RCC_CCIPR_LPTIM1SEL | (RCC_CCIPR_LPTIM1SEL_0 >> 16)) /*!< LSI clock used as LPTIM1 clock source */
#define LL_RCC_LPTIM1_CLKSOURCE_HSI        (uint32_t)(RCC_CCIPR_LPTIM1SEL | (RCC_CCIPR_LPTIM1SEL_1 >> 16)) /*!< HSI clock used as LPTIM1 clock source */
#define LL_RCC_LPTIM1_CLKSOURCE_LSE        (uint32_t)(RCC_CCIPR_LPTIM1SEL | (RCC_CCIPR_LPTIM1SEL >> 16))   /*!< LSE clock used as LPTIM1 clock source */
#define LL_RCC_LPTIM2_CLKSOURCE_PCLK1      (uint32_t)(RCC_CCIPR_LPTIM2SEL | (0x00000000 >> 16))            /*!< PCLK1 clock used as LPTIM2 clock source */
#define LL_RCC_LPTIM2_CLKSOURCE_LSI        (uint32_t)(RCC_CCIPR_LPTIM2SEL | (RCC_CCIPR_LPTIM2SEL_0 >> 16)) /*!< LSI clock used as LPTIM2 clock source */
#define LL_RCC_LPTIM2_CLKSOURCE_HSI        (uint32_t)(RCC_CCIPR_LPTIM2SEL | (RCC_CCIPR_LPTIM2SEL_1 >> 16)) /*!< HSI clock used as LPTIM2 clock source */
#define LL_RCC_LPTIM2_CLKSOURCE_LSE        (uint32_t)(RCC_CCIPR_LPTIM2SEL | (RCC_CCIPR_LPTIM2SEL >> 16))   /*!< LSE clock used as LPTIM2 clock source */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_SAI1_CLKSOURCE  Peripheral SAI clock source selection
  * @{
  */
#define LL_RCC_SAI1_CLKSOURCE_PLLSAI1      (uint32_t)(RCC_CCIPR_SAI1SEL | (0x00000000 >> 16))          /*!< PLLSAI1 clock used as SAI1 clock source */
#if defined(RCC_PLLSAI2_SUPPORT)
#define LL_RCC_SAI1_CLKSOURCE_PLLSAI2      (uint32_t)(RCC_CCIPR_SAI1SEL | (RCC_CCIPR_SAI1SEL_0 >> 16)) /*!< PLLSAI2 clock used as SAI1 clock source */
#endif /* RCC_PLLSAI2_SUPPORT */
#define LL_RCC_SAI1_CLKSOURCE_PLL          (uint32_t)(RCC_CCIPR_SAI1SEL | (RCC_CCIPR_SAI1SEL_1 >> 16)) /*!< PLL clock used as SAI1 clock source */
#define LL_RCC_SAI1_CLKSOURCE_PIN          (uint32_t)(RCC_CCIPR_SAI1SEL | (RCC_CCIPR_SAI1SEL >> 16))   /*!< External input clock used as SAI1 clock source */
#if defined(SAI2)
#define LL_RCC_SAI2_CLKSOURCE_PLLSAI1      (uint32_t)(RCC_CCIPR_SAI2SEL | (0x00000000 >> 16))          /*!< PLLSAI1 clock used as SAI2 clock source */
#if defined(RCC_PLLSAI2_SUPPORT)
#define LL_RCC_SAI2_CLKSOURCE_PLLSAI2      (uint32_t)(RCC_CCIPR_SAI2SEL | (RCC_CCIPR_SAI2SEL_0 >> 16)) /*!< PLLSAI2 clock used as SAI2 clock source */
#endif /* RCC_PLLSAI2_SUPPORT */
#define LL_RCC_SAI2_CLKSOURCE_PLL          (uint32_t)(RCC_CCIPR_SAI2SEL | (RCC_CCIPR_SAI2SEL_1 >> 16)) /*!< PLL clock used as SAI2 clock source */
#define LL_RCC_SAI2_CLKSOURCE_PIN          (uint32_t)(RCC_CCIPR_SAI2SEL | (RCC_CCIPR_SAI2SEL >> 16))   /*!< External input clock used as SAI2 clock source */
#endif /* SAI2 */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_SDMMC1_CLKSOURCE  Peripheral SDMMC clock source selection
  * @{
  */
#define LL_RCC_SDMMC1_CLKSOURCE_NONE       (uint32_t)(0x00000000)           /*!< No clock used as SDMMC1 clock source */
#define LL_RCC_SDMMC1_CLKSOURCE_PLLSAI1    (uint32_t)(RCC_CCIPR_CLK48SEL_0) /*!< PLLSAI1 clock used as SDMMC1 clock source */
#define LL_RCC_SDMMC1_CLKSOURCE_PLL        (uint32_t)(RCC_CCIPR_CLK48SEL_1) /*!< PLL clock used as SDMMC1 clock source */
#define LL_RCC_SDMMC1_CLKSOURCE_MSI        (uint32_t)(RCC_CCIPR_CLK48SEL)   /*!< MSI clock used as SDMMC1 clock source */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_RNG_CLKSOURCE  Peripheral RNG clock source selection
  * @{
  */
#define LL_RCC_RNG_CLKSOURCE_NONE          (uint32_t)(0x00000000)           /*!< No clock used as RNG clock source */
#define LL_RCC_RNG_CLKSOURCE_PLLSAI1       (uint32_t)(RCC_CCIPR_CLK48SEL_0) /*!< PLLSAI1 clock used as RNG clock source */
#define LL_RCC_RNG_CLKSOURCE_PLL           (uint32_t)(RCC_CCIPR_CLK48SEL_1) /*!< PLL clock used as RNG clock source */
#define LL_RCC_RNG_CLKSOURCE_MSI           (uint32_t)(RCC_CCIPR_CLK48SEL)   /*!< MSI clock used as RNG clock source */
/**
  * @}
  */

#if defined(USB_OTG_FS)
/** @defgroup RCC_LL_EC_USB_CLKSOURCE  Peripheral USB clock source selection
  * @{
  */
#define LL_RCC_USB_CLKSOURCE_NONE          (uint32_t)(0x00000000)           /*!< No clock used as USB clock source */
#define LL_RCC_USB_CLKSOURCE_PLLSAI1       (uint32_t)(RCC_CCIPR_CLK48SEL_0) /*!< PLLSAI1 clock used as USB clock source */
#define LL_RCC_USB_CLKSOURCE_PLL           (uint32_t)(RCC_CCIPR_CLK48SEL_1) /*!< PLL clock used as USB clock source */
#define LL_RCC_USB_CLKSOURCE_MSI           (uint32_t)(RCC_CCIPR_CLK48SEL)   /*!< MSI clock used as USB clock source */
/**
  * @}
  */

#endif /* USB_OTG_FS */

/** @defgroup RCC_LL_EC_ADC_CLKSOURCE  Peripheral ADC clock source selection
  * @{
  */
#define LL_RCC_ADC_CLKSOURCE_NONE          (uint32_t)(0x00000000)           /*!< No clock used as ADC clock source */
#define LL_RCC_ADC_CLKSOURCE_PLLSAI1       (uint32_t)(RCC_CCIPR_ADCSEL_0)   /*!< PLLSAI1 clock used as ADC clock source */
#if defined(RCC_PLLSAI2_SUPPORT)
#define LL_RCC_ADC_CLKSOURCE_PLLSAI2       (uint32_t)(RCC_CCIPR_ADCSEL_1)   /*!< PLLSAI2 clock used as ADC clock source */
#endif /* RCC_PLLSAI2_SUPPORT */
#define LL_RCC_ADC_CLKSOURCE_SYSCLK        (uint32_t)(RCC_CCIPR_ADCSEL)     /*!< SYSCLK clock used as ADC clock source */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_SWPMI1_CLKSOURCE  Peripheral SWPMI clock source selection
  * @{
  */
#define LL_RCC_SWPMI1_CLKSOURCE_PCLK       (uint32_t)(0x00000000)           /*!< PCLK used as SWPMI1 clock source */
#define LL_RCC_SWPMI1_CLKSOURCE_HSI        (uint32_t)(RCC_CCIPR_SWPMI1SEL)  /*!< HSI used as SWPMI1 clock source */
/**
  * @}
  */

#if defined(DFSDM_Channel0)
/** @defgroup RCC_LL_EC_DFSDM_CLKSOURCE  Peripheral DFSDM clock source selection
  * @{
  */
#define LL_RCC_DFSDM_CLKSOURCE_PCLK        (uint32_t)(0x00000000)           /*!< PCLK used as DFSDM clock source */
#define LL_RCC_DFSDM_CLKSOURCE_SYSCLK      (uint32_t)(RCC_CCIPR_DFSDMSEL)   /*!< SYSCLK used as DFSDM clock source */
/**
  * @}
  */
#endif /* DFSDM_Channel0 */

/** @defgroup RCC_LL_EC_USART1 Peripheral USART get clock source
  * @{
  */
#define LL_RCC_USART1_CLKSOURCE            RCC_CCIPR_USART1SEL /*!< USART1 Clock source selection */
#define LL_RCC_USART2_CLKSOURCE            RCC_CCIPR_USART2SEL /*!< USART2 Clock source selection */
#define LL_RCC_USART3_CLKSOURCE            RCC_CCIPR_USART3SEL /*!< USART3 Clock source selection */
/**
  * @}
  */

#if defined(UART4) || defined(UART5)
/** @defgroup RCC_LL_EC_UART4 Peripheral UART get clock source
  * @{
  */
#if defined(UART4)
#define LL_RCC_UART4_CLKSOURCE             RCC_CCIPR_UART4SEL /*!< UART4 Clock source selection */
#endif /* UART4 */
#if defined(UART5)
#define LL_RCC_UART5_CLKSOURCE             RCC_CCIPR_UART5SEL /*!< UART5 Clock source selection */
#endif /* UART5 */
/**
  * @}
  */
#endif /* UART4 || UART5 */

/** @defgroup RCC_LL_EC_LPUART1 Peripheral LPUART get clock source
  * @{
  */
#define LL_RCC_LPUART1_CLKSOURCE           RCC_CCIPR_LPUART1SEL /*!< LPUART1 Clock source selection */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_I2C1 Peripheral I2C get clock source
  * @{
  */
#define LL_RCC_I2C1_CLKSOURCE              RCC_CCIPR_I2C1SEL /*!< I2C1 Clock source selection */
#define LL_RCC_I2C2_CLKSOURCE              RCC_CCIPR_I2C2SEL /*!< I2C2 Clock source selection */
#define LL_RCC_I2C3_CLKSOURCE              RCC_CCIPR_I2C3SEL /*!< I2C3 Clock source selection */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_LPTIM1 Peripheral LPTIM get clock source
  * @{
  */
#define LL_RCC_LPTIM1_CLKSOURCE            RCC_CCIPR_LPTIM1SEL /*!< LPTIM1 Clock source selection */
#define LL_RCC_LPTIM2_CLKSOURCE            RCC_CCIPR_LPTIM2SEL /*!< LPTIM2 Clock source selection */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_SAI1  Peripheral SAI get clock source
  * @{
  */
#define LL_RCC_SAI1_CLKSOURCE              RCC_CCIPR_SAI1SEL /*!< SAI1 Clock source selection */
#if defined(SAI2)
#define LL_RCC_SAI2_CLKSOURCE              RCC_CCIPR_SAI2SEL /*!< SAI2 Clock source selection */
#endif /* SAI2 */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_SDMMC1  Peripheral SDMMC get clock source
  * @{
  */
#define LL_RCC_SDMMC1_CLKSOURCE            RCC_CCIPR_CLK48SEL /*!< SDMMC1 Clock source selection */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_RNG  Peripheral RNG get clock source
  * @{
  */
#define LL_RCC_RNG_CLKSOURCE               RCC_CCIPR_CLK48SEL /*!< RNG Clock source selection */
/**
  * @}
  */

#if defined(USB_OTG_FS)
/** @defgroup RCC_LL_EC_USB  Peripheral USB get clock source
  * @{
  */
#define LL_RCC_USB_CLKSOURCE               RCC_CCIPR_CLK48SEL /*!< USB Clock source selection */
/**
  * @}
  */

#endif /* USB_OTG_FS */

/** @defgroup RCC_LL_EC_ADC  Peripheral ADC get clock source
  * @{
  */
#define LL_RCC_ADC_CLKSOURCE               RCC_CCIPR_ADCSEL /*!< ADC Clock source selection */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_SWPMI1  Peripheral SWPMI get clock source
  * @{
  */
#define LL_RCC_SWPMI1_CLKSOURCE            RCC_CCIPR_SWPMI1SEL /*!< SWPMI1 Clock source selection */
/**
  * @}
  */

#if defined(DFSDM_Channel0)
/** @defgroup RCC_LL_EC_DFSDM  Peripheral DFSDM get clock source
  * @{
  */
#define LL_RCC_DFSDM_CLKSOURCE             RCC_CCIPR_DFSDMSEL /*!< DFSDM Clock source selection */
/**
  * @}
  */
#endif /* DFSDM_Channel0 */

/** @defgroup RCC_LL_EC_RTC_CLKSOURCE  RTC clock source selection
  * @{
  */
#define LL_RCC_RTC_CLKSOURCE_NONE          (uint32_t)(0x00000000)        /*!< No clock used as RTC clock */
#define LL_RCC_RTC_CLKSOURCE_LSE           RCC_BDCR_RTCSEL_0       /*!< LSE oscillator clock used as RTC clock */
#define LL_RCC_RTC_CLKSOURCE_LSI           RCC_BDCR_RTCSEL_1       /*!< LSI oscillator clock used as RTC clock */
#define LL_RCC_RTC_CLKSOURCE_HSE_DIV32     RCC_BDCR_RTCSEL         /*!< HSE oscillator clock divided by 32 used as RTC clock */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_PLLSOURCE  PLL, PLLSAI1 and PLLSAI2 entry clock source
  * @{
  */
#define LL_RCC_PLLSOURCE_NONE              (uint32_t)0x00000000    /*!< No clock */
#define LL_RCC_PLLSOURCE_MSI               RCC_PLLCFGR_PLLSRC_MSI  /*!< MSI clock selected as PLL entry clock source */
#define LL_RCC_PLLSOURCE_HSI               RCC_PLLCFGR_PLLSRC_HSI  /*!< HSI16 clock selected as PLL entry clock source */
#define LL_RCC_PLLSOURCE_HSE               RCC_PLLCFGR_PLLSRC_HSE  /*!< HSE clock selected as PLL entry clock source */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_PLLM_DIV  PLL, PLLSAI1 and PLLSAI2 division factor
  * @{
  */
#define LL_RCC_PLLM_DIV_1                  ((uint32_t)0x00000000)                      /*!< PLL, PLLSAI1 and PLLSAI2 division factor by 1 */
#define LL_RCC_PLLM_DIV_2                  (RCC_PLLCFGR_PLLM_0)                        /*!< PLL, PLLSAI1 and PLLSAI2 division factor by 2 */
#define LL_RCC_PLLM_DIV_3                  (RCC_PLLCFGR_PLLM_1)                        /*!< PLL, PLLSAI1 and PLLSAI2 division factor by 3 */
#define LL_RCC_PLLM_DIV_4                  ((RCC_PLLCFGR_PLLM_1 | RCC_PLLCFGR_PLLM_0)) /*!< PLL, PLLSAI1 and PLLSAI2 division factor by 4 */
#define LL_RCC_PLLM_DIV_5                  (RCC_PLLCFGR_PLLM_2)                        /*!< PLL, PLLSAI1 and PLLSAI2 division factor by 5 */
#define LL_RCC_PLLM_DIV_6                  ((RCC_PLLCFGR_PLLM_2 | RCC_PLLCFGR_PLLM_0)) /*!< PLL, PLLSAI1 and PLLSAI2 division factor by 6 */
#define LL_RCC_PLLM_DIV_7                  ((RCC_PLLCFGR_PLLM_2 | RCC_PLLCFGR_PLLM_1)) /*!< PLL, PLLSAI1 and PLLSAI2 division factor by 7 */
#define LL_RCC_PLLM_DIV_8                  (RCC_PLLCFGR_PLLM)                          /*!< PLL, PLLSAI1 and PLLSAI2 division factor by 8 */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_PLLR_DIV  PLL division factor (PLLR)
  * @{
  */
#define LL_RCC_PLLR_DIV_2                  ((uint32_t)0x00000000) /*!< Main PLL division factor for PLLCLK (system clock) by 2 */
#define LL_RCC_PLLR_DIV_4                  (RCC_PLLCFGR_PLLR_0)   /*!< Main PLL division factor for PLLCLK (system clock) by 4 */
#define LL_RCC_PLLR_DIV_6                  (RCC_PLLCFGR_PLLR_1)   /*!< Main PLL division factor for PLLCLK (system clock) by 6 */
#define LL_RCC_PLLR_DIV_8                  (RCC_PLLCFGR_PLLR)     /*!< Main PLL division factor for PLLCLK (system clock) by 8 */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_PLLP_DIV  PLL division factor (PLLP)
  * @{
  */
#define LL_RCC_PLLP_DIV_7                  ((uint32_t)0x00000000) /*!< Main PLL division factor for PLLP output by 7 */
#define LL_RCC_PLLP_DIV_17                 (RCC_PLLCFGR_PLLP)     /*!< Main PLL division factor for PLLP output by 17 */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_PLLQ_DIV  PLL division factor (PLLQ)
  * @{
  */
#define LL_RCC_PLLQ_DIV_2                  ((uint32_t)0x00000000)  /*!< Main PLL division factor for PLLQ output by 2 */
#define LL_RCC_PLLQ_DIV_4                  (RCC_PLLCFGR_PLLQ_0)    /*!< Main PLL division factor for PLLQ output by 4 */
#define LL_RCC_PLLQ_DIV_6                  (RCC_PLLCFGR_PLLQ_1)    /*!< Main PLL division factor for PLLQ output by 6 */
#define LL_RCC_PLLQ_DIV_8                  (RCC_PLLCFGR_PLLQ)      /*!< Main PLL division factor for PLLQ output by 8 */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_PLLSAI1Q  PLLSAI1 division factor (PLLSAI1Q)
  * @{
  */
#define LL_RCC_PLLSAI1Q_DIV_2              ((uint32_t)0x00000000)       /*!< PLLSAI1 division factor for PLLSAI1Q output by 2 */
#define LL_RCC_PLLSAI1Q_DIV_4              (RCC_PLLSAI1CFGR_PLLSAI1Q_0) /*!< PLLSAI1 division factor for PLLSAI1Q output by 4 */
#define LL_RCC_PLLSAI1Q_DIV_6              (RCC_PLLSAI1CFGR_PLLSAI1Q_1) /*!< PLLSAI1 division factor for PLLSAI1Q output by 6 */
#define LL_RCC_PLLSAI1Q_DIV_8              (RCC_PLLSAI1CFGR_PLLSAI1Q)   /*!< PLLSAI1 division factor for PLLSAI1Q output by 8 */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_PLLSAI1P  PLLSAI1 division factor (PLLSAI1P)
  * @{
  */
#define LL_RCC_PLLSAI1P_DIV_7              ((uint32_t)0x00000000)     /*!< PLLSAI1 division factor for PLLSAI1P output by 7 */
#define LL_RCC_PLLSAI1P_DIV_17             (RCC_PLLSAI1CFGR_PLLSAI1P) /*!< PLLSAI1 division factor for PLLSAI1P output by 17 */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_PLLSAI1R  PLLSAI1 division factor (PLLSAI1R)
  * @{
  */
#define LL_RCC_PLLSAI1R_DIV_2              ((uint32_t)0x00000000)       /*!< PLLSAI1 division factor for PLLSAI1R output by 2 */
#define LL_RCC_PLLSAI1R_DIV_4              (RCC_PLLSAI1CFGR_PLLSAI1R_0) /*!< PLLSAI1 division factor for PLLSAI1R output by 4 */
#define LL_RCC_PLLSAI1R_DIV_6              (RCC_PLLSAI1CFGR_PLLSAI1R_1) /*!< PLLSAI1 division factor for PLLSAI1R output by 6 */
#define LL_RCC_PLLSAI1R_DIV_8              (RCC_PLLSAI1CFGR_PLLSAI1R)   /*!< PLLSAI1 division factor for PLLSAI1R output by 8 */
/**
  * @}
  */

#if defined(RCC_PLLSAI2_SUPPORT)
/** @defgroup RCC_LL_EC_PLLSAI2P  PLLSAI2 division factor (PLLSAI2P)
  * @{
  */
#define LL_RCC_PLLSAI2P_DIV_7              ((uint32_t)0x00000000)     /*!< PLLSAI2 division factor for PLLSAI2P output by 7 */
#define LL_RCC_PLLSAI2P_DIV_17             (RCC_PLLSAI2CFGR_PLLSAI2P) /*!< PLLSAI2 division factor for PLLSAI2P output by 17 */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_PLLSAI2R  PLLSAI2 division factor (PLLSAI2R)
  * @{
  */
#define LL_RCC_PLLSAI2R_DIV_2              ((uint32_t)0x00000000)       /*!< PLLSAI2 division factor for PLLSAI2R output by 2 */
#define LL_RCC_PLLSAI2R_DIV_4              (RCC_PLLSAI2CFGR_PLLSAI2R_0) /*!< PLLSAI2 division factor for PLLSAI2R output by 4 */
#define LL_RCC_PLLSAI2R_DIV_6              (RCC_PLLSAI2CFGR_PLLSAI2R_1) /*!< PLLSAI2 division factor for PLLSAI2R output by 6 */
#define LL_RCC_PLLSAI2R_DIV_8              (RCC_PLLSAI2CFGR_PLLSAI2R)   /*!< PLLSAI2 division factor for PLLSAI2R output by 8 */
/**
  * @}
  */
#endif /* RCC_PLLSAI2_SUPPORT */

/** @defgroup RCC_LL_EC_MSIRANGESEL  MSI clock range selection
  * @{
  */
#define LL_RCC_MSIRANGESEL_STANDBY         (uint32_t)0                  /*!< MSI Range is provided by MSISRANGE */
#define LL_RCC_MSIRANGESEL_RUN             (uint32_t)1                  /*!< MSI Range is provided by MSIRANGE */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup RCC_LL_Exported_Macros RCC Exported Macros
  * @{
  */

/** @defgroup RCC_LL_EM_WRITE_READ Common Write and read registers Macros
  * @{
  */

/**
  * @brief  Write a value in RCC register
  * @param  __REG__ Register to be written
  * @param  __VALUE__ Value to be written in the register
  * @retval None
  */
#define LL_RCC_WriteReg(__REG__, __VALUE__) WRITE_REG(RCC->__REG__, (__VALUE__))

/**
  * @brief  Read a value in RCC register
  * @param  __REG__ Register to be read
  * @retval Register value
  */
#define LL_RCC_ReadReg(__REG__) READ_REG(RCC->__REG__)
/**
  * @}
  */

/** @defgroup RCC_LL_EM_CALC_FREQ Calculate frequencies
  * @{
  */

/**
  * @brief  Helper macro to calculate the PLLCLK frequency on system domain
  * @note ex: @ref __LL_RCC_CALC_PLLCLK_FREQ (HSE_VALUE,@ref LL_RCC_PLL_GetDivider (),
  *             @ref LL_RCC_PLL_GetN (), @ref LL_RCC_PLL_GetR ());
  * @param  __INPUTFREQ__ PLL Input frequency (based on MSI/HSE/HSI)
  * @param  __PLLM__ This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLM_DIV_1
  *         @arg @ref LL_RCC_PLLM_DIV_2
  *         @arg @ref LL_RCC_PLLM_DIV_3
  *         @arg @ref LL_RCC_PLLM_DIV_4
  *         @arg @ref LL_RCC_PLLM_DIV_5
  *         @arg @ref LL_RCC_PLLM_DIV_6
  *         @arg @ref LL_RCC_PLLM_DIV_7
  *         @arg @ref LL_RCC_PLLM_DIV_8
  * @param  __PLLN__ Between 8 and 86
  * @param  __PLLR__ This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLR_DIV_2
  *         @arg @ref LL_RCC_PLLR_DIV_4
  *         @arg @ref LL_RCC_PLLR_DIV_6
  *         @arg @ref LL_RCC_PLLR_DIV_8
  * @retval PLL clock frequency (in Hz)
  */
#define __LL_RCC_CALC_PLLCLK_FREQ(__INPUTFREQ__, __PLLM__, __PLLN__, __PLLR__) ((__INPUTFREQ__) / ((((__PLLM__)>> RCC_POSITION_PLLM) + 1)) * (__PLLN__) / \
                   ((((__PLLR__) >> RCC_POSITION_PLLR ) + 1 ) * 2))

/**
  * @brief  Helper macro to calculate the PLLCLK frequency used on SAI domain
  * @note ex: @ref __LL_RCC_CALC_PLLCLK_SAI_FREQ (HSE_VALUE,@ref LL_RCC_PLL_GetDivider (),
  *             @ref LL_RCC_PLL_GetN (), @ref LL_RCC_PLL_GetP ());
  * @param  __INPUTFREQ__ PLL Input frequency (based on MSI/HSE/HSI)
  * @param  __PLLM__ This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLM_DIV_1
  *         @arg @ref LL_RCC_PLLM_DIV_2
  *         @arg @ref LL_RCC_PLLM_DIV_3
  *         @arg @ref LL_RCC_PLLM_DIV_4
  *         @arg @ref LL_RCC_PLLM_DIV_5
  *         @arg @ref LL_RCC_PLLM_DIV_6
  *         @arg @ref LL_RCC_PLLM_DIV_7
  *         @arg @ref LL_RCC_PLLM_DIV_8
  * @param  __PLLN__ Between 8 and 86
  * @param  __PLLP__ This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLP_DIV_7
  *         @arg @ref LL_RCC_PLLP_DIV_17
  * @retval PLL clock frequency (in Hz)
  */
#define __LL_RCC_CALC_PLLCLK_SAI_FREQ(__INPUTFREQ__, __PLLM__, __PLLN__, __PLLP__) ((__INPUTFREQ__) / ((((__PLLM__)>> RCC_POSITION_PLLM) + 1)) * (__PLLN__) / \
                   (((__PLLP__) == LL_RCC_PLLP_DIV_7) ? 7 : 17))

/**
  * @brief  Helper macro to calculate the PLLCLK frequency used on 48M domain
  * @note ex: @ref __LL_RCC_CALC_PLLCLK_48M_FREQ (HSE_VALUE,@ref LL_RCC_PLL_GetDivider (),
  *             @ref LL_RCC_PLL_GetN (), @ref LL_RCC_PLL_GetQ ());
  * @param  __INPUTFREQ__ PLL Input frequency (based on MSI/HSE/HSI)
  * @param  __PLLM__ This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLM_DIV_1
  *         @arg @ref LL_RCC_PLLM_DIV_2
  *         @arg @ref LL_RCC_PLLM_DIV_3
  *         @arg @ref LL_RCC_PLLM_DIV_4
  *         @arg @ref LL_RCC_PLLM_DIV_5
  *         @arg @ref LL_RCC_PLLM_DIV_6
  *         @arg @ref LL_RCC_PLLM_DIV_7
  *         @arg @ref LL_RCC_PLLM_DIV_8
  * @param  __PLLN__ Between 8 and 86
  * @param  __PLLQ__ This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLQ_DIV_2
  *         @arg @ref LL_RCC_PLLQ_DIV_4
  *         @arg @ref LL_RCC_PLLQ_DIV_6
  *         @arg @ref LL_RCC_PLLQ_DIV_8
  * @retval PLL clock frequency (in Hz)
  */
#define __LL_RCC_CALC_PLLCLK_48M_FREQ(__INPUTFREQ__, __PLLM__, __PLLN__, __PLLQ__) ((__INPUTFREQ__) / ((((__PLLM__)>> RCC_POSITION_PLLM) + 1)) * (__PLLN__) / \
                   ((((__PLLQ__) >> RCC_POSITION_PLLQ) + 1) << 1))

/**
  * @brief  Helper macro to calculate the PLLSAI1 frequency used for SAI domain
  * @note ex: @ref __LL_RCC_CALC_PLLSAI1_SAI_FREQ (HSE_VALUE,@ref LL_RCC_PLL_GetDivider (),
  *             @ref LL_RCC_PLLSAI1_GetN (), @ref LL_RCC_PLLSAI1_GetP ());
  * @param  __INPUTFREQ__ PLL Input frequency (based on MSI/HSE/HSI)
  * @param  __PLLM__ This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLM_DIV_1
  *         @arg @ref LL_RCC_PLLM_DIV_2
  *         @arg @ref LL_RCC_PLLM_DIV_3
  *         @arg @ref LL_RCC_PLLM_DIV_4
  *         @arg @ref LL_RCC_PLLM_DIV_5
  *         @arg @ref LL_RCC_PLLM_DIV_6
  *         @arg @ref LL_RCC_PLLM_DIV_7
  *         @arg @ref LL_RCC_PLLM_DIV_8
  * @param  __PLLSAI1N__ Between 8 and 86
  * @param  __PLLSAI1P__ This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_7
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_17
  * @retval PLLSAI1 clock frequency (in Hz)
  */
#define __LL_RCC_CALC_PLLSAI1_SAI_FREQ(__INPUTFREQ__, __PLLM__, __PLLSAI1N__, __PLLSAI1P__) ((__INPUTFREQ__) / ((((__PLLM__)>> RCC_POSITION_PLLM) + 1)) * (__PLLSAI1N__) / \
                   (((__PLLSAI1P__) == LL_RCC_PLLSAI1P_DIV_7) ? 7 : 17))

/**
  * @brief  Helper macro to calculate the PLLSAI1 frequency used on 48M domain
  * @note ex: @ref __LL_RCC_CALC_PLLSAI1_48M_FREQ (HSE_VALUE,@ref LL_RCC_PLL_GetDivider (),
  *             @ref LL_RCC_PLLSAI1_GetN (), @ref LL_RCC_PLLSAI1_GetQ ());
  * @param  __INPUTFREQ__ PLL Input frequency (based on MSI/HSE/HSI)
  * @param  __PLLM__ This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLM_DIV_1
  *         @arg @ref LL_RCC_PLLM_DIV_2
  *         @arg @ref LL_RCC_PLLM_DIV_3
  *         @arg @ref LL_RCC_PLLM_DIV_4
  *         @arg @ref LL_RCC_PLLM_DIV_5
  *         @arg @ref LL_RCC_PLLM_DIV_6
  *         @arg @ref LL_RCC_PLLM_DIV_7
  *         @arg @ref LL_RCC_PLLM_DIV_8
  * @param  __PLLSAI1N__ Between 8 and 86
  * @param  __PLLSAI1Q__ This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLSAI1Q_DIV_2
  *         @arg @ref LL_RCC_PLLSAI1Q_DIV_4
  *         @arg @ref LL_RCC_PLLSAI1Q_DIV_6
  *         @arg @ref LL_RCC_PLLSAI1Q_DIV_8
  * @retval PLLSAI1 clock frequency (in Hz)
  */
#define __LL_RCC_CALC_PLLSAI1_48M_FREQ(__INPUTFREQ__, __PLLM__, __PLLSAI1N__, __PLLSAI1Q__) ((__INPUTFREQ__) / ((((__PLLM__)>> RCC_POSITION_PLLM) + 1)) * (__PLLSAI1N__) / \
                   ((((__PLLSAI1Q__) >> RCC_POSITION_PLLSAI1Q) + 1) << 1))

/**
  * @brief  Helper macro to calculate the PLLSAI1 frequency used on ADC domain
  * @note ex: @ref __LL_RCC_CALC_PLLSAI1_ADC_FREQ (HSE_VALUE,@ref LL_RCC_PLL_GetDivider (),
  *             @ref LL_RCC_PLLSAI1_GetN (), @ref LL_RCC_PLLSAI1_GetR ());
  * @param  __INPUTFREQ__ PLL Input frequency (based on MSI/HSE/HSI)
  * @param  __PLLM__ This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLM_DIV_1
  *         @arg @ref LL_RCC_PLLM_DIV_2
  *         @arg @ref LL_RCC_PLLM_DIV_3
  *         @arg @ref LL_RCC_PLLM_DIV_4
  *         @arg @ref LL_RCC_PLLM_DIV_5
  *         @arg @ref LL_RCC_PLLM_DIV_6
  *         @arg @ref LL_RCC_PLLM_DIV_7
  *         @arg @ref LL_RCC_PLLM_DIV_8
  * @param  __PLLSAI1N__ Between 8 and 86
  * @param  __PLLSAI1R__ This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLSAI1R_DIV_2
  *         @arg @ref LL_RCC_PLLSAI1R_DIV_4
  *         @arg @ref LL_RCC_PLLSAI1R_DIV_6
  *         @arg @ref LL_RCC_PLLSAI1R_DIV_8
  * @retval PLLSAI1 clock frequency (in Hz)
  */
#define __LL_RCC_CALC_PLLSAI1_ADC_FREQ(__INPUTFREQ__, __PLLM__, __PLLSAI1N__, __PLLSAI1R__) ((__INPUTFREQ__) / ((((__PLLM__)>> RCC_POSITION_PLLM) + 1)) * (__PLLSAI1N__) / \
                   ((((__PLLSAI1R__) >> RCC_POSITION_PLLSAI1R ) + 1 ) * 2))

/**
  * @brief  Helper macro to calculate the PLLSAI2 frequency used for SAI domain
  * @note ex: @ref __LL_RCC_CALC_PLLSAI2_SAI_FREQ (HSE_VALUE,@ref LL_RCC_PLL_GetDivider (),
  *             @ref LL_RCC_PLLSAI2_GetN (), @ref LL_RCC_PLLSAI2_GetP ());
  * @param  __INPUTFREQ__ PLL Input frequency (based on MSI/HSE/HSI)
  * @param  __PLLM__ This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLM_DIV_1
  *         @arg @ref LL_RCC_PLLM_DIV_2
  *         @arg @ref LL_RCC_PLLM_DIV_3
  *         @arg @ref LL_RCC_PLLM_DIV_4
  *         @arg @ref LL_RCC_PLLM_DIV_5
  *         @arg @ref LL_RCC_PLLM_DIV_6
  *         @arg @ref LL_RCC_PLLM_DIV_7
  *         @arg @ref LL_RCC_PLLM_DIV_8
  * @param  __PLLSAI2N__ Between 8 and 86
  * @param  __PLLSAI2P__ This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLSAI2P_DIV_7
  *         @arg @ref LL_RCC_PLLSAI2P_DIV_17
  * @retval PLLSAI2 clock frequency (in Hz)
  */
#define __LL_RCC_CALC_PLLSAI2_SAI_FREQ(__INPUTFREQ__, __PLLM__, __PLLSAI2N__, __PLLSAI2P__) ((__INPUTFREQ__) / ((((__PLLM__)>> RCC_POSITION_PLLM) + 1)) * (__PLLSAI2N__) / \
                   (((__PLLSAI2P__) == LL_RCC_PLLSAI2P_DIV_7) ? 7 : 17))

/**
  * @brief  Helper macro to calculate the PLLSAI2 frequency used on ADC domain
  * @note ex: @ref __LL_RCC_CALC_PLLSAI2_ADC_FREQ (HSE_VALUE,@ref LL_RCC_PLL_GetDivider (),
  *             @ref LL_RCC_PLLSAI2_GetN (), @ref LL_RCC_PLLSAI2_GetR ());
  * @param  __INPUTFREQ__ PLL Input frequency (based on MSI/HSE/HSI)
  * @param  __PLLM__ This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLM_DIV_1
  *         @arg @ref LL_RCC_PLLM_DIV_2
  *         @arg @ref LL_RCC_PLLM_DIV_3
  *         @arg @ref LL_RCC_PLLM_DIV_4
  *         @arg @ref LL_RCC_PLLM_DIV_5
  *         @arg @ref LL_RCC_PLLM_DIV_6
  *         @arg @ref LL_RCC_PLLM_DIV_7
  *         @arg @ref LL_RCC_PLLM_DIV_8
  * @param  __PLLSAI2N__ Between 8 and 86
  * @param  __PLLSAI2R__ This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLSAI2R_DIV_2
  *         @arg @ref LL_RCC_PLLSAI2R_DIV_4
  *         @arg @ref LL_RCC_PLLSAI2R_DIV_6
  *         @arg @ref LL_RCC_PLLSAI2R_DIV_8
  * @retval PLLSAI2 clock frequency (in Hz)
  */
#define __LL_RCC_CALC_PLLSAI2_ADC_FREQ(__INPUTFREQ__, __PLLM__, __PLLSAI2N__, __PLLSAI2R__) ((__INPUTFREQ__) / ((((__PLLM__)>> RCC_POSITION_PLLM) + 1)) * (__PLLSAI2N__) / \
                   ((((__PLLSAI2R__) >> RCC_POSITION_PLLSAI2R ) + 1 ) * 2))

/**
  * @brief  Helper macro to calculate the HCLK frequency
  * @param  __SYSCLKFREQ__ SYSCLK frequency (based on MSI/HSE/HSI/PLLCLK)
  * @param  __AHBPRESCALER__ This parameter can be one of the following values:
  *         @arg @ref LL_RCC_SYSCLK_DIV_1
  *         @arg @ref LL_RCC_SYSCLK_DIV_2
  *         @arg @ref LL_RCC_SYSCLK_DIV_4
  *         @arg @ref LL_RCC_SYSCLK_DIV_8
  *         @arg @ref LL_RCC_SYSCLK_DIV_16
  *         @arg @ref LL_RCC_SYSCLK_DIV_64
  *         @arg @ref LL_RCC_SYSCLK_DIV_128
  *         @arg @ref LL_RCC_SYSCLK_DIV_256
  *         @arg @ref LL_RCC_SYSCLK_DIV_512
  * @retval HCLK clock frequency (in Hz)
  */
#define __LL_RCC_CALC_HCLK_FREQ(__SYSCLKFREQ__, __AHBPRESCALER__) ((__SYSCLKFREQ__) >> AHBPrescTable[((__AHBPRESCALER__) & RCC_CFGR_HPRE) >>  RCC_POSITION_HPRE])

/**
  * @brief  Helper macro to calculate the PCLK1 frequency (ABP1)
  * @param  __HCLKFREQ__ HCLK frequency
  * @param  __APB1PRESCALER__ This parameter can be one of the following values:
  *         @arg @ref LL_RCC_APB1_DIV_1
  *         @arg @ref LL_RCC_APB1_DIV_2
  *         @arg @ref LL_RCC_APB1_DIV_4
  *         @arg @ref LL_RCC_APB1_DIV_8
  *         @arg @ref LL_RCC_APB1_DIV_16
  * @retval PCLK1 clock frequency (in Hz)
  */
#define __LL_RCC_CALC_PCLK1_FREQ(__HCLKFREQ__, __APB1PRESCALER__) ((__HCLKFREQ__) >> aRCC_APBAHBPrescTable[(__APB1PRESCALER__) >>  RCC_POSITION_PPRE1])

/**
  * @brief  Helper macro to calculate the PCLK2 frequency (ABP2)
  * @param  __HCLKFREQ__ HCLK frequency
  * @param  __APB2PRESCALER__ This parameter can be one of the following values:
  *         @arg @ref LL_RCC_APB2_DIV_1
  *         @arg @ref LL_RCC_APB2_DIV_2
  *         @arg @ref LL_RCC_APB2_DIV_4
  *         @arg @ref LL_RCC_APB2_DIV_8
  *         @arg @ref LL_RCC_APB2_DIV_16
  * @retval PCLK2 clock frequency (in Hz)
  */
#define __LL_RCC_CALC_PCLK2_FREQ(__HCLKFREQ__, __APB2PRESCALER__) ((__HCLKFREQ__) >> aRCC_APBAHBPrescTable[(__APB2PRESCALER__) >>  RCC_POSITION_PPRE2])

/**
  * @brief  Helper macro to calculate the MSI frequency (in Hz)
  * @note: __MSISEL__ can be retrieved thanks to function LL_RCC_MSI_IsEnabledRangeSelect
  * @note: if __MSISEL__ is equal to LL_RCC_MSIRANGESEL_STANDBY,
  *        __MSIRANGE__can be retrieved by LL_RCC_MSI_GetRangeAfterStandby
  *        else by LL_RCC_MSI_GetRange
  *        ex: __LL_RCC_CALC_MSI_FREQ(LL_RCC_MSI_IsEnabledRangeSelect(),
  *              (LL_RCC_MSI_IsEnabledRangeSelect()?
  *               LL_RCC_MSI_GetRange():
  *               LL_RCC_MSI_GetRangeAfterStandby()))
  * @param  __MSISEL__ This parameter can be one of the following values:
  *         @arg @ref LL_RCC_MSIRANGESEL_STANDBY
  *         @arg @ref LL_RCC_MSIRANGESEL_RUN
  * @param  __MSIRANGE__ This parameter can be one of the following values:
  *         @arg @ref LL_RCC_MSIRANGE_0
  *         @arg @ref LL_RCC_MSIRANGE_1
  *         @arg @ref LL_RCC_MSIRANGE_2
  *         @arg @ref LL_RCC_MSIRANGE_3
  *         @arg @ref LL_RCC_MSIRANGE_4
  *         @arg @ref LL_RCC_MSIRANGE_5
  *         @arg @ref LL_RCC_MSIRANGE_6
  *         @arg @ref LL_RCC_MSIRANGE_7
  *         @arg @ref LL_RCC_MSIRANGE_8
  *         @arg @ref LL_RCC_MSIRANGE_9
  *         @arg @ref LL_RCC_MSIRANGE_10
  *         @arg @ref LL_RCC_MSIRANGE_11
  *         @arg @ref LL_RCC_MSISRANGE_4
  *         @arg @ref LL_RCC_MSISRANGE_5
  *         @arg @ref LL_RCC_MSISRANGE_6
  *         @arg @ref LL_RCC_MSISRANGE_7
  * @retval MSI clock frequency (in Hz)
  */
#define __LL_RCC_CALC_MSI_FREQ(__MSISEL__, __MSIRANGE__)   (((__MSISEL__) == LL_RCC_MSIRANGESEL_STANDBY) ? \
                           (MSIRangeTable[(__MSIRANGE__) >> 8]) : \
                           (MSIRangeTable[(__MSIRANGE__) >> 4]))

/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup RCC_LL_Exported_Functions RCC Exported Functions
  * @{
  */

/** @defgroup RCC_LL_EF_HSE HSE
  * @{
  */

/**
  * @brief  Enable the Clock Security System.
  * @rmtoll CR           CSSON         LL_RCC_HSE_EnableCSS
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSE_EnableCSS(void)
{
  SET_BIT(RCC->CR, RCC_CR_CSSON);
}

/**
  * @brief  Enable HSE external oscillator (HSE Bypass)
  * @rmtoll CR           HSEBYP        LL_RCC_HSE_EnableBypass
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSE_EnableBypass(void)
{
  SET_BIT(RCC->CR, RCC_CR_HSEBYP);
}

/**
  * @brief  Disable HSE external oscillator (HSE Bypass)
  * @rmtoll CR           HSEBYP        LL_RCC_HSE_DisableBypass
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSE_DisableBypass(void)
{
  CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);
}

/**
  * @brief  Enable HSE crystal oscillator (HSE ON)
  * @rmtoll CR           HSEON         LL_RCC_HSE_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSE_Enable(void)
{
  SET_BIT(RCC->CR, RCC_CR_HSEON);
}

/**
  * @brief  Disable HSE crystal oscillator (HSE ON)
  * @rmtoll CR           HSEON         LL_RCC_HSE_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSE_Disable(void)
{
  CLEAR_BIT(RCC->CR, RCC_CR_HSEON);
}

/**
  * @brief  Check if HSE oscillator Ready
  * @rmtoll CR           HSERDY        LL_RCC_HSE_IsReady
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_HSE_IsReady(void)
{
  return (READ_BIT(RCC->CR, RCC_CR_HSERDY) == (RCC_CR_HSERDY));
}

/**
  * @}
  */

/** @defgroup RCC_LL_EF_HSI HSI
  * @{
  */

/**
  * @brief  Enable HSI even in stop mode
  * @note HSI oscillator is forced ON even in Stop mode
  * @rmtoll CR           HSIKERON      LL_RCC_HSI_EnableInStopMode
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSI_EnableInStopMode(void)
{
  SET_BIT(RCC->CR, RCC_CR_HSIKERON);
}

/**
  * @brief  Disable HSI in stop mode
  * @rmtoll CR           HSIKERON      LL_RCC_HSI_DisableInStopMode
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSI_DisableInStopMode(void)
{
  CLEAR_BIT(RCC->CR, RCC_CR_HSIKERON);
}

/**
  * @brief  Enable HSI oscillator
  * @rmtoll CR           HSION         LL_RCC_HSI_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSI_Enable(void)
{
  SET_BIT(RCC->CR, RCC_CR_HSION);
}

/**
  * @brief  Disable HSI oscillator
  * @rmtoll CR           HSION         LL_RCC_HSI_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSI_Disable(void)
{
  CLEAR_BIT(RCC->CR, RCC_CR_HSION);
}

/**
  * @brief  Check if HSI clock is ready
  * @rmtoll CR           HSIRDY        LL_RCC_HSI_IsReady
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_HSI_IsReady(void)
{
  return (READ_BIT(RCC->CR, RCC_CR_HSIRDY) == (RCC_CR_HSIRDY));
}

/**
  * @brief  Enable HSI Automatic from stop mode
  * @rmtoll CR           HSIASFS       LL_RCC_HSI_EnableAutoFromStop
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSI_EnableAutoFromStop(void)
{
  SET_BIT(RCC->CR, RCC_CR_HSIASFS);
}

/**
  * @brief  Disable HSI Automatic from stop mode
  * @rmtoll CR           HSIASFS       LL_RCC_HSI_DisableAutoFromStop
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSI_DisableAutoFromStop(void)
{
  CLEAR_BIT(RCC->CR, RCC_CR_HSIASFS);
}
/**
  * @brief  Get HSI Calibration value
  * @note When HSITRIM is written, HSICAL is updated with the sum of
  *       HSITRIM and the factory trim value
  * @rmtoll ICSCR        HSICAL        LL_RCC_HSI_GetCalibration
  * @retval Between Min_Data = 0x00 and Max_Data = 0xFF
  */
__STATIC_INLINE uint32_t LL_RCC_HSI_GetCalibration(void)
{
  return (uint32_t)(READ_BIT(RCC->ICSCR, RCC_ICSCR_HSICAL) >> RCC_POSITION_HSICAL);
}

/**
  * @brief  Set HSI Calibration trimming
  * @note user-programmable trimming value that is added to the HSICAL
  * @note Default value is 16, which, when added to the HSICAL value,
  *       should trim the HSI to 16 MHz +/- 1 %
  * @rmtoll ICSCR        HSITRIM       LL_RCC_HSI_SetCalibTrimming
  * @param  Value Between Min_Data = 0 and Max_Data = 31
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSI_SetCalibTrimming(uint32_t Value)
{
  MODIFY_REG(RCC->ICSCR, RCC_ICSCR_HSITRIM, Value << RCC_POSITION_HSITRIM);
}

/**
  * @brief  Get HSI Calibration trimming
  * @rmtoll ICSCR        HSITRIM       LL_RCC_HSI_GetCalibTrimming
  * @retval Between Min_Data = 0 and Max_Data = 31
  */
__STATIC_INLINE uint32_t LL_RCC_HSI_GetCalibTrimming(void)
{
  return (uint32_t)(READ_BIT(RCC->ICSCR, RCC_ICSCR_HSITRIM) >> RCC_POSITION_HSITRIM);
}

/**
  * @}
  */

/** @defgroup RCC_LL_EF_LSE LSE
  * @{
  */

/**
  * @brief  Enable  Low Speed External (LSE) crystal.
  * @rmtoll BDCR         LSEON         LL_RCC_LSE_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_LSE_Enable(void)
{
  SET_BIT(RCC->BDCR, RCC_BDCR_LSEON);
}

/**
  * @brief  Disable  Low Speed External (LSE) crystal.
  * @rmtoll BDCR         LSEON         LL_RCC_LSE_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_LSE_Disable(void)
{
  CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEON);
}

/**
  * @brief  Enable external clock source (LSE bypass).
  * @rmtoll BDCR         LSEBYP        LL_RCC_LSE_EnableBypass
  * @retval None
  */
__STATIC_INLINE void LL_RCC_LSE_EnableBypass(void)
{
  SET_BIT(RCC->BDCR, RCC_BDCR_LSEBYP);
}

/**
  * @brief  Disable external clock source (LSE bypass).
  * @rmtoll BDCR         LSEBYP        LL_RCC_LSE_DisableBypass
  * @retval None
  */
__STATIC_INLINE void LL_RCC_LSE_DisableBypass(void)
{
  CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEBYP);
}

/**
  * @brief  Set LSE oscillator drive capability
  * @note The oscillator is in Xtal mode when it is not in bypass mode.
  * @rmtoll BDCR         LSEDRV        LL_RCC_LSE_SetDriveCapability
  * @param  LSEDrive This parameter can be one of the following values:
  *         @arg @ref LL_RCC_LSEDRIVE_LOW
  *         @arg @ref LL_RCC_LSEDRIVE_MEDIUMLOW
  *         @arg @ref LL_RCC_LSEDRIVE_MEDIUMHIGH
  *         @arg @ref LL_RCC_LSEDRIVE_HIGH
  * @retval None
  */
__STATIC_INLINE void LL_RCC_LSE_SetDriveCapability(uint32_t LSEDrive)
{
  MODIFY_REG(RCC->BDCR, RCC_BDCR_LSEDRV, LSEDrive);
}

/**
  * @brief  Get LSE oscillator drive capability
  * @rmtoll BDCR         LSEDRV        LL_RCC_LSE_GetDriveCapability
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_LSEDRIVE_LOW
  *         @arg @ref LL_RCC_LSEDRIVE_MEDIUMLOW
  *         @arg @ref LL_RCC_LSEDRIVE_MEDIUMHIGH
  *         @arg @ref LL_RCC_LSEDRIVE_HIGH
  */
__STATIC_INLINE uint32_t LL_RCC_LSE_GetDriveCapability(void)
{
  return (uint32_t)(READ_BIT(RCC->BDCR, RCC_BDCR_LSEDRV));
}

/**
  * @brief  Enable Clock security system on LSE.
  * @rmtoll BDCR         LSECSSON      LL_RCC_LSE_EnableCSS
  * @retval None
  */
__STATIC_INLINE void LL_RCC_LSE_EnableCSS(void)
{
  SET_BIT(RCC->BDCR, RCC_BDCR_LSECSSON);
}

/**
  * @brief  Disable Clock security system on LSE.
  * @note Clock security system can be disabled only after a LSE
  *       failure detection. In that case it MUST be disabled by software.
  * @rmtoll BDCR         LSECSSON      LL_RCC_LSE_DisableCSS
  * @retval None
  */
__STATIC_INLINE void LL_RCC_LSE_DisableCSS(void)
{
  CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSECSSON);
}

/**
  * @brief  Check if LSE oscillator Ready
  * @rmtoll BDCR         LSERDY        LL_RCC_LSE_IsReady
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_LSE_IsReady(void)
{
  return (READ_BIT(RCC->BDCR, RCC_BDCR_LSERDY) == (RCC_BDCR_LSERDY));
}

/**
  * @brief  Check if CSS on LSE failure Detection
  * @rmtoll BDCR         LSECSSD       LL_RCC_LSE_IsCSSDetected
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_LSE_IsCSSDetected(void)
{
  return (READ_BIT(RCC->BDCR, RCC_BDCR_LSECSSD) == (RCC_BDCR_LSECSSD));
}

/**
  * @}
  */

/** @defgroup RCC_LL_EF_LSI LSI
  * @{
  */

/**
  * @brief  Enable LSI Oscillator
  * @rmtoll CSR          LSION         LL_RCC_LSI_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_LSI_Enable(void)
{
  SET_BIT(RCC->CSR, RCC_CSR_LSION);
}

/**
  * @brief  Disable LSI Oscillator
  * @rmtoll CSR          LSION         LL_RCC_LSI_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_LSI_Disable(void)
{
  CLEAR_BIT(RCC->CSR, RCC_CSR_LSION);
}

/**
  * @brief  Check if LSI is Ready
  * @rmtoll CSR          LSIRDY        LL_RCC_LSI_IsReady
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_LSI_IsReady(void)
{
  return (READ_BIT(RCC->CSR, RCC_CSR_LSIRDY) == (RCC_CSR_LSIRDY));
}

/**
  * @}
  */

/** @defgroup RCC_LL_EF_MSI MSI
  * @{
  */

/**
  * @brief  Enable MSI oscillator
  * @rmtoll CR           MSION         LL_RCC_MSI_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_MSI_Enable(void)
{
  SET_BIT(RCC->CR, RCC_CR_MSION);
}

/**
  * @brief  Disable MSI oscillator
  * @rmtoll CR           MSION         LL_RCC_MSI_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_MSI_Disable(void)
{
  CLEAR_BIT(RCC->CR, RCC_CR_MSION);
}

/**
  * @brief  Check if MSI oscillator Ready
  * @rmtoll CR           MSIRDY        LL_RCC_MSI_IsReady
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_MSI_IsReady(void)
{
  return (READ_BIT(RCC->CR, RCC_CR_MSIRDY) == (RCC_CR_MSIRDY));
}

/**
  * @brief  Enable MSI PLL-mode (Hardware auto calibration with LSE)
  * @note MSIPLLEN must be enabled after LSE is enabled (LSEON enabled)
  *       and ready (LSERDY set by hardware)
  * @note hardware protection to avoid enabling MSIPLLEN if LSE is not
  *       ready
  * @rmtoll CR           MSIPLLEN      LL_RCC_MSI_EnablePLLMode
  * @retval None
  */
__STATIC_INLINE void LL_RCC_MSI_EnablePLLMode(void)
{
  SET_BIT(RCC->CR, RCC_CR_MSIPLLEN);
}

/**
  * @brief  Disable MSI-PLL mode
  * @note cleared by hardware when LSE is disabled (LSEON = 0) or when
  *       the Clock Security System on LSE detects a LSE failure
  * @rmtoll CR           MSIPLLEN      LL_RCC_MSI_DisablePLLMode
  * @retval None
  */
__STATIC_INLINE void LL_RCC_MSI_DisablePLLMode(void)
{
  CLEAR_BIT(RCC->CR, RCC_CR_MSIPLLEN);
}

/**
  * @brief  Enable MSI clock range selection with MSIRANGE register
  * @note Write 0 has no effect. After a standby or a reset
  *       MSIRGSEL is at 0 and the MSI range value is provided by
  *       MSISRANGE
  * @rmtoll CR           MSIRGSEL      LL_RCC_MSI_EnableRangeSelection
  * @retval None
  */
__STATIC_INLINE void LL_RCC_MSI_EnableRangeSelection(void)
{
  SET_BIT(RCC->CR, RCC_CR_MSIRGSEL);
}

/**
  * @brief  Check if MSI clock range is selected with MSIRANGE register
  * @rmtoll CR           MSIRGSEL      LL_RCC_MSI_IsEnabledRangeSelect
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_MSI_IsEnabledRangeSelect(void)
{
  return (READ_BIT(RCC->CR, RCC_CR_MSIRGSEL) == (RCC_CR_MSIRGSEL));
}

/**
  * @brief  Configure the Internal Multi Speed oscillator (MSI) clock range in run mode.
  * @rmtoll CR           MSIRANGE      LL_RCC_MSI_SetRange
  * @param  Range This parameter can be one of the following values:
  *         @arg @ref LL_RCC_MSIRANGE_0
  *         @arg @ref LL_RCC_MSIRANGE_1
  *         @arg @ref LL_RCC_MSIRANGE_2
  *         @arg @ref LL_RCC_MSIRANGE_3
  *         @arg @ref LL_RCC_MSIRANGE_4
  *         @arg @ref LL_RCC_MSIRANGE_5
  *         @arg @ref LL_RCC_MSIRANGE_6
  *         @arg @ref LL_RCC_MSIRANGE_7
  *         @arg @ref LL_RCC_MSIRANGE_8
  *         @arg @ref LL_RCC_MSIRANGE_9
  *         @arg @ref LL_RCC_MSIRANGE_10
  *         @arg @ref LL_RCC_MSIRANGE_11
  * @retval None
  */
__STATIC_INLINE void LL_RCC_MSI_SetRange(uint32_t Range)
{
  MODIFY_REG(RCC->CR, RCC_CR_MSIRANGE, Range);
}

/**
  * @brief  Get the Internal Multi Speed oscillator (MSI) clock range in run mode.
  * @rmtoll CR           MSIRANGE      LL_RCC_MSI_GetRange
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_MSIRANGE_0
  *         @arg @ref LL_RCC_MSIRANGE_1
  *         @arg @ref LL_RCC_MSIRANGE_2
  *         @arg @ref LL_RCC_MSIRANGE_3
  *         @arg @ref LL_RCC_MSIRANGE_4
  *         @arg @ref LL_RCC_MSIRANGE_5
  *         @arg @ref LL_RCC_MSIRANGE_6
  *         @arg @ref LL_RCC_MSIRANGE_7
  *         @arg @ref LL_RCC_MSIRANGE_8
  *         @arg @ref LL_RCC_MSIRANGE_9
  *         @arg @ref LL_RCC_MSIRANGE_10
  *         @arg @ref LL_RCC_MSIRANGE_11
  */
__STATIC_INLINE uint32_t LL_RCC_MSI_GetRange(void)
{
  return (uint32_t)(READ_BIT(RCC->CR, RCC_CR_MSIRANGE));
}

/**
  * @brief  Configure MSI range used after standby
  * @rmtoll CSR          MSISRANGE     LL_RCC_MSI_SetRangeAfterStandby
  * @param  Range This parameter can be one of the following values:
  *         @arg @ref LL_RCC_MSISRANGE_4
  *         @arg @ref LL_RCC_MSISRANGE_5
  *         @arg @ref LL_RCC_MSISRANGE_6
  *         @arg @ref LL_RCC_MSISRANGE_7
  * @retval None
  */
__STATIC_INLINE void LL_RCC_MSI_SetRangeAfterStandby(uint32_t Range)
{
  MODIFY_REG(RCC->CSR, RCC_CSR_MSISRANGE, Range);
}

/**
  * @brief  Get MSI range used after standby
  * @rmtoll CSR          MSISRANGE     LL_RCC_MSI_GetRangeAfterStandby
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_MSISRANGE_4
  *         @arg @ref LL_RCC_MSISRANGE_5
  *         @arg @ref LL_RCC_MSISRANGE_6
  *         @arg @ref LL_RCC_MSISRANGE_7
  */
__STATIC_INLINE uint32_t LL_RCC_MSI_GetRangeAfterStandby(void)
{
  return (uint32_t)(READ_BIT(RCC->CSR, RCC_CSR_MSISRANGE));
}

/**
  * @brief  Get MSI Calibration value
  * @note When MSITRIM is written, MSICAL is updated with the sum of
  *       MSITRIM and the factory trim value
  * @rmtoll ICSCR        MSICAL        LL_RCC_MSI_GetCalibration
  * @retval Between Min_Data = 0 and Max_Data = 255
  */
__STATIC_INLINE uint32_t LL_RCC_MSI_GetCalibration(void)
{
  return (uint32_t)(READ_BIT(RCC->ICSCR, RCC_ICSCR_MSICAL) >> RCC_POSITION_MSICAL);
}

/**
  * @brief  Set MSI Calibration trimming
  * @note user-programmable trimming value that is added to the MSICAL
  * @rmtoll ICSCR        MSITRIM       LL_RCC_MSI_SetCalibTrimming
  * @param  Value Between Min_Data = 0 and Max_Data = 255
  * @retval None
  */
__STATIC_INLINE void LL_RCC_MSI_SetCalibTrimming(uint32_t Value)
{
  MODIFY_REG(RCC->ICSCR, RCC_ICSCR_MSITRIM, Value << RCC_POSITION_MSITRIM);
}

/**
  * @brief  Get MSI Calibration trimming
  * @rmtoll ICSCR        MSITRIM       LL_RCC_MSI_GetCalibTrimming
  * @retval Between 0 and 255
  */
__STATIC_INLINE uint32_t LL_RCC_MSI_GetCalibTrimming(void)
{
  return (uint32_t)(READ_BIT(RCC->ICSCR, RCC_ICSCR_MSITRIM) >> RCC_POSITION_MSITRIM);
}

/**
  * @}
  */

/** @defgroup RCC_LL_EF_LSCO LSCO
  * @{
  */

/**
  * @brief  Enable Low speed clock
  * @rmtoll BDCR         LSCOEN        LL_RCC_LSCO_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_LSCO_Enable(void)
{
  SET_BIT(RCC->BDCR, RCC_BDCR_LSCOEN);
}

/**
  * @brief  Disable Low speed clock
  * @rmtoll BDCR         LSCOEN        LL_RCC_LSCO_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_LSCO_Disable(void)
{
  CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSCOEN);
}

/**
  * @brief  Configure Low speed clock selection
  * @rmtoll BDCR         LSCOSEL       LL_RCC_LSCO_SetSource
  * @param  Source This parameter can be one of the following values:
  *         @arg @ref LL_RCC_LSCO_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_LSCO_CLKSOURCE_LSE
  * @retval None
  */
__STATIC_INLINE void LL_RCC_LSCO_SetSource(uint32_t Source)
{
  MODIFY_REG(RCC->BDCR, RCC_BDCR_LSCOSEL, Source);
}

/**
  * @brief  Get Low speed clock selection
  * @rmtoll BDCR         LSCOSEL       LL_RCC_LSCO_GetSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_LSCO_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_LSCO_CLKSOURCE_LSE
  */
__STATIC_INLINE uint32_t LL_RCC_LSCO_GetSource(void)
{
  return (uint32_t)(READ_BIT(RCC->BDCR, RCC_BDCR_LSCOSEL));
}

/**
  * @}
  */

/** @defgroup RCC_LL_EF_System System
  * @{
  */

/**
  * @brief  Configure the system clock source
  * @rmtoll CFGR         SW            LL_RCC_SetSysClkSource
  * @param  Source This parameter can be one of the following values:
  *         @arg @ref LL_RCC_SYS_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SYS_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SYS_CLKSOURCE_HSE
  *         @arg @ref LL_RCC_SYS_CLKSOURCE_PLL
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetSysClkSource(uint32_t Source)
{
  MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, Source);
}

/**
  * @brief  Get the system clock source
  * @rmtoll CFGR         SWS           LL_RCC_GetSysClkSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_SYS_CLKSOURCE_STATUS_MSI
  *         @arg @ref LL_RCC_SYS_CLKSOURCE_STATUS_HSI
  *         @arg @ref LL_RCC_SYS_CLKSOURCE_STATUS_HSE
  *         @arg @ref LL_RCC_SYS_CLKSOURCE_STATUS_PLL
  */
__STATIC_INLINE uint32_t LL_RCC_GetSysClkSource(void)
{
  return (uint32_t)(READ_BIT(RCC->CFGR, RCC_CFGR_SWS));
}

/**
  * @brief  Set AHB prescaler
  * @rmtoll CFGR         HPRE          LL_RCC_SetAHBPrescaler
  * @param  Prescaler This parameter can be one of the following values:
  *         @arg @ref LL_RCC_SYSCLK_DIV_1
  *         @arg @ref LL_RCC_SYSCLK_DIV_2
  *         @arg @ref LL_RCC_SYSCLK_DIV_4
  *         @arg @ref LL_RCC_SYSCLK_DIV_8
  *         @arg @ref LL_RCC_SYSCLK_DIV_16
  *         @arg @ref LL_RCC_SYSCLK_DIV_64
  *         @arg @ref LL_RCC_SYSCLK_DIV_128
  *         @arg @ref LL_RCC_SYSCLK_DIV_256
  *         @arg @ref LL_RCC_SYSCLK_DIV_512
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetAHBPrescaler(uint32_t Prescaler)
{
  MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, Prescaler);
}

/**
  * @brief  Set APB1 prescaler
  * @rmtoll CFGR         PPRE1         LL_RCC_SetAPB1Prescaler
  * @param  Prescaler This parameter can be one of the following values:
  *         @arg @ref LL_RCC_APB1_DIV_1
  *         @arg @ref LL_RCC_APB1_DIV_2
  *         @arg @ref LL_RCC_APB1_DIV_4
  *         @arg @ref LL_RCC_APB1_DIV_8
  *         @arg @ref LL_RCC_APB1_DIV_16
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetAPB1Prescaler(uint32_t Prescaler)
{
  MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, Prescaler);
}

/**
  * @brief  Set APB2 prescaler
  * @rmtoll CFGR         PPRE2         LL_RCC_SetAPB2Prescaler
  * @param  Prescaler This parameter can be one of the following values:
  *         @arg @ref LL_RCC_APB2_DIV_1
  *         @arg @ref LL_RCC_APB2_DIV_2
  *         @arg @ref LL_RCC_APB2_DIV_4
  *         @arg @ref LL_RCC_APB2_DIV_8
  *         @arg @ref LL_RCC_APB2_DIV_16
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetAPB2Prescaler(uint32_t Prescaler)
{
  MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, Prescaler);
}

/**
  * @brief  Get AHB prescaler
  * @rmtoll CFGR         HPRE          LL_RCC_GetAHBPrescaler
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_SYSCLK_DIV_1
  *         @arg @ref LL_RCC_SYSCLK_DIV_2
  *         @arg @ref LL_RCC_SYSCLK_DIV_4
  *         @arg @ref LL_RCC_SYSCLK_DIV_8
  *         @arg @ref LL_RCC_SYSCLK_DIV_16
  *         @arg @ref LL_RCC_SYSCLK_DIV_64
  *         @arg @ref LL_RCC_SYSCLK_DIV_128
  *         @arg @ref LL_RCC_SYSCLK_DIV_256
  *         @arg @ref LL_RCC_SYSCLK_DIV_512
  */
__STATIC_INLINE uint32_t LL_RCC_GetAHBPrescaler(void)
{
  return (uint32_t)(READ_BIT(RCC->CFGR, RCC_CFGR_HPRE));
}

/**
  * @brief  Get APB1 prescaler
  * @rmtoll CFGR         PPRE1         LL_RCC_GetAPB1Prescaler
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_APB1_DIV_1
  *         @arg @ref LL_RCC_APB1_DIV_2
  *         @arg @ref LL_RCC_APB1_DIV_4
  *         @arg @ref LL_RCC_APB1_DIV_8
  *         @arg @ref LL_RCC_APB1_DIV_16
  */
__STATIC_INLINE uint32_t LL_RCC_GetAPB1Prescaler(void)
{
  return (uint32_t)(READ_BIT(RCC->CFGR, RCC_CFGR_PPRE1));
}

/**
  * @brief  Get APB2 prescaler
  * @rmtoll CFGR         PPRE2         LL_RCC_GetAPB2Prescaler
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_APB2_DIV_1
  *         @arg @ref LL_RCC_APB2_DIV_2
  *         @arg @ref LL_RCC_APB2_DIV_4
  *         @arg @ref LL_RCC_APB2_DIV_8
  *         @arg @ref LL_RCC_APB2_DIV_16
  */
__STATIC_INLINE uint32_t LL_RCC_GetAPB2Prescaler(void)
{
  return (uint32_t)(READ_BIT(RCC->CFGR, RCC_CFGR_PPRE2));
}


/**
  * @brief  Set Clock After Wake-Up From Stop mode
  * @rmtoll CFGR         STOPWUCK      LL_RCC_SetClkAfterWakeFromStop
  * @param  Clock This parameter can be one of the following values:
  *         @arg @ref LL_RCC_STOP_WAKEUPCLOCK_MSI
  *         @arg @ref LL_RCC_STOP_WAKEUPCLOCK_HSI
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetClkAfterWakeFromStop(uint32_t Clock)
{
  MODIFY_REG(RCC->CFGR, RCC_CFGR_STOPWUCK, Clock);
}

/**
  * @brief  Get Clock After Wake-Up From Stop mode
  * @rmtoll CFGR         STOPWUCK      LL_RCC_GetClkAfterWakeFromStop
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_STOP_WAKEUPCLOCK_MSI
  *         @arg @ref LL_RCC_STOP_WAKEUPCLOCK_HSI
  */
__STATIC_INLINE uint32_t LL_RCC_GetClkAfterWakeFromStop(void)
{
  return (uint32_t)(READ_BIT(RCC->CFGR, RCC_CFGR_STOPWUCK));
}

/**
  * @}
  */

/** @defgroup RCC_LL_EF_MCO MCO
  * @{
  */

/**
  * @brief  Configure MCOx
  * @rmtoll CFGR         MCOSEL        LL_RCC_ConfigMCO\n
  *         CFGR         MCOPRE        LL_RCC_ConfigMCO
  * @param  MCOxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_MCO1SOURCE_NOCLOCK
  *         @arg @ref LL_RCC_MCO1SOURCE_SYSCLK
  *         @arg @ref LL_RCC_MCO1SOURCE_MSI
  *         @arg @ref LL_RCC_MCO1SOURCE_HSI
  *         @arg @ref LL_RCC_MCO1SOURCE_HSE
  *         @arg @ref LL_RCC_MCO1SOURCE_PLLCLK
  *         @arg @ref LL_RCC_MCO1SOURCE_LSI
  *         @arg @ref LL_RCC_MCO1SOURCE_LSE
  *
  * @param  MCOxPrescaler This parameter can be one of the following values:
  *         @arg @ref LL_RCC_MCO1_DIV_1
  *         @arg @ref LL_RCC_MCO1_DIV_2
  *         @arg @ref LL_RCC_MCO1_DIV_4
  *         @arg @ref LL_RCC_MCO1_DIV_8
  *         @arg @ref LL_RCC_MCO1_DIV_16
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ConfigMCO(uint32_t MCOxSource, uint32_t MCOxPrescaler)
{
  MODIFY_REG(RCC->CFGR, RCC_CFGR_MCOSEL | RCC_CFGR_MCOPRE, MCOxSource | MCOxPrescaler);
}

/**
  * @}
  */

/** @defgroup RCC_LL_EF_Peripheral_Clock_Source Peripheral Clock Source
  * @{
  */

/**
  * @brief  Configure USARTx clock source
  * @rmtoll CCIPR        USARTxSEL     LL_RCC_SetUSARTClockSource
  * @param  USARTxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_SYSCLK
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_SYSCLK
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_SYSCLK
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_LSE
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetUSARTClockSource(uint32_t USARTxSource)
{
  MODIFY_REG(RCC->CCIPR, (USARTxSource >> 16), (USARTxSource & 0x0000FFFF));
}

#if defined(UART4) || defined(UART5)
/**
  * @brief  Configure UARTx clock source
  * @rmtoll CCIPR        UARTxSEL      LL_RCC_SetUARTClockSource
  * @param  UARTxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_SYSCLK
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_SYSCLK
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_LSE
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetUARTClockSource(uint32_t UARTxSource)
{
  MODIFY_REG(RCC->CCIPR, (UARTxSource >> 16), (UARTxSource & 0x0000FFFF));
}
#endif /* UART4 || UART5 */

/**
  * @brief  Configure LPUART1x clock source
  * @rmtoll CCIPR        LPUART1SEL    LL_RCC_SetLPUARTClockSource
  * @param  LPUARTxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_LPUART1_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_LPUART1_CLKSOURCE_SYSCLK
  *         @arg @ref LL_RCC_LPUART1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_LPUART1_CLKSOURCE_LSE
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetLPUARTClockSource(uint32_t LPUARTxSource)
{
  MODIFY_REG(RCC->CCIPR, RCC_CCIPR_LPUART1SEL, LPUARTxSource);
}

/**
  * @brief  Configure I2Cx clock source
  * @rmtoll CCIPR        I2CxSEL       LL_RCC_SetI2CClockSource
  * @param  I2CxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_SYSCLK
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_SYSCLK
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_SYSCLK
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_HSI
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetI2CClockSource(uint32_t I2CxSource)
{
  MODIFY_REG(RCC->CCIPR, ((I2CxSource >> 4) & 0x000FF000), ((I2CxSource << 4) & 0x000FF000));
}

/**
  * @brief  Configure LPTIMx clock source
  * @rmtoll CCIPR        LPTIMxSEL     LL_RCC_SetLPTIMClockSource
  * @param  LPTIMxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_LSE
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetLPTIMClockSource(uint32_t LPTIMxSource)
{
  MODIFY_REG(RCC->CCIPR, (LPTIMxSource & 0xFFFF0000U), (LPTIMxSource << 16));
}

/**
  * @brief  Configure SAIx clock source
  * @rmtoll CCIPR        SAIxSEL       LL_RCC_SetSAIClockSource
  * @param  SAIxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_PLLSAI1
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_PLLSAI2 (*)
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_PLL
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_PIN
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_PLLSAI1 (*)
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_PLLSAI2 (*)
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_PLL (*)
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_PIN (*)
  *
  *         (*) value not defined in all devices.
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetSAIClockSource(uint32_t SAIxSource)
{
  MODIFY_REG(RCC->CCIPR, (SAIxSource & 0xFFFF0000U), (SAIxSource << 16));
}

/**
  * @brief  Configure SDMMC1 clock source
  * @rmtoll CCIPR        CLK48SEL      LL_RCC_SetSDMMCClockSource
  * @param  SDMMCxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE_NONE
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE_PLLSAI1
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE_PLL
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE_MSI
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetSDMMCClockSource(uint32_t SDMMCxSource)
{
  MODIFY_REG(RCC->CCIPR, RCC_CCIPR_CLK48SEL, SDMMCxSource);
}

/**
  * @brief  Configure RNG clock source
  * @rmtoll CCIPR        CLK48SEL      LL_RCC_SetRNGClockSource
  * @param  RNGxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_RNG_CLKSOURCE_NONE
  *         @arg @ref LL_RCC_RNG_CLKSOURCE_PLLSAI1
  *         @arg @ref LL_RCC_RNG_CLKSOURCE_PLL
  *         @arg @ref LL_RCC_RNG_CLKSOURCE_MSI
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetRNGClockSource(uint32_t RNGxSource)
{
  MODIFY_REG(RCC->CCIPR, RCC_CCIPR_CLK48SEL, RNGxSource);
}

#if defined(USB_OTG_FS)
/**
  * @brief  Configure USB clock source
  * @rmtoll CCIPR        CLK48SEL      LL_RCC_SetUSBClockSource
  * @param  USBxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_USB_CLKSOURCE_NONE
  *         @arg @ref LL_RCC_USB_CLKSOURCE_PLLSAI1
  *         @arg @ref LL_RCC_USB_CLKSOURCE_PLL
  *         @arg @ref LL_RCC_USB_CLKSOURCE_MSI
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetUSBClockSource(uint32_t USBxSource)
{
  MODIFY_REG(RCC->CCIPR, RCC_CCIPR_CLK48SEL, USBxSource);
}
#endif /* USB_OTG_FS */

/**
  * @brief  Configure ADC clock source
  * @rmtoll CCIPR        ADCSEL        LL_RCC_SetADCClockSource
  * @param  ADCxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_ADC_CLKSOURCE_NONE
  *         @arg @ref LL_RCC_ADC_CLKSOURCE_PLLSAI1
  *         @arg @ref LL_RCC_ADC_CLKSOURCE_PLLSAI2 (*)
  *         @arg @ref LL_RCC_ADC_CLKSOURCE_SYSCLK
  *
  *         (*) value not defined in all devices.
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetADCClockSource(uint32_t ADCxSource)
{
  MODIFY_REG(RCC->CCIPR, RCC_CCIPR_ADCSEL, ADCxSource);
}

/**
  * @brief  Configure SWPMI clock source
  * @rmtoll CCIPR        SWPMI1SEL     LL_RCC_SetSWPMIClockSource
  * @param  SWPMIxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_SWPMI1_CLKSOURCE_PCLK
  *         @arg @ref LL_RCC_SWPMI1_CLKSOURCE_HSI
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetSWPMIClockSource(uint32_t SWPMIxSource)
{
  MODIFY_REG(RCC->CCIPR, RCC_CCIPR_SWPMI1SEL, SWPMIxSource);
}

#if defined(DFSDM_Channel0)
/**
  * @brief  Configure DFSDM clock source
  * @rmtoll CCIPR        DFSDMSEL      LL_RCC_SetDFSDMClockSource
  * @param  DFSDMxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_DFSDM_CLKSOURCE_PCLK
  *         @arg @ref LL_RCC_DFSDM_CLKSOURCE_SYSCLK
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetDFSDMClockSource(uint32_t DFSDMxSource)
{
  MODIFY_REG(RCC->CCIPR, RCC_CCIPR_DFSDMSEL, DFSDMxSource);
}
#endif /* DFSDM_Channel0 */

/**
  * @brief  Get USARTx clock source
  * @rmtoll CCIPR        USARTxSEL     LL_RCC_GetUSARTClockSource
  * @param  USARTx This parameter can be one of the following values:
  *         @arg @ref LL_RCC_USART1_CLKSOURCE
  *         @arg @ref LL_RCC_USART2_CLKSOURCE
  *         @arg @ref LL_RCC_USART3_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_SYSCLK
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_SYSCLK
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_SYSCLK
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_LSE
  */
__STATIC_INLINE uint32_t LL_RCC_GetUSARTClockSource(uint32_t USARTx)
{
  return (uint32_t)(READ_BIT(RCC->CCIPR, USARTx) | (USARTx << 16));
}

#if defined(UART4) || defined(UART5)
/**
  * @brief  Get UARTx clock source
  * @rmtoll CCIPR        UARTxSEL      LL_RCC_GetUARTClockSource
  * @param  UARTx This parameter can be one of the following values:
  *         @arg @ref LL_RCC_UART4_CLKSOURCE
  *         @arg @ref LL_RCC_UART5_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_SYSCLK
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_SYSCLK
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_LSE
  */
__STATIC_INLINE uint32_t LL_RCC_GetUARTClockSource(uint32_t UARTx)
{
  return (uint32_t)(READ_BIT(RCC->CCIPR, UARTx) | (UARTx << 16));
}
#endif /* UART4 || UART5 */

/**
  * @brief  Get LPUARTx clock source
  * @rmtoll CCIPR        LPUART1SEL    LL_RCC_GetLPUARTClockSource
  * @param  LPUARTx This parameter can be one of the following values:
  *         @arg @ref LL_RCC_LPUART1_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_LPUART1_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_LPUART1_CLKSOURCE_SYSCLK
  *         @arg @ref LL_RCC_LPUART1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_LPUART1_CLKSOURCE_LSE
  */
__STATIC_INLINE uint32_t LL_RCC_GetLPUARTClockSource(uint32_t LPUARTx)
{
  return (uint32_t)(READ_BIT(RCC->CCIPR, LPUARTx));
}

/**
  * @brief  Get I2Cx clock source
  * @rmtoll CCIPR        I2CxSEL       LL_RCC_GetI2CClockSource
  * @param  I2Cx This parameter can be one of the following values:
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_SYSCLK
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_SYSCLK
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_SYSCLK
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_HSI
  */
__STATIC_INLINE uint32_t LL_RCC_GetI2CClockSource(uint32_t I2Cx)
{
  return (uint32_t)((READ_BIT(RCC->CCIPR, I2Cx) >> 4) | (I2Cx << 4));
}

/**
  * @brief  Get LPTIMx clock source
  * @rmtoll CCIPR        LPTIMxSEL     LL_RCC_GetLPTIMClockSource
  * @param  LPTIMx This parameter can be one of the following values:
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_LSE
  */
__STATIC_INLINE uint32_t LL_RCC_GetLPTIMClockSource(uint32_t LPTIMx)
{
  return (uint32_t)(READ_BIT(RCC->CCIPR, LPTIMx) >> 16 | LPTIMx);
}

/**
  * @brief  Get SAIx clock source
  * @rmtoll CCIPR        SAIxSEL       LL_RCC_GetSAIClockSource
  * @param  SAIx This parameter can be one of the following values:
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE (*)
  *
  *         (*) value not defined in all devices.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_PLLSAI1
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_PLLSAI2 (*)
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_PLL
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_PIN
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_PLLSAI1 (*)
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_PLLSAI2 (*)
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_PLL (*)
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_PIN (*)
  *
  *         (*) value not defined in all devices.
  */
__STATIC_INLINE uint32_t LL_RCC_GetSAIClockSource(uint32_t SAIx)
{
  return (uint32_t)(READ_BIT(RCC->CCIPR, SAIx) >> 16 | SAIx);
}

/**
  * @brief  Get SDMMCx clock source
  * @rmtoll CCIPR        CLK48SEL      LL_RCC_GetSDMMCClockSource
  * @param  SDMMCx This parameter can be one of the following values:
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE_NONE
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE_PLLSAI1
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE_PLL
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE_MSI
  */
__STATIC_INLINE uint32_t LL_RCC_GetSDMMCClockSource(uint32_t SDMMCx)
{
  return (uint32_t)(READ_BIT(RCC->CCIPR, SDMMCx));
}

/**
  * @brief  Get RNGx clock source
  * @rmtoll CCIPR        CLK48SEL      LL_RCC_GetRNGClockSource
  * @param  RNGx This parameter can be one of the following values:
  *         @arg @ref LL_RCC_RNG_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_RNG_CLKSOURCE_NONE
  *         @arg @ref LL_RCC_RNG_CLKSOURCE_PLLSAI1
  *         @arg @ref LL_RCC_RNG_CLKSOURCE_PLL
  *         @arg @ref LL_RCC_RNG_CLKSOURCE_MSI
  */
__STATIC_INLINE uint32_t LL_RCC_GetRNGClockSource(uint32_t RNGx)
{
  return (uint32_t)(READ_BIT(RCC->CCIPR, RNGx));
}

#if defined(USB_OTG_FS)
/**
  * @brief  Get USBx clock source
  * @rmtoll CCIPR        CLK48SEL      LL_RCC_GetUSBClockSource
  * @param  USBx This parameter can be one of the following values:
  *         @arg @ref LL_RCC_USB_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_USB_CLKSOURCE_NONE
  *         @arg @ref LL_RCC_USB_CLKSOURCE_PLLSAI1
  *         @arg @ref LL_RCC_USB_CLKSOURCE_PLL
  *         @arg @ref LL_RCC_USB_CLKSOURCE_MSI
  */
__STATIC_INLINE uint32_t LL_RCC_GetUSBClockSource(uint32_t USBx)
{
  return (uint32_t)(READ_BIT(RCC->CCIPR, USBx));
}
#endif /* USB_OTG_FS */

/**
  * @brief  Get ADCx clock source
  * @rmtoll CCIPR        ADCSEL        LL_RCC_GetADCClockSource
  * @param  ADCx This parameter can be one of the following values:
  *         @arg @ref LL_RCC_ADC_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_ADC_CLKSOURCE_NONE
  *         @arg @ref LL_RCC_ADC_CLKSOURCE_PLLSAI1
  *         @arg @ref LL_RCC_ADC_CLKSOURCE_PLLSAI2 (*)
  *         @arg @ref LL_RCC_ADC_CLKSOURCE_SYSCLK
  *
  *         (*) value not defined in all devices.
  */
__STATIC_INLINE uint32_t LL_RCC_GetADCClockSource(uint32_t ADCx)
{
  return (uint32_t)(READ_BIT(RCC->CCIPR, ADCx));
}

/**
  * @brief  Get SWPMIx clock source
  * @rmtoll CCIPR        SWPMI1SEL     LL_RCC_GetSWPMIClockSource
  * @param  SPWMIx This parameter can be one of the following values:
  *         @arg @ref LL_RCC_SWPMI1_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_SWPMI1_CLKSOURCE_PCLK
  *         @arg @ref LL_RCC_SWPMI1_CLKSOURCE_HSI
  */
__STATIC_INLINE uint32_t LL_RCC_GetSWPMIClockSource(uint32_t SPWMIx)
{
  return (uint32_t)(READ_BIT(RCC->CCIPR, SPWMIx));
}

#if defined(DFSDM_Channel0)
/**
  * @brief  Get DFSDMx clock source
  * @rmtoll CCIPR        DFSDMSEL      LL_RCC_GetDFSDMClockSource
  * @param  DFSDMx This parameter can be one of the following values:
  *         @arg @ref LL_RCC_DFSDM_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_DFSDM_CLKSOURCE_PCLK
  *         @arg @ref LL_RCC_DFSDM_CLKSOURCE_SYSCLK
  */
__STATIC_INLINE uint32_t LL_RCC_GetDFSDMClockSource(uint32_t DFSDMx)
{
  return (uint32_t)(READ_BIT(RCC->CCIPR, DFSDMx));
}
#endif /* DFSDM_Channel0 */

/**
  * @}
  */

/** @defgroup RCC_LL_EF_RTC RTC
  * @{
  */

/**
  * @brief  Set RTC Clock Source
  * @note Once the RTC clock source has been selected, it cannot be changed anymore unless
  *       the Backup domain is reset, or unless a failure is detected on LSE (LSECSSD is
  *       set). The BDRST bit can be used to reset them.
  * @rmtoll BDCR         RTCSEL        LL_RCC_SetRTCClockSource
  * @param  Source This parameter can be one of the following values:
  *         @arg @ref LL_RCC_RTC_CLKSOURCE_NONE
  *         @arg @ref LL_RCC_RTC_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_RTC_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_RTC_CLKSOURCE_HSE_DIV32
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetRTCClockSource(uint32_t Source)
{
  MODIFY_REG(RCC->BDCR, RCC_BDCR_RTCSEL, Source);
}

/**
  * @brief  Get RTC Clock Source
  * @rmtoll BDCR         RTCSEL        LL_RCC_GetRTCClockSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_RTC_CLKSOURCE_NONE
  *         @arg @ref LL_RCC_RTC_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_RTC_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_RTC_CLKSOURCE_HSE_DIV32
  */
__STATIC_INLINE uint32_t LL_RCC_GetRTCClockSource(void)
{
  return (uint32_t)(READ_BIT(RCC->BDCR, RCC_BDCR_RTCSEL));
}

/**
  * @brief  Enable RTC
  * @rmtoll BDCR         RTCEN         LL_RCC_EnableRTC
  * @retval None
  */
__STATIC_INLINE void LL_RCC_EnableRTC(void)
{
  SET_BIT(RCC->BDCR, RCC_BDCR_RTCEN);
}

/**
  * @brief  Disable RTC
  * @rmtoll BDCR         RTCEN         LL_RCC_DisableRTC
  * @retval None
  */
__STATIC_INLINE void LL_RCC_DisableRTC(void)
{
  CLEAR_BIT(RCC->BDCR, RCC_BDCR_RTCEN);
}

/**
  * @brief  Check if RTC has been enabled or not
  * @rmtoll BDCR         RTCEN         LL_RCC_IsEnabledRTC
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsEnabledRTC(void)
{
  return (READ_BIT(RCC->BDCR, RCC_BDCR_RTCEN) == (RCC_BDCR_RTCEN));
}

/**
  * @brief  Force the Backup domain reset
  * @rmtoll BDCR         BDRST         LL_RCC_ForceBackupDomainReset
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ForceBackupDomainReset(void)
{
  SET_BIT(RCC->BDCR, RCC_BDCR_BDRST);
}

/**
  * @brief  Release the Backup domain reset
  * @rmtoll BDCR         BDRST         LL_RCC_ReleaseBackupDomainReset
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ReleaseBackupDomainReset(void)
{
  CLEAR_BIT(RCC->BDCR, RCC_BDCR_BDRST);
}

/**
  * @}
  */

/** @defgroup RCC_LL_EF_PLL PLL
  * @{
  */

/**
  * @brief  Enable PLL
  * @rmtoll CR           PLLON         LL_RCC_PLL_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL_Enable(void)
{
  SET_BIT(RCC->CR, RCC_CR_PLLON);
}

/**
  * @brief  Disable PLL
  * @note Cannot be disabled if the PLL clock is used as the system clock
  * @rmtoll CR           PLLON         LL_RCC_PLL_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL_Disable(void)
{
  CLEAR_BIT(RCC->CR, RCC_CR_PLLON);
}

/**
  * @brief  Check if PLL Ready
  * @rmtoll CR           PLLRDY        LL_RCC_PLL_IsReady
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL_IsReady(void)
{
  return (READ_BIT(RCC->CR, RCC_CR_PLLRDY) == (RCC_CR_PLLRDY));
}

/**
  * @brief  Configure PLL used for SYSCLK Domain
  * @note PLL Source and PLLM Divider can be written only when PLL,
  *       PLLSAI1 and PLLSAI2 (*) are disabled
  * @note PLLN/PLLR can be written only when PLL is disabled
  * @rmtoll PLLCFGR      PLLSRC        LL_RCC_PLL_ConfigDomain_SYS\n
  *         PLLCFGR      PLLM          LL_RCC_PLL_ConfigDomain_SYS\n
  *         PLLCFGR      PLLN          LL_RCC_PLL_ConfigDomain_SYS\n
  *         PLLCFGR      PLLR          LL_RCC_PLL_ConfigDomain_SYS
  * @param  Source This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLSOURCE_NONE
  *         @arg @ref LL_RCC_PLLSOURCE_MSI
  *         @arg @ref LL_RCC_PLLSOURCE_HSI
  *         @arg @ref LL_RCC_PLLSOURCE_HSE
  * @param  PLLM This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLM_DIV_1
  *         @arg @ref LL_RCC_PLLM_DIV_2
  *         @arg @ref LL_RCC_PLLM_DIV_3
  *         @arg @ref LL_RCC_PLLM_DIV_4
  *         @arg @ref LL_RCC_PLLM_DIV_5
  *         @arg @ref LL_RCC_PLLM_DIV_6
  *         @arg @ref LL_RCC_PLLM_DIV_7
  *         @arg @ref LL_RCC_PLLM_DIV_8
  * @param  PLLN Between 8 and 86
  * @param  PLLR This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLR_DIV_2
  *         @arg @ref LL_RCC_PLLR_DIV_4
  *         @arg @ref LL_RCC_PLLR_DIV_6
  *         @arg @ref LL_RCC_PLLR_DIV_8
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL_ConfigDomain_SYS(uint32_t Source, uint32_t PLLM, uint32_t PLLN, uint32_t PLLR)
{
  MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC | RCC_PLLCFGR_PLLM | RCC_PLLCFGR_PLLN | RCC_PLLCFGR_PLLR,
             Source | PLLM | PLLN << RCC_POSITION_PLLN | PLLR);
}

/**
  * @brief  Configure PLL used for SAI domain clock
  * @note   PLL Source and PLLM Divider can be written only when PLL,
  *         PLLSAI1 and PLLSAI2 are disabled
  * @note   PLLN/PLLP can be written only when PLL is disabled
  * @note   This  can be selected for SAI1 or SAI2
  * @rmtoll PLLCFGR      PLLSRC        LL_RCC_PLL_ConfigDomain_SAI\n
  *         PLLCFGR      PLLM          LL_RCC_PLL_ConfigDomain_SAI\n
  *         PLLCFGR      PLLN          LL_RCC_PLL_ConfigDomain_SAI\n
  *         PLLCFGR      PLLP          LL_RCC_PLL_ConfigDomain_SAI
  * @param  Source This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLSOURCE_NONE
  *         @arg @ref LL_RCC_PLLSOURCE_MSI
  *         @arg @ref LL_RCC_PLLSOURCE_HSI
  *         @arg @ref LL_RCC_PLLSOURCE_HSE
  * @param  PLLM This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLM_DIV_1
  *         @arg @ref LL_RCC_PLLM_DIV_2
  *         @arg @ref LL_RCC_PLLM_DIV_3
  *         @arg @ref LL_RCC_PLLM_DIV_4
  *         @arg @ref LL_RCC_PLLM_DIV_5
  *         @arg @ref LL_RCC_PLLM_DIV_6
  *         @arg @ref LL_RCC_PLLM_DIV_7
  *         @arg @ref LL_RCC_PLLM_DIV_8
  * @param  PLLN Between 8 and 86
  * @param  PLLP This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLP_DIV_7
  *         @arg @ref LL_RCC_PLLP_DIV_17
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL_ConfigDomain_SAI(uint32_t Source, uint32_t PLLM, uint32_t PLLN, uint32_t PLLP)
{
  MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC | RCC_PLLCFGR_PLLM | RCC_PLLCFGR_PLLN | RCC_PLLCFGR_PLLP,
             Source | PLLM | PLLN << RCC_POSITION_PLLN | PLLP);
}

/**
  * @brief  Configure PLL used for 48Mhz domain clock
  * @note PLL Source and PLLM Divider can be written only when PLL,
  *       PLLSAI1 and PLLSAI2 (*) are disabled
  * @note PLLN/PLLQ can be written only when PLL is disabled
  * @note This  can be selected for USB, RNG, SDMMC
  * @rmtoll PLLCFGR      PLLSRC        LL_RCC_PLL_ConfigDomain_48M\n
  *         PLLCFGR      PLLM          LL_RCC_PLL_ConfigDomain_48M\n
  *         PLLCFGR      PLLN          LL_RCC_PLL_ConfigDomain_48M\n
  *         PLLCFGR      PLLQ          LL_RCC_PLL_ConfigDomain_48M
  * @param  Source This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLSOURCE_NONE
  *         @arg @ref LL_RCC_PLLSOURCE_MSI
  *         @arg @ref LL_RCC_PLLSOURCE_HSI
  *         @arg @ref LL_RCC_PLLSOURCE_HSE
  * @param  PLLM This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLM_DIV_1
  *         @arg @ref LL_RCC_PLLM_DIV_2
  *         @arg @ref LL_RCC_PLLM_DIV_3
  *         @arg @ref LL_RCC_PLLM_DIV_4
  *         @arg @ref LL_RCC_PLLM_DIV_5
  *         @arg @ref LL_RCC_PLLM_DIV_6
  *         @arg @ref LL_RCC_PLLM_DIV_7
  *         @arg @ref LL_RCC_PLLM_DIV_8
  * @param  PLLN Between 8 and 86
  * @param  PLLQ This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLQ_DIV_2
  *         @arg @ref LL_RCC_PLLQ_DIV_4
  *         @arg @ref LL_RCC_PLLQ_DIV_6
  *         @arg @ref LL_RCC_PLLQ_DIV_8
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL_ConfigDomain_48M(uint32_t Source, uint32_t PLLM, uint32_t PLLN, uint32_t PLLQ)
{
  MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC | RCC_PLLCFGR_PLLM | RCC_PLLCFGR_PLLN | RCC_PLLCFGR_PLLQ,
             Source | PLLM | PLLN << RCC_POSITION_PLLN | PLLQ);
}

/**
  * @brief  Get Main PLL multiplication factor for VCO
  * @rmtoll PLLCFGR      PLLN          LL_RCC_PLL_GetN
  * @retval Between 8 and 86
  */
__STATIC_INLINE uint32_t LL_RCC_PLL_GetN(void)
{
  return (uint32_t)(READ_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLN) >>  RCC_POSITION_PLLN);
}

/**
  * @brief  Get Main PLL division factor for PLLP
  * @note   used for PLLSAI3CLK (SAI1 and SAI2 clock)
  * @rmtoll PLLCFGR      PLLP          LL_RCC_PLL_GetP
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_PLLP_DIV_7
  *         @arg @ref LL_RCC_PLLP_DIV_17
  */
__STATIC_INLINE uint32_t LL_RCC_PLL_GetP(void)
{
  return (uint32_t)(READ_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP));
}

/**
  * @brief  Get Main PLL division factor for PLLQ
  * @note used for PLL48M1CLK selected for USB, RNG, SDMMC (48 MHz clock)
  * @rmtoll PLLCFGR      PLLQ          LL_RCC_PLL_GetQ
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_PLLQ_DIV_2
  *         @arg @ref LL_RCC_PLLQ_DIV_4
  *         @arg @ref LL_RCC_PLLQ_DIV_6
  *         @arg @ref LL_RCC_PLLQ_DIV_8
  */
__STATIC_INLINE uint32_t LL_RCC_PLL_GetQ(void)
{
  return (uint32_t)(READ_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLQ));
}

/**
  * @brief  Get Main PLL division factor for PLLR
  * @note used for PLLCLK (system clock)
  * @rmtoll PLLCFGR      PLLR          LL_RCC_PLL_GetR
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_PLLR_DIV_2
  *         @arg @ref LL_RCC_PLLR_DIV_4
  *         @arg @ref LL_RCC_PLLR_DIV_6
  *         @arg @ref LL_RCC_PLLR_DIV_8
  */
__STATIC_INLINE uint32_t LL_RCC_PLL_GetR(void)
{
  return (uint32_t)(READ_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLR));
}

/**
  * @brief  Get the oscillator used as PLL clock source.
  * @rmtoll PLLCFGR      PLLSRC        LL_RCC_PLL_GetMainSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_PLLSOURCE_NONE
  *         @arg @ref LL_RCC_PLLSOURCE_MSI
  *         @arg @ref LL_RCC_PLLSOURCE_HSI
  *         @arg @ref LL_RCC_PLLSOURCE_HSE
  */
__STATIC_INLINE uint32_t LL_RCC_PLL_GetMainSource(void)
{
  return (uint32_t)(READ_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC));
}

/**
  * @brief  Get Division factor for the main PLL and other PLL
  * @rmtoll PLLCFGR      PLLM          LL_RCC_PLL_GetDivider
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_PLLM_DIV_1
  *         @arg @ref LL_RCC_PLLM_DIV_2
  *         @arg @ref LL_RCC_PLLM_DIV_3
  *         @arg @ref LL_RCC_PLLM_DIV_4
  *         @arg @ref LL_RCC_PLLM_DIV_5
  *         @arg @ref LL_RCC_PLLM_DIV_6
  *         @arg @ref LL_RCC_PLLM_DIV_7
  *         @arg @ref LL_RCC_PLLM_DIV_8
  */
__STATIC_INLINE uint32_t LL_RCC_PLL_GetDivider(void)
{
  return (uint32_t)(READ_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLM));
}

/**
  * @brief  Enable PLL output mapped on SAI domain clock
  * @rmtoll PLLCFGR      PLLPEN        LL_RCC_PLL_EnableDomain_SAI
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL_EnableDomain_SAI(void)
{
  SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLPEN);
}

/**
  * @brief  Disable PLL output mapped on SAI domain clock
  * @note Cannot be disabled if the PLL clock is used as the system
  *       clock
  * @note In order to save power, when the PLLCLK  of the PLL is
  *       not used,  should be 0
  * @rmtoll PLLCFGR      PLLPEN        LL_RCC_PLL_DisableDomain_SAI
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL_DisableDomain_SAI(void)
{
  CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLPEN);
}

/**
  * @brief  Enable PLL output mapped on 48MHz domain clock
  * @rmtoll PLLCFGR      PLLQEN        LL_RCC_PLL_EnableDomain_48M
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL_EnableDomain_48M(void)
{
  SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLQEN);
}

/**
  * @brief  Disable PLL output mapped on 48MHz domain clock
  * @note Cannot be disabled if the PLL clock is used as the system
  *       clock
  * @note In order to save power, when the PLLCLK  of the PLL is
  *       not used,  should be 0
  * @rmtoll PLLCFGR      PLLQEN        LL_RCC_PLL_DisableDomain_48M
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL_DisableDomain_48M(void)
{
  CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLQEN);
}

/**
  * @brief  Enable PLL output mapped on SYSCLK domain
  * @rmtoll PLLCFGR      PLLREN        LL_RCC_PLL_EnableDomain_SYS
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL_EnableDomain_SYS(void)
{
  SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLREN);
}

/**
  * @brief  Disable PLL output mapped on SYSCLK domain
  * @note Cannot be disabled if the PLL clock is used as the system
  *       clock
  * @note In order to save power, when the PLLCLK  of the PLL is
  *       not used, Main PLL  should be 0
  * @rmtoll PLLCFGR      PLLREN        LL_RCC_PLL_DisableDomain_SYS
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL_DisableDomain_SYS(void)
{
  CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLREN);
}

/**
  * @}
  */

/** @defgroup RCC_LL_EF_PLLSAI1 PLLSAI1
  * @{
  */

/**
  * @brief  Enable PLLSAI1
  * @rmtoll CR           PLLSAI1ON     LL_RCC_PLLSAI1_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLLSAI1_Enable(void)
{
  SET_BIT(RCC->CR, RCC_CR_PLLSAI1ON);
}

/**
  * @brief  Disable PLLSAI1
  * @rmtoll CR           PLLSAI1ON     LL_RCC_PLLSAI1_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLLSAI1_Disable(void)
{
  CLEAR_BIT(RCC->CR, RCC_CR_PLLSAI1ON);
}

/**
  * @brief  Check if PLLSAI1 Ready
  * @rmtoll CR           PLLSAI1RDY    LL_RCC_PLLSAI1_IsReady
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_PLLSAI1_IsReady(void)
{
  return (READ_BIT(RCC->CR, RCC_CR_PLLSAI1RDY) == (RCC_CR_PLLSAI1RDY));
}

/**
  * @brief  Configure PLLSAI1 used for 48Mhz domain clock
  * @note PLL Source and PLLM Divider can be written only when PLL,
  *       PLLSAI1 and PLLSAI2 (*) are disabled
  * @note PLLN/PLLQ can be written only when PLLSAI1 is disabled
  * @note This  can be selected for USB, RNG, SDMMC
  * @rmtoll PLLCFGR      PLLSRC        LL_RCC_PLLSAI1_ConfigDomain_48M\n
  *         PLLCFGR      PLLM          LL_RCC_PLLSAI1_ConfigDomain_48M\n
  *         PLLSAI1CFGR  PLLSAI1N      LL_RCC_PLLSAI1_ConfigDomain_48M\n
  *         PLLSAI1CFGR  PLLSAI1Q      LL_RCC_PLLSAI1_ConfigDomain_48M
  * @param  Source This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLSOURCE_NONE
  *         @arg @ref LL_RCC_PLLSOURCE_MSI
  *         @arg @ref LL_RCC_PLLSOURCE_HSI
  *         @arg @ref LL_RCC_PLLSOURCE_HSE
  * @param  PLLM This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLM_DIV_1
  *         @arg @ref LL_RCC_PLLM_DIV_2
  *         @arg @ref LL_RCC_PLLM_DIV_3
  *         @arg @ref LL_RCC_PLLM_DIV_4
  *         @arg @ref LL_RCC_PLLM_DIV_5
  *         @arg @ref LL_RCC_PLLM_DIV_6
  *         @arg @ref LL_RCC_PLLM_DIV_7
  *         @arg @ref LL_RCC_PLLM_DIV_8
  * @param  PLLN Between 8 and 86
  * @param  PLLQ This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLSAI1Q_DIV_2
  *         @arg @ref LL_RCC_PLLSAI1Q_DIV_4
  *         @arg @ref LL_RCC_PLLSAI1Q_DIV_6
  *         @arg @ref LL_RCC_PLLSAI1Q_DIV_8
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLLSAI1_ConfigDomain_48M(uint32_t Source, uint32_t PLLM, uint32_t PLLN, uint32_t PLLQ)
{
  MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC | RCC_PLLCFGR_PLLM, Source | PLLM);
  MODIFY_REG(RCC->PLLSAI1CFGR, RCC_PLLSAI1CFGR_PLLSAI1N | RCC_PLLSAI1CFGR_PLLSAI1Q, PLLN << RCC_POSITION_PLLSAI1N | PLLQ);
}

#if defined(RCC_PLLSAI1P_DIV_2_31_SUPPORT)
/**
  * @brief  Configure PLLSAI1 used for SAI domain clock
  * @note PLL Source and PLLM Divider can be written only when PLL,
  *       PLLSAI1 and PLLSAI2 (*) are disabled
  * @note PLLN/PLLP can be written only when PLLSAI1 is disabled
  * @note This  can be selected for SAI1 or SAI2 (*)
  * @rmtoll PLLCFGR      PLLSRC        LL_RCC_PLLSAI1_ConfigDomain_SAI\n
  *         PLLCFGR      PLLM          LL_RCC_PLLSAI1_ConfigDomain_SAI\n
  *         PLLSAI1CFGR  PLLSAI1N      LL_RCC_PLLSAI1_ConfigDomain_SAI\n
  *         PLLSAI1CFGR  PLLSAI1PDIV   LL_RCC_PLLSAI1_ConfigDomain_SAI
  * @param  Source This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLSOURCE_NONE
  *         @arg @ref LL_RCC_PLLSOURCE_MSI
  *         @arg @ref LL_RCC_PLLSOURCE_HSI
  *         @arg @ref LL_RCC_PLLSOURCE_HSE
  * @param  PLLM This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLM_DIV_1
  *         @arg @ref LL_RCC_PLLM_DIV_2
  *         @arg @ref LL_RCC_PLLM_DIV_3
  *         @arg @ref LL_RCC_PLLM_DIV_4
  *         @arg @ref LL_RCC_PLLM_DIV_5
  *         @arg @ref LL_RCC_PLLM_DIV_6
  *         @arg @ref LL_RCC_PLLM_DIV_7
  *         @arg @ref LL_RCC_PLLM_DIV_8
  * @param  PLLN Between 8 and 86
  * @param  PLLP This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_2
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_3
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_4
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_5
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_6
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_7
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_8
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_9
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_10
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_11
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_12
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_13
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_14
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_15
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_16
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_17
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_18
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_19
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_20
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_21
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_22
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_23
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_24
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_25
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_26
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_27
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_28
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_29
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_30
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_31
  * @retval None
  */
#else
/**
  * @brief  Configure PLLSAI1 used for SAI domain clock
  * @note PLL Source and PLLM Divider can be written only when PLL,
  *       PLLSAI1 and PLLSAI2 (*) are disabled
  * @note PLLN/PLLP can be written only when PLLSAI1 is disabled
  * @note This  can be selected for SAI1 or SAI2 (*)
  * @rmtoll PLLCFGR      PLLSRC        LL_RCC_PLLSAI1_ConfigDomain_SAI\n
  *         PLLCFGR      PLLM          LL_RCC_PLLSAI1_ConfigDomain_SAI\n
  *         PLLSAI1CFGR  PLLSAI1N      LL_RCC_PLLSAI1_ConfigDomain_SAI\n
  *         PLLSAI1CFGR  PLLSAI1P      LL_RCC_PLLSAI1_ConfigDomain_SAI
  * @param  Source This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLSOURCE_NONE
  *         @arg @ref LL_RCC_PLLSOURCE_MSI
  *         @arg @ref LL_RCC_PLLSOURCE_HSI
  *         @arg @ref LL_RCC_PLLSOURCE_HSE
  * @param  PLLM This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLM_DIV_1
  *         @arg @ref LL_RCC_PLLM_DIV_2
  *         @arg @ref LL_RCC_PLLM_DIV_3
  *         @arg @ref LL_RCC_PLLM_DIV_4
  *         @arg @ref LL_RCC_PLLM_DIV_5
  *         @arg @ref LL_RCC_PLLM_DIV_6
  *         @arg @ref LL_RCC_PLLM_DIV_7
  *         @arg @ref LL_RCC_PLLM_DIV_8
  * @param  PLLN Between 8 and 86
  * @param  PLLP This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_7
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_17
  * @retval None
  */
#endif /* RCC_PLLSAI1P_DIV_2_31_SUPPORT */
__STATIC_INLINE void LL_RCC_PLLSAI1_ConfigDomain_SAI(uint32_t Source, uint32_t PLLM, uint32_t PLLN, uint32_t PLLP)
{
  MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC | RCC_PLLCFGR_PLLM, Source | PLLM);
#if defined(RCC_PLLSAI1P_DIV_2_31_SUPPORT)
  MODIFY_REG(RCC->PLLSAI1CFGR, RCC_PLLSAI1CFGR_PLLSAI1N | RCC_PLLSAI1CFGR_PLLSAI1PDIV,
             PLLN << RCC_POSITION_PLLSAI1N | PLLP);
#else
  MODIFY_REG(RCC->PLLSAI1CFGR, RCC_PLLSAI1CFGR_PLLSAI1N | RCC_PLLSAI1CFGR_PLLSAI1P, PLLN << RCC_POSITION_PLLSAI1N | PLLP);
#endif /* RCC_PLLSAI1P_DIV_2_31_SUPPORT */
}

/**
  * @brief  Configure PLLSAI1 used for ADC domain clock
  * @note PLL Source and PLLM Divider can be written only when PLL,
  *       PLLSAI1 and PLLSAI2 (*) are disabled
  * @note PLLN/PLLR can be written only when PLLSAI1 is disabled
  * @note This  can be selected for ADC
  * @rmtoll PLLCFGR      PLLSRC        LL_RCC_PLLSAI1_ConfigDomain_ADC\n
  *         PLLCFGR      PLLM          LL_RCC_PLLSAI1_ConfigDomain_ADC\n
  *         PLLSAI1CFGR  PLLSAI1N      LL_RCC_PLLSAI1_ConfigDomain_ADC\n
  *         PLLSAI1CFGR  PLLSAI1R      LL_RCC_PLLSAI1_ConfigDomain_ADC
  * @param  Source This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLSOURCE_NONE
  *         @arg @ref LL_RCC_PLLSOURCE_MSI
  *         @arg @ref LL_RCC_PLLSOURCE_HSI
  *         @arg @ref LL_RCC_PLLSOURCE_HSE
  * @param  PLLM This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLM_DIV_1
  *         @arg @ref LL_RCC_PLLM_DIV_2
  *         @arg @ref LL_RCC_PLLM_DIV_3
  *         @arg @ref LL_RCC_PLLM_DIV_4
  *         @arg @ref LL_RCC_PLLM_DIV_5
  *         @arg @ref LL_RCC_PLLM_DIV_6
  *         @arg @ref LL_RCC_PLLM_DIV_7
  *         @arg @ref LL_RCC_PLLM_DIV_8
  * @param  PLLN Between 8 and 86
  * @param  PLLR This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLSAI1R_DIV_2
  *         @arg @ref LL_RCC_PLLSAI1R_DIV_4
  *         @arg @ref LL_RCC_PLLSAI1R_DIV_6
  *         @arg @ref LL_RCC_PLLSAI1R_DIV_8
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLLSAI1_ConfigDomain_ADC(uint32_t Source, uint32_t PLLM, uint32_t PLLN, uint32_t PLLR)
{
  MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC | RCC_PLLCFGR_PLLM, Source | PLLM);
  MODIFY_REG(RCC->PLLSAI1CFGR, RCC_PLLSAI1CFGR_PLLSAI1N | RCC_PLLSAI1CFGR_PLLSAI1R, PLLN << RCC_POSITION_PLLSAI1N | PLLR);
}

/**
  * @brief  Get SAI1PLL multiplication factor for VCO
  * @rmtoll PLLSAI1CFGR  PLLSAI1N      LL_RCC_PLLSAI1_GetN
  * @retval Between 8 and 86
  */
__STATIC_INLINE uint32_t LL_RCC_PLLSAI1_GetN(void)
{
  return (uint32_t)(READ_BIT(RCC->PLLSAI1CFGR, RCC_PLLSAI1CFGR_PLLSAI1N) >> RCC_POSITION_PLLSAI1N);
}

#if defined(RCC_PLLSAI1P_DIV_2_31_SUPPORT)
/**
  * @brief  Get SAI1PLL division factor for PLLSAI1P
  * @note used for PLLSAI1CLK (SAI1 or SAI2 (*) clock).
  * @rmtoll PLLSAI1CFGR  PLLSAI1PDIV      LL_RCC_PLLSAI1_GetP
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_2
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_3
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_4
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_5
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_6
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_7
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_8
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_9
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_10
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_11
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_12
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_13
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_14
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_15
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_16
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_17
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_18
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_19
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_20
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_21
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_22
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_23
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_24
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_25
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_26
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_27
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_28
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_29
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_30
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_31
  */
#else
/**
  * @brief  Get SAI1PLL division factor for PLLSAI1P
  * @note used for PLLSAI1CLK (SAI1 or SAI2 (*) clock).
  * @rmtoll PLLSAI1CFGR  PLLSAI1P      LL_RCC_PLLSAI1_GetP
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_7
  *         @arg @ref LL_RCC_PLLSAI1P_DIV_17
  */
#endif /* RCC_PLLSAI1P_DIV_2_31_SUPPORT */
__STATIC_INLINE uint32_t LL_RCC_PLLSAI1_GetP(void)
{
#if defined(RCC_PLLSAI1P_DIV_2_31_SUPPORT)
  return (uint32_t)(READ_BIT(RCC->PLLSAI1CFGR, RCC_PLLSAI1CFGR_PLLSAI1PDIV));
#else
  return (uint32_t)(READ_BIT(RCC->PLLSAI1CFGR, RCC_PLLSAI1CFGR_PLLSAI1P));
#endif /* RCC_PLLSAI1P_DIV_2_31_SUPPORT */
}

/**
  * @brief  Get SAI1PLL division factor for PLLSAI1Q
  * @note used PLL48M2CLK selected for USB, RNG, SDMMC (48 MHz clock)
  * @rmtoll PLLSAI1CFGR  PLLSAI1Q      LL_RCC_PLLSAI1_GetQ
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_PLLSAI1Q_DIV_2
  *         @arg @ref LL_RCC_PLLSAI1Q_DIV_4
  *         @arg @ref LL_RCC_PLLSAI1Q_DIV_6
  *         @arg @ref LL_RCC_PLLSAI1Q_DIV_8
  */
__STATIC_INLINE uint32_t LL_RCC_PLLSAI1_GetQ(void)
{
  return (uint32_t)(READ_BIT(RCC->PLLSAI1CFGR, RCC_PLLSAI1CFGR_PLLSAI1Q));
}

/**
  * @brief  Get PLLSAI1 division factor for PLLSAIR
  * @note used for PLLADC1CLK (ADC clock)
  * @rmtoll PLLSAI1CFGR  PLLSAI1R      LL_RCC_PLLSAI1_GetR
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_PLLSAI1R_DIV_2
  *         @arg @ref LL_RCC_PLLSAI1R_DIV_4
  *         @arg @ref LL_RCC_PLLSAI1R_DIV_6
  *         @arg @ref LL_RCC_PLLSAI1R_DIV_8
  */
__STATIC_INLINE uint32_t LL_RCC_PLLSAI1_GetR(void)
{
  return (uint32_t)(READ_BIT(RCC->PLLSAI1CFGR, RCC_PLLSAI1CFGR_PLLSAI1R));
}

/**
  * @brief  Enable PLLSAI1 output mapped on SAI domain clock
  * @rmtoll PLLSAI1CFGR  PLLSAI1PEN    LL_RCC_PLLSAI1_EnableDomain_SAI
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLLSAI1_EnableDomain_SAI(void)
{
  SET_BIT(RCC->PLLSAI1CFGR, RCC_PLLSAI1CFGR_PLLSAI1PEN);
}

/**
  * @brief  Disable PLLSAI1 output mapped on SAI domain clock
  * @note In order to save power, when  of the PLLSAI1 is
  *       not used,  should be 0
  * @rmtoll PLLSAI1CFGR  PLLSAI1PEN    LL_RCC_PLLSAI1_DisableDomain_SAI
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLLSAI1_DisableDomain_SAI(void)
{
  CLEAR_BIT(RCC->PLLSAI1CFGR, RCC_PLLSAI1CFGR_PLLSAI1PEN);
}

/**
  * @brief  Enable PLLSAI1 output mapped on 48MHz domain clock
  * @rmtoll PLLSAI1CFGR  PLLSAI1QEN    LL_RCC_PLLSAI1_EnableDomain_48M
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLLSAI1_EnableDomain_48M(void)
{
  SET_BIT(RCC->PLLSAI1CFGR, RCC_PLLSAI1CFGR_PLLSAI1QEN);
}

/**
  * @brief  Disable PLLSAI1 output mapped on 48MHz domain clock
  * @note In order to save power, when  of the PLLSAI1 is
  *       not used,  should be 0
  * @rmtoll PLLSAI1CFGR  PLLSAI1QEN    LL_RCC_PLLSAI1_DisableDomain_48M
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLLSAI1_DisableDomain_48M(void)
{
  CLEAR_BIT(RCC->PLLSAI1CFGR, RCC_PLLSAI1CFGR_PLLSAI1QEN);
}

/**
  * @brief  Enable PLLSAI1 output mapped on ADC domain clock
  * @rmtoll PLLSAI1CFGR  PLLSAI1REN    LL_RCC_PLLSAI1_EnableDomain_ADC
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLLSAI1_EnableDomain_ADC(void)
{
  SET_BIT(RCC->PLLSAI1CFGR, RCC_PLLSAI1CFGR_PLLSAI1REN);
}

/**
  * @brief  Disable PLLSAI1 output mapped on ADC domain clock
  * @note In order to save power, when  of the PLLSAI1 is
  *       not used, Main PLLSAI1  should be 0
  * @rmtoll PLLSAI1CFGR  PLLSAI1REN    LL_RCC_PLLSAI1_DisableDomain_ADC
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLLSAI1_DisableDomain_ADC(void)
{
  CLEAR_BIT(RCC->PLLSAI1CFGR, RCC_PLLSAI1CFGR_PLLSAI1REN);
}

/**
  * @}
  */

#if defined(RCC_PLLSAI2_SUPPORT)
/** @defgroup RCC_LL_EF_PLLSAI2 PLLSAI2
  * @{
  */

/**
  * @brief  Enable PLLSAI2
  * @rmtoll CR           PLLSAI2ON     LL_RCC_PLLSAI2_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLLSAI2_Enable(void)
{
  SET_BIT(RCC->CR, RCC_CR_PLLSAI2ON);
}

/**
  * @brief  Disable PLLSAI2
  * @rmtoll CR           PLLSAI2ON     LL_RCC_PLLSAI2_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLLSAI2_Disable(void)
{
  CLEAR_BIT(RCC->CR, RCC_CR_PLLSAI2ON);
}

/**
  * @brief  Check if PLLSAI2 Ready
  * @rmtoll CR           PLLSAI2RDY    LL_RCC_PLLSAI2_IsReady
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_PLLSAI2_IsReady(void)
{
  return (READ_BIT(RCC->CR, RCC_CR_PLLSAI2RDY) == (RCC_CR_PLLSAI2RDY));
}

/**
  * @brief  Configure PLLSAI2 used for SAI domain clock
  * @note PLL Source and PLLM Divider can be written only when PLL,
  *       PLLSAI2 and PLLSAI2 are disabled
  * @note PLLN/PLLP can be written only when PLLSAI2 is disabled
  * @note This  can be selected for SAI1 or SAI2
  * @rmtoll PLLCFGR      PLLSRC        LL_RCC_PLLSAI2_ConfigDomain_SAI\n
  *         PLLCFGR      PLLM          LL_RCC_PLLSAI2_ConfigDomain_SAI\n
  *         PLLSAI2CFGR  PLLSAI2N      LL_RCC_PLLSAI2_ConfigDomain_SAI\n
  *         PLLSAI2CFGR  PLLSAI2P      LL_RCC_PLLSAI2_ConfigDomain_SAI
  * @param  Source This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLSOURCE_NONE
  *         @arg @ref LL_RCC_PLLSOURCE_MSI
  *         @arg @ref LL_RCC_PLLSOURCE_HSI
  *         @arg @ref LL_RCC_PLLSOURCE_HSE
  * @param  PLLM This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLM_DIV_1
  *         @arg @ref LL_RCC_PLLM_DIV_2
  *         @arg @ref LL_RCC_PLLM_DIV_3
  *         @arg @ref LL_RCC_PLLM_DIV_4
  *         @arg @ref LL_RCC_PLLM_DIV_5
  *         @arg @ref LL_RCC_PLLM_DIV_6
  *         @arg @ref LL_RCC_PLLM_DIV_7
  *         @arg @ref LL_RCC_PLLM_DIV_8
  * @param  PLLN Between 8 and 86
  * @param  PLLP This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLSAI2P_DIV_7
  *         @arg @ref LL_RCC_PLLSAI2P_DIV_17
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLLSAI2_ConfigDomain_SAI(uint32_t Source, uint32_t PLLM, uint32_t PLLN, uint32_t PLLP)
{
  MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC | RCC_PLLCFGR_PLLM, Source | PLLM);
  MODIFY_REG(RCC->PLLSAI2CFGR, RCC_PLLSAI2CFGR_PLLSAI2N | RCC_PLLSAI2CFGR_PLLSAI2P, PLLN << RCC_POSITION_PLLSAI2N | PLLP);
}

/**
  * @brief  Configure PLLSAI2 used for ADC domain clock
  * @note PLL Source and PLLM Divider can be written only when PLL,
  *       PLLSAI2 and PLLSAI2 are disabled
  * @note PLLN/PLLR can be written only when PLLSAI2 is disabled
  * @note This  can be selected for ADC
  * @rmtoll PLLCFGR      PLLSRC        LL_RCC_PLLSAI2_ConfigDomain_ADC\n
  *         PLLCFGR      PLLM          LL_RCC_PLLSAI2_ConfigDomain_ADC\n
  *         PLLSAI2CFGR  PLLSAI2N      LL_RCC_PLLSAI2_ConfigDomain_ADC\n
  *         PLLSAI2CFGR  PLLSAI2R      LL_RCC_PLLSAI2_ConfigDomain_ADC
  * @param  Source This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLSOURCE_NONE
  *         @arg @ref LL_RCC_PLLSOURCE_MSI
  *         @arg @ref LL_RCC_PLLSOURCE_HSI
  *         @arg @ref LL_RCC_PLLSOURCE_HSE
  * @param  PLLM This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLM_DIV_1
  *         @arg @ref LL_RCC_PLLM_DIV_2
  *         @arg @ref LL_RCC_PLLM_DIV_3
  *         @arg @ref LL_RCC_PLLM_DIV_4
  *         @arg @ref LL_RCC_PLLM_DIV_5
  *         @arg @ref LL_RCC_PLLM_DIV_6
  *         @arg @ref LL_RCC_PLLM_DIV_7
  *         @arg @ref LL_RCC_PLLM_DIV_8
  * @param  PLLN Between 8 and 86
  * @param  PLLR This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLSAI2R_DIV_2
  *         @arg @ref LL_RCC_PLLSAI2R_DIV_4
  *         @arg @ref LL_RCC_PLLSAI2R_DIV_6
  *         @arg @ref LL_RCC_PLLSAI2R_DIV_8
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLLSAI2_ConfigDomain_ADC(uint32_t Source, uint32_t PLLM, uint32_t PLLN, uint32_t PLLR)
{
  MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC | RCC_PLLCFGR_PLLM, Source | PLLM);
  MODIFY_REG(RCC->PLLSAI2CFGR, RCC_PLLSAI2CFGR_PLLSAI2N | RCC_PLLSAI2CFGR_PLLSAI2R, PLLN << RCC_POSITION_PLLSAI2N | PLLR);
}

/**
  * @brief  Get SAI2PLL multiplication factor for VCO
  * @rmtoll PLLSAI2CFGR  PLLSAI2N      LL_RCC_PLLSAI2_GetN
  * @retval Between 8 and 86
  */
__STATIC_INLINE uint32_t LL_RCC_PLLSAI2_GetN(void)
{
  return (uint32_t)(READ_BIT(RCC->PLLSAI2CFGR, RCC_PLLSAI2CFGR_PLLSAI2N) >> RCC_POSITION_PLLSAI2N);
}

/**
  * @brief  Get SAI2PLL division factor for PLLSAI2P
  * @note   used for PLLSAI2CLK (SAI1 or SAI2 clock).
  * @rmtoll PLLSAI2CFGR  PLLSAI2P      LL_RCC_PLLSAI2_GetP
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_PLLSAI2P_DIV_7
  *         @arg @ref LL_RCC_PLLSAI2P_DIV_17
  */
__STATIC_INLINE uint32_t LL_RCC_PLLSAI2_GetP(void)
{
  return (uint32_t)(READ_BIT(RCC->PLLSAI2CFGR, RCC_PLLSAI2CFGR_PLLSAI2P));
}

/**
  * @brief  Get SAI2PLL division factor for PLLSAI2R
  * @note used for PLLADC2CLK (ADC clock)
  * @rmtoll PLLSAI2CFGR  PLLSAI2R      LL_RCC_PLLSAI2_GetR
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_PLLSAI2R_DIV_2
  *         @arg @ref LL_RCC_PLLSAI2R_DIV_4
  *         @arg @ref LL_RCC_PLLSAI2R_DIV_6
  *         @arg @ref LL_RCC_PLLSAI2R_DIV_8
  */
__STATIC_INLINE uint32_t LL_RCC_PLLSAI2_GetR(void)
{
  return (uint32_t)(READ_BIT(RCC->PLLSAI2CFGR, RCC_PLLSAI2CFGR_PLLSAI2R));
}

/**
  * @brief  Enable PLLSAI2 output mapped on SAI domain clock
  * @rmtoll PLLSAI2CFGR  PLLSAI2PEN    LL_RCC_PLLSAI2_EnableDomain_SAI
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLLSAI2_EnableDomain_SAI(void)
{
  SET_BIT(RCC->PLLSAI2CFGR, RCC_PLLSAI2CFGR_PLLSAI2PEN);
}

/**
  * @brief  Disable PLLSAI2 output mapped on SAI domain clock
  * @note In order to save power, when  of the PLLSAI2 is
  *       not used,  should be 0
  * @rmtoll PLLSAI2CFGR  PLLSAI2PEN    LL_RCC_PLLSAI2_DisableDomain_SAI
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLLSAI2_DisableDomain_SAI(void)
{
  CLEAR_BIT(RCC->PLLSAI2CFGR, RCC_PLLSAI2CFGR_PLLSAI2PEN);
}

/**
  * @brief  Enable PLLSAI2 output mapped on ADC domain clock
  * @rmtoll PLLSAI2CFGR  PLLSAI2REN    LL_RCC_PLLSAI2_EnableDomain_ADC
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLLSAI2_EnableDomain_ADC(void)
{
  SET_BIT(RCC->PLLSAI2CFGR, RCC_PLLSAI2CFGR_PLLSAI2REN);
}

/**
  * @brief  Disable PLLSAI2 output mapped on ADC domain clock
  * @note In order to save power, when  of the PLLSAI2 is
  *       not used, Main PLLSAI2  should be 0
  * @rmtoll PLLSAI2CFGR  PLLSAI2REN    LL_RCC_PLLSAI2_DisableDomain_ADC
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLLSAI2_DisableDomain_ADC(void)
{
  CLEAR_BIT(RCC->PLLSAI2CFGR, RCC_PLLSAI2CFGR_PLLSAI2REN);
}

/**
  * @}
  */

#endif /* RCC_PLLSAI2_SUPPORT */
/** @defgroup RCC_LL_EF_FLAG_Management FLAG Management
  * @{
  */

/**
  * @brief  Clear LSI ready interrupt flag
  * @rmtoll CICR         LSIRDYC       LL_RCC_ClearFlag_LSIRDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ClearFlag_LSIRDY(void)
{
  SET_BIT(RCC->CICR, RCC_CICR_LSIRDYC);
}

/**
  * @brief  Clear LSE ready interrupt flag
  * @rmtoll CICR         LSERDYC       LL_RCC_ClearFlag_LSERDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ClearFlag_LSERDY(void)
{
  SET_BIT(RCC->CICR, RCC_CICR_LSERDYC);
}

/**
  * @brief  Clear MSI ready interrupt flag
  * @rmtoll CICR         MSIRDYC       LL_RCC_ClearFlag_MSIRDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ClearFlag_MSIRDY(void)
{
  SET_BIT(RCC->CICR, RCC_CICR_MSIRDYC);
}

/**
  * @brief  Clear HSI ready interrupt flag
  * @rmtoll CICR         HSIRDYC       LL_RCC_ClearFlag_HSIRDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ClearFlag_HSIRDY(void)
{
  SET_BIT(RCC->CICR, RCC_CICR_HSIRDYC);
}

/**
  * @brief  Clear HSE ready interrupt flag
  * @rmtoll CICR         HSERDYC       LL_RCC_ClearFlag_HSERDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ClearFlag_HSERDY(void)
{
  SET_BIT(RCC->CICR, RCC_CICR_HSERDYC);
}

/**
  * @brief  Clear PLL ready interrupt flag
  * @rmtoll CICR         PLLRDYC       LL_RCC_ClearFlag_PLLRDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ClearFlag_PLLRDY(void)
{
  SET_BIT(RCC->CICR, RCC_CICR_PLLRDYC);
}

/**
  * @brief  Clear PLLSAI1 ready interrupt flag
  * @rmtoll CICR         PLLSAI1RDYC   LL_RCC_ClearFlag_PLLSAI1RDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ClearFlag_PLLSAI1RDY(void)
{
  SET_BIT(RCC->CICR, RCC_CICR_PLLSAI1RDYC);
}

#if defined(RCC_PLLSAI2_SUPPORT)
/**
  * @brief  Clear PLLSAI1 ready interrupt flag
  * @rmtoll CICR         PLLSAI2RDYC   LL_RCC_ClearFlag_PLLSAI2RDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ClearFlag_PLLSAI2RDY(void)
{
  SET_BIT(RCC->CICR, RCC_CICR_PLLSAI2RDYC);
}

#endif /* RCC_PLLSAI2_SUPPORT */
/**
  * @brief  Clear Clock security system interrupt flag
  * @rmtoll CICR         CSSC          LL_RCC_ClearFlag_HSECSS
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ClearFlag_HSECSS(void)
{
  SET_BIT(RCC->CICR, RCC_CICR_CSSC);
}

/**
  * @brief  Clear LSE Clock security system interrupt flag
  * @rmtoll CICR         LSECSSC       LL_RCC_ClearFlag_LSECSS
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ClearFlag_LSECSS(void)
{
  SET_BIT(RCC->CICR, RCC_CICR_LSECSSC);
}

/**
  * @brief  Check if LSI ready interrupt occurred or not
  * @rmtoll CIFR         LSIRDYF       LL_RCC_IsActiveFlag_LSIRDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_LSIRDY(void)
{
  return (READ_BIT(RCC->CIFR, RCC_CIFR_LSIRDYF) == (RCC_CIFR_LSIRDYF));
}

/**
  * @brief  Check if LSE ready interrupt occurred or not
  * @rmtoll CIFR         LSERDYF       LL_RCC_IsActiveFlag_LSERDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_LSERDY(void)
{
  return (READ_BIT(RCC->CIFR, RCC_CIFR_LSERDYF) == (RCC_CIFR_LSERDYF));
}

/**
  * @brief  Check if MSI ready interrupt occurred or not
  * @rmtoll CIFR         MSIRDYF       LL_RCC_IsActiveFlag_MSIRDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_MSIRDY(void)
{
  return (READ_BIT(RCC->CIFR, RCC_CIFR_MSIRDYF) == (RCC_CIFR_MSIRDYF));
}

/**
  * @brief  Check if HSI ready interrupt occurred or not
  * @rmtoll CIFR         HSIRDYF       LL_RCC_IsActiveFlag_HSIRDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_HSIRDY(void)
{
  return (READ_BIT(RCC->CIFR, RCC_CIFR_HSIRDYF) == (RCC_CIFR_HSIRDYF));
}

/**
  * @brief  Check if HSE ready interrupt occurred or not
  * @rmtoll CIFR         HSERDYF       LL_RCC_IsActiveFlag_HSERDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_HSERDY(void)
{
  return (READ_BIT(RCC->CIFR, RCC_CIFR_HSERDYF) == (RCC_CIFR_HSERDYF));
}

/**
  * @brief  Check if PLL ready interrupt occurred or not
  * @rmtoll CIFR         PLLRDYF       LL_RCC_IsActiveFlag_PLLRDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_PLLRDY(void)
{
  return (READ_BIT(RCC->CIFR, RCC_CIFR_PLLRDYF) == (RCC_CIFR_PLLRDYF));
}

/**
  * @brief  Check if PLLSAI1 ready interrupt occurred or not
  * @rmtoll CIFR         PLLSAI1RDYF   LL_RCC_IsActiveFlag_PLLSAI1RDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_PLLSAI1RDY(void)
{
  return (READ_BIT(RCC->CIFR, RCC_CIFR_PLLSAI1RDYF) == (RCC_CIFR_PLLSAI1RDYF));
}

#if defined(RCC_PLLSAI2_SUPPORT)
/**
  * @brief  Check if PLLSAI1 ready interrupt occurred or not
  * @rmtoll CIFR         PLLSAI2RDYF   LL_RCC_IsActiveFlag_PLLSAI2RDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_PLLSAI2RDY(void)
{
  return (READ_BIT(RCC->CIFR, RCC_CIFR_PLLSAI2RDYF) == (RCC_CIFR_PLLSAI2RDYF));
}

#endif /* RCC_PLLSAI2_SUPPORT */
/**
  * @brief  Check if Clock security system interrupt occurred or not
  * @rmtoll CIFR         CSSF          LL_RCC_IsActiveFlag_HSECSS
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_HSECSS(void)
{
  return (READ_BIT(RCC->CIFR, RCC_CIFR_CSSF) == (RCC_CIFR_CSSF));
}

/**
  * @brief  Check if LSE Clock security system interrupt occurred or not
  * @rmtoll CIFR         LSECSSF       LL_RCC_IsActiveFlag_LSECSS
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_LSECSS(void)
{
  return (READ_BIT(RCC->CIFR, RCC_CIFR_LSECSSF) == (RCC_CIFR_LSECSSF));
}


/**
  * @brief  Check if RCC flag FW reset is set or not.
  * @rmtoll CSR          FWRSTF        LL_RCC_IsActiveFlag_FWRST
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_FWRST(void)
{
  return (READ_BIT(RCC->CSR, RCC_CSR_FWRSTF) == (RCC_CSR_FWRSTF));
}

/**
  * @brief  Check if RCC flag Independent Watchdog reset is set or not.
  * @rmtoll CSR          IWDGRSTF      LL_RCC_IsActiveFlag_IWDGRST
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_IWDGRST(void)
{
  return (READ_BIT(RCC->CSR, RCC_CSR_IWDGRSTF) == (RCC_CSR_IWDGRSTF));
}

/**
  * @brief  Check if RCC flag Low Power reset is set or not.
  * @rmtoll CSR          LPWRRSTF      LL_RCC_IsActiveFlag_LPWRRST
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_LPWRRST(void)
{
  return (READ_BIT(RCC->CSR, RCC_CSR_LPWRRSTF) == (RCC_CSR_LPWRRSTF));
}

/**
  * @brief  Check if RCC flag is set or not.
  * @rmtoll CSR          OBLRSTF       LL_RCC_IsActiveFlag_OBLRST
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_OBLRST(void)
{
  return (READ_BIT(RCC->CSR, RCC_CSR_OBLRSTF) == (RCC_CSR_OBLRSTF));
}

/**
  * @brief  Check if RCC flag Pin reset is set or not.
  * @rmtoll CSR          PINRSTF       LL_RCC_IsActiveFlag_PINRST
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_PINRST(void)
{
  return (READ_BIT(RCC->CSR, RCC_CSR_PINRSTF) == (RCC_CSR_PINRSTF));
}

/**
  * @brief  Check if RCC flag Software reset is set or not.
  * @rmtoll CSR          SFTRSTF       LL_RCC_IsActiveFlag_SFTRST
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_SFTRST(void)
{
  return (READ_BIT(RCC->CSR, RCC_CSR_SFTRSTF) == (RCC_CSR_SFTRSTF));
}

/**
  * @brief  Check if RCC flag Window Watchdog reset is set or not.
  * @rmtoll CSR          WWDGRSTF      LL_RCC_IsActiveFlag_WWDGRST
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_WWDGRST(void)
{
  return (READ_BIT(RCC->CSR, RCC_CSR_WWDGRSTF) == (RCC_CSR_WWDGRSTF));
}

/**
  * @brief  Check if RCC flag BOR reset is set or not.
  * @rmtoll CSR          BORRSTF       LL_RCC_IsActiveFlag_BORRST
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_BORRST(void)
{
  return (READ_BIT(RCC->CSR, RCC_CSR_BORRSTF) == (RCC_CSR_BORRSTF));
}

/**
  * @brief  Set RMVF bit to clear the reset flags.
  * @rmtoll CSR          RMVF          LL_RCC_ClearResetFlags
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ClearResetFlags(void)
{
  SET_BIT(RCC->CSR, RCC_CSR_RMVF);
}

/**
  * @}
  */

/** @defgroup RCC_LL_EF_IT_Management IT Management
  * @{
  */

/**
  * @brief  Enable LSI ready interrupt
  * @rmtoll CIER         LSIRDYIE      LL_RCC_EnableIT_LSIRDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_EnableIT_LSIRDY(void)
{
  SET_BIT(RCC->CIER, RCC_CIER_LSIRDYIE);
}

/**
  * @brief  Enable LSE ready interrupt
  * @rmtoll CIER         LSERDYIE      LL_RCC_EnableIT_LSERDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_EnableIT_LSERDY(void)
{
  SET_BIT(RCC->CIER, RCC_CIER_LSERDYIE);
}

/**
  * @brief  Enable MSI ready interrupt
  * @rmtoll CIER         MSIRDYIE      LL_RCC_EnableIT_MSIRDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_EnableIT_MSIRDY(void)
{
  SET_BIT(RCC->CIER, RCC_CIER_MSIRDYIE);
}

/**
  * @brief  Enable HSI ready interrupt
  * @rmtoll CIER         HSIRDYIE      LL_RCC_EnableIT_HSIRDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_EnableIT_HSIRDY(void)
{
  SET_BIT(RCC->CIER, RCC_CIER_HSIRDYIE);
}

/**
  * @brief  Enable HSE ready interrupt
  * @rmtoll CIER         HSERDYIE      LL_RCC_EnableIT_HSERDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_EnableIT_HSERDY(void)
{
  SET_BIT(RCC->CIER, RCC_CIER_HSERDYIE);
}

/**
  * @brief  Enable PLL ready interrupt
  * @rmtoll CIER         PLLRDYIE      LL_RCC_EnableIT_PLLRDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_EnableIT_PLLRDY(void)
{
  SET_BIT(RCC->CIER, RCC_CIER_PLLRDYIE);
}

/**
  * @brief  Enable PLLSAI1 ready interrupt
  * @rmtoll CIER         PLLSAI1RDYIE  LL_RCC_EnableIT_PLLSAI1RDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_EnableIT_PLLSAI1RDY(void)
{
  SET_BIT(RCC->CIER, RCC_CIER_PLLSAI1RDYIE);
}

#if defined(RCC_PLLSAI2_SUPPORT)
/**
  * @brief  Enable PLLSAI2 ready interrupt
  * @rmtoll CIER         PLLSAI2RDYIE  LL_RCC_EnableIT_PLLSAI2RDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_EnableIT_PLLSAI2RDY(void)
{
  SET_BIT(RCC->CIER, RCC_CIER_PLLSAI2RDYIE);
}

#endif /* RCC_PLLSAI2_SUPPORT */
/**
  * @brief  Enable LSE clock security system interrupt
  * @rmtoll CIER         LSECSSIE      LL_RCC_EnableIT_LSECSS
  * @retval None
  */
__STATIC_INLINE void LL_RCC_EnableIT_LSECSS(void)
{
  SET_BIT(RCC->CIER, RCC_CIER_LSECSSIE);
}

/**
  * @brief  Disable LSI ready interrupt
  * @rmtoll CIER         LSIRDYIE      LL_RCC_DisableIT_LSIRDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_DisableIT_LSIRDY(void)
{
  CLEAR_BIT(RCC->CIER, RCC_CIER_LSIRDYIE);
}

/**
  * @brief  Disable LSE ready interrupt
  * @rmtoll CIER         LSERDYIE      LL_RCC_DisableIT_LSERDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_DisableIT_LSERDY(void)
{
  CLEAR_BIT(RCC->CIER, RCC_CIER_LSERDYIE);
}

/**
  * @brief  Disable MSI ready interrupt
  * @rmtoll CIER         MSIRDYIE      LL_RCC_DisableIT_MSIRDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_DisableIT_MSIRDY(void)
{
  CLEAR_BIT(RCC->CIER, RCC_CIER_MSIRDYIE);
}

/**
  * @brief  Disable HSI ready interrupt
  * @rmtoll CIER         HSIRDYIE      LL_RCC_DisableIT_HSIRDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_DisableIT_HSIRDY(void)
{
  CLEAR_BIT(RCC->CIER, RCC_CIER_HSIRDYIE);
}

/**
  * @brief  Disable HSE ready interrupt
  * @rmtoll CIER         HSERDYIE      LL_RCC_DisableIT_HSERDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_DisableIT_HSERDY(void)
{
  CLEAR_BIT(RCC->CIER, RCC_CIER_HSERDYIE);
}

/**
  * @brief  Disable PLL ready interrupt
  * @rmtoll CIER         PLLRDYIE      LL_RCC_DisableIT_PLLRDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_DisableIT_PLLRDY(void)
{
  CLEAR_BIT(RCC->CIER, RCC_CIER_PLLRDYIE);
}

/**
  * @brief  Disable PLLSAI1 ready interrupt
  * @rmtoll CIER         PLLSAI1RDYIE  LL_RCC_DisableIT_PLLSAI1RDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_DisableIT_PLLSAI1RDY(void)
{
  CLEAR_BIT(RCC->CIER, RCC_CIER_PLLSAI1RDYIE);
}

#if defined(RCC_PLLSAI2_SUPPORT)
/**
  * @brief  Disable PLLSAI2 ready interrupt
  * @rmtoll CIER         PLLSAI2RDYIE  LL_RCC_DisableIT_PLLSAI2RDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_DisableIT_PLLSAI2RDY(void)
{
  CLEAR_BIT(RCC->CIER, RCC_CIER_PLLSAI2RDYIE);
}

#endif /* RCC_PLLSAI2_SUPPORT */
/**
  * @brief  Disable LSE clock security system interrupt
  * @rmtoll CIER         LSECSSIE      LL_RCC_DisableIT_LSECSS
  * @retval None
  */
__STATIC_INLINE void LL_RCC_DisableIT_LSECSS(void)
{
  CLEAR_BIT(RCC->CIER, RCC_CIER_LSECSSIE);
}

/**
  * @brief  Checks if LSI ready interrupt source is enabled or disabled.
  * @rmtoll CIER         LSIRDYIE      LL_RCC_IsEnabledIT_LSIRDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsEnabledIT_LSIRDY(void)
{
  return (READ_BIT(RCC->CIER, RCC_CIER_LSIRDYIE) == (RCC_CIER_LSIRDYIE));
}

/**
  * @brief  Checks if LSE ready interrupt source is enabled or disabled.
  * @rmtoll CIER         LSERDYIE      LL_RCC_IsEnabledIT_LSERDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsEnabledIT_LSERDY(void)
{
  return (READ_BIT(RCC->CIER, RCC_CIER_LSERDYIE) == (RCC_CIER_LSERDYIE));
}

/**
  * @brief  Checks if MSI ready interrupt source is enabled or disabled.
  * @rmtoll CIER         MSIRDYIE      LL_RCC_IsEnabledIT_MSIRDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsEnabledIT_MSIRDY(void)
{
  return (READ_BIT(RCC->CIER, RCC_CIER_MSIRDYIE) == (RCC_CIER_MSIRDYIE));
}

/**
  * @brief  Checks if HSI ready interrupt source is enabled or disabled.
  * @rmtoll CIER         HSIRDYIE      LL_RCC_IsEnabledIT_HSIRDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsEnabledIT_HSIRDY(void)
{
  return (READ_BIT(RCC->CIER, RCC_CIER_HSIRDYIE) == (RCC_CIER_HSIRDYIE));
}

/**
  * @brief  Checks if HSE ready interrupt source is enabled or disabled.
  * @rmtoll CIER         HSERDYIE      LL_RCC_IsEnabledIT_HSERDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsEnabledIT_HSERDY(void)
{
  return (READ_BIT(RCC->CIER, RCC_CIER_HSERDYIE) == (RCC_CIER_HSERDYIE));
}

/**
  * @brief  Checks if PLL ready interrupt source is enabled or disabled.
  * @rmtoll CIER         PLLRDYIE      LL_RCC_IsEnabledIT_PLLRDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsEnabledIT_PLLRDY(void)
{
  return (READ_BIT(RCC->CIER, RCC_CIER_PLLRDYIE) == (RCC_CIER_PLLRDYIE));
}

/**
  * @brief  Checks if PLLSAI1 ready interrupt source is enabled or disabled.
  * @rmtoll CIER         PLLSAI1RDYIE  LL_RCC_IsEnabledIT_PLLSAI1RDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsEnabledIT_PLLSAI1RDY(void)
{
  return (READ_BIT(RCC->CIER, RCC_CIER_PLLSAI1RDYIE) == (RCC_CIER_PLLSAI1RDYIE));
}

#if defined(RCC_PLLSAI2_SUPPORT)
/**
  * @brief  Checks if PLLSAI2 ready interrupt source is enabled or disabled.
  * @rmtoll CIER         PLLSAI2RDYIE  LL_RCC_IsEnabledIT_PLLSAI2RDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsEnabledIT_PLLSAI2RDY(void)
{
  return (READ_BIT(RCC->CIER, RCC_CIER_PLLSAI2RDYIE) == (RCC_CIER_PLLSAI2RDYIE));
}

#endif /* RCC_PLLSAI2_SUPPORT */
/**
  * @brief  Checks if LSECSS interrupt source is enabled or disabled.
  * @rmtoll CIER         LSECSSIE      LL_RCC_IsEnabledIT_LSECSS
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsEnabledIT_LSECSS(void)
{
  return (READ_BIT(RCC->CIER, RCC_CIER_LSECSSIE) == (RCC_CIER_LSECSSIE));
}

/**
  * @}
  */

#if defined(USE_FULL_LL_DRIVER)
/** @defgroup RCC_LL_EF_Init De-initialization function
  * @{
  */
ErrorStatus LL_RCC_DeInit(void);
/**
  * @}
  */

/** @defgroup RCC_LL_EF_Get_Freq Get system and peripherals clocks frequency functions
  * @{
  */
void        LL_RCC_GetSystemClocksFreq(LL_RCC_ClocksTypeDef *RCC_Clocks);
uint32_t    LL_RCC_GetUSARTClockFreq(uint32_t USARTxSource);
#if defined(UART4) || defined(UART5)
uint32_t    LL_RCC_GetUARTClockFreq(uint32_t UARTxSource);
#endif /* UART4 || UART5 */
uint32_t    LL_RCC_GetI2CClockFreq(uint32_t I2CxSource);
uint32_t    LL_RCC_GetLPUARTClockFreq(uint32_t LPUARTxSource);
uint32_t    LL_RCC_GetLPTIMClockFreq(uint32_t LPTIMxSource);
uint32_t    LL_RCC_GetSAIClockFreq(uint32_t SAIxSource);
uint32_t    LL_RCC_GetSDMMCClockFreq(uint32_t SDMMCxSource);
uint32_t    LL_RCC_GetRNGClockFreq(uint32_t RNGxSource);
#if defined(USB_OTG_FS)
uint32_t    LL_RCC_GetUSBClockFreq(uint32_t USBxSource);
#endif /* USB_OTG_FS */
uint32_t    LL_RCC_GetADCClockFreq(uint32_t ADCxSource);
uint32_t    LL_RCC_GetSWPMIClockFreq(uint32_t SWPMIxSource);
#if defined(DFSDM_Channel0)
uint32_t    LL_RCC_GetDFSDMClockFreq(uint32_t DFSDMxSource);
#endif /* DFSDM_Channel0 */
/**
  * @}
  */
#endif /* USE_FULL_LL_DRIVER */

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

#endif /* __STM32L4xx_LL_RCC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
