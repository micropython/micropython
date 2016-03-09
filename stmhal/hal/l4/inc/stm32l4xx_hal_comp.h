/**
  ******************************************************************************
  * @file    stm32l4xx_hal_comp.h
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   Header file of COMP HAL module.
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
#ifndef __STM32L4xx_HAL_COMP_H
#define __STM32L4xx_HAL_COMP_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal_def.h"

/** @addtogroup STM32L4xx_HAL_Driver
  * @{
  */

/** @addtogroup COMP
  * @{
  */

/* Exported types ------------------------------------------------------------*/ 
/** @defgroup COMP_Exported_Types COMP Exported Types
  * @{
  */

/** 
  * @brief  COMP Init structure definition  
  */
typedef struct
{

  uint32_t InvertingInput;     /*!< Selects the inverting input of the comparator.
                                    This parameter can be a value of @ref COMP_InvertingInput */

  uint32_t NonInvertingInput;  /*!< Selects the non inverting input of the comparator.
                                    This parameter can be a value of @ref COMP_NonInvertingInput */

  uint32_t OutputPol;          /*!< Selects the output polarity of the comparator.
                                    This parameter can be a value of @ref COMP_OutputPolarity */

  uint32_t Hysteresis;         /*!< Selects the hysteresis voltage of the comparator.
                                    This parameter can be a value of @ref COMP_Hysteresis */

  uint32_t BlankingSrce;       /*!< Selects the output blanking source of the comparator.
                                    This parameter can be a value of @ref COMP_BlankingSrce */

  uint32_t Mode;               /*!< Selects the operating consumption mode of the comparator
                                    to adjust the speed/consumption.
                                    This parameter can be a value of @ref COMP_Mode */

  uint32_t WindowMode;         /*!< Selects the window mode of the comparator 2.
                                    This parameter can be a value of @ref COMP_WindowMode */

  uint32_t TriggerMode;        /*!< Selects the trigger mode of the comparator (interrupt mode).
                                    This parameter can be a value of @ref COMP_TriggerMode */

}COMP_InitTypeDef;

/** 
  * @brief  HAL State structures definition
  */
typedef enum
{
  HAL_COMP_STATE_RESET             = 0x00,    /*!< COMP not yet initialized or disabled             */
  HAL_COMP_STATE_RESET_LOCKED      = 0x10,    /*!< COMP configuration is locked in its reset state  */
  HAL_COMP_STATE_READY             = 0x01,    /*!< COMP initialized and ready for use               */
  HAL_COMP_STATE_READY_LOCKED      = 0x11,    /*!< COMP initialized but the configuration is locked */
  HAL_COMP_STATE_BUSY              = 0x02,    /*!< COMP is running                                  */
  HAL_COMP_STATE_BUSY_LOCKED       = 0x12     /*!< COMP is running and the configuration is locked  */
}HAL_COMP_StateTypeDef;

/** 
  * @brief  COMP Handle Structure definition
  */
