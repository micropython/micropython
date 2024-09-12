/**
  ******************************************************************************
  * @file    stm32n6xx_ll_pwr.h
  * @author  MCD Application Team
  * @brief   Header file of PWR LL module.
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
#ifndef STM32N6xx_LL_PWR_H
#define STM32N6xx_LL_PWR_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx.h"

/** @addtogroup STM32N6xx_LL_Driver
  * @{
  */

#if defined (PWR)

/** @defgroup PWR_LL PWR
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/

/** @defgroup PWR_LL_Private_Constants PWR Private Constants
  * @{
  */

/** @defgroup PWR_LL_WAKEUP_PIN_OFFSET Wake-Up Pins register offsets Defines
  * @brief    Flags defines which can be used with LL_PWR_WriteReg function
  * @{
  */
/* Wake-Up Pins PWR register offsets */
#define LL_PWR_WAKEUP_PINS_PULL_SHIFT_OFFSET    (2UL)
#define LL_PWR_WAKEUP_PINS_MAX_SHIFT_MASK       (0x1FU)
/**
  * @}
  */

/**
  * @}
  */


/* Private macros ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/** @defgroup PWR_LL_Exported_Constants PWR Exported Constants
  * @{
  */

/** @defgroup PWR_LL_EC_SUPPLY_PWR Power supply source configuration
  * @{
  */
#define LL_PWR_SMPS_SUPPLY             PWR_CR1_SDEN                            /*!< Core domains are supplied from the SMPS                                     */
#define LL_PWR_EXTERNAL_SOURCE_SUPPLY  (0U)                                    /*!< The SMPS is Bypassed. The Core domains are supplied from an external source */
/**
  * @}
  */

/** @defgroup PWR_LL_EC_SUPPLY_PWR Power supply source configuration
  * @{
  */
#define LL_PWR_VCOREMON_LEVEL_SCALE1  (0U)                                     /*!< VDDCORE low-voltage threshold 1 selected (VOS 1) */
#define LL_PWR_VCOREMON_LEVEL_SCALE0  PWR_CR3_VCORELLS                         /*!< VDDCORE low-voltage threshold 2 selected (VOS 0) */
/**
  * @}
  */

/** @defgroup PWR_LL_EC_REGU_VOLTAGE Run mode Regulator Voltage Scaling
  * @{
  */
#define LL_PWR_REGU_VOLTAGE_SCALE0     PWR_VOSCR_VOS                           /*!< Voltage scaling range 0 (highest frequency) */
#define LL_PWR_REGU_VOLTAGE_SCALE1     (0U)                                    /*!< Voltage scaling range 1 (lowest power)      */
/**
  * @}
  */

/** @defgroup PWR_LL_EC_MODE_PWR Power Down Mode In Deep Sleep Mode
  * @{
  */
#define LL_PWR_POWERDOWN_MODE_DS_STOP     (0U)                                 /*!< Enter to Stop mode when the CPU enters deepsleep    */
#define LL_PWR_POWERDOWN_MODE_DS_STANDBY  PWR_CPUCR_PDDS                       /*!< Enter to Standby mode when the CPU enters deepsleep */
/**
  * @}
  */

/** @defgroup PWR_LL_EC_STOP_MODE_REGU_VOLTAGE Stop mode Regulator Voltage Scaling
  * @{
  */
#define LL_PWR_REGU_STOP_VOLTAGE_SCALE3     PWR_CPUCR_SVOS                     /*!< Voltage scaling range 3 (highest frequency) when system enters STOP mode */
#define LL_PWR_REGU_STOP_VOLTAGE_SCALE5     (0U)                               /*!< Voltage scaling range 5 (lowest power) when system enters STOP mode      */
/**
  * @}
  */

/** @defgroup PWR_LL_EC_VDDIO_VOLTAGE_RANGE VDD I/O voltage range selection
  * @{
  */
#define LL_PWR_VDDIO_VOLTAGE_RANGE_1V8    (1U)                                 /*!< 1v8 voltage range */
#define LL_PWR_VDDIO_VOLTAGE_RANGE_3V3    (0U)                                 /*!< 3v3 voltage range  */
/**
  * @}
  */

/** @defgroup PWR_LL_EC_WAKEUP_PIN  Wakeup Pins
  * @{
  */
#define LL_PWR_WAKEUP_PIN1             PWR_WKUPEPR_WKUPEN1                     /*!< Wake-Up pin 1 : PA0  */
#define LL_PWR_WAKEUP_PIN2             PWR_WKUPEPR_WKUPEN2                     /*!< Wake-Up pin 2 : PA2  */
#define LL_PWR_WAKEUP_PIN3             PWR_WKUPEPR_WKUPEN3                     /*!< Wake-Up pin 3 : PC13 */
#define LL_PWR_WAKEUP_PIN4             PWR_WKUPEPR_WKUPEN4                     /*!< Wake-Up pin 4 : PD2  */
/**
  * @}
  */

/** @defgroup PWR_LL_EC_WAKEUP_PIN_PULL  Wakeup Pins pull configuration
  * @{
  */
#define LL_PWR_WAKEUP_PIN_NOPULL       (0UL)                                   /*!< Configure Wake-Up pin in no pull   */
#define LL_PWR_WAKEUP_PIN_PULLUP       (1UL)                                   /*!< Configure Wake-Up pin in pull Up   */
#define LL_PWR_WAKEUP_PIN_PULLDOWN     (2UL)                                   /*!< Configure Wake-Up pin in pull Down */
/**
  * @}
  */

/** @defgroup PWR_LL_EC_ITEMS_PRIVILEGE_ATTRIBUTE PWR Items Privilege Attribute
  * @{
  */
#define LL_PWR_PRIV0_NPRIV             (0U)                                    /*!< System supply configuration non privilege mode                       */
#define LL_PWR_PRIV0_PRIV              PWR_PRIVCFGR_PRIV0                      /*!< System supply configuration privilege mode                           */
#define LL_PWR_PRIV1_NPRIV             (0U)                                    /*!< Programmable voltage detector non privilege mode                     */
#define LL_PWR_PRIV1_PRIV              PWR_PRIVCFGR_PRIV1                      /*!< Programmable voltage detect privilege mode                           */
#define LL_PWR_PRIV2_NPRIV             (0U)                                    /*!< VDDCORE monitor non privilege mode                                   */
#define LL_PWR_PRIV2_PRIV              PWR_PRIVCFGR_PRIV2                      /*!< VDDCORE monitor privilege mode                                       */
#define LL_PWR_PRIV3_NPRIV             (0U)                                    /*!< I-TCM, D-TCM, and I-TCM FLEXMEM low power control non privilege mode */
#define LL_PWR_PRIV3_PRIV              PWR_PRIVCFGR_PRIV3                      /*!< I-TCM, D-TCM, and I-TCM FLEXMEM low power control privilege mode     */
#define LL_PWR_PRIV4_NPRIV             (0U)                                    /*!< Voltage scaling selection nprivilege mode                            */
#define LL_PWR_PRIV4_PRIV              PWR_PRIVCFGR_PRIV4                      /*!< Voltage scaling selection privilege mode                             */
#define LL_PWR_PRIV5_NPRIV             (0U)                                    /*!< Backup domain non privilege mode                                     */
#define LL_PWR_PRIV5_PRIV              PWR_PRIVCFGR_PRIV5                      /*!< Backup domain privilege mode                                         */
#define LL_PWR_PRIV6_NPRIV             (0U)                                    /*!< CPU power control non privilege mode                                 */
#define LL_PWR_PRIV6_PRIV              PWR_PRIVCFGR_PRIV6                      /*!< CPU power control privilege mode                                     */
#define LL_PWR_PRIV7_NPRIV             (0U)                                    /*!< Peripheral voltage monitor non privilege mode                        */
#define LL_PWR_PRIV7_PRIV              PWR_PRIVCFGR_PRIV7                      /*!< Peripheral voltage monitor privilege mode                            */
#define LL_PWR_WAKEUP_PIN1_NPRIV       (0U)                                    /*!< Wake up pin 1 non privilege mode                                     */
#define LL_PWR_WAKEUP_PIN1_PRIV        PWR_PRIVCFGR_WKUPPRIV1                  /*!< Wake up pin 1 privilege mode                                         */
#define LL_PWR_WAKEUP_PIN2_NPRIV       (0U)                                    /*!< Wake up pin 2 non privilege mode                                     */
#define LL_PWR_WAKEUP_PIN2_PRIV        PWR_PRIVCFGR_WKUPPRIV2                  /*!< Wake up pin 2 privilege mode                                         */
#define LL_PWR_WAKEUP_PIN3_NPRIV       (0U)                                    /*!< Wake up pin 3 non privilege mode                                     */
#define LL_PWR_WAKEUP_PIN3_PRIV        PWR_PRIVCFGR_WKUPPRIV3                  /*!< Wake up pin 3 privilege mode                                         */
#define LL_PWR_WAKEUP_PIN4_NPRIV       (0U)                                    /*!< Wake up pin 4 non privilege mode                                     */
#define LL_PWR_WAKEUP_PIN4_PRIV        PWR_PRIVCFGR_WKUPPRIV4                  /*!< Wake up pin 4 privilege mode                                         */
/**
  * @}
  */

/** @defgroup PWR_LL_EC_ITEMS_SECURE_ATTRIBUTE PWR Items Secure Attribute
  * @{
  */
