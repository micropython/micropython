/**
  ******************************************************************************
  * @file    stm32l4xx_ll_opamp.h
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   Header file of OPAMP LL module.
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
#ifndef __STM32L4xx_LL_OPAMP_H
#define __STM32L4xx_LL_OPAMP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx.h"

/** @addtogroup STM32L4xx_LL_Driver
  * @{
  */

#if defined (OPAMP1) || defined (OPAMP2)

/** @defgroup OPAMP_LL OPAMP
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
/** @defgroup OPAMP_LL_Private_Constants OPAMP Private Constants
  * @{
  */
/* Internal mask for OPAMP power mode:                                        */
/* To select into literal LL_OPAMP_POWERMODE_x the relevant bits for:         */
/* - OPAMP power mode into control register                                   */
/* - OPAMP trimming register offset                                           */

/* Internal register offset for OPAMP trimming configuration */
#define OPAMP_POWERMODE_OTR_REGOFFSET       ((uint32_t)0x00000000)
#define OPAMP_POWERMODE_LPOTR_REGOFFSET     ((uint32_t)0x00000001)
#define OPAMP_POWERMODE_OTR_REGOFFSET_MASK  (OPAMP_POWERMODE_OTR_REGOFFSET | OPAMP_POWERMODE_LPOTR_REGOFFSET)

/* Mask for OPAMP power mode into control register */
#define OPAMP_POWERMODE_CSR_BIT_MASK        (OPAMP_CSR_OPALPM)

/* Internal mask for OPAMP trimming of transistors differential pair NMOS     */
/* or PMOS.                                                                   */
/* To select into literal LL_OPAMP_TRIMMING_x the relevant bits for:          */
/* - OPAMP trimming selection of transistors differential pair                */
/* - OPAMP trimming values of transistors differential pair                   */
#define OPAMP_TRIMMING_SELECT_MASK          (OPAMP1_CSR_CALSEL)
#define OPAMP_TRIMMING_VALUE_MASK           (OPAMP_OTR_TRIMOFFSETP | OPAMP_OTR_TRIMOFFSETN)

/**
  * @}
  */


/* Private macros ------------------------------------------------------------*/
/** @defgroup OPAMP_LL_Private_Macros OPAMP Private Macros
  * @{
  */

/**
  * @brief  Driver macro reserved for internal use: set a pointer to
  *         a register from a register basis from which an offset
  *         is applied.
  * @param  __REG__ Register basis from which the offset is applied.
  * @param  __REG_OFFSET__ Offset to be applied (unit: number of registers).
  * @retval Register address
*/
#define __OPAMP_PTR_REG_OFFSET(__REG__, __REG_OFFSET__)                        \
 ((uint32_t *)((uint32_t) ((uint32_t)(&(__REG__)) + ((__REG_OFFSET__) << 2))))

/**
  * @}
  */


/* Exported types ------------------------------------------------------------*/
#if defined(USE_FULL_LL_DRIVER)
/** @defgroup OPAMP_LL_ES_INIT OPAMP Exported Init structure
  * @{
  */

/**
  * @brief  Structure definition of some features of OPAMP instance.
  */
typedef struct
{
  uint32_t PowerMode;                   /*!< Set OPAMP power mode.
                                             This parameter can be a value of @ref OPAMP_LL_EC_POWERMODE
                                             
                                             This feature can be modified afterwards using unitary function @ref LL_OPAMP_SetPowerMode(). */

  uint32_t FunctionalMode;              /*!< Set OPAMP functional mode by setting internal connections: OPAMP operation in standalone, follower, ...
                                             This parameter can be a value of @ref OPAMP_LL_EC_FUNCTIONAL_MODE
                                             @note If OPAMP is configured in mode PGA, the gain can be configured using function @ref LL_OPAMP_SetPGAGain().
                                             
                                             This feature can be modified afterwards using unitary function @ref LL_OPAMP_SetFunctionalMode(). */

  uint32_t InputNonInverting;           /*!< Set OPAMP input non-inverting connection.
                                             This parameter can be a value of @ref OPAMP_LL_EC_INPUT_NONINVERTING
                                             
                                             This feature can be modified afterwards using unitary function @ref LL_OPAMP_SetInputNonInverting(). */

  uint32_t InputInverting;              /*!< Set OPAMP inverting input connection.
                                             This parameter can be a value of @ref OPAMP_LL_EC_INPUT_INVERTING
                                             @note OPAMP inverting input is used with OPAMP in mode standalone or PGA with external capacitors for filtering circuit. Otherwise (OPAMP in mode follower), OPAMP inverting input is not used (not connected to GPIO pin), this parameter is discarded.
                                             
                                             This feature can be modified afterwards using unitary function @ref LL_OPAMP_SetInputInverting(). */

} LL_OPAMP_InitTypeDef;

/**
  * @}
  */
#endif /* USE_FULL_LL_DRIVER */