typedef struct
{
  COMP_TypeDef       *Instance;       /*!< Register base address    */
  COMP_InitTypeDef   Init;            /*!< COMP required parameters */
  HAL_LockTypeDef    Lock;            /*!< Locking object           */
  __IO HAL_COMP_StateTypeDef  State;  /*!< COMP communication state */
} COMP_HandleTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup COMP_Exported_Constants COMP Exported Constants
  * @{
  */

/** @defgroup COMP_InvertingInput COMP Inverting Input
  * @{
  */
#define COMP_INVERTINGINPUT_1_4VREFINT     ((uint32_t)0x00000000)                  /*!< 1/4 VREFINT connected to comparator inverting input (minus) */
#define COMP_INVERTINGINPUT_1_2VREFINT     COMP_CSR_INMSEL_0                       /*!< 1/2 VREFINT connected to comparator inverting input (minus) */
#define COMP_INVERTINGINPUT_3_4VREFINT     COMP_CSR_INMSEL_1                       /*!< 3/4 VREFINT connected to comparator inverting input (minus) */
#define COMP_INVERTINGINPUT_VREFINT        (COMP_CSR_INMSEL_1 | COMP_CSR_INMSEL_0) /*!< VREFINT connected to comparator1 inverting input (minus) */
#define COMP_INVERTINGINPUT_DAC1           COMP_CSR_INMSEL_2                       /*!< DAC_OUT1 connected to comparator inverting input (minus) */
#define COMP_INVERTINGINPUT_DAC2           (COMP_CSR_INMSEL_2 | COMP_CSR_INMSEL_0) /*!< DAC_OUT2 connected to comparator inverting input (minus) */
#define COMP_INVERTINGINPUT_IO1            (COMP_CSR_INMSEL_2 | COMP_CSR_INMSEL_1) /*!< IO1 connected to comparator inverting input (minus) */
#define COMP_INVERTINGINPUT_IO2            COMP_CSR_INMSEL                         /*!< IO2 connected to comparator inverting input (minus) */
/**
  * @}
  */

/** @defgroup COMP_NonInvertingInput COMP NonInverting Input
  * @{
  */
#define COMP_NONINVERTINGINPUT_IO1         ((uint32_t)0x00000000) /*!< IO1 connected to comparator non inverting input (plus) */
#define COMP_NONINVERTINGINPUT_IO2         COMP_CSR_INPSEL_0      /*!< IO2 connected to comparator non inverting input (plus) */
/**
  * @}
  */

/** @defgroup COMP_OutputPolarity COMP Output Polarity
  * @{
  */
#define COMP_OUTPUTPOL_NONINVERTED         ((uint32_t)0x00000000)  /*!< COMP output on GPIO isn't inverted */
#define COMP_OUTPUTPOL_INVERTED            COMP_CSR_POLARITY       /*!< COMP output on GPIO is inverted */
/**
  * @}
  */

/** @defgroup COMP_Mode COMP Mode
  * @{
  */
/* Please refer to the electrical characteristics in the device datasheet for
   the power consumption values */
#define COMP_MODE_HIGHSPEED                ((uint32_t)0x00000000) /*!< High Speed */
#define COMP_MODE_MEDIUMSPEED              COMP_CSR_PWRMODE_0     /*!< Medium Speed */
#define COMP_MODE_ULTRALOWPOWER            COMP_CSR_PWRMODE       /*!< Ultra-low power mode */
/**
  * @}
  */

/** @defgroup COMP_Hysteresis COMP Hysteresis
  * @{
  */
#define COMP_HYSTERESIS_NONE               ((uint32_t)0x00000000)  /*!< No hysteresis */
#define COMP_HYSTERESIS_LOW                COMP_CSR_HYST_0         /*!< Hysteresis level low */
#define COMP_HYSTERESIS_MEDIUM             COMP_CSR_HYST_1         /*!< Hysteresis level medium */
#define COMP_HYSTERESIS_HIGH               COMP_CSR_HYST           /*!< Hysteresis level high */
/**
  * @}
  */

/** @defgroup COMP_BlankingSrce  COMP Blanking Source
  * @{
  */
/* No blanking source can be selected for all comparators */
#define COMP_BLANKINGSRCE_NONE             ((uint32_t)0x00000000)    /*!< No blanking source */
/* Blanking source for COMP1 */
#define COMP_BLANKINGSRCE_TIM1OC5          COMP_CSR_BLANKING_0  /*!< TIM1 OC5 selected as blanking source for comparator */
#define COMP_BLANKINGSRCE_TIM2OC3          COMP_CSR_BLANKING_1  /*!< TIM2 OC3 selected as blanking source for comparator */
#define COMP_BLANKINGSRCE_TIM3OC3          COMP_CSR_BLANKING_2  /*!< TIM3 OC3 selected as blanking source for comparator */
/* Blanking source for COMP2 */
#define COMP_BLANKINGSRCE_TIM3OC4          COMP_CSR_BLANKING_0  /*!< TIM3 OC4 selected as blanking source for comparator */
#define COMP_BLANKINGSRCE_TIM8OC5          COMP_CSR_BLANKING_1  /*!< TIM8 OC5 selected as blanking source for comparator */
#define COMP_BLANKINGSRCE_TIM15OC1         COMP_CSR_BLANKING_2  /*!< TIM15 OC1 selected as blanking source for comparator */
/**
  * @}
  */

/** @defgroup COMP_WindowMode COMP Window Mode
  * @{
  */
#define COMP_WINDOWMODE_DISABLE            ((uint32_t)0x00000000) /*!< Window mode disable: Input plus of Comparator 2 not connected to Comparator 1 */
#define COMP_WINDOWMODE_ENABLE             COMP_CSR_WINMODE       /*!< Window mode enable: Input plus of Comparator 2 is connected to input plus of Comparator 1 */
/**
  * @}
  */

/** @defgroup COMP_OutputLevel COMP Output Level
  * @{
  */ 
/* When output polarity is not inverted, comparator output is low when
   the non-inverting input is at a lower voltage than the inverting input*/
#define COMP_OUTPUTLEVEL_LOW               ((uint32_t)0x00000000)
/* When output polarity is not inverted, comparator output is high when
   the non-inverting input is at a higher voltage than the inverting input */
#define COMP_OUTPUTLEVEL_HIGH              COMP_CSR_VALUE
/**
  * @}
  */

/** @defgroup COMP_TriggerMode COMP Trigger Mode
  * @{
  */
#define COMP_TRIGGERMODE_NONE                 ((uint32_t)0x00000000)   /*!< No External Interrupt trigger detection */
#define COMP_TRIGGERMODE_IT_RISING            ((uint32_t)0x00000001)   /*!< External Interrupt Mode with Rising edge trigger detection */
#define COMP_TRIGGERMODE_IT_FALLING           ((uint32_t)0x00000002)   /*!< External Interrupt Mode with Falling edge trigger detection */
#define COMP_TRIGGERMODE_IT_RISING_FALLING    ((uint32_t)0x00000003)   /*!< External Interrupt Mode with Rising/Falling edge trigger detection */
#define COMP_TRIGGERMODE_EVENT_RISING         ((uint32_t)0x00000010)   /*!< Event Mode with Rising edge trigger detection */
#define COMP_TRIGGERMODE_EVENT_FALLING        ((uint32_t)0x00000020)   /*!< Event Mode with Falling edge trigger detection */
#define COMP_TRIGGERMODE_EVENT_RISING_FALLING ((uint32_t)0x00000030)   /*!< Event Mode with Rising/Falling edge trigger detection */
/**
  * @}
  */

/** @defgroup COMP_Flag COMP Flag
  * @{
  */
#define COMP_FLAG_LOCK                 COMP_CSR_LOCK           /*!< Lock flag */

/**
  * @}
  */

/**
  * @}
  */
 
/* Exported macros -----------------------------------------------------------*/
/** @defgroup COMP_Exported_Macros COMP Exported Macros
  * @{
  */

/** @brief  Reset COMP handle state.
  * @param  __HANDLE__  COMP handle.
  * @retval None
  */
#define __HAL_COMP_RESET_HANDLE_STATE(__HANDLE__)  ((__HANDLE__)->State = HAL_COMP_STATE_RESET)

/**
  * @brief  Enable the specified comparator.
  * @param  __HANDLE__  COMP handle.
  * @retval None
  */
#define __HAL_COMP_ENABLE(__HANDLE__)                 SET_BIT((__HANDLE__)->Instance->CSR, COMP_CSR_EN)

/**
  * @brief  Disable the specified comparator.
  * @param  __HANDLE__  COMP handle.
  * @retval None
  */
#define __HAL_COMP_DISABLE(__HANDLE__)                CLEAR_BIT((__HANDLE__)->Instance->CSR, COMP_CSR_EN)

/**
  * @brief  Lock the specified comparator configuration.
  * @param  __HANDLE__  COMP handle.
  * @retval None
  */
#define __HAL_COMP_LOCK(__HANDLE__)                   SET_BIT((__HANDLE__)->Instance->CSR, COMP_CSR_LOCK)

/**
  * @brief  Enable the COMP1 EXTI line rising edge trigger.
  * @retval None
  */                                         
#define __HAL_COMP_COMP1_EXTI_ENABLE_RISING_EDGE()    SET_BIT(EXTI->RTSR1, COMP_EXTI_LINE_COMP1)

/**
  * @brief  Disable the COMP1 EXTI line rising edge trigger.
  * @retval None
  */                                         
#define __HAL_COMP_COMP1_EXTI_DISABLE_RISING_EDGE()   CLEAR_BIT(EXTI->RTSR1, COMP_EXTI_LINE_COMP1)

/**
  * @brief  Enable the COMP1 EXTI line falling edge trigger.
  * @retval None
  */                                         
#define __HAL_COMP_COMP1_EXTI_ENABLE_FALLING_EDGE()   SET_BIT(EXTI->FTSR1, COMP_EXTI_LINE_COMP1)

/**
  * @brief  Disable the COMP1 EXTI line falling edge trigger.
  * @retval None
  */                                         
#define __HAL_COMP_COMP1_EXTI_DISABLE_FALLING_EDGE()  CLEAR_BIT(EXTI->FTSR1, COMP_EXTI_LINE_COMP1)

/**
  * @brief  Enable the COMP1 EXTI line rising & falling edge trigger.
  * @retval None
  */                                         
#define __HAL_COMP_COMP1_EXTI_ENABLE_RISING_FALLING_EDGE()   do { \
                                                               __HAL_COMP_COMP1_EXTI_ENABLE_RISING_EDGE(); \
                                                               __HAL_COMP_COMP1_EXTI_ENABLE_FALLING_EDGE(); \
                                                             } while(0)