#define LL_PWR_SEC0_NSEC               (0U)                                    /*!< System supply configuration non secure mode                       */
#define LL_PWR_SEC0_SEC                PWR_SECCFGR_SEC0                        /*!< System supply configuration secure mode                           */
#define LL_PWR_SEC1_NSEC               (0U)                                    /*!< Programmable voltage detector non secure mode                     */
#define LL_PWR_SEC1_SEC                PWR_SECCFGR_SEC1                        /*!< Programmable voltage detect secure mode                           */
#define LL_PWR_SEC2_NSEC               (0U)                                    /*!< VDDCORE monitor non secure mode                                   */
#define LL_PWR_SEC2_SEC                PWR_SECCFGR_SEC2                        /*!< VDDCORE monitor secure mode                                       */
#define LL_PWR_SEC3_NSEC               (0U)                                    /*!< I-TCM, D-TCM, and I-TCM FLEXMEM low power control non secure mode */
#define LL_PWR_SEC3_SEC                PWR_SECCFGR_SEC3                        /*!< I-TCM, D-TCM, and I-TCM FLEXMEM low power control secure mode     */
#define LL_PWR_SEC4_NSEC               (0U)                                    /*!< Voltage scaling selection nsecure mode                            */
#define LL_PWR_SEC4_SEC                PWR_SECCFGR_SEC4                        /*!< Voltage scaling selection secure mode                             */
#define LL_PWR_SEC5_NSEC               (0U)                                    /*!< Backup domain non secure mode                                     */
#define LL_PWR_SEC5_SEC                PWR_SECCFGR_SEC5                        /*!< Backup domain secure mode                                         */
#define LL_PWR_SEC6_NSEC               (0U)                                    /*!< CPU power control non secure mode                                 */
#define LL_PWR_SEC6_SEC                PWR_SECCFGR_SEC6                        /*!< CPU power control secure mode                                     */
#define LL_PWR_SEC7_NSEC               (0U)                                    /*!< Peripheral voltage monitor non secure mode                        */
#define LL_PWR_SEC7_SEC                PWR_SECCFGR_SEC7                        /*!< Peripheral voltage monitor secure mode                            */
#define LL_PWR_WAKEUP_PIN1_NSEC        (0U)                                    /*!< Wake up pin 1 non secure mode                                     */
#define LL_PWR_WAKEUP_PIN1_SEC         PWR_SECCFGR_WKUPSEC1                    /*!< Wake up pin 1 secure mode                                         */
#define LL_PWR_WAKEUP_PIN2_NSEC        (0U)                                    /*!< Wake up pin 2 non secure mode                                     */
#define LL_PWR_WAKEUP_PIN2_SEC         PWR_SECCFGR_WKUPSEC2                    /*!< Wake up pin 2 secure mode                                         */
#define LL_PWR_WAKEUP_PIN3_NSEC        (0U)                                    /*!< Wake up pin 3 non secure mode                                     */
#define LL_PWR_WAKEUP_PIN3_SEC         PWR_SECCFGR_WKUPSEC3                    /*!< Wake up pin 3 secure mode                                         */
#define LL_PWR_WAKEUP_PIN4_NSEC        (0U)                                    /*!< Wake up pin 4 non secure mode                                     */
#define LL_PWR_WAKEUP_PIN4_SEC         PWR_SECCFGR_WKUPSEC4                    /*!< Wake up pin 4 secure mode                                         */
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup PWR_LL_Exported_Functions PWR Exported Functions
  * @{
  */

/** @defgroup PWR_LL_EF_Configuration PWR Configuration
  * @{
  */

/**
  * @brief  Configure the PWR supply
  * @rmtoll CR1       SDEN             LL_PWR_ConfigSupply
  * @param  SupplySource This parameter can be one of the following values:
  *         @arg @ref LL_PWR_SMPS_SUPPLY
  *         @arg @ref LL_PWR_EXTERNAL_SOURCE_SUPPLY
  * @retval None
  */
__STATIC_INLINE void LL_PWR_ConfigSupply(uint32_t SupplySource)
{
  /* Set the power supply configuration */
  MODIFY_REG(PWR->CR1, PWR_CR1_SDEN, SupplySource);
}

/**
  * @brief  Get the PWR supply
  * @rmtoll CR1       SDEN             LL_PWR_GetSupply
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_PWR_SMPS_SUPPLY
  *         @arg @ref LL_PWR_EXTERNAL_SOURCE_SUPPLY
  */
__STATIC_INLINE uint32_t LL_PWR_GetSupply(void)
{
  /* Get the power supply configuration */
  return READ_BIT(PWR->CR1, PWR_CR1_SDEN);
}

/**
  * @brief  Enable the SMPS low-power mode.
  * @rmtoll CR1        LPDS08V         LL_PWR_EnableSMPSLPMode
  * @note This bit is used to keep the SMPS in PWM mode (MR)
  * in Stop SVOS 3.
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableSMPSLPMode(void)
{
  SET_BIT(PWR->CR1, PWR_CR1_LPDS08V);
}

/**
  * @brief  Disable the SMPS low-power mode.
  * @rmtoll CR1        LPDS08V         LL_PWR_DisableSMPSLPMode
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableSMPSLPMode(void)
{
  CLEAR_BIT(PWR->CR1, PWR_CR1_LPDS08V);
}

/**
  * @brief  Check if the SMPS low-power mode is enabled.
  * @rmtoll CR1        LPDS08V         LL_PWR_IsEnabledSMPSLPMode
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledSMPSLPMode(void)
{
  return ((READ_BIT(PWR->CR1, PWR_CR1_LPDS08V) == (PWR_CR1_LPDS08V)) ? 1UL : 0UL);
}

/**
  * @brief  Enable the pull down on output voltage during power-down mode.
  * @rmtoll CR2        MODE_PDN         LL_PWR_EnablePullDown
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnablePullDown(void)
{
  SET_BIT(PWR->CR1, PWR_CR1_MODE_PDN);
}

/**
  * @brief  Disable the pull down on output voltage during power-down mode.
  * @rmtoll CR1        MODE_PDN         LL_PWR_DisablePullDown
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisablePullDown(void)
{
  CLEAR_BIT(PWR->CR1, PWR_CR1_MODE_PDN);
}

/**
  * @brief  Check if the pull down on output voltage during power-down mode is enabled.
  * @rmtoll CR1        MODE_PDN         LL_PWR_IsEnabledPullDown
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledPullDown(void)
{
  return ((READ_BIT(PWR->CR1, PWR_CR1_MODE_PDN) == (PWR_CR1_MODE_PDN)) ? 1UL : 0UL);
}

/**
  * @brief  Set the pwr_on pulse low time
  * @rmtoll CR1        POPL            LL_PWR_SetPwrONPulseLowTime
  * @param  PulseTime Value between Min_Data = 0 and Max_Data = 31
  * @retval None
  */
__STATIC_INLINE void LL_PWR_SetPwrONPulseLowTime(uint32_t PulseTime)
{
  MODIFY_REG(PWR->CR1, PWR_CR1_POPL, (PulseTime << PWR_CR1_POPL_Pos));
}

/**
  * @brief  Get the pwr_on pulse low time
  * @rmtoll CR1        POPL            LL_PWR_GetPwrONPulseLowTime
  * @retval Value between Min_Data = 0 and Max_Data = 31
  */
__STATIC_INLINE uint32_t LL_PWR_GetPwrONPulseLowTime(void)
{
  return (uint32_t)(READ_BIT(PWR->CR1, PWR_CR1_POPL) >> PWR_CR1_POPL_Pos);
}

/**
  * @brief  Enable Power Voltage Detector
  * @rmtoll CR2        PVDEN           LL_PWR_EnablePVD
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnablePVD(void)
{
  SET_BIT(PWR->CR2, PWR_CR2_PVDEN);
}

/**
  * @brief  Disable Power Voltage Detector
  * @rmtoll CR2        PVDEN           LL_PWR_DisablePVD
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisablePVD(void)
{
  CLEAR_BIT(PWR->CR2, PWR_CR2_PVDEN);
}

/**
  * @brief  Check if Power Voltage Detector is enabled
  * @rmtoll CR2        PVDEN           LL_PWR_IsEnabledPVD
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledPVD(void)
{
  return ((READ_BIT(PWR->CR2, PWR_CR2_PVDEN) == (PWR_CR2_PVDEN)) ? 1UL : 0UL);
}

/**
  * @brief  Enable VCORE monitoring
  * @rmtoll CR3        VCOREMONEN      LL_PWR_EnableVCOREMonitoring
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableVCOREMonitoring(void)
{
  SET_BIT(PWR->CR3, PWR_CR3_VCOREMONEN);
}

/**
  * @brief  Disable VCORE monitoring
  * @rmtoll CR3        VCOREMONEN      LL_PWR_DisableVCOREMonitoring
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableVCOREMonitoring(void)
{
  CLEAR_BIT(PWR->CR3, PWR_CR3_VCOREMONEN);
}

/**
  * @brief  Check if the VCORE monitoring is enabled
  * @rmtoll CR3        VCOREMONEN      LL_PWR_IsEnabledVCOREMonitoring
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledVCOREMonitoring(void)
{
  return ((READ_BIT(PWR->CR3, PWR_CR3_VCOREMONEN) == (PWR_CR3_VCOREMONEN)) ? 1UL : 0UL);
}

/**
  * @brief  Set the VDDCORE voltage detector low-level selection
  * @rmtoll CR3        VCORELLS        LL_PWR_SetVCORELowLevelDetection
  * @param  Lowlevel This parameter can be one of the following values:
  *         @arg @ref LL_PWR_VCOREMON_LEVEL_SCALE1
  *         @arg @ref LL_PWR_VCOREMON_LEVEL_SCALE0
  * @retval None
  */
__STATIC_INLINE void LL_PWR_SetVCORELowLevelDetection(uint32_t Lowlevel)
{
  MODIFY_REG(PWR->CR3, PWR_CR3_VCORELLS, Lowlevel);
}

/**
  * @brief  Get the VDDCORE voltage detector low-level selection
  * @rmtoll CR3        VCORELLS        LL_PWR_GetVCORELowLevelDetection
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_PWR_VCOREMON_LEVEL_SCALE1
  *         @arg @ref LL_PWR_VCOREMON_LEVEL_SCALE0
  */
__STATIC_INLINE uint32_t LL_PWR_GetVCORELowLevelDetection(void)
{
  return (uint32_t)(READ_BIT(PWR->CR3, PWR_CR3_VCORELLS));
}

/**
  * @brief  Enable I-TCM and D-TCM RAMs retention in Standby mode.
  * @rmtoll CR4        TCMRBSEN        LL_PWR_EnableTCMSBRetention
  * @note   When this bit is set, I-TCM and D-TCM RAMs are supplied
  *         from backup regulator in Standby mode.
  *         When this bit is reset, I-TCM and D-TCM RAMs can still
  *         be used in Run and Stop modes, but their content is lost
  *         in Standby mode.
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableTCMSBRetention(void)
{
  SET_BIT(PWR->CR4, PWR_CR4_TCMRBSEN);
}

/**
  * @brief  Disable I-TCM and D-TCM RAMs retention in Standby mode.
  * @rmtoll CR4        TCMRBSEN        LL_PWR_DisableTCMSBRetention
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableTCMSBRetention(void)
{
  CLEAR_BIT(PWR->CR4, PWR_CR4_TCMRBSEN);
}

/**
  * @brief  Check if the I-TCM and D-TCM RAMs retention in Standby mode is enabled
  * @rmtoll CR4        TCMRBSEN          LL_PWR_IsEnabledTCMSBRetention
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledTCMSBRetention(void)
{
  return ((READ_BIT(PWR->CR4, PWR_CR4_TCMRBSEN) == (PWR_CR4_TCMRBSEN)) ? 1UL : 0UL);
}

/**
  * @brief  Enable I-TCM FLEXMEM retention in Standby mode.
  * @rmtoll CR4        TCMFLXRBSEN     LL_PWR_EnableTCMFLXSBRetention
  * @note   When this bit is set, the I-TCM FLEXMEM is supplied
  *         from backup regulator in Standby mode.
  *         When this bit is reset, the I-TCM FLEXMEM can still
  *         be used in Run and Stop modes, but its content is lost
  *         in Standby mode.
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableTCMFLXSBRetention(void)
{
  SET_BIT(PWR->CR4, PWR_CR4_TCMFLXRBSEN);
}

/**
  * @brief  Disable I-TCM FLEXMEM RAMs retention in Standby mode.
  * @rmtoll CR4        TCMFLXRBSEN     LL_PWR_DisableTCMFLXSBRetention
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableTCMFLXSBRetention(void)
{
  CLEAR_BIT(PWR->CR4, PWR_CR4_TCMFLXRBSEN);
}

/**
  * @brief  Check if the I-TCM FLEXMEM retention in Standby mode is enabled
  * @rmtoll CR4        TCMFLXRBSEN     LL_PWR_IsEnabledTCMFLXSBRetention
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledTCMFLXSBRetention(void)
{
  return ((READ_BIT(PWR->CR4, PWR_CR4_TCMFLXRBSEN) == (PWR_CR4_TCMFLXRBSEN)) ? 1UL : 0UL);
}

/**
  * @brief  Set the main internal Regulator output voltage
  * @rmtoll VOSCR       VOS            LL_PWR_SetRegulVoltageScaling
  * @param  VoltageScaling This parameter can be one of the following values:
  *         @arg @ref LL_PWR_REGU_VOLTAGE_SCALE0
  *         @arg @ref LL_PWR_REGU_VOLTAGE_SCALE1
  * @retval None
  */
__STATIC_INLINE void LL_PWR_SetRegulVoltageScaling(uint32_t VoltageScaling)
{
  MODIFY_REG(PWR->VOSCR, PWR_VOSCR_VOS, VoltageScaling);
}

/**
  * @brief  Get the main internal Regulator output voltage. Reflecting the last
  *         VOS value applied to the PMU.
  * @rmtoll VOSCR       ACTVOS         LL_PWR_GetRegulVoltageScaling
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_PWR_REGU_VOLTAGE_SCALE0
  *         @arg @ref LL_PWR_REGU_VOLTAGE_SCALE1
  */
__STATIC_INLINE uint32_t LL_PWR_GetRegulVoltageScaling(void)
{
  return ((READ_BIT(PWR->VOSCR, PWR_VOSCR_ACTVOS) == (PWR_VOSCR_ACTVOS)) ? 1UL : 0UL);
}

/**
  * @brief  Enable VBAT and Temperature monitoring
  * @rmtoll BDCR1      MONEN           LL_PWR_EnableMonitoring
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableMonitoring(void)
{
  SET_BIT(PWR->BDCR1, PWR_BDCR1_MONEN);
}

/**
  * @brief  Disable VBAT and Temperature monitoring
  * @rmtoll BDCR1      MONEN           LL_PWR_DisableMonitoring
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableMonitoring(void)
{
  CLEAR_BIT(PWR->BDCR1, PWR_BDCR1_MONEN);
}

/**
  * @brief  Check if the VBAT and Temperature monitoring is enabled
  * @rmtoll BDCR1      MONEN           LL_PWR_IsEnabledMonitoring
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledMonitoring(void)
{
  return ((READ_BIT(PWR->BDCR1, PWR_BDCR1_MONEN) == (PWR_BDCR1_MONEN)) ? 1UL : 0UL);
}

/**
  * @brief  Enable BKPSRAM retention in Standby mode.
  * @rmtoll BDCR2      BKPRBSEN        LL_PWR_EnableBkpSRAMSBRetention
  * @note   When this bit is set, the backup ram is supplied from backup regulator in Standby and
  *         VBAT modes.
  *         When this bit is reset, the backup ram can still be used in Run and Stop modes,
  *         but its content is lost in Standby and VBAT modes.
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableBkpSRAMSBRetention(void)
{
  SET_BIT(PWR->BDCR2, PWR_BDCR2_BKPRBSEN);
}

/**
  * @brief  Disable BKPSRAM retention in Standby mode.
  * @rmtoll BDCR2      BKPRBSEN           LL_PWR_DisableBkpSRAMSBRetention
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableBkpSRAMSBRetention(void)
{
  CLEAR_BIT(PWR->BDCR2, PWR_BDCR2_BKPRBSEN);
}

/**
  * @brief  Check if the BKPSRAM retention in Standby mode is enabled
  * @rmtoll BDCR2      BKPRBSEN           LL_PWR_IsEnabledBkpSRAMSBRetention
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledBkpSRAMSBRetention(void)
{
  return ((READ_BIT(PWR->BDCR2, PWR_BDCR2_BKPRBSEN) == (PWR_BDCR2_BKPRBSEN)) ? 1UL : 0UL);
}

/**
  * @brief  Enable access to the backup domain
  * @rmtoll DBPCR      DBP             LL_PWR_EnableBkUpAccess
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableBkUpAccess(void)
{
  SET_BIT(PWR->DBPCR, PWR_DBPCR_DBP);
}

/**
  * @brief  Disable access to the backup domain
  * @rmtoll DBPCR      DBP             LL_PWR_DisableBkUpAccess
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableBkUpAccess(void)
{
  CLEAR_BIT(PWR->DBPCR, PWR_DBPCR_DBP);
}

/**
  * @brief  Check if the backup domain is enabled
  * @rmtoll DBPCR      DBP             LL_PWR_IsEnabledBkUpAccess
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledBkUpAccess(void)
{
  return ((READ_BIT(PWR->DBPCR, PWR_DBPCR_DBP) == (PWR_DBPCR_DBP)) ? 1UL : 0UL);
}

/**
  * @brief  Set the internal Regulator output voltage in STOP mode
  * @rmtoll CPUCR      SVOS            LL_PWR_SetStopModeRegulVoltageScaling
  * @param  VoltageScaling This parameter can be one of the following values:
  *         @arg @ref LL_PWR_REGU_STOP_VOLTAGE_SCALE3
  *         @arg @ref LL_PWR_REGU_STOP_VOLTAGE_SCALE5
  * @retval None
  */
__STATIC_INLINE void LL_PWR_SetStopModeRegulVoltageScaling(uint32_t VoltageScaling)
{
  MODIFY_REG(PWR->CPUCR, PWR_CPUCR_SVOS, VoltageScaling);
}

/**
  * @brief  Get the internal Regulator output voltage in STOP mode
  * @rmtoll CPUCR      SVOS            LL_PWR_GetStopModeRegulVoltageScaling
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_PWR_REGU_STOP_VOLTAGE_SCALE3
  *         @arg @ref LL_PWR_REGU_STOP_VOLTAGE_SCALE5
  */
__STATIC_INLINE uint32_t LL_PWR_GetStopModeRegulVoltageScaling(void)
{
  return READ_BIT(PWR->CPUCR, PWR_CPUCR_SVOS);
}

/**
  * @brief  Set the Power Down mode when device enters deepsleep mode
  * @rmtoll CPUCR      PDDS            LL_PWR_SetPowerDownModeDS
  * @param  PDMode This parameter can be one of the following values:
  *         @arg @ref LL_PWR_POWERDOWN_MODE_DS_STOP
  *         @arg @ref LL_PWR_POWERDOWN_MODE_DS_STANDBY
  * @retval None
  */
__STATIC_INLINE void LL_PWR_SetPowerDownModeDS(uint32_t PDMode)
{
  MODIFY_REG(PWR->CPUCR, PWR_CPUCR_PDDS, PDMode);
}

/**
  * @brief  Get the Power Down mode when device enters deepsleep mode
  * @rmtoll CPUCR      PDDS            LL_PWR_GetPowerDownModeDS
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_PWR_POWERDOWN_MODE_DS_STOP
  *         @arg @ref LL_PWR_POWERDOWN_MODE_DS_STANDBY
  */
__STATIC_INLINE uint32_t LL_PWR_GetPowerDownModeDS(void)
{
  return (uint32_t)(READ_BIT(PWR->CPUCR, PWR_CPUCR_PDDS));
}

/**
  * @brief  Enable Vdd IO2 voltage supply
  * @rmtoll SVMCR3     VDDIO2SV        LL_PWR_EnableVddIO2
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableVddIO2(void)
{
  SET_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIO2SV);
}

/**
  * @brief  Disable Vdd IO2 voltage supply
  * @rmtoll SVMCR3     VDDIO2SV        LL_PWR_DisableVddIO2
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableVddIO2(void)
{
  CLEAR_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIO2SV);
}

/**
  * @brief  Check if the Vdd IO2 voltage supply is enabled
  * @rmtoll SVMCR3     VDDIO2SV        LL_PWR_IsEnabledVddIO2
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledVddIO2(void)
{
  return ((READ_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIO2SV) == (PWR_SVMCR3_VDDIO2SV)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Vdd IO3 voltage supply
  * @rmtoll SVMCR3     VDDIO3SV        LL_PWR_EnableVddIO3
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableVddIO3(void)
{
  SET_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIO3SV);
}

/**
  * @brief  Disable Vdd IO3 voltage supply
  * @rmtoll SVMCR3     VDDIO3SV        LL_PWR_DisableVddIO3
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableVddIO3(void)
{
  CLEAR_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIO3SV);
}

/**
  * @brief  Check if the Vdd IO3 voltage supply is enabled
  * @rmtoll SVMCR3     VDDIO3SV        LL_PWR_IsEnabledVddIO3
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledVddIO3(void)
{
  return ((READ_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIO3SV) == (PWR_SVMCR3_VDDIO3SV)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Vdd IO4 voltage supply
  * @rmtoll SVMCR1     VDDIO4SV        LL_PWR_EnableVddIO4
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableVddIO4(void)
{
  SET_BIT(PWR->SVMCR1, PWR_SVMCR1_VDDIO4SV);
}

/**
  * @brief  Disable Vdd IO4 voltage supply
  * @rmtoll SVMCR1     VDDIO4SV        LL_PWR_DisableVddIO4
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableVddIO4(void)
{
  CLEAR_BIT(PWR->SVMCR1, PWR_SVMCR1_VDDIO4SV);
}

/**
  * @brief  Check if the Vdd IO4 voltage supply is enabled
  * @rmtoll SVMCR1     VDDIO4SV        LL_PWR_IsEnabledVddIO4
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledVddIO4(void)
{
  return ((READ_BIT(PWR->SVMCR1, PWR_SVMCR1_VDDIO4SV) == (PWR_SVMCR1_VDDIO4SV)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Vdd IO5 voltage supply
  * @rmtoll SVMCR2     VDDIO5SV        LL_PWR_EnableVddIO5
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableVddIO5(void)
{
  SET_BIT(PWR->SVMCR2, PWR_SVMCR2_VDDIO5SV);
}

/**
  * @brief  Disable Vdd IO5 voltage supply
  * @rmtoll SVMCR2     VDDIO5SV        LL_PWR_DisableVddIO5
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableVddIO5(void)
{
  CLEAR_BIT(PWR->SVMCR2, PWR_SVMCR2_VDDIO5SV);
}

/**
  * @brief  Check if the Vdd IO5 voltage supply is enabled
  * @rmtoll SVMCR2     VDDIO5SV        LL_PWR_IsEnabledVddIO5
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledVddIO5(void)
{
  return ((READ_BIT(PWR->SVMCR2, PWR_SVMCR2_VDDIO5SV) == (PWR_SVMCR2_VDDIO5SV)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Vdd USB voltage supply
  * @rmtoll SVMCR3     USB33SV         LL_PWR_EnableVddUSB
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableVddUSB(void)
{
  SET_BIT(PWR->SVMCR3, PWR_SVMCR3_USB33SV);
}

/**
  * @brief  Disable Vdd USB voltage supply
  * @rmtoll SVMCR3     USB33SV         LL_PWR_DisableVddUSB
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableVddUSB(void)
{
  CLEAR_BIT(PWR->SVMCR3, PWR_SVMCR3_USB33SV);
}

/**
  * @brief  Check if the Vdd USB voltage supply is enabled
  * @rmtoll SVMCR3     USB33SV         LL_PWR_IsEnabledVddUSB
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledVddUSB(void)
{
  return ((READ_BIT(PWR->SVMCR3, PWR_SVMCR3_USB33SV) == (PWR_SVMCR3_USB33SV)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Vdd ADC voltage supply
  * @rmtoll SVMCR3     ASV             LL_PWR_EnableVddADC
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableVddADC(void)
{
  SET_BIT(PWR->SVMCR3, PWR_SVMCR3_ASV);
}

/**
  * @brief  Disable Vdd ADC voltage supply
  * @rmtoll SVMCR3     ASV             LL_PWR_DisableVddADC
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableVddADC(void)
{
  CLEAR_BIT(PWR->SVMCR3, PWR_SVMCR3_ASV);
}

/**
  * @brief  Check if the Vdd ADC voltage supply is enabled
  * @rmtoll SVMCR3     ASV             LL_PWR_IsEnabledVddADC
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledVddADC(void)
{
  return ((READ_BIT(PWR->SVMCR3, PWR_SVMCR3_ASV) == (PWR_SVMCR3_ASV)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Vdd IO2 voltage monitoring
  * @rmtoll SVMCR3     VDDIO2VMEN      LL_PWR_EnableVddIO2Monitoring
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableVddIO2Monitoring(void)
{
  SET_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIO2VMEN);
}

/**
  * @brief  Disable Vdd IO2 voltage monitoring
  * @rmtoll SVMCR3     VDDIO2VMEN      LL_PWR_DisableVddIO2Monitoring
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableVddIO2Monitoring(void)
{
  CLEAR_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIO2VMEN);
}

/**
  * @brief  Check if the Vdd IO2 voltage monitoring is enabled
  * @rmtoll SVMCR3     VDDIO2VMEN      LL_PWR_IsEnabledVddIO2Monitoring
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledVddIO2Monitoring(void)
{
  return ((READ_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIO2VMEN) == (PWR_SVMCR3_VDDIO2VMEN)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Vdd IO3 voltage monitoring
  * @rmtoll SVMCR3     VDDIO3VMEN      LL_PWR_EnableVddIO3Monitoring
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableVddIO3Monitoring(void)
{
  SET_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIO3VMEN);
}

/**
  * @brief  Disable Vdd IO3 voltage monitoring
  * @rmtoll SVMCR3     VDDIO3VMEN      LL_PWR_DisableVddIO3Monitoring
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableVddIO3Monitoring(void)
{
  CLEAR_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIO3VMEN);
}

/**
  * @brief  Check if the Vdd IO3 voltage monitoring is enabled
  * @rmtoll SVMCR3     VDDIO3VMEN      LL_PWR_IsEnabledVddIO3Monitoring
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledVddIO3Monitoring(void)
{
  return ((READ_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIO3VMEN) == (PWR_SVMCR3_VDDIO3VMEN)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Vdd IO4 voltage monitoring
  * @rmtoll SVMCR1     VDDIO4VMEN      LL_PWR_EnableVddIO4Monitoring
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableVddIO4Monitoring(void)
{
  SET_BIT(PWR->SVMCR1, PWR_SVMCR1_VDDIO4VMEN);
}

/**
  * @brief  Disable Vdd IO4 voltage monitoring
  * @rmtoll SVMCR1     VDDIO4VMEN      LL_PWR_DisableVddIO4Monitoring
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableVddIO4Monitoring(void)
{
  CLEAR_BIT(PWR->SVMCR1, PWR_SVMCR1_VDDIO4VMEN);
}

/**
  * @brief  Check if the Vdd IO4 voltage monitoring is enabled
  * @rmtoll SVMCR1     VDDIO4VMEN      LL_PWR_IsEnabledVddIO4Monitoring
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledVddIO4Monitoring(void)
{
  return ((READ_BIT(PWR->SVMCR1, PWR_SVMCR1_VDDIO4VMEN) == (PWR_SVMCR1_VDDIO4VMEN)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Vdd IO5 voltage monitoring
  * @rmtoll SVMCR2     VDDIO5VMEN      LL_PWR_EnableVddIO5Monitoring
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableVddIO5Monitoring(void)
{
  SET_BIT(PWR->SVMCR2, PWR_SVMCR2_VDDIO5VMEN);
}

/**
  * @brief  Disable Vdd IO5 voltage monitoring
  * @rmtoll SVMCR2     VDDIO5VMEN      LL_PWR_DisableVddIO5Monitoring
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableVddIO5Monitoring(void)
{
  CLEAR_BIT(PWR->SVMCR2, PWR_SVMCR2_VDDIO5VMEN);
}

/**
  * @brief  Check if the Vdd IO5 voltage monitoring is enabled
  * @rmtoll SVMCR2     VDDIO5VMEN      LL_PWR_IsEnabledVddIO5Monitoring
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledVddIO5Monitoring(void)
{
  return ((READ_BIT(PWR->SVMCR2, PWR_SVMCR2_VDDIO5VMEN) == (PWR_SVMCR2_VDDIO5VMEN)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Vdd ADC voltage monitoring
  * @rmtoll SVMCR3     AVMEN           LL_PWR_EnableVddADCMonitoring
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableVddADCMonitoring(void)
{
  SET_BIT(PWR->SVMCR3, PWR_SVMCR3_AVMEN);
}

/**
  * @brief  Disable Vdd ADC voltage monitoring
  * @rmtoll SVMCR3     AVMEN           LL_PWR_DisableVddADCMonitoring
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableVddADCMonitoring(void)
{
  CLEAR_BIT(PWR->SVMCR3, PWR_SVMCR3_AVMEN);
}

/**
  * @brief  Check if the Vdd ADC voltage monitoring is enabled
  * @rmtoll SVMCR3     AVMEN           LL_PWR_IsEnabledVddADCMonitoring
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledVddADCMonitoring(void)
{
  return ((READ_BIT(PWR->SVMCR3, PWR_SVMCR3_AVMEN) == (PWR_SVMCR3_AVMEN)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Vdd USB voltage monitoring
  * @rmtoll SVMCR3     USB33VMEN       LL_PWR_EnableVddUSBMonitoring
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableVddUSBMonitoring(void)
{
  SET_BIT(PWR->SVMCR3, PWR_SVMCR3_USB33VMEN);
}

/**
  * @brief  Disable Vdd USB voltage monitoring
  * @rmtoll SVMCR3     USB33VMEN       LL_PWR_DisableVddUSBMonitoring
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableVddUSBMonitoring(void)
{
  CLEAR_BIT(PWR->SVMCR3, PWR_SVMCR3_USB33VMEN);
}

/**
  * @brief  Check if the Vdd USB voltage monitoring is enabled
  * @rmtoll SVMCR3     USB33VMEN       LL_PWR_IsEnabledVddUSBMonitoring
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledVddUSBMonitoring(void)
{
  return ((READ_BIT(PWR->SVMCR3, PWR_SVMCR3_USB33VMEN) == (PWR_SVMCR3_USB33VMEN)) ? 1UL : 0UL);
}

/**
  * @brief  Set the VDD I/O voltage range
  * @rmtoll SVMCR3     VDDIOVRSEL      LL_PWR_SetVddIOVoltageRange
  * @param  VoltageRange This parameter can be one of the following values:
  *         @arg @ref LL_PWR_VDDIO_VOLTAGE_RANGE_3V3
  *         @arg @ref LL_PWR_VDDIO_VOLTAGE_RANGE_1V8
  * @note   HSLV_VDD option bit must be set to allow 1v8 voltage range
  *         operation.
  *         Setting this configuration while VDD is in 3v3 range damages the device.
  * @retval None
  */
__STATIC_INLINE void LL_PWR_SetVddIOVoltageRange(uint32_t VoltageRange)
{
  MODIFY_REG(PWR->SVMCR3, PWR_SVMCR3_VDDIOVRSEL, (VoltageRange << PWR_SVMCR3_VDDIOVRSEL_Pos));
}

/**
  * @brief  Get the VDD IO voltage range
  * @rmtoll SVMCR3     VDDIOVRSEL      LL_PWR_GetVddIOVoltageRange
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_PWR_VDDIO_VOLTAGE_RANGE_3V3
  *         @arg @ref LL_PWR_VDDIO_VOLTAGE_RANGE_1V8
  */
__STATIC_INLINE uint32_t LL_PWR_GetVddIOVoltageRange(void)
{
  return (uint32_t)(READ_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIOVRSEL) >> PWR_SVMCR3_VDDIOVRSEL_Pos);
}

