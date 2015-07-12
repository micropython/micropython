/**
  ******************************************************************************
  * @file    stm32f2xx_hal_rcc.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    25-March-2014
  * @brief   Header file of RCC HAL module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
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
#ifndef __STM32F2xx_HAL_RCC_H
#define __STM32F2xx_HAL_RCC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx_hal_def.h"

/** @addtogroup STM32F2xx_HAL_Driver
  * @{
  */

/** @addtogroup RCC
  * @{
  */

/* Exported types ------------------------------------------------------------*/ 

/**
  * @brief  RCC PLL configuration structure definition
  */
typedef struct
{
  uint32_t PLLState;   /*!< The new state of the PLL.
                            This parameter can be a value of @ref RCC_PLL_Config                      */

  uint32_t PLLSource;  /*!< RCC_PLLSource: PLL entry clock source.
                            This parameter must be a value of @ref RCC_PLL_Clock_Source               */           

  uint32_t PLLM;       /*!< PLLM: Division factor for PLL VCO input clock
                            This parameter must be a number between Min_Data = 0 and Max_Data = 63    */        

  uint32_t PLLN;       /*!< PLLN: Multiplication factor for PLL VCO output clock
                            This parameter must be a number between Min_Data = 192 and Max_Data = 432 */

  uint32_t PLLP;       /*!< PLLP: Division factor for main system clock (SYSCLK)
                            This parameter must be a value of @ref RCC_PLLP_Clock_Divider.            */

  uint32_t PLLQ;       /*!< PLLQ: Division factor for OTG FS, SDIO and RNG clocks
                            This parameter must be a number between Min_Data = 0 and Max_Data = 63    */

}RCC_PLLInitTypeDef;

/**
  * @brief  RCC Internal/External Oscillator (HSE, HSI, LSE and LSI) configuration structure definition  
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

  uint32_t HSICalibrationValue;  /*!< The calibration trimming value.
                                      This parameter must be a number between Min_Data = 0x00 and Max_Data = 0x1F */
                               
  uint32_t LSIState;             /*!< The new state of the LSI.
                                      This parameter can be a value of @ref RCC_LSI_Config                        */

  RCC_PLLInitTypeDef PLL;        /*!< PLL structure parameters                                                    */      

}RCC_OscInitTypeDef;

/**
  * @brief  RCC System, AHB and APB busses clock configuration structure definition  
  */
typedef struct
{
  uint32_t ClockType;             /*!< The clock to be configured.
                                       This parameter can be a value of @ref RCC_System_Clock_Type      */

  uint32_t SYSCLKSource;          /*!< The clock source (SYSCLKS) used as system clock.
                                       This parameter can be a value of @ref RCC_System_Clock_Source    */

  uint32_t AHBCLKDivider;         /*!< The AHB clock (HCLK) divider. This clock is derived from the system clock (SYSCLK).
                                       This parameter can be a value of @ref RCC_AHB_Clock_Source       */

  uint32_t APB1CLKDivider;        /*!< The APB1 clock (PCLK1) divider. This clock is derived from the AHB clock (HCLK).
                                       This parameter can be a value of @ref RCC_APB1_APB2_Clock_Source */

  uint32_t APB2CLKDivider;        /*!< The APB2 clock (PCLK2) divider. This clock is derived from the AHB clock (HCLK).
                                       This parameter can be a value of @ref RCC_APB1_APB2_Clock_Source */

}RCC_ClkInitTypeDef;

/* Exported constants --------------------------------------------------------*/
/** @defgroup RCC_Exported_Constants
  * @{
  */

/** @defgroup RCC_BitAddress_AliasRegion
  * @brief RCC registers bit address in the alias region
  * @{
  */
#define RCC_OFFSET                (RCC_BASE - PERIPH_BASE)
/* --- CR Register ---*/
/* Alias word address of HSION bit */
#define RCC_CR_OFFSET             (RCC_OFFSET + 0x00)
#define HSION_BitNumber           0x00
#define CR_HSION_BB               (PERIPH_BB_BASE + (RCC_CR_OFFSET * 32) + (HSION_BitNumber * 4))
/* Alias word address of CSSON bit */
#define CSSON_BitNumber           0x13
#define CR_CSSON_BB               (PERIPH_BB_BASE + (RCC_CR_OFFSET * 32) + (CSSON_BitNumber * 4))
/* Alias word address of PLLON bit */
#define PLLON_BitNumber           0x18
#define CR_PLLON_BB               (PERIPH_BB_BASE + (RCC_CR_OFFSET * 32) + (PLLON_BitNumber * 4))
/* Alias word address of PLLI2SON bit */
#define PLLI2SON_BitNumber        0x1A
#define CR_PLLI2SON_BB            (PERIPH_BB_BASE + (RCC_CR_OFFSET * 32) + (PLLI2SON_BitNumber * 4))

/* --- CFGR Register ---*/
/* Alias word address of I2SSRC bit */
#define RCC_CFGR_OFFSET           (RCC_OFFSET + 0x08)
#define I2SSRC_BitNumber          0x17
#define CFGR_I2SSRC_BB            (PERIPH_BB_BASE + (RCC_CFGR_OFFSET * 32) + (I2SSRC_BitNumber * 4))

/* --- BDCR Register ---*/
/* Alias word address of RTCEN bit */
#define RCC_BDCR_OFFSET           (RCC_OFFSET + 0x70)
#define RTCEN_BitNumber           0x0F
#define BDCR_RTCEN_BB             (PERIPH_BB_BASE + (RCC_BDCR_OFFSET * 32) + (RTCEN_BitNumber * 4))
/* Alias word address of BDRST bit */
#define BDRST_BitNumber           0x10
#define BDCR_BDRST_BB             (PERIPH_BB_BASE + (RCC_BDCR_OFFSET * 32) + (BDRST_BitNumber * 4))

/* --- CSR Register ---*/
/* Alias word address of LSION bit */
#define RCC_CSR_OFFSET            (RCC_OFFSET + 0x74)
#define LSION_BitNumber           0x00
#define CSR_LSION_BB              (PERIPH_BB_BASE + (RCC_CSR_OFFSET * 32) + (LSION_BitNumber * 4))

/* CR register byte 3 (Bits[23:16]) base address */
#define CR_BYTE2_ADDRESS          ((uint32_t)0x40023802)

/* CIR register byte 2 (Bits[15:8]) base address */
#define CIR_BYTE1_ADDRESS         ((uint32_t)(RCC_BASE + 0x0C + 0x01))

/* CIR register byte 3 (Bits[23:16]) base address */
#define CIR_BYTE2_ADDRESS         ((uint32_t)(RCC_BASE + 0x0C + 0x02))

/* BDCR register base address */
#define BDCR_BYTE0_ADDRESS        (PERIPH_BASE + RCC_BDCR_OFFSET)


#define DBP_TIMEOUT_VALUE          ((uint32_t)100)
#define LSE_TIMEOUT_VALUE          ((uint32_t)5000)
/**
  * @}
  */

/** @defgroup RCC_Oscillator_Type
  * @{
  */
#define RCC_OSCILLATORTYPE_NONE            ((uint32_t)0x00000000)
#define RCC_OSCILLATORTYPE_HSE             ((uint32_t)0x00000001)
#define RCC_OSCILLATORTYPE_HSI             ((uint32_t)0x00000002)
#define RCC_OSCILLATORTYPE_LSE             ((uint32_t)0x00000004)
#define RCC_OSCILLATORTYPE_LSI             ((uint32_t)0x00000008)

#define IS_RCC_OSCILLATORTYPE(OSCILLATOR) ((OSCILLATOR) <= 15)
/**
  * @}
  */

/** @defgroup RCC_HSE_Config
  * @{
  */
#define RCC_HSE_OFF                      ((uint8_t)0x00)
#define RCC_HSE_ON                       ((uint8_t)0x01)
#define RCC_HSE_BYPASS                   ((uint8_t)0x05)

#define IS_RCC_HSE(HSE) (((HSE) == RCC_HSE_OFF) || ((HSE) == RCC_HSE_ON) || \
                         ((HSE) == RCC_HSE_BYPASS))
/**
  * @}
  */

/** @defgroup RCC_LSE_Config
  * @{
  */
#define RCC_LSE_OFF                      ((uint8_t)0x00)
#define RCC_LSE_ON                       ((uint8_t)0x01)
#define RCC_LSE_BYPASS                   ((uint8_t)0x05)

#define IS_RCC_LSE(LSE) (((LSE) == RCC_LSE_OFF) || ((LSE) == RCC_LSE_ON) || \
                         ((LSE) == RCC_LSE_BYPASS))
/**
  * @}
  */

/** @defgroup RCC_HSI_Config
  * @{
  */
#define RCC_HSI_OFF                      ((uint8_t)0x00)
#define RCC_HSI_ON                       ((uint8_t)0x01)

#define IS_RCC_HSI(HSI) (((HSI) == RCC_HSI_OFF) || ((HSI) == RCC_HSI_ON))
/**
  * @}
  */

/** @defgroup RCC_LSI_Config 
  * @{
  */
#define RCC_LSI_OFF                      ((uint8_t)0x00)
#define RCC_LSI_ON                       ((uint8_t)0x01)

#define IS_RCC_LSI(LSI) (((LSI) == RCC_LSI_OFF) || ((LSI) == RCC_LSI_ON))
/**
  * @}
  */

/** @defgroup RCC_PLL_Config 
  * @{
  */
#define RCC_PLL_NONE                      ((uint8_t)0x00)
#define RCC_PLL_OFF                       ((uint8_t)0x01)
#define RCC_PLL_ON                        ((uint8_t)0x02)

#define IS_RCC_PLL(PLL) (((PLL) == RCC_PLL_NONE) ||((PLL) == RCC_PLL_OFF) || ((PLL) == RCC_PLL_ON))
/**
  * @}
  */

/** @defgroup RCC_PLLP_Clock_Divider
  * @{
  */
#define RCC_PLLP_DIV2                  ((uint32_t)0x00000002)
#define RCC_PLLP_DIV4                  ((uint32_t)0x00000004)
#define RCC_PLLP_DIV6                  ((uint32_t)0x00000006)
#define RCC_PLLP_DIV8                  ((uint32_t)0x00000008)
/**
  * @}
  */

/** @defgroup RCC_PLL_Clock_Source 
  * @{
  */
#define RCC_PLLSOURCE_HSI                RCC_PLLCFGR_PLLSRC_HSI
#define RCC_PLLSOURCE_HSE                RCC_PLLCFGR_PLLSRC_HSE