/**
  * @brief  Disable the COMP1 EXTI line rising & falling edge trigger.
  * @retval None
  */                                         
#define __HAL_COMP_COMP1_EXTI_DISABLE_RISING_FALLING_EDGE()  do { \
                                                               __HAL_COMP_COMP1_EXTI_DISABLE_RISING_EDGE(); \
                                                               __HAL_COMP_COMP1_EXTI_DISABLE_FALLING_EDGE(); \
                                                             } while(0)

/**
  * @brief  Enable the COMP1 EXTI line in interrupt mode.
  * @retval None
  */                                         
#define __HAL_COMP_COMP1_EXTI_ENABLE_IT()             SET_BIT(EXTI->IMR1, COMP_EXTI_LINE_COMP1)

/**
  * @brief  Disable the COMP1 EXTI line in interrupt mode.
  * @retval None
  */
#define __HAL_COMP_COMP1_EXTI_DISABLE_IT()            CLEAR_BIT(EXTI->IMR1, COMP_EXTI_LINE_COMP1)

/**
  * @brief  Generate a software interrupt on the COMP1 EXTI line.
  * @retval None
  */
#define __HAL_COMP_COMP1_EXTI_GENERATE_SWIT()         SET_BIT(EXTI->SWIER1, COMP_EXTI_LINE_COMP1)