/* Exported constants --------------------------------------------------------*/
/** @defgroup OPAMP_LL_Exported_Constants OPAMP Exported Constants
  * @{
  */

/** @defgroup OPAMP_LL_EC_POWERSUPPLY_RANGE OPAMP power supply range
  * @{
  */
#define LL_OPAMP_POWERSUPPLY_RANGE_LOW  ((uint32_t)0x00000000) /*!< Power supply range low. On STM32L4: Vdda lower than 2.4V. */
#define LL_OPAMP_POWERSUPPLY_RANGE_HIGH (OPAMP1_CSR_OPARANGE)  /*!< Power supply range high. On STM32L4: Vdda higher than 2.4V. */

/**
  * @}
  */

/** @defgroup OPAMP_LL_EC_POWERMODE OPAMP power mode
  * @{
  */
#define LL_OPAMP_POWERMODE_NORMAL       (OPAMP_POWERMODE_OTR_REGOFFSET)                      /*!< OPAMP power mode normal */
#define LL_OPAMP_POWERMODE_LOWPOWER     (OPAMP_POWERMODE_LPOTR_REGOFFSET | OPAMP_CSR_OPALPM) /*!< OPAMP power mode low-power */
/**
  * @}
  */

/** @defgroup OPAMP_LL_EC_MODE OPAMP mode calibration or functional.
  * @{
  */
#define LL_OPAMP_MODE_FUNCTIONAL        ((uint32_t)0x00000000)                      /*!< OPAMP functional mode */
#define LL_OPAMP_MODE_CALIBRATION       (OPAMP1_CSR_CALON)                          /*!< OPAMP calibration mode */
/**
  * @}
  */

/** @defgroup OPAMP_LL_EC_FUNCTIONAL_MODE OPAMP functional mode
  * @{
  */
#define LL_OPAMP_MODE_STANDALONE        ((uint32_t)0x00000000)                      /*!< OPAMP functional mode, OPAMP operation in standalone */
#define LL_OPAMP_MODE_FOLLOWER          (OPAMP_CSR_OPAMODE_1 | OPAMP_CSR_OPAMODE_0) /*!< OPAMP functional mode, OPAMP operation in follower */
#define LL_OPAMP_MODE_PGA               (OPAMP_CSR_OPAMODE_1)                       /*!< OPAMP functional mode, OPAMP operation in PGA */
/**
  * @}
  */

/** @defgroup OPAMP_LL_EC_MODE_PGA_GAIN OPAMP PGA gain (relevant when OPAMP is in functional mode PGA)
  * @{
  */
#define LL_OPAMP_PGA_GAIN_2             ((uint32_t)0x00000000)                     /*!< OPAMP PGA gain 2 */
#define LL_OPAMP_PGA_GAIN_4             (OPAMP_CSR_PGGAIN_0)                       /*!< OPAMP PGA gain 4 */
#define LL_OPAMP_PGA_GAIN_8             (OPAMP_CSR_PGGAIN_1)                       /*!< OPAMP PGA gain 8 */
#define LL_OPAMP_PGA_GAIN_16            (OPAMP_CSR_PGGAIN_1 | OPAMP_CSR_PGGAIN_0 ) /*!< OPAMP PGA gain 16 */
/**
  * @}
  */

/** @defgroup OPAMP_LL_EC_INPUT_NONINVERTING OPAMP input non-inverting
  * @{
  */
#define LL_OPAMP_INPUT_NONINVERT_IO0      ((uint32_t)0x00000000) /*!< OPAMP non inverting input connected to GPIO pin */
#define LL_OPAMP_INPUT_NONINVERT_DAC1_CH1 (OPAMP1_CSR_VPSEL)     /*!< OPAMP non inverting input connected to DAC1 channel1 output */
/**
  * @}
  */

/** @defgroup OPAMP_LL_EC_INPUT_INVERTING OPAMP input inverting
  * @{
  */
#define LL_OPAMP_INPUT_INVERT_IO0        ((uint32_t)0x00000000) /*!< OPAMP inverting input connected to GPIO pin (valid also in PGA mode for filtering). Note: OPAMP inverting input is used with OPAMP in mode standalone or PGA with external capacitors for filtering circuit. Otherwise (OPAMP in mode follower), OPAMP inverting input is not used (not connected to GPIO pin). */
#define LL_OPAMP_INPUT_INVERT_IO1        (OPAMP1_CSR_VMSEL_0)   /*!< OPAMP inverting input (low leakage input) connected to GPIO pin (available only on package BGA132). Note: OPAMP inverting input is used with OPAMP in mode standalone or PGA with external capacitors for filtering circuit. Otherwise (OPAMP in mode follower), OPAMP inverting input is not used (not connected to GPIO pin). */
#define LL_OPAMP_INPUT_INVERT_CONNECT_NO (OPAMP1_CSR_VMSEL_1)   /*!< OPAMP inverting input not externally connected (intended for OPAMP in mode follower or PGA without external capacitors for filtering) */
/**
  * @}
  */

