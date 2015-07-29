/**
  ******************************************************************************
  * @file    stm32f7xx_hal_adc.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    25-June-2015
  * @brief   Header file of ADC HAL extension module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
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
#ifndef __STM32F7xx_ADC_H
#define __STM32F7xx_ADC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal_def.h"

/** @addtogroup STM32F7xx_HAL_Driver
  * @{
  */

/** @addtogroup ADC
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup ADC_Exported_Types ADC Exported Types
  * @{
  */

/**
  * @brief  HAL State structures definition
  */
typedef enum
{
  HAL_ADC_STATE_RESET                   = 0x00,    /*!< ADC not yet initialized or disabled */
  HAL_ADC_STATE_READY                   = 0x01,    /*!< ADC peripheral ready for use */
  HAL_ADC_STATE_BUSY                    = 0x02,    /*!< An internal process is ongoing */
  HAL_ADC_STATE_BUSY_REG                = 0x12,    /*!< Regular conversion is ongoing */
  HAL_ADC_STATE_BUSY_INJ                = 0x22,    /*!< Injected conversion is ongoing */
  HAL_ADC_STATE_BUSY_INJ_REG            = 0x32,    /*!< Injected and regular conversion are ongoing */
  HAL_ADC_STATE_TIMEOUT                 = 0x03,    /*!< Timeout state */
  HAL_ADC_STATE_ERROR                   = 0x04,    /*!< ADC state error */
  HAL_ADC_STATE_EOC                     = 0x05,    /*!< Conversion is completed */
  HAL_ADC_STATE_EOC_REG                 = 0x15,    /*!< Regular conversion is completed */
  HAL_ADC_STATE_EOC_INJ                 = 0x25,    /*!< Injected conversion is completed */
  HAL_ADC_STATE_EOC_INJ_REG             = 0x35,    /*!< Injected and regular conversion are completed */
  HAL_ADC_STATE_AWD                     = 0x06    /*!< ADC state analog watchdog */

}HAL_ADC_StateTypeDef;

/**
  * @brief   ADC Init structure definition
  */
typedef struct
{
  uint32_t ClockPrescaler;        /*!< Select the frequency of the clock to the ADC. The clock is common for
                                       all the ADCs.
                                       This parameter can be a value of @ref ADC_ClockPrescaler */
  uint32_t Resolution;            /*!< Configures the ADC resolution dual mode.
                                       This parameter can be a value of @ref ADC_Resolution */
  uint32_t DataAlign;             /*!< Specifies whether the ADC data  alignment is left or right.
                                       This parameter can be a value of @ref ADC_data_align */
  uint32_t ScanConvMode;          /*!< Specifies whether the conversion is performed in Scan (multi channels) or
                                       Single (one channel) mode.
                                       This parameter can be set to ENABLE or DISABLE */
  uint32_t EOCSelection;          /*!< Specifies whether the EOC flag is set
                                       at the end of single channel conversion or at the end of all conversions.
                                       This parameter can be a value of @ref ADC_EOCSelection */
  uint32_t ContinuousConvMode;    /*!< Specifies whether the conversion is performed in Continuous or Single mode.
                                       This parameter can be set to ENABLE or DISABLE. */
  uint32_t DMAContinuousRequests; /*!< Specifies whether the DMA requests is performed in Continuous or in Single mode.
                                       This parameter can be set to ENABLE or DISABLE. */
  uint32_t NbrOfConversion;       /*!< Specifies the number of ADC conversions that will be done using the sequencer for
                                       regular channel group.
                                       This parameter must be a number between Min_Data = 1 and Max_Data = 16. */
  uint32_t DiscontinuousConvMode; /*!< Specifies whether the conversion is performed in Discontinuous or not
                                       for regular channels.
                                       This parameter can be set to ENABLE or DISABLE. */
  uint32_t NbrOfDiscConversion;   /*!< Specifies the number of ADC discontinuous conversions that will be done
                                       using the sequencer for regular channel group.
                                       This parameter must be a number between Min_Data = 1 and Max_Data = 8. */
  uint32_t ExternalTrigConv;      /*!< Selects the external event used to trigger the conversion start of regular group.
                                       If set to ADC_SOFTWARE_START, external triggers are disabled.
                                       This parameter can be a value of @ref ADC_External_trigger_Source_Regular
                                       Note: This parameter can be modified only if there is no conversion is ongoing. */
  uint32_t ExternalTrigConvEdge;  /*!< Selects the external trigger edge of regular group.
                                       If trigger is set to ADC_SOFTWARE_START, this parameter is discarded.
                                       This parameter can be a value of @ref ADC_External_trigger_edge_Regular
                                       Note: This parameter can be modified only if there is no conversion is ongoing. */
}ADC_InitTypeDef;

/**
  * @brief  ADC handle Structure definition
  */
typedef struct
{
  ADC_TypeDef                   *Instance;                   /*!< Register base address */

  ADC_InitTypeDef               Init;                        /*!< ADC required parameters */

  __IO uint32_t                 NbrOfCurrentConversionRank;  /*!< ADC number of current conversion rank */

  DMA_HandleTypeDef             *DMA_Handle;                 /*!< Pointer DMA Handler */

  HAL_LockTypeDef               Lock;                        /*!< ADC locking object */

  __IO HAL_ADC_StateTypeDef     State;                       /*!< ADC communication state */

  __IO uint32_t                 ErrorCode;                   /*!< ADC Error code */
}ADC_HandleTypeDef;