/**
  * @brief  Set the VDD IO2 voltage range
  * @rmtoll SVMCR3     VDDIO2VRSEL     LL_PWR_SetVddIO2VoltageRange
  * @param  VoltageRange This parameter can be one of the following values:
  *         @arg @ref LL_PWR_VDDIO_VOLTAGE_RANGE_3V3
  *         @arg @ref LL_PWR_VDDIO_VOLTAGE_RANGE_1V8
  * @note   HSLV_VDD option bit must be set to allow 1v8 voltage range
  *         operation.
  *         Setting this configuration while VDD is in 3v3 range damages the device.
  * @retval None
  */
__STATIC_INLINE void LL_PWR_SetVddIO2VoltageRange(uint32_t VoltageRange)
{
  MODIFY_REG(PWR->SVMCR3, PWR_SVMCR3_VDDIO2VRSEL, (VoltageRange << PWR_SVMCR3_VDDIO2VRSEL_Pos));
}

/**
  * @brief  Get the VDD IO2 voltage range
  * @rmtoll SVMCR3     VDDIO2VRSEL     LL_PWR_GetVddIO2VoltageRange
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_PWR_VDDIO_VOLTAGE_RANGE_3V3
  *         @arg @ref LL_PWR_VDDIO_VOLTAGE_RANGE_1V8
  */
