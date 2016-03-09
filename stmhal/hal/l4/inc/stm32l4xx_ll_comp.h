/**
  ******************************************************************************
  * @file    stm32l4xx_ll_comp.h
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   Header file of COMP LL module.
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
#ifndef __STM32L4xx_LL_COMP_H
#define __STM32L4xx_LL_COMP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx.h"

/** @addtogroup STM32L4xx_LL_Driver
  * @{
  */

#if defined (COMP1) || defined (COMP2)

/** @defgroup COMP_LL COMP
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
#if defined(USE_FULL_LL_DRIVER)
/** @defgroup COMP_LL_ES_INIT COMP Exported Init structure
  * @{
  */

/**
  * @brief  Structure definition of some features of COMP instance.
  */
typedef struct
{
  uint32_t PowerMode;                   /*!< Set comparator operating mode to adjust power and speed.
                                             This parameter can be a value of @ref COMP_LL_EC_POWERMODE
                                             
                                             This feature can be modified afterwards using unitary function @ref LL_COMP_SetPowerMode(). */

  uint32_t InputPlus;                   /*!< Set comparator input non-inverting.
                                             This parameter can be a value of @ref COMP_LL_EC_INPUT_PLUS
                                             
                                             This feature can be modified afterwards using unitary function @ref LL_COMP_SetInputPlus(). */

  uint32_t InputMinus;                  /*!< Set comparator input minus (inverting).
                                             This parameter can be a value of @ref COMP_LL_EC_INPUT_MINUS
                                             
                                             This feature can be modified afterwards using unitary function @ref LL_COMP_SetInputMinus(). */

  uint32_t InputHysteresis;             /*!< Set comparator hysteresis mode of the minus (inverting) input.
                                             This parameter can be a value of @ref COMP_LL_EC_INPUT_HYSTERESIS
                                             
                                             This feature can be modified afterwards using unitary function @ref LL_COMP_SetInputHysteresis(). */

  uint32_t OutputPolarity;              /*!< Set comparator output polarity.
                                             This parameter can be a value of @ref COMP_LL_EC_OUTPUT_POLARITY
                                             
                                             This feature can be modified afterwards using unitary function @ref LL_COMP_SetOutputPolarity(). */

  uint32_t OutputBlankingSource;        /*!< Set comparator blanking source.
                                             This parameter can be a value of @ref COMP_LL_EC_OUTPUT_BLANKING_SOURCE
                                             
                                             This feature can be modified afterwards using unitary function @ref LL_COMP_SetOutputBlankingSource(). */

} LL_COMP_InitTypeDef;

/**
  * @}
  */
#endif /* USE_FULL_LL_DRIVER */

/* Exported constants --------------------------------------------------------*/
/** @defgroup COMP_LL_Exported_Constants COMP Exported Constants
  * @{
  */

/** @defgroup COMP_LL_EC_COMMON_WINDOWMODE Comparator common modes - Window mode
  * @{
  */
#define LL_COMP_WINDOWMODE_DISABLE      ((uint32_t)0x00000000) /*!< Window mode disable: Comparators 1 and 2 are independent (no input connection) */
#define LL_COMP_WINDOWMODE_ENABLE       (COMP_CSR_WINMODE)     /*!< Window mode enable: Comparators 1 and 2 have their input plus (non-inverting) connected together. The common input to be used is the one of COMP1, COMP2 plus (non-inverting) input is no more accessible.  */
/**
  * @}
  */

/** @defgroup COMP_LL_EC_POWERMODE Comparator modes - Power mode
  * @{
  */
#define LL_COMP_POWERMODE_HIGHSPEED     ((uint32_t)0x00000000)                    /*!< High speed */
#define LL_COMP_POWERMODE_MEDIUMSPEED   (COMP_CSR_PWRMODE_0)                      /*!< Medium speed */
#define LL_COMP_POWERMODE_ULTRALOWPOWER (COMP_CSR_PWRMODE_1 | COMP_CSR_PWRMODE_0) /*!< Ultra-low power mode */
/**
  * @}
  */

/** @defgroup COMP_LL_EC_INPUT_PLUS Comparator inputs - Input plus (input non-inverting) selection
  * @{
  */
#define LL_COMP_INPUT_PLUS_IO1          ((uint32_t)0x00000000)                  /*!< IO1 connected to comparator non inverting input (pin PC5 for COMP1, pin PB4 for COMP2) */
#define LL_COMP_INPUT_PLUS_IO2          (COMP_CSR_INPSEL_0)                     /*!< IO2 connected to comparator non inverting input (pin PB2 for COMP1, pin PB6 for COMP2) */
/**
  * @}
  */

