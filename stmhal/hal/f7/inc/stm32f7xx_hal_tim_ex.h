/**
  ******************************************************************************
  * @file    stm32f7xx_hal_tim_ex.h
  * @author  MCD Application Team
  * @version V1.1.2
  * @date    23-September-2016
  * @brief   Header file of TIM HAL Extension module.
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
#ifndef __STM32F7xx_HAL_TIM_EX_H
#define __STM32F7xx_HAL_TIM_EX_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal_def.h"

/** @addtogroup STM32F7xx_HAL_Driver
  * @{
  */

/** @addtogroup TIMEx
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup TIMEx_Exported_Types TIM Exported Types
  * @{
  */

/**
  * @brief  TIM Hall sensor Configuration Structure definition
  */

typedef struct
{

  uint32_t IC1Polarity;            /*!< Specifies the active edge of the input signal.
                                        This parameter can be a value of @ref TIM_Input_Capture_Polarity */

  uint32_t IC1Prescaler;        /*!< Specifies the Input Capture Prescaler.
                                     This parameter can be a value of @ref TIM_Input_Capture_Prescaler */

  uint32_t IC1Filter;           /*!< Specifies the input capture filter.
                                     This parameter can be a number between Min_Data = 0x0 and Max_Data = 0xF */
  uint32_t Commutation_Delay;  /*!< Specifies the pulse value to be loaded into the Capture Compare Register.
                                    This parameter can be a number between Min_Data = 0x0000 and Max_Data = 0xFFFF */
} TIM_HallSensor_InitTypeDef;

/**
  * @brief  TIM Master configuration Structure definition
  */
typedef struct {
  uint32_t  MasterOutputTrigger;   /*!< Trigger output (TRGO) selection.
                                      This parameter can be a value of @ref TIM_Master_Mode_Selection */
  uint32_t  MasterOutputTrigger2;  /*!< Trigger output2 (TRGO2) selection
                                      This parameter can be a value of @ref TIMEx_Master_Mode_Selection_2 */
  uint32_t  MasterSlaveMode;       /*!< Master/slave mode selection.
                                      This parameter can be a value of @ref TIM_Master_Slave_Mode */
}TIM_MasterConfigTypeDef;

/**
  * @brief  TIM Break input(s) and Dead time configuration Structure definition
  * @note   2 break inputs can be configured (BKIN and BKIN2) with configurable
  *        filter and polarity.
  */
typedef struct
{
  uint32_t OffStateRunMode;	    /*!< TIM off state in run mode.
                                       This parameter can be a value of @ref TIM_OSSR_Off_State_Selection_for_Run_mode_state */
  uint32_t OffStateIDLEMode;	    /*!< TIM off state in IDLE mode.
                                       This parameter can be a value of @ref TIM_OSSI_Off_State_Selection_for_Idle_mode_state */
  uint32_t LockLevel;	 	        /*!< TIM Lock level.
                                       This parameter can be a value of @ref TIM_Lock_level */
  uint32_t DeadTime;	 	        /*!< TIM dead Time.
                                       This parameter can be a number between Min_Data = 0x00 and Max_Data = 0xFF */
  uint32_t BreakState;	 	        /*!< TIM Break State.
                                       This parameter can be a value of @ref TIM_Break_Input_enable_disable */
  uint32_t BreakPolarity;           /*!< TIM Break input polarity.
                                       This parameter can be a value of @ref TIM_Break_Polarity */
  uint32_t BreakFilter;             /*!< Specifies the break input filter.
                                       This parameter can be a number between Min_Data = 0x0 and Max_Data = 0xF */
  uint32_t Break2State;	 	        /*!< TIM Break2 State
                                       This parameter can be a value of @ref TIMEx_Break2_Input_enable_disable */
  uint32_t Break2Polarity;          /*!< TIM Break2 input polarity
                                       This parameter can be a value of @ref TIMEx_Break2_Polarity */
  uint32_t Break2Filter;            /*!< TIM break2 input filter.
                                       This parameter can be a number between Min_Data = 0x0 and Max_Data = 0xF */
  uint32_t AutomaticOutput;         /*!< TIM Automatic Output Enable state
                                       This parameter can be a value of @ref TIM_AOE_Bit_Set_Reset */
} TIM_BreakDeadTimeConfigTypeDef;

