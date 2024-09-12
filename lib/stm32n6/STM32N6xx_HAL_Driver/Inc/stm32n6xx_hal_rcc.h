/**
  ******************************************************************************
  * @file    stm32n6xx_hal_rcc.h
  * @author  MCD Application Team
  * @brief   Header file of RCC HAL module.
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
#ifndef STM32N6xx_HAL_RCC_H
#define STM32N6xx_HAL_RCC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"
#include "stm32n6xx_ll_bus.h"
#include "stm32n6xx_ll_rcc.h"

/** @addtogroup STM32N6xx_HAL_Driver
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
  *         (allow to configure the PLL in integer and fractional modes)
  *         Only PLLState field is applicable when its value is different from RCC_PLL_ON
  */
typedef struct
{
  uint32_t PLLState;            /*!< The new state of the PLL.
                                     This parameter can be a value of @ref RCC_PLL_Config            */

  uint32_t PLLSource;           /*!< PLL entry clock source.
                                     This parameter must be a value of @ref RCC_PLL_Clock_Source     */

  uint32_t PLLM;                /*!< Division factor M for PLL VCO input clock.
                                     This parameter must be a number between Min_Data = 1 and Max_Data = 63    */

  uint32_t PLLFractional;       /*!< Fractional part of he VCO mulliplication factor.
                                     This parameter must be a number between Min_Data = 0 and Max_Data = 0xFFFFFF */

  uint32_t PLLN;                /*!< Multiplication factor N for PLL VCO output clock.
                                     In integer mode, this parameter must be a number between Min_Data = 10 and Max_Data = 2500.
                                     In fractional mode, this parameter must be a number between Min_Data = 20 and Max_Data = 500.*/

  uint32_t PLLP1;               /*!< Division factor P1 for system clock.
                                     This parameter must be a number between Min_Data = 1 and Max_Data = 7     */

  uint32_t PLLP2;               /*!< Division factor P2 for system clock.
                                     This parameter must be a number between Min_Data = 1 and Max_Data = 7     */

} RCC_PLLInitTypeDef;

/**
  * @brief  RCC Internal/External Oscillator (HSE, HSI, MSI, LSE and LSI) and PLLs configuration structure definition
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

  uint32_t HSIDiv;               /*!< The division factor of the HSI.
                                      This parameter can be a value of @ref RCC_HSI_Div                           */

  uint32_t HSICalibrationValue;  /*!< The calibration trimming value.
                                      This parameter must be a number between Min_Data = 0 and Max_Data = 127     */

  uint32_t LSIState;             /*!< The new state of the LSI.
                                      This parameter can be a value of @ref RCC_LSI_Config                        */

  uint32_t MSIState;             /*!< The new state of the MSI.
                                      This parameter can be a value of @ref RCC_MSI_Config */

  uint32_t MSIFrequency;         /*!< The MSI frequency selection.
                                      This parameter can be a value of @ref RCC_MSI_Frequency */

  uint32_t MSICalibrationValue;  /*!< The calibration trimming value.
                                      This parameter must be a number between Min_Data = 0 and Max_Data = 31      */

  RCC_PLLInitTypeDef PLL1;       /*!< PLL1 structure parameters                                                   */

  RCC_PLLInitTypeDef PLL2;       /*!< PLL2 structure parameters                                                   */

  RCC_PLLInitTypeDef PLL3;       /*!< PLL3 structure parameters                                                   */

  RCC_PLLInitTypeDef PLL4;       /*!< PLL4 structure parameters                                                   */

} RCC_OscInitTypeDef;

/**
  * @brief  RCC extended interconnection structure definition
  */
typedef struct
{
  uint32_t  ClockSelection;        /*!< Specifies ICx clock source.
                                        This parameter can be a value of @ref RCC_IC_Clock_Source */

  uint32_t  ClockDivider;          /*!< Specifies ICx clock divider.
                                        This parameter must be a number between Min_Data = 1 and Max_Data = 256 */
} RCC_ICInitTypeDef;

/**
  * @brief  RCC System, AHB and APB busses clock configuration structure definition
  */
typedef struct
{
  uint32_t ClockType;             /*!< The clock to be configured.
                                       This parameter can be a value of @ref RCC_Clock_Type                           */

  uint32_t CPUCLKSource;          /*!< The clock source used as CPU clock (CPUCLK).
                                       This parameter can be a value of @ref RCC_CPU_Clock_Source                     */

  uint32_t SYSCLKSource;          /*!< The clock source used as system bus clock (SYSCLK).
                                       This parameter can be a value of @ref RCC_System_Bus_Clock_Source              */

  uint32_t AHBCLKDivider;         /*!< The AHB clock divider for HCLK.
                                       This clock is derived from the system clock divided by the system clock divider.
                                       This parameter can be a value of @ref RCC_HCLK_Clock_Source                    */

  uint32_t APB1CLKDivider;        /*!< The APB1 clock divider for PCLK1.
                                       This clock is derived from the AHB clock (HCLK).
                                       This parameter can be a value of @ref RCC_APB1_Clock_Source                    */

  uint32_t APB2CLKDivider;        /*!< The APB2 clock divider for PCLK2.
                                       This clock is derived from the AHB clock (HCLK).
                                       This parameter can be a value of @ref RCC_APB2_Clock_Source                    */

  uint32_t APB4CLKDivider;        /*!< The APB4 clock divider for PCLK4.
                                       This clock is derived from the AHB clock (HCLK).
                                       This parameter can be a value of @ref RCC_APB4_Clock_Source                    */

  uint32_t APB5CLKDivider;        /*!< The APB5 clock divider for PCLK5.
                                       This clock is derived from the AHB clock (HCLK).
                                       This parameter can be a value of @ref RCC_APB5_Clock_Source                    */

  RCC_ICInitTypeDef IC1Selection; /*!< IC1 parameters.
                                       This parameter shall be used when IC1 is selected as CPU clock source (sysa_ck)         */

  RCC_ICInitTypeDef IC2Selection; /*!< IC2 parameters.
                                       This parameter shall be used when IC2 is selected as system bus clock source (sysb_ck)  */

  RCC_ICInitTypeDef IC6Selection; /*!< IC6 parameters.
                                       This parameter shall be used when IC6 is selected as system bus clock source (sysc_ck)  */

  RCC_ICInitTypeDef IC11Selection; /*!< IC11 parameters.
                                       This parameter shall be used when IC11 is selected as system bus clock source (sysd_ck) */
} RCC_ClkInitTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup RCC_Exported_Constants RCC Exported Constants
  * @{
  */

/** @defgroup RCC_Oscillator_Type  Oscillator Type
  * @{
  */
#define RCC_OSCILLATORTYPE_NONE        0x00U  /*!< Oscillator configuration unchanged */
#define RCC_OSCILLATORTYPE_HSE         0x01U  /*!< HSE selected */
#define RCC_OSCILLATORTYPE_HSI         0x02U  /*!< HSI selected */
#define RCC_OSCILLATORTYPE_LSE         0x04U  /*!< LSE selected */
#define RCC_OSCILLATORTYPE_LSI         0x08U  /*!< LSI selected */
#define RCC_OSCILLATORTYPE_MSI         0x10U  /*!< MSI selected */
/**
  * @}
  */

/** @defgroup RCC_HSE_Config  HSE Config
  * @{
  */
#define RCC_HSE_OFF                    0U                                                       /*!< HSE clock deactivation */
#define RCC_HSE_ON                     RCC_CR_HSEON                                             /*!< HSE clock activation */
#define RCC_HSE_BYPASS                 (RCC_HSECFGR_HSEBYP | RCC_CR_HSEON)                      /*!< HSE bypass analog clock activation */
#define RCC_HSE_BYPASS_DIGITAL         (RCC_HSECFGR_HSEEXT | RCC_HSECFGR_HSEBYP | RCC_CR_HSEON) /*!< HSE bypass digital clock activation */
/**
  * @}
  */

/** @defgroup RCC_LSE_Config  LSE Config
  * @{
  */
#define RCC_LSE_OFF                    0U                                                       /*!< LSE clock deactivation */
#define RCC_LSE_ON                     RCC_CR_LSEON                                             /*!< LSE clock activation */
#define RCC_LSE_BYPASS                 (RCC_LSECFGR_LSEBYP | RCC_CR_LSEON)                      /*!< LSE bypass analog clock activation */
#define RCC_LSE_BYPASS_DIGITAL         (RCC_LSECFGR_LSEEXT | RCC_LSECFGR_LSEBYP | RCC_CR_LSEON) /*!< LSE bypass digital clock activation */
/**
  * @}
  */

/** @defgroup RCC_HSI_Config  HSI Config
  * @{
  */
#define RCC_HSI_OFF                    0U                   /*!< HSI clock deactivation */
#define RCC_HSI_ON                     RCC_CR_HSION         /*!< HSI clock activation */
/**
  * @}
  */

/** @defgroup RCC_HSI_Div HSI Clock Divider
  * @{
  */
#define RCC_HSI_DIV1                   LL_RCC_HSI_DIV_1  /*!< HSI clock is not divided */
#define RCC_HSI_DIV2                   LL_RCC_HSI_DIV_2  /*!< HSI clock is divided by 2 */
#define RCC_HSI_DIV4                   LL_RCC_HSI_DIV_4  /*!< HSI clock is divided by 4 */
#define RCC_HSI_DIV8                   LL_RCC_HSI_DIV_8  /*!< HSI clock is divided by 8 */
/**
  * @}
  */

/** @defgroup RCC_HSI_Calibration_Default  HSI Calibration default
  * @{
  */
#define RCC_HSICALIBRATION_DEFAULT     0U                  /*!< Default HSI calibration trimming value */
/**
  * @}
  */

/** @defgroup RCC_LSI_Config  LSI Config
  * @{
  */
#define RCC_LSI_OFF                    0U                  /*!< LSI clock deactivation */
#define RCC_LSI_ON                     RCC_CR_LSION        /*!< LSI clock activation */
/**
  * @}
  */

/** @defgroup RCC_MSI_Config  MSI Config
  * @{
  */
#define RCC_MSI_OFF                    0U                  /*!< MSI clock deactivation */
#define RCC_MSI_ON                     RCC_CR_MSION        /*!< MSI clock activation */
/**
  * @}
  */

/** @defgroup RCC_MSI_Frequency  MSI Frequency
  * @{
  */
#define RCC_MSI_FREQ_4MHZ              LL_RCC_MSI_FREQ_4MHZ  /*!< MSI 4MHz selection */
#define RCC_MSI_FREQ_16MHZ             LL_RCC_MSI_FREQ_16MHZ /*!< MSI 16MHz selection */
/**
  * @}
  */

/** @defgroup RCC_MSI_Calibration_Default  MSI Calibration default
  * @{
  */
#define RCC_MSICALIBRATION_DEFAULT     0U                  /*!< Default MSI calibration trimming value */
/**
  * @}
  */

/** @defgroup RCC_PLL_Config  PLL Config
  * @{
  */
#define RCC_PLL_NONE                   0U                  /*!< PLL configuration unchanged */
#define RCC_PLL_OFF                    1U                  /*!< PLL deactivation */
#define RCC_PLL_ON                     2U                  /*!< PLL activation */
#define RCC_PLL_BYPASS                 3U                  /*!< PLL activation in bypass mode with FREF set as source */
/**
  * @}
  */

/** @defgroup RCC_PLL_Clock_Source  PLL Clock Source
  * @{
  */
#define RCC_PLLSOURCE_HSI              LL_RCC_PLLSOURCE_HSI      /*!< HSI clock selected as PLL entry clock source */
#define RCC_PLLSOURCE_MSI              LL_RCC_PLLSOURCE_MSI      /*!< MSI clock selected as PLL entry clock source */
#define RCC_PLLSOURCE_HSE              LL_RCC_PLLSOURCE_HSE      /*!< HSE clock selected as PLL entry clock source */
#define RCC_PLLSOURCE_PIN              LL_RCC_PLLSOURCE_I2S_CKIN /*!< External clock I2S_CKIN selected as PLL entry clock source  */
/**
  * @}
  */

/** @defgroup RCC_Clock_Type  Clock Type
  * @{
  */
#define RCC_CLOCKTYPE_CPUCLK           0x01U         /*!< CPU clock to configure */
#define RCC_CLOCKTYPE_SYSCLK           0x02U         /*!< System bus clock to configure for AXI */
#define RCC_CLOCKTYPE_HCLK             0x04U         /*!< HCLK to configure for AHB */
#define RCC_CLOCKTYPE_PCLK1            0x08U         /*!< PCLK1 to configure for APB1 */
#define RCC_CLOCKTYPE_PCLK2            0x10U         /*!< PCLK2 to configure for APB2 */
#define RCC_CLOCKTYPE_PCLK4            0x20U         /*!< PCLK4 to configure for APB4 */
#define RCC_CLOCKTYPE_PCLK5            0x40U         /*!< PCLK5 to configure for APB5 */
/**
  * @}
  */

/** @defgroup RCC_CPU_Clock_Source  CPU Clock Source
  * @{
  */
#define RCC_CPUCLKSOURCE_HSI           LL_RCC_CPU_CLKSOURCE_HSI  /*!< HSI selection as CPU clock */
#define RCC_CPUCLKSOURCE_MSI           LL_RCC_CPU_CLKSOURCE_MSI  /*!< MSI selection as CPU clock */
#define RCC_CPUCLKSOURCE_HSE           LL_RCC_CPU_CLKSOURCE_HSE  /*!< HSE selection as CPU clock */
#define RCC_CPUCLKSOURCE_IC1           LL_RCC_CPU_CLKSOURCE_IC1  /*!< IC1 selection as CPU clock */
/**
  * @}
  */

/** @defgroup RCC_CPU_Clock_Source_Status CPU Clock Source Status
  * @{
  */
#define RCC_CPUCLKSOURCE_STATUS_HSI    LL_RCC_CPU_CLKSOURCE_STATUS_HSI  /*!< HSI used as CPU clock */
#define RCC_CPUCLKSOURCE_STATUS_MSI    LL_RCC_CPU_CLKSOURCE_STATUS_MSI  /*!< MSI used as CPU clock */
#define RCC_CPUCLKSOURCE_STATUS_HSE    LL_RCC_CPU_CLKSOURCE_STATUS_HSE  /*!< HSE used as CPU clock */
#define RCC_CPUCLKSOURCE_STATUS_IC1    LL_RCC_CPU_CLKSOURCE_STATUS_IC1  /*!< IC1 used as CPU clock */
/**
  * @}
  */

/** @defgroup RCC_System_Bus_Clock_Source  System Bus Clock Source
  * @{
  */
#define RCC_SYSCLKSOURCE_HSI           LL_RCC_SYS_CLKSOURCE_HSI  /*!< HSI selection as system bus clocks */
#define RCC_SYSCLKSOURCE_MSI           LL_RCC_SYS_CLKSOURCE_MSI  /*!< MSI selection as system bus clocks */
#define RCC_SYSCLKSOURCE_HSE           LL_RCC_SYS_CLKSOURCE_HSE  /*!< HSE selection as system bus clocks */
#define RCC_SYSCLKSOURCE_IC2_IC6_IC11  LL_RCC_SYS_CLKSOURCE_IC2_IC6_IC11 /*!< IC2/IC6/IC11 selection as system bus clocks */
/**
  * @}
  */

/** @defgroup RCC_System_Bus_Clock_Source_Status System Bus Clock Source Status
  * @{
  */
#define RCC_SYSCLKSOURCE_STATUS_HSI          LL_RCC_SYS_CLKSOURCE_STATUS_HSI  /*!< HSI used as system bus clocks */
#define RCC_SYSCLKSOURCE_STATUS_MSI          LL_RCC_SYS_CLKSOURCE_STATUS_MSI  /*!< MSI used as system bus clocks */
#define RCC_SYSCLKSOURCE_STATUS_HSE          LL_RCC_SYS_CLKSOURCE_STATUS_HSE  /*!< HSE used as system bus clocks */
#define RCC_SYSCLKSOURCE_STATUS_IC2_IC6_IC11 LL_RCC_SYS_CLKSOURCE_STATUS_IC2_IC6_IC11  /*!< IC2/IC6/IC11 used as system bus clocks */
/**
  * @}
  */

/** @defgroup RCC_HCLK_Clock_Source  HCLK Clock Source
  * @{
  */
#define RCC_HCLK_DIV1        LL_RCC_AHB_DIV_1    /*!< HCLK not divided */
#define RCC_HCLK_DIV2        LL_RCC_AHB_DIV_2    /*!< HCLK divided by 2 */
#define RCC_HCLK_DIV4        LL_RCC_AHB_DIV_4    /*!< HCLK divided by 4 */
#define RCC_HCLK_DIV8        LL_RCC_AHB_DIV_8    /*!< HCLK divided by 8 */
#define RCC_HCLK_DIV16       LL_RCC_AHB_DIV_16   /*!< HCLK divided by 16 */
#define RCC_HCLK_DIV32       LL_RCC_AHB_DIV_32   /*!< HCLK divided by 32 */
#define RCC_HCLK_DIV64       LL_RCC_AHB_DIV_64   /*!< HCLK divided by 64 */
#define RCC_HCLK_DIV128      LL_RCC_AHB_DIV_128  /*!< HCLK divided by 128 */
/**
  * @}
  */

/** @defgroup RCC_APB1_Clock_Source  APB1 Clock Source
  * @{
  */
#define RCC_APB1_DIV1        LL_RCC_APB1_DIV_1    /*!< APB1 not divided */
#define RCC_APB1_DIV2        LL_RCC_APB1_DIV_2    /*!< APB1 divided by 2 */
#define RCC_APB1_DIV4        LL_RCC_APB1_DIV_4    /*!< APB1 divided by 4 */
#define RCC_APB1_DIV8        LL_RCC_APB1_DIV_8    /*!< APB1 divided by 8 */
#define RCC_APB1_DIV16       LL_RCC_APB1_DIV_16   /*!< APB1 divided by 16 */
#define RCC_APB1_DIV32       LL_RCC_APB1_DIV_32   /*!< APB1 divided by 32 */
#define RCC_APB1_DIV64       LL_RCC_APB1_DIV_64   /*!< APB1 divided by 64 */
#define RCC_APB1_DIV128      LL_RCC_APB1_DIV_128  /*!< APB1 divided by 128 */
/**
  * @}
  */

/** @defgroup RCC_APB2_Clock_Source  APB2 Clock Source
  * @{
  */
#define RCC_APB2_DIV1        LL_RCC_APB2_DIV_1    /*!< APB2 not divided */
#define RCC_APB2_DIV2        LL_RCC_APB2_DIV_2    /*!< APB2 divided by 2 */
#define RCC_APB2_DIV4        LL_RCC_APB2_DIV_4    /*!< APB2 divided by 4 */
#define RCC_APB2_DIV8        LL_RCC_APB2_DIV_8    /*!< APB2 divided by 8 */
#define RCC_APB2_DIV16       LL_RCC_APB2_DIV_16   /*!< APB2 divided by 16 */
#define RCC_APB2_DIV32       LL_RCC_APB2_DIV_32   /*!< APB2 divided by 32 */
#define RCC_APB2_DIV64       LL_RCC_APB2_DIV_64   /*!< APB2 divided by 64 */
#define RCC_APB2_DIV128      LL_RCC_APB2_DIV_128  /*!< APB2 divided by 128 */
/**
  * @}
  */

/** @defgroup RCC_APB4_Clock_Source  APB4 Clock Source
  * @{
  */
#define RCC_APB4_DIV1        LL_RCC_APB4_DIV_1    /*!< APB4 not divided */
#define RCC_APB4_DIV2        LL_RCC_APB4_DIV_2    /*!< APB4 divided by 2 */
#define RCC_APB4_DIV4        LL_RCC_APB4_DIV_4    /*!< APB4 divided by 4 */
#define RCC_APB4_DIV8        LL_RCC_APB4_DIV_8    /*!< APB4 divided by 8 */
#define RCC_APB4_DIV16       LL_RCC_APB4_DIV_16   /*!< APB4 divided by 16 */
#define RCC_APB4_DIV32       LL_RCC_APB4_DIV_32   /*!< APB4 divided by 32 */
#define RCC_APB4_DIV64       LL_RCC_APB4_DIV_64   /*!< APB4 divided by 64 */
#define RCC_APB4_DIV128      LL_RCC_APB4_DIV_128  /*!< APB4 divided by 128 */
/**
  * @}
  */

/** @defgroup RCC_APB5_Clock_Source  APB5 Clock Source
  * @{
  */
#define RCC_APB5_DIV1        LL_RCC_APB5_DIV_1    /*!< APB5 not divided */
#define RCC_APB5_DIV2        LL_RCC_APB5_DIV_2    /*!< APB5 divided by 2 */
#define RCC_APB5_DIV4        LL_RCC_APB5_DIV_4    /*!< APB5 divided by 4 */
#define RCC_APB5_DIV8        LL_RCC_APB5_DIV_8    /*!< APB5 divided by 8 */
#define RCC_APB5_DIV16       LL_RCC_APB5_DIV_16   /*!< APB5 divided by 16 */
#define RCC_APB5_DIV32       LL_RCC_APB5_DIV_32   /*!< APB5 divided by 32 */
#define RCC_APB5_DIV64       LL_RCC_APB5_DIV_64   /*!< APB5 divided by 64 */
#define RCC_APB5_DIV128      LL_RCC_APB5_DIV_128  /*!< APB5 divided by 128 */
/**
  * @}
  */

/** @defgroup RCC_IC_Clock_Source  IC Clock Source
  * @{
  */
#define RCC_ICCLKSOURCE_PLL1           LL_RCC_ICCLKSOURCE_PLL1 /*!< ICx clock source selection is PLL1 output */
#define RCC_ICCLKSOURCE_PLL2           LL_RCC_ICCLKSOURCE_PLL2 /*!< ICx clock source selection is PLL2 output */
#define RCC_ICCLKSOURCE_PLL3           LL_RCC_ICCLKSOURCE_PLL3 /*!< ICx clock source selection is PLL3 output */
#define RCC_ICCLKSOURCE_PLL4           LL_RCC_ICCLKSOURCE_PLL4 /*!< ICx clock source selection is PLL4 output */
/**
  * @}
  */

/** @defgroup RCC_RTC_Clock_Source  RTC Clock Source
  * @{
  */
#define RCC_RTCCLKSOURCE_DISABLE       0U                                                      /*!< No clock used as RTC clock */
#define RCC_RTCCLKSOURCE_LSE           RCC_CCIPR7_RTCSEL_0                                     /*!< LSE oscillator clock used as RTC clock */
#define RCC_RTCCLKSOURCE_LSI           RCC_CCIPR7_RTCSEL_1                                     /*!< LSI oscillator clock used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV1      ((0x00U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 1 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV2      ((0x01U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 2 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV3      ((0x02U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 3 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV4      ((0x03U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 4 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV5      ((0x04U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 5 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV6      ((0x05U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 6 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV7      ((0x06U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 7 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV8      ((0x07U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 8 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV9      ((0x08U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 9 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV10     ((0x09U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 10 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV11     ((0x0AU << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 11 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV12     ((0x0BU << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 12 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV13     ((0x0CU << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 13 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV14     ((0x0DU << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 14 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV15     ((0x0EU << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 15 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV16     ((0x0FU << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 16 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV17     ((0x10U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 17 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV18     ((0x11U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 18 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV19     ((0x12U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 19 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV20     ((0x13U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 20 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV21     ((0x14U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 21 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV22     ((0x15U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 22 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV23     ((0x16U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 23 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV24     ((0x17U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 24 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV25     ((0x18U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 25 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV26     ((0x19U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 26 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV27     ((0x1AU << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 27 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV28     ((0x1BU << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 28 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV29     ((0x1CU << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 29 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV30     ((0x1DU << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 30 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV31     ((0x1EU << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 31 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV32     ((0x1FU << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 32 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV33     ((0x20U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 33 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV34     ((0x21U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 34 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV35     ((0x22U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 35 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV36     ((0x23U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 36 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV37     ((0x24U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 37 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV38     ((0x25U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 38 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV39     ((0x26U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 39 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV40     ((0x27U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 40 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV41     ((0x28U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 41 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV42     ((0x29U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 42 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV43     ((0x2AU << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 43 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV44     ((0x2BU << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 44 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV45     ((0x2CU << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 45 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV46     ((0x2DU << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 46 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV47     ((0x2EU << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 47 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV48     ((0x2FU << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 48 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV49     ((0x30U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 49 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV50     ((0x31U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 50 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV51     ((0x32U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 51 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV52     ((0x33U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 52 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV53     ((0x34U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 53 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV54     ((0x35U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 54 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV55     ((0x36U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 55 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV56     ((0x37U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 56 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV57     ((0x38U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 57 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV58     ((0x39U << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 58 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV59     ((0x3AU << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 59 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV60     ((0x3BU << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 60 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV61     ((0x3CU << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 61 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV62     ((0x3DU << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 62 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV63     ((0x3EU << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 63 used as RTC clock */
#define RCC_RTCCLKSOURCE_HSE_DIV64     ((0x3FU << RCC_CCIPR7_RTCPRE_Pos) | RCC_CCIPR7_RTCSEL)  /*!< HSE oscillator clock divided by 64 used as RTC clock */
/**
  * @}
  */

/** @defgroup RCC_MCOx_Index  MCOx Index
  * @{
  */
#define RCC_MCO1                       0U    /*!< Microcontroller Clock Output 1 */
#define RCC_MCO2                       1U    /*!< Microcontroller Clock Output 2 */
/**
  * @}
  */

/** @defgroup RCC_MCO1_Clock_Source  MCO1 Clock Source
  * @{
  */
#define RCC_MCO1SOURCE_HSI             LL_RCC_MCO1SOURCE_HSI     /*!< HSI clock selected as MCO1 source (reset) */
#define RCC_MCO1SOURCE_LSE             LL_RCC_MCO1SOURCE_LSE     /*!< LSE clock selected as MCO1 source */
#define RCC_MCO1SOURCE_MSI             LL_RCC_MCO1SOURCE_MSI     /*!< MSI clock selected as MCO1 source */
#define RCC_MCO1SOURCE_LSI             LL_RCC_MCO1SOURCE_LSI     /*!< HSI clock selected as MCO1 source */
#define RCC_MCO1SOURCE_HSE             LL_RCC_MCO1SOURCE_HSE     /*!< HSE clock selected as MCO1 source */
#define RCC_MCO1SOURCE_IC5             LL_RCC_MCO1SOURCE_IC5     /*!< IC5 clock selected as MCO1 source */
#define RCC_MCO1SOURCE_IC10            LL_RCC_MCO1SOURCE_IC10    /*!< IC10 clock selected as MCO1 source */
#define RCC_MCO1SOURCE_SYSA            LL_RCC_MCO1SOURCE_SYSA    /*!< SYSA CPU clock selected as MCO1 source */
/**
  * @}
  */

/** @defgroup RCC_MCO2_Clock_Source  MCO2 Clock Source
  * @{
  */
#define RCC_MCO2SOURCE_HSI             LL_RCC_MCO2SOURCE_HSI     /*!< HSI clock selected as MCO2 source (reset) */
#define RCC_MCO2SOURCE_LSE             LL_RCC_MCO2SOURCE_LSE     /*!< LSE clock selected as MCO2 source */
#define RCC_MCO2SOURCE_MSI             LL_RCC_MCO2SOURCE_MSI     /*!< MSI clock selected as MCO2 source */
#define RCC_MCO2SOURCE_LSI             LL_RCC_MCO2SOURCE_LSI     /*!< LSI clock selected as MCO2 source */
#define RCC_MCO2SOURCE_HSE             LL_RCC_MCO2SOURCE_HSE     /*!< HSE clock selected as MCO2 source */
#define RCC_MCO2SOURCE_IC15            LL_RCC_MCO2SOURCE_IC15    /*!< IC15 clock selected as MCO2 source */
#define RCC_MCO2SOURCE_IC20            LL_RCC_MCO2SOURCE_IC20    /*!< IC20 clock selected as MCO2 source */
#define RCC_MCO2SOURCE_SYSB            LL_RCC_MCO2SOURCE_SYSB    /*!< SYSB bus clock selected as MCO2 source */
/**
  * @}
  */

/** @defgroup RCC_MCOx_Clock_Prescaler  MCOx Clock Prescaler
  * @{
  */
#define RCC_MCODIV_1                   LL_RCC_MCO1_DIV_1   /*!< MCO divided by 1 */
#define RCC_MCODIV_2                   LL_RCC_MCO1_DIV_2   /*!< MCO divided by 2 */
#define RCC_MCODIV_3                   LL_RCC_MCO1_DIV_3   /*!< MCO divided by 3 */
#define RCC_MCODIV_4                   LL_RCC_MCO1_DIV_4   /*!< MCO divided by 4 */
#define RCC_MCODIV_5                   LL_RCC_MCO1_DIV_5   /*!< MCO divided by 5 */
#define RCC_MCODIV_6                   LL_RCC_MCO1_DIV_6   /*!< MCO divided by 6 */
#define RCC_MCODIV_7                   LL_RCC_MCO1_DIV_7   /*!< MCO divided by 7 */
#define RCC_MCODIV_8                   LL_RCC_MCO1_DIV_8   /*!< MCO divided by 8 */
#define RCC_MCODIV_9                   LL_RCC_MCO1_DIV_9   /*!< MCO divided by 9 */
#define RCC_MCODIV_10                  LL_RCC_MCO1_DIV_10  /*!< MCO divided by 10 */
#define RCC_MCODIV_11                  LL_RCC_MCO1_DIV_11  /*!< MCO divided by 11 */
#define RCC_MCODIV_12                  LL_RCC_MCO1_DIV_12  /*!< MCO divided by 12 */
#define RCC_MCODIV_13                  LL_RCC_MCO1_DIV_13  /*!< MCO divided by 13 */
#define RCC_MCODIV_14                  LL_RCC_MCO1_DIV_14  /*!< MCO divided by 14 */
#define RCC_MCODIV_15                  LL_RCC_MCO1_DIV_15  /*!< MCO divided by 15 */
#define RCC_MCODIV_16                  LL_RCC_MCO1_DIV_16  /*!< MCO divided by 16 (reset) */
/**
  * @}
  */

/** @defgroup RCC_Interrupt  Interrupt
  * @{
  */
#define RCC_IT_LSIRDY                  RCC_CIER_LSIRDYIE   /*!< LSI Ready Interrupt */
#define RCC_IT_LSERDY                  RCC_CIER_LSERDYIE   /*!< LSE Ready Interrupt */
#define RCC_IT_MSIRDY                  RCC_CIER_MSIRDYIE   /*!< MSI Ready Interrupt */
#define RCC_IT_HSIRDY                  RCC_CIER_HSIRDYIE   /*!< HSI Ready Interrupt */
#define RCC_IT_HSERDY                  RCC_CIER_HSERDYIE   /*!< HSE Ready Interrupt */
#define RCC_IT_PLL1RDY                 RCC_CIER_PLL1RDYIE  /*!< PLL1 Ready Interrupt */
#define RCC_IT_PLL2RDY                 RCC_CIER_PLL2RDYIE  /*!< PLL2 Ready Interrupt */
#define RCC_IT_PLL3RDY                 RCC_CIER_PLL3RDYIE  /*!< PLL3 Ready Interrupt */
#define RCC_IT_PLL4RDY                 RCC_CIER_PLL4RDYIE  /*!< PLL4 Ready Interrupt */
#define RCC_IT_LSECSS                  RCC_CIER_LSECSSIE   /*!< LSE Clock Security System Interrupt */
#define RCC_IT_HSECSS                  RCC_CIER_HSECSSIE   /*!< HSE Clock Security System Interrupt */
#define RCC_IT_WKUP                    RCC_CIER_WKUPIE     /*!< CPU Wakeup Interrupt */
/**
  * @}
  */

/** @defgroup RCC_Flag  Flag
  *        Elements values convention: XXXYYYYYb
  *           - YYYYY  : Flag position in the register
  *           - XXX  : Register index
  *                 - 001: SR register
  *                 - 010: LSECFGR register
  *                 - 011: HSECFGR register
  *                 - 100: RSR register
  * @{
  */
/* Flags in the SR register */
#define RCC_FLAG_LSIRDY                ((RCC_SR_REG_INDEX << 5U) | RCC_SR_LSIRDY_Pos)   /*!< LSI ready flag */
#define RCC_FLAG_LSERDY                ((RCC_SR_REG_INDEX << 5U) | RCC_SR_LSERDY_Pos)   /*!< LSE ready flag */
#define RCC_FLAG_HSIRDY                ((RCC_SR_REG_INDEX << 5U) | RCC_SR_HSIRDY_Pos)   /*!< HSI ready flag */
#define RCC_FLAG_MSIRDY                ((RCC_SR_REG_INDEX << 5U) | RCC_SR_MSIRDY_Pos)   /*!< MSI ready flag */
#define RCC_FLAG_HSERDY                ((RCC_SR_REG_INDEX << 5U) | RCC_SR_HSERDY_Pos)   /*!< HSE ready flag */
#define RCC_FLAG_PLL1RDY               ((RCC_SR_REG_INDEX << 5U) | RCC_SR_PLL1RDY_Pos)  /*!< PLL1 ready flag */
#define RCC_FLAG_PLL2RDY               ((RCC_SR_REG_INDEX << 5U) | RCC_SR_PLL2RDY_Pos)  /*!< PLL2 ready flag */
#define RCC_FLAG_PLL3RDY               ((RCC_SR_REG_INDEX << 5U) | RCC_SR_PLL3RDY_Pos)  /*!< PLL3 ready flag */
#define RCC_FLAG_PLL4RDY               ((RCC_SR_REG_INDEX << 5U) | RCC_SR_PLL4RDY_Pos)  /*!< PLL4 ready flag */

/* Flags in the LSECFGR register */
#define RCC_FLAG_LSECSSD               ((RCC_LSECFGR_REG_INDEX << 5U) | RCC_LSECFGR_LSECSSD_Pos) /*!< LSE Clock Security System failure detection flag */

/* Flags in the HSECFGR register */
#define RCC_FLAG_HSECSSD               ((RCC_HSECFGR_REG_INDEX << 5U) | RCC_HSECFGR_HSECSSD_Pos) /*!< HSE Clock Security System failure detection flag */

/* Flags in the RSR register */
#define RCC_FLAG_LCKRST                ((RCC_RSR_REG_INDEX << 5U) | RCC_RSR_LCKRSTF_Pos)  /*!< CPU Lockup reset flag */
#define RCC_FLAG_BORRST                ((RCC_RSR_REG_INDEX << 5U) | RCC_RSR_BORRSTF_Pos)  /*!< BOR reset flag */
#define RCC_FLAG_PINRST                ((RCC_RSR_REG_INDEX << 5U) | RCC_RSR_PINRSTF_Pos)  /*!< PIN reset flag */
#define RCC_FLAG_PORRST                ((RCC_RSR_REG_INDEX << 5U) | RCC_RSR_PORRSTF_Pos)  /*!< Power-on reset flag */
#define RCC_FLAG_SFTRST                ((RCC_RSR_REG_INDEX << 5U) | RCC_RSR_SFTRSTF_Pos)  /*!< Software Reset flag */
#define RCC_FLAG_IWDGRST               ((RCC_RSR_REG_INDEX << 5U) | RCC_RSR_IWDGRSTF_Pos) /*!< Independent Watchdog reset flag */
#define RCC_FLAG_WWDGRST               ((RCC_RSR_REG_INDEX << 5U) | RCC_RSR_WWDGRSTF_Pos) /*!< Window watchdog reset flag */
#define RCC_FLAG_LPWRRST               ((RCC_RSR_REG_INDEX << 5U) | RCC_RSR_LPWRRSTF_Pos) /*!< Low-Power reset flag */

/**
  * @}
  */

/** @defgroup RCC_LSEDrive_Config LSE Drive Config
  * @{
  */
#define RCC_LSEDRIVE_LOW               LL_RCC_LSEDRIVE_LOW         /*!< LSE low drive capability */
#define RCC_LSEDRIVE_MEDIUMLOW         LL_RCC_LSEDRIVE_MEDIUMLOW   /*!< LSE medium low drive capability */
#define RCC_LSEDRIVE_MEDIUMHIGH        LL_RCC_LSEDRIVE_MEDIUMHIGH  /*!< LSE medium high drive capability */
#define RCC_LSEDRIVE_HIGH              LL_RCC_LSEDRIVE_HIGH        /*!< LSE high drive capability */
/**
  * @}
  */

/** @defgroup RCC_Stop_WakeUpClock  Stop WakeUp Clock
  * @{
  */
#define RCC_STOP_WAKEUPCLOCK_HSI       LL_RCC_SYSWAKEUP_CLKSOURCE_HSI  /*!< HSI selected as wake up system clock from system Stop (default after reset) */
#define RCC_STOP_WAKEUPCLOCK_MSI       LL_RCC_SYSWAKEUP_CLKSOURCE_MSI  /*!< MSI selected as wake up system clock from system Stop */
/**
  * @}
  */


/** @defgroup RCC_items RCC items
  * @brief RCC items to configure attributes on
  * @{
  */
#define RCC_ITEM_LSI           (RCC_ITEM_GROUP_OSC | RCC_SECCFGR0_LSISEC)
#define RCC_ITEM_LSE           (RCC_ITEM_GROUP_OSC | RCC_SECCFGR0_LSESEC)
#define RCC_ITEM_MSI           (RCC_ITEM_GROUP_OSC | RCC_SECCFGR0_MSISEC)
#define RCC_ITEM_HSI           (RCC_ITEM_GROUP_OSC | RCC_SECCFGR0_HSISEC)
#define RCC_ITEM_HSE           (RCC_ITEM_GROUP_OSC | RCC_SECCFGR0_HSESEC)
#define RCC_ITEM_PLL1          (RCC_ITEM_GROUP_PLL | RCC_SECCFGR1_PLL1SEC)
#define RCC_ITEM_PLL2          (RCC_ITEM_GROUP_PLL | RCC_SECCFGR1_PLL2SEC)
#define RCC_ITEM_PLL3          (RCC_ITEM_GROUP_PLL | RCC_SECCFGR1_PLL3SEC)
#define RCC_ITEM_PLL4          (RCC_ITEM_GROUP_PLL | RCC_SECCFGR1_PLL4SEC)
#define RCC_ITEM_IC1           (RCC_ITEM_GROUP_IC | RCC_SECCFGR2_IC1SEC)
#define RCC_ITEM_IC2           (RCC_ITEM_GROUP_IC | RCC_SECCFGR2_IC2SEC)
#define RCC_ITEM_IC3           (RCC_ITEM_GROUP_IC | RCC_SECCFGR2_IC3SEC)
#define RCC_ITEM_IC4           (RCC_ITEM_GROUP_IC | RCC_SECCFGR2_IC4SEC)
#define RCC_ITEM_IC5           (RCC_ITEM_GROUP_IC | RCC_SECCFGR2_IC5SEC)
#define RCC_ITEM_IC6           (RCC_ITEM_GROUP_IC | RCC_SECCFGR2_IC6SEC)
#define RCC_ITEM_IC7           (RCC_ITEM_GROUP_IC | RCC_SECCFGR2_IC7SEC)
#define RCC_ITEM_IC8           (RCC_ITEM_GROUP_IC | RCC_SECCFGR2_IC8SEC)
#define RCC_ITEM_IC9           (RCC_ITEM_GROUP_IC | RCC_SECCFGR2_IC9SEC)
#define RCC_ITEM_IC10          (RCC_ITEM_GROUP_IC | RCC_SECCFGR2_IC10SEC)
#define RCC_ITEM_IC11          (RCC_ITEM_GROUP_IC | RCC_SECCFGR2_IC11SEC)
#define RCC_ITEM_IC12          (RCC_ITEM_GROUP_IC | RCC_SECCFGR2_IC12SEC)
#define RCC_ITEM_IC13          (RCC_ITEM_GROUP_IC | RCC_SECCFGR2_IC13SEC)
#define RCC_ITEM_IC14          (RCC_ITEM_GROUP_IC | RCC_SECCFGR2_IC14SEC)
#define RCC_ITEM_IC15          (RCC_ITEM_GROUP_IC | RCC_SECCFGR2_IC15SEC)
#define RCC_ITEM_IC16          (RCC_ITEM_GROUP_IC | RCC_SECCFGR2_IC16SEC)
#define RCC_ITEM_IC17          (RCC_ITEM_GROUP_IC | RCC_SECCFGR2_IC17SEC)
#define RCC_ITEM_IC18          (RCC_ITEM_GROUP_IC | RCC_SECCFGR2_IC18SEC)
#define RCC_ITEM_IC19          (RCC_ITEM_GROUP_IC | RCC_SECCFGR2_IC19SEC)
#define RCC_ITEM_IC20          (RCC_ITEM_GROUP_IC | RCC_SECCFGR2_IC20SEC)
#define RCC_ITEM_MOD           (RCC_ITEM_GROUP_SYSCFG | RCC_SECCFGR3_MODSEC)
#define RCC_ITEM_SYS           (RCC_ITEM_GROUP_SYSCFG | RCC_SECCFGR3_SYSSEC)
#define RCC_ITEM_BUS           (RCC_ITEM_GROUP_SYSCFG | RCC_SECCFGR3_BUSSEC)
#define RCC_ITEM_PER           (RCC_ITEM_GROUP_SYSCFG | RCC_SECCFGR3_PERSEC)
#define RCC_ITEM_INT           (RCC_ITEM_GROUP_SYSCFG | RCC_SECCFGR3_INTSEC)
#define RCC_ITEM_RST           (RCC_ITEM_GROUP_SYSCFG | RCC_SECCFGR3_RSTSEC)
#define RCC_ITEM_ACLKN         (RCC_ITEM_GROUP_BUS | RCC_SECCFGR4_ACLKNSEC)
#define RCC_ITEM_ACLKNC        (RCC_ITEM_GROUP_BUS | RCC_SECCFGR4_ACLKNCSEC)
#define RCC_ITEM_AHBM          (RCC_ITEM_GROUP_BUS | RCC_SECCFGR4_AHBMSEC)
#define RCC_ITEM_AHB1          (RCC_ITEM_GROUP_BUS | RCC_SECCFGR4_AHB1SEC)
#define RCC_ITEM_AHB2          (RCC_ITEM_GROUP_BUS | RCC_SECCFGR4_AHB2SEC)
#define RCC_ITEM_AHB3          (RCC_ITEM_GROUP_BUS | RCC_SECCFGR4_AHB3SEC)
#define RCC_ITEM_AHB4          (RCC_ITEM_GROUP_BUS | RCC_SECCFGR4_AHB4SEC)
#define RCC_ITEM_AHB5          (RCC_ITEM_GROUP_BUS | RCC_SECCFGR4_AHB5SEC)
#define RCC_ITEM_APB1          (RCC_ITEM_GROUP_BUS | RCC_SECCFGR4_APB1SEC)
#define RCC_ITEM_APB2          (RCC_ITEM_GROUP_BUS | RCC_SECCFGR4_APB2SEC)
#define RCC_ITEM_APB3          (RCC_ITEM_GROUP_BUS | RCC_SECCFGR4_APB3SEC)
#define RCC_ITEM_APB4          (RCC_ITEM_GROUP_BUS | RCC_SECCFGR4_APB4SEC)
#define RCC_ITEM_APB5          (RCC_ITEM_GROUP_BUS | RCC_SECCFGR4_APB5SEC)
#define RCC_ITEM_NOC           (RCC_ITEM_GROUP_BUS | RCC_SECCFGR4_NOCSEC)
#define RCC_ITEM_AXISRAM3      (RCC_ITEM_GROUP_MEM | RCC_PUBCFGR5_AXISRAM3PUB)
#define RCC_ITEM_AXISRAM4      (RCC_ITEM_GROUP_MEM | RCC_PUBCFGR5_AXISRAM4PUB)
#define RCC_ITEM_AXISRAM5      (RCC_ITEM_GROUP_MEM | RCC_PUBCFGR5_AXISRAM5PUB)
#define RCC_ITEM_AXISRAM6      (RCC_ITEM_GROUP_MEM | RCC_PUBCFGR5_AXISRAM6PUB)
#define RCC_ITEM_AHBSRAM1      (RCC_ITEM_GROUP_MEM | RCC_PUBCFGR5_AHBSRAM1PUB)
#define RCC_ITEM_AHBSRAM2      (RCC_ITEM_GROUP_MEM | RCC_PUBCFGR5_AHBSRAM2PUB)
#define RCC_ITEM_BKPSRAM       (RCC_ITEM_GROUP_MEM | RCC_PUBCFGR5_BKPSRAMPUB)
#define RCC_ITEM_AXISRAM1      (RCC_ITEM_GROUP_MEM | RCC_PUBCFGR5_AXISRAM1PUB)
#define RCC_ITEM_AXISRAM2      (RCC_ITEM_GROUP_MEM | RCC_PUBCFGR5_AXISRAM2PUB)
#define RCC_ITEM_FLEXRAM       (RCC_ITEM_GROUP_MEM | RCC_PUBCFGR5_FLEXRAMPUB)
#if defined(CACHEAXI)
#define RCC_ITEM_CACHEAXIRAM   (RCC_ITEM_GROUP_MEM | RCC_PUBCFGR5_CACHEAXIRAMPUB)
#else
#define RCC_ITEM_CACHEAXIRAM   (RCC_ITEM_GROUP_MEM)
#endif /* defined(CACHEAXI) */
#define RCC_ITEM_VENCRAM       (RCC_ITEM_GROUP_MEM | RCC_PUBCFGR5_VENCRAMPUB)

#define RCC_ITEM_ALL           (RCC_ITEM_LSI | RCC_ITEM_LSE | RCC_ITEM_MSI | RCC_ITEM_HSI | RCC_ITEM_HSE | \
                                RCC_ITEM_PLL1 | RCC_ITEM_PLL2 | RCC_ITEM_PLL3 | RCC_ITEM_PLL4 | RCC_ITEM_IC1 | \
                                RCC_ITEM_IC2 | RCC_ITEM_IC3 | RCC_ITEM_IC4 | RCC_ITEM_IC5 | RCC_ITEM_IC6 | \
                                RCC_ITEM_IC7 | RCC_ITEM_IC8 | RCC_ITEM_IC9 | RCC_ITEM_IC10 | RCC_ITEM_IC11 | \
                                RCC_ITEM_IC12 | RCC_ITEM_IC13 | RCC_ITEM_IC14 | RCC_ITEM_IC15 | RCC_ITEM_IC16 | \
                                RCC_ITEM_IC17 | RCC_ITEM_IC18 | RCC_ITEM_IC19 | RCC_ITEM_IC20 | RCC_ITEM_MOD | \
                                RCC_ITEM_SYS | RCC_ITEM_BUS | RCC_ITEM_PER | RCC_ITEM_INT | RCC_ITEM_RST | \
                                RCC_ITEM_ACLKN | RCC_ITEM_ACLKNC | RCC_ITEM_AHBM | RCC_ITEM_AHB1 | RCC_ITEM_AHB2 | \
                                RCC_ITEM_AHB3 | RCC_ITEM_AHB4 | RCC_ITEM_AHB5 | RCC_ITEM_APB1 | RCC_ITEM_APB2 | \
                                RCC_ITEM_APB3 | RCC_ITEM_APB4 | RCC_ITEM_APB5 | RCC_ITEM_NOC | RCC_ITEM_AXISRAM3 | \
                                RCC_ITEM_AXISRAM4 | RCC_ITEM_AXISRAM5 | RCC_ITEM_AXISRAM6 | RCC_ITEM_AHBSRAM1 | \
                                RCC_ITEM_AHBSRAM2 | RCC_ITEM_BKPSRAM | RCC_ITEM_AXISRAM1 | RCC_ITEM_AXISRAM2 | \
                                RCC_ITEM_FLEXRAM | RCC_ITEM_CACHEAXIRAM | RCC_ITEM_VENCRAM)

#define RCC_ITEM_ALL_CLK       (RCC_ITEM_LSI | RCC_ITEM_LSE | RCC_ITEM_MSI | RCC_ITEM_HSI | RCC_ITEM_HSE)
#define RCC_ITEM_ALL_PLL       (RCC_ITEM_PLL1 | RCC_ITEM_PLL2 | RCC_ITEM_PLL3 | RCC_ITEM_PLL4)
#define RCC_ITEM_ALL_ICx       (RCC_ITEM_IC1 | RCC_ITEM_IC2 | RCC_ITEM_IC3 | RCC_ITEM_IC4 | RCC_ITEM_IC5 | \
                                RCC_ITEM_IC6 | RCC_ITEM_IC7 | RCC_ITEM_IC8 | RCC_ITEM_IC9 | RCC_ITEM_IC10 | \
                                RCC_ITEM_IC11 | RCC_ITEM_IC12 | RCC_ITEM_IC13 | RCC_ITEM_IC14 | RCC_ITEM_IC15 | \
                                RCC_ITEM_IC16 | RCC_ITEM_IC17 | RCC_ITEM_IC18 | RCC_ITEM_IC19 | RCC_ITEM_IC20)
#define RCC_ITEM_ALL_SYSCFG    (RCC_ITEM_MOD | RCC_ITEM_SYS | RCC_ITEM_BUS | RCC_ITEM_PER | RCC_ITEM_INT | \
                                RCC_ITEM_RST)
#define RCC_ITEM_ALL_BUS       (RCC_ITEM_ACLKN | RCC_ITEM_ACLKNC | RCC_ITEM_AHBM | RCC_ITEM_AHB1 | RCC_ITEM_AHB2 | \
                                RCC_ITEM_AHB3 | RCC_ITEM_AHB4 | RCC_ITEM_AHB5 | RCC_ITEM_APB1 | RCC_ITEM_APB2 | \
                                RCC_ITEM_APB3 | RCC_ITEM_APB4 | RCC_ITEM_APB5 | RCC_ITEM_NOC)
#define RCC_ITEM_ALL_MEM       (RCC_ITEM_AXISRAM3 | RCC_ITEM_AXISRAM4 | RCC_ITEM_AXISRAM5 | RCC_ITEM_AXISRAM6 | \
                                RCC_ITEM_AHBSRAM1 | RCC_ITEM_AHBSRAM2 | RCC_ITEM_BKPSRAM | RCC_ITEM_AXISRAM1 | \
                                RCC_ITEM_AXISRAM2 | RCC_ITEM_FLEXRAM | RCC_ITEM_CACHEAXIRAM | RCC_ITEM_VENCRAM)
/**
  * @}
  */

/** @defgroup RCC_attributes RCC attributes
  * @brief RCC privilege/non-privilege, secure/non-secure, public/non-public and lock attributes
  * @note Configuration registers of a SECURED item are only accessible from secure state.
  * @note Configuration registers of a PRIVILEGED item are only writable from privileged state.
  * @note Configuration registers of a PUBLIC and SECURED item are visible from NS (and S) software.
  * @note Setting an item to LOCKED, definitively locks the SEC and PRIV attribute setting for this item.
  * @{
  */
#define RCC_ATTR_PRIV          (RCC_ATTR_PRIV_MASK | 0x01U)    /*!< attribute is privileged    */
#define RCC_ATTR_NPRIV         RCC_ATTR_PRIV_MASK              /*!< attribute is unprivileged  */

#define RCC_ATTR_SEC           (RCC_ATTR_SEC_MASK | 0x04U)     /*!< attribute is secure        */
#define RCC_ATTR_NSEC          RCC_ATTR_SEC_MASK               /*!< attribute is non-secure    */

#define RCC_ATTR_PUB           (RCC_ATTR_PUB_MASK | 0x10U)     /*!< attribute is public        */
#define RCC_ATTR_NPUB          RCC_ATTR_PUB_MASK               /*!< attribute is not public    */

#define RCC_ATTR_LOCK          (RCC_ATTR_LOCK_MASK | 0x40U)    /*!< attribute is locked        */
#define RCC_ATTR_NLOCK         RCC_ATTR_LOCK_MASK              /*!< attribute is not locked    */
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

/** @defgroup RCC_Embedded_Mem_Clock_Enable_Disable Embedded Memory Clock Enable Disable
  * @brief  Enable or disable the Embedded Memory clock.
  * @note   After reset, some embedded memory clocks are disabled
  *         and the application software has to enable these memory clocks before using them.
  * @{
  */

#define __HAL_RCC_AXISRAM1_MEM_CLK_ENABLE()     LL_MEM_EnableClock(LL_MEM_AXISRAM1)
#define __HAL_RCC_AXISRAM1_MEM_CLK_DISABLE()    LL_MEM_DisableClock(LL_MEM_AXISRAM1)

#define __HAL_RCC_AXISRAM2_MEM_CLK_ENABLE()     LL_MEM_EnableClock(LL_MEM_AXISRAM2)
#define __HAL_RCC_AXISRAM2_MEM_CLK_DISABLE()    LL_MEM_DisableClock(LL_MEM_AXISRAM2)

#define __HAL_RCC_AXISRAM3_MEM_CLK_ENABLE()     LL_MEM_EnableClock(LL_MEM_AXISRAM3)
#define __HAL_RCC_AXISRAM3_MEM_CLK_DISABLE()    LL_MEM_DisableClock(LL_MEM_AXISRAM3)

#define __HAL_RCC_AXISRAM4_MEM_CLK_ENABLE()     LL_MEM_EnableClock(LL_MEM_AXISRAM4)
#define __HAL_RCC_AXISRAM4_MEM_CLK_DISABLE()    LL_MEM_DisableClock(LL_MEM_AXISRAM4)

#define __HAL_RCC_AXISRAM5_MEM_CLK_ENABLE()     LL_MEM_EnableClock(LL_MEM_AXISRAM5)
#define __HAL_RCC_AXISRAM5_MEM_CLK_DISABLE()    LL_MEM_DisableClock(LL_MEM_AXISRAM5)

#define __HAL_RCC_AXISRAM6_MEM_CLK_ENABLE()     LL_MEM_EnableClock(LL_MEM_AXISRAM6)
#define __HAL_RCC_AXISRAM6_MEM_CLK_DISABLE()    LL_MEM_DisableClock(LL_MEM_AXISRAM6)

#define __HAL_RCC_AHBSRAM1_MEM_CLK_ENABLE()     LL_MEM_EnableClock(LL_MEM_AHBSRAM1)
#define __HAL_RCC_AHBSRAM1_MEM_CLK_DISABLE()    LL_MEM_DisableClock(LL_MEM_AHBSRAM1)

#define __HAL_RCC_AHBSRAM2_MEM_CLK_ENABLE()     LL_MEM_EnableClock(LL_MEM_AHBSRAM2)
#define __HAL_RCC_AHBSRAM2_MEM_CLK_DISABLE()    LL_MEM_DisableClock(LL_MEM_AHBSRAM2)

#define __HAL_RCC_BKPSRAM_MEM_CLK_ENABLE()      LL_MEM_EnableClock(LL_MEM_BKPSRAM)
#define __HAL_RCC_BKPSRAM_MEM_CLK_DISABLE()     LL_MEM_DisableClock(LL_MEM_BKPSRAM)

#define __HAL_RCC_FLEXRAM_MEM_CLK_ENABLE()      LL_MEM_EnableClock(LL_MEM_FLEXRAM)
#define __HAL_RCC_FLEXRAM_MEM_CLK_DISABLE()     LL_MEM_DisableClock(LL_MEM_FLEXRAM)

#define __HAL_RCC_CACHEAXIRAM_MEM_CLK_ENABLE()  LL_MEM_EnableClock(LL_MEM_CACHEAXIRAM)
#define __HAL_RCC_CACHEAXIRAM_MEM_CLK_DISABLE() LL_MEM_DisableClock(LL_MEM_CACHEAXIRAM)

#define __HAL_RCC_VENCRAM_MEM_CLK_ENABLE()      LL_MEM_EnableClock(LL_MEM_VENCRAM)
#define __HAL_RCC_VENCRAM_MEM_CLK_DISABLE()     LL_MEM_DisableClock(LL_MEM_VENCRAM)

/**
  * @}
  */

/** @defgroup RCC_AHB1_Peripheral_Clock_Enable_Disable AHB1 Peripheral Clock Enable Disable
  * @brief  Enable or disable the AHB1 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_ADC12_CLK_ENABLE()   LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_ADC12)
#define __HAL_RCC_ADC12_CLK_DISABLE()  LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_ADC12)

#define __HAL_RCC_GPDMA1_CLK_ENABLE()  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPDMA1)
#define __HAL_RCC_GPDMA1_CLK_DISABLE() LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_GPDMA1)

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

#define __HAL_RCC_ADF1_CLK_ENABLE()    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADF1)
#define __HAL_RCC_ADF1_CLK_DISABLE()   LL_AHB2_GRP1_DisableClock(LL_AHB2_GRP1_PERIPH_ADF1)

#define __HAL_RCC_MDF1_CLK_ENABLE()    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_MDF1)
#define __HAL_RCC_MDF1_CLK_DISABLE()   LL_AHB2_GRP1_DisableClock(LL_AHB2_GRP1_PERIPH_MDF1)

#define __HAL_RCC_RAMCFG_CLK_ENABLE()  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_RAMCFG)
#define __HAL_RCC_RAMCFG_CLK_DISABLE() LL_AHB2_GRP1_DisableClock(LL_AHB2_GRP1_PERIPH_RAMCFG)

/**
  * @}
  */

/** @defgroup RCC_AHB3_Peripheral_Clock_Enable_Disable AHB3 Peripheral Clock Enable Disable
  * @brief  Enable or disable the AHB3 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @note   IAC, RIFSC and RISAF peripheral clocks are always security-protected and thus hidden
  *         to the non-secure application.
  * @{
  */

#if defined(CRYP)
#define __HAL_RCC_CRYP_CLK_ENABLE()    LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_CRYP)
#define __HAL_RCC_CRYP_CLK_DISABLE()   LL_AHB3_GRP1_DisableClock(LL_AHB3_GRP1_PERIPH_CRYP)
#endif /* CRYP */

#define __HAL_RCC_HASH_CLK_ENABLE()    LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_HASH)
#define __HAL_RCC_HASH_CLK_DISABLE()   LL_AHB3_GRP1_DisableClock(LL_AHB3_GRP1_PERIPH_HASH)

#if defined (CPU_IN_SECURE_STATE)
#define __HAL_RCC_IAC_CLK_ENABLE()     LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_IAC)
#define __HAL_RCC_IAC_CLK_DISABLE()    LL_AHB3_GRP1_DisableClock(LL_AHB3_GRP1_PERIPH_IAC)
#endif /* CPU_IN_SECURE_STATE */

#define __HAL_RCC_PKA_CLK_ENABLE()     LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_PKA)
#define __HAL_RCC_PKA_CLK_DISABLE()    LL_AHB3_GRP1_DisableClock(LL_AHB3_GRP1_PERIPH_PKA)

#if defined (CPU_IN_SECURE_STATE)
#define __HAL_RCC_RIFSC_CLK_ENABLE()   LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_RIFSC)
#define __HAL_RCC_RIFSC_CLK_DISABLE()  LL_AHB3_GRP1_DisableClock(LL_AHB3_GRP1_PERIPH_RIFSC)
#endif /* CPU_IN_SECURE_STATE */

#if defined (CPU_IN_SECURE_STATE)
#define __HAL_RCC_RISAF_CLK_ENABLE()   LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_RISAF)
#define __HAL_RCC_RISAF_CLK_DISABLE()  LL_AHB3_GRP1_DisableClock(LL_AHB3_GRP1_PERIPH_RISAF)
#endif /* CPU_IN_SECURE_STATE */

#define __HAL_RCC_RNG_CLK_ENABLE()     LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_RNG)
#define __HAL_RCC_RNG_CLK_DISABLE()    LL_AHB3_GRP1_DisableClock(LL_AHB3_GRP1_PERIPH_RNG)

#if defined(SAES)
#define __HAL_RCC_SAES_CLK_ENABLE()    LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_SAES)
#define __HAL_RCC_SAES_CLK_DISABLE()   LL_AHB3_GRP1_DisableClock(LL_AHB3_GRP1_PERIPH_SAES)
#endif /* SAES */

/**
  * @}
  */

/** @defgroup RCC_AHB4_Peripheral_Clock_Enable_Disable AHB4 Peripheral Clock Enable Disable
  * @brief  Enable or disable the AHB4 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @note   PWR peripheral clock is always security-protected and thus hidden to the non-secure
  *         application.
  * @{
  */

#define __HAL_RCC_CRC_CLK_ENABLE()     LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_CRC)
#define __HAL_RCC_CRC_CLK_DISABLE()    LL_AHB4_GRP1_DisableClock(LL_AHB4_GRP1_PERIPH_CRC)

#define __HAL_RCC_GPIOA_CLK_ENABLE()   LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOA)
#define __HAL_RCC_GPIOA_CLK_DISABLE()  LL_AHB4_GRP1_DisableClock(LL_AHB4_GRP1_PERIPH_GPIOA)

#define __HAL_RCC_GPIOB_CLK_ENABLE()   LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOB)
#define __HAL_RCC_GPIOB_CLK_DISABLE()  LL_AHB4_GRP1_DisableClock(LL_AHB4_GRP1_PERIPH_GPIOB)

#define __HAL_RCC_GPIOC_CLK_ENABLE()   LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOC)
#define __HAL_RCC_GPIOC_CLK_DISABLE()  LL_AHB4_GRP1_DisableClock(LL_AHB4_GRP1_PERIPH_GPIOC)

#define __HAL_RCC_GPIOD_CLK_ENABLE()   LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOD)
#define __HAL_RCC_GPIOD_CLK_DISABLE()  LL_AHB4_GRP1_DisableClock(LL_AHB4_GRP1_PERIPH_GPIOD)

#define __HAL_RCC_GPIOE_CLK_ENABLE()   LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOE)
#define __HAL_RCC_GPIOE_CLK_DISABLE()  LL_AHB4_GRP1_DisableClock(LL_AHB4_GRP1_PERIPH_GPIOE)

#define __HAL_RCC_GPIOF_CLK_ENABLE()   LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOF)
#define __HAL_RCC_GPIOF_CLK_DISABLE()  LL_AHB4_GRP1_DisableClock(LL_AHB4_GRP1_PERIPH_GPIOF)

#define __HAL_RCC_GPIOG_CLK_ENABLE()   LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOG)
#define __HAL_RCC_GPIOG_CLK_DISABLE()  LL_AHB4_GRP1_DisableClock(LL_AHB4_GRP1_PERIPH_GPIOG)

#define __HAL_RCC_GPIOH_CLK_ENABLE()   LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOH)
#define __HAL_RCC_GPIOH_CLK_DISABLE()  LL_AHB4_GRP1_DisableClock(LL_AHB4_GRP1_PERIPH_GPIOH)

#define __HAL_RCC_GPION_CLK_ENABLE()   LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPION)
#define __HAL_RCC_GPION_CLK_DISABLE()  LL_AHB4_GRP1_DisableClock(LL_AHB4_GRP1_PERIPH_GPION)

#define __HAL_RCC_GPIOO_CLK_ENABLE()   LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOO)
#define __HAL_RCC_GPIOO_CLK_DISABLE()  LL_AHB4_GRP1_DisableClock(LL_AHB4_GRP1_PERIPH_GPIOO)

#define __HAL_RCC_GPIOP_CLK_ENABLE()   LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOP)
#define __HAL_RCC_GPIOP_CLK_DISABLE()  LL_AHB4_GRP1_DisableClock(LL_AHB4_GRP1_PERIPH_GPIOP)

#define __HAL_RCC_GPIOQ_CLK_ENABLE()   LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOQ)
#define __HAL_RCC_GPIOQ_CLK_DISABLE()  LL_AHB4_GRP1_DisableClock(LL_AHB4_GRP1_PERIPH_GPIOQ)

#if defined (CPU_IN_SECURE_STATE)
#define __HAL_RCC_PWR_CLK_ENABLE()     LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_PWR)
#define __HAL_RCC_PWR_CLK_DISABLE()    LL_AHB4_GRP1_DisableClock(LL_AHB4_GRP1_PERIPH_PWR)
#endif /* CPU_IN_SECURE_STATE */

/**
  * @}
  */

/** @defgroup RCC_AHB5_Peripheral_Clock_Enable_Disable AHB5 Peripheral Clock Enable Disable
  * @brief  Enable or disable the AHB5 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_DMA2D_CLK_ENABLE()   LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_DMA2D)
#define __HAL_RCC_DMA2D_CLK_DISABLE()  LL_AHB5_GRP1_DisableClock(LL_AHB5_GRP1_PERIPH_DMA2D)

#if defined(ETH1)
#define __HAL_RCC_ETH1_CLK_ENABLE()    LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_ETH1)
#define __HAL_RCC_ETH1_CLK_DISABLE()   LL_AHB5_GRP1_DisableClock(LL_AHB5_GRP1_PERIPH_ETH1)

#define __HAL_RCC_ETH1MAC_CLK_ENABLE() LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_ETH1MAC)
#define __HAL_RCC_ETH1MAC_CLK_DISABLE() LL_AHB5_GRP1_DisableClock(LL_AHB5_GRP1_PERIPH_ETH1MAC)

#define __HAL_RCC_ETH1TX_CLK_ENABLE()  LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_ETH1TX)
#define __HAL_RCC_ETH1TX_CLK_DISABLE() LL_AHB5_GRP1_DisableClock(LL_AHB5_GRP1_PERIPH_ETH1TX)

#define __HAL_RCC_ETH1RX_CLK_ENABLE()  LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_ETH1RX)
#define __HAL_RCC_ETH1RX_CLK_DISABLE() LL_AHB5_GRP1_DisableClock(LL_AHB5_GRP1_PERIPH_ETH1RX)
#endif /* ETH1 */

#define __HAL_RCC_FMC_CLK_ENABLE()     LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_FMC)
#define __HAL_RCC_FMC_CLK_DISABLE()    LL_AHB5_GRP1_DisableClock(LL_AHB5_GRP1_PERIPH_FMC)

#if defined(GFXMMU)
#define __HAL_RCC_GFXMMU_CLK_ENABLE()  LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_GFXMMU)
#define __HAL_RCC_GFXMMU_CLK_DISABLE() LL_AHB5_GRP1_DisableClock(LL_AHB5_GRP1_PERIPH_GFXMMU)
#endif /* GFXMMU */

#if defined(GPU2D)
#define __HAL_RCC_GPU2D_CLK_ENABLE()   LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_GPU2D)
#define __HAL_RCC_GPU2D_CLK_DISABLE()  LL_AHB5_GRP1_DisableClock(LL_AHB5_GRP1_PERIPH_GPU2D)
#endif /* GPU2D */

#define __HAL_RCC_HPDMA1_CLK_ENABLE()  LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_HPDMA1)
#define __HAL_RCC_HPDMA1_CLK_DISABLE() LL_AHB5_GRP1_DisableClock(LL_AHB5_GRP1_PERIPH_HPDMA1)

#if defined(JPEG)
#define __HAL_RCC_JPEG_CLK_ENABLE()    LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_JPEG)
#define __HAL_RCC_JPEG_CLK_DISABLE()   LL_AHB5_GRP1_DisableClock(LL_AHB5_GRP1_PERIPH_JPEG)
#endif /* JPEG */

#define __HAL_RCC_XSPI1_CLK_ENABLE()   LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_XSPI1)
#define __HAL_RCC_XSPI1_CLK_DISABLE()  LL_AHB5_GRP1_DisableClock(LL_AHB5_GRP1_PERIPH_XSPI1)

#define __HAL_RCC_XSPI2_CLK_ENABLE()   LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_XSPI2)
#define __HAL_RCC_XSPI2_CLK_DISABLE()  LL_AHB5_GRP1_DisableClock(LL_AHB5_GRP1_PERIPH_XSPI2)

#define __HAL_RCC_XSPI3_CLK_ENABLE()   LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_XSPI3)
#define __HAL_RCC_XSPI3_CLK_DISABLE()  LL_AHB5_GRP1_DisableClock(LL_AHB5_GRP1_PERIPH_XSPI3)

#define __HAL_RCC_XSPIM_CLK_ENABLE()   LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_XSPIM)
#define __HAL_RCC_XSPIM_CLK_DISABLE()  LL_AHB5_GRP1_DisableClock(LL_AHB5_GRP1_PERIPH_XSPIM)

#define __HAL_RCC_MCE1_CLK_ENABLE()    LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_MCE1)
#define __HAL_RCC_MCE1_CLK_DISABLE()   LL_AHB5_GRP1_DisableClock(LL_AHB5_GRP1_PERIPH_MCE1)

#define __HAL_RCC_MCE2_CLK_ENABLE()    LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_MCE2)
#define __HAL_RCC_MCE2_CLK_DISABLE()   LL_AHB5_GRP1_DisableClock(LL_AHB5_GRP1_PERIPH_MCE2)

#define __HAL_RCC_MCE3_CLK_ENABLE()    LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_MCE3)
#define __HAL_RCC_MCE3_CLK_DISABLE()   LL_AHB5_GRP1_DisableClock(LL_AHB5_GRP1_PERIPH_MCE3)

#define __HAL_RCC_MCE4_CLK_ENABLE()    LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_MCE4)
#define __HAL_RCC_MCE4_CLK_DISABLE()   LL_AHB5_GRP1_DisableClock(LL_AHB5_GRP1_PERIPH_MCE4)

#define __HAL_RCC_CACHEAXI_CLK_ENABLE()  LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_CACHEAXI)
#define __HAL_RCC_CACHEAXI_CLK_DISABLE() LL_AHB5_GRP1_DisableClock(LL_AHB5_GRP1_PERIPH_CACHEAXI)

#define __HAL_RCC_NPU_CLK_ENABLE()     LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_NPU)
#define __HAL_RCC_NPU_CLK_DISABLE()    LL_AHB5_GRP1_DisableClock(LL_AHB5_GRP1_PERIPH_NPU)

#define __HAL_RCC_DCMI_PSSI_CLK_ENABLE()    LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_PSSI)
#define __HAL_RCC_DCMI_PSSI_CLK_DISABLE()   LL_AHB5_GRP1_DisableClock(LL_AHB5_GRP1_PERIPH_PSSI)

#define __HAL_RCC_SDMMC1_CLK_ENABLE()  LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_SDMMC1)
#define __HAL_RCC_SDMMC1_CLK_DISABLE() LL_AHB5_GRP1_DisableClock(LL_AHB5_GRP1_PERIPH_SDMMC1)

#define __HAL_RCC_SDMMC2_CLK_ENABLE()  LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_SDMMC2)
#define __HAL_RCC_SDMMC2_CLK_DISABLE() LL_AHB5_GRP1_DisableClock(LL_AHB5_GRP1_PERIPH_SDMMC2)

#define __HAL_RCC_USB1_OTG_HS_CLK_ENABLE()  LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_OTG1)
#define __HAL_RCC_USB1_OTG_HS_CLK_DISABLE() LL_AHB5_GRP1_DisableClock(LL_AHB5_GRP1_PERIPH_OTG1)

#define __HAL_RCC_USB1_OTG_HS_PHY_CLK_ENABLE()  LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_OTGPHY1)
#define __HAL_RCC_USB1_OTG_HS_PHY_CLK_DISABLE() LL_AHB5_GRP1_DisableClock(LL_AHB5_GRP1_PERIPH_OTGPHY1)

#define __HAL_RCC_USB2_OTG_HS_CLK_ENABLE()  LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_OTG2)
#define __HAL_RCC_USB2_OTG_HS_CLK_DISABLE() LL_AHB5_GRP1_DisableClock(LL_AHB5_GRP1_PERIPH_OTG2)

#define __HAL_RCC_USB2_OTG_HS_PHY_CLK_ENABLE()  LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_OTGPHY2)
#define __HAL_RCC_USB2_OTG_HS_PHY_CLK_DISABLE() LL_AHB5_GRP1_DisableClock(LL_AHB5_GRP1_PERIPH_OTGPHY2)

/**
  * @}
  */

/** @defgroup RCC_APB1_Peripheral_Clock_Enable_Disable APB1 Peripheral Clock Enable Disable
  * @brief  Enable or disable the APB1 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_FDCAN_CLK_ENABLE()  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_FDCAN)
#define __HAL_RCC_FDCAN_CLK_DISABLE() LL_APB1_GRP2_DisableClock(LL_APB1_GRP2_PERIPH_FDCAN)

#define __HAL_RCC_I2C1_CLK_ENABLE()   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1)
#define __HAL_RCC_I2C1_CLK_DISABLE()  LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_I2C1)

#define __HAL_RCC_I2C2_CLK_ENABLE()   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2)
#define __HAL_RCC_I2C2_CLK_DISABLE()  LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_I2C2)

#define __HAL_RCC_I2C3_CLK_ENABLE()   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C3)
#define __HAL_RCC_I2C3_CLK_DISABLE()  LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_I2C3)

#define __HAL_RCC_I3C1_CLK_ENABLE()   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I3C1)
#define __HAL_RCC_I3C1_CLK_DISABLE()  LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_I3C1)

#define __HAL_RCC_I3C2_CLK_ENABLE()   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I3C2)
#define __HAL_RCC_I3C2_CLK_DISABLE()  LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_I3C2)

#define __HAL_RCC_LPTIM1_CLK_ENABLE()  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_LPTIM1)
#define __HAL_RCC_LPTIM1_CLK_DISABLE() LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_LPTIM1)

#define __HAL_RCC_MDIOS_CLK_ENABLE()  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_MDIOS)
#define __HAL_RCC_MDIOS_CLK_DISABLE() LL_APB1_GRP2_DisableClock(LL_APB1_GRP2_PERIPH_MDIOS)

#define __HAL_RCC_SPDIFRX1_CLK_ENABLE()  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPDIFRX1)
#define __HAL_RCC_SPDIFRX1_CLK_DISABLE() LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_SPDIFRX1)

#define __HAL_RCC_SPI2_CLK_ENABLE()   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2)
#define __HAL_RCC_SPI2_CLK_DISABLE()  LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_SPI2)

#define __HAL_RCC_SPI3_CLK_ENABLE()   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI3)
#define __HAL_RCC_SPI3_CLK_DISABLE()  LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_SPI3)

#define __HAL_RCC_TIM2_CLK_ENABLE()   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2)
#define __HAL_RCC_TIM2_CLK_DISABLE()  LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM2)

#define __HAL_RCC_TIM3_CLK_ENABLE()   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3)
#define __HAL_RCC_TIM3_CLK_DISABLE()  LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM3)

#define __HAL_RCC_TIM4_CLK_ENABLE()   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4)
#define __HAL_RCC_TIM4_CLK_DISABLE()  LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM4)

#define __HAL_RCC_TIM5_CLK_ENABLE()   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM5)
#define __HAL_RCC_TIM5_CLK_DISABLE()  LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM5)

#define __HAL_RCC_TIM6_CLK_ENABLE()   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6)
#define __HAL_RCC_TIM6_CLK_DISABLE()  LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM6)

#define __HAL_RCC_TIM7_CLK_ENABLE()   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM7)
#define __HAL_RCC_TIM7_CLK_DISABLE()  LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM7)

#define __HAL_RCC_TIM10_CLK_ENABLE()  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM10)
#define __HAL_RCC_TIM10_CLK_DISABLE() LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM10)

#define __HAL_RCC_TIM11_CLK_ENABLE()  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM11)
#define __HAL_RCC_TIM11_CLK_DISABLE() LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM11)

#define __HAL_RCC_TIM12_CLK_ENABLE()  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM12)
#define __HAL_RCC_TIM12_CLK_DISABLE() LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM12)

#define __HAL_RCC_TIM13_CLK_ENABLE()  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM13)
#define __HAL_RCC_TIM13_CLK_DISABLE() LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM13)

#define __HAL_RCC_TIM14_CLK_ENABLE()  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM14)
#define __HAL_RCC_TIM14_CLK_DISABLE() LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM14)

#define __HAL_RCC_USART2_CLK_ENABLE()  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2)
#define __HAL_RCC_USART2_CLK_DISABLE() LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_USART2)

#define __HAL_RCC_USART3_CLK_ENABLE()  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3)
#define __HAL_RCC_USART3_CLK_DISABLE() LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_USART3)

#define __HAL_RCC_UART4_CLK_ENABLE()  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART4)
#define __HAL_RCC_UART4_CLK_DISABLE() LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_UART4)

#define __HAL_RCC_UART5_CLK_ENABLE()  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART5)
#define __HAL_RCC_UART5_CLK_DISABLE() LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_UART5)

#define __HAL_RCC_UART7_CLK_ENABLE()  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART7)
#define __HAL_RCC_UART7_CLK_DISABLE() LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_UART7)

#define __HAL_RCC_UART8_CLK_ENABLE()  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART8)
#define __HAL_RCC_UART8_CLK_DISABLE() LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_UART8)

#define __HAL_RCC_UCPD1_CLK_ENABLE()  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_UCPD1)
#define __HAL_RCC_UCPD1_CLK_DISABLE() LL_APB1_GRP2_DisableClock(LL_APB1_GRP2_PERIPH_UCPD1)

#define __HAL_RCC_WWDG_CLK_ENABLE()   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_WWDG)

/**
  * @}
  */

/** @defgroup RCC_APB2_Peripheral_Clock_Enable_Disable APB2 Peripheral Clock Enable Disable
  * @brief  Enable or disable the APB2 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_SAI1_CLK_ENABLE()    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SAI1)
#define __HAL_RCC_SAI1_CLK_DISABLE()   LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_SAI1)

#define __HAL_RCC_SAI2_CLK_ENABLE()    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SAI2)
#define __HAL_RCC_SAI2_CLK_DISABLE()   LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_SAI2)

#define __HAL_RCC_SPI1_CLK_ENABLE()    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1)
#define __HAL_RCC_SPI1_CLK_DISABLE()   LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_SPI1)

#define __HAL_RCC_SPI4_CLK_ENABLE()    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI4)
#define __HAL_RCC_SPI4_CLK_DISABLE()   LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_SPI4)

#define __HAL_RCC_SPI5_CLK_ENABLE()    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI5)
#define __HAL_RCC_SPI5_CLK_DISABLE()   LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_SPI5)

#define __HAL_RCC_TIM1_CLK_ENABLE()    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1)
#define __HAL_RCC_TIM1_CLK_DISABLE()   LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM1)

#define __HAL_RCC_TIM8_CLK_ENABLE()    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM8)
#define __HAL_RCC_TIM8_CLK_DISABLE()   LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM8)

#define __HAL_RCC_TIM9_CLK_ENABLE()    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM9)
#define __HAL_RCC_TIM9_CLK_DISABLE()   LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM9)

#define __HAL_RCC_TIM15_CLK_ENABLE()   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM15)
#define __HAL_RCC_TIM15_CLK_DISABLE()  LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM15)

#define __HAL_RCC_TIM16_CLK_ENABLE()   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM16)
#define __HAL_RCC_TIM16_CLK_DISABLE()  LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM16)

#define __HAL_RCC_TIM17_CLK_ENABLE()   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM17)
#define __HAL_RCC_TIM17_CLK_DISABLE()  LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM17)

#define __HAL_RCC_TIM18_CLK_ENABLE()   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM18)
#define __HAL_RCC_TIM18_CLK_DISABLE()  LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM18)

#define __HAL_RCC_USART1_CLK_ENABLE()  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1)
#define __HAL_RCC_USART1_CLK_DISABLE() LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_USART1)

#define __HAL_RCC_USART6_CLK_ENABLE()  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART6)
#define __HAL_RCC_USART6_CLK_DISABLE() LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_USART6)

#define __HAL_RCC_UART9_CLK_ENABLE()   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_UART9)
#define __HAL_RCC_UART9_CLK_DISABLE()  LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_UART9)

#define __HAL_RCC_USART10_CLK_ENABLE()  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART10)
#define __HAL_RCC_USART10_CLK_DISABLE() LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_USART10)

/**
  * @}
  */

/** @defgroup RCC_APB3_Peripheral_Clock_Enable_Disable APB3 Peripheral Clock Enable Disable
  * @brief  Enable or disable the APB3 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_DBGMCU_CLK_ENABLE()  LL_BUS_EnableClock(LL_APB3);
#define __HAL_RCC_DBGMCU_CLK_DISABLE() LL_BUS_DisableClock(LL_APB3);

/**
  * @}
  */

/** @defgroup RCC_APB4_Peripheral_Clock_Enable_Disable APB4 Peripheral Clock Enable Disable
  * @brief  Enable or disable the APB4 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @note   BSEC peripheral clock is always security-protected and thus hidden to the non-secure
  *         application.
  * @{
  */

#if defined (CPU_IN_SECURE_STATE)
#define __HAL_RCC_BSEC_CLK_ENABLE()     LL_APB4_GRP2_EnableClock(LL_APB4_GRP2_PERIPH_BSEC)
#define __HAL_RCC_BSEC_CLK_DISABLE()    LL_APB4_GRP2_DisableClock(LL_APB4_GRP2_PERIPH_BSEC)
#endif /* CPU_IN_SECURE_STATE */

#define __HAL_RCC_DTS_CLK_ENABLE()      LL_APB4_GRP2_EnableClock(LL_APB4_GRP2_PERIPH_DTS)
#define __HAL_RCC_DTS_CLK_DISABLE()     LL_APB4_GRP2_DisableClock(LL_APB4_GRP2_PERIPH_DTS)

#define __HAL_RCC_HDP_CLK_ENABLE()      LL_APB4_GRP1_EnableClock(LL_APB4_GRP1_PERIPH_HDP)
#define __HAL_RCC_HDP_CLK_DISABLE()     LL_APB4_GRP1_DisableClock(LL_APB4_GRP1_PERIPH_HDP)

#define __HAL_RCC_I2C4_CLK_ENABLE()     LL_APB4_GRP1_EnableClock(LL_APB4_GRP1_PERIPH_I2C4)
#define __HAL_RCC_I2C4_CLK_DISABLE()    LL_APB4_GRP1_DisableClock(LL_APB4_GRP1_PERIPH_I2C4)

#define __HAL_RCC_LPTIM2_CLK_ENABLE()   LL_APB4_GRP1_EnableClock(LL_APB4_GRP1_PERIPH_LPTIM2)
#define __HAL_RCC_LPTIM2_CLK_DISABLE()  LL_APB4_GRP1_DisableClock(LL_APB4_GRP1_PERIPH_LPTIM2)

#define __HAL_RCC_LPTIM3_CLK_ENABLE()   LL_APB4_GRP1_EnableClock(LL_APB4_GRP1_PERIPH_LPTIM3)
#define __HAL_RCC_LPTIM3_CLK_DISABLE()  LL_APB4_GRP1_DisableClock(LL_APB4_GRP1_PERIPH_LPTIM3)

#define __HAL_RCC_LPTIM4_CLK_ENABLE()   LL_APB4_GRP1_EnableClock(LL_APB4_GRP1_PERIPH_LPTIM4)
#define __HAL_RCC_LPTIM4_CLK_DISABLE()  LL_APB4_GRP1_DisableClock(LL_APB4_GRP1_PERIPH_LPTIM4)

#define __HAL_RCC_LPTIM5_CLK_ENABLE()   LL_APB4_GRP1_EnableClock(LL_APB4_GRP1_PERIPH_LPTIM5)
#define __HAL_RCC_LPTIM5_CLK_DISABLE()  LL_APB4_GRP1_DisableClock(LL_APB4_GRP1_PERIPH_LPTIM5)

#define __HAL_RCC_LPUART1_CLK_ENABLE()  LL_APB4_GRP1_EnableClock(LL_APB4_GRP1_PERIPH_LPUART1)
#define __HAL_RCC_LPUART1_CLK_DISABLE() LL_APB4_GRP1_DisableClock(LL_APB4_GRP1_PERIPH_LPUART1)

#define __HAL_RCC_RTC_CLK_ENABLE()      LL_APB4_GRP1_EnableClock(LL_APB4_GRP1_PERIPH_RTC)
#define __HAL_RCC_RTC_CLK_DISABLE()     LL_APB4_GRP1_DisableClock(LL_APB4_GRP1_PERIPH_RTC)

#define __HAL_RCC_RTCAPB_CLK_ENABLE()   LL_APB4_GRP1_EnableClock(LL_APB4_GRP1_PERIPH_RTCAPB)
#define __HAL_RCC_RTCAPB_CLK_DISABLE()  LL_APB4_GRP1_DisableClock(LL_APB4_GRP1_PERIPH_RTCAPB)

#define __HAL_RCC_SPI6_CLK_ENABLE()     LL_APB4_GRP1_EnableClock(LL_APB4_GRP1_PERIPH_SPI6)
#define __HAL_RCC_SPI6_CLK_DISABLE()    LL_APB4_GRP1_DisableClock(LL_APB4_GRP1_PERIPH_SPI6)

#define __HAL_RCC_SYSCFG_CLK_ENABLE()   LL_APB4_GRP2_EnableClock(LL_APB4_GRP2_PERIPH_SYSCFG)
#define __HAL_RCC_SYSCFG_CLK_DISABLE()  LL_APB4_GRP2_DisableClock(LL_APB4_GRP2_PERIPH_SYSCFG)

#define __HAL_RCC_VREFBUF_CLK_ENABLE()  LL_APB4_GRP1_EnableClock(LL_APB4_GRP1_PERIPH_VREFBUF)
#define __HAL_RCC_VREFBUF_CLK_DISABLE() LL_APB4_GRP1_DisableClock(LL_APB4_GRP1_PERIPH_VREFBUF)

/**
  * @}
  */

/** @defgroup RCC_APB5_Peripheral_Clock_Enable_Disable APB5 Peripheral Clock Enable Disable
  * @brief  Enable or disable the APB5 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_CSI_CLK_ENABLE()     LL_APB5_GRP1_EnableClock(LL_APB5_GRP1_PERIPH_CSI)
#define __HAL_RCC_CSI_CLK_DISABLE()    LL_APB5_GRP1_DisableClock(LL_APB5_GRP1_PERIPH_CSI)

#define __HAL_RCC_DCMIPP_CLK_ENABLE()  LL_APB5_GRP1_EnableClock(LL_APB5_GRP1_PERIPH_DCMIPP)
#define __HAL_RCC_DCMIPP_CLK_DISABLE() LL_APB5_GRP1_DisableClock(LL_APB5_GRP1_PERIPH_DCMIPP)

#define __HAL_RCC_GFXTIM_CLK_ENABLE()  LL_APB5_GRP1_EnableClock(LL_APB5_GRP1_PERIPH_GFXTIM)
#define __HAL_RCC_GFXTIM_CLK_DISABLE() LL_APB5_GRP1_DisableClock(LL_APB5_GRP1_PERIPH_GFXTIM)

#define __HAL_RCC_LTDC_CLK_ENABLE()    LL_APB5_GRP1_EnableClock(LL_APB5_GRP1_PERIPH_LTDC)
#define __HAL_RCC_LTDC_CLK_DISABLE()   LL_APB5_GRP1_DisableClock(LL_APB5_GRP1_PERIPH_LTDC)

#define __HAL_RCC_VENC_CLK_ENABLE()    do { \
                                            LL_APB5_GRP1_EnableClock(LL_APB5_GRP1_PERIPH_VENC); \
                                            LL_BUS_EnableClock(LL_APB5); \
                                          } while(0)
#define __HAL_RCC_VENC_CLK_DISABLE()   do { \
                                            LL_BUS_DisableClock(LL_APB5); \
                                            LL_APB5_GRP1_DisableClock(LL_APB5_GRP1_PERIPH_VENC); \
                                          } while(0)

/**
  * @}
  */

/** @defgroup RCC_MISC_Configuration_Clock_Enable_Disable Misc Configuration Clock Enable Disable
  * @brief  Enable or disable the misc configuration clock.
  * @note   After reset, the misc configuration clock is disabled and
  *         the application software has to enable this clock before using it.
  * @note   DBG clock is always security-protected and thus hidden to the non-secure application.
  * @{
  */

#if defined (CPU_IN_SECURE_STATE)
#define __HAL_RCC_DBG_CLK_ENABLE()     LL_MISC_EnableClock(LL_DBG)
#define __HAL_RCC_DBG_CLK_DISABLE()    LL_MISC_DisableClock(LL_DBG)
#endif /* CPU_IN_SECURE_STATE */

#define __HAL_RCC_XSPIPHYCOMP_CLK_ENABLE()  LL_MISC_EnableClock(LL_XSPIPHYCOMP)
#define __HAL_RCC_XSPIPHYCOMP_CLK_DISABLE() LL_MISC_DisableClock(LL_XSPIPHYCOMP)

#define __HAL_RCC_PER_CLK_ENABLE()  LL_MISC_EnableClock(LL_PER)
#define __HAL_RCC_PER_CLK_DISABLE() LL_MISC_DisableClock(LL_PER)

/**
  * @}
  */

/** @defgroup RCC_Embedded_Mem_Clock_Status Embedded Memory Clock Enabled Status
  * @brief  Check whether the embedded memory clock is enabled or not.
  * @note   After reset, some embedded memory clocks are disabled
  *         and the application software has to enable these memory clocks before using them.
  * @{
  */

#define __HAL_RCC_AXISRAM1_MEM_IS_CLK_ENABLED()     LL_MEM_IsEnabledClock(LL_MEM_AXISRAM1)
#define __HAL_RCC_AXISRAM2_MEM_IS_CLK_ENABLED()     LL_MEM_IsEnabledClock(LL_MEM_AXISRAM2)
#define __HAL_RCC_AXISRAM3_MEM_IS_CLK_ENABLED()     LL_MEM_IsEnabledClock(LL_MEM_AXISRAM3)
#define __HAL_RCC_AXISRAM4_MEM_IS_CLK_ENABLED()     LL_MEM_IsEnabledClock(LL_MEM_AXISRAM4)
#define __HAL_RCC_AXISRAM5_MEM_IS_CLK_ENABLED()     LL_MEM_IsEnabledClock(LL_MEM_AXISRAM5)
#define __HAL_RCC_AXISRAM6_MEM_IS_CLK_ENABLED()     LL_MEM_IsEnabledClock(LL_MEM_AXISRAM6)
#define __HAL_RCC_AHBSRAM1_MEM_IS_CLK_ENABLED()     LL_MEM_IsEnabledClock(LL_MEM_AHBSRAM1)
#define __HAL_RCC_AHBSRAM2_MEM_IS_CLK_ENABLED()     LL_MEM_IsEnabledClock(LL_MEM_AHBSRAM2)
#define __HAL_RCC_BKPSRAM_MEM_IS_CLK_ENABLED()      LL_MEM_IsEnabledClock(LL_MEM_BKPSRAM)
#define __HAL_RCC_FLEXRAM_MEM_IS_CLK_ENABLED()      LL_MEM_IsEnabledClock(LL_MEM_FLEXRAM)
#define __HAL_RCC_CACHEAXIRAM_MEM_IS_CLK_ENABLED()  LL_MEM_IsEnabledClock(LL_MEM_CACHEAXIRAM)
#define __HAL_RCC_VENCRAM_MEM_IS_CLK_ENABLED()      LL_MEM_IsEnabledClock(LL_MEM_VENCRAM)

/**
  * @}
  */

/** @defgroup RCC_AHB1_Clock_Enable_Status AHB1 Peripheral Clock Enabled Status
  * @brief  Check whether the AHB1 peripheral clock is enabled or not.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_ADC12_IS_CLK_ENABLED()   LL_AHB1_GRP1_IsEnabledClock(LL_AHB1_GRP1_PERIPH_ADC12)

#define __HAL_RCC_GPDMA1_IS_CLK_ENABLED()  LL_AHB1_GRP1_IsEnabledClock(LL_AHB1_GRP1_PERIPH_GPDMA1)

/**
  * @}
  */

/** @defgroup RCC_AHB2_Clock_Enable_Status AHB2 Peripheral Clock Enabled Status
  * @brief  Check whether the AHB2 peripheral clock is enabled or not.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_ADF1_IS_CLK_ENABLED()    LL_AHB2_GRP1_IsEnabledClock(LL_AHB2_GRP1_PERIPH_ADF1)

#define __HAL_RCC_MDF1_IS_CLK_ENABLED()    LL_AHB2_GRP1_IsEnabledClock(LL_AHB2_GRP1_PERIPH_MDF1)

#define __HAL_RCC_RAMCFG_IS_CLK_ENABLED()  LL_AHB2_GRP1_IsEnabledClock(LL_AHB2_GRP1_PERIPH_RAMCFG)

/**
  * @}
  */

/** @defgroup RCC_AHB3_Clock_Enable_Status AHB3 Peripheral Clock Enabled Status
  * @brief  Check whether the AHB3 peripheral clock is enabled or not.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#if defined(CRYP)
#define __HAL_RCC_CRYP_IS_CLK_ENABLED()    LL_AHB3_GRP1_IsEnabledClock(LL_AHB3_GRP1_PERIPH_CRYP)
#endif /* CRYP */

#define __HAL_RCC_HASH_IS_CLK_ENABLED()    LL_AHB3_GRP1_IsEnabledClock(LL_AHB3_GRP1_PERIPH_HASH)

#define __HAL_RCC_IAC_IS_CLK_ENABLED()     LL_AHB3_GRP1_IsEnabledClock(LL_AHB3_GRP1_PERIPH_IAC)

#define __HAL_RCC_PKA_IS_CLK_ENABLED()     LL_AHB3_GRP1_IsEnabledClock(LL_AHB3_GRP1_PERIPH_PKA)

#define __HAL_RCC_RIFSC_IS_CLK_ENABLED()   LL_AHB3_GRP1_IsEnabledClock(LL_AHB3_GRP1_PERIPH_RIFSC)

#define __HAL_RCC_RISAF_IS_CLK_ENABLED()   LL_AHB3_GRP1_IsEnabledClock(LL_AHB3_GRP1_PERIPH_RISAF)

#define __HAL_RCC_RNG_IS_CLK_ENABLED()     LL_AHB3_GRP1_IsEnabledClock(LL_AHB3_GRP1_PERIPH_RNG)

#if defined(SAES)
#define __HAL_RCC_SAES_IS_CLK_ENABLED()    LL_AHB3_GRP1_IsEnabledClock(LL_AHB3_GRP1_PERIPH_SAES)
#endif /* SAES */

/**
  * @}
  */

/** @defgroup RCC_AHB4_Clock_Enable_Status AHB4 Peripheral Clock Enabled Status
  * @brief  Check whether the AHB4 peripheral clock is enabled or not.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_CRC_IS_CLK_ENABLED()     LL_AHB4_GRP1_IsEnabledClock(LL_AHB4_GRP1_PERIPH_CRC)

#define __HAL_RCC_GPIOA_IS_CLK_ENABLED()   LL_AHB4_GRP1_IsEnabledClock(LL_AHB4_GRP1_PERIPH_GPIOA)

#define __HAL_RCC_GPIOB_IS_CLK_ENABLED()   LL_AHB4_GRP1_IsEnabledClock(LL_AHB4_GRP1_PERIPH_GPIOB)

#define __HAL_RCC_GPIOC_IS_CLK_ENABLED()   LL_AHB4_GRP1_IsEnabledClock(LL_AHB4_GRP1_PERIPH_GPIOC)

#define __HAL_RCC_GPIOD_IS_CLK_ENABLED()   LL_AHB4_GRP1_IsEnabledClock(LL_AHB4_GRP1_PERIPH_GPIOD)

#define __HAL_RCC_GPIOE_IS_CLK_ENABLED()   LL_AHB4_GRP1_IsEnabledClock(LL_AHB4_GRP1_PERIPH_GPIOE)

#define __HAL_RCC_GPIOF_IS_CLK_ENABLED()   LL_AHB4_GRP1_IsEnabledClock(LL_AHB4_GRP1_PERIPH_GPIOF)

#define __HAL_RCC_GPIOG_IS_CLK_ENABLED()   LL_AHB4_GRP1_IsEnabledClock(LL_AHB4_GRP1_PERIPH_GPIOG)

#define __HAL_RCC_GPIOH_IS_CLK_ENABLED()   LL_AHB4_GRP1_IsEnabledClock(LL_AHB4_GRP1_PERIPH_GPIOH)

#define __HAL_RCC_GPION_IS_CLK_ENABLED()   LL_AHB4_GRP1_IsEnabledClock(LL_AHB4_GRP1_PERIPH_GPION)

#define __HAL_RCC_GPIOO_IS_CLK_ENABLED()   LL_AHB4_GRP1_IsEnabledClock(LL_AHB4_GRP1_PERIPH_GPIOO)

#define __HAL_RCC_GPIOP_IS_CLK_ENABLED()   LL_AHB4_GRP1_IsEnabledClock(LL_AHB4_GRP1_PERIPH_GPIOP)

#define __HAL_RCC_GPIOQ_IS_CLK_ENABLED()   LL_AHB4_GRP1_IsEnabledClock(LL_AHB4_GRP1_PERIPH_GPIOQ)

#define __HAL_RCC_PWR_IS_CLK_ENABLED()     LL_AHB4_GRP1_IsEnabledClock(LL_AHB4_GRP1_PERIPH_PWR)

/**
  * @}
  */

/** @defgroup RCC_AHB5_Clock_Enable_Status AHB5 Peripheral Clock Enabled Status
  * @brief  Check whether the AHB5 peripheral clock is enabled or not.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_DMA2D_IS_CLK_ENABLED()   LL_AHB5_GRP1_IsEnabledClock(LL_AHB5_GRP1_PERIPH_DMA2D)

#if defined(ETH1)
#define __HAL_RCC_ETH1_IS_CLK_ENABLED()    LL_AHB5_GRP1_IsEnabledClock(LL_AHB5_GRP1_PERIPH_ETH1)

#define __HAL_RCC_ETH1MAC_IS_CLK_ENABLED() LL_AHB5_GRP1_IsEnabledClock(LL_AHB5_GRP1_PERIPH_ETH1MAC)

#define __HAL_RCC_ETH1TX_IS_CLK_ENABLED()  LL_AHB5_GRP1_IsEnabledClock(LL_AHB5_GRP1_PERIPH_ETH1TX)

#define __HAL_RCC_ETH1RX_IS_CLK_ENABLED()  LL_AHB5_GRP1_IsEnabledClock(LL_AHB5_GRP1_PERIPH_ETH1RX)
#endif /* ETH1 */

#define __HAL_RCC_FMC_IS_CLK_ENABLED()     LL_AHB5_GRP1_IsEnabledClock(LL_AHB5_GRP1_PERIPH_FMC)

#if defined(GFXMMU)
#define __HAL_RCC_GFXMMU_IS_CLK_ENABLED()  LL_AHB5_GRP1_IsEnabledClock(LL_AHB5_GRP1_PERIPH_GFXMMU)
#endif /* GFXMMU */

#if defined(GPU2D)
#define __HAL_RCC_GPU2D_IS_CLK_ENABLED()   LL_AHB5_GRP1_IsEnabledClock(LL_AHB5_GRP1_PERIPH_GPU2D)
#endif /* GPU2D */

#define __HAL_RCC_HPDMA1_IS_CLK_ENABLED()  LL_AHB5_GRP1_IsEnabledClock(LL_AHB5_GRP1_PERIPH_HPDMA1)

#if defined(JPEG)
#define __HAL_RCC_JPEG_IS_CLK_ENABLED()    LL_AHB5_GRP1_IsEnabledClock(LL_AHB5_GRP1_PERIPH_JPEG)
#endif /* JPEG */

#define __HAL_RCC_XSPI1_IS_CLK_ENABLED()   LL_AHB5_GRP1_IsEnabledClock(LL_AHB5_GRP1_PERIPH_XSPI1)

#define __HAL_RCC_XSPI2_IS_CLK_ENABLED()   LL_AHB5_GRP1_IsEnabledClock(LL_AHB5_GRP1_PERIPH_XSPI2)

#define __HAL_RCC_XSPI3_IS_CLK_ENABLED()   LL_AHB5_GRP1_IsEnabledClock(LL_AHB5_GRP1_PERIPH_XSPI3)

#define __HAL_RCC_XSPIM_IS_CLK_ENABLED()   LL_AHB5_GRP1_IsEnabledClock(LL_AHB5_GRP1_PERIPH_XSPIM)

#define __HAL_RCC_MCE1_IS_CLK_ENABLED()    LL_AHB5_GRP1_IsEnabledClock(LL_AHB5_GRP1_PERIPH_MCE1)

#define __HAL_RCC_MCE2_IS_CLK_ENABLED()    LL_AHB5_GRP1_IsEnabledClock(LL_AHB5_GRP1_PERIPH_MCE2)

#define __HAL_RCC_MCE3_IS_CLK_ENABLED()    LL_AHB5_GRP1_IsEnabledClock(LL_AHB5_GRP1_PERIPH_MCE3)

#define __HAL_RCC_MCE4_IS_CLK_ENABLED()    LL_AHB5_GRP1_IsEnabledClock(LL_AHB5_GRP1_PERIPH_MCE4)

#define __HAL_RCC_CACHEAXI_IS_CLK_ENABLED()  LL_AHB5_GRP1_IsEnabledClock(LL_AHB5_GRP1_PERIPH_CACHEAXI)

#define __HAL_RCC_NPU_IS_CLK_ENABLED()     LL_AHB5_GRP1_IsEnabledClock(LL_AHB5_GRP1_PERIPH_NPU)

#define __HAL_RCC_DCMI_PSSI_IS_CLK_ENABLED()    LL_AHB5_GRP1_IsEnabledClock(LL_AHB5_GRP1_PERIPH_PSSI)

#define __HAL_RCC_SDMMC1_IS_CLK_ENABLED()  LL_AHB5_GRP1_IsEnabledClock(LL_AHB5_GRP1_PERIPH_SDMMC1)

#define __HAL_RCC_SDMMC2_IS_CLK_ENABLED()  LL_AHB5_GRP1_IsEnabledClock(LL_AHB5_GRP1_PERIPH_SDMMC2)

#define __HAL_RCC_USB1_OTG_HS_IS_CLK_ENABLED()  LL_AHB5_GRP1_IsEnabledClock(LL_AHB5_GRP1_PERIPH_OTG1)

#define __HAL_RCC_USB1_OTG_HS_PHY_IS_CLK_ENABLED()  LL_AHB5_GRP1_IsEnabledClock(LL_AHB5_GRP1_PERIPH_OTGPHY1)

#define __HAL_RCC_USB2_OTG_HS_IS_CLK_ENABLED()  LL_AHB5_GRP1_IsEnabledClock(LL_AHB5_GRP1_PERIPH_OTG2)

#define __HAL_RCC_USB2_OTG_HS_PHY_IS_CLK_ENABLED()  LL_AHB5_GRP1_IsEnabledClock(LL_AHB5_GRP1_PERIPH_OTGPHY2)

/**
  * @}
  */

/** @defgroup RCC_APB1_Clock_Enable_Status APB1 Peripheral Clock Enabled Status
  * @brief  Check whether the APB1 peripheral clock is enabled or not.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_FDCAN_IS_CLK_ENABLED()  LL_APB1_GRP2_IsEnabledClock(LL_APB1_GRP2_PERIPH_FDCAN)

#define __HAL_RCC_I2C1_IS_CLK_ENABLED()   LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_I2C1)

#define __HAL_RCC_I2C2_IS_CLK_ENABLED()   LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_I2C2)

#define __HAL_RCC_I2C3_IS_CLK_ENABLED()   LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_I2C3)

#define __HAL_RCC_I3C1_IS_CLK_ENABLED()   LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_I3C1)

#define __HAL_RCC_I3C2_IS_CLK_ENABLED()   LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_I3C2)

#define __HAL_RCC_LPTIM1_IS_CLK_ENABLED()  LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_LPTIM1)

#define __HAL_RCC_MDIOS_IS_CLK_ENABLED()  LL_APB1_GRP2_IsEnabledClock(LL_APB1_GRP2_PERIPH_MDIOS)

#define __HAL_RCC_SPDIFRX1_IS_CLK_ENABLED()  LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_SPDIFRX1)

#define __HAL_RCC_SPI2_IS_CLK_ENABLED()   LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_SPI2)

#define __HAL_RCC_SPI3_IS_CLK_ENABLED()   LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_SPI3)

#define __HAL_RCC_TIM2_IS_CLK_ENABLED()   LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_TIM2)

#define __HAL_RCC_TIM3_IS_CLK_ENABLED()   LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_TIM3)

#define __HAL_RCC_TIM4_IS_CLK_ENABLED()   LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_TIM4)

#define __HAL_RCC_TIM5_IS_CLK_ENABLED()   LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_TIM5)

#define __HAL_RCC_TIM6_IS_CLK_ENABLED()   LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_TIM6)

#define __HAL_RCC_TIM7_IS_CLK_ENABLED()   LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_TIM7)

#define __HAL_RCC_TIM10_IS_CLK_ENABLED()  LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_TIM10)

#define __HAL_RCC_TIM11_IS_CLK_ENABLED()  LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_TIM11)

#define __HAL_RCC_TIM12_IS_CLK_ENABLED()  LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_TIM12)

#define __HAL_RCC_TIM13_IS_CLK_ENABLED()  LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_TIM13)

#define __HAL_RCC_TIM14_IS_CLK_ENABLED()  LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_TIM14)

#define __HAL_RCC_USART2_IS_CLK_ENABLED()  LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_USART2)

#define __HAL_RCC_USART3_IS_CLK_ENABLED()  LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_USART3)

#define __HAL_RCC_UART4_IS_CLK_ENABLED()  LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_UART4)

#define __HAL_RCC_UART5_IS_CLK_ENABLED()  LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_UART5)

#define __HAL_RCC_UART7_IS_CLK_ENABLED()  LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_UART7)

#define __HAL_RCC_UART8_IS_CLK_ENABLED()  LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_UART8)

#define __HAL_RCC_UCPD1_IS_CLK_ENABLED()  LL_APB1_GRP2_IsEnabledClock(LL_APB1_GRP2_PERIPH_UCPD1)

#define __HAL_RCC_WWDG_IS_CLK_ENABLED()   LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_WWDG)

/**
  * @}
  */

/** @defgroup RCC_APB2_Clock_Enable_Status APB2 Peripheral Clock Enabled Status
  * @brief  Check whether the APB2 peripheral clock is enabled or not.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_SAI1_IS_CLK_ENABLED()    LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_SAI1)

#define __HAL_RCC_SAI2_IS_CLK_ENABLED()    LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_SAI2)

#define __HAL_RCC_SPI1_IS_CLK_ENABLED()    LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_SPI1)

#define __HAL_RCC_SPI4_IS_CLK_ENABLED()    LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_SPI4)

#define __HAL_RCC_SPI5_IS_CLK_ENABLED()    LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_SPI5)

#define __HAL_RCC_TIM1_IS_CLK_ENABLED()    LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_TIM1)

#define __HAL_RCC_TIM8_IS_CLK_ENABLED()    LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_TIM8)

#define __HAL_RCC_TIM9_IS_CLK_ENABLED()    LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_TIM9)

#define __HAL_RCC_TIM15_IS_CLK_ENABLED()   LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_TIM15)

#define __HAL_RCC_TIM16_IS_CLK_ENABLED()   LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_TIM16)

#define __HAL_RCC_TIM17_IS_CLK_ENABLED()   LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_TIM17)

#define __HAL_RCC_TIM18_IS_CLK_ENABLED()   LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_TIM18)

#define __HAL_RCC_USART1_IS_CLK_ENABLED()  LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_USART1)

#define __HAL_RCC_USART6_IS_CLK_ENABLED()  LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_USART6)

#define __HAL_RCC_UART9_IS_CLK_ENABLED()   LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_UART9)

#define __HAL_RCC_USART10_IS_CLK_ENABLED()  LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_USART10)

/**
  * @}
  */


/** @defgroup RCC_APB3_Clock_Enable_Status APB3 Peripheral Clock Enabled Status
  * @brief  Check whether the APB3 peripheral clock is enabled or not.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_DBGMCU_IS_CLK_ENABLED()   LL_BUS_IsEnabledClock(LL_APB3);

/**
  * @}
  */

/** @defgroup RCC_APB4_Clock_Enable_Status APB4 Peripheral Clock Enabled Status
  * @brief  Check whether the APB4 peripheral clock is enabled or not.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_BSEC_IS_CLK_ENABLED()     LL_APB4_GRP2_IsEnabledClock(LL_APB4_GRP2_PERIPH_BSEC)

#define __HAL_RCC_DTS_IS_CLK_ENABLED()      LL_APB4_GRP2_IsEnabledClock(LL_APB4_GRP2_PERIPH_DTS)

#define __HAL_RCC_HDP_IS_CLK_ENABLED()      LL_APB4_GRP1_IsEnabledClock(LL_APB4_GRP1_PERIPH_HDP)

#define __HAL_RCC_I2C4_IS_CLK_ENABLED()     LL_APB4_GRP1_IsEnabledClock(LL_APB4_GRP1_PERIPH_I2C4)

#define __HAL_RCC_LPTIM2_IS_CLK_ENABLED()   LL_APB4_GRP1_IsEnabledClock(LL_APB4_GRP1_PERIPH_LPTIM2)

#define __HAL_RCC_LPTIM3_IS_CLK_ENABLED()   LL_APB4_GRP1_IsEnabledClock(LL_APB4_GRP1_PERIPH_LPTIM3)

#define __HAL_RCC_LPTIM4_IS_CLK_ENABLED()   LL_APB4_GRP1_IsEnabledClock(LL_APB4_GRP1_PERIPH_LPTIM4)

#define __HAL_RCC_LPTIM5_IS_CLK_ENABLED()   LL_APB4_GRP1_IsEnabledClock(LL_APB4_GRP1_PERIPH_LPTIM5)

#define __HAL_RCC_LPUART1_IS_CLK_ENABLED()  LL_APB4_GRP1_IsEnabledClock(LL_APB4_GRP1_PERIPH_LPUART1)

#define __HAL_RCC_RTC_IS_CLK_ENABLED()      LL_APB4_GRP1_IsEnabledClock(LL_APB4_GRP1_PERIPH_RTC)

#define __HAL_RCC_RTCAPB_IS_CLK_ENABLED()   LL_APB4_GRP1_IsEnabledClock(LL_APB4_GRP1_PERIPH_RTCAPB)

#define __HAL_RCC_SPI6_IS_CLK_ENABLED()     LL_APB4_GRP1_IsEnabledClock(LL_APB4_GRP1_PERIPH_SPI6)

#define __HAL_RCC_SYSCFG_IS_CLK_ENABLED()   LL_APB4_GRP2_IsEnabledClock(LL_APB4_GRP2_PERIPH_SYSCFG)

#define __HAL_RCC_VREFBUF_IS_CLK_ENABLED()  LL_APB4_GRP1_IsEnabledClock(LL_APB4_GRP1_PERIPH_VREFBUF)

/**
  * @}
  */

/** @defgroup RCC_APB5_Clock_Enable_Status APB5 Peripheral Clock Enabled Status
  * @brief  Check whether the APB5 peripheral clock is enabled or not.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_CSI_IS_CLK_ENABLED()     LL_APB5_GRP1_IsEnabledClock(LL_APB5_GRP1_PERIPH_CSI)

#define __HAL_RCC_DCMIPP_IS_CLK_ENABLED()  LL_APB5_GRP1_IsEnabledClock(LL_APB5_GRP1_PERIPH_DCMIPP)

#define __HAL_RCC_GFXTIM_IS_CLK_ENABLED()  LL_APB5_GRP1_IsEnabledClock(LL_APB5_GRP1_PERIPH_GFXTIM)

#define __HAL_RCC_LTDC_IS_CLK_ENABLED()    LL_APB5_GRP1_IsEnabledClock(LL_APB5_GRP1_PERIPH_LTDC)

#define __HAL_RCC_VENC_IS_CLK_ENABLED()    LL_APB5_GRP1_IsEnabledClock(LL_APB5_GRP1_PERIPH_VENC)

/**
  * @}
  */

/** @defgroup RCC_MISC_Configuration_Clock_Enable_Status Misc Configuration Clock Enabled Status
  * @brief  Check whether the misc configuration clock is enabled or not.
  * @note   After reset, the misc configuration clock is disabled and
  *         the application software has to enable this clock before using it.
  * @{
  */

#define __HAL_RCC_DBG_IS_CLK_ENABLED()          LL_MISC_IsEnabledClock(LL_DBG)

#define __HAL_RCC_MCO1_IS_CLK_ENABLED()         LL_MISC_IsEnabledClock(LL_MCO1)

#define __HAL_RCC_MCO2_IS_CLK_ENABLED()         LL_MISC_IsEnabledClock(LL_MCO2)

#define __HAL_RCC_XSPIPHYCOMP_IS_CLK_ENABLED()  LL_MISC_IsEnabledClock(LL_XSPIPHYCOMP)

#define __HAL_RCC_PER_IS_CLK_ENABLED()          LL_MISC_IsEnabledClock(LL_PER)

/**
  * @}
  */

/** @defgroup RCC_AHB1_Force_Release_Reset AHB1 Peripheral Force Release Reset
  * @brief  Force or release AHB1 peripheral reset.
  * @{
  */

#define __HAL_RCC_AHB1_FORCE_RESET()       WRITE_REG(RCC->AHB1RSTSR, 0x00000030UL);
#define __HAL_RCC_AHB1_RELEASE_RESET()     WRITE_REG(RCC->AHB1RSTSR, 0UL)

#define __HAL_RCC_GPDMA1_FORCE_RESET()     LL_AHB1_GRP1_ForceReset(LL_AHB1_GRP1_PERIPH_GPDMA1)
#define __HAL_RCC_GPDMA1_RELEASE_RESET()   LL_AHB1_GRP1_ReleaseReset(LL_AHB1_GRP1_PERIPH_GPDMA1)

#define __HAL_RCC_ADC12_FORCE_RESET()      LL_AHB1_GRP1_ForceReset(LL_AHB1_GRP1_PERIPH_ADC12)
#define __HAL_RCC_ADC12_RELEASE_RESET()    LL_AHB1_GRP1_ReleaseReset(LL_AHB1_GRP1_PERIPH_ADC12)

/**
  * @}
  */

/** @defgroup RCC_AHB2_Force_Release_Reset AHB2 Peripheral Force Release Reset
  * @brief  Force or release AHB2 peripheral reset.
  * @{
  */

#define __HAL_RCC_AHB2_FORCE_RESET()       WRITE_REG(RCC->AHB2RSTSR, 0x00031000UL);
#define __HAL_RCC_AHB2_RELEASE_RESET()     WRITE_REG(RCC->AHB2RSTSR, 0UL)

#define __HAL_RCC_ADF1_FORCE_RESET()       LL_AHB2_GRP1_ForceReset(LL_AHB2_GRP1_PERIPH_ADF1)
#define __HAL_RCC_ADF1_RELEASE_RESET()     LL_AHB2_GRP1_ReleaseReset(LL_AHB2_GRP1_PERIPH_ADF1)

#define __HAL_RCC_MDF1_FORCE_RESET()       LL_AHB2_GRP1_ForceReset(LL_AHB2_GRP1_PERIPH_MDF1)
#define __HAL_RCC_MDF1_RELEASE_RESET()     LL_AHB2_GRP1_ReleaseReset(LL_AHB2_GRP1_PERIPH_MDF1)

#define __HAL_RCC_RAMCFG_FORCE_RESET()     LL_AHB2_GRP1_ForceReset(LL_AHB2_GRP1_PERIPH_RAMCFG)
#define __HAL_RCC_RAMCFG_RELEASE_RESET()   LL_AHB2_GRP1_ReleaseReset(LL_AHB2_GRP1_PERIPH_RAMCFG)

/**
  * @}
  */

/** @defgroup RCC_AHB3_Force_Release_Reset AHB3 Peripheral Force Release Reset
  * @brief  Force or release AHB3 peripheral reset.
  * @note   IAC peripheral reset is always security-protected and thus hidden to the non-secure
  *         application.
  * @{
  */

#if defined (CPU_IN_SECURE_STATE)
#define __HAL_RCC_AHB3_FORCE_RESET()       WRITE_REG(RCC->AHB3RSTSR, 0x00000517UL);
#else
#define __HAL_RCC_AHB3_FORCE_RESET()       WRITE_REG(RCC->AHB3RSTSR, 0x00000117UL);
#endif /* CPU_IN_SECURE_STATE */
#define __HAL_RCC_AHB3_RELEASE_RESET()     WRITE_REG(RCC->AHB3RSTSR, 0UL)

#if defined (CPU_IN_SECURE_STATE)
#define __HAL_RCC_IAC_FORCE_RESET()        LL_AHB3_GRP1_ForceReset(LL_AHB3_GRP1_PERIPH_IAC)
#define __HAL_RCC_IAC_RELEASE_RESET()      LL_AHB3_GRP1_ReleaseReset(LL_AHB3_GRP1_PERIPH_IAC)
#endif /* CPU_IN_SECURE_STATE */

#if defined(CRYP)
#define __HAL_RCC_CRYP_FORCE_RESET()       LL_AHB3_GRP1_ForceReset(LL_AHB3_GRP1_PERIPH_CRYP)
#define __HAL_RCC_CRYP_RELEASE_RESET()     LL_AHB3_GRP1_ReleaseReset(LL_AHB3_GRP1_PERIPH_CRYP)
#endif /* CRYP */

#define __HAL_RCC_HASH_FORCE_RESET()       LL_AHB3_GRP1_ForceReset(LL_AHB3_GRP1_PERIPH_HASH)
#define __HAL_RCC_HASH_RELEASE_RESET()     LL_AHB3_GRP1_ReleaseReset(LL_AHB3_GRP1_PERIPH_HASH)

#define __HAL_RCC_PKA_FORCE_RESET()        LL_AHB3_GRP1_ForceReset(LL_AHB3_GRP1_PERIPH_PKA)
#define __HAL_RCC_PKA_RELEASE_RESET()      LL_AHB3_GRP1_ReleaseReset(LL_AHB3_GRP1_PERIPH_PKA)

#define __HAL_RCC_RNG_FORCE_RESET()        LL_AHB3_GRP1_ForceReset(LL_AHB3_GRP1_PERIPH_RNG)
#define __HAL_RCC_RNG_RELEASE_RESET()      LL_AHB3_GRP1_ReleaseReset(LL_AHB3_GRP1_PERIPH_RNG)

#if defined(SAES)
#define __HAL_RCC_SAES_FORCE_RESET()       LL_AHB3_GRP1_ForceReset(LL_AHB3_GRP1_PERIPH_SAES)
#define __HAL_RCC_SAES_RELEASE_RESET()     LL_AHB3_GRP1_ReleaseReset(LL_AHB3_GRP1_PERIPH_SAES)
#endif /* SAES */

/**
  * @}
  */

/** @defgroup RCC_AHB4_Force_Release_Reset AHB4 Peripheral Force Release Reset
  * @brief  Force or release AHB4 peripheral reset.
  * @note   PWR peripheral reset is always security-protected and thus hidden to the non-secure
  *         application.
  * @{
  */

#if defined (CPU_IN_SECURE_STATE)
#define __HAL_RCC_AHB4_FORCE_RESET()       WRITE_REG(RCC->AHB4RSTSR, 0x000DE0FFUL);
#else
#define __HAL_RCC_AHB4_FORCE_RESET()       WRITE_REG(RCC->AHB4RSTSR, 0x0005E0FFUL);
#endif /* CPU_IN_SECURE_STATE */
#define __HAL_RCC_AHB4_RELEASE_RESET()     WRITE_REG(RCC->AHB4RSTSR, 0UL)

#define __HAL_RCC_CRC_FORCE_RESET()        LL_AHB4_GRP1_ForceReset(LL_AHB4_GRP1_PERIPH_CRC)
#define __HAL_RCC_CRC_RELEASE_RESET()      LL_AHB4_GRP1_ReleaseReset(LL_AHB4_GRP1_PERIPH_CRC)

#define __HAL_RCC_GPIOA_FORCE_RESET()      LL_AHB4_GRP1_ForceReset(LL_AHB4_GRP1_PERIPH_GPIOA)
#define __HAL_RCC_GPIOA_RELEASE_RESET()    LL_AHB4_GRP1_ReleaseReset(LL_AHB4_GRP1_PERIPH_GPIOA)

#define __HAL_RCC_GPIOB_FORCE_RESET()      LL_AHB4_GRP1_ForceReset(LL_AHB4_GRP1_PERIPH_GPIOB)
#define __HAL_RCC_GPIOB_RELEASE_RESET()    LL_AHB4_GRP1_ReleaseReset(LL_AHB4_GRP1_PERIPH_GPIOB)

#define __HAL_RCC_GPIOC_FORCE_RESET()      LL_AHB4_GRP1_ForceReset(LL_AHB4_GRP1_PERIPH_GPIOC)
#define __HAL_RCC_GPIOC_RELEASE_RESET()    LL_AHB4_GRP1_ReleaseReset(LL_AHB4_GRP1_PERIPH_GPIOC)

#define __HAL_RCC_GPIOD_FORCE_RESET()      LL_AHB4_GRP1_ForceReset(LL_AHB4_GRP1_PERIPH_GPIOD)
#define __HAL_RCC_GPIOD_RELEASE_RESET()    LL_AHB4_GRP1_ReleaseReset(LL_AHB4_GRP1_PERIPH_GPIOD)

#define __HAL_RCC_GPIOE_FORCE_RESET()      LL_AHB4_GRP1_ForceReset(LL_AHB4_GRP1_PERIPH_GPIOE)
#define __HAL_RCC_GPIOE_RELEASE_RESET()    LL_AHB4_GRP1_ReleaseReset(LL_AHB4_GRP1_PERIPH_GPIOE)

#define __HAL_RCC_GPIOF_FORCE_RESET()      LL_AHB4_GRP1_ForceReset(LL_AHB4_GRP1_PERIPH_GPIOF)
#define __HAL_RCC_GPIOF_RELEASE_RESET()    LL_AHB4_GRP1_ReleaseReset(LL_AHB4_GRP1_PERIPH_GPIOF)

#define __HAL_RCC_GPIOG_FORCE_RESET()      LL_AHB4_GRP1_ForceReset(LL_AHB4_GRP1_PERIPH_GPIOG)
#define __HAL_RCC_GPIOG_RELEASE_RESET()    LL_AHB4_GRP1_ReleaseReset(LL_AHB4_GRP1_PERIPH_GPIOG)

#define __HAL_RCC_GPIOH_FORCE_RESET()      LL_AHB4_GRP1_ForceReset(LL_AHB4_GRP1_PERIPH_GPIOH)
#define __HAL_RCC_GPIOH_RELEASE_RESET()    LL_AHB4_GRP1_ReleaseReset(LL_AHB4_GRP1_PERIPH_GPIOH)

#define __HAL_RCC_GPION_FORCE_RESET()      LL_AHB4_GRP1_ForceReset(LL_AHB4_GRP1_PERIPH_GPION)
#define __HAL_RCC_GPION_RELEASE_RESET()    LL_AHB4_GRP1_ReleaseReset(LL_AHB4_GRP1_PERIPH_GPION)

#define __HAL_RCC_GPIOO_FORCE_RESET()      LL_AHB4_GRP1_ForceReset(LL_AHB4_GRP1_PERIPH_GPIOO)
#define __HAL_RCC_GPIOO_RELEASE_RESET()    LL_AHB4_GRP1_ReleaseReset(LL_AHB4_GRP1_PERIPH_GPIOO)

#define __HAL_RCC_GPIOP_FORCE_RESET()      LL_AHB4_GRP1_ForceReset(LL_AHB4_GRP1_PERIPH_GPIOP)
#define __HAL_RCC_GPIOP_RELEASE_RESET()    LL_AHB4_GRP1_ReleaseReset(LL_AHB4_GRP1_PERIPH_GPIOP)

#define __HAL_RCC_GPIOQ_FORCE_RESET()      LL_AHB4_GRP1_ForceReset(LL_AHB4_GRP1_PERIPH_GPIOQ)
#define __HAL_RCC_GPIOQ_RELEASE_RESET()    LL_AHB4_GRP1_ReleaseReset(LL_AHB4_GRP1_PERIPH_GPIOQ)

#if defined (CPU_IN_SECURE_STATE)
#define __HAL_RCC_PWR_FORCE_RESET()        LL_AHB4_GRP1_ForceReset(LL_AHB4_GRP1_PERIPH_PWR)
#define __HAL_RCC_PWR_RELEASE_RESET()      LL_AHB4_GRP1_ReleaseReset(LL_AHB4_GRP1_PERIPH_PWR)
#endif /* CPU_IN_SECURE_STATE */

/**
  * @}
  */

/** @defgroup RCC_AHB5_Force_Release_Reset AHB5 Peripheral Force Release Reset
  * @brief  Force or release AHB5 peripheral reset.
  * @{
  */

/* Caution: The two following macros should only be called from code running in internal RAM
 *          since it resets the external peripheral memory interfaces.
 */
#define __HAL_RCC_AHB5_FORCE_RESET()       WRITE_REG(RCC->AHB5RSTSR, 0xFF9A31FBUL)
#define __HAL_RCC_AHB5_RELEASE_RESET()     WRITE_REG(RCC->AHB5RSTSR, 0UL)

#define __HAL_RCC_DMA2D_FORCE_RESET()      LL_AHB5_GRP1_ForceReset(LL_AHB5_GRP1_PERIPH_DMA2D)
#define __HAL_RCC_DMA2D_RELEASE_RESET()    LL_AHB5_GRP1_ReleaseReset(LL_AHB5_GRP1_PERIPH_DMA2D)

#if defined(ETH1)
#define __HAL_RCC_ETH1_FORCE_RESET()       LL_AHB5_GRP1_ForceReset(LL_AHB5_GRP1_PERIPH_ETH1);
#define __HAL_RCC_ETH1_RELEASE_RESET()     LL_AHB5_GRP1_ReleaseReset(LL_AHB5_GRP1_PERIPH_ETH1);
#endif /* ETH1 */

#define __HAL_RCC_FMC_FORCE_RESET()        LL_AHB5_GRP1_ForceReset(LL_AHB5_GRP1_PERIPH_FMC)
#define __HAL_RCC_FMC_RELEASE_RESET()      LL_AHB5_GRP1_ReleaseReset(LL_AHB5_GRP1_PERIPH_FMC)

#if defined(GFXMMU)
#define __HAL_RCC_GFXMMU_FORCE_RESET()     LL_AHB5_GRP1_ForceReset(LL_AHB5_GRP1_PERIPH_GFXMMU)
#define __HAL_RCC_GFXMMU_RELEASE_RESET()   LL_AHB5_GRP1_ReleaseReset(LL_AHB5_GRP1_PERIPH_GFXMMU)
#endif /* GFXMMU */

#if defined(GPU2D)
#define __HAL_RCC_GPU2D_FORCE_RESET()      LL_AHB5_GRP1_ForceReset(LL_AHB5_GRP1_PERIPH_GPU2D)
#define __HAL_RCC_GPU2D_RELEASE_RESET()    LL_AHB5_GRP1_ReleaseReset(LL_AHB5_GRP1_PERIPH_GPU2D)
#endif /* GPU2D */

#define __HAL_RCC_HPDMA1_FORCE_RESET()     LL_AHB5_GRP1_ForceReset(LL_AHB5_GRP1_PERIPH_HPDMA1)
#define __HAL_RCC_HPDMA1_RELEASE_RESET()   LL_AHB5_GRP1_ReleaseReset(LL_AHB5_GRP1_PERIPH_HPDMA1)

#define __HAL_RCC_XSPI1_FORCE_RESET()      LL_AHB5_GRP1_ForceReset(LL_AHB5_GRP1_PERIPH_XSPI1)
#define __HAL_RCC_XSPI1_RELEASE_RESET()    LL_AHB5_GRP1_ReleaseReset(LL_AHB5_GRP1_PERIPH_XSPI1)

#define __HAL_RCC_XSPI2_FORCE_RESET()      LL_AHB5_GRP1_ForceReset(LL_AHB5_GRP1_PERIPH_XSPI2)
#define __HAL_RCC_XSPI2_RELEASE_RESET()    LL_AHB5_GRP1_ReleaseReset(LL_AHB5_GRP1_PERIPH_XSPI2)

#define __HAL_RCC_XSPI3_FORCE_RESET()      LL_AHB5_GRP1_ForceReset(LL_AHB5_GRP1_PERIPH_XSPI3);
#define __HAL_RCC_XSPI3_RELEASE_RESET()    LL_AHB5_GRP1_ReleaseReset(LL_AHB5_GRP1_PERIPH_XSPI3);

#define __HAL_RCC_XSPIM_FORCE_RESET()      LL_AHB5_GRP1_ForceReset(LL_AHB5_GRP1_PERIPH_XSPIM)
#define __HAL_RCC_XSPIM_RELEASE_RESET()   LL_AHB5_GRP1_ReleaseReset(LL_AHB5_GRP1_PERIPH_XSPIM)

#if defined(JPEG)
#define __HAL_RCC_JPEG_FORCE_RESET()       LL_AHB5_GRP1_ForceReset(LL_AHB5_GRP1_PERIPH_JPEG)
#define __HAL_RCC_JPEG_RELEASE_RESET()     LL_AHB5_GRP1_ReleaseReset(LL_AHB5_GRP1_PERIPH_JPEG)
#endif /* JPEG */

#define __HAL_RCC_CACHEAXI_FORCE_RESET()   LL_AHB5_GRP1_ForceReset(LL_AHB5_GRP1_PERIPH_CACHEAXI);
#define __HAL_RCC_CACHEAXI_RELEASE_RESET() LL_AHB5_GRP1_ReleaseReset(LL_AHB5_GRP1_PERIPH_CACHEAXI);

#define __HAL_RCC_NPU_FORCE_RESET()        LL_AHB5_GRP1_ForceReset(LL_AHB5_GRP1_PERIPH_NPU);
#define __HAL_RCC_NPU_RELEASE_RESET()      LL_AHB5_GRP1_ReleaseReset(LL_AHB5_GRP1_PERIPH_NPU);

#define __HAL_RCC_DCMI_PSSI_FORCE_RESET()       LL_AHB5_GRP1_ForceReset(LL_AHB5_GRP1_PERIPH_PSSI)
#define __HAL_RCC_DCMI_PSSI_RELEASE_RESET()     LL_AHB5_GRP1_ReleaseReset(LL_AHB5_GRP1_PERIPH_PSSI)

#define __HAL_RCC_SDMMC1_FORCE_RESET()     LL_AHB5_GRP1_ForceReset(LL_AHB5_GRP1_PERIPH_SDMMC1)
#define __HAL_RCC_SDMMC1_RELEASE_RESET()   LL_AHB5_GRP1_ReleaseReset(LL_AHB5_GRP1_PERIPH_SDMMC1)

#define __HAL_RCC_SDMMC2_FORCE_RESET()     LL_AHB5_GRP1_ForceReset(LL_AHB5_GRP1_PERIPH_SDMMC2)
#define __HAL_RCC_SDMMC2_RELEASE_RESET()   LL_AHB5_GRP1_ReleaseReset(LL_AHB5_GRP1_PERIPH_SDMMC2)

#define __HAL_RCC_USB1_OTG_HS_FORCE_RESET()   LL_AHB5_GRP1_ForceReset(LL_AHB5_GRP1_PERIPH_OTG1)
#define __HAL_RCC_USB1_OTG_HS_RELEASE_RESET() LL_AHB5_GRP1_ReleaseReset(LL_AHB5_GRP1_PERIPH_OTG1)

#define __HAL_RCC_USB1_OTG_HS_PHY_FORCE_RESET()   LL_AHB5_GRP1_ForceReset(LL_AHB5_GRP1_PERIPH_OTGPHY1);
#define __HAL_RCC_USB1_OTG_HS_PHY_RELEASE_RESET() LL_AHB5_GRP1_ReleaseReset(LL_AHB5_GRP1_PERIPH_OTGPHY1);

#define __HAL_RCC_USB1_OTG_HS_CTL_FORCE_RESET()   LL_AHB5_GRP1_ForceReset(LL_AHB5_GRP1_PERIPH_OTG1PHYCTL);
#define __HAL_RCC_USB1_OTG_HS_CTL_RELEASE_RESET() LL_AHB5_GRP1_ReleaseReset(LL_AHB5_GRP1_PERIPH_OTG1PHYCTL);

#define __HAL_RCC_USB2_OTG_HS_FORCE_RESET()   LL_AHB5_GRP1_ForceReset(LL_AHB5_GRP1_PERIPH_OTG2)
#define __HAL_RCC_USB2_OTG_HS_RELEASE_RESET() LL_AHB5_GRP1_ReleaseReset(LL_AHB5_GRP1_PERIPH_OTG2)

#define __HAL_RCC_USB2_OTG_HS_PHY_FORCE_RESET()   LL_AHB5_GRP1_ForceReset(LL_AHB5_GRP1_PERIPH_OTGPHY2);
#define __HAL_RCC_USB2_OTG_HS_PHY_RELEASE_RESET() LL_AHB5_GRP1_ReleaseReset(LL_AHB5_GRP1_PERIPH_OTGPHY2);

#define __HAL_RCC_USB2_OTG_HS_CTL_FORCE_RESET()   LL_AHB5_GRP1_ForceReset(LL_AHB5_GRP1_PERIPH_OTG2PHYCTL);
#define __HAL_RCC_USB2_OTG_HS_CTL_RELEASE_RESET() LL_AHB5_GRP1_ReleaseReset(LL_AHB5_GRP1_PERIPH_OTG2PHYCTL);

/**
  * @}
  */

/** @defgroup RCC_APB1_Force_Release_Reset APB1 Peripheral Force Release Reset
  * @brief  Force or release APB1 peripheral reset.
  * @{
  */

#define __HAL_RCC_APB1_FORCE_RESET()     do { \
                                              WRITE_REG(RCC->APB1RSTSR1, 0xC3FFF3FFUL); \
                                              WRITE_REG(RCC->APB1RSTSR2, 0x00040120UL); \
                                            } while(0)
#define __HAL_RCC_APB1_RELEASE_RESET()   do { \
                                              WRITE_REG(RCC->APB1RSTSR1, 0UL); \
                                              WRITE_REG(RCC->APB1RSTSR2, 0UL); \
                                            } while(0)

#define __HAL_RCC_FDCAN_FORCE_RESET()    LL_APB1_GRP2_ForceReset(LL_APB1_GRP2_PERIPH_FDCAN)
#define __HAL_RCC_FDCAN_RELEASE_RESET()  LL_APB1_GRP2_ReleaseReset(LL_APB1_GRP2_PERIPH_FDCAN)

#define __HAL_RCC_I2C1_FORCE_RESET()     LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_I2C1)
#define __HAL_RCC_I2C1_RELEASE_RESET()   LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_I2C1)

#define __HAL_RCC_I2C2_FORCE_RESET()     LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_I2C2)
#define __HAL_RCC_I2C2_RELEASE_RESET()   LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_I2C2)

#define __HAL_RCC_I2C3_FORCE_RESET()     LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_I2C3)
#define __HAL_RCC_I2C3_RELEASE_RESET()   LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_I2C3)

#define __HAL_RCC_I3C1_FORCE_RESET()     LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_I3C1)
#define __HAL_RCC_I3C1_RELEASE_RESET()   LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_I3C1)

#define __HAL_RCC_I3C2_FORCE_RESET()     LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_I3C2)
#define __HAL_RCC_I3C2_RELEASE_RESET()   LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_I3C2)

#define __HAL_RCC_LPTIM1_FORCE_RESET()   LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_LPTIM1)
#define __HAL_RCC_LPTIM1_RELEASE_RESET() LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_LPTIM1)

#define __HAL_RCC_MDIOS_FORCE_RESET()    LL_APB1_GRP2_ForceReset(LL_APB1_GRP2_PERIPH_MDIOS)
#define __HAL_RCC_MDIOS_RELEASE_RESET()  LL_APB1_GRP2_ReleaseReset(LL_APB1_GRP2_PERIPH_MDIOS)

#define __HAL_RCC_SPDIFRX1_FORCE_RESET()   LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_SPDIFRX1)
#define __HAL_RCC_SPDIFRX1_RELEASE_RESET() LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_SPDIFRX1)

#define __HAL_RCC_SPI2_FORCE_RESET()     LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_SPI2)
#define __HAL_RCC_SPI2_RELEASE_RESET()   LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_SPI2)

#define __HAL_RCC_SPI3_FORCE_RESET()     LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_SPI3)
#define __HAL_RCC_SPI3_RELEASE_RESET()   LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_SPI3)

#define __HAL_RCC_TIM2_FORCE_RESET()     LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_TIM2)
#define __HAL_RCC_TIM2_RELEASE_RESET()   LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_TIM2)

#define __HAL_RCC_TIM3_FORCE_RESET()     LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_TIM3)
#define __HAL_RCC_TIM3_RELEASE_RESET()   LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_TIM3)

#define __HAL_RCC_TIM4_FORCE_RESET()     LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_TIM4)
#define __HAL_RCC_TIM4_RELEASE_RESET()   LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_TIM4)

#define __HAL_RCC_TIM5_FORCE_RESET()     LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_TIM5)
#define __HAL_RCC_TIM5_RELEASE_RESET()   LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_TIM5)

#define __HAL_RCC_TIM6_FORCE_RESET()     LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_TIM6)
#define __HAL_RCC_TIM6_RELEASE_RESET()   LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_TIM6)

#define __HAL_RCC_TIM7_FORCE_RESET()     LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_TIM7)
#define __HAL_RCC_TIM7_RELEASE_RESET()   LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_TIM7)

#define __HAL_RCC_TIM10_FORCE_RESET()    LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_TIM10)
#define __HAL_RCC_TIM10_RELEASE_RESET()  LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_TIM10)

#define __HAL_RCC_TIM11_FORCE_RESET()    LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_TIM11)
#define __HAL_RCC_TIM11_RELEASE_RESET()  LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_TIM11)

#define __HAL_RCC_TIM12_FORCE_RESET()    LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_TIM12)
#define __HAL_RCC_TIM12_RELEASE_RESET()  LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_TIM12)

#define __HAL_RCC_TIM13_FORCE_RESET()    LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_TIM13)
#define __HAL_RCC_TIM13_RELEASE_RESET()  LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_TIM13)

#define __HAL_RCC_TIM14_FORCE_RESET()    LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_TIM14)
#define __HAL_RCC_TIM14_RELEASE_RESET()  LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_TIM14)

#define __HAL_RCC_UCPD1_FORCE_RESET()    LL_APB1_GRP2_ForceReset(LL_APB1_GRP2_PERIPH_UCPD1)
#define __HAL_RCC_UCPD1_RELEASE_RESET()  LL_APB1_GRP2_ReleaseReset(LL_APB1_GRP2_PERIPH_UCPD1)

#define __HAL_RCC_USART2_FORCE_RESET()   LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_USART2)
#define __HAL_RCC_USART2_RELEASE_RESET() LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_USART2)

#define __HAL_RCC_USART3_FORCE_RESET()   LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_USART3)
#define __HAL_RCC_USART3_RELEASE_RESET() LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_USART3)

#define __HAL_RCC_UART4_FORCE_RESET()    LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_UART4)
#define __HAL_RCC_UART4_RELEASE_RESET()  LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_UART4)

#define __HAL_RCC_UART5_FORCE_RESET()    LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_UART5)
#define __HAL_RCC_UART5_RELEASE_RESET()  LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_UART5)

#define __HAL_RCC_UART7_FORCE_RESET()    LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_UART7)
#define __HAL_RCC_UART7_RELEASE_RESET()  LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_UART7)

#define __HAL_RCC_UART8_FORCE_RESET()    LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_UART8)
#define __HAL_RCC_UART8_RELEASE_RESET()  LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_UART8)

/**
  * @}
  */

/** @defgroup RCC_APB2_Force_Release_Reset APB2 Peripheral Force Release Reset
  * @brief  Force or release APB2 peripheral reset.
  * @{
  */

#define __HAL_RCC_APB2_FORCE_RESET()      WRITE_REG(RCC->APB2RSTSR, 0x007FB0F3UL)
#define __HAL_RCC_APB2_RELEASE_RESET()    WRITE_REG(RCC->APB2RSTSR, 0UL)

#define __HAL_RCC_SAI1_FORCE_RESET()      LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_SAI1)
#define __HAL_RCC_SAI1_RELEASE_RESET()    LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_SAI1)

#define __HAL_RCC_SAI2_FORCE_RESET()      LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_SAI2)
#define __HAL_RCC_SAI2_RELEASE_RESET()    LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_SAI2)

#define __HAL_RCC_SPI1_FORCE_RESET()      LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_SPI1)
#define __HAL_RCC_SPI1_RELEASE_RESET()    LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_SPI1)

#define __HAL_RCC_SPI4_FORCE_RESET()      LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_SPI4)
#define __HAL_RCC_SPI4_RELEASE_RESET()    LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_SPI4)

#define __HAL_RCC_SPI5_FORCE_RESET()      LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_SPI5)
#define __HAL_RCC_SPI5_RELEASE_RESET()    LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_SPI5)

#define __HAL_RCC_TIM1_FORCE_RESET()      LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_TIM1)
#define __HAL_RCC_TIM1_RELEASE_RESET()    LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_TIM1)

#define __HAL_RCC_TIM8_FORCE_RESET()      LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_TIM8)
#define __HAL_RCC_TIM8_RELEASE_RESET()    LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_TIM8)

#define __HAL_RCC_TIM9_FORCE_RESET()      LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_TIM9)
#define __HAL_RCC_TIM9_RELEASE_RESET()    LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_TIM9)

#define __HAL_RCC_TIM15_FORCE_RESET()     LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_TIM15)
#define __HAL_RCC_TIM15_RELEASE_RESET()   LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_TIM15)

#define __HAL_RCC_TIM16_FORCE_RESET()     LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_TIM16)
#define __HAL_RCC_TIM16_RELEASE_RESET()   LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_TIM16)

#define __HAL_RCC_TIM17_FORCE_RESET()     LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_TIM17)
#define __HAL_RCC_TIM17_RELEASE_RESET()   LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_TIM17)

#define __HAL_RCC_TIM18_FORCE_RESET()     LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_TIM18)
#define __HAL_RCC_TIM18_RELEASE_RESET()   LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_TIM18)

#define __HAL_RCC_USART1_FORCE_RESET()    LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_USART1)
#define __HAL_RCC_USART1_RELEASE_RESET()  LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_USART1)

#define __HAL_RCC_USART6_FORCE_RESET()    LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_USART6)
#define __HAL_RCC_USART6_RELEASE_RESET()  LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_USART6)

#define __HAL_RCC_UART9_FORCE_RESET()     LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_UART9)
#define __HAL_RCC_UART9_RELEASE_RESET()   LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_UART9)

#define __HAL_RCC_USART10_FORCE_RESET()   LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_USART10)
#define __HAL_RCC_USART10_RELEASE_RESET() LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_USART10)

/**
  * @}
  */

/** @defgroup RCC_APB4_Force_Release_Reset APB4 Peripheral Force Release Reset
  * @brief  Force or release APB4 peripheral reset.
  * @{
  */

#define __HAL_RCC_APB4_FORCE_RESET()      do { \
                                               WRITE_REG(RCC->APB4RSTSR1, 0x00019EACUL); \
                                               WRITE_REG(RCC->APB4RSTSR2, 0x00000005UL); \
                                             } while(0)
#define __HAL_RCC_APB4_RELEASE_RESET()    do { \
                                               WRITE_REG(RCC->APB4RSTSR1, 0UL); \
                                               WRITE_REG(RCC->APB4RSTSR2, 0UL); \
                                             } while(0)

#define __HAL_RCC_DTS_FORCE_RESET()       LL_APB4_GRP2_ForceReset(LL_APB4_GRP2_PERIPH_DTS)
#define __HAL_RCC_DTS_RELEASE_RESET()     LL_APB4_GRP2_ReleaseReset(LL_APB4_GRP2_PERIPH_DTS)

#define __HAL_RCC_HDP_FORCE_RESET()       LL_APB4_GRP1_ForceReset(LL_APB4_GRP1_PERIPH_HDP)
#define __HAL_RCC_HDP_RELEASE_RESET()     LL_APB4_GRP1_ReleaseReset(LL_APB4_GRP1_PERIPH_HDP)

#define __HAL_RCC_I2C4_FORCE_RESET()      LL_APB4_GRP1_ForceReset(LL_APB4_GRP1_PERIPH_I2C4)
#define __HAL_RCC_I2C4_RELEASE_RESET()    LL_APB4_GRP1_ReleaseReset(LL_APB4_GRP1_PERIPH_I2C4)

#define __HAL_RCC_LPTIM2_FORCE_RESET()    LL_APB4_GRP1_ForceReset(LL_APB4_GRP1_PERIPH_LPTIM2)
#define __HAL_RCC_LPTIM2_RELEASE_RESET()  LL_APB4_GRP1_ReleaseReset(LL_APB4_GRP1_PERIPH_LPTIM2)

#define __HAL_RCC_LPTIM3_FORCE_RESET()    LL_APB4_GRP1_ForceReset(LL_APB4_GRP1_PERIPH_LPTIM3)
#define __HAL_RCC_LPTIM3_RELEASE_RESET()  LL_APB4_GRP1_ReleaseReset(LL_APB4_GRP1_PERIPH_LPTIM3)

#define __HAL_RCC_LPTIM4_FORCE_RESET()    LL_APB4_GRP1_ForceReset(LL_APB4_GRP1_PERIPH_LPTIM4)
#define __HAL_RCC_LPTIM4_RELEASE_RESET()  LL_APB4_GRP1_ReleaseReset(LL_APB4_GRP1_PERIPH_LPTIM4)

#define __HAL_RCC_LPTIM5_FORCE_RESET()    LL_APB4_GRP1_ForceReset(LL_APB4_GRP1_PERIPH_LPTIM5)
#define __HAL_RCC_LPTIM5_RELEASE_RESET()  LL_APB4_GRP1_ReleaseReset(LL_APB4_GRP1_PERIPH_LPTIM5)

#define __HAL_RCC_LPUART1_FORCE_RESET()   LL_APB4_GRP1_ForceReset(LL_APB4_GRP1_PERIPH_LPUART1)
#define __HAL_RCC_LPUART1_RELEASE_RESET() LL_APB4_GRP1_ReleaseReset(LL_APB4_GRP1_PERIPH_LPUART1)

#define __HAL_RCC_RTC_FORCE_RESET()       LL_APB4_GRP1_ForceReset(LL_APB4_GRP1_PERIPH_RTC)
#define __HAL_RCC_RTC_RELEASE_RESET()     LL_APB4_GRP1_ReleaseReset(LL_APB4_GRP1_PERIPH_RTC)

#define __HAL_RCC_SPI6_FORCE_RESET()      LL_APB4_GRP1_ForceReset(LL_APB4_GRP1_PERIPH_SPI6)
#define __HAL_RCC_SPI6_RELEASE_RESET()    LL_APB4_GRP1_ReleaseReset(LL_APB4_GRP1_PERIPH_SPI6)

#define __HAL_RCC_SYSCFG_FORCE_RESET()    LL_APB4_GRP2_ForceReset(LL_APB4_GRP2_PERIPH_SYSCFG)
#define __HAL_RCC_SYSCFG_RELEASE_RESET()  LL_APB4_GRP2_ReleaseReset(LL_APB4_GRP2_PERIPH_SYSCFG)

#define __HAL_RCC_VREFBUF_FORCE_RESET()   LL_APB4_GRP1_ForceReset(LL_APB4_GRP1_PERIPH_VREFBUF)
#define __HAL_RCC_VREFBUF_RELEASE_RESET() LL_APB4_GRP1_ReleaseReset(LL_APB4_GRP1_PERIPH_VREFBUF)

/**
  * @}
  */

/** @defgroup RCC_APB5_Force_Release_Reset APB5 Peripheral Force Release Reset
  * @brief  Force or release APB5 peripheral reset.
  * @{
  */

#define __HAL_RCC_APB5_FORCE_RESET()      WRITE_REG(RCC->APB5RSTSR, 0x00000076UL)
#define __HAL_RCC_APB5_RELEASE_RESET()    WRITE_REG(RCC->APB5RSTSR, 0UL)

#define __HAL_RCC_CSI_FORCE_RESET()       LL_APB5_GRP1_ForceReset(LL_APB5_GRP1_PERIPH_CSI)
#define __HAL_RCC_CSI_RELEASE_RESET()     LL_APB5_GRP1_ReleaseReset(LL_APB5_GRP1_PERIPH_CSI)

#define __HAL_RCC_DCMIPP_FORCE_RESET()    LL_APB5_GRP1_ForceReset(LL_APB5_GRP1_PERIPH_DCMIPP)
#define __HAL_RCC_DCMIPP_RELEASE_RESET()  LL_APB5_GRP1_ReleaseReset(LL_APB5_GRP1_PERIPH_DCMIPP)

#define __HAL_RCC_GFXTIM_FORCE_RESET()    LL_APB5_GRP1_ForceReset(LL_APB5_GRP1_PERIPH_GFXTIM)
#define __HAL_RCC_GFXTIM_RELEASE_RESET()  LL_APB5_GRP1_ReleaseReset(LL_APB5_GRP1_PERIPH_GFXTIM)

#define __HAL_RCC_LTDC_FORCE_RESET()      LL_APB5_GRP1_ForceReset(LL_APB5_GRP1_PERIPH_LTDC)
#define __HAL_RCC_LTDC_RELEASE_RESET()    LL_APB5_GRP1_ReleaseReset(LL_APB5_GRP1_PERIPH_LTDC)

#define __HAL_RCC_VENC_FORCE_RESET()     LL_APB5_GRP1_ForceReset(LL_APB5_GRP1_PERIPH_VENC)
#define __HAL_RCC_VENC_RELEASE_RESET()   LL_APB5_GRP1_ReleaseReset(LL_APB5_GRP1_PERIPH_VENC)

/**
  * @}
  */

/** @defgroup RCC_MISC_Configuration_Force_Release_Reset Misc Configuration Force Release Reset
  * @brief  Force or release misc configuration reset.
  * @note   DBG reset is always security-protected and thus hidden to the non-secure application.
  * @{
  */

#if defined (CPU_IN_SECURE_STATE)
#define __HAL_RCC_DBG_FORCE_RESET()         LL_MISC_ForceReset(LL_DBG)
#define __HAL_RCC_DBG_RELEASE_RESET()       LL_MISC_ReleaseReset(LL_DBG)
#endif /* CPU_IN_SECURE_STATE */

#define __HAL_RCC_XSPIPHY1_FORCE_RESET()    LL_MISC_ForceReset(LL_XSPIPHY1)
#define __HAL_RCC_XSPIPHY1_RELEASE_RESET()  LL_MISC_ReleaseReset(LL_XSPIPHY1)

#define __HAL_RCC_XSPIPHY2_FORCE_RESET()    LL_MISC_ForceReset(LL_XSPIPHY2)
#define __HAL_RCC_XSPIPHY2_RELEASE_RESET()  LL_MISC_ReleaseReset(LL_XSPIPHY2)

#define __HAL_RCC_SDMMC1DLL_FORCE_RESET()   LL_MISC_ForceReset(LL_SDMMC1DLL)
#define __HAL_RCC_SDMMC1DLL_RELEASE_RESET() LL_MISC_ReleaseReset(LL_SDMMC1DLL)

#define __HAL_RCC_SDMMC2DLL_FORCE_RESET()   LL_MISC_ForceReset(LL_SDMMC2DLL)
#define __HAL_RCC_SDMMC2DLL_RELEASE_RESET() LL_MISC_ReleaseReset(LL_SDMMC2DLL)

/**
  * @}
  */


/** @defgroup RCC_Embedded_Mem_Clock_Sleep_Enable_Disable Embedded memory Clock Sleep Enable Disable
  * @brief  Enable or disable the embedded memory clock during Low Power (Sleep) mode.
  * @note   Embedded memory clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   By default, all embedded memory clocks are disabled during SLEEP mode.
  * @{
  */
#define __HAL_RCC_AXISRAM1_MEM_CLK_SLEEP_ENABLE()     LL_MEM_EnableClockLowPower(LL_MEM_AXISRAM1)
#define __HAL_RCC_AXISRAM1_MEM_CLK_SLEEP_DISABLE()    LL_MEM_DisableClockLowPower(LL_MEM_AXISRAM1)

#define __HAL_RCC_AXISRAM2_MEM_CLK_SLEEP_ENABLE()     LL_MEM_EnableClockLowPower(LL_MEM_AXISRAM2)
#define __HAL_RCC_AXISRAM2_MEM_CLK_SLEEP_DISABLE()    LL_MEM_DisableClockLowPower(LL_MEM_AXISRAM2)

#define __HAL_RCC_AXISRAM3_MEM_CLK_SLEEP_ENABLE()     LL_MEM_EnableClockLowPower(LL_MEM_AXISRAM3)
#define __HAL_RCC_AXISRAM3_MEM_CLK_SLEEP_DISABLE()    LL_MEM_DisableClockLowPower(LL_MEM_AXISRAM3)

#define __HAL_RCC_AXISRAM4_MEM_CLK_SLEEP_ENABLE()     LL_MEM_EnableClockLowPower(LL_MEM_AXISRAM4)
#define __HAL_RCC_AXISRAM4_MEM_CLK_SLEEP_DISABLE()    LL_MEM_DisableClockLowPower(LL_MEM_AXISRAM4)

#define __HAL_RCC_AXISRAM5_MEM_CLK_SLEEP_ENABLE()     LL_MEM_EnableClockLowPower(LL_MEM_AXISRAM5)
#define __HAL_RCC_AXISRAM5_MEM_CLK_SLEEP_DISABLE()    LL_MEM_DisableClockLowPower(LL_MEM_AXISRAM5)

#define __HAL_RCC_AXISRAM6_MEM_CLK_SLEEP_ENABLE()     LL_MEM_EnableClockLowPower(LL_MEM_AXISRAM6)
#define __HAL_RCC_AXISRAM6_MEM_CLK_SLEEP_DISABLE()    LL_MEM_DisableClockLowPower(LL_MEM_AXISRAM6)

#define __HAL_RCC_AHBSRAM1_MEM_CLK_SLEEP_ENABLE()     LL_MEM_EnableClockLowPower(LL_MEM_AHBSRAM1)
#define __HAL_RCC_AHBSRAM1_MEM_CLK_SLEEP_DISABLE()    LL_MEM_DisableClockLowPower(LL_MEM_AHBSRAM1)

#define __HAL_RCC_AHBSRAM2_MEM_CLK_SLEEP_ENABLE()     LL_MEM_EnableClockLowPower(LL_MEM_AHBSRAM2)
#define __HAL_RCC_AHBSRAM2_MEM_CLK_SLEEP_DISABLE()    LL_MEM_DisableClockLowPower(LL_MEM_AHBSRAM2)

#define __HAL_RCC_BKPSRAM_MEM_CLK_SLEEP_ENABLE()      LL_MEM_EnableClockLowPower(LL_MEM_BKPSRAM)
#define __HAL_RCC_BKPSRAM_MEM_CLK_SLEEP_DISABLE()     LL_MEM_DisableClockLowPower(LL_MEM_BKPSRAM)

#define __HAL_RCC_FLEXRAM_MEM_CLK_SLEEP_ENABLE()      LL_MEM_EnableClockLowPower(LL_MEM_FLEXRAM)
#define __HAL_RCC_FLEXRAM_MEM_CLK_SLEEP_DISABLE()     LL_MEM_DisableClockLowPower(LL_MEM_FLEXRAM)

#define __HAL_RCC_CACHEAXIRAM_MEM_CLK_SLEEP_ENABLE()  LL_MEM_EnableClockLowPower(LL_MEM_CACHEAXIRAM)
#define __HAL_RCC_CACHEAXIRAM_MEM_CLK_SLEEP_DISABLE() LL_MEM_DisableClockLowPower(LL_MEM_CACHEAXIRAM)

#define __HAL_RCC_VENCRAM_MEM_CLK_SLEEP_ENABLE()      LL_MEM_EnableClockLowPower(LL_MEM_VENCRAM)
#define __HAL_RCC_VENCRAM_MEM_CLK_SLEEP_DISABLE()     LL_MEM_DisableClockLowPower(LL_MEM_VENCRAM)

/**
  * @}
  */

/** @defgroup RCC_AHB1_Clock_Sleep_Enable_Disable AHB1 Peripheral Clock Sleep Enable Disable
  * @brief  Enable or disable the AHB1 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   By default, all peripheral clocks are disabled during SLEEP mode.
  * @{
  */

#define __HAL_RCC_ADC12_CLK_SLEEP_ENABLE()   LL_AHB1_GRP1_EnableClockLowPower(LL_AHB1_GRP1_PERIPH_ADC12)
#define __HAL_RCC_ADC12_CLK_SLEEP_DISABLE()  LL_AHB1_GRP1_DisableClockLowPower(LL_AHB1_GRP1_PERIPH_ADC12)

#define __HAL_RCC_GPDMA1_CLK_SLEEP_ENABLE()  LL_AHB1_GRP1_EnableClockLowPower(LL_AHB1_GRP1_PERIPH_GPDMA1)
#define __HAL_RCC_GPDMA1_CLK_SLEEP_DISABLE() LL_AHB1_GRP1_DisableClockLowPower(LL_AHB1_GRP1_PERIPH_GPDMA1)

/**
  * @}
  */

/** @defgroup RCC_AHB2_Clock_Sleep_Enable_Disable AHB2 Peripheral Clock Sleep Enable Disable
  * @brief  Enable or disable the AHB2 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   By default, all peripheral clocks are disabled during SLEEP mode.
  * @{
  */

#define __HAL_RCC_ADF1_CLK_SLEEP_ENABLE()    LL_AHB2_GRP1_EnableClockLowPower(LL_AHB2_GRP1_PERIPH_ADF1)
#define __HAL_RCC_ADF1_CLK_SLEEP_DISABLE()   LL_AHB2_GRP1_DisableClockLowPower(LL_AHB2_GRP1_PERIPH_ADF1)

#define __HAL_RCC_MDF1_CLK_SLEEP_ENABLE()    LL_AHB2_GRP1_EnableClockLowPower(LL_AHB2_GRP1_PERIPH_MDF1)
#define __HAL_RCC_MDF1_CLK_SLEEP_DISABLE()   LL_AHB2_GRP1_DisableClockLowPower(LL_AHB2_GRP1_PERIPH_MDF1)

#define __HAL_RCC_RAMCFG_CLK_SLEEP_ENABLE()  LL_AHB2_GRP1_EnableClockLowPower(LL_AHB2_GRP1_PERIPH_RAMCFG)
#define __HAL_RCC_RAMCFG_CLK_SLEEP_DISABLE() LL_AHB2_GRP1_DisableClockLowPower(LL_AHB2_GRP1_PERIPH_RAMCFG)

/**
  * @}
  */

/** @defgroup RCC_AHB3_Clock_Sleep_Enable_Disable AHB3 Peripheral Clock Sleep Enable Disable
  * @brief  Enable or disable the AHB3 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   By default, all peripheral clocks except IAC are disabled during SLEEP mode.
  * @note   IAC, RIFSC and RIFAF peripheral clocks are always security-protected and thus hidden
  *         to the non-secure application.
  * @{
  */

#if defined(CRYP)
#define __HAL_RCC_CRYP_CLK_SLEEP_ENABLE()    LL_AHB3_GRP1_EnableClockLowPower(LL_AHB3_GRP1_PERIPH_CRYP)
#define __HAL_RCC_CRYP_CLK_SLEEP_DISABLE()   LL_AHB3_GRP1_DisableClockLowPower(LL_AHB3_GRP1_PERIPH_CRYP)
#endif /* CRYP */

#define __HAL_RCC_HASH_CLK_SLEEP_ENABLE()    LL_AHB3_GRP1_EnableClockLowPower(LL_AHB3_GRP1_PERIPH_HASH)
#define __HAL_RCC_HASH_CLK_SLEEP_DISABLE()   LL_AHB3_GRP1_DisableClockLowPower(LL_AHB3_GRP1_PERIPH_HASH)

#if defined (CPU_IN_SECURE_STATE)
#define __HAL_RCC_IAC_CLK_SLEEP_ENABLE()     LL_AHB3_GRP1_EnableClockLowPower(LL_AHB3_GRP1_PERIPH_IAC)
#define __HAL_RCC_IAC_CLK_SLEEP_DISABLE()    LL_AHB3_GRP1_DisableClockLowPower(LL_AHB3_GRP1_PERIPH_IAC)
#endif /* CPU_IN_SECURE_STATE */

#define __HAL_RCC_PKA_CLK_SLEEP_ENABLE()     LL_AHB3_GRP1_EnableClockLowPower(LL_AHB3_GRP1_PERIPH_PKA)
#define __HAL_RCC_PKA_CLK_SLEEP_DISABLE()    LL_AHB3_GRP1_DisableClockLowPower(LL_AHB3_GRP1_PERIPH_PKA)

#if defined (CPU_IN_SECURE_STATE)
#define __HAL_RCC_RIFSC_CLK_SLEEP_ENABLE()   LL_AHB3_GRP1_EnableClockLowPower(LL_AHB3_GRP1_PERIPH_RIFSC)
#define __HAL_RCC_RIFSC_CLK_SLEEP_DISABLE()  LL_AHB3_GRP1_DisableClockLowPower(LL_AHB3_GRP1_PERIPH_RIFSC)
#endif /* CPU_IN_SECURE_STATE */

#if defined (CPU_IN_SECURE_STATE)
#define __HAL_RCC_RISAF_CLK_SLEEP_ENABLE()   LL_AHB3_GRP1_EnableClockLowPower(LL_AHB3_GRP1_PERIPH_RISAF)
#define __HAL_RCC_RISAF_CLK_SLEEP_DISABLE()  LL_AHB3_GRP1_DisableClockLowPower(LL_AHB3_GRP1_PERIPH_RISAF)
#endif /* CPU_IN_SECURE_STATE */

#define __HAL_RCC_RNG_CLK_SLEEP_ENABLE()     LL_AHB3_GRP1_EnableClockLowPower(LL_AHB3_GRP1_PERIPH_RNG)
#define __HAL_RCC_RNG_CLK_SLEEP_DISABLE()    LL_AHB3_GRP1_DisableClockLowPower(LL_AHB3_GRP1_PERIPH_RNG)

#if defined(SAES)
#define __HAL_RCC_SAES_CLK_SLEEP_ENABLE()    LL_AHB3_GRP1_EnableClockLowPower(LL_AHB3_GRP1_PERIPH_SAES)
#define __HAL_RCC_SAES_CLK_SLEEP_DISABLE()   LL_AHB3_GRP1_DisableClockLowPower(LL_AHB3_GRP1_PERIPH_SAES)
#endif /* SAES */

/**
  * @}
  */

/** @defgroup RCC_AHB4_Clock_Sleep_Enable_Disable AHB4 Peripheral Clock Sleep Enable Disable
  * @brief  Enable or disable the AHB4 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   By default, all peripheral clocks except PWR are disabled during SLEEP mode.
  * @note   PWR is always security-protected and thus hidden for non-secure application
  * @{
  */

#define __HAL_RCC_CRC_CLK_SLEEP_ENABLE()     LL_AHB4_GRP1_EnableClockLowPower(LL_AHB4_GRP1_PERIPH_CRC)
#define __HAL_RCC_CRC_CLK_SLEEP_DISABLE()    LL_AHB4_GRP1_DisableClockLowPower(LL_AHB4_GRP1_PERIPH_CRC)

#define __HAL_RCC_GPIOA_CLK_SLEEP_ENABLE()   LL_AHB4_GRP1_EnableClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOA)
#define __HAL_RCC_GPIOA_CLK_SLEEP_DISABLE()  LL_AHB4_GRP1_DisableClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOA)

#define __HAL_RCC_GPIOB_CLK_SLEEP_ENABLE()   LL_AHB4_GRP1_EnableClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOB)
#define __HAL_RCC_GPIOB_CLK_SLEEP_DISABLE()  LL_AHB4_GRP1_DisableClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOB)

#define __HAL_RCC_GPIOC_CLK_SLEEP_ENABLE()   LL_AHB4_GRP1_EnableClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOC)
#define __HAL_RCC_GPIOC_CLK_SLEEP_DISABLE()  LL_AHB4_GRP1_DisableClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOC)

#define __HAL_RCC_GPIOD_CLK_SLEEP_ENABLE()   LL_AHB4_GRP1_EnableClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOD)
#define __HAL_RCC_GPIOD_CLK_SLEEP_DISABLE()  LL_AHB4_GRP1_DisableClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOD)

#define __HAL_RCC_GPIOE_CLK_SLEEP_ENABLE()   LL_AHB4_GRP1_EnableClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOE)
#define __HAL_RCC_GPIOE_CLK_SLEEP_DISABLE()  LL_AHB4_GRP1_DisableClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOE)

#define __HAL_RCC_GPIOF_CLK_SLEEP_ENABLE()   LL_AHB4_GRP1_EnableClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOF)
#define __HAL_RCC_GPIOF_CLK_SLEEP_DISABLE()  LL_AHB4_GRP1_DisableClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOF)

#define __HAL_RCC_GPIOG_CLK_SLEEP_ENABLE()   LL_AHB4_GRP1_EnableClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOG)
#define __HAL_RCC_GPIOG_CLK_SLEEP_DISABLE()  LL_AHB4_GRP1_DisableClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOG)

#define __HAL_RCC_GPIOH_CLK_SLEEP_ENABLE()   LL_AHB4_GRP1_EnableClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOH)
#define __HAL_RCC_GPIOH_CLK_SLEEP_DISABLE()  LL_AHB4_GRP1_DisableClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOH)

#define __HAL_RCC_GPION_CLK_SLEEP_ENABLE()   LL_AHB4_GRP1_EnableClockLowPower(LL_AHB4_GRP1_PERIPH_GPION)
#define __HAL_RCC_GPION_CLK_SLEEP_DISABLE()  LL_AHB4_GRP1_DisableClockLowPower(LL_AHB4_GRP1_PERIPH_GPION)

#define __HAL_RCC_GPIOO_CLK_SLEEP_ENABLE()   LL_AHB4_GRP1_EnableClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOO)
#define __HAL_RCC_GPIOO_CLK_SLEEP_DISABLE()  LL_AHB4_GRP1_DisableClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOO)

#define __HAL_RCC_GPIOP_CLK_SLEEP_ENABLE()   LL_AHB4_GRP1_EnableClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOP)
#define __HAL_RCC_GPIOP_CLK_SLEEP_DISABLE()  LL_AHB4_GRP1_DisableClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOP)

#define __HAL_RCC_GPIOQ_CLK_SLEEP_ENABLE()   LL_AHB4_GRP1_EnableClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOQ)
#define __HAL_RCC_GPIOQ_CLK_SLEEP_DISABLE()  LL_AHB4_GRP1_DisableClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOQ)

#if defined (CPU_IN_SECURE_STATE)
#define __HAL_RCC_PWR_CLK_SLEEP_ENABLE()     LL_AHB4_GRP1_EnableClockLowPower(LL_AHB4_GRP1_PERIPH_PWR)
#define __HAL_RCC_PWR_CLK_SLEEP_DISABLE()    LL_AHB4_GRP1_DisableClockLowPower(LL_AHB4_GRP1_PERIPH_PWR)
#endif /* CPU_IN_SECURE_STATE */

/**
  * @}
  */

/** @defgroup RCC_AHB5_Clock_Sleep_Enable_Disable AHB5 Peripheral Clock Sleep Enable Disable
  * @brief  Enable or disable the AHB1 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   By default, all peripheral clocks are disabled during SLEEP mode.
  * @{
  */

#define __HAL_RCC_DMA2D_CLK_SLEEP_ENABLE()   LL_AHB5_GRP1_EnableClockLowPower(LL_AHB5_GRP1_PERIPH_DMA2D)
#define __HAL_RCC_DMA2D_CLK_SLEEP_DISABLE()  LL_AHB5_GRP1_DisableClockLowPower(LL_AHB5_GRP1_PERIPH_DMA2D)

#if defined(ETH1)
#define __HAL_RCC_ETH1_CLK_SLEEP_ENABLE()    LL_AHB5_GRP1_EnableClockLowPower(LL_AHB5_GRP1_PERIPH_ETH1)
#define __HAL_RCC_ETH1_CLK_SLEEP_DISABLE()   LL_AHB5_GRP1_DisableClockLowPower(LL_AHB5_GRP1_PERIPH_ETH1)

#define __HAL_RCC_ETH1MAC_CLK_SLEEP_ENABLE()  LL_AHB5_GRP1_EnableClockLowPower(LL_AHB5_GRP1_PERIPH_ETH1MAC)
#define __HAL_RCC_ETH1MAC_CLK_SLEEP_DISABLE() LL_AHB5_GRP1_DisableClockLowPower(LL_AHB5_GRP1_PERIPH_ETH1MAC)

#define __HAL_RCC_ETH1TX_CLK_SLEEP_ENABLE()  LL_AHB5_GRP1_EnableClockLowPower(LL_AHB5_GRP1_PERIPH_ETH1TX)
#define __HAL_RCC_ETH1TX_CLK_SLEEP_DISABLE() LL_AHB5_GRP1_DisableClockLowPower(LL_AHB5_GRP1_PERIPH_ETH1TX)

#define __HAL_RCC_ETH1RX_CLK_SLEEP_ENABLE()  LL_AHB5_GRP1_EnableClockLowPower(LL_AHB5_GRP1_PERIPH_ETH1RX)
#define __HAL_RCC_ETH1RX_CLK_SLEEP_DISABLE() LL_AHB5_GRP1_DisableClockLowPower(LL_AHB5_GRP1_PERIPH_ETH1RX)
#endif /* ETH1 */

#define __HAL_RCC_FMC_CLK_SLEEP_ENABLE()     LL_AHB5_GRP1_EnableClockLowPower(LL_AHB5_GRP1_PERIPH_FMC)
#define __HAL_RCC_FMC_CLK_SLEEP_DISABLE()    LL_AHB5_GRP1_DisableClockLowPower(LL_AHB5_GRP1_PERIPH_FMC)

#if defined(GFXMMU)
#define __HAL_RCC_GFXMMU_CLK_SLEEP_ENABLE()  LL_AHB5_GRP1_EnableClockLowPower(LL_AHB5_GRP1_PERIPH_GFXMMU)
#define __HAL_RCC_GFXMMU_CLK_SLEEP_DISABLE() LL_AHB5_GRP1_DisableClockLowPower(LL_AHB5_GRP1_PERIPH_GFXMMU)
#endif /* GFXMMU */

#if defined(GPU2D)
#define __HAL_RCC_GPU2D_CLK_SLEEP_ENABLE()   LL_AHB5_GRP1_EnableClockLowPower(LL_AHB5_GRP1_PERIPH_GPU2D)
#define __HAL_RCC_GPU2D_CLK_SLEEP_DISABLE()  LL_AHB5_GRP1_DisableClockLowPower(LL_AHB5_GRP1_PERIPH_GPU2D)
#endif /* GPU2D */

#define __HAL_RCC_HPDMA1_CLK_SLEEP_ENABLE()  LL_AHB5_GRP1_EnableClockLowPower(LL_AHB5_GRP1_PERIPH_HPDMA1)
#define __HAL_RCC_HPDMA1_CLK_SLEEP_DISABLE() LL_AHB5_GRP1_DisableClockLowPower(LL_AHB5_GRP1_PERIPH_HPDMA1)

#if defined(JPEG)
#define __HAL_RCC_JPEG_CLK_SLEEP_ENABLE()    LL_AHB5_GRP1_EnableClockLowPower(LL_AHB5_GRP1_PERIPH_JPEG)
#define __HAL_RCC_JPEG_CLK_SLEEP_DISABLE()   LL_AHB5_GRP1_DisableClockLowPower(LL_AHB5_GRP1_PERIPH_JPEG)
#endif /* JPEG */

#define __HAL_RCC_XSPI1_CLK_SLEEP_ENABLE()   LL_AHB5_GRP1_EnableClockLowPower(LL_AHB5_GRP1_PERIPH_XSPI1)
#define __HAL_RCC_XSPI1_CLK_SLEEP_DISABLE()  LL_AHB5_GRP1_DisableClockLowPower(LL_AHB5_GRP1_PERIPH_XSPI1)

#define __HAL_RCC_XSPI2_CLK_SLEEP_ENABLE()   LL_AHB5_GRP1_EnableClockLowPower(LL_AHB5_GRP1_PERIPH_XSPI2)
#define __HAL_RCC_XSPI2_CLK_SLEEP_DISABLE()  LL_AHB5_GRP1_DisableClockLowPower(LL_AHB5_GRP1_PERIPH_XSPI2)

#define __HAL_RCC_XSPI3_CLK_SLEEP_ENABLE()   LL_AHB5_GRP1_EnableClockLowPower(LL_AHB5_GRP1_PERIPH_XSPI3)
#define __HAL_RCC_XSPI3_CLK_SLEEP_DISABLE()  LL_AHB5_GRP1_DisableClockLowPower(LL_AHB5_GRP1_PERIPH_XSPI3)

#define __HAL_RCC_XSPIM_CLK_SLEEP_ENABLE()   LL_AHB5_GRP1_EnableClockLowPower(LL_AHB5_GRP1_PERIPH_XSPIM)
#define __HAL_RCC_XSPIM_CLK_SLEEP_DISABLE()  LL_AHB5_GRP1_DisableClockLowPower(LL_AHB5_GRP1_PERIPH_XSPIM)

#define __HAL_RCC_MCE1_CLK_SLEEP_ENABLE()    LL_AHB5_GRP1_EnableClockLowPower(LL_AHB5_GRP1_PERIPH_MCE1)
#define __HAL_RCC_MCE1_CLK_SLEEP_DISABLE()   LL_AHB5_GRP1_DisableClockLowPower(LL_AHB5_GRP1_PERIPH_MCE1)

#define __HAL_RCC_MCE2_CLK_SLEEP_ENABLE()    LL_AHB5_GRP1_EnableClockLowPower(LL_AHB5_GRP1_PERIPH_MCE2)
#define __HAL_RCC_MCE2_CLK_SLEEP_DISABLE()   LL_AHB5_GRP1_DisableClockLowPower(LL_AHB5_GRP1_PERIPH_MCE2)

#define __HAL_RCC_MCE3_CLK_SLEEP_ENABLE()    LL_AHB5_GRP1_EnableClockLowPower(LL_AHB5_GRP1_PERIPH_MCE3)
#define __HAL_RCC_MCE3_CLK_SLEEP_DISABLE()   LL_AHB5_GRP1_DisableClockLowPower(LL_AHB5_GRP1_PERIPH_MCE3)

#define __HAL_RCC_MCE4_CLK_SLEEP_ENABLE()    LL_AHB5_GRP1_EnableClockLowPower(LL_AHB5_GRP1_PERIPH_MCE4)
#define __HAL_RCC_MCE4_CLK_SLEEP_DISABLE()   LL_AHB5_GRP1_DisableClockLowPower(LL_AHB5_GRP1_PERIPH_MCE4)

#define __HAL_RCC_CACHEAXI_CLK_SLEEP_ENABLE()  LL_AHB5_GRP1_EnableClockLowPower(LL_AHB5_GRP1_PERIPH_CACHEAXI)
#define __HAL_RCC_CACHEAXI_CLK_SLEEP_DISABLE() LL_AHB5_GRP1_DisableClockLowPower(LL_AHB5_GRP1_PERIPH_CACHEAXI)

#define __HAL_RCC_NPU_CLK_SLEEP_ENABLE()     LL_AHB5_GRP1_EnableClockLowPower(LL_AHB5_GRP1_PERIPH_NPU)
#define __HAL_RCC_NPU_CLK_SLEEP_DISABLE()    LL_AHB5_GRP1_DisableClockLowPower(LL_AHB5_GRP1_PERIPH_NPU)

#define __HAL_RCC_DCMI_PSSI_CLK_SLEEP_ENABLE()    LL_AHB5_GRP1_EnableClockLowPower(LL_AHB5_GRP1_PERIPH_PSSI)
#define __HAL_RCC_DCMI_PSSI_CLK_SLEEP_DISABLE()   LL_AHB5_GRP1_DisableClockLowPower(LL_AHB5_GRP1_PERIPH_PSSI)

#define __HAL_RCC_SDMMC1_CLK_SLEEP_ENABLE()  LL_AHB5_GRP1_EnableClockLowPower(LL_AHB5_GRP1_PERIPH_SDMMC1)
#define __HAL_RCC_SDMMC1_CLK_SLEEP_DISABLE() LL_AHB5_GRP1_DisableClockLowPower(LL_AHB5_GRP1_PERIPH_SDMMC1)

#define __HAL_RCC_SDMMC2_CLK_SLEEP_ENABLE()  LL_AHB5_GRP1_EnableClockLowPower(LL_AHB5_GRP1_PERIPH_SDMMC2)
#define __HAL_RCC_SDMMC2_CLK_SLEEP_DISABLE() LL_AHB5_GRP1_DisableClockLowPower(LL_AHB5_GRP1_PERIPH_SDMMC2)

#define __HAL_RCC_USB1_OTG_HS_CLK_SLEEP_ENABLE()  LL_AHB5_GRP1_EnableClockLowPower(LL_AHB5_GRP1_PERIPH_OTG1)
#define __HAL_RCC_USB1_OTG_HS_CLK_SLEEP_DISABLE() LL_AHB5_GRP1_DisableClockLowPower(LL_AHB5_GRP1_PERIPH_OTG1)

#define __HAL_RCC_USB1_OTG_HS_PHY_CLK_SLEEP_ENABLE()  LL_AHB5_GRP1_EnableClockLowPower(LL_AHB5_GRP1_PERIPH_OTGPHY1)
#define __HAL_RCC_USB1_OTG_HS_PHY_CLK_SLEEP_DISABLE() LL_AHB5_GRP1_DisableClockLowPower(LL_AHB5_GRP1_PERIPH_OTGPHY1)

#define __HAL_RCC_USB2_OTG_HS_CLK_SLEEP_ENABLE()  LL_AHB5_GRP1_EnableClockLowPower(LL_AHB5_GRP1_PERIPH_OTG2)
#define __HAL_RCC_USB2_OTG_HS_CLK_SLEEP_DISABLE() LL_AHB5_GRP1_DisableClockLowPower(LL_AHB5_GRP1_PERIPH_OTG2)

#define __HAL_RCC_USB2_OTG_HS_PHY_CLK_SLEEP_ENABLE()  LL_AHB5_GRP1_EnableClockLowPower(LL_AHB5_GRP1_PERIPH_OTGPHY2)
#define __HAL_RCC_USB2_OTG_HS_PHY_CLK_SLEEP_DISABLE() LL_AHB5_GRP1_DisableClockLowPower(LL_AHB5_GRP1_PERIPH_OTGPHY2)

/**
  * @}
  */

/** @defgroup RCC_APB1_Clock_Sleep_Enable_Disable APB1 Peripheral Clock Sleep Enable Disable
  * @brief  Enable or disable the APB1 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   By default, all peripheral clocks are disabled during SLEEP mode.
  * @{
  */

#define __HAL_RCC_FDCAN_CLK_SLEEP_ENABLE()  LL_APB1_GRP2_EnableClockLowPower(LL_APB1_GRP2_PERIPH_FDCAN)
#define __HAL_RCC_FDCAN_CLK_SLEEP_DISABLE() LL_APB1_GRP2_DisableClockLowPower(LL_APB1_GRP2_PERIPH_FDCAN)

#define __HAL_RCC_I2C1_CLK_SLEEP_ENABLE()   LL_APB1_GRP1_EnableClockLowPower(LL_APB1_GRP1_PERIPH_I2C1)
#define __HAL_RCC_I2C1_CLK_SLEEP_DISABLE()  LL_APB1_GRP1_DisableClockLowPower(LL_APB1_GRP1_PERIPH_I2C1)

#define __HAL_RCC_I2C2_CLK_SLEEP_ENABLE()   LL_APB1_GRP1_EnableClockLowPower(LL_APB1_GRP1_PERIPH_I2C2)
#define __HAL_RCC_I2C2_CLK_SLEEP_DISABLE()  LL_APB1_GRP1_DisableClockLowPower(LL_APB1_GRP1_PERIPH_I2C2)

#define __HAL_RCC_I2C3_CLK_SLEEP_ENABLE()   LL_APB1_GRP1_EnableClockLowPower(LL_APB1_GRP1_PERIPH_I2C3)
#define __HAL_RCC_I2C3_CLK_SLEEP_DISABLE()  LL_APB1_GRP1_DisableClockLowPower(LL_APB1_GRP1_PERIPH_I2C3)

#define __HAL_RCC_I3C1_CLK_SLEEP_ENABLE()   LL_APB1_GRP1_EnableClockLowPower(LL_APB1_GRP1_PERIPH_I3C1)
#define __HAL_RCC_I3C1_CLK_SLEEP_DISABLE()  LL_APB1_GRP1_DisableClockLowPower(LL_APB1_GRP1_PERIPH_I3C1)

#define __HAL_RCC_I3C2_CLK_SLEEP_ENABLE()   LL_APB1_GRP1_EnableClockLowPower(LL_APB1_GRP1_PERIPH_I3C2)
#define __HAL_RCC_I3C2_CLK_SLEEP_DISABLE()  LL_APB1_GRP1_DisableClockLowPower(LL_APB1_GRP1_PERIPH_I3C2)

#define __HAL_RCC_LPTIM1_CLK_SLEEP_ENABLE()  LL_APB1_GRP1_EnableClockLowPower(LL_APB1_GRP1_PERIPH_LPTIM1)
#define __HAL_RCC_LPTIM1_CLK_SLEEP_DISABLE() LL_APB1_GRP1_DisableClockLowPower(LL_APB1_GRP1_PERIPH_LPTIM1)

#define __HAL_RCC_MDIOS_CLK_SLEEP_ENABLE()  LL_APB1_GRP2_EnableClockLowPower(LL_APB1_GRP2_PERIPH_MDIOS)
#define __HAL_RCC_MDIOS_CLK_SLEEP_DISABLE() LL_APB1_GRP2_DisableClockLowPower(LL_APB1_GRP2_PERIPH_MDIOS)

#define __HAL_RCC_SPDIFRX1_CLK_SLEEP_ENABLE()  LL_APB1_GRP1_EnableClockLowPower(LL_APB1_GRP1_PERIPH_SPDIFRX1)
#define __HAL_RCC_SPDIFRX1_CLK_SLEEP_DISABLE() LL_APB1_GRP1_DisableClockLowPower(LL_APB1_GRP1_PERIPH_SPDIFRX1)

#define __HAL_RCC_SPI2_CLK_SLEEP_ENABLE()   LL_APB1_GRP1_EnableClockLowPower(LL_APB1_GRP1_PERIPH_SPI2)
#define __HAL_RCC_SPI2_CLK_SLEEP_DISABLE()  LL_APB1_GRP1_DisableClockLowPower(LL_APB1_GRP1_PERIPH_SPI2)

#define __HAL_RCC_SPI3_CLK_SLEEP_ENABLE()   LL_APB1_GRP1_EnableClockLowPower(LL_APB1_GRP1_PERIPH_SPI3)
#define __HAL_RCC_SPI3_CLK_SLEEP_DISABLE()  LL_APB1_GRP1_DisableClockLowPower(LL_APB1_GRP1_PERIPH_SPI3)

#define __HAL_RCC_TIM2_CLK_SLEEP_ENABLE()   LL_APB1_GRP1_EnableClockLowPower(LL_APB1_GRP1_PERIPH_TIM2)
#define __HAL_RCC_TIM2_CLK_SLEEP_DISABLE()  LL_APB1_GRP1_DisableClockLowPower(LL_APB1_GRP1_PERIPH_TIM2)

#define __HAL_RCC_TIM3_CLK_SLEEP_ENABLE()   LL_APB1_GRP1_EnableClockLowPower(LL_APB1_GRP1_PERIPH_TIM3)
#define __HAL_RCC_TIM3_CLK_SLEEP_DISABLE()  LL_APB1_GRP1_DisableClockLowPower(LL_APB1_GRP1_PERIPH_TIM3)

#define __HAL_RCC_TIM4_CLK_SLEEP_ENABLE()   LL_APB1_GRP1_EnableClockLowPower(LL_APB1_GRP1_PERIPH_TIM4)
#define __HAL_RCC_TIM4_CLK_SLEEP_DISABLE()  LL_APB1_GRP1_DisableClockLowPower(LL_APB1_GRP1_PERIPH_TIM4)

#define __HAL_RCC_TIM5_CLK_SLEEP_ENABLE()   LL_APB1_GRP1_EnableClockLowPower(LL_APB1_GRP1_PERIPH_TIM5)
#define __HAL_RCC_TIM5_CLK_SLEEP_DISABLE()  LL_APB1_GRP1_DisableClockLowPower(LL_APB1_GRP1_PERIPH_TIM5)

#define __HAL_RCC_TIM6_CLK_SLEEP_ENABLE()   LL_APB1_GRP1_EnableClockLowPower(LL_APB1_GRP1_PERIPH_TIM6)
#define __HAL_RCC_TIM6_CLK_SLEEP_DISABLE()  LL_APB1_GRP1_DisableClockLowPower(LL_APB1_GRP1_PERIPH_TIM6)

#define __HAL_RCC_TIM7_CLK_SLEEP_ENABLE()   LL_APB1_GRP1_EnableClockLowPower(LL_APB1_GRP1_PERIPH_TIM7)
#define __HAL_RCC_TIM7_CLK_SLEEP_DISABLE()  LL_APB1_GRP1_DisableClockLowPower(LL_APB1_GRP1_PERIPH_TIM7)

#define __HAL_RCC_TIM10_CLK_SLEEP_ENABLE()  LL_APB1_GRP1_EnableClockLowPower(LL_APB1_GRP1_PERIPH_TIM10)
#define __HAL_RCC_TIM10_CLK_SLEEP_DISABLE() LL_APB1_GRP1_DisableClockLowPower(LL_APB1_GRP1_PERIPH_TIM10)

#define __HAL_RCC_TIM11_CLK_SLEEP_ENABLE()  LL_APB1_GRP1_EnableClockLowPower(LL_APB1_GRP1_PERIPH_TIM11)
#define __HAL_RCC_TIM11_CLK_SLEEP_DISABLE() LL_APB1_GRP1_DisableClockLowPower(LL_APB1_GRP1_PERIPH_TIM11)

#define __HAL_RCC_TIM12_CLK_SLEEP_ENABLE()  LL_APB1_GRP1_EnableClockLowPower(LL_APB1_GRP1_PERIPH_TIM12)
#define __HAL_RCC_TIM12_CLK_SLEEP_DISABLE() LL_APB1_GRP1_DisableClockLowPower(LL_APB1_GRP1_PERIPH_TIM12)

#define __HAL_RCC_TIM13_CLK_SLEEP_ENABLE()  LL_APB1_GRP1_EnableClockLowPower(LL_APB1_GRP1_PERIPH_TIM13)
#define __HAL_RCC_TIM13_CLK_SLEEP_DISABLE() LL_APB1_GRP1_DisableClockLowPower(LL_APB1_GRP1_PERIPH_TIM13)

#define __HAL_RCC_TIM14_CLK_SLEEP_ENABLE()  LL_APB1_GRP1_EnableClockLowPower(LL_APB1_GRP1_PERIPH_TIM14)
#define __HAL_RCC_TIM14_CLK_SLEEP_DISABLE() LL_APB1_GRP1_DisableClockLowPower(LL_APB1_GRP1_PERIPH_TIM14)

#define __HAL_RCC_USART2_CLK_SLEEP_ENABLE()  LL_APB1_GRP1_EnableClockLowPower(LL_APB1_GRP1_PERIPH_USART2)
#define __HAL_RCC_USART2_CLK_SLEEP_DISABLE() LL_APB1_GRP1_DisableClockLowPower(LL_APB1_GRP1_PERIPH_USART2)

#define __HAL_RCC_USART3_CLK_SLEEP_ENABLE()  LL_APB1_GRP1_EnableClockLowPower(LL_APB1_GRP1_PERIPH_USART3)
#define __HAL_RCC_USART3_CLK_SLEEP_DISABLE() LL_APB1_GRP1_DisableClockLowPower(LL_APB1_GRP1_PERIPH_USART3)

#define __HAL_RCC_UART4_CLK_SLEEP_ENABLE()  LL_APB1_GRP1_EnableClockLowPower(LL_APB1_GRP1_PERIPH_UART4)
#define __HAL_RCC_UART4_CLK_SLEEP_DISABLE() LL_APB1_GRP1_DisableClockLowPower(LL_APB1_GRP1_PERIPH_UART4)

#define __HAL_RCC_UART5_CLK_SLEEP_ENABLE()  LL_APB1_GRP1_EnableClockLowPower(LL_APB1_GRP1_PERIPH_UART5)
#define __HAL_RCC_UART5_CLK_SLEEP_DISABLE() LL_APB1_GRP1_DisableClockLowPower(LL_APB1_GRP1_PERIPH_UART5)

#define __HAL_RCC_UART7_CLK_SLEEP_ENABLE()  LL_APB1_GRP1_EnableClockLowPower(LL_APB1_GRP1_PERIPH_UART7)
#define __HAL_RCC_UART7_CLK_SLEEP_DISABLE() LL_APB1_GRP1_DisableClockLowPower(LL_APB1_GRP1_PERIPH_UART7)

#define __HAL_RCC_UART8_CLK_SLEEP_ENABLE()  LL_APB1_GRP1_EnableClockLowPower(LL_APB1_GRP1_PERIPH_UART8)
#define __HAL_RCC_UART8_CLK_SLEEP_DISABLE() LL_APB1_GRP1_DisableClockLowPower(LL_APB1_GRP1_PERIPH_UART8)

#define __HAL_RCC_UCPD1_CLK_SLEEP_ENABLE()  LL_APB1_GRP2_EnableClockLowPower(LL_APB1_GRP2_PERIPH_UCPD1)
#define __HAL_RCC_UCPD1_CLK_SLEEP_DISABLE() LL_APB1_GRP2_DisableClockLowPower(LL_APB1_GRP2_PERIPH_UCPD1)

#define __HAL_RCC_WWDG_CLK_SLEEP_ENABLE()   LL_APB1_GRP1_EnableClockLowPower(LL_APB1_GRP1_PERIPH_WWDG)
#define __HAL_RCC_WWDG_CLK_SLEEP_DISABLE()  LL_APB1_GRP1_DisableClockLowPower(LL_APB1_GRP1_PERIPH_WWDG)

/**
  * @}
  */

/** @defgroup RCC_APB2_Clock_Sleep_Enable_Disable APB2 Peripheral Clock Sleep Enable Disable
  * @brief  Enable or disable the APB2 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   By default, all peripheral clocks are disabled during SLEEP mode.
  * @{
  */

#define __HAL_RCC_SAI1_CLK_SLEEP_ENABLE()    LL_APB2_GRP1_EnableClockLowPower(LL_APB2_GRP1_PERIPH_SAI1)
#define __HAL_RCC_SAI1_CLK_SLEEP_DISABLE()   LL_APB2_GRP1_DisableClockLowPower(LL_APB2_GRP1_PERIPH_SAI1)

#define __HAL_RCC_SAI2_CLK_SLEEP_ENABLE()    LL_APB2_GRP1_EnableClockLowPower(LL_APB2_GRP1_PERIPH_SAI2)
#define __HAL_RCC_SAI2_CLK_SLEEP_DISABLE()   LL_APB2_GRP1_DisableClockLowPower(LL_APB2_GRP1_PERIPH_SAI2)

#define __HAL_RCC_SPI1_CLK_SLEEP_ENABLE()    LL_APB2_GRP1_EnableClockLowPower(LL_APB2_GRP1_PERIPH_SPI1)
#define __HAL_RCC_SPI1_CLK_SLEEP_DISABLE()   LL_APB2_GRP1_DisableClockLowPower(LL_APB2_GRP1_PERIPH_SPI1)

#define __HAL_RCC_SPI4_CLK_SLEEP_ENABLE()    LL_APB2_GRP1_EnableClockLowPower(LL_APB2_GRP1_PERIPH_SPI4)
#define __HAL_RCC_SPI4_CLK_SLEEP_DISABLE()   LL_APB2_GRP1_DisableClockLowPower(LL_APB2_GRP1_PERIPH_SPI4)

#define __HAL_RCC_SPI5_CLK_SLEEP_ENABLE()    LL_APB2_GRP1_EnableClockLowPower(LL_APB2_GRP1_PERIPH_SPI5)
#define __HAL_RCC_SPI5_CLK_SLEEP_DISABLE()   LL_APB2_GRP1_DisableClockLowPower(LL_APB2_GRP1_PERIPH_SPI5)

#define __HAL_RCC_TIM1_CLK_SLEEP_ENABLE()    LL_APB2_GRP1_EnableClockLowPower(LL_APB2_GRP1_PERIPH_TIM1)
#define __HAL_RCC_TIM1_CLK_SLEEP_DISABLE()   LL_APB2_GRP1_DisableClockLowPower(LL_APB2_GRP1_PERIPH_TIM1)

#define __HAL_RCC_TIM8_CLK_SLEEP_ENABLE()    LL_APB2_GRP1_EnableClockLowPower(LL_APB2_GRP1_PERIPH_TIM8)
#define __HAL_RCC_TIM8_CLK_SLEEP_DISABLE()   LL_APB2_GRP1_DisableClockLowPower(LL_APB2_GRP1_PERIPH_TIM8)

#define __HAL_RCC_TIM9_CLK_SLEEP_ENABLE()    LL_APB2_GRP1_EnableClockLowPower(LL_APB2_GRP1_PERIPH_TIM9)
#define __HAL_RCC_TIM9_CLK_SLEEP_DISABLE()   LL_APB2_GRP1_DisableClockLowPower(LL_APB2_GRP1_PERIPH_TIM9)

#define __HAL_RCC_TIM15_CLK_SLEEP_ENABLE()   LL_APB2_GRP1_EnableClockLowPower(LL_APB2_GRP1_PERIPH_TIM15)
#define __HAL_RCC_TIM15_CLK_SLEEP_DISABLE()  LL_APB2_GRP1_DisableClockLowPower(LL_APB2_GRP1_PERIPH_TIM15)

#define __HAL_RCC_TIM16_CLK_SLEEP_ENABLE()   LL_APB2_GRP1_EnableClockLowPower(LL_APB2_GRP1_PERIPH_TIM16)
#define __HAL_RCC_TIM16_CLK_SLEEP_DISABLE()  LL_APB2_GRP1_DisableClockLowPower(LL_APB2_GRP1_PERIPH_TIM16)

#define __HAL_RCC_TIM17_CLK_SLEEP_ENABLE()   LL_APB2_GRP1_EnableClockLowPower(LL_APB2_GRP1_PERIPH_TIM17)
#define __HAL_RCC_TIM17_CLK_SLEEP_DISABLE()  LL_APB2_GRP1_DisableClockLowPower(LL_APB2_GRP1_PERIPH_TIM17)

#define __HAL_RCC_TIM18_CLK_SLEEP_ENABLE()   LL_APB2_GRP1_EnableClockLowPower(LL_APB2_GRP1_PERIPH_TIM18)
#define __HAL_RCC_TIM18_CLK_SLEEP_DISABLE()  LL_APB2_GRP1_DisableClockLowPower(LL_APB2_GRP1_PERIPH_TIM18)

#define __HAL_RCC_USART1_CLK_SLEEP_ENABLE()  LL_APB2_GRP1_EnableClockLowPower(LL_APB2_GRP1_PERIPH_USART1)
#define __HAL_RCC_USART1_CLK_SLEEP_DISABLE() LL_APB2_GRP1_DisableClockLowPower(LL_APB2_GRP1_PERIPH_USART1)

#define __HAL_RCC_USART6_CLK_SLEEP_ENABLE()  LL_APB2_GRP1_EnableClockLowPower(LL_APB2_GRP1_PERIPH_USART6)
#define __HAL_RCC_USART6_CLK_SLEEP_DISABLE() LL_APB2_GRP1_DisableClockLowPower(LL_APB2_GRP1_PERIPH_USART6)

#define __HAL_RCC_UART9_CLK_SLEEP_ENABLE()   LL_APB2_GRP1_EnableClockLowPower(LL_APB2_GRP1_PERIPH_UART9)
#define __HAL_RCC_UART9_CLK_SLEEP_DISABLE()  LL_APB2_GRP1_DisableClockLowPower(LL_APB2_GRP1_PERIPH_UART9)

#define __HAL_RCC_USART10_CLK_SLEEP_ENABLE()  LL_APB2_GRP1_EnableClockLowPower(LL_APB2_GRP1_PERIPH_USART10)
#define __HAL_RCC_USART10_CLK_SLEEP_DISABLE() LL_APB2_GRP1_DisableClockLowPower(LL_APB2_GRP1_PERIPH_USART10)

/**
  * @}
  */

/** @defgroup RCC_APB4_Clock_Sleep_Enable_Disable APB4 Peripheral Clock Sleep Enable Disable
  * @brief  Enable or disable the APB4 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   By default, all peripheral clocks except BSEC are disabled during SLEEP mode.
  * @note   BSEC peripheral clock is always security-protected and thus hidden to the non-secure
  *         application.
  * @{
  */

#if defined (CPU_IN_SECURE_STATE)
#define __HAL_RCC_BSEC_CLK_SLEEP_ENABLE()     LL_APB4_GRP2_EnableClockLowPower(LL_APB4_GRP2_PERIPH_BSEC)
#define __HAL_RCC_BSEC_CLK_SLEEP_DISABLE()    LL_APB4_GRP2_DisableClockLowPower(LL_APB4_GRP2_PERIPH_BSEC)
#endif /* CPU_IN_SECURE_STATE */

#define __HAL_RCC_DTS_CLK_SLEEP_ENABLE()      LL_APB4_GRP2_EnableClockLowPower(LL_APB4_GRP2_PERIPH_DTS)
#define __HAL_RCC_DTS_CLK_SLEEP_DISABLE()     LL_APB4_GRP2_DisableClockLowPower(LL_APB4_GRP2_PERIPH_DTS)

#define __HAL_RCC_HDP_CLK_SLEEP_ENABLE()      LL_APB4_GRP1_EnableClockLowPower(LL_APB4_GRP1_PERIPH_HDP)
#define __HAL_RCC_HDP_CLK_SLEEP_DISABLE()     LL_APB4_GRP1_DisableClockLowPower(LL_APB4_GRP1_PERIPH_HDP)

#define __HAL_RCC_I2C4_CLK_SLEEP_ENABLE()     LL_APB4_GRP1_EnableClockLowPower(LL_APB4_GRP1_PERIPH_I2C4)
#define __HAL_RCC_I2C4_CLK_SLEEP_DISABLE()    LL_APB4_GRP1_DisableClockLowPower(LL_APB4_GRP1_PERIPH_I2C4)

#define __HAL_RCC_LPTIM2_CLK_SLEEP_ENABLE()   LL_APB4_GRP1_EnableClockLowPower(LL_APB4_GRP1_PERIPH_LPTIM2)
#define __HAL_RCC_LPTIM2_CLK_SLEEP_DISABLE()  LL_APB4_GRP1_DisableClockLowPower(LL_APB4_GRP1_PERIPH_LPTIM2)

#define __HAL_RCC_LPTIM3_CLK_SLEEP_ENABLE()   LL_APB4_GRP1_EnableClockLowPower(LL_APB4_GRP1_PERIPH_LPTIM3)
#define __HAL_RCC_LPTIM3_CLK_SLEEP_DISABLE()  LL_APB4_GRP1_DisableClockLowPower(LL_APB4_GRP1_PERIPH_LPTIM3)

#define __HAL_RCC_LPTIM4_CLK_SLEEP_ENABLE()   LL_APB4_GRP1_EnableClockLowPower(LL_APB4_GRP1_PERIPH_LPTIM4)
#define __HAL_RCC_LPTIM4_CLK_SLEEP_DISABLE()  LL_APB4_GRP1_DisableClockLowPower(LL_APB4_GRP1_PERIPH_LPTIM4)

#define __HAL_RCC_LPTIM5_CLK_SLEEP_ENABLE()   LL_APB4_GRP1_EnableClockLowPower(LL_APB4_GRP1_PERIPH_LPTIM5)
#define __HAL_RCC_LPTIM5_CLK_SLEEP_DISABLE()  LL_APB4_GRP1_DisableClockLowPower(LL_APB4_GRP1_PERIPH_LPTIM5)

#define __HAL_RCC_LPUART1_CLK_SLEEP_ENABLE()  LL_APB4_GRP1_EnableClockLowPower(LL_APB4_GRP1_PERIPH_LPUART1)
#define __HAL_RCC_LPUART1_CLK_SLEEP_DISABLE() LL_APB4_GRP1_DisableClockLowPower(LL_APB4_GRP1_PERIPH_LPUART1)

#define __HAL_RCC_RTC_CLK_SLEEP_ENABLE()      LL_APB4_GRP1_EnableClockLowPower(LL_APB4_GRP1_PERIPH_RTC)
#define __HAL_RCC_RTC_CLK_SLEEP_DISABLE()     LL_APB4_GRP1_DisableClockLowPower(LL_APB4_GRP1_PERIPH_RTC)

#define __HAL_RCC_RTCAPB_CLK_SLEEP_ENABLE()   LL_APB4_GRP1_EnableClockLowPower(LL_APB4_GRP1_PERIPH_RTCAPB)
#define __HAL_RCC_RTCAPB_CLK_SLEEP_DISABLE()  LL_APB4_GRP1_DisableClockLowPower(LL_APB4_GRP1_PERIPH_RTCAPB)

#define __HAL_RCC_SPI6_CLK_SLEEP_ENABLE()     LL_APB4_GRP1_EnableClockLowPower(LL_APB4_GRP1_PERIPH_SPI6)
#define __HAL_RCC_SPI6_CLK_SLEEP_DISABLE()    LL_APB4_GRP1_DisableClockLowPower(LL_APB4_GRP1_PERIPH_SPI6)

#define __HAL_RCC_SYSCFG_CLK_SLEEP_ENABLE()   LL_APB4_GRP2_EnableClockLowPower(LL_APB4_GRP2_PERIPH_SYSCFG)
#define __HAL_RCC_SYSCFG_CLK_SLEEP_DISABLE()  LL_APB4_GRP2_DisableClockLowPower(LL_APB4_GRP2_PERIPH_SYSCFG)

#define __HAL_RCC_VREFBUF_CLK_SLEEP_ENABLE()  LL_APB4_GRP1_EnableClockLowPower(LL_APB4_GRP1_PERIPH_VREFBUF)
#define __HAL_RCC_VREFBUF_CLK_SLEEP_DISABLE() LL_APB4_GRP1_DisableClockLowPower(LL_APB4_GRP1_PERIPH_VREFBUF)

/**
  * @}
  */

/** @defgroup RCC_APB5_Clock_Sleep_Enable_Disable APB5 Peripheral Clock Sleep Enable Disable
  * @brief  Enable or disable the APB5 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   By default, all peripheral clocks are disabled during SLEEP mode.
  * @{
  */

#define __HAL_RCC_CSI_CLK_SLEEP_ENABLE()     LL_APB5_GRP1_EnableClockLowPower(LL_APB5_GRP1_PERIPH_CSI)
#define __HAL_RCC_CSI_CLK_SLEEP_DISABLE()    LL_APB5_GRP1_DisableClockLowPower(LL_APB5_GRP1_PERIPH_CSI)

#define __HAL_RCC_DCMIPP_CLK_SLEEP_ENABLE()  LL_APB5_GRP1_EnableClockLowPower(LL_APB5_GRP1_PERIPH_DCMIPP)
#define __HAL_RCC_DCMIPP_CLK_SLEEP_DISABLE() LL_APB5_GRP1_DisableClockLowPower(LL_APB5_GRP1_PERIPH_DCMIPP)

#define __HAL_RCC_GFXTIM_CLK_SLEEP_ENABLE()  LL_APB5_GRP1_EnableClockLowPower(LL_APB5_GRP1_PERIPH_GFXTIM)
#define __HAL_RCC_GFXTIM_CLK_SLEEP_DISABLE() LL_APB5_GRP1_DisableClockLowPower(LL_APB5_GRP1_PERIPH_GFXTIM)

#define __HAL_RCC_LTDC_CLK_SLEEP_ENABLE()    LL_APB5_GRP1_EnableClockLowPower(LL_APB5_GRP1_PERIPH_LTDC)
#define __HAL_RCC_LTDC_CLK_SLEEP_DISABLE()   LL_APB5_GRP1_DisableClockLowPower(LL_APB5_GRP1_PERIPH_LTDC)

#define __HAL_RCC_VENC_CLK_SLEEP_ENABLE()    LL_APB5_GRP1_EnableClockLowPower(LL_APB5_GRP1_PERIPH_VENC)
#define __HAL_RCC_VENC_CLK_SLEEP_DISABLE()   LL_APB5_GRP1_DisableClockLowPower(LL_APB5_GRP1_PERIPH_VENC)

/**
  * @}
  */

/** @defgroup RCC_MISC_Configuration_Clock_Sleep_Enable_Disable Misc Configuration Clock Sleep Enable Disable
  * @brief  Enable or disable the misc configuration clock during Low Power (Sleep) mode.
  * @note   After reset, the misc configuration clock is disabled and
  *         the application software has to enable this clock before using it.
  * @note   After wakeup from SLEEP mode, the misc configuration clock is enabled again.
  * @note   DBG clock is always security-protected and thus hidden the non-secure application.
  * @{
  */

#if defined (CPU_IN_SECURE_STATE)
#define __HAL_RCC_DBG_CLK_SLEEP_ENABLE()     LL_MISC_EnableClockLowPower(LL_DBG)
#define __HAL_RCC_DBG_CLK_SLEEP_DISABLE()    LL_MISC_DisableClockLowPower(LL_DBG)
#endif /* CPU_IN_SECURE_STATE */

#define __HAL_RCC_XSPIPHYCOMP_CLK_SLEEP_ENABLE()  LL_MISC_EnableClockLowPower(LL_XSPIPHYCOMP)
#define __HAL_RCC_XSPIPHYCOMP_CLK_SLEEP_DISABLE() LL_MISC_DisableClockLowPower(LL_XSPIPHYCOMP)

#define __HAL_RCC_PER_CLK_SLEEP_ENABLE()  LL_MISC_EnableClockLowPower(LL_PER)
#define __HAL_RCC_PER_CLK_SLEEP_DISABLE() LL_MISC_DisableClockLowPower(LL_PER)

/**
  * @}
  */

/** @defgroup RCC_Embedded_Mem_Clock_Sleep_Enable_Status Embedded Memory Clock Sleep Enabled Status
  * @brief  Check whether the embedded memory clock sleep is enabled or not.
  * @note   After reset, some embedded memory clocks are disabled
  *         and the application software has to enable these memory clocks before using them.
  * @{
  */

#define __HAL_RCC_AXISRAM1_MEM_IS_CLK_SLEEP_ENABLED()     LL_MEM_IsEnabledClockLowPower(LL_MEM_AXISRAM1)
#define __HAL_RCC_AXISRAM2_MEM_IS_CLK_SLEEP_ENABLED()     LL_MEM_IsEnabledClockLowPower(LL_MEM_AXISRAM2)
#define __HAL_RCC_AXISRAM3_MEM_IS_CLK_SLEEP_ENABLED()     LL_MEM_IsEnabledClockLowPower(LL_MEM_AXISRAM3)
#define __HAL_RCC_AXISRAM4_MEM_IS_CLK_SLEEP_ENABLED()     LL_MEM_IsEnabledClockLowPower(LL_MEM_AXISRAM4)
#define __HAL_RCC_AXISRAM5_MEM_IS_CLK_SLEEP_ENABLED()     LL_MEM_IsEnabledClockLowPower(LL_MEM_AXISRAM5)
#define __HAL_RCC_AXISRAM6_MEM_IS_CLK_SLEEP_ENABLED()     LL_MEM_IsEnabledClockLowPower(LL_MEM_AXISRAM6)
#define __HAL_RCC_AHBSRAM1_MEM_IS_CLK_SLEEP_ENABLED()     LL_MEM_IsEnabledClockLowPower(LL_MEM_AHBSRAM1)
#define __HAL_RCC_AHBSRAM2_MEM_IS_CLK_SLEEP_ENABLED()     LL_MEM_IsEnabledClockLowPower(LL_MEM_AHBSRAM2)
#define __HAL_RCC_BKPSRAM_MEM_IS_CLK_SLEEP_ENABLED()      LL_MEM_IsEnabledClockLowPower(LL_MEM_BKPSRAM)
#define __HAL_RCC_FLEXRAM_MEM_IS_CLK_SLEEP_ENABLED()      LL_MEM_IsEnabledClockLowPower(LL_MEM_FLEXRAM)
#define __HAL_RCC_CACHEAXIRAM_MEM_IS_CLK_SLEEP_ENABLED()  LL_MEM_IsEnabledClockLowPower(LL_MEM_CACHEAXIRAM)
#define __HAL_RCC_VENCRAM_MEM_IS_CLK_SLEEP_ENABLED()      LL_MEM_IsEnabledClockLowPower(LL_MEM_VENCRAM)

/**
  * @}
  */

/** @defgroup RCC_AHB1_Clock_Sleep_Enable_Status AHB1 Peripheral Clock Sleep Enabled Status
  * @brief  Check whether the AHB1 peripheral clock sleep is enabled or not.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_ADC12_IS_CLK_SLEEP_ENABLED()   LL_AHB1_GRP1_IsEnabledClockLowPower(LL_AHB1_GRP1_PERIPH_ADC12)

#define __HAL_RCC_GPDMA1_IS_CLK_SLEEP_ENABLED()  LL_AHB1_GRP1_IsEnabledClockLowPower(LL_AHB1_GRP1_PERIPH_GPDMA1)

/**
  * @}
  */

/** @defgroup RCC_AHB2_Clock_Sleep_Enable_Status AHB2 Peripheral Clock Sleep Enabled Status
  * @brief  Check whether the AHB2 peripheral clock sleep is enabled or not.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_ADF1_IS_CLK_SLEEP_ENABLED()    LL_AHB2_GRP1_IsEnabledClockLowPower(LL_AHB2_GRP1_PERIPH_ADF1)

#define __HAL_RCC_MDF1_IS_CLK_SLEEP_ENABLED()    LL_AHB2_GRP1_IsEnabledClockLowPower(LL_AHB2_GRP1_PERIPH_MDF1)

#define __HAL_RCC_RAMCFG_IS_CLK_SLEEP_ENABLED()  LL_AHB2_GRP1_IsEnabledClockLowPower(LL_AHB2_GRP1_PERIPH_RAMCFG)

/**
  * @}
  */

/** @defgroup RCC_AHB3_Clock_Sleep_Enable_Status AHB3 Peripheral Clock Sleep Enabled Status
  * @brief  Check whether the AHB3 peripheral clock sleep is enabled or not.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#if defined(CRYP)
#define __HAL_RCC_CRYP_IS_CLK_SLEEP_ENABLED()    LL_AHB3_GRP1_IsEnabledClockLowPower(LL_AHB3_GRP1_PERIPH_CRYP)
#endif /* CRYP */

#define __HAL_RCC_HASH_IS_CLK_SLEEP_ENABLED()    LL_AHB3_GRP1_IsEnabledClockLowPower(LL_AHB3_GRP1_PERIPH_HASH)

#define __HAL_RCC_IAC_IS_CLK_SLEEP_ENABLED()     LL_AHB3_GRP1_IsEnabledClockLowPower(LL_AHB3_GRP1_PERIPH_IAC)

#define __HAL_RCC_PKA_IS_CLK_SLEEP_ENABLED()     LL_AHB3_GRP1_IsEnabledClockLowPower(LL_AHB3_GRP1_PERIPH_PKA)

#define __HAL_RCC_RIFSC_IS_CLK_SLEEP_ENABLED()   LL_AHB3_GRP1_IsEnabledClockLowPower(LL_AHB3_GRP1_PERIPH_RIFSC)

#define __HAL_RCC_RISAF_IS_CLK_SLEEP_ENABLED()   LL_AHB3_GRP1_IsEnabledClockLowPower(LL_AHB3_GRP1_PERIPH_RISAF)

#define __HAL_RCC_RNG_IS_CLK_SLEEP_ENABLED()     LL_AHB3_GRP1_IsEnabledClockLowPower(LL_AHB3_GRP1_PERIPH_RNG)

#if defined(SAES)
#define __HAL_RCC_SAES_IS_CLK_SLEEP_ENABLED()    LL_AHB3_GRP1_IsEnabledClockLowPower(LL_AHB3_GRP1_PERIPH_SAES)
#endif /* SAES */

/**
  * @}
  */

/** @defgroup RCC_AHB4_Clock_Sleep_Enable_Status AHB4 Peripheral Clock Sleep Enabled Status
  * @brief  Check whether the AHB4 peripheral clock sleep is enabled or not.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_CRC_IS_CLK_SLEEP_ENABLED()     LL_AHB4_GRP1_IsEnabledClockLowPower(LL_AHB4_GRP1_PERIPH_CRC)

#define __HAL_RCC_GPIOA_IS_CLK_SLEEP_ENABLED()   LL_AHB4_GRP1_IsEnabledClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOA)

#define __HAL_RCC_GPIOB_IS_CLK_SLEEP_ENABLED()   LL_AHB4_GRP1_IsEnabledClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOB)

#define __HAL_RCC_GPIOC_IS_CLK_SLEEP_ENABLED()   LL_AHB4_GRP1_IsEnabledClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOC)

#define __HAL_RCC_GPIOD_IS_CLK_SLEEP_ENABLED()   LL_AHB4_GRP1_IsEnabledClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOD)

#define __HAL_RCC_GPIOE_IS_CLK_SLEEP_ENABLED()   LL_AHB4_GRP1_IsEnabledClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOE)

#define __HAL_RCC_GPIOF_IS_CLK_SLEEP_ENABLED()   LL_AHB4_GRP1_IsEnabledClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOF)

#define __HAL_RCC_GPIOG_IS_CLK_SLEEP_ENABLED()   LL_AHB4_GRP1_IsEnabledClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOG)

#define __HAL_RCC_GPIOH_IS_CLK_SLEEP_ENABLED()   LL_AHB4_GRP1_IsEnabledClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOH)

#define __HAL_RCC_GPION_IS_CLK_SLEEP_ENABLED()   LL_AHB4_GRP1_IsEnabledClockLowPower(LL_AHB4_GRP1_PERIPH_GPION)

#define __HAL_RCC_GPIOO_IS_CLK_SLEEP_ENABLED()   LL_AHB4_GRP1_IsEnabledClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOO)

#define __HAL_RCC_GPIOP_IS_CLK_SLEEP_ENABLED()   LL_AHB4_GRP1_IsEnabledClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOP)

#define __HAL_RCC_GPIOQ_IS_CLK_SLEEP_ENABLED()   LL_AHB4_GRP1_IsEnabledClockLowPower(LL_AHB4_GRP1_PERIPH_GPIOQ)

#define __HAL_RCC_PWR_IS_CLK_SLEEP_ENABLED()     LL_AHB4_GRP1_IsEnabledClockLowPower(LL_AHB4_GRP1_PERIPH_PWR)

/**
  * @}
  */

/** @defgroup RCC_AHB5_Clock_Sleep_Enable_Status AHB5 Peripheral Clock Sleep Enabled Status
  * @brief  Check whether the AHB5 peripheral clock sleep is enabled or not.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_DMA2D_IS_CLK_SLEEP_ENABLED()   LL_AHB5_GRP1_IsEnabledClockLowPower(LL_AHB5_GRP1_PERIPH_DMA2D)

#if defined(ETH1)
#define __HAL_RCC_ETH1_IS_CLK_SLEEP_ENABLED()    LL_AHB5_GRP1_IsEnabledClockLowPower(LL_AHB5_GRP1_PERIPH_ETH1)

#define __HAL_RCC_ETH1MAC_IS_CLK_SLEEP_ENABLED() LL_AHB5_GRP1_IsEnabledClockLowPower(LL_AHB5_GRP1_PERIPH_ETH1MAC)

#define __HAL_RCC_ETH1TX_IS_CLK_SLEEP_ENABLED()  LL_AHB5_GRP1_IsEnabledClockLowPower(LL_AHB5_GRP1_PERIPH_ETH1TX)

#define __HAL_RCC_ETH1RX_IS_CLK_SLEEP_ENABLED()  LL_AHB5_GRP1_IsEnabledClockLowPower(LL_AHB5_GRP1_PERIPH_ETH1RX)
#endif /* ETH1 */

#define __HAL_RCC_FMC_IS_CLK_SLEEP_ENABLED()     LL_AHB5_GRP1_IsEnabledClockLowPower(LL_AHB5_GRP1_PERIPH_FMC)

#if defined(GFXMMU)
#define __HAL_RCC_GFXMMU_IS_CLK_SLEEP_ENABLED()  LL_AHB5_GRP1_IsEnabledClockLowPower(LL_AHB5_GRP1_PERIPH_GFXMMU)
#endif /* GFXMMU */

#if defined(GPU2D)
#define __HAL_RCC_GPU2D_IS_CLK_SLEEP_ENABLED()   LL_AHB5_GRP1_IsEnabledClockLowPower(LL_AHB5_GRP1_PERIPH_GPU2D)
#endif /* GPU2D */

#define __HAL_RCC_HPDMA1_IS_CLK_SLEEP_ENABLED()  LL_AHB5_GRP1_IsEnabledClockLowPower(LL_AHB5_GRP1_PERIPH_HPDMA1)

#if defined(JPEG)
#define __HAL_RCC_JPEG_IS_CLK_SLEEP_ENABLED()    LL_AHB5_GRP1_IsEnabledClockLowPower(LL_AHB5_GRP1_PERIPH_JPEG)
#endif /* JPEG */

#define __HAL_RCC_XSPI1_IS_CLK_SLEEP_ENABLED()   LL_AHB5_GRP1_IsEnabledClockLowPower(LL_AHB5_GRP1_PERIPH_XSPI1)

#define __HAL_RCC_XSPI2_IS_CLK_SLEEP_ENABLED()   LL_AHB5_GRP1_IsEnabledClockLowPower(LL_AHB5_GRP1_PERIPH_XSPI2)

#define __HAL_RCC_XSPI3_IS_CLK_SLEEP_ENABLED()   LL_AHB5_GRP1_IsEnabledClockLowPower(LL_AHB5_GRP1_PERIPH_XSPI3)

#define __HAL_RCC_XSPIM_IS_CLK_SLEEP_ENABLED()   LL_AHB5_GRP1_IsEnabledClockLowPower(LL_AHB5_GRP1_PERIPH_XSPIM)

#define __HAL_RCC_MCE1_IS_CLK_SLEEP_ENABLED()    LL_AHB5_GRP1_IsEnabledClockLowPower(LL_AHB5_GRP1_PERIPH_MCE1)

#define __HAL_RCC_MCE2_IS_CLK_SLEEP_ENABLED()    LL_AHB5_GRP1_IsEnabledClockLowPower(LL_AHB5_GRP1_PERIPH_MCE2)

#define __HAL_RCC_MCE3_IS_CLK_SLEEP_ENABLED()    LL_AHB5_GRP1_IsEnabledClockLowPower(LL_AHB5_GRP1_PERIPH_MCE3)

#define __HAL_RCC_MCE4_IS_CLK_SLEEP_ENABLED()    LL_AHB5_GRP1_IsEnabledClockLowPower(LL_AHB5_GRP1_PERIPH_MCE4)

#define __HAL_RCC_CACHEAXI_IS_CLK_SLEEP_ENABLED()  LL_AHB5_GRP1_IsEnabledClockLowPower(LL_AHB5_GRP1_PERIPH_CACHEAXI)

#define __HAL_RCC_NPU_IS_CLK_SLEEP_ENABLED()     LL_AHB5_GRP1_IsEnabledClockLowPower(LL_AHB5_GRP1_PERIPH_NPU)

#define __HAL_RCC_DCMI_PSSI_IS_CLK_SLEEP_ENABLED()    LL_AHB5_GRP1_IsEnabledClockLowPower(LL_AHB5_GRP1_PERIPH_PSSI)

#define __HAL_RCC_SDMMC1_IS_CLK_SLEEP_ENABLED()  LL_AHB5_GRP1_IsEnabledClockLowPower(LL_AHB5_GRP1_PERIPH_SDMMC1)

#define __HAL_RCC_SDMMC2_IS_CLK_SLEEP_ENABLED()  LL_AHB5_GRP1_IsEnabledClockLowPower(LL_AHB5_GRP1_PERIPH_SDMMC2)

#define __HAL_RCC_USB1_OTG_HS_IS_CLK_SLEEP_ENABLED()  LL_AHB5_GRP1_IsEnabledClockLowPower(LL_AHB5_GRP1_PERIPH_OTG1)

#define __HAL_RCC_USB1_OTG_HS_PHY_IS_CLK_SLEEP_ENABLED()  LL_AHB5_GRP1_IsEnabledClockLowPower(LL_AHB5_GRP1_PERIPH_OTGPHY1)

#define __HAL_RCC_USB2_OTG_HS_IS_CLK_SLEEP_ENABLED()  LL_AHB5_GRP1_IsEnabledClockLowPower(LL_AHB5_GRP1_PERIPH_OTG2)

#define __HAL_RCC_USB2_OTG_HS_PHY_IS_CLK_SLEEP_ENABLED()  LL_AHB5_GRP1_IsEnabledClockLowPower(LL_AHB5_GRP1_PERIPH_OTGPHY2)

/**
  * @}
  */

/** @defgroup RCC_APB1_Clock_Sleep_Enable_Status APB1 Peripheral Clock Sleep Enabled Status
  * @brief  Check whether the APB1 peripheral clock sleep is enabled or not.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_FDCAN_IS_CLK_SLEEP_ENABLED()  LL_APB1_GRP2_IsEnabledClockLowPower(LL_APB1_GRP2_PERIPH_FDCAN)

#define __HAL_RCC_I2C1_IS_CLK_SLEEP_ENABLED()   LL_APB1_GRP1_IsEnabledClockLowPower(LL_APB1_GRP1_PERIPH_I2C1)

#define __HAL_RCC_I2C2_IS_CLK_SLEEP_ENABLED()   LL_APB1_GRP1_IsEnabledClockLowPower(LL_APB1_GRP1_PERIPH_I2C2)

#define __HAL_RCC_I2C3_IS_CLK_SLEEP_ENABLED()   LL_APB1_GRP1_IsEnabledClockLowPower(LL_APB1_GRP1_PERIPH_I2C3)

#define __HAL_RCC_I3C1_IS_CLK_SLEEP_ENABLED()   LL_APB1_GRP1_IsEnabledClockLowPower(LL_APB1_GRP1_PERIPH_I3C1)

#define __HAL_RCC_I3C2_IS_CLK_SLEEP_ENABLED()   LL_APB1_GRP1_IsEnabledClockLowPower(LL_APB1_GRP1_PERIPH_I3C2)

#define __HAL_RCC_LPTIM1_IS_CLK_SLEEP_ENABLED()  LL_APB1_GRP1_IsEnabledClockLowPower(LL_APB1_GRP1_PERIPH_LPTIM1)

#define __HAL_RCC_MDIOS_IS_CLK_SLEEP_ENABLED()  LL_APB1_GRP2_IsEnabledClockLowPower(LL_APB1_GRP2_PERIPH_MDIOS)

#define __HAL_RCC_SPDIFRX1_IS_CLK_SLEEP_ENABLED()  LL_APB1_GRP1_IsEnabledClockLowPower(LL_APB1_GRP1_PERIPH_SPDIFRX1)

#define __HAL_RCC_SPI2_IS_CLK_SLEEP_ENABLED()   LL_APB1_GRP1_IsEnabledClockLowPower(LL_APB1_GRP1_PERIPH_SPI2)

#define __HAL_RCC_SPI3_IS_CLK_SLEEP_ENABLED()   LL_APB1_GRP1_IsEnabledClockLowPower(LL_APB1_GRP1_PERIPH_SPI3)

#define __HAL_RCC_TIM2_IS_CLK_SLEEP_ENABLED()   LL_APB1_GRP1_IsEnabledClockLowPower(LL_APB1_GRP1_PERIPH_TIM2)

#define __HAL_RCC_TIM3_IS_CLK_SLEEP_ENABLED()   LL_APB1_GRP1_IsEnabledClockLowPower(LL_APB1_GRP1_PERIPH_TIM3)

#define __HAL_RCC_TIM4_IS_CLK_SLEEP_ENABLED()   LL_APB1_GRP1_IsEnabledClockLowPower(LL_APB1_GRP1_PERIPH_TIM4)

#define __HAL_RCC_TIM5_IS_CLK_SLEEP_ENABLED()   LL_APB1_GRP1_IsEnabledClockLowPower(LL_APB1_GRP1_PERIPH_TIM5)

#define __HAL_RCC_TIM6_IS_CLK_SLEEP_ENABLED()   LL_APB1_GRP1_IsEnabledClockLowPower(LL_APB1_GRP1_PERIPH_TIM6)

#define __HAL_RCC_TIM7_IS_CLK_SLEEP_ENABLED()   LL_APB1_GRP1_IsEnabledClockLowPower(LL_APB1_GRP1_PERIPH_TIM7)

#define __HAL_RCC_TIM10_IS_CLK_SLEEP_ENABLED()  LL_APB1_GRP1_IsEnabledClockLowPower(LL_APB1_GRP1_PERIPH_TIM10)

#define __HAL_RCC_TIM11_IS_CLK_SLEEP_ENABLED()  LL_APB1_GRP1_IsEnabledClockLowPower(LL_APB1_GRP1_PERIPH_TIM11)

#define __HAL_RCC_TIM12_IS_CLK_SLEEP_ENABLED()  LL_APB1_GRP1_IsEnabledClockLowPower(LL_APB1_GRP1_PERIPH_TIM12)

#define __HAL_RCC_TIM13_IS_CLK_SLEEP_ENABLED()  LL_APB1_GRP1_IsEnabledClockLowPower(LL_APB1_GRP1_PERIPH_TIM13)

#define __HAL_RCC_TIM14_IS_CLK_SLEEP_ENABLED()  LL_APB1_GRP1_IsEnabledClockLowPower(LL_APB1_GRP1_PERIPH_TIM14)

#define __HAL_RCC_USART2_IS_CLK_SLEEP_ENABLED()  LL_APB1_GRP1_IsEnabledClockLowPower(LL_APB1_GRP1_PERIPH_USART2)

#define __HAL_RCC_USART3_IS_CLK_SLEEP_ENABLED()  LL_APB1_GRP1_IsEnabledClockLowPower(LL_APB1_GRP1_PERIPH_USART3)

#define __HAL_RCC_UART4_IS_CLK_SLEEP_ENABLED()  LL_APB1_GRP1_IsEnabledClockLowPower(LL_APB1_GRP1_PERIPH_UART4)

#define __HAL_RCC_UART5_IS_CLK_SLEEP_ENABLED()  LL_APB1_GRP1_IsEnabledClockLowPower(LL_APB1_GRP1_PERIPH_UART5)

#define __HAL_RCC_UART7_IS_CLK_SLEEP_ENABLED()  LL_APB1_GRP1_IsEnabledClockLowPower(LL_APB1_GRP1_PERIPH_UART7)

#define __HAL_RCC_UART8_IS_CLK_SLEEP_ENABLED()  LL_APB1_GRP1_IsEnabledClockLowPower(LL_APB1_GRP1_PERIPH_UART8)

#define __HAL_RCC_UCPD1_IS_CLK_SLEEP_ENABLED()  LL_APB1_GRP2_IsEnabledClockLowPower(LL_APB1_GRP2_PERIPH_UCPD1)

#define __HAL_RCC_WWDG_IS_CLK_SLEEP_ENABLED()   LL_APB1_GRP1_IsEnabledClockLowPower(LL_APB1_GRP1_PERIPH_WWDG)

/**
  * @}
  */

/** @defgroup RCC_APB2_Clock_Sleep_Enable_Status APB2 Peripheral Clock Sleep Enabled Status
  * @brief  Check whether the APB2 peripheral clock sleep is enabled or not.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_SAI1_IS_CLK_SLEEP_ENABLED()    LL_APB2_GRP1_IsEnabledClockLowPower(LL_APB2_GRP1_PERIPH_SAI1)

#define __HAL_RCC_SAI2_IS_CLK_SLEEP_ENABLED()    LL_APB2_GRP1_IsEnabledClockLowPower(LL_APB2_GRP1_PERIPH_SAI2)

#define __HAL_RCC_SPI1_IS_CLK_SLEEP_ENABLED()    LL_APB2_GRP1_IsEnabledClockLowPower(LL_APB2_GRP1_PERIPH_SPI1)

#define __HAL_RCC_SPI4_IS_CLK_SLEEP_ENABLED()    LL_APB2_GRP1_IsEnabledClockLowPower(LL_APB2_GRP1_PERIPH_SPI4)

#define __HAL_RCC_SPI5_IS_CLK_SLEEP_ENABLED()    LL_APB2_GRP1_IsEnabledClockLowPower(LL_APB2_GRP1_PERIPH_SPI5)

#define __HAL_RCC_TIM1_IS_CLK_SLEEP_ENABLED()    LL_APB2_GRP1_IsEnabledClockLowPower(LL_APB2_GRP1_PERIPH_TIM1)

#define __HAL_RCC_TIM8_IS_CLK_SLEEP_ENABLED()    LL_APB2_GRP1_IsEnabledClockLowPower(LL_APB2_GRP1_PERIPH_TIM8)

#define __HAL_RCC_TIM9_IS_CLK_SLEEP_ENABLED()    LL_APB2_GRP1_IsEnabledClockLowPower(LL_APB2_GRP1_PERIPH_TIM9)

#define __HAL_RCC_TIM15_IS_CLK_SLEEP_ENABLED()   LL_APB2_GRP1_IsEnabledClockLowPower(LL_APB2_GRP1_PERIPH_TIM15)

#define __HAL_RCC_TIM16_IS_CLK_SLEEP_ENABLED()   LL_APB2_GRP1_IsEnabledClockLowPower(LL_APB2_GRP1_PERIPH_TIM16)

#define __HAL_RCC_TIM17_IS_CLK_SLEEP_ENABLED()   LL_APB2_GRP1_IsEnabledClockLowPower(LL_APB2_GRP1_PERIPH_TIM17)

#define __HAL_RCC_TIM18_IS_CLK_SLEEP_ENABLED()   LL_APB2_GRP1_IsEnabledClockLowPower(LL_APB2_GRP1_PERIPH_TIM18)

#define __HAL_RCC_USART1_IS_CLK_SLEEP_ENABLED()  LL_APB2_GRP1_IsEnabledClockLowPower(LL_APB2_GRP1_PERIPH_USART1)

#define __HAL_RCC_USART6_IS_CLK_SLEEP_ENABLED()  LL_APB2_GRP1_IsEnabledClockLowPower(LL_APB2_GRP1_PERIPH_USART6)

#define __HAL_RCC_UART9_IS_CLK_SLEEP_ENABLED()   LL_APB2_GRP1_IsEnabledClockLowPower(LL_APB2_GRP1_PERIPH_UART9)

#define __HAL_RCC_USART10_IS_CLK_SLEEP_ENABLED()  LL_APB2_GRP1_IsEnabledClockLowPower(LL_APB2_GRP1_PERIPH_USART10)

/**
  * @}
  */

/** @defgroup RCC_APB4_Clock_Sleep_Enable_Status APB4 Peripheral Clock Sleep Enabled Status
  * @brief  Check whether the APB4 peripheral clock sleep is enabled or not.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_BSEC_IS_CLK_SLEEP_ENABLED()     LL_APB4_GRP2_IsEnabledClockLowPower(LL_APB4_GRP2_PERIPH_BSEC)

#define __HAL_RCC_DTS_IS_CLK_SLEEP_ENABLED()      LL_APB4_GRP2_IsEnabledClockLowPower(LL_APB4_GRP2_PERIPH_DTS)

#define __HAL_RCC_HDP_IS_CLK_SLEEP_ENABLED()      LL_APB4_GRP1_IsEnabledClockLowPower(LL_APB4_GRP1_PERIPH_HDP)

#define __HAL_RCC_I2C4_IS_CLK_SLEEP_ENABLED()     LL_APB4_GRP1_IsEnabledClockLowPower(LL_APB4_GRP1_PERIPH_I2C4)

#define __HAL_RCC_LPTIM2_IS_CLK_SLEEP_ENABLED()   LL_APB4_GRP1_IsEnabledClockLowPower(LL_APB4_GRP1_PERIPH_LPTIM2)

#define __HAL_RCC_LPTIM3_IS_CLK_SLEEP_ENABLED()   LL_APB4_GRP1_IsEnabledClockLowPower(LL_APB4_GRP1_PERIPH_LPTIM3)

#define __HAL_RCC_LPTIM4_IS_CLK_SLEEP_ENABLED()   LL_APB4_GRP1_IsEnabledClockLowPower(LL_APB4_GRP1_PERIPH_LPTIM4)

#define __HAL_RCC_LPTIM5_IS_CLK_SLEEP_ENABLED()   LL_APB4_GRP1_IsEnabledClockLowPower(LL_APB4_GRP1_PERIPH_LPTIM5)

#define __HAL_RCC_LPUART1_IS_CLK_SLEEP_ENABLED()  LL_APB4_GRP1_IsEnabledClockLowPower(LL_APB4_GRP1_PERIPH_LPUART1)

#define __HAL_RCC_RTC_IS_CLK_SLEEP_ENABLED()      LL_APB4_GRP1_IsEnabledClockLowPower(LL_APB4_GRP1_PERIPH_RTC)

#define __HAL_RCC_RTCAPB_IS_CLK_SLEEP_ENABLED()   LL_APB4_GRP1_IsEnabledClockLowPower(LL_APB4_GRP1_PERIPH_RTCAPB)

#define __HAL_RCC_SPI6_IS_CLK_SLEEP_ENABLED()     LL_APB4_GRP1_IsEnabledClockLowPower(LL_APB4_GRP1_PERIPH_SPI6)

#define __HAL_RCC_SYSCFG_IS_CLK_SLEEP_ENABLED()   LL_APB4_GRP2_IsEnabledClockLowPower(LL_APB4_GRP2_PERIPH_SYSCFG)

#define __HAL_RCC_VREFBUF_IS_CLK_SLEEP_ENABLED()  LL_APB4_GRP1_IsEnabledClockLowPower(LL_APB4_GRP1_PERIPH_VREFBUF)

/**
  * @}
  */

/** @defgroup RCC_APB5_Clock_Sleep_Enable_Status APB5 Peripheral Clock Sleep Enabled Status
  * @brief  Check whether the APB5 peripheral clock sleep is enabled or not.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before
  *         using it.
  * @{
  */

#define __HAL_RCC_CSI_IS_CLK_SLEEP_ENABLED()     LL_APB5_GRP1_IsEnabledClockLowPower(LL_APB5_GRP1_PERIPH_CSI)

#define __HAL_RCC_DCMIPP_IS_CLK_SLEEP_ENABLED()  LL_APB5_GRP1_IsEnabledClockLowPower(LL_APB5_GRP1_PERIPH_DCMIPP)

#define __HAL_RCC_GFXTIM_IS_CLK_SLEEP_ENABLED()  LL_APB5_GRP1_IsEnabledClockLowPower(LL_APB5_GRP1_PERIPH_GFXTIM)

#define __HAL_RCC_LTDC_IS_CLK_SLEEP_ENABLED()    LL_APB5_GRP1_IsEnabledClockLowPower(LL_APB5_GRP1_PERIPH_LTDC)

#define __HAL_RCC_VENC_IS_CLK_SLEEP_ENABLED()    LL_APB5_GRP1_IsEnabledClockLowPower(LL_APB5_GRP1_PERIPH_VENC)


/**
  * @}
  */

/** @defgroup RCC_MISC_Configuration_Clock_Sleep_Enable_Status Misc Configuration Clock Sleep Enabled Status
  * @brief  Check whether the misc configuration clock sleep is enabled or not.
  * @note   After reset, the misc configuration clock is disabled and
  *         the application software has to enable this clock before using it.
  * @{
  */

#define __HAL_RCC_DBG_IS_CLK_SLEEP_ENABLED()          LL_MISC_IsEnabledClockLowPower(LL_DBG)

#define __HAL_RCC_XSPIPHYCOMP_IS_CLK_SLEEP_ENABLED()  LL_MISC_IsEnabledClockLowPower(LL_XSPIPHYCOMP)

#define __HAL_RCC_PER_IS_CLK_SLEEP_ENABLED()          LL_MISC_IsEnabledClockLowPower(LL_PER)

/**
  * @}
  */

/** @brief  Macro to enable or disable the Internal High Speed oscillator (HSI).
  * @note   After enabling the HSI, the application software should wait on
  *         HSIRDY flag to be set indicating that the HSI clock is stable
  *         and that HSI clock can be used to clock the PLL and/or system clock.
  * @note   HSI can not be stopped if it is used directly or through the PLL
  *         as system clock. In this case, you have to select another source
  *         of the system clock then stop the HSI.
  * @note   The HSI is stopped by hardware when entering STOP and STANDBY modes.
  * @param  __STATE__ specifies the new state of the HSI.
  *         This parameter can be one of the following values:
  *            @arg @ref RCC_HSI_OFF turn OFF the HSI oscillator
  *            @arg @ref RCC_HSI_ON turn ON the HSI oscillator
  * @note   When the HSI is stopped, HSIRDY flag goes low after 6 HSI oscillator
  *         clock cycles.
  */
#define __HAL_RCC_HSI_CONFIG(__STATE__) MODIFY_REG(RCC->CR, RCC_CR_HSION, (uint32_t)(__STATE__))

/** @brief  Macro to configure the Internal High Speed oscillator (HSI) clock divider.
  * @param  __HSIDIV__ specifies the HSI division factor.
  *         This parameter can be one of the following values:
  *            @arg RCC_HSI_DIV1 Divide the HSI oscillator clock by 1 (default after reset)
  *            @arg RCC_HSI_DIV2 Divide the HSI oscillator clock by 2
  *            @arg RCC_HSI_DIV4 Divide the HSI oscillator clock by 4
  *            @arg RCC_HSI_DIV8 Divide the HSI oscillator clock by 8
  */
#define __HAL_RCC_HSI_DIVIDER_CONFIG(__HSIDIV__) LL_RCC_HSI_SetDivider((uint32_t)(__HSIDIV__))

/** @brief  Macro to get the HSI divider.
  * @retval The HSI divider. The returned value can be one of the following values:
  *         of the following:
  *            @arg RCC_HSI_DIV1 Divide the HSI oscillator clock by 1 (default after reset)
  *            @arg RCC_HSI_DIV2 Divide the HSI oscillator clock by 2
  *            @arg RCC_HSI_DIV4 Divide the HSI oscillator clock by 4
  *            @arg RCC_HSI_DIV8 Divide the HSI oscillator clock by 8
  */
#define __HAL_RCC_GET_HSI_DIVIDER() ((uint32_t)LL_RCC_HSI_GetDivider())

/** @brief  Macros to enable or disable the Internal High Speed oscillator (HSI).
  * @note   The HSI is stopped by hardware when entering STOP and STANDBY modes.
  *         It is used (enabled by hardware) as system clock source after start-up
  *         from Reset, wakeup from STOP and STANDBY mode, or in case of failure
  *         of the HSE used directly or indirectly as system clock (if the Clock
  *         Security System CSS is enabled).
  * @note   HSI can not be stopped if it is used as system clock source. In this case,
  *         you have to select another source of the system clock then stop the HSI.
  * @note   After enabling the HSI, the application software should wait on HSIRDY
  *         flag to be set indicating that HSI clock is stable and can be used as
  *         system clock source.
  * @note   When the HSI is stopped, HSIRDY flag goes low after 6 HSI oscillator
  *         clock cycles.
  */
#define __HAL_RCC_HSI_ENABLE()  LL_RCC_HSI_Enable()

#define __HAL_RCC_HSI_DISABLE() LL_RCC_HSI_Disable()

/** @brief  Macro to adjust the Internal High Speed 64MHz oscillator (HSI) calibration value.
  * @note   The calibration is used to compensate for the variations in voltage
  *         and temperature that influence the frequency of the internal HSI RC.
  * @param  __HSICALIBRATIONVALUE__ specifies the calibration trimming value
  *         (default is RCC_HSICALIBRATION_DEFAULT).
  *         This parameter must be a number between 0 and 127.
  * @retval None
  */
#define __HAL_RCC_HSI_CALIBRATIONVALUE_ADJUST(__HSICALIBRATIONVALUE__) \
  LL_RCC_HSI_SetCalibTrimming((uint32_t)(__HSICALIBRATIONVALUE__));

/**
  * @brief    Macros to enable or disable the force of the Internal High Speed oscillator (HSI)
  *           in STOP mode to be quickly available as kernel clock for some peripherals.
  * @note     Keeping the HSI ON in STOP mode allows to avoid slowing down the communication
  *           speed because of the HSI start-up time.
  * @note     The enable of this function has not effect on the HSION bit.
  *           This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
#define __HAL_RCC_HSISTOP_ENABLE()     LL_RCC_HSI_EnableInStopMode()

#define __HAL_RCC_HSISTOP_DISABLE()    LL_RCC_HSI_DisableInStopMode()

/**
  * @brief  Macros to enable or disable the Internal oscillator (MSI).
  * @note     The MSI is stopped by hardware when entering STOP and STANDBY modes.
  *           It is used (enabled by hardware) as system clock source after
  *           start-up from Reset, wakeup from STOP and STANDBY mode, or in case
  *           of failure of the HSE used directly or indirectly as system clock
  *           (if the Clock Security System CSS is enabled).
  * @note     MSI can not be stopped if it is used as system clock source.
  *           In this case, you have to select another source of the system
  *           clock then stop the MSI.
  * @note     After enabling the MSI, the application software should wait on
  *           MSIRDY flag to be set indicating that MSI clock is stable and can
  *           be used as system clock source.
  * @note     When the MSI is stopped, MSIRDY flag goes low after 6 MSI oscillator
  *           clock cycles.
  */
#define __HAL_RCC_MSI_ENABLE()  LL_RCC_MSI_Enable()

#define __HAL_RCC_MSI_DISABLE() LL_RCC_MSI_Disable()

/** @brief  Macro to configure the Internal oscillator (MSI) frequency.
  * @param  __FREQ__ specifies the MSI frequency.
  *         This parameter can be one of the following values:
  *            @arg RCC_MSI_FREQ_4MHZ  4MHz selection (default after reset)
  *            @arg RCC_MSI_FREQ_16MHZ 16MHz selection
  */
#define __HAL_RCC_MSI_FREQUENCY_CONFIG(__FREQ__) LL_RCC_MSI_SetFrequency((uint32_t)(__FREQ__))

/** @brief  Macro to adjust the Internal oscillator (MSI) calibration value.
  * @note   The calibration is used to compensate for the variations in voltage
  *         and temperature that influence the frequency of the internal MSI RC.
  * @param  __MSICALIBRATIONVALUE__ specifies the calibration trimming value
  *         (default is RCC_MSICALIBRATION_DEFAULT).
  *         This parameter must be a number between 0 and 0x1F.
  * @retval None
  */
#define __HAL_RCC_MSI_CALIBRATIONVALUE_ADJUST(__MSICALIBRATIONVALUE__) \
  LL_RCC_MSI_SetCalibTrimming((uint32_t)(__MSICALIBRATIONVALUE__))

/**
  * @brief  Macros to enable or disable the force of the Low-power Internal oscillator (MSI)
  *         in STOP mode to be quickly available as kernel clock for USARTs and I2Cs.
  * @note   Keeping the MSI ON in STOP mode allows to avoid slowing down the communication
  *         speed because of the MSI start-up time.
  * @note   The enable of this function has not effect on the MSION bit.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
#define __HAL_RCC_MSISTOP_ENABLE()     LL_RCC_MSI_EnableInStopMode()

#define __HAL_RCC_MSISTOP_DISABLE()    LL_RCC_MSI_DisableInStopMode()

/** @brief  Macros to enable or disable the Internal Low Speed oscillator (LSI).
  * @note   After enabling the LSI, the application software should wait on
  *         LSIRDY flag to be set indicating that LSI clock is stable and can
  *         be used to clock the IWDG and/or the RTC.
  * @note   LSI can not be disabled if the IWDG is running.
  * @note   When the LSI is stopped, LSIRDY flag goes low after 6 LSI oscillator
  *         clock cycles.
  */
#define __HAL_RCC_LSI_ENABLE()         LL_RCC_LSI_Enable()

#define __HAL_RCC_LSI_DISABLE()        LL_RCC_LSI_Disable()

/**
  * @brief  Macros to configure the External High Speed oscillator (HSE).
  * @note   After enabling the HSE (RCC_HSE_ON or RCC_HSE_BYPASS or RCC_HSE_BYPASS_DIGITAL),
  *         the application software should wait on HSERDY flag to be set indicating that HSE clock
  *         is stable and can be used to clock the PLL and/or system clock and peripheral kernel
  *         clock source.
  * @note   HSE state can not be changed if it is used directly or through the
  *         PLL as system clock. In this case, you have to select another source
  *         of the system clock then change the HSE state (ex. disable it).
  * @note   The HSE is stopped by hardware when entering STOP and STANDBY modes.
  * @param  __STATE__ specifies the new state of the HSE.
  *         This parameter can be one of the following values:
  *            @arg @ref RCC_HSE_OFF            turn OFF the HSE oscillator, HSERDY flag goes low after
  *                                             6 HSE oscillator clock cycles.
  *            @arg @ref RCC_HSE_ON             turn ON the HSE oscillator.
  *            @arg @ref RCC_HSE_BYPASS         HSE oscillator bypassed with external clock (analog).
  *            @arg @ref RCC_HSE_BYPASS_DIGITAL HSE oscillator bypassed with external digital clock.
  */
#define __HAL_RCC_HSE_CONFIG(__STATE__)                                                 \
  do {                                                                  \
    if ((__STATE__) == RCC_HSE_ON)                                      \
    {                                                                   \
      WRITE_REG(RCC->CSR, RCC_CSR_HSEONS);                              \
    }                                                                   \
    else if ((__STATE__) == RCC_HSE_OFF)                                \
    {                                                                   \
      WRITE_REG(RCC->CCR, RCC_CCR_HSEONC);                              \
      CLEAR_BIT(RCC->HSECFGR, RCC_HSECFGR_HSEEXT | RCC_HSECFGR_HSEBYP); \
    }                                                                   \
    else if ((__STATE__) == RCC_HSE_BYPASS)                             \
    {                                                                   \
      SET_BIT(RCC->HSECFGR, RCC_HSECFGR_HSEBYP);                        \
      CLEAR_BIT(RCC->HSECFGR, RCC_HSECFGR_HSEEXT);                      \
      WRITE_REG(RCC->CSR, RCC_CSR_HSEONS);                              \
    }                                                                   \
    else if ((__STATE__) == RCC_HSE_BYPASS_DIGITAL)                     \
    {                                                                   \
      SET_BIT(RCC->HSECFGR, RCC_HSECFGR_HSEEXT | RCC_HSECFGR_HSEBYP);   \
      WRITE_REG(RCC->CSR, RCC_CSR_HSEONS);                              \
    }                                                                   \
    else                                                                \
    {                                                                   \
      WRITE_REG(RCC->CCR, RCC_CCR_HSEONC);                              \
      CLEAR_BIT(RCC->HSECFGR, RCC_HSECFGR_HSEEXT | RCC_HSECFGR_HSEBYP); \
    }                                                                   \
  } while(0)

/**
  * @brief  Macros to configure the External Low Speed oscillator (LSE).
  * @note   Transition LSE Bypass to LSE On and LSE On to LSE Bypass are not supported by this macro.
  *         User should request a transition to LSE Off first and then LSE On or LSE Bypass.
  * @note   The external input clock can have a frequency up to 1 MHz and be low swing (analog) or digital(*).
            A duty cycle close to 50% is recommended.
  * @note   As the LSE is in the Backup domain and write access is denied to
  *         this domain after reset, you have to enable write access using
  *         HAL_PWR_EnableBkUpAccess() function before to configure the LSE
  *         (to be done once after reset).
  * @note   After enabling the LSE (RCC_LSE_ON, RCC_LSE_BYPASS or RCC_LSE_BYPASS_DIGITAL), the application
  *         software should wait on LSERDY flag to be set indicating that LSE clock
  *         is stable and can be used to clock the RTC.
  * @note   If the RTC is used, the LSE bypass must not be configured in digital mode but in low swing analog mode (*)
  * @param  __STATE__ specifies the new state of the LSE.
  *         This parameter can be one of the following values:
  *            @arg @ref RCC_LSE_OFF            turn OFF the LSE oscillator, LSERDY flag goes low after
  *                                        6 LSE oscillator clock cycles.
  *            @arg @ref RCC_LSE_ON             turn ON the LSE oscillator.
  *            @arg @ref RCC_LSE_BYPASS         LSE oscillator bypassed with external clock (analog).
  *            @arg @ref RCC_LSE_BYPASS_DIGITAL LSE oscillator bypassed with external digital clock.
  */
#define __HAL_RCC_LSE_CONFIG(__STATE__) \
  do {                                               \
    if((__STATE__) == RCC_LSE_ON)                    \
    {                                                \
      WRITE_REG(RCC->CSR, RCC_CSR_LSEONS);           \
    }                                                \
    else if((__STATE__) == RCC_LSE_OFF)              \
    {                                                \
      WRITE_REG(RCC->CCR, RCC_CCR_LSEONC);           \
      CLEAR_BIT(RCC->LSECFGR, RCC_LSECFGR_LSEEXT);   \
      CLEAR_BIT(RCC->LSECFGR, RCC_LSECFGR_LSEBYP);   \
    }                                                \
    else if((__STATE__) == RCC_LSE_BYPASS)           \
    {                                                \
      SET_BIT(RCC->LSECFGR, RCC_LSECFGR_LSEBYP);     \
      CLEAR_BIT(RCC->LSECFGR, RCC_LSECFGR_LSEEXT);   \
      WRITE_REG(RCC->CSR, RCC_CSR_LSEONS);           \
    }                                                \
    else if((__STATE__) == RCC_LSE_BYPASS_DIGITAL)   \
    {                                                \
      SET_BIT(RCC->LSECFGR, RCC_LSECFGR_LSEBYP);     \
      SET_BIT(RCC->LSECFGR, RCC_LSECFGR_LSEEXT);     \
      WRITE_REG(RCC->CSR, RCC_CSR_LSEONS);           \
    }                                                \
    else                                             \
    {                                                \
      WRITE_REG(RCC->CCR, RCC_CCR_LSEONC);           \
      CLEAR_BIT(RCC->LSECFGR, RCC_LSECFGR_LSEBYP);   \
      CLEAR_BIT(RCC->LSECFGR, RCC_LSECFGR_LSEEXT);   \
    }                                                \
  } while(0)

/** @brief  Macros to enable or disable the the RTC clock.
  * @note   These macros must be used only after the RTC clock source was selected.
  */
#define __HAL_RCC_RTC_ENABLE()         LL_RCC_EnableRTC()

#define __HAL_RCC_RTC_DISABLE()        LL_RCC_DisableRTC()

/** @brief  Macro to configure the RTC clock (RTCCLK).
  * @note   As the RTC clock configuration bits are in the Backup domain and write
  *         access is denied to this domain after reset, you have to enable write
  *         access using the Power Backup Access macro before to configure
  *         the RTC clock source (to be done once after reset).
  * @note   Once the RTC clock is configured it cannot be changed unless the
  *         Backup domain is reset using __HAL_RCC_BACKUPRESET_FORCE() and
  *         __HAL_RCC_BACKUPRESET_RELEASE() macros, or by a Power On Reset (POR).
  * @param  __RTC_CLKSOURCE__ specifies the RTC clock source.
  *         This parameter can be one of the following values:
  *            RCC_RTCCLKSOURCE_LSE      LSE selected as RTC clock.
  *            RCC_RTCCLKSOURCE_LSI      LSI selected as RTC clock.
  *            RCC_RTCCLKSOURCE_HSE_DIVx HSE clock divided by x selected
  *                                      as RTC clock, where x:[1,64]
  * @note   If the LSE or LSI is used as RTC clock source, the RTC continues to
  *         work in STOP and STANDBY modes, and can be used as wakeup source.
  *         However, when the HSE clock is used as RTC clock source, the RTC
  *         cannot be used in STOP and STANDBY modes.
  * @note   The maximum input clock frequency for RTC is 1MHz (when using HSE as
  *         RTC clock source).
  */
#define __HAL_RCC_RTC_CONFIG(__RTC_CLKSOURCE__) \
  do { \
    if (((__RTC_CLKSOURCE__) & (RCC_CCIPR7_RTCSEL)) == RCC_CCIPR7_RTCSEL) { \
      __HAL_RCC_RTC_CLKPRESCALER(__RTC_CLKSOURCE__); \
    } \
    LL_RCC_SetRTCClockSource((__RTC_CLKSOURCE__) & RCC_CCIPR7_RTCSEL); \
  } while(0)

/** @brief  Macro to configure the RTC clock prescaler for HSE for RTCCLK.
  * @param  __RTC_CLKSOURCE__ specifies the RTC clock prescaler for HSE.
  *         This parameter can be one of the following values:
  *            RCC_RTCCLKSOURCE_HSE_DIVx HSE clock divided by x selected
  *                                      as RTC clock, where x:[1,64]
  */
#define __HAL_RCC_RTC_CLKPRESCALER(__RTC_CLKSOURCE__) \
  LL_RCC_SetRTC_HSEPrescaler(((__RTC_CLKSOURCE__) & (~RCC_CCIPR7_RTCSEL)))

/** @brief  Macros to get the RTC clock (RTCCLK).
  * @retval The returned value can be one of the following values:
  *            RCC_RTCCLKSOURCE_DISABLE  No clock selected as RTC clock.
  *            RCC_RTCCLKSOURCE_LSE      LSE selected as RTC clock.
  *            RCC_RTCCLKSOURCE_LSI      LSI selected as RTC clock.
  *            RCC_RTCCLKSOURCE_HSE_DIVx HSE clock divided by x selected
  *                                      as RTC clock, where x:[1,64]
  */
#define __HAL_RCC_GET_RTC_SOURCE() \
  ((READ_BIT(RCC->CCIPR7, RCC_CCIPR7_RTCSEL) == RCC_CCIPR7_RTCSEL) ? \
   (READ_BIT(RCC->CCIPR7, RCC_CCIPR7_RTCPRE) | RCC_CCIPR7_RTCSEL) : READ_BIT(RCC->CCIPR7, RCC_CCIPR7_RTCSEL))

/** @brief  Macros to force or release the Vswitch backup domain reset.
  * @note   This function resets the RTC peripheral (including the backup registers)
  *         and the RTC clock source selection in RCC_BDCR register.
  * @note   The BKPSRAM is not affected by this reset.
  */
#define __HAL_RCC_BACKUPRESET_FORCE()   LL_RCC_ForceBackupDomainReset()

#define __HAL_RCC_BACKUPRESET_RELEASE() LL_RCC_ReleaseBackupDomainReset()

/** @brief  Macros to enable or disable the PLL1.
  * @note   After enabling the PLL1, the application software should wait on
  *         PLL1RDY flag to be set indicating that PLL1 clock is stable and can
  *         be used as CPU and/or system bus and/or kernel clock source thru ICx.
  * @note   The PLL1 can not be disabled if it is used as system clock source
  * @note   The PLL1 is disabled by hardware when entering STOP and STANDBY modes.
  */
#define __HAL_RCC_PLL1_ENABLE()        LL_RCC_PLL1_Enable()

#define __HAL_RCC_PLL1_DISABLE()       LL_RCC_PLL1_Disable()

/**
  * @brief  Macros to enable or disable the PLL1 post divider 1, post divider 2
  *         and PLL1 clock output when not in bypass mode.
  */
#define __HAL_RCC_PLL1CLKOUT_ENABLE()       LL_RCC_PLL1P_Enable()
#define __HAL_RCC_PLL1CLKOUT_DISABLE()      LL_RCC_PLL1P_Disable()

/**
  * @brief  Macro to configure the PLL1 in integer mode with clock source, multiplication
  *         and division factors.
  * @note   This macro must be used only when the PLL1 is disabled or the PLL1 output is bypassed
  *         and driven by the PLL reference clock. This macro switches off the bypass mode.
  *
  * @param  __PLLSOURCE__ specifies the PLL entry clock source.
  *           This parameter can be one of the following values:
  *             @arg @ref RCC_PLLSOURCE_HSI HSI oscillator clock selected as PLL clock entry
  *             @arg @ref RCC_PLLSOURCE_MSI MSI oscillator clock selected as PLL clock entry
  *             @arg @ref RCC_PLLSOURCE_HSE HSE oscillator clock selected as PLL clock entry
  *             @arg @ref RCC_PLLSOURCE_PIN I2S_CKIN clock selected as PLL clock entry
  * @note   This clock source cannot be changed on any PLL if another PLL is already enabled.
  *
  * @param  __PLLM__ specifies the division factor for PLL VCO input clock.
  *           This parameter must be a number between 1 and 63.
  * @note   You have to set the PLLM parameter correctly to ensure that the VCO input
  *         frequency ranges from 1 to 50 MHz.
  *
  * @param  __PLLN__ specifies the multiplication factor for PLL VCO output clock.
  *           This parameter must be a number between 16 and 2500.
  *
  * @param  __PLLP1__ specifies the post division factor 1 for system  clock.
  *           This parameter must be a number between 1 and 7.
  *
  * @param  __PLLP2__ specifies the post division factor 2 for system  clock.
  *           This parameter must be a number between 1 and 7.
  *
  * @retval None
  */

#define __HAL_RCC_PLL1_CONFIG(__PLLSOURCE__, __PLLM__, __PLLN__, __PLLP1__, __PLLP2__) \
  do { \
    MODIFY_REG(RCC->PLL1CFGR1, (RCC_PLL1CFGR1_PLL1SEL | RCC_PLL1CFGR1_PLL1BYP | RCC_PLL1CFGR1_PLL1DIVM | RCC_PLL1CFGR1_PLL1DIVN), \
               ((__PLLSOURCE__) | ( (__PLLM__) << RCC_PLL1CFGR1_PLL1DIVM_Pos) | (((__PLLN__) << RCC_PLL1CFGR1_PLL1DIVN_Pos))));  \
    MODIFY_REG(RCC->PLL1CFGR2, RCC_PLL1CFGR2_PLL1DIVNFRAC, 0U); \
    MODIFY_REG(RCC->PLL1CFGR3, (RCC_PLL1CFGR3_PLL1PDIV1 | RCC_PLL1CFGR3_PLL1PDIV2), \
               ((((__PLLP1__) << RCC_PLL1CFGR3_PLL1PDIV1_Pos) & RCC_PLL1CFGR3_PLL1PDIV1) | \
                ((((__PLLP2__) << RCC_PLL1CFGR3_PLL1PDIV2_Pos) & RCC_PLL1CFGR3_PLL1PDIV2)))); \
  } while(0)

/** @brief  Macro to configure the PLL1 clock source.
  * @note   This macro must be used only when the PLL1 is disabled or the PLL1 output is bypassed
  *         and driven by the PLL reference clock. This macro switches off the bypass mode.
  *
  * @param  __PLLSOURCE__ specifies the PLL entry clock source.
  *         This parameter can be one of the following values:
  *            @arg @ref RCC_PLLSOURCE_HSI  HSI oscillator clock selected as PLL clock entry
  *            @arg @ref RCC_PLLSOURCE_MSI  MSI oscillator clock selected as PLL clock entry
  *            @arg @ref RCC_PLLSOURCE_HSE  HSE oscillator clock selected as PLL clock entry
  *            @arg @ref RCC_PLLSOURCE_PIN  External I2S_CKIN clock selected as PLL clock entry
  */
#define __HAL_RCC_PLL1_PLLSOURCE_CONFIG(__PLLSOURCE__)  LL_RCC_PLL1_SetSource((uin32_t)__PLLSOURCE__)

/** @brief  Macro to get the clock source used as PLL1 clock source.
  * @retval The oscillator used as PLL clock source.
  *         The returned value can be one of the following values:
  *            @arg @ref RCC_PLLSOURCE_HSI  HSI oscillator is used as PLL clock source.
  *            @arg @ref RCC_PLLSOURCE_MSI  MSI oscillator is used as PLL clock source.
  *            @arg @ref RCC_PLLSOURCE_HSE  HSE oscillator is used as PLL clock source.
  *            @arg @ref RCC_PLLSOURCE_PIN  External I2S_CKIN is used as PLL clock source.
  */
#define __HAL_RCC_GET_PLL1_OSCSOURCE()  LL_RCC_PLL1_GetSource()

/**
  * @brief  Macro to configure the PLL1 clock Fractional Part Of The Multiplication Factor
  * @note   This configuration cannot be requested when the PLL1 has been enabled.
  *
  * @param  __PLLDIVNFRAC__ specifies Fractional Part Of The Multiplication Factor for PLL1 VCO.
  *                         It should be a value between 0 and 2^24.
  * @retval None
  */
#define  __HAL_RCC_PLL1_DIVNFRAC_CONFIG(__PLLDIVNFRAC__) LL_RCC_PLL1_SetFRACN((uint32_t)(__PLLDIVNFRAC__))

/**
  * @brief  Macro to enable the PLL1 clock Fractional mode
  * @note   This configuration cannot be requested when the PLL1 has been enabled.
  * @retval None
  */
#define  __HAL_RCC_PLL1_FRACN_ENABLE() LL_RCC_PLL1_EnableFractionalModulationSpreadSpectrum()

/**
  * @brief  Macro to disable the PLL1 clock Fractional mode
  * @note   This configuration cannot be requested when the PLL1 has been enabled.
  * @retval None
  */
#define  __HAL_RCC_PLL1_FRACN_DISABLE() LL_RCC_PLL1_DisableFractionalModulationSpreadSpectrum()

/** @brief  Macros to enable or disable PLL2.
  * @note   After enabling PLL2, the application software should wait on
  *         PLL2RDY flag to be set indicating that PLL2 clock is stable and can
  *         be used as CPU and/or system bus and/or kernel clock source thru ICx.
  * @note   PLL2 is disabled by hardware when entering Stop and Standby modes.
  */
#define __HAL_RCC_PLL2_ENABLE()         LL_RCC_PLL2_Enable()
#define __HAL_RCC_PLL2_DISABLE()        LL_RCC_PLL2_Disable()

/**
  * @brief  Macros to enable or disable the PLL2 post divider 1, post divider 2
  *         and PLL2 clock output when not in bypass mode.
  */
#define __HAL_RCC_PLL2CLKOUT_ENABLE()       LL_RCC_PLL2P_Enable()
#define __HAL_RCC_PLL2CLKOUT_DISABLE()      LL_RCC_PLL2P_Disable()

/**
  * @brief  Macro to configure the PLL2 in integer mode with clock source, multiplication
  *         and division factors.
  * @note   This macro must be used only when the PLL2 is disabled or the PLL2 output is bypassed
  *         and driven by the PLL reference clock. This macro switches off the bypass mode.
  *
  * @param  __PLLSOURCE__ specifies the PLL entry clock source.
  *           This parameter can be one of the following values:
  *             @arg @ref RCC_PLLSOURCE_HSI HSI oscillator clock selected as PLL clock entry
  *             @arg @ref RCC_PLLSOURCE_MSI MSI oscillator clock selected as PLL clock entry
  *             @arg @ref RCC_PLLSOURCE_HSE HSE oscillator clock selected as PLL clock entry
  *             @arg @ref RCC_PLLSOURCE_PIN I2S_CKIN clock selected as PLL clock entry
  * @note   This clock source cannot be changed on any PLL if another PLL is already enabled.
  *
  * @param  __PLLM__ specifies the division factor for PLL VCO input clock
  *           This parameter must be a number between 1 and 63.
  * @note   You have to set the PLLM parameter correctly to ensure that the VCO input
  *         frequency ranges from 1 to 50 MHz.
  *
  * @param  __PLLN__ specifies the multiplication factor for PLL VCO output clock
  *           This parameter must be a number between 16 and 2500.
  * @param  __PLLP1__ specifies the post division factor 1 for system  clock.
  *           This parameter must be a number between 1 and 7.
  * @param  __PLLP2__ specifies the post division factor 2 for system  clock.
  *           This parameter must be a number between 1 and 7.
  *
  * @retval None
  */

#define __HAL_RCC_PLL2_CONFIG(__PLLSOURCE__, __PLLM__, __PLLN__, __PLLP1__, __PLLP2__) \
  do { \
    MODIFY_REG(RCC->PLL2CFGR1, (RCC_PLL2CFGR1_PLL2SEL | RCC_PLL2CFGR1_PLL2BYP | RCC_PLL2CFGR1_PLL2DIVM | RCC_PLL2CFGR1_PLL2DIVN), \
               ((__PLLSOURCE__) | ( (__PLLM__) << RCC_PLL2CFGR1_PLL2DIVM_Pos) | (((__PLLN__) << RCC_PLL2CFGR1_PLL2DIVN_Pos))));  \
    MODIFY_REG(RCC->PLL2CFGR2, RCC_PLL2CFGR2_PLL2DIVNFRAC, 0U); \
    MODIFY_REG(RCC->PLL2CFGR3, (RCC_PLL2CFGR3_PLL2PDIV1 | RCC_PLL2CFGR3_PLL2PDIV2), \
               ((((__PLLP1__) << RCC_PLL2CFGR3_PLL2PDIV1_Pos) & RCC_PLL2CFGR3_PLL2PDIV1) | \
                ((((__PLLP2__) << RCC_PLL2CFGR3_PLL2PDIV2_Pos) & RCC_PLL2CFGR3_PLL2PDIV2)))); \
  } while(0)


/** @brief  Macro to configure the PLL2 clock source.
  * @note   This macro must be used only when the PLL2 is disabled or the PLL2 output is bypassed
  *         and driven by the PLL reference clock. This macro switches off the bypass mode.
  *
  * @param  __PLLSOURCE__ specifies the PLL entry clock source.
  *           This parameter can be one of the following values:
  *              @arg @ref RCC_PLLSOURCE_HSI  HSI oscillator clock selected as PLL clock entry
  *              @arg @ref RCC_PLLSOURCE_MSI  MSI oscillator clock selected as PLL clock entry
  *              @arg @ref RCC_PLLSOURCE_HSE  HSE oscillator clock selected as PLL clock entry
  *              @arg @ref RCC_PLLSOURCE_PIN  External I2S_CKIN clock selected as PLL clock entry
  */
#define __HAL_RCC_PLL2_PLLSOURCE_CONFIG(__PLLSOURCE__) LL_RCC_PLL2_SetSource((uin32_t)__PLLSOURCE__)

/** @brief  Macro to get the clock source used as PLL2 clock source.
  * @retval The oscillator used as PLL clock source.
  *         The returned value can be one of the following values:
  *            @arg @ref RCC_PLLSOURCE_HSI  HSI oscillator is used as PLL clock source.
  *            @arg @ref RCC_PLLSOURCE_MSI  MSI oscillator is used as PLL clock source.
  *            @arg @ref RCC_PLLSOURCE_HSE  HSE oscillator is used as PLL clock source.
  *            @arg @ref RCC_PLLSOURCE_PIN  External I2S_CKIN is used as PLL clock source.
  */
#define __HAL_RCC_GET_PLL2_OSCSOURCE() LL_RCC_PLL2_GetSource()

/**
  * @brief  Macro to configure the PLL2 clock Fractional Part Of The Multiplication Factor
  * @note   This configuration cannot be requested when the PLL2 has been enabled.
  *
  * @param  __PLLDIVNFRAC__ specifies Fractional Part Of The Multiplication Factor for PLL2 VCO.
  *                         It should be a value between 0 and 2^24.
  * @retval None
  */
#define  __HAL_RCC_PLL2_DIVNFRAC_CONFIG(__PLLDIVNFRAC__) LL_RCC_PLL2_SetFRACN((uint32_t)(__PLLDIVNFRAC__))

/**
  * @brief  Macro to enable the PLL2 clock Fractional mode
  * @note   This configuration cannot be requested when the PLL2 has been enabled.
  * @retval None
  */
#define  __HAL_RCC_PLL2_FRACN_ENABLE() LL_RCC_PLL2_EnableFractionalModulationSpreadSpectrum()

/**
  * @brief  Macro to disable the PLL2 clock Fractional mode
  * @note   This configuration cannot be requested when the PLL2 has been enabled.
  * @retval None
  */
#define  __HAL_RCC_PLL2_FRACN_DISABLE() LL_RCC_PLL2_DisableFractionalModulationSpreadSpectrum()

/** @brief  Macros to enable or disable PLL3.
  * @note   After enabling PLL3, the application software should wait on
  *         PLL3RDY flag to be set indicating that PLL3 clock is stable and can
  *         be used as CPU and/or system bus and/or kernel clock source thru ICx.
  * @note   PLL3 is disabled by hardware when entering Stop and Standby modes.
  */
#define __HAL_RCC_PLL3_ENABLE()         LL_RCC_PLL3_Enable()
#define __HAL_RCC_PLL3_DISABLE()        LL_RCC_PLL3_Disable()

/**
  * @brief  Macros to enable or disable the PLL3 post divider 1, post divider 2
  *         and PLL3 clock output when not in bypass mode.
  */
#define __HAL_RCC_PLL3CLKOUT_ENABLE()       LL_RCC_PLL3P_Enable()
#define __HAL_RCC_PLL3CLKOUT_DISABLE()      LL_RCC_PLL3P_Disable()

/**
  * @brief  Macro to configure the PLL3 in integer mode with clock source, multiplication
  *         and division factors.
  * @note   This macro must be used only when the PLL3 is disabled or the PLL3 output is bypassed
  *         and driven by the PLL reference clock. This macro switches off the bypass mode.
  *
  * @param  __PLLSOURCE__ specifies the PLL entry clock source.
  *           This parameter can be one of the following values:
  *             @arg @ref RCC_PLLSOURCE_HSI HSI oscillator clock selected as PLL clock entry
  *             @arg @ref RCC_PLLSOURCE_MSI MSI oscillator clock selected as PLL clock entry
  *             @arg @ref RCC_PLLSOURCE_HSE HSE oscillator clock selected as PLL clock entry
  *             @arg @ref RCC_PLLSOURCE_PIN I2S_CKIN clock selected as PLL clock entry
  * @note   This clock source cannot be changed on any PLL if another PLL is already enabled.
  *
  * @param  __PLLM__ specifies the division factor for PLL VCO input clock
  *           This parameter must be a number between 1 and 63.
  * @note   You have to set the PLLM parameter correctly to ensure that the VCO input
  *         frequency ranges from 1 to 50 MHz.
  *
  * @param  __PLLN__ specifies the multiplication factor for PLL VCO output clock
  *           This parameter must be a number between 16 and 2500.
  * @param  __PLLP1__ specifies the post division factor 1 for system  clock.
  *           This parameter must be a number between 1 and 7.
  * @param  __PLLP2__ specifies the post division factor 2 for system  clock.
  *           This parameter must be a number between 1 and 7.
  *
  * @retval None
  */

#define __HAL_RCC_PLL3_CONFIG(__PLLSOURCE__, __PLLM__, __PLLN__, __PLLP1__, __PLLP2__) \
  do { \
    MODIFY_REG(RCC->PLL3CFGR1, (RCC_PLL3CFGR1_PLL3SEL | RCC_PLL3CFGR1_PLL3BYP | RCC_PLL3CFGR1_PLL3DIVM | RCC_PLL3CFGR1_PLL3DIVN), \
               ((__PLLSOURCE__) | ( (__PLLM__) << RCC_PLL3CFGR1_PLL3DIVM_Pos) | (((__PLLN__) << RCC_PLL3CFGR1_PLL3DIVN_Pos))));  \
    MODIFY_REG(RCC->PLL3CFGR2, RCC_PLL3CFGR2_PLL3DIVNFRAC, 0U); \
    MODIFY_REG(RCC->PLL3CFGR3, (RCC_PLL3CFGR3_PLL3PDIV1 | RCC_PLL3CFGR3_PLL3PDIV2), \
               ((((__PLLP1__) << RCC_PLL3CFGR3_PLL3PDIV1_Pos) & RCC_PLL3CFGR3_PLL3PDIV1) | \
                ((((__PLLP2__) << RCC_PLL3CFGR3_PLL3PDIV2_Pos) & RCC_PLL3CFGR3_PLL3PDIV2)))); \
  } while(0)


/** @brief  Macro to configure the PLL3 clock source.
  * @note   This macro must be used only when the PLL3 is disabled or the PLL3 output is bypassed
  *         and driven by the PLL reference clock. This macro switches off the bypass mode.
  *
  * @param  __PLLSOURCE__ specifies the PLL entry clock source.
  *           This parameter can be one of the following values:
  *              @arg @ref RCC_PLLSOURCE_HSI  HSI oscillator clock selected as PLL clock entry
  *              @arg @ref RCC_PLLSOURCE_MSI  MSI oscillator clock selected as PLL clock entry
  *              @arg @ref RCC_PLLSOURCE_HSE  HSE oscillator clock selected as PLL clock entry
  *              @arg @ref RCC_PLLSOURCE_PIN  External I2S_CKIN clock selected as PLL clock entry
  */
#define __HAL_RCC_PLL3_PLLSOURCE_CONFIG(__PLLSOURCE__) LL_RCC_PLL3_SetSource((uin32_t)__PLLSOURCE__)

/** @brief  Macro to get the clock source used as PLL3 clock source.
  * @retval The oscillator used as PLL clock source.
  *         The returned value can be one of the following values:
  *            @arg @ref RCC_PLLSOURCE_HSI  HSI oscillator is used as PLL clock source.
  *            @arg @ref RCC_PLLSOURCE_MSI  MSI oscillator is used as PLL clock source.
  *            @arg @ref RCC_PLLSOURCE_HSE  HSE oscillator is used as PLL clock source.
  *            @arg @ref RCC_PLLSOURCE_PIN  External I2S_CKIN is used as PLL clock source.
  */
#define __HAL_RCC_GET_PLL3_OSCSOURCE() LL_RCC_PLL3_GetSource()

/**
  * @brief  Macro to configure the PLL3 clock Fractional Part Of The Multiplication Factor
  * @note   This configuration cannot be requested when the PLL3 has been enabled.
  *
  * @param  __PLLDIVNFRAC__ specifies Fractional Part Of The Multiplication Factor for PLL3 VCO.
  *                         It should be a value between 0 and 2^24.
  * @retval None
  */
#define  __HAL_RCC_PLL3_DIVNFRAC_CONFIG(__PLLDIVNFRAC__) LL_RCC_PLL3_SetFRACN((uint32_t)(__PLLDIVNFRAC__))

/**
  * @brief  Macro to enable the PLL3 clock Fractional mode
  * @note   This configuration cannot be requested when the PLL3 has been enabled.
  * @retval None
  */
#define  __HAL_RCC_PLL3_FRACN_ENABLE() LL_RCC_PLL3_EnableFractionalModulationSpreadSpectrum()

/**
  * @brief  Macro to disable the PLL3 clock Fractional mode
  * @note   This configuration cannot be requested when the PLL3 has been enabled.
  * @retval None
  */
#define  __HAL_RCC_PLL3_FRACN_DISABLE() LL_RCC_PLL3_DisableFractionalModulationSpreadSpectrum()

/** @brief  Macros to enable or disable PLL4.
  * @note   After enabling PLL4, the application software should wait on
  *         PLL4RDY flag to be set indicating that PLL4 clock is stable and can
  *         be used as CPU and/or system bus and/or kernel clock source thru ICx.
  * @note   PLL4 is disabled by hardware when entering Stop and Standby modes.
  */
#define __HAL_RCC_PLL4_ENABLE()         LL_RCC_PLL4_Enable()
#define __HAL_RCC_PLL4_DISABLE()        LL_RCC_PLL4_Disable()

/**
  * @brief  Macros to enable or disable the PLL4 post divider 1, post divider 2
  *         and PLL4 clock output when not in bypass mode.
  */
#define __HAL_RCC_PLL4CLKOUT_ENABLE()       LL_RCC_PLL4P_Enable()
#define __HAL_RCC_PLL4CLKOUT_DISABLE()      LL_RCC_PLL4P_Disable()

/**
  * @brief  Macro to configure the PLL4 in integer mode with clock source, multiplication
  *         and division factors.
  * @note   This macro must be used only when the PLL4 is disabled or the PLL4 output is bypassed
  *         and driven by the PLL reference clock. This macro switches off the bypass mode.
  *
  * @param  __PLLSOURCE__ specifies the PLL entry clock source.
  *           This parameter can be one of the following values:
  *             @arg @ref RCC_PLLSOURCE_HSI HSI oscillator clock selected as PLL clock entry
  *             @arg @ref RCC_PLLSOURCE_MSI MSI oscillator clock selected as PLL clock entry
  *             @arg @ref RCC_PLLSOURCE_HSE HSE oscillator clock selected as PLL clock entry
  *             @arg @ref RCC_PLLSOURCE_PIN I2S_CKIN clock selected as PLL clock entry
  * @note   This clock source cannot be changed on any PLL if another PLL is already enabled.
  *
  * @param  __PLLM__ specifies the division factor for PLL VCO input clock
  *           This parameter must be a number between 1 and 63.
  * @note   You have to set the PLLM parameter correctly to ensure that the VCO input
  *         frequency ranges from 1 to 50 MHz.
  *
  * @param  __PLLN__ specifies the multiplication factor for PLL VCO output clock
  *           This parameter must be a number between 16 and 2500.
  * @param  __PLLP1__ specifies the post division factor 1 for system  clock.
  *           This parameter must be a number between 1 and 7.
  * @param  __PLLP2__ specifies the post division factor 2 for system  clock.
  *           This parameter must be a number between 1 and 7.
  *
  * @retval None
  */

#define __HAL_RCC_PLL4_CONFIG(__PLLSOURCE__, __PLLM__, __PLLN__, __PLLP1__, __PLLP2__) \
  do { \
    MODIFY_REG(RCC->PLL4CFGR1, (RCC_PLL4CFGR1_PLL4SEL | RCC_PLL4CFGR1_PLL4BYP | RCC_PLL4CFGR1_PLL4DIVM | RCC_PLL4CFGR1_PLL4DIVN), \
               ((__PLLSOURCE__) | ( (__PLLM__) << RCC_PLL4CFGR1_PLL4DIVM_Pos) | (((__PLLN__) << RCC_PLL4CFGR1_PLL4DIVN_Pos))));  \
    MODIFY_REG(RCC->PLL4CFGR2, RCC_PLL4CFGR2_PLL4DIVNFRAC, 0U); \
    MODIFY_REG(RCC->PLL4CFGR3, (RCC_PLL4CFGR3_PLL4PDIV1 | RCC_PLL4CFGR3_PLL4PDIV2), \
               ((((__PLLP1__) << RCC_PLL4CFGR3_PLL4PDIV1_Pos) & RCC_PLL4CFGR3_PLL4PDIV1) | \
                ((((__PLLP2__) << RCC_PLL4CFGR3_PLL4PDIV2_Pos) & RCC_PLL4CFGR3_PLL4PDIV2)))); \
  } while(0)


/** @brief  Macro to configure the PLL4 clock source.
  * @note   This macro must be used only when the PLL4 is disabled or the PLL4 output is bypassed
  *         and driven by the PLL reference clock. This macro switches off the bypass mode.
  * @param  __PLLSOURCE__ specifies the PLL entry clock source.
  *         This parameter can be one of the following values:
  *            @arg @ref RCC_PLLSOURCE_HSI  HSI oscillator clock selected as PLL clock entry
  *            @arg @ref RCC_PLLSOURCE_MSI  MSI oscillator clock selected as PLL clock entry
  *            @arg @ref RCC_PLLSOURCE_HSE  HSE oscillator clock selected as PLL clock entry
  *            @arg @ref RCC_PLLSOURCE_PIN  External I2S_CKIN clock selected as PLL clock entry
  */
#define __HAL_RCC_PLL4_PLLSOURCE_CONFIG(__PLLSOURCE__) LL_RCC_PLL4_SetSource((uin32_t)__PLLSOURCE__)

/** @brief  Macro to get the clock source used as PLL4 clock source.
  * @retval The oscillator used as PLL clock source.
  *         The returned value can be one of the following values:
  *            @arg @ref RCC_PLLSOURCE_HSI  HSI oscillator is used as PLL clock source.
  *            @arg @ref RCC_PLLSOURCE_MSI  MSI oscillator is used as PLL clock source.
  *            @arg @ref RCC_PLLSOURCE_HSE  HSE oscillator is used as PLL clock source.
  *            @arg @ref RCC_PLLSOURCE_PIN  External I2S_CKIN is used as PLL clock source.
  */
#define __HAL_RCC_GET_PLL4_OSCSOURCE() LL_RCC_PLL4_GetSource()

/**
  * @brief  Macro to configure the PLL4 clock Fractional Part Of The Multiplication Factor
  * @note   This configuration cannot be requested when the PLL4 has been enabled.
  *
  * @param  __PLLDIVNFRAC__ specifies Fractional Part Of The Multiplication Factor for PLL4 VCO.
  *                         It should be a value between 0 and 2^24.
  * @retval None
  */
#define  __HAL_RCC_PLL4_DIVNFRAC_CONFIG(__PLLDIVNFRAC__)  LL_RCC_PLL4_SetFRACN((uint32_t)(__PLLDIVNFRAC__))

/**
  * @brief  Macro to enable the PLL4 clock Fractional mode
  * @note   This configuration cannot be requested when the PLL4 has been enabled.
  * @retval None
  */
#define  __HAL_RCC_PLL4_FRACN_ENABLE() LL_RCC_PLL4_EnableFractionalModulationSpreadSpectrum()


/**
  * @brief  Macro to disable the PLL4 clock Fractional mode
  * @note   This configuration cannot be requested when the PLL4 has been enabled.
  * @retval None
  */
#define  __HAL_RCC_PLL4_FRACN_DISABLE()  LL_RCC_PLL4_DisableFractionalModulationSpreadSpectrum()

/**
  * @brief Macro to configure the CPU clock source.
  * @param  __CPUCLKSOURCE__ specifies the CPU clock source.
  *         This parameter can be one of the following values:
  *            @arg @ref RCC_CPUCLKSOURCE_HSI    HSI oscillator is used as CPU clock source.
  *            @arg @ref RCC_CPUCLKSOURCE_MSI    MSI oscillator is used as CPU clock source.
  *            @arg @ref RCC_CPUCLKSOURCE_HSE    HSE oscillator is used as CPU clock source.
  *            @arg @ref RCC_CPUCLKSOURCE_IC1    IC1 output is used as CPU clock source.
  * @retval None
  */
#define __HAL_RCC_CPUCLK_CONFIG(__CPUCLKSOURCE__) LL_RCC_SetCpuClkSource((uint32_t)(__CPUCLKSOURCE__))

/** @brief  Macro to get the clock source used as CPU clock.
  * @retval The clock source used as CPU clock.
  *         The returned value can be one of the following values:
  *            @arg @ref RCC_CPUCLKSOURCE_STATUS_HSI    HSI used as CPU clock.
  *            @arg @ref RCC_CPUCLKSOURCE_STATUS_MSI    MSI used as CPU clock.
  *            @arg @ref RCC_CPUCLKSOURCE_STATUS_HSE    HSE used as CPU clock.
  *            @arg @ref RCC_CPUCLKSOURCE_STATUS_IC1    IC1 used as CPU clock.
  */
#define __HAL_RCC_GET_CPUCLK_SOURCE() LL_RCC_GetCpuClkSource()


/**
  * @brief Macro to configure the system bus clock source.
  * @param  __SYSCLKSOURCE__ specifies the system clock source.
  *         This parameter can be one of the following values:
  *            @arg @ref RCC_SYSCLKSOURCE_HSI           HSI oscillator is used as system bus clock source.
  *            @arg @ref RCC_SYSCLKSOURCE_MSI           MSI oscillator is used as system bus clock source.
  *            @arg @ref RCC_SYSCLKSOURCE_HSE           HSE oscillator is used as system bus clock source.
  *            @arg @ref RCC_SYSCLKSOURCE_IC2_IC6_IC11  IC2, IC6 and IC11 outputs are used as system bus clock source.
  * @retval None
  */
#define __HAL_RCC_SYSCLK_CONFIG(__SYSCLKSOURCE__) LL_RCC_SetSysClkSource((uint32_t)(__SYSCLKSOURCE__))

/** @brief  Macro to get the clock source used as system bus clock.
  * @retval The clock source used as system clock.
  *         The returned value can be one of the following values:
  *            @arg @ref RCC_SYSCLKSOURCE_STATUS_HSI           HSI used as system bus clock.
  *            @arg @ref RCC_SYSCLKSOURCE_STATUS_MSI           MSI used as system bus clock.
  *            @arg @ref RCC_SYSCLKSOURCE_STATUS_HSE           HSE used as system bus clock.
  *            @arg @ref RCC_SYSCLKSOURCE_STATUS_IC2_IC6_IC11  IC2, IC6 and IC11 outputs used as system bus clock.
  */
#define __HAL_RCC_GET_SYSCLK_SOURCE() LL_RCC_GetSysClkSource()

/**
  * @brief  Macro to configure the External Low Speed oscillator (LSE) drive capability.
  * @note   As the LSE is in the Backup domain and write access is denied to
  *         this domain after reset, you have to enable write access using
  *         HAL_PWR_EnableBkUpAccess() function before to configure the LSE
  *         (to be done once after reset).
  * @note   This parameter cannot be updated while LSE is ON.
  * @param  __LSEDRIVE__ specifies the new state of the LSE drive capability.
  *          This parameter can be one of the following values:
  *            @arg @ref RCC_LSEDRIVE_LOW        LSE oscillator low drive capability.
  *            @arg @ref RCC_LSEDRIVE_MEDIUMLOW  LSE oscillator medium low drive capability.
  *            @arg @ref RCC_LSEDRIVE_MEDIUMHIGH LSE oscillator medium high drive capability.
  *            @arg @ref RCC_LSEDRIVE_HIGH       LSE oscillator high drive capability.
  * @retval None
  */
#define __HAL_RCC_LSEDRIVE_CONFIG(__LSEDRIVE__) LL_RCC_LSE_SetDriveCapability((uint32_t)(__LSEDRIVE__))

/**
  * @brief  Macro to configure the wake up from stop clock.
  * @param  __STOPWUCLK__ specifies the clock source used after wake up from stop
  *         This parameter can be one of the following values:
  *            @arg @ref RCC_STOP_WAKEUPCLOCK_HSI HSI selected as system clock source
  *            @arg @ref RCC_STOP_WAKEUPCLOCK_MSI MSI selected as system clock source
  * @retval None
  */
#define __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(__STOPWUCLK__) LL_RCC_SetSysWakeUpClkSource((uint32_t)(__STOPWUCLK__))

/** @defgroup RCCEx_MCOx_Clock_Config RCC Extended MCOx Clock Config
  * @{
  */

/**
  * @brief  Macros to enable or disable the MCO1 clock output
  * @retval None
  */
#define __HAL_RCC_MCO1_ENABLE()     LL_RCC_EnableMCO(LL_RCC_MCO1)

#define __HAL_RCC_MCO1_DISABLE()    LL_RCC_DisableMCO(LL_RCC_MCO1)

/**
  * @brief  Macros to enable or disable the MCO2 clock output
  * @retval None
  */
#define __HAL_RCC_MCO2_ENABLE()     LL_RCC_EnableMCO(LL_RCC_MCO2)

#define __HAL_RCC_MCO2_DISABLE()    LL_RCC_DisableMCO(LL_RCC_MCO2)


/** @brief  Macro to configure the MCO1 clock.
  * @note   MCO1 clock output shall be enabled with __HAL_RCC_MCO1_ENABLE()
  * @note   The MCO switch to the new clock source only occurs when the previous clock source is active (dynamic switch).
  * @param  __MCOCLKSOURCE__ specifies the MCO clock source.
  *          This parameter can be one of the following values:
  *            @arg @ref RCC_MCO1SOURCE_HSI       HSI clock selected as MCO1 source
  *            @arg @ref RCC_MCO1SOURCE_LSE       LSE clock selected as MCO1 source
  *            @arg @ref RCC_MCO1SOURCE_MSI       MSI clock selected as MCO1 source
  *            @arg @ref RCC_MCO1SOURCE_LSI       LSI clock selected as MCO1 source
  *            @arg @ref RCC_MCO1SOURCE_HSE       HSE clock selected as MCO1 source
  *            @arg @ref RCC_MCO1SOURCE_IC5       IC5 clock selected as MCO1 source
  *            @arg @ref RCC_MCO1SOURCE_IC10      IC10 clock selected as MCO1 source
  *            @arg @ref RCC_MCO1SOURCE_SYSA      SYSA CPU clock selected as MCO1 source
  * @param  __MCODIV__ specifies the MCO clock prescaler.
  *          This parameter can be one of the following values:
  *            @arg @ref RCC_MCODIV_1 up to RCC_MCODIV_16  : divider applied to MCO1 clock
  */
#define __HAL_RCC_MCO1_CONFIG(__MCOCLKSOURCE__, __MCODIV__) LL_RCC_ConfigMCO(__MCOCLKSOURCE__, __MCODIV__)

/** @brief  Macro to configure the MCO2 clock.
  * @note   MCO2 clock output shall be enabled with __HAL_RCC_MCO2_ENABLE()
  * @note   The MCO switch to the new clock source only occurs when the previous clock source is active (dynamic switch).
  * @param  __MCOCLKSOURCE__ specifies the MCO clock source.
  *          This parameter can be one of the following values:
  *            @arg @ref RCC_MCO2SOURCE_HSI       HSI clock selected as MCO2 source
  *            @arg @ref RCC_MCO2SOURCE_LSE       LSE clock selected as MCO2 source
  *            @arg @ref RCC_MCO2SOURCE_MSI       MSI clock selected as MCO2 source
  *            @arg @ref RCC_MCO2SOURCE_LSI       LSI clock selected as MCO2 source
  *            @arg @ref RCC_MCO2SOURCE_HSE       HSE clock selected as MCO2 source
  *            @arg @ref RCC_MCO2SOURCE_IC15      IC15 clock selected as MCO2 source
  *            @arg @ref RCC_MCO2SOURCE_IC20      IC20 clock selected as MCO2 source
  *            @arg @ref RCC_MCO2SOURCE_SYSB      SYSB bus clock selected as MCO2 source
  * @param  __MCODIV__ specifies the MCO clock prescaler.
  *          This parameter can be one of the following values:
  *            @arg @ref RCC_MCODIV_1 up to RCC_MCODIV_16  : divider applied to MCO2 clock
  */
#define __HAL_RCC_MCO2_CONFIG(__MCOCLKSOURCE__, __MCODIV__) LL_RCC_ConfigMCO(__MCOCLKSOURCE__, __MCODIV__)

/**
  * @}
  */

/** @defgroup RCC_Flags_Interrupts_Management Flags Interrupts Management
  * @brief macros to manage the specified RCC Flags and interrupts.
  * @{
  */
/** @brief  Enable RCC interrupt.
  * @param  __INTERRUPT__ specifies the RCC interrupt source(s) to be enabled.
  *         This parameter can be any combination of the following values:
  *            @arg @ref RCC_IT_LSIRDY   LSI ready interrupt
  *            @arg @ref RCC_IT_LSERDY   LSE ready interrupt
  *            @arg @ref RCC_IT_HSIRDY   HSI ready interrupt
  *            @arg @ref RCC_IT_MSIRDY   MSI ready interrupt
  *            @arg @ref RCC_IT_HSERDY   HSE ready interrupt
  *            @arg @ref RCC_IT_PLL1RDY  PLL1 ready interrupt
  *            @arg @ref RCC_IT_PLL2RDY  PLL2 ready interrupt
  *            @arg @ref RCC_IT_PLL3RDY  PLL3 ready interrupt
  *            @arg @ref RCC_IT_PLL4RDY  PLL4 ready interrupt
  *            @arg @ref RCC_IT_LSECSS   LSE clock security system interrupt
  *            @arg @ref RCC_IT_HSECSS   HSE clock security system interrupt
  *            @arg @ref RCC_IT_WKUP     CPU wakeup interrupt
  */
#define __HAL_RCC_ENABLE_IT(__INTERRUPT__) SET_BIT(RCC->CIER, (__INTERRUPT__))

/** @brief  Disable RCC interrupt.
  * @param  __INTERRUPT__ specifies the RCC interrupt source(s) to be disabled.
  *         This parameter can be any combination of the following values:
  *            @arg @ref RCC_IT_LSIRDY   LSI ready interrupt
  *            @arg @ref RCC_IT_LSERDY   LSE ready interrupt
  *            @arg @ref RCC_IT_HSIRDY   HSI ready interrupt
  *            @arg @ref RCC_IT_MSIRDY   MSI ready interrupt
  *            @arg @ref RCC_IT_HSERDY   HSE ready interrupt
  *            @arg @ref RCC_IT_PLL1RDY  PLL1 ready interrupt
  *            @arg @ref RCC_IT_PLL2RDY  PLL2 ready interrupt
  *            @arg @ref RCC_IT_PLL3RDY  PLL3 ready interrupt
  *            @arg @ref RCC_IT_PLL4RDY  PLL4 ready interrupt
  *            @arg @ref RCC_IT_LSECSS   LSE clock security system interrupt
  *            @arg @ref RCC_IT_HSECSS   HSE clock security system interrupt
  *            @arg @ref RCC_IT_WKUP     CPU wakeup interrupt
  */
#define __HAL_RCC_DISABLE_IT(__INTERRUPT__) CLEAR_BIT(RCC->CIER, (__INTERRUPT__))

/** @brief  Clear the RCC's interrupt pending bits
  * @param  __INTERRUPT__ specifies the interrupt pending bit to clear.
  *         This parameter can be any combination of the following values:
  *            @arg @ref RCC_IT_LSIRDY   LSI ready interrupt
  *            @arg @ref RCC_IT_LSERDY   LSE ready interrupt
  *            @arg @ref RCC_IT_HSIRDY   HSI ready interrupt
  *            @arg @ref RCC_IT_MSIRDY   MSI ready interrupt
  *            @arg @ref RCC_IT_HSERDY   HSE ready interrupt
  *            @arg @ref RCC_IT_PLL1RDY  PLL1 ready interrupt
  *            @arg @ref RCC_IT_PLL2RDY  PLL2 ready interrupt
  *            @arg @ref RCC_IT_PLL3RDY  PLL3 ready interrupt
  *            @arg @ref RCC_IT_PLL4RDY  PLL4 ready interrupt
  *            @arg @ref RCC_IT_LSECSS   LSE clock security system interrupt
  *            @arg @ref RCC_IT_HSECSS   HSE clock security interrupt
  *            @arg @ref RCC_IT_WKUP     CPU wakeup interrupt
  */
#define __HAL_RCC_CLEAR_IT(__INTERRUPT__) WRITE_REG(RCC->CICR, (__INTERRUPT__))

/** @brief  Check whether the RCC interrupt has occurred or not.
  * @param  __INTERRUPT__ specifies the RCC interrupt source to check.
  *         This parameter can be one of the following values:
  *            @arg @ref RCC_IT_LSIRDY   LSI ready interrupt
  *            @arg @ref RCC_IT_LSERDY   LSE ready interrupt
  *            @arg @ref RCC_IT_HSIRDY   HSI ready interrupt
  *            @arg @ref RCC_IT_MSIRDY   MSI ready interrupt
  *            @arg @ref RCC_IT_HSERDY   HSE ready interrupt
  *            @arg @ref RCC_IT_PLL1RDY  PLL1 ready interrupt
  *            @arg @ref RCC_IT_PLL2RDY  PLL2 ready interrupt
  *            @arg @ref RCC_IT_PLL3RDY  PLL3 ready interrupt
  *            @arg @ref RCC_IT_PLL4RDY  PLL4 ready interrupt
  *            @arg @ref RCC_IT_LSECSS   LSE clock security system interrupt
  *            @arg @ref RCC_IT_HSECSS   HSE clock security interrupt
  *            @arg @ref RCC_IT_WKUP     CPU wakeup interrupt
  * @retval The pending state of __INTERRUPT__ (TRUE or FALSE).
  */
#define __HAL_RCC_GET_IT(__INTERRUPT__) (READ_BIT(RCC->CIFR, (__INTERRUPT__)) == (__INTERRUPT__))

/** @brief Set RMVF bit to clear the reset flags.
  *        The reset flags are: RCC_FLAG_LCKRST, RCC_FLAG_BORRST, RCC_FLAG_PINRST, RCC_FLAG_PORRST,
  *        RCC_FLAG_SFTRST, RCC_FLAG_IWDGRST, RCC_FLAG_WWDGRST and RCC_FLAG_LPWRRST.
 */
#define __HAL_RCC_CLEAR_RESET_FLAGS() LL_RCC_ClearResetFlags()

/** @brief  Check whether the selected RCC flag is set or not.
  * @param  __FLAG__ specifies the flag to check.
  *         This parameter can be one of the following values:
  *            @arg @ref RCC_FLAG_LCKRST   CPU lockup reset flag
  *            @arg @ref RCC_FLAG_BORRST   BOR reset flag
  *            @arg @ref RCC_FLAG_PINRST   Pin reset flag
  *            @arg @ref RCC_FLAG_PORRST   Power-on reset flag
  *            @arg @ref RCC_FLAG_SFTRST   Software reset flag
  *            @arg @ref RCC_FLAG_IWDGRST  Independent Watchdog reset flag
  *            @arg @ref RCC_FLAG_WWDGRST  Window Watchdog reset flag
  *            @arg @ref RCC_FLAG_LPWRRST  Low Power reset flag
  *            @arg @ref RCC_FLAG_LSECSSD  Clock security system failure on LSE oscillator detection flag
  *            @arg @ref RCC_FLAG_HSECSSD  Clock security system failure on HSE oscillator detection flag
  *            @arg @ref RCC_FLAG_LSIRDY   LSI oscillator clock ready flag
  *            @arg @ref RCC_FLAG_LSERDY   LSE oscillator clock ready flag
  *            @arg @ref RCC_FLAG_HSIRDY   HSI oscillator clock ready flag
  *            @arg @ref RCC_FLAG_MSIRDY   MSI oscillator clock ready flag
  *            @arg @ref RCC_FLAG_HSERDY   HSE oscillator clock ready flag
  *            @arg @ref RCC_FLAG_PLL1RDY  PLL1 ready flag
  *            @arg @ref RCC_FLAG_PLL2RDY  PLL2 ready flag
  *            @arg @ref RCC_FLAG_PLL3RDY  PLL3 ready flag
  *            @arg @ref RCC_FLAG_PLL4RDY  PLL4 ready flag
  * @retval The new state of __FLAG__ (TRUE or FALSE).
  */
#define __HAL_RCC_GET_FLAG(__FLAG__) ((((((((__FLAG__) >> 5U) == RCC_RSR_REG_INDEX)     ? RCC->RSR :                   \
                                          ((((__FLAG__) >> 5U) == RCC_LSECFGR_REG_INDEX) ? RCC->LSECFGR :              \
                                           ((((__FLAG__) >> 5U) == RCC_HSECFGR_REG_INDEX) ? RCC->HSECFGR : RCC->SR)))) \
                                        & (1UL << ((__FLAG__) & RCC_FLAG_POS_MASK))) != 0U) ? 1U : 0U)

/**
  * @}
  */

/** @defgroup RCC_Attributes_Management Attribute Management
  * @brief macros to manage the RCC Attributes.
  * @{
  */

/** @brief  Check whether an item attribute is secured
  * @param  __ATTRIBUTES__ specifies the item attributes
  *         This parameter is a combination of @ref RCC_attributes
  * @retval 1 if the item attribute is secured, 0 otherwise.
  */
#define HAL_RCC_ATTRIBUTES_IS_SEC(__ATTRIBUTES__) (((__ATTRIBUTES__ & RCC_ATTR_SEC) == RCC_ATTR_SEC) ? 1U : 0U)

/** @brief  Check whether an item attribute is privileged
  * @param  __ATTRIBUTES__ specifies the item attributes
  *         This parameter is a combination of @ref RCC_attributes
  * @retval 1 if the item attribute is privileged, 0 otherwise.
  */
#define HAL_RCC_ATTRIBUTES_IS_PRIV(__ATTRIBUTES__) (((__ATTRIBUTES__ & RCC_ATTR_PRIV) == RCC_ATTR_PRIV) ? 1U : 0U)

/** @brief  Check whether an item attribute is public
  * @param  __ATTRIBUTES__ specifies the item attributes
  *         This parameter is a combination of @ref RCC_attributes
  * @retval 1 if the item attribute is public, 0 otherwise.
  */
#define HAL_RCC_ATTRIBUTES_IS_PUB(__ATTRIBUTES__) (((__ATTRIBUTES__ & RCC_ATTR_PUB) == RCC_ATTR_PUB) ? 1U : 0U)

/** @brief  Check whether an item attribute is locked
  * @param  __ATTRIBUTES__ specifies the item attributes
  *         This parameter is a combination of @ref RCC_attributes
  * @retval 1 if the item attribute is locked, 0 otherwise.
  */
#define HAL_RCC_ATTRIBUTES_IS_LOCK(__ATTRIBUTES__) (((__ATTRIBUTES__ & RCC_ATTR_LOCK) == RCC_ATTR_LOCK) ? 1U : 0U)

/**
  * @}
  */

/**
  * @}
  */

/* Include RCC HAL Extension module */
#include "stm32n6xx_hal_rcc_ex.h"

/* Exported functions --------------------------------------------------------*/
/** @addtogroup RCC_Exported_Functions
  * @{
  */

/** @addtogroup RCC_Exported_Functions_Group1
  * @{
  */
/* Initialization and de-initialization functions  ******************************/
HAL_StatusTypeDef HAL_RCC_DeInit(void);
HAL_StatusTypeDef HAL_RCC_OscConfig(const RCC_OscInitTypeDef *pRCC_OscInitStruct);
HAL_StatusTypeDef HAL_RCC_ClockConfig(const RCC_ClkInitTypeDef *pRCC_ClkInitStruct);

/**
  * @}
  */

/** @addtogroup RCC_Exported_Functions_Group2
  * @{
  */
/* Peripheral Control functions  ************************************************/
void     HAL_RCC_MCOConfig(uint32_t RCC_MCOx, uint32_t RCC_MCOSource, uint32_t RCC_MCODiv);
void     HAL_RCC_EnableCSS(void);
uint32_t HAL_RCC_GetCpuClockFreq(void);
uint32_t HAL_RCC_GetSysClockFreq(void);
uint32_t HAL_RCC_GetHCLKFreq(void);
uint32_t HAL_RCC_GetPCLK1Freq(void);
uint32_t HAL_RCC_GetPCLK2Freq(void);
uint32_t HAL_RCC_GetPCLK4Freq(void);
uint32_t HAL_RCC_GetPCLK5Freq(void);
void     HAL_RCC_GetOscConfig(RCC_OscInitTypeDef *pRCC_OscInitStruct);
void     HAL_RCC_GetClockConfig(RCC_ClkInitTypeDef *pRCC_ClkInitStruct);
/* CSS NMI IRQ handler */
void     HAL_RCC_NMI_IRQHandler(void);
/* User callback in non blocking mode (IT mode) */
void     HAL_RCC_CSSCallback(void);

/**
  * @}
  */

/** @addtogroup RCC_Exported_Functions_Group3
  * @{
  */
/* Attributes management functions ********************************************/
void HAL_RCC_ConfigAttributes(uint32_t Item, uint32_t Attributes);
HAL_StatusTypeDef HAL_RCC_GetConfigAttributes(uint32_t Item, uint32_t *pAttributes);

/**
  * @}
  */

/**
  * @}
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @defgroup RCC_Private_Constants RCC Private Constants
  * @{
  */
#define RCC_LSE_TIMEOUT_VALUE     LSE_STARTUP_TIMEOUT
#define RCC_PLL_TIMEOUT_VALUE     1U    /* 1 ms */

/* Defines used for Flags */
#define RCC_SR_REG_INDEX          1U
#define RCC_LSECFGR_REG_INDEX     2U
#define RCC_HSECFGR_REG_INDEX     3U
#define RCC_RSR_REG_INDEX         4U
#define RCC_FLAG_POS_MASK         0x0000001FU

/* Defines RCC privilege/secure/public/lock attribute masks */
#define RCC_ATTR_PRIV_MASK     (0x00000002U)    /* RCC privilege mask               */
#define RCC_ATTR_SEC_MASK      (0x00000008U)    /* RCC secure mask                  */
#define RCC_ATTR_PUB_MASK      (0x00000020U)    /* RCC public mask                  */
#define RCC_ATTR_LOCK_MASK     (0x00000080U)    /* RCC lock mask                    */

/* Defines RCC item groups masks */
/* Item ID are masked with item group to get an unique ID
   31           24            16           8             0
   --------------------------------------------------------
   | Item Group |               Item                       |
   --------------------------------------------------------*/
#define RCC_ITEM_GROUP_POS          24UL
#define RCC_ITEM_GROUP_MASK         0xFF000000UL
#define RCC_ITEM_MASK               0x00FFFFFFUL
#define RCC_ITEM_GROUP_OSC          0x01000000UL
#define RCC_ITEM_GROUP_PLL          0x02000000UL
#define RCC_ITEM_GROUP_IC           0x04000000UL
#define RCC_ITEM_GROUP_SYSCFG       0x08000000UL
#define RCC_ITEM_GROUP_BUS          0x10000000UL
#define RCC_ITEM_GROUP_MEM          0x20000000UL

#define RCC_ITEM_GROUP_OSC_MASK     0x0000001FUL
#define RCC_ITEM_GROUP_PLL_MASK     0x0000000FUL
#define RCC_ITEM_GROUP_IC_MASK      0x000FFFFFUL
#define RCC_ITEM_GROUP_SYSCFG_MASK  0x0000003FUL
#define RCC_ITEM_GROUP_BUS_MASK     0x00003FFFUL
#define RCC_ITEM_GROUP_MEM_MASK     0x00000FFFUL

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @addtogroup RCC_Private_Macros
  * @{
  */

/** @defgroup RCC_IS_RCC_Definitions RCC Private macros to check input parameters
  * @{
  */

#define IS_RCC_OSCILLATORTYPE(__OSCILLATOR__) (((__OSCILLATOR__) == RCC_OSCILLATORTYPE_NONE)                           || \
                                               (((__OSCILLATOR__) & RCC_OSCILLATORTYPE_HSE) == RCC_OSCILLATORTYPE_HSE) || \
                                               (((__OSCILLATOR__) & RCC_OSCILLATORTYPE_HSI) == RCC_OSCILLATORTYPE_HSI) || \
                                               (((__OSCILLATOR__) & RCC_OSCILLATORTYPE_MSI) == RCC_OSCILLATORTYPE_MSI) || \
                                               (((__OSCILLATOR__) & RCC_OSCILLATORTYPE_LSI) == RCC_OSCILLATORTYPE_LSI) || \
                                               (((__OSCILLATOR__) & RCC_OSCILLATORTYPE_LSE) == RCC_OSCILLATORTYPE_LSE))

#define IS_RCC_HSE(__HSE__) (((__HSE__) == RCC_HSE_OFF)    || ((__HSE__) == RCC_HSE_ON) || \
                             ((__HSE__) == RCC_HSE_BYPASS) || ((__HSE__) == RCC_HSE_BYPASS_DIGITAL))

#define IS_RCC_LSE(__LSE__) (((__LSE__) == RCC_LSE_OFF)    || ((__LSE__) == RCC_LSE_ON) || \
                             ((__LSE__) == RCC_LSE_BYPASS) || ((__LSE__) == RCC_LSE_BYPASS_DIGITAL))

#define IS_RCC_HSI(__HSI__) (((__HSI__) == RCC_HSI_OFF)     || ((__HSI__) == RCC_HSI_ON))

#define IS_RCC_HSI_DIV(__HSI__) (((__HSI__) == RCC_HSI_DIV1) || ((__HSI__) == RCC_HSI_DIV2) || \
                                 ((__HSI__) == RCC_HSI_DIV4) || ((__HSI__) == RCC_HSI_DIV8))

#define IS_RCC_LSI(__LSI__) (((__LSI__) == RCC_LSI_OFF) || ((__LSI__) == RCC_LSI_ON))

#define IS_RCC_MSI(__MSI__) (((__MSI__) == RCC_MSI_OFF) || ((__MSI__) == RCC_MSI_ON))

#define IS_RCC_MSI_FREQUENCY(__FREQUENCY__) (((__FREQUENCY__) == RCC_MSI_FREQ_4MHZ) ||\
                                             ((__FREQUENCY__) == RCC_MSI_FREQ_16MHZ))

#define IS_RCC_PLL(__PLL__) (((__PLL__) == RCC_PLL_NONE) || \
                             ((__PLL__) == RCC_PLL_OFF)  || \
                             ((__PLL__) == RCC_PLL_ON)   || \
                             ((__PLL__) == RCC_PLL_BYPASS))

#define IS_RCC_PLLSOURCE(__SOURCE__) (((__SOURCE__) == RCC_PLLSOURCE_HSI) || \
                                      ((__SOURCE__) == RCC_PLLSOURCE_MSI) || \
                                      ((__SOURCE__) == RCC_PLLSOURCE_HSE) || \
                                      ((__SOURCE__) == RCC_PLLSOURCE_PIN))

#define IS_RCC_PLLM_VALUE(__VALUE__) ((1U <= (__VALUE__)) && ((__VALUE__) <= 63U))

#define IS_RCC_PLLN_VALUE(__VALUE__) ((10U <= (__VALUE__)) && ((__VALUE__) <= 2500U))

#define IS_RCC_PLLP_VALUE(__VALUE__) ((1U <= (__VALUE__)) && ((__VALUE__) <= 7U))

#define IS_RCC_PLLFRACN_VALUE(__VALUE__) ((__VALUE__) <= (RCC_PLL1CFGR2_PLL1DIVNFRAC >> RCC_PLL1CFGR2_PLL1DIVNFRAC_Pos))

#define IS_RCC_CLOCKTYPE(__CLK__) ((__CLK__) <= 0x7FU)

#define IS_RCC_CPUCLKSOURCE(__SOURCE__) (((__SOURCE__) == RCC_CPUCLKSOURCE_HSI) || \
                                         ((__SOURCE__) == RCC_CPUCLKSOURCE_MSI) || \
                                         ((__SOURCE__) == RCC_CPUCLKSOURCE_HSE) || \
                                         ((__SOURCE__) == RCC_CPUCLKSOURCE_IC1))

#define IS_RCC_SYSCLKSOURCE(__SOURCE__) (((__SOURCE__) == RCC_SYSCLKSOURCE_HSI) || \
                                         ((__SOURCE__) == RCC_SYSCLKSOURCE_MSI) || \
                                         ((__SOURCE__) == RCC_SYSCLKSOURCE_HSE) || \
                                         ((__SOURCE__) == RCC_SYSCLKSOURCE_IC2_IC6_IC11))

#define IS_RCC_HCLK(__HCLK__) (((__HCLK__) == RCC_HCLK_DIV1)   || ((__HCLK__) == RCC_HCLK_DIV2)  || \
                               ((__HCLK__) == RCC_HCLK_DIV4)   || ((__HCLK__) == RCC_HCLK_DIV8)  || \
                               ((__HCLK__) == RCC_HCLK_DIV16)  || ((__HCLK__) == RCC_HCLK_DIV32) || \
                               ((__HCLK__) == RCC_HCLK_DIV64)  || ((__HCLK__) == RCC_HCLK_DIV128))

#define IS_RCC_PCLK1(__PCLK1__) (((__PCLK1__) == RCC_APB1_DIV1)  || ((__PCLK1__) == RCC_APB1_DIV2)  || \
                                 ((__PCLK1__) == RCC_APB1_DIV4)  || ((__PCLK1__) == RCC_APB1_DIV8)  || \
                                 ((__PCLK1__) == RCC_APB1_DIV16) || ((__PCLK1__) == RCC_APB1_DIV32) || \
                                 ((__PCLK1__) == RCC_APB1_DIV64) || ((__PCLK1__) == RCC_APB1_DIV128))

#define IS_RCC_PCLK2(__PCLK2__) (((__PCLK2__) == RCC_APB2_DIV1)  || ((__PCLK2__) == RCC_APB2_DIV2) || \
                                 ((__PCLK2__) == RCC_APB2_DIV4)  || ((__PCLK2__) == RCC_APB2_DIV8) || \
                                 ((__PCLK2__) == RCC_APB2_DIV16) || ((__PCLK2__) == RCC_APB2_DIV32) || \
                                 ((__PCLK2__) == RCC_APB2_DIV64) || ((__PCLK2__) == RCC_APB2_DIV128))

#define IS_RCC_PCLK4(__PCLK4__) (((__PCLK4__) == RCC_APB4_DIV1)  || ((__PCLK4__) == RCC_APB4_DIV2) || \
                                 ((__PCLK4__) == RCC_APB4_DIV4)  || ((__PCLK4__) == RCC_APB4_DIV8) || \
                                 ((__PCLK4__) == RCC_APB4_DIV16) || ((__PCLK4__) == RCC_APB4_DIV32) || \
                                 ((__PCLK4__) == RCC_APB4_DIV64) || ((__PCLK4__) == RCC_APB4_DIV128))

#define IS_RCC_PCLK5(__PCLK5__) (((__PCLK5__) == RCC_APB5_DIV1)  || ((__PCLK5__) == RCC_APB5_DIV2) || \
                                 ((__PCLK5__) == RCC_APB5_DIV4)  || ((__PCLK5__) == RCC_APB5_DIV8) || \
                                 ((__PCLK5__) == RCC_APB5_DIV16) || ((__PCLK5__) == RCC_APB5_DIV32) || \
                                 ((__PCLK5__) == RCC_APB5_DIV64) || ((__PCLK5__) == RCC_APB5_DIV128))

#define IS_RCC_RTCCLKSOURCE(__SOURCE__) (((__SOURCE__) == RCC_RTCCLKSOURCE_LSE)       || ((__SOURCE__) == RCC_RTCCLKSOURCE_LSI)       || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV1)  || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV2)  || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV3)  || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV4)  || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV5)  || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV6)  || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV7)  || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV8)  || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV9)  || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV10) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV11) || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV12) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV13) || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV14) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV15) || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV16) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV17) || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV18) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV19) || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV20) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV21) || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV22) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV23) || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV24) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV25) || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV26) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV27) || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV28) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV29) || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV30) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV31) || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV32) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV33) || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV34) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV35) || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV36) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV37) || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV38) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV39) || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV40) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV41) || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV42) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV43) || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV44) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV45) || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV46) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV47) || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV48) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV49) || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV50) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV51) || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV52) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV53) || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV54) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV55) || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV56) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV57) || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV58) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV59) || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV60) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV61) || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV62) || \
                                         ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV63) || ((__SOURCE__) == RCC_RTCCLKSOURCE_HSE_DIV64))

#define IS_RCC_MCO(__MCOX__) (((__MCOX__) == RCC_MCO1) || ((__MCOX__) == RCC_MCO2))

#define IS_RCC_MCO1SOURCE(__SOURCE__) (((__SOURCE__) == RCC_MCO1SOURCE_LSI)  || ((__SOURCE__) == RCC_MCO1SOURCE_LSE)  || \
                                       ((__SOURCE__) == RCC_MCO1SOURCE_MSI)  || ((__SOURCE__) == RCC_MCO1SOURCE_HSI)  || \
                                       ((__SOURCE__) == RCC_MCO1SOURCE_HSE)  || ((__SOURCE__) == RCC_MCO1SOURCE_IC5)  || \
                                       ((__SOURCE__) == RCC_MCO1SOURCE_IC10) || ((__SOURCE__) == RCC_MCO1SOURCE_SYSA))

#define IS_RCC_MCO2SOURCE(__SOURCE__) (((__SOURCE__) == RCC_MCO2SOURCE_LSI)  || ((__SOURCE__) == RCC_MCO2SOURCE_LSE)  || \
                                       ((__SOURCE__) == RCC_MCO2SOURCE_MSI)  || ((__SOURCE__) == RCC_MCO2SOURCE_HSI)  || \
                                       ((__SOURCE__) == RCC_MCO2SOURCE_HSE)  || ((__SOURCE__) == RCC_MCO2SOURCE_IC15) || \
                                       ((__SOURCE__) == RCC_MCO2SOURCE_IC20) || ((__SOURCE__) == RCC_MCO2SOURCE_SYSB))

#define IS_RCC_MCODIV(__DIV__) (((__DIV__) == RCC_MCODIV_1)  || ((__DIV__) == RCC_MCODIV_2)   || \
                                ((__DIV__) == RCC_MCODIV_3)  || ((__DIV__) == RCC_MCODIV_4)   || \
                                ((__DIV__) == RCC_MCODIV_5)  || ((__DIV__) == RCC_MCODIV_6)   || \
                                ((__DIV__) == RCC_MCODIV_7)  || ((__DIV__) == RCC_MCODIV_8)   || \
                                ((__DIV__) == RCC_MCODIV_9)  || ((__DIV__) == RCC_MCODIV_10)  || \
                                ((__DIV__) == RCC_MCODIV_11) || ((__DIV__) == RCC_MCODIV_12)  || \
                                ((__DIV__) == RCC_MCODIV_13) || ((__DIV__) == RCC_MCODIV_14)  || \
                                ((__DIV__) == RCC_MCODIV_15) || ((__DIV__) == RCC_MCODIV_16))

#define IS_RCC_HSI_CALIBRATION_VALUE(__VALUE__) ((__VALUE__) <= 0x7FU)

#define IS_RCC_MSI_CALIBRATION_VALUE(__VALUE__) ((__VALUE__) <= 0x1FU)

#define IS_RCC_STOP_WAKEUPCLOCK(__SOURCE__) (((__SOURCE__) == RCC_STOP_WAKEUPCLOCK_MSI) || \
                                             ((__SOURCE__) == RCC_STOP_WAKEUPCLOCK_HSI))

#define IS_RCC_ICCLKSOURCE(__SOURCE__) (((__SOURCE__) == RCC_ICCLKSOURCE_PLL1) || \
                                        ((__SOURCE__) == RCC_ICCLKSOURCE_PLL2) || \
                                        ((__SOURCE__) == RCC_ICCLKSOURCE_PLL3)  || \
                                        ((__SOURCE__) == RCC_ICCLKSOURCE_PLL4))

#define IS_RCC_ICCLKDIVIDER(__VALUE__) ((1U <= (__VALUE__)) && ((__VALUE__) <= 256U))

#define IS_RCC_ITEM_ATTRIBUTES(ITEM)  ((((ITEM) & RCC_ITEM_ALL) != 0U) && (((ITEM) & ~RCC_ITEM_ALL) == 0U))

#define IS_RCC_SINGLE_ITEM_ATTRIBUTES(ITEM) (((ITEM) == RCC_ITEM_LSI)         || \
                                             ((ITEM) == RCC_ITEM_LSE)         || \
                                             ((ITEM) == RCC_ITEM_MSI)         || \
                                             ((ITEM) == RCC_ITEM_HSI)         || \
                                             ((ITEM) == RCC_ITEM_HSE)         || \
                                             ((ITEM) == RCC_ITEM_PLL1)        || \
                                             ((ITEM) == RCC_ITEM_PLL2)        || \
                                             ((ITEM) == RCC_ITEM_PLL3)        || \
                                             ((ITEM) == RCC_ITEM_PLL4)        || \
                                             ((ITEM) == RCC_ITEM_IC1)         || \
                                             ((ITEM) == RCC_ITEM_IC2)         || \
                                             ((ITEM) == RCC_ITEM_IC3)         || \
                                             ((ITEM) == RCC_ITEM_IC4)         || \
                                             ((ITEM) == RCC_ITEM_IC5)         || \
                                             ((ITEM) == RCC_ITEM_IC6)         || \
                                             ((ITEM) == RCC_ITEM_IC7)         || \
                                             ((ITEM) == RCC_ITEM_IC8)         || \
                                             ((ITEM) == RCC_ITEM_IC9)         || \
                                             ((ITEM) == RCC_ITEM_IC10)        || \
                                             ((ITEM) == RCC_ITEM_IC11)        || \
                                             ((ITEM) == RCC_ITEM_IC12)        || \
                                             ((ITEM) == RCC_ITEM_IC13)        || \
                                             ((ITEM) == RCC_ITEM_IC14)        || \
                                             ((ITEM) == RCC_ITEM_IC15)        || \
                                             ((ITEM) == RCC_ITEM_IC16)        || \
                                             ((ITEM) == RCC_ITEM_IC17)        || \
                                             ((ITEM) == RCC_ITEM_IC18)        || \
                                             ((ITEM) == RCC_ITEM_IC19)        || \
                                             ((ITEM) == RCC_ITEM_IC20)        || \
                                             ((ITEM) == RCC_ITEM_MOD)         || \
                                             ((ITEM) == RCC_ITEM_SYS)         || \
                                             ((ITEM) == RCC_ITEM_BUS)         || \
                                             ((ITEM) == RCC_ITEM_PER)         || \
                                             ((ITEM) == RCC_ITEM_INT)         || \
                                             ((ITEM) == RCC_ITEM_RST)         || \
                                             ((ITEM) == RCC_ITEM_ACLKN)       || \
                                             ((ITEM) == RCC_ITEM_ACLKNC)      || \
                                             ((ITEM) == RCC_ITEM_AHBM)        || \
                                             ((ITEM) == RCC_ITEM_AHB1)        || \
                                             ((ITEM) == RCC_ITEM_AHB2)        || \
                                             ((ITEM) == RCC_ITEM_AHB3)        || \
                                             ((ITEM) == RCC_ITEM_AHB4)        || \
                                             ((ITEM) == RCC_ITEM_AHB5)        || \
                                             ((ITEM) == RCC_ITEM_APB1)        || \
                                             ((ITEM) == RCC_ITEM_APB2)        || \
                                             ((ITEM) == RCC_ITEM_APB3)        || \
                                             ((ITEM) == RCC_ITEM_APB4)        || \
                                             ((ITEM) == RCC_ITEM_APB5)        || \
                                             ((ITEM) == RCC_ITEM_NOC)         || \
                                             ((ITEM) == RCC_ITEM_AXISRAM3)    || \
                                             ((ITEM) == RCC_ITEM_AXISRAM4)    || \
                                             ((ITEM) == RCC_ITEM_AXISRAM5)    || \
                                             ((ITEM) == RCC_ITEM_AXISRAM6)    || \
                                             ((ITEM) == RCC_ITEM_AHBSRAM1)    || \
                                             ((ITEM) == RCC_ITEM_AHBSRAM2)    || \
                                             ((ITEM) == RCC_ITEM_BKPSRAM)     || \
                                             ((ITEM) == RCC_ITEM_AXISRAM1)    || \
                                             ((ITEM) == RCC_ITEM_AXISRAM2)    || \
                                             ((ITEM) == RCC_ITEM_FLEXRAM)     || \
                                             ((ITEM) == RCC_ITEM_CACHEAXIRAM) || \
                                             ((ITEM) == RCC_ITEM_VENCRAM))


#if defined (CPU_IN_SECURE_STATE)
#define IS_RCC_ATTRIBUTES(ATTRIBUTE) (((((ATTRIBUTE) & RCC_ATTR_SEC) == RCC_ATTR_SEC)        || \
                                       (((ATTRIBUTE) & RCC_ATTR_NSEC) == RCC_ATTR_NSEC)      || \
                                       (((ATTRIBUTE) & RCC_ATTR_PRIV) == RCC_ATTR_PRIV)      || \
                                       (((ATTRIBUTE) & RCC_ATTR_NPRIV) == RCC_ATTR_NPRIV)    || \
                                       (((ATTRIBUTE) & RCC_ATTR_PUB)  == RCC_ATTR_PUB)       || \
                                       (((ATTRIBUTE) & RCC_ATTR_NPUB) == RCC_ATTR_NPUB)      || \
                                       (((ATTRIBUTE) & RCC_ATTR_LOCK) == RCC_ATTR_LOCK))     && \
                                      (((ATTRIBUTE) & ~(RCC_ATTR_SEC|RCC_ATTR_NSEC|RCC_ATTR_PRIV|RCC_ATTR_NPRIV|\
                                                        RCC_ATTR_PUB|RCC_ATTR_NPUB|RCC_ATTR_LOCK)) == 0U))
#else
#define IS_RCC_ATTRIBUTES(ATTRIBUTE) (((((ATTRIBUTE) & RCC_ATTR_PRIV) == RCC_ATTR_PRIV) || \
                                       (((ATTRIBUTE) & RCC_ATTR_NPRIV) == RCC_ATTR_NPRIV)) && \
                                      (((ATTRIBUTE) & ~(RCC_ATTR_PRIV|RCC_ATTR_NPRIV)) == 0U))

#endif /* CPU_IN_SECURE_STATE */


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

#endif /* STM32N6xx_HAL_RCC_H */