#define IS_RCC_PLLSOURCE(SOURCE) (((SOURCE) == RCC_PLLSOURCE_HSI) || \
                                  ((SOURCE) == RCC_PLLSOURCE_HSE))
#define IS_RCC_PLLM_VALUE(VALUE) ((VALUE) <= 63)
#define IS_RCC_PLLN_VALUE(VALUE) ((192 <= (VALUE)) && ((VALUE) <= 432))
#define IS_RCC_PLLP_VALUE(VALUE) (((VALUE) == 2) || ((VALUE) == 4) || ((VALUE) == 6) || ((VALUE) == 8))
#define IS_RCC_PLLQ_VALUE(VALUE) ((4 <= (VALUE)) && ((VALUE) <= 15))

#define IS_RCC_PLLI2SN_VALUE(VALUE) ((192 <= (VALUE)) && ((VALUE) <= 432))
#define IS_RCC_PLLI2SR_VALUE(VALUE) ((2 <= (VALUE)) && ((VALUE) <= 7))  

/**
  * @}
  */

/** @defgroup RCC_System_Clock_Type
  * @{
  */
#define RCC_CLOCKTYPE_SYSCLK             ((uint32_t)0x00000001)
#define RCC_CLOCKTYPE_HCLK               ((uint32_t)0x00000002)
#define RCC_CLOCKTYPE_PCLK1              ((uint32_t)0x00000004)
#define RCC_CLOCKTYPE_PCLK2              ((uint32_t)0x00000008)

#define IS_RCC_CLOCKTYPE(CLK) ((1 <= (CLK)) && ((CLK) <= 15))
/**
  * @}
  */
  
/** @defgroup RCC_System_Clock_Source 
  * @{
  */
#define RCC_SYSCLKSOURCE_HSI             RCC_CFGR_SW_HSI
#define RCC_SYSCLKSOURCE_HSE             RCC_CFGR_SW_HSE
#define RCC_SYSCLKSOURCE_PLLCLK          RCC_CFGR_SW_PLL

#define IS_RCC_SYSCLKSOURCE(SOURCE) (((SOURCE) == RCC_SYSCLKSOURCE_HSI) || \
                                     ((SOURCE) == RCC_SYSCLKSOURCE_HSE) || \
                                     ((SOURCE) == RCC_SYSCLKSOURCE_PLLCLK))
/**
  * @}
  */ 

/** @defgroup RCC_AHB_Clock_Source
  * @{
  */
#define RCC_SYSCLK_DIV1                  RCC_CFGR_HPRE_DIV1
#define RCC_SYSCLK_DIV2                  RCC_CFGR_HPRE_DIV2
#define RCC_SYSCLK_DIV4                  RCC_CFGR_HPRE_DIV4
#define RCC_SYSCLK_DIV8                  RCC_CFGR_HPRE_DIV8
#define RCC_SYSCLK_DIV16                 RCC_CFGR_HPRE_DIV16
#define RCC_SYSCLK_DIV64                 RCC_CFGR_HPRE_DIV64
#define RCC_SYSCLK_DIV128                RCC_CFGR_HPRE_DIV128
#define RCC_SYSCLK_DIV256                RCC_CFGR_HPRE_DIV256
#define RCC_SYSCLK_DIV512                RCC_CFGR_HPRE_DIV512

#define IS_RCC_HCLK(HCLK) (((HCLK) == RCC_SYSCLK_DIV1)   || ((HCLK) == RCC_SYSCLK_DIV2)   || \
                           ((HCLK) == RCC_SYSCLK_DIV4)   || ((HCLK) == RCC_SYSCLK_DIV8)   || \
                           ((HCLK) == RCC_SYSCLK_DIV16)  || ((HCLK) == RCC_SYSCLK_DIV64)  || \
                           ((HCLK) == RCC_SYSCLK_DIV128) || ((HCLK) == RCC_SYSCLK_DIV256) || \
                           ((HCLK) == RCC_SYSCLK_DIV512))
/**
  * @}
  */ 
  
/** @defgroup RCC_APB1_APB2_Clock_Source
  * @{
  */
#define RCC_HCLK_DIV1                    RCC_CFGR_PPRE1_DIV1
#define RCC_HCLK_DIV2                    RCC_CFGR_PPRE1_DIV2
#define RCC_HCLK_DIV4                    RCC_CFGR_PPRE1_DIV4
#define RCC_HCLK_DIV8                    RCC_CFGR_PPRE1_DIV8
#define RCC_HCLK_DIV16                   RCC_CFGR_PPRE1_DIV16

#define IS_RCC_PCLK(PCLK) (((PCLK) == RCC_HCLK_DIV1) || ((PCLK) == RCC_HCLK_DIV2) || \
                           ((PCLK) == RCC_HCLK_DIV4) || ((PCLK) == RCC_HCLK_DIV8) || \
                           ((PCLK) == RCC_HCLK_DIV16))
/**
  * @}
  */ 

/** @defgroup RCC_RTC_Clock_Source
  * @{
  */
#define RCC_RTCCLKSOURCE_LSE             ((uint32_t)0x00000100)
#define RCC_RTCCLKSOURCE_LSI             ((uint32_t)0x00000200)
#define RCC_RTCCLKSOURCE_HSE_DIV2        ((uint32_t)0x00020300)
#define RCC_RTCCLKSOURCE_HSE_DIV3        ((uint32_t)0x00030300)
#define RCC_RTCCLKSOURCE_HSE_DIV4        ((uint32_t)0x00040300)
#define RCC_RTCCLKSOURCE_HSE_DIV5        ((uint32_t)0x00050300)
#define RCC_RTCCLKSOURCE_HSE_DIV6        ((uint32_t)0x00060300)
#define RCC_RTCCLKSOURCE_HSE_DIV7        ((uint32_t)0x00070300)
#define RCC_RTCCLKSOURCE_HSE_DIV8        ((uint32_t)0x00080300)
#define RCC_RTCCLKSOURCE_HSE_DIV9        ((uint32_t)0x00090300)
#define RCC_RTCCLKSOURCE_HSE_DIV10       ((uint32_t)0x000A0300)
#define RCC_RTCCLKSOURCE_HSE_DIV11       ((uint32_t)0x000B0300)
#define RCC_RTCCLKSOURCE_HSE_DIV12       ((uint32_t)0x000C0300)
#define RCC_RTCCLKSOURCE_HSE_DIV13       ((uint32_t)0x000D0300)
#define RCC_RTCCLKSOURCE_HSE_DIV14       ((uint32_t)0x000E0300)
#define RCC_RTCCLKSOURCE_HSE_DIV15       ((uint32_t)0x000F0300)
#define RCC_RTCCLKSOURCE_HSE_DIV16       ((uint32_t)0x00100300)
#define RCC_RTCCLKSOURCE_HSE_DIV17       ((uint32_t)0x00110300)
#define RCC_RTCCLKSOURCE_HSE_DIV18       ((uint32_t)0x00120300)
#define RCC_RTCCLKSOURCE_HSE_DIV19       ((uint32_t)0x00130300)
#define RCC_RTCCLKSOURCE_HSE_DIV20       ((uint32_t)0x00140300)
#define RCC_RTCCLKSOURCE_HSE_DIV21       ((uint32_t)0x00150300)
#define RCC_RTCCLKSOURCE_HSE_DIV22       ((uint32_t)0x00160300)
#define RCC_RTCCLKSOURCE_HSE_DIV23       ((uint32_t)0x00170300)
#define RCC_RTCCLKSOURCE_HSE_DIV24       ((uint32_t)0x00180300)
#define RCC_RTCCLKSOURCE_HSE_DIV25       ((uint32_t)0x00190300)
#define RCC_RTCCLKSOURCE_HSE_DIV26       ((uint32_t)0x001A0300)
#define RCC_RTCCLKSOURCE_HSE_DIV27       ((uint32_t)0x001B0300)
#define RCC_RTCCLKSOURCE_HSE_DIV28       ((uint32_t)0x001C0300)
#define RCC_RTCCLKSOURCE_HSE_DIV29       ((uint32_t)0x001D0300)
#define RCC_RTCCLKSOURCE_HSE_DIV30       ((uint32_t)0x001E0300)
#define RCC_RTCCLKSOURCE_HSE_DIV31       ((uint32_t)0x001F0300)
/**
  * @}
  */

/** @defgroup RCC_I2S_Clock_Source
  * @{
  */
#define RCC_I2SCLKSOURCE_PLLI2S         ((uint32_t)0x00000000)
#define RCC_I2SCLKSOURCE_EXT            ((uint32_t)0x00000001)
/**
  * @}
  */

/** @defgroup RCC_MCO_Index
  * @{
  */
#define RCC_MCO1                         ((uint32_t)0x00000000)
#define RCC_MCO2                         ((uint32_t)0x00000001)

#define IS_RCC_MCO(MCOx) (((MCOx) == RCC_MCO1) || ((MCOx) == RCC_MCO2))
/**
  * @}
  */

/** @defgroup RCC_MCO1_Clock_Source
  * @{
  */
#define RCC_MCO1SOURCE_HSI               ((uint32_t)0x00000000)
#define RCC_MCO1SOURCE_LSE               RCC_CFGR_MCO1_0
#define RCC_MCO1SOURCE_HSE               RCC_CFGR_MCO1_1
#define RCC_MCO1SOURCE_PLLCLK            RCC_CFGR_MCO1

#define IS_RCC_MCO1SOURCE(SOURCE) (((SOURCE) == RCC_MCO1SOURCE_HSI) || ((SOURCE) == RCC_MCO1SOURCE_LSE) || \
                                   ((SOURCE) == RCC_MCO1SOURCE_HSE) || ((SOURCE) == RCC_MCO1SOURCE_PLLCLK))
/**
  * @}
  */

/** @defgroup RCC_MCO2_Clock_Source
  * @{
  */
#define RCC_MCO2SOURCE_SYSCLK            ((uint32_t)0x00000000)
#define RCC_MCO2SOURCE_PLLI2SCLK         RCC_CFGR_MCO2_0
#define RCC_MCO2SOURCE_HSE               RCC_CFGR_MCO2_1
#define RCC_MCO2SOURCE_PLLCLK            RCC_CFGR_MCO2

#define IS_RCC_MCO2SOURCE(SOURCE) (((SOURCE) == RCC_MCO2SOURCE_SYSCLK) || ((SOURCE) == RCC_MCO2SOURCE_PLLI2SCLK)|| \
                                   ((SOURCE) == RCC_MCO2SOURCE_HSE)    || ((SOURCE) == RCC_MCO2SOURCE_PLLCLK))
/**
  * @}
  */

/** @defgroup RCC_MCOx_Clock_Prescaler
  * @{
  */