#if defined (STM32F765xx) || defined(STM32F767xx) || defined(STM32F769xx) || defined(STM32F777xx) || defined(STM32F779xx)
/**
  * @brief  TIM Break/Break2 input configuration
  */
typedef struct {
  uint32_t Source;         /*!< Specifies the source of the timer break input.
                                This parameter can be a value of @ref TIMEx_Break_Input_Source */
  uint32_t Enable;         /*!< Specifies whether or not the break input source is enabled.
                                This parameter can be a value of @ref TIMEx_Break_Input_Source_Enable */
} TIMEx_BreakInputConfigTypeDef;
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
/**
  * @}
  */
/* Exported constants --------------------------------------------------------*/
/** @defgroup TIMEx_Exported_Constants  TIMEx Exported Constants
  * @{
  */

/** @defgroup TIMEx_Channel TIMEx Channel
  * @{
  */

#define TIM_CHANNEL_1                      ((uint32_t)0x0000U)
#define TIM_CHANNEL_2                      ((uint32_t)0x0004U)
#define TIM_CHANNEL_3                      ((uint32_t)0x0008U)
#define TIM_CHANNEL_4                      ((uint32_t)0x000CU)
#define TIM_CHANNEL_5                      ((uint32_t)0x0010U)
#define TIM_CHANNEL_6                      ((uint32_t)0x0014U)
#define TIM_CHANNEL_ALL                    ((uint32_t)0x003CU)

/**
  * @}
  */

/** @defgroup TIMEx_Output_Compare_and_PWM_modes TIMEx Output Compare and PWM Modes
  * @{
  */
#define TIM_OCMODE_TIMING                   ((uint32_t)0x0000U)
#define TIM_OCMODE_ACTIVE                   ((uint32_t)TIM_CCMR1_OC1M_0)
#define TIM_OCMODE_INACTIVE                 ((uint32_t)TIM_CCMR1_OC1M_1)
#define TIM_OCMODE_TOGGLE                   ((uint32_t)TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0)
#define TIM_OCMODE_PWM1                     ((uint32_t)TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1)
#define TIM_OCMODE_PWM2                     ((uint32_t)TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0)
#define TIM_OCMODE_FORCED_ACTIVE            ((uint32_t)TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_0)
#define TIM_OCMODE_FORCED_INACTIVE          ((uint32_t)TIM_CCMR1_OC1M_2)

#define TIM_OCMODE_RETRIGERRABLE_OPM1      ((uint32_t)TIM_CCMR1_OC1M_3)
#define TIM_OCMODE_RETRIGERRABLE_OPM2      ((uint32_t)TIM_CCMR1_OC1M_3 | TIM_CCMR1_OC1M_0)
#define TIM_OCMODE_COMBINED_PWM1           ((uint32_t)TIM_CCMR1_OC1M_3 | TIM_CCMR1_OC1M_2)
#define TIM_OCMODE_COMBINED_PWM2           ((uint32_t)TIM_CCMR1_OC1M_3 | TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_2)
#define TIM_OCMODE_ASSYMETRIC_PWM1         ((uint32_t)TIM_CCMR1_OC1M_3 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2)
#define TIM_OCMODE_ASSYMETRIC_PWM2         ((uint32_t)TIM_CCMR1_OC1M_3 | TIM_CCMR1_OC1M)
/**
  * @}
  */

/** @defgroup TIMEx_Remap  TIMEx Remap
  * @{
  */
#define TIM_TIM2_TIM8_TRGO                     (0x00000000U)
#define TIM_TIM2_ETH_PTP                       (0x00000400U)
#define TIM_TIM2_USBFS_SOF                     (0x00000800U)
#define TIM_TIM2_USBHS_SOF                     (0x00000C00U)
#define TIM_TIM5_GPIO                          (0x00000000U)
#define TIM_TIM5_LSI                           (0x00000040U)
#define TIM_TIM5_LSE                           (0x00000080U)
#define TIM_TIM5_RTC                           (0x000000C0U)
#define TIM_TIM11_GPIO                         (0x00000000U)
#define TIM_TIM11_SPDIFRX                      (0x00000001U)
#define TIM_TIM11_HSE                          (0x00000002U)
#define TIM_TIM11_MCO1                         (0x00000003U)
/**
  * @}
  */

