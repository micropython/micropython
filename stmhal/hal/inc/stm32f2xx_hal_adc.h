/**
  ******************************************************************************
  * @file    stm32f2xx_hal_adc.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    25-March-2014
  * @brief   Header file of ADC HAL extension module.
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
#ifndef __STM32F2xx_ADC_H
#define __STM32F2xx_ADC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx_hal_def.h"

/** @addtogroup STM32F2xx_HAL_Driver
  * @{
  */

/** @addtogroup ADC
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/
   
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
  uint32_t ExternalTrigConvEdge;  /*!< Select the external trigger edge and enable the trigger of a regular group. 
                                       This parameter can be a value of @ref ADC_External_trigger_edge_Regular */
  uint32_t ExternalTrigConv;      /*!< Select the external event used to trigger the start of conversion of a regular group.
                                       This parameter can be a value of @ref ADC_External_trigger_Source_Regular */ 
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
  uint32_t Channel;        /*!< The ADC channel to configure 
                                This parameter can be a value of @ref ADC_channels */
  uint32_t Rank;           /*!< The rank in the regular group sequencer 
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
                                   This parameter can be a value of @ref ADC_analog_watchdog_selection. */
  uint32_t HighThreshold;     /*!< Configures the ADC analog watchdog High threshold value.
                                   This parameter must be a 12-bit value. */     
  uint32_t LowThreshold;      /*!< Configures the ADC analog watchdog High threshold value.
                                   This parameter must be a 12-bit value. */
  uint32_t Channel;           /*!< Configures ADC channel for the analog watchdog. 
                                   This parameter has an effect only if watchdog mode is configured on single channel 
                                   This parameter can be a value of @ref ADC_channels. */      
  uint32_t ITMode;            /*!< Specifies whether the analog watchdog is configured
                                   is interrupt mode or in polling mode.
                                   This parameter can be set to ENABLE or DISABLE */
  uint32_t WatchdogNumber;    /*!< Reserved for future use, can be set to 0 */
}ADC_AnalogWDGConfTypeDef;

/* Exported constants --------------------------------------------------------*/

/** @defgroup ADC_Exported_Constants
  * @{
  */


/** @defgroup ADC_Error_Code 
  * @{
  */ 

#define HAL_ADC_ERROR_NONE        ((uint32_t)0x00)   /*!< No error             */
#define HAL_ADC_ERROR_OVR         ((uint32_t)0x01)   /*!< OVR error            */
#define HAL_ADC_ERROR_DMA         ((uint32_t)0x02)   /*!< DMA transfer error   */
/**
  * @}
  */  


/** @defgroup ADC_ClockPrescaler 
  * @{
  */ 
#define ADC_CLOCKPRESCALER_PCLK_DIV2    ((uint32_t)0x00000000)
#define ADC_CLOCKPRESCALER_PCLK_DIV4    ((uint32_t)ADC_CCR_ADCPRE_0)
#define ADC_CLOCKPRESCALER_PCLK_DIV6    ((uint32_t)ADC_CCR_ADCPRE_1)
#define ADC_CLOCKPRESCALER_PCLK_DIV8    ((uint32_t)ADC_CCR_ADCPRE)
#define IS_ADC_CLOCKPRESCALER(ADC_CLOCK)     (((ADC_CLOCK) == ADC_CLOCKPRESCALER_PCLK_DIV2) || \
                                              ((ADC_CLOCK) == ADC_CLOCKPRESCALER_PCLK_DIV4) || \
                                              ((ADC_CLOCK) == ADC_CLOCKPRESCALER_PCLK_DIV6) || \
                                              ((ADC_CLOCK) == ADC_CLOCKPRESCALER_PCLK_DIV8))
/**
  * @}
  */ 