/** @defgroup OPAMP_LL_EC_INPUT_LEGACY OPAMP inputs legacy literals name
  * @{
  */
#define LL_OPAMP_NONINVERTINGINPUT_IO0      LL_OPAMP_INPUT_NONINVERT_IO0
#define LL_OPAMP_NONINVERTINGINPUT_DAC_CH   LL_OPAMP_INPUT_NONINVERT_DAC1_CH1

#define LL_OPAMP_INVERTINGINPUT_IO0         LL_OPAMP_INPUT_INVERT_IO0
#define LL_OPAMP_INVERTINGINPUT_IO1         LL_OPAMP_INPUT_INVERT_IO1
#define LL_OPAMP_INVERTINGINPUT_CONNECT_NO  LL_OPAMP_INPUT_INVERT_CONNECT_NO
/**
  * @}
  */

/** @defgroup OPAMP_LL_EC_TRIMMING_MODE OPAMP trimming mode
  * @{
  */
#define LL_OPAMP_TRIMMING_FACTORY       ((uint32_t)0x00000000) /*!< Factory trimming */
#define LL_OPAMP_TRIMMING_USER          (OPAMP_CSR_USERTRIM)   /*!< User trimming */
/**
  * @}
  */

/** @defgroup OPAMP_LL_EC_TRIMMING_TRANSISTORS_DIFF_PAIR OPAMP trimming of transistors differential pair NMOS or PMOS
  * @{
  */
