/**
  ******************************************************************************
  * @file    stm32n6xx_ll_rcc.h
  * @author  MCD Application Team
  * @brief   Header file of RCC LL module.
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
#ifndef STM32N6xx_LL_RCC_H
#define STM32N6xx_LL_RCC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx.h"
#include <math.h>

/** @addtogroup STM32N6xx_LL_Driver
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

/**
  * @}
  */

/* Private constants ---------------------------------------------------------*/
/** @defgroup RCC_LL_Private_Constants RCC Private Constants
  * @{
  */
/* Constants for LL_CLKSOURCE_xxx() macros
   31           24            16           8             0
   --------------------------------------------------------
   | Mask        | ClkSource   |  Bit       | Register    |
   |             |  Config     | Position   | Offset      |
   --------------------------------------------------------*/

/* Clock source register offset Vs CCIPR1 register */
#define CCIPR1_OFFSET      0x0UL
#define CCIPR2_OFFSET      0x4UL
#define CCIPR3_OFFSET      0x8UL
#define CCIPR4_OFFSET      0xCUL
#define CCIPR5_OFFSET      0x10UL
#define CCIPR6_OFFSET      0x14UL
#define CCIPR7_OFFSET      0x18UL
#define CCIPR8_OFFSET      0x1CUL
#define CCIPR9_OFFSET      0x20UL
#define CCIPR12_OFFSET     0x2CUL
#define CCIPR13_OFFSET     0x30UL
#define CCIPR14_OFFSET     0x34UL

#define LL_RCC_REG_SHIFT     0U
#define LL_RCC_POS_SHIFT     8U
#define LL_RCC_CONFIG_SHIFT  16U
#define LL_RCC_MASK_SHIFT    24U
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup RCC_LL_Private_Macros RCC Private Macros
  * @{
  */
#if !defined(UNUSED)
#define UNUSED(x) ((void)(x))
#endif /* UNUSED */

#define LL_CLKSOURCE_SHIFT(__CLKSOURCE__)   (((__CLKSOURCE__) >> LL_RCC_POS_SHIFT   ) & 0x1FUL)

#define LL_CLKSOURCE_MASK(__CLKSOURCE__)   ((((__CLKSOURCE__) >> LL_RCC_MASK_SHIFT  ) &\
                                             0xFFUL) << LL_CLKSOURCE_SHIFT(__CLKSOURCE__))

#define LL_CLKSOURCE_CONFIG(__CLKSOURCE__) ((((__CLKSOURCE__) >> LL_RCC_CONFIG_SHIFT) &\
                                             0xFFUL) << LL_CLKSOURCE_SHIFT(__CLKSOURCE__))

#define LL_CLKSOURCE_REG(__CLKSOURCE__)     (((__CLKSOURCE__) >> LL_RCC_REG_SHIFT   ) & 0xFFUL)

#define LL_CLKSOURCE(__REG__, __MSK__, __POS__, __CLK__) ((uint32_t)((((__MSK__) >> (__POS__)) << LL_RCC_MASK_SHIFT) | \
                                                                     (( __POS__              ) << LL_RCC_POS_SHIFT)  | \
                                                                     (( __REG__              ) << LL_RCC_REG_SHIFT)  | \
                                                                     (((__CLK__) >> (__POS__)) << LL_RCC_CONFIG_SHIFT)))
/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/
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
  uint32_t SYSCLK_Frequency;
  uint32_t CPUCLK_Frequency;
  uint32_t HCLK_Frequency;
  uint32_t PCLK1_Frequency;
  uint32_t PCLK2_Frequency;
  uint32_t PCLK4_Frequency;
  uint32_t PCLK5_Frequency;
} LL_RCC_ClocksTypeDef;

/**
  * @}
  */

/**
  * @}
  */

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
#define HSE_VALUE    48000000U  /*!< Value of the HSE oscillator in Hz */
#endif /* HSE_VALUE */

#if !defined  (HSI_VALUE)
#define HSI_VALUE    64000000U  /*!< Value of the HSI oscillator in Hz */
#endif /* HSI_VALUE */

#if !defined  (MSI_VALUE)
#define MSI_VALUE    4000000U   /*!< Value of the MSI oscillator in Hz */
#endif /* MSI_VALUE */

#if !defined  (LSE_VALUE)
#define LSE_VALUE    32768U     /*!< Value of the LSE oscillator in Hz */
#endif /* LSE_VALUE */

#if !defined  (LSI_VALUE)
#define LSI_VALUE    32000U     /*!< Value of the LSI oscillator in Hz */
#endif /* LSI_VALUE */

#if !defined  (EXTERNAL_CLOCK_VALUE)
#define EXTERNAL_CLOCK_VALUE    12288000U /*!< Value of the I2S_CKIN external oscillator in Hz */
#endif /* EXTERNAL_CLOCK_VALUE */

/**
  * @}
  */

/** @defgroup RCC_LL_EC_HSIDIV  HSI oscillator divider
  * @{
  */
#define LL_RCC_HSI_DIV_1                   0U
#define LL_RCC_HSI_DIV_2                   RCC_HSICFGR_HSIDIV_0
#define LL_RCC_HSI_DIV_4                   RCC_HSICFGR_HSIDIV_1
#define LL_RCC_HSI_DIV_8                   RCC_HSICFGR_HSIDIV
/**
  * @}
  */

/** @defgroup RCC_LL_EC_MSIFREQ  MSI oscillator frequency select
  * @{
  */
#define LL_RCC_MSI_FREQ_4MHZ               0U
#define LL_RCC_MSI_FREQ_16MHZ              RCC_MSICFGR_MSIFREQSEL
/**
  * @}
  */

/** @defgroup RCC_LL_EC_LSEDRIVE  LSE oscillator drive capability
  * @{
  */
#define LL_RCC_LSEDRIVE_LOW                0U
#define LL_RCC_LSEDRIVE_MEDIUMLOW          RCC_LSECFGR_LSEDRV_1
#define LL_RCC_LSEDRIVE_MEDIUMHIGH         RCC_LSECFGR_LSEDRV_0
#define LL_RCC_LSEDRIVE_HIGH               RCC_LSECFGR_LSEDRV
/**
  * @}
  */

/** @defgroup RCC_LL_EC_HSECSSBYP_DIV  HSI divider to apply for replacement of HSE on CSS bypass
  * @{
  */
#define LL_RCC_HSECSSBYP_DIV_1             0U
#define LL_RCC_HSECSSBYP_DIV_2             RCC_HSECFGR_HSECSSBPRE_0
#define LL_RCC_HSECSSBYP_DIV_3             RCC_HSECFGR_HSECSSBPRE_1
#define LL_RCC_HSECSSBYP_DIV_4             (RCC_HSECFGR_HSECSSBPRE_1 | RCC_HSECFGR_HSECSSBPRE_0)
#define LL_RCC_HSECSSBYP_DIV_5             RCC_HSECFGR_HSECSSBPRE_2
#define LL_RCC_HSECSSBYP_DIV_6             (RCC_HSECFGR_HSECSSBPRE_2 | RCC_HSECFGR_HSECSSBPRE_0)
#define LL_RCC_HSECSSBYP_DIV_7             (RCC_HSECFGR_HSECSSBPRE_2 | RCC_HSECFGR_HSECSSBPRE_1)
#define LL_RCC_HSECSSBYP_DIV_8             (RCC_HSECFGR_HSECSSBPRE_2 |\
                                            RCC_HSECFGR_HSECSSBPRE_1 | RCC_HSECFGR_HSECSSBPRE_0)
#define LL_RCC_HSECSSBYP_DIV_9             RCC_HSECFGR_HSECSSBPRE_3
#define LL_RCC_HSECSSBYP_DIV_10            (RCC_HSECFGR_HSECSSBPRE_3 | RCC_HSECFGR_HSECSSBPRE_0)
#define LL_RCC_HSECSSBYP_DIV_11            (RCC_HSECFGR_HSECSSBPRE_3 | RCC_HSECFGR_HSECSSBPRE_1)
#define LL_RCC_HSECSSBYP_DIV_12            (RCC_HSECFGR_HSECSSBPRE_3 |\
                                            RCC_HSECFGR_HSECSSBPRE_1 | RCC_HSECFGR_HSECSSBPRE_0)
#define LL_RCC_HSECSSBYP_DIV_13            (RCC_HSECFGR_HSECSSBPRE_3 | RCC_HSECFGR_HSECSSBPRE_2)
#define LL_RCC_HSECSSBYP_DIV_14            (RCC_HSECFGR_HSECSSBPRE_3 |\
                                            RCC_HSECFGR_HSECSSBPRE_2 | RCC_HSECFGR_HSECSSBPRE_0)
#define LL_RCC_HSECSSBYP_DIV_15            (RCC_HSECFGR_HSECSSBPRE_3 |\
                                            RCC_HSECFGR_HSECSSBPRE_2 | RCC_HSECFGR_HSECSSBPRE_1)
#define LL_RCC_HSECSSBYP_DIV_16            (RCC_HSECFGR_HSECSSBPRE_3 | RCC_HSECFGR_HSECSSBPRE_2 |\
                                            RCC_HSECFGR_HSECSSBPRE_1 | RCC_HSECFGR_HSECSSBPRE_0)
/**
  * @}
  */


/** @defgroup RCC_LL_EC_CPU_CLKSOURCE  CPU clock switch
  * @{
  */
#define LL_RCC_CPU_CLKSOURCE_HSI           0U                                       /*!< Select HSI as CPU clock */
#define LL_RCC_CPU_CLKSOURCE_MSI           RCC_CFGR1_CPUSW_0                        /*!< Select MSI as CPU clock */
#define LL_RCC_CPU_CLKSOURCE_HSE           RCC_CFGR1_CPUSW_1                        /*!< Select HSE as CPU clock */
#define LL_RCC_CPU_CLKSOURCE_IC1           (RCC_CFGR1_CPUSW_1 | RCC_CFGR1_CPUSW_0)  /*!< Select IC1 as CPU clock */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_CPU_CLKSOURCE_STATUS  CPU clock switch status
  * @{
  */
#define LL_RCC_CPU_CLKSOURCE_STATUS_HSI    0U                                        /*!< HSI used as CPU clock */
#define LL_RCC_CPU_CLKSOURCE_STATUS_MSI    RCC_CFGR1_CPUSWS_0                        /*!< MSI used as CPU clock */
#define LL_RCC_CPU_CLKSOURCE_STATUS_HSE    RCC_CFGR1_CPUSWS_1                        /*!< HSE used as CPU clock */
#define LL_RCC_CPU_CLKSOURCE_STATUS_IC1    (RCC_CFGR1_CPUSWS_1 | RCC_CFGR1_CPUSWS_0) /*!< IC1 used as CPU clock */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_SYS_CLKSOURCE  System bus clock switch
  * @{
  */
#define LL_RCC_SYS_CLKSOURCE_HSI           0U                                        /*!< Select HSI as system bus clocks */
#define LL_RCC_SYS_CLKSOURCE_MSI           RCC_CFGR1_SYSSW_0                         /*!< Select MSI as system bus clocks */
#define LL_RCC_SYS_CLKSOURCE_HSE           RCC_CFGR1_SYSSW_1                         /*!< Select HSE as system bus clocks */
#define LL_RCC_SYS_CLKSOURCE_IC2_IC6_IC11  (RCC_CFGR1_SYSSW_1 | RCC_CFGR1_SYSSW_0)   /*!< Select IC2/IC6/IC11 as system bus clocks */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_SYS_CLKSOURCE_STATUS  System bus clock switch status
  * @{
  */
#define LL_RCC_SYS_CLKSOURCE_STATUS_HSI    0U                                        /*!< HSI used as system bus clocks */
#define LL_RCC_SYS_CLKSOURCE_STATUS_MSI    RCC_CFGR1_SYSSWS_0                        /*!< MSI used as system bus clocks */
#define LL_RCC_SYS_CLKSOURCE_STATUS_HSE    RCC_CFGR1_SYSSWS_1                        /*!< HSE used as system bus clocks */
#define LL_RCC_SYS_CLKSOURCE_STATUS_IC2_IC6_IC11 (RCC_CFGR1_SYSSWS_1 | RCC_CFGR1_SYSSWS_0) /*!< IC2/IC6/IC11 used as system bus clocks */
/**
  * @}
  */

/** @defgroup RCC_LL_EC_SYSWAKEUP_CLKSOURCE  System wakeup clock source
  * @{
  */
#define LL_RCC_SYSWAKEUP_CLKSOURCE_HSI     0U
#define LL_RCC_SYSWAKEUP_CLKSOURCE_MSI     RCC_CFGR1_STOPWUCK
/**
  * @}
  */

/** @defgroup RCC_LL_EC_AHB_DIV  AHB prescaler
  * @{
  */
#define LL_RCC_AHB_DIV_1                   0U
#define LL_RCC_AHB_DIV_2                   RCC_CFGR2_HPRE_0
#define LL_RCC_AHB_DIV_4                   RCC_CFGR2_HPRE_1
#define LL_RCC_AHB_DIV_8                   (RCC_CFGR2_HPRE_1 | RCC_CFGR2_HPRE_0)
#define LL_RCC_AHB_DIV_16                  RCC_CFGR2_HPRE_2
#define LL_RCC_AHB_DIV_32                  (RCC_CFGR2_HPRE_2 | RCC_CFGR2_HPRE_0)
#define LL_RCC_AHB_DIV_64                  (RCC_CFGR2_HPRE_2 | RCC_CFGR2_HPRE_1)
#define LL_RCC_AHB_DIV_128                 (RCC_CFGR2_HPRE_2 | RCC_CFGR2_HPRE_1 | RCC_CFGR2_HPRE_0)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_APB1_DIV  APB1 prescaler
  * @{
  */
#define LL_RCC_APB1_DIV_1                  0U
#define LL_RCC_APB1_DIV_2                  RCC_CFGR2_PPRE1_0
#define LL_RCC_APB1_DIV_4                  RCC_CFGR2_PPRE1_1
#define LL_RCC_APB1_DIV_8                  (RCC_CFGR2_PPRE1_1 | RCC_CFGR2_PPRE1_0)
#define LL_RCC_APB1_DIV_16                 RCC_CFGR2_PPRE1_2
#define LL_RCC_APB1_DIV_32                 (RCC_CFGR2_PPRE1_2 | RCC_CFGR2_PPRE1_0)
#define LL_RCC_APB1_DIV_64                 (RCC_CFGR2_PPRE1_2 | RCC_CFGR2_PPRE1_1)
#define LL_RCC_APB1_DIV_128                (RCC_CFGR2_PPRE1_2 | RCC_CFGR2_PPRE1_1 | RCC_CFGR2_PPRE1_0)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_APB2_DIV  APB2 prescaler
  * @{
  */
#define LL_RCC_APB2_DIV_1                  0U
#define LL_RCC_APB2_DIV_2                  RCC_CFGR2_PPRE2_0
#define LL_RCC_APB2_DIV_4                  RCC_CFGR2_PPRE2_1
#define LL_RCC_APB2_DIV_8                  (RCC_CFGR2_PPRE2_1 | RCC_CFGR2_PPRE2_0)
#define LL_RCC_APB2_DIV_16                 RCC_CFGR2_PPRE2_2
#define LL_RCC_APB2_DIV_32                 (RCC_CFGR2_PPRE2_2 | RCC_CFGR2_PPRE2_0)
#define LL_RCC_APB2_DIV_64                 (RCC_CFGR2_PPRE2_2 | RCC_CFGR2_PPRE2_1)
#define LL_RCC_APB2_DIV_128                (RCC_CFGR2_PPRE2_2 | RCC_CFGR2_PPRE2_1 | RCC_CFGR2_PPRE2_0)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_APB4_DIV  APB4 prescaler
  * @{
  */
#define LL_RCC_APB4_DIV_1                  0U
#define LL_RCC_APB4_DIV_2                  RCC_CFGR2_PPRE4_0
#define LL_RCC_APB4_DIV_4                  RCC_CFGR2_PPRE4_1
#define LL_RCC_APB4_DIV_8                  (RCC_CFGR2_PPRE4_1 | RCC_CFGR2_PPRE4_0)
#define LL_RCC_APB4_DIV_16                 RCC_CFGR2_PPRE4_2
#define LL_RCC_APB4_DIV_32                 (RCC_CFGR2_PPRE4_2 | RCC_CFGR2_PPRE4_0)
#define LL_RCC_APB4_DIV_64                 (RCC_CFGR2_PPRE4_2 | RCC_CFGR2_PPRE4_1)
#define LL_RCC_APB4_DIV_128                (RCC_CFGR2_PPRE4_2 | RCC_CFGR2_PPRE4_1 | RCC_CFGR2_PPRE4_0)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_APB5_DIV  APB5 prescaler
  * @{
  */
#define LL_RCC_APB5_DIV_1                  0U
#define LL_RCC_APB5_DIV_2                  RCC_CFGR2_PPRE5_0
#define LL_RCC_APB5_DIV_4                  RCC_CFGR2_PPRE5_1
#define LL_RCC_APB5_DIV_8                  (RCC_CFGR2_PPRE5_1 | RCC_CFGR2_PPRE5_0)
#define LL_RCC_APB5_DIV_16                 RCC_CFGR2_PPRE5_2
#define LL_RCC_APB5_DIV_32                 (RCC_CFGR2_PPRE5_2 | RCC_CFGR2_PPRE5_0)
#define LL_RCC_APB5_DIV_64                 (RCC_CFGR2_PPRE5_2 | RCC_CFGR2_PPRE5_1)
#define LL_RCC_APB5_DIV_128                (RCC_CFGR2_PPRE5_2 | RCC_CFGR2_PPRE5_1 | RCC_CFGR2_PPRE5_0)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_MCOx  MCO selection
  * @{
  */
#define LL_RCC_MCO1                        RCC_MISCENR_MCO1EN
#define LL_RCC_MCO2                        RCC_MISCENR_MCO2EN
/**
  * @}
  */

/** @defgroup RCC_LL_EC_MCOxSOURCE  MCO source selection
  * @{
  */
#define LL_RCC_MCO1SOURCE_HSI              ((RCC_CCIPR5_MCO1SEL<<16U) | 0U)
#define LL_RCC_MCO1SOURCE_LSE              ((RCC_CCIPR5_MCO1SEL<<16U) | RCC_CCIPR5_MCO1SEL_0)
#define LL_RCC_MCO1SOURCE_MSI              ((RCC_CCIPR5_MCO1SEL<<16U) | RCC_CCIPR5_MCO1SEL_1)
#define LL_RCC_MCO1SOURCE_LSI              ((RCC_CCIPR5_MCO1SEL<<16U) | RCC_CCIPR5_MCO1SEL_1 | RCC_CCIPR5_MCO1SEL_0)
#define LL_RCC_MCO1SOURCE_HSE              ((RCC_CCIPR5_MCO1SEL<<16U) | RCC_CCIPR5_MCO1SEL_2)
#define LL_RCC_MCO1SOURCE_IC5              ((RCC_CCIPR5_MCO1SEL<<16U) | RCC_CCIPR5_MCO1SEL_2 | RCC_CCIPR5_MCO1SEL_0)
#define LL_RCC_MCO1SOURCE_IC10             ((RCC_CCIPR5_MCO1SEL<<16U) | RCC_CCIPR5_MCO1SEL_2 | RCC_CCIPR5_MCO1SEL_1)
#define LL_RCC_MCO1SOURCE_SYSA             ((RCC_CCIPR5_MCO1SEL<<16U) |\
                                            RCC_CCIPR5_MCO1SEL_2 | RCC_CCIPR5_MCO1SEL_1 | RCC_CCIPR5_MCO1SEL_0)
#define LL_RCC_MCO2SOURCE_HSI              ((RCC_CCIPR5_MCO2SEL<<16U) | 0U)
#define LL_RCC_MCO2SOURCE_LSE              ((RCC_CCIPR5_MCO2SEL<<16U) | RCC_CCIPR5_MCO2SEL_0)
#define LL_RCC_MCO2SOURCE_MSI              ((RCC_CCIPR5_MCO2SEL<<16U) | RCC_CCIPR5_MCO2SEL_1)
#define LL_RCC_MCO2SOURCE_LSI              ((RCC_CCIPR5_MCO2SEL<<16U) | RCC_CCIPR5_MCO2SEL_1 | RCC_CCIPR5_MCO2SEL_0)
#define LL_RCC_MCO2SOURCE_HSE              ((RCC_CCIPR5_MCO2SEL<<16U) | RCC_CCIPR5_MCO2SEL_2)
#define LL_RCC_MCO2SOURCE_IC15             ((RCC_CCIPR5_MCO2SEL<<16U) | RCC_CCIPR5_MCO2SEL_2 | RCC_CCIPR5_MCO2SEL_0)
#define LL_RCC_MCO2SOURCE_IC20             ((RCC_CCIPR5_MCO2SEL<<16U) | RCC_CCIPR5_MCO2SEL_2 | RCC_CCIPR5_MCO2SEL_1)
#define LL_RCC_MCO2SOURCE_SYSB             ((RCC_CCIPR5_MCO2SEL<<16U) |\
                                            RCC_CCIPR5_MCO2SEL_2 | RCC_CCIPR5_MCO2SEL_1 | RCC_CCIPR5_MCO2SEL_0)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_MCOx_DIV  MCO prescaler
  * @{
  */
#define LL_RCC_MCO1_DIV_1                  (RCC_CCIPR5_MCO1PRE<<16U)
#define LL_RCC_MCO1_DIV_2                  ((RCC_CCIPR5_MCO1PRE<<16U) | RCC_CCIPR5_MCO1PRE_0)
#define LL_RCC_MCO1_DIV_3                  ((RCC_CCIPR5_MCO1PRE<<16U) | RCC_CCIPR5_MCO1PRE_1)
#define LL_RCC_MCO1_DIV_4                  ((RCC_CCIPR5_MCO1PRE<<16U) | RCC_CCIPR5_MCO1PRE_1 | RCC_CCIPR5_MCO1PRE_0)
#define LL_RCC_MCO1_DIV_5                  ((RCC_CCIPR5_MCO1PRE<<16U) | RCC_CCIPR5_MCO1PRE_2)
#define LL_RCC_MCO1_DIV_6                  ((RCC_CCIPR5_MCO1PRE<<16U) | RCC_CCIPR5_MCO1PRE_2 | RCC_CCIPR5_MCO1PRE_0)
#define LL_RCC_MCO1_DIV_7                  ((RCC_CCIPR5_MCO1PRE<<16U) | RCC_CCIPR5_MCO1PRE_2 | RCC_CCIPR5_MCO1PRE_1)
#define LL_RCC_MCO1_DIV_8                  ((RCC_CCIPR5_MCO1PRE<<16U) |\
                                            RCC_CCIPR5_MCO1PRE_2 | RCC_CCIPR5_MCO1PRE_1 | RCC_CCIPR5_MCO1PRE_0)
#define LL_RCC_MCO1_DIV_9                  ((RCC_CCIPR5_MCO1PRE<<16U) | RCC_CCIPR5_MCO1PRE_3)
#define LL_RCC_MCO1_DIV_10                 ((RCC_CCIPR5_MCO1PRE<<16U) | RCC_CCIPR5_MCO1PRE_3 | RCC_CCIPR5_MCO1PRE_0)
#define LL_RCC_MCO1_DIV_11                 ((RCC_CCIPR5_MCO1PRE<<16U) | RCC_CCIPR5_MCO1PRE_3 | RCC_CCIPR5_MCO1PRE_1)
#define LL_RCC_MCO1_DIV_12                 ((RCC_CCIPR5_MCO1PRE<<16U) |\
                                            RCC_CCIPR5_MCO1PRE_3 | RCC_CCIPR5_MCO1PRE_1 | RCC_CCIPR5_MCO1PRE_0)
#define LL_RCC_MCO1_DIV_13                 ((RCC_CCIPR5_MCO1PRE<<16U) | RCC_CCIPR5_MCO1PRE_3 | RCC_CCIPR5_MCO1PRE_2)
#define LL_RCC_MCO1_DIV_14                 ((RCC_CCIPR5_MCO1PRE<<16U) |\
                                            RCC_CCIPR5_MCO1PRE_3 | RCC_CCIPR5_MCO1PRE_2 | RCC_CCIPR5_MCO1PRE_0)
#define LL_RCC_MCO1_DIV_15                 ((RCC_CCIPR5_MCO1PRE<<16U) |\
                                            RCC_CCIPR5_MCO1PRE_3 | RCC_CCIPR5_MCO1PRE_2 | RCC_CCIPR5_MCO1PRE_1)
#define LL_RCC_MCO1_DIV_16                 ((RCC_CCIPR5_MCO1PRE<<16U) | RCC_CCIPR5_MCO1PRE)
#define LL_RCC_MCO2_DIV_1                  (RCC_CCIPR5_MCO2PRE<<16U)
#define LL_RCC_MCO2_DIV_2                  ((RCC_CCIPR5_MCO2PRE<<16U) | RCC_CCIPR5_MCO2PRE_0)
#define LL_RCC_MCO2_DIV_3                  ((RCC_CCIPR5_MCO2PRE<<16U) | RCC_CCIPR5_MCO2PRE_1)
#define LL_RCC_MCO2_DIV_4                  ((RCC_CCIPR5_MCO2PRE<<16U) | RCC_CCIPR5_MCO2PRE_1 | RCC_CCIPR5_MCO2PRE_0)
#define LL_RCC_MCO2_DIV_5                  ((RCC_CCIPR5_MCO2PRE<<16U) | RCC_CCIPR5_MCO2PRE_2)
#define LL_RCC_MCO2_DIV_6                  ((RCC_CCIPR5_MCO2PRE<<16U) | RCC_CCIPR5_MCO2PRE_2 | RCC_CCIPR5_MCO2PRE_0)
#define LL_RCC_MCO2_DIV_7                  ((RCC_CCIPR5_MCO2PRE<<16U) | RCC_CCIPR5_MCO2PRE_2 | RCC_CCIPR5_MCO2PRE_1)
#define LL_RCC_MCO2_DIV_8                  ((RCC_CCIPR5_MCO2PRE<<16U) |\
                                            RCC_CCIPR5_MCO2PRE_2 | RCC_CCIPR5_MCO2PRE_1 | RCC_CCIPR5_MCO2PRE_0)
#define LL_RCC_MCO2_DIV_9                  ((RCC_CCIPR5_MCO2PRE<<16U) | RCC_CCIPR5_MCO2PRE_3)
#define LL_RCC_MCO2_DIV_10                 ((RCC_CCIPR5_MCO2PRE<<16U) | RCC_CCIPR5_MCO2PRE_3 | RCC_CCIPR5_MCO2PRE_0)
#define LL_RCC_MCO2_DIV_11                 ((RCC_CCIPR5_MCO2PRE<<16U) | RCC_CCIPR5_MCO2PRE_3 | RCC_CCIPR5_MCO2PRE_1)
#define LL_RCC_MCO2_DIV_12                 ((RCC_CCIPR5_MCO2PRE<<16U) |\
                                            RCC_CCIPR5_MCO2PRE_3 | RCC_CCIPR5_MCO2PRE_1 | RCC_CCIPR5_MCO2PRE_0)
#define LL_RCC_MCO2_DIV_13                 ((RCC_CCIPR5_MCO2PRE<<16U) | RCC_CCIPR5_MCO2PRE_3 | RCC_CCIPR5_MCO2PRE_2)
#define LL_RCC_MCO2_DIV_14                 ((RCC_CCIPR5_MCO2PRE<<16U) |\
                                            RCC_CCIPR5_MCO2PRE_3 | RCC_CCIPR5_MCO2PRE_2 | RCC_CCIPR5_MCO2PRE_0)
#define LL_RCC_MCO2_DIV_15                 ((RCC_CCIPR5_MCO2PRE<<16U) |\
                                            RCC_CCIPR5_MCO2PRE_3 | RCC_CCIPR5_MCO2PRE_2 | RCC_CCIPR5_MCO2PRE_1)
#define LL_RCC_MCO2_DIV_16                 ((RCC_CCIPR5_MCO2PRE<<16U) | RCC_CCIPR5_MCO2PRE)

/**
  * @}
  */

/** @defgroup RCC_LL_EC_RTC_HSE_DIV  Prescaler for RTC clock
  * @{
  */
#define LL_RCC_RTC_HSE_DIV_1               0U
#define LL_RCC_RTC_HSE_DIV_2               RCC_CCIPR7_RTCPRE_0
#define LL_RCC_RTC_HSE_DIV_3               RCC_CCIPR7_RTCPRE_1
#define LL_RCC_RTC_HSE_DIV_4               (RCC_CCIPR7_RTCPRE_1 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_5               RCC_CCIPR7_RTCPRE_2
#define LL_RCC_RTC_HSE_DIV_6               (RCC_CCIPR7_RTCPRE_2 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_7               (RCC_CCIPR7_RTCPRE_2 | RCC_CCIPR7_RTCPRE_1)
#define LL_RCC_RTC_HSE_DIV_8               (RCC_CCIPR7_RTCPRE_2 | RCC_CCIPR7_RTCPRE_1 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_9               RCC_CCIPR7_RTCPRE_3
#define LL_RCC_RTC_HSE_DIV_10              (RCC_CCIPR7_RTCPRE_3 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_11              (RCC_CCIPR7_RTCPRE_3 | RCC_CCIPR7_RTCPRE_1)
#define LL_RCC_RTC_HSE_DIV_12              (RCC_CCIPR7_RTCPRE_3 | RCC_CCIPR7_RTCPRE_1 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_13              (RCC_CCIPR7_RTCPRE_3 | RCC_CCIPR7_RTCPRE_2)
#define LL_RCC_RTC_HSE_DIV_14              (RCC_CCIPR7_RTCPRE_3 | RCC_CCIPR7_RTCPRE_2 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_15              (RCC_CCIPR7_RTCPRE_3 | RCC_CCIPR7_RTCPRE_2 | RCC_CCIPR7_RTCPRE_1)
#define LL_RCC_RTC_HSE_DIV_16              (RCC_CCIPR7_RTCPRE_3 |\
                                            RCC_CCIPR7_RTCPRE_2 | RCC_CCIPR7_RTCPRE_1 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_17              RCC_CCIPR7_RTCPRE_4
#define LL_RCC_RTC_HSE_DIV_18              (RCC_CCIPR7_RTCPRE_4 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_19              (RCC_CCIPR7_RTCPRE_4 | RCC_CCIPR7_RTCPRE_1)
#define LL_RCC_RTC_HSE_DIV_20              (RCC_CCIPR7_RTCPRE_4 | RCC_CCIPR7_RTCPRE_1 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_21              (RCC_CCIPR7_RTCPRE_4 | RCC_CCIPR7_RTCPRE_2)
#define LL_RCC_RTC_HSE_DIV_22              (RCC_CCIPR7_RTCPRE_4 | RCC_CCIPR7_RTCPRE_2 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_23              (RCC_CCIPR7_RTCPRE_4 | RCC_CCIPR7_RTCPRE_2 | RCC_CCIPR7_RTCPRE_1)
#define LL_RCC_RTC_HSE_DIV_24              (RCC_CCIPR7_RTCPRE_4 |\
                                            RCC_CCIPR7_RTCPRE_2 | RCC_CCIPR7_RTCPRE_1 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_25              (RCC_CCIPR7_RTCPRE_4 | RCC_CCIPR7_RTCPRE_3)
#define LL_RCC_RTC_HSE_DIV_26              (RCC_CCIPR7_RTCPRE_4 | RCC_CCIPR7_RTCPRE_3 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_27              (RCC_CCIPR7_RTCPRE_4 | RCC_CCIPR7_RTCPRE_3 | RCC_CCIPR7_RTCPRE_1)
#define LL_RCC_RTC_HSE_DIV_28              (RCC_CCIPR7_RTCPRE_4 |\
                                            RCC_CCIPR7_RTCPRE_3 | RCC_CCIPR7_RTCPRE_1 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_29              (RCC_CCIPR7_RTCPRE_4 | RCC_CCIPR7_RTCPRE_3 | RCC_CCIPR7_RTCPRE_2)
#define LL_RCC_RTC_HSE_DIV_30              (RCC_CCIPR7_RTCPRE_4 |\
                                            RCC_CCIPR7_RTCPRE_3 | RCC_CCIPR7_RTCPRE_2 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_31              (RCC_CCIPR7_RTCPRE_4 |\
                                            RCC_CCIPR7_RTCPRE_3 | RCC_CCIPR7_RTCPRE_2 | RCC_CCIPR7_RTCPRE_1)
#define LL_RCC_RTC_HSE_DIV_32              (RCC_CCIPR7_RTCPRE_4 | RCC_CCIPR7_RTCPRE_3 | RCC_CCIPR7_RTCPRE_2 |\
                                            RCC_CCIPR7_RTCPRE_1 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_33              RCC_CCIPR7_RTCPRE_5
#define LL_RCC_RTC_HSE_DIV_34              (RCC_CCIPR7_RTCPRE_5 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_35              (RCC_CCIPR7_RTCPRE_5 | RCC_CCIPR7_RTCPRE_1)
#define LL_RCC_RTC_HSE_DIV_36              (RCC_CCIPR7_RTCPRE_5 | RCC_CCIPR7_RTCPRE_1 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_37              (RCC_CCIPR7_RTCPRE_5 | RCC_CCIPR7_RTCPRE_2)
#define LL_RCC_RTC_HSE_DIV_38              (RCC_CCIPR7_RTCPRE_5 | RCC_CCIPR7_RTCPRE_2 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_39              (RCC_CCIPR7_RTCPRE_5 | RCC_CCIPR7_RTCPRE_2 | RCC_CCIPR7_RTCPRE_1)
#define LL_RCC_RTC_HSE_DIV_40              (RCC_CCIPR7_RTCPRE_5 |\
                                            RCC_CCIPR7_RTCPRE_2 | RCC_CCIPR7_RTCPRE_1 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_41              (RCC_CCIPR7_RTCPRE_5 | RCC_CCIPR7_RTCPRE_3)
#define LL_RCC_RTC_HSE_DIV_42              (RCC_CCIPR7_RTCPRE_5 | RCC_CCIPR7_RTCPRE_3 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_43              (RCC_CCIPR7_RTCPRE_5 | RCC_CCIPR7_RTCPRE_3 | RCC_CCIPR7_RTCPRE_1)
#define LL_RCC_RTC_HSE_DIV_44              (RCC_CCIPR7_RTCPRE_5 |\
                                            RCC_CCIPR7_RTCPRE_3 | RCC_CCIPR7_RTCPRE_1 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_45              (RCC_CCIPR7_RTCPRE_5 | RCC_CCIPR7_RTCPRE_3 | RCC_CCIPR7_RTCPRE_2)
#define LL_RCC_RTC_HSE_DIV_46              (RCC_CCIPR7_RTCPRE_5 |\
                                            RCC_CCIPR7_RTCPRE_3 | RCC_CCIPR7_RTCPRE_2 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_47              (RCC_CCIPR7_RTCPRE_5 |\
                                            RCC_CCIPR7_RTCPRE_3 | RCC_CCIPR7_RTCPRE_2 | RCC_CCIPR7_RTCPRE_1)
#define LL_RCC_RTC_HSE_DIV_48              (RCC_CCIPR7_RTCPRE_5 | RCC_CCIPR7_RTCPRE_3 | RCC_CCIPR7_RTCPRE_2 |\
                                            RCC_CCIPR7_RTCPRE_1 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_49              (RCC_CCIPR7_RTCPRE_5 | RCC_CCIPR7_RTCPRE_4)
#define LL_RCC_RTC_HSE_DIV_50              (RCC_CCIPR7_RTCPRE_5 | RCC_CCIPR7_RTCPRE_4 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_51              (RCC_CCIPR7_RTCPRE_5 | RCC_CCIPR7_RTCPRE_4 | RCC_CCIPR7_RTCPRE_1)
#define LL_RCC_RTC_HSE_DIV_52              (RCC_CCIPR7_RTCPRE_5 |\
                                            RCC_CCIPR7_RTCPRE_4 | RCC_CCIPR7_RTCPRE_1 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_53              (RCC_CCIPR7_RTCPRE_5 | RCC_CCIPR7_RTCPRE_4 | RCC_CCIPR7_RTCPRE_2)
#define LL_RCC_RTC_HSE_DIV_54              (RCC_CCIPR7_RTCPRE_5 |\
                                            RCC_CCIPR7_RTCPRE_4 | RCC_CCIPR7_RTCPRE_2 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_55              (RCC_CCIPR7_RTCPRE_5 |\
                                            RCC_CCIPR7_RTCPRE_4 | RCC_CCIPR7_RTCPRE_2 | RCC_CCIPR7_RTCPRE_1)
#define LL_RCC_RTC_HSE_DIV_56              (RCC_CCIPR7_RTCPRE_5 | RCC_CCIPR7_RTCPRE_4 | RCC_CCIPR7_RTCPRE_2 |\
                                            RCC_CCIPR7_RTCPRE_1 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_57              (RCC_CCIPR7_RTCPRE_5 | RCC_CCIPR7_RTCPRE_4 | RCC_CCIPR7_RTCPRE_3)
#define LL_RCC_RTC_HSE_DIV_58              (RCC_CCIPR7_RTCPRE_5 |\
                                            RCC_CCIPR7_RTCPRE_4 | RCC_CCIPR7_RTCPRE_3 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_59              (RCC_CCIPR7_RTCPRE_5 |\
                                            RCC_CCIPR7_RTCPRE_4 | RCC_CCIPR7_RTCPRE_3 | RCC_CCIPR7_RTCPRE_1)
#define LL_RCC_RTC_HSE_DIV_60              (RCC_CCIPR7_RTCPRE_5 | RCC_CCIPR7_RTCPRE_4 | RCC_CCIPR7_RTCPRE_3 |\
                                            RCC_CCIPR7_RTCPRE_1 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_61              (RCC_CCIPR7_RTCPRE_5 |\
                                            RCC_CCIPR7_RTCPRE_4 | RCC_CCIPR7_RTCPRE_3 | RCC_CCIPR7_RTCPRE_2)
#define LL_RCC_RTC_HSE_DIV_62              (RCC_CCIPR7_RTCPRE_5 | RCC_CCIPR7_RTCPRE_4 | RCC_CCIPR7_RTCPRE_3 |\
                                            RCC_CCIPR7_RTCPRE_2 | RCC_CCIPR7_RTCPRE_0)
#define LL_RCC_RTC_HSE_DIV_63              (RCC_CCIPR7_RTCPRE_5 | RCC_CCIPR7_RTCPRE_4 | RCC_CCIPR7_RTCPRE_3 |\
                                            RCC_CCIPR7_RTCPRE_2 | RCC_CCIPR7_RTCPRE_1)
#define LL_RCC_RTC_HSE_DIV_64              (RCC_CCIPR7_RTCPRE_5 | RCC_CCIPR7_RTCPRE_4 | RCC_CCIPR7_RTCPRE_3 |\
                                            RCC_CCIPR7_RTCPRE_2 | RCC_CCIPR7_RTCPRE_1 | RCC_CCIPR7_RTCPRE_0)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_ADC_CLKSOURCE  Peripheral ADC clock source selection
  * @{
  */
#define LL_RCC_ADC_CLKSOURCE_HCLK          0U
#define LL_RCC_ADC_CLKSOURCE_CLKP          RCC_CCIPR1_ADC12SEL_0
#define LL_RCC_ADC_CLKSOURCE_IC7           RCC_CCIPR1_ADC12SEL_1
#define LL_RCC_ADC_CLKSOURCE_IC8           (RCC_CCIPR1_ADC12SEL_1 | RCC_CCIPR1_ADC12SEL_0)
#define LL_RCC_ADC_CLKSOURCE_MSI           RCC_CCIPR1_ADC12SEL_2
#define LL_RCC_ADC_CLKSOURCE_HSI           (RCC_CCIPR1_ADC12SEL_2 | RCC_CCIPR1_ADC12SEL_0)
#define LL_RCC_ADC_CLKSOURCE_I2S_CKIN      (RCC_CCIPR1_ADC12SEL_2 | RCC_CCIPR1_ADC12SEL_1)
#define LL_RCC_ADC_CLKSOURCE_TIMG          (RCC_CCIPR1_ADC12SEL_2 | RCC_CCIPR1_ADC12SEL_1 | RCC_CCIPR1_ADC12SEL_0)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_ADF_CLKSOURCE  Peripheral ADF clock source selection
  * @{
  */
#define LL_RCC_ADF1_CLKSOURCE_HCLK         0U
#define LL_RCC_ADF1_CLKSOURCE_CLKP         RCC_CCIPR1_ADF1SEL_0
#define LL_RCC_ADF1_CLKSOURCE_IC7          RCC_CCIPR1_ADF1SEL_1
#define LL_RCC_ADF1_CLKSOURCE_IC8          (RCC_CCIPR1_ADF1SEL_1 | RCC_CCIPR1_ADF1SEL_0)
#define LL_RCC_ADF1_CLKSOURCE_MSI          RCC_CCIPR1_ADF1SEL_2
#define LL_RCC_ADF1_CLKSOURCE_HSI          (RCC_CCIPR1_ADF1SEL_2 | RCC_CCIPR1_ADF1SEL_0)
#define LL_RCC_ADF1_CLKSOURCE_I2S_CKIN     (RCC_CCIPR1_ADF1SEL_2 | RCC_CCIPR1_ADF1SEL_1)
#define LL_RCC_ADF1_CLKSOURCE_TIMG         (RCC_CCIPR1_ADF1SEL_2 | RCC_CCIPR1_ADF1SEL_1 | RCC_CCIPR1_ADF1SEL_0)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_CLKP_CLKSOURCE  Peripheral CLKP clock source selection
  * @{
  */
#define LL_RCC_CLKP_CLKSOURCE_HSI          0U
#define LL_RCC_CLKP_CLKSOURCE_MSI          RCC_CCIPR7_PERSEL_0
#define LL_RCC_CLKP_CLKSOURCE_HSE          RCC_CCIPR7_PERSEL_1
#define LL_RCC_CLKP_CLKSOURCE_IC19         (RCC_CCIPR7_PERSEL_1 | RCC_CCIPR7_PERSEL_0)
#define LL_RCC_CLKP_CLKSOURCE_IC5          RCC_CCIPR7_PERSEL_2
#define LL_RCC_CLKP_CLKSOURCE_IC10         (RCC_CCIPR7_PERSEL_2 | RCC_CCIPR7_PERSEL_0)
#define LL_RCC_CLKP_CLKSOURCE_IC15         (RCC_CCIPR7_PERSEL_2 | RCC_CCIPR7_PERSEL_1)
#define LL_RCC_CLKP_CLKSOURCE_IC20         (RCC_CCIPR7_PERSEL_2 | RCC_CCIPR7_PERSEL_1 | RCC_CCIPR7_PERSEL_0)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_DCMIPP_CLKSOURCE  Peripheral DCMIPP clock source selection
  * @{
  */
#define LL_RCC_DCMIPP_CLKSOURCE_PCLK5      0U
#define LL_RCC_DCMIPP_CLKSOURCE_CLKP       RCC_CCIPR1_DCMIPPSEL_0
#define LL_RCC_DCMIPP_CLKSOURCE_IC17       RCC_CCIPR1_DCMIPPSEL_1
#define LL_RCC_DCMIPP_CLKSOURCE_HSI        (RCC_CCIPR1_DCMIPPSEL_1 | RCC_CCIPR1_DCMIPPSEL_0)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_ETH_CLKSOURCE  Peripheral ETH kernel clock source selection
  * @{
  */
#define LL_RCC_ETH1_CLKSOURCE_HCLK         0U
#define LL_RCC_ETH1_CLKSOURCE_CLKP         RCC_CCIPR2_ETH1CLKSEL_0
#define LL_RCC_ETH1_CLKSOURCE_IC12         RCC_CCIPR2_ETH1CLKSEL_1
#define LL_RCC_ETH1_CLKSOURCE_HSE          (RCC_CCIPR2_ETH1CLKSEL_1 | RCC_CCIPR2_ETH1CLKSEL_0)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_ETHPHY_IF  Peripheral ETH PHY interface selection
  * @{
  */
#define LL_RCC_ETH1PHY_IF_MII              0U
#define LL_RCC_ETH1PHY_IF_RGMII            RCC_CCIPR2_ETH1SEL_0
#define LL_RCC_ETH1PHY_IF_RMII             RCC_CCIPR2_ETH1SEL_2
/**
  * @}
  */

/** @defgroup RCC_LL_EC_ETHREFRX_CLKSOURCE  Peripheral ETH Reference RX clock source selection
  * @{
  */
#define LL_RCC_ETH1REFRX_CLKSOURCE_EXT     0U
#define LL_RCC_ETH1REFRX_CLKSOURCE_INT     RCC_CCIPR2_ETH1REFCLKSEL
/**
  * @}
  */

/** @defgroup RCC_LL_EC_ETHREFTX_CLKSOURCE  Peripheral ETH Reference TX clock source selection
  * @{
  */
#define LL_RCC_ETH1REFTX_CLKSOURCE_EXT     0U
#define LL_RCC_ETH1REFTX_CLKSOURCE_INT     RCC_CCIPR2_ETH1GTXCLKSEL
/**
  * @}
  */

/** @defgroup RCC_LL_EC_ETHPTP_CLKSOURCE  Peripheral ETH PTP kernel clock source selection
  * @{
  */
#define LL_RCC_ETH1PTP_CLKSOURCE_HCLK      0U
#define LL_RCC_ETH1PTP_CLKSOURCE_CLKP      RCC_CCIPR2_ETH1PTPSEL_0
#define LL_RCC_ETH1PTP_CLKSOURCE_IC13      RCC_CCIPR2_ETH1PTPSEL_1
#define LL_RCC_ETH1PTP_CLKSOURCE_HSE       (RCC_CCIPR2_ETH1PTPSEL_1 | RCC_CCIPR2_ETH1PTPSEL_0)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_ETH1PTP_DIV  ETH1 PTP kernel clock divider selection
  * @{
  */
#define LL_RCC_ETH1PTP_DIV_1               0U
#define LL_RCC_ETH1PTP_DIV_2               RCC_CCIPR2_ETH1PTPDIV_0
#define LL_RCC_ETH1PTP_DIV_3               RCC_CCIPR2_ETH1PTPDIV_1
#define LL_RCC_ETH1PTP_DIV_4               (RCC_CCIPR2_ETH1PTPDIV_1 | RCC_CCIPR2_ETH1PTPDIV_0)
#define LL_RCC_ETH1PTP_DIV_5               RCC_CCIPR2_ETH1PTPDIV_2
#define LL_RCC_ETH1PTP_DIV_6               (RCC_CCIPR2_ETH1PTPDIV_2 | RCC_CCIPR2_ETH1PTPDIV_0)
#define LL_RCC_ETH1PTP_DIV_7               (RCC_CCIPR2_ETH1PTPDIV_2 | RCC_CCIPR2_ETH1PTPDIV_1)
#define LL_RCC_ETH1PTP_DIV_8               (RCC_CCIPR2_ETH1PTPDIV_2 | RCC_CCIPR2_ETH1PTPDIV_1 | RCC_CCIPR2_ETH1PTPDIV_0)
#define LL_RCC_ETH1PTP_DIV_9               RCC_CCIPR2_ETH1PTPDIV_3
#define LL_RCC_ETH1PTP_DIV_10              (RCC_CCIPR2_ETH1PTPDIV_3 | RCC_CCIPR2_ETH1PTPDIV_0)
#define LL_RCC_ETH1PTP_DIV_11              (RCC_CCIPR2_ETH1PTPDIV_3 | RCC_CCIPR2_ETH1PTPDIV_1)
#define LL_RCC_ETH1PTP_DIV_12              (RCC_CCIPR2_ETH1PTPDIV_3 | RCC_CCIPR2_ETH1PTPDIV_1 | RCC_CCIPR2_ETH1PTPDIV_0)
#define LL_RCC_ETH1PTP_DIV_13              (RCC_CCIPR2_ETH1PTPDIV_3 | RCC_CCIPR2_ETH1PTPDIV_2)
#define LL_RCC_ETH1PTP_DIV_14              (RCC_CCIPR2_ETH1PTPDIV_3 | RCC_CCIPR2_ETH1PTPDIV_2 | RCC_CCIPR2_ETH1PTPDIV_0)
#define LL_RCC_ETH1PTP_DIV_15              (RCC_CCIPR2_ETH1PTPDIV_3 | RCC_CCIPR2_ETH1PTPDIV_2 | RCC_CCIPR2_ETH1PTPDIV_1)
#define LL_RCC_ETH1PTP_DIV_16              (RCC_CCIPR2_ETH1PTPDIV_3 |\
                                            RCC_CCIPR2_ETH1PTPDIV_2 | RCC_CCIPR2_ETH1PTPDIV_1 | RCC_CCIPR2_ETH1PTPDIV_0)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_FDCAN_CLKSOURCE  Peripheral FDCAN clock source selection
  * @{
  */
#define LL_RCC_FDCAN_CLKSOURCE_PCLK1       0U
#define LL_RCC_FDCAN_CLKSOURCE_CLKP        RCC_CCIPR3_FDCANSEL_0
#define LL_RCC_FDCAN_CLKSOURCE_IC19        RCC_CCIPR3_FDCANSEL_1
#define LL_RCC_FDCAN_CLKSOURCE_HSE         (RCC_CCIPR3_FDCANSEL_1 | RCC_CCIPR3_FDCANSEL_0)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_FMC_CLKSOURCE  Peripheral FMC clock source selection
  * @{
  */
#define LL_RCC_FMC_CLKSOURCE_HCLK          0U
#define LL_RCC_FMC_CLKSOURCE_CLKP          RCC_CCIPR3_FMCSEL_0
#define LL_RCC_FMC_CLKSOURCE_IC3           RCC_CCIPR3_FMCSEL_1
#define LL_RCC_FMC_CLKSOURCE_IC4           (RCC_CCIPR3_FMCSEL_1 | RCC_CCIPR3_FMCSEL_0)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_I2C_CLKSOURCE  Peripheral I2C clock source selection
  * @{
  */
#define LL_RCC_I2C1_CLKSOURCE_PCLK1        LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I2C1SEL, RCC_CCIPR4_I2C1SEL_Pos, 0U)
#define LL_RCC_I2C1_CLKSOURCE_CLKP         LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I2C1SEL, RCC_CCIPR4_I2C1SEL_Pos,\
                                                        RCC_CCIPR4_I2C1SEL_0)
#define LL_RCC_I2C1_CLKSOURCE_IC10         LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I2C1SEL, RCC_CCIPR4_I2C1SEL_Pos,\
                                                        RCC_CCIPR4_I2C1SEL_1)
#define LL_RCC_I2C1_CLKSOURCE_IC15         LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I2C1SEL, RCC_CCIPR4_I2C1SEL_Pos,\
                                                        RCC_CCIPR4_I2C1SEL_1 | RCC_CCIPR4_I2C1SEL_0)
#define LL_RCC_I2C1_CLKSOURCE_MSI          LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I2C1SEL, RCC_CCIPR4_I2C1SEL_Pos,\
                                                        RCC_CCIPR4_I2C1SEL_2)
#define LL_RCC_I2C1_CLKSOURCE_HSI          LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I2C1SEL, RCC_CCIPR4_I2C1SEL_Pos,\
                                                        RCC_CCIPR4_I2C1SEL_2| RCC_CCIPR4_I2C1SEL_0)

#define LL_RCC_I2C2_CLKSOURCE_PCLK1        LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I2C2SEL, RCC_CCIPR4_I2C2SEL_Pos, 0U)
#define LL_RCC_I2C2_CLKSOURCE_CLKP         LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I2C2SEL, RCC_CCIPR4_I2C2SEL_Pos,\
                                                        RCC_CCIPR4_I2C2SEL_0)
#define LL_RCC_I2C2_CLKSOURCE_IC10         LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I2C2SEL, RCC_CCIPR4_I2C2SEL_Pos,\
                                                        RCC_CCIPR4_I2C2SEL_1)
#define LL_RCC_I2C2_CLKSOURCE_IC15         LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I2C2SEL, RCC_CCIPR4_I2C2SEL_Pos,\
                                                        RCC_CCIPR4_I2C2SEL_1 | RCC_CCIPR4_I2C2SEL_0)
#define LL_RCC_I2C2_CLKSOURCE_MSI          LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I2C2SEL, RCC_CCIPR4_I2C2SEL_Pos,\
                                                        RCC_CCIPR4_I2C2SEL_2)
#define LL_RCC_I2C2_CLKSOURCE_HSI          LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I2C2SEL, RCC_CCIPR4_I2C2SEL_Pos,\
                                                        RCC_CCIPR4_I2C2SEL_2 | RCC_CCIPR4_I2C2SEL_0)

#define LL_RCC_I2C3_CLKSOURCE_PCLK1        LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I2C3SEL, RCC_CCIPR4_I2C3SEL_Pos, 0U)
#define LL_RCC_I2C3_CLKSOURCE_CLKP         LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I2C3SEL, RCC_CCIPR4_I2C3SEL_Pos,\
                                                        RCC_CCIPR4_I2C3SEL_0)
#define LL_RCC_I2C3_CLKSOURCE_IC10         LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I2C3SEL, RCC_CCIPR4_I2C3SEL_Pos,\
                                                        RCC_CCIPR4_I2C3SEL_1)
#define LL_RCC_I2C3_CLKSOURCE_IC15         LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I2C3SEL, RCC_CCIPR4_I2C3SEL_Pos,\
                                                        RCC_CCIPR4_I2C3SEL_1 | RCC_CCIPR4_I2C3SEL_0)
#define LL_RCC_I2C3_CLKSOURCE_MSI          LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I2C3SEL, RCC_CCIPR4_I2C3SEL_Pos,\
                                                        RCC_CCIPR4_I2C3SEL_2)
#define LL_RCC_I2C3_CLKSOURCE_HSI          LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I2C3SEL, RCC_CCIPR4_I2C3SEL_Pos,\
                                                        RCC_CCIPR4_I2C3SEL_2| RCC_CCIPR4_I2C3SEL_0)

#define LL_RCC_I2C4_CLKSOURCE_PCLK1        LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I2C4SEL, RCC_CCIPR4_I2C4SEL_Pos, 0U)
#define LL_RCC_I2C4_CLKSOURCE_CLKP         LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I2C4SEL, RCC_CCIPR4_I2C4SEL_Pos,\
                                                        RCC_CCIPR4_I2C4SEL_0)
#define LL_RCC_I2C4_CLKSOURCE_IC10         LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I2C4SEL, RCC_CCIPR4_I2C4SEL_Pos,\
                                                        RCC_CCIPR4_I2C4SEL_1)
#define LL_RCC_I2C4_CLKSOURCE_IC15         LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I2C4SEL, RCC_CCIPR4_I2C4SEL_Pos,\
                                                        RCC_CCIPR4_I2C4SEL_1 | RCC_CCIPR4_I2C4SEL_0)
#define LL_RCC_I2C4_CLKSOURCE_MSI          LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I2C4SEL, RCC_CCIPR4_I2C4SEL_Pos,\
                                                        RCC_CCIPR4_I2C4SEL_2)
#define LL_RCC_I2C4_CLKSOURCE_HSI          LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I2C4SEL, RCC_CCIPR4_I2C4SEL_Pos,\
                                                        RCC_CCIPR4_I2C4SEL_2| RCC_CCIPR4_I2C4SEL_0)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_I3C_CLKSOURCE  Peripheral I3C clock source selection
  * @{
  */
#define LL_RCC_I3C1_CLKSOURCE_PCLK1        LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I3C1SEL, RCC_CCIPR4_I3C1SEL_Pos, 0U)
#define LL_RCC_I3C1_CLKSOURCE_CLKP         LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I3C1SEL, RCC_CCIPR4_I3C1SEL_Pos,\
                                                        RCC_CCIPR4_I3C1SEL_0)
#define LL_RCC_I3C1_CLKSOURCE_IC10         LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I3C1SEL, RCC_CCIPR4_I3C1SEL_Pos,\
                                                        RCC_CCIPR4_I3C1SEL_1)
#define LL_RCC_I3C1_CLKSOURCE_IC15         LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I3C1SEL, RCC_CCIPR4_I3C1SEL_Pos,\
                                                        RCC_CCIPR4_I3C1SEL_1 | RCC_CCIPR4_I3C1SEL_0)
#define LL_RCC_I3C1_CLKSOURCE_MSI          LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I3C1SEL, RCC_CCIPR4_I3C1SEL_Pos,\
                                                        RCC_CCIPR4_I3C1SEL_2)
#define LL_RCC_I3C1_CLKSOURCE_HSI          LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I3C1SEL, RCC_CCIPR4_I3C1SEL_Pos,\
                                                        RCC_CCIPR4_I3C1SEL_2| RCC_CCIPR4_I3C1SEL_0)

#define LL_RCC_I3C2_CLKSOURCE_PCLK1        LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I3C2SEL, RCC_CCIPR4_I3C2SEL_Pos, 0U)
#define LL_RCC_I3C2_CLKSOURCE_CLKP         LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I3C2SEL, RCC_CCIPR4_I3C2SEL_Pos,\
                                                        RCC_CCIPR4_I3C2SEL_0)
#define LL_RCC_I3C2_CLKSOURCE_IC10         LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I3C2SEL, RCC_CCIPR4_I3C2SEL_Pos,\
                                                        RCC_CCIPR4_I3C2SEL_1)
#define LL_RCC_I3C2_CLKSOURCE_IC15         LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I3C2SEL, RCC_CCIPR4_I3C2SEL_Pos,\
                                                        RCC_CCIPR4_I3C2SEL_1 | RCC_CCIPR4_I3C2SEL_0)
#define LL_RCC_I3C2_CLKSOURCE_MSI          LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I3C2SEL, RCC_CCIPR4_I3C2SEL_Pos,\
                                                        RCC_CCIPR4_I3C2SEL_2)
#define LL_RCC_I3C2_CLKSOURCE_HSI          LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I3C2SEL, RCC_CCIPR4_I3C2SEL_Pos,\
                                                        RCC_CCIPR4_I3C2SEL_2 | RCC_CCIPR4_I3C2SEL_0)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_LPTIM_CLKSOURCE  Peripheral LPTIM clock source selection
  * @{
  */
#define LL_RCC_LPTIM1_CLKSOURCE_PCLK1      LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM1SEL,  RCC_CCIPR12_LPTIM1SEL_Pos,  0U)
#define LL_RCC_LPTIM1_CLKSOURCE_CLKP       LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM1SEL,  RCC_CCIPR12_LPTIM1SEL_Pos,  RCC_CCIPR12_LPTIM1SEL_0)
#define LL_RCC_LPTIM1_CLKSOURCE_IC15       LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM1SEL,  RCC_CCIPR12_LPTIM1SEL_Pos,  RCC_CCIPR12_LPTIM1SEL_1)
#define LL_RCC_LPTIM1_CLKSOURCE_LSE        LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM1SEL,  RCC_CCIPR12_LPTIM1SEL_Pos,  RCC_CCIPR12_LPTIM1SEL_1 |\
                                                        RCC_CCIPR12_LPTIM1SEL_0)
#define LL_RCC_LPTIM1_CLKSOURCE_LSI        LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM1SEL,  RCC_CCIPR12_LPTIM1SEL_Pos,  RCC_CCIPR12_LPTIM1SEL_2)
#define LL_RCC_LPTIM1_CLKSOURCE_TIMG       LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM1SEL,  RCC_CCIPR12_LPTIM1SEL_Pos,  RCC_CCIPR12_LPTIM1SEL_2 |\
                                                        RCC_CCIPR12_LPTIM1SEL_0)

#define LL_RCC_LPTIM2_CLKSOURCE_PCLK4      LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM2SEL,  RCC_CCIPR12_LPTIM2SEL_Pos,  0U)
#define LL_RCC_LPTIM2_CLKSOURCE_CLKP       LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM2SEL,  RCC_CCIPR12_LPTIM2SEL_Pos,  RCC_CCIPR12_LPTIM2SEL_0)
#define LL_RCC_LPTIM2_CLKSOURCE_IC15       LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM2SEL,  RCC_CCIPR12_LPTIM2SEL_Pos,  RCC_CCIPR12_LPTIM2SEL_1)
#define LL_RCC_LPTIM2_CLKSOURCE_LSE        LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM2SEL,  RCC_CCIPR12_LPTIM2SEL_Pos,  RCC_CCIPR12_LPTIM2SEL_1 |\
                                                        RCC_CCIPR12_LPTIM2SEL_0)
#define LL_RCC_LPTIM2_CLKSOURCE_LSI        LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM2SEL,  RCC_CCIPR12_LPTIM2SEL_Pos,  RCC_CCIPR12_LPTIM2SEL_2)
#define LL_RCC_LPTIM2_CLKSOURCE_TIMG       LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM2SEL,  RCC_CCIPR12_LPTIM2SEL_Pos,  RCC_CCIPR12_LPTIM2SEL_2 |\
                                                        RCC_CCIPR12_LPTIM2SEL_0)

#define LL_RCC_LPTIM3_CLKSOURCE_PCLK4      LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM3SEL,  RCC_CCIPR12_LPTIM3SEL_Pos,  0U)
#define LL_RCC_LPTIM3_CLKSOURCE_CLKP       LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM3SEL,  RCC_CCIPR12_LPTIM3SEL_Pos,  RCC_CCIPR12_LPTIM3SEL_0)
#define LL_RCC_LPTIM3_CLKSOURCE_IC15       LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM3SEL,  RCC_CCIPR12_LPTIM3SEL_Pos,  RCC_CCIPR12_LPTIM3SEL_1)
#define LL_RCC_LPTIM3_CLKSOURCE_LSE        LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM3SEL,  RCC_CCIPR12_LPTIM3SEL_Pos,  RCC_CCIPR12_LPTIM3SEL_1 |\
                                                        RCC_CCIPR12_LPTIM3SEL_0)
#define LL_RCC_LPTIM3_CLKSOURCE_LSI        LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM3SEL,  RCC_CCIPR12_LPTIM3SEL_Pos,  RCC_CCIPR12_LPTIM3SEL_2)
#define LL_RCC_LPTIM3_CLKSOURCE_TIMG       LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM3SEL,  RCC_CCIPR12_LPTIM3SEL_Pos,  RCC_CCIPR12_LPTIM3SEL_2 |\
                                                        RCC_CCIPR12_LPTIM3SEL_0)

#define LL_RCC_LPTIM4_CLKSOURCE_PCLK4      LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM4SEL,  RCC_CCIPR12_LPTIM4SEL_Pos,  0U)
#define LL_RCC_LPTIM4_CLKSOURCE_CLKP       LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM4SEL,  RCC_CCIPR12_LPTIM4SEL_Pos,  RCC_CCIPR12_LPTIM4SEL_0)
#define LL_RCC_LPTIM4_CLKSOURCE_IC15       LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM4SEL,  RCC_CCIPR12_LPTIM4SEL_Pos,  RCC_CCIPR12_LPTIM4SEL_1)
#define LL_RCC_LPTIM4_CLKSOURCE_LSE        LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM4SEL,  RCC_CCIPR12_LPTIM4SEL_Pos,  RCC_CCIPR12_LPTIM4SEL_1 |\
                                                        RCC_CCIPR12_LPTIM4SEL_0)
#define LL_RCC_LPTIM4_CLKSOURCE_LSI        LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM4SEL,  RCC_CCIPR12_LPTIM4SEL_Pos,  RCC_CCIPR12_LPTIM4SEL_2)
#define LL_RCC_LPTIM4_CLKSOURCE_TIMG       LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM4SEL,  RCC_CCIPR12_LPTIM4SEL_Pos,  RCC_CCIPR12_LPTIM4SEL_2 |\
                                                        RCC_CCIPR12_LPTIM4SEL_0)

#define LL_RCC_LPTIM5_CLKSOURCE_PCLK4      LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM5SEL,  RCC_CCIPR12_LPTIM5SEL_Pos,  0U)
#define LL_RCC_LPTIM5_CLKSOURCE_CLKP       LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM5SEL,  RCC_CCIPR12_LPTIM5SEL_Pos,  RCC_CCIPR12_LPTIM5SEL_0)
#define LL_RCC_LPTIM5_CLKSOURCE_IC15       LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM5SEL,  RCC_CCIPR12_LPTIM5SEL_Pos,  RCC_CCIPR12_LPTIM5SEL_1)
#define LL_RCC_LPTIM5_CLKSOURCE_LSE        LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM5SEL,  RCC_CCIPR12_LPTIM5SEL_Pos,  RCC_CCIPR12_LPTIM5SEL_1 |\
                                                        RCC_CCIPR12_LPTIM5SEL_0)
#define LL_RCC_LPTIM5_CLKSOURCE_LSI        LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM5SEL,  RCC_CCIPR12_LPTIM5SEL_Pos,  RCC_CCIPR12_LPTIM5SEL_2)
#define LL_RCC_LPTIM5_CLKSOURCE_TIMG       LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM5SEL,  RCC_CCIPR12_LPTIM5SEL_Pos,  RCC_CCIPR12_LPTIM5SEL_2 |\
                                                        RCC_CCIPR12_LPTIM5SEL_0)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_LPUART_CLKSOURCE  Peripheral LPUART clock source selection
  * @{
  */
#define LL_RCC_LPUART1_CLKSOURCE_PCLK4     0U
#define LL_RCC_LPUART1_CLKSOURCE_CLKP      RCC_CCIPR14_LPUART1SEL_0
#define LL_RCC_LPUART1_CLKSOURCE_IC9       RCC_CCIPR14_LPUART1SEL_1
#define LL_RCC_LPUART1_CLKSOURCE_IC14      (RCC_CCIPR14_LPUART1SEL_1 | RCC_CCIPR14_LPUART1SEL_0)
#define LL_RCC_LPUART1_CLKSOURCE_LSE       RCC_CCIPR14_LPUART1SEL_2
#define LL_RCC_LPUART1_CLKSOURCE_MSI       (RCC_CCIPR14_LPUART1SEL_2 | RCC_CCIPR14_LPUART1SEL_0)
#define LL_RCC_LPUART1_CLKSOURCE_HSI       (RCC_CCIPR14_LPUART1SEL_2 | RCC_CCIPR14_LPUART1SEL_1)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_LTDC_CLKSOURCE  Peripheral LTDC clock source selection
  * @{
  */
#define LL_RCC_LTDC_CLKSOURCE_PCLK5        0U
#define LL_RCC_LTDC_CLKSOURCE_CLKP         RCC_CCIPR4_LTDCSEL_0
#define LL_RCC_LTDC_CLKSOURCE_IC16         RCC_CCIPR4_LTDCSEL_1
#define LL_RCC_LTDC_CLKSOURCE_HSI          (RCC_CCIPR4_LTDCSEL_1 | RCC_CCIPR4_LTDCSEL_0)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_MDF_CLKSOURCE  Peripheral MDF clock source selection
  * @{
  */
#define LL_RCC_MDF1_CLKSOURCE_HCLK         0U
#define LL_RCC_MDF1_CLKSOURCE_CLKP         RCC_CCIPR5_MDF1SEL_0
#define LL_RCC_MDF1_CLKSOURCE_IC7          RCC_CCIPR5_MDF1SEL_1
#define LL_RCC_MDF1_CLKSOURCE_IC8          (RCC_CCIPR5_MDF1SEL_1 | RCC_CCIPR5_MDF1SEL_0)
#define LL_RCC_MDF1_CLKSOURCE_MSI          RCC_CCIPR5_MDF1SEL_2
#define LL_RCC_MDF1_CLKSOURCE_HSI          (RCC_CCIPR5_MDF1SEL_2 | RCC_CCIPR5_MDF1SEL_0)
#define LL_RCC_MDF1_CLKSOURCE_I2S_CKIN     (RCC_CCIPR5_MDF1SEL_2 | RCC_CCIPR5_MDF1SEL_1)
#define LL_RCC_MDF1_CLKSOURCE_TIMG         (RCC_CCIPR5_MDF1SEL_2 | RCC_CCIPR5_MDF1SEL_1 | RCC_CCIPR5_MDF1SEL_0)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_OTGPHY_CLKSOURCE  Peripheral OTGPHY clock source selection
  * @{
  */
#define LL_RCC_OTGPHY1_CLKSOURCE_HSE_DIV_2     LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_OTGPHY1SEL, RCC_CCIPR6_OTGPHY1SEL_Pos, 0U)
#define LL_RCC_OTGPHY1_CLKSOURCE_CLKP          LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_OTGPHY1SEL, RCC_CCIPR6_OTGPHY1SEL_Pos, RCC_CCIPR6_OTGPHY1SEL_0)
#define LL_RCC_OTGPHY1_CLKSOURCE_IC15          LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_OTGPHY1SEL, RCC_CCIPR6_OTGPHY1SEL_Pos, RCC_CCIPR6_OTGPHY1SEL_1)
#define LL_RCC_OTGPHY1_CLKSOURCE_HSE_DIV_2_OSC LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_OTGPHY1SEL, RCC_CCIPR6_OTGPHY1SEL_Pos, RCC_CCIPR6_OTGPHY1SEL_1 |\
                                                            RCC_CCIPR6_OTGPHY1SEL_0)

#define LL_RCC_OTGPHY2_CLKSOURCE_HSE_DIV_2     LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_OTGPHY2SEL, RCC_CCIPR6_OTGPHY2SEL_Pos, 0U)
#define LL_RCC_OTGPHY2_CLKSOURCE_CLKP          LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_OTGPHY2SEL, RCC_CCIPR6_OTGPHY2SEL_Pos, RCC_CCIPR6_OTGPHY2SEL_0)
#define LL_RCC_OTGPHY2_CLKSOURCE_IC15          LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_OTGPHY2SEL, RCC_CCIPR6_OTGPHY2SEL_Pos, RCC_CCIPR6_OTGPHY2SEL_1)
#define LL_RCC_OTGPHY2_CLKSOURCE_HSE_DIV_2_OSC LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_OTGPHY2SEL, RCC_CCIPR6_OTGPHY2SEL_Pos, RCC_CCIPR6_OTGPHY2SEL_1 |\
                                                            RCC_CCIPR6_OTGPHY2SEL_0)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_OTGPHYCKREF_CLKSOURCE  Peripheral OTGPHYCKREF clock source selection
  * @{
  */
#define LL_RCC_OTGPHY1CKREF_CLKSOURCE_OTGPHY1       LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_OTGPHY1CKREFSEL, RCC_CCIPR6_OTGPHY1CKREFSEL_Pos, 0U)
#define LL_RCC_OTGPHY1CKREF_CLKSOURCE_HSE_DIV_2_OSC LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_OTGPHY1CKREFSEL, RCC_CCIPR6_OTGPHY1CKREFSEL_Pos, RCC_CCIPR6_OTGPHY1CKREFSEL)

#define LL_RCC_OTGPHY2CKREF_CLKSOURCE_OTGPHY2       LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_OTGPHY2CKREFSEL, RCC_CCIPR6_OTGPHY2CKREFSEL_Pos, 0U)
#define LL_RCC_OTGPHY2CKREF_CLKSOURCE_HSE_DIV_2_OSC LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_OTGPHY2CKREFSEL, RCC_CCIPR6_OTGPHY2CKREFSEL_Pos, RCC_CCIPR6_OTGPHY2CKREFSEL)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_PSSI_CLKSOURCE  Peripheral PSSI clock source selection
  * @{
  */
#define LL_RCC_PSSI_CLKSOURCE_HCLK         0U
#define LL_RCC_PSSI_CLKSOURCE_CLKP         RCC_CCIPR7_PSSISEL_0
#define LL_RCC_PSSI_CLKSOURCE_IC20         RCC_CCIPR7_PSSISEL_1
#define LL_RCC_PSSI_CLKSOURCE_HSI          (RCC_CCIPR7_PSSISEL_1 | RCC_CCIPR7_PSSISEL_0)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_RTC_CLKSOURCE  RTC clock source selection
  * @{
  */
#define LL_RCC_RTC_CLKSOURCE_NONE          0U
#define LL_RCC_RTC_CLKSOURCE_LSE           RCC_CCIPR7_RTCSEL_0
#define LL_RCC_RTC_CLKSOURCE_LSI           RCC_CCIPR7_RTCSEL_1
#define LL_RCC_RTC_CLKSOURCE_HSE           (RCC_CCIPR7_RTCSEL_1 | RCC_CCIPR7_RTCSEL_0)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_SAI_CLKSOURCE  Peripheral SAI clock source selection
  * @{
  */
#define LL_RCC_SAI1_CLKSOURCE_PCLK2        LL_CLKSOURCE(CCIPR7_OFFSET, RCC_CCIPR7_SAI1SEL, RCC_CCIPR7_SAI1SEL_Pos, 0U)
#define LL_RCC_SAI1_CLKSOURCE_CLKP         LL_CLKSOURCE(CCIPR7_OFFSET, RCC_CCIPR7_SAI1SEL, RCC_CCIPR7_SAI1SEL_Pos, RCC_CCIPR7_SAI1SEL_0)
#define LL_RCC_SAI1_CLKSOURCE_IC7          LL_CLKSOURCE(CCIPR7_OFFSET, RCC_CCIPR7_SAI1SEL, RCC_CCIPR7_SAI1SEL_Pos, RCC_CCIPR7_SAI1SEL_1)
#define LL_RCC_SAI1_CLKSOURCE_IC8          LL_CLKSOURCE(CCIPR7_OFFSET, RCC_CCIPR7_SAI1SEL, RCC_CCIPR7_SAI1SEL_Pos, RCC_CCIPR7_SAI1SEL_1 |\
                                                        RCC_CCIPR7_SAI1SEL_0)
#define LL_RCC_SAI1_CLKSOURCE_MSI          LL_CLKSOURCE(CCIPR7_OFFSET, RCC_CCIPR7_SAI1SEL, RCC_CCIPR7_SAI1SEL_Pos, RCC_CCIPR7_SAI1SEL_2)
#define LL_RCC_SAI1_CLKSOURCE_HSI          LL_CLKSOURCE(CCIPR7_OFFSET, RCC_CCIPR7_SAI1SEL, RCC_CCIPR7_SAI1SEL_Pos, RCC_CCIPR7_SAI1SEL_2 |\
                                                        RCC_CCIPR7_SAI1SEL_0)
#define LL_RCC_SAI1_CLKSOURCE_I2S_CKIN     LL_CLKSOURCE(CCIPR7_OFFSET, RCC_CCIPR7_SAI1SEL, RCC_CCIPR7_SAI1SEL_Pos, RCC_CCIPR7_SAI1SEL_2 |\
                                                        RCC_CCIPR7_SAI1SEL_1)
#define LL_RCC_SAI1_CLKSOURCE_SPDIFRX1     LL_CLKSOURCE(CCIPR7_OFFSET, RCC_CCIPR7_SAI1SEL, RCC_CCIPR7_SAI1SEL_Pos, RCC_CCIPR7_SAI1SEL_2 |\
                                                        RCC_CCIPR7_SAI1SEL_1 | RCC_CCIPR7_SAI1SEL_0)

#define LL_RCC_SAI2_CLKSOURCE_PCLK2        LL_CLKSOURCE(CCIPR7_OFFSET, RCC_CCIPR7_SAI2SEL, RCC_CCIPR7_SAI2SEL_Pos, 0U)
#define LL_RCC_SAI2_CLKSOURCE_CLKP         LL_CLKSOURCE(CCIPR7_OFFSET, RCC_CCIPR7_SAI2SEL, RCC_CCIPR7_SAI2SEL_Pos, RCC_CCIPR7_SAI2SEL_0)
#define LL_RCC_SAI2_CLKSOURCE_IC7          LL_CLKSOURCE(CCIPR7_OFFSET, RCC_CCIPR7_SAI2SEL, RCC_CCIPR7_SAI2SEL_Pos, RCC_CCIPR7_SAI2SEL_1)
#define LL_RCC_SAI2_CLKSOURCE_IC8          LL_CLKSOURCE(CCIPR7_OFFSET, RCC_CCIPR7_SAI2SEL, RCC_CCIPR7_SAI2SEL_Pos, RCC_CCIPR7_SAI2SEL_1 |\
                                                        RCC_CCIPR7_SAI2SEL_0)
#define LL_RCC_SAI2_CLKSOURCE_MSI          LL_CLKSOURCE(CCIPR7_OFFSET, RCC_CCIPR7_SAI2SEL, RCC_CCIPR7_SAI2SEL_Pos, RCC_CCIPR7_SAI2SEL_2)
#define LL_RCC_SAI2_CLKSOURCE_HSI          LL_CLKSOURCE(CCIPR7_OFFSET, RCC_CCIPR7_SAI2SEL, RCC_CCIPR7_SAI2SEL_Pos, RCC_CCIPR7_SAI2SEL_2 |\
                                                        RCC_CCIPR7_SAI2SEL_0)
#define LL_RCC_SAI2_CLKSOURCE_I2S_CKIN     LL_CLKSOURCE(CCIPR7_OFFSET, RCC_CCIPR7_SAI2SEL, RCC_CCIPR7_SAI2SEL_Pos, RCC_CCIPR7_SAI2SEL_2 |\
                                                        RCC_CCIPR7_SAI2SEL_1)
#define LL_RCC_SAI2_CLKSOURCE_SPDIFRX1     LL_CLKSOURCE(CCIPR7_OFFSET, RCC_CCIPR7_SAI2SEL, RCC_CCIPR7_SAI2SEL_Pos, RCC_CCIPR7_SAI2SEL_2 |\
                                                        RCC_CCIPR7_SAI2SEL_1 | RCC_CCIPR7_SAI2SEL_0)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_SDMMC_CLKSOURCE  Peripheral SDMMC clock source selection
  * @{
  */
#define LL_RCC_SDMMC1_CLKSOURCE_HCLK       LL_CLKSOURCE(CCIPR8_OFFSET, RCC_CCIPR8_SDMMC1SEL, RCC_CCIPR8_SDMMC1SEL_Pos, 0U)
#define LL_RCC_SDMMC1_CLKSOURCE_CLKP       LL_CLKSOURCE(CCIPR8_OFFSET, RCC_CCIPR8_SDMMC1SEL, RCC_CCIPR8_SDMMC1SEL_Pos, RCC_CCIPR8_SDMMC1SEL_0)
#define LL_RCC_SDMMC1_CLKSOURCE_IC4        LL_CLKSOURCE(CCIPR8_OFFSET, RCC_CCIPR8_SDMMC1SEL, RCC_CCIPR8_SDMMC1SEL_Pos, RCC_CCIPR8_SDMMC1SEL_1)
#define LL_RCC_SDMMC1_CLKSOURCE_IC5        LL_CLKSOURCE(CCIPR8_OFFSET, RCC_CCIPR8_SDMMC1SEL, RCC_CCIPR8_SDMMC1SEL_Pos, RCC_CCIPR8_SDMMC1SEL_1 |\
                                                        RCC_CCIPR8_SDMMC1SEL_0)

#define LL_RCC_SDMMC2_CLKSOURCE_HCLK       LL_CLKSOURCE(CCIPR8_OFFSET, RCC_CCIPR8_SDMMC2SEL, RCC_CCIPR8_SDMMC2SEL_Pos, 0U)
#define LL_RCC_SDMMC2_CLKSOURCE_CLKP       LL_CLKSOURCE(CCIPR8_OFFSET, RCC_CCIPR8_SDMMC2SEL, RCC_CCIPR8_SDMMC2SEL_Pos, RCC_CCIPR8_SDMMC2SEL_0)
#define LL_RCC_SDMMC2_CLKSOURCE_IC4        LL_CLKSOURCE(CCIPR8_OFFSET, RCC_CCIPR8_SDMMC2SEL, RCC_CCIPR8_SDMMC2SEL_Pos, RCC_CCIPR8_SDMMC2SEL_1)
#define LL_RCC_SDMMC2_CLKSOURCE_IC5        LL_CLKSOURCE(CCIPR8_OFFSET, RCC_CCIPR8_SDMMC2SEL, RCC_CCIPR8_SDMMC2SEL_Pos, RCC_CCIPR8_SDMMC2SEL_1 |\
                                                        RCC_CCIPR8_SDMMC2SEL_0)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_SPDIFRX_CLKSOURCE  Peripheral SPDIFRX clock source selection
  * @{
  */
#define LL_RCC_SPDIFRX1_CLKSOURCE_PCLK1    0U
#define LL_RCC_SPDIFRX1_CLKSOURCE_CLKP     RCC_CCIPR9_SPDIFRX1SEL_0
#define LL_RCC_SPDIFRX1_CLKSOURCE_IC7      RCC_CCIPR9_SPDIFRX1SEL_1
#define LL_RCC_SPDIFRX1_CLKSOURCE_IC8      (RCC_CCIPR9_SPDIFRX1SEL_1 | RCC_CCIPR9_SPDIFRX1SEL_0)
#define LL_RCC_SPDIFRX1_CLKSOURCE_MSI      RCC_CCIPR9_SPDIFRX1SEL_2
#define LL_RCC_SPDIFRX1_CLKSOURCE_HSI      (RCC_CCIPR9_SPDIFRX1SEL_2 | RCC_CCIPR9_SPDIFRX1SEL_0)
#define LL_RCC_SPDIFRX1_CLKSOURCE_I2S_CKIN (RCC_CCIPR9_SPDIFRX1SEL_2 | RCC_CCIPR9_SPDIFRX1SEL_1)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_SPI_CLKSOURCE  Peripheral SPI clock source selection
  * @{
  */
#define LL_RCC_SPI1_CLKSOURCE_PCLK2        LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI1SEL, RCC_CCIPR9_SPI1SEL_Pos, 0U)
#define LL_RCC_SPI1_CLKSOURCE_CLKP         LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI1SEL, RCC_CCIPR9_SPI1SEL_Pos, RCC_CCIPR9_SPI1SEL_0)
#define LL_RCC_SPI1_CLKSOURCE_IC8          LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI1SEL, RCC_CCIPR9_SPI1SEL_Pos, RCC_CCIPR9_SPI1SEL_1)
#define LL_RCC_SPI1_CLKSOURCE_IC9          LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI1SEL, RCC_CCIPR9_SPI1SEL_Pos, RCC_CCIPR9_SPI1SEL_1 |\
                                                        RCC_CCIPR9_SPI1SEL_0)
#define LL_RCC_SPI1_CLKSOURCE_MSI          LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI1SEL, RCC_CCIPR9_SPI1SEL_Pos, RCC_CCIPR9_SPI1SEL_2)
#define LL_RCC_SPI1_CLKSOURCE_HSI          LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI1SEL, RCC_CCIPR9_SPI1SEL_Pos, RCC_CCIPR9_SPI1SEL_2 |\
                                                        RCC_CCIPR9_SPI1SEL_0)
#define LL_RCC_SPI1_CLKSOURCE_I2S_CKIN     LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI1SEL, RCC_CCIPR9_SPI1SEL_Pos, RCC_CCIPR9_SPI1SEL_2 |\
                                                        RCC_CCIPR9_SPI1SEL_1)

#define LL_RCC_SPI2_CLKSOURCE_PCLK1        LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI2SEL, RCC_CCIPR9_SPI2SEL_Pos, 0U)
#define LL_RCC_SPI2_CLKSOURCE_CLKP         LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI2SEL, RCC_CCIPR9_SPI2SEL_Pos, RCC_CCIPR9_SPI2SEL_0)
#define LL_RCC_SPI2_CLKSOURCE_IC8          LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI2SEL, RCC_CCIPR9_SPI2SEL_Pos, RCC_CCIPR9_SPI2SEL_1)
#define LL_RCC_SPI2_CLKSOURCE_IC9          LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI2SEL, RCC_CCIPR9_SPI2SEL_Pos, RCC_CCIPR9_SPI2SEL_1 |\
                                                        RCC_CCIPR9_SPI2SEL_0)
#define LL_RCC_SPI2_CLKSOURCE_MSI          LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI2SEL, RCC_CCIPR9_SPI2SEL_Pos, RCC_CCIPR9_SPI2SEL_2)
#define LL_RCC_SPI2_CLKSOURCE_HSI          LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI2SEL, RCC_CCIPR9_SPI2SEL_Pos, RCC_CCIPR9_SPI2SEL_2 |\
                                                        RCC_CCIPR9_SPI2SEL_0)
#define LL_RCC_SPI2_CLKSOURCE_I2S_CKIN     LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI2SEL, RCC_CCIPR9_SPI2SEL_Pos, RCC_CCIPR9_SPI2SEL_2 |\
                                                        RCC_CCIPR9_SPI2SEL_1)

#define LL_RCC_SPI3_CLKSOURCE_PCLK1        LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI3SEL, RCC_CCIPR9_SPI3SEL_Pos, 0U)
#define LL_RCC_SPI3_CLKSOURCE_CLKP         LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI3SEL, RCC_CCIPR9_SPI3SEL_Pos, RCC_CCIPR9_SPI3SEL_0)
#define LL_RCC_SPI3_CLKSOURCE_IC8          LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI3SEL, RCC_CCIPR9_SPI3SEL_Pos, RCC_CCIPR9_SPI3SEL_1)
#define LL_RCC_SPI3_CLKSOURCE_IC9          LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI3SEL, RCC_CCIPR9_SPI3SEL_Pos, RCC_CCIPR9_SPI3SEL_1 |\
                                                        RCC_CCIPR9_SPI3SEL_0)
#define LL_RCC_SPI3_CLKSOURCE_MSI          LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI3SEL, RCC_CCIPR9_SPI3SEL_Pos, RCC_CCIPR9_SPI3SEL_2)
#define LL_RCC_SPI3_CLKSOURCE_HSI          LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI3SEL, RCC_CCIPR9_SPI3SEL_Pos, RCC_CCIPR9_SPI3SEL_2 |\
                                                        RCC_CCIPR9_SPI3SEL_0)
#define LL_RCC_SPI3_CLKSOURCE_I2S_CKIN     LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI3SEL, RCC_CCIPR9_SPI3SEL_Pos, RCC_CCIPR9_SPI3SEL_2 |\
                                                        RCC_CCIPR9_SPI3SEL_1)

#define LL_RCC_SPI4_CLKSOURCE_PCLK2        LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI4SEL, RCC_CCIPR9_SPI4SEL_Pos, 0U)
#define LL_RCC_SPI4_CLKSOURCE_CLKP         LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI4SEL, RCC_CCIPR9_SPI4SEL_Pos, RCC_CCIPR9_SPI4SEL_0)
#define LL_RCC_SPI4_CLKSOURCE_IC9          LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI4SEL, RCC_CCIPR9_SPI4SEL_Pos, RCC_CCIPR9_SPI4SEL_1)
#define LL_RCC_SPI4_CLKSOURCE_IC14         LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI4SEL, RCC_CCIPR9_SPI4SEL_Pos, RCC_CCIPR9_SPI4SEL_1 |\
                                                        RCC_CCIPR9_SPI4SEL_0)
#define LL_RCC_SPI4_CLKSOURCE_MSI          LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI4SEL, RCC_CCIPR9_SPI4SEL_Pos, RCC_CCIPR9_SPI4SEL_2)
#define LL_RCC_SPI4_CLKSOURCE_HSI          LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI4SEL, RCC_CCIPR9_SPI4SEL_Pos, RCC_CCIPR9_SPI4SEL_2 |\
                                                        RCC_CCIPR9_SPI4SEL_0)
#define LL_RCC_SPI4_CLKSOURCE_HSE          LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI4SEL, RCC_CCIPR9_SPI4SEL_Pos, RCC_CCIPR9_SPI4SEL_2 |\
                                                        RCC_CCIPR9_SPI4SEL_1)

#define LL_RCC_SPI5_CLKSOURCE_PCLK2        LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI5SEL, RCC_CCIPR9_SPI5SEL_Pos, 0U)
#define LL_RCC_SPI5_CLKSOURCE_CLKP         LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI5SEL, RCC_CCIPR9_SPI5SEL_Pos, RCC_CCIPR9_SPI5SEL_0)
#define LL_RCC_SPI5_CLKSOURCE_IC9          LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI5SEL, RCC_CCIPR9_SPI5SEL_Pos, RCC_CCIPR9_SPI5SEL_1)
#define LL_RCC_SPI5_CLKSOURCE_IC14         LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI5SEL, RCC_CCIPR9_SPI5SEL_Pos, RCC_CCIPR9_SPI5SEL_1 |\
                                                        RCC_CCIPR9_SPI5SEL_0)
#define LL_RCC_SPI5_CLKSOURCE_MSI          LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI5SEL, RCC_CCIPR9_SPI5SEL_Pos, RCC_CCIPR9_SPI5SEL_2)
#define LL_RCC_SPI5_CLKSOURCE_HSI          LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI5SEL, RCC_CCIPR9_SPI5SEL_Pos, RCC_CCIPR9_SPI5SEL_2 |\
                                                        RCC_CCIPR9_SPI5SEL_0)
#define LL_RCC_SPI5_CLKSOURCE_HSE          LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI5SEL, RCC_CCIPR9_SPI5SEL_Pos, RCC_CCIPR9_SPI5SEL_2 |\
                                                        RCC_CCIPR9_SPI5SEL_1)

#define LL_RCC_SPI6_CLKSOURCE_PCLK4        LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI6SEL, RCC_CCIPR9_SPI6SEL_Pos, 0U)
#define LL_RCC_SPI6_CLKSOURCE_CLKP         LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI6SEL, RCC_CCIPR9_SPI6SEL_Pos, RCC_CCIPR9_SPI6SEL_0)
#define LL_RCC_SPI6_CLKSOURCE_IC8          LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI6SEL, RCC_CCIPR9_SPI6SEL_Pos, RCC_CCIPR9_SPI6SEL_1)
#define LL_RCC_SPI6_CLKSOURCE_IC9          LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI6SEL, RCC_CCIPR9_SPI6SEL_Pos, RCC_CCIPR9_SPI6SEL_1 |\
                                                        RCC_CCIPR9_SPI6SEL_0)
#define LL_RCC_SPI6_CLKSOURCE_MSI          LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI6SEL, RCC_CCIPR9_SPI6SEL_Pos, RCC_CCIPR9_SPI6SEL_2)
#define LL_RCC_SPI6_CLKSOURCE_HSI          LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI6SEL, RCC_CCIPR9_SPI6SEL_Pos, RCC_CCIPR9_SPI6SEL_2 |\
                                                        RCC_CCIPR9_SPI6SEL_0)
#define LL_RCC_SPI6_CLKSOURCE_I2S_CKIN     LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI6SEL, RCC_CCIPR9_SPI6SEL_Pos, RCC_CCIPR9_SPI6SEL_2 |\
                                                        RCC_CCIPR9_SPI6SEL_1)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_UART_CLKSOURCE  Peripheral UART clock source selection
  * @{
  */
#define LL_RCC_UART4_CLKSOURCE_PCLK1       LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART4SEL, RCC_CCIPR13_UART4SEL_Pos, 0U)
#define LL_RCC_UART4_CLKSOURCE_CLKP        LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART4SEL, RCC_CCIPR13_UART4SEL_Pos, RCC_CCIPR13_UART4SEL_0)
#define LL_RCC_UART4_CLKSOURCE_IC9         LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART4SEL, RCC_CCIPR13_UART4SEL_Pos, RCC_CCIPR13_UART4SEL_1)
#define LL_RCC_UART4_CLKSOURCE_IC14        LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART4SEL, RCC_CCIPR13_UART4SEL_Pos, RCC_CCIPR13_UART4SEL_1 |\
                                                        RCC_CCIPR13_UART4SEL_0)
#define LL_RCC_UART4_CLKSOURCE_LSE         LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART4SEL, RCC_CCIPR13_UART4SEL_Pos, RCC_CCIPR13_UART4SEL_2)
#define LL_RCC_UART4_CLKSOURCE_MSI         LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART4SEL, RCC_CCIPR13_UART4SEL_Pos, RCC_CCIPR13_UART4SEL_2 |\
                                                        RCC_CCIPR13_UART4SEL_0)
#define LL_RCC_UART4_CLKSOURCE_HSI         LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART4SEL, RCC_CCIPR13_UART4SEL_Pos, RCC_CCIPR13_UART4SEL_2 |\
                                                        RCC_CCIPR13_UART4SEL_1)

#define LL_RCC_UART5_CLKSOURCE_PCLK1       LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART5SEL, RCC_CCIPR13_UART5SEL_Pos, 0U)
#define LL_RCC_UART5_CLKSOURCE_CLKP        LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART5SEL, RCC_CCIPR13_UART5SEL_Pos, RCC_CCIPR13_UART5SEL_0)
#define LL_RCC_UART5_CLKSOURCE_IC9         LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART5SEL, RCC_CCIPR13_UART5SEL_Pos, RCC_CCIPR13_UART5SEL_1)
#define LL_RCC_UART5_CLKSOURCE_IC14        LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART5SEL, RCC_CCIPR13_UART5SEL_Pos, RCC_CCIPR13_UART5SEL_1 |\
                                                        RCC_CCIPR13_UART5SEL_0)
#define LL_RCC_UART5_CLKSOURCE_LSE         LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART5SEL, RCC_CCIPR13_UART5SEL_Pos, RCC_CCIPR13_UART5SEL_2)
#define LL_RCC_UART5_CLKSOURCE_MSI         LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART5SEL, RCC_CCIPR13_UART5SEL_Pos, RCC_CCIPR13_UART5SEL_2 |\
                                                        RCC_CCIPR13_UART5SEL_0)
#define LL_RCC_UART5_CLKSOURCE_HSI         LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART5SEL, RCC_CCIPR13_UART5SEL_Pos, RCC_CCIPR13_UART5SEL_2 |\
                                                        RCC_CCIPR13_UART5SEL_1)

#define LL_RCC_UART7_CLKSOURCE_PCLK1       LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART7SEL, RCC_CCIPR13_UART7SEL_Pos, 0U)
#define LL_RCC_UART7_CLKSOURCE_CLKP        LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART7SEL, RCC_CCIPR13_UART7SEL_Pos, RCC_CCIPR13_UART7SEL_0)
#define LL_RCC_UART7_CLKSOURCE_IC9         LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART7SEL, RCC_CCIPR13_UART7SEL_Pos, RCC_CCIPR13_UART7SEL_1)
#define LL_RCC_UART7_CLKSOURCE_IC14        LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART7SEL, RCC_CCIPR13_UART7SEL_Pos, RCC_CCIPR13_UART7SEL_1 |\
                                                        RCC_CCIPR13_UART7SEL_0)
#define LL_RCC_UART7_CLKSOURCE_LSE         LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART7SEL, RCC_CCIPR13_UART7SEL_Pos, RCC_CCIPR13_UART7SEL_2)
#define LL_RCC_UART7_CLKSOURCE_MSI         LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART7SEL, RCC_CCIPR13_UART7SEL_Pos, RCC_CCIPR13_UART7SEL_2 |\
                                                        RCC_CCIPR13_UART7SEL_0)
#define LL_RCC_UART7_CLKSOURCE_HSI         LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART7SEL, RCC_CCIPR13_UART7SEL_Pos, RCC_CCIPR13_UART7SEL_2 |\
                                                        RCC_CCIPR13_UART7SEL_1)

#define LL_RCC_UART8_CLKSOURCE_PCLK1       LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART8SEL, RCC_CCIPR13_UART8SEL_Pos, 0U)
#define LL_RCC_UART8_CLKSOURCE_CLKP        LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART8SEL, RCC_CCIPR13_UART8SEL_Pos, RCC_CCIPR13_UART8SEL_0)
#define LL_RCC_UART8_CLKSOURCE_IC9         LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART8SEL, RCC_CCIPR13_UART8SEL_Pos, RCC_CCIPR13_UART8SEL_1)
#define LL_RCC_UART8_CLKSOURCE_IC14        LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART8SEL, RCC_CCIPR13_UART8SEL_Pos, RCC_CCIPR13_UART8SEL_1 |\
                                                        RCC_CCIPR13_UART8SEL_0)
#define LL_RCC_UART8_CLKSOURCE_LSE         LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART8SEL, RCC_CCIPR13_UART8SEL_Pos, RCC_CCIPR13_UART8SEL_2)
#define LL_RCC_UART8_CLKSOURCE_MSI         LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART8SEL, RCC_CCIPR13_UART8SEL_Pos, RCC_CCIPR13_UART8SEL_2 |\
                                                        RCC_CCIPR13_UART8SEL_0)
#define LL_RCC_UART8_CLKSOURCE_HSI         LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART8SEL, RCC_CCIPR13_UART8SEL_Pos, RCC_CCIPR13_UART8SEL_2 |\
                                                        RCC_CCIPR13_UART8SEL_1)

#define LL_RCC_UART9_CLKSOURCE_PCLK2       LL_CLKSOURCE(CCIPR14_OFFSET, RCC_CCIPR14_UART9SEL, RCC_CCIPR14_UART9SEL_Pos, 0U)
#define LL_RCC_UART9_CLKSOURCE_CLKP        LL_CLKSOURCE(CCIPR14_OFFSET, RCC_CCIPR14_UART9SEL, RCC_CCIPR14_UART9SEL_Pos, RCC_CCIPR14_UART9SEL_0)
#define LL_RCC_UART9_CLKSOURCE_IC9         LL_CLKSOURCE(CCIPR14_OFFSET, RCC_CCIPR14_UART9SEL, RCC_CCIPR14_UART9SEL_Pos, RCC_CCIPR14_UART9SEL_1)
#define LL_RCC_UART9_CLKSOURCE_IC14        LL_CLKSOURCE(CCIPR14_OFFSET, RCC_CCIPR14_UART9SEL, RCC_CCIPR14_UART9SEL_Pos, RCC_CCIPR14_UART9SEL_1 |\
                                                        RCC_CCIPR14_UART9SEL_0)
#define LL_RCC_UART9_CLKSOURCE_LSE         LL_CLKSOURCE(CCIPR14_OFFSET, RCC_CCIPR14_UART9SEL, RCC_CCIPR14_UART9SEL_Pos, RCC_CCIPR14_UART9SEL_2)
#define LL_RCC_UART9_CLKSOURCE_MSI         LL_CLKSOURCE(CCIPR14_OFFSET, RCC_CCIPR14_UART9SEL, RCC_CCIPR14_UART9SEL_Pos, RCC_CCIPR14_UART9SEL_2 |\
                                                        RCC_CCIPR14_UART9SEL_0)
#define LL_RCC_UART9_CLKSOURCE_HSI         LL_CLKSOURCE(CCIPR14_OFFSET, RCC_CCIPR14_UART9SEL, RCC_CCIPR14_UART9SEL_Pos, RCC_CCIPR14_UART9SEL_2 |\
                                                        RCC_CCIPR14_UART9SEL_1)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_USART_CLKSOURCE  Peripheral USART clock source selection
  * @{
  */
#define LL_RCC_USART1_CLKSOURCE_PCLK2      LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART1SEL, RCC_CCIPR13_USART1SEL_Pos, 0U)
#define LL_RCC_USART1_CLKSOURCE_CLKP       LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART1SEL, RCC_CCIPR13_USART1SEL_Pos, RCC_CCIPR13_USART1SEL_0)
#define LL_RCC_USART1_CLKSOURCE_IC9        LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART1SEL, RCC_CCIPR13_USART1SEL_Pos, RCC_CCIPR13_USART1SEL_1)
#define LL_RCC_USART1_CLKSOURCE_IC14       LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART1SEL, RCC_CCIPR13_USART1SEL_Pos, RCC_CCIPR13_USART1SEL_1 |\
                                                        RCC_CCIPR13_USART1SEL_0)
#define LL_RCC_USART1_CLKSOURCE_LSE        LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART1SEL, RCC_CCIPR13_USART1SEL_Pos, RCC_CCIPR13_USART1SEL_2)
#define LL_RCC_USART1_CLKSOURCE_MSI        LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART1SEL, RCC_CCIPR13_USART1SEL_Pos, RCC_CCIPR13_USART1SEL_2 |\
                                                        RCC_CCIPR13_USART1SEL_0)
#define LL_RCC_USART1_CLKSOURCE_HSI        LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART1SEL, RCC_CCIPR13_USART1SEL_Pos, RCC_CCIPR13_USART1SEL_2 |\
                                                        RCC_CCIPR13_USART1SEL_1)

#define LL_RCC_USART2_CLKSOURCE_PCLK1      LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART2SEL, RCC_CCIPR13_USART2SEL_Pos, 0U)
#define LL_RCC_USART2_CLKSOURCE_CLKP       LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART2SEL, RCC_CCIPR13_USART2SEL_Pos, RCC_CCIPR13_USART2SEL_0)
#define LL_RCC_USART2_CLKSOURCE_IC9        LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART2SEL, RCC_CCIPR13_USART2SEL_Pos, RCC_CCIPR13_USART2SEL_1)
#define LL_RCC_USART2_CLKSOURCE_IC14       LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART2SEL, RCC_CCIPR13_USART2SEL_Pos, RCC_CCIPR13_USART2SEL_1 |\
                                                        RCC_CCIPR13_USART2SEL_0)
#define LL_RCC_USART2_CLKSOURCE_LSE        LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART2SEL, RCC_CCIPR13_USART2SEL_Pos, RCC_CCIPR13_USART2SEL_2)
#define LL_RCC_USART2_CLKSOURCE_MSI        LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART2SEL, RCC_CCIPR13_USART2SEL_Pos, RCC_CCIPR13_USART2SEL_2 |\
                                                        RCC_CCIPR13_USART2SEL_0)
#define LL_RCC_USART2_CLKSOURCE_HSI        LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART2SEL, RCC_CCIPR13_USART2SEL_Pos, RCC_CCIPR13_USART2SEL_2 |\
                                                        RCC_CCIPR13_USART2SEL_1)

#define LL_RCC_USART3_CLKSOURCE_PCLK1      LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART3SEL, RCC_CCIPR13_USART3SEL_Pos, 0U)
#define LL_RCC_USART3_CLKSOURCE_CLKP       LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART3SEL, RCC_CCIPR13_USART3SEL_Pos, RCC_CCIPR13_USART3SEL_0)
#define LL_RCC_USART3_CLKSOURCE_IC9        LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART3SEL, RCC_CCIPR13_USART3SEL_Pos, RCC_CCIPR13_USART3SEL_1)
#define LL_RCC_USART3_CLKSOURCE_IC14       LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART3SEL, RCC_CCIPR13_USART3SEL_Pos, RCC_CCIPR13_USART3SEL_1 |\
                                                        RCC_CCIPR13_USART3SEL_0)
#define LL_RCC_USART3_CLKSOURCE_LSE        LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART3SEL, RCC_CCIPR13_USART3SEL_Pos, RCC_CCIPR13_USART3SEL_2)
#define LL_RCC_USART3_CLKSOURCE_MSI        LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART3SEL, RCC_CCIPR13_USART3SEL_Pos, RCC_CCIPR13_USART3SEL_2 |\
                                                        RCC_CCIPR13_USART3SEL_0)
#define LL_RCC_USART3_CLKSOURCE_HSI        LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART3SEL, RCC_CCIPR13_USART3SEL_Pos, RCC_CCIPR13_USART3SEL_2 |\
                                                        RCC_CCIPR13_USART3SEL_1)

#define LL_RCC_USART6_CLKSOURCE_PCLK2      LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART6SEL, RCC_CCIPR13_USART6SEL_Pos, 0U)
#define LL_RCC_USART6_CLKSOURCE_CLKP       LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART6SEL, RCC_CCIPR13_USART6SEL_Pos, RCC_CCIPR13_USART6SEL_0)
#define LL_RCC_USART6_CLKSOURCE_IC9        LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART6SEL, RCC_CCIPR13_USART6SEL_Pos, RCC_CCIPR13_USART6SEL_1)
#define LL_RCC_USART6_CLKSOURCE_IC14       LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART6SEL, RCC_CCIPR13_USART6SEL_Pos, RCC_CCIPR13_USART6SEL_1 |\
                                                        RCC_CCIPR13_USART6SEL_0)
#define LL_RCC_USART6_CLKSOURCE_LSE        LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART6SEL, RCC_CCIPR13_USART6SEL_Pos, RCC_CCIPR13_USART6SEL_2)
#define LL_RCC_USART6_CLKSOURCE_MSI        LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART6SEL, RCC_CCIPR13_USART6SEL_Pos, RCC_CCIPR13_USART6SEL_2 |\
                                                        RCC_CCIPR13_USART6SEL_0)
#define LL_RCC_USART6_CLKSOURCE_HSI        LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART6SEL, RCC_CCIPR13_USART6SEL_Pos, RCC_CCIPR13_USART6SEL_2 |\
                                                        RCC_CCIPR13_USART6SEL_1)

#define LL_RCC_USART10_CLKSOURCE_PCLK2     LL_CLKSOURCE(CCIPR14_OFFSET, RCC_CCIPR14_USART10SEL, RCC_CCIPR14_USART10SEL_Pos, 0U)
#define LL_RCC_USART10_CLKSOURCE_CLKP      LL_CLKSOURCE(CCIPR14_OFFSET, RCC_CCIPR14_USART10SEL, RCC_CCIPR14_USART10SEL_Pos, RCC_CCIPR14_USART10SEL_0)
#define LL_RCC_USART10_CLKSOURCE_IC9       LL_CLKSOURCE(CCIPR14_OFFSET, RCC_CCIPR14_USART10SEL, RCC_CCIPR14_USART10SEL_Pos, RCC_CCIPR14_USART10SEL_1)
#define LL_RCC_USART10_CLKSOURCE_IC14      LL_CLKSOURCE(CCIPR14_OFFSET, RCC_CCIPR14_USART10SEL, RCC_CCIPR14_USART10SEL_Pos, RCC_CCIPR14_USART10SEL_1 |\
                                                        RCC_CCIPR14_USART10SEL_0)
#define LL_RCC_USART10_CLKSOURCE_LSE       LL_CLKSOURCE(CCIPR14_OFFSET, RCC_CCIPR14_USART10SEL, RCC_CCIPR14_USART10SEL_Pos, RCC_CCIPR14_USART10SEL_2)
#define LL_RCC_USART10_CLKSOURCE_MSI       LL_CLKSOURCE(CCIPR14_OFFSET, RCC_CCIPR14_USART10SEL, RCC_CCIPR14_USART10SEL_Pos, RCC_CCIPR14_USART10SEL_2 |\
                                                        RCC_CCIPR14_USART10SEL_0)
#define LL_RCC_USART10_CLKSOURCE_HSI       LL_CLKSOURCE(CCIPR14_OFFSET, RCC_CCIPR14_USART10SEL, RCC_CCIPR14_USART10SEL_Pos, RCC_CCIPR14_USART10SEL_2 |\
                                                        RCC_CCIPR14_USART10SEL_1)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_XSPI_CLKSOURCE  Peripheral XSPI clock source selection
  * @{
  */
#define LL_RCC_XSPI1_CLKSOURCE_HCLK        LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_XSPI1SEL, RCC_CCIPR6_XSPI1SEL_Pos, 0U)
#define LL_RCC_XSPI1_CLKSOURCE_CLKP        LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_XSPI1SEL, RCC_CCIPR6_XSPI1SEL_Pos, RCC_CCIPR6_XSPI1SEL_0)
#define LL_RCC_XSPI1_CLKSOURCE_IC3         LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_XSPI1SEL, RCC_CCIPR6_XSPI1SEL_Pos, RCC_CCIPR6_XSPI1SEL_1)
#define LL_RCC_XSPI1_CLKSOURCE_IC4         LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_XSPI1SEL, RCC_CCIPR6_XSPI1SEL_Pos, RCC_CCIPR6_XSPI1SEL_1 |\
                                                        RCC_CCIPR6_XSPI1SEL_0)

#define LL_RCC_XSPI2_CLKSOURCE_HCLK        LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_XSPI2SEL, RCC_CCIPR6_XSPI2SEL_Pos, 0U)
#define LL_RCC_XSPI2_CLKSOURCE_CLKP        LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_XSPI2SEL, RCC_CCIPR6_XSPI2SEL_Pos, RCC_CCIPR6_XSPI2SEL_0)
#define LL_RCC_XSPI2_CLKSOURCE_IC3         LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_XSPI2SEL, RCC_CCIPR6_XSPI2SEL_Pos, RCC_CCIPR6_XSPI2SEL_1)
#define LL_RCC_XSPI2_CLKSOURCE_IC4         LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_XSPI2SEL, RCC_CCIPR6_XSPI2SEL_Pos, RCC_CCIPR6_XSPI2SEL_1 |\
                                                        RCC_CCIPR6_XSPI2SEL_0)

#define LL_RCC_XSPI3_CLKSOURCE_HCLK        LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_XSPI3SEL, RCC_CCIPR6_XSPI3SEL_Pos, 0U)
#define LL_RCC_XSPI3_CLKSOURCE_CLKP        LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_XSPI3SEL, RCC_CCIPR6_XSPI3SEL_Pos, RCC_CCIPR6_XSPI3SEL_0)
#define LL_RCC_XSPI3_CLKSOURCE_IC3         LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_XSPI3SEL, RCC_CCIPR6_XSPI3SEL_Pos, RCC_CCIPR6_XSPI3SEL_1)
#define LL_RCC_XSPI3_CLKSOURCE_IC4         LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_XSPI3SEL, RCC_CCIPR6_XSPI3SEL_Pos, RCC_CCIPR6_XSPI3SEL_1 |\
                                                        RCC_CCIPR6_XSPI3SEL_0)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_ADC  Peripheral ADC get clock source
  * @{
  */
#define LL_RCC_ADC_CLKSOURCE               RCC_CCIPR1_ADC12SEL
/**
  * @}
  */

/** @defgroup RCC_LL_EC_ADF  Peripheral ADF get clock source
  * @{
  */
#define LL_RCC_ADF1_CLKSOURCE              RCC_CCIPR1_ADF1SEL
/**
  * @}
  */

/** @defgroup RCC_LL_EC_CLKP  Peripheral CLKP get clock source
  * @{
  */
#define LL_RCC_CLKP_CLKSOURCE              RCC_CCIPR7_PERSEL
/**
  * @}
  */

/** @defgroup RCC_LL_EC_DCMIPP  Peripheral DCMIPP get clock source
  * @{
  */
#define LL_RCC_DCMIPP_CLKSOURCE             RCC_CCIPR1_DCMIPPSEL
/**
  * @}
  */

/** @defgroup RCC_LL_EC_ETH  Peripheral ETH get clock source
  * @{
  */
#define LL_RCC_ETH1_CLKSOURCE              RCC_CCIPR2_ETH1CLKSEL
/**
  * @}
  */

/** @defgroup RCC_LL_EC_ETHPHY  Peripheral ETH PHY get interface
  * @{
  */
#define LL_RCC_ETH1PHY_IF                  RCC_CCIPR2_ETH1SEL
/**
  * @}
  */

/** @defgroup RCC_LL_EC_ETHPTP  Peripheral ETHPTP get clock source
  * @{
  */
#define LL_RCC_ETH1PTP_CLKSOURCE           RCC_CCIPR2_ETH1PTPSEL
/**
  * @}
  */

/** @defgroup RCC_LL_EC_ETHREFRX  Peripheral ETH Reference RX get clock source
  * @{
  */
#define LL_RCC_ETH1REFRX_CLKSOURCE         RCC_CCIPR2_ETH1REFCLKSEL
/**
  * @}
  */

/** @defgroup RCC_LL_EC_ETHREFTX  Peripheral ETH Reference TX get clock source
  * @{
  */
#define LL_RCC_ETH1REFTX_CLKSOURCE         RCC_CCIPR2_ETH1GTXCLKSEL
/**
  * @}
  */

/** @defgroup RCC_LL_EC_FDCAN  Peripheral FDCAN get clock source
  * @{
  */
#define LL_RCC_FDCAN_CLKSOURCE             RCC_CCIPR3_FDCANSEL
/**
  * @}
  */

/** @defgroup RCC_LL_EC_FMC  Peripheral FMC get clock source
  * @{
  */
#define LL_RCC_FMC_CLKSOURCE               RCC_CCIPR3_FMCSEL
/**
  * @}
  */

/** @defgroup RCC_LL_EC_I2C  Peripheral I2C get clock source
  * @{
  */
#define LL_RCC_I2C1_CLKSOURCE              LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I2C1SEL, RCC_CCIPR4_I2C1SEL_Pos, 0U)
#define LL_RCC_I2C2_CLKSOURCE              LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I2C2SEL, RCC_CCIPR4_I2C2SEL_Pos, 0U)
#define LL_RCC_I2C3_CLKSOURCE              LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I2C3SEL, RCC_CCIPR4_I2C3SEL_Pos, 0U)
#define LL_RCC_I2C4_CLKSOURCE              LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I2C4SEL, RCC_CCIPR4_I2C4SEL_Pos, 0U)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_I3C  Peripheral I3C get clock source
  * @{
  */
#define LL_RCC_I3C1_CLKSOURCE              LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I3C1SEL, RCC_CCIPR4_I3C1SEL_Pos, 0U)
#define LL_RCC_I3C2_CLKSOURCE              LL_CLKSOURCE(CCIPR4_OFFSET, RCC_CCIPR4_I3C2SEL, RCC_CCIPR4_I3C2SEL_Pos, 0U)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_LPTIM  Peripheral LPTIM get clock source
  * @{
  */
#define LL_RCC_LPTIM1_CLKSOURCE            LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM1SEL, RCC_CCIPR12_LPTIM1SEL_Pos, 0U)
#define LL_RCC_LPTIM2_CLKSOURCE            LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM2SEL, RCC_CCIPR12_LPTIM2SEL_Pos, 0U)
#define LL_RCC_LPTIM3_CLKSOURCE            LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM3SEL, RCC_CCIPR12_LPTIM3SEL_Pos, 0U)
#define LL_RCC_LPTIM4_CLKSOURCE            LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM4SEL, RCC_CCIPR12_LPTIM4SEL_Pos, 0U)
#define LL_RCC_LPTIM5_CLKSOURCE            LL_CLKSOURCE(CCIPR12_OFFSET, RCC_CCIPR12_LPTIM5SEL, RCC_CCIPR12_LPTIM5SEL_Pos, 0U)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_LPUART  Peripheral LPUART get clock source
  * @{
  */
#define LL_RCC_LPUART1_CLKSOURCE           RCC_CCIPR14_LPUART1SEL
/**
  * @}
  */

/** @defgroup RCC_LL_EC_LTDC  Peripheral LTDC get clock source
  * @{
  */
#define LL_RCC_LTDC_CLKSOURCE              RCC_CCIPR4_LTDCSEL
/**
  * @}
  */

/** @defgroup RCC_LL_EC_MDF  Peripheral MDF get clock source
  * @{
  */
#define LL_RCC_MDF1_CLKSOURCE              RCC_CCIPR5_MDF1SEL
/**
  * @}
  */

/** @defgroup RCC_LL_EC_OTGPHY  Peripheral OTGPHY get clock source
  * @{
  */
#define LL_RCC_OTGPHY1_CLKSOURCE           LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_OTGPHY1SEL, RCC_CCIPR6_OTGPHY1SEL_Pos, 0U)
#define LL_RCC_OTGPHY2_CLKSOURCE           LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_OTGPHY2SEL, RCC_CCIPR6_OTGPHY2SEL_Pos, 0U)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_OTGPHYCKREF  Peripheral OTGPHYCKREF get clock source
  * @{
  */
#define LL_RCC_OTGPHY1CKREF_CLKSOURCE      LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_OTGPHY1CKREFSEL, RCC_CCIPR6_OTGPHY1CKREFSEL_Pos, 0U)
#define LL_RCC_OTGPHY2CKREF_CLKSOURCE      LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_OTGPHY2CKREFSEL, RCC_CCIPR6_OTGPHY2CKREFSEL_Pos, 0U)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_PSSI  Peripheral PSSI get clock source
  * @{
  */
#define LL_RCC_PSSI_CLKSOURCE              RCC_CCIPR7_PSSISEL
/**
  * @}
  */

/** @defgroup RCC_LL_EC_SAI  Peripheral SAI get clock source
  * @{
  */
#define LL_RCC_SAI1_CLKSOURCE              LL_CLKSOURCE(CCIPR7_OFFSET, RCC_CCIPR7_SAI1SEL, RCC_CCIPR7_SAI1SEL_Pos, 0U)
#define LL_RCC_SAI2_CLKSOURCE              LL_CLKSOURCE(CCIPR7_OFFSET, RCC_CCIPR7_SAI2SEL, RCC_CCIPR7_SAI2SEL_Pos, 0U)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_SDMMC  Peripheral SDMMC get clock source
  * @{
  */
#define LL_RCC_SDMMC1_CLKSOURCE            LL_CLKSOURCE(CCIPR8_OFFSET, RCC_CCIPR8_SDMMC1SEL, RCC_CCIPR8_SDMMC1SEL_Pos, 0U)
#define LL_RCC_SDMMC2_CLKSOURCE            LL_CLKSOURCE(CCIPR8_OFFSET, RCC_CCIPR8_SDMMC2SEL, RCC_CCIPR8_SDMMC2SEL_Pos, 0U)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_SPDIFRX  Peripheral SPDIFRX get clock source
  * @{
  */
#define LL_RCC_SPDIFRX1_CLKSOURCE          RCC_CCIPR9_SPDIFRX1SEL
/**
  * @}
  */

/** @defgroup RCC_LL_EC_SPI  Peripheral SPI get clock source
  * @{
  */
#define LL_RCC_SPI1_CLKSOURCE              LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI1SEL, RCC_CCIPR9_SPI1SEL_Pos, 0U)
#define LL_RCC_SPI2_CLKSOURCE              LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI2SEL, RCC_CCIPR9_SPI2SEL_Pos, 0U)
#define LL_RCC_SPI3_CLKSOURCE              LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI3SEL, RCC_CCIPR9_SPI3SEL_Pos, 0U)
#define LL_RCC_SPI4_CLKSOURCE              LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI4SEL, RCC_CCIPR9_SPI4SEL_Pos, 0U)
#define LL_RCC_SPI5_CLKSOURCE              LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI5SEL, RCC_CCIPR9_SPI5SEL_Pos, 0U)
#define LL_RCC_SPI6_CLKSOURCE              LL_CLKSOURCE(CCIPR9_OFFSET, RCC_CCIPR9_SPI6SEL, RCC_CCIPR9_SPI6SEL_Pos, 0U)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_UART  Peripheral UART get clock source
  * @{
  */
#define LL_RCC_UART4_CLKSOURCE             LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART4SEL, RCC_CCIPR13_UART4SEL_Pos, 0U)
#define LL_RCC_UART5_CLKSOURCE             LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART5SEL, RCC_CCIPR13_UART5SEL_Pos, 0U)
#define LL_RCC_UART7_CLKSOURCE             LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART7SEL, RCC_CCIPR13_UART7SEL_Pos, 0U)
#define LL_RCC_UART8_CLKSOURCE             LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_UART8SEL, RCC_CCIPR13_UART8SEL_Pos, 0U)
#define LL_RCC_UART9_CLKSOURCE             LL_CLKSOURCE(CCIPR14_OFFSET, RCC_CCIPR14_UART9SEL, RCC_CCIPR14_UART9SEL_Pos, 0U)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_USART  Peripheral USART get clock source
  * @{
  */
#define LL_RCC_USART1_CLKSOURCE            LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART1SEL, RCC_CCIPR13_USART1SEL_Pos, 0U)
#define LL_RCC_USART2_CLKSOURCE            LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART2SEL, RCC_CCIPR13_USART2SEL_Pos, 0U)
#define LL_RCC_USART3_CLKSOURCE            LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART3SEL, RCC_CCIPR13_USART3SEL_Pos, 0U)
#define LL_RCC_USART6_CLKSOURCE            LL_CLKSOURCE(CCIPR13_OFFSET, RCC_CCIPR13_USART6SEL, RCC_CCIPR13_USART6SEL_Pos, 0U)
#define LL_RCC_USART10_CLKSOURCE           LL_CLKSOURCE(CCIPR14_OFFSET, RCC_CCIPR14_USART10SEL, RCC_CCIPR14_USART10SEL_Pos, 0U)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_XSPI  Peripheral XSPI get clock source
  * @{
  */
#define LL_RCC_XSPI1_CLKSOURCE             LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_XSPI1SEL, RCC_CCIPR6_XSPI1SEL_Pos, 0U)
#define LL_RCC_XSPI2_CLKSOURCE             LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_XSPI2SEL, RCC_CCIPR6_XSPI2SEL_Pos, 0U)
#define LL_RCC_XSPI3_CLKSOURCE             LL_CLKSOURCE(CCIPR6_OFFSET, RCC_CCIPR6_XSPI3SEL, RCC_CCIPR6_XSPI3SEL_Pos, 0U)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_TIM_CLKPRESCALER  Timers clocks prescalers selection
  * @{
  */
#define LL_RCC_TIM_PRESCALER_1             0U
#define LL_RCC_TIM_PRESCALER_2             1U
#define LL_RCC_TIM_PRESCALER_4             2U
#define LL_RCC_TIM_PRESCALER_8             3U
/**
  * @}
  */

/** @defgroup RCC_LL_EC_PLLSOURCE   All PLLs entry clock source
  * @{
  */
#define LL_RCC_PLLSOURCE_HSI               0U
#define LL_RCC_PLLSOURCE_MSI               RCC_PLL1CFGR1_PLL1SEL_0
#define LL_RCC_PLLSOURCE_HSE               RCC_PLL1CFGR1_PLL1SEL_1
#define LL_RCC_PLLSOURCE_I2S_CKIN          (RCC_PLL1CFGR1_PLL1SEL_1 | RCC_PLL1CFGR1_PLL1SEL_0)
/**
  * @}
  */

/** @defgroup RCC_LL_EC_ICSOURCE   All ICs entry clock source
  * @{
  */
#define LL_RCC_ICCLKSOURCE_PLL1               0U
#define LL_RCC_ICCLKSOURCE_PLL2               RCC_IC1CFGR_IC1SEL_0
#define LL_RCC_ICCLKSOURCE_PLL3               RCC_IC1CFGR_IC1SEL_1
#define LL_RCC_ICCLKSOURCE_PLL4               (RCC_IC1CFGR_IC1SEL_1 | RCC_IC1CFGR_IC1SEL_0)
/**
  * @}
  */

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/
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
  * @brief  Helper macro to calculate the HCLK frequency
  * @param  __SYSCLKFREQ__ SYSCLK frequency.
  * @param  __HPRESCALER__ This parameter can be one of the following values:
  *         @arg @ref LL_RCC_AHB_DIV_1
  *         @arg @ref LL_RCC_AHB_DIV_2
  *         @arg @ref LL_RCC_AHB_DIV_4
  *         @arg @ref LL_RCC_AHB_DIV_8
  *         @arg @ref LL_RCC_AHB_DIV_16
  *         @arg @ref LL_RCC_AHB_DIV_32
  *         @arg @ref LL_RCC_AHB_DIV_64
  *         @arg @ref LL_RCC_AHB_DIV_128
  * @retval HCLK clock frequency (in Hz)
  */
#define LL_RCC_CALC_HCLK_FREQ(__SYSCLKFREQ__, __HPRESCALER__) ((__SYSCLKFREQ__) >> (((__HPRESCALER__) &\
                                                               RCC_CFGR2_HPRE) >> RCC_CFGR2_HPRE_Pos))

/**
  * @brief  Helper macro to calculate the PCLK1 frequency (ABP1)
  * @param  __HCLKFREQ__ HCLK frequency
  * @param  __APB1PRESCALER__ This parameter can be one of the following values:
  *         @arg @ref LL_RCC_APB1_DIV_1
  *         @arg @ref LL_RCC_APB1_DIV_2
  *         @arg @ref LL_RCC_APB1_DIV_4
  *         @arg @ref LL_RCC_APB1_DIV_8
  *         @arg @ref LL_RCC_APB1_DIV_16
  *         @arg @ref LL_RCC_APB1_DIV_32
  *         @arg @ref LL_RCC_APB1_DIV_64
  *         @arg @ref LL_RCC_APB1_DIV_128
  * @retval PCLK1 clock frequency (in Hz)
  */
#define LL_RCC_CALC_PCLK1_FREQ(__HCLKFREQ__, __APB1PRESCALER__) ((__HCLKFREQ__) >> (((__APB1PRESCALER__) &\
                                                                 RCC_CFGR2_PPRE1) >> RCC_CFGR2_PPRE1_Pos))

/**
  * @brief  Helper macro to calculate the PCLK2 frequency (ABP2)
  * @param  __HCLKFREQ__ HCLK frequency
  * @param  __APB2PRESCALER__ This parameter can be one of the following values:
  *         @arg @ref LL_RCC_APB2_DIV_1
  *         @arg @ref LL_RCC_APB2_DIV_2
  *         @arg @ref LL_RCC_APB2_DIV_4
  *         @arg @ref LL_RCC_APB2_DIV_8
  *         @arg @ref LL_RCC_APB2_DIV_16
  *         @arg @ref LL_RCC_APB2_DIV_32
  *         @arg @ref LL_RCC_APB2_DIV_64
  *         @arg @ref LL_RCC_APB2_DIV_128
  * @retval PCLK2 clock frequency (in Hz)
  */
#define LL_RCC_CALC_PCLK2_FREQ(__HCLKFREQ__, __APB2PRESCALER__) ((__HCLKFREQ__) >> (((__APB2PRESCALER__) &\
                                                                 RCC_CFGR2_PPRE2) >> RCC_CFGR2_PPRE2_Pos))

/**
  * @brief  Helper macro to calculate the PCLK4 frequency (ABP4)
  * @param  __HCLKFREQ__ HCLK frequency
  * @param  __APB4PRESCALER__ This parameter can be one of the following values:
  *         @arg @ref LL_RCC_APB4_DIV_1
  *         @arg @ref LL_RCC_APB4_DIV_2
  *         @arg @ref LL_RCC_APB4_DIV_4
  *         @arg @ref LL_RCC_APB4_DIV_8
  *         @arg @ref LL_RCC_APB4_DIV_16
  *         @arg @ref LL_RCC_APB4_DIV_32
  *         @arg @ref LL_RCC_APB4_DIV_64
  *         @arg @ref LL_RCC_APB4_DIV_128
  * @retval PCLK1 clock frequency (in Hz)
  */
#define LL_RCC_CALC_PCLK4_FREQ(__HCLKFREQ__, __APB4PRESCALER__) ((__HCLKFREQ__) >> (((__APB4PRESCALER__) &\
                                                                 RCC_CFGR2_PPRE4) >> RCC_CFGR2_PPRE4_Pos))

/**
  * @brief  Helper macro to calculate the PCLK5 frequency (APB5)
  * @param  __HCLKFREQ__ HCLK frequency
  * @param  __APB5PRESCALER__ This parameter can be one of the following values:
  *         @arg @ref LL_RCC_APB5_DIV_1
  *         @arg @ref LL_RCC_APB5_DIV_2
  *         @arg @ref LL_RCC_APB5_DIV_4
  *         @arg @ref LL_RCC_APB5_DIV_8
  *         @arg @ref LL_RCC_APB5_DIV_16
  *         @arg @ref LL_RCC_APB5_DIV_32
  *         @arg @ref LL_RCC_APB5_DIV_64
  *         @arg @ref LL_RCC_APB5_DIV_128
  * @retval PCLK1 clock frequency (in Hz)
  */
#define LL_RCC_CALC_PCLK5_FREQ(__HCLKFREQ__, __APB5PRESCALER__) ((__HCLKFREQ__) >> (((__APB5PRESCALER__) &\
                                                                 RCC_CFGR2_PPRE5) >> RCC_CFGR2_PPRE5_Pos))

/**
  * @}
  */

/** @defgroup RCC_LL_EC_PERIPH_FREQUENCY Peripheral clock frequency
  * @{
  */
#define LL_RCC_PERIPH_FREQUENCY_NO         0U                 /*!< No clock enabled for the peripheral            */
#define LL_RCC_PERIPH_FREQUENCY_NA         0xFFFFFFFFUL       /*!< Frequency cannot be provided as external clock */
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
  * @brief  Enable the HSE Clock Security System.
  * @note Once HSE Clock Security System is enabled it cannot be changed anymore unless
  *       a reset occurs or system enter in standby mode.
  * @rmtoll HSECFGR      HSECSSON         LL_RCC_HSE_EnableCSS
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSE_EnableCSS(void)
{
  SET_BIT(RCC->HSECFGR, RCC_HSECFGR_HSECSSON);
}

/**
  * @brief  Check if HSE failure is detected by Clock Security System
  * @rmtoll HSECFGR      HSECSSD       LL_RCC_HSE_IsFailureDetected
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_HSE_IsFailureDetected(void)
{
  return ((READ_BIT(RCC->HSECFGR, RCC_HSECFGR_HSECSSD) != 0UL) ? 1UL : 0UL);
}

/**
  * @brief  Enable the HSE Clock Security System bypass.
  * @note Bypass the HSE oscillator when a failure is detected and get the clock from
  *       the HSI oscillator (HSI injection)
  * @rmtoll HSECFGR      HSECSSBYP     LL_RCC_HSE_EnableCSSBypass
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSE_EnableCSSBypass(void)
{
  SET_BIT(RCC->HSECFGR, RCC_HSECFGR_HSECSSBYP);
}

/**
  * @brief  Disable the HSE Clock Security System bypass.
  * @rmtoll HSECFGR      HSECSSBYP     LL_RCC_HSE_DisableCSSBypass
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSE_DisableCSSBypass(void)
{
  CLEAR_BIT(RCC->HSECFGR, RCC_HSECFGR_HSECSSBYP);
}

/**
  * @brief  Set HSE Clock Security System bypass divider
  * @note   To divide the replacement internal HSI oscillator that
  *         bypasses the HSE oscillator when a failure is detected
  * @rmtoll HSECFGR      HSECSSBPRE    LL_RCC_HSE_SetCSSBypassDivider
  * @param  Divider This parameter can be a value from RCC_LL_EC_HSECSSBYP_DIV.
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSE_SetCSSBypassDivider(uint32_t Divider)
{
  MODIFY_REG(RCC->HSECFGR, RCC_HSECFGR_HSECSSBPRE, Divider);
}

/**
  * @brief  Get HSE Clock Security System bypass divider
  * @note   To divide the replacement internal HSI oscillator that
  *         bypasses the HSE oscillator when a failure is detected
  * @rmtoll HSECFGR      HSECSSBPRE    LL_RCC_HSE_GetCSSBypassDivider
  * @retval can be a value from RCC_LL_EC_HSECSSBYP_DIV.
  */
__STATIC_INLINE uint32_t LL_RCC_HSE_GetCSSBypassDivider(void)
{
  return (READ_BIT(RCC->HSECFGR, RCC_HSECFGR_HSECSSBPRE));
}

/**
  * @brief  Enable HSE external oscillator (HSE Bypass)
  * @rmtoll HSECFGR      HSEBYP        LL_RCC_HSE_EnableBypass
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSE_EnableBypass(void)
{
  SET_BIT(RCC->HSECFGR, RCC_HSECFGR_HSEBYP);
}

/**
  * @brief  Disable HSE external oscillator (HSE Bypass)
  * @rmtoll HSECFGR      HSEBYP        LL_RCC_HSE_DisableBypass
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSE_DisableBypass(void)
{
  CLEAR_BIT(RCC->HSECFGR, RCC_HSECFGR_HSEBYP);
}

/**
  * @brief  Select the Analog HSE external clock type in Bypass mode
  * @rmtoll HSECFGR      HSEEXT        LL_RCC_HSE_SelectAnalogClock
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSE_SelectAnalogClock(void)
{
  CLEAR_BIT(RCC->HSECFGR, RCC_HSECFGR_HSEEXT);
}

/**
  * @brief  Select the Digital HSE external clock type in Bypass mode
  * @rmtoll HSECFGR      HSEEXT        LL_RCC_HSE_SelectDigitalClock
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSE_SelectDigitalClock(void)
{
  SET_BIT(RCC->HSECFGR, RCC_HSECFGR_HSEEXT);
}

/**
  * @brief  Select the HSE as hse_div2_osc_ck output clock
  * @rmtoll HSECFGR      HSEDIV2SEL    LL_RCC_HSE_SelectHSEAsDiv2Clock
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSE_SelectHSEAsDiv2Clock(void)
{
  CLEAR_BIT(RCC->HSECFGR, RCC_HSECFGR_HSEDIV2SEL);
}

/**
  * @brief  Select the HSE divided by 2 as hse_div2_osc_ck output clock
  * @rmtoll HSECFGR      HSEDIV2SEL    LL_RCC_HSE_SelectHSEDiv2AsDiv2Clock
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSE_SelectHSEDiv2AsDiv2Clock(void)
{
  SET_BIT(RCC->HSECFGR, RCC_HSECFGR_HSEDIV2SEL);
}

/**
  * @brief  Check if hse_div2_osc_ck output clock is divided by 2
  * @rmtoll HSECFGR      HSEDIV2SEL    LL_RCC_HSE_IsSelectedHSEDiv2AsDiv2Clock
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_HSE_IsSelectedHSEDiv2AsDiv2Clock(void)
{
  return ((READ_BIT(RCC->HSECFGR, RCC_HSECFGR_HSEDIV2SEL) == RCC_HSECFGR_HSEDIV2SEL) ? 1UL : 0UL);
}

/**
  * @brief  Enable HSE crystal oscillator (HSE ON)
  * @rmtoll CSR          HSEONS        LL_RCC_HSE_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSE_Enable(void)
{
  WRITE_REG(RCC->CSR, RCC_CSR_HSEONS);
}

/**
  * @brief  Disable HSE crystal oscillator (HSE ON)
  * @rmtoll CCR          HSEONC        LL_RCC_HSE_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSE_Disable(void)
{
  WRITE_REG(RCC->CCR, RCC_CCR_HSEONC);
}

/**
  * @brief  Check if HSE oscillator Ready
  * @rmtoll SR           HSERDY        LL_RCC_HSE_IsReady
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_HSE_IsReady(void)
{
  return ((READ_BIT(RCC->SR, RCC_SR_HSERDY) != 0UL) ? 1UL : 0UL);
}

/**
  * @}
  */

/** @defgroup RCC_LL_EF_HSI HSI
  * @{
  */

/**
  * @brief  Enable HSI oscillator
  * @rmtoll CSR          HSIONS        LL_RCC_HSI_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSI_Enable(void)
{
  WRITE_REG(RCC->CSR, RCC_CSR_HSIONS);
}

/**
  * @brief  Disable HSI oscillator
  * @rmtoll CCR          HSIONC        LL_RCC_HSI_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSI_Disable(void)
{
  WRITE_REG(RCC->CCR, RCC_CCR_HSIONC);
}

/**
  * @brief  Check if HSI clock is ready
  * @rmtoll SR           HSIRDY        LL_RCC_HSI_IsReady
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_HSI_IsReady(void)
{
  return ((READ_BIT(RCC->SR, RCC_SR_HSIRDY) != 0UL) ? 1UL : 0UL);
}

/**
  * @brief  Set HSI divider
  * @rmtoll HSICFGR      HSIDIV        LL_RCC_HSI_SetDivider
  * @param  Divider This parameter can be one of the following values:
  *         @arg @ref LL_RCC_HSI_DIV_1
  *         @arg @ref LL_RCC_HSI_DIV_2
  *         @arg @ref LL_RCC_HSI_DIV_4
  *         @arg @ref LL_RCC_HSI_DIV_8
  * @retval None.
  */
__STATIC_INLINE void LL_RCC_HSI_SetDivider(uint32_t Divider)
{
  MODIFY_REG(RCC->HSICFGR, RCC_HSICFGR_HSIDIV, Divider);
}

/**
  * @brief  Get HSI divider
  * @rmtoll HSICFGR      HSIDIV        LL_RCC_HSI_GetDivider
  * @retval can be one of the following values:
  *         @arg @ref LL_RCC_HSI_DIV_1
  *         @arg @ref LL_RCC_HSI_DIV_2
  *         @arg @ref LL_RCC_HSI_DIV_4
  *         @arg @ref LL_RCC_HSI_DIV_8
  */
__STATIC_INLINE uint32_t LL_RCC_HSI_GetDivider(void)
{
  return (READ_BIT(RCC->HSICFGR, RCC_HSICFGR_HSIDIV));
}

/**
  * @brief  Get HSI Calibration value
  * @note When HSITRIM is written, HSICAL is updated with the sum of
  *       HSITRIM and the factory trim value
  * @rmtoll HSICFGR      HSICAL        LL_RCC_HSI_GetCalibration
  * @retval A value between 0 and 511 (0x1FF)
  */
__STATIC_INLINE uint32_t LL_RCC_HSI_GetCalibration(void)
{
  return (uint32_t)(READ_BIT(RCC->HSICFGR, RCC_HSICFGR_HSICAL) >> RCC_HSICFGR_HSICAL_Pos);
}

/**
  * @brief  Set HSI Calibration trimming
  * @note user-programmable trimming value that is added to the HSICAL
  * @note Default value is 32, which, when added to the HSICAL value,
  *       should trim the HSI to 32 MHz +/- 1 %
  * @rmtoll HSICFGR      HSITRIM       LL_RCC_HSI_SetCalibTrimming
  * @param  Value This parameter can be a value between 0 and 63
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSI_SetCalibTrimming(uint32_t Value)
{
  MODIFY_REG(RCC->HSICFGR, RCC_HSICFGR_HSITRIM, Value << RCC_HSICFGR_HSITRIM_Pos);
}

/**
  * @brief  Get HSI Calibration trimming
  * @rmtoll HSICFGR      HSITRIM       LL_RCC_HSI_GetCalibTrimming
  * @retval A value between 0 and 63
  */
__STATIC_INLINE uint32_t LL_RCC_HSI_GetCalibTrimming(void)
{
  return (uint32_t)(READ_BIT(RCC->HSICFGR, RCC_HSICFGR_HSITRIM) >> RCC_HSICFGR_HSITRIM_Pos);
}

/**
  * @brief  Enable HSI even in stop mode
  * @note HSI oscillator is forced ON even in Stop mode
  * @rmtoll STOPCR       HSISTOPEN     LL_RCC_HSI_EnableInStopMode
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSI_EnableInStopMode(void)
{
  SET_BIT(RCC->STOPCR, RCC_STOPCR_HSISTOPEN);
}

/**
  * @brief  Disable HSI in stop mode
  * @rmtoll STOPCR       HSISTOPEN     LL_RCC_HSI_DisableInStopMode
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSI_DisableInStopMode(void)
{
  CLEAR_BIT(RCC->STOPCR, RCC_STOPCR_HSISTOPEN);
}

/**
  * @brief  Check if HSI is enabled in stop mode
  * @rmtoll STOPCR       HSISTOPEN     LL_RCC_HSI_IsEnabledInStopMode
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_HSI_IsEnabledInStopMode(void)
{
  return ((READ_BIT(RCC->STOPCR, RCC_STOPCR_HSISTOPEN) == RCC_STOPCR_HSISTOPEN) ? 1UL : 0UL);
}

/**
  * @}
  */

/** @defgroup RCC_LL_EF_MSI MSI
  * @{
  */

/**
  * @brief  Enable MSI oscillator
  * @rmtoll CSR          MSIONS        LL_RCC_MSI_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_MSI_Enable(void)
{
  WRITE_REG(RCC->CSR, RCC_CSR_MSIONS);
}

/**
  * @brief  Disable MSI oscillator
  * @rmtoll CCR          MSIONC        LL_RCC_MSI_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_MSI_Disable(void)
{
  WRITE_REG(RCC->CCR, RCC_CCR_MSIONC);
}

/**
  * @brief  Check if MSI clock is ready
  * @rmtoll SR           MSIRDY        LL_RCC_MSI_IsReady
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_MSI_IsReady(void)
{
  return ((READ_BIT(RCC->SR, RCC_SR_MSIRDY) != 0UL) ? 1UL : 0UL);
}

/**
  * @brief  Set MSI frequency
  * @rmtoll MSICFGR      MSIFREQSEL    LL_RCC_MSI_SetFrequency
  * @param  Value This parameter can be one of the following values:
  *         @arg @ref LL_RCC_MSI_FREQ_4MHZ
  *         @arg @ref LL_RCC_MSI_FREQ_16MHZ
  * @retval None.
  */
__STATIC_INLINE void LL_RCC_MSI_SetFrequency(uint32_t Value)
{
  MODIFY_REG(RCC->MSICFGR, RCC_MSICFGR_MSIFREQSEL, Value);
}

/**
  * @brief  Get HSI divider
  * @rmtoll MSICFGR      MSIFREQSEL    LL_RCC_MSI_GetFrequency
  * @retval can be one of the following values:
  *         @arg @ref LL_RCC_MSI_FREQ_4MHZ
  *         @arg @ref LL_RCC_MSI_FREQ_16MHZ
  */
__STATIC_INLINE uint32_t LL_RCC_MSI_GetFrequency(void)
{
  return (READ_BIT(RCC->MSICFGR, RCC_MSICFGR_MSIFREQSEL));
}


/**
  * @brief  Get MSI Calibration value
  * @note When MSITRIM is written, MSICAL is updated with the sum of
  *       MSITRIM and the factory trim value
  * @rmtoll MSICFGR      MSICAL        LL_RCC_MSI_GetCalibration
  * @retval A value between 0 and 255 (0xFF)
  */
__STATIC_INLINE uint32_t LL_RCC_MSI_GetCalibration(void)
{
  return (uint32_t)(READ_BIT(RCC->MSICFGR, RCC_MSICFGR_MSICAL) >> RCC_MSICFGR_MSICAL_Pos);
}

/**
  * @brief  Set MSI Calibration trimming
  * @note user-programmable trimming value that is added to the MSICAL
  * @note Default value is 16, which, when added to the MSICAL value,
  *       should trim the MSI to 4 MHz +/- 1 %
  * @rmtoll MSICFGR      MSITRIM       LL_RCC_MSI_SetCalibTrimming
  * @param  Value can be a value between 0 and 31
  * @retval None
  */
__STATIC_INLINE void LL_RCC_MSI_SetCalibTrimming(uint32_t Value)
{
  MODIFY_REG(RCC->MSICFGR, RCC_MSICFGR_MSITRIM, Value << RCC_MSICFGR_MSITRIM_Pos);
}

/**
  * @brief  Get MSI Calibration trimming
  * @rmtoll MSICFGR      MSITRIM       LL_RCC_MSI_GetCalibTrimming
  * @retval A value between 0 and 31
  */
__STATIC_INLINE uint32_t LL_RCC_MSI_GetCalibTrimming(void)
{
  return (uint32_t)(READ_BIT(RCC->MSICFGR, RCC_MSICFGR_MSITRIM) >> RCC_MSICFGR_MSITRIM_Pos);
}

/**
  * @brief  Enable MSI even in stop mode
  * @note MSI oscillator is forced ON even in Stop mode
  * @rmtoll STOPCR       MSISTOPEN     LL_RCC_MSI_EnableInStopMode
  * @retval None
  */
__STATIC_INLINE void LL_RCC_MSI_EnableInStopMode(void)
{
  SET_BIT(RCC->STOPCR, RCC_STOPCR_MSISTOPEN);
}

/**
  * @brief  Disable MSI in stop mode
  * @rmtoll STOPCR       MSISTOPEN     LL_RCC_MSI_DisableInStopMode
  * @retval None
  */
__STATIC_INLINE void LL_RCC_MSI_DisableInStopMode(void)
{
  CLEAR_BIT(RCC->STOPCR, RCC_STOPCR_MSISTOPEN);
}

/**
  * @brief  Check if MSI is enabled in stop mode
  * @rmtoll STOPCR       MSISTOPEN     LL_RCC_MSI_IsEnabledInStopMode
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_MSI_IsEnabledInStopMode(void)
{
  return ((READ_BIT(RCC->STOPCR, RCC_STOPCR_MSISTOPEN) == RCC_STOPCR_MSISTOPEN) ? 1UL : 0UL);
}

/**
  * @}
  */

/** @defgroup RCC_LL_EF_LSE LSE
  * @{
  */

/**
  * @brief  Enable the Clock Security System on LSE
  * @note Once LSE Clock Security System is enabled it cannot be changed anymore unless
  *       a clock failure is detected.
  * @rmtoll LSECFGR      LSECSSON         LL_RCC_LSE_EnableCSS
  * @retval None
  */
__STATIC_INLINE void LL_RCC_LSE_EnableCSS(void)
{
  SET_BIT(RCC->LSECFGR, RCC_LSECFGR_LSECSSON);
}

/**
  * @brief  Disable the Clock Security System on LSE
  * @note Once LSE Clock Security System is enabled it cannot be changed anymore unless
  *       a clock failure is detected.
  * @rmtoll LSECFGR      LSECSSON         LL_RCC_LSE_DisableCSS
  * @retval None
  */
__STATIC_INLINE void LL_RCC_LSE_DisableCSS(void)
{
  CLEAR_BIT(RCC->LSECFGR, RCC_LSECFGR_LSECSSON);
}

/**
  * @brief  Check if LSE failure is detected by Clock Security System
  * @rmtoll LSECFGR      LSECSSD       LL_RCC_LSE_IsFailureDetected
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_LSE_IsFailureDetected(void)
{
  return ((READ_BIT(RCC->LSECFGR, RCC_LSECFGR_LSECSSD) != 0UL) ? 1UL : 0UL);
}

/**
  * @brief  Re-arm the Clock Security System on LSE
  * @note Once a clock failure is detected, the LSE Clock Security System can be re-armed providing that
  *       LSECSSON is disabled.
  * @rmtoll LSECFGR      LSECSSRA         LL_RCC_LSE_ReArmCSS
  * @retval None
  */
__STATIC_INLINE void LL_RCC_LSE_ReArmCSS(void)
{
  SET_BIT(RCC->LSECFGR, RCC_LSECFGR_LSECSSRA);
}

/**
  * @brief  Enable external clock source (LSE bypass)
  * @rmtoll LSECFGR      LSEBYP        LL_RCC_LSE_EnableBypass
  * @retval None
  */
__STATIC_INLINE void LL_RCC_LSE_EnableBypass(void)
{
  SET_BIT(RCC->LSECFGR, RCC_LSECFGR_LSEBYP);
}

/**
  * @brief  Disable external clock source (LSE bypass)
  * @rmtoll LSECFGR      LSEBYP        LL_RCC_LSE_DisableBypass
  * @retval None
  */
__STATIC_INLINE void LL_RCC_LSE_DisableBypass(void)
{
  CLEAR_BIT(RCC->LSECFGR, RCC_LSECFGR_LSEBYP);
}

/**
  * @brief  Enable Low-speed external DIGITAL clock type in Bypass mode (not to be used if RTC is active).
  * @note   The external clock must be enabled with the LSEON bit, to be used by the device.
  *         The LSEEXT bit can be written only if the LSE oscillator is disabled.
  * @rmtoll LSECFGR      LSEEXT        LL_RCC_LSE_SelectDigitalClock
  * @retval None
  */
__STATIC_INLINE void LL_RCC_LSE_SelectDigitalClock(void)
{
  SET_BIT(RCC->LSECFGR, RCC_LSECFGR_LSEEXT);
}

/**
  * @brief  Enable Low-speed external ANALOG clock type in Bypass mode (default after Backup domain reset).
  * @note   The external clock must be enabled with the LSEON bit, to be used by the device.
  *         The LSEEXT bit can be written only if the LSE oscillator is disabled.
  * @rmtoll LSECFGR      LSEEXT        LL_RCC_LSE_SelectAnalogClock
  * @retval None
  */
__STATIC_INLINE void LL_RCC_LSE_SelectAnalogClock(void)
{
  CLEAR_BIT(RCC->LSECFGR, RCC_LSECFGR_LSEEXT);
}

/**
  * @brief  Set LSE oscillator drive capability
  * @note The oscillator is in Xtal mode when it is not in bypass mode.
  * @rmtoll LSECFGR      LSEDRV        LL_RCC_LSE_SetDriveCapability
  * @param  LSEDrive This parameter can be one of the following values:
  *         @arg @ref LL_RCC_LSEDRIVE_LOW
  *         @arg @ref LL_RCC_LSEDRIVE_MEDIUMLOW
  *         @arg @ref LL_RCC_LSEDRIVE_MEDIUMHIGH
  *         @arg @ref LL_RCC_LSEDRIVE_HIGH
  * @retval None
  */
__STATIC_INLINE void LL_RCC_LSE_SetDriveCapability(uint32_t LSEDrive)
{
  MODIFY_REG(RCC->LSECFGR, RCC_LSECFGR_LSEDRV, LSEDrive);
}

/**
  * @brief  Get LSE oscillator drive capability
  * @rmtoll LSECFGR      LSEDRV        LL_RCC_LSE_GetDriveCapability
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_LSEDRIVE_LOW
  *         @arg @ref LL_RCC_LSEDRIVE_MEDIUMLOW
  *         @arg @ref LL_RCC_LSEDRIVE_MEDIUMHIGH
  *         @arg @ref LL_RCC_LSEDRIVE_HIGH
  */
__STATIC_INLINE uint32_t LL_RCC_LSE_GetDriveCapability(void)
{
  return (uint32_t)(READ_BIT(RCC->LSECFGR, RCC_LSECFGR_LSEDRV));
}

/**
  * @brief  Enable  Low Speed External (LSE) crystal
  * @rmtoll CSR          LSEONS        LL_RCC_LSE_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_LSE_Enable(void)
{
  WRITE_REG(RCC->CSR, RCC_CSR_LSEONS);
}

/**
  * @brief  Disable  Low Speed External (LSE) crystal
  * @rmtoll CCR          LSEONC        LL_RCC_LSE_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_LSE_Disable(void)
{
  WRITE_REG(RCC->CCR, RCC_CCR_LSEONC);
}

/**
  * @brief  Check if LSE oscillator Ready
  * @rmtoll SR           LSERDY        LL_RCC_LSE_IsReady
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_LSE_IsReady(void)
{
  return ((READ_BIT(RCC->SR, RCC_SR_LSERDY) != 0UL) ? 1UL : 0UL);
}

/**
  * @brief  Enable the LSE clock glitch filter
  * @note   This API shall be called only when LSE is disabled.
  * @rmtoll LSECFGR      LSEGFON          LL_RCC_LSE_EnableGlitchFilter
  * @retval None
  */
__STATIC_INLINE void LL_RCC_LSE_EnableGlitchFilter(void)
{
  SET_BIT(RCC->LSECFGR, RCC_LSECFGR_LSEGFON);
}

/**
  * @brief  Disable the LSE clock glitch filter
  * @note   This API shall be called only when LSE is disabled.
  * @rmtoll LSECFGR      LSEGFON         LL_RCC_LSE_DisableGlitchFilter
  * @retval None
  */
__STATIC_INLINE void LL_RCC_LSE_DisableGlitchFilter(void)
{
  CLEAR_BIT(RCC->LSECFGR, RCC_LSECFGR_LSEGFON);
}

/**
  * @brief  Check if LSE clock glitch filter is enabled
  * @rmtoll LSECFGR      LSEGFON         LL_RCC_LSE_IsEnabledGlitchFilter
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_LSE_IsEnabledGlitchFilter(void)
{
  return ((READ_BIT(RCC->STOPCR, RCC_LSECFGR_LSEGFON) == RCC_LSECFGR_LSEGFON) ? 1UL : 0UL);
}

/**
  * @}
  */

/** @defgroup RCC_LL_EF_LSI LSI
  * @{
  */

/**
  * @brief  Enable LSI Oscillator
  * @rmtoll CSR          LSIONS        LL_RCC_LSI_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_LSI_Enable(void)
{
  WRITE_REG(RCC->CSR, RCC_CSR_LSIONS);
}

/**
  * @brief  Disable LSI Oscillator
  * @rmtoll CCR          LSIONC        LL_RCC_LSI_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_LSI_Disable(void)
{
  WRITE_REG(RCC->CCR, RCC_CCR_LSIONC);
}

/**
  * @brief  Check if LSI is Ready
  * @rmtoll SR           LSIRDY        LL_RCC_LSI_IsReady
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_LSI_IsReady(void)
{
  return ((READ_BIT(RCC->SR, RCC_SR_LSIRDY) != 0UL) ? 1UL : 0UL);
}

/**
  * @}
  */

/** @defgroup RCC_LL_EF_System System
  * @{
  */

/**
  * @brief  Configure the CPU clock source
  * @rmtoll CFGR1        CPUSW         LL_RCC_SetCpuClkSource
  * @param  Source This parameter can be one of the following values:
  *         @arg @ref LL_RCC_CPU_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_CPU_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_CPU_CLKSOURCE_HSE
  *         @arg @ref LL_RCC_CPU_CLKSOURCE_IC1
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetCpuClkSource(uint32_t Source)
{
  MODIFY_REG(RCC->CFGR1, RCC_CFGR1_CPUSW, Source);
}

/**
  * @brief  Get the CPU clock source
  * @rmtoll CFGR1        CPUSWS        LL_RCC_GetCpuClkSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_CPU_CLKSOURCE_STATUS_HSI
  *         @arg @ref LL_RCC_CPU_CLKSOURCE_STATUS_MSI
  *         @arg @ref LL_RCC_CPU_CLKSOURCE_STATUS_HSE
  *         @arg @ref LL_RCC_CPU_CLKSOURCE_STATUS_IC1
  */
__STATIC_INLINE uint32_t LL_RCC_GetCpuClkSource(void)
{
  return (uint32_t)(READ_BIT(RCC->CFGR1, RCC_CFGR1_CPUSWS));
}

/**
  * @brief  Configure the system clock source (bus clock)
  * @rmtoll CFGR1        SYSSW         LL_RCC_SetSysClkSource
  * @param  Source This parameter can be one of the following values:
  *         @arg @ref LL_RCC_SYS_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SYS_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SYS_CLKSOURCE_HSE
  *         @arg @ref LL_RCC_SYS_CLKSOURCE_IC2_IC6_IC11
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetSysClkSource(uint32_t Source)
{
  MODIFY_REG(RCC->CFGR1, RCC_CFGR1_SYSSW, Source);
}

/**
  * @brief  Get the system clock source (bus clock)
  * @rmtoll CFGR1        SYSSWS        LL_RCC_GetSysClkSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_SYS_CLKSOURCE_STATUS_HSI
  *         @arg @ref LL_RCC_SYS_CLKSOURCE_STATUS_MSI
  *         @arg @ref LL_RCC_SYS_CLKSOURCE_STATUS_HSE
  *         @arg @ref LL_RCC_SYS_CLKSOURCE_STATUS_IC2_IC6_IC11
  */
__STATIC_INLINE uint32_t LL_RCC_GetSysClkSource(void)
{
  return (uint32_t)(READ_BIT(RCC->CFGR1, RCC_CFGR1_SYSSWS));
}

/**
  * @brief  Configure the system wakeup clock source
  * @rmtoll CFGR1        STOPWUCK       LL_RCC_SetSysWakeUpClkSource
  * @param  Source This parameter can be one of the following values:
  *         @arg @ref LL_RCC_SYSWAKEUP_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SYSWAKEUP_CLKSOURCE_MSI
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetSysWakeUpClkSource(uint32_t Source)
{
  MODIFY_REG(RCC->CFGR1, RCC_CFGR1_STOPWUCK, Source);
}

/**
  * @brief  Get the system wakeup clock source
  * @rmtoll CFGR1        STOPWUCK           LL_RCC_GetSysWakeUpClkSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_SYSWAKEUP_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SYSWAKEUP_CLKSOURCE_MSI
  */
__STATIC_INLINE uint32_t LL_RCC_GetSysWakeUpClkSource(void)
{
  return (uint32_t)(READ_BIT(RCC->CFGR1, RCC_CFGR1_STOPWUCK));
}

/**
  * @brief  Set AHB prescaler
  * @rmtoll CFGR2         HPRE          LL_RCC_SetAHBPrescaler
  * @param  Prescaler This parameter can be one of the following values:
  *         @arg @ref LL_RCC_AHB_DIV_1
  *         @arg @ref LL_RCC_AHB_DIV_2
  *         @arg @ref LL_RCC_AHB_DIV_4
  *         @arg @ref LL_RCC_AHB_DIV_8
  *         @arg @ref LL_RCC_AHB_DIV_16
  *         @arg @ref LL_RCC_AHB_DIV_32
  *         @arg @ref LL_RCC_AHB_DIV_64
  *         @arg @ref LL_RCC_AHB_DIV_128
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetAHBPrescaler(uint32_t Prescaler)
{
  MODIFY_REG(RCC->CFGR2, RCC_CFGR2_HPRE, Prescaler);
}

/**
  * @brief  Get AHB prescaler
  * @rmtoll CFGR2         HPRE          LL_RCC_GetAHBPrescaler
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_AHB_DIV_1
  *         @arg @ref LL_RCC_AHB_DIV_2
  *         @arg @ref LL_RCC_AHB_DIV_4
  *         @arg @ref LL_RCC_AHB_DIV_8
  *         @arg @ref LL_RCC_AHB_DIV_16
  *         @arg @ref LL_RCC_AHB_DIV_32
  *         @arg @ref LL_RCC_AHB_DIV_64
  *         @arg @ref LL_RCC_AHB_DIV_128
  */
__STATIC_INLINE uint32_t LL_RCC_GetAHBPrescaler(void)
{
  return (uint32_t)(READ_BIT(RCC->CFGR2, RCC_CFGR2_HPRE));
}

/**
  * @brief  Set APB1 prescaler
  * @rmtoll CFGR2         PPRE1         LL_RCC_SetAPB1Prescaler
  * @param  Prescaler This parameter can be one of the following values:
  *         @arg @ref LL_RCC_APB1_DIV_1
  *         @arg @ref LL_RCC_APB1_DIV_2
  *         @arg @ref LL_RCC_APB1_DIV_4
  *         @arg @ref LL_RCC_APB1_DIV_8
  *         @arg @ref LL_RCC_APB1_DIV_16
  *         @arg @ref LL_RCC_APB1_DIV_32
  *         @arg @ref LL_RCC_APB1_DIV_64
  *         @arg @ref LL_RCC_APB1_DIV_128
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetAPB1Prescaler(uint32_t Prescaler)
{
  MODIFY_REG(RCC->CFGR2, RCC_CFGR2_PPRE1, Prescaler);
}

/**
  * @brief  Get APB1 prescaler
  * @rmtoll CFGR2         PPRE1         LL_RCC_GetAPB1Prescaler
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_APB1_DIV_1
  *         @arg @ref LL_RCC_APB1_DIV_2
  *         @arg @ref LL_RCC_APB1_DIV_4
  *         @arg @ref LL_RCC_APB1_DIV_8
  *         @arg @ref LL_RCC_APB1_DIV_16
  *         @arg @ref LL_RCC_APB1_DIV_32
  *         @arg @ref LL_RCC_APB1_DIV_64
  *         @arg @ref LL_RCC_APB1_DIV_128
  */
__STATIC_INLINE uint32_t LL_RCC_GetAPB1Prescaler(void)
{
  return (uint32_t)(READ_BIT(RCC->CFGR2, RCC_CFGR2_PPRE1));
}

/**
  * @brief  Set APB2 prescaler
  * @rmtoll CFGR2         PPRE2         LL_RCC_SetAPB2Prescaler
  * @param  Prescaler This parameter can be one of the following values:
  *         @arg @ref LL_RCC_APB2_DIV_1
  *         @arg @ref LL_RCC_APB2_DIV_2
  *         @arg @ref LL_RCC_APB2_DIV_4
  *         @arg @ref LL_RCC_APB2_DIV_8
  *         @arg @ref LL_RCC_APB2_DIV_16
  *         @arg @ref LL_RCC_APB2_DIV_32
  *         @arg @ref LL_RCC_APB2_DIV_64
  *         @arg @ref LL_RCC_APB2_DIV_128
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetAPB2Prescaler(uint32_t Prescaler)
{
  MODIFY_REG(RCC->CFGR2, RCC_CFGR2_PPRE2, Prescaler);
}

/**
  * @brief  Get APB2 prescaler
  * @rmtoll CFGR2         PPRE2         LL_RCC_GetAPB2Prescaler
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_APB2_DIV_1
  *         @arg @ref LL_RCC_APB2_DIV_2
  *         @arg @ref LL_RCC_APB2_DIV_4
  *         @arg @ref LL_RCC_APB2_DIV_8
  *         @arg @ref LL_RCC_APB2_DIV_16
  *         @arg @ref LL_RCC_APB2_DIV_32
  *         @arg @ref LL_RCC_APB2_DIV_64
  *         @arg @ref LL_RCC_APB2_DIV_128
  */
__STATIC_INLINE uint32_t LL_RCC_GetAPB2Prescaler(void)
{
  return (uint32_t)(READ_BIT(RCC->CFGR2, RCC_CFGR2_PPRE2));
}

/**
  * @brief  Set APB4 prescaler
  * @rmtoll CFGR2         PPRE4         LL_RCC_SetAPB4Prescaler
  * @param  Prescaler This parameter can be one of the following values:
  *         @arg @ref LL_RCC_APB4_DIV_1
  *         @arg @ref LL_RCC_APB4_DIV_2
  *         @arg @ref LL_RCC_APB4_DIV_4
  *         @arg @ref LL_RCC_APB4_DIV_8
  *         @arg @ref LL_RCC_APB4_DIV_16
  *         @arg @ref LL_RCC_APB4_DIV_32
  *         @arg @ref LL_RCC_APB4_DIV_64
  *         @arg @ref LL_RCC_APB4_DIV_128
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetAPB4Prescaler(uint32_t Prescaler)
{
  MODIFY_REG(RCC->CFGR2, RCC_CFGR2_PPRE4, Prescaler);
}

/**
  * @brief  Get APB4 prescaler
  * @rmtoll CFGR2         PPRE4         LL_RCC_GetAPB4Prescaler
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_APB4_DIV_1
  *         @arg @ref LL_RCC_APB4_DIV_2
  *         @arg @ref LL_RCC_APB4_DIV_4
  *         @arg @ref LL_RCC_APB4_DIV_8
  *         @arg @ref LL_RCC_APB4_DIV_16
  *         @arg @ref LL_RCC_APB4_DIV_32
  *         @arg @ref LL_RCC_APB4_DIV_64
  *         @arg @ref LL_RCC_APB4_DIV_128
  */
__STATIC_INLINE uint32_t LL_RCC_GetAPB4Prescaler(void)
{
  return (uint32_t)(READ_BIT(RCC->CFGR2, RCC_CFGR2_PPRE4));
}

/**
  * @brief  Set APB5 prescaler
  * @rmtoll CFGR2        PPRE5         LL_RCC_SetAPB5Prescaler
  * @param  Prescaler This parameter can be one of the following values:
  *         @arg @ref LL_RCC_APB5_DIV_1
  *         @arg @ref LL_RCC_APB5_DIV_2
  *         @arg @ref LL_RCC_APB5_DIV_4
  *         @arg @ref LL_RCC_APB5_DIV_8
  *         @arg @ref LL_RCC_APB5_DIV_16
  *         @arg @ref LL_RCC_APB5_DIV_32
  *         @arg @ref LL_RCC_APB5_DIV_64
  *         @arg @ref LL_RCC_APB5_DIV_128
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetAPB5Prescaler(uint32_t Prescaler)
{
  MODIFY_REG(RCC->CFGR2, RCC_CFGR2_PPRE5, Prescaler);
}

/**
  * @brief  Get APB5 prescaler
  * @rmtoll CFGR2        PPRE5         LL_RCC_GetAPB5Prescaler
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_APB5_DIV_1
  *         @arg @ref LL_RCC_APB5_DIV_2
  *         @arg @ref LL_RCC_APB5_DIV_4
  *         @arg @ref LL_RCC_APB5_DIV_8
  *         @arg @ref LL_RCC_APB5_DIV_16
  *         @arg @ref LL_RCC_APB5_DIV_32
  *         @arg @ref LL_RCC_APB5_DIV_64
  *         @arg @ref LL_RCC_APB5_DIV_128
  */
__STATIC_INLINE uint32_t LL_RCC_GetAPB5Prescaler(void)
{
  return (uint32_t)(READ_BIT(RCC->CFGR2, RCC_CFGR2_PPRE5));
}

/**
  * @}
  */

/** @defgroup RCC_LL_EF_MCO MCO
  * @{
  */

/**
  * @brief  Enable MCOx output
  * @note   The clock provided to the MCOx outputs must not exceed the maximum IO speed,
  *         refer to the product datasheet for information about the supported IO speed.
  * @rmtoll MISCENSR     MCO1ENS       LL_RCC_EnableMCO\n
  *         MISCENSR     MCO2ENS       LL_RCC_EnableMCO
  * @param  MCOx  This parameter can be one or a combination of the following values:
  *         @arg @ref LL_RCC_MCO1
  *         @arg @ref LL_RCC_MCO2
  * @retval None
  */
__STATIC_INLINE void LL_RCC_EnableMCO(uint32_t MCOx)
{
  WRITE_REG(RCC->MISCENSR, MCOx);
}

/**
  * @brief  Disable MCOx output
  * @rmtoll MISCENCR     MCO1ENC       LL_RCC_DisableMCO\n
  *         MISCENCR     MCO2ENC       LL_RCC_DisableMCO
  * @param  MCOx  This parameter can be one or a combination of the following values:
  *         @arg @ref LL_RCC_MCO1
  *         @arg @ref LL_RCC_MCO2
  * @retval None
  */
__STATIC_INLINE void LL_RCC_DisableMCO(uint32_t MCOx)
{
  WRITE_REG(RCC->MISCENCR, MCOx);
}

/**
  * @brief  Configure MCOx
  * @note   The clock provided to the MCOx outputs must not exceed the maximum IO speed,
  *         refer to the product datasheet for information about the supported IO speed.
  * @note   The MCO switch to the new clock source only occurs when the previous clock source is active (dynamic switch).
  * @rmtoll CCIPR5       MCO1SEL       LL_RCC_ConfigMCO\n
  *         CCIPR5       MCO1PRE       LL_RCC_ConfigMCO\n
  *         CCIPR5       MCO2SEL       LL_RCC_ConfigMCO\n
  *         CCIPR5       MCO2PRE       LL_RCC_ConfigMCO
  * @param  MCOxSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_MCO1SOURCE_LSI
  *         @arg @ref LL_RCC_MCO1SOURCE_LSE
  *         @arg @ref LL_RCC_MCO1SOURCE_MSI
  *         @arg @ref LL_RCC_MCO1SOURCE_HSI
  *         @arg @ref LL_RCC_MCO1SOURCE_HSE
  *         @arg @ref LL_RCC_MCO1SOURCE_IC5
  *         @arg @ref LL_RCC_MCO1SOURCE_IC10
  *         @arg @ref LL_RCC_MCO1SOURCE_SYSA
  *         @arg @ref LL_RCC_MCO2SOURCE_LSI
  *         @arg @ref LL_RCC_MCO2SOURCE_LSE
  *         @arg @ref LL_RCC_MCO2SOURCE_MSI
  *         @arg @ref LL_RCC_MCO2SOURCE_HSI
  *         @arg @ref LL_RCC_MCO2SOURCE_HSE
  *         @arg @ref LL_RCC_MCO2SOURCE_IC15
  *         @arg @ref LL_RCC_MCO2SOURCE_IC20
  *         @arg @ref LL_RCC_MCO2SOURCE_SYSB
  * @param  MCOxPrescaler This parameter can be one of the following values:
  *         @arg @ref LL_RCC_MCO1_DIV_1
  *         @arg @ref LL_RCC_MCO1_DIV_2
  *         @arg @ref LL_RCC_MCO1_DIV_3
  *         @arg @ref LL_RCC_MCO1_DIV_4
  *         @arg @ref LL_RCC_MCO1_DIV_5
  *         @arg @ref LL_RCC_MCO1_DIV_6
  *         @arg @ref LL_RCC_MCO1_DIV_7
  *         @arg @ref LL_RCC_MCO1_DIV_8
  *         @arg @ref LL_RCC_MCO1_DIV_9
  *         @arg @ref LL_RCC_MCO1_DIV_10
  *         @arg @ref LL_RCC_MCO1_DIV_11
  *         @arg @ref LL_RCC_MCO1_DIV_12
  *         @arg @ref LL_RCC_MCO1_DIV_13
  *         @arg @ref LL_RCC_MCO1_DIV_14
  *         @arg @ref LL_RCC_MCO1_DIV_15
  *         @arg @ref LL_RCC_MCO2_DIV_1
  *         @arg @ref LL_RCC_MCO2_DIV_2
  *         @arg @ref LL_RCC_MCO2_DIV_3
  *         @arg @ref LL_RCC_MCO2_DIV_4
  *         @arg @ref LL_RCC_MCO2_DIV_5
  *         @arg @ref LL_RCC_MCO2_DIV_6
  *         @arg @ref LL_RCC_MCO2_DIV_7
  *         @arg @ref LL_RCC_MCO2_DIV_8
  *         @arg @ref LL_RCC_MCO2_DIV_9
  *         @arg @ref LL_RCC_MCO2_DIV_10
  *         @arg @ref LL_RCC_MCO2_DIV_11
  *         @arg @ref LL_RCC_MCO2_DIV_12
  *         @arg @ref LL_RCC_MCO2_DIV_13
  *         @arg @ref LL_RCC_MCO2_DIV_14
  *         @arg @ref LL_RCC_MCO2_DIV_15
  *         @arg @ref LL_RCC_MCO2_DIV_16
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ConfigMCO(uint32_t MCOxSource, uint32_t MCOxPrescaler)
{
  MODIFY_REG(RCC->CCIPR5, ((MCOxSource | MCOxPrescaler) >> 16U), \
             (MCOxSource & (RCC_CCIPR5_MCO1SEL | RCC_CCIPR5_MCO2SEL)) | (MCOxPrescaler & (RCC_CCIPR5_MCO1PRE | RCC_CCIPR5_MCO2PRE)));
}

/**
  * @}
  */

/** @defgroup RCC_LL_EF_Peripheral_Clock_Source Peripheral Clock Source
  * @{
  */

/**
  * @brief  Configure periph clock source
  * @rmtoll CCIPR4        *     LL_RCC_SetClockSource\n
  *         CCIPR6        *     LL_RCC_SetClockSource\n
  *         CCIPR8        *     LL_RCC_SetClockSource\n
  *         CCIPR9        *     LL_RCC_SetClockSource\n
  *         CCIPR12       *     LL_RCC_SetClockSource\n
  *         CCIPR13       *     LL_RCC_SetClockSource\n
  *         CCIPR14       *     LL_RCC_SetClockSource
  * @param  ClkSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_IC10
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_IC10
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_IC10
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_I2C4_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I2C4_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_I2C4_CLKSOURCE_IC10
  *         @arg @ref LL_RCC_I2C4_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_I2C4_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_I2C4_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_I3C1_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I3C1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_I3C1_CLKSOURCE_IC10
  *         @arg @ref LL_RCC_I3C1_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_I3C1_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_I3C1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_I3C2_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I3C2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_I3C2_CLKSOURCE_IC10
  *         @arg @ref LL_RCC_I3C2_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_I3C2_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_I3C2_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_TIMG
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_PCLK4
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_TIMG
  *         @arg @ref LL_RCC_LPTIM3_CLKSOURCE_PCLK4
  *         @arg @ref LL_RCC_LPTIM3_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_LPTIM3_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_LPTIM3_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_LPTIM3_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_LPTIM3_CLKSOURCE_TIMG
  *         @arg @ref LL_RCC_LPTIM4_CLKSOURCE_PCLK4
  *         @arg @ref LL_RCC_LPTIM4_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_LPTIM4_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_LPTIM4_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_LPTIM4_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_LPTIM4_CLKSOURCE_TIMG
  *         @arg @ref LL_RCC_LPTIM5_CLKSOURCE_PCLK4
  *         @arg @ref LL_RCC_LPTIM5_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_LPTIM5_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_LPTIM5_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_LPTIM5_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_LPTIM5_CLKSOURCE_TIMG
  *         @arg @ref LL_RCC_OTGPHY1_CLKSOURCE_HSE_DIV_2
  *         @arg @ref LL_RCC_OTGPHY1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_OTGPHY1_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_OTGPHY1_CLKSOURCE_HSE_DIV_2_OSC
  *         @arg @ref LL_RCC_OTGPHY1CKREF_CLKSOURCE_OTGPHY1
  *         @arg @ref LL_RCC_OTGPHY1CKREF_CLKSOURCE_HSE_DIV_2_OSC
  *         @arg @ref LL_RCC_OTGPHY2_CLKSOURCE_HSE_DIV_2
  *         @arg @ref LL_RCC_OTGPHY2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_OTGPHY2_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_OTGPHY2_CLKSOURCE_HSE_DIV_2_OSC
  *         @arg @ref LL_RCC_OTGPHY2CKREF_CLKSOURCE_OTGPHY2
  *         @arg @ref LL_RCC_OTGPHY2CKREF_CLKSOURCE_HSE_DIV_2_OSC
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_IC7
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_I2S_CKIN
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_SPDIFRX1
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_IC7
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_I2S_CKIN
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_SPDIFRX1
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE_IC4
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE_IC5
  *         @arg @ref LL_RCC_SDMMC2_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_SDMMC2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SDMMC2_CLKSOURCE_IC4
  *         @arg @ref LL_RCC_SDMMC2_CLKSOURCE_IC5
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE_I2S_CKIN
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE_I2S_CKIN
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE_I2S_CKIN
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE_HSE
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE_HSE
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE_PCLK4
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE_I2S_CKIN
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_UART7_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_UART7_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_UART7_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_UART7_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_UART7_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_UART7_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_UART7_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_UART8_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_UART8_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_UART8_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_UART8_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_UART8_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_UART8_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_UART8_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_UART9_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_UART9_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_UART9_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_UART9_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_UART9_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_UART9_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_UART9_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_USART6_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_USART6_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_USART6_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_USART6_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_USART6_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_USART6_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_USART6_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_USART10_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_USART10_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_USART10_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_USART10_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_USART10_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_USART10_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_USART10_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_XSPI1_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_XSPI1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_XSPI1_CLKSOURCE_IC3
  *         @arg @ref LL_RCC_XSPI1_CLKSOURCE_IC4
  *         @arg @ref LL_RCC_XSPI2_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_XSPI2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_XSPI2_CLKSOURCE_IC3
  *         @arg @ref LL_RCC_XSPI2_CLKSOURCE_IC4
  *         @arg @ref LL_RCC_XSPI3_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_XSPI3_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_XSPI3_CLKSOURCE_IC3
  *         @arg @ref LL_RCC_XSPI3_CLKSOURCE_IC4
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetClockSource(uint32_t ClkSource)
{
  volatile uint32_t *pReg = (uint32_t *)((uint32_t)&RCC->CCIPR1 + LL_CLKSOURCE_REG(ClkSource));
  MODIFY_REG(*pReg, LL_CLKSOURCE_MASK(ClkSource), LL_CLKSOURCE_CONFIG(ClkSource));
}

/**
  * @brief  Set ADC prescaler
  * @rmtoll CCIPR1        ADCPRE        LL_RCC_SetADCPrescaler
  * @param  Prescaler This parameter must be a number between Min_Data = 0 and Max_Data = 255 for prescaler 1 to 256
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetADCPrescaler(uint32_t Prescaler)
{
  MODIFY_REG(RCC->CCIPR1, RCC_CCIPR1_ADCPRE, (Prescaler << RCC_CCIPR1_ADCPRE_Pos));
}

/**
  * @brief  Get ADC prescaler
  * @rmtoll CCIPR1        ADCPRE        LL_RCC_GetADCPrescaler
  * @retval Returned value between Min_Data = 0 and Max_Data = 255 for prescaler 1 to 256
  */
__STATIC_INLINE uint32_t LL_RCC_GetADCPrescaler(void)
{
  return (uint32_t)(READ_BIT(RCC->CCIPR1, RCC_CCIPR1_ADCPRE) >> RCC_CCIPR1_ADCPRE_Pos);
}

/**
  * @brief  Configure ADCx Kernel clock source
  * @rmtoll CCIPR1        ADC12SEL        LL_RCC_SetADCClockSource
  * @param  ClkSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_ADC_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_ADC_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_ADC_CLKSOURCE_IC7
  *         @arg @ref LL_RCC_ADC_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_ADC_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_ADC_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_ADC_CLKSOURCE_I2S_CKIN
  *         @arg @ref LL_RCC_ADC_CLKSOURCE_TIMG
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetADCClockSource(uint32_t ClkSource)
{
  MODIFY_REG(RCC->CCIPR1, RCC_CCIPR1_ADC12SEL, ClkSource);
}

/**
  * @brief  Configure ADFx Kernel clock source
  * @rmtoll CCIPR1         ADF1SEL        LL_RCC_SetADFClockSource
  * @param  ClkSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_ADF1_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_ADF1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_ADF1_CLKSOURCE_IC7
  *         @arg @ref LL_RCC_ADF1_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_ADF1_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_ADF1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_ADF1_CLKSOURCE_I2S_CKIN
  *         @arg @ref LL_RCC_ADF1_CLKSOURCE_TIMG
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetADFClockSource(uint32_t ClkSource)
{
  MODIFY_REG(RCC->CCIPR1, RCC_CCIPR1_ADF1SEL, ClkSource);
}

/**
  * @brief  Configure CLKP Kernel clock source
  * @rmtoll CCIPR7         PERSEL         LL_RCC_SetCLKPClockSource
  * @param  ClkSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_CLKP_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_CLKP_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_CLKP_CLKSOURCE_HSE
  *         @arg @ref LL_RCC_CLKP_CLKSOURCE_IC19
  *         @arg @ref LL_RCC_CLKP_CLKSOURCE_IC5
  *         @arg @ref LL_RCC_CLKP_CLKSOURCE_IC10
  *         @arg @ref LL_RCC_CLKP_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_CLKP_CLKSOURCE_IC20
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetCLKPClockSource(uint32_t ClkSource)
{
  MODIFY_REG(RCC->CCIPR7, RCC_CCIPR7_PERSEL, ClkSource);
}

/**
  * @brief  Configure DCMIPP Kernel clock source
  * @rmtoll CCIPR1         DCMIPPSEL        LL_RCC_SetDCMIPPClockSource
  * @param  ClkSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_DCMIPP_CLKSOURCE_PCLK5
  *         @arg @ref LL_RCC_DCMIPP_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_DCMIPP_CLKSOURCE_IC17
  *         @arg @ref LL_RCC_DCMIPP_CLKSOURCE_HSI
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetDCMIPPClockSource(uint32_t ClkSource)
{
  MODIFY_REG(RCC->CCIPR1, RCC_CCIPR1_DCMIPPSEL, ClkSource);
}

/**
  * @brief  Configure ETHx Kernel clock source
  * @rmtoll CCIPR2        ETH1CLKSEL     LL_RCC_SetETHClockSource
  * @param  ClkSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_ETH1_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_ETH1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_ETH1_CLKSOURCE_IC12
  *         @arg @ref LL_RCC_ETH1_CLKSOURCE_HSE
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetETHClockSource(uint32_t ClkSource)
{
  MODIFY_REG(RCC->CCIPR2, RCC_CCIPR2_ETH1CLKSEL, ClkSource);
}

/**
  * @brief  Configure ETHx PTP Kernel clock source
  * @rmtoll CCIPR2        ETH1PTPSEL     LL_RCC_SetETHPTPClockSource
  * @param  ClkSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_ETH1PTP_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_ETH1PTP_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_ETH1PTP_CLKSOURCE_IC13
  *         @arg @ref LL_RCC_ETH1PTP_CLKSOURCE_HSE
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetETHPTPClockSource(uint32_t ClkSource)
{
  MODIFY_REG(RCC->CCIPR2, RCC_CCIPR2_ETH1PTPSEL, ClkSource);
}

/**
  * @brief  Configure ETHx PHY interface
  * @rmtoll CCIPR2        ETH1SEL        LL_RCC_SetETHPHYInterface
  * @param  Interface This parameter can be one of the following values:
  *         @arg @ref LL_RCC_ETH1PHY_IF_MII
  *         @arg @ref LL_RCC_ETH1PHY_IF_RGMII
  *         @arg @ref LL_RCC_ETH1PHY_IF_RMII
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetETHPHYInterface(uint32_t Interface)
{
  MODIFY_REG(RCC->CCIPR2, RCC_CCIPR2_ETH1SEL, Interface);
}

/**
  * @brief  Set ETH1 PTP Kernel clock divider
  * @rmtoll CCIPR2       ETH1PTPDIV    LL_RCC_SetETH1PTPDivider
  * @param  Divider This parameter can be one of the following values:
  *         @arg @ref LL_RCC_ETH1PTP_DIV_1
  *         @arg @ref LL_RCC_ETH1PTP_DIV_2
  *         @arg @ref LL_RCC_ETH1PTP_DIV_3
  *         @arg @ref LL_RCC_ETH1PTP_DIV_4
  *         @arg @ref LL_RCC_ETH1PTP_DIV_5
  *         @arg @ref LL_RCC_ETH1PTP_DIV_6
  *         @arg @ref LL_RCC_ETH1PTP_DIV_7
  *         @arg @ref LL_RCC_ETH1PTP_DIV_8
  *         @arg @ref LL_RCC_ETH1PTP_DIV_9
  *         @arg @ref LL_RCC_ETH1PTP_DIV_10
  *         @arg @ref LL_RCC_ETH1PTP_DIV_11
  *         @arg @ref LL_RCC_ETH1PTP_DIV_12
  *         @arg @ref LL_RCC_ETH1PTP_DIV_13
  *         @arg @ref LL_RCC_ETH1PTP_DIV_14
  *         @arg @ref LL_RCC_ETH1PTP_DIV_15
  *         @arg @ref LL_RCC_ETH1PTP_DIV_16
  * @retval None.
  */
__STATIC_INLINE void LL_RCC_SetETH1PTPDivider(uint32_t Divider)
{
  MODIFY_REG(RCC->CCIPR2, RCC_CCIPR2_ETH1PTPDIV, Divider);
}

/**
  * @brief  Get ETH1 PTP Kernel clock divider
  * @rmtoll CCIPR2       ETH1PTPDIV    LL_RCC_GetETH1PTPDivider
  * @retval can be one of the following values:
  *         @arg @ref LL_RCC_ETH1PTP_DIV_1
  *         @arg @ref LL_RCC_ETH1PTP_DIV_2
  *         @arg @ref LL_RCC_ETH1PTP_DIV_3
  *         @arg @ref LL_RCC_ETH1PTP_DIV_4
  *         @arg @ref LL_RCC_ETH1PTP_DIV_5
  *         @arg @ref LL_RCC_ETH1PTP_DIV_6
  *         @arg @ref LL_RCC_ETH1PTP_DIV_7
  *         @arg @ref LL_RCC_ETH1PTP_DIV_8
  *         @arg @ref LL_RCC_ETH1PTP_DIV_9
  *         @arg @ref LL_RCC_ETH1PTP_DIV_10
  *         @arg @ref LL_RCC_ETH1PTP_DIV_11
  *         @arg @ref LL_RCC_ETH1PTP_DIV_12
  *         @arg @ref LL_RCC_ETH1PTP_DIV_13
  *         @arg @ref LL_RCC_ETH1PTP_DIV_14
  *         @arg @ref LL_RCC_ETH1PTP_DIV_15
  *         @arg @ref LL_RCC_ETH1PTP_DIV_16
  */
__STATIC_INLINE uint32_t LL_RCC_GetETH1PTPDivider(void)
{
  return (READ_BIT(RCC->CCIPR2, RCC_CCIPR2_ETH1PTPDIV));
}

/**
  * @brief  Configure ETHx Reference RX Kernel clock source
  * @rmtoll CCIPR2        ETH1REFCLKSEL  LL_RCC_SetETHREFRXClockSource
  * @param  ClkSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_ETH1REFRX_CLKSOURCE_EXT
  *         @arg @ref LL_RCC_ETH1REFRX_CLKSOURCE_INT
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetETHREFRXClockSource(uint32_t ClkSource)
{
  MODIFY_REG(RCC->CCIPR2, RCC_CCIPR2_ETH1REFCLKSEL, ClkSource);
}

/**
  * @brief  Configure ETHx Reference TX Kernel clock source
  * @rmtoll CCIPR2        ETH1GTXCLKSEL  LL_RCC_SetETHREFTXClockSource
  * @param  ClkSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_ETH1REFTX_CLKSOURCE_EXT
  *         @arg @ref LL_RCC_ETH1REFTX_CLKSOURCE_INT
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetETHREFTXClockSource(uint32_t ClkSource)
{
  MODIFY_REG(RCC->CCIPR2, RCC_CCIPR2_ETH1GTXCLKSEL, ClkSource);
}

/**
  * @brief  Configure FDCANx Kernel clock source
  * @rmtoll CCIPR3        FDCANSEL        LL_RCC_SetFDCANClockSource
  * @param  ClkSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_FDCAN_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_FDCAN_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_FDCAN_CLKSOURCE_IC19
  *         @arg @ref LL_RCC_FDCAN_CLKSOURCE_HSE
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetFDCANClockSource(uint32_t ClkSource)
{
  MODIFY_REG(RCC->CCIPR3, RCC_CCIPR3_FDCANSEL, ClkSource);
}

/**
  * @brief  Configure FMCx Kernel clock source
  * @rmtoll CCIPR3       FMCSEL        LL_RCC_SetFMCClockSource
  * @param  ClkSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_FMC_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_FMC_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_FMC_CLKSOURCE_IC3
  *         @arg @ref LL_RCC_FMC_CLKSOURCE_IC4
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetFMCClockSource(uint32_t ClkSource)
{
  MODIFY_REG(RCC->CCIPR3, RCC_CCIPR3_FMCSEL, ClkSource);
}

/**
  * @brief  Configure I2Cx clock source
  * @rmtoll CCIPR4       I2C1SEL       LL_RCC_SetI2CClockSource\n
  * @rmtoll CCIPR4       I2C2SEL       LL_RCC_SetI2CClockSource\n
  * @rmtoll CCIPR4       I2C3SEL       LL_RCC_SetI2CClockSource\n
  * @rmtoll CCIPR4       I2C4SEL       LL_RCC_SetI2CClockSource
  * @param  ClkSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_IC10
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_IC10
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_IC10
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_I2C4_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I2C4_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_I2C4_CLKSOURCE_IC10
  *         @arg @ref LL_RCC_I2C4_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_I2C4_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_I2C4_CLKSOURCE_HSI
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetI2CClockSource(uint32_t ClkSource)
{
  LL_RCC_SetClockSource(ClkSource);
}

/**
  * @brief  Configure I3Cx clock source
  * @rmtoll CCIPR4       I3C1SEL       LL_RCC_SetI3CClockSource\n
  * @rmtoll CCIPR4       I3C2SEL       LL_RCC_SetI3CClockSource
  * @param  ClkSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_I3C1_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I3C1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_I3C1_CLKSOURCE_IC10
  *         @arg @ref LL_RCC_I3C1_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_I3C1_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_I3C1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_I3C2_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I3C2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_I3C2_CLKSOURCE_IC10
  *         @arg @ref LL_RCC_I3C2_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_I3C2_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_I3C2_CLKSOURCE_HSI
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetI3CClockSource(uint32_t ClkSource)
{
  LL_RCC_SetClockSource(ClkSource);
}

/**
  * @brief  Configure LPTIMx clock source
  * @rmtoll CCIPR12      LPTIM1SEL     LL_RCC_SetLPTIMClockSource\n
  *         CCIPR12      LPTIM2SEL     LL_RCC_SetLPTIMClockSource\n
  *         CCIPR12      LPTIM3SEL     LL_RCC_SetLPTIMClockSource\n
  *         CCIPR12      LPTIM4SEL     LL_RCC_SetLPTIMClockSource\n
  *         CCIPR12      LPTIM5SEL     LL_RCC_SetLPTIMClockSource
  * @param  ClkSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_TIMG
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_PCLK4
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_TIMG
  *         @arg @ref LL_RCC_LPTIM3_CLKSOURCE_PCLK4
  *         @arg @ref LL_RCC_LPTIM3_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_LPTIM3_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_LPTIM3_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_LPTIM3_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_LPTIM3_CLKSOURCE_TIMG
  *         @arg @ref LL_RCC_LPTIM4_CLKSOURCE_PCLK4
  *         @arg @ref LL_RCC_LPTIM4_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_LPTIM4_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_LPTIM4_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_LPTIM4_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_LPTIM4_CLKSOURCE_TIMG
  *         @arg @ref LL_RCC_LPTIM5_CLKSOURCE_PCLK4
  *         @arg @ref LL_RCC_LPTIM5_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_LPTIM5_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_LPTIM5_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_LPTIM5_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_LPTIM5_CLKSOURCE_TIMG
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetLPTIMClockSource(uint32_t ClkSource)
{
  LL_RCC_SetClockSource(ClkSource);
}

/**
  * @brief  Configure LPUARTx clock source
  * @rmtoll CCIPR14        LPUART1SEL     LL_RCC_SetLPUARTClockSource
  * @param  ClkSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_LPUART1_CLKSOURCE_PCLK4
  *         @arg @ref LL_RCC_LPUART1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_LPUART1_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_LPUART1_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_LPUART1_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_LPUART1_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_LPUART1_CLKSOURCE_HSI
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetLPUARTClockSource(uint32_t ClkSource)
{
  MODIFY_REG(RCC->CCIPR14, RCC_CCIPR14_LPUART1SEL, ClkSource);
}

/**
  * @brief  Configure LTDC clock source
  * @rmtoll CCIPR4         LTDCSEL     LL_RCC_SetLTDCClockSource
  * @param  ClkSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_LTDC_CLKSOURCE_PCLK5
  *         @arg @ref LL_RCC_LTDC_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_LTDC_CLKSOURCE_IC16
  *         @arg @ref LL_RCC_LTDC_CLKSOURCE_HSI
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetLTDCClockSource(uint32_t ClkSource)
{
  MODIFY_REG(RCC->CCIPR4, RCC_CCIPR4_LTDCSEL, ClkSource);
}

/**
  * @brief  Configure MDFx Kernel clock source
  * @rmtoll CCIPR5         MDF1SEL        LL_RCC_SetMDFClockSource
  * @param  ClkSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_MDF1_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_MDF1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_MDF1_CLKSOURCE_IC7
  *         @arg @ref LL_RCC_MDF1_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_MDF1_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_MDF1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_MDF1_CLKSOURCE_I2S_CKIN
  *         @arg @ref LL_RCC_MDF1_CLKSOURCE_TIMG
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetMDFClockSource(uint32_t ClkSource)
{
  MODIFY_REG(RCC->CCIPR5, RCC_CCIPR5_MDF1SEL, ClkSource);
}

/**
  * @brief  Configure OTGPHY clock source
  * @rmtoll CCIPR6       OTGPHY1SEL        LL_RCC_SetOTGPHYClockSource\n
  *         CCIPR6       OTGPHY2SEL        LL_RCC_SetOTGPHYClockSource
  * @param  ClkSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_OTGPHY1_CLKSOURCE_HSE_DIV_2
  *         @arg @ref LL_RCC_OTGPHY1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_OTGPHY1_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_OTGPHY1_CLKSOURCE_HSE_DIV_2_OSC
  *         @arg @ref LL_RCC_OTGPHY2_CLKSOURCE_HSE_DIV_2
  *         @arg @ref LL_RCC_OTGPHY2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_OTGPHY2_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_OTGPHY2_CLKSOURCE_HSE_DIV_2_OSC
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetOTGPHYClockSource(uint32_t ClkSource)
{
  LL_RCC_SetClockSource(ClkSource);
}

/**
  * @brief  Configure OTGPHYCKREF clock source
  * @rmtoll CCIPR6       OTGPHY1CKREFSEL   LL_RCC_SetOTGPHYCKREFClockSource\n
  *         CCIPR6       OTGPHY2CKREFSEL   LL_RCC_SetOTGPHYCKREFClockSource
  * @param  ClkSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_OTGPHY1CKREF_CLKSOURCE_OTGPHY1
  *         @arg @ref LL_RCC_OTGPHY1CKREF_CLKSOURCE_HSE_DIV_2_OSC
  *         @arg @ref LL_RCC_OTGPHY2CKREF_CLKSOURCE_OTGPHY2
  *         @arg @ref LL_RCC_OTGPHY2CKREF_CLKSOURCE_HSE_DIV_2_OSC
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetOTGPHYCKREFClockSource(uint32_t ClkSource)
{
  LL_RCC_SetClockSource(ClkSource);
}

/**
  * @brief  Configure PSSI clock source
  * @rmtoll CCIPR7       PSSISEL        LL_RCC_SetPSSIClockSource
  * @param  ClkSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PSSI_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_PSSI_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_PSSI_CLKSOURCE_IC20
  *         @arg @ref LL_RCC_PSSI_CLKSOURCE_HSI
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetPSSIClockSource(uint32_t ClkSource)
{
  MODIFY_REG(RCC->CCIPR7, RCC_CCIPR7_PSSISEL, ClkSource);
}

/**
  * @brief  Configure SAIx clock source
  * @rmtoll CCIPR7       SAI1SEL      LL_RCC_SetSAIClockSource\n
  *         CCIPR7       SAI2SEL      LL_RCC_SetSAIClockSource
  * @param  ClkSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_IC7
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_I2S_CKIN
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_SPDIFRX1
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_IC7
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_I2S_CKIN
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_SPDIFRX1
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetSAIClockSource(uint32_t ClkSource)
{
  LL_RCC_SetClockSource(ClkSource);
}

/**
  * @brief  Configure SDMMCx clock source
  * @rmtoll CCIPR8       SDMMC1SEL      LL_RCC_SetSDMMCClockSource\n
  *         CCIPR8       SDMMC2SEL      LL_RCC_SetSDMMCClockSource
  * @param  ClkSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE_IC4
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE_IC5
  *         @arg @ref LL_RCC_SDMMC2_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_SDMMC2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SDMMC2_CLKSOURCE_IC4
  *         @arg @ref LL_RCC_SDMMC2_CLKSOURCE_IC5
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetSDMMCClockSource(uint32_t ClkSource)
{
  LL_RCC_SetClockSource(ClkSource);
}

/**
  * @brief  Configure SPDIFRX1 Kernel clock source
  * @rmtoll CCIPR9       SPDIFRX1SEL        LL_RCC_SetSPDIFRXClockSource
  * @param  ClkSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_SPDIFRX1_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_SPDIFRX1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SPDIFRX1_CLKSOURCE_IC7
  *         @arg @ref LL_RCC_SPDIFRX1_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_SPDIFRX1_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SPDIFRX1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SPDIFRX1_CLKSOURCE_I2S_CKIN
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetSPDIFRXClockSource(uint32_t ClkSource)
{
  MODIFY_REG(RCC->CCIPR9, RCC_CCIPR9_SPDIFRX1SEL, ClkSource);
}

/**
  * @brief  Configure SPIx Kernel clock source
  * @rmtoll CCIPR9       SPI1SEL        LL_RCC_SetSPIClockSource\n
  *         CCIPR9       SPI2SEL        LL_RCC_SetSPIClockSource\n
  *         CCIPR9       SPI3SEL        LL_RCC_SetSPIClockSource\n
  *         CCIPR9       SPI4SEL        LL_RCC_SetSPIClockSource\n
  *         CCIPR9       SPI5SEL        LL_RCC_SetSPIClockSource\n
  *         CCIPR9       SPI6SEL        LL_RCC_SetSPIClockSource
  * @param  ClkSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE_I2S_CKIN
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE_I2S_CKIN
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE_I2S_CKIN
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE_HSE
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE_HSE
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE_PCLK4
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE_I2S_CKIN
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetSPIClockSource(uint32_t ClkSource)
{
  LL_RCC_SetClockSource(ClkSource);
}

/**
  * @brief  Configure USARTx clock source
  * @rmtoll CCIPR13        USART1SEL      LL_RCC_SetUSARTClockSource\n
  *         CCIPR13        USART2SEL      LL_RCC_SetUSARTClockSource\n
  *         CCIPR13        USART3SEL      LL_RCC_SetUSARTClockSource\n
  *         CCIPR13        USART6SEL      LL_RCC_SetUSARTClockSource\n
  *         CCIPR14        USART10SEL     LL_RCC_SetUSARTClockSource
  * @param  ClkSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_USART6_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_USART6_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_USART6_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_USART6_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_USART6_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_USART6_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_USART6_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_USART10_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_USART10_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_USART10_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_USART10_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_USART10_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_USART10_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_USART10_CLKSOURCE_HSI
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetUSARTClockSource(uint32_t ClkSource)
{
  LL_RCC_SetClockSource(ClkSource);
}

/**
  * @brief  Configure UARTx clock source
  * @rmtoll CCIPR13      UART4SEL      LL_RCC_SetUARTClockSource\n
  *         CCIPR13      UART5SEL      LL_RCC_SetUARTClockSource\n
  *         CCIPR13      UART7SEL      LL_RCC_SetUARTClockSource\n
  *         CCIPR13      UART8SEL      LL_RCC_SetUARTClockSource\n
  *         CCIPR14      UART9SEL      LL_RCC_SetUARTClockSource
  * @param  ClkSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_UART7_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_UART7_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_UART7_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_UART7_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_UART7_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_UART7_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_UART7_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_UART8_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_UART8_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_UART8_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_UART8_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_UART8_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_UART8_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_UART8_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_UART9_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_UART9_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_UART9_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_UART9_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_UART9_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_UART9_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_UART9_CLKSOURCE_HSI
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetUARTClockSource(uint32_t ClkSource)
{
  LL_RCC_SetClockSource(ClkSource);
}

/**
  * @brief  Configure USBx clock source
  * @rmtoll CCIPR6      OTGPHY1SEL        LL_RCC_GetUSBClockSource\n
  *         CCIPR6      OTGPHY1CKREFSEL   LL_RCC_GetUSBClockSource\n
  *         CCIPR6      OTGPHY2SEL        LL_RCC_GetUSBClockSource\n
  *         CCIPR6      OTGPHY2CKREFSEL   LL_RCC_GetUSBClockSource\n
  * @param  ClkSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_OTGPHY1_CLKSOURCE_HSE_DIV_2
  *         @arg @ref LL_RCC_OTGPHY1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_OTGPHY1_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_OTGPHY1_CLKSOURCE_HSE_DIV_2_OSC
  *         @arg @ref LL_RCC_OTGPHY1CKREF_CLKSOURCE_OTGPHY1
  *         @arg @ref LL_RCC_OTGPHY1CKREF_CLKSOURCE_HSE_DIV_2_OSC
  *         @arg @ref LL_RCC_OTGPHY2_CLKSOURCE_HSE_DIV_2
  *         @arg @ref LL_RCC_OTGPHY2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_OTGPHY2_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_OTGPHY2_CLKSOURCE_HSE_DIV_2_OSC
  *         @arg @ref LL_RCC_OTGPHY2CKREF_CLKSOURCE_OTGPHY2
  *         @arg @ref LL_RCC_OTGPHY2CKREF_CLKSOURCE_HSE_DIV_2_OSC
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetUSBClockSource(uint32_t ClkSource)
{
  LL_RCC_SetClockSource(ClkSource);
}

/**
  * @brief  Configure XSPIx Kernel clock source
  * @rmtoll CCIPR6         XSPI1SEL        LL_RCC_SetXSPIClockSource\n
  *         CCIPR6         XSPI2SEL        LL_RCC_SetXSPIClockSource\n
  *         CCIPR6         XSPI3SEL        LL_RCC_SetXSPIClockSource
  * @param  ClkSource This parameter can be one of the following values:
  *         @arg @ref LL_RCC_XSPI1_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_XSPI1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_XSPI1_CLKSOURCE_IC3
  *         @arg @ref LL_RCC_XSPI1_CLKSOURCE_IC4
  *         @arg @ref LL_RCC_XSPI2_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_XSPI2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_XSPI2_CLKSOURCE_IC3
  *         @arg @ref LL_RCC_XSPI2_CLKSOURCE_IC4
  *         @arg @ref LL_RCC_XSPI3_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_XSPI3_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_XSPI3_CLKSOURCE_IC3
  *         @arg @ref LL_RCC_XSPI3_CLKSOURCE_IC4
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetXSPIClockSource(uint32_t ClkSource)
{
  LL_RCC_SetClockSource(ClkSource);
}

/**
  * @brief  Get periph clock source
  * @rmtoll CCIPR1      *     LL_RCC_GetClockSource\n
  *         CCIPR3      *     LL_RCC_GetClockSource\n
  *         CCIPR4      *     LL_RCC_GetClockSource\n
  *         CCIPR6      *     LL_RCC_GetClockSource\n
  *         CCIPR7      *     LL_RCC_GetClockSource\n
  *         CCIPR8      *     LL_RCC_GetClockSource\n
  *         CCIPR9      *     LL_RCC_GetClockSource\n
  *         CCIPR12     *     LL_RCC_GetClockSource\n
  *         CCIPR13     *     LL_RCC_GetClockSource\n
  *         CCIPR14     *     LL_RCC_GetClockSource
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE
  *         @arg @ref LL_RCC_I2C4_CLKSOURCE
  *         @arg @ref LL_RCC_I3C1_CLKSOURCE
  *         @arg @ref LL_RCC_I3C2_CLKSOURCE
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE
  *         @arg @ref LL_RCC_LPTIM3_CLKSOURCE
  *         @arg @ref LL_RCC_LPTIM4_CLKSOURCE
  *         @arg @ref LL_RCC_LPTIM5_CLKSOURCE
  *         @arg @ref LL_RCC_OTGPHY1_CLKSOURCE
  *         @arg @ref LL_RCC_OTGPHY1CKREF_CLKSOURCE
  *         @arg @ref LL_RCC_OTGPHY2_CLKSOURCE
  *         @arg @ref LL_RCC_OTGPHY2CKREF_CLKSOURCE
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE
  *         @arg @ref LL_RCC_SDMMC2_CLKSOURCE
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE
  *         @arg @ref LL_RCC_UART4_CLKSOURCE
  *         @arg @ref LL_RCC_UART5_CLKSOURCE
  *         @arg @ref LL_RCC_UART7_CLKSOURCE
  *         @arg @ref LL_RCC_UART8_CLKSOURCE
  *         @arg @ref LL_RCC_UART9_CLKSOURCE
  *         @arg @ref LL_RCC_USART1_CLKSOURCE
  *         @arg @ref LL_RCC_USART2_CLKSOURCE
  *         @arg @ref LL_RCC_USART3_CLKSOURCE
  *         @arg @ref LL_RCC_USART6_CLKSOURCE
  *         @arg @ref LL_RCC_USART10_CLKSOURCE
  *         @arg @ref LL_RCC_XSPI1_CLKSOURCE
  *         @arg @ref LL_RCC_XSPI2_CLKSOURCE
  *         @arg @ref LL_RCC_XSPI3_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_IC10
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_IC10
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_IC10
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_I2C4_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I2C4_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_I2C4_CLKSOURCE_IC10
  *         @arg @ref LL_RCC_I2C4_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_I2C4_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_I2C4_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_I3C1_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I3C1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_I3C1_CLKSOURCE_IC10
  *         @arg @ref LL_RCC_I3C1_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_I3C1_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_I3C1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_I3C2_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I3C2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_I3C2_CLKSOURCE_IC10
  *         @arg @ref LL_RCC_I3C2_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_I3C2_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_I3C2_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_TIMG
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_PCLK4
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_TIMG
  *         @arg @ref LL_RCC_LPTIM3_CLKSOURCE_PCLK4
  *         @arg @ref LL_RCC_LPTIM3_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_LPTIM3_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_LPTIM3_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_LPTIM3_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_LPTIM3_CLKSOURCE_TIMG
  *         @arg @ref LL_RCC_LPTIM4_CLKSOURCE_PCLK4
  *         @arg @ref LL_RCC_LPTIM4_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_LPTIM4_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_LPTIM4_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_LPTIM4_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_LPTIM4_CLKSOURCE_TIMG
  *         @arg @ref LL_RCC_LPTIM5_CLKSOURCE_PCLK4
  *         @arg @ref LL_RCC_LPTIM5_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_LPTIM5_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_LPTIM5_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_LPTIM5_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_LPTIM5_CLKSOURCE_TIMG
  *         @arg @ref LL_RCC_OTGPHY1_CLKSOURCE_HSE_DIV_2
  *         @arg @ref LL_RCC_OTGPHY1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_OTGPHY1_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_OTGPHY1_CLKSOURCE_HSE_DIV_2_OSC
  *         @arg @ref LL_RCC_OTGPHY1CKREF_CLKSOURCE_OTGPHY1
  *         @arg @ref LL_RCC_OTGPHY1CKREF_CLKSOURCE_HSE_DIV_2_OSC
  *         @arg @ref LL_RCC_OTGPHY2_CLKSOURCE_HSE_DIV_2
  *         @arg @ref LL_RCC_OTGPHY2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_OTGPHY2_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_OTGPHY2_CLKSOURCE_HSE_DIV_2_OSC
  *         @arg @ref LL_RCC_OTGPHY2CKREF_CLKSOURCE_OTGPHY2
  *         @arg @ref LL_RCC_OTGPHY2CKREF_CLKSOURCE_HSE_DIV_2_OSC
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_IC7
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_I2S_CKIN
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_SPDIFRX1
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_IC7
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_I2S_CKIN
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_SPDIFRX1
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE_IC4
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE_IC5
  *         @arg @ref LL_RCC_SDMMC2_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_SDMMC2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SDMMC2_CLKSOURCE_IC4
  *         @arg @ref LL_RCC_SDMMC2_CLKSOURCE_IC5
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE_I2S_CKIN
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE_I2S_CKIN
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE_I2S_CKIN
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE_HSE
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE_HSE
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE_PCLK4
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE_I2S_CKIN
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_UART7_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_UART7_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_UART7_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_UART7_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_UART7_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_UART7_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_UART7_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_UART8_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_UART8_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_UART8_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_UART8_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_UART8_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_UART8_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_UART8_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_UART9_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_UART9_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_UART9_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_UART9_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_UART9_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_UART9_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_UART9_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_USART6_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_USART6_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_USART6_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_USART6_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_USART6_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_USART6_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_USART6_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_USART10_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_USART10_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_USART10_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_USART10_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_USART10_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_USART10_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_USART10_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_XSPI1_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_XSPI1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_XSPI1_CLKSOURCE_IC3
  *         @arg @ref LL_RCC_XSPI1_CLKSOURCE_IC4
  *         @arg @ref LL_RCC_XSPI2_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_XSPI2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_XSPI2_CLKSOURCE_IC3
  *         @arg @ref LL_RCC_XSPI2_CLKSOURCE_IC4
  *         @arg @ref LL_RCC_XSPI3_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_XSPI3_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_XSPI3_CLKSOURCE_IC3
  *         @arg @ref LL_RCC_XSPI3_CLKSOURCE_IC4
  * @retval None
  */
__STATIC_INLINE uint32_t LL_RCC_GetClockSource(uint32_t Periph)
{
  const volatile uint32_t *pReg = (uint32_t *)((uint32_t)((uint32_t)(&RCC->CCIPR1) + LL_CLKSOURCE_REG(Periph)));
  return (uint32_t)(Periph | (((READ_BIT(*pReg, LL_CLKSOURCE_MASK(Periph))) >> LL_CLKSOURCE_SHIFT(Periph)) << LL_RCC_CONFIG_SHIFT));
}

/**
  * @brief  Get ADC Kernel clock source
  * @rmtoll CCIPR1       ADC12SEL       LL_RCC_GetADCClockSource
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_ADC_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_ADC_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_ADC_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_ADC_CLKSOURCE_IC7
  *         @arg @ref LL_RCC_ADC_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_ADC_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_ADC_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_ADC_CLKSOURCE_I2S_CKIN
  *         @arg @ref LL_RCC_ADC_CLKSOURCE_TIMG
  */
__STATIC_INLINE uint32_t LL_RCC_GetADCClockSource(uint32_t Periph)
{
  UNUSED(Periph);
  return (uint32_t)(READ_BIT(RCC->CCIPR1, RCC_CCIPR1_ADC12SEL));
}

/**
  * @brief  Get ADFx clock source
  * @rmtoll CCIPR1       ADF1SEL        LL_RCC_GetADFClockSource
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_ADF1_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_ADF1_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_ADF1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_ADF1_CLKSOURCE_IC7
  *         @arg @ref LL_RCC_ADF1_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_ADF1_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_ADF1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_ADF1_CLKSOURCE_I2S_CKIN
  *         @arg @ref LL_RCC_ADF1_CLKSOURCE_TIMG
  */
__STATIC_INLINE uint32_t LL_RCC_GetADFClockSource(uint32_t Periph)
{
  UNUSED(Periph);
  return (uint32_t)(READ_BIT(RCC->CCIPR1, RCC_CCIPR1_ADF1SEL));
}

/**
  * @brief  Get CLKP Kernel clock source
  * @rmtoll CCIPR7         PERSEL         LL_RCC_GetCLKPClockSource
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_CLKP_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_CLKP_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_CLKP_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_CLKP_CLKSOURCE_HSE
  *         @arg @ref LL_RCC_CLKP_CLKSOURCE_IC19
  *         @arg @ref LL_RCC_CLKP_CLKSOURCE_IC5
  *         @arg @ref LL_RCC_CLKP_CLKSOURCE_IC10
  *         @arg @ref LL_RCC_CLKP_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_CLKP_CLKSOURCE_IC20
  */
__STATIC_INLINE uint32_t LL_RCC_GetCLKPClockSource(uint32_t Periph)
{
  UNUSED(Periph);
  return (uint32_t)(READ_BIT(RCC->CCIPR7, RCC_CCIPR7_PERSEL));
}

/**
  * @brief  Get DCMIPP clock source
  * @rmtoll CCIPR1       DCMIPPSEL        LL_RCC_GetDCMIPPClockSource
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_DCMIPP_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_DCMIPP_CLKSOURCE_PCLK5
  *         @arg @ref LL_RCC_DCMIPP_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_DCMIPP_CLKSOURCE_IC17
  *         @arg @ref LL_RCC_DCMIPP_CLKSOURCE_HSI
  */
__STATIC_INLINE uint32_t LL_RCC_GetDCMIPPClockSource(uint32_t Periph)
{
  UNUSED(Periph);
  return (uint32_t)(READ_BIT(RCC->CCIPR1, RCC_CCIPR1_DCMIPPSEL));
}

/**
  * @brief  Get ETHx Kernel clock source
  * @rmtoll CCIPR2        ETH1CLKSEL     LL_RCC_GetETHClockSource
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_ETH1_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_ETH1_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_ETH1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_ETH1_CLKSOURCE_IC12
  *         @arg @ref LL_RCC_ETH1_CLKSOURCE_HSE
  */
__STATIC_INLINE uint32_t LL_RCC_GetETHClockSource(uint32_t Periph)
{
  UNUSED(Periph);
  return (uint32_t)(READ_BIT(RCC->CCIPR2, RCC_CCIPR2_ETH1CLKSEL));
}

/**
  * @brief  Get ETHx PTP Kernel clock source
  * @rmtoll CCIPR2        ETH1PTPSEL     LL_RCC_GetETHPTPClockSource
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_ETH1PTP_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_ETH1PTP_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_ETH1PTP_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_ETH1PTP_CLKSOURCE_IC13
  *         @arg @ref LL_RCC_ETH1PTP_CLKSOURCE_HSE
  */
__STATIC_INLINE uint32_t LL_RCC_GetETHPTPClockSource(uint32_t Periph)
{
  UNUSED(Periph);
  return (uint32_t)(READ_BIT(RCC->CCIPR2, RCC_CCIPR2_ETH1PTPSEL));
}

/**
  * @brief  Get ETHx PHY interface
  * @rmtoll CCIPR2        ETH1SEL        LL_RCC_GetETHPHYInterface
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_ETH1PHY_IF
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_ETH1PHY_IF_MII
  *         @arg @ref LL_RCC_ETH1PHY_IF_RGMII
  *         @arg @ref LL_RCC_ETH1PHY_IF_RMII
  */
__STATIC_INLINE uint32_t LL_RCC_GetETHPHYInterface(uint32_t Periph)
{
  UNUSED(Periph);
  return (uint32_t)(READ_BIT(RCC->CCIPR2, RCC_CCIPR2_ETH1SEL));
}

/**
  * @brief  Get ETHx Reference RX Kernel clock source
  * @rmtoll CCIPR2        ETH1REFCLKSEL  LL_RCC_GetETHREFRXClockSource
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_ETH1REFRX_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_ETH1REFRX_CLKSOURCE_EXT
  *         @arg @ref LL_RCC_ETH1REFRX_CLKSOURCE_INT
  */
__STATIC_INLINE uint32_t LL_RCC_GetETHREFRXClockSource(uint32_t Periph)
{
  UNUSED(Periph);
  return (uint32_t)(READ_BIT(RCC->CCIPR2, RCC_CCIPR2_ETH1REFCLKSEL));
}

/**
  * @brief  Get ETHx Reference TX Kernel clock source
  * @rmtoll CCIPR2        ETH1GTXCLKSEL  LL_RCC_GetETHREFTXClockSource
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_ETH1REFTX_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_ETH1REFTX_CLKSOURCE_EXT
  *         @arg @ref LL_RCC_ETH1REFTX_CLKSOURCE_INT
  */
__STATIC_INLINE uint32_t LL_RCC_GetETHREFTXClockSource(uint32_t Periph)
{
  UNUSED(Periph);
  return (uint32_t)(READ_BIT(RCC->CCIPR2, RCC_CCIPR2_ETH1GTXCLKSEL));
}

/**
  * @brief  Get FDCAN Kernel clock source
  * @rmtoll CCIPR3       FDCANSEL        LL_RCC_GetFDCANClockSource
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_FDCAN_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_FDCAN_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_FDCAN_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_FDCAN_CLKSOURCE_IC19
  *         @arg @ref LL_RCC_FDCAN_CLKSOURCE_HSE
  */
__STATIC_INLINE uint32_t LL_RCC_GetFDCANClockSource(uint32_t Periph)
{
  UNUSED(Periph);
  return (uint32_t)(READ_BIT(RCC->CCIPR3, RCC_CCIPR3_FDCANSEL));
}

/**
  * @brief  Get FMC Kernel clock source
  * @rmtoll CCIPR3         FMCSEL        LL_RCC_GetFMCClockSource
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_FMC_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_FMC_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_FMC_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_FMC_CLKSOURCE_IC3
  *         @arg @ref LL_RCC_FMC_CLKSOURCE_IC4
  */
__STATIC_INLINE uint32_t LL_RCC_GetFMCClockSource(uint32_t Periph)
{
  UNUSED(Periph);
  return (uint32_t)(READ_BIT(RCC->CCIPR3, RCC_CCIPR3_FMCSEL));
}

/**
  * @brief  Get I2Cx clock source
  * @rmtoll CCIPR4       I2C1SEL       LL_RCC_GetI2CClockSource\n
  * @rmtoll CCIPR4       I2C2SEL       LL_RCC_GetI2CClockSource\n
  * @rmtoll CCIPR4       I2C3SEL       LL_RCC_GetI2CClockSource\n
  * @rmtoll CCIPR4       I2C4SEL       LL_RCC_GetI2CClockSource
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE
  *         @arg @ref LL_RCC_I2C4_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_IC10
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_I2C1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_IC10
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_I2C2_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_IC10
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_I2C3_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_I2C4_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I2C4_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_I2C4_CLKSOURCE_IC10
  *         @arg @ref LL_RCC_I2C4_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_I2C4_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_I2C4_CLKSOURCE_HSI
  */
__STATIC_INLINE uint32_t LL_RCC_GetI2CClockSource(uint32_t Periph)
{
  return LL_RCC_GetClockSource(Periph);
}

/**
  * @brief  Get I3Cx clock source
  * @rmtoll CCIPR4       I3C1SEL       LL_RCC_GetI3CClockSource\n
  * @rmtoll CCIPR4       I3C2SEL       LL_RCC_GetI3CClockSource
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_I3C1_CLKSOURCE
  *         @arg @ref LL_RCC_I3C2_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_I3C1_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I3C1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_I3C1_CLKSOURCE_IC10
  *         @arg @ref LL_RCC_I3C1_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_I3C1_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_I3C1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_I3C2_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_I3C2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_I3C2_CLKSOURCE_IC10
  *         @arg @ref LL_RCC_I3C2_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_I3C2_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_I3C2_CLKSOURCE_HSI
  */
__STATIC_INLINE uint32_t LL_RCC_GetI3CClockSource(uint32_t Periph)
{
  UNUSED(Periph);
  return LL_RCC_GetClockSource(Periph);
}

/**
  * @brief  Get LPTIM clock source
  * @rmtoll CCIPR12      LPTIM1SEL     LL_RCC_GetLPTIMClockSource\n
  *         CCIPR12      LPTIM2SEL     LL_RCC_GetLPTIMClockSource\n
  *         CCIPR12      LPTIM3SEL     LL_RCC_GetLPTIMClockSource\n
  *         CCIPR12      LPTIM4SEL     LL_RCC_GetLPTIMClockSource\n
  *         CCIPR12      LPTIM5SEL     LL_RCC_GetLPTIMClockSource
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE
  *         @arg @ref LL_RCC_LPTIM3_CLKSOURCE
  *         @arg @ref LL_RCC_LPTIM4_CLKSOURCE
  *         @arg @ref LL_RCC_LPTIM5_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_LPTIM1_CLKSOURCE_TIMG
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_PCLK4
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_LPTIM2_CLKSOURCE_TIMG
  *         @arg @ref LL_RCC_LPTIM3_CLKSOURCE_PCLK4
  *         @arg @ref LL_RCC_LPTIM3_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_LPTIM3_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_LPTIM3_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_LPTIM3_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_LPTIM3_CLKSOURCE_TIMG
  *         @arg @ref LL_RCC_LPTIM4_CLKSOURCE_PCLK4
  *         @arg @ref LL_RCC_LPTIM4_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_LPTIM4_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_LPTIM4_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_LPTIM4_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_LPTIM4_CLKSOURCE_TIMG
  *         @arg @ref LL_RCC_LPTIM5_CLKSOURCE_PCLK4
  *         @arg @ref LL_RCC_LPTIM5_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_LPTIM5_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_LPTIM5_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_LPTIM5_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_LPTIM5_CLKSOURCE_TIMG
  * @retval None
  */
__STATIC_INLINE uint32_t LL_RCC_GetLPTIMClockSource(uint32_t Periph)
{
  return LL_RCC_GetClockSource(Periph);
}

/**
  * @brief  Get LPUART clock source
  * @rmtoll CCIPR14       LPUART1SEL     LL_RCC_GetLPUARTClockSource
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_LPUART1_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_LPUART1_CLKSOURCE_PCLK4
  *         @arg @ref LL_RCC_LPUART1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_LPUART1_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_LPUART1_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_LPUART1_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_LPUART1_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_LPUART1_CLKSOURCE_HSI
  */
__STATIC_INLINE uint32_t LL_RCC_GetLPUARTClockSource(uint32_t Periph)
{
  UNUSED(Periph);
  return (uint32_t)(READ_BIT(RCC->CCIPR14, RCC_CCIPR14_LPUART1SEL));
}

/**
  * @brief  Get LTDC clock source
  * @rmtoll CCIPR4        LTDCSEL     LL_RCC_GetLTDCClockSource
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_LTDC_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_LTDC_CLKSOURCE_PCLK5
  *         @arg @ref LL_RCC_LTDC_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_LTDC_CLKSOURCE_IC16
  *         @arg @ref LL_RCC_LTDC_CLKSOURCE_HSI
  */
__STATIC_INLINE uint32_t LL_RCC_GetLTDCClockSource(uint32_t Periph)
{
  UNUSED(Periph);
  return (uint32_t)(READ_BIT(RCC->CCIPR4, RCC_CCIPR4_LTDCSEL));
}

/**
  * @brief  Get MDFx clock source
  * @rmtoll CCIPR5       MDF1SEL        LL_RCC_GetMDFClockSource
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_MDF1_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_MDF1_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_MDF1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_MDF1_CLKSOURCE_IC7
  *         @arg @ref LL_RCC_MDF1_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_MDF1_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_MDF1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_MDF1_CLKSOURCE_I2S_CKIN
  *         @arg @ref LL_RCC_MDF1_CLKSOURCE_TIMG
  */
__STATIC_INLINE uint32_t LL_RCC_GetMDFClockSource(uint32_t Periph)
{
  UNUSED(Periph);
  return (uint32_t)(READ_BIT(RCC->CCIPR5, RCC_CCIPR5_MDF1SEL));
}

/**
  * @brief  Get OTGPHY clock source
  * @rmtoll CCIPR6       OTGPHY1SEL        LL_RCC_GetOTGPHYClockSource\n
  *         CCIPR6       OTGPHY2SEL        LL_RCC_GetOTGPHYClockSource
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_OTGPHY1_CLKSOURCE
  *         @arg @ref LL_RCC_OTGPHY2_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_OTGPHY1_CLKSOURCE_HSE_DIV_2
  *         @arg @ref LL_RCC_OTGPHY1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_OTGPHY1_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_OTGPHY1_CLKSOURCE_HSE_DIV_2_OSC
  *         @arg @ref LL_RCC_OTGPHY2_CLKSOURCE_HSE_DIV_2
  *         @arg @ref LL_RCC_OTGPHY2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_OTGPHY2_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_OTGPHY2_CLKSOURCE_HSE_DIV_2_OSC
  */
__STATIC_INLINE uint32_t LL_RCC_GetOTGPHYClockSource(uint32_t Periph)
{
  return LL_RCC_GetClockSource(Periph);
}

/**
  * @brief  Get OTGPHYCKREF clock source
  * @rmtoll CCIPR6       OTGPHY1CKREFSEL   LL_RCC_GetOTGPHYCKREFClockSource\n
  *         CCIPR6       OTGPHY2CKREFSEL   LL_RCC_GetOTGPHYCKREFClockSource
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_OTGPHY1CKREF_CLKSOURCE
  *         @arg @ref LL_RCC_OTGPHY2CKREF_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_OTGPHY1CKREF_CLKSOURCE_OTGPHY1
  *         @arg @ref LL_RCC_OTGPHY1CKREF_CLKSOURCE_HSE_DIV_2_OSC
  *         @arg @ref LL_RCC_OTGPHY2CKREF_CLKSOURCE_OTGPHY2
  *         @arg @ref LL_RCC_OTGPHY2CKREF_CLKSOURCE_HSE_DIV_2_OSC
  */
__STATIC_INLINE uint32_t LL_RCC_GetOTGPHYCKREFClockSource(uint32_t Periph)
{
  return LL_RCC_GetClockSource(Periph);
}

/**
  * @brief  Get PSSI clock source
  * @rmtoll CCIPR7      PSSISEL      LL_RCC_GetPSSIClockSource
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_PSSI_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_PSSI_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_PSSI_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_PSSI_CLKSOURCE_IC20
  *         @arg @ref LL_RCC_PSSI_CLKSOURCE_HSI
  */
__STATIC_INLINE uint32_t LL_RCC_GetPSSIClockSource(uint32_t Periph)
{
  UNUSED(Periph);
  return (uint32_t)(READ_BIT(RCC->CCIPR7, RCC_CCIPR7_PSSISEL));
}

/**
  * @brief  Get SAIx clock source
  * @rmtoll CCIPR7     SAI1SEL      LL_RCC_GetSAIClockSource\n
  *         CCIPR7     SAI2SEL      LL_RCC_GetSAIClockSource
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_IC7
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_I2S_CKIN
  *         @arg @ref LL_RCC_SAI1_CLKSOURCE_SPDIFRX1
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_IC7
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_I2S_CKIN
  *         @arg @ref LL_RCC_SAI2_CLKSOURCE_SPDIFRX1
  */
__STATIC_INLINE uint32_t LL_RCC_GetSAIClockSource(uint32_t Periph)
{
  return LL_RCC_GetClockSource(Periph);
}

/**
  * @brief  Get SDMMC clock source
  * @rmtoll CCIPR8      SDMMC1SEL      LL_RCC_GetSDMMCClockSource\n
  *         CCIPR8      SDMMC2SEL      LL_RCC_GetSDMMCClockSource
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE
  *         @arg @ref LL_RCC_SDMMC2_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE_IC4
  *         @arg @ref LL_RCC_SDMMC1_CLKSOURCE_IC5
  *         @arg @ref LL_RCC_SDMMC2_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_SDMMC2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SDMMC2_CLKSOURCE_IC4
  *         @arg @ref LL_RCC_SDMMC2_CLKSOURCE_IC5
  */
__STATIC_INLINE uint32_t LL_RCC_GetSDMMCClockSource(uint32_t Periph)
{
  return LL_RCC_GetClockSource(Periph);
}

/**
  * @brief  Get SPDIFRX Kernel clock source
  * @rmtoll CCIPR9      SPDIFRX1SEL        LL_RCC_GetSPDIFRXClockSource
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_SPDIFRX1_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_SPDIFRX1_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_SPDIFRX1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SPDIFRX1_CLKSOURCE_IC7
  *         @arg @ref LL_RCC_SPDIFRX1_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_SPDIFRX1_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SPDIFRX1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SPDIFRX1_CLKSOURCE_I2S_CKIN
  */
__STATIC_INLINE uint32_t LL_RCC_GetSPDIFRXClockSource(uint32_t Periph)
{
  UNUSED(Periph);
  return (uint32_t)(READ_BIT(RCC->CCIPR9, RCC_CCIPR9_SPDIFRX1SEL));
}

/**
  * @brief  Get SPIx Kernel clock source
  * @rmtoll CCIPR9     SPI1SEL         LL_RCC_GetSPIClockSource\n
  *         CCIPR9     SPI2SEL         LL_RCC_GetSPIClockSource\n
  *         CCIPR9     SPI3SEL         LL_RCC_GetSPIClockSource\n
  *         CCIPR9     SPI4SEL         LL_RCC_GetSPIClockSource\n
  *         CCIPR9     SPI5SEL         LL_RCC_GetSPIClockSource\n
  *         CCIPR9     SPI6SEL         LL_RCC_GetSPIClockSource
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SPI1_CLKSOURCE_I2S_CKIN
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SPI2_CLKSOURCE_I2S_CKIN
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SPI3_CLKSOURCE_I2S_CKIN
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SPI4_CLKSOURCE_HSE
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SPI5_CLKSOURCE_HSE
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE_PCLK4
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE_IC8
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_SPI6_CLKSOURCE_I2S_CKIN
  */
__STATIC_INLINE uint32_t LL_RCC_GetSPIClockSource(uint32_t Periph)
{
  return LL_RCC_GetClockSource(Periph);
}

/**
  * @brief  Get USARTx clock source
  * @rmtoll CCIPR13        USART1SEL      LL_RCC_GetUSARTClockSource\n
  *         CCIPR13        USART2SEL      LL_RCC_GetUSARTClockSource\n
  *         CCIPR13        USART3SEL      LL_RCC_GetUSARTClockSource\n
  *         CCIPR13        USART6SEL      LL_RCC_GetUSARTClockSource\n
  *         CCIPR14        USART10SEL     LL_RCC_GetUSARTClockSource
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_USART1_CLKSOURCE
  *         @arg @ref LL_RCC_USART2_CLKSOURCE
  *         @arg @ref LL_RCC_USART3_CLKSOURCE
  *         @arg @ref LL_RCC_USART6_CLKSOURCE
  *         @arg @ref LL_RCC_USART10_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_USART1_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_USART2_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_USART3_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_USART6_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_USART6_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_USART6_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_USART6_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_USART6_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_USART6_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_USART6_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_USART10_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_USART10_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_USART10_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_USART10_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_USART10_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_USART10_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_USART10_CLKSOURCE_HSI
  */
__STATIC_INLINE uint32_t LL_RCC_GetUSARTClockSource(uint32_t Periph)
{
  return LL_RCC_GetClockSource(Periph);
}

/**
  * @brief  Get USARTx clock source
  * @rmtoll CCIPR13        UART4SEL      LL_RCC_GetUARTClockSource\n
  *         CCIPR13        UART5SEL      LL_RCC_GetUARTClockSource\n
  *         CCIPR13        UART7SEL      LL_RCC_GetUARTClockSource\n
  *         CCIPR13        UART8SEL      LL_RCC_GetUARTClockSource\n
  *         CCIPR14        UART9SEL      LL_RCC_GetUARTClockSource
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_UART4_CLKSOURCE
  *         @arg @ref LL_RCC_UART5_CLKSOURCE
  *         @arg @ref LL_RCC_UART7_CLKSOURCE
  *         @arg @ref LL_RCC_UART8_CLKSOURCE
  *         @arg @ref LL_RCC_UART9_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_UART4_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_UART5_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_UART7_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_UART7_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_UART7_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_UART7_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_UART7_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_UART7_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_UART7_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_UART8_CLKSOURCE_PCLK1
  *         @arg @ref LL_RCC_UART8_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_UART8_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_UART8_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_UART8_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_UART8_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_UART8_CLKSOURCE_HSI
  *         @arg @ref LL_RCC_UART9_CLKSOURCE_PCLK2
  *         @arg @ref LL_RCC_UART9_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_UART9_CLKSOURCE_IC9
  *         @arg @ref LL_RCC_UART9_CLKSOURCE_IC14
  *         @arg @ref LL_RCC_UART9_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_UART9_CLKSOURCE_MSI
  *         @arg @ref LL_RCC_UART9_CLKSOURCE_HSI
  * @retval None
  */
__STATIC_INLINE uint32_t LL_RCC_GetUARTClockSource(uint32_t Periph)
{
  return LL_RCC_GetClockSource(Periph);
}

/**
  * @brief  Get USB clock source
  * @rmtoll CCIPR6      OTGPHY1SEL        LL_RCC_GetUSBClockSource\n
  *         CCIPR6      OTGPHY1CKREFSEL   LL_RCC_GetUSBClockSource\n
  *         CCIPR6      OTGPHY2SEL        LL_RCC_GetUSBClockSource\n
  *         CCIPR6      OTGPHY2CKREFSEL   LL_RCC_GetUSBClockSource\n
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_OTGPHY1_CLKSOURCE
  *         @arg @ref LL_RCC_OTGPHY1CKREF_CLKSOURCE
  *         @arg @ref LL_RCC_OTGPHY2_CLKSOURCE
  *         @arg @ref LL_RCC_OTGPHY2CKREF_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_OTGPHY1_CLKSOURCE_HSE_DIV_2
  *         @arg @ref LL_RCC_OTGPHY1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_OTGPHY1_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_OTGPHY1_CLKSOURCE_HSE_DIV_2_OSC
  *         @arg @ref LL_RCC_OTGPHY1CKREF_CLKSOURCE_OTGPHY1
  *         @arg @ref LL_RCC_OTGPHY1CKREF_CLKSOURCE_HSE_DIV_2_OSC
  *         @arg @ref LL_RCC_OTGPHY2_CLKSOURCE_HSE_DIV_2
  *         @arg @ref LL_RCC_OTGPHY2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_OTGPHY2_CLKSOURCE_IC15
  *         @arg @ref LL_RCC_OTGPHY2_CLKSOURCE_HSE_DIV_2_OSC
  *         @arg @ref LL_RCC_OTGPHY2CKREF_CLKSOURCE_OTGPHY2
  *         @arg @ref LL_RCC_OTGPHY2CKREF_CLKSOURCE_HSE_DIV_2_OSC
  */
__STATIC_INLINE uint32_t LL_RCC_GetUSBClockSource(uint32_t Periph)
{
  return LL_RCC_GetClockSource(Periph);
}


/**
  * @brief  Get XSPI Kernel clock source
  * @rmtoll CCIPR6      XSPI1SEL        LL_RCC_GetXSPIClockSource\n
  *         CCIPR6      XSPI2SEL        LL_RCC_GetXSPIClockSource\n
  *         CCIPR6      XSPI3SEL        LL_RCC_GetXSPIClockSource
  * @param  Periph This parameter can be one of the following values:
  *         @arg @ref LL_RCC_XSPI1_CLKSOURCE
  *         @arg @ref LL_RCC_XSPI2_CLKSOURCE
  *         @arg @ref LL_RCC_XSPI3_CLKSOURCE
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_XSPI1_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_XSPI1_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_XSPI1_CLKSOURCE_IC3
  *         @arg @ref LL_RCC_XSPI1_CLKSOURCE_IC4
  *         @arg @ref LL_RCC_XSPI2_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_XSPI2_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_XSPI2_CLKSOURCE_IC3
  *         @arg @ref LL_RCC_XSPI2_CLKSOURCE_IC4
  *         @arg @ref LL_RCC_XSPI3_CLKSOURCE_HCLK
  *         @arg @ref LL_RCC_XSPI3_CLKSOURCE_CLKP
  *         @arg @ref LL_RCC_XSPI3_CLKSOURCE_IC3
  *         @arg @ref LL_RCC_XSPI3_CLKSOURCE_IC4
  */
__STATIC_INLINE uint32_t LL_RCC_GetXSPIClockSource(uint32_t Periph)
{
  return LL_RCC_GetClockSource(Periph);
}


/**
  * @}
  */

/** @defgroup RCC_LL_EF_RTC RTC
  * @{
  */

/**
  * @brief  Set RTC Clock Source
  * @note   Once the RTC clock source has been selected, it cannot be changed anymore unless
  *         the Backup domain is reset, or unless a failure is detected on LSE (LSECSSD is
  *         set).
  * @rmtoll CCIPR7         RTCSEL        LL_RCC_SetRTCClockSource
  * @param  Source This parameter can be one of the following values:
  *         @arg @ref LL_RCC_RTC_CLKSOURCE_NONE
  *         @arg @ref LL_RCC_RTC_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_RTC_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_RTC_CLKSOURCE_HSE
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetRTCClockSource(uint32_t Source)
{
  MODIFY_REG(RCC->CCIPR7, RCC_CCIPR7_RTCSEL, Source);
}

/**
  * @brief  Get RTC Clock Source
  * @rmtoll CCIPR7       RTCSEL        LL_RCC_GetRTCClockSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_RTC_CLKSOURCE_NONE
  *         @arg @ref LL_RCC_RTC_CLKSOURCE_LSE
  *         @arg @ref LL_RCC_RTC_CLKSOURCE_LSI
  *         @arg @ref LL_RCC_RTC_CLKSOURCE_HSE
  */
__STATIC_INLINE uint32_t LL_RCC_GetRTCClockSource(void)
{
  return (uint32_t)(READ_BIT(RCC->CCIPR7, RCC_CCIPR7_RTCSEL));
}

/**
  * @brief  Enable RTC
  * @rmtoll APB4ENSR1    RTCENS        LL_RCC_EnableRTC
  * @retval None
  */
__STATIC_INLINE void LL_RCC_EnableRTC(void)
{
  WRITE_REG(RCC->APB4ENSR1, RCC_APB4ENSR1_RTCENS);
}

/**
  * @brief  Disable RTC
  * @rmtoll APB4ENCR1    RTCENC        LL_RCC_DisableRTC
  * @retval None
  */
__STATIC_INLINE void LL_RCC_DisableRTC(void)
{
  WRITE_REG(RCC->APB4ENCR1, RCC_APB4ENCR1_RTCENC);
}

/**
  * @brief  Check if RTC has been enabled or not
  * @rmtoll APB4ENR1       RTCEN         LL_RCC_IsEnabledRTC
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsEnabledRTC(void)
{
  return ((READ_BIT(RCC->APB4ENR1, RCC_APB4ENR1_RTCEN) == (RCC_APB4ENR1_RTCEN)) ? 1UL : 0UL);
}

/**
  * @brief  Force the Backup domain reset
  * @rmtoll BDCR         VSWRST         LL_RCC_ForceBackupDomainReset
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ForceBackupDomainReset(void)
{
  SET_BIT(RCC->BDCR, RCC_BDCR_VSWRST);
}

/**
  * @brief  Release the Backup domain reset
  * @rmtoll BDCR         VSWRST          LL_RCC_ReleaseBackupDomainReset
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ReleaseBackupDomainReset(void)
{
  CLEAR_BIT(RCC->BDCR, RCC_BDCR_VSWRST);
}

/**
  * @brief  Set HSE Prescaler for RTC Clock
  * @rmtoll CCIPR7       RTCPRE        LL_RCC_SetRTC_HSEPrescaler
  * @param  Prescaler This parameter can be one of the following values:
  *         @arg @ref LL_RCC_RTC_HSE_DIV_1
  *         @arg @ref LL_RCC_RTC_HSE_DIV_2
  *         @arg @ref LL_RCC_RTC_HSE_DIV_3
  *         @arg @ref LL_RCC_RTC_HSE_DIV_4
  *         @arg @ref LL_RCC_RTC_HSE_DIV_5
  *         @arg @ref LL_RCC_RTC_HSE_DIV_6
  *         @arg @ref LL_RCC_RTC_HSE_DIV_7
  *         @arg @ref LL_RCC_RTC_HSE_DIV_8
  *         @arg @ref LL_RCC_RTC_HSE_DIV_9
  *         @arg @ref LL_RCC_RTC_HSE_DIV_10
  *         @arg @ref LL_RCC_RTC_HSE_DIV_11
  *         @arg @ref LL_RCC_RTC_HSE_DIV_12
  *         @arg @ref LL_RCC_RTC_HSE_DIV_13
  *         @arg @ref LL_RCC_RTC_HSE_DIV_14
  *         @arg @ref LL_RCC_RTC_HSE_DIV_15
  *         @arg @ref LL_RCC_RTC_HSE_DIV_16
  *         @arg @ref LL_RCC_RTC_HSE_DIV_17
  *         @arg @ref LL_RCC_RTC_HSE_DIV_18
  *         @arg @ref LL_RCC_RTC_HSE_DIV_19
  *         @arg @ref LL_RCC_RTC_HSE_DIV_20
  *         @arg @ref LL_RCC_RTC_HSE_DIV_21
  *         @arg @ref LL_RCC_RTC_HSE_DIV_22
  *         @arg @ref LL_RCC_RTC_HSE_DIV_23
  *         @arg @ref LL_RCC_RTC_HSE_DIV_24
  *         @arg @ref LL_RCC_RTC_HSE_DIV_25
  *         @arg @ref LL_RCC_RTC_HSE_DIV_26
  *         @arg @ref LL_RCC_RTC_HSE_DIV_27
  *         @arg @ref LL_RCC_RTC_HSE_DIV_28
  *         @arg @ref LL_RCC_RTC_HSE_DIV_29
  *         @arg @ref LL_RCC_RTC_HSE_DIV_30
  *         @arg @ref LL_RCC_RTC_HSE_DIV_31
  *         @arg @ref LL_RCC_RTC_HSE_DIV_32
  *         @arg @ref LL_RCC_RTC_HSE_DIV_33
  *         @arg @ref LL_RCC_RTC_HSE_DIV_34
  *         @arg @ref LL_RCC_RTC_HSE_DIV_35
  *         @arg @ref LL_RCC_RTC_HSE_DIV_36
  *         @arg @ref LL_RCC_RTC_HSE_DIV_37
  *         @arg @ref LL_RCC_RTC_HSE_DIV_38
  *         @arg @ref LL_RCC_RTC_HSE_DIV_39
  *         @arg @ref LL_RCC_RTC_HSE_DIV_40
  *         @arg @ref LL_RCC_RTC_HSE_DIV_41
  *         @arg @ref LL_RCC_RTC_HSE_DIV_42
  *         @arg @ref LL_RCC_RTC_HSE_DIV_43
  *         @arg @ref LL_RCC_RTC_HSE_DIV_44
  *         @arg @ref LL_RCC_RTC_HSE_DIV_45
  *         @arg @ref LL_RCC_RTC_HSE_DIV_46
  *         @arg @ref LL_RCC_RTC_HSE_DIV_47
  *         @arg @ref LL_RCC_RTC_HSE_DIV_48
  *         @arg @ref LL_RCC_RTC_HSE_DIV_49
  *         @arg @ref LL_RCC_RTC_HSE_DIV_50
  *         @arg @ref LL_RCC_RTC_HSE_DIV_51
  *         @arg @ref LL_RCC_RTC_HSE_DIV_52
  *         @arg @ref LL_RCC_RTC_HSE_DIV_53
  *         @arg @ref LL_RCC_RTC_HSE_DIV_54
  *         @arg @ref LL_RCC_RTC_HSE_DIV_55
  *         @arg @ref LL_RCC_RTC_HSE_DIV_56
  *         @arg @ref LL_RCC_RTC_HSE_DIV_57
  *         @arg @ref LL_RCC_RTC_HSE_DIV_58
  *         @arg @ref LL_RCC_RTC_HSE_DIV_59
  *         @arg @ref LL_RCC_RTC_HSE_DIV_60
  *         @arg @ref LL_RCC_RTC_HSE_DIV_61
  *         @arg @ref LL_RCC_RTC_HSE_DIV_62
  *         @arg @ref LL_RCC_RTC_HSE_DIV_63
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetRTC_HSEPrescaler(uint32_t Prescaler)
{
  MODIFY_REG(RCC->CCIPR7, RCC_CCIPR7_RTCPRE, Prescaler);
}

/**
  * @brief  Get HSE Prescaler for RTC Clock
  * @rmtoll CCIPR7       RTCPRE        LL_RCC_GetRTC_HSEPrescaler
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_RTC_HSE_DIV_1
  *         @arg @ref LL_RCC_RTC_HSE_DIV_2
  *         @arg @ref LL_RCC_RTC_HSE_DIV_3
  *         @arg @ref LL_RCC_RTC_HSE_DIV_4
  *         @arg @ref LL_RCC_RTC_HSE_DIV_5
  *         @arg @ref LL_RCC_RTC_HSE_DIV_6
  *         @arg @ref LL_RCC_RTC_HSE_DIV_7
  *         @arg @ref LL_RCC_RTC_HSE_DIV_8
  *         @arg @ref LL_RCC_RTC_HSE_DIV_9
  *         @arg @ref LL_RCC_RTC_HSE_DIV_10
  *         @arg @ref LL_RCC_RTC_HSE_DIV_11
  *         @arg @ref LL_RCC_RTC_HSE_DIV_12
  *         @arg @ref LL_RCC_RTC_HSE_DIV_13
  *         @arg @ref LL_RCC_RTC_HSE_DIV_14
  *         @arg @ref LL_RCC_RTC_HSE_DIV_15
  *         @arg @ref LL_RCC_RTC_HSE_DIV_16
  *         @arg @ref LL_RCC_RTC_HSE_DIV_17
  *         @arg @ref LL_RCC_RTC_HSE_DIV_18
  *         @arg @ref LL_RCC_RTC_HSE_DIV_19
  *         @arg @ref LL_RCC_RTC_HSE_DIV_20
  *         @arg @ref LL_RCC_RTC_HSE_DIV_21
  *         @arg @ref LL_RCC_RTC_HSE_DIV_22
  *         @arg @ref LL_RCC_RTC_HSE_DIV_23
  *         @arg @ref LL_RCC_RTC_HSE_DIV_24
  *         @arg @ref LL_RCC_RTC_HSE_DIV_25
  *         @arg @ref LL_RCC_RTC_HSE_DIV_26
  *         @arg @ref LL_RCC_RTC_HSE_DIV_27
  *         @arg @ref LL_RCC_RTC_HSE_DIV_28
  *         @arg @ref LL_RCC_RTC_HSE_DIV_29
  *         @arg @ref LL_RCC_RTC_HSE_DIV_30
  *         @arg @ref LL_RCC_RTC_HSE_DIV_31
  *         @arg @ref LL_RCC_RTC_HSE_DIV_32
  *         @arg @ref LL_RCC_RTC_HSE_DIV_33
  *         @arg @ref LL_RCC_RTC_HSE_DIV_34
  *         @arg @ref LL_RCC_RTC_HSE_DIV_35
  *         @arg @ref LL_RCC_RTC_HSE_DIV_36
  *         @arg @ref LL_RCC_RTC_HSE_DIV_37
  *         @arg @ref LL_RCC_RTC_HSE_DIV_38
  *         @arg @ref LL_RCC_RTC_HSE_DIV_39
  *         @arg @ref LL_RCC_RTC_HSE_DIV_40
  *         @arg @ref LL_RCC_RTC_HSE_DIV_41
  *         @arg @ref LL_RCC_RTC_HSE_DIV_42
  *         @arg @ref LL_RCC_RTC_HSE_DIV_43
  *         @arg @ref LL_RCC_RTC_HSE_DIV_44
  *         @arg @ref LL_RCC_RTC_HSE_DIV_45
  *         @arg @ref LL_RCC_RTC_HSE_DIV_46
  *         @arg @ref LL_RCC_RTC_HSE_DIV_47
  *         @arg @ref LL_RCC_RTC_HSE_DIV_48
  *         @arg @ref LL_RCC_RTC_HSE_DIV_49
  *         @arg @ref LL_RCC_RTC_HSE_DIV_50
  *         @arg @ref LL_RCC_RTC_HSE_DIV_51
  *         @arg @ref LL_RCC_RTC_HSE_DIV_52
  *         @arg @ref LL_RCC_RTC_HSE_DIV_53
  *         @arg @ref LL_RCC_RTC_HSE_DIV_54
  *         @arg @ref LL_RCC_RTC_HSE_DIV_55
  *         @arg @ref LL_RCC_RTC_HSE_DIV_56
  *         @arg @ref LL_RCC_RTC_HSE_DIV_57
  *         @arg @ref LL_RCC_RTC_HSE_DIV_58
  *         @arg @ref LL_RCC_RTC_HSE_DIV_59
  *         @arg @ref LL_RCC_RTC_HSE_DIV_60
  *         @arg @ref LL_RCC_RTC_HSE_DIV_61
  *         @arg @ref LL_RCC_RTC_HSE_DIV_62
  *         @arg @ref LL_RCC_RTC_HSE_DIV_63
  */
__STATIC_INLINE uint32_t LL_RCC_GetRTC_HSEPrescaler(void)
{
  return (uint32_t)(READ_BIT(RCC->CCIPR7, RCC_CCIPR7_RTCPRE));
}

/**
  * @}
  */

/** @defgroup RCC_LL_EF_TIM_CLOCK_PRESCALER TIM
  * @{
  */

/**
  * @brief  Set Timers Clock Prescalers
  * @rmtoll CFGR2          TIMPRE        LL_RCC_SetTIMPrescaler
  * @param  Prescaler This parameter can be one of the following values:
  *         @arg @ref LL_RCC_TIM_PRESCALER_1
  *         @arg @ref LL_RCC_TIM_PRESCALER_2
  *         @arg @ref LL_RCC_TIM_PRESCALER_4
  *         @arg @ref LL_RCC_TIM_PRESCALER_8
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetTIMPrescaler(uint32_t Prescaler)
{
  MODIFY_REG(RCC->CFGR2, RCC_CFGR2_TIMPRE, Prescaler << RCC_CFGR2_TIMPRE_Pos);
}

/**
  * @brief  Get Timers Clock Prescalers
  * @rmtoll CFGR2          TIMPRE        LL_RCC_GetTIMPrescaler
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_TIM_PRESCALER_1
  *         @arg @ref LL_RCC_TIM_PRESCALER_2
  *         @arg @ref LL_RCC_TIM_PRESCALER_4
  *         @arg @ref LL_RCC_TIM_PRESCALER_8
  */
__STATIC_INLINE uint32_t LL_RCC_GetTIMPrescaler(void)
{
  return (uint32_t)(READ_BIT(RCC->CFGR2, RCC_CFGR2_TIMPRE) >> RCC_CFGR2_TIMPRE_Pos);
}

/**
  * @}
  */

/** @defgroup RCC_LL_EF_PLL PLL
  * @{
  */

/**
  * @brief  Set the oscillator used as PLL clock source.
  * @note   PLL1SEL can be written only when All PLLs are disabled.
  * @rmtoll PLL1CFGR1      PLL1SEL        LL_RCC_PLL1_SetSource
  * @param  PLLSource parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLSOURCE_HSI
  *         @arg @ref LL_RCC_PLLSOURCE_MSI
  *         @arg @ref LL_RCC_PLLSOURCE_HSE
  *         @arg @ref LL_RCC_PLLSOURCE_I2S_CKIN
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL1_SetSource(uint32_t PLLSource)
{
  MODIFY_REG(RCC->PLL1CFGR1, RCC_PLL1CFGR1_PLL1SEL, PLLSource);
}

/**
  * @brief  Get the oscillator used as PLL clock source.
  * @rmtoll PLL1CFGR1      PLL1SEL        LL_RCC_PLL1_GetSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_PLLSOURCE_HSI
  *         @arg @ref LL_RCC_PLLSOURCE_MSI
  *         @arg @ref LL_RCC_PLLSOURCE_HSE
  *         @arg @ref LL_RCC_PLLSOURCE_I2S_CKIN
  */
__STATIC_INLINE uint32_t LL_RCC_PLL1_GetSource(void)
{
  return (uint32_t)(READ_BIT(RCC->PLL1CFGR1, RCC_PLL1CFGR1_PLL1SEL));
}

/**
  * @brief  Enable PLL1
  * @rmtoll CSR          PLL1ONS         LL_RCC_PLL1_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL1_Enable(void)
{
  WRITE_REG(RCC->CSR, RCC_CSR_PLL1ONS);
}

/**
  * @brief  Disable PLL1
  * @note Cannot be disabled if the PLL1 clock is used as the system clock
  * @rmtoll CCR          PLL1ONC         LL_RCC_PLL1_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL1_Disable(void)
{
  WRITE_REG(RCC->CCR, RCC_CCR_PLL1ONC);
}

/**
  * @brief  Check if PLL1 Ready
  * @rmtoll SR           PLL1RDY         LL_RCC_PLL1_IsReady
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL1_IsReady(void)
{
  return ((READ_BIT(RCC->SR, RCC_SR_PLL1RDY) == (RCC_SR_PLL1RDY)) ? 1UL : 0UL);
}

/**
  * @brief  Enable PLL1 bypass (PLL1 output is driven by the PLL1 reference clock)
  * @note   This API shall be called only when PLL1 is disabled.
  * @rmtoll PLL1CFGR1    PLL1BYP         LL_RCC_PLL1_EnableBypass
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL1_EnableBypass(void)
{
  SET_BIT(RCC->PLL1CFGR1, RCC_PLL1CFGR1_PLL1BYP);
}

/**
  * @brief  Disable PLL1 bypass (PLL1 output is driven by the VCO)
  * @note   This API shall be called only when PLL1 is disabled.
  * @rmtoll PLL1CFGR1    PLL1BYP         LL_RCC_PLL1_DisableBypass
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL1_DisableBypass(void)
{
  CLEAR_BIT(RCC->PLL1CFGR1, RCC_PLL1CFGR1_PLL1BYP);
}

/**
  * @brief  Check if PLL1 bypass is enabled
  * @rmtoll PLL1CFGR1    PLL1BYP         LL_RCC_PLL1_IsEnabledBypass
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL1_IsEnabledBypass(void)
{
  return ((READ_BIT(RCC->PLL1CFGR1, RCC_PLL1CFGR1_PLL1BYP) == RCC_PLL1CFGR1_PLL1BYP) ? 1UL : 0UL);
}

/**
  * @brief  Assert PLL1 modulation spread-spectrum reset
  * @note   This API shall be called only when PLL1 is disabled.
  * @rmtoll PLL1CFGR3    PLL1MODSSRST    LL_RCC_PLL1_AssertModulationSpreadSpectrumReset
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL1_AssertModulationSpreadSpectrumReset(void)
{
  SET_BIT(RCC->PLL1CFGR3, RCC_PLL1CFGR3_PLL1MODSSRST);
}

/**
  * @brief  Release PLL1 modulation spread-spectrum reset
  * @note   This API shall be called only when PLL1 is disabled.
  * @rmtoll PLL1CFGR3    PLL1MODSSRST    LL_RCC_PLL1_ReleaseModulationSpreadSpectrumReset
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL1_ReleaseModulationSpreadSpectrumReset(void)
{
  CLEAR_BIT(RCC->PLL1CFGR3, RCC_PLL1CFGR3_PLL1MODSSRST);
}

/**
  * @brief  Enable PLL1 noise canceling DAC in fractional mode
  * @note   This API shall be called only when PLL1 is disabled.
  * @rmtoll PLL1CFGR3    PLL1DACEN       LL_RCC_PLL1_EnableDAC
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL1_EnableDAC(void)
{
  SET_BIT(RCC->PLL1CFGR3, RCC_PLL1CFGR3_PLL1DACEN);
}

/**
  * @brief  Disable PLL1 noise canceling DAC in fractional mode
  * @note   This API shall be called only when PLL1 is disabled.
  * @rmtoll PLL1CFGR3    PLL1DACEN     LL_RCC_PLL1_DisableDAC
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL1_DisableDAC(void)
{
  CLEAR_BIT(RCC->PLL1CFGR3, RCC_PLL1CFGR3_PLL1DACEN);
}

/**
  * @brief  Check if PLL1 noise canceling DAC in fractional mode is enabled
  * @rmtoll PLL1CFGR3    PLL1DACEN     LL_RCC_PLL1_IsEnabledDAC
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL1_IsEnabledDAC(void)
{
  return ((READ_BIT(RCC->PLL1CFGR3, RCC_PLL1CFGR3_PLL1DACEN) == RCC_PLL1CFGR3_PLL1DACEN) ? 1UL : 0UL);
}

/**
  * @brief  Enable PLL1 modulation spread-spectrum with fractional divide inactive
  * @note   This API shall be called only when PLL1 is disabled.
  * @rmtoll PLL1CFGR3    PLL1MODSSDIS    LL_RCC_PLL1_EnableModulationSpreadSpectrum
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL1_EnableModulationSpreadSpectrum(void)
{
  CLEAR_BIT(RCC->PLL1CFGR3, RCC_PLL1CFGR3_PLL1MODSSDIS);
}

/**
  * @brief  Disable PLL1 modulation spread-spectrum but active fractional divide
  * @note   This API shall be called only when PLL1 is disabled.
  * @rmtoll PLL1CFGR3    PLL1MODSSDIS    LL_RCC_PLL1_DisableModulationSpreadSpectrum
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL1_DisableModulationSpreadSpectrum(void)
{
  SET_BIT(RCC->PLL1CFGR3, RCC_PLL1CFGR3_PLL1MODSSDIS);
}

/**
  * @brief  Check if PLL1 modulation spread-spectrum and inactive fractional divide is enabled
  * @rmtoll PLL1CFGR3    PLL1MODSSDIS    LL_RCC_PLL1_IsEnabledModulationSpreadSpectrum
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL1_IsEnabledModulationSpreadSpectrum(void)
{
  return ((READ_BIT(RCC->PLL1CFGR3, RCC_PLL1CFGR3_PLL1MODSSDIS) == 0UL) ? 1UL : 0UL);
}

/**
  * @brief  Enable PLL1 fractional divide and modulation spread-spectrum
  * @note   This API shall be called only when PLL1 is disabled.
  * @rmtoll PLL1CFGR3    PLL1MODDSEN     LL_RCC_PLL1_EnableFractionalModulationSpreadSpectrum
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL1_EnableFractionalModulationSpreadSpectrum(void)
{
  SET_BIT(RCC->PLL1CFGR3, RCC_PLL1CFGR3_PLL1MODDSEN);
}

/**
  * @brief  Disable PLL1 fractional divide and modulation spread-spectrum
  * @note   This API shall be called only when PLL1 is disabled.
  * @rmtoll PLL1CFGR3    PLL1MODDSEN     LL_RCC_PLL1_DisableFractionalModulationSpreadSpectrum
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL1_DisableFractionalModulationSpreadSpectrum(void)
{
  CLEAR_BIT(RCC->PLL1CFGR3, RCC_PLL1CFGR3_PLL1MODDSEN);
}

/**
  * @brief  Check if PLL1 fractional divide and modulation spread-spectrum is enabled
  * @rmtoll PLL1CFGR3    PLL1MODDSEN     LL_RCC_PLL1_IsEnabledFractionalModulationSpreadSpectrum
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL1_IsEnabledFractionalModulationSpreadSpectrum(void)
{
  return ((READ_BIT(RCC->PLL1CFGR3, RCC_PLL1CFGR3_PLL1MODDSEN) == RCC_PLL1CFGR3_PLL1MODDSEN) ? 1UL : 0UL);
}

/**
  * @brief  Set PLL1 DIVN Coefficient
  * @note   This API shall be called only when PLL1 is disabled.
  * @rmtoll PLL1CFGR1    PLL1DIVN        LL_RCC_PLL1_SetN
  * @param  N  In integer mode, N parameter can be a value between 16 (0x10) and 2500 (0x9C4).
  *         In fractional mode, N parameter can be a value between 20 (0x14) and 500 (0x1F4).
  */
__STATIC_INLINE void LL_RCC_PLL1_SetN(uint32_t N)
{
  MODIFY_REG(RCC->PLL1CFGR1, RCC_PLL1CFGR1_PLL1DIVN, N << RCC_PLL1CFGR1_PLL1DIVN_Pos);
}

/**
  * @brief  Get PLL1 DIVN Coefficient
  * @rmtoll PLL1CFGR1    PLL1DIVN        LL_RCC_PLL1_GetN
  * @retval In integer mode, a value between 16 (0x10) and 2500 (0x9C4).
  *         In fractional mode, a value between 20 (0x14) and 500 (0x1F4).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL1_GetN(void)
{
  return (uint32_t)((READ_BIT(RCC->PLL1CFGR1, RCC_PLL1CFGR1_PLL1DIVN) >> RCC_PLL1CFGR1_PLL1DIVN_Pos));
}

/**
  * @brief  Set PLL1 DIVM Coefficient
  * @note   This API shall be called only when PLL1 is disabled.
  * @rmtoll PLL1CFGR1    PLL1DIVM        LL_RCC_PLL1_SetM
  * @param  M parameter can be a value between 1 and 63
  */
__STATIC_INLINE void LL_RCC_PLL1_SetM(uint32_t M)
{
  MODIFY_REG(RCC->PLL1CFGR1, RCC_PLL1CFGR1_PLL1DIVM, M << RCC_PLL1CFGR1_PLL1DIVM_Pos);
}

/**
  * @brief  Get PLL1 DIVM Coefficient
  * @rmtoll PLL1CFGR1    PLL1DIVM        LL_RCC_PLL1_GetM
  * @retval A value between 1 and 63
  */
__STATIC_INLINE uint32_t LL_RCC_PLL1_GetM(void)
{
  return (uint32_t)(READ_BIT(RCC->PLL1CFGR1, RCC_PLL1CFGR1_PLL1DIVM) >> RCC_PLL1CFGR1_PLL1DIVM_Pos);
}

/**
  * @brief  Set PLL1 PDIV1 Coefficient
  * @note   This API shall be called only when PLL1 is disabled.
  * @rmtoll PLL1CFGR3    PLL1PDIV1       LL_RCC_PLL1_SetP1
  * @param  P1 parameter can be a value between 1 and 7 when PLL1 is enabled, 0 when disabled
  */
__STATIC_INLINE void LL_RCC_PLL1_SetP1(uint32_t P1)
{
  MODIFY_REG(RCC->PLL1CFGR3, RCC_PLL1CFGR3_PLL1PDIV1, P1 << RCC_PLL1CFGR3_PLL1PDIV1_Pos);
}

/**
  * @brief  Get PLL1 PDIV2 Coefficient
  * @rmtoll PLL1CFGR3    PLL1PDIV1       LL_RCC_PLL1_GetP1
  * @retval A value between 0 and 7
  */
__STATIC_INLINE uint32_t LL_RCC_PLL1_GetP1(void)
{
  return (uint32_t)(READ_BIT(RCC->PLL1CFGR3, RCC_PLL1CFGR3_PLL1PDIV1) >> RCC_PLL1CFGR3_PLL1PDIV1_Pos);
}

/**
  * @brief  Set PLL1 PDIV2 Coefficient
  * @note   This API shall be called only when PLL1 is disabled.
  * @rmtoll PLL1CFGR3    PLL1PDIV2       LL_RCC_PLL1_SetP2
  * @param  P2 parameter can be a value between 1 and 7
  */
__STATIC_INLINE void LL_RCC_PLL1_SetP2(uint32_t P2)
{
  MODIFY_REG(RCC->PLL1CFGR3, RCC_PLL1CFGR3_PLL1PDIV2, P2 << RCC_PLL1CFGR3_PLL1PDIV2_Pos);
}

/**
  * @brief  Get PLL1 PDIV2 Coefficient
  * @rmtoll PLL1CFGR3    PLL1PDIV2       LL_RCC_PLL1_GetP2
  * @retval A value between 1 and 7
  */
__STATIC_INLINE uint32_t LL_RCC_PLL1_GetP2(void)
{
  return (uint32_t)(READ_BIT(RCC->PLL1CFGR3, RCC_PLL1CFGR3_PLL1PDIV2) >> RCC_PLL1CFGR3_PLL1PDIV2_Pos);
}

/**
  * @brief  Enable PLL1P
  * @note   This API shall be called only when PLL1 is disabled.
  * @rmtoll PLL1CFGR3           PLL1PDIVEN         LL_RCC_PLL1P_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL1P_Enable(void)
{
  SET_BIT(RCC->PLL1CFGR3, RCC_PLL1CFGR3_PLL1PDIVEN);
}

/**
  * @brief  Disable PLL1P
  * @note   This API shall be called only when PLL1 is disabled.
  * @rmtoll PLL1CFGR3           PLL1PDIVEN         LL_RCC_PLL1P_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL1P_Disable(void)
{
  CLEAR_BIT(RCC->PLL1CFGR3, RCC_PLL1CFGR3_PLL1PDIVEN);
}

/**
  * @brief  Check if PLL1 P is enabled
  * @rmtoll PLL1CFGR3           PLL1PDIVEN         LL_RCC_PLL1P_IsEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL1P_IsEnabled(void)
{
  return ((READ_BIT(RCC->PLL1CFGR3, RCC_PLL1CFGR3_PLL1PDIVEN) == RCC_PLL1CFGR3_PLL1PDIVEN) ? 1UL : 0UL);
}

/**
  * @brief  Set PLL1 FRACN Coefficient
  * @note   This API shall be called only when PLL1 is disabled.
  * @rmtoll PLL1CFGR2        PLL1DIVNFRAC    LL_RCC_PLL1_SetFRACN
  * @param  FRACN parameter can be a value between 0 and 2^24 (0xFFFFFF)
  */
__STATIC_INLINE void LL_RCC_PLL1_SetFRACN(uint32_t FRACN)
{
  MODIFY_REG(RCC->PLL1CFGR2, RCC_PLL1CFGR2_PLL1DIVNFRAC, FRACN << RCC_PLL1CFGR2_PLL1DIVNFRAC_Pos);
}

/**
  * @brief  Get PLL1 FRACN Coefficient
  * @rmtoll PLL1CFGR2        PLL1DIVNFRAC    LL_RCC_PLL1_GetFRACN
  * @retval A value between 0 and 2^24 (0xFFFFFF)
  */
__STATIC_INLINE uint32_t LL_RCC_PLL1_GetFRACN(void)
{
  return (uint32_t)(READ_BIT(RCC->PLL1CFGR2, RCC_PLL1CFGR2_PLL1DIVNFRAC) >> RCC_PLL1CFGR2_PLL1DIVNFRAC_Pos);
}

/**
  * @brief  Set the oscillator used as PLL clock source.
  * @note   PLL2SEL can be written only when All PLLs are disabled.
  * @rmtoll PLL2CFGR1      PLL2SEL        LL_RCC_PLL2_SetSource
  * @param  PLLSource parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLSOURCE_HSI
  *         @arg @ref LL_RCC_PLLSOURCE_MSI
  *         @arg @ref LL_RCC_PLLSOURCE_HSE
  *         @arg @ref LL_RCC_PLLSOURCE_I2S_CKIN
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL2_SetSource(uint32_t PLLSource)
{
  MODIFY_REG(RCC->PLL2CFGR1, RCC_PLL2CFGR1_PLL2SEL, PLLSource);
}

/**
  * @brief  Get the oscillator used as PLL clock source.
  * @rmtoll PLL2CFGR1      PLL2SEL        LL_RCC_PLL2_GetSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_PLLSOURCE_HSI
  *         @arg @ref LL_RCC_PLLSOURCE_MSI
  *         @arg @ref LL_RCC_PLLSOURCE_HSE
  *         @arg @ref LL_RCC_PLLSOURCE_I2S_CKIN
  */
__STATIC_INLINE uint32_t LL_RCC_PLL2_GetSource(void)
{
  return (uint32_t)(READ_BIT(RCC->PLL2CFGR1, RCC_PLL2CFGR1_PLL2SEL));
}

/**
  * @brief  Enable PLL2
  * @rmtoll CSR          PLL2ONS         LL_RCC_PLL2_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL2_Enable(void)
{
  WRITE_REG(RCC->CSR, RCC_CSR_PLL2ONS);
}

/**
  * @brief  Disable PLL2
  * @note Cannot be disabled if the PLL2 clock is used as the system clock
  * @rmtoll CCR          PLL2ONC         LL_RCC_PLL2_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL2_Disable(void)
{
  WRITE_REG(RCC->CCR, RCC_CCR_PLL2ONC);
}

/**
  * @brief  Check if PLL2 Ready
  * @rmtoll SR           PLL2RDY         LL_RCC_PLL2_IsReady
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL2_IsReady(void)
{
  return ((READ_BIT(RCC->SR, RCC_SR_PLL2RDY) == (RCC_SR_PLL2RDY)) ? 1UL : 0UL);
}

/**
  * @brief  Enable PLL2 bypass (PLL2 output is driven by the PLL2 reference clock)
  * @note   This API shall be called only when PLL2 is disabled.
  * @rmtoll PLL2CFGR1    PLL2BYP         LL_RCC_PLL2_EnableBypass
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL2_EnableBypass(void)
{
  SET_BIT(RCC->PLL2CFGR1, RCC_PLL2CFGR1_PLL2BYP);
}

/**
  * @brief  Disable PLL2 bypass (PLL2 output is driven by the VCO)
  * @note   This API shall be called only when PLL2 is disabled.
  * @rmtoll PLL2CFGR1    PLL2BYP         LL_RCC_PLL2_DisableBypass
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL2_DisableBypass(void)
{
  CLEAR_BIT(RCC->PLL2CFGR1, RCC_PLL2CFGR1_PLL2BYP);
}

/**
  * @brief  Check if PLL2 bypass is enabled
  * @rmtoll PLL2CFGR1    PLL2BYP         LL_RCC_PLL2_IsEnabledBypass
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL2_IsEnabledBypass(void)
{
  return ((READ_BIT(RCC->PLL2CFGR1, RCC_PLL2CFGR1_PLL2BYP) == RCC_PLL2CFGR1_PLL2BYP) ? 1UL : 0UL);
}

/**
  * @brief  Assert PLL2 modulation spread-spectrum reset
  * @note   This API shall be called only when PLL2 is disabled.
  * @rmtoll PLL2CFGR3    PLL2MODSSRST    LL_RCC_PLL2_AssertModulationSpreadSpectrumReset
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL2_AssertModulationSpreadSpectrumReset(void)
{
  SET_BIT(RCC->PLL2CFGR3, RCC_PLL2CFGR3_PLL2MODSSRST);
}

/**
  * @brief  Release PLL2 modulation spread-spectrum reset
  * @note   This API shall be called only when PLL2 is disabled.
  * @rmtoll PLL2CFGR3    PLL2MODSSRST    LL_RCC_PLL2_ReleaseModulationSpreadSpectrumReset
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL2_ReleaseModulationSpreadSpectrumReset(void)
{
  CLEAR_BIT(RCC->PLL2CFGR3, RCC_PLL2CFGR3_PLL2MODSSRST);
}

/**
  * @brief  Enable PLL2 noise canceling DAC in fractional mode
  * @note   This API shall be called only when PLL2 is disabled.
  * @rmtoll PLL2CFGR3    PLL2DACEN       LL_RCC_PLL2_EnableDAC
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL2_EnableDAC(void)
{
  SET_BIT(RCC->PLL2CFGR3, RCC_PLL2CFGR3_PLL2DACEN);
}

/**
  * @brief  Disable PLL2 noise canceling DAC in fractional mode
  * @note   This API shall be called only when PLL2 is disabled.
  * @rmtoll PLL2CFGR3    PLL2DACEN     LL_RCC_PLL2_DisableDAC
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL2_DisableDAC(void)
{
  CLEAR_BIT(RCC->PLL2CFGR3, RCC_PLL2CFGR3_PLL2DACEN);
}

/**
  * @brief  Check if PLL2 noise canceling DAC in fractional mode is enabled
  * @rmtoll PLL2CFGR3    PLL2DACEN     LL_RCC_PLL2_IsEnabledDAC
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL2_IsEnabledDAC(void)
{
  return ((READ_BIT(RCC->PLL2CFGR3, RCC_PLL2CFGR3_PLL2DACEN) == RCC_PLL2CFGR3_PLL2DACEN) ? 1UL : 0UL);
}

/**
  * @brief  Enable PLL2 modulation spread-spectrum with fractional divide inactive
  * @note   This API shall be called only when PLL2 is disabled.
  * @rmtoll PLL2CFGR3    PLL2MODSSDIS    LL_RCC_PLL2_EnableModulationSpreadSpectrum
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL2_EnableModulationSpreadSpectrum(void)
{
  CLEAR_BIT(RCC->PLL2CFGR3, RCC_PLL2CFGR3_PLL2MODSSDIS);
}

/**
  * @brief  Disable PLL2 modulation spread-spectrum but active fractional divide
  * @note   This API shall be called only when PLL2 is disabled.
  * @rmtoll PLL2CFGR3    PLL2MODSSDIS    LL_RCC_PLL2_DisableModulationSpreadSpectrum
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL2_DisableModulationSpreadSpectrum(void)
{
  SET_BIT(RCC->PLL2CFGR3, RCC_PLL2CFGR3_PLL2MODSSDIS);
}

/**
  * @brief  Check if PLL2 modulation spread-spectrum and inactive fractional divide is enabled
  * @rmtoll PLL2CFGR3    PLL2MODSSDIS    LL_RCC_PLL2_IsEnabledModulationSpreadSpectrum
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL2_IsEnabledModulationSpreadSpectrum(void)
{
  return ((READ_BIT(RCC->PLL2CFGR3, RCC_PLL2CFGR3_PLL2MODSSDIS) == 0UL) ? 1UL : 0UL);
}

/**
  * @brief  Enable PLL2 fractional divide and modulation spread-spectrum
  * @note   This API shall be called only when PLL2 is disabled.
  * @rmtoll PLL2CFGR3    PLL2MODDSEN     LL_RCC_PLL2_EnableFractionalModulationSpreadSpectrum
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL2_EnableFractionalModulationSpreadSpectrum(void)
{
  SET_BIT(RCC->PLL2CFGR3, RCC_PLL2CFGR3_PLL2MODDSEN);
}

/**
  * @brief  Disable PLL2 fractional divide and modulation spread-spectrum
  * @note   This API shall be called only when PLL2 is disabled.
  * @rmtoll PLL2CFGR3    PLL2MODDSEN     LL_RCC_PLL2_DisableFractionalModulationSpreadSpectrum
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL2_DisableFractionalModulationSpreadSpectrum(void)
{
  CLEAR_BIT(RCC->PLL2CFGR3, RCC_PLL2CFGR3_PLL2MODDSEN);
}

/**
  * @brief  Check if PLL2 fractional divide and modulation spread-spectrum is enabled
  * @rmtoll PLL2CFGR3    PLL2MODDSEN     LL_RCC_PLL2_IsEnabledFractionalModulationSpreadSpectrum
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL2_IsEnabledFractionalModulationSpreadSpectrum(void)
{
  return ((READ_BIT(RCC->PLL2CFGR3, RCC_PLL2CFGR3_PLL2MODDSEN) == RCC_PLL2CFGR3_PLL2MODDSEN) ? 1UL : 0UL);
}

/**
  * @brief  Set PLL2 DIVN Coefficient
  * @note   This API shall be called only when PLL2 is disabled.
  * @rmtoll PLL2CFGR1    PLL2DIVN        LL_RCC_PLL2_SetN
  * @param  N  In integer mode, N parameter can be a value between 16 (0x10) and 2500 (0x9C4).
  *         In fractional mode, N parameter can be a value between 20 (0x14) and 500 (0x1F4).
  */
__STATIC_INLINE void LL_RCC_PLL2_SetN(uint32_t N)
{
  MODIFY_REG(RCC->PLL2CFGR1, RCC_PLL2CFGR1_PLL2DIVN, N << RCC_PLL2CFGR1_PLL2DIVN_Pos);
}

/**
  * @brief  Get PLL2 DIVN Coefficient
  * @rmtoll PLL2CFGR1    PLL2DIVN        LL_RCC_PLL2_GetN
  * @retval In integer mode, a value between 16 (0x10) and 2500 (0x9C4).
  *         In fractional mode, a value between 20 (0x14) and 500 (0x1F4).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL2_GetN(void)
{
  return (uint32_t)((READ_BIT(RCC->PLL2CFGR1, RCC_PLL2CFGR1_PLL2DIVN) >> RCC_PLL2CFGR1_PLL2DIVN_Pos));
}

/**
  * @brief  Set PLL2 DIVM Coefficient
  * @note   This API shall be called only when PLL2 is disabled.
  * @rmtoll PLL2CFGR1    PLL2DIVM        LL_RCC_PLL2_SetM
  * @param  M parameter can be a value between 1 and 63
  */
__STATIC_INLINE void LL_RCC_PLL2_SetM(uint32_t M)
{
  MODIFY_REG(RCC->PLL2CFGR1, RCC_PLL2CFGR1_PLL2DIVM, M << RCC_PLL2CFGR1_PLL2DIVM_Pos);
}

/**
  * @brief  Get PLL2 DIVM Coefficient
  * @rmtoll PLL2CFGR1    PLL2DIVM        LL_RCC_PLL2_GetM
  * @retval A value between 1 and 63
  */
__STATIC_INLINE uint32_t LL_RCC_PLL2_GetM(void)
{
  return (uint32_t)(READ_BIT(RCC->PLL2CFGR1, RCC_PLL2CFGR1_PLL2DIVM) >> RCC_PLL2CFGR1_PLL2DIVM_Pos);
}

/**
  * @brief  Set PLL2 PDIV1 Coefficient
  * @note   This API shall be called only when PLL2 is disabled.
  * @rmtoll PLL2CFGR3    PLL2PDIV1       LL_RCC_PLL2_SetP1
  * @param  P1 parameter can be a value between 1 and 7 when PLL2 is enabled, 0 when disabled
  */
__STATIC_INLINE void LL_RCC_PLL2_SetP1(uint32_t P1)
{
  MODIFY_REG(RCC->PLL2CFGR3, RCC_PLL2CFGR3_PLL2PDIV1, P1 << RCC_PLL2CFGR3_PLL2PDIV1_Pos);
}

/**
  * @brief  Get PLL2 PDIV2 Coefficient
  * @rmtoll PLL2CFGR3    PLL2PDIV1       LL_RCC_PLL2_GetP1
  * @retval A value between 0 and 7
  */
__STATIC_INLINE uint32_t LL_RCC_PLL2_GetP1(void)
{
  return (uint32_t)(READ_BIT(RCC->PLL2CFGR3, RCC_PLL2CFGR3_PLL2PDIV1) >> RCC_PLL2CFGR3_PLL2PDIV1_Pos);
}

/**
  * @brief  Set PLL2 PDIV2 Coefficient
  * @note   This API shall be called only when PLL2 is disabled.
  * @rmtoll PLL2CFGR3    PLL2PDIV2       LL_RCC_PLL2_SetP2
  * @param  P2 parameter can be a value between 1 and 7
  */
__STATIC_INLINE void LL_RCC_PLL2_SetP2(uint32_t P2)
{
  MODIFY_REG(RCC->PLL2CFGR3, RCC_PLL2CFGR3_PLL2PDIV2, P2 << RCC_PLL2CFGR3_PLL2PDIV2_Pos);
}

/**
  * @brief  Get PLL2 PDIV2 Coefficient
  * @rmtoll PLL2CFGR3    PLL2PDIV2       LL_RCC_PLL2_GetP2
  * @retval A value between 1 and 7
  */
__STATIC_INLINE uint32_t LL_RCC_PLL2_GetP2(void)
{
  return (uint32_t)(READ_BIT(RCC->PLL2CFGR3, RCC_PLL2CFGR3_PLL2PDIV2) >> RCC_PLL2CFGR3_PLL2PDIV2_Pos);
}

/**
  * @brief  Enable PLL2P
  * @note   This API shall be called only when PLL2 is disabled.
  * @rmtoll PLL2CFGR3           PLL2PDIVEN         LL_RCC_PLL2P_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL2P_Enable(void)
{
  SET_BIT(RCC->PLL2CFGR3, RCC_PLL2CFGR3_PLL2PDIVEN);
}

/**
  * @brief  Disable PLL2P
  * @note   This API shall be called only when PLL2 is disabled.
  * @rmtoll PLL2CFGR3           PLL2PDIVEN         LL_RCC_PLL2P_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL2P_Disable(void)
{
  CLEAR_BIT(RCC->PLL2CFGR3, RCC_PLL2CFGR3_PLL2PDIVEN);
}

/**
  * @brief  Check if PLL2 P is enabled
  * @rmtoll PLL2CFGR3           PLL2PDIVEN         LL_RCC_PLL2P_IsEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL2P_IsEnabled(void)
{
  return ((READ_BIT(RCC->PLL2CFGR3, RCC_PLL2CFGR3_PLL2PDIVEN) == RCC_PLL2CFGR3_PLL2PDIVEN) ? 1UL : 0UL);
}

/**
  * @brief  Set PLL2 FRACN Coefficient
  * @note   This API shall be called only when PLL2 is disabled.
  * @rmtoll PLL2CFGR2        PLL2DIVNFRAC    LL_RCC_PLL2_SetFRACN
  * @param  FRACN parameter can be a value between 0 and 2^24 (0xFFFFFF)
  */
__STATIC_INLINE void LL_RCC_PLL2_SetFRACN(uint32_t FRACN)
{
  MODIFY_REG(RCC->PLL2CFGR2, RCC_PLL2CFGR2_PLL2DIVNFRAC, FRACN << RCC_PLL2CFGR2_PLL2DIVNFRAC_Pos);
}

/**
  * @brief  Get PLL2 FRACN Coefficient
  * @rmtoll PLL2CFGR2        PLL2DIVNFRAC    LL_RCC_PLL2_GetFRACN
  * @retval A value between 0 and 2^24 (0xFFFFFF)
  */
__STATIC_INLINE uint32_t LL_RCC_PLL2_GetFRACN(void)
{
  return (uint32_t)(READ_BIT(RCC->PLL2CFGR2, RCC_PLL2CFGR2_PLL2DIVNFRAC) >> RCC_PLL2CFGR2_PLL2DIVNFRAC_Pos);
}

/**
  * @brief  Set the oscillator used as PLL clock source.
  * @note   PLL3SEL can be written only when All PLLs are disabled.
  * @rmtoll PLL3CFGR1      PLL3SEL        LL_RCC_PLL3_SetSource
  * @param  PLLSource parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLSOURCE_HSI
  *         @arg @ref LL_RCC_PLLSOURCE_MSI
  *         @arg @ref LL_RCC_PLLSOURCE_HSE
  *         @arg @ref LL_RCC_PLLSOURCE_I2S_CKIN
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL3_SetSource(uint32_t PLLSource)
{
  MODIFY_REG(RCC->PLL3CFGR1, RCC_PLL3CFGR1_PLL3SEL, PLLSource);
}

/**
  * @brief  Get the oscillator used as PLL clock source.
  * @rmtoll PLL3CFGR1      PLL3SEL        LL_RCC_PLL3_GetSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_PLLSOURCE_HSI
  *         @arg @ref LL_RCC_PLLSOURCE_MSI
  *         @arg @ref LL_RCC_PLLSOURCE_HSE
  *         @arg @ref LL_RCC_PLLSOURCE_I2S_CKIN
  */
__STATIC_INLINE uint32_t LL_RCC_PLL3_GetSource(void)
{
  return (uint32_t)(READ_BIT(RCC->PLL3CFGR1, RCC_PLL3CFGR1_PLL3SEL));
}

/**
  * @brief  Enable PLL3
  * @rmtoll CSR          PLL3ONS         LL_RCC_PLL3_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL3_Enable(void)
{
  WRITE_REG(RCC->CSR, RCC_CSR_PLL3ONS);
}

/**
  * @brief  Disable PLL3
  * @note Cannot be disabled if the PLL3 clock is used as the system clock
  * @rmtoll CCR          PLL3ONC         LL_RCC_PLL3_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL3_Disable(void)
{
  WRITE_REG(RCC->CCR, RCC_CCR_PLL3ONC);
}

/**
  * @brief  Check if PLL3 Ready
  * @rmtoll SR           PLL3RDY         LL_RCC_PLL3_IsReady
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL3_IsReady(void)
{
  return ((READ_BIT(RCC->SR, RCC_SR_PLL3RDY) == (RCC_SR_PLL3RDY)) ? 1UL : 0UL);
}

/**
  * @brief  Enable PLL3 bypass (PLL3 output is driven by the PLL3 reference clock)
  * @note   This API shall be called only when PLL3 is disabled.
  * @rmtoll PLL3CFGR1    PLL3BYP         LL_RCC_PLL3_EnableBypass
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL3_EnableBypass(void)
{
  SET_BIT(RCC->PLL3CFGR1, RCC_PLL3CFGR1_PLL3BYP);
}

/**
  * @brief  Disable PLL3 bypass (PLL3 output is driven by the VCO)
  * @note   This API shall be called only when PLL3 is disabled.
  * @rmtoll PLL3CFGR1    PLL3BYP         LL_RCC_PLL3_DisableBypass
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL3_DisableBypass(void)
{
  CLEAR_BIT(RCC->PLL3CFGR1, RCC_PLL3CFGR1_PLL3BYP);
}

/**
  * @brief  Check if PLL3 bypass is enabled
  * @rmtoll PLL3CFGR1    PLL3BYP         LL_RCC_PLL3_IsEnabledBypass
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL3_IsEnabledBypass(void)
{
  return ((READ_BIT(RCC->PLL3CFGR1, RCC_PLL3CFGR1_PLL3BYP) == RCC_PLL3CFGR1_PLL3BYP) ? 1UL : 0UL);
}

/**
  * @brief  Assert PLL3 modulation spread-spectrum reset
  * @note   This API shall be called only when PLL3 is disabled.
  * @rmtoll PLL3CFGR3    PLL3MODSSRST    LL_RCC_PLL3_AssertModulationSpreadSpectrumReset
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL3_AssertModulationSpreadSpectrumReset(void)
{
  SET_BIT(RCC->PLL3CFGR3, RCC_PLL3CFGR3_PLL3MODSSRST);
}

/**
  * @brief  Release PLL3 modulation spread-spectrum reset
  * @note   This API shall be called only when PLL3 is disabled.
  * @rmtoll PLL3CFGR3    PLL3MODSSRST    LL_RCC_PLL3_ReleaseModulationSpreadSpectrumReset
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL3_ReleaseModulationSpreadSpectrumReset(void)
{
  CLEAR_BIT(RCC->PLL3CFGR3, RCC_PLL3CFGR3_PLL3MODSSRST);
}

/**
  * @brief  Enable PLL3 noise canceling DAC in fractional mode
  * @note   This API shall be called only when PLL3 is disabled.
  * @rmtoll PLL3CFGR3    PLL3DACEN       LL_RCC_PLL3_EnableDAC
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL3_EnableDAC(void)
{
  SET_BIT(RCC->PLL3CFGR3, RCC_PLL3CFGR3_PLL3DACEN);
}

/**
  * @brief  Disable PLL3 noise canceling DAC in fractional mode
  * @note   This API shall be called only when PLL3 is disabled.
  * @rmtoll PLL3CFGR3    PLL3DACEN     LL_RCC_PLL3_DisableDAC
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL3_DisableDAC(void)
{
  CLEAR_BIT(RCC->PLL3CFGR3, RCC_PLL3CFGR3_PLL3DACEN);
}

/**
  * @brief  Check if PLL3 noise canceling DAC in fractional mode is enabled
  * @rmtoll PLL3CFGR3    PLL3DACEN     LL_RCC_PLL3_IsEnabledDAC
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL3_IsEnabledDAC(void)
{
  return ((READ_BIT(RCC->PLL3CFGR3, RCC_PLL3CFGR3_PLL3DACEN) == RCC_PLL3CFGR3_PLL3DACEN) ? 1UL : 0UL);
}

/**
  * @brief  Enable PLL3 modulation spread-spectrum with fractional divide inactive
  * @note   This API shall be called only when PLL3 is disabled.
  * @rmtoll PLL3CFGR3    PLL3MODSSDIS    LL_RCC_PLL3_EnableModulationSpreadSpectrum
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL3_EnableModulationSpreadSpectrum(void)
{
  CLEAR_BIT(RCC->PLL3CFGR3, RCC_PLL3CFGR3_PLL3MODSSDIS);
}

/**
  * @brief  Disable PLL3 modulation spread-spectrum but active fractional divide
  * @note   This API shall be called only when PLL3 is disabled.
  * @rmtoll PLL3CFGR3    PLL3MODSSDIS    LL_RCC_PLL3_DisableModulationSpreadSpectrum
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL3_DisableModulationSpreadSpectrum(void)
{
  SET_BIT(RCC->PLL3CFGR3, RCC_PLL3CFGR3_PLL3MODSSDIS);
}

/**
  * @brief  Check if PLL3 modulation spread-spectrum and inactive fractional divide is enabled
  * @rmtoll PLL3CFGR3    PLL3MODSSDIS    LL_RCC_PLL3_IsEnabledModulationSpreadSpectrum
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL3_IsEnabledModulationSpreadSpectrum(void)
{
  return ((READ_BIT(RCC->PLL3CFGR3, RCC_PLL3CFGR3_PLL3MODSSDIS) == 0UL) ? 1UL : 0UL);
}

/**
  * @brief  Enable PLL3 fractional divide and modulation spread-spectrum
  * @note   This API shall be called only when PLL3 is disabled.
  * @rmtoll PLL3CFGR3    PLL3MODDSEN     LL_RCC_PLL3_EnableFractionalModulationSpreadSpectrum
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL3_EnableFractionalModulationSpreadSpectrum(void)
{
  SET_BIT(RCC->PLL3CFGR3, RCC_PLL3CFGR3_PLL3MODDSEN);
}

/**
  * @brief  Disable PLL3 fractional divide and modulation spread-spectrum
  * @note   This API shall be called only when PLL3 is disabled.
  * @rmtoll PLL3CFGR3    PLL3MODDSEN     LL_RCC_PLL3_DisableFractionalModulationSpreadSpectrum
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL3_DisableFractionalModulationSpreadSpectrum(void)
{
  CLEAR_BIT(RCC->PLL3CFGR3, RCC_PLL3CFGR3_PLL3MODDSEN);
}

/**
  * @brief  Check if PLL3 fractional divide and modulation spread-spectrum is enabled
  * @rmtoll PLL3CFGR3    PLL3MODDSEN     LL_RCC_PLL3_IsEnabledFractionalModulationSpreadSpectrum
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL3_IsEnabledFractionalModulationSpreadSpectrum(void)
{
  return ((READ_BIT(RCC->PLL3CFGR3, RCC_PLL3CFGR3_PLL3MODDSEN) == RCC_PLL3CFGR3_PLL3MODDSEN) ? 1UL : 0UL);
}

/**
  * @brief  Set PLL3 DIVN Coefficient
  * @note   This API shall be called only when PLL3 is disabled.
  * @rmtoll PLL3CFGR1    PLL3DIVN        LL_RCC_PLL3_SetN
  * @param  N  In integer mode, N parameter can be a value between 16 (0x10) and 2500 (0x9C4).
  *         In fractional mode, N parameter can be a value between 20 (0x14) and 500 (0x1F4).
  */
__STATIC_INLINE void LL_RCC_PLL3_SetN(uint32_t N)
{
  MODIFY_REG(RCC->PLL3CFGR1, RCC_PLL3CFGR1_PLL3DIVN, N << RCC_PLL3CFGR1_PLL3DIVN_Pos);
}

/**
  * @brief  Get PLL3 DIVN Coefficient
  * @rmtoll PLL3CFGR1    PLL3DIVN        LL_RCC_PLL3_GetN
  * @retval In integer mode, a value between 16 (0x10) and 2500 (0x9C4).
  *         In fractional mode, a value between 20 (0x14) and 500 (0x1F4).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL3_GetN(void)
{
  return (uint32_t)((READ_BIT(RCC->PLL3CFGR1, RCC_PLL3CFGR1_PLL3DIVN) >> RCC_PLL3CFGR1_PLL3DIVN_Pos));
}

/**
  * @brief  Set PLL3 DIVM Coefficient
  * @note   This API shall be called only when PLL3 is disabled.
  * @rmtoll PLL3CFGR1    PLL3DIVM        LL_RCC_PLL3_SetM
  * @param  M parameter can be a value between 1 and 63
  */
__STATIC_INLINE void LL_RCC_PLL3_SetM(uint32_t M)
{
  MODIFY_REG(RCC->PLL3CFGR1, RCC_PLL3CFGR1_PLL3DIVM, M << RCC_PLL3CFGR1_PLL3DIVM_Pos);
}

/**
  * @brief  Get PLL3 DIVM Coefficient
  * @rmtoll PLL3CFGR1    PLL3DIVM        LL_RCC_PLL3_GetM
  * @retval A value between 1 and 63
  */
__STATIC_INLINE uint32_t LL_RCC_PLL3_GetM(void)
{
  return (uint32_t)(READ_BIT(RCC->PLL3CFGR1, RCC_PLL3CFGR1_PLL3DIVM) >> RCC_PLL3CFGR1_PLL3DIVM_Pos);
}

/**
  * @brief  Set PLL3 PDIV1 Coefficient
  * @note   This API shall be called only when PLL3 is disabled.
  * @rmtoll PLL3CFGR3    PLL3PDIV1       LL_RCC_PLL3_SetP1
  * @param  P1 parameter can be a value between 1 and 7 when PLL3 is enabled, 0 when disabled
  */
__STATIC_INLINE void LL_RCC_PLL3_SetP1(uint32_t P1)
{
  MODIFY_REG(RCC->PLL3CFGR3, RCC_PLL3CFGR3_PLL3PDIV1, P1 << RCC_PLL3CFGR3_PLL3PDIV1_Pos);
}

/**
  * @brief  Get PLL3 PDIV2 Coefficient
  * @rmtoll PLL3CFGR3    PLL3PDIV1       LL_RCC_PLL3_GetP1
  * @retval A value between 1 and 7
  */
__STATIC_INLINE uint32_t LL_RCC_PLL3_GetP1(void)
{
  return (uint32_t)(READ_BIT(RCC->PLL3CFGR3, RCC_PLL3CFGR3_PLL3PDIV1) >> RCC_PLL3CFGR3_PLL3PDIV1_Pos);
}

/**
  * @brief  Set PLL3 PDIV2 Coefficient
  * @note   This API shall be called only when PLL3 is disabled.
  * @rmtoll PLL3CFGR3    PLL3PDIV2       LL_RCC_PLL3_SetP2
  * @param  P2 parameter can be a value between 0 and 7
  */
__STATIC_INLINE void LL_RCC_PLL3_SetP2(uint32_t P2)
{
  MODIFY_REG(RCC->PLL3CFGR3, RCC_PLL3CFGR3_PLL3PDIV2, P2 << RCC_PLL3CFGR3_PLL3PDIV2_Pos);
}

/**
  * @brief  Get PLL3 PDIV2 Coefficient
  * @rmtoll PLL3CFGR3    PLL3PDIV2       LL_RCC_PLL3_GetP2
  * @retval A value between 1 and 7
  */
__STATIC_INLINE uint32_t LL_RCC_PLL3_GetP2(void)
{
  return (uint32_t)(READ_BIT(RCC->PLL3CFGR3, RCC_PLL3CFGR3_PLL3PDIV2) >> RCC_PLL3CFGR3_PLL3PDIV2_Pos);
}

/**
  * @brief  Enable PLL3P
  * @note   This API shall be called only when PLL3 is disabled.
  * @rmtoll PLL3CFGR3           PLL3PDIVEN         LL_RCC_PLL3P_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL3P_Enable(void)
{
  SET_BIT(RCC->PLL3CFGR3, RCC_PLL3CFGR3_PLL3PDIVEN);
}

/**
  * @brief  Disable PLL3P
  * @note   This API shall be called only when PLL3 is disabled.
  * @rmtoll PLL3CFGR3           PLL3PDIVEN         LL_RCC_PLL3P_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL3P_Disable(void)
{
  CLEAR_BIT(RCC->PLL3CFGR3, RCC_PLL3CFGR3_PLL3PDIVEN);
}

/**
  * @brief  Check if PLL3 P is enabled
  * @rmtoll PLL3CFGR3           PLL3PDIVEN         LL_RCC_PLL3P_IsEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL3P_IsEnabled(void)
{
  return ((READ_BIT(RCC->PLL3CFGR3, RCC_PLL3CFGR3_PLL3PDIVEN) == RCC_PLL3CFGR3_PLL3PDIVEN) ? 1UL : 0UL);
}

/**
  * @brief  Set PLL3 FRACN Coefficient
  * @note   This API shall be called only when PLL3 is disabled.
  * @rmtoll PLL3CFGR2        PLL3DIVNFRAC    LL_RCC_PLL3_SetFRACN
  * @param  FRACN parameter can be a value between 0 and 2^24 (0xFFFFFF)
  */
__STATIC_INLINE void LL_RCC_PLL3_SetFRACN(uint32_t FRACN)
{
  MODIFY_REG(RCC->PLL3CFGR2, RCC_PLL3CFGR2_PLL3DIVNFRAC, FRACN << RCC_PLL3CFGR2_PLL3DIVNFRAC_Pos);
}

/**
  * @brief  Get PLL3 FRACN Coefficient
  * @rmtoll PLL3CFGR2        PLL3DIVNFRAC    LL_RCC_PLL3_GetFRACN
  * @retval A value between 0 and 2^24 (0xFFFFFF)
  */
__STATIC_INLINE uint32_t LL_RCC_PLL3_GetFRACN(void)
{
  return (uint32_t)(READ_BIT(RCC->PLL3CFGR2, RCC_PLL3CFGR2_PLL3DIVNFRAC) >> RCC_PLL3CFGR2_PLL3DIVNFRAC_Pos);
}

/**
  * @brief  Set the oscillator used as PLL clock source.
  * @note   PLL4SEL can be written only when All PLLs are disabled.
  * @rmtoll PLL4CFGR1      PLL4SEL        LL_RCC_PLL4_SetSource
  * @param  PLLSource parameter can be one of the following values:
  *         @arg @ref LL_RCC_PLLSOURCE_HSI
  *         @arg @ref LL_RCC_PLLSOURCE_MSI
  *         @arg @ref LL_RCC_PLLSOURCE_HSE
  *         @arg @ref LL_RCC_PLLSOURCE_I2S_CKIN
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL4_SetSource(uint32_t PLLSource)
{
  MODIFY_REG(RCC->PLL4CFGR1, RCC_PLL4CFGR1_PLL4SEL, PLLSource);
}

/**
  * @brief  Get the oscillator used as PLL clock source.
  * @rmtoll PLL4CFGR1      PLL4SEL        LL_RCC_PLL4_GetSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_PLLSOURCE_HSI
  *         @arg @ref LL_RCC_PLLSOURCE_MSI
  *         @arg @ref LL_RCC_PLLSOURCE_HSE
  *         @arg @ref LL_RCC_PLLSOURCE_I2S_CKIN
  */
__STATIC_INLINE uint32_t LL_RCC_PLL4_GetSource(void)
{
  return (uint32_t)(READ_BIT(RCC->PLL4CFGR1, RCC_PLL4CFGR1_PLL4SEL));
}

/**
  * @brief  Enable PLL4
  * @rmtoll CSR          PLL4ONS         LL_RCC_PLL4_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL4_Enable(void)
{
  WRITE_REG(RCC->CSR, RCC_CSR_PLL4ONS);
}

/**
  * @brief  Disable PLL4
  * @note Cannot be disabled if the PLL4 clock is used as the system clock
  * @rmtoll CCR          PLL4ONC         LL_RCC_PLL4_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL4_Disable(void)
{
  WRITE_REG(RCC->CCR, RCC_CCR_PLL4ONC);
}

/**
  * @brief  Check if PLL4 Ready
  * @rmtoll SR           PLL4RDY         LL_RCC_PLL4_IsReady
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL4_IsReady(void)
{
  return ((READ_BIT(RCC->SR, RCC_SR_PLL4RDY) == (RCC_SR_PLL4RDY)) ? 1UL : 0UL);
}

/**
  * @brief  Enable PLL4 bypass (PLL4 output is driven by the PLL4 reference clock)
  * @note   This API shall be called only when PLL4 is disabled.
  * @rmtoll PLL4CFGR1    PLL4BYP         LL_RCC_PLL4_EnableBypass
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL4_EnableBypass(void)
{
  SET_BIT(RCC->PLL4CFGR1, RCC_PLL4CFGR1_PLL4BYP);
}

/**
  * @brief  Disable PLL4 bypass (PLL4 output is driven by the VCO)
  * @note   This API shall be called only when PLL4 is disabled.
  * @rmtoll PLL4CFGR1    PLL4BYP         LL_RCC_PLL4_DisableBypass
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL4_DisableBypass(void)
{
  CLEAR_BIT(RCC->PLL4CFGR1, RCC_PLL4CFGR1_PLL4BYP);
}

/**
  * @brief  Check if PLL4 bypass is enabled
  * @rmtoll PLL4CFGR1    PLL4BYP         LL_RCC_PLL4_IsEnabledBypass
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL4_IsEnabledBypass(void)
{
  return ((READ_BIT(RCC->PLL4CFGR1, RCC_PLL4CFGR1_PLL4BYP) == RCC_PLL4CFGR1_PLL4BYP) ? 1UL : 0UL);
}

/**
  * @brief  Assert PLL4 modulation spread-spectrum reset
  * @note   This API shall be called only when PLL4 is disabled.
  * @rmtoll PLL4CFGR3    PLL4MODSSRST    LL_RCC_PLL4_AssertModulationSpreadSpectrumReset
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL4_AssertModulationSpreadSpectrumReset(void)
{
  SET_BIT(RCC->PLL4CFGR3, RCC_PLL4CFGR3_PLL4MODSSRST);
}

/**
  * @brief  Release PLL4 modulation spread-spectrum reset
  * @note   This API shall be called only when PLL4 is disabled.
  * @rmtoll PLL4CFGR3    PLL4MODSSRST    LL_RCC_PLL4_ReleaseModulationSpreadSpectrumReset
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL4_ReleaseModulationSpreadSpectrumReset(void)
{
  CLEAR_BIT(RCC->PLL4CFGR3, RCC_PLL4CFGR3_PLL4MODSSRST);
}

/**
  * @brief  Enable PLL4 noise canceling DAC in fractional mode
  * @note   This API shall be called only when PLL4 is disabled.
  * @rmtoll PLL4CFGR3    PLL4DACEN       LL_RCC_PLL4_EnableDAC
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL4_EnableDAC(void)
{
  SET_BIT(RCC->PLL4CFGR3, RCC_PLL4CFGR3_PLL4DACEN);
}

/**
  * @brief  Disable PLL4 noise canceling DAC in fractional mode
  * @note   This API shall be called only when PLL4 is disabled.
  * @rmtoll PLL4CFGR3    PLL4DACEN     LL_RCC_PLL4_DisableDAC
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL4_DisableDAC(void)
{
  CLEAR_BIT(RCC->PLL4CFGR3, RCC_PLL4CFGR3_PLL4DACEN);
}

/**
  * @brief  Check if PLL4 noise canceling DAC in fractional mode is enabled
  * @rmtoll PLL4CFGR3    PLL4DACEN     LL_RCC_PLL4_IsEnabledDAC
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL4_IsEnabledDAC(void)
{
  return ((READ_BIT(RCC->PLL4CFGR3, RCC_PLL4CFGR3_PLL4DACEN) == RCC_PLL4CFGR3_PLL4DACEN) ? 1UL : 0UL);
}

/**
  * @brief  Enable PLL4 modulation spread-spectrum with fractional divide inactive
  * @note   This API shall be called only when PLL4 is disabled.
  * @rmtoll PLL4CFGR3    PLL4MODSSDIS    LL_RCC_PLL4_EnableModulationSpreadSpectrum
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL4_EnableModulationSpreadSpectrum(void)
{
  CLEAR_BIT(RCC->PLL4CFGR3, RCC_PLL4CFGR3_PLL4MODSSDIS);
}

/**
  * @brief  Disable PLL4 modulation spread-spectrum but active fractional divide
  * @note   This API shall be called only when PLL4 is disabled.
  * @rmtoll PLL4CFGR3    PLL4MODSSDIS    LL_RCC_PLL4_DisableModulationSpreadSpectrum
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL4_DisableModulationSpreadSpectrum(void)
{
  SET_BIT(RCC->PLL4CFGR3, RCC_PLL4CFGR3_PLL4MODSSDIS);
}

/**
  * @brief  Check if PLL4 modulation spread-spectrum and inactive fractional divide is enabled
  * @rmtoll PLL4CFGR3    PLL4MODSSDIS    LL_RCC_PLL4_IsEnabledModulationSpreadSpectrum
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL4_IsEnabledModulationSpreadSpectrum(void)
{
  return ((READ_BIT(RCC->PLL4CFGR3, RCC_PLL4CFGR3_PLL4MODSSDIS) == 0UL) ? 1UL : 0UL);
}

/**
  * @brief  Enable PLL4 fractional divide and modulation spread-spectrum
  * @note   This API shall be called only when PLL4 is disabled.
  * @rmtoll PLL4CFGR3    PLL4MODDSEN     LL_RCC_PLL4_EnableFractionalModulationSpreadSpectrum
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL4_EnableFractionalModulationSpreadSpectrum(void)
{
  SET_BIT(RCC->PLL4CFGR3, RCC_PLL4CFGR3_PLL4MODDSEN);
}

/**
  * @brief  Disable PLL4 fractional divide and modulation spread-spectrum
  * @note   This API shall be called only when PLL4 is disabled.
  * @rmtoll PLL4CFGR3    PLL4MODDSEN     LL_RCC_PLL4_DisableFractionalModulationSpreadSpectrum
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL4_DisableFractionalModulationSpreadSpectrum(void)
{
  CLEAR_BIT(RCC->PLL4CFGR3, RCC_PLL4CFGR3_PLL4MODDSEN);
}

/**
  * @brief  Check if PLL4 fractional divide and modulation spread-spectrum is enabled
  * @rmtoll PLL4CFGR3    PLL4MODDSEN     LL_RCC_PLL4_IsEnabledFractionalModulationSpreadSpectrum
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL4_IsEnabledFractionalModulationSpreadSpectrum(void)
{
  return ((READ_BIT(RCC->PLL4CFGR3, RCC_PLL4CFGR3_PLL4MODDSEN) == RCC_PLL4CFGR3_PLL4MODDSEN) ? 1UL : 0UL);
}

/**
  * @brief  Set PLL4 DIVN Coefficient
  * @note   This API shall be called only when PLL4 is disabled.
  * @rmtoll PLL4CFGR1    PLL4DIVN        LL_RCC_PLL4_SetN
  * @param  N  In integer mode, N parameter can be a value between 16 (0x10) and 2500 (0x9C4).
  *         In fractional mode, N parameter can be a value between 20 (0x14) and 500 (0x1F4).
  */
__STATIC_INLINE void LL_RCC_PLL4_SetN(uint32_t N)
{
  MODIFY_REG(RCC->PLL4CFGR1, RCC_PLL4CFGR1_PLL4DIVN, N << RCC_PLL4CFGR1_PLL4DIVN_Pos);
}

/**
  * @brief  Get PLL4 DIVN Coefficient
  * @rmtoll PLL4CFGR1    PLL4DIVN        LL_RCC_PLL4_GetN
  * @retval In integer mode, a value between 16 (0x10) and 2500 (0x9C4).
  *         In fractional mode, a value between 20 (0x14) and 500 (0x1F4).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL4_GetN(void)
{
  return (uint32_t)((READ_BIT(RCC->PLL4CFGR1, RCC_PLL4CFGR1_PLL4DIVN) >> RCC_PLL4CFGR1_PLL4DIVN_Pos));
}

/**
  * @brief  Set PLL4 DIVM Coefficient
  * @note   This API shall be called only when PLL4 is disabled.
  * @rmtoll PLL4CFGR1    PLL4DIVM        LL_RCC_PLL4_SetM
  * @param  M parameter can be a value between 1 and 63
  */
__STATIC_INLINE void LL_RCC_PLL4_SetM(uint32_t M)
{
  MODIFY_REG(RCC->PLL4CFGR1, RCC_PLL4CFGR1_PLL4DIVM, M << RCC_PLL4CFGR1_PLL4DIVM_Pos);
}

/**
  * @brief  Get PLL4 DIVM Coefficient
  * @rmtoll PLL4CFGR1    PLL4DIVM        LL_RCC_PLL4_GetM
  * @retval A value between 1 and 63
  */
__STATIC_INLINE uint32_t LL_RCC_PLL4_GetM(void)
{
  return (uint32_t)(READ_BIT(RCC->PLL4CFGR1, RCC_PLL4CFGR1_PLL4DIVM) >> RCC_PLL4CFGR1_PLL4DIVM_Pos);
}

/**
  * @brief  Set PLL4 PDIV1 Coefficient
  * @note   This API shall be called only when PLL4 is disabled.
  * @rmtoll PLL4CFGR3    PLL4PDIV1       LL_RCC_PLL4_SetP1
  * @param  P1 parameter can be a value between 1 and 7 when PLL4 is enabled, 0 when disabled
  */
__STATIC_INLINE void LL_RCC_PLL4_SetP1(uint32_t P1)
{
  MODIFY_REG(RCC->PLL4CFGR3, RCC_PLL4CFGR3_PLL4PDIV1, P1 << RCC_PLL4CFGR3_PLL4PDIV1_Pos);
}

/**
  * @brief  Get PLL4 PDIV2 Coefficient
  * @rmtoll PLL4CFGR3    PLL4PDIV1       LL_RCC_PLL4_GetP1
  * @retval A value between 0 and 7
  */
__STATIC_INLINE uint32_t LL_RCC_PLL4_GetP1(void)
{
  return (uint32_t)(READ_BIT(RCC->PLL4CFGR3, RCC_PLL4CFGR3_PLL4PDIV1) >> RCC_PLL4CFGR3_PLL4PDIV1_Pos);
}

/**
  * @brief  Set PLL4 PDIV2 Coefficient
  * @note   This API shall be called only when PLL4 is disabled.
  * @rmtoll PLL4CFGR3    PLL4PDIV2       LL_RCC_PLL4_SetP2
  * @param  P2 parameter can be a value between 1 and 7
  */
__STATIC_INLINE void LL_RCC_PLL4_SetP2(uint32_t P2)
{
  MODIFY_REG(RCC->PLL4CFGR3, RCC_PLL4CFGR3_PLL4PDIV2, P2 << RCC_PLL4CFGR3_PLL4PDIV2_Pos);
}

/**
  * @brief  Get PLL4 PDIV2 Coefficient
  * @rmtoll PLL4CFGR3    PLL4PDIV2       LL_RCC_PLL4_GetP2
  * @retval A value between 1 and 7
  */
__STATIC_INLINE uint32_t LL_RCC_PLL4_GetP2(void)
{
  return (uint32_t)(READ_BIT(RCC->PLL4CFGR3, RCC_PLL4CFGR3_PLL4PDIV2) >> RCC_PLL4CFGR3_PLL4PDIV2_Pos);
}

/**
  * @brief  Enable PLL4P
  * @note   This API shall be called only when PLL4 is disabled.
  * @rmtoll PLL4CFGR3           PLL4PDIVEN         LL_RCC_PLL4P_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL4P_Enable(void)
{
  SET_BIT(RCC->PLL4CFGR3, RCC_PLL4CFGR3_PLL4PDIVEN);
}

/**
  * @brief  Disable PLL4P
  * @note   This API shall be called only when PLL4 is disabled.
  * @rmtoll PLL4CFGR3           PLL4PDIVEN         LL_RCC_PLL4P_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_PLL4P_Disable(void)
{
  CLEAR_BIT(RCC->PLL4CFGR3, RCC_PLL4CFGR3_PLL4PDIVEN);
}

/**
  * @brief  Check if PLL4 P is enabled
  * @rmtoll PLL4CFGR3           PLL4PDIVEN         LL_RCC_PLL4P_IsEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_PLL4P_IsEnabled(void)
{
  return ((READ_BIT(RCC->PLL4CFGR3, RCC_PLL4CFGR3_PLL4PDIVEN) == RCC_PLL4CFGR3_PLL4PDIVEN) ? 1UL : 0UL);
}

/**
  * @brief  Set PLL4 FRACN Coefficient
  * @note   This API shall be called only when PLL4 is disabled.
  * @rmtoll PLL4CFGR2        PLL4DIVNFRAC    LL_RCC_PLL4_SetFRACN
  * @param  FRACN parameter can be a value between 0 and 2^24 (0xFFFFFF)
  */
__STATIC_INLINE void LL_RCC_PLL4_SetFRACN(uint32_t FRACN)
{
  MODIFY_REG(RCC->PLL4CFGR2, RCC_PLL4CFGR2_PLL4DIVNFRAC, FRACN << RCC_PLL4CFGR2_PLL4DIVNFRAC_Pos);
}

/**
  * @brief  Get PLL4 FRACN Coefficient
  * @rmtoll PLL4CFGR2        PLL4DIVNFRAC    LL_RCC_PLL4_GetFRACN
  * @retval A value between 0 and 2^24 (0xFFFFFF)
  */
__STATIC_INLINE uint32_t LL_RCC_PLL4_GetFRACN(void)
{
  return (uint32_t)(READ_BIT(RCC->PLL4CFGR2, RCC_PLL4CFGR2_PLL4DIVNFRAC) >> RCC_PLL4CFGR2_PLL4DIVNFRAC_Pos);
}

/**
  * @brief  Enable IC1
  * @rmtoll DIVENSR      IC1ENS        LL_RCC_IC1_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC1_Enable(void)
{
  WRITE_REG(RCC->DIVENSR, RCC_DIVENSR_IC1ENS);
}

/**
  * @brief  Disable IC1
  * @rmtoll DIVENCR      IC1ENC        LL_RCC_IC1_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC1_Disable(void)
{
  WRITE_REG(RCC->DIVENCR, RCC_DIVENCR_IC1ENC);
}

/**
  * @brief  Check if IC1 is enabled
  * @rmtoll DIVENR       IC1EN         LL_RCC_IC1_IsEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IC1_IsEnabled(void)
{
  return ((READ_BIT(RCC->DIVENR, RCC_DIVENR_IC1EN) == RCC_DIVENR_IC1EN) ? 1UL : 0UL);
}

/**
  * @brief  Set the PLL source used as IC1 clock source.
  * @rmtoll IC1CFGR      IC1SEL        LL_RCC_IC1_SetSource
  * @param  Source parameter can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC1_SetSource(uint32_t Source)
{
  MODIFY_REG(RCC->IC1CFGR, RCC_IC1CFGR_IC1SEL, Source);
}

/**
  * @brief  Get the PLL source used as IC1 clock source.
  * @rmtoll IC1CFGR      IC1SEL        LL_RCC_IC1_GetSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  */
__STATIC_INLINE uint32_t LL_RCC_IC1_GetSource(void)
{
  return (uint32_t)(READ_BIT(RCC->IC1CFGR, RCC_IC1CFGR_IC1SEL));
}

/**
  * @brief  Set  divider
  * @rmtoll IC1CFGR      IC1INT        LL_RCC_IC1_SetDivider
  * @param  Divider This parameter can be a value between 1 and 256.
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC1_SetDivider(uint32_t Divider)
{
  MODIFY_REG(RCC->IC1CFGR, RCC_IC1CFGR_IC1INT, (Divider - 1UL) << RCC_IC1CFGR_IC1INT_Pos);
}

/**
  * @brief  Get IC1 divider
  * @rmtoll IC1CFGR      IC1INT        LL_RCC_IC1_GetDivider
  * @retval can be a value between 1 and 256.
  */
__STATIC_INLINE uint32_t LL_RCC_IC1_GetDivider(void)
{
  return ((READ_BIT(RCC->IC1CFGR, RCC_IC1CFGR_IC1INT) >> RCC_IC1CFGR_IC1INT_Pos) + 1UL);
}

/**
  * @brief  Enable IC2
  * @rmtoll DIVENSR       IC2ENS        LL_RCC_IC2_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC2_Enable(void)
{
  WRITE_REG(RCC->DIVENSR, RCC_DIVENSR_IC2ENS);
}

/**
  * @brief  Disable IC2
  * @rmtoll DIVENCR       IC2ENC        LL_RCC_IC2_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC2_Disable(void)
{
  WRITE_REG(RCC->DIVENCR, RCC_DIVENCR_IC2ENC);
}

/**
  * @brief  Check if IC2 is enabled
  * @rmtoll DIVENR       IC2EN         LL_RCC_IC2_IsEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IC2_IsEnabled(void)
{
  return ((READ_BIT(RCC->DIVENR, RCC_DIVENR_IC2EN) == RCC_DIVENR_IC2EN) ? 1UL : 0UL);
}

/**
  * @brief  Set the PLL source used as IC2 clock source.
  * @rmtoll IC2CFGR      IC2SEL        LL_RCC_IC2_SetSource
  * @param  Source parameter can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC2_SetSource(uint32_t Source)
{
  MODIFY_REG(RCC->IC2CFGR, RCC_IC2CFGR_IC2SEL, Source);
}

/**
  * @brief  Get the PLL source used as IC2 clock source.
  * @rmtoll IC2CFGR      IC2SEL        LL_RCC_IC2_GetSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  */
__STATIC_INLINE uint32_t LL_RCC_IC2_GetSource(void)
{
  return (uint32_t)(READ_BIT(RCC->IC2CFGR, RCC_IC2CFGR_IC2SEL));
}

/**
  * @brief  Set  divider
  * @rmtoll IC2CFGR      IC2INT        LL_RCC_IC2_SetDivider
  * @param  Divider This parameter can be a value between 1 and 256.
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC2_SetDivider(uint32_t Divider)
{
  MODIFY_REG(RCC->IC2CFGR, RCC_IC2CFGR_IC2INT, (Divider - 1UL) << RCC_IC2CFGR_IC2INT_Pos);
}

/**
  * @brief  Get IC2 divider
  * @rmtoll IC2CFGR      IC2INT        LL_RCC_IC2_GetDivider
  * @retval can be a value between 1 and 256.
  */
__STATIC_INLINE uint32_t LL_RCC_IC2_GetDivider(void)
{
  return ((READ_BIT(RCC->IC2CFGR, RCC_IC2CFGR_IC2INT) >> RCC_IC2CFGR_IC2INT_Pos) + 1UL);
}

/**
  * @brief  Enable IC3
  * @rmtoll DIVENSR       IC3ENS        LL_RCC_IC3_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC3_Enable(void)
{
  WRITE_REG(RCC->DIVENSR, RCC_DIVENSR_IC3ENS);
}

/**
  * @brief  Disable IC3
  * @rmtoll DIVENCR       IC3ENC        LL_RCC_IC3_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC3_Disable(void)
{
  WRITE_REG(RCC->DIVENCR, RCC_DIVENCR_IC3ENC);
}

/**
  * @brief  Check if IC3 is enabled
  * @rmtoll DIVENR       IC3EN         LL_RCC_IC3_IsEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IC3_IsEnabled(void)
{
  return ((READ_BIT(RCC->DIVENR, RCC_DIVENR_IC3EN) == RCC_DIVENR_IC3EN) ? 1UL : 0UL);
}

/**
  * @brief  Set the PLL source used as IC3 clock source.
  * @rmtoll IC3CFGR      IC3SEL        LL_RCC_IC3_SetSource
  * @param  Source parameter can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC3_SetSource(uint32_t Source)
{
  MODIFY_REG(RCC->IC3CFGR, RCC_IC3CFGR_IC3SEL, Source);
}

/**
  * @brief  Get the PLL source used as IC3 clock source.
  * @rmtoll IC3CFGR      IC3SEL        LL_RCC_IC3_GetSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  */
__STATIC_INLINE uint32_t LL_RCC_IC3_GetSource(void)
{
  return (uint32_t)(READ_BIT(RCC->IC3CFGR, RCC_IC3CFGR_IC3SEL));
}

/**
  * @brief  Set  divider
  * @rmtoll IC3CFGR      IC3INT        LL_RCC_IC3_SetDivider
  * @param  Divider This parameter can be a value between 1 and 256.
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC3_SetDivider(uint32_t Divider)
{
  MODIFY_REG(RCC->IC3CFGR, RCC_IC3CFGR_IC3INT, (Divider - 1UL) << RCC_IC3CFGR_IC3INT_Pos);
}

/**
  * @brief  Get IC3 divider
  * @rmtoll IC3CFGR      IC3INT        LL_RCC_IC3_GetDivider
  * @retval can be a value between 1 and 256.
  */
__STATIC_INLINE uint32_t LL_RCC_IC3_GetDivider(void)
{
  return ((READ_BIT(RCC->IC3CFGR, RCC_IC3CFGR_IC3INT) >> RCC_IC3CFGR_IC3INT_Pos) + 1UL);
}

/**
  * @brief  Enable IC4
  * @rmtoll DIVENSR       IC4ENS        LL_RCC_IC4_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC4_Enable(void)
{
  WRITE_REG(RCC->DIVENSR, RCC_DIVENSR_IC4ENS);
}

/**
  * @brief  Disable IC4
  * @rmtoll DIVENCR       IC4ENC        LL_RCC_IC4_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC4_Disable(void)
{
  WRITE_REG(RCC->DIVENCR, RCC_DIVENCR_IC4ENC);
}

/**
  * @brief  Check if IC4 is enabled
  * @rmtoll DIVENR       IC4EN         LL_RCC_IC4_IsEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IC4_IsEnabled(void)
{
  return ((READ_BIT(RCC->DIVENR, RCC_DIVENR_IC4EN) == RCC_DIVENR_IC4EN) ? 1UL : 0UL);
}

/**
  * @brief  Set the PLL source used as IC4 clock source.
  * @rmtoll IC4CFGR      IC4SEL        LL_RCC_IC4_SetSource
  * @param  Source parameter can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC4_SetSource(uint32_t Source)
{
  MODIFY_REG(RCC->IC4CFGR, RCC_IC4CFGR_IC4SEL, Source);
}

/**
  * @brief  Get the PLL source used as IC4 clock source.
  * @rmtoll IC4CFGR      IC4SEL        LL_RCC_IC4_GetSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  */
__STATIC_INLINE uint32_t LL_RCC_IC4_GetSource(void)
{
  return (uint32_t)(READ_BIT(RCC->IC4CFGR, RCC_IC4CFGR_IC4SEL));
}

/**
  * @brief  Set  divider
  * @rmtoll IC4CFGR      IC4INT        LL_RCC_IC4_SetDivider
  * @param  Divider This parameter can be a value between 1 and 256.
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC4_SetDivider(uint32_t Divider)
{
  MODIFY_REG(RCC->IC4CFGR, RCC_IC4CFGR_IC4INT, (Divider - 1UL) << RCC_IC4CFGR_IC4INT_Pos);
}

/**
  * @brief  Get IC4 divider
  * @rmtoll IC4CFGR      IC4INT        LL_RCC_IC4_GetDivider
  * @retval can be a value between 1 and 256.
  */
__STATIC_INLINE uint32_t LL_RCC_IC4_GetDivider(void)
{
  return ((READ_BIT(RCC->IC4CFGR, RCC_IC4CFGR_IC4INT) >> RCC_IC4CFGR_IC4INT_Pos) + 1UL);
}

/**
  * @brief  Enable IC5
  * @rmtoll DIVENSR       IC5ENS        LL_RCC_IC5_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC5_Enable(void)
{
  WRITE_REG(RCC->DIVENSR, RCC_DIVENSR_IC5ENS);
}

/**
  * @brief  Disable IC5
  * @rmtoll DIVENCR       IC5ENC        LL_RCC_IC5_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC5_Disable(void)
{
  WRITE_REG(RCC->DIVENCR, RCC_DIVENCR_IC5ENC);
}

/**
  * @brief  Check if IC5 is enabled
  * @rmtoll DIVENR       IC5EN         LL_RCC_IC5_IsEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IC5_IsEnabled(void)
{
  return ((READ_BIT(RCC->DIVENR, RCC_DIVENR_IC5EN) == RCC_DIVENR_IC5EN) ? 1UL : 0UL);
}

/**
  * @brief  Set the PLL source used as IC5 clock source.
  * @rmtoll IC5CFGR      IC5SEL        LL_RCC_IC5_SetSource
  * @param  Source parameter can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC5_SetSource(uint32_t Source)
{
  MODIFY_REG(RCC->IC5CFGR, RCC_IC5CFGR_IC5SEL, Source);
}

/**
  * @brief  Get the PLL source used as IC5 clock source.
  * @rmtoll IC5CFGR      IC5SEL        LL_RCC_IC5_GetSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  */
__STATIC_INLINE uint32_t LL_RCC_IC5_GetSource(void)
{
  return (uint32_t)(READ_BIT(RCC->IC5CFGR, RCC_IC5CFGR_IC5SEL));
}

/**
  * @brief  Set  divider
  * @rmtoll IC5CFGR      IC5INT        LL_RCC_IC5_SetDivider
  * @param  Divider This parameter can be a value between 1 and 256.
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC5_SetDivider(uint32_t Divider)
{
  MODIFY_REG(RCC->IC5CFGR, RCC_IC5CFGR_IC5INT, (Divider - 1UL) << RCC_IC5CFGR_IC5INT_Pos);
}

/**
  * @brief  Get IC5 divider
  * @rmtoll IC5CFGR      IC5INT        LL_RCC_IC5_GetDivider
  * @retval can be a value between 1 and 256.
  */
__STATIC_INLINE uint32_t LL_RCC_IC5_GetDivider(void)
{
  return ((READ_BIT(RCC->IC5CFGR, RCC_IC5CFGR_IC5INT) >> RCC_IC5CFGR_IC5INT_Pos) + 1UL);
}

/**
  * @brief  Enable IC6
  * @rmtoll DIVENSR       IC6ENS        LL_RCC_IC6_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC6_Enable(void)
{
  WRITE_REG(RCC->DIVENSR, RCC_DIVENSR_IC6ENS);
}

/**
  * @brief  Disable IC6
  * @rmtoll DIVENCR       IC6ENC        LL_RCC_IC6_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC6_Disable(void)
{
  WRITE_REG(RCC->DIVENCR, RCC_DIVENCR_IC6ENC);
}

/**
  * @brief  Check if IC6 is enabled
  * @rmtoll DIVENR       IC6EN         LL_RCC_IC6_IsEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IC6_IsEnabled(void)
{
  return ((READ_BIT(RCC->DIVENR, RCC_DIVENR_IC6EN) == RCC_DIVENR_IC6EN) ? 1UL : 0UL);
}

/**
  * @brief  Set the PLL source used as IC6 clock source.
  * @rmtoll IC6CFGR      IC6SEL        LL_RCC_IC6_SetSource
  * @param  Source parameter can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC6_SetSource(uint32_t Source)
{
  MODIFY_REG(RCC->IC6CFGR, RCC_IC6CFGR_IC6SEL, Source);
}

/**
  * @brief  Get the PLL source used as IC6 clock source.
  * @rmtoll IC6CFGR      IC6SEL        LL_RCC_IC6_GetSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  */
__STATIC_INLINE uint32_t LL_RCC_IC6_GetSource(void)
{
  return (uint32_t)(READ_BIT(RCC->IC6CFGR, RCC_IC6CFGR_IC6SEL));
}

/**
  * @brief  Set  divider
  * @rmtoll IC6CFGR      IC6INT        LL_RCC_IC6_SetDivider
  * @param  Divider This parameter can be a value between 1 and 256.
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC6_SetDivider(uint32_t Divider)
{
  MODIFY_REG(RCC->IC6CFGR, RCC_IC6CFGR_IC6INT, (Divider - 1UL) << RCC_IC6CFGR_IC6INT_Pos);
}

/**
  * @brief  Get IC6 divider
  * @rmtoll IC6CFGR      IC6INT        LL_RCC_IC6_GetDivider
  * @retval can be a value between 1 and 256.
  */
__STATIC_INLINE uint32_t LL_RCC_IC6_GetDivider(void)
{
  return ((READ_BIT(RCC->IC6CFGR, RCC_IC6CFGR_IC6INT) >> RCC_IC6CFGR_IC6INT_Pos) + 1UL);
}

/**
  * @brief  Enable IC7
  * @rmtoll DIVENSR       IC7ENS        LL_RCC_IC7_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC7_Enable(void)
{
  WRITE_REG(RCC->DIVENSR, RCC_DIVENSR_IC7ENS);
}

/**
  * @brief  Disable IC7
  * @rmtoll DIVENCR       IC7ENC        LL_RCC_IC7_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC7_Disable(void)
{
  WRITE_REG(RCC->DIVENCR, RCC_DIVENCR_IC7ENC);
}

/**
  * @brief  Check if IC7 is enabled
  * @rmtoll DIVENR       IC7EN         LL_RCC_IC7_IsEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IC7_IsEnabled(void)
{
  return ((READ_BIT(RCC->DIVENR, RCC_DIVENR_IC7EN) == RCC_DIVENR_IC7EN) ? 1UL : 0UL);
}

/**
  * @brief  Set the PLL source used as IC7 clock source.
  * @rmtoll IC7CFGR      IC7SEL        LL_RCC_IC7_SetSource
  * @param  Source parameter can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC7_SetSource(uint32_t Source)
{
  MODIFY_REG(RCC->IC7CFGR, RCC_IC7CFGR_IC7SEL, Source);
}

/**
  * @brief  Get the PLL source used as IC7 clock source.
  * @rmtoll IC7CFGR      IC7SEL        LL_RCC_IC7_GetSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  */
__STATIC_INLINE uint32_t LL_RCC_IC7_GetSource(void)
{
  return (uint32_t)(READ_BIT(RCC->IC7CFGR, RCC_IC7CFGR_IC7SEL));
}

/**
  * @brief  Set  divider
  * @rmtoll IC7CFGR      IC7INT        LL_RCC_IC7_SetDivider
  * @param  Divider This parameter can be a value between 1 and 256.
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC7_SetDivider(uint32_t Divider)
{
  MODIFY_REG(RCC->IC7CFGR, RCC_IC7CFGR_IC7INT, (Divider - 1UL) << RCC_IC7CFGR_IC7INT_Pos);
}

/**
  * @brief  Get IC7 divider
  * @rmtoll IC7CFGR      IC7INT        LL_RCC_IC7_GetDivider
  * @retval can be a value between 1 and 256.
  */
__STATIC_INLINE uint32_t LL_RCC_IC7_GetDivider(void)
{
  return ((READ_BIT(RCC->IC7CFGR, RCC_IC7CFGR_IC7INT) >> RCC_IC7CFGR_IC7INT_Pos) + 1UL);
}

/**
  * @brief  Enable IC8
  * @rmtoll DIVENSR       IC8ENS        LL_RCC_IC8_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC8_Enable(void)
{
  WRITE_REG(RCC->DIVENSR, RCC_DIVENSR_IC8ENS);
}

/**
  * @brief  Disable IC8
  * @rmtoll DIVENCR       IC8ENC        LL_RCC_IC8_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC8_Disable(void)
{
  WRITE_REG(RCC->DIVENCR, RCC_DIVENCR_IC8ENC);
}

/**
  * @brief  Check if IC8 is enabled
  * @rmtoll DIVENR       IC8EN         LL_RCC_IC8_IsEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IC8_IsEnabled(void)
{
  return ((READ_BIT(RCC->DIVENR, RCC_DIVENR_IC8EN) == RCC_DIVENR_IC8EN) ? 1UL : 0UL);
}

/**
  * @brief  Set the PLL source used as IC8 clock source.
  * @rmtoll IC8CFGR      IC8SEL        LL_RCC_IC8_SetSource
  * @param  Source parameter can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC8_SetSource(uint32_t Source)
{
  MODIFY_REG(RCC->IC8CFGR, RCC_IC8CFGR_IC8SEL, Source);
}

/**
  * @brief  Get the PLL source used as IC8 clock source.
  * @rmtoll IC8CFGR      IC8SEL        LL_RCC_IC8_GetSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  */
__STATIC_INLINE uint32_t LL_RCC_IC8_GetSource(void)
{
  return (uint32_t)(READ_BIT(RCC->IC8CFGR, RCC_IC8CFGR_IC8SEL));
}

/**
  * @brief  Set  divider
  * @rmtoll IC8CFGR      IC8INT        LL_RCC_IC8_SetDivider
  * @param  Divider This parameter can be a value between 1 and 256.
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC8_SetDivider(uint32_t Divider)
{
  MODIFY_REG(RCC->IC8CFGR, RCC_IC8CFGR_IC8INT, (Divider - 1UL) << RCC_IC8CFGR_IC8INT_Pos);
}

/**
  * @brief  Get IC8 divider
  * @rmtoll IC8CFGR      IC8INT        LL_RCC_IC8_GetDivider
  * @retval can be a value between 1 and 256.
  */
__STATIC_INLINE uint32_t LL_RCC_IC8_GetDivider(void)
{
  return ((READ_BIT(RCC->IC8CFGR, RCC_IC8CFGR_IC8INT) >> RCC_IC8CFGR_IC8INT_Pos) + 1UL);
}

/**
  * @brief  Enable IC9
  * @rmtoll DIVENSR       IC9ENS        LL_RCC_IC9_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC9_Enable(void)
{
  WRITE_REG(RCC->DIVENSR, RCC_DIVENSR_IC9ENS);
}

/**
  * @brief  Disable IC9
  * @rmtoll DIVENCR       IC9ENC        LL_RCC_IC9_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC9_Disable(void)
{
  WRITE_REG(RCC->DIVENCR, RCC_DIVENCR_IC9ENC);
}

/**
  * @brief  Check if IC9 is enabled
  * @rmtoll DIVENR       IC9EN         LL_RCC_IC9_IsEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IC9_IsEnabled(void)
{
  return ((READ_BIT(RCC->DIVENR, RCC_DIVENR_IC9EN) == RCC_DIVENR_IC9EN) ? 1UL : 0UL);
}

/**
  * @brief  Set the PLL source used as IC9 clock source.
  * @rmtoll IC9CFGR      IC9SEL        LL_RCC_IC9_SetSource
  * @param  Source parameter can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC9_SetSource(uint32_t Source)
{
  MODIFY_REG(RCC->IC9CFGR, RCC_IC9CFGR_IC9SEL, Source);
}

/**
  * @brief  Get the PLL source used as IC9 clock source.
  * @rmtoll IC9CFGR      IC9SEL        LL_RCC_IC9_GetSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  */
__STATIC_INLINE uint32_t LL_RCC_IC9_GetSource(void)
{
  return (uint32_t)(READ_BIT(RCC->IC9CFGR, RCC_IC9CFGR_IC9SEL));
}

/**
  * @brief  Set  divider
  * @rmtoll IC9CFGR      IC9INT        LL_RCC_IC9_SetDivider
  * @param  Divider This parameter can be a value between 1 and 256.
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC9_SetDivider(uint32_t Divider)
{
  MODIFY_REG(RCC->IC9CFGR, RCC_IC9CFGR_IC9INT, (Divider - 1UL) << RCC_IC9CFGR_IC9INT_Pos);
}

/**
  * @brief  Get IC9 divider
  * @rmtoll IC9CFGR      IC9INT        LL_RCC_IC9_GetDivider
  * @retval can be a value between 1 and 256.
  */
__STATIC_INLINE uint32_t LL_RCC_IC9_GetDivider(void)
{
  return ((READ_BIT(RCC->IC9CFGR, RCC_IC9CFGR_IC9INT) >> RCC_IC9CFGR_IC9INT_Pos) + 1UL);
}

/**
  * @brief  Enable IC10
  * @rmtoll DIVENSR       IC10ENS        LL_RCC_IC10_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC10_Enable(void)
{
  WRITE_REG(RCC->DIVENSR, RCC_DIVENSR_IC10ENS);
}

/**
  * @brief  Disable IC10
  * @rmtoll DIVENCR       IC10ENC        LL_RCC_IC10_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC10_Disable(void)
{
  WRITE_REG(RCC->DIVENCR, RCC_DIVENCR_IC10ENC);
}

/**
  * @brief  Check if IC10 is enabled
  * @rmtoll DIVENR       IC10EN         LL_RCC_IC10_IsEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IC10_IsEnabled(void)
{
  return ((READ_BIT(RCC->DIVENR, RCC_DIVENR_IC10EN) == RCC_DIVENR_IC10EN) ? 1UL : 0UL);
}

/**
  * @brief  Set the PLL source used as IC10 clock source.
  * @rmtoll IC10CFGR      IC10SEL        LL_RCC_IC10_SetSource
  * @param  Source parameter can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC10_SetSource(uint32_t Source)
{
  MODIFY_REG(RCC->IC10CFGR, RCC_IC10CFGR_IC10SEL, Source);
}

/**
  * @brief  Get the PLL source used as IC10 clock source.
  * @rmtoll IC10CFGR      IC10SEL        LL_RCC_IC10_GetSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  */
__STATIC_INLINE uint32_t LL_RCC_IC10_GetSource(void)
{
  return (uint32_t)(READ_BIT(RCC->IC10CFGR, RCC_IC10CFGR_IC10SEL));
}

/**
  * @brief  Set  divider
  * @rmtoll IC10CFGR      IC10INT        LL_RCC_IC10_SetDivider
  * @param  Divider This parameter can be a value between 1 and 256.
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC10_SetDivider(uint32_t Divider)
{
  MODIFY_REG(RCC->IC10CFGR, RCC_IC10CFGR_IC10INT, (Divider - 1UL) << RCC_IC10CFGR_IC10INT_Pos);
}

/**
  * @brief  Get IC10 divider
  * @rmtoll IC10CFGR      IC10INT        LL_RCC_IC10_GetDivider
  * @retval can be a value between 1 and 256.
  */
__STATIC_INLINE uint32_t LL_RCC_IC10_GetDivider(void)
{
  return ((READ_BIT(RCC->IC10CFGR, RCC_IC10CFGR_IC10INT) >> RCC_IC10CFGR_IC10INT_Pos) + 1UL);
}

/**
  * @brief  Enable IC11
  * @rmtoll DIVENSR       IC11ENS        LL_RCC_IC11_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC11_Enable(void)
{
  WRITE_REG(RCC->DIVENSR, RCC_DIVENSR_IC11ENS);
}

/**
  * @brief  Disable IC11
  * @rmtoll DIVENCR       IC11ENC        LL_RCC_IC11_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC11_Disable(void)
{
  WRITE_REG(RCC->DIVENCR, RCC_DIVENCR_IC11ENC);
}

/**
  * @brief  Check if IC11 is enabled
  * @rmtoll DIVENR       IC11EN         LL_RCC_IC11_IsEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IC11_IsEnabled(void)
{
  return ((READ_BIT(RCC->DIVENR, RCC_DIVENR_IC11EN) == RCC_DIVENR_IC11EN) ? 1UL : 0UL);
}

/**
  * @brief  Set the PLL source used as IC11 clock source.
  * @rmtoll IC11CFGR      IC11SEL        LL_RCC_IC11_SetSource
  * @param  Source parameter can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC11_SetSource(uint32_t Source)
{
  MODIFY_REG(RCC->IC11CFGR, RCC_IC11CFGR_IC11SEL, Source);
}

/**
  * @brief  Get the PLL source used as IC11 clock source.
  * @rmtoll IC11CFGR      IC11SEL        LL_RCC_IC11_GetSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  */
__STATIC_INLINE uint32_t LL_RCC_IC11_GetSource(void)
{
  return (uint32_t)(READ_BIT(RCC->IC11CFGR, RCC_IC11CFGR_IC11SEL));
}

/**
  * @brief  Set  divider
  * @rmtoll IC11CFGR      IC11INT        LL_RCC_IC11_SetDivider
  * @param  Divider This parameter can be a value between 1 and 256.
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC11_SetDivider(uint32_t Divider)
{
  MODIFY_REG(RCC->IC11CFGR, RCC_IC11CFGR_IC11INT, (Divider - 1UL) << RCC_IC11CFGR_IC11INT_Pos);
}

/**
  * @brief  Get IC11 divider
  * @rmtoll IC11CFGR      IC11INT        LL_RCC_IC11_GetDivider
  * @retval can be a value between 1 and 256.
  */
__STATIC_INLINE uint32_t LL_RCC_IC11_GetDivider(void)
{
  return ((READ_BIT(RCC->IC11CFGR, RCC_IC11CFGR_IC11INT) >> RCC_IC11CFGR_IC11INT_Pos) + 1UL);
}

/**
  * @brief  Enable IC12
  * @rmtoll DIVENSR       IC12ENS        LL_RCC_IC12_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC12_Enable(void)
{
  WRITE_REG(RCC->DIVENSR, RCC_DIVENSR_IC12ENS);
}

/**
  * @brief  Disable IC12
  * @rmtoll DIVENCR       IC12ENC        LL_RCC_IC12_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC12_Disable(void)
{
  WRITE_REG(RCC->DIVENCR, RCC_DIVENCR_IC12ENC);
}

/**
  * @brief  Check if IC12 is enabled
  * @rmtoll DIVENR       IC12EN         LL_RCC_IC12_IsEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IC12_IsEnabled(void)
{
  return ((READ_BIT(RCC->DIVENR, RCC_DIVENR_IC12EN) == RCC_DIVENR_IC12EN) ? 1UL : 0UL);
}

/**
  * @brief  Set the PLL source used as IC12 clock source.
  * @rmtoll IC12CFGR      IC12SEL        LL_RCC_IC12_SetSource
  * @param  Source parameter can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC12_SetSource(uint32_t Source)
{
  MODIFY_REG(RCC->IC12CFGR, RCC_IC12CFGR_IC12SEL, Source);
}

/**
  * @brief  Get the PLL source used as IC12 clock source.
  * @rmtoll IC12CFGR      IC12SEL        LL_RCC_IC12_GetSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  */
__STATIC_INLINE uint32_t LL_RCC_IC12_GetSource(void)
{
  return (uint32_t)(READ_BIT(RCC->IC12CFGR, RCC_IC12CFGR_IC12SEL));
}

/**
  * @brief  Set  divider
  * @rmtoll IC12CFGR      IC12INT        LL_RCC_IC12_SetDivider
  * @param  Divider This parameter can be a value between 1 and 256.
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC12_SetDivider(uint32_t Divider)
{
  MODIFY_REG(RCC->IC12CFGR, RCC_IC12CFGR_IC12INT, (Divider - 1UL) << RCC_IC12CFGR_IC12INT_Pos);
}

/**
  * @brief  Get IC12 divider
  * @rmtoll IC12CFGR      IC12INT        LL_RCC_IC12_GetDivider
  * @retval can be a value between 1 and 256.
  */
__STATIC_INLINE uint32_t LL_RCC_IC12_GetDivider(void)
{
  return ((READ_BIT(RCC->IC12CFGR, RCC_IC12CFGR_IC12INT) >> RCC_IC12CFGR_IC12INT_Pos) + 1UL);
}

/**
  * @brief  Enable IC13
  * @rmtoll DIVENSR       IC13ENS        LL_RCC_IC13_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC13_Enable(void)
{
  WRITE_REG(RCC->DIVENSR, RCC_DIVENSR_IC13ENS);
}

/**
  * @brief  Disable IC13
  * @rmtoll DIVENCR       IC13ENC        LL_RCC_IC13_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC13_Disable(void)
{
  WRITE_REG(RCC->DIVENCR, RCC_DIVENCR_IC13ENC);
}

/**
  * @brief  Check if IC13 is enabled
  * @rmtoll DIVENR       IC13EN         LL_RCC_IC13_IsEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IC13_IsEnabled(void)
{
  return ((READ_BIT(RCC->DIVENR, RCC_DIVENR_IC13EN) == RCC_DIVENR_IC13EN) ? 1UL : 0UL);
}

/**
  * @brief  Set the PLL source used as IC13 clock source.
  * @rmtoll IC13CFGR      IC13SEL        LL_RCC_IC13_SetSource
  * @param  Source parameter can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC13_SetSource(uint32_t Source)
{
  MODIFY_REG(RCC->IC13CFGR, RCC_IC13CFGR_IC13SEL, Source);
}

/**
  * @brief  Get the PLL source used as IC13 clock source.
  * @rmtoll IC13CFGR      IC13SEL        LL_RCC_IC13_GetSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  */
__STATIC_INLINE uint32_t LL_RCC_IC13_GetSource(void)
{
  return (uint32_t)(READ_BIT(RCC->IC13CFGR, RCC_IC13CFGR_IC13SEL));
}

/**
  * @brief  Set  divider
  * @rmtoll IC13CFGR      IC13INT        LL_RCC_IC13_SetDivider
  * @param  Divider This parameter can be a value between 1 and 256.
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC13_SetDivider(uint32_t Divider)
{
  MODIFY_REG(RCC->IC13CFGR, RCC_IC13CFGR_IC13INT, (Divider - 1UL) << RCC_IC13CFGR_IC13INT_Pos);
}

/**
  * @brief  Get IC13 divider
  * @rmtoll IC13CFGR      IC13INT        LL_RCC_IC13_GetDivider
  * @retval can be a value between 1 and 256.
  */
__STATIC_INLINE uint32_t LL_RCC_IC13_GetDivider(void)
{
  return ((READ_BIT(RCC->IC13CFGR, RCC_IC13CFGR_IC13INT) >> RCC_IC13CFGR_IC13INT_Pos) + 1UL);
}

/**
  * @brief  Enable IC14
  * @rmtoll DIVENSR       IC14ENS        LL_RCC_IC14_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC14_Enable(void)
{
  WRITE_REG(RCC->DIVENSR, RCC_DIVENSR_IC14ENS);
}

/**
  * @brief  Disable IC14
  * @rmtoll DIVENCR       IC14ENC        LL_RCC_IC14_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC14_Disable(void)
{
  WRITE_REG(RCC->DIVENCR, RCC_DIVENCR_IC14ENC);
}

/**
  * @brief  Check if IC14 is enabled
  * @rmtoll DIVENR       IC14EN         LL_RCC_IC14_IsEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IC14_IsEnabled(void)
{
  return ((READ_BIT(RCC->DIVENR, RCC_DIVENR_IC14EN) == RCC_DIVENR_IC14EN) ? 1UL : 0UL);
}

/**
  * @brief  Set the PLL source used as IC14 clock source.
  * @rmtoll IC14CFGR      IC14SEL        LL_RCC_IC14_SetSource
  * @param  Source parameter can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC14_SetSource(uint32_t Source)
{
  MODIFY_REG(RCC->IC14CFGR, RCC_IC14CFGR_IC14SEL, Source);
}

/**
  * @brief  Get the PLL source used as IC14 clock source.
  * @rmtoll IC14CFGR      IC14SEL        LL_RCC_IC14_GetSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  */
__STATIC_INLINE uint32_t LL_RCC_IC14_GetSource(void)
{
  return (uint32_t)(READ_BIT(RCC->IC14CFGR, RCC_IC14CFGR_IC14SEL));
}

/**
  * @brief  Set  divider
  * @rmtoll IC14CFGR      IC14INT        LL_RCC_IC14_SetDivider
  * @param  Divider This parameter can be a value between 1 and 256.
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC14_SetDivider(uint32_t Divider)
{
  MODIFY_REG(RCC->IC14CFGR, RCC_IC14CFGR_IC14INT, (Divider - 1UL) << RCC_IC14CFGR_IC14INT_Pos);
}

/**
  * @brief  Get IC14 divider
  * @rmtoll IC14CFGR      IC14INT        LL_RCC_IC14_GetDivider
  * @retval can be a value between 1 and 256.
  */
__STATIC_INLINE uint32_t LL_RCC_IC14_GetDivider(void)
{
  return ((READ_BIT(RCC->IC14CFGR, RCC_IC14CFGR_IC14INT) >> RCC_IC14CFGR_IC14INT_Pos) + 1UL);
}

/**
  * @brief  Enable IC15
  * @rmtoll DIVENSR       IC15ENS        LL_RCC_IC15_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC15_Enable(void)
{
  WRITE_REG(RCC->DIVENSR, RCC_DIVENSR_IC15ENS);
}

/**
  * @brief  Disable IC15
  * @rmtoll DIVENCR       IC15ENC        LL_RCC_IC15_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC15_Disable(void)
{
  WRITE_REG(RCC->DIVENCR, RCC_DIVENCR_IC15ENC);
}

/**
  * @brief  Check if IC15 is enabled
  * @rmtoll DIVENR       IC15EN         LL_RCC_IC15_IsEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IC15_IsEnabled(void)
{
  return ((READ_BIT(RCC->DIVENR, RCC_DIVENR_IC15EN) == RCC_DIVENR_IC15EN) ? 1UL : 0UL);
}

/**
  * @brief  Set the PLL source used as IC15 clock source.
  * @rmtoll IC15CFGR      IC15SEL        LL_RCC_IC15_SetSource
  * @param  Source parameter can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC15_SetSource(uint32_t Source)
{
  MODIFY_REG(RCC->IC15CFGR, RCC_IC15CFGR_IC15SEL, Source);
}

/**
  * @brief  Get the PLL source used as IC15 clock source.
  * @rmtoll IC15CFGR      IC15SEL        LL_RCC_IC15_GetSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  */
__STATIC_INLINE uint32_t LL_RCC_IC15_GetSource(void)
{
  return (uint32_t)(READ_BIT(RCC->IC15CFGR, RCC_IC15CFGR_IC15SEL));
}

/**
  * @brief  Set  divider
  * @rmtoll IC15CFGR      IC15INT        LL_RCC_IC15_SetDivider
  * @param  Divider This parameter can be a value between 1 and 256.
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC15_SetDivider(uint32_t Divider)
{
  MODIFY_REG(RCC->IC15CFGR, RCC_IC15CFGR_IC15INT, (Divider - 1UL) << RCC_IC15CFGR_IC15INT_Pos);
}

/**
  * @brief  Get IC15 divider
  * @rmtoll IC15CFGR      IC15INT        LL_RCC_IC15_GetDivider
  * @retval can be a value between 1 and 256.
  */
__STATIC_INLINE uint32_t LL_RCC_IC15_GetDivider(void)
{
  return ((READ_BIT(RCC->IC15CFGR, RCC_IC15CFGR_IC15INT) >> RCC_IC15CFGR_IC15INT_Pos) + 1UL);
}

/**
  * @brief  Enable IC16
  * @rmtoll DIVENSR       IC16ENS        LL_RCC_IC16_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC16_Enable(void)
{
  WRITE_REG(RCC->DIVENSR, RCC_DIVENSR_IC16ENS);
}

/**
  * @brief  Disable IC16
  * @rmtoll DIVENCR       IC16ENC        LL_RCC_IC16_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC16_Disable(void)
{
  WRITE_REG(RCC->DIVENCR, RCC_DIVENCR_IC16ENC);
}

/**
  * @brief  Check if IC16 is enabled
  * @rmtoll DIVENR       IC16EN         LL_RCC_IC16_IsEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IC16_IsEnabled(void)
{
  return ((READ_BIT(RCC->DIVENR, RCC_DIVENR_IC16EN) == RCC_DIVENR_IC16EN) ? 1UL : 0UL);
}

/**
  * @brief  Set the PLL source used as IC16 clock source.
  * @rmtoll IC16CFGR      IC16SEL        LL_RCC_IC16_SetSource
  * @param  Source parameter can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC16_SetSource(uint32_t Source)
{
  MODIFY_REG(RCC->IC16CFGR, RCC_IC16CFGR_IC16SEL, Source);
}

/**
  * @brief  Get the PLL source used as IC16 clock source.
  * @rmtoll IC16CFGR      IC16SEL        LL_RCC_IC16_GetSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  */
__STATIC_INLINE uint32_t LL_RCC_IC16_GetSource(void)
{
  return (uint32_t)(READ_BIT(RCC->IC16CFGR, RCC_IC16CFGR_IC16SEL));
}

/**
  * @brief  Set  divider
  * @rmtoll IC16CFGR      IC16INT        LL_RCC_IC16_SetDivider
  * @param  Divider This parameter can be a value between 1 and 256.
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC16_SetDivider(uint32_t Divider)
{
  MODIFY_REG(RCC->IC16CFGR, RCC_IC16CFGR_IC16INT, (Divider - 1UL) << RCC_IC16CFGR_IC16INT_Pos);
}

/**
  * @brief  Get IC16 divider
  * @rmtoll IC16CFGR      IC16INT        LL_RCC_IC16_GetDivider
  * @retval can be a value between 1 and 256.
  */
__STATIC_INLINE uint32_t LL_RCC_IC16_GetDivider(void)
{
  return ((READ_BIT(RCC->IC16CFGR, RCC_IC16CFGR_IC16INT) >> RCC_IC16CFGR_IC16INT_Pos) + 1UL);
}

/**
  * @brief  Enable IC17
  * @rmtoll DIVENSR       IC17ENS        LL_RCC_IC17_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC17_Enable(void)
{
  WRITE_REG(RCC->DIVENSR, RCC_DIVENSR_IC17ENS);
}

/**
  * @brief  Disable IC17
  * @rmtoll DIVENCR       IC17ENC        LL_RCC_IC17_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC17_Disable(void)
{
  WRITE_REG(RCC->DIVENCR, RCC_DIVENCR_IC17ENC);
}

/**
  * @brief  Check if IC17 is enabled
  * @rmtoll DIVENR       IC17EN         LL_RCC_IC17_IsEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IC17_IsEnabled(void)
{
  return ((READ_BIT(RCC->DIVENR, RCC_DIVENR_IC17EN) == RCC_DIVENR_IC17EN) ? 1UL : 0UL);
}

/**
  * @brief  Set the PLL source used as IC17 clock source.
  * @rmtoll IC17CFGR      IC17SEL        LL_RCC_IC17_SetSource
  * @param  Source parameter can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC17_SetSource(uint32_t Source)
{
  MODIFY_REG(RCC->IC17CFGR, RCC_IC17CFGR_IC17SEL, Source);
}

/**
  * @brief  Get the PLL source used as IC17 clock source.
  * @rmtoll IC17CFGR      IC17SEL        LL_RCC_IC17_GetSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  */
__STATIC_INLINE uint32_t LL_RCC_IC17_GetSource(void)
{
  return (uint32_t)(READ_BIT(RCC->IC17CFGR, RCC_IC17CFGR_IC17SEL));
}

/**
  * @brief  Set  divider
  * @rmtoll IC17CFGR      IC17INT        LL_RCC_IC17_SetDivider
  * @param  Divider This parameter can be a value between 1 and 256.
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC17_SetDivider(uint32_t Divider)
{
  MODIFY_REG(RCC->IC17CFGR, RCC_IC17CFGR_IC17INT, (Divider - 1UL) << RCC_IC17CFGR_IC17INT_Pos);
}

/**
  * @brief  Get IC17 divider
  * @rmtoll IC17CFGR      IC17INT        LL_RCC_IC17_GetDivider
  * @retval can be a value between 1 and 256.
  */
__STATIC_INLINE uint32_t LL_RCC_IC17_GetDivider(void)
{
  return ((READ_BIT(RCC->IC17CFGR, RCC_IC17CFGR_IC17INT) >> RCC_IC17CFGR_IC17INT_Pos) + 1UL);
}

/**
  * @brief  Enable IC18
  * @rmtoll DIVENSR       IC18ENS        LL_RCC_IC18_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC18_Enable(void)
{
  WRITE_REG(RCC->DIVENSR, RCC_DIVENSR_IC18ENS);
}

/**
  * @brief  Disable IC18
  * @rmtoll DIVENCR       IC18ENC        LL_RCC_IC18_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC18_Disable(void)
{
  WRITE_REG(RCC->DIVENCR, RCC_DIVENCR_IC18ENC);
}

/**
  * @brief  Check if IC18 is enabled
  * @rmtoll DIVENR       IC18EN         LL_RCC_IC18_IsEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IC18_IsEnabled(void)
{
  return ((READ_BIT(RCC->DIVENR, RCC_DIVENR_IC18EN) == RCC_DIVENR_IC18EN) ? 1UL : 0UL);
}

/**
  * @brief  Set the PLL source used as IC18 clock source.
  * @rmtoll IC18CFGR      IC18SEL        LL_RCC_IC18_SetSource
  * @param  Source parameter can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC18_SetSource(uint32_t Source)
{
  MODIFY_REG(RCC->IC18CFGR, RCC_IC18CFGR_IC18SEL, Source);
}

/**
  * @brief  Get the PLL source used as IC18 clock source.
  * @rmtoll IC18CFGR      IC18SEL        LL_RCC_IC18_GetSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  */
__STATIC_INLINE uint32_t LL_RCC_IC18_GetSource(void)
{
  return (uint32_t)(READ_BIT(RCC->IC18CFGR, RCC_IC18CFGR_IC18SEL));
}

/**
  * @brief  Set  divider
  * @rmtoll IC18CFGR      IC18INT        LL_RCC_IC18_SetDivider
  * @param  Divider This parameter can be a value between 1 and 256.
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC18_SetDivider(uint32_t Divider)
{
  MODIFY_REG(RCC->IC18CFGR, RCC_IC18CFGR_IC18INT, (Divider - 1UL) << RCC_IC18CFGR_IC18INT_Pos);
}

/**
  * @brief  Get IC18 divider
  * @rmtoll IC18CFGR      IC18INT        LL_RCC_IC18_GetDivider
  * @retval can be a value between 1 and 256.
  */
__STATIC_INLINE uint32_t LL_RCC_IC18_GetDivider(void)
{
  return ((READ_BIT(RCC->IC18CFGR, RCC_IC18CFGR_IC18INT) >> RCC_IC18CFGR_IC18INT_Pos) + 1UL);
}

/**
  * @brief  Enable IC19
  * @rmtoll DIVENSR       IC19ENS        LL_RCC_IC19_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC19_Enable(void)
{
  WRITE_REG(RCC->DIVENSR, RCC_DIVENSR_IC19ENS);
}

/**
  * @brief  Disable IC19
  * @rmtoll DIVENCR       IC19ENC        LL_RCC_IC19_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC19_Disable(void)
{
  WRITE_REG(RCC->DIVENCR, RCC_DIVENCR_IC19ENC);
}

/**
  * @brief  Check if IC19 is enabled
  * @rmtoll DIVENR       IC19EN         LL_RCC_IC19_IsEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IC19_IsEnabled(void)
{
  return ((READ_BIT(RCC->DIVENR, RCC_DIVENR_IC19EN) == RCC_DIVENR_IC19EN) ? 1UL : 0UL);
}

/**
  * @brief  Set the PLL source used as IC19 clock source.
  * @rmtoll IC19CFGR      IC19SEL        LL_RCC_IC19_SetSource
  * @param  Source parameter can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC19_SetSource(uint32_t Source)
{
  MODIFY_REG(RCC->IC19CFGR, RCC_IC19CFGR_IC19SEL, Source);
}

/**
  * @brief  Get the PLL source used as IC19 clock source.
  * @rmtoll IC19CFGR      IC19SEL        LL_RCC_IC19_GetSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  */
__STATIC_INLINE uint32_t LL_RCC_IC19_GetSource(void)
{
  return (uint32_t)(READ_BIT(RCC->IC19CFGR, RCC_IC19CFGR_IC19SEL));
}

/**
  * @brief  Set  divider
  * @rmtoll IC19CFGR      IC19INT        LL_RCC_IC19_SetDivider
  * @param  Divider This parameter can be a value between 1 and 256.
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC19_SetDivider(uint32_t Divider)
{
  MODIFY_REG(RCC->IC19CFGR, RCC_IC19CFGR_IC19INT, (Divider - 1UL) << RCC_IC19CFGR_IC19INT_Pos);
}

/**
  * @brief  Get IC19 divider
  * @rmtoll IC19CFGR      IC19INT        LL_RCC_IC19_GetDivider
  * @retval can be a value between 1 and 256.
  */
__STATIC_INLINE uint32_t LL_RCC_IC19_GetDivider(void)
{
  return ((READ_BIT(RCC->IC19CFGR, RCC_IC19CFGR_IC19INT) >> RCC_IC19CFGR_IC19INT_Pos) + 1UL);
}

/**
  * @brief  Enable IC20
  * @rmtoll DIVENSR       IC20ENS        LL_RCC_IC20_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC20_Enable(void)
{
  WRITE_REG(RCC->DIVENSR, RCC_DIVENSR_IC20ENS);
}

/**
  * @brief  Disable IC20
  * @rmtoll DIVENCR       IC20ENC        LL_RCC_IC20_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC20_Disable(void)
{
  WRITE_REG(RCC->DIVENCR, RCC_DIVENCR_IC20ENC);
}

/**
  * @brief  Check if IC20 is enabled
  * @rmtoll DIVENR       IC20EN         LL_RCC_IC20_IsEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IC20_IsEnabled(void)
{
  return ((READ_BIT(RCC->DIVENR, RCC_DIVENR_IC20EN) == RCC_DIVENR_IC20EN) ? 1UL : 0UL);
}

/**
  * @brief  Set the PLL source used as IC20 clock source.
  * @rmtoll IC20CFGR      IC20SEL        LL_RCC_IC20_SetSource
  * @param  Source parameter can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC20_SetSource(uint32_t Source)
{
  MODIFY_REG(RCC->IC20CFGR, RCC_IC20CFGR_IC20SEL, Source);
}

/**
  * @brief  Get the PLL source used as IC20 clock source.
  * @rmtoll IC20CFGR      IC20SEL        LL_RCC_IC20_GetSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL1
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL2
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL3
  *         @arg @ref LL_RCC_ICCLKSOURCE_PLL4
  */
__STATIC_INLINE uint32_t LL_RCC_IC20_GetSource(void)
{
  return (uint32_t)(READ_BIT(RCC->IC20CFGR, RCC_IC20CFGR_IC20SEL));
}

/**
  * @brief  Set  divider
  * @rmtoll IC20CFGR      IC20INT        LL_RCC_IC20_SetDivider
  * @param  Divider This parameter can be a value between 1 and 256.
  * @retval None
  */
__STATIC_INLINE void LL_RCC_IC20_SetDivider(uint32_t Divider)
{
  MODIFY_REG(RCC->IC20CFGR, RCC_IC20CFGR_IC20INT, (Divider - 1UL) << RCC_IC20CFGR_IC20INT_Pos);
}

/**
  * @brief  Get IC20 divider
  * @rmtoll IC20CFGR      IC20INT        LL_RCC_IC20_GetDivider
  * @retval can be a value between 1 and 256.
  */
__STATIC_INLINE uint32_t LL_RCC_IC20_GetDivider(void)
{
  return ((READ_BIT(RCC->IC20CFGR, RCC_IC20CFGR_IC20INT) >> RCC_IC20CFGR_IC20INT_Pos) + 1UL);
}

/**
  * @brief  Enable CLKP
  * @rmtoll MISCENSR      PERENS        LL_RCC_CLKP_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_CLKP_Enable(void)
{
  WRITE_REG(RCC->MISCENSR, RCC_MISCENSR_PERENS);
}

/**
  * @brief  Disable CLKP
  * @rmtoll MISCENCR      PERENC        LL_RCC_CLKP_Disable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_CLKP_Disable(void)
{
  WRITE_REG(RCC->MISCENCR, RCC_MISCENCR_PERENC);
}

/**
  * @brief  Check if CLKP is enabled
  * @rmtoll MISCENR       PEREN         LL_RCC_CLKP_IsEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_CLKP_IsEnabled(void)
{
  return ((READ_BIT(RCC->MISCENR, RCC_MISCENR_PEREN) == RCC_MISCENR_PEREN) ? 1UL : 0UL);
}

/**
  * @}
  */


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
  WRITE_REG(RCC->CICR, RCC_CICR_LSIRDYC);
}

/**
  * @brief  Clear LSE ready interrupt flag
  * @rmtoll CICR         LSERDYC       LL_RCC_ClearFlag_LSERDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ClearFlag_LSERDY(void)
{
  WRITE_REG(RCC->CICR, RCC_CICR_LSERDYC);
}

/**
  * @brief  Clear HSI ready interrupt flag
  * @rmtoll CICR         HSIRDYC       LL_RCC_ClearFlag_HSIRDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ClearFlag_HSIRDY(void)
{
  WRITE_REG(RCC->CICR, RCC_CICR_HSIRDYC);
}

/**
  * @brief  Clear HSE ready interrupt flag
  * @rmtoll CICR         HSERDYC       LL_RCC_ClearFlag_HSERDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ClearFlag_HSERDY(void)
{
  WRITE_REG(RCC->CICR, RCC_CICR_HSERDYC);
}

/**
  * @brief  Clear MSI ready interrupt flag
  * @rmtoll CICR         MSIRDYC       LL_RCC_ClearFlag_MSIRDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ClearFlag_MSIRDY(void)
{
  WRITE_REG(RCC->CICR, RCC_CICR_MSIRDYC);
}

/**
  * @brief  Clear PLL1 ready interrupt flag
  * @rmtoll CICR         PLL1RDYC       LL_RCC_ClearFlag_PLL1RDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ClearFlag_PLL1RDY(void)
{
  WRITE_REG(RCC->CICR, RCC_CICR_PLL1RDYC);
}

/**
  * @brief  Clear PLL2 ready interrupt flag
  * @rmtoll CICR         PLL2RDYC       LL_RCC_ClearFlag_PLL2RDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ClearFlag_PLL2RDY(void)
{
  WRITE_REG(RCC->CICR, RCC_CICR_PLL2RDYC);
}

/**
  * @brief  Clear PLL3 ready interrupt flag
  * @rmtoll CICR         PLL3RDYC       LL_RCC_ClearFlag_PLL3RDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ClearFlag_PLL3RDY(void)
{
  WRITE_REG(RCC->CICR, RCC_CICR_PLL3RDYC);
}

/**
  * @brief  Clear PLL4 ready interrupt flag
  * @rmtoll CICR         PLL4RDYC       LL_RCC_ClearFlag_PLL4RDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ClearFlag_PLL4RDY(void)
{
  WRITE_REG(RCC->CICR, RCC_CICR_PLL4RDYC);
}

/**
  * @brief  Clear LSE Clock security system interrupt flag
  * @rmtoll CICR         LSECSSC         LL_RCC_ClearFlag_LSECSS
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ClearFlag_LSECSS(void)
{
  WRITE_REG(RCC->CICR, RCC_CICR_LSECSSC);
}

/**
  * @brief  Clear HSE Clock security system interrupt flag
  * @rmtoll CICR         HSECSSC         LL_RCC_ClearFlag_HSECSS
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ClearFlag_HSECSS(void)
{
  SET_BIT(RCC->CICR, RCC_CICR_HSECSSC);
}

/**
  * @brief  Clear WKUP ready interrupt flag
  * @rmtoll CICR         WKUPFC          LL_RCC_ClearFlag_WKUP
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ClearFlag_WKUP(void)
{
  WRITE_REG(RCC->CICR, RCC_CICR_WKUPFC);
}

/**
  * @brief  Check if LSI ready interrupt occurred or not
  * @rmtoll CIFR         LSIRDYF         LL_RCC_IsActiveFlag_LSIRDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_LSIRDY(void)
{
  return ((READ_BIT(RCC->CIFR, RCC_CIFR_LSIRDYF) == RCC_CIFR_LSIRDYF) ? 1UL : 0UL);
}

/**
  * @brief  Check if LSE ready interrupt occurred or not
  * @rmtoll CIFR         LSERDYF         LL_RCC_IsActiveFlag_LSERDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_LSERDY(void)
{
  return ((READ_BIT(RCC->CIFR, RCC_CIFR_LSERDYF) == RCC_CIFR_LSERDYF) ? 1UL : 0UL);
}

/**
  * @brief  Check if HSI ready interrupt occurred or not
  * @rmtoll CIFR         HSIRDYF         LL_RCC_IsActiveFlag_HSIRDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_HSIRDY(void)
{
  return ((READ_BIT(RCC->CIFR, RCC_CIFR_HSIRDYF) == RCC_CIFR_HSIRDYF) ? 1UL : 0UL);
}

/**
  * @brief  Check if HSE ready interrupt occurred or not
  * @rmtoll CIFR         HSERDYF         LL_RCC_IsActiveFlag_HSERDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_HSERDY(void)
{
  return ((READ_BIT(RCC->CIFR, RCC_CIFR_HSERDYF) == RCC_CIFR_HSERDYF) ? 1UL : 0UL);
}

/**
  * @brief  Check if MSI ready interrupt occurred or not
  * @rmtoll CIFR         MSIRDYF         LL_RCC_IsActiveFlag_MSIRDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_MSIRDY(void)
{
  return ((READ_BIT(RCC->CIFR, RCC_CIFR_MSIRDYF) == RCC_CIFR_MSIRDYF) ? 1UL : 0UL);
}

/**
  * @brief  Check if PLL1 ready interrupt occurred or not
  * @rmtoll CIFR         PLL1RDYF        LL_RCC_IsActiveFlag_PLL1RDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_PLL1RDY(void)
{
  return ((READ_BIT(RCC->CIFR, RCC_CIFR_PLL1RDYF) == RCC_CIFR_PLL1RDYF) ? 1UL : 0UL);
}

/**
  * @brief  Check if PLL2 ready interrupt occurred or not
  * @rmtoll CIFR         PLL2RDYF        LL_RCC_IsActiveFlag_PLL2RDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_PLL2RDY(void)
{
  return ((READ_BIT(RCC->CIFR, RCC_CIFR_PLL2RDYF) == RCC_CIFR_PLL2RDYF) ? 1UL : 0UL);
}

/**
  * @brief  Check if PLL3 ready interrupt occurred or not
  * @rmtoll CIFR         PLL3RDYF        LL_RCC_IsActiveFlag_PLL3RDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_PLL3RDY(void)
{
  return ((READ_BIT(RCC->CIFR, RCC_CIFR_PLL3RDYF) == RCC_CIFR_PLL3RDYF) ? 1UL : 0UL);
}

/**
  * @brief  Check if PLL4 ready interrupt occurred or not
  * @rmtoll CIFR         PLL4RDYF        LL_RCC_IsActiveFlag_PLL4RDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_PLL4RDY(void)
{
  return ((READ_BIT(RCC->CIFR, RCC_CIFR_PLL4RDYF) == RCC_CIFR_PLL4RDYF) ? 1UL : 0UL);
}

/**
  * @brief  Check if LSE Clock security system interrupt occurred or not
  * @rmtoll CIFR         LSECSSF         LL_RCC_IsActiveFlag_LSECSS
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_LSECSS(void)
{
  return ((READ_BIT(RCC->CIFR, RCC_CIFR_LSECSSF) == RCC_CIFR_LSECSSF) ? 1UL : 0UL);
}

/**
  * @brief  Check if HSE Clock security system interrupt occurred or not
  * @rmtoll CIFR         HSECSSF         LL_RCC_IsActiveFlag_HSECSS
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_HSECSS(void)
{
  return ((READ_BIT(RCC->CIFR, RCC_CIFR_HSECSSF) == RCC_CIFR_HSECSSF) ? 1UL : 0UL);
}

/**
  * @brief  Check if WKUP from STOP interrupt occurred or not
  * @rmtoll CIFR         WKUPF           LL_RCC_IsActiveFlag_WKUP
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_WKUP(void)
{
  return ((READ_BIT(RCC->CIFR, RCC_CIFR_WKUPF) == RCC_CIFR_WKUPF) ? 1UL : 0UL);
}

/**
  * @brief  Check if RCC flag Low Power D1 reset is set or not.
  * @rmtoll RSR          LPWRRSTF       LL_RCC_IsActiveFlag_LPWRRST
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_LPWRRST(void)
{
  return ((READ_BIT(RCC->RSR, RCC_RSR_LPWRRSTF) == RCC_RSR_LPWRRSTF) ? 1UL : 0UL);
}

/**
  * @brief  Check if RCC flag Window Watchdog reset is set or not.
  * @rmtoll RSR          WWDGRSTF      LL_RCC_IsActiveFlag_WWDGRST
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_WWDGRST(void)
{
  return ((READ_BIT(RCC->RSR, RCC_RSR_WWDGRSTF) == RCC_RSR_WWDGRSTF) ? 1UL : 0UL);
}

/**
  * @brief  Check if RCC flag Independent Watchdog reset is set or not.
  * @rmtoll RSR          IWDGRSTF      LL_RCC_IsActiveFlag_IWDGRST
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_IWDGRST(void)
{
  return ((READ_BIT(RCC->RSR, RCC_RSR_IWDGRSTF) == RCC_RSR_IWDGRSTF) ? 1UL : 0UL);
}

/**
  * @brief  Check if RCC flag Software reset is set or not.
  * @rmtoll RSR          SFTRSTF        LL_RCC_IsActiveFlag_SFTRST
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_SFTRST(void)
{
  return ((READ_BIT(RCC->RSR, RCC_RSR_SFTRSTF) == RCC_RSR_SFTRSTF) ? 1UL : 0UL);
}

/**
  * @brief  Check if RCC flag POR/PDR reset is set or not.
  * @rmtoll RSR          PORRSTF       LL_RCC_IsActiveFlag_PORRST
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_PORRST(void)
{
  return ((READ_BIT(RCC->RSR, RCC_RSR_PORRSTF) == RCC_RSR_PORRSTF) ? 1UL : 0UL);
}

/**
  * @brief  Check if RCC flag Pin reset is set or not.
  * @rmtoll RSR          PINRSTF       LL_RCC_IsActiveFlag_PINRST
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_PINRST(void)
{
  return ((READ_BIT(RCC->RSR, RCC_RSR_PINRSTF) == RCC_RSR_PINRSTF) ? 1UL : 0UL);
}

/**
  * @brief  Check if RCC flag BOR reset is set or not.
  * @rmtoll RSR          BORRSTF       LL_RCC_IsActiveFlag_BORRST
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_BORRST(void)
{
  return ((READ_BIT(RCC->RSR, RCC_RSR_BORRSTF) == (RCC_RSR_BORRSTF)) ? 1UL : 0UL);
}

/**
  * @brief  Check if RCC flag CPU Lockup reset is set or not.
  * @rmtoll RSR          BORRSTF       LL_RCC_IsActiveFlag_LCKRST
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_LCKRST(void)
{
  return ((READ_BIT(RCC->RSR, RCC_RSR_LCKRSTF) == (RCC_RSR_LCKRSTF)) ? 1UL : 0UL);
}

/**
  * @brief  Check if RCC ETH1 power-down start acknowledged is set or not.
  * @rmtoll CCIPR2       ETH1PWRDOWNACK LL_RCC_IsActiveFlag_ETH1PWRDOWNACK
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsActiveFlag_ETH1PWRDOWNACK(void)
{
  return ((READ_BIT(RCC->CCIPR2, RCC_CCIPR2_ETH1PWRDOWNACK) == (RCC_CCIPR2_ETH1PWRDOWNACK)) ? 1UL : 0UL);
}

/**
  * @brief  Set RMVF bit to clear all reset flags.
  * @rmtoll RSR          RMVF          LL_RCC_ClearResetFlags
  * @retval None
  */
__STATIC_INLINE void LL_RCC_ClearResetFlags(void)
{
  SET_BIT(RCC->RSR, RCC_RSR_RMVF);
}

/**
  * @}
  */

/** @defgroup RCC_LL_EF_IT_Management IT Management
  * @{
  */

/**
  * @brief  Enable LSI ready interrupt
  * @rmtoll CIER         LSIRDYIE        LL_RCC_EnableIT_LSIRDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_EnableIT_LSIRDY(void)
{
  SET_BIT(RCC->CIER, RCC_CIER_LSIRDYIE);
}

/**
  * @brief  Enable LSE ready interrupt
  * @rmtoll CIER         LSERDYIE        LL_RCC_EnableIT_LSERDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_EnableIT_LSERDY(void)
{
  SET_BIT(RCC->CIER, RCC_CIER_LSERDYIE);
}

/**
  * @brief  Enable MSI ready interrupt
  * @rmtoll CIER         MSIRDYIE        LL_RCC_EnableIT_MSIRDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_EnableIT_MSIRDY(void)
{
  SET_BIT(RCC->CIER, RCC_CIER_MSIRDYIE);
}

/**
  * @brief  Enable HSI ready interrupt
  * @rmtoll CIER         HSIRDYIE        LL_RCC_EnableIT_HSIRDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_EnableIT_HSIRDY(void)
{
  SET_BIT(RCC->CIER, RCC_CIER_HSIRDYIE);
}

/**
  * @brief  Enable HSE ready interrupt
  * @rmtoll CIER         HSERDYIE        LL_RCC_EnableIT_HSERDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_EnableIT_HSERDY(void)
{
  SET_BIT(RCC->CIER, RCC_CIER_HSERDYIE);
}

/**
  * @brief  Enable PLL1 ready interrupt
  * @rmtoll CIER         PLL1RDYIE       LL_RCC_EnableIT_PLL1RDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_EnableIT_PLL1RDY(void)
{
  SET_BIT(RCC->CIER, RCC_CIER_PLL1RDYIE);
}

/**
  * @brief  Enable PLL2 ready interrupt
  * @rmtoll CIER         PLL2RDYIE       LL_RCC_EnableIT_PLL2RDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_EnableIT_PLL2RDY(void)
{
  SET_BIT(RCC->CIER, RCC_CIER_PLL2RDYIE);
}

/**
  * @brief  Enable PLL3 ready interrupt
  * @rmtoll CIER         PLL3RDYIE       LL_RCC_EnableIT_PLL3RDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_EnableIT_PLL3RDY(void)
{
  SET_BIT(RCC->CIER, RCC_CIER_PLL3RDYIE);
}

/**
  * @brief  Enable PLL4 ready interrupt
  * @rmtoll CIER         PLL4RDYIE       LL_RCC_EnableIT_PLL4RDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_EnableIT_PLL4RDY(void)
{
  SET_BIT(RCC->CIER, RCC_CIER_PLL4RDYIE);
}

/**
  * @brief  Enable LSECSS interrupt
  * @rmtoll CIER         LSECSSIE        LL_RCC_EnableIT_LSECSS
  * @retval None
  */
__STATIC_INLINE void LL_RCC_EnableIT_LSECSS(void)
{
  SET_BIT(RCC->CIER, RCC_CIER_LSECSSIE);
}

/**
  * @brief  Enable HSECSS interrupt
  * @rmtoll CIER         HSECSSIE        LL_RCC_EnableIT_HSECSS
  * @retval None
  */
__STATIC_INLINE void LL_RCC_EnableIT_HSECSS(void)
{
  SET_BIT(RCC->CIER, RCC_CIER_HSECSSIE);
}

/**
  * @brief  Enable WKUP interrupt
  * @rmtoll CIER         WKUPIE          LL_RCC_EnableIT_WKUP
  * @retval None
  */
__STATIC_INLINE void LL_RCC_EnableIT_WKUP(void)
{
  SET_BIT(RCC->CIER, RCC_CIER_WKUPIE);
}

/**
  * @brief  Disable LSI ready interrupt
  * @rmtoll CIER         LSIRDYIE        LL_RCC_DisableIT_LSIRDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_DisableIT_LSIRDY(void)
{
  CLEAR_BIT(RCC->CIER, RCC_CIER_LSIRDYIE);
}

/**
  * @brief  Disable LSE ready interrupt
  * @rmtoll CIER         LSERDYIE        LL_RCC_DisableIT_LSERDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_DisableIT_LSERDY(void)
{
  CLEAR_BIT(RCC->CIER, RCC_CIER_LSERDYIE);
}

/**
  * @brief  Disable MSI ready interrupt
  * @rmtoll CIER         MSIRDYIE        LL_RCC_DisableIT_MSIRDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_DisableIT_MSIRDY(void)
{
  CLEAR_BIT(RCC->CIER, RCC_CIER_MSIRDYIE);
}

/**
  * @brief  Disable HSI ready interrupt
  * @rmtoll CIER         HSIRDYIE        LL_RCC_DisableIT_HSIRDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_DisableIT_HSIRDY(void)
{
  CLEAR_BIT(RCC->CIER, RCC_CIER_HSIRDYIE);
}

/**
  * @brief  Disable HSE ready interrupt
  * @rmtoll CIER         HSERDYIE        LL_RCC_DisableIT_HSERDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_DisableIT_HSERDY(void)
{
  CLEAR_BIT(RCC->CIER, RCC_CIER_HSERDYIE);
}

/**
  * @brief  Disable PLL1 ready interrupt
  * @rmtoll CIER         PLL1RDYIE       LL_RCC_DisableIT_PLL1RDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_DisableIT_PLL1RDY(void)
{
  CLEAR_BIT(RCC->CIER, RCC_CIER_PLL1RDYIE);
}

/**
  * @brief  Disable PLL2 ready interrupt
  * @rmtoll CIER         PLL2RDYIE       LL_RCC_DisableIT_PLL2RDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_DisableIT_PLL2RDY(void)
{
  CLEAR_BIT(RCC->CIER, RCC_CIER_PLL2RDYIE);
}

/**
  * @brief  Disable PLL3 ready interrupt
  * @rmtoll CIER         PLL3RDYIE       LL_RCC_DisableIT_PLL3RDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_DisableIT_PLL3RDY(void)
{
  CLEAR_BIT(RCC->CIER, RCC_CIER_PLL3RDYIE);
}

/**
  * @brief  Disable PLL4 ready interrupt
  * @rmtoll CIER         PLL4RDYIE       LL_RCC_DisableIT_PLL4RDY
  * @retval None
  */
__STATIC_INLINE void LL_RCC_DisableIT_PLL4RDY(void)
{
  CLEAR_BIT(RCC->CIER, RCC_CIER_PLL4RDYIE);
}

/**
  * @brief  Disable LSECSS interrupt
  * @rmtoll CIER         LSECSSIE        LL_RCC_DisableIT_LSECSS
  * @retval None
  */
__STATIC_INLINE void LL_RCC_DisableIT_LSECSS(void)
{
  CLEAR_BIT(RCC->CIER, RCC_CIER_LSECSSIE);
}

/**
  * @brief  Disable HSECSS interrupt
  * @rmtoll CIER         HSECSSIE        LL_RCC_DisableIT_HSECSS
  * @retval None
  */
__STATIC_INLINE void LL_RCC_DisableIT_HSECSS(void)
{
  CLEAR_BIT(RCC->CIER, RCC_CIER_HSECSSIE);
}

/**
  * @brief  Disable WKUP interrupt
  * @rmtoll CIER         WKUPIE          LL_RCC_DisableIT_WKUP
  * @retval None
  */
__STATIC_INLINE void LL_RCC_DisableIT_WKUP(void)
{
  CLEAR_BIT(RCC->CIER, RCC_CIER_WKUPIE);
}

/**
  * @brief  Checks if LSI ready interrupt source is enabled or disabled.
  * @rmtoll CIER         LSIRDYIE        LL_RCC_IsEnabledIT_LSIRDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsEnabledIT_LSIRDY(void)
{
  return ((READ_BIT(RCC->CIER, RCC_CIER_LSIRDYIE) == RCC_CIER_LSIRDYIE) ? 1UL : 0UL);
}

/**
  * @brief  Checks if LSE ready interrupt source is enabled or disabled.
  * @rmtoll CIER         LSERDYIE        LL_RCC_IsEnabledIT_LSERDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsEnabledIT_LSERDY(void)
{
  return ((READ_BIT(RCC->CIER, RCC_CIER_LSERDYIE) == RCC_CIER_LSERDYIE) ? 1UL : 0UL);
}

/**
  * @brief  Checks if MSI ready interrupt source is enabled or disabled.
  * @rmtoll CIER         MSIRDYIE        LL_RCC_IsEnabledIT_MSIRDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsEnabledIT_MSIRDY(void)
{
  return ((READ_BIT(RCC->CIER, RCC_CIER_MSIRDYIE) == RCC_CIER_MSIRDYIE) ? 1UL : 0UL);
}

/**
  * @brief  Checks if HSI ready interrupt source is enabled or disabled.
  * @rmtoll CIER         HSIRDYIE        LL_RCC_IsEnabledIT_HSIRDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsEnabledIT_HSIRDY(void)
{
  return ((READ_BIT(RCC->CIER, RCC_CIER_HSIRDYIE) == RCC_CIER_HSIRDYIE) ? 1UL : 0UL);
}

/**
  * @brief  Checks if HSE ready interrupt source is enabled or disabled.
  * @rmtoll CIER         HSERDYIE        LL_RCC_IsEnabledIT_HSERDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsEnabledIT_HSERDY(void)
{
  return ((READ_BIT(RCC->CIER, RCC_CIER_HSERDYIE) == RCC_CIER_HSERDYIE) ? 1UL : 0UL);
}

/**
  * @brief  Checks if PLL1 ready interrupt source is enabled or disabled.
  * @rmtoll CIER         PLL1RDYIE       LL_RCC_IsEnabledIT_PLL1RDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsEnabledIT_PLL1RDY(void)
{
  return ((READ_BIT(RCC->CIER, RCC_CIER_PLL1RDYIE) == RCC_CIER_PLL1RDYIE) ? 1UL : 0UL);
}

/**
  * @brief  Checks if PLL2 ready interrupt source is enabled or disabled.
  * @rmtoll CIER         PLL2RDYIE       LL_RCC_IsEnabledIT_PLL2RDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsEnabledIT_PLL2RDY(void)
{
  return ((READ_BIT(RCC->CIER, RCC_CIER_PLL2RDYIE) == RCC_CIER_PLL2RDYIE) ? 1UL : 0UL);
}

/**
  * @brief  Checks if PLL3 ready interrupt source is enabled or disabled.
  * @rmtoll CIER         PLL3RDYIE       LL_RCC_IsEnabledIT_PLL3RDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsEnabledIT_PLL3RDY(void)
{
  return ((READ_BIT(RCC->CIER, RCC_CIER_PLL3RDYIE) == RCC_CIER_PLL3RDYIE) ? 1UL : 0UL);
}

/**
  * @brief  Checks if PLL4 ready interrupt source is enabled or disabled.
  * @rmtoll CIER         PLL4RDYIE       LL_RCC_IsEnabledIT_PLL4RDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsEnabledIT_PLL4RDY(void)
{
  return ((READ_BIT(RCC->CIER, RCC_CIER_PLL4RDYIE) == RCC_CIER_PLL4RDYIE) ? 1UL : 0UL);
}

/**
  * @brief  Checks if LSECSS interrupt source is enabled or disabled.
  * @rmtoll CIER         LSECSSIE        LL_RCC_IsEnabledIT_LSECSS
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsEnabledIT_LSECSS(void)
{
  return ((READ_BIT(RCC->CIER, RCC_CIER_LSECSSIE) == RCC_CIER_LSECSSIE) ? 1UL : 0UL);
}

/**
  * @brief  Checks if HSECSS interrupt source is enabled or disabled.
  * @rmtoll CIER         HSECSSIE        LL_RCC_IsEnabledIT_HSECSS
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsEnabledIT_HSECSS(void)
{
  return ((READ_BIT(RCC->CIER, RCC_CIER_HSECSSIE) == RCC_CIER_HSECSSIE) ? 1UL : 0UL);
}

/**
  * @brief  Checks if WKUP interrupt source is enabled or disabled.
  * @rmtoll CIER         WKUPIE          LL_RCC_IsEnabledIT_WKUP
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_IsEnabledIT_WKUP(void)
{
  return ((READ_BIT(RCC->CIER, RCC_CIER_WKUPIE) == RCC_CIER_WKUPIE) ? 1UL : 0UL);
}
/**
  * @}
  */

/** @defgroup RCC_LL_EF_Init De-initialization function
  * @{
  */
void LL_RCC_DeInit(void);
/**
  * @}
  */

/** @defgroup RCC_LL_EF_Get_Freq Get system and peripherals clocks frequency functions
  * @{
  */
void        LL_RCC_GetSystemClocksFreq(LL_RCC_ClocksTypeDef *RCC_Clocks);

uint32_t    LL_RCC_GetCpuClockFreq(void);
uint32_t    LL_RCC_GetSystemClockFreq(void);
uint32_t    LL_RCC_GetPLL1ClockFreq(void);
uint32_t    LL_RCC_GetPLL2ClockFreq(void);
uint32_t    LL_RCC_GetPLL3ClockFreq(void);
uint32_t    LL_RCC_GetPLL4ClockFreq(void);
uint32_t    LL_RCC_CalcPLLClockFreq(uint32_t PLLInputFreq, uint32_t M, uint32_t N, uint32_t FRACN, uint32_t P1,
                                    uint32_t P2);

uint32_t    LL_RCC_GetADCClockFreq(uint32_t ADCxSource);
uint32_t    LL_RCC_GetADFClockFreq(uint32_t ADFxSource);
uint32_t    LL_RCC_GetCLKPClockFreq(uint32_t CLKPxSource);
uint32_t    LL_RCC_GetDCMIPPClockFreq(uint32_t DCMIPPxSource);
uint32_t    LL_RCC_GetETHClockFreq(uint32_t ETHxSource);
uint32_t    LL_RCC_GetETHPTPClockFreq(uint32_t ETHxPTPSource);
uint32_t    LL_RCC_GetFDCANClockFreq(uint32_t FDCANxSource);
uint32_t    LL_RCC_GetFMCClockFreq(uint32_t FMCxSource);
uint32_t    LL_RCC_GetI2CClockFreq(uint32_t I2CxSource);
uint32_t    LL_RCC_GetI3CClockFreq(uint32_t I3CxSource);
uint32_t    LL_RCC_GetLPTIMClockFreq(uint32_t LPTIMxSource);
uint32_t    LL_RCC_GetLPUARTClockFreq(uint32_t LPUARTxSource);
uint32_t    LL_RCC_GetLTDCClockFreq(uint32_t LTDCxSource);
uint32_t    LL_RCC_GetMDFClockFreq(uint32_t MDFxSource);
uint32_t    LL_RCC_GetOTGPHYClockFreq(uint32_t OTGPHYxSource);
uint32_t    LL_RCC_GetOTGPHYCKREFClockFreq(uint32_t OTGPHYxCKREFSource);
uint32_t    LL_RCC_GetPSSIClockFreq(uint32_t PSSIxSource);
uint32_t    LL_RCC_GetSAIClockFreq(uint32_t SAIxSource);
uint32_t    LL_RCC_GetSDMMCClockFreq(uint32_t SDMMCxSource);
uint32_t    LL_RCC_GetSPDIFRXClockFreq(uint32_t SPDIFRXxSource);
uint32_t    LL_RCC_GetSPIClockFreq(uint32_t SPIxSource);
uint32_t    LL_RCC_GetUARTClockFreq(uint32_t UARTxSource);
uint32_t    LL_RCC_GetUSARTClockFreq(uint32_t USARTxSource);
uint32_t    LL_RCC_GetUSBClockFreq(uint32_t USBxSource);
uint32_t    LL_RCC_GetXSPIClockFreq(uint32_t XSPIxSource);

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

#endif /* STM32N6xx_LL_RCC_H */