#define RCC_MCODIV_1                    ((uint32_t)0x00000000)
#define RCC_MCODIV_2                    RCC_CFGR_MCO1PRE_2
#define RCC_MCODIV_3                    ((uint32_t)RCC_CFGR_MCO1PRE_0 | RCC_CFGR_MCO1PRE_2)
#define RCC_MCODIV_4                    ((uint32_t)RCC_CFGR_MCO1PRE_1 | RCC_CFGR_MCO1PRE_2)
#define RCC_MCODIV_5                    RCC_CFGR_MCO1PRE

#define IS_RCC_MCODIV(DIV) (((DIV) == RCC_MCODIV_1)  || ((DIV) == RCC_MCODIV_2) || \
                             ((DIV) == RCC_MCODIV_3) || ((DIV) == RCC_MCODIV_4) || \
                             ((DIV) == RCC_MCODIV_5)) 
/**
  * @}
  */

/** @defgroup RCC_Interrupt 
  * @{
  */
#define RCC_IT_LSIRDY                    ((uint8_t)0x01)
#define RCC_IT_LSERDY                    ((uint8_t)0x02)
#define RCC_IT_HSIRDY                    ((uint8_t)0x04)
#define RCC_IT_HSERDY                    ((uint8_t)0x08)
#define RCC_IT_PLLRDY                    ((uint8_t)0x10)
#define RCC_IT_PLLI2SRDY                 ((uint8_t)0x20)
#define RCC_IT_CSS                       ((uint8_t)0x80)
/**
  * @}
  */
  
/** @defgroup RCC_Flag
  *        Elements values convention: 0XXYYYYYb
  *           - YYYYY  : Flag position in the register
  *           - 0XX  : Register index
  *                 - 01: CR register
  *                 - 10: BDCR register
  *                 - 11: CSR register
  * @{
  */
/* Flags in the CR register */
#define RCC_FLAG_HSIRDY                  ((uint8_t)0x21)
#define RCC_FLAG_HSERDY                  ((uint8_t)0x31)
#define RCC_FLAG_PLLRDY                  ((uint8_t)0x39)
#define RCC_FLAG_PLLI2SRDY               ((uint8_t)0x3B)

/* Flags in the BDCR register */
#define RCC_FLAG_LSERDY                  ((uint8_t)0x41)

/* Flags in the CSR register */
#define RCC_FLAG_LSIRDY                  ((uint8_t)0x61)
#define RCC_FLAG_BORRST                  ((uint8_t)0x79)
#define RCC_FLAG_PINRST                  ((uint8_t)0x7A)
#define RCC_FLAG_PORRST                  ((uint8_t)0x7B)
#define RCC_FLAG_SFTRST                  ((uint8_t)0x7C)
#define RCC_FLAG_IWDGRST                 ((uint8_t)0x7D)
#define RCC_FLAG_WWDGRST                 ((uint8_t)0x7E)
#define RCC_FLAG_LPWRRST                 ((uint8_t)0x7F)

#define IS_RCC_CALIBRATION_VALUE(VALUE) ((VALUE) <= 0x1F)
/**
  * @}
  */

/** @defgroup RCC_PLLI2S_DIVQ 
  * @{
  */ 
#define IS_RCC_PLLI2S_DIVQ_VALUE(VALUE) ((1 <= (VALUE)) && ((VALUE) <= 32))

/**
  * @}
  */

/** @defgroup RCC_TIM_PRescaler_Selection
  * @{
  */
#define RCC_TIMPRES_DESACTIVATED        ((uint8_t)0x00)
#define RCC_TIMPRES_ACTIVATED           ((uint8_t)0x01)
/**
  * @}
  */



/**
  * @}
  */   
/* Exported macro ------------------------------------------------------------*/

/** @brief  Enable or disable the AHB1 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before 
  *         using it.   
  */
#define __GPIOA_CLK_ENABLE()           (RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN))
#define __GPIOB_CLK_ENABLE()           (RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOBEN))
#define __GPIOC_CLK_ENABLE()           (RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOCEN))
#define __GPIOD_CLK_ENABLE()           (RCC->AHB1ENR |= (RCC_AHB1ENR_GPIODEN))
#define __GPIOE_CLK_ENABLE()           (RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOEEN))
#define __GPIOF_CLK_ENABLE()           (RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOFEN))
#define __GPIOG_CLK_ENABLE()           (RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOGEN))
#define __GPIOH_CLK_ENABLE()           (RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOHEN))
#define __GPIOI_CLK_ENABLE()           (RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOIEN))
#define __CRC_CLK_ENABLE()             (RCC->AHB1ENR |= (RCC_AHB1ENR_CRCEN))
#define __BKPSRAM_CLK_ENABLE()         (RCC->AHB1ENR |= (RCC_AHB1ENR_BKPSRAMEN))
#define __DMA1_CLK_ENABLE()            (RCC->AHB1ENR |= (RCC_AHB1ENR_DMA1EN))
#define __DMA2_CLK_ENABLE()            (RCC->AHB1ENR |= (RCC_AHB1ENR_DMA2EN))
#define __USB_OTG_HS_CLK_ENABLE()      (RCC->AHB1ENR |= (RCC_AHB1ENR_OTGHSEN))
#define __USB_OTG_HS_ULPI_CLK_ENABLE() (RCC->AHB1ENR |= (RCC_AHB1ENR_OTGHSULPIEN))

#define __GPIOA_CLK_DISABLE()           (RCC->AHB1ENR &= ~(RCC_AHB1ENR_GPIOAEN))
#define __GPIOB_CLK_DISABLE()           (RCC->AHB1ENR &= ~(RCC_AHB1ENR_GPIOBEN))
#define __GPIOC_CLK_DISABLE()           (RCC->AHB1ENR &= ~(RCC_AHB1ENR_GPIOCEN))
#define __GPIOD_CLK_DISABLE()           (RCC->AHB1ENR &= ~(RCC_AHB1ENR_GPIODEN))
#define __GPIOE_CLK_DISABLE()           (RCC->AHB1ENR &= ~(RCC_AHB1ENR_GPIOEEN))
#define __GPIOF_CLK_DISABLE()           (RCC->AHB1ENR &= ~(RCC_AHB1ENR_GPIOFEN))
#define __GPIOG_CLK_DISABLE()           (RCC->AHB1ENR &= ~(RCC_AHB1ENR_GPIOGEN))
#define __GPIOH_CLK_DISABLE()           (RCC->AHB1ENR &= ~(RCC_AHB1ENR_GPIOHEN))
#define __GPIOI_CLK_DISABLE()           (RCC->AHB1ENR &= ~(RCC_AHB1ENR_GPIOIEN))
#define __CRC_CLK_DISABLE()             (RCC->AHB1ENR &= ~(RCC_AHB1ENR_CRCEN))
#define __BKPSRAM_CLK_DISABLE()         (RCC->AHB1ENR &= ~(RCC_AHB1ENR_BKPSRAMEN))
#define __DMA1_CLK_DISABLE()            (RCC->AHB1ENR &= ~(RCC_AHB1ENR_DMA1EN))
#define __DMA2_CLK_DISABLE()            (RCC->AHB1ENR &= ~(RCC_AHB1ENR_DMA2EN))
#define __USB_OTG_HS_CLK_DISABLE()      (RCC->AHB1ENR &= ~(RCC_AHB1ENR_OTGHSEN))
#define __USB_OTG_HS_ULPI_CLK_DISABLE() (RCC->AHB1ENR &= ~(RCC_AHB1ENR_OTGHSULPIEN))

/** @brief  Enable or disable the AHB2 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before 
  *         using it.
  */
#define __USB_OTG_FS_CLK_ENABLE()  do {(RCC->AHB2ENR |= (RCC_AHB2ENR_OTGFSEN));\
                                       __SYSCFG_CLK_ENABLE();\
                                    }while(0)
                                        

#define __USB_OTG_FS_CLK_DISABLE() do { (RCC->AHB2ENR &= ~(RCC_AHB2ENR_OTGFSEN));\
                                         __SYSCFG_CLK_DISABLE();\
                                    }while(0)
/** @brief  Enables or disables the RNG peripheral clock.
 * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before 
  *         using it. 
  */
#define __RNG_CLK_ENABLE()    (RCC->AHB2ENR |= (RCC_AHB2ENR_RNGEN))
#define __RNG_CLK_DISABLE()   (RCC->AHB2ENR &= ~(RCC_AHB2ENR_RNGEN))

/** @brief  Enables or disables the AHB3 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before 
  *         using it. 
  */
#define __FSMC_CLK_ENABLE()  (RCC->AHB3ENR |= (RCC_AHB3ENR_FSMCEN))
#define __FSMC_CLK_DISABLE() (RCC->AHB3ENR &= ~(RCC_AHB3ENR_FSMCEN))

/** @brief  Enable or disable the Low Speed APB (APB1) peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  */
#define __TIM2_CLK_ENABLE()    (RCC->APB1ENR |= (RCC_APB1ENR_TIM2EN))
#define __TIM3_CLK_ENABLE()    (RCC->APB1ENR |= (RCC_APB1ENR_TIM3EN))
#define __TIM4_CLK_ENABLE()    (RCC->APB1ENR |= (RCC_APB1ENR_TIM4EN))
#define __TIM5_CLK_ENABLE()    (RCC->APB1ENR |= (RCC_APB1ENR_TIM5EN))
#define __TIM6_CLK_ENABLE()    (RCC->APB1ENR |= (RCC_APB1ENR_TIM6EN))
#define __TIM7_CLK_ENABLE()    (RCC->APB1ENR |= (RCC_APB1ENR_TIM7EN))
#define __TIM12_CLK_ENABLE()   (RCC->APB1ENR |= (RCC_APB1ENR_TIM12EN))
#define __TIM13_CLK_ENABLE()   (RCC->APB1ENR |= (RCC_APB1ENR_TIM13EN))
#define __TIM14_CLK_ENABLE()   (RCC->APB1ENR |= (RCC_APB1ENR_TIM14EN))
#define __WWDG_CLK_ENABLE()    (RCC->APB1ENR |= (RCC_APB1ENR_WWDGEN))
#define __SPI2_CLK_ENABLE()    (RCC->APB1ENR |= (RCC_APB1ENR_SPI2EN))
#define __SPI3_CLK_ENABLE()    (RCC->APB1ENR |= (RCC_APB1ENR_SPI3EN))
#define __USART2_CLK_ENABLE()  (RCC->APB1ENR |= (RCC_APB1ENR_USART2EN))
#define __USART3_CLK_ENABLE()  (RCC->APB1ENR |= (RCC_APB1ENR_USART3EN))
#define __UART4_CLK_ENABLE()   (RCC->APB1ENR |= (RCC_APB1ENR_UART4EN))
#define __UART5_CLK_ENABLE()   (RCC->APB1ENR |= (RCC_APB1ENR_UART5EN))
#define __I2C1_CLK_ENABLE()    (RCC->APB1ENR |= (RCC_APB1ENR_I2C1EN))
#define __I2C2_CLK_ENABLE()    (RCC->APB1ENR |= (RCC_APB1ENR_I2C2EN))
#define __I2C3_CLK_ENABLE()    (RCC->APB1ENR |= (RCC_APB1ENR_I2C3EN))
#define __PWR_CLK_ENABLE()     (RCC->APB1ENR |= (RCC_APB1ENR_PWREN))
#define __CAN1_CLK_ENABLE()    (RCC->APB1ENR |= (RCC_APB1ENR_CAN1EN))
#define __CAN2_CLK_ENABLE()    (RCC->APB1ENR |= (RCC_APB1ENR_CAN2EN))
#define __DAC_CLK_ENABLE()     (RCC->APB1ENR |= (RCC_APB1ENR_DACEN))