/** @defgroup TIMEx_ClearInput_Source TIMEx Clear Input Source
  * @{
  */
#define TIM_CLEARINPUTSOURCE_ETR            ((uint32_t)0x0001U)
#define TIM_CLEARINPUTSOURCE_OCREFCLR       ((uint32_t)0x0002U)
#define TIM_CLEARINPUTSOURCE_NONE           ((uint32_t)0x0000U)
/**
  * @}
  */

/** @defgroup TIMEx_Break2_Input_enable_disable  TIMEx Break input 2 Enable
  * @{
  */
#define TIM_BREAK2_DISABLE         ((uint32_t)0x00000000U)
#define TIM_BREAK2_ENABLE          ((uint32_t)TIM_BDTR_BK2E)
/**
  * @}
  */

/** @defgroup TIMEx_Break2_Polarity TIMEx Break2 Polarity
  * @{
  */
#define TIM_BREAK2POLARITY_LOW        ((uint32_t)0x00000000U)
#define TIM_BREAK2POLARITY_HIGH       (TIM_BDTR_BK2P)
/**
  * @}
  */

/** @defgroup TIMEx_Group_Channel5 TIMEx Group Channel 5 and Channel 1, 2 or 3
  * @{
  */
#define TIM_GROUPCH5_NONE       ((uint32_t)0x00000000U)  /* !< No effect of OC5REF on OC1REFC, OC2REFC and OC3REFC */
#define TIM_GROUPCH5_OC1REFC    (TIM_CCR5_GC5C1)      /* !< OC1REFC is the logical AND of OC1REFC and OC5REF */
#define TIM_GROUPCH5_OC2REFC    (TIM_CCR5_GC5C2)      /* !< OC2REFC is the logical AND of OC2REFC and OC5REF */
#define TIM_GROUPCH5_OC3REFC    (TIM_CCR5_GC5C3)       /* !< OC3REFC is the logical AND of OC3REFC and OC5REF */
/**
  * @}
  */

/** @defgroup TIMEx_Master_Mode_Selection_2 TIMEx Master Mode Selection 2 (TRGO2)
  * @{
  */
#define	TIM_TRGO2_RESET                          ((uint32_t)0x00000000U)
#define	TIM_TRGO2_ENABLE                         ((uint32_t)(TIM_CR2_MMS2_0))
#define	TIM_TRGO2_UPDATE                         ((uint32_t)(TIM_CR2_MMS2_1))
#define	TIM_TRGO2_OC1                            ((uint32_t)(TIM_CR2_MMS2_1 | TIM_CR2_MMS2_0))
#define	TIM_TRGO2_OC1REF                         ((uint32_t)(TIM_CR2_MMS2_2))
#define	TIM_TRGO2_OC2REF                         ((uint32_t)(TIM_CR2_MMS2_2 | TIM_CR2_MMS2_0))
#define	TIM_TRGO2_OC3REF                         ((uint32_t)(TIM_CR2_MMS2_2 | TIM_CR2_MMS2_1))
#define	TIM_TRGO2_OC4REF                         ((uint32_t)(TIM_CR2_MMS2_2 | TIM_CR2_MMS2_1 | TIM_CR2_MMS2_0))
#define	TIM_TRGO2_OC5REF                         ((uint32_t)(TIM_CR2_MMS2_3))
#define	TIM_TRGO2_OC6REF                         ((uint32_t)(TIM_CR2_MMS2_3 | TIM_CR2_MMS2_0))
#define	TIM_TRGO2_OC4REF_RISINGFALLING           ((uint32_t)(TIM_CR2_MMS2_3 | TIM_CR2_MMS2_1))
#define	TIM_TRGO2_OC6REF_RISINGFALLING           ((uint32_t)(TIM_CR2_MMS2_3 | TIM_CR2_MMS2_1 | TIM_CR2_MMS2_0))
#define	TIM_TRGO2_OC4REF_RISING_OC6REF_RISING    ((uint32_t)(TIM_CR2_MMS2_3 | TIM_CR2_MMS2_2))
#define	TIM_TRGO2_OC4REF_RISING_OC6REF_FALLING   ((uint32_t)(TIM_CR2_MMS2_3 | TIM_CR2_MMS2_2 | TIM_CR2_MMS2_0))
#define	TIM_TRGO2_OC5REF_RISING_OC6REF_RISING    ((uint32_t)(TIM_CR2_MMS2_3 | TIM_CR2_MMS2_2 |TIM_CR2_MMS2_1))
#define	TIM_TRGO2_OC5REF_RISING_OC6REF_FALLING   ((uint32_t)(TIM_CR2_MMS2_3 | TIM_CR2_MMS2_2 | TIM_CR2_MMS2_1 | TIM_CR2_MMS2_0))
/**
  * @}
  */