/**
  * @brief   ADC Configuration regular Channel structure definition
  */
typedef struct
{
  uint32_t Channel;        /*!< The ADC channel to configure.
                                This parameter can be a value of @ref ADC_channels */
  uint32_t Rank;           /*!< The rank in the regular group sequencer.
                                This parameter must be a number between Min_Data = 1 and Max_Data = 16 */
  uint32_t SamplingTime;   /*!< The sample time value to be set for the selected channel.
                                This parameter can be a value of @ref ADC_sampling_times */
  uint32_t Offset;         /*!< Reserved for future use, can be set to 0 */
}ADC_ChannelConfTypeDef;

/**
  * @brief   ADC Configuration multi-mode structure definition
  */
typedef struct
{
  uint32_t WatchdogMode;      /*!< Configures the ADC analog watchdog mode.
                                   This parameter can be a value of @ref ADC_analog_watchdog_selection */
  uint32_t HighThreshold;     /*!< Configures the ADC analog watchdog High threshold value.
                                   This parameter must be a 12-bit value. */
  uint32_t LowThreshold;      /*!< Configures the ADC analog watchdog High threshold value.
                                   This parameter must be a 12-bit value. */
  uint32_t Channel;           /*!< Configures ADC channel for the analog watchdog.
                                   This parameter has an effect only if watchdog mode is configured on single channel
                                   This parameter can be a value of @ref ADC_channels */
  uint32_t ITMode;            /*!< Specifies whether the analog watchdog is configured
                                   is interrupt mode or in polling mode.
                                   This parameter can be set to ENABLE or DISABLE */
  uint32_t WatchdogNumber;    /*!< Reserved for future use, can be set to 0 */
}ADC_AnalogWDGConfTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup ADC_Exported_Constants ADC Exported Constants
  * @{
  */


/** @defgroup ADC_Error_Code ADC Error Code
  * @{
  */

#define HAL_ADC_ERROR_NONE        ((uint32_t)0x00)   /*!< No error             */
#define HAL_ADC_ERROR_OVR         ((uint32_t)0x01)   /*!< OVR error            */
#define HAL_ADC_ERROR_DMA         ((uint32_t)0x02)   /*!< DMA transfer error   */
/**
  * @}
  */


/** @defgroup ADC_ClockPrescaler ADC Clock Prescaler
  * @{
  */
#define ADC_CLOCK_SYNC_PCLK_DIV2    ((uint32_t)0x00000000)
#define ADC_CLOCK_SYNC_PCLK_DIV4    ((uint32_t)ADC_CCR_ADCPRE_0)
#define ADC_CLOCK_SYNC_PCLK_DIV6    ((uint32_t)ADC_CCR_ADCPRE_1)
#define ADC_CLOCK_SYNC_PCLK_DIV8    ((uint32_t)ADC_CCR_ADCPRE)
/**
  * @}
  */

/** @defgroup ADC_delay_between_2_sampling_phases ADC Delay Between 2 Sampling Phases
  * @{
  */
#define ADC_TWOSAMPLINGDELAY_5CYCLES    ((uint32_t)0x00000000)
#define ADC_TWOSAMPLINGDELAY_6CYCLES    ((uint32_t)ADC_CCR_DELAY_0)
#define ADC_TWOSAMPLINGDELAY_7CYCLES    ((uint32_t)ADC_CCR_DELAY_1)
#define ADC_TWOSAMPLINGDELAY_8CYCLES    ((uint32_t)(ADC_CCR_DELAY_1 | ADC_CCR_DELAY_0))
#define ADC_TWOSAMPLINGDELAY_9CYCLES    ((uint32_t)ADC_CCR_DELAY_2)
#define ADC_TWOSAMPLINGDELAY_10CYCLES   ((uint32_t)(ADC_CCR_DELAY_2 | ADC_CCR_DELAY_0))
#define ADC_TWOSAMPLINGDELAY_11CYCLES   ((uint32_t)(ADC_CCR_DELAY_2 | ADC_CCR_DELAY_1))
#define ADC_TWOSAMPLINGDELAY_12CYCLES   ((uint32_t)(ADC_CCR_DELAY_2 | ADC_CCR_DELAY_1 | ADC_CCR_DELAY_0))
#define ADC_TWOSAMPLINGDELAY_13CYCLES   ((uint32_t)ADC_CCR_DELAY_3)
#define ADC_TWOSAMPLINGDELAY_14CYCLES   ((uint32_t)(ADC_CCR_DELAY_3 | ADC_CCR_DELAY_0))
#define ADC_TWOSAMPLINGDELAY_15CYCLES   ((uint32_t)(ADC_CCR_DELAY_3 | ADC_CCR_DELAY_1))
#define ADC_TWOSAMPLINGDELAY_16CYCLES   ((uint32_t)(ADC_CCR_DELAY_3 | ADC_CCR_DELAY_1 | ADC_CCR_DELAY_0))
#define ADC_TWOSAMPLINGDELAY_17CYCLES   ((uint32_t)(ADC_CCR_DELAY_3 | ADC_CCR_DELAY_2))
#define ADC_TWOSAMPLINGDELAY_18CYCLES   ((uint32_t)(ADC_CCR_DELAY_3 | ADC_CCR_DELAY_2 | ADC_CCR_DELAY_0))
#define ADC_TWOSAMPLINGDELAY_19CYCLES   ((uint32_t)(ADC_CCR_DELAY_3 | ADC_CCR_DELAY_2 | ADC_CCR_DELAY_1))
#define ADC_TWOSAMPLINGDELAY_20CYCLES   ((uint32_t)ADC_CCR_DELAY)
/**
  * @}
  */

/** @defgroup ADC_Resolution ADC Resolution
  * @{
  */
#define ADC_RESOLUTION_12B  ((uint32_t)0x00000000)
#define ADC_RESOLUTION_10B  ((uint32_t)ADC_CR1_RES_0)
#define ADC_RESOLUTION_8B   ((uint32_t)ADC_CR1_RES_1)
#define ADC_RESOLUTION_6B   ((uint32_t)ADC_CR1_RES)
/**
  * @}
  */

/** @defgroup ADC_External_trigger_edge_Regular ADC External Trigger Edge Regular
  * @{
  */
#define ADC_EXTERNALTRIGCONVEDGE_NONE           ((uint32_t)0x00000000)
#define ADC_EXTERNALTRIGCONVEDGE_RISING         ((uint32_t)ADC_CR2_EXTEN_0)
#define ADC_EXTERNALTRIGCONVEDGE_FALLING        ((uint32_t)ADC_CR2_EXTEN_1)
#define ADC_EXTERNALTRIGCONVEDGE_RISINGFALLING  ((uint32_t)ADC_CR2_EXTEN)
/**
  * @}
  */

/** @defgroup ADC_External_trigger_Source_Regular ADC External Trigger Source Regular
  * @{
  */
/* Note: Parameter ADC_SOFTWARE_START is a software parameter used for        */
/*       compatibility with other STM32 devices.                              */
#define ADC_EXTERNALTRIGCONV_T1_CC1    ((uint32_t)0x00000000)
#define ADC_EXTERNALTRIGCONV_T1_CC2    ((uint32_t)ADC_CR2_EXTSEL_0)
#define ADC_EXTERNALTRIGCONV_T1_CC3    ((uint32_t)ADC_CR2_EXTSEL_1)
#define ADC_EXTERNALTRIGCONV_T2_CC2    ((uint32_t)(ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_0))
#define ADC_EXTERNALTRIGCONV_T5_TRGO   ((uint32_t)ADC_CR2_EXTSEL_2)
#define ADC_EXTERNALTRIGCONV_T4_CC4    ((uint32_t)(ADC_CR2_EXTSEL_2 | ADC_CR2_EXTSEL_0))
#define ADC_EXTERNALTRIGCONV_T3_CC4    ((uint32_t)(ADC_CR2_EXTSEL_2 | ADC_CR2_EXTSEL_1))
#define ADC_EXTERNALTRIGCONV_T8_TRGO   ((uint32_t)(ADC_CR2_EXTSEL_2 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_0))
#define ADC_EXTERNALTRIGCONV_T8_TRGO2  ((uint32_t)ADC_CR2_EXTSEL_3)
#define ADC_EXTERNALTRIGCONV_T1_TRGO   ((uint32_t)(ADC_CR2_EXTSEL_3 | ADC_CR2_EXTSEL_0))
#define ADC_EXTERNALTRIGCONV_T1_TRGO2  ((uint32_t)(ADC_CR2_EXTSEL_3 | ADC_CR2_EXTSEL_1))
#define ADC_EXTERNALTRIGCONV_T2_TRGO   ((uint32_t)(ADC_CR2_EXTSEL_3 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_0))
#define ADC_EXTERNALTRIGCONV_T4_TRGO   ((uint32_t)(ADC_CR2_EXTSEL_3 | ADC_CR2_EXTSEL_2))
#define ADC_EXTERNALTRIGCONV_T6_TRGO   ((uint32_t)(ADC_CR2_EXTSEL_3 | ADC_CR2_EXTSEL_2 | ADC_CR2_EXTSEL_0))

#define ADC_EXTERNALTRIGCONV_EXT_IT11  ((uint32_t)ADC_CR2_EXTSEL)
#define ADC_SOFTWARE_START             ((uint32_t)ADC_CR2_EXTSEL + 1)
/**
  * @}
  */

/** @defgroup ADC_data_align ADC Data Align
  * @{
  */
#define ADC_DATAALIGN_RIGHT      ((uint32_t)0x00000000)
#define ADC_DATAALIGN_LEFT       ((uint32_t)ADC_CR2_ALIGN)
/**
  * @}
  */

/** @defgroup ADC_channels ADC Common Channels
  * @{
  */
#define ADC_CHANNEL_0           ((uint32_t)0x00000000)
#define ADC_CHANNEL_1           ((uint32_t)ADC_CR1_AWDCH_0)
#define ADC_CHANNEL_2           ((uint32_t)ADC_CR1_AWDCH_1)
#define ADC_CHANNEL_3           ((uint32_t)(ADC_CR1_AWDCH_1 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_4           ((uint32_t)ADC_CR1_AWDCH_2)
#define ADC_CHANNEL_5           ((uint32_t)(ADC_CR1_AWDCH_2 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_6           ((uint32_t)(ADC_CR1_AWDCH_2 | ADC_CR1_AWDCH_1))
#define ADC_CHANNEL_7           ((uint32_t)(ADC_CR1_AWDCH_2 | ADC_CR1_AWDCH_1 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_8           ((uint32_t)ADC_CR1_AWDCH_3)
#define ADC_CHANNEL_9           ((uint32_t)(ADC_CR1_AWDCH_3 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_10          ((uint32_t)(ADC_CR1_AWDCH_3 | ADC_CR1_AWDCH_1))
#define ADC_CHANNEL_11          ((uint32_t)(ADC_CR1_AWDCH_3 | ADC_CR1_AWDCH_1 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_12          ((uint32_t)(ADC_CR1_AWDCH_3 | ADC_CR1_AWDCH_2))
#define ADC_CHANNEL_13          ((uint32_t)(ADC_CR1_AWDCH_3 | ADC_CR1_AWDCH_2 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_14          ((uint32_t)(ADC_CR1_AWDCH_3 | ADC_CR1_AWDCH_2 | ADC_CR1_AWDCH_1))
#define ADC_CHANNEL_15          ((uint32_t)(ADC_CR1_AWDCH_3 | ADC_CR1_AWDCH_2 | ADC_CR1_AWDCH_1 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_16          ((uint32_t)ADC_CR1_AWDCH_4)
#define ADC_CHANNEL_17          ((uint32_t)(ADC_CR1_AWDCH_4 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_18          ((uint32_t)(ADC_CR1_AWDCH_4 | ADC_CR1_AWDCH_1))

#define ADC_CHANNEL_VREFINT     ((uint32_t)ADC_CHANNEL_17)
#define ADC_CHANNEL_VBAT        ((uint32_t)ADC_CHANNEL_18)
/**
  * @}
  */

/** @defgroup ADC_sampling_times ADC Sampling Times
  * @{
  */
#define ADC_SAMPLETIME_3CYCLES    ((uint32_t)0x00000000)
#define ADC_SAMPLETIME_15CYCLES   ((uint32_t)ADC_SMPR1_SMP10_0)
#define ADC_SAMPLETIME_28CYCLES   ((uint32_t)ADC_SMPR1_SMP10_1)
#define ADC_SAMPLETIME_56CYCLES   ((uint32_t)(ADC_SMPR1_SMP10_1 | ADC_SMPR1_SMP10_0))
#define ADC_SAMPLETIME_84CYCLES   ((uint32_t)ADC_SMPR1_SMP10_2)
#define ADC_SAMPLETIME_112CYCLES  ((uint32_t)(ADC_SMPR1_SMP10_2 | ADC_SMPR1_SMP10_0))
#define ADC_SAMPLETIME_144CYCLES  ((uint32_t)(ADC_SMPR1_SMP10_2 | ADC_SMPR1_SMP10_1))
#define ADC_SAMPLETIME_480CYCLES  ((uint32_t)ADC_SMPR1_SMP10)
/**
  * @}
  */

  /** @defgroup ADC_EOCSelection ADC EOC Selection
  * @{
  */
#define ADC_EOC_SEQ_CONV              ((uint32_t)0x00000000)
#define ADC_EOC_SINGLE_CONV           ((uint32_t)0x00000001)
#define ADC_EOC_SINGLE_SEQ_CONV       ((uint32_t)0x00000002)  /*!< reserved for future use */
/**
  * @}
  */

/** @defgroup ADC_Event_type ADC Event Type
  * @{
  */
#define ADC_AWD_EVENT             ((uint32_t)ADC_FLAG_AWD)
#define ADC_OVR_EVENT             ((uint32_t)ADC_FLAG_OVR)
/**
  * @}
  */

/** @defgroup ADC_analog_watchdog_selection ADC Analog Watchdog Selection
  * @{
  */
#define ADC_ANALOGWATCHDOG_SINGLE_REG         ((uint32_t)(ADC_CR1_AWDSGL | ADC_CR1_AWDEN))
#define ADC_ANALOGWATCHDOG_SINGLE_INJEC       ((uint32_t)(ADC_CR1_AWDSGL | ADC_CR1_JAWDEN))
#define ADC_ANALOGWATCHDOG_SINGLE_REGINJEC    ((uint32_t)(ADC_CR1_AWDSGL | ADC_CR1_AWDEN | ADC_CR1_JAWDEN))
#define ADC_ANALOGWATCHDOG_ALL_REG            ((uint32_t)ADC_CR1_AWDEN)
#define ADC_ANALOGWATCHDOG_ALL_INJEC          ((uint32_t)ADC_CR1_JAWDEN)
#define ADC_ANALOGWATCHDOG_ALL_REGINJEC       ((uint32_t)(ADC_CR1_AWDEN | ADC_CR1_JAWDEN))
#define ADC_ANALOGWATCHDOG_NONE               ((uint32_t)0x00000000)
/**
  * @}
  */

/** @defgroup ADC_interrupts_definition ADC Interrupts Definition
  * @{
  */
#define ADC_IT_EOC      ((uint32_t)ADC_CR1_EOCIE)
#define ADC_IT_AWD      ((uint32_t)ADC_CR1_AWDIE)
#define ADC_IT_JEOC     ((uint32_t)ADC_CR1_JEOCIE)
#define ADC_IT_OVR      ((uint32_t)ADC_CR1_OVRIE)
/**
  * @}
  */

/** @defgroup ADC_flags_definition ADC Flags Definition
  * @{
  */
#define ADC_FLAG_AWD    ((uint32_t)ADC_SR_AWD)
#define ADC_FLAG_EOC    ((uint32_t)ADC_SR_EOC)
#define ADC_FLAG_JEOC   ((uint32_t)ADC_SR_JEOC)
#define ADC_FLAG_JSTRT  ((uint32_t)ADC_SR_JSTRT)
#define ADC_FLAG_STRT   ((uint32_t)ADC_SR_STRT)
#define ADC_FLAG_OVR    ((uint32_t)ADC_SR_OVR)
/**
  * @}
  */

/** @defgroup ADC_channels_type ADC Channels Type
  * @{
  */
#define ADC_ALL_CHANNELS      ((uint32_t)0x00000001)
#define ADC_REGULAR_CHANNELS  ((uint32_t)0x00000002) /*!< reserved for future use */
#define ADC_INJECTED_CHANNELS ((uint32_t)0x00000003) /*!< reserved for future use */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup ADC_Exported_Macros ADC Exported Macros
  * @{
  */

/** @brief Reset ADC handle state
  * @param  __HANDLE__: ADC handle
  * @retval None
  */
#define __HAL_ADC_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_ADC_STATE_RESET)

/**
  * @brief  Enable the ADC peripheral.
  * @param  __HANDLE__: ADC handle
  * @retval None
  */
#define __HAL_ADC_ENABLE(__HANDLE__) ((__HANDLE__)->Instance->CR2 |=  ADC_CR2_ADON)

/**
  * @brief  Disable the ADC peripheral.
  * @param  __HANDLE__: ADC handle
  * @retval None
  */
#define __HAL_ADC_DISABLE(__HANDLE__) ((__HANDLE__)->Instance->CR2 &=  ~ADC_CR2_ADON)

/**
  * @brief  Enable the ADC end of conversion interrupt.
  * @param  __HANDLE__: specifies the ADC Handle.
  * @param  __INTERRUPT__: ADC Interrupt.
  * @retval None
  */
#define __HAL_ADC_ENABLE_IT(__HANDLE__, __INTERRUPT__) (((__HANDLE__)->Instance->CR1) |= (__INTERRUPT__))

/**
  * @brief  Disable the ADC end of conversion interrupt.
  * @param  __HANDLE__: specifies the ADC Handle.
  * @param  __INTERRUPT__: ADC interrupt.
  * @retval None
  */
#define __HAL_ADC_DISABLE_IT(__HANDLE__, __INTERRUPT__) (((__HANDLE__)->Instance->CR1) &= ~(__INTERRUPT__))

/** @brief  Check if the specified ADC interrupt source is enabled or disabled.
  * @param  __HANDLE__: specifies the ADC Handle.
  * @param  __INTERRUPT__: specifies the ADC interrupt source to check.
  * @retval The new state of __IT__ (TRUE or FALSE).
  */
#define __HAL_ADC_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__)  (((__HANDLE__)->Instance->CR1 & (__INTERRUPT__)) == (__INTERRUPT__))

/**
  * @brief  Clear the ADC's pending flags.
  * @param  __HANDLE__: specifies the ADC Handle.
  * @param  __FLAG__: ADC flag.
  * @retval None
  */
#define __HAL_ADC_CLEAR_FLAG(__HANDLE__, __FLAG__) (((__HANDLE__)->Instance->SR) = ~(__FLAG__))

/**
  * @brief  Get the selected ADC's flag status.
  * @param  __HANDLE__: specifies the ADC Handle.
  * @param  __FLAG__: ADC flag.
  * @retval None
  */
#define __HAL_ADC_GET_FLAG(__HANDLE__, __FLAG__) ((((__HANDLE__)->Instance->SR) & (__FLAG__)) == (__FLAG__))

/**
  * @}
  */

/* Include ADC HAL Extension module */
#include "stm32f7xx_hal_adc_ex.h"

/* Exported functions --------------------------------------------------------*/
/** @addtogroup ADC_Exported_Functions
  * @{
  */

/** @addtogroup ADC_Exported_Functions_Group1
  * @{
  */
/* Initialization/de-initialization functions ***********************************/
HAL_StatusTypeDef HAL_ADC_Init(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef HAL_ADC_DeInit(ADC_HandleTypeDef *hadc);
void       HAL_ADC_MspInit(ADC_HandleTypeDef* hadc);
void       HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc);
/**
  * @}
  */

/** @addtogroup ADC_Exported_Functions_Group2
  * @{
  */
/* I/O operation functions ******************************************************/
HAL_StatusTypeDef HAL_ADC_Start(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef HAL_ADC_Stop(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef HAL_ADC_PollForConversion(ADC_HandleTypeDef* hadc, uint32_t Timeout);

HAL_StatusTypeDef HAL_ADC_PollForEvent(ADC_HandleTypeDef* hadc, uint32_t EventType, uint32_t Timeout);

HAL_StatusTypeDef HAL_ADC_Start_IT(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef HAL_ADC_Stop_IT(ADC_HandleTypeDef* hadc);

void              HAL_ADC_IRQHandler(ADC_HandleTypeDef* hadc);

HAL_StatusTypeDef HAL_ADC_Start_DMA(ADC_HandleTypeDef* hadc, uint32_t* pData, uint32_t Length);
HAL_StatusTypeDef HAL_ADC_Stop_DMA(ADC_HandleTypeDef* hadc);

uint32_t          HAL_ADC_GetValue(ADC_HandleTypeDef* hadc);

void       HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
void       HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc);
void       HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc);
void       HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc);
/**
  * @}
  */

/** @addtogroup ADC_Exported_Functions_Group3
  * @{
  */
/* Peripheral Control functions *************************************************/
HAL_StatusTypeDef HAL_ADC_ConfigChannel(ADC_HandleTypeDef* hadc, ADC_ChannelConfTypeDef* sConfig);
HAL_StatusTypeDef HAL_ADC_AnalogWDGConfig(ADC_HandleTypeDef* hadc, ADC_AnalogWDGConfTypeDef* AnalogWDGConfig);
/**
  * @}
  */

/** @addtogroup ADC_Exported_Functions_Group4
  * @{
  */
/* Peripheral State functions ***************************************************/
HAL_ADC_StateTypeDef HAL_ADC_GetState(ADC_HandleTypeDef* hadc);
uint32_t             HAL_ADC_GetError(ADC_HandleTypeDef *hadc);
/**
  * @}
  */

/**
  * @}
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @defgroup ADC_Private_Constants ADC Private Constants
  * @{
  */
/* Delay for ADC stabilization time.                                        */
/* Maximum delay is 1us (refer to device datasheet, parameter tSTAB).       */
/* Unit: us                                                                 */
#define ADC_STAB_DELAY_US               ((uint32_t) 3)
/* Delay for temperature sensor stabilization time.                         */
/* Maximum delay is 10us (refer to device datasheet, parameter tSTART).     */
/* Unit: us                                                                 */
#define ADC_TEMPSENSOR_DELAY_US         ((uint32_t) 10)
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup ADC_Private_Macros ADC Private Macros
  * @{
  */
#define IS_ADC_CLOCKPRESCALER(__ADC_CLOCK__)     (((__ADC_CLOCK__) == ADC_CLOCK_SYNC_PCLK_DIV2) || \
                                                  ((__ADC_CLOCK__) == ADC_CLOCK_SYNC_PCLK_DIV4) || \
                                                  ((__ADC_CLOCK__) == ADC_CLOCK_SYNC_PCLK_DIV6) || \
                                                  ((__ADC_CLOCK__) == ADC_CLOCK_SYNC_PCLK_DIV8))
#define IS_ADC_SAMPLING_DELAY(__DELAY__) (((__DELAY__) == ADC_TWOSAMPLINGDELAY_5CYCLES)  || \
                                          ((__DELAY__) == ADC_TWOSAMPLINGDELAY_6CYCLES)  || \
                                          ((__DELAY__) == ADC_TWOSAMPLINGDELAY_7CYCLES)  || \
                                          ((__DELAY__) == ADC_TWOSAMPLINGDELAY_8CYCLES)  || \
                                          ((__DELAY__) == ADC_TWOSAMPLINGDELAY_9CYCLES)  || \
                                          ((__DELAY__) == ADC_TWOSAMPLINGDELAY_10CYCLES) || \
                                          ((__DELAY__) == ADC_TWOSAMPLINGDELAY_11CYCLES) || \
                                          ((__DELAY__) == ADC_TWOSAMPLINGDELAY_12CYCLES) || \
                                          ((__DELAY__) == ADC_TWOSAMPLINGDELAY_13CYCLES) || \
                                          ((__DELAY__) == ADC_TWOSAMPLINGDELAY_14CYCLES) || \
                                          ((__DELAY__) == ADC_TWOSAMPLINGDELAY_15CYCLES) || \
                                          ((__DELAY__) == ADC_TWOSAMPLINGDELAY_16CYCLES) || \
                                          ((__DELAY__) == ADC_TWOSAMPLINGDELAY_17CYCLES) || \
                                          ((__DELAY__) == ADC_TWOSAMPLINGDELAY_18CYCLES) || \
                                          ((__DELAY__) == ADC_TWOSAMPLINGDELAY_19CYCLES) || \
                                          ((__DELAY__) == ADC_TWOSAMPLINGDELAY_20CYCLES))
#define IS_ADC_RESOLUTION(__RESOLUTION__) (((__RESOLUTION__) == ADC_RESOLUTION_12B) || \
                                           ((__RESOLUTION__) == ADC_RESOLUTION_10B) || \
                                           ((__RESOLUTION__) == ADC_RESOLUTION_8B)  || \
                                           ((__RESOLUTION__) == ADC_RESOLUTION_6B))