#define LL_OPAMP_TRIMMING_NMOS          (OPAMP_OTR_TRIMOFFSETN)                     /*!< Trimming of transistors differential pair NMOS */
#define LL_OPAMP_TRIMMING_PMOS          (OPAMP_OTR_TRIMOFFSETP | OPAMP1_CSR_CALSEL) /*!< Trimming of transistors differential pair PMOS */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup OPAMP_LL_Exported_Macros OPAMP Exported Macros
  * @{
  */
/** @defgroup OPAMP_LL_EM_WRITE_READ Common write and read registers macro
  * @{
  */
/**
  * @brief  Write a value in OPAMP register
  * @param  __INSTANCE__ OPAMP Instance
  * @param  __REG__ Register to be written
  * @param  __VALUE__ Value to be written in the register
  * @retval None
  */
#define LL_OPAMP_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG(__INSTANCE__->__REG__, (__VALUE__))

/**
  * @brief  Read a value in OPAMP register
  * @param  __INSTANCE__ OPAMP Instance
  * @param  __REG__ Register to be read
  * @retval Register value
  */
#define LL_OPAMP_ReadReg(__INSTANCE__, __REG__) READ_REG(__INSTANCE__->__REG__)
/**
  * @}
  */

/** @defgroup OPAMP_LL_EM_HELPER_MACRO OPAMP helper macro
  * @{
  */

/**
  * @brief  Helper macro to select the OPAMP common instance
  *         to which is belonging the selected OPAMP instance.
  * @note   OPAMP common register instance can be used to
  *         set parameters common to several OPAMP instances.
  *         Refer to functions having argument "OPAMPxy_COMMON" as parameter.
  * @param  __OPAMPx__ OPAMP instance
  * @retval OPAMP common instance
  */
#if defined(OPAMP1) && defined(OPAMP2)
#define __LL_OPAMP_COMMON_INSTANCE(__OPAMPx__)                                   \
  (OPAMP12_COMMON)
#else
#define __LL_OPAMP_COMMON_INSTANCE(__OPAMPx__)                                   \
  (OPAMP1_COMMON)
#endif


/**
  * @brief  Helper macro to check if all OPAMP instances sharing the same
  *         OPAMP common instance are disabled.
  * @note   This check is required by functions with setting conditioned to
  *         OPAMP state:
  *         All OPAMP instances of the OPAMP common group must be disabled.
  *         Refer to functions having argument "OPAMPxy_COMMON" as parameter.
  * @retval 0: All OPAMP instances sharing the same OPAMP common instance
  *            are disabled.
  *         1: At least one OPAMP instance sharing the same OPAMP common instance
  *            is enabled
  */
#if defined(OPAMP1) && defined(OPAMP2)
#define __LL_OPAMP_IS_ENABLED_ALL_COMMON_INSTANCE()                            \
  (LL_OPAMP_IsEnabled(OPAMP1) |                                                \
   LL_OPAMP_IsEnabled(OPAMP2)  )
#else
#define __LL_OPAMP_IS_ENABLED_ALL_COMMON_INSTANCE()                            \
  (LL_OPAMP_IsEnabled(OPAMP1))
#endif

/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup OPAMP_LL_Exported_Functions OPAMP Exported Functions
  * @{
  */

/** @defgroup OPAMP_LL_EF_Configuration_opamp_common Configuration of OPAMP hierarchical scope: common to several OPAMP instances
  * @{
  */

/**
  * @brief  Set OPAMP power range.
  * @note   The OPAMP power range applies to several OPAMP instances
  *         (if several OPAMP instances available on the selected device).
  * @note   On this STM32 family, setting of this feature is conditioned to
  *         OPAMP state:
  *         All OPAMP instances of the OPAMP common group must be disabled.
  *         This check can be done with function @ref LL_OPAMP_IsEnabled() for each
  *         OPAMP instance or by using helper macro
  *         @ref __LL_OPAMP_IS_ENABLED_ALL_COMMON_INSTANCE().
  * @rmtoll CSR    OPARANGE       LL_OPAMP_SetCommonPowerRange
  * @param  OPAMPxy_COMMON OPAMP common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_OPAMP_COMMON_INSTANCE() )
  * @param  PowerRange This parameter can be one of the following values:
  *         @arg @ref LL_OPAMP_POWERSUPPLY_RANGE_LOW
  *         @arg @ref LL_OPAMP_POWERSUPPLY_RANGE_HIGH
  * @retval None
  */
__STATIC_INLINE void LL_OPAMP_SetCommonPowerRange(OPAMP_Common_TypeDef *OPAMPxy_COMMON, uint32_t PowerRange)
{
  MODIFY_REG(OPAMP1->CSR, OPAMP1_CSR_OPARANGE, PowerRange);
}

/**
  * @brief  Get OPAMP power range.
  * @note   The OPAMP power range applies to several OPAMP instances
  *         (if several OPAMP instances available on the selected device).
  * @note   On this STM32 family, setting of this feature is conditioned to
  *         OPAMP state:
  *         All OPAMP instances of the OPAMP common group must be disabled.
  *         This check can be done with function @ref LL_OPAMP_IsEnabled() for each
  *         OPAMP instance or by using helper macro
  *         @ref __LL_OPAMP_IS_ENABLED_ALL_COMMON_INSTANCE().
  * @rmtoll CSR    OPARANGE       LL_OPAMP_GetCommonPowerRange
  * @param  OPAMPxy_COMMON OPAMP common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_OPAMP_COMMON_INSTANCE() )
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_OPAMP_POWERSUPPLY_RANGE_LOW
  *         @arg @ref LL_OPAMP_POWERSUPPLY_RANGE_HIGH
  */
__STATIC_INLINE uint32_t LL_OPAMP_GetCommonPowerRange(OPAMP_Common_TypeDef *OPAMPxy_COMMON)
{
  return (uint32_t)(READ_BIT(OPAMP1->CSR, OPAMP1_CSR_OPARANGE));
}

/**
  * @}
  */

/** @defgroup OPAMP_LL_EF_CONFIGURATION_OPAMP_INSTANCE Configuration of OPAMP hierarchical scope: OPAMP instance
  * @{
  */

/**
  * @brief  Set OPAMP power mode.
  * @note   The OPAMP must be disabled to change this configuration.
  * @rmtoll CSR    OPALPM         LL_OPAMP_SetPowerMode
  * @param  OPAMPx OPAMP instance
  * @param  PowerMode This parameter can be one of the following values:
  *         @arg @ref LL_OPAMP_POWERMODE_NORMAL
  *         @arg @ref LL_OPAMP_POWERMODE_LOWPOWER
  * @retval None
  */
__STATIC_INLINE void LL_OPAMP_SetPowerMode(OPAMP_TypeDef *OPAMPx, uint32_t PowerMode)
{
  MODIFY_REG(OPAMPx->CSR, OPAMP_CSR_OPALPM, (PowerMode & OPAMP_POWERMODE_CSR_BIT_MASK));
}

/**
  * @brief  Get OPAMP power mode.
  * @rmtoll CSR    OPALPM         LL_OPAMP_GetPowerMode
  * @param  OPAMPx OPAMP instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_OPAMP_POWERMODE_NORMAL
  *         @arg @ref LL_OPAMP_POWERMODE_LOWPOWER
  */
__STATIC_INLINE uint32_t LL_OPAMP_GetPowerMode(OPAMP_TypeDef *OPAMPx)
{
  register uint32_t power_mode = (READ_BIT(OPAMPx->CSR, OPAMP_CSR_OPALPM));
  
  return (uint32_t)(power_mode | (power_mode >> (POSITION_VAL(OPAMP_CSR_OPALPM))));
}

/**
  * @brief  Set OPAMP mode calibration or functional.
  * @note   OPAMP mode corresponds to functional or calibration mode:
  *          - functional mode: OPAMP operation in standalone, follower, ...
  *            Set functional mode using function
  *            @ref LL_OPAMP_SetFunctionalMode().
  *          - calibration mode: offset calibration of the selected
  *            transistors differential pair NMOS or PMOS.
  * @note   On this STM32 family, during calibration, OPAMP functional
  *         mode must be set to standalone or follower mode
  *         (in order to open internal connections to resistors
  *         of PGA mode).
  *         Refer to function @ref LL_OPAMP_SetFunctionalMode().
  * @rmtoll CSR    CALON          LL_OPAMP_SetMode
  * @param  OPAMPx OPAMP instance
  * @param  Mode This parameter can be one of the following values:
  *         @arg @ref LL_OPAMP_MODE_FUNCTIONAL
  *         @arg @ref LL_OPAMP_MODE_CALIBRATION
  * @retval None
  */
__STATIC_INLINE void LL_OPAMP_SetMode(OPAMP_TypeDef *OPAMPx, uint32_t Mode)
{
  MODIFY_REG(OPAMPx->CSR, OPAMP_CSR_CALON, Mode);
}

/**
  * @brief  Get OPAMP mode calibration or functional.
  * @note   OPAMP mode corresponds to functional or calibration mode:
  *          - functional mode: OPAMP operation in standalone, follower, ...
  *            Set functional mode using function
  *            @ref LL_OPAMP_SetFunctionalMode().
  *          - calibration mode: offset calibration of the selected
  *            transistors differential pair NMOS or PMOS.
  * @rmtoll CSR    CALON          LL_OPAMP_GetMode
  * @param  OPAMPx OPAMP instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_OPAMP_MODE_FUNCTIONAL
  *         @arg @ref LL_OPAMP_MODE_CALIBRATION
  */
__STATIC_INLINE uint32_t LL_OPAMP_GetMode(OPAMP_TypeDef *OPAMPx)
{
  return (uint32_t)(READ_BIT(OPAMPx->CSR, OPAMP_CSR_CALON));
}

/**
  * @brief  Set OPAMP functional mode by setting internal connections.
  *         OPAMP operation in standalone, follower, ...
  * @note   This function reset bit of calibration mode to ensure
  *         to be in functional mode, in order to have OPAMP parameters
  *         (inputs selection, ...) set with the corresponding OPAMP mode
  *         to be effective.
  * @rmtoll CSR    OPAMODE        LL_OPAMP_SetFunctionalMode
  * @param  OPAMPx OPAMP instance
  * @param  FunctionalMode This parameter can be one of the following values:
  *         @arg @ref LL_OPAMP_MODE_STANDALONE
  *         @arg @ref LL_OPAMP_MODE_FOLLOWER
  *         @arg @ref LL_OPAMP_MODE_PGA
  * @retval None
  */
__STATIC_INLINE void LL_OPAMP_SetFunctionalMode(OPAMP_TypeDef *OPAMPx, uint32_t FunctionalMode)
{
  /* Note: Bit OPAMP_CSR_CALON reset to ensure to be in functional mode */
  MODIFY_REG(OPAMPx->CSR, OPAMP_CSR_OPAMODE | OPAMP_CSR_CALON, FunctionalMode);
}

/**
  * @brief  Get OPAMP functional mode from setting of internal connections.
  *         OPAMP operation in standalone, follower, ...
  * @rmtoll CSR    OPAMODE        LL_OPAMP_GetFunctionalMode
  * @param  OPAMPx OPAMP instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_OPAMP_MODE_STANDALONE
  *         @arg @ref LL_OPAMP_MODE_FOLLOWER
  *         @arg @ref LL_OPAMP_MODE_PGA
  */
__STATIC_INLINE uint32_t LL_OPAMP_GetFunctionalMode(OPAMP_TypeDef *OPAMPx)
{
  return (uint32_t)(READ_BIT(OPAMPx->CSR, OPAMP_CSR_OPAMODE));
}

/**
  * @brief  Set OPAMP PGA gain.
  * @note   Preliminarily, OPAMP must be set in mode PGA
  *         using function @ref LL_OPAMP_SetFunctionalMode().
  * @rmtoll CSR    PGGAIN         LL_OPAMP_SetPGAGain
  * @param  OPAMPx OPAMP instance
  * @param  PGAGain This parameter can be one of the following values:
  *         @arg @ref LL_OPAMP_PGA_GAIN_2
  *         @arg @ref LL_OPAMP_PGA_GAIN_4
  *         @arg @ref LL_OPAMP_PGA_GAIN_8
  *         @arg @ref LL_OPAMP_PGA_GAIN_16
  * @retval None
  */
__STATIC_INLINE void LL_OPAMP_SetPGAGain(OPAMP_TypeDef *OPAMPx, uint32_t PGAGain)
{
  MODIFY_REG(OPAMPx->CSR, OPAMP_CSR_PGGAIN, PGAGain);
}

/**
  * @brief  Get OPAMP PGA gain.
  * @note   Preliminarily, OPAMP must be set in mode PGA
  *         using function @ref LL_OPAMP_SetFunctionalMode().
  * @rmtoll CSR    PGGAIN         LL_OPAMP_GetPGAGain
  * @param  OPAMPx OPAMP instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_OPAMP_PGA_GAIN_2
  *         @arg @ref LL_OPAMP_PGA_GAIN_4
  *         @arg @ref LL_OPAMP_PGA_GAIN_8
  *         @arg @ref LL_OPAMP_PGA_GAIN_16
  */
__STATIC_INLINE uint32_t LL_OPAMP_GetPGAGain(OPAMP_TypeDef *OPAMPx)
{
  return (uint32_t)(READ_BIT(OPAMPx->CSR, OPAMP_CSR_PGGAIN));
}

/**
  * @}
  */

/** @defgroup OPAMP_LL_EF_CONFIGURATION_INPUTS Configuration of OPAMP inputs
  * @{
  */

/**
  * @brief  Set OPAMP non-inverting input connection.
  * @rmtoll CSR    VPSEL          LL_OPAMP_SetInputNonInverting
  * @param  OPAMPx OPAMP instance
  * @param  InputNonInverting This parameter can be one of the following values:
  *         @arg @ref LL_OPAMP_INPUT_NONINVERT_IO0
  *         @arg @ref LL_OPAMP_INPUT_NONINVERT_DAC1_CH1
  * @retval None
  */
__STATIC_INLINE void LL_OPAMP_SetInputNonInverting(OPAMP_TypeDef *OPAMPx, uint32_t InputNonInverting)
{
  MODIFY_REG(OPAMPx->CSR, OPAMP_CSR_VPSEL, InputNonInverting);
}

/**
  * @brief  Get OPAMP non-inverting input connection.
  * @rmtoll CSR    VPSEL          LL_OPAMP_GetInputNonInverting
  * @param  OPAMPx OPAMP instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_OPAMP_INPUT_NONINVERT_IO0
  *         @arg @ref LL_OPAMP_INPUT_NONINVERT_DAC1_CH1
  */
__STATIC_INLINE uint32_t LL_OPAMP_GetInputNonInverting(OPAMP_TypeDef *OPAMPx)
{
  return (uint32_t)(READ_BIT(OPAMPx->CSR, OPAMP_CSR_VPSEL));
}

/**
  * @brief  Set OPAMP inverting input connection.
  * @note   OPAMP inverting input is used with OPAMP in mode standalone
  *         or PGA with external capacitors for filtering circuit.
  *         Otherwise (OPAMP in mode follower), OPAMP inverting input
  *         is not used (not connected to GPIO pin).
  * @rmtoll CSR    VMSEL          LL_OPAMP_SetInputInverting
  * @param  OPAMPx OPAMP instance
  * @param  InputInverting This parameter can be one of the following values:
  *         @arg @ref LL_OPAMP_INPUT_INVERT_IO0
  *         @arg @ref LL_OPAMP_INPUT_INVERT_IO1
  *         @arg @ref LL_OPAMP_INPUT_INVERT_CONNECT_NO
  * @retval None
  */
__STATIC_INLINE void LL_OPAMP_SetInputInverting(OPAMP_TypeDef *OPAMPx, uint32_t InputInverting)
{
  MODIFY_REG(OPAMPx->CSR, OPAMP_CSR_VMSEL, InputInverting);
}

/**
  * @brief  Get OPAMP inverting input connection.
  * @rmtoll CSR    VMSEL          LL_OPAMP_GetInputInverting
  * @param  OPAMPx OPAMP instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_OPAMP_INPUT_INVERT_IO0
  *         @arg @ref LL_OPAMP_INPUT_INVERT_IO1
  *         @arg @ref LL_OPAMP_INPUT_INVERT_CONNECT_NO
  */
__STATIC_INLINE uint32_t LL_OPAMP_GetInputInverting(OPAMP_TypeDef *OPAMPx)
{
  return (uint32_t)(READ_BIT(OPAMPx->CSR, OPAMP_CSR_VMSEL));
}

/**
  * @}
  */

/** @defgroup OPAMP_LL_EF_Configuration_Legacy_Functions Configuration of OPAMP, legacy functions name
  * @{
  */
/* Old functions name kept for legacy purpose, to be replaced by the          */
/* current functions name.                                                    */
__STATIC_INLINE void LL_OPAMP_SetNonInvertingInput(OPAMP_TypeDef *OPAMPx, uint32_t NonInvertingInput)
{
  LL_OPAMP_SetInputNonInverting(OPAMPx, NonInvertingInput);
}

__STATIC_INLINE void LL_OPAMP_SetInvertingInput(OPAMP_TypeDef *OPAMPx, uint32_t InvertingInput)
{
  LL_OPAMP_SetInputInverting(OPAMPx, InvertingInput);
}

/**
  * @}
  */

/** @defgroup OPAMP_LL_EF_OPAMP_TRIMMING Configuration and operation of OPAMP trimming
  * @{
  */

/**
  * @brief  Set OPAMP trimming mode.
  * @rmtoll CSR    USERTRIM       LL_OPAMP_SetTrimmingMode
  * @param  OPAMPx OPAMP instance
  * @param  TrimmingMode This parameter can be one of the following values:
  *         @arg @ref LL_OPAMP_TRIMMING_FACTORY
  *         @arg @ref LL_OPAMP_TRIMMING_USER
  * @retval None
  */
__STATIC_INLINE void LL_OPAMP_SetTrimmingMode(OPAMP_TypeDef *OPAMPx, uint32_t TrimmingMode)
{
  MODIFY_REG(OPAMPx->CSR, OPAMP_CSR_USERTRIM, TrimmingMode);
}

/**
  * @brief  Get OPAMP trimming mode.
  * @rmtoll CSR    USERTRIM       LL_OPAMP_GetTrimmingMode
  * @param  OPAMPx OPAMP instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_OPAMP_TRIMMING_FACTORY
  *         @arg @ref LL_OPAMP_TRIMMING_USER
  */
__STATIC_INLINE uint32_t LL_OPAMP_GetTrimmingMode(OPAMP_TypeDef *OPAMPx)
{
  return (uint32_t)(READ_BIT(OPAMPx->CSR, OPAMP_CSR_USERTRIM));
}

/**
  * @brief  Set OPAMP offset to calibrate the selected transistors
  *         differential pair NMOS or PMOS.
  * @note   Preliminarily, OPAMP must be set in mode calibration
  *         using function @ref LL_OPAMP_SetMode().
  * @rmtoll CSR    CALSEL         LL_OPAMP_SetCalibrationSelection
  * @param  OPAMPx OPAMP instance
  * @param  TransistorsDiffPair This parameter can be one of the following values:
  *         @arg @ref LL_OPAMP_TRIMMING_NMOS
  *         @arg @ref LL_OPAMP_TRIMMING_PMOS
  * @retval None
  */
__STATIC_INLINE void LL_OPAMP_SetCalibrationSelection(OPAMP_TypeDef *OPAMPx, uint32_t TransistorsDiffPair)
{
  /* Parameter used with mask "OPAMP_TRIMMING_SELECT_MASK" because            */
  /* containing other bits reserved for other purpose.                        */
  MODIFY_REG(OPAMPx->CSR, OPAMP_CSR_CALSEL, (TransistorsDiffPair & OPAMP_TRIMMING_SELECT_MASK));
}

/**
  * @brief  Get OPAMP offset to calibrate the selected transistors
  *         differential pair NMOS or PMOS.
  * @note   Preliminarily, OPAMP must be set in mode calibration
  *         using function @ref LL_OPAMP_SetMode().
  * @rmtoll CSR    CALSEL         LL_OPAMP_GetCalibrationSelection
  * @param  OPAMPx OPAMP instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_OPAMP_TRIMMING_NMOS
  *         @arg @ref LL_OPAMP_TRIMMING_PMOS
  */
__STATIC_INLINE uint32_t LL_OPAMP_GetCalibrationSelection(OPAMP_TypeDef *OPAMPx)
{
  register uint32_t CalibrationSelection = (uint32_t)(READ_BIT(OPAMPx->CSR, OPAMP_CSR_CALSEL));
  
  return (CalibrationSelection |
          ((OPAMP_OTR_TRIMOFFSETN) << (POSITION_VAL(OPAMP_OTR_TRIMOFFSETP) * (CalibrationSelection && OPAMP1_CSR_CALSEL))));
}

/**
  * @brief  Get OPAMP calibration result of toggling output.
  * @rmtoll CSR    CALOUT         LL_OPAMP_IsCalibrationOutputSet
  * @param  OPAMPx OPAMP instance
  * @retval 0 if OPAMP calibration output is reset,
  *         1 if OPAMP calibration output is set.
  */
__STATIC_INLINE uint32_t LL_OPAMP_IsCalibrationOutputSet(OPAMP_TypeDef *OPAMPx)
{
  return (READ_BIT(OPAMPx->CSR, OPAMP_CSR_CALOUT) == OPAMP_CSR_CALOUT);
}

/**
  * @brief  Set OPAMP trimming factor for the selected transistors
  *         differential pair NMOS or PMOS, corresponding to the selected
  *         power mode.
  * @param  OPAMPx OPAMP instance
  * @param  PowerMode This parameter can be one of the following values:
  *         @arg @ref LL_OPAMP_POWERMODE_NORMAL
  *         @arg @ref LL_OPAMP_POWERMODE_LOWPOWER
  * @param  TransistorsDiffPair This parameter can be one of the following values:
  *         @arg @ref LL_OPAMP_TRIMMING_NMOS
  *         @arg @ref LL_OPAMP_TRIMMING_PMOS
  * @param  TrimmingValue 0x00...0x1F
  * @retval None
  */
__STATIC_INLINE void LL_OPAMP_SetTrimmingValue(OPAMP_TypeDef* OPAMPx, uint32_t PowerMode, uint32_t TransistorsDiffPair, uint32_t TrimmingValue)
{
  register uint32_t *preg = __OPAMP_PTR_REG_OFFSET(OPAMPx->OTR, (PowerMode & OPAMP_POWERMODE_OTR_REGOFFSET_MASK));
  
  /* Set bits with position in register depending on parameter                */
  /* "TransistorsDiffPair".                                                   */
  /* Parameter used with mask "OPAMP_TRIMMING_VALUE_MASK" because             */
  /* containing other bits reserved for other purpose.                        */
  MODIFY_REG(*preg,
             (TransistorsDiffPair & OPAMP_TRIMMING_VALUE_MASK),
             TrimmingValue << (POSITION_VAL(TransistorsDiffPair & OPAMP_TRIMMING_VALUE_MASK)));
}

/**
  * @brief  Get OPAMP trimming factor for the selected transistors
  *         differential pair NMOS or PMOS, corresponding to the selected
  *         power mode.
  * @param  OPAMPx OPAMP instance
  * @param  PowerMode This parameter can be one of the following values:
  *         @arg @ref LL_OPAMP_POWERMODE_NORMAL
  *         @arg @ref LL_OPAMP_POWERMODE_LOWPOWER
  * @param  TransistorsDiffPair This parameter can be one of the following values:
  *         @arg @ref LL_OPAMP_TRIMMING_NMOS
  *         @arg @ref LL_OPAMP_TRIMMING_PMOS
  * @retval 0x0...0x1F
  */
__STATIC_INLINE uint32_t LL_OPAMP_GetTrimmingValue(OPAMP_TypeDef* OPAMPx, uint32_t PowerMode, uint32_t TransistorsDiffPair)
{
  register uint32_t *preg = __OPAMP_PTR_REG_OFFSET(OPAMPx->OTR, (PowerMode & OPAMP_POWERMODE_OTR_REGOFFSET_MASK));

  /* Retrieve bits with position in register depending on parameter           */
  /* "TransistorsDiffPair".                                                   */
  /* Parameter used with mask "OPAMP_TRIMMING_VALUE_MASK" because             */
  /* containing other bits reserved for other purpose.                        */
  return (uint32_t)(READ_BIT(*preg, (TransistorsDiffPair & OPAMP_TRIMMING_VALUE_MASK))
                    >> (POSITION_VAL((TransistorsDiffPair & OPAMP_TRIMMING_VALUE_MASK)))
                   );
}

/**
  * @}
  */

/** @defgroup OPAMP_LL_EF_OPERATION Operation on OPAMP instance
  * @{
  */
/**
  * @brief  Enable OPAMP instance.
  * @note   After enable from off state, OPAMP requires a delay
  *         to fullfill wake up time specification.
  *         Refer to device datasheet, parameter "tWAKEUP".
  * @rmtoll CSR    OPAMPXEN       LL_OPAMP_Enable
  * @param  OPAMPx OPAMP instance
  * @retval None
  */
__STATIC_INLINE void LL_OPAMP_Enable(OPAMP_TypeDef *OPAMPx)
{
  SET_BIT(OPAMPx->CSR, OPAMP_CSR_OPAMPxEN);
}

/**
  * @brief  Disable OPAMP instance.
  * @rmtoll CSR    OPAMPXEN       LL_OPAMP_Disable
  * @param  OPAMPx OPAMP instance
  * @retval None
  */
__STATIC_INLINE void LL_OPAMP_Disable(OPAMP_TypeDef *OPAMPx)
{
  CLEAR_BIT(OPAMPx->CSR, OPAMP_CSR_OPAMPxEN);
}

/**
  * @brief  Get OPAMP instance enable state
  *         (0: OPAMP is disabled, 1: OPAMP is enabled)
  * @rmtoll CSR    OPAMPXEN       LL_OPAMP_IsEnabled
  * @param  OPAMPx OPAMP instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_OPAMP_IsEnabled(OPAMP_TypeDef *OPAMPx)
{
  return (READ_BIT(OPAMPx->CSR, OPAMP_CSR_OPAMPxEN) == (OPAMP_CSR_OPAMPxEN));
}

/**
  * @}
  */

#if defined(USE_FULL_LL_DRIVER)
/** @defgroup OPAMP_LL_EF_Init Initialization and de-initialization functions
  * @{
  */

ErrorStatus LL_OPAMP_DeInit(OPAMP_TypeDef *OPAMPx);
ErrorStatus LL_OPAMP_Init(OPAMP_TypeDef *OPAMPx, LL_OPAMP_InitTypeDef *OPAMP_InitStruct);
void        LL_OPAMP_StructInit(LL_OPAMP_InitTypeDef *OPAMP_InitStruct);

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

#endif /* OPAMP1 || OPAMP2 */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32L4xx_LL_OPAMP_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