#define __TIM2_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_TIM2EN))
#define __TIM3_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_TIM3EN))
#define __TIM4_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_TIM4EN))
#define __TIM5_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_TIM5EN))
#define __TIM6_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_TIM6EN))
#define __TIM7_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_TIM7EN))
#define __TIM12_CLK_DISABLE()  (RCC->APB1ENR &= ~(RCC_APB1ENR_TIM12EN))
#define __TIM13_CLK_DISABLE()  (RCC->APB1ENR &= ~(RCC_APB1ENR_TIM13EN))
#define __TIM14_CLK_DISABLE()  (RCC->APB1ENR &= ~(RCC_APB1ENR_TIM14EN))
#define __WWDG_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_WWDGEN))
#define __SPI2_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_SPI2EN))
#define __SPI3_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_SPI3EN))
#define __USART2_CLK_DISABLE() (RCC->APB1ENR &= ~(RCC_APB1ENR_USART2EN))
#define __USART3_CLK_DISABLE() (RCC->APB1ENR &= ~(RCC_APB1ENR_USART3EN))
#define __UART4_CLK_DISABLE()  (RCC->APB1ENR &= ~(RCC_APB1ENR_UART4EN))
#define __UART5_CLK_DISABLE()  (RCC->APB1ENR &= ~(RCC_APB1ENR_UART5EN))
#define __I2C1_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_I2C1EN))
#define __I2C2_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_I2C2EN))
#define __I2C3_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_I2C3EN))
#define __PWR_CLK_DISABLE()    (RCC->APB1ENR &= ~(RCC_APB1ENR_PWREN))
#define __CAN1_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_CAN1EN))
#define __CAN2_CLK_DISABLE()   (RCC->APB1ENR &= ~(RCC_APB1ENR_CAN2EN))
#define __DAC_CLK_DISABLE()    (RCC->APB1ENR &= ~(RCC_APB1ENR_DACEN))


/** @brief  Enable or disable the High Speed APB (APB2) peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before 
  *         using it.
  */
#define __TIM1_CLK_ENABLE()    (RCC->APB2ENR |= (RCC_APB2ENR_TIM1EN))
#define __USART1_CLK_ENABLE()  (RCC->APB2ENR |= (RCC_APB2ENR_USART1EN))
#define __USART6_CLK_ENABLE()  (RCC->APB2ENR |= (RCC_APB2ENR_USART6EN))
#define __ADC1_CLK_ENABLE()    (RCC->APB2ENR |= (RCC_APB2ENR_ADC1EN))
#define __ADC2_CLK_ENABLE()    (RCC->APB2ENR |= (RCC_APB2ENR_ADC2EN))
#define __ADC3_CLK_ENABLE()    (RCC->APB2ENR |= (RCC_APB2ENR_ADC3EN))
#define __SDIO_CLK_ENABLE()    (RCC->APB2ENR |= (RCC_APB2ENR_SDIOEN))
#define __SPI1_CLK_ENABLE()    (RCC->APB2ENR |= (RCC_APB2ENR_SPI1EN))
#define __SYSCFG_CLK_ENABLE()  (RCC->APB2ENR |= (RCC_APB2ENR_SYSCFGEN))
#define __TIM8_CLK_ENABLE()    (RCC->APB2ENR |= (RCC_APB2ENR_TIM8EN))
#define __TIM9_CLK_ENABLE()    (RCC->APB2ENR |= (RCC_APB2ENR_TIM9EN))
#define __TIM10_CLK_ENABLE()   (RCC->APB2ENR |= (RCC_APB2ENR_TIM10EN))
#define __TIM11_CLK_ENABLE()   (RCC->APB2ENR |= (RCC_APB2ENR_TIM11EN))

#define __TIM1_CLK_DISABLE()   (RCC->APB2ENR &= ~(RCC_APB2ENR_TIM1EN))
#define __USART1_CLK_DISABLE() (RCC->APB2ENR &= ~(RCC_APB2ENR_USART1EN))
#define __USART6_CLK_DISABLE() (RCC->APB2ENR &= ~(RCC_APB2ENR_USART6EN))
#define __ADC1_CLK_DISABLE()   (RCC->APB2ENR &= ~(RCC_APB2ENR_ADC1EN))
#define __ADC2_CLK_DISABLE()   (RCC->APB2ENR &= ~(RCC_APB2ENR_ADC2EN))
#define __ADC3_CLK_DISABLE()   (RCC->APB2ENR &= ~(RCC_APB2ENR_ADC3EN))
#define __SDIO_CLK_DISABLE()   (RCC->APB2ENR &= ~(RCC_APB2ENR_SDIOEN))
#define __SPI1_CLK_DISABLE()   (RCC->APB2ENR &= ~(RCC_APB2ENR_SPI1EN))
#define __SYSCFG_CLK_DISABLE() (RCC->APB2ENR &= ~(RCC_APB2ENR_SYSCFGEN))
#define __TIM8_CLK_DISABLE()   (RCC->APB2ENR &= ~(RCC_APB2ENR_TIM8EN))
#define __TIM9_CLK_DISABLE()   (RCC->APB2ENR &= ~(RCC_APB2ENR_TIM9EN))
#define __TIM10_CLK_DISABLE()  (RCC->APB2ENR &= ~(RCC_APB2ENR_TIM10EN))
#define __TIM11_CLK_DISABLE()  (RCC->APB2ENR &= ~(RCC_APB2ENR_TIM11EN))

/** @brief  Force or release AHB1 peripheral reset.
  */  
#define __AHB1_FORCE_RESET()    (RCC->AHB1RSTR = 0xFFFFFFFF)
#define __GPIOA_FORCE_RESET()   (RCC->AHB1RSTR |= (RCC_AHB1RSTR_GPIOARST))
#define __GPIOB_FORCE_RESET()   (RCC->AHB1RSTR |= (RCC_AHB1RSTR_GPIOBRST))
#define __GPIOC_FORCE_RESET()   (RCC->AHB1RSTR |= (RCC_AHB1RSTR_GPIOCRST))
#define __GPIOD_FORCE_RESET()   (RCC->AHB1RSTR |= (RCC_AHB1RSTR_GPIODRST))
#define __GPIOE_FORCE_RESET()   (RCC->AHB1RSTR |= (RCC_AHB1RSTR_GPIOERST))
#define __GPIOF_FORCE_RESET()   (RCC->AHB1RSTR |= (RCC_AHB1RSTR_GPIOFRST))
#define __GPIOG_FORCE_RESET()   (RCC->AHB1RSTR |= (RCC_AHB1RSTR_GPIOGRST))
#define __GPIOH_FORCE_RESET()   (RCC->AHB1RSTR |= (RCC_AHB1RSTR_GPIOHRST))
#define __GPIOI_FORCE_RESET()   (RCC->AHB1RSTR |= (RCC_AHB1RSTR_GPIOIRST))
#define __CRC_FORCE_RESET()     (RCC->AHB1RSTR |= (RCC_AHB1RSTR_CRCRST))
#define __DMA1_FORCE_RESET()    (RCC->AHB1RSTR |= (RCC_AHB1RSTR_DMA1RST))
#define __DMA2_FORCE_RESET()    (RCC->AHB1RSTR |= (RCC_AHB1RSTR_DMA2RST))
#define __OTGHS_FORCE_RESET()   (RCC->AHB1RSTR |= (RCC_AHB1RSTR_OTGHRST))
#define __OTGHSULPI_FORCE_RESET() (RCC->AHB1RSTR |= (RCC_AHB1RSTR_OTGHSULPIRST))

#define __AHB1_RELEASE_RESET()  (RCC->AHB1RSTR = 0x00)
#define __GPIOA_RELEASE_RESET() (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_GPIOARST))
#define __GPIOB_RELEASE_RESET() (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_GPIOBRST))
#define __GPIOC_RELEASE_RESET() (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_GPIOCRST))
#define __GPIOD_RELEASE_RESET() (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_GPIODRST))
#define __GPIOE_RELEASE_RESET() (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_GPIOERST))
#define __GPIOF_RELEASE_RESET() (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_GPIOFRST))
#define __GPIOG_RELEASE_RESET() (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_GPIOGRST))
#define __GPIOH_RELEASE_RESET() (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_GPIOHRST))
#define __GPIOI_RELEASE_RESET() (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_GPIOIRST))
#define __CRC_RELEASE_RESET()   (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_CRCRST))
#define __DMA1_RELEASE_RESET()  (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_DMA1RST))
#define __DMA2_RELEASE_RESET()  (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_DMA2RST))
#define __OTGHS_RELEASE_RESET() (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_OTGHRST))
#define __OTGHSULPI_RELEASE_RESET() (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_OTGHSULPIRST))

/** @brief  Force or release AHB2 peripheral reset.
  */
#define __AHB2_FORCE_RESET()    (RCC->AHB2RSTR = 0xFFFFFFFF) 
#define __OTGFS_FORCE_RESET()   (RCC->AHB2RSTR |= (RCC_AHB2RSTR_OTGFSRST))

#define __AHB2_RELEASE_RESET()  (RCC->AHB2RSTR = 0x00)
#define __OTGFS_RELEASE_RESET() (RCC->AHB2RSTR &= ~(RCC_AHB2RSTR_OTGFSRST))

#define __RNG_FORCE_RESET()    (RCC->AHB2RSTR |= (RCC_AHB2RSTR_RNGRST))
#define __RNG_RELEASE_RESET()  (RCC->AHB2RSTR &= ~(RCC_AHB2RSTR_RNGRST))