/**
  * @brief  Enable the COMP1 EXTI Line in event mode.
  * @retval None
  */
#define __HAL_COMP_COMP1_EXTI_ENABLE_EVENT()           SET_BIT(EXTI->EMR1, COMP_EXTI_LINE_COMP1)

/**
  * @brief  Disable the COMP1 EXTI Line in event mode.
  * @retval None
  */
#define __HAL_COMP_COMP1_EXTI_DISABLE_EVENT()          CLEAR_BIT(EXTI->EMR1, COMP_EXTI_LINE_COMP1)

/**
  * @brief  Check whether the COMP1 EXTI line flag is set or not.
  * @retval RESET or SET
  */
#define __HAL_COMP_COMP1_EXTI_GET_FLAG()              READ_BIT(EXTI->PR1, COMP_EXTI_LINE_COMP1)
     
/**
  * @brief  Clear the COMP1 EXTI flag.
  * @retval None
  */
#define __HAL_COMP_COMP1_EXTI_CLEAR_FLAG()            WRITE_REG(EXTI->PR1, COMP_EXTI_LINE_COMP1)

/**
  * @brief  Enable the COMP2 EXTI line rising edge trigger.
  * @retval None
  */                                         
#define __HAL_COMP_COMP2_EXTI_ENABLE_RISING_EDGE()    SET_BIT(EXTI->RTSR1, COMP_EXTI_LINE_COMP2)

/**
  * @brief  Disable the COMP2 EXTI line rising edge trigger.
  * @retval None
  */                                         
#define __HAL_COMP_COMP2_EXTI_DISABLE_RISING_EDGE()   CLEAR_BIT(EXTI->RTSR1, COMP_EXTI_LINE_COMP2)