#define IS_ADC_EXT_TRIG_EDGE(__EDGE__) (((__EDGE__) == ADC_EXTERNALTRIGCONVEDGE_NONE)    || \
                                        ((__EDGE__) == ADC_EXTERNALTRIGCONVEDGE_RISING)  || \
                                        ((__EDGE__) == ADC_EXTERNALTRIGCONVEDGE_FALLING) || \
                                        ((__EDGE__) == ADC_EXTERNALTRIGCONVEDGE_RISINGFALLING))
#define IS_ADC_EXT_TRIG(__REGTRIG__) (((__REGTRIG__) == ADC_EXTERNALTRIGCONV_T1_CC1)   || \
                                      ((__REGTRIG__) == ADC_EXTERNALTRIGCONV_T1_CC2)   || \
                                      ((__REGTRIG__) == ADC_EXTERNALTRIGCONV_T1_CC3)   || \
                                      ((__REGTRIG__) == ADC_EXTERNALTRIGCONV_T2_CC2)   || \
                                      ((__REGTRIG__) == ADC_EXTERNALTRIGCONV_T5_TRGO)  || \
                                      ((__REGTRIG__) == ADC_EXTERNALTRIGCONV_T4_CC4)   || \
                                      ((__REGTRIG__) == ADC_EXTERNALTRIGCONV_T3_CC4) || \
                                      ((__REGTRIG__) == ADC_EXTERNALTRIGCONV_T8_TRGO)  || \
                                      ((__REGTRIG__) == ADC_EXTERNALTRIGCONV_T8_TRGO2) || \
                                      ((__REGTRIG__) == ADC_EXTERNALTRIGCONV_T1_TRGO)  || \
                                      ((__REGTRIG__) == ADC_EXTERNALTRIGCONV_T1_TRGO2) || \
                                      ((__REGTRIG__) == ADC_EXTERNALTRIGCONV_T2_TRGO)  || \
                                      ((__REGTRIG__) == ADC_EXTERNALTRIGCONV_T4_TRGO)  || \
                                      ((__REGTRIG__) == ADC_EXTERNALTRIGCONV_T6_TRGO)  || \
                                      ((__REGTRIG__) == ADC_EXTERNALTRIGCONV_EXT_IT11) || \
																			((__REGTRIG__) == ADC_SOFTWARE_START))