/** @defgroup ADC_delay_between_2_sampling_phases 
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

#define IS_ADC_SAMPLING_DELAY(DELAY) (((DELAY) == ADC_TWOSAMPLINGDELAY_5CYCLES)  || \
                                      ((DELAY) == ADC_TWOSAMPLINGDELAY_6CYCLES)  || \
                                      ((DELAY) == ADC_TWOSAMPLINGDELAY_7CYCLES)  || \
                                      ((DELAY) == ADC_TWOSAMPLINGDELAY_8CYCLES)  || \
                                      ((DELAY) == ADC_TWOSAMPLINGDELAY_9CYCLES)  || \
                                      ((DELAY) == ADC_TWOSAMPLINGDELAY_10CYCLES) || \
                                      ((DELAY) == ADC_TWOSAMPLINGDELAY_11CYCLES) || \
                                      ((DELAY) == ADC_TWOSAMPLINGDELAY_12CYCLES) || \
                                      ((DELAY) == ADC_TWOSAMPLINGDELAY_13CYCLES) || \
                                      ((DELAY) == ADC_TWOSAMPLINGDELAY_14CYCLES) || \
                                      ((DELAY) == ADC_TWOSAMPLINGDELAY_15CYCLES) || \
                                      ((DELAY) == ADC_TWOSAMPLINGDELAY_16CYCLES) || \
                                      ((DELAY) == ADC_TWOSAMPLINGDELAY_17CYCLES) || \
                                      ((DELAY) == ADC_TWOSAMPLINGDELAY_18CYCLES) || \
                                      ((DELAY) == ADC_TWOSAMPLINGDELAY_19CYCLES) || \
                                      ((DELAY) == ADC_TWOSAMPLINGDELAY_20CYCLES))
/**
  * @}
  */ 

/** @defgroup ADC_Resolution 
  * @{
  */ 
#define ADC_RESOLUTION12b  ((uint32_t)0x00000000)
#define ADC_RESOLUTION10b  ((uint32_t)ADC_CR1_RES_0)
#define ADC_RESOLUTION8b   ((uint32_t)ADC_CR1_RES_1)
#define ADC_RESOLUTION6b   ((uint32_t)ADC_CR1_RES)

#define IS_ADC_RESOLUTION(RESOLUTION) (((RESOLUTION) == ADC_RESOLUTION12b) || \
                                       ((RESOLUTION) == ADC_RESOLUTION10b) || \
                                       ((RESOLUTION) == ADC_RESOLUTION8b)  || \
                                       ((RESOLUTION) == ADC_RESOLUTION6b))
/**
  * @}
  */ 

/** @defgroup ADC_External_trigger_edge_Regular 
  * @{
  */ 
#define ADC_EXTERNALTRIGCONVEDGE_NONE           ((uint32_t)0x00000000)
#define ADC_EXTERNALTRIGCONVEDGE_RISING         ((uint32_t)ADC_CR2_EXTEN_0)
#define ADC_EXTERNALTRIGCONVEDGE_FALLING        ((uint32_t)ADC_CR2_EXTEN_1)
#define ADC_EXTERNALTRIGCONVEDGE_RISINGFALLING  ((uint32_t)ADC_CR2_EXTEN)

#define IS_ADC_EXT_TRIG_EDGE(EDGE) (((EDGE) == ADC_EXTERNALTRIGCONVEDGE_NONE)    || \
                                    ((EDGE) == ADC_EXTERNALTRIGCONVEDGE_RISING)  || \
                                    ((EDGE) == ADC_EXTERNALTRIGCONVEDGE_FALLING) || \
                                    ((EDGE) == ADC_EXTERNALTRIGCONVEDGE_RISINGFALLING))
/**
  * @}
  */ 

/** @defgroup ADC_External_trigger_Source_Regular 
  * @{
  */ 
