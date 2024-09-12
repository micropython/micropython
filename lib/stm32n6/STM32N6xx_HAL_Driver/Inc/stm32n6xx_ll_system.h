/**
  ******************************************************************************
  * @file    stm32n6xx_ll_system.h
  * @author  MCD Application Team
  * @brief   Header file of SYSTEM LL module.
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
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
    [..]
    The LL SYSTEM driver contains a set of generic APIs that can be
    used by user:
      (+) Some of the FLASH features need to be handled in the SYSTEM file.
      (+) Access to DBGCMU registers
      (+) Access to SYSCFG registers
  @endverbatim
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32N6xx_LL_SYSTEM_H
#define STM32N6xx_LL_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx.h"

/** @addtogroup STM32N6xx_LL_Driver
  * @{
  */

#if defined (SYSCFG) || defined (DBGMCU)

/** @defgroup SYSTEM_LL SYSTEM
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
/** @defgroup SYSTEM_LL_Private_Constants SYSTEM Private Constants
  * @{
  */

/** @defgroup SYSTEM_LL_EC_CS1 SYSCFG Vdd compensation cell Code selection
  * @{
  */
#define LL_SYSCFG_VDD_CELL_CODE                  0U               /*VDD I/Os code from the cell (available in the SYSCFG_CCVR)*/
#define LL_SYSCFG_VDD_REGISTER_CODE              SYSCFG_CCCSR_CS1 /*VDD I/Os code from the SYSCFG compensation cell code register (SYSCFG_CCCR)*/
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/** @defgroup SYSTEM_LL_Exported_Constants SYSTEM Exported Constants
  * @{
  */

/** @defgroup SYSTEM_LL_EC_I2C_FASTMODEPLUS SYSCFG I2C FASTMODEPLUS
  * @{
  */