__STATIC_INLINE uint32_t LL_PWR_GetVddIO2VoltageRange(void)
{
  return (uint32_t)(READ_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIO2VRSEL) >> PWR_SVMCR3_VDDIO2VRSEL_Pos);
}

/**
  * @brief  Set the VDD IO3 voltage range
  * @rmtoll SVMCR3     VDDIO3VRSEL     LL_PWR_SetVddIO3VoltageRange
  * @param  VoltageRange This parameter can be one of the following values:
  *         @arg @ref LL_PWR_VDDIO_VOLTAGE_RANGE_3V3
  *         @arg @ref LL_PWR_VDDIO_VOLTAGE_RANGE_1V8
  * @note   HSLV_VDD option bit must be set to allow 1v8 voltage range
  *         operation.
  *         Setting this configuration while VDD is in 3v3 range damages the device.
  * @retval None
  */
__STATIC_INLINE void LL_PWR_SetVddIO3VoltageRange(uint32_t VoltageRange)
{
  MODIFY_REG(PWR->SVMCR3, PWR_SVMCR3_VDDIO3VRSEL, (VoltageRange << PWR_SVMCR3_VDDIO3VRSEL_Pos));
}

/**
  * @brief  Get the VDD IO3 voltage range
  * @rmtoll SVMCR3     VDDIO3VRSEL     LL_PWR_GetVddIO3VoltageRange
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_PWR_VDDIO_VOLTAGE_RANGE_3V3
  *         @arg @ref LL_PWR_VDDIO_VOLTAGE_RANGE_1V8
  */