/** @brief  Force or release APB1 peripheral reset.
  */
#define __APB1_FORCE_RESET()     (RCC->APB1RSTR = 0xFFFFFFFF)  
#define __TIM2_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_TIM2RST))
#define __TIM3_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_TIM3RST))
#define __TIM4_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_TIM4RST))
#define __TIM5_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_TIM5RST))
#define __TIM6_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_TIM6RST))
#define __TIM7_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_TIM7RST))
#define __TIM12_FORCE_RESET()    (RCC->APB1RSTR |= (RCC_APB1RSTR_TIM12RST))
#define __TIM13_FORCE_RESET()    (RCC->APB1RSTR |= (RCC_APB1RSTR_TIM13RST))
#define __TIM14_FORCE_RESET()    (RCC->APB1RSTR |= (RCC_APB1RSTR_TIM14RST))
#define __WWDG_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_WWDGRST))
#define __SPI2_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_SPI2RST))
#define __SPI3_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_SPI3RST))
#define __USART2_FORCE_RESET()   (RCC->APB1RSTR |= (RCC_APB1RSTR_USART2RST))
#define __USART3_FORCE_RESET()   (RCC->APB1RSTR |= (RCC_APB1RSTR_USART3RST))
#define __UART4_FORCE_RESET()    (RCC->APB1RSTR |= (RCC_APB1RSTR_UART4RST))
#define __UART5_FORCE_RESET()    (RCC->APB1RSTR |= (RCC_APB1RSTR_UART5RST))
#define __I2C1_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_I2C1RST))
#define __I2C2_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_I2C2RST))
#define __I2C3_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_I2C3RST))
#define __PWR_FORCE_RESET()      (RCC->APB1RSTR |= (RCC_APB1RSTR_PWRRST))
#define __CAN1_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_CAN1RST))
#define __CAN2_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_CAN2RST))
#define __DAC_FORCE_RESET()      (RCC->APB1RSTR |= (RCC_APB1RSTR_DACRST))

#define __APB1_RELEASE_RESET()   (RCC->APB1RSTR = 0x00) 
#define __TIM2_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM2RST))
#define __TIM3_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM3RST))
#define __TIM4_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM4RST))
#define __TIM5_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM5RST))
#define __TIM6_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM6RST))
#define __TIM7_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM7RST))
#define __TIM12_RELEASE_RESET()  (RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM12RST))
#define __TIM13_RELEASE_RESET()  (RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM13RST))
#define __TIM14_RELEASE_RESET()  (RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM14RST))
#define __USART3_RELEASE_RESET() (RCC->APB1RSTR &= ~(RCC_APB1RSTR_USART3RST))
#define __WWDG_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_WWDGRST))
#define __SPI2_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_SPI2RST))
#define __SPI3_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_SPI3RST))
#define __USART2_RELEASE_RESET() (RCC->APB1RSTR &= ~(RCC_APB1RSTR_USART2RST))
#define __USART3_RELEASE_RESET() (RCC->APB1RSTR &= ~(RCC_APB1RSTR_USART3RST))
#define __UART4_RELEASE_RESET()  (RCC->APB1RSTR &= ~(RCC_APB1RSTR_UART4RST))
#define __UART5_RELEASE_RESET()  (RCC->APB1RSTR &= ~(RCC_APB1RSTR_UART5RST))
#define __I2C1_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_I2C1RST))
#define __I2C2_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_I2C2RST))
#define __I2C3_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_I2C3RST))
#define __PWR_RELEASE_RESET()    (RCC->APB1RSTR &= ~(RCC_APB1RSTR_PWRRST))
#define __CAN1_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_CAN1RST))
#define __CAN2_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_CAN2RST))
#define __DAC_RELEASE_RESET()    (RCC->APB1RSTR &= ~(RCC_APB1RSTR_DACRST))

/** @brief  Force or release APB2 peripheral reset.
  */
#define __APB2_FORCE_RESET()     (RCC->APB2RSTR = 0xFFFFFFFF)  
#define __TIM1_FORCE_RESET()     (RCC->APB2RSTR |= (RCC_APB2RSTR_TIM1RST))
#define __USART1_FORCE_RESET()   (RCC->APB2RSTR |= (RCC_APB2RSTR_USART1RST))
#define __USART6_FORCE_RESET()   (RCC->APB2RSTR |= (RCC_APB2RSTR_USART6RST))
#define __ADC_FORCE_RESET()      (RCC->APB2RSTR |= (RCC_APB2RSTR_ADCRST))
#define __SDIO_FORCE_RESET()     (RCC->APB2RSTR |= (RCC_APB2RSTR_SDIORST))
#define __SPI1_FORCE_RESET()     (RCC->APB2RSTR |= (RCC_APB2RSTR_SPI1RST))
#define __SYSCFG_FORCE_RESET()   (RCC->APB2RSTR |= (RCC_APB2RSTR_SYSCFGRST))
#define __TIM8_FORCE_RESET()     (RCC->APB2RSTR |= (RCC_APB2RSTR_TIM8RST))
#define __TIM9_FORCE_RESET()     (RCC->APB2RSTR |= (RCC_APB2RSTR_TIM9RST))
#define __TIM10_FORCE_RESET()    (RCC->APB2RSTR |= (RCC_APB2RSTR_TIM10RST))
#define __TIM11_FORCE_RESET()    (RCC->APB2RSTR |= (RCC_APB2RSTR_TIM11RST))

#define __APB2_RELEASE_RESET()   (RCC->APB2RSTR = 0x00)
#define __TIM1_RELEASE_RESET()   (RCC->APB2RSTR &= ~(RCC_APB2RSTR_TIM1RST))
#define __USART1_RELEASE_RESET() (RCC->APB2RSTR &= ~(RCC_APB2RSTR_USART1RST))
#define __USART6_RELEASE_RESET() (RCC->APB2RSTR &= ~(RCC_APB2RSTR_USART6RST))
#define __ADC_RELEASE_RESET()    (RCC->APB2RSTR &= ~(RCC_APB2RSTR_ADCRST))
#define __SDIO_RELEASE_RESET()   (RCC->APB2RSTR &= ~(RCC_APB2RSTR_SDIORST))
#define __SPI1_RELEASE_RESET()   (RCC->APB2RSTR &= ~(RCC_APB2RSTR_SPI1RST))
#define __SYSCFG_RELEASE_RESET() (RCC->APB2RSTR &= ~(RCC_APB2RSTR_SYSCFGRST))
#define __TIM8_RELEASE_RESET()   (RCC->APB2RSTR &= ~(RCC_APB2RSTR_TIM8RST))
#define __TIM9_RELEASE_RESET()   (RCC->APB2RSTR &= ~(RCC_APB2RSTR_TIM9RST))
#define __TIM10_RELEASE_RESET()  (RCC->APB2RSTR &= ~(RCC_APB2RSTR_TIM10RST))
#define __TIM11_RELEASE_RESET()  (RCC->APB2RSTR &= ~(RCC_APB2RSTR_TIM11RST))

/** @brief  Force or release AHB3 peripheral reset.
  */  
#define __AHB3_FORCE_RESET() (RCC->AHB3RSTR = 0xFFFFFFFF)
#define __FSMC_FORCE_RESET()   (RCC->AHB3RSTR |= (RCC_AHB3RSTR_FSMCRST))

#define __AHB3_RELEASE_RESET() (RCC->AHB3RSTR = 0x00) 
#define __FSMC_RELEASE_RESET() (RCC->AHB3RSTR &= ~(RCC_AHB3RSTR_FSMCRST))

/** @brief  Enable or disable the AHB1 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  */
#define __GPIOA_CLK_SLEEP_ENABLE()    (RCC->AHB1LPENR |= (RCC_AHB1LPENR_GPIOALPEN))
#define __GPIOB_CLK_SLEEP_ENABLE()    (RCC->AHB1LPENR |= (RCC_AHB1LPENR_GPIOBLPEN))
#define __GPIOC_CLK_SLEEP_ENABLE()    (RCC->AHB1LPENR |= (RCC_AHB1LPENR_GPIOCLPEN))
#define __GPIOD_CLK_SLEEP_ENABLE()    (RCC->AHB1LPENR |= (RCC_AHB1LPENR_GPIODLPEN))
#define __GPIOE_CLK_SLEEP_ENABLE()    (RCC->AHB1LPENR |= (RCC_AHB1LPENR_GPIOELPEN))
#define __GPIOF_CLK_SLEEP_ENABLE()    (RCC->AHB1LPENR |= (RCC_AHB1LPENR_GPIOFLPEN))
#define __GPIOG_CLK_SLEEP_ENABLE()    (RCC->AHB1LPENR |= (RCC_AHB1LPENR_GPIOGLPEN))
#define __GPIOH_CLK_SLEEP_ENABLE()    (RCC->AHB1LPENR |= (RCC_AHB1LPENR_GPIOHLPEN))
#define __GPIOI_CLK_SLEEP_ENABLE()    (RCC->AHB1LPENR |= (RCC_AHB1LPENR_GPIOILPEN))
#define __CRC_CLK_SLEEP_ENABLE()      (RCC->AHB1LPENR |= (RCC_AHB1LPENR_CRCLPEN))
#define __FLITF_CLK_SLEEP_ENABLE()    (RCC->AHB1LPENR |= (RCC_AHB1LPENR_FLITFLPEN))
#define __SRAM1_CLK_SLEEP_ENABLE()    (RCC->AHB1LPENR |= (RCC_AHB1LPENR_SRAM1LPEN))
#define __SRAM2_CLK_SLEEP_ENABLE()    (RCC->AHB1LPENR |= (RCC_AHB1LPENR_SRAM2LPEN))
#define __BKPSRAM_CLK_SLEEP_ENABLE()  (RCC->AHB1LPENR |= (RCC_AHB1LPENR_BKPSRAMLPEN))
#define __DMA1_CLK_SLEEP_ENABLE()     (RCC->AHB1LPENR |= (RCC_AHB1LPENR_DMA1LPEN))
#define __DMA2_CLK_SLEEP_ENABLE()     (RCC->AHB1LPENR |= (RCC_AHB1LPENR_DMA2LPEN))
#define __OTGHS_CLK_SLEEP_ENABLE()    (RCC->AHB1LPENR |= (RCC_AHB1LPENR_OTGHSLPEN))
#define __OTGHSULPI_CLK_SLEEP_ENABLE() (RCC->AHB1LPENR |= (RCC_AHB1LPENR_OTGHSULPILPEN))