#define ADC_EXTERNALTRIGCONV_T1_CC1    ((uint32_t)0x00000000)
#define ADC_EXTERNALTRIGCONV_T1_CC2    ((uint32_t)ADC_CR2_EXTSEL_0)
#define ADC_EXTERNALTRIGCONV_T1_CC3    ((uint32_t)ADC_CR2_EXTSEL_1)
#define ADC_EXTERNALTRIGCONV_T2_CC2    ((uint32_t)(ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_0))
#define ADC_EXTERNALTRIGCONV_T2_CC3    ((uint32_t)ADC_CR2_EXTSEL_2)
#define ADC_EXTERNALTRIGCONV_T2_CC4    ((uint32_t)(ADC_CR2_EXTSEL_2 | ADC_CR2_EXTSEL_0))
#define ADC_EXTERNALTRIGCONV_T2_TRGO   ((uint32_t)(ADC_CR2_EXTSEL_2 | ADC_CR2_EXTSEL_1))
#define ADC_EXTERNALTRIGCONV_T3_CC1    ((uint32_t)(ADC_CR2_EXTSEL_2 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_0))
#define ADC_EXTERNALTRIGCONV_T3_TRGO   ((uint32_t)ADC_CR2_EXTSEL_3)
#define ADC_EXTERNALTRIGCONV_T4_CC4    ((uint32_t)(ADC_CR2_EXTSEL_3 | ADC_CR2_EXTSEL_0))
#define ADC_EXTERNALTRIGCONV_T5_CC1    ((uint32_t)(ADC_CR2_EXTSEL_3 | ADC_CR2_EXTSEL_1))
#define ADC_EXTERNALTRIGCONV_T5_CC2    ((uint32_t)(ADC_CR2_EXTSEL_3 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_0))
#define ADC_EXTERNALTRIGCONV_T5_CC3    ((uint32_t)(ADC_CR2_EXTSEL_3 | ADC_CR2_EXTSEL_2))
#define ADC_EXTERNALTRIGCONV_T8_CC1    ((uint32_t)(ADC_CR2_EXTSEL_3 | ADC_CR2_EXTSEL_2 | ADC_CR2_EXTSEL_0))
#define ADC_EXTERNALTRIGCONV_T8_TRGO   ((uint32_t)(ADC_CR2_EXTSEL_3 | ADC_CR2_EXTSEL_2 | ADC_CR2_EXTSEL_1))
#define ADC_EXTERNALTRIGCONV_Ext_IT11  ((uint32_t)ADC_CR2_EXTSEL)

#define IS_ADC_EXT_TRIG(REGTRIG) (((REGTRIG) == ADC_EXTERNALTRIGCONV_T1_CC1)  || \
                                  ((REGTRIG) == ADC_EXTERNALTRIGCONV_T1_CC2)  || \
                                  ((REGTRIG) == ADC_EXTERNALTRIGCONV_T1_CC3)  || \
                                  ((REGTRIG) == ADC_EXTERNALTRIGCONV_T2_CC2)  || \
                                  ((REGTRIG) == ADC_EXTERNALTRIGCONV_T2_CC3)  || \
                                  ((REGTRIG) == ADC_EXTERNALTRIGCONV_T2_CC4)  || \
                                  ((REGTRIG) == ADC_EXTERNALTRIGCONV_T2_TRGO) || \
                                  ((REGTRIG) == ADC_EXTERNALTRIGCONV_T3_CC1)  || \
                                  ((REGTRIG) == ADC_EXTERNALTRIGCONV_T3_TRGO) || \
                                  ((REGTRIG) == ADC_EXTERNALTRIGCONV_T4_CC4)  || \
                                  ((REGTRIG) == ADC_EXTERNALTRIGCONV_T5_CC1)  || \
                                  ((REGTRIG) == ADC_EXTERNALTRIGCONV_T5_CC2)  || \
                                  ((REGTRIG) == ADC_EXTERNALTRIGCONV_T5_CC3)  || \
                                  ((REGTRIG) == ADC_EXTERNALTRIGCONV_T8_CC1)  || \
                                  ((REGTRIG) == ADC_EXTERNALTRIGCONV_T8_TRGO) || \
                                  ((REGTRIG) == ADC_EXTERNALTRIGCONV_Ext_IT11))
/**
  * @}
  */ 

/** @defgroup ADC_data_align 
  * @{
  */ 