#define LL_SYSCFG_I2C_FASTMODEPLUS_PA6     SYSCFG_CFGR1_PA6_FMP   /*!< Enable Fast Mode Plus on PA6  */
#define LL_SYSCFG_I2C_FASTMODEPLUS_PA7     SYSCFG_CFGR1_PA7_FMP   /*!< Enable Fast Mode Plus on PA7  */
#define LL_SYSCFG_I2C_FASTMODEPLUS_PA15    SYSCFG_CFGR1_PA15_FMP  /*!< Enable Fast Mode Plus on PA15 */
#define LL_SYSCFG_I2C_FASTMODEPLUS_PB3     SYSCFG_CFGR1_PB3_FMP   /*!< Enable Fast Mode Plus on PB3  */
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_TIMBREAK SYSCFG TIMER BREAK
  * @{
  */
#define LL_SYSCFG_TIMBREAK_ECC             SYSCFG_CFGR2_ECCL  /*!< Enables and locks the ECC error signal
                                                                   with Break Input of TIM1/16/17                       */
#define LL_SYSCFG_TIMBREAK_PVD             SYSCFG_CFGR2_PVDL  /*!< Enables and locks the PVD connection
                                                                   with TIM1/16/17 Break Input and also
                                                                   the PVDE and PLS bits of the Power Control Interface */
#define LL_SYSCFG_TIMBREAK_SRAM_ECC_LOCK   SYSCFG_CFGR2_SPL   /*!< Enables and locks the SRAM ECC double error signal
                                                                   with Break Input of TIM1/16/17                       */
#define LL_SYSCFG_TIMBREAK_LOCKUP          SYSCFG_CFGR2_CLL   /*!< Enables and locks the LOCKUP output of CortexM33
                                                                   with Break Input of TIM1/16/17                       */
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_SECURE_ATTRIBUTES Secure attributes
  * @note Only available when system implements security (TZEN=1)
  * @{
  */
#define LL_SYSCFG_CLOCK_SEC                SYSCFG_SECCFGR_SYSCFGSEC  /*!< SYSCFG clock configuration secure-only access */
#define LL_SYSCFG_CLOCK_NSEC               0U                        /*!< SYSCFG clock configuration secure/non-secure access */
#define LL_SYSCFG_CLASSB_SEC               SYSCFG_SECCFGR_CLASSBSEC  /*!< Class B configuration secure-only access */
#define LL_SYSCFG_CLASSB_NSEC              0U                        /*!< Class B configuration secure/non-secure access */
#define LL_SYSCFG_FPU_SEC                  SYSCFG_SECCFGR_FPUSEC     /*!< FPU configuration secure-only access */
#define LL_SYSCFG_FPU_NSEC                 0U                        /*!< FPU configuration secure/non-secure access */
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_TIMBREAK SYSCFG TIMER BREAK
  * @{
  */
#define LL_DBGMCU_DBTRGIN                  0U                  /*!< DBTRGIO connected to DBTRGIN   */
#define LL_DBGMCU_DBTRGOUT                 DBGMCU_CR_DBTRGOEN  /*!<  DBTRGIO connected to DBTRGOUT */
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_APB1_GRP1_STOP_IP DBGMCU APB1 GRP1 STOP IP
  * @{
  */
#define LL_DBGMCU_APB1_GRP1_TIM2_STOP          DBGMCU_APB1LFZ1_DBG_TIM2_STOP
#define LL_DBGMCU_APB1_GRP1_TIM3_STOP          DBGMCU_APB1LFZ1_DBG_TIM3_STOP         /*!< TIM3 stop in debug */
#define LL_DBGMCU_APB1_GRP1_TIM4_STOP          DBGMCU_APB1LFZ1_DBG_TIM4_STOP         /*!< TIM4 stop in debug */
#define LL_DBGMCU_APB1_GRP1_TIM5_STOP          DBGMCU_APB1LFZ1_DBG_TIM5_STOP         /*!< TIM5 stop in debug */
#define LL_DBGMCU_APB1_GRP1_TIM6_STOP          DBGMCU_APB1LFZ1_DBG_TIM6_STOP         /*!< TIM6 stop in debug */
#define LL_DBGMCU_APB1_GRP1_TIM7_STOP          DBGMCU_APB1LFZ1_DBG_TIM7_STOP         /*!< TIM7 stop in debug */
#define LL_DBGMCU_APB1_GRP1_TIM12_STOP         DBGMCU_APB1LFZ1_DBG_TIM12_STOP        /*!< TIM12 stop in debug */
#define LL_DBGMCU_APB1_GRP1_TIM13_STOP         DBGMCU_APB1LFZ1_DBG_TIM13_STOP        /*!< TIM13 stop in debug */
#define LL_DBGMCU_APB1_GRP1_TIM14_STOP         DBGMCU_APB1LFZ1_DBG_TIM14_STOP        /*!< TIM14 stop in debug */
#define LL_DBGMCU_APB1_GRP1_LPTIM1_STOP        DBGMCU_APB1LFZ1_DBG_LPTIM1_STOP       /*!< LPTIM1 stop in debug */
#define LL_DBGMCU_APB1_GRP1_WWDG_STOP          DBGMCU_APB1LFZ1_DBG_WWDG1_STOP        /*!< WWDG1 stop in debug */
#define LL_DBGMCU_APB1_GRP1_TIM10_STOP         DBGMCU_APB1LFZ1_DBG_TIM10_STOP        /*!< TIM10 stop in debug */
#define LL_DBGMCU_APB1_GRP1_TIM11_STOP         DBGMCU_APB1LFZ1_DBG_TIM11_STOP        /*!< TIM11 stop in debug */
#define LL_DBGMCU_APB1_GRP1_I2C1_STOP          DBGMCU_APB1LFZ1_DBG_I2C1_STOP         /*!< I2C1 SMBUS timeout stop in debug */
#define LL_DBGMCU_APB1_GRP1_I2C2_STOP          DBGMCU_APB1LFZ1_DBG_I2C2_STOP         /*!< I2C2 SMBUS timeout stop in debug */
#define LL_DBGMCU_APB1_GRP1_I2C3_STOP          DBGMCU_APB1LFZ1_DBG_I2C3_STOP         /*!< I2C3 SMBUS timeout stop in debug */
#define LL_DBGMCU_APB1_GRP1_I3C1_STOP          DBGMCU_APB1LFZ1_DBG_I3C1_STOP         /*!< I3C1 SMBUS timeout stop in debug */
#define LL_DBGMCU_APB1_GRP1_I3C2_STOP          DBGMCU_APB1LFZ1_DBG_I3C2_STOP         /*!< I3C2 SMBUS timeout stop in debug */
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_APB1_GRP2_STOP_IP DBGMCU APB1 GRP2 STOP IP
  * @{
  */
#define LL_DBGMCU_APB1_GRP2_FDCAN_STOP         DBGMCU_APB1HFZ1_DBG_FDCAN_STOP       /*!< FDCAN stop in debug */
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_APB2_GRP1_STOP_IP DBGMCU APB2 GRP1 STOP IP
  * @{
  */
#define LL_DBGMCU_APB2_GRP1_TIM1_STOP         DBGMCU_APB2FZ1_DBG_TIM1_STOP            /*!< TIM1 stop in debug */
#define LL_DBGMCU_APB2_GRP1_TIM8_STOP         DBGMCU_APB2FZ1_DBG_TIM8_STOP            /*!< TIM8 stop in debug */
#define LL_DBGMCU_APB2_GRP1_TIM18_STOP        DBGMCU_APB2FZ1_DBG_TIM18_STOP           /*!< TIM18 stop in debug */
#define LL_DBGMCU_APB2_GRP1_TIM15_STOP        DBGMCU_APB2FZ1_DBG_TIM15_STOP           /*!< TIM15 stop in debug */
#define LL_DBGMCU_APB2_GRP1_TIM16_STOP        DBGMCU_APB2FZ1_DBG_TIM16_STOP           /*!< TIM16 stop in debug */
#define LL_DBGMCU_APB2_GRP1_TIM17_STOP        DBGMCU_APB2FZ1_DBG_TIM17_STOP           /*!< TIM17 stop in debug */
#define LL_DBGMCU_APB2_GRP1_TIM9_STOP         DBGMCU_APB2FZ1_DBG_TIM9_STOP            /*!< TIM9 stop in debug */
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_APB4_GRP1_STOP_IP DBGMCU APB4 GRP1 STOP IP
  * @{
  */
#define LL_DBGMCU_APB4_GRP1_I2C4_STOP         DBGMCU_APB4FZ1_DBG_I2C4_STOP            /*!< I2C4 stop in debug */
#define LL_DBGMCU_APB4_GRP1_LPTIM2_STOP       DBGMCU_APB4FZ1_DBG_LPTIM2_STOP          /*!< LPTIM2 stop in debug */
#define LL_DBGMCU_APB4_GRP1_LPTIM3_STOP       DBGMCU_APB4FZ1_DBG_LPTIM3_STOP          /*!< LPTIM3 stop in debug */
#define LL_DBGMCU_APB4_GRP1_LPTIM4_STOP       DBGMCU_APB4FZ1_DBG_LPTIM4_STOP          /*!< LPTIM4 stop in debug */
#define LL_DBGMCU_APB4_GRP1_LPTIM5_STOP       DBGMCU_APB4FZ1_DBG_LPTIM5_STOP          /*!< LPTIM5 stop in debug */
#define LL_DBGMCU_APB4_GRP1_RTC_STOP          DBGMCU_APB4FZ1_DBG_RTC_STOP             /*!< RTC stop in debug */
#define LL_DBGMCU_APB4_GRP1_IWDG_STOP         DBGMCU_APB4FZ1_DBG_IWDG_STOP            /*!< IWDG stop in debug */
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_APB5_GRP1_STOP_IP DBGMCU APB5 GRP1 STOP IP
  * @{
  */
#define LL_DBGMCU_APB5_GRP1_GFXTIM_STOP       DBGMCU_APB5FZ1_DBG_GFXTIM_STOP          /*!< GFXTIM stop in debug */
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_AHB1_GRP1_STOP_IP DBGMCU AHB1 GRP1 STOP IP
  * @{
  */
#define LL_DBGMCU_AHB1_GRP1_GPDMA1_CH0_STOP   DBGMCU_AHB1FZ1_DBG_GPDMA1_CH0_STOP      /*!< GPDMA1_CH0 suspend in debug */
#define LL_DBGMCU_AHB1_GRP1_GPDMA1_CH1_STOP   DBGMCU_AHB1FZ1_DBG_GPDMA1_CH1_STOP      /*!< GPDMA1_CH1 suspend in debug */
#define LL_DBGMCU_AHB1_GRP1_GPDMA1_CH2_STOP   DBGMCU_AHB1FZ1_DBG_GPDMA1_CH2_STOP      /*!< GPDMA1_CH2 suspend in debug */
#define LL_DBGMCU_AHB1_GRP1_GPDMA1_CH3_STOP   DBGMCU_AHB1FZ1_DBG_GPDMA1_CH3_STOP      /*!< GPDMA1_CH3 suspend in debug */
#define LL_DBGMCU_AHB1_GRP1_GPDMA1_CH4_STOP   DBGMCU_AHB1FZ1_DBG_GPDMA1_CH4_STOP      /*!< GPDMA1_CH4 suspend in debug */
#define LL_DBGMCU_AHB1_GRP1_GPDMA1_CH5_STOP   DBGMCU_AHB1FZ1_DBG_GPDMA1_CH5_STOP      /*!< GPDMA1_CH5 suspend in debug */
#define LL_DBGMCU_AHB1_GRP1_GPDMA1_CH6_STOP   DBGMCU_AHB1FZ1_DBG_GPDMA1_CH6_STOP      /*!< GPDMA1_CH6 suspend in debug */
#define LL_DBGMCU_AHB1_GRP1_GPDMA1_CH7_STOP   DBGMCU_AHB1FZ1_DBG_GPDMA1_CH7_STOP      /*!< GPDMA1_CH7 suspend in debug */
#define LL_DBGMCU_AHB1_GRP1_GPDMA1_CH8_STOP   DBGMCU_AHB1FZ1_DBG_GPDMA1_CH8_STOP      /*!< GPDMA1_CH8 suspend in debug */
#define LL_DBGMCU_AHB1_GRP1_GPDMA1_CH9_STOP   DBGMCU_AHB1FZ1_DBG_GPDMA1_CH9_STOP      /*!< GPDMA1_CH9 suspend in debug */
#define LL_DBGMCU_AHB1_GRP1_GPDMA1_CH10_STOP  DBGMCU_AHB1FZ1_DBG_GPDMA1_CH10_STOP     /*!< GPDMA1_CH10 suspend in debug */
#define LL_DBGMCU_AHB1_GRP1_GPDMA1_CH11_STOP  DBGMCU_AHB1FZ1_DBG_GPDMA1_CH11_STOP     /*!< GPDMA1_CH11 suspend in debug */
#define LL_DBGMCU_AHB1_GRP1_GPDMA1_CH12_STOP  DBGMCU_AHB1FZ1_DBG_GPDMA1_CH12_STOP     /*!< GPDMA1_CH12 suspend in debug */
#define LL_DBGMCU_AHB1_GRP1_GPDMA1_CH13_STOP  DBGMCU_AHB1FZ1_DBG_GPDMA1_CH13_STOP     /*!< GPDMA1_CH13 suspend in debug */
#define LL_DBGMCU_AHB1_GRP1_GPDMA1_CH14_STOP  DBGMCU_AHB1FZ1_DBG_GPDMA1_CH14_STOP     /*!< GPDMA1_CH14 suspend in debug */
#define LL_DBGMCU_AHB1_GRP1_GPDMA1_CH15_STOP  DBGMCU_AHB1FZ1_DBG_GPDMA1_CH15_STOP     /*!< GPDMA1_CH15 suspend in debug */
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_AHB5_GRP1_STOP_IP DBGMCU AHB5 GRP1 STOP IP
  * @{
  */
#define LL_DBGMCU_AHB5_GRP1_HPDMA1_CH0_STOP   DBGMCU_AHB5FZ1_DBG_HPDMA1_CH0_STOP      /*!< HPDMA1_CH0 suspend in debug */
#define LL_DBGMCU_AHB5_GRP1_HPDMA1_CH1_STOP   DBGMCU_AHB5FZ1_DBG_HPDMA1_CH1_STOP      /*!< HPDMA1_CH1 suspend in debug */
#define LL_DBGMCU_AHB5_GRP1_HPDMA1_CH2_STOP   DBGMCU_AHB5FZ1_DBG_HPDMA1_CH2_STOP      /*!< HPDMA1_CH2 suspend in debug */
#define LL_DBGMCU_AHB5_GRP1_HPDMA1_CH3_STOP   DBGMCU_AHB5FZ1_DBG_HPDMA1_CH3_STOP      /*!< HPDMA1_CH3 suspend in debug */
#define LL_DBGMCU_AHB5_GRP1_HPDMA1_CH4_STOP   DBGMCU_AHB5FZ1_DBG_HPDMA1_CH4_STOP      /*!< HPDMA1_CH4 suspend in debug */
#define LL_DBGMCU_AHB5_GRP1_HPDMA1_CH5_STOP   DBGMCU_AHB5FZ1_DBG_HPDMA1_CH5_STOP      /*!< HPDMA1_CH5 suspend in debug */
#define LL_DBGMCU_AHB5_GRP1_HPDMA1_CH6_STOP   DBGMCU_AHB5FZ1_DBG_HPDMA1_CH6_STOP      /*!< HPDMA1_CH6 suspend in debug */
#define LL_DBGMCU_AHB5_GRP1_HPDMA1_CH7_STOP   DBGMCU_AHB5FZ1_DBG_HPDMA1_CH7_STOP      /*!< HPDMA1_CH7 suspend in debug */
#define LL_DBGMCU_AHB5_GRP1_HPDMA1_CH8_STOP   DBGMCU_AHB5FZ1_DBG_HPDMA1_CH8_STOP      /*!< HPDMA1_CH8 suspend in debug */
#define LL_DBGMCU_AHB5_GRP1_HPDMA1_CH9_STOP   DBGMCU_AHB5FZ1_DBG_HPDMA1_CH9_STOP      /*!< HPDMA1_CH9 suspend in debug */
#define LL_DBGMCU_AHB5_GRP1_HPDMA1_CH10_STOP  DBGMCU_AHB5FZ1_DBG_HPDMA1_CH10_STOP     /*!< HPDMA1_CH10 suspend in debug */
#define LL_DBGMCU_AHB5_GRP1_HPDMA1_CH11_STOP  DBGMCU_AHB5FZ1_DBG_HPDMA1_CH11_STOP     /*!< HPDMA1_CH11 suspend in debug */
#define LL_DBGMCU_AHB5_GRP1_HPDMA1_CH12_STOP  DBGMCU_AHB5FZ1_DBG_HPDMA1_CH12_STOP     /*!< HPDMA1_CH12 suspend in debug */
#define LL_DBGMCU_AHB5_GRP1_HPDMA1_CH13_STOP  DBGMCU_AHB5FZ1_DBG_HPDMA1_CH13_STOP     /*!< HPDMA1_CH13 suspend in debug */
#define LL_DBGMCU_AHB5_GRP1_HPDMA1_CH14_STOP  DBGMCU_AHB5FZ1_DBG_HPDMA1_CH14_STOP     /*!< HPDMA1_CH14 suspend in debug */
#define LL_DBGMCU_AHB5_GRP1_HPDMA1_CH15_STOP  DBGMCU_AHB5FZ1_DBG_HPDMA1_CH15_STOP     /*!< HPDMA1_CH15 suspend in debug */
#define LL_DBGMCU_AHB5_GRP1_NPU_STOP          DBGMCU_AHB5FZ1_NPU_DBG_FREEZE           /*!< NPU stop in debug mode */
/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @defgroup SYSTEM_LL_Exported_Functions SYSTEM Exported Functions
  * @{
  */


#if defined (CPU_IN_SECURE_STATE)
/**
  * @brief  Returns the device identifier
  * @note The BSEC clock must be enabled before calling this function
  *       Returned Device ID can be
  *       0x00006200 for STM32N645xx
  *       0x00006000 for STM32N655xx
  *       0x00002200 for STM32N647xx
  *       0x00002000 for STM32N657xx
  *       0xffffffff if an error occurs
  * @retval Device identifier
  */
__STATIC_INLINE uint32_t LL_GetDeviceID(void)
{
  return (uint32_t)(READ_REG(BSEC->FVRw[9]));
}


/**
  * @brief  Returns the device revision identifier
  * @note Returned Revision ID can be
  *       0x00000100 for Cut1.0
  *       0x00000101 for Cut1.1
  *       0x00000200 for Cut2.0
  * @retval Device revision identifier
  */
__STATIC_INLINE uint32_t LL_GetRevisionID(void)
{
  return *(uint32_t *)(REVID_BASE);
}
#endif /* CPU_IN_SECURE_STATE */

/** @defgroup SYSTEM_LL_EF_DBGMCU DBGMCU
  * @{
  */

/**
  * @brief  Enable the Debug Module during SLEEP mode
  * @rmtoll DBGMCU_CR    DBG_SLEEP      LL_DBGMCU_EnableDBGSleepMode
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_EnableDBGSleepMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_SLEEP);
}

/**
  * @brief  Disable the Debug Module during SLEEP mode
  * @rmtoll DBGMCU_CR    DBG_SLEEP      LL_DBGMCU_DisableDBGSleepMode
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_DisableDBGSleepMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_SLEEP);
}

/**
  * @brief  Enable the Debug Module during STOP mode
  * @rmtoll DBGMCU_CR    DBG_STOP      LL_DBGMCU_EnableDBGStopMode
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_EnableDBGStopMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOP);
}

/**
  * @brief  Disable the Debug Module during STOP mode
  * @rmtoll DBGMCU_CR    DBG_STOP      LL_DBGMCU_DisableDBGStopMode
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_DisableDBGStopMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOP);
}

/**
  * @brief  Enable the Debug Module during STANDBY mode
  * @rmtoll DBGMCU_CR    DBG_STANDBY   LL_DBGMCU_EnableDBGStandbyMode
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_EnableDBGStandbyMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STANDBY);
}

/**
  * @brief  Disable the Debug Module during STANDBY mode
  * @rmtoll DBGMCU_CR    DBG_STANDBY   LL_DBGMCU_DisableDBGStandbyMode
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_DisableDBGStandbyMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STANDBY);
}

/**
  * @brief  Enable the Debug clock through software
  * @rmtoll DBGMCU_CR    DBGCLKEN   LL_DBGMCU_EnableDBGClock
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_EnableDBGClock(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBGCLKEN);
}

/**
  * @brief  Disable the Debug clock through software
  * @rmtoll DBGMCU_CR    DBGCLKEN   LL_DBGMCU_DisableDBGClock
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_DisableDBGClock(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBGCLKEN);
}

/**
  * @brief  Enable the TPIU export clock enable through software
  * @rmtoll DBGMCU_CR    TRACECLKEN   LL_DBGMCU_EnableTPIUExportClock
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_EnableTPIUExportClock(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_TRACECLKEN);
}

/**
  * @brief  Disable the TPIU export clock enable through software
  * @rmtoll DBGMCU_CR    TRACECLKEN   LL_DBGMCU_DisableTPIUExportClock
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_DisableTPIUExportClock(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_TRACECLKEN);
}

/**
  * @brief  Set DBTRGIO connection control
  * @rmtoll DBGMCU_CR    DBTRGOEN   LL_DBGMCU_SetDBTRGIOConnectionControl
  * @param  ConfigDBTRGIO This parameter can be one of the following values:
  *         @arg @ref LL_DBGMCU_DBTRGIN
  *         @arg @ref LL_DBGMCU_DBTRGOUT
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_SetDBTRGIOConnectionControl(uint32_t ConfigDBTRGIO)
{
  MODIFY_REG(DBGMCU->CR, DBGMCU_CR_DBTRGOEN, ConfigDBTRGIO);
}

/**
  * @brief  Get DBTRGIO connection control
  * @rmtoll DBGMCU_CR    DBTRGOEN   LL_DBGMCU_GetDBTRGIOConnectionControl
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DBGMCU_DBTRGIN
  *         @arg @ref LL_DBGMCU_DBTRGOUT
  */
__STATIC_INLINE uint32_t LL_DBGMCU_GetDBTRGIOConnectionControl(void)
{
  return (uint32_t)(READ_BIT(DBGMCU->CR, DBGMCU_CR_DBTRGOEN));
}

/**
  * @brief  Enable TSGEN halt
  * @rmtoll DBGMCU_CR    HLT_TSGEN_EN   LL_DBGMCU_EnableTSGENHalt
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_EnableTSGENHalt(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_HLT_TSGEN_EN);
}

/**
  * @brief  Disable TSGEN halt
  * @rmtoll DBGMCU_CR    HLT_TSGEN_EN   LL_DBGMCU_DisableTSGENHalt
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_DisableTSGENHalt(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_HLT_TSGEN_EN);
}

/**
  * @brief  Freeze APB1 peripherals (group1 peripherals)
  * @rmtoll DBGMCU_APB1LFZ1 DBG_xxxx_STOP  LL_DBGMCU_APB1_GRP1_FreezePeriph
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM2_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM3_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM4_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM5_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM6_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM7_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM12_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM13_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM14_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_LPTIM1_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_WWDG_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM10_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM11_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_I2C1_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_I2C2_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_I2C3_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_I3C1_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_I3C2_STOP
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_APB1_GRP1_FreezePeriph(uint32_t Periphs)
{
  SET_BIT(DBGMCU->APB1LFZ1, Periphs);
}

/**
  * @brief  Unfreeze APB1 peripherals (group1 peripherals)
  * @rmtoll DBGMCU_APB1LFZ DBG_xxxx_STOP  LL_DBGMCU_APB1_GRP1_UnFreezePeriph
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM2_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM3_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM4_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM5_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM6_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM7_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM12_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM13_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM14_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_LPTIM1_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_WWDG_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM10_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM11_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_I2C1_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_I2C2_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_I2C3_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_I3C1_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_I3C2_STOP
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_APB1_GRP1_UnFreezePeriph(uint32_t Periphs)
{
  CLEAR_BIT(DBGMCU->APB1LFZ1, Periphs);
}

/**
  * @brief  Freeze APB1 peripherals (group2 peripherals)
  * @rmtoll DBGMCU_APB1HFZ1 DBG_xxxx_STOP  LL_DBGMCU_APB1_GRP2_FreezePeriph
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_DBGMCU_APB1_GRP2_FDCAN_STOP
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_APB1_GRP2_FreezePeriph(uint32_t Periphs)
{
  CLEAR_BIT(DBGMCU->APB1HFZ1, Periphs);
}

/**
  * @brief  Unfreeze APB1 peripherals (group2 peripherals)
  * @rmtoll DBGMCU_APB1HFZ1 DBG_xxxx_STOP  LL_DBGMCU_APB1_GRP2_UnFreezePeriph
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_DBGMCU_APB1_GRP2_FDCAN_STOP
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_APB1_GRP2_UnFreezePeriph(uint32_t Periphs)
{
  CLEAR_BIT(DBGMCU->APB1HFZ1, Periphs);
}

/**
  * @brief  Freeze APB2 peripherals
  * @rmtoll DBGMCU_APB2FZ1 DBG_TIMx_STOP  LL_DBGMCU_APB2_FreezePeriph
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_DBGMCU_APB2_GRP1_TIM1_STOP
  *         @arg @ref LL_DBGMCU_APB2_GRP1_TIM8_STOP
  *         @arg @ref LL_DBGMCU_APB2_GRP1_TIM18_STOP
  *         @arg @ref LL_DBGMCU_APB2_GRP1_TIM15_STOP
  *         @arg @ref LL_DBGMCU_APB2_GRP1_TIM16_STOP
  *         @arg @ref LL_DBGMCU_APB2_GRP1_TIM17_STOP
  *         @arg @ref LL_DBGMCU_APB2_GRP1_TIM9_STOP
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_APB2_FreezePeriph(uint32_t Periphs)
{
  SET_BIT(DBGMCU->APB2FZ1, Periphs);
}

/**
  * @brief  Unfreeze APB2 peripherals
  * @rmtoll DBGMCU_APB2FZ1 DBG_TIMx_STOP  LL_DBGMCU_APB2_UnFreezePeriph
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_DBGMCU_APB2_GRP1_TIM1_STOP
  *         @arg @ref LL_DBGMCU_APB2_GRP1_TIM8_STOP
  *         @arg @ref LL_DBGMCU_APB2_GRP1_TIM18_STOP
  *         @arg @ref LL_DBGMCU_APB2_GRP1_TIM15_STOP
  *         @arg @ref LL_DBGMCU_APB2_GRP1_TIM16_STOP
  *         @arg @ref LL_DBGMCU_APB2_GRP1_TIM17_STOP
  *         @arg @ref LL_DBGMCU_APB2_GRP1_TIM9_STOP
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_APB2_UnFreezePeriph(uint32_t Periphs)
{
  SET_BIT(DBGMCU->APB2FZ1, Periphs);
}

/**
  * @brief  Freeze APB4 peripherals
  * @rmtoll DBGMCU_APB4FZ1 DBG_TIMx_STOP  LL_DBGMCU_APB4_FreezePeriph
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_DBGMCU_APB4_GRP1_I2C4_STOP
  *         @arg @ref LL_DBGMCU_APB4_GRP1_LPTIM2_STOP
  *         @arg @ref LL_DBGMCU_APB4_GRP1_LPTIM3_STOP
  *         @arg @ref LL_DBGMCU_APB4_GRP1_LPTIM4_STOP
  *         @arg @ref LL_DBGMCU_APB4_GRP1_LPTIM5_STOP
  *         @arg @ref LL_DBGMCU_APB4_GRP1_RTC_STOP
  *         @arg @ref LL_DBGMCU_APB4_GRP1_IWDG_STOP
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_APB4_FreezePeriph(uint32_t Periphs)
{
  SET_BIT(DBGMCU->APB4FZ1, Periphs);
}

/**
  * @brief  Unreeze APB4 peripherals
  * @rmtoll DBGMCU_APB4FZ1 DBG_TIMx_STOP  LL_DBGMCU_APB4_UnFreezePeriph
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_DBGMCU_APB4_GRP1_I2C4_STOP
  *         @arg @ref LL_DBGMCU_APB4_GRP1_LPTIM2_STOP
  *         @arg @ref LL_DBGMCU_APB4_GRP1_LPTIM3_STOP
  *         @arg @ref LL_DBGMCU_APB4_GRP1_LPTIM4_STOP
  *         @arg @ref LL_DBGMCU_APB4_GRP1_LPTIM5_STOP
  *         @arg @ref LL_DBGMCU_APB4_GRP1_RTC_STOP
  *         @arg @ref LL_DBGMCU_APB4_GRP1_IWDG_STOP
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_APB4_UnFreezePeriph(uint32_t Periphs)
{
  SET_BIT(DBGMCU->APB4FZ1, Periphs);
}

/**
  * @brief  Freeze APB5 peripherals
  * @rmtoll DBGMCU_APB5FZ1 DBG_TIMx_STOP  LL_DBGMCU_APB5_FreezePeriph
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_DBGMCU_APB5_GRP1_GFXTIM_STOP
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_APB5_FreezePeriph(uint32_t Periphs)
{
  SET_BIT(DBGMCU->APB5FZ1, Periphs);
}

/**
  * @brief  Unreeze APB5 peripherals
  * @rmtoll DBGMCU_APB5FZ1 DBG_TIMx_STOP  LL_DBGMCU_APB5_UnFreezePeriph
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_DBGMCU_APB5_GRP1_GFXTIM_STOP
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_APB5_UnFreezePeriph(uint32_t Periphs)
{
  SET_BIT(DBGMCU->APB5FZ1, Periphs);
}

/**
  * @brief  Freeze AHB1 peripherals
  * @rmtoll DBGMCU_AHB1FZ1 DBG_TIMx_STOP  LL_DBGMCU_AHB1_FreezePeriph
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH0_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH1_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH2_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH3_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH4_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH5_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH6_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH7_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH8_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH9_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH10_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH11_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH12_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH13_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH14_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH15_STOP
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_AHB1_FreezePeriph(uint32_t Periphs)
{
  SET_BIT(DBGMCU->AHB1FZ1, Periphs);
}

/**
  * @brief  Unreeze AHB1 peripherals
  * @rmtoll DBGMCU_AHB1FZ1 DBG_TIMx_STOP  LL_DBGMCU_AHB1_UnFreezePeriph
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH0_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH1_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH2_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH3_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH4_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH5_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH6_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH7_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH8_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH9_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH10_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH11_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH12_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH13_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH14_STOP
  *         @arg @ref LL_DBGMCU_AHB1_GRP1_GPDMA1_CH15_STOP
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_AHB1_UnFreezePeriph(uint32_t Periphs)
{
  SET_BIT(DBGMCU->AHB1FZ1, Periphs);
}

/**
  * @brief  Freeze AHB5 peripherals
  * @rmtoll DBGMCU_AHB5FZ1 DBG_TIMx_STOP  LL_DBGMCU_AHB5_FreezePeriph
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH0_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH1_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH2_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH3_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH4_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH5_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH6_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH7_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH8_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH9_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH10_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH11_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH12_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH13_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH14_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH15_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_NPU_STOP
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_AHB5_FreezePeriph(uint32_t Periphs)
{
  SET_BIT(DBGMCU->AHB5FZ1, Periphs);
}

/**
  * @brief  Unreeze AHB5 peripherals
  * @rmtoll DBGMCU_AHB5FZ1 DBG_TIMx_STOP  LL_DBGMCU_AHB5_UnFreezePeriph
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH0_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH1_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH2_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH3_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH4_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH5_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH6_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH7_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH8_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH9_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH10_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH11_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH12_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH13_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH14_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_HPDMA1_CH15_STOP
  *         @arg @ref LL_DBGMCU_AHB5_GRP1_NPU_STOP
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_AHB5_UnFreezePeriph(uint32_t Periphs)
{
  SET_BIT(DBGMCU->AHB5FZ1, Periphs);
}


/**
  * @}
  */

#endif /* defined (SYSCFG) || defined (DBGMCU) */

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

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32N6xx_LL_SYSTEM_H */