/**
  * @brief  Enable the COMP2 EXTI line falling edge trigger.
  * @retval None
  */                                         
#define __HAL_COMP_COMP2_EXTI_ENABLE_FALLING_EDGE()   SET_BIT(EXTI->FTSR1, COMP_EXTI_LINE_COMP2)

/**
  * @brief  Disable the COMP2 EXTI line falling edge trigger.
  * @retval None
  */                                         
#define __HAL_COMP_COMP2_EXTI_DISABLE_FALLING_EDGE()  CLEAR_BIT(EXTI->FTSR1, COMP_EXTI_LINE_COMP2)

/**
  * @brief  Enable the COMP2 EXTI line rising & falling edge trigger.
  * @retval None
  */                                         
#define __HAL_COMP_COMP2_EXTI_ENABLE_RISING_FALLING_EDGE()   do { \
                                                               __HAL_COMP_COMP2_EXTI_ENABLE_RISING_EDGE(); \
                                                               __HAL_COMP_COMP2_EXTI_ENABLE_FALLING_EDGE(); \
                                                             } while(0)

/**
  * @brief  Disable the COMP2 EXTI line rising & falling edge trigger.
  * @retval None
  */                                         
#define __HAL_COMP_COMP2_EXTI_DISABLE_RISING_FALLING_EDGE()   do { \
                                                               __HAL_COMP_COMP2_EXTI_DISABLE_RISING_EDGE(); \
                                                               __HAL_COMP_COMP2_EXTI_DISABLE_FALLING_EDGE(); \
                                                             } while(0)

/**
  * @brief  Enable the COMP2 EXTI line in interrupt mode.
  * @retval None
  */                                         
#define __HAL_COMP_COMP2_EXTI_ENABLE_IT()             SET_BIT(EXTI->IMR1, COMP_EXTI_LINE_COMP2)

/**
  * @brief  Disable the COMP2 EXTI line in interrupt mode.
  * @retval None
  */
#define __HAL_COMP_COMP2_EXTI_DISABLE_IT()            CLEAR_BIT(EXTI->IMR1, COMP_EXTI_LINE_COMP2)

/**
  * @brief  Generate a software interrupt on the COMP2 EXTI line.
  * @retval None
  */
#define __HAL_COMP_COMP2_EXTI_GENERATE_SWIT()         SET_BIT(EXTI->SWIER1, COMP_EXTI_LINE_COMP2)

/**
  * @brief  Enable the COMP2 EXTI Line in event mode.
  * @retval None
  */
#define __HAL_COMP_COMP2_EXTI_ENABLE_EVENT()           SET_BIT(EXTI->EMR1, COMP_EXTI_LINE_COMP2)

/**
  * @brief  Disable the COMP2 EXTI Line in event mode.
  * @retval None
  */
#define __HAL_COMP_COMP2_EXTI_DISABLE_EVENT()          CLEAR_BIT(EXTI->EMR1, COMP_EXTI_LINE_COMP2)

/**
  * @brief  Check whether the COMP2 EXTI line flag is set or not.
  * @retval RESET or SET
  */
#define __HAL_COMP_COMP2_EXTI_GET_FLAG()              READ_BIT(EXTI->PR1, COMP_EXTI_LINE_COMP2)
     
/**
  * @brief  Clear the COMP2 EXTI flag.
  * @retval None
  */
#define __HAL_COMP_COMP2_EXTI_CLEAR_FLAG()            WRITE_REG(EXTI->PR1, COMP_EXTI_LINE_COMP2)

/** @brief  Check whether the specified COMP flag is set or not.
  * @param  __HANDLE__  specifies the COMP Handle.
  * @param  __FLAG__  specifies the flag to check.
  *        This parameter can be one of the following values:
  *            @arg @ref COMP_FLAG_LOCK   lock flag
  * @retval The new state of __FLAG__ (TRUE or FALSE).
  */