#define __GPIOA_CLK_SLEEP_DISABLE()   (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_GPIOALPEN))
#define __GPIOB_CLK_SLEEP_DISABLE()   (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_GPIOBLPEN))
#define __GPIOC_CLK_SLEEP_DISABLE()   (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_GPIOCLPEN))
#define __GPIOD_CLK_SLEEP_DISABLE()   (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_GPIODLPEN))
#define __GPIOE_CLK_SLEEP_DISABLE()   (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_GPIOELPEN))
#define __GPIOF_CLK_SLEEP_DISABLE()   (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_GPIOFLPEN))
#define __GPIOG_CLK_SLEEP_DISABLE()   (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_GPIOGLPEN))
#define __GPIOH_CLK_SLEEP_DISABLE()   (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_GPIOHLPEN))
#define __GPIOI_CLK_SLEEP_DISABLE()   (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_GPIOILPEN))
#define __CRC_CLK_SLEEP_DISABLE()     (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_CRCLPEN))
#define __FLITF_CLK_SLEEP_DISABLE()   (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_FLITFLPEN))
#define __SRAM1_CLK_SLEEP_DISABLE()   (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_SRAM1LPEN))
#define __SRAM2_CLK_SLEEP_DISABLE()    RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_SRAM2LPEN))
#define __BKPSRAM_CLK_SLEEP_DISABLE() (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_BKPSRAMLPEN))
#define __DMA1_CLK_SLEEP_DISABLE()    (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_DMA1LPEN))
#define __DMA2_CLK_SLEEP_DISABLE()    (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_DMA2LPEN))
#define __OTGHS_CLK_SLEEP_DISABLE()   (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_OTGHSLPEN))
#define __OTGHSULPI_CLK_SLEEP_DISABLE() (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_OTGHSULPILPEN))

/** @brief  Enable or disable the AHB2 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  */
#define __OTGFS_CLK_SLEEP_ENABLE()   (RCC->AHB2LPENR |= (RCC_AHB2LPENR_OTGFSLPEN))

#define __OTGFS_CLK_SLEEP_DISABLE()  (RCC->AHB2LPENR &= ~(RCC_AHB2LPENR_OTGFSLPEN))

#define __RNG_CLK_SLEEP_ENABLE()   (RCC->AHB2LPENR |= (RCC_AHB2LPENR_RNGLPEN))
#define __RNG_CLK_SLEEP_DISABLE()  (RCC->AHB2LPENR &= ~(RCC_AHB2LPENR_RNGLPEN))

/** @brief  Enables or disables the AHB3 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *           power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  */
#define __FSMC_CLK_SLEEP_ENABLE()    (RCC->AHB3LPENR |= (RCC_AHB3LPENR_FSMCLPEN))
#define __FSMC_CLK_SLEEP_DISABLE()   (RCC->AHB3LPENR &= ~(RCC_AHB3LPENR_FSMCLPEN))


/** @brief  Enable or disable the APB1 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  */
#define __TIM2_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_TIM2LPEN))
#define __TIM3_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_TIM3LPEN))
#define __TIM4_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_TIM4LPEN))
#define __TIM5_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_TIM5LPEN))
#define __TIM6_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_TIM6LPEN))
#define __TIM7_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_TIM7LPEN))
#define __TIM12_CLK_SLEEP_ENABLE()   (RCC->APB1LPENR |= (RCC_APB1LPENR_TIM12LPEN))
#define __TIM13_CLK_SLEEP_ENABLE()   (RCC->APB1LPENR |= (RCC_APB1LPENR_TIM13LPEN))
#define __TIM14_CLK_SLEEP_ENABLE()   (RCC->APB1LPENR |= (RCC_APB1LPENR_TIM14LPEN))
#define __WWDG_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_WWDGLPEN))
#define __SPI2_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_SPI2LPEN))
#define __SPI3_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_SPI3LPEN))
#define __USART2_CLK_SLEEP_ENABLE()  (RCC->APB1LPENR |= (RCC_APB1LPENR_USART2LPEN))
#define __USART3_CLK_SLEEP_ENABLE()  (RCC->APB1LPENR |= (RCC_APB1LPENR_USART3LPEN))
#define __UART4_CLK_SLEEP_ENABLE()   (RCC->APB1LPENR |= (RCC_APB1LPENR_UART4LPEN))
#define __UART5_CLK_SLEEP_ENABLE()   (RCC->APB1LPENR |= (RCC_APB1LPENR_UART5LPEN))
#define __I2C1_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_I2C1LPEN))
#define __I2C2_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_I2C2LPEN))
#define __I2C3_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_I2C3LPEN))
#define __PWR_CLK_SLEEP_ENABLE()     (RCC->APB1LPENR |= (RCC_APB1LPENR_PWRLPEN))
#define __CAN1_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_CAN1LPEN))
#define __CAN2_CLK_SLEEP_ENABLE()    (RCC->APB1LPENR |= (RCC_APB1LPENR_CAN2LPEN))
#define __DAC_CLK_SLEEP_ENABLE()     (RCC->APB1LPENR |= (RCC_APB1LPENR_DACLPEN))                                  
                                      
#define __TIM2_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_TIM2LPEN))
#define __TIM3_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_TIM3LPEN))
#define __TIM4_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_TIM4LPEN))
#define __TIM5_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_TIM5LPEN))
#define __TIM6_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_TIM6LPEN))
#define __TIM7_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_TIM7LPEN))
#define __TIM12_CLK_SLEEP_DISABLE()  (RCC->APB1LPENR &= ~(RCC_APB1LPENR_TIM12LPEN))
#define __TIM13_CLK_SLEEP_DISABLE()  (RCC->APB1LPENR &= ~(RCC_APB1LPENR_TIM13LPEN))
#define __TIM14_CLK_SLEEP_DISABLE()  (RCC->APB1LPENR &= ~(RCC_APB1LPENR_TIM14LPEN))
#define __WWDG_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_WWDGLPEN))
#define __SPI2_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_SPI2LPEN))
#define __SPI3_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_SPI3LPEN))
#define __USART2_CLK_SLEEP_DISABLE() (RCC->APB1LPENR &= ~(RCC_APB1LPENR_USART2LPEN))
#define __USART3_CLK_SLEEP_DISABLE() (RCC->APB1LPENR &= ~(RCC_APB1LPENR_USART3LPEN))
#define __UART4_CLK_SLEEP_DISABLE()  (RCC->APB1LPENR &= ~(RCC_APB1LPENR_UART4LPEN))
#define __UART5_CLK_SLEEP_DISABLE()  (RCC->APB1LPENR &= ~(RCC_APB1LPENR_UART5LPEN))
#define __I2C1_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_I2C1LPEN))
#define __I2C2_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_I2C2LPEN))
#define __I2C3_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_I2C3LPEN))
#define __PWR_CLK_SLEEP_DISABLE()    (RCC->APB1LPENR &= ~(RCC_APB1LPENR_PWRLPEN))
#define __CAN1_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_CAN1LPEN))
#define __CAN2_CLK_SLEEP_DISABLE()   (RCC->APB1LPENR &= ~(RCC_APB1LPENR_CAN2LPEN))
#define __DAC_CLK_SLEEP_DISABLE()    (RCC->APB1LPENR &= ~(RCC_APB1LPENR_DACLPEN))

/** @brief  Enable or disable the APB2 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *           power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  */
#define __TIM1_CLK_SLEEP_ENABLE()    (RCC->APB2LPENR |= (RCC_APB2LPENR_TIM1LPEN))
#define __USART1_CLK_SLEEP_ENABLE()  (RCC->APB2LPENR |= (RCC_APB2LPENR_USART1LPEN))
#define __USART6_CLK_SLEEP_ENABLE()  (RCC->APB2LPENR |= (RCC_APB2LPENR_USART6LPEN))
#define __ADC1_CLK_SLEEP_ENABLE()    (RCC->APB2LPENR |= (RCC_APB2LPENR_ADC1LPEN))
#define __SDIO_CLK_SLEEP_ENABLE()    (RCC->APB2LPENR |= (RCC_APB2LPENR_SDIOLPEN))
#define __SPI1_CLK_SLEEP_ENABLE()    (RCC->APB2LPENR |= (RCC_APB2LPENR_SPI1LPEN))
#define __SYSCFG_CLK_SLEEP_ENABLE()  (RCC->APB2LPENR |= (RCC_APB2LPENR_SYSCFGLPEN))
#define __TIM8_CLK_SLEEP_ENABLE()    (RCC->APB2LPENR |= (RCC_APB2LPENR_TIM8LPEN))
#define __TIM9_CLK_SLEEP_ENABLE()    (RCC->APB2LPENR |= (RCC_APB2LPENR_TIM9LPEN))
#define __TIM10_CLK_SLEEP_ENABLE()   (RCC->APB2LPENR |= (RCC_APB2LPENR_TIM10LPEN))
#define __TIM11_CLK_SLEEP_ENABLE()   (RCC->APB2LPENR |= (RCC_APB2LPENR_TIM11LPEN))
#define __ADC2_CLK_SLEEP_ENABLE()    (RCC->APB2LPENR |= (RCC_APB2LPENR_ADC2LPEN))
#define __ADC3_CLK_SLEEP_ENABLE()    (RCC->APB2LPENR |= (RCC_APB2LPENR_ADC3LPEN))

#define __TIM1_CLK_SLEEP_DISABLE()   (RCC->APB2LPENR &= ~(RCC_APB2LPENR_TIM1LPEN))
#define __USART1_CLK_SLEEP_DISABLE() (RCC->APB2LPENR &= ~(RCC_APB2LPENR_USART1LPEN))
#define __USART6_CLK_SLEEP_DISABLE() (RCC->APB2LPENR &= ~(RCC_APB2LPENR_USART6LPEN))
#define __ADC1_CLK_SLEEP_DISABLE()   (RCC->APB2LPENR &= ~(RCC_APB2LPENR_ADC1LPEN))
#define __SDIO_CLK_SLEEP_DISABLE()   (RCC->APB2LPENR &= ~(RCC_APB2LPENR_SDIOLPEN))
#define __SPI1_CLK_SLEEP_DISABLE()   (RCC->APB2LPENR &= ~(RCC_APB2LPENR_SPI1LPEN))
#define __SYSCFG_CLK_SLEEP_DISABLE() (RCC->APB2LPENR &= ~(RCC_APB2LPENR_SYSCFGLPEN))
#define __TIM8_CLK_SLEEP_DISABLE()   (RCC->APB2LPENR &= ~(RCC_APB2LPENR_TIM8LPEN))
#define __TIM9_CLK_SLEEP_DISABLE()   (RCC->APB2LPENR &= ~(RCC_APB2LPENR_TIM9LPEN))
#define __TIM10_CLK_SLEEP_DISABLE()  (RCC->APB2LPENR &= ~(RCC_APB2LPENR_TIM10LPEN))
#define __TIM11_CLK_SLEEP_DISABLE()  (RCC->APB2LPENR &= ~(RCC_APB2LPENR_TIM11LPEN))
#define __ADC2_CLK_SLEEP_DISABLE()   (RCC->APB2LPENR &= ~(RCC_APB2LPENR_ADC2LPEN))
#define __ADC3_CLK_SLEEP_DISABLE()   (RCC->APB2LPENR &= ~(RCC_APB2LPENR_ADC3LPEN))