#define IS_ADC_DATA_ALIGN(__ALIGN__) (((__ALIGN__) == ADC_DATAALIGN_RIGHT) || \
                                      ((__ALIGN__) == ADC_DATAALIGN_LEFT))

#define IS_ADC_SAMPLE_TIME(__TIME__) (((__TIME__) == ADC_SAMPLETIME_3CYCLES)   || \
                                      ((__TIME__) == ADC_SAMPLETIME_15CYCLES)  || \
                                      ((__TIME__) == ADC_SAMPLETIME_28CYCLES)  || \
                                      ((__TIME__) == ADC_SAMPLETIME_56CYCLES)  || \
                                      ((__TIME__) == ADC_SAMPLETIME_84CYCLES)  || \
                                      ((__TIME__) == ADC_SAMPLETIME_112CYCLES) || \
                                      ((__TIME__) == ADC_SAMPLETIME_144CYCLES) || \
                                      ((__TIME__) == ADC_SAMPLETIME_480CYCLES))
#define IS_ADC_EOCSelection(__EOCSelection__) (((__EOCSelection__) == ADC_EOC_SINGLE_CONV)   || \
                                               ((__EOCSelection__) == ADC_EOC_SEQ_CONV)  || \
                                               ((__EOCSelection__) == ADC_EOC_SINGLE_SEQ_CONV))