#define __HAL_COMP_GET_FLAG(__HANDLE__, __FLAG__)     (((__HANDLE__)->Instance->CSR & (__FLAG__)) == (__FLAG__))   

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup COMP_Exported_Functions
  * @{
  */

/** @addtogroup COMP_Exported_Functions_Group1
  * @{
  */
/* Initialization and de-initialization functions  **********************************/
HAL_StatusTypeDef HAL_COMP_Init(COMP_HandleTypeDef *hcomp);
HAL_StatusTypeDef HAL_COMP_DeInit (COMP_HandleTypeDef *hcomp);
void              HAL_COMP_MspInit(COMP_HandleTypeDef *hcomp);
void              HAL_COMP_MspDeInit(COMP_HandleTypeDef *hcomp);
/**
  * @}
  */

/* IO operation functions  *****************************************************/
/** @addtogroup COMP_Exported_Functions_Group2
  * @{
  */
HAL_StatusTypeDef HAL_COMP_Start(COMP_HandleTypeDef *hcomp);
HAL_StatusTypeDef HAL_COMP_Stop(COMP_HandleTypeDef *hcomp);
HAL_StatusTypeDef HAL_COMP_Start_IT(COMP_HandleTypeDef *hcomp);
HAL_StatusTypeDef HAL_COMP_Stop_IT(COMP_HandleTypeDef *hcomp);
void              HAL_COMP_IRQHandler(COMP_HandleTypeDef *hcomp);
/* Callback in Interrupt mode */
void              HAL_COMP_TriggerCallback(COMP_HandleTypeDef *hcomp);
/**
  * @}
  */

/* Peripheral Control functions  ************************************************/
/** @addtogroup COMP_Exported_Functions_Group3
  * @{
  */
HAL_StatusTypeDef HAL_COMP_Lock(COMP_HandleTypeDef *hcomp);
uint32_t          HAL_COMP_GetOutputLevel(COMP_HandleTypeDef *hcomp);
/**
  * @}
  */

/* Peripheral State functions  **************************************************/
/** @addtogroup COMP_Exported_Functions_Group4
  * @{
  */
HAL_COMP_StateTypeDef HAL_COMP_GetState(COMP_HandleTypeDef *hcomp);
/**
  * @}
  */

/**
  * @}
  */

/* Private types -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @defgroup COMP_Private_Constants COMP Private Constants
  * @{
  */
/** @defgroup COMP_ExtiLine COMP EXTI Lines
  * @{
  */
#define COMP_EXTI_LINE_COMP1           ((uint32_t)0x00200000)  /*!< EXTI line 21 connected to COMP1 output */
#define COMP_EXTI_LINE_COMP2           ((uint32_t)0x00400000)  /*!< EXTI line 22 connected to COMP2 output */
/**
  * @}
  */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup COMP_Private_Macros COMP Private Macros
  * @{
  */
/** @defgroup COMP_GET_EXTI_LINE COMP Private macros to get EXTI line associated with Comparators 
  * @{
  */
/**
  * @brief  Get the specified EXTI line for a comparator instance.
  * @param  __INSTANCE__  specifies the COMP instance.
  * @retval value of @ref COMP_ExtiLine
  */
#define COMP_GET_EXTI_LINE(__INSTANCE__)             (((__INSTANCE__) == COMP1) ? COMP_EXTI_LINE_COMP1 : \
                                                      COMP_EXTI_LINE_COMP2)
/**
  * @}
  */

/** @defgroup COMP_IS_COMP_Definitions COMP Private macros to check input parameters
  * @{
  */

#define IS_COMP_INVERTINGINPUT(__INPUT__) (((__INPUT__) == COMP_INVERTINGINPUT_1_4VREFINT)  || \
                                           ((__INPUT__) == COMP_INVERTINGINPUT_1_2VREFINT)  || \
                                           ((__INPUT__) == COMP_INVERTINGINPUT_3_4VREFINT)  || \
                                           ((__INPUT__) == COMP_INVERTINGINPUT_VREFINT)     || \
                                           ((__INPUT__) == COMP_INVERTINGINPUT_DAC1)        || \
                                           ((__INPUT__) == COMP_INVERTINGINPUT_DAC2)        || \
                                           ((__INPUT__) == COMP_INVERTINGINPUT_IO1)         || \
                                           ((__INPUT__) == COMP_INVERTINGINPUT_IO2))