__STATIC_INLINE uint32_t LL_PWR_GetVddIO3VoltageRange(void)
{
  return (uint32_t)(READ_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIO3VRSEL) >> PWR_SVMCR3_VDDIO3VRSEL_Pos);
}

/**
  * @brief  Set the VDD IO4 voltage range
  * @rmtoll SVMCR1     VDDIO4VRSEL     LL_PWR_SetVddIO4VoltageRange
  * @param  VoltageRange This parameter can be one of the following values:
  *         @arg @ref LL_PWR_VDDIO_VOLTAGE_RANGE_3V3
  *         @arg @ref LL_PWR_VDDIO_VOLTAGE_RANGE_1V8
  * @note   HSLV_VDD option bit must be set to allow 1v8 voltage range
  *         operation.
  *         Setting this configuration while VDD is in 3v3 range damages the device.
  * @retval None
  */
__STATIC_INLINE void LL_PWR_SetVddIO4VoltageRange(uint32_t VoltageRange)
{
  MODIFY_REG(PWR->SVMCR1, PWR_SVMCR1_VDDIO4VRSEL, (VoltageRange << PWR_SVMCR1_VDDIO4VRSEL_Pos));
}

/**
  * @brief  Get the VDD IO4 voltage range
  * @rmtoll SVMCR1     VDDIO4VRSEL     LL_PWR_GetVddIO4VoltageRange
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_PWR_VDDIO_VOLTAGE_RANGE_3V3
  *         @arg @ref LL_PWR_VDDIO_VOLTAGE_RANGE_1V8
  */
__STATIC_INLINE uint32_t LL_PWR_GetVddIO4VoltageRange(void)
{
  return (uint32_t)(READ_BIT(PWR->SVMCR1, PWR_SVMCR1_VDDIO4VRSEL) >> PWR_SVMCR1_VDDIO4VRSEL_Pos);
}

/**
  * @brief  Set the VDD IO5 voltage range
  * @rmtoll SVMCR2     VDDIO5VRSEL     LL_PWR_SetVddIO5VoltageRange
  * @param  VoltageRange This parameter can be one of the following values:
  *         @arg @ref LL_PWR_VDDIO_VOLTAGE_RANGE_3V3
  *         @arg @ref LL_PWR_VDDIO_VOLTAGE_RANGE_1V8
  * @note   HSLV_VDD option bit must be set to allow 1v8 voltage range
  *         operation.
  *         Setting this configuration while VDD is in 3v3 range damages the device.
  * @retval None
  */
__STATIC_INLINE void LL_PWR_SetVddIO5VoltageRange(uint32_t VoltageRange)
{
  MODIFY_REG(PWR->SVMCR2, PWR_SVMCR2_VDDIO5VRSEL, (VoltageRange << PWR_SVMCR2_VDDIO5VRSEL_Pos));
}

/**
  * @brief  Get the VDD IO5 voltage range
  * @rmtoll SVMCR2     VDDIO5VRSEL     LL_PWR_GetVddIO5VoltageRange
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_PWR_VDDIO_VOLTAGE_RANGE_3V3
  *         @arg @ref LL_PWR_VDDIO_VOLTAGE_RANGE_1V8
  */
__STATIC_INLINE uint32_t LL_PWR_GetVddIO5VoltageRange(void)
{
  return (uint32_t)(READ_BIT(PWR->SVMCR2, PWR_SVMCR2_VDDIO5VRSEL) >> PWR_SVMCR2_VDDIO5VRSEL_Pos);
}

/**
  * @brief  Enable Vdd IO4 voltage range retained in Standby mode
  * @rmtoll SVMCR1     VDDIO4VRSTBY    LL_PWR_EnableVddIO4VoltageRangeSB
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableVddIO4VoltageRangeSB(void)
{
  SET_BIT(PWR->SVMCR1, PWR_SVMCR1_VDDIO4VRSTBY);
}

/**
  * @brief  Disable Vdd IO4 voltage range retained in Standby mode
  * @rmtoll SVMCR1     VDDIO4VRSTBY    LL_PWR_DisableVddIO4VoltageRangeSB
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableVddIO4VoltageRangeSB(void)
{
  CLEAR_BIT(PWR->SVMCR1, PWR_SVMCR1_VDDIO4VRSTBY);
}

/**
  * @brief  Check if the Vdd IO4 voltage range retained in Standby mode  is enabled
  * @rmtoll SVMCR1     VDDIO4VRSTBY    LL_PWR_IsEnabledVddIO4VoltageRangeSB
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledVddIO4VoltageRangeSB(void)
{
  return ((READ_BIT(PWR->SVMCR1, PWR_SVMCR1_VDDIO4VRSTBY) == (PWR_SVMCR1_VDDIO4VRSTBY)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Vdd IO5 voltage range retained in Standby mode
  * @rmtoll SVMCR2     VDDIO5VRSTBY    LL_PWR_EnableVddIO5VoltageRangeSB
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableVddIO5VoltageRangeSB(void)
{
  SET_BIT(PWR->SVMCR2, PWR_SVMCR2_VDDIO5VRSTBY);
}

/**
  * @brief  Disable Vdd IO5 voltage range retained in Standby mode
  * @rmtoll SVMCR2     VDDIO5VRSTBY    LL_PWR_DisableVddIO5VoltageRangeSB
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableVddIO5VoltageRangeSB(void)
{
  CLEAR_BIT(PWR->SVMCR2, PWR_SVMCR2_VDDIO5VRSTBY);
}

/**
  * @brief  Check if the Vdd IO5 voltage range retained in Standby mode  is enabled
  * @rmtoll SVMCR2     VDDIO5VRSTBY    LL_PWR_IsEnabledVddIO5VoltageRangeSB
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledVddIO5VoltageRangeSB(void)
{
  return ((READ_BIT(PWR->SVMCR2, PWR_SVMCR2_VDDIO5VRSTBY) == (PWR_SVMCR2_VDDIO5VRSTBY)) ? 1UL : 0UL);
}

/**
  * @brief  Enable the WakeUp PINx functionality
  * @rmtoll WKUPEPR    WKUPEN1         LL_PWR_EnableWakeUpPin\n
  *         WKUPEPR    WKUPEN2         LL_PWR_EnableWakeUpPin\n
  *         WKUPEPR    WKUPEN3         LL_PWR_EnableWakeUpPin\n
  *         WKUPEPR    WKUPEN4         LL_PWR_EnableWakeUpPin
  * @param  WakeUpPin This parameter can be one of the following values:
  *         @arg @ref LL_PWR_WAKEUP_PIN1
  *         @arg @ref LL_PWR_WAKEUP_PIN2
  *         @arg @ref LL_PWR_WAKEUP_PIN3
  *         @arg @ref LL_PWR_WAKEUP_PIN4
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableWakeUpPin(uint32_t WakeUpPin)
{
  SET_BIT(PWR->WKUPEPR, WakeUpPin);
}

/**
  * @brief  Disable the WakeUp PINx functionality
  * @rmtoll WKUPEPR   WKUPEN1          LL_PWR_DisableWakeUpPin\n
  *         WKUPEPR   WKUPEN2          LL_PWR_DisableWakeUpPin\n
  *         WKUPEPR   WKUPEN3          LL_PWR_DisableWakeUpPin\n
  *         WKUPEPR   WKUPEN4          LL_PWR_DisableWakeUpPin
  * @param  WakeUpPin This parameter can be one of the following values:
  *         @arg @ref LL_PWR_WAKEUP_PIN1
  *         @arg @ref LL_PWR_WAKEUP_PIN2
  *         @arg @ref LL_PWR_WAKEUP_PIN3
  *         @arg @ref LL_PWR_WAKEUP_PIN4
  * @retval None
  */
__STATIC_INLINE void LL_PWR_DisableWakeUpPin(uint32_t WakeUpPin)
{
  CLEAR_BIT(PWR->WKUPEPR, WakeUpPin);
}

/**
  * @brief  Check if the WakeUp PINx functionality is enabled
  * @rmtoll WKUPEPR    WKUPEN1         LL_PWR_IsEnabledWakeUpPin\n
  *         WKUPEPR    WKUPEN2         LL_PWR_IsEnabledWakeUpPin\n
  *         WKUPEPR    WKUPEN3         LL_PWR_IsEnabledWakeUpPin\n
  *         WKUPEPR    WKUPEN4         LL_PWR_IsEnabledWakeUpPin
  * @param  WakeUpPin This parameter can be one of the following values:
  *         @arg @ref LL_PWR_WAKEUP_PIN1
  *         @arg @ref LL_PWR_WAKEUP_PIN2
  *         @arg @ref LL_PWR_WAKEUP_PIN3
  *         @arg @ref LL_PWR_WAKEUP_PIN4
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsEnabledWakeUpPin(uint32_t WakeUpPin)
{
  return ((READ_BIT(PWR->WKUPEPR, WakeUpPin) == (WakeUpPin)) ? 1UL : 0UL);
}

/**
  * @brief  Set the Wake-Up pin polarity low for the event detection
  * @rmtoll WKUPEPR    WKUPP1          LL_PWR_SetWakeUpPinPolarityLow\n
  *         WKUPEPR    WKUPP2          LL_PWR_SetWakeUpPinPolarityLow\n
  *         WKUPEPR    WKUPP3          LL_PWR_SetWakeUpPinPolarityLow\n
  *         WKUPEPR    WKUPP4          LL_PWR_SetWakeUpPinPolarityLow
  * @param  WakeUpPin This parameter can be one of the following values:
  *         @arg @ref LL_PWR_WAKEUP_PIN1
  *         @arg @ref LL_PWR_WAKEUP_PIN2
  *         @arg @ref LL_PWR_WAKEUP_PIN3
  *         @arg @ref LL_PWR_WAKEUP_PIN4
  * @retval None
  */