/** @brief  Macros to enable or disable the Internal High Speed oscillator (HSI).
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
  */
#define __HAL_RCC_HSI_ENABLE() (*(__IO uint32_t *) CR_HSION_BB = ENABLE)
#define __HAL_RCC_HSI_DISABLE() (*(__IO uint32_t *) CR_HSION_BB = DISABLE)

/** @brief  Macro to adjust the Internal High Speed oscillator (HSI) calibration value.
  * @note   The calibration is used to compensate for the variations in voltage
  *         and temperature that influence the frequency of the internal HSI RC.
  * @param  __HSICalibrationValue__: specifies the calibration trimming value.
  *         This parameter must be a number between 0 and 0x1F.
  */
#define __HAL_RCC_HSI_CALIBRATIONVALUE_ADJUST(__HSICalibrationValue__) (MODIFY_REG(RCC->CR,\
        RCC_CR_HSITRIM, (uint32_t)(__HSICalibrationValue__) << POSITION_VAL(RCC_CR_HSITRIM)))

/** @brief  Macros to enable or disable the Internal Low Speed oscillator (LSI).
  * @note   After enabling the LSI, the application software should wait on 
  *         LSIRDY flag to be set indicating that LSI clock is stable and can
  *         be used to clock the IWDG and/or the RTC.
  * @note   LSI can not be disabled if the IWDG is running.
  * @note   When the LSI is stopped, LSIRDY flag goes low after 6 LSI oscillator
  *         clock cycles. 
  */
#define __HAL_RCC_LSI_ENABLE() (*(__IO uint32_t *) CSR_LSION_BB = ENABLE)
#define __HAL_RCC_LSI_DISABLE() (*(__IO uint32_t *) CSR_LSION_BB = DISABLE)

/**
  * @brief  Macro to configure the External High Speed oscillator (HSE).
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
  *            @arg RCC_HSE_OFF: turn OFF the HSE oscillator, HSERDY flag goes low after
  *                              6 HSE oscillator clock cycles.
  *            @arg RCC_HSE_ON: turn ON the HSE oscillator.
  *            @arg RCC_HSE_BYPASS: HSE oscillator bypassed with external clock.
  */
#define __HAL_RCC_HSE_CONFIG(__STATE__) (*(__IO uint8_t *) CR_BYTE2_ADDRESS = (__STATE__))

/**
  * @brief  Macro to configure the External Low Speed oscillator (LSE).
  * @note   As the LSE is in the Backup domain and write access is denied to
  *         this domain after reset, you have to enable write access using 
  *         HAL_PWR_EnableBkUpAccess() function before to configure the LSE
  *         (to be done once after reset).  
  * @note   After enabling the LSE (RCC_LSE_ON or RCC_LSE_BYPASS), the application
  *         software should wait on LSERDY flag to be set indicating that LSE clock
  *         is stable and can be used to clock the RTC.
  * @param  __STATE__: specifies the new state of the LSE.
  *         This parameter can be one of the following values:
  *            @arg RCC_LSE_OFF: turn OFF the LSE oscillator, LSERDY flag goes low after
  *                              6 LSE oscillator clock cycles.
  *            @arg RCC_LSE_ON: turn ON the LSE oscillator.
  *            @arg RCC_LSE_BYPASS: LSE oscillator bypassed with external clock.
  */
#define __HAL_RCC_LSE_CONFIG(__STATE__)  (*(__IO uint8_t *) BDCR_BYTE0_ADDRESS = (__STATE__))

/** @brief  Macros to enable or disable the the RTC clock.
  * @note   These macros must be used only after the RTC clock source was selected.
  */
#define __HAL_RCC_RTC_ENABLE() (*(__IO uint32_t *) BDCR_RTCEN_BB = ENABLE)
#define __HAL_RCC_RTC_DISABLE() (*(__IO uint32_t *) BDCR_RTCEN_BB = DISABLE)

/** @brief  Macros to configure the RTC clock (RTCCLK).
  * @note   As the RTC clock configuration bits are in the Backup domain and write
  *         access is denied to this domain after reset, you have to enable write
  *         access using the Power Backup Access macro before to configure
  *         the RTC clock source (to be done once after reset).    
  * @note   Once the RTC clock is configured it can't be changed unless the  
  *         Backup domain is reset using __HAL_RCC_BackupReset_RELEASE() macro, or by
  *         a Power On Reset (POR).
  * @param  __RTCCLKSource__: specifies the RTC clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_RTCCLKSOURCE_LSE: LSE selected as RTC clock.
  *            @arg RCC_RTCCLKSOURCE_LSI: LSI selected as RTC clock.
  *            @arg RCC_RTCCLKSOURCE_HSE_DIVx: HSE clock divided by x selected
  *                                            as RTC clock, where x:[2,31]
  * @note   If the LSE or LSI is used as RTC clock source, the RTC continues to
  *         work in STOP and STANDBY modes, and can be used as wakeup source.
  *         However, when the HSE clock is used as RTC clock source, the RTC
  *         cannot be used in STOP and STANDBY modes.    
  * @note   The maximum input clock frequency for RTC is 1MHz (when using HSE as
  *         RTC clock source).
  */
#define __HAL_RCC_RTC_CLKPRESCALER(__RTCCLKSource__) (((__RTCCLKSource__) & RCC_BDCR_RTCSEL) == RCC_BDCR_RTCSEL) ?    \
                                                 MODIFY_REG(RCC->CFGR, RCC_CFGR_RTCPRE, ((__RTCCLKSource__) & 0xFFFFCFF)) : CLEAR_BIT(RCC->CFGR, RCC_CFGR_RTCPRE)
                                                   
#define __HAL_RCC_RTC_CONFIG(__RTCCLKSource__) do { __HAL_RCC_RTC_CLKPRESCALER(__RTCCLKSource__);    \
                                                    RCC->BDCR |= ((__RTCCLKSource__) & 0x00000FFF);  \
                                                   } while (0)

/** @brief  Macros to force or release the Backup domain reset.
  * @note   This function resets the RTC peripheral (including the backup registers)
  *         and the RTC clock source selection in RCC_CSR register.
  * @note   The BKPSRAM is not affected by this reset.   
  */
#define __HAL_RCC_BACKUPRESET_FORCE() (*(__IO uint32_t *) BDCR_BDRST_BB = ENABLE)
#define __HAL_RCC_BACKUPRESET_RELEASE() (*(__IO uint32_t *) BDCR_BDRST_BB = DISABLE)

/** @brief  Macros to enable or disable the main PLL.
  * @note   After enabling the main PLL, the application software should wait on 
  *         PLLRDY flag to be set indicating that PLL clock is stable and can
  *         be used as system clock source.
  * @note   The main PLL can not be disabled if it is used as system clock source
  * @note   The main PLL is disabled by hardware when entering STOP and STANDBY modes.
  */
#define __HAL_RCC_PLL_ENABLE() (*(__IO uint32_t *) CR_PLLON_BB = ENABLE)
#define __HAL_RCC_PLL_DISABLE() (*(__IO uint32_t *) CR_PLLON_BB = DISABLE)

/** @brief  Macro to configure the main PLL clock source, multiplication and division factors.
  * @note   This function must be used only when the main PLL is disabled.
  * @param  __RCC_PLLSource__: specifies the PLL entry clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_PLLSOURCE_HSI: HSI oscillator clock selected as PLL clock entry
  *            @arg RCC_PLLSOURCE_HSE: HSE oscillator clock selected as PLL clock entry
  * @note   This clock source (RCC_PLLSource) is common for the main PLL and PLLI2S.  
  * @param  __PLLM__: specifies the division factor for PLL VCO input clock
  *         This parameter must be a number between Min_Data = 2 and Max_Data = 63.
  * @note   You have to set the PLLM parameter correctly to ensure that the VCO input
  *         frequency ranges from 1 to 2 MHz. It is recommended to select a frequency
  *         of 2 MHz to limit PLL jitter.
  * @param  __PLLN__: specifies the multiplication factor for PLL VCO output clock
  *         This parameter must be a number between Min_Data = 192 and Max_Data = 432.
  * @note   You have to set the PLLN parameter correctly to ensure that the VCO
  *         output frequency is between 192 and 432 MHz.
  * @param  __PLLP__: specifies the division factor for main system clock (SYSCLK)
  *         This parameter must be a number in the range {2, 4, 6, or 8}.
  * @note   You have to set the PLLP parameter correctly to not exceed 120 MHz on
  *         the System clock frequency.
  * @param  __PLLQ__: specifies the division factor for OTG FS, SDIO and RNG clocks
  *         This parameter must be a number between Min_Data = 2 and Max_Data = 15.
  * @note   If the USB OTG FS is used in your application, you have to set the
  *         PLLQ parameter correctly to have 48 MHz clock for the USB. However,
  *         the SDIO and RNG need a frequency lower than or equal to 48 MHz to work
  *         correctly.
  */
#define __HAL_RCC_PLL_CONFIG(__RCC_PLLSource__, __PLLM__, __PLLN__, __PLLP__, __PLLQ__)\
                            (RCC->PLLCFGR = (0x20000000 | (__PLLM__) | ((__PLLN__) << POSITION_VAL(RCC_PLLCFGR_PLLN)) | \
                            ((((__PLLP__) >> 1) -1) << POSITION_VAL(RCC_PLLCFGR_PLLP)) | (__RCC_PLLSource__) | \
                            ((__PLLQ__) << POSITION_VAL(RCC_PLLCFGR_PLLQ))))

/** @brief  Macro to configure the I2S clock source (I2SCLK).
  * @note   This function must be called before enabling the I2S APB clock.
  * @param  __SOURCE__: specifies the I2S clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_I2SCLKSOURCE_PLLI2S: PLLI2S clock used as I2S clock source.
  *            @arg RCC_I2SCLKSOURCE_EXT: External clock mapped on the I2S_CKIN pin
  *                                       used as I2S clock source.
  */
#define __HAL_RCC_I2SCLK(__SOURCE__) (*(__IO uint32_t *) CFGR_I2SSRC_BB = (__SOURCE__))