#define ADC_DATAALIGN_RIGHT      ((uint32_t)0x00000000)
#define ADC_DATAALIGN_LEFT       ((uint32_t)ADC_CR2_ALIGN)

#define IS_ADC_DATA_ALIGN(ALIGN) (((ALIGN) == ADC_DATAALIGN_RIGHT) || \
                                  ((ALIGN) == ADC_DATAALIGN_LEFT))
/**
  * @}
  */ 

/** @defgroup ADC_channels 
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

#define ADC_CHANNEL_TEMPSENSOR  ((uint32_t)ADC_CHANNEL_16)
#define ADC_CHANNEL_VREFINT     ((uint32_t)ADC_CHANNEL_17)
#define ADC_CHANNEL_VBAT        ((uint32_t)ADC_CHANNEL_18)

#define IS_ADC_CHANNEL(CHANNEL) (((CHANNEL) == ADC_CHANNEL_0)  || \
                                 ((CHANNEL) == ADC_CHANNEL_1)  || \
                                 ((CHANNEL) == ADC_CHANNEL_2)  || \
                                 ((CHANNEL) == ADC_CHANNEL_3)  || \
                                 ((CHANNEL) == ADC_CHANNEL_4)  || \
                                 ((CHANNEL) == ADC_CHANNEL_5)  || \
                                 ((CHANNEL) == ADC_CHANNEL_6)  || \
                                 ((CHANNEL) == ADC_CHANNEL_7)  || \
                                 ((CHANNEL) == ADC_CHANNEL_8)  || \
                                 ((CHANNEL) == ADC_CHANNEL_9)  || \
                                 ((CHANNEL) == ADC_CHANNEL_10) || \
                                 ((CHANNEL) == ADC_CHANNEL_11) || \
                                 ((CHANNEL) == ADC_CHANNEL_12) || \
                                 ((CHANNEL) == ADC_CHANNEL_13) || \
                                 ((CHANNEL) == ADC_CHANNEL_14) || \
                                 ((CHANNEL) == ADC_CHANNEL_15) || \
                                 ((CHANNEL) == ADC_CHANNEL_16) || \
                                 ((CHANNEL) == ADC_CHANNEL_17) || \
                                 ((CHANNEL) == ADC_CHANNEL_18))
/**
  * @}
  */ 

/** @defgroup ADC_sampling_times 
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

#define IS_ADC_SAMPLE_TIME(TIME) (((TIME) == ADC_SAMPLETIME_3CYCLES)   || \
                                  ((TIME) == ADC_SAMPLETIME_15CYCLES)  || \
                                  ((TIME) == ADC_SAMPLETIME_28CYCLES)  || \
                                  ((TIME) == ADC_SAMPLETIME_56CYCLES)  || \
                                  ((TIME) == ADC_SAMPLETIME_84CYCLES)  || \
                                  ((TIME) == ADC_SAMPLETIME_112CYCLES) || \
                                  ((TIME) == ADC_SAMPLETIME_144CYCLES) || \
                                  ((TIME) == ADC_SAMPLETIME_480CYCLES))
/**
  * @}
  */ 

  /** @defgroup ADC_EOCSelection 
  * @{
  */ 
#define EOC_SEQ_CONV              ((uint32_t)0x00000000)
#define EOC_SINGLE_CONV           ((uint32_t)0x00000001)
#define EOC_SINGLE_SEQ_CONV       ((uint32_t)0x00000002)  /*!< reserved for future use */

#define IS_ADC_EOCSelection(EOCSelection) (((EOCSelection) == EOC_SINGLE_CONV)   || \
                                           ((EOCSelection) == EOC_SEQ_CONV)  || \
                                           ((EOCSelection) == EOC_SINGLE_SEQ_CONV))
/**
  * @}
  */ 

/** @defgroup ADC_Event_type 
  * @{
  */ 
#define AWD_EVENT             ((uint32_t)ADC_FLAG_AWD)
#define OVR_EVENT             ((uint32_t)ADC_FLAG_OVR)