#define IS_COMP_NONINVERTINGINPUT(__INPUT__) (((__INPUT__) == COMP_NONINVERTINGINPUT_IO1) || \
                                              ((__INPUT__) == COMP_NONINVERTINGINPUT_IO2))

#define IS_COMP_OUTPUTPOL(__POL__)  (((__POL__) == COMP_OUTPUTPOL_NONINVERTED)  || \
                                     ((__POL__) == COMP_OUTPUTPOL_INVERTED))

#define IS_COMP_MODE(__MODE__)  (((__MODE__) == COMP_MODE_HIGHSPEED)     || \
                                 ((__MODE__) == COMP_MODE_MEDIUMSPEED)   || \
                                 ((__MODE__) == COMP_MODE_ULTRALOWPOWER))

#define IS_COMP_HYSTERESIS(__HYSTERESIS__) (((__HYSTERESIS__) == COMP_HYSTERESIS_NONE)   || \
                                            ((__HYSTERESIS__) == COMP_HYSTERESIS_LOW)    || \
                                            ((__HYSTERESIS__) == COMP_HYSTERESIS_MEDIUM) || \
                                            ((__HYSTERESIS__) == COMP_HYSTERESIS_HIGH))

#define IS_COMP_BLANKINGSRCE(__SOURCE__) (((__SOURCE__) == COMP_BLANKINGSRCE_NONE)     || \
                                          ((__SOURCE__) == COMP_BLANKINGSRCE_TIM1OC5)  || \
                                          ((__SOURCE__) == COMP_BLANKINGSRCE_TIM2OC3)  || \
                                          ((__SOURCE__) == COMP_BLANKINGSRCE_TIM3OC3)  || \
                                          ((__SOURCE__) == COMP_BLANKINGSRCE_TIM3OC4)  || \
                                          ((__SOURCE__) == COMP_BLANKINGSRCE_TIM8OC5)  || \
                                          ((__SOURCE__) == COMP_BLANKINGSRCE_TIM15OC1))

#define IS_COMP_BLANKINGSRCE_INSTANCE(__INSTANCE__, __BLANKINGSRCE__) \
   ((((__INSTANCE__) == COMP1)  &&                                    \
    (((__BLANKINGSRCE__) == COMP_BLANKINGSRCE_NONE)     ||            \
     ((__BLANKINGSRCE__) == COMP_BLANKINGSRCE_TIM1OC5)  ||            \
     ((__BLANKINGSRCE__) == COMP_BLANKINGSRCE_TIM2OC3)  ||            \
     ((__BLANKINGSRCE__) == COMP_BLANKINGSRCE_TIM3OC3)))              \
    ||                                                                \
    (((__INSTANCE__) == COMP2) &&                                     \
     (((__BLANKINGSRCE__) == COMP_BLANKINGSRCE_NONE)    ||            \
      ((__BLANKINGSRCE__) == COMP_BLANKINGSRCE_TIM3OC4) ||            \
      ((__BLANKINGSRCE__) == COMP_BLANKINGSRCE_TIM8OC5) ||            \
      ((__BLANKINGSRCE__) == COMP_BLANKINGSRCE_TIM15OC1))))

#define IS_COMP_WINDOWMODE(__WINDOWMODE__) (((__WINDOWMODE__) == COMP_WINDOWMODE_DISABLE) || \
                                            ((__WINDOWMODE__) == COMP_WINDOWMODE_ENABLE))

#define IS_COMP_TRIGGERMODE(__MODE__)  (((__MODE__) == COMP_TRIGGERMODE_NONE)                 || \
                                        ((__MODE__) == COMP_TRIGGERMODE_IT_RISING)            || \
                                        ((__MODE__) == COMP_TRIGGERMODE_IT_FALLING)           || \
                                        ((__MODE__) == COMP_TRIGGERMODE_IT_RISING_FALLING)    || \
                                        ((__MODE__) == COMP_TRIGGERMODE_EVENT_RISING)         || \
                                        ((__MODE__) == COMP_TRIGGERMODE_EVENT_FALLING)        || \
                                        ((__MODE__) == COMP_TRIGGERMODE_EVENT_RISING_FALLING))

/**
  * @}
  */

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32L4xx_HAL_COMP_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