__STATIC_INLINE void LL_PWR_SetWakeUpPinPolarityLow(uint32_t WakeUpPin)
{
  SET_BIT(PWR->WKUPEPR, (WakeUpPin << PWR_WKUPEPR_WKUPP1_Pos));
}

/**
  * @brief  Set the Wake-Up pin polarity high for the event detection
  * @rmtoll WKUPEPR    WKUPP1          LL_PWR_SetWakeUpPinPolarityHigh\n
  *         WKUPEPR    WKUPP2          LL_PWR_SetWakeUpPinPolarityHigh\n
  *         WKUPEPR    WKUPP3          LL_PWR_SetWakeUpPinPolarityHigh\n
  *         WKUPEPR    WKUPP4          LL_PWR_SetWakeUpPinPolarityHigh
  * @param  WakeUpPin This parameter can be one of the following values:
  *         @arg @ref LL_PWR_WAKEUP_PIN1
  *         @arg @ref LL_PWR_WAKEUP_PIN2
  *         @arg @ref LL_PWR_WAKEUP_PIN3
  *         @arg @ref LL_PWR_WAKEUP_PIN4
  * @retval None
  */
__STATIC_INLINE void LL_PWR_SetWakeUpPinPolarityHigh(uint32_t WakeUpPin)
{
  CLEAR_BIT(PWR->WKUPEPR, (WakeUpPin << PWR_WKUPEPR_WKUPP1_Pos));
}

/**
  * @brief  Check if the Wake-Up pin polarity is low for event detection
  * @rmtoll WKUPEPR    WKUPP1          LL_PWR_IsWakeUpPinPolarityLow\n
  *         WKUPEPR    WKUPP2          LL_PWR_IsWakeUpPinPolarityLow\n
  *         WKUPEPR    WKUPP3          LL_PWR_IsWakeUpPinPolarityLow\n
  *         WKUPEPR    WKUPP4          LL_PWR_IsWakeUpPinPolarityLow
  * @param  WakeUpPin This parameter can be one of the following values:
  *         @arg @ref LL_PWR_WAKEUP_PIN1
  *         @arg @ref LL_PWR_WAKEUP_PIN2
  *         @arg @ref LL_PWR_WAKEUP_PIN3
  *         @arg @ref LL_PWR_WAKEUP_PIN4
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsWakeUpPinPolarityLow(uint32_t WakeUpPin)
{
  return ((READ_BIT(PWR->WKUPEPR, (WakeUpPin << PWR_WKUPEPR_WKUPP1_Pos)) == (WakeUpPin << PWR_WKUPEPR_WKUPP1_Pos)) ? 1UL : 0UL);
}

/**
  * @brief  Set the Wake-Up pin Pull None
  * @rmtoll WKUPEPR    WKUPPUPD1       LL_PWR_SetWakeUpPinPullNone\n
  *         WKUPEPR    WKUPPUPD2       LL_PWR_SetWakeUpPinPullNone\n
  *         WKUPEPR    WKUPPUPD3       LL_PWR_SetWakeUpPinPullNone\n
  *         WKUPEPR    WKUPPUPD4       LL_PWR_SetWakeUpPinPullNone
  * @param  WakeUpPin This parameter can be one of the following values:
  *         @arg @ref LL_PWR_WAKEUP_PIN1
  *         @arg @ref LL_PWR_WAKEUP_PIN2
  *         @arg @ref LL_PWR_WAKEUP_PIN3
  *         @arg @ref LL_PWR_WAKEUP_PIN4
  * @retval None
  */
__STATIC_INLINE void LL_PWR_SetWakeUpPinPullNone(uint32_t WakeUpPin)
{
  MODIFY_REG(PWR->WKUPEPR, \
             (PWR_WKUPEPR_WKUPPUPD1 << ((LL_PWR_WAKEUP_PINS_PULL_SHIFT_OFFSET * POSITION_VAL(WakeUpPin)) & LL_PWR_WAKEUP_PINS_MAX_SHIFT_MASK)), \
             (LL_PWR_WAKEUP_PIN_NOPULL << ((PWR_WKUPEPR_WKUPPUPD1_Pos + (LL_PWR_WAKEUP_PINS_PULL_SHIFT_OFFSET * POSITION_VAL(WakeUpPin))) & LL_PWR_WAKEUP_PINS_MAX_SHIFT_MASK)));
}

/**
  * @brief  Set the Wake-Up pin Pull Up
  * @rmtoll WKUPEPR    WKUPPUPD1       LL_PWR_SetWakeUpPinPullUp\n
  *         WKUPEPR    WKUPPUPD2       LL_PWR_SetWakeUpPinPullUp\n
  *         WKUPEPR    WKUPPUPD3       LL_PWR_SetWakeUpPinPullUp\n
  *         WKUPEPR    WKUPPUPD4       LL_PWR_SetWakeUpPinPullUp
  * @param  WakeUpPin This parameter can be one of the following values:
  *         @arg @ref LL_PWR_WAKEUP_PIN1
  *         @arg @ref LL_PWR_WAKEUP_PIN2
  *         @arg @ref LL_PWR_WAKEUP_PIN3
  *         @arg @ref LL_PWR_WAKEUP_PIN4
  * @retval None
  */
__STATIC_INLINE void LL_PWR_SetWakeUpPinPullUp(uint32_t WakeUpPin)
{
  MODIFY_REG(PWR->WKUPEPR, \
             (PWR_WKUPEPR_WKUPPUPD1 << ((LL_PWR_WAKEUP_PINS_PULL_SHIFT_OFFSET * POSITION_VAL(WakeUpPin)) & LL_PWR_WAKEUP_PINS_MAX_SHIFT_MASK)), \
             (LL_PWR_WAKEUP_PIN_PULLUP << ((PWR_WKUPEPR_WKUPPUPD1_Pos + (LL_PWR_WAKEUP_PINS_PULL_SHIFT_OFFSET * POSITION_VAL(WakeUpPin))) & LL_PWR_WAKEUP_PINS_MAX_SHIFT_MASK)));
}

/**
  * @brief  Set the Wake-Up pin Pull Down
  * @rmtoll WKUPEPR    WKUPPUPD1       LL_PWR_SetWakeUpPinPullDown\n
  *         WKUPEPR    WKUPPUPD2       LL_PWR_SetWakeUpPinPullDown\n
  *         WKUPEPR    WKUPPUPD3       LL_PWR_SetWakeUpPinPullDown\n
  *         WKUPEPR    WKUPPUPD4       LL_PWR_SetWakeUpPinPullDown
  * @param  WakeUpPin This parameter can be one of the following values:
  *         @arg @ref LL_PWR_WAKEUP_PIN1
  *         @arg @ref LL_PWR_WAKEUP_PIN2
  *         @arg @ref LL_PWR_WAKEUP_PIN3
  *         @arg @ref LL_PWR_WAKEUP_PIN4
  * @retval None
  */
__STATIC_INLINE void LL_PWR_SetWakeUpPinPullDown(uint32_t WakeUpPin)
{
  MODIFY_REG(PWR->WKUPEPR, \
             (PWR_WKUPEPR_WKUPPUPD1 << ((LL_PWR_WAKEUP_PINS_PULL_SHIFT_OFFSET * POSITION_VAL(WakeUpPin)) & LL_PWR_WAKEUP_PINS_MAX_SHIFT_MASK)), \
             (LL_PWR_WAKEUP_PIN_PULLDOWN << ((PWR_WKUPEPR_WKUPPUPD1_Pos + (LL_PWR_WAKEUP_PINS_PULL_SHIFT_OFFSET * POSITION_VAL(WakeUpPin))) & LL_PWR_WAKEUP_PINS_MAX_SHIFT_MASK)));
}

/**
  * @brief  Get the Wake-Up pin pull
  * @rmtoll WKUPEPR    WKUPPUPD1       LL_PWR_GetWakeUpPinPull\n
  *         WKUPEPR    WKUPPUPD2       LL_PWR_GetWakeUpPinPull\n
  *         WKUPEPR    WKUPPUPD3       LL_PWR_GetWakeUpPinPull\n
  *         WKUPEPR    WKUPPUPD4       LL_PWR_GetWakeUpPinPull\
  * @param  WakeUpPin This parameter can be one of the following values:
  *         @arg @ref LL_PWR_WAKEUP_PIN1
  *         @arg @ref LL_PWR_WAKEUP_PIN2
  *         @arg @ref LL_PWR_WAKEUP_PIN3
  *         @arg @ref LL_PWR_WAKEUP_PIN4
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_PWR_WAKEUP_PIN_NOPULL
  *         @arg @ref LL_PWR_WAKEUP_PIN_PULLUP
  *         @arg @ref LL_PWR_WAKEUP_PIN_PULLDOWN
  */
__STATIC_INLINE uint32_t LL_PWR_GetWakeUpPinPull(uint32_t WakeUpPin)
{
  uint32_t regValue = READ_BIT(PWR->WKUPEPR, (PWR_WKUPEPR_WKUPPUPD1 << ((LL_PWR_WAKEUP_PINS_PULL_SHIFT_OFFSET * POSITION_VAL(WakeUpPin)) & LL_PWR_WAKEUP_PINS_MAX_SHIFT_MASK)));

  return (uint32_t)(regValue >> ((PWR_WKUPEPR_WKUPPUPD1_Pos + (LL_PWR_WAKEUP_PINS_PULL_SHIFT_OFFSET * POSITION_VAL(
                                                                 WakeUpPin))) & LL_PWR_WAKEUP_PINS_MAX_SHIFT_MASK));
}

/**
  * @}
  */

/** @defgroup PWR_LL_EF_FLAG_MANAGEMENT PWR FLAG Management
  * @{
  */