#define IS_ADC_EVENT_TYPE(EVENT) (((EVENT) == AWD_EVENT) || \
                                  ((EVENT) == OVR_EVENT))
/**
  * @}
  */

/** @defgroup ADC_analog_watchdog_selection 
  * @{
  */ 
#define ADC_ANALOGWATCHDOG_SINGLE_REG         ((uint32_t)(ADC_CR1_AWDSGL | ADC_CR1_AWDEN))
#define ADC_ANALOGWATCHDOG_SINGLE_INJEC       ((uint32_t)(ADC_CR1_AWDSGL | ADC_CR1_JAWDEN))
#define ADC_ANALOGWATCHDOG_SINGLE_REGINJEC    ((uint32_t)(ADC_CR1_AWDSGL | ADC_CR1_AWDEN | ADC_CR1_JAWDEN))
#define ADC_ANALOGWATCHDOG_ALL_REG            ((uint32_t)ADC_CR1_AWDEN)
#define ADC_ANALOGWATCHDOG_ALL_INJEC          ((uint32_t)ADC_CR1_JAWDEN)
#define ADC_ANALOGWATCHDOG_ALL_REGINJEC       ((uint32_t)(ADC_CR1_AWDEN | ADC_CR1_JAWDEN))
#define ADC_ANALOGWATCHDOG_NONE               ((uint32_t)0x00000000)

#define IS_ADC_ANALOG_WATCHDOG(WATCHDOG) (((WATCHDOG) == ADC_ANALOGWATCHDOG_SINGLE_REG)        || \
                                          ((WATCHDOG) == ADC_ANALOGWATCHDOG_SINGLE_INJEC)      || \
                                          ((WATCHDOG) == ADC_ANALOGWATCHDOG_SINGLE_REGINJEC)   || \
                                          ((WATCHDOG) == ADC_ANALOGWATCHDOG_ALL_REG)           || \
                                          ((WATCHDOG) == ADC_ANALOGWATCHDOG_ALL_INJEC)         || \
                                          ((WATCHDOG) == ADC_ANALOGWATCHDOG_ALL_REGINJEC)      || \
                                          ((WATCHDOG) == ADC_ANALOGWATCHDOG_NONE))
/**
  * @}
  */ 
    
/** @defgroup ADC_interrupts_definition 
  * @{
  */ 
#define ADC_IT_EOC      ((uint32_t)ADC_CR1_EOCIE)  
#define ADC_IT_AWD      ((uint32_t)ADC_CR1_AWDIE) 
#define ADC_IT_JEOC     ((uint32_t)ADC_CR1_JEOCIE)
#define ADC_IT_OVR      ((uint32_t)ADC_CR1_OVRIE) 

#define IS_ADC_IT(IT) (((IT) == ADC_IT_EOC) || ((IT) == ADC_IT_AWD) || \
                       ((IT) == ADC_IT_JEOC)|| ((IT) == ADC_IT_OVR)) 
/**
  * @}
  */ 
    
/** @defgroup ADC_flags_definition 
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

/** @defgroup ADC_channels_type 
  * @{
  */ 
#define ALL_CHANNELS      ((uint32_t)0x00000001)
#define REGULAR_CHANNELS  ((uint32_t)0x00000002) /*!< reserved for future use */
#define INJECTED_CHANNELS ((uint32_t)0x00000003) /*!< reserved for future use */

#define IS_ADC_CHANNELS_TYPE(CHANNEL_TYPE) (((CHANNEL_TYPE) == ALL_CHANNELS) || \
                                            ((CHANNEL_TYPE) == REGULAR_CHANNELS) || \
                                            ((CHANNEL_TYPE) == INJECTED_CHANNELS))
/**
  * @}
  */

/** @defgroup ADC_thresholds 
  * @{
  */ 
#define IS_ADC_THRESHOLD(THRESHOLD) ((THRESHOLD) <= ((uint32_t)0xFFF))
/**
  * @}
  */ 