/** @defgroup TIMEx_Slave_Mode TIMEx Slave mode
  * @{
  */
#define TIM_SLAVEMODE_DISABLE                ((uint32_t)0x0000U)
#define TIM_SLAVEMODE_RESET                  ((uint32_t)(TIM_SMCR_SMS_2))
#define TIM_SLAVEMODE_GATED                  ((uint32_t)(TIM_SMCR_SMS_2 | TIM_SMCR_SMS_0))
#define TIM_SLAVEMODE_TRIGGER                ((uint32_t)(TIM_SMCR_SMS_2 | TIM_SMCR_SMS_1))
#define TIM_SLAVEMODE_EXTERNAL1              ((uint32_t)(TIM_SMCR_SMS_2 | TIM_SMCR_SMS_1 | TIM_SMCR_SMS_0))
#define TIM_SLAVEMODE_COMBINED_RESETTRIGGER  ((uint32_t)(TIM_SMCR_SMS_3))
/**
  * @}
  */
#if defined(STM32F765xx) || defined(STM32F767xx) || defined(STM32F769xx) || defined(STM32F777xx) || defined(STM32F779xx)
/** @defgroup TIMEx_Break_Input TIM  Extended Break input
  * @{
  */
#define TIM_BREAKINPUT_BRK     ((uint32_t)0x00000001U) /* !< Timer break input  */
#define TIM_BREAKINPUT_BRK2    ((uint32_t)0x00000002U) /* !< Timer break2 input */
/**
  * @}
  */

/** @defgroup TIMEx_Break_Input_Source TIM  Extended Break input source
  * @{
  */
#define TIM_BREAKINPUTSOURCE_BKIN     ((uint32_t)0x00000001U) /* !< An external source (GPIO) is connected to the BKIN pin  */
#define TIM_BREAKINPUTSOURCE_DFSDM1    ((uint32_t)0x00000008U) /* !< The analog watchdog output of the DFSDM1 peripheral is connected to the break input */
/**
  * @}
  */

/** @defgroup TIMEx_Break_Input_Source_Enable TIM Extended Break input source enabling
  * @{
  */
#define TIM_BREAKINPUTSOURCE_DISABLE     ((uint32_t)0x00000000U) /* !< Break input source is disabled */
#define TIM_BREAKINPUTSOURCE_ENABLE      ((uint32_t)0x00000001U) /* !< Break input source is enabled */
/**
  * @}
  */