/** @defgroup COMP_LL_EC_INPUT_MINUS Comparator inputs - Input minus (input inverting) selection
  * @{
  */
#define LL_COMP_INPUT_MINUS_1_4VREFINT  (                                                            COMP_CSR_SCALEN | COMP_CSR_BRGEN) /*!< 1/4 VREFINT connected to comparator minus (inverting) input */
#define LL_COMP_INPUT_MINUS_1_2VREFINT  (                                        COMP_CSR_INMSEL_0 | COMP_CSR_SCALEN | COMP_CSR_BRGEN) /*!< 1/2 VREFINT connected to comparator minus (inverting) input */
#define LL_COMP_INPUT_MINUS_3_4VREFINT  (                    COMP_CSR_INMSEL_1                     | COMP_CSR_SCALEN | COMP_CSR_BRGEN) /*!< 3/4 VREFINT connected to comparator minus (inverting) input */
#define LL_COMP_INPUT_MINUS_VREFINT     (                    COMP_CSR_INMSEL_1 | COMP_CSR_INMSEL_0 | COMP_CSR_SCALEN                 ) /*!< VREFINT connected to comparator1 minus (inverting) input */
#define LL_COMP_INPUT_MINUS_DAC1_CH1    (COMP_CSR_INMSEL_2                                        ) /*!< DAC1 channel 1 (DAC_OUT1) connected to comparator minus (inverting) input */
#define LL_COMP_INPUT_MINUS_DAC1_CH2    (COMP_CSR_INMSEL_2                     | COMP_CSR_INMSEL_0) /*!< DAC1 channel 2 (DAC_OUT2) connected to comparator minus (inverting) input */
#define LL_COMP_INPUT_MINUS_IO1         (COMP_CSR_INMSEL_2 | COMP_CSR_INMSEL_1                    ) /*!< IO1 connected to comparator minus (inverting) input (pin PB1 for COMP1, pin PB3 for COMP2) */
#define LL_COMP_INPUT_MINUS_IO2         (COMP_CSR_INMSEL_2 | COMP_CSR_INMSEL_1 | COMP_CSR_INMSEL_0) /*!< IO2 connected to comparator minus (inverting) input (pin PC4 for COMP1, pin PB7 for COMP2) */
/**
  * @}
  */

/** @defgroup COMP_LL_EC_INPUT_LEGACY Comparator inputs legacy literals name
  * @{
  */
#define LL_COMP_INVERTINGINPUT_1_4VREFINT LL_COMP_INPUT_MINUS_1_4VREFINT
#define LL_COMP_INVERTINGINPUT_1_2VREFINT LL_COMP_INPUT_MINUS_1_2VREFINT
#define LL_COMP_INVERTINGINPUT_3_4VREFINT LL_COMP_INPUT_MINUS_3_4VREFINT
#define LL_COMP_INVERTINGINPUT_VREFINT    LL_COMP_INPUT_MINUS_VREFINT
#define LL_COMP_INVERTINGINPUT_DAC1       LL_COMP_INPUT_MINUS_DAC1_CH1
#define LL_COMP_INVERTINGINPUT_DAC2       LL_COMP_INPUT_MINUS_DAC1_CH2
#define LL_COMP_INVERTINGINPUT_IO1        LL_COMP_INPUT_MINUS_IO1
#define LL_COMP_INVERTINGINPUT_IO2        LL_COMP_INPUT_MINUS_IO2

#define LL_COMP_NONINVERTINGINPUT_IO1     LL_COMP_INPUT_PLUS_IO1
#define LL_COMP_NONINVERTINGINPUT_IO2     LL_COMP_INPUT_PLUS_IO2
/**
  * @}
  */

/** @defgroup COMP_LL_EC_INPUT_HYSTERESIS Comparator input - Hysteresis
  * @{
  */
#define LL_COMP_HYSTERESIS_NONE         ((uint32_t)0x00000000)              /*!< No hysteresis */
#define LL_COMP_HYSTERESIS_LOW          (                  COMP_CSR_HYST_0) /*!< Hysteresis level low */
#define LL_COMP_HYSTERESIS_MEDIUM       (COMP_CSR_HYST_1                  ) /*!< Hysteresis level medium */
#define LL_COMP_HYSTERESIS_HIGH         (COMP_CSR_HYST_1 | COMP_CSR_HYST_0) /*!< Hysteresis level high */
/**
  * @}
  */