/**
  * @brief  Indicate whether the voltage level is ready for current actual used VOS
  * @rmtoll VOSCR        ACTVOSRDY     LL_PWR_IsActiveFlag_ACTVOSRDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_ACTVOSRDY(void)
{
  return ((READ_BIT(PWR->VOSCR, PWR_VOSCR_ACTVOSRDY) == (PWR_VOSCR_ACTVOSRDY)) ? 1UL : 0UL);
}

/**
  * @brief  Indicate whether VDD voltage is below the selected PVD threshold
  * @rmtoll CR2        PVDO            LL_PWR_IsActiveFlag_PVDO
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_PVDO(void)
{
  return ((READ_BIT(PWR->CR2, PWR_CR2_PVDO) == (PWR_CR2_PVDO)) ? 1UL : 0UL);
}

/**
  * @brief  Indicate whether the VCORE level is above or below low threshold
  * @rmtoll CR3      VCOREL           LL_PWR_IsActiveFlag_VCOREL
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_VCOREL(void)
{
  return ((READ_BIT(PWR->CR3, PWR_CR3_VCOREL) == (PWR_CR3_VCOREL)) ? 1UL : 0UL);
}

/**
  * @brief  Indicate whether the VCORE level is above or below high threshold
  * @rmtoll CR3      VCOREH           LL_PWR_IsActiveFlag_VCOREH
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_VCOREH(void)
{
  return ((READ_BIT(PWR->CR3, PWR_CR3_VCOREH) == (PWR_CR3_VCOREH)) ? 1UL : 0UL);
}

/**
  * @brief  Indicate whether the Regulator is ready in the selected voltage range
  *         or if its output voltage is still changing to the required voltage level
  * @rmtoll VOSCR      VOSRDY          LL_PWR_IsActiveFlag_VOSRDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_VOSRDY(void)
{
  return ((READ_BIT(PWR->VOSCR, PWR_VOSCR_VOSRDY) == (PWR_VOSCR_VOSRDY)) ? 1UL : 0UL);
}

/**
  * @brief  Indicate whether the VBAT level is above or below low threshold
  * @rmtoll BDCR1      VBATL           LL_PWR_IsActiveFlag_VBATL
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_VBATL(void)
{
  return ((READ_BIT(PWR->BDCR1, PWR_BDCR1_VBATL) == (PWR_BDCR1_VBATL)) ? 1UL : 0UL);
}

/**
  * @brief  Indicate whether the VBAT level is above or below high threshold
  * @rmtoll BDCR1      VBATH           LL_PWR_IsActiveFlag_VBATH
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_VBATH(void)
{
  return ((READ_BIT(PWR->BDCR1, PWR_BDCR1_VBATH) == (PWR_BDCR1_VBATH)) ? 1UL : 0UL);
}

/**
  * @brief  Indicate whether the CPU temperature level is above or below low threshold
  * @rmtoll BDCR1      TEMPL           LL_PWR_IsActiveFlag_TEMPL
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_TEMPL(void)
{
  return ((READ_BIT(PWR->BDCR1, PWR_BDCR1_TEMPL) == (PWR_BDCR1_TEMPL)) ? 1UL : 0UL);
}

/**
  * @brief  Indicate whether the CPU temperature level is above or below high threshold
  * @rmtoll BDCR1      TEMPH           LL_PWR_IsActiveFlag_TEMPH
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_TEMPH(void)
{
  return ((READ_BIT(PWR->BDCR1, PWR_BDCR1_TEMPH) == (PWR_BDCR1_TEMPH)) ? 1UL : 0UL);
}

/**
  * @brief  Get System Stop Flag
  * @rmtoll CPUCR      STOPF           LL_PWR_IsActiveFlag_STOP
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_STOP(void)
{
  return ((READ_BIT(PWR->CPUCR, PWR_CPUCR_STOPF) == (PWR_CPUCR_STOPF)) ? 1UL : 0UL);
}

/**
  * @brief  Get System Standby Flag
  * @rmtoll CPUCR      SBF             LL_PWR_IsActiveFlag_SB
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_SB(void)
{
  return ((READ_BIT(PWR->CPUCR, PWR_CPUCR_SBF) == (PWR_CPUCR_SBF)) ? 1UL : 0UL);
}

/**
  * @brief  Get VDDIO2 ready Flag
  * @rmtoll SVMCR3     VDDIO2RDY       LL_PWR_IsActiveFlag_VDDIO2RDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_VDDIO2RDY(void)
{
  return ((READ_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIO2RDY) == (PWR_SVMCR3_VDDIO2RDY)) ? 1UL : 0UL);
}

/**
  * @brief  Get VDDIO3 ready Flag
  * @rmtoll SVMCR3     VDDIO3RDY       LL_PWR_IsActiveFlag_VDDIO3RDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_VDDIO3RDY(void)
{
  return ((READ_BIT(PWR->SVMCR3, PWR_SVMCR3_VDDIO3RDY) == (PWR_SVMCR3_VDDIO3RDY)) ? 1UL : 0UL);
}

/**
  * @brief  Get VDDIO4 ready Flag
  * @rmtoll SVMCR1     VDDIO4RDY       LL_PWR_IsActiveFlag_VDDIO4RDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_VDDIO4RDY(void)
{
  return ((READ_BIT(PWR->SVMCR1, PWR_SVMCR1_VDDIO4RDY) == (PWR_SVMCR1_VDDIO4RDY)) ? 1UL : 0UL);
}

/**
  * @brief  Get VDDIO5 ready Flag
  * @rmtoll SVMCR2     VDDIO5RDY       LL_PWR_IsActiveFlag_VDDIO5RDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_VDDIO5RDY(void)
{
  return ((READ_BIT(PWR->SVMCR2, PWR_SVMCR2_VDDIO5RDY) == (PWR_SVMCR2_VDDIO5RDY)) ? 1UL : 0UL);
}

/**
  * @brief  Get VDD ADC ready Flag
  * @rmtoll SVMCR3     ARDY            LL_PWR_IsActiveFlag_ARDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_ARDY(void)
{
  return ((READ_BIT(PWR->SVMCR3, PWR_SVMCR3_ARDY) == (PWR_SVMCR3_ARDY)) ? 1UL : 0UL);
}

/**
  * @brief  Get VDD USB33 ready Flag
  * @rmtoll SVMCR3     USB33RDY        LL_PWR_IsActiveFlag_USB33RDY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_USB33RDY(void)
{
  return ((READ_BIT(PWR->SVMCR3, PWR_SVMCR3_USB33RDY) == (PWR_SVMCR3_USB33RDY)) ? 1UL : 0UL);
}

/**
  * @brief  Get Wake-up Flag 1
  * @rmtoll WKUPSR     WKUPF1          LL_PWR_IsActiveFlag_WU1
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_WU1(void)
{
  return ((READ_BIT(PWR->WKUPSR, PWR_WKUPSR_WKUPF1) == (PWR_WKUPSR_WKUPF1)) ? 1UL : 0UL);
}

/**
  * @brief  Get Wake-up Flag 2
  * @rmtoll WKUPSR     WKUPF2          LL_PWR_IsActiveFlag_WU2
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_WU2(void)
{
  return ((READ_BIT(PWR->WKUPSR, PWR_WKUPSR_WKUPF2) == (PWR_WKUPSR_WKUPF2)) ? 1UL : 0UL);
}

/**
  * @brief  Get Wake-up Flag 3
  * @rmtoll WKUPSR     WKUPF3          LL_PWR_IsActiveFlag_WU3
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_WU3(void)
{
  return ((READ_BIT(PWR->WKUPSR, PWR_WKUPSR_WKUPF3) == (PWR_WKUPSR_WKUPF3)) ? 1UL : 0UL);
}

/**
  * @brief  Get Wake-up Flag 4
  * @rmtoll WKUPSR     WKUPF4          LL_PWR_IsActiveFlag_WU4
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_PWR_IsActiveFlag_WU4(void)
{
  return ((READ_BIT(PWR->WKUPSR, PWR_WKUPSR_WKUPF4) == (PWR_WKUPSR_WKUPF4)) ? 1UL : 0UL);
}

/**
  * @brief  Clear STOP and STANDBY and flags
  * @rmtoll CPUCR      CSSF            LL_PWR_ClearFlag_STOP_SB
  * @retval None
  */
__STATIC_INLINE void LL_PWR_ClearFlag_STOP_SB(void)
{
  SET_BIT(PWR->CPUCR, PWR_CPUCR_CSSF);
}

/**
  * @brief  Clear Wake-up Flag 1
  * @rmtoll WKUPCR     WKUPC1          LL_PWR_ClearFlag_WU1
  * @retval None
  */
__STATIC_INLINE void LL_PWR_ClearFlag_WU1(void)
{
  WRITE_REG(PWR->WKUPCR, PWR_WKUPCR_WKUPC1);
}

/**
  * @brief  Clear Wake-up Flag 2
  * @rmtoll WKUPCR     WKUPC2          LL_PWR_ClearFlag_WU2
  * @retval None
  */
__STATIC_INLINE void LL_PWR_ClearFlag_WU2(void)
{
  WRITE_REG(PWR->WKUPCR, PWR_WKUPCR_WKUPC2);
}

/**
  * @brief  Clear Wake-up Flag 3
  * @rmtoll WKUPCR     WKUPC3          LL_PWR_ClearFlag_WU3
  * @retval None
  */
__STATIC_INLINE void LL_PWR_ClearFlag_WU3(void)
{
  WRITE_REG(PWR->WKUPCR, PWR_WKUPCR_WKUPC3);
}

/**
  * @brief  Clear Wake-up Flag 4
  * @rmtoll WKUPCR     WKUPC4          LL_PWR_ClearFlag_WU4
  * @retval None
  */
__STATIC_INLINE void LL_PWR_ClearFlag_WU4(void)
{
  WRITE_REG(PWR->WKUPCR, PWR_WKUPCR_WKUPC4);
}

/**
  * @brief  Clear all wake up flags.
  * @rmtoll WUSCR          WKUPC         LL_PWR_ClearFlag_WU
  * @retval None
  */
__STATIC_INLINE void LL_PWR_ClearFlag_WU(void)
{
  WRITE_REG(PWR->WKUPCR, PWR_WKUPCR_WKUPC);
}

/**
  * @}
  */

/** @defgroup PWR_LL_EF_ATTRIBUTE_MANAGEMENT PWR Attribute Management
  * @{
  */