/**
  * @}
  */
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup TIMEx_Exported_Macros TIMEx Exported Macros
  * @{
  */

/**
  * @brief  Sets the TIM Capture Compare Register value on runtime without
  *         calling another time ConfigChannel function.
  * @param  __HANDLE__: TIM handle.
  * @param  __CHANNEL__ : TIM Channels to be configured.
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  *            @arg TIM_CHANNEL_5: TIM Channel 5 selected
  *            @arg TIM_CHANNEL_6: TIM Channel 6 selected
  * @param  __COMPARE__: specifies the Capture Compare register new value.
  * @retval None
  */
#define __HAL_TIM_SET_COMPARE(__HANDLE__, __CHANNEL__, __COMPARE__) \
(((__CHANNEL__) == TIM_CHANNEL_1) ? ((__HANDLE__)->Instance->CCR1 = (__COMPARE__)) :\
 ((__CHANNEL__) == TIM_CHANNEL_2) ? ((__HANDLE__)->Instance->CCR2 = (__COMPARE__)) :\
 ((__CHANNEL__) == TIM_CHANNEL_3) ? ((__HANDLE__)->Instance->CCR3 = (__COMPARE__)) :\
 ((__CHANNEL__) == TIM_CHANNEL_4) ? ((__HANDLE__)->Instance->CCR4 = (__COMPARE__)) :\
 ((__CHANNEL__) == TIM_CHANNEL_5) ? ((__HANDLE__)->Instance->CCR5 = (__COMPARE__)) :\
 ((__HANDLE__)->Instance->CCR6 = (__COMPARE__)))

/**
  * @brief  Gets the TIM Capture Compare Register value on runtime
  * @param  __HANDLE__: TIM handle.
  * @param  __CHANNEL__ : TIM Channel associated with the capture compare register
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: get capture/compare 1 register value
  *            @arg TIM_CHANNEL_2: get capture/compare 2 register value
  *            @arg TIM_CHANNEL_3: get capture/compare 3 register value
  *            @arg TIM_CHANNEL_4: get capture/compare 4 register value
  *            @arg TIM_CHANNEL_5: get capture/compare 5 register value
  *            @arg TIM_CHANNEL_6: get capture/compare 6 register value
  * @retval None
  */
#define __HAL_TIM_GET_COMPARE(__HANDLE__, __CHANNEL__) \
(((__CHANNEL__) == TIM_CHANNEL_1) ? ((__HANDLE__)->Instance->CCR1) :\
 ((__CHANNEL__) == TIM_CHANNEL_2) ? ((__HANDLE__)->Instance->CCR2) :\
 ((__CHANNEL__) == TIM_CHANNEL_3) ? ((__HANDLE__)->Instance->CCR3) :\
 ((__CHANNEL__) == TIM_CHANNEL_4) ? ((__HANDLE__)->Instance->CCR4) :\
 ((__CHANNEL__) == TIM_CHANNEL_5) ? ((__HANDLE__)->Instance->CCR5) :\
 ((__HANDLE__)->Instance->CCR6))

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup TIMEx_Exported_Functions
  * @{
  */

/** @addtogroup TIMEx_Exported_Functions_Group1
  * @{
  */
/*  Timer Hall Sensor functions  **********************************************/
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Init(TIM_HandleTypeDef* htim, TIM_HallSensor_InitTypeDef* sConfig);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_DeInit(TIM_HandleTypeDef* htim);

void HAL_TIMEx_HallSensor_MspInit(TIM_HandleTypeDef* htim);
void HAL_TIMEx_HallSensor_MspDeInit(TIM_HandleTypeDef* htim);

 /* Blocking mode: Polling */
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start(TIM_HandleTypeDef* htim);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop(TIM_HandleTypeDef* htim);
/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start_IT(TIM_HandleTypeDef* htim);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop_IT(TIM_HandleTypeDef* htim);
/* Non-Blocking mode: DMA */
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start_DMA(TIM_HandleTypeDef* htim, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop_DMA(TIM_HandleTypeDef* htim);
/**
  * @}
  */

/** @addtogroup TIMEx_Exported_Functions_Group2
  * @{
  */
/*  Timer Complementary Output Compare functions  *****************************/
/* Blocking mode: Polling */
HAL_StatusTypeDef HAL_TIMEx_OCN_Start(TIM_HandleTypeDef* htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_OCN_Stop(TIM_HandleTypeDef* htim, uint32_t Channel);

/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_TIMEx_OCN_Start_IT(TIM_HandleTypeDef* htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_OCN_Stop_IT(TIM_HandleTypeDef* htim, uint32_t Channel);

/* Non-Blocking mode: DMA */
HAL_StatusTypeDef HAL_TIMEx_OCN_Start_DMA(TIM_HandleTypeDef* htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIMEx_OCN_Stop_DMA(TIM_HandleTypeDef* htim, uint32_t Channel);
/**
  * @}
  */

/** @addtogroup TIMEx_Exported_Functions_Group3
  * @{
  */
/*  Timer Complementary PWM functions  ****************************************/
/* Blocking mode: Polling */
HAL_StatusTypeDef HAL_TIMEx_PWMN_Start(TIM_HandleTypeDef* htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop(TIM_HandleTypeDef* htim, uint32_t Channel);

/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_TIMEx_PWMN_Start_IT(TIM_HandleTypeDef* htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop_IT(TIM_HandleTypeDef* htim, uint32_t Channel);
/* Non-Blocking mode: DMA */
HAL_StatusTypeDef HAL_TIMEx_PWMN_Start_DMA(TIM_HandleTypeDef* htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop_DMA(TIM_HandleTypeDef* htim, uint32_t Channel);
/**
  * @}
  */

/** @addtogroup TIMEx_Exported_Functions_Group4
  * @{
  */
/*  Timer Complementary One Pulse functions  **********************************/
/* Blocking mode: Polling */
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Start(TIM_HandleTypeDef* htim, uint32_t OutputChannel);
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Stop(TIM_HandleTypeDef* htim, uint32_t OutputChannel);

/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Start_IT(TIM_HandleTypeDef* htim, uint32_t OutputChannel);
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Stop_IT(TIM_HandleTypeDef* htim, uint32_t OutputChannel);
/**
  * @}
  */

/** @addtogroup TIMEx_Exported_Functions_Group5
  * @{
  */
/* Extension Control functions  ************************************************/
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutationEvent(TIM_HandleTypeDef* htim, uint32_t  InputTrigger, uint32_t  CommutationSource);
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutationEvent_IT(TIM_HandleTypeDef* htim, uint32_t  InputTrigger, uint32_t  CommutationSource);
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutationEvent_DMA(TIM_HandleTypeDef* htim, uint32_t  InputTrigger, uint32_t  CommutationSource);
HAL_StatusTypeDef HAL_TIMEx_MasterConfigSynchronization(TIM_HandleTypeDef* htim, TIM_MasterConfigTypeDef * sMasterConfig);
HAL_StatusTypeDef HAL_TIMEx_ConfigBreakDeadTime(TIM_HandleTypeDef* htim, TIM_BreakDeadTimeConfigTypeDef *sBreakDeadTimeConfig);
#if defined (STM32F765xx) || defined(STM32F767xx) || defined(STM32F769xx) || defined(STM32F777xx) || defined(STM32F779xx)
HAL_StatusTypeDef HAL_TIMEx_ConfigBreakInput(TIM_HandleTypeDef *htim, uint32_t BreakInput, TIMEx_BreakInputConfigTypeDef *sBreakInputConfig);
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
HAL_StatusTypeDef HAL_TIMEx_RemapConfig(TIM_HandleTypeDef* htim, uint32_t Remap);
HAL_StatusTypeDef HAL_TIMEx_GroupChannel5(TIM_HandleTypeDef *htim, uint32_t OCRef);
/**
  * @}
  */

/** @addtogroup TIMEx_Exported_Functions_Group6
  * @{
  */
/* Extension Callback *********************************************************/
void HAL_TIMEx_CommutationCallback(TIM_HandleTypeDef* htim);
void HAL_TIMEx_BreakCallback(TIM_HandleTypeDef* htim);
void HAL_TIMEx_DMACommutationCplt(DMA_HandleTypeDef *hdma);
/**
  * @}
  */

/** @addtogroup TIMEx_Exported_Functions_Group7
  * @{
  */
/* Extension Peripheral State functions  **************************************/
HAL_TIM_StateTypeDef HAL_TIMEx_HallSensor_GetState(TIM_HandleTypeDef* htim);
/**
  * @}
  */

/**
  * @}
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/** @defgroup TIMEx_Private_Macros TIMEx Private Macros
  * @{
  */
#define IS_TIM_CHANNELS(CHANNEL) (((CHANNEL) == TIM_CHANNEL_1) || \
                                  ((CHANNEL) == TIM_CHANNEL_2) || \
                                  ((CHANNEL) == TIM_CHANNEL_3) || \
                                  ((CHANNEL) == TIM_CHANNEL_4) || \
                                  ((CHANNEL) == TIM_CHANNEL_5) || \
                                  ((CHANNEL) == TIM_CHANNEL_6) || \
                                  ((CHANNEL) == TIM_CHANNEL_ALL))

#define IS_TIM_PWMI_CHANNELS(CHANNEL) (((CHANNEL) == TIM_CHANNEL_1) || \
                                       ((CHANNEL) == TIM_CHANNEL_2))

#define IS_TIM_OPM_CHANNELS(CHANNEL) (((CHANNEL) == TIM_CHANNEL_1) || \
                                      ((CHANNEL) == TIM_CHANNEL_2))

#define IS_TIM_COMPLEMENTARY_CHANNELS(CHANNEL) (((CHANNEL) == TIM_CHANNEL_1) || \
                                                ((CHANNEL) == TIM_CHANNEL_2) || \
                                                ((CHANNEL) == TIM_CHANNEL_3))
#define IS_TIM_PWM_MODE(MODE) (((MODE) == TIM_OCMODE_PWM1)               || \
	                       ((MODE) == TIM_OCMODE_PWM2)               || \
                               ((MODE) == TIM_OCMODE_COMBINED_PWM1)      || \
                               ((MODE) == TIM_OCMODE_COMBINED_PWM2)      || \
                               ((MODE) == TIM_OCMODE_ASSYMETRIC_PWM1)    || \
                               ((MODE) == TIM_OCMODE_ASSYMETRIC_PWM2))

#define IS_TIM_OC_MODE(MODE) (((MODE) == TIM_OCMODE_TIMING)             || \
                             ((MODE) == TIM_OCMODE_ACTIVE)             || \
                             ((MODE) == TIM_OCMODE_INACTIVE)           || \
                             ((MODE) == TIM_OCMODE_TOGGLE)             || \
                             ((MODE) == TIM_OCMODE_FORCED_ACTIVE)      || \
                             ((MODE) == TIM_OCMODE_FORCED_INACTIVE)    || \
                             ((MODE) == TIM_OCMODE_RETRIGERRABLE_OPM1) || \
                             ((MODE) == TIM_OCMODE_RETRIGERRABLE_OPM2))
#define IS_TIM_REMAP(__TIM_REMAP__)	 (((__TIM_REMAP__) == TIM_TIM2_TIM8_TRGO)||\
                                      ((__TIM_REMAP__) == TIM_TIM2_ETH_PTP)||\
                                      ((__TIM_REMAP__) == TIM_TIM2_USBFS_SOF)||\
                                      ((__TIM_REMAP__) == TIM_TIM2_USBHS_SOF)||\
                                      ((__TIM_REMAP__) == TIM_TIM5_GPIO)||\
                                      ((__TIM_REMAP__) == TIM_TIM5_LSI)||\
                                      ((__TIM_REMAP__) == TIM_TIM5_LSE)||\
                                      ((__TIM_REMAP__) == TIM_TIM5_RTC)||\
                                      ((__TIM_REMAP__) == TIM_TIM11_GPIO)||\
                                      ((__TIM_REMAP__) == TIM_TIM11_SPDIFRX)||\
                                      ((__TIM_REMAP__) == TIM_TIM11_HSE)||\
                                      ((__TIM_REMAP__) == TIM_TIM11_MCO1))
#define IS_TIM_DEADTIME(__DEADTIME__)      ((__DEADTIME__) <= 0xFF)
#define IS_TIM_BREAK_FILTER(__FILTER__) ((__FILTER__) <= 0xF)
#define IS_TIM_CLEARINPUT_SOURCE(MODE) (((MODE) == TIM_CLEARINPUTSOURCE_ETR)      || \
                                        ((MODE) == TIM_CLEARINPUTSOURCE_OCREFCLR)  || \
                                        ((MODE) == TIM_CLEARINPUTSOURCE_NONE))
#define IS_TIM_BREAK2_STATE(STATE) (((STATE) == TIM_BREAK2_ENABLE) || \
                                    ((STATE) == TIM_BREAK2_DISABLE))