/** @defgroup COMP_LL_EC_OUTPUT_POLARITY Comparator output - Output polarity
  * @{
  */
#define LL_COMP_OUTPUTPOL_NONINVERTED   ((uint32_t)0x00000000) /*!< COMP output polarity is not inverted: comparator output is high when the plus (non-inverting) input is at a higher voltage than the minus (inverting) input */
#define LL_COMP_OUTPUTPOL_INVERTED      (COMP_CSR_POLARITY)    /*!< COMP output polarity is inverted: comparator output is low when the plus (non-inverting) input is at a lower voltage than the minus (inverting) input */
/**
  * @}
  */

/** @defgroup COMP_LL_EC_OUTPUT_BLANKING_SOURCE Comparator output - Blanking source
  * @{
  */
#define LL_COMP_BLANKINGSRC_NONE        ((uint32_t)0x00000000) /*!< No blanking source */
#define LL_COMP_BLANKINGSRC_TIM1_OC5    (COMP_CSR_BLANKING_0)  /*!< Blanking source for COMP1: TIM1 OC5 selected as blanking source for comparator */
#define LL_COMP_BLANKINGSRC_TIM2_OC3    (COMP_CSR_BLANKING_1)  /*!< Blanking source for COMP1: TIM2 OC3 selected as blanking source for comparator */
#define LL_COMP_BLANKINGSRC_TIM3_OC3    (COMP_CSR_BLANKING_2)  /*!< Blanking source for COMP1: TIM3 OC3 selected as blanking source for comparator */
#define LL_COMP_BLANKINGSRC_TIM3_OC4    (COMP_CSR_BLANKING_0)  /*!< Blanking source for COMP2: TIM3 OC4 selected as blanking source for comparator */
#define LL_COMP_BLANKINGSRC_TIM8_OC5    (COMP_CSR_BLANKING_1)  /*!< Blanking source for COMP2: TIM8 OC5 selected as blanking source for comparator */
#define LL_COMP_BLANKINGSRC_TIM15_OC1   (COMP_CSR_BLANKING_2)  /*!< Blanking source for COMP2: TIM15 OC1 selected as blanking source for comparator */
/**
  * @}
  */

/** @defgroup COMP_LL_EC_OUTPUT_LEVEL Comparator output - Output level
  * @{
  */