#define IS_ADC_EVENT_TYPE(__EVENT__) (((__EVENT__) == ADC_AWD_EVENT) || \
                                      ((__EVENT__) == ADC_OVR_EVENT))
#define IS_ADC_ANALOG_WATCHDOG(__WATCHDOG__) (((__WATCHDOG__) == ADC_ANALOGWATCHDOG_SINGLE_REG)        || \
                                              ((__WATCHDOG__) == ADC_ANALOGWATCHDOG_SINGLE_INJEC)      || \
                                              ((__WATCHDOG__) == ADC_ANALOGWATCHDOG_SINGLE_REGINJEC)   || \
                                              ((__WATCHDOG__) == ADC_ANALOGWATCHDOG_ALL_REG)           || \
                                              ((__WATCHDOG__) == ADC_ANALOGWATCHDOG_ALL_INJEC)         || \
                                              ((__WATCHDOG__) == ADC_ANALOGWATCHDOG_ALL_REGINJEC)      || \
                                              ((__WATCHDOG__) == ADC_ANALOGWATCHDOG_NONE))
#define IS_ADC_CHANNELS_TYPE(CHANNEL_TYPE) (((CHANNEL_TYPE) == ADC_ALL_CHANNELS) || \
                                            ((CHANNEL_TYPE) == ADC_REGULAR_CHANNELS) || \
                                            ((CHANNEL_TYPE) == ADC_INJECTED_CHANNELS))