/** @defgroup ADC_regular_length 
  * @{
  */ 
#define IS_ADC_REGULAR_LENGTH(LENGTH) (((LENGTH) >= ((uint32_t)1)) && ((LENGTH) <= ((uint32_t)16)))
/**
  * @}
  */ 

/** @defgroup ADC_regular_rank 
  * @{
  */ 
#define IS_ADC_REGULAR_RANK(RANK) (((RANK) >= ((uint32_t)1)) && ((RANK) <= ((uint32_t)16)))
/**
  * @}
  */ 

/** @defgroup ADC_regular_discontinuous_mode_number 
  * @{
  */ 
#define IS_ADC_REGULAR_DISC_NUMBER(NUMBER) (((NUMBER) >= ((uint32_t)1)) && ((NUMBER) <= ((uint32_t)8)))
/**
  * @}
  */ 

/** @defgroup ADC_range_verification
  * @{
  */ 
#define IS_ADC_RANGE(RESOLUTION, ADC_VALUE)                                     \
   ((((RESOLUTION) == ADC_RESOLUTION12b) && ((ADC_VALUE) <= ((uint32_t)0x0FFF))) || \
    (((RESOLUTION) == ADC_RESOLUTION10b) && ((ADC_VALUE) <= ((uint32_t)0x03FF))) || \
    (((RESOLUTION) == ADC_RESOLUTION8b)  && ((ADC_VALUE) <= ((uint32_t)0x00FF))) || \
    (((RESOLUTION) == ADC_RESOLUTION6b)  && ((ADC_VALUE) <= ((uint32_t)0x003F))))
/**
  * @}
  */   
  
/**
  * @}
  */ 

/* Exported macro ------------------------------------------------------------*/
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
  * @brief  Set ADC Regular channel sequence length.
  * @param  _NbrOfConversion_: Regular channel sequence length. 
  * @retval None
  */
#define __HAL_ADC_SQR1(_NbrOfConversion_) (((_NbrOfConversion_) - (uint8_t)1) << 20)

/**
  * @brief  Set the ADC's sample time for channel numbers between 10 and 18.
  * @param  _SAMPLETIME_: Sample time parameter.
  * @param  _CHANNELNB_: Channel number.  
  * @retval None
  */
#define __HAL_ADC_SMPR1(_SAMPLETIME_, _CHANNELNB_) ((_SAMPLETIME_) << (3 * ((_CHANNELNB_) - 10)))

/**
  * @brief  Set the ADC's sample time for channel numbers between 0 and 9.
  * @param  _SAMPLETIME_: Sample time parameter.
  * @param  _CHANNELNB_: Channel number.  
  * @retval None
  */
#define __HAL_ADC_SMPR2(_SAMPLETIME_, _CHANNELNB_) ((_SAMPLETIME_) << (3 * (_CHANNELNB_)))

/**
  * @brief  Set the selected regular channel rank for rank between 1 and 6.
  * @param  _CHANNELNB_: Channel number.
  * @param  _RANKNB_: Rank number.    
  * @retval None
  */
#define __HAL_ADC_SQR3_RK(_CHANNELNB_, _RANKNB_) ((_CHANNELNB_) << (5 * ((_RANKNB_) - 1)))

/**
  * @brief  Set the selected regular channel rank for rank between 7 and 12.
  * @param  _CHANNELNB_: Channel number.
  * @param  _RANKNB_: Rank number.    
  * @retval None
  */
#define __HAL_ADC_SQR2_RK(_CHANNELNB_, _RANKNB_) ((_CHANNELNB_) << (5 * ((_RANKNB_) - 7)))

/**
  * @brief  Set the selected regular channel rank for rank between 13 and 16.
  * @param  _CHANNELNB_: Channel number.
  * @param  _RANKNB_: Rank number.    
  * @retval None
  */
#define __HAL_ADC_SQR1_RK(_CHANNELNB_, _RANKNB_) ((_CHANNELNB_) << (5 * ((_RANKNB_) - 13)))