#define IS_TIM_BREAK2_POLARITY(__POLARITY__) (((__POLARITY__) == TIM_BREAK2POLARITY_LOW) || \
                                              ((__POLARITY__) == TIM_BREAK2POLARITY_HIGH))
#define IS_TIM_GROUPCH5(OCREF) ((((OCREF) & 0x1FFFFFFF) == 0x00000000))
#define IS_TIM_TRGO2_SOURCE(SOURCE) (((SOURCE) == TIM_TRGO2_RESET)                        || \
                                     ((SOURCE) == TIM_TRGO2_ENABLE)                       || \
                                     ((SOURCE) == TIM_TRGO2_UPDATE)                       || \
                                     ((SOURCE) == TIM_TRGO2_OC1)                          || \
                                     ((SOURCE) == TIM_TRGO2_OC1REF)                       || \
                                     ((SOURCE) == TIM_TRGO2_OC2REF)                       || \
                                     ((SOURCE) == TIM_TRGO2_OC3REF)                       || \
                                     ((SOURCE) == TIM_TRGO2_OC3REF)                       || \
                                     ((SOURCE) == TIM_TRGO2_OC4REF)                       || \
                                     ((SOURCE) == TIM_TRGO2_OC5REF)                       || \
                                     ((SOURCE) == TIM_TRGO2_OC6REF)                       || \
                                     ((SOURCE) == TIM_TRGO2_OC4REF_RISINGFALLING)         || \
                                     ((SOURCE) == TIM_TRGO2_OC6REF_RISINGFALLING)         || \
                                     ((SOURCE) == TIM_TRGO2_OC4REF_RISING_OC6REF_RISING)  || \
                                     ((SOURCE) == TIM_TRGO2_OC4REF_RISING_OC6REF_FALLING) || \
                                     ((SOURCE) == TIM_TRGO2_OC5REF_RISING_OC6REF_RISING)  || \
                                     ((SOURCE) == TIM_TRGO2_OC5REF_RISING_OC6REF_FALLING))