#define IS_ADC_THRESHOLD(__THRESHOLD__) ((__THRESHOLD__) <= ((uint32_t)0xFFF))
#define IS_ADC_REGULAR_LENGTH(__LENGTH__) (((__LENGTH__) >= ((uint32_t)1)) && ((__LENGTH__) <= ((uint32_t)16)))
#define IS_ADC_REGULAR_RANK(__RANK__) (((__RANK__) >= ((uint32_t)1)) && ((__RANK__) <= ((uint32_t)16)))
#define IS_ADC_REGULAR_DISC_NUMBER(__NUMBER__) (((__NUMBER__) >= ((uint32_t)1)) && ((__NUMBER__) <= ((uint32_t)8)))
#define IS_ADC_RANGE(__RESOLUTION__, __ADC_VALUE__)                                     \
   ((((__RESOLUTION__) == ADC_RESOLUTION_12B) && ((__ADC_VALUE__) <= ((uint32_t)0x0FFF))) || \
    (((__RESOLUTION__) == ADC_RESOLUTION_10B) && ((__ADC_VALUE__) <= ((uint32_t)0x03FF))) || \
    (((__RESOLUTION__) == ADC_RESOLUTION_8B)  && ((__ADC_VALUE__) <= ((uint32_t)0x00FF))) || \
    (((__RESOLUTION__) == ADC_RESOLUTION_6B)  && ((__ADC_VALUE__) <= ((uint32_t)0x003F))))