/**
  * @brief  Enable ADC continuous conversion mode.
  * @param  _CONTINUOUS_MODE_: Continuous mode.
  * @retval None
  */
#define __HAL_ADC_CR2_CONTINUOUS(_CONTINUOUS_MODE_) ((_CONTINUOUS_MODE_) << 1)

/**
  * @brief  Configures the number of discontinuous conversions for the regular group channels.
  * @param  _NBR_DISCONTINUOUSCONV_: Number of discontinuous conversions.
  * @retval None
  */
#define __HAL_ADC_CR1_DISCONTINUOUS(_NBR_DISCONTINUOUSCONV_) (((_NBR_DISCONTINUOUSCONV_) - 1) << 13)

/**
  * @brief  Enable ADC scan mode.
  * @param  _SCANCONV_MODE_: Scan conversion mode.
  * @retval None
  */
#define __HAL_ADC_CR1_SCANCONV(_SCANCONV_MODE_) ((_SCANCONV_MODE_) << 8)

/**
  * @brief  Enable the ADC end of conversion selection.
  * @param  _EOCSelection_MODE_: End of conversion selection mode.
  * @retval None
  */
#define __HAL_ADC_CR2_EOCSelection(_EOCSelection_MODE_) ((_EOCSelection_MODE_) << 10)

/**
  * @brief  Enable the ADC DMA continuous request.
  * @param  _DMAContReq_MODE_: DMA continuous request mode.
  * @retval None
  */
#define __HAL_ADC_CR2_DMAContReq(_DMAContReq_MODE_) ((_DMAContReq_MODE_) << 9)

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
#define __HAL_ADC_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((((__HANDLE__)->Instance->CR1 & (__INTERRUPT__)) == (__INTERRUPT__)) ? SET : RESET)

/**
  * @brief  Clear the ADC's pending flags.
  * @param  __HANDLE__: specifies the ADC Handle.
  * @param  __FLAG__: ADC flag.
  * @retval None
  */
#define __HAL_ADC_CLEAR_FLAG(__HANDLE__, __FLAG__) (((__HANDLE__)->Instance->SR) &= ~(__FLAG__))

/**
  * @brief  Get the selected ADC's flag status.
  * @param  __HANDLE__: specifies the ADC Handle.
  * @param  __FLAG__: ADC flag.
  * @retval None
  */
#define __HAL_ADC_GET_FLAG(__HANDLE__, __FLAG__) ((((__HANDLE__)->Instance->SR) & (__FLAG__)) == (__FLAG__))

/**
  * @brief Return resolution bits in CR1 register.
  * @param __HANDLE__: ADC handle
  * @retval None
  */
#define __HAL_ADC_GET_RESOLUTION(__HANDLE__) (((__HANDLE__)->Instance->CR1) & ADC_CR1_RES)

/* Include ADC HAL Extension module */
#include "stm32f2xx_hal_adc_ex.h"

/* Exported functions --------------------------------------------------------*/
/* Initialization/de-initialization functions ***********************************/
HAL_StatusTypeDef HAL_ADC_Init(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef HAL_ADC_DeInit(ADC_HandleTypeDef *hadc);
void       HAL_ADC_MspInit(ADC_HandleTypeDef* hadc);
void       HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc);

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

/* Peripheral Control functions *************************************************/
HAL_StatusTypeDef HAL_ADC_ConfigChannel(ADC_HandleTypeDef* hadc, ADC_ChannelConfTypeDef* sConfig);
HAL_StatusTypeDef HAL_ADC_AnalogWDGConfig(ADC_HandleTypeDef* hadc, ADC_AnalogWDGConfTypeDef* AnalogWDGConfig);

/* Peripheral State functions ***************************************************/
HAL_ADC_StateTypeDef HAL_ADC_GetState(ADC_HandleTypeDef* hadc);
uint32_t             HAL_ADC_GetError(ADC_HandleTypeDef *hadc);

/**
  * @}
  */ 

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /*__STM32F2xx_ADC_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