/**
  * @brief  Configure privilege attribute mode.
  * @note   This API can be executed only by CPU in privilege mode.
  * @rmtoll PRIVCFGR   PRIV0           LL_PWR_ConfigPrivilege\n
  *         PRIVCFGR   PRIV1           LL_PWR_ConfigPrivilege\n
  *         PRIVCFGR   PRIV2           LL_PWR_ConfigPrivilege\n
  *         PRIVCFGR   PRIV3           LL_PWR_ConfigPrivilege\n
  *         PRIVCFGR   PRIV4           LL_PWR_ConfigPrivilege\n
  *         PRIVCFGR   PRIV5           LL_PWR_ConfigPrivilege\n
  *         PRIVCFGR   PRIV6           LL_PWR_ConfigPrivilege\n
  *         PRIVCFGR   PRIV7           LL_PWR_ConfigPrivilege\n
  *         PRIVCFGR   WKUPPRIV1       LL_PWR_ConfigPrivilege\n
  *         PRIVCFGR   WKUPPRIV2       LL_PWR_ConfigPrivilege\n
  *         PRIVCFGR   WKUPPRIV3       LL_PWR_ConfigPrivilege\n
  *         PRIVCFGR   WKUPPRIV4       LL_PWR_ConfigPrivilege
  * @param  PrivilegeConfig This parameter can be the full combination
  *         of the following values:
  *         @arg @ref LL_PWR_PRIV0_PRIV or LL_PWR_PRIV0_NPRIV
  *         @arg @ref LL_PWR_PRIV1_PRIV or LL_PWR_PRIV1_NPRIV
  *         @arg @ref LL_PWR_PRIV2_PRIV or LL_PWR_PRIV2_NPRIV
  *         @arg @ref LL_PWR_PRIV3_PRIV or LL_PWR_PRIV3_NPRIV
  *         @arg @ref LL_PWR_PRIV4_PRIV or LL_PWR_PRIV4_NPRIV
  *         @arg @ref LL_PWR_PRIV5_PRIV or LL_PWR_PRIV5_NPRIV
  *         @arg @ref LL_PWR_PRIV6_PRIV or LL_PWR_PRIV6_NPRIV
  *         @arg @ref LL_PWR_PRIV7_PRIV or LL_PWR_PRIV7_NPRIV
  *         @arg @ref LL_PWR_WAKEUP_PIN1_NPRIV or LL_PWR_WAKEUP_PIN1_PRIV
  *         @arg @ref LL_PWR_WAKEUP_PIN2_NPRIV or LL_PWR_WAKEUP_PIN2_PRIV
  *         @arg @ref LL_PWR_WAKEUP_PIN3_NPRIV or LL_PWR_WAKEUP_PIN3_PRIV
  *         @arg @ref LL_PWR_WAKEUP_PIN4_NPRIV or LL_PWR_WAKEUP_PIN4_PRIV
  * @retval None.
  */
__STATIC_INLINE void LL_PWR_ConfigPrivilege(uint32_t PrivilegeConfig)
{
  WRITE_REG(PWR->PRIVCFGR, PrivilegeConfig);
}

/**
  * @brief  Get privilege attribute configuration.
  * @rmtoll PRIVCFGR    PRIV0            LL_PWR_GetConfigPrivilege\n
  *         PRIVCFGR    PRIV1            LL_PWR_GetConfigPrivilege\n
  *         PRIVCFGR    PRIV2            LL_PWR_GetConfigPrivilege\n
  *         PRIVCFGR    PRIV3            LL_PWR_GetConfigPrivilege\n
  *         PRIVCFGR    PRIV4            LL_PWR_GetConfigPrivilege\n
  *         PRIVCFGR    PRIV5            LL_PWR_GetConfigPrivilege\n
  *         PRIVCFGR    PRIV6            LL_PWR_GetConfigPrivilege\n
  *         PRIVCFGR    PRIV7            LL_PWR_GetConfigPrivilege\n
  *         PRIVCFGR    WKUPPRIV1        LL_PWR_GetConfigPrivilege\n
  *         PRIVCFGR    WKUPPRIV2        LL_PWR_GetConfigPrivilege\n
  *         PRIVCFGR    WKUPPRIV3        LL_PWR_GetConfigPrivilege\n
  *         PRIVCFGR    WKUPPRIV4        LL_PWR_GetConfigPrivilege
  * @retval Returned value is the combination of the following values:
  *         @arg @ref LL_PWR_PRIV0_PRIV or LL_PWR_PRIV0_NPRIV
  *         @arg @ref LL_PWR_PRIV1_PRIV or LL_PWR_PRIV1_NPRIV
  *         @arg @ref LL_PWR_PRIV2_PRIV or LL_PWR_PRIV2_NPRIV
  *         @arg @ref LL_PWR_PRIV3_PRIV or LL_PWR_PRIV3_NPRIV
  *         @arg @ref LL_PWR_PRIV4_PRIV or LL_PWR_PRIV4_NPRIV
  *         @arg @ref LL_PWR_PRIV5_PRIV or LL_PWR_PRIV5_NPRIV
  *         @arg @ref LL_PWR_PRIV6_PRIV or LL_PWR_PRIV6_NPRIV
  *         @arg @ref LL_PWR_PRIV7_PRIV or LL_PWR_PRIV7_NPRIV
  *         @arg @ref LL_PWR_WAKEUP_PIN1_NPRIV or LL_PWR_WAKEUP_PIN1_PRIV
  *         @arg @ref LL_PWR_WAKEUP_PIN2_NPRIV or LL_PWR_WAKEUP_PIN2_PRIV
  *         @arg @ref LL_PWR_WAKEUP_PIN3_NPRIV or LL_PWR_WAKEUP_PIN3_PRIV
  *         @arg @ref LL_PWR_WAKEUP_PIN4_NPRIV or LL_PWR_WAKEUP_PIN4_PRIV
  */
__STATIC_INLINE uint32_t LL_PWR_GetConfigPrivilege(void)
{
  return (READ_REG(PWR->PRIVCFGR));
}

#if defined (__ARM_FEATURE_CMSE) &&  (__ARM_FEATURE_CMSE == 3U)
/**
  * @brief  Configure secure attribute mode.
  * @note   This API can be executed only by CPU in secure mode.
  * @rmtoll SECCFGR    SEC0            LL_PWR_ConfigSecure\n
  *         SECCFGR    SEC1            LL_PWR_ConfigSecure\n
  *         SECCFGR    SEC2            LL_PWR_ConfigSecure\n
  *         SECCFGR    SEC3            LL_PWR_ConfigSecure\n
  *         SECCFGR    SEC4            LL_PWR_ConfigSecure\n
  *         SECCFGR    SEC5            LL_PWR_ConfigSecure\n
  *         SECCFGR    SEC6            LL_PWR_ConfigSecure\n
  *         SECCFGR    SEC7            LL_PWR_ConfigSecure\n
  *         SECCFGR    WKUPSEC1        LL_PWR_ConfigSecure\n
  *         SECCFGR    WKUPSEC2        LL_PWR_ConfigSecure\n
  *         SECCFGR    WKUPSEC3        LL_PWR_ConfigSecure\n
  *         SECCFGR    WKUPSEC4        LL_PWR_ConfigSecure
  * @param  SecureConfig This parameter can be the full combination
  *         of the following values:
  *         @arg @ref LL_PWR_SEC0_SEC or LL_PWR_SEC0_NSEC
  *         @arg @ref LL_PWR_SEC1_SEC or LL_PWR_SEC1_NSEC
  *         @arg @ref LL_PWR_SEC2_SEC or LL_PWR_SEC2_NSEC
  *         @arg @ref LL_PWR_SEC3_SEC or LL_PWR_SEC3_NSEC
  *         @arg @ref LL_PWR_SEC4_SEC or LL_PWR_SEC4_NSEC
  *         @arg @ref LL_PWR_SEC5_SEC or LL_PWR_SEC5_NSEC
  *         @arg @ref LL_PWR_SEC6_SEC or LL_PWR_SEC6_NSEC
  *         @arg @ref LL_PWR_SEC7_SEC or LL_PWR_SEC7_NSEC
  *         @arg @ref LL_PWR_WAKEUP_PIN1_NSEC or LL_PWR_WAKEUP_PIN1_SEC
  *         @arg @ref LL_PWR_WAKEUP_PIN2_NSEC or LL_PWR_WAKEUP_PIN2_SEC
  *         @arg @ref LL_PWR_WAKEUP_PIN3_NSEC or LL_PWR_WAKEUP_PIN3_SEC
  *         @arg @ref LL_PWR_WAKEUP_PIN4_NSEC or LL_PWR_WAKEUP_PIN4_SEC
  * @retval None.
  */
__STATIC_INLINE void LL_PWR_ConfigSecure(uint32_t SecureConfig)
{
  WRITE_REG(PWR->SECCFGR, SecureConfig);
}

/**
  * @brief  Get secure attribute configuration.
  * @note   This API can be executed only by CPU in secure mode.
  * @rmtoll SECCFGR    SEC0            LL_PWR_GetConfigSecure\n
  *         SECCFGR    SEC1            LL_PWR_GetConfigSecure\n
  *         SECCFGR    SEC2            LL_PWR_GetConfigSecure\n
  *         SECCFGR    SEC3            LL_PWR_GetConfigSecure\n
  *         SECCFGR    SEC4            LL_PWR_GetConfigSecure\n
  *         SECCFGR    SEC5            LL_PWR_GetConfigSecure\n
  *         SECCFGR    SEC6            LL_PWR_GetConfigSecure\n
  *         SECCFGR    SEC7            LL_PWR_GetConfigSecure\n
  *         SECCFGR    WKUPSEC1        LL_PWR_GetConfigSecure\n
  *         SECCFGR    WKUPSEC2        LL_PWR_GetConfigSecure\n
  *         SECCFGR    WKUPSEC3        LL_PWR_GetConfigSecure\n
  *         SECCFGR    WKUPSEC4        LL_PWR_GetConfigSecure
  * @retval Returned value is the combination of the following values:
  *         @arg @ref LL_PWR_SEC0_SEC or LL_PWR_SEC0_NSEC
  *         @arg @ref LL_PWR_SEC1_SEC or LL_PWR_SEC1_NSEC
  *         @arg @ref LL_PWR_SEC2_SEC or LL_PWR_SEC2_NSEC
  *         @arg @ref LL_PWR_SEC3_SEC or LL_PWR_SEC3_NSEC
  *         @arg @ref LL_PWR_SEC4_SEC or LL_PWR_SEC4_NSEC
  *         @arg @ref LL_PWR_SEC5_SEC or LL_PWR_SEC5_NSEC
  *         @arg @ref LL_PWR_SEC6_SEC or LL_PWR_SEC6_NSEC
  *         @arg @ref LL_PWR_SEC7_SEC or LL_PWR_SEC7_NSEC
  *         @arg @ref LL_PWR_WAKEUP_PIN1_NSEC or LL_PWR_WAKEUP_PIN1_SEC
  *         @arg @ref LL_PWR_WAKEUP_PIN2_NSEC or LL_PWR_WAKEUP_PIN2_SEC
  *         @arg @ref LL_PWR_WAKEUP_PIN3_NSEC or LL_PWR_WAKEUP_PIN3_SEC
  *         @arg @ref LL_PWR_WAKEUP_PIN4_NSEC or LL_PWR_WAKEUP_PIN4_SEC
  */
__STATIC_INLINE uint32_t LL_PWR_GetConfigSecure(void)
{
  return (READ_REG(PWR->SECCFGR));
}
#endif /* defined (__ARM_FEATURE_CMSE) &&  (__ARM_FEATURE_CMSE == 3U) */
/**
  * @}
  */

#if defined (USE_FULL_LL_DRIVER)
/** @defgroup PWR_LL_EF_Init De-initialization function
  * @{
  */
ErrorStatus LL_PWR_DeInit(void);
/**
  * @}
  */
#endif /* defined (USE_FULL_LL_DRIVER) */
/**
  * @}
  */

/**
  * @}
  */

#endif /* defined (PWR) */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STM32N6xx_LL_PWR_H */