/**
  * @brief  Set ADC Regular channel sequence length.
  * @param  _NbrOfConversion_: Regular channel sequence length.
  * @retval None
  */
#define ADC_SQR1(_NbrOfConversion_) (((_NbrOfConversion_) - (uint8_t)1) << 20)

/**
  * @brief  Set the ADC's sample time for channel numbers between 10 and 18.
  * @param  _SAMPLETIME_: Sample time parameter.
  * @param  _CHANNELNB_: Channel number.
  * @retval None
  */
#define ADC_SMPR1(_SAMPLETIME_, _CHANNELNB_) ((_SAMPLETIME_) << (3 * (((uint32_t)((uint16_t)(_CHANNELNB_))) - 10)))

/**
  * @brief  Set the ADC's sample time for channel numbers between 0 and 9.
  * @param  _SAMPLETIME_: Sample time parameter.
  * @param  _CHANNELNB_: Channel number.
  * @retval None
  */
#define ADC_SMPR2(_SAMPLETIME_, _CHANNELNB_) ((_SAMPLETIME_) << (3 * ((uint32_t)((uint16_t)(_CHANNELNB_)))))

/**
  * @brief  Set the selected regular channel rank for rank between 1 and 6.
  * @param  _CHANNELNB_: Channel number.
  * @param  _RANKNB_: Rank number.
  * @retval None
  */