/** @brief Macros to enable or disable the PLLI2S. 
  * @note  The PLLI2S is disabled by hardware when entering STOP and STANDBY modes.
  */
#define __HAL_RCC_PLLI2S_ENABLE() (*(__IO uint32_t *) CR_PLLI2SON_BB = ENABLE)
#define __HAL_RCC_PLLI2S_DISABLE() (*(__IO uint32_t *) CR_PLLI2SON_BB = DISABLE)

/** @brief  Macro to configure the PLLI2S clock multiplication and division factors .
  * @note   This macro must be used only when the PLLI2S is disabled.
  * @note   PLLI2S clock source is common with the main PLL (configured in 
  *         HAL_RCC_ClockConfig() API).  
  * @param  __PLLI2SN__: specifies the multiplication factor for PLLI2S VCO output clock
  *         This parameter must be a number between Min_Data = 192 and Max_Data = 432.
  * @note   You have to set the PLLI2SN parameter correctly to ensure that the VCO 
  *         output frequency is between Min_Data = 192 and Max_Data = 432 MHz.
  * @param  __PLLI2SR__: specifies the division factor for I2S clock
  *         This parameter must be a number between Min_Data = 2 and Max_Data = 7.
  * @note   You have to set the PLLI2SR parameter correctly to not exceed 192 MHz
  *         on the I2S clock frequency.
  */
#define __HAL_RCC_PLLI2S_CONFIG(__PLLI2SN__, __PLLI2SR__) (RCC->PLLI2SCFGR = ((__PLLI2SN__) << POSITION_VAL(RCC_PLLI2SCFGR_PLLI2SN)) | ((__PLLI2SR__) << POSITION_VAL(RCC_PLLI2SCFGR_PLLI2SR)))

/** @brief  Macro to get the clock source used as system clock.
  * @retval The clock source used as system clock. The returned value can be one
  *         of the following:
  *              - RCC_CFGR_SWS_HSI: HSI used as system clock.
  *              - RCC_CFGR_SWS_HSE: HSE used as system clock.
  *              - RCC_CFGR_SWS_PLL: PLL used as system clock.
  */     
#define __HAL_RCC_GET_SYSCLK_SOURCE() ((uint32_t)(RCC->CFGR & RCC_CFGR_SWS))

/** @brief  Macro to get the oscillator used as PLL clock source.
  * @retval The oscillator used as PLL clock source. The returned value can be one
  *         of the following:
  *              - RCC_PLLSOURCE_HSI: HSI oscillator is used as PLL clock source.
  *              - RCC_PLLSOURCE_HSE: HSE oscillator is used as PLL clock source.
  */
#define __HAL_RCC_GET_PLL_OSCSOURCE() ((uint32_t)(RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC))

/** @defgroup RCC_Flags_Interrupts_Management
  * @brief macros to manage the specified RCC Flags and interrupts.
  * @{
  */

/** @brief  Enable RCC interrupt (Perform Byte access to RCC_CIR[14:8] bits to enable
  *         the selected interrupts).
  * @param  __INTERRUPT__: specifies the RCC interrupt sources to be enabled.
  *         This parameter can be any combination of the following values:
  *            @arg RCC_IT_LSIRDY: LSI ready interrupt.
  *            @arg RCC_IT_LSERDY: LSE ready interrupt.
  *            @arg RCC_IT_HSIRDY: HSI ready interrupt.
  *            @arg RCC_IT_HSERDY: HSE ready interrupt.
  *            @arg RCC_IT_PLLRDY: Main PLL ready interrupt.
  *            @arg RCC_IT_PLLI2SRDY: PLLI2S ready interrupt.
  */
#define __HAL_RCC_ENABLE_IT(__INTERRUPT__) (*(__IO uint8_t *) CIR_BYTE1_ADDRESS |= (__INTERRUPT__))

/** @brief Disable RCC interrupt (Perform Byte access to RCC_CIR[14:8] bits to disable 
  *        the selected interrupts).
  * @param  __INTERRUPT__: specifies the RCC interrupt sources to be disabled.
  *         This parameter can be any combination of the following values:
  *            @arg RCC_IT_LSIRDY: LSI ready interrupt.
  *            @arg RCC_IT_LSERDY: LSE ready interrupt.
  *            @arg RCC_IT_HSIRDY: HSI ready interrupt.
  *            @arg RCC_IT_HSERDY: HSE ready interrupt.
  *            @arg RCC_IT_PLLRDY: Main PLL ready interrupt.
  *            @arg RCC_IT_PLLI2SRDY: PLLI2S ready interrupt.
  */
#define __HAL_RCC_DISABLE_IT(__INTERRUPT__) (*(__IO uint8_t *) CIR_BYTE1_ADDRESS &= ~(__INTERRUPT__))

/** @brief  Clear the RCC's interrupt pending bits (Perform Byte access to RCC_CIR[23:16]
  *         bits to clear the selected interrupt pending bits.
  * @param  __INTERRUPT__: specifies the interrupt pending bit to clear.
  *         This parameter can be any combination of the following values:
  *            @arg RCC_IT_LSIRDY: LSI ready interrupt.
  *            @arg RCC_IT_LSERDY: LSE ready interrupt.
  *            @arg RCC_IT_HSIRDY: HSI ready interrupt.
  *            @arg RCC_IT_HSERDY: HSE ready interrupt.
  *            @arg RCC_IT_PLLRDY: Main PLL ready interrupt.
  *            @arg RCC_IT_PLLI2SRDY: PLLI2S ready interrupt.  
  *            @arg RCC_IT_CSS: Clock Security System interrupt
  */
#define __HAL_RCC_CLEAR_IT(__INTERRUPT__) (*(__IO uint8_t *) CIR_BYTE2_ADDRESS = (__INTERRUPT__))

/** @brief  Check the RCC's interrupt has occurred or not.
  * @param  __INTERRUPT__: specifies the RCC interrupt source to check.
  *         This parameter can be one of the following values:
  *            @arg RCC_IT_LSIRDY: LSI ready interrupt.
  *            @arg RCC_IT_LSERDY: LSE ready interrupt.
  *            @arg RCC_IT_HSIRDY: HSI ready interrupt.
  *            @arg RCC_IT_HSERDY: HSE ready interrupt.
  *            @arg RCC_IT_PLLRDY: Main PLL ready interrupt.
  *            @arg RCC_IT_PLLI2SRDY: PLLI2S ready interrupt.
  *            @arg RCC_IT_CSS: Clock Security System interrupt
  * @retval The new state of __INTERRUPT__ (TRUE or FALSE).
  */
#define __HAL_RCC_GET_IT(__INTERRUPT__) ((RCC->CIR & (__INTERRUPT__)) == (__INTERRUPT__))

/** @brief Set RMVF bit to clear the reset flags: RCC_FLAG_PINRST, RCC_FLAG_PORRST, 
  *        RCC_FLAG_SFTRST, RCC_FLAG_IWDGRST, RCC_FLAG_WWDGRST and RCC_FLAG_LPWRRST.
  */
#define __HAL_RCC_CLEAR_RESET_FLAGS() (RCC->CSR |= RCC_CSR_RMVF)

/** @brief  Check RCC flag is set or not.
  * @param  __FLAG__: specifies the flag to check.
  *         This parameter can be one of the following values:
  *            @arg RCC_FLAG_HSIRDY: HSI oscillator clock ready.
  *            @arg RCC_FLAG_HSERDY: HSE oscillator clock ready.
  *            @arg RCC_FLAG_PLLRDY: Main PLL clock ready.
  *            @arg RCC_FLAG_PLLI2SRDY: PLLI2S clock ready.
  *            @arg RCC_FLAG_LSERDY: LSE oscillator clock ready.
  *            @arg RCC_FLAG_LSIRDY: LSI oscillator clock ready.
  *            @arg RCC_FLAG_BORRST: POR/PDR or BOR reset.
  *            @arg RCC_FLAG_PINRST: Pin reset.
  *            @arg RCC_FLAG_PORRST: POR/PDR reset.
  *            @arg RCC_FLAG_SFTRST: Software reset.
  *            @arg RCC_FLAG_IWDGRST: Independent Watchdog reset.
  *            @arg RCC_FLAG_WWDGRST: Window Watchdog reset.
  *            @arg RCC_FLAG_LPWRRST: Low Power reset.
  * @retval The new state of __FLAG__ (TRUE or FALSE).
  */
#define RCC_FLAG_MASK  ((uint8_t)0x1F)
#define __HAL_RCC_GET_FLAG(__FLAG__) (((((((__FLAG__) >> 5) == 1)? RCC->CR :((((__FLAG__) >> 5) == 2) ? RCC->BDCR :((((__FLAG__) >> 5) == 3)? RCC->CSR :RCC->CIR))) & ((uint32_t)1 << ((__FLAG__) & RCC_FLAG_MASK)))!= 0)? 1 : 0)
/**
  * @}
  */

#define __RCC_PLLSRC() ((RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) >> POSITION_VAL(RCC_PLLCFGR_PLLSRC))


/* Include RCC HAL Extension module */
#include "stm32f2xx_hal_rcc_ex.h"

/* Exported functions --------------------------------------------------------*/
                              
/* Initialization and de-initialization functions  ******************************/
void HAL_RCC_DeInit(void);
HAL_StatusTypeDef HAL_RCC_OscConfig(RCC_OscInitTypeDef *RCC_OscInitStruct);
HAL_StatusTypeDef HAL_RCC_ClockConfig(RCC_ClkInitTypeDef *RCC_ClkInitStruct, uint32_t FLatency);

/* Peripheral Control functions  ************************************************/
void     HAL_RCC_MCOConfig(uint32_t RCC_MCOx, uint32_t RCC_MCOSource, uint32_t RCC_MCODiv);
void     HAL_RCC_EnableCSS(void);
void     HAL_RCC_DisableCSS(void);
uint32_t HAL_RCC_GetSysClockFreq(void);
uint32_t HAL_RCC_GetHCLKFreq(void);
uint32_t HAL_RCC_GetPCLK1Freq(void);
uint32_t HAL_RCC_GetPCLK2Freq(void);
void     HAL_RCC_GetOscConfig(RCC_OscInitTypeDef *RCC_OscInitStruct);
void     HAL_RCC_GetClockConfig(RCC_ClkInitTypeDef *RCC_ClkInitStruct, uint32_t *pFLatency);

/* CSS NMI IRQ handler */
void HAL_RCC_NMI_IRQHandler(void);

/* User Callbacks in non blocking mode (IT mode) */ 
void HAL_RCC_CCSCallback(void);

/**
  * @}
  */ 

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F2xx_HAL_RCC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