#define LL_COMP_OUTPUT_LEVEL_LOW        (0x00000000)        /*!< Comparator output level low (if the polarity is not inverted, otherwise to be complemented) */
#define LL_COMP_OUTPUT_LEVEL_HIGH       (COMP_CSR_VALUE)    /*!< Comparator output level high (if the polarity is not inverted, otherwise to be complemented) */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup COMP_LL_Exported_Macros COMP Exported Macros
  * @{
  */
/** @defgroup COMP_LL_EM_WRITE_READ Common write and read registers macro
  * @{
  */

/**
  * @brief  Write a value in COMP register
  * @param  __INSTANCE__ comparator instance
  * @param  __REG__ Register to be written
  * @param  __VALUE__ Value to be written in the register
  * @retval None
  */
#define LL_COMP_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG(__INSTANCE__->__REG__, (__VALUE__))

/**
  * @brief  Read a value in COMP register
  * @param  __INSTANCE__ comparator instance
  * @param  __REG__ Register to be read
  * @retval Register value
  */
#define LL_COMP_ReadReg(__INSTANCE__, __REG__) READ_REG(__INSTANCE__->__REG__)
/**
  * @}
  */

/** @defgroup COMP_LL_EM_HELPER_MACRO COMP helper macro
  * @{
  */

/**
  * @brief  Helper macro to select the COMP common instance
  *         to which is belonging the selected COMP instance.
  * @note   COMP common register instance can be used to
  *         set parameters common to several COMP instances.
  *         Refer to functions having argument "COMPxy_COMMON" as parameter.
  * @param  __COMPx__ COMP instance
  * @retval COMP common instance
  */
#define __LL_COMP_COMMON_INSTANCE(__COMPx__)                                   \
  (COMP12_COMMON)
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup COMP_LL_Exported_Functions COMP Exported Functions
  * @{
  */

/** @defgroup COMP_LL_EF_Configuration_comparator_common Configuration of COMP hierarchical scope: common to several COMP instances
  * @{
  */

/**
  * @brief  Set window mode of 2 comparators instances.
  * @rmtoll CSR         WINMODE         LL_COMP_SetCommonWindowMode
  * @param  COMPxy_COMMON Comparator common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_COMP_COMMON_INSTANCE() )
  *         On this STM32 family, window mode can be set only
  *         from COMP instance: COMP2.
  * @param  WindowMode This parameter can be one of the following values:
  *         @arg @ref LL_COMP_WINDOWMODE_DISABLE
  *         @arg @ref LL_COMP_WINDOWMODE_ENABLE
  * @retval None
  */
__STATIC_INLINE void LL_COMP_SetCommonWindowMode(COMP_Common_TypeDef *COMPxy_COMMON, uint32_t WindowMode)
{
  /* Note: On this STM32 family, window mode can be set only                  */
  /*       from COMP instance: COMP2.                                         */
  MODIFY_REG(COMPxy_COMMON->CSR, COMP_CSR_WINMODE, WindowMode);
}

/**
  * @brief  Get window mode of 2 comparator instances.
  * @rmtoll CSR         WINMODE         LL_COMP_GetCommonWindowMode
  * @param  COMPxy_COMMON Comparator common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_COMP_COMMON_INSTANCE() )
  *         On this STM32 family, window mode can be retrieved only
  *         from COMP instance: COMP2.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_COMP_WINDOWMODE_DISABLE
  *         @arg @ref LL_COMP_WINDOWMODE_ENABLE
  */
__STATIC_INLINE uint32_t LL_COMP_GetCommonWindowMode(COMP_Common_TypeDef *COMPxy_COMMON)
{
  return (uint32_t)(READ_BIT(COMPxy_COMMON->CSR, COMP_CSR_WINMODE));
}

/**
  * @}
  */

/** @defgroup COMP_LL_EF_Configuration_comparator_modes Configuration of comparator modes
  * @{
  */

/**
  * @brief  Set comparator instance operating mode to adjust power and speed.
  * @rmtoll CSR         PWRMODE         LL_COMP_SetPowerMode
  * @param  COMPx Comparator instance
  * @param  PowerMode This parameter can be one of the following values:
  *         @arg @ref LL_COMP_POWERMODE_HIGHSPEED
  *         @arg @ref LL_COMP_POWERMODE_MEDIUMSPEED
  *         @arg @ref LL_COMP_POWERMODE_ULTRALOWPOWER
  * @retval None
  */
__STATIC_INLINE void LL_COMP_SetPowerMode(COMP_TypeDef *COMPx, uint32_t PowerMode)
{
  MODIFY_REG(COMPx->CSR, COMP_CSR_PWRMODE, PowerMode);
}

/**
  * @brief  Get comparator instance operating mode to adjust power and speed.
  * @rmtoll CSR         PWRMODE         LL_COMP_GetPowerMode
  * @param  COMPx Comparator instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_COMP_POWERMODE_HIGHSPEED
  *         @arg @ref LL_COMP_POWERMODE_MEDIUMSPEED
  *         @arg @ref LL_COMP_POWERMODE_ULTRALOWPOWER
  */
__STATIC_INLINE uint32_t LL_COMP_GetPowerMode(COMP_TypeDef *COMPx)
{
  return (uint32_t)(READ_BIT(COMPx->CSR, COMP_CSR_PWRMODE));
}

/**
  * @}
  */

/** @defgroup COMP_LL_EF_Configuration_comparator_inputs Configuration of comparator inputs
  * @{
  */

/**
  * @brief  Set comparator inputs minus (inverting) and plus (non-inverting).
  * @note   In case of comparator input selected to be connected to IO:
  *         IP pins are specific to each comparator instance.
  *         Refer to description of parameters or to reference manual.
  * @note   On this STM32 family, scaler bridge is configurable:
  *         to optimize power consumption, this function enables the
  *         voltage scaler bridge only when required (when selecting
  *         comparator input based on VrefInt).
  *         - For scaler bridge power consumption values, 
  *           refer to device datasheet, parameter "IDDA(SCALER)".
  *         - Scaler bridge requires a delay for voltage stabilization.
  *           Refer to device datasheet, parameter "tSTART_SCALER".
  *         - Scaler bridge is common for all comparator instances,
  *           therefore if at least one of the comparator instance
  *           is requiring the scaler bridge, it remains enabled.
  * @rmtoll CSR         INMSEL          LL_COMP_ConfigInputs\n
  *         CSR         INPSEL          LL_COMP_ConfigInputs\n
  *         CSR         BRGEN           LL_COMP_ConfigInputs\n
  *         CSR         SCALEN          LL_COMP_ConfigInputs
  * @param  COMPx Comparator instance
  * @param  InputInverting This parameter can be one of the following values:
  *         @arg @ref LL_COMP_INPUT_MINUS_1_4VREFINT
  *         @arg @ref LL_COMP_INPUT_MINUS_1_2VREFINT
  *         @arg @ref LL_COMP_INPUT_MINUS_3_4VREFINT
  *         @arg @ref LL_COMP_INPUT_MINUS_VREFINT
  *         @arg @ref LL_COMP_INPUT_MINUS_DAC1_CH1
  *         @arg @ref LL_COMP_INPUT_MINUS_DAC1_CH2
  *         @arg @ref LL_COMP_INPUT_MINUS_IO1
  *         @arg @ref LL_COMP_INPUT_MINUS_IO2
  * @param  InputNonInverting This parameter can be one of the following values:
  *         @arg @ref LL_COMP_INPUT_PLUS_IO1
  *         @arg @ref LL_COMP_INPUT_PLUS_IO2
  * @retval None
  */
__STATIC_INLINE void LL_COMP_ConfigInputs(COMP_TypeDef *COMPx, uint32_t InputInverting, uint32_t InputNonInverting)
{
  MODIFY_REG(COMPx->CSR,
             COMP_CSR_INMSEL | COMP_CSR_INPSEL | COMP_CSR_SCALEN | COMP_CSR_BRGEN,
             InputInverting | InputNonInverting);
}

/**
  * @brief  Set comparator input minus (inverting).
  * @note   In case of comparator input selected to be connected to IO:
  *         IP pins are specific to each comparator instance.
  *         Refer to description of parameters or to reference manual.
  * @note   On this STM32 family, scaler bridge is configurable:
  *         to optimize power consumption, this function enables the
  *         voltage scaler bridge only when required (when selecting
  *         comparator input based on VrefInt).
  *         - For scaler bridge power consumption values, 
  *           refer to device datasheet, parameter "IDDA(SCALER)".
  *         - Scaler bridge requires a delay for voltage stabilization.
  *           Refer to device datasheet, parameter "tSTART_SCALER".
  *         - Scaler bridge is common for all comparator instances,
  *           therefore if at least one of the comparator instance
  *           is requiring the scaler bridge, it remains enabled.
  * @rmtoll CSR         INPSEL         LL_COMP_SetInputMinus\n
  *         CSR         BRGEN          LL_COMP_SetInputMinus\n
  *         CSR         SCALEN         LL_COMP_SetInputMinus
  * @param  COMPx Comparator instance
  * @param  InputInverting This parameter can be one of the following values:
  *         @arg @ref LL_COMP_INPUT_MINUS_1_4VREFINT
  *         @arg @ref LL_COMP_INPUT_MINUS_1_2VREFINT
  *         @arg @ref LL_COMP_INPUT_MINUS_3_4VREFINT
  *         @arg @ref LL_COMP_INPUT_MINUS_VREFINT
  *         @arg @ref LL_COMP_INPUT_MINUS_DAC1_CH1
  *         @arg @ref LL_COMP_INPUT_MINUS_DAC1_CH2
  *         @arg @ref LL_COMP_INPUT_MINUS_IO1
  *         @arg @ref LL_COMP_INPUT_MINUS_IO2
  * @retval None
  */
__STATIC_INLINE void LL_COMP_SetInputMinus(COMP_TypeDef *COMPx, uint32_t InputInverting)
{
  MODIFY_REG(COMPx->CSR, COMP_CSR_INMSEL | COMP_CSR_SCALEN | COMP_CSR_BRGEN, InputInverting);
}

/**
  * @brief  Get comparator input minus (inverting).
  * @note   In case of comparator input selected to be connected to IO:
  *         IP pins are specific to each comparator instance.
  *         Refer to description of parameters or to reference manual.
  * @rmtoll CSR         INPSEL         LL_COMP_GetInputMinus\n
  *         CSR         BRGEN          LL_COMP_GetInputMinus\n
  *         CSR         SCALEN         LL_COMP_GetInputMinus
  * @param  COMPx Comparator instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_COMP_INPUT_MINUS_1_4VREFINT
  *         @arg @ref LL_COMP_INPUT_MINUS_1_2VREFINT
  *         @arg @ref LL_COMP_INPUT_MINUS_3_4VREFINT
  *         @arg @ref LL_COMP_INPUT_MINUS_VREFINT
  *         @arg @ref LL_COMP_INPUT_MINUS_DAC1_CH1
  *         @arg @ref LL_COMP_INPUT_MINUS_DAC1_CH2
  *         @arg @ref LL_COMP_INPUT_MINUS_IO1
  *         @arg @ref LL_COMP_INPUT_MINUS_IO2
  */
__STATIC_INLINE uint32_t LL_COMP_GetInputMinus(COMP_TypeDef *COMPx)
{
  return (uint32_t)(READ_BIT(COMPx->CSR, COMP_CSR_INMSEL | COMP_CSR_SCALEN | COMP_CSR_BRGEN));
}

/**
  * @brief  Set comparator input plus (non-inverting).
  * @note   In case of comparator input selected to be connected to IO:
  *         IP pins are specific to each comparator instance.
  *         Refer to description of parameters or to reference manual.
  * @rmtoll CSR         INMSEL          LL_COMP_SetInputPlus
  * @param  COMPx Comparator instance
  * @param  InputNonInverting This parameter can be one of the following values:
  *         @arg @ref LL_COMP_INPUT_PLUS_IO1
  *         @arg @ref LL_COMP_INPUT_PLUS_IO2
  * @retval None
  */
__STATIC_INLINE void LL_COMP_SetInputPlus(COMP_TypeDef *COMPx, uint32_t InputNonInverting)
{
  MODIFY_REG(COMPx->CSR, COMP_CSR_INPSEL, InputNonInverting);
}

/**
  * @brief  Get comparator input plus (non-inverting).
  * @note   In case of comparator input selected to be connected to IO:
  *         IP pins are specific to each comparator instance.
  *         Refer to description of parameters or to reference manual.
  * @rmtoll CSR         INMSEL          LL_COMP_GetInputPlus
  * @param  COMPx Comparator instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_COMP_INPUT_PLUS_IO1
  *         @arg @ref LL_COMP_INPUT_PLUS_IO2
  */
__STATIC_INLINE uint32_t LL_COMP_GetInputPlus(COMP_TypeDef *COMPx)
{
  return (uint32_t)(READ_BIT(COMPx->CSR, COMP_CSR_INPSEL));
}

/**
  * @brief  Set comparator instance hysteresis mode of the input minus (inverting input).
  * @rmtoll CSR     HYST            LL_COMP_SetInputHysteresis
  * @param  COMPx Comparator instance
  * @param  InputHysteresis This parameter can be one of the following values:
  *         @arg @ref LL_COMP_HYSTERESIS_NONE
  *         @arg @ref LL_COMP_HYSTERESIS_LOW
  *         @arg @ref LL_COMP_HYSTERESIS_MEDIUM
  *         @arg @ref LL_COMP_HYSTERESIS_HIGH
  * @retval None
  */
__STATIC_INLINE void LL_COMP_SetInputHysteresis(COMP_TypeDef *COMPx, uint32_t InputHysteresis)
{
  MODIFY_REG(COMPx->CSR, COMP_CSR_HYST, InputHysteresis);
}

/**
  * @brief  Get comparator instance hysteresis mode of the minus (inverting) input.
  * @rmtoll CSR     HYST            LL_COMP_GetInputHysteresis
  * @param  COMPx Comparator instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_COMP_HYSTERESIS_NONE
  *         @arg @ref LL_COMP_HYSTERESIS_LOW
  *         @arg @ref LL_COMP_HYSTERESIS_MEDIUM
  *         @arg @ref LL_COMP_HYSTERESIS_HIGH
  */
__STATIC_INLINE uint32_t LL_COMP_GetInputHysteresis(COMP_TypeDef *COMPx)
{
  return (uint32_t)(READ_BIT(COMPx->CSR, COMP_CSR_HYST));
}

/**
  * @}
  */

/** @defgroup COMP_LL_EF_Configuration_comparator_output Configuration of comparator output
  * @{
  */

/**
  * @brief  Set comparator instance output polarity.
  * @rmtoll CSR         POLARITY        LL_COMP_SetOutputPolarity
  * @param  COMPx Comparator instance
  * @param  OutputPolarity This parameter can be one of the following values:
  *         @arg @ref LL_COMP_OUTPUTPOL_NONINVERTED
  *         @arg @ref LL_COMP_OUTPUTPOL_INVERTED
  * @retval None
  */
__STATIC_INLINE void LL_COMP_SetOutputPolarity(COMP_TypeDef *COMPx, uint32_t OutputPolarity)
{
  MODIFY_REG(COMPx->CSR, COMP_CSR_POLARITY, OutputPolarity);
}

/**
  * @brief  Get comparator instance output polarity.
  * @rmtoll CSR         POLARITY        LL_COMP_GetOutputPolarity
  * @param  COMPx Comparator instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_COMP_OUTPUTPOL_NONINVERTED
  *         @arg @ref LL_COMP_OUTPUTPOL_INVERTED
  */
__STATIC_INLINE uint32_t LL_COMP_GetOutputPolarity(COMP_TypeDef *COMPx)
{
  return (uint32_t)(READ_BIT(COMPx->CSR, COMP_CSR_POLARITY));
}

/**
  * @brief  Set comparator instance blanking source.
  * @note   Blanking source may be specific to each comparator instance.
  *         Refer to description of parameters or to reference manual.
  * @note   Availability of parameters of blanking source from timer
  *         depends on timers availability on the selected device.
  * @rmtoll CSR     BLANKING        LL_COMP_SetOutputBlankingSource
  * @param  COMPx Comparator instance
  * @param  BlankingSource This parameter can be one of the following values:
  *         @arg @ref LL_COMP_BLANKINGSRC_NONE
  *         @arg @ref LL_COMP_BLANKINGSRC_TIM1_OC5  (1)(2)
  *         @arg @ref LL_COMP_BLANKINGSRC_TIM2_OC3  (1)(2)
  *         @arg @ref LL_COMP_BLANKINGSRC_TIM3_OC3  (1)(2)
  *         @arg @ref LL_COMP_BLANKINGSRC_TIM3_OC4  (1)(3)
  *         @arg @ref LL_COMP_BLANKINGSRC_TIM8_OC5  (1)(3)
  *         @arg @ref LL_COMP_BLANKINGSRC_TIM15_OC1 (1)(3)
  *         
  *         (1) Parameter availability depending on timer availability
  *             on the selected device.
  *         (2) On STM32L4, parameter available only on comparator instance: COMP1.
  *         (3) On STM32L4, parameter available only on comparator instance: COMP2.
  * @retval None
  */
__STATIC_INLINE void LL_COMP_SetOutputBlankingSource(COMP_TypeDef *COMPx, uint32_t BlankingSource)
{
  MODIFY_REG(COMPx->CSR, COMP_CSR_BLANKING, BlankingSource);
}

/**
  * @brief  Get comparator instance blanking source.
  * @note   Availability of parameters of blanking source from timer
  *         depends on timers availability on the selected device.
  * @note   Blanking source may be specific to each comparator instance.
  *         Refer to description of parameters or to reference manual.
  * @rmtoll CSR     BLANKING        LL_COMP_GetOutputBlankingSource
  * @param  COMPx Comparator instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_COMP_BLANKINGSRC_NONE
  *         @arg @ref LL_COMP_BLANKINGSRC_TIM1_OC5  (1)(2)
  *         @arg @ref LL_COMP_BLANKINGSRC_TIM2_OC3  (1)(2)
  *         @arg @ref LL_COMP_BLANKINGSRC_TIM3_OC3  (1)(2)
  *         @arg @ref LL_COMP_BLANKINGSRC_TIM3_OC4  (1)(3)
  *         @arg @ref LL_COMP_BLANKINGSRC_TIM8_OC5  (1)(3)
  *         @arg @ref LL_COMP_BLANKINGSRC_TIM15_OC1 (1)(3)
  *         
  *         (1) Parameter availability depending on timer availability
  *             on the selected device.
  *         (2) On STM32L4, parameter available only on comparator instance: COMP1.
  *         (3) On STM32L4, parameter available only on comparator instance: COMP2.
  */
__STATIC_INLINE uint32_t LL_COMP_GetOutputBlankingSource(COMP_TypeDef *COMPx)
{
  return (uint32_t)(READ_BIT(COMPx->CSR, COMP_CSR_BLANKING));
}

/**
  * @}
  */

/** @defgroup COMP_LL_EF_Configuration_Legacy_Functions Configuration of COMP, legacy functions name
  * @{
  */
/* Old functions name kept for legacy purpose, to be replaced by the          */
/* current functions name.                                                    */
__STATIC_INLINE void LL_COMP_SetInputNonInverting(COMP_TypeDef *COMPx, uint32_t InputNonInverting)
{
  LL_COMP_SetInputPlus(COMPx, InputNonInverting);
}
__STATIC_INLINE uint32_t LL_COMP_GetInputNonInverting(COMP_TypeDef *COMPx)
{
  return LL_COMP_GetInputPlus(COMPx);
}

__STATIC_INLINE void LL_COMP_SetInputInverting(COMP_TypeDef *COMPx, uint32_t InputInverting)
{
  LL_COMP_SetInputMinus(COMPx, InputInverting);
}
__STATIC_INLINE uint32_t LL_COMP_GetInputInverting(COMP_TypeDef *COMPx)
{
  return LL_COMP_GetInputMinus(COMPx);
}

/**
  * @}
  */

/** @defgroup COMP_LL_EF_Operation Operation on comparator instance
  * @{
  */

/**
  * @brief  Enable comparator instance.
  * @note   After enable from off state, comparator requires a delay
  *         to reach reach propagation delay specification.
  *         Refer to device datasheet, parameter "tSTART".
  * @rmtoll CSR         EN              LL_COMP_Enable
  * @param  COMPx Comparator instance
  * @retval None
  */
__STATIC_INLINE void LL_COMP_Enable(COMP_TypeDef *COMPx)
{
  SET_BIT(COMPx->CSR, COMP_CSR_EN);
}

/**
  * @brief  Disable comparator instance.
  * @rmtoll CSR         EN              LL_COMP_Disable
  * @param  COMPx Comparator instance
  * @retval None
  */
__STATIC_INLINE void LL_COMP_Disable(COMP_TypeDef *COMPx)
{
  CLEAR_BIT(COMPx->CSR, COMP_CSR_EN);
}

/**
  * @brief  Get comparator enable state
  *         (0: COMP is disabled, 1: COMP is enabled)
  * @rmtoll CSR         EN              LL_COMP_IsEnabled
  * @param  COMPx Comparator instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_COMP_IsEnabled(COMP_TypeDef *COMPx)
{
  return (READ_BIT(COMPx->CSR, COMP_CSR_EN) == (COMP_CSR_EN));
}

/**
  * @brief  Lock comparator instance.
  * @note   Once locked, comparator configuration can be accessed in read-only.
  * @note   The only way to unlock the comparator is a device hardware reset.
  * @rmtoll CSR         LOCK            LL_COMP_Lock
  * @param  COMPx Comparator instance
  * @retval None
  */
__STATIC_INLINE void LL_COMP_Lock(COMP_TypeDef *COMPx)
{
  SET_BIT(COMPx->CSR, COMP_CSR_LOCK);
}

/**
  * @brief  Get comparator lock state (0: COMP is unlocked, 1: COMP is locked).
  * @note   Once locked, comparator configuration can be accessed in read-only.
  * @note   The only way to unlock the comparator is a device hardware reset.
  * @rmtoll CSR         LOCK            LL_COMP_IsLocked
  * @param  COMPx Comparator instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_COMP_IsLocked(COMP_TypeDef *COMPx)
{
  return (READ_BIT(COMPx->CSR, COMP_CSR_LOCK) == (COMP_CSR_LOCK));
}

/**
  * @brief  Read comparator instance output level.
  * @note   The output level depends on the selected polarity.
  *         If the polarity is not inverted:
  *          - Comparator output is low when the plus (non-inverting) input is at a lower
  *            voltage than the minus (inverting) input
  *          - Comparator output is high when the plus (non-inverting) input is at a higher
  *            voltage than the minus (inverting) input
  *         If the polarity is inverted:
  *          - Comparator output is high when the plus (non-inverting) input is at a lower
  *            voltage than the minus (inverting) input
  *          - Comparator output is low when the plus (non-inverting) input is at a higher
  *            voltage than the minus (inverting) input
  * @rmtoll CSR         VALUE           LL_COMP_ReadOutputLevel
  * @param  COMPx Comparator instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_COMP_OUTPUT_LEVEL_LOW
  *         @arg @ref LL_COMP_OUTPUT_LEVEL_HIGH
  */
__STATIC_INLINE uint32_t LL_COMP_ReadOutputLevel(COMP_TypeDef *COMPx)
{
  return (uint32_t)(READ_BIT(COMPx->CSR, COMP_CSR_VALUE));
}

/**
  * @}
  */

#if defined(USE_FULL_LL_DRIVER)
/** @defgroup COMP_LL_EF_Init Initialization and de-initialization functions
  * @{
  */

ErrorStatus LL_COMP_DeInit(COMP_TypeDef *COMPx);
ErrorStatus LL_COMP_Init(COMP_TypeDef *COMPx, LL_COMP_InitTypeDef *COMP_InitStruct);
void        LL_COMP_StructInit(LL_COMP_InitTypeDef *COMP_InitStruct);

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

#endif /* COMP1 || COMP2 */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32L4xx_LL_COMP_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