#define IS_TIM_SLAVE_MODE(MODE) (((MODE) == TIM_SLAVEMODE_DISABLE)   || \
                                 ((MODE) == TIM_SLAVEMODE_RESET)     || \
                                 ((MODE) == TIM_SLAVEMODE_GATED)     || \
                                 ((MODE) == TIM_SLAVEMODE_TRIGGER)   || \
                                 ((MODE) == TIM_SLAVEMODE_EXTERNAL1) || \
                                 ((MODE) == TIM_SLAVEMODE_COMBINED_RESETTRIGGER))

#if defined (STM32F765xx) || defined(STM32F767xx) || defined(STM32F769xx) || defined(STM32F777xx) || defined(STM32F779xx)
#define IS_TIM_BREAKINPUT(__BREAKINPUT__)  (((__BREAKINPUT__) == TIM_BREAKINPUT_BRK)  || \
                                            ((__BREAKINPUT__) == TIM_BREAKINPUT_BRK2))

#define IS_TIM_BREAKINPUTSOURCE(__SOURCE__)  (((__SOURCE__) == TIM_BREAKINPUTSOURCE_BKIN)  || \
                                              ((__SOURCE__) == TIM_BREAKINPUTSOURCE_DFSDM))

#define IS_TIM_BREAKINPUTSOURCE_STATE(__STATE__)  (((__STATE__) == TIM_BREAKINPUTSOURCE_DISABLE)  || \
                                                   ((__STATE__) == TIM_BREAKINPUTSOURCE_ENABLE))

#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup TIMEx_Private_Functions TIMEx Private Functions
  * @{
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

#endif /* __STM32F7xx_HAL_TIM_EX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