#define ADC_SQR3_RK(_CHANNELNB_, _RANKNB_) (((uint32_t)((uint16_t)(_CHANNELNB_))) << (5 * ((_RANKNB_) - 1)))

/**
  * @brief  Set the selected regular channel rank for rank between 7 and 12.
  * @param  _CHANNELNB_: Channel number.
  * @param  _RANKNB_: Rank number.
  * @retval None
  */
#define ADC_SQR2_RK(_CHANNELNB_, _RANKNB_) (((uint32_t)((uint16_t)(_CHANNELNB_))) << (5 * ((_RANKNB_) - 7)))

/**
  * @brief  Set the selected regular channel rank for rank between 13 and 16.
  * @param  _CHANNELNB_: Channel number.
  * @param  _RANKNB_: Rank number.
  * @retval None
  */
#define ADC_SQR1_RK(_CHANNELNB_, _RANKNB_) (((uint32_t)((uint16_t)(_CHANNELNB_))) << (5 * ((_RANKNB_) - 13)))

/**
  * @brief  Enable ADC continuous conversion mode.
  * @param  _CONTINUOUS_MODE_: Continuous mode.
  * @retval None
  */
#define ADC_CR2_CONTINUOUS(_CONTINUOUS_MODE_) ((_CONTINUOUS_MODE_) << 1)

/**
  * @brief  Configures the number of discontinuous conversions for the regular group channels.
  * @param  _NBR_DISCONTINUOUSCONV_: Number of discontinuous conversions.
  * @retval None
  */
#define ADC_CR1_DISCONTINUOUS(_NBR_DISCONTINUOUSCONV_) (((_NBR_DISCONTINUOUSCONV_) - 1) << POSITION_VAL(ADC_CR1_DISCNUM))

/**
  * @brief  Enable ADC scan mode.
  * @param  _SCANCONV_MODE_: Scan conversion mode.
  * @retval None
  */
#define ADC_CR1_SCANCONV(_SCANCONV_MODE_) ((_SCANCONV_MODE_) << 8)

/**
  * @brief  Enable the ADC end of conversion selection.
  * @param  _EOCSelection_MODE_: End of conversion selection mode.
  * @retval None
  */
#define ADC_CR2_EOCSelection(_EOCSelection_MODE_) ((_EOCSelection_MODE_) << 10)

/**
  * @brief  Enable the ADC DMA continuous request.
  * @param  _DMAContReq_MODE_: DMA continuous request mode.
  * @retval None
  */
#define ADC_CR2_DMAContReq(_DMAContReq_MODE_) ((_DMAContReq_MODE_) << 9)

/**
  * @brief Return resolution bits in CR1 register.
  * @param __HANDLE__: ADC handle
  * @retval None
  */
#define ADC_GET_RESOLUTION(__HANDLE__) (((__HANDLE__)->Instance->CR1) & ADC_CR1_RES)

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup ADC_Private_Functions ADC Private Functions
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

#endif /*__STM32F7xx_ADC_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
