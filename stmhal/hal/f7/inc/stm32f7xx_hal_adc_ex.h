/**
  ******************************************************************************
  * @file    stm32f7xx_hal_adc.h
  * @author  MCD Application Team
  * @version V1.1.2
  * @date    23-September-2016
  * @brief   Header file of ADC HAL module.
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
#ifndef __STM32F7xx_ADC_EX_H
#define __STM32F7xx_ADC_EX_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal_def.h"

/** @addtogroup STM32F7xx_HAL_Driver
  * @{
  */

/** @addtogroup ADCEx
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup ADCEx_Exported_Types ADC Exported Types
  * @{
  */

/**
  * @brief  ADC Configuration injected Channel structure definition
  * @note   Parameters of this structure are shared within 2 scopes:
  *          - Scope channel: InjectedChannel, InjectedRank, InjectedSamplingTime, InjectedOffset
  *          - Scope injected group (affects all channels of injected group): InjectedNbrOfConversion, InjectedDiscontinuousConvMode,
  *            AutoInjectedConv, ExternalTrigInjecConvEdge, ExternalTrigInjecConv.
  * @note   The setting of these parameters with function HAL_ADCEx_InjectedConfigChannel() is conditioned to ADC state.
  *         ADC state can be either:
  *          - For all parameters: ADC disabled
  *          - For all except parameters 'InjectedDiscontinuousConvMode' and 'AutoInjectedConv': ADC enabled without conversion on going on injected group.
  *          - For parameters 'ExternalTrigInjecConv' and 'ExternalTrigInjecConvEdge': ADC enabled, even with conversion on going on injected group.
  */
typedef struct
{
  uint32_t InjectedChannel;               /*!< Selection of ADC channel to configure
                                               This parameter can be a value of @ref ADC_channels
                                               Note: Depending on devices, some channels may not be available on package pins. Refer to device datasheet for channels availability. */
  uint32_t InjectedRank;                  /*!< Rank in the injected group sequencer
                                               This parameter must be a value of @ref ADCEx_injected_rank
                                               Note: In case of need to disable a channel or change order of conversion sequencer, rank containing a previous channel setting can be overwritten by the new channel setting (or parameter number of conversions can be adjusted) */
  uint32_t InjectedSamplingTime;          /*!< Sampling time value to be set for the selected channel.
                                               Unit: ADC clock cycles
                                               Conversion time is the addition of sampling time and processing time (12 ADC clock cycles at ADC resolution 12 bits, 11 cycles at 10 bits, 9 cycles at 8 bits, 7 cycles at 6 bits).
                                               This parameter can be a value of @ref ADC_sampling_times
                                               Caution: This parameter updates the parameter property of the channel, that can be used into regular and/or injected groups.
                                                        If this same channel has been previously configured in the other group (regular/injected), it will be updated to last setting.
                                               Note: In case of usage of internal measurement channels (VrefInt/Vbat/TempSensor),
                                                     sampling time constraints must be respected (sampling time can be adjusted in function of ADC clock frequency and sampling time setting)
                                                     Refer to device datasheet for timings values, parameters TS_vrefint, TS_temp (values rough order: 4us min). */
  uint32_t InjectedOffset;                /*!< Defines the offset to be subtracted from the raw converted data (for channels set on injected group only).
                                               Offset value must be a positive number.
                                               Depending of ADC resolution selected (12, 10, 8 or 6 bits),
                                               this parameter must be a number between Min_Data = 0x000 and Max_Data = 0xFFF, 0x3FF, 0xFF or 0x3F respectively. */
  uint32_t InjectedNbrOfConversion;       /*!< Specifies the number of ranks that will be converted within the injected group sequencer.
                                               To use the injected group sequencer and convert several ranks, parameter 'ScanConvMode' must be enabled.
                                               This parameter must be a number between Min_Data = 1 and Max_Data = 4.
                                               Caution: this setting impacts the entire injected group. Therefore, call of HAL_ADCEx_InjectedConfigChannel() to
                                                        configure a channel on injected group can impact the configuration of other channels previously set. */
  uint32_t InjectedDiscontinuousConvMode; /*!< Specifies whether the conversions sequence of injected group is performed in Complete-sequence/Discontinuous-sequence (main sequence subdivided in successive parts).
                                               Discontinuous mode is used only if sequencer is enabled (parameter 'ScanConvMode'). If sequencer is disabled, this parameter is discarded.
                                               Discontinuous mode can be enabled only if continuous mode is disabled. If continuous mode is enabled, this parameter setting is discarded.
                                               This parameter can be set to ENABLE or DISABLE.
                                               Note: For injected group, number of discontinuous ranks increment is fixed to one-by-one.
                                               Caution: this setting impacts the entire injected group. Therefore, call of HAL_ADCEx_InjectedConfigChannel() to
                                                        configure a channel on injected group can impact the configuration of other channels previously set. */
  uint32_t AutoInjectedConv;              /*!< Enables or disables the selected ADC automatic injected group conversion after regular one
                                               This parameter can be set to ENABLE or DISABLE.
                                               Note: To use Automatic injected conversion, discontinuous mode must be disabled ('DiscontinuousConvMode' and 'InjectedDiscontinuousConvMode' set to DISABLE)
                                               Note: To use Automatic injected conversion, injected group external triggers must be disabled ('ExternalTrigInjecConv' set to ADC_SOFTWARE_START)
                                               Note: In case of DMA used with regular group: if DMA configured in normal mode (single shot) JAUTO will be stopped upon DMA transfer complete.
                                                     To maintain JAUTO always enabled, DMA must be configured in circular mode.
                                               Caution: this setting impacts the entire injected group. Therefore, call of HAL_ADCEx_InjectedConfigChannel() to
                                                        configure a channel on injected group can impact the configuration of other channels previously set. */
  uint32_t ExternalTrigInjecConv;         /*!< Selects the external event used to trigger the conversion start of injected group.
                                               If set to ADC_INJECTED_SOFTWARE_START, external triggers are disabled.
                                               If set to external trigger source, triggering is on event rising edge.
                                               This parameter can be a value of @ref ADCEx_External_trigger_Source_Injected
                                               Note: This parameter must be modified when ADC is disabled (before ADC start conversion or after ADC stop conversion).
                                                     If ADC is enabled, this parameter setting is bypassed without error reporting (as it can be the expected behaviour in case of another parameter update on the fly)
                                               Caution: this setting impacts the entire injected group. Therefore, call of HAL_ADCEx_InjectedConfigChannel() to
                                                        configure a channel on injected group can impact the configuration of other channels previously set. */
  uint32_t ExternalTrigInjecConvEdge;     /*!< Selects the external trigger edge of injected group.
                                               This parameter can be a value of @ref ADCEx_External_trigger_edge_Injected.
                                               If trigger is set to ADC_INJECTED_SOFTWARE_START, this parameter is discarded.
                                               Caution: this setting impacts the entire injected group. Therefore, call of HAL_ADCEx_InjectedConfigChannel() to
                                                        configure a channel on injected group can impact the configuration of other channels previously set. */
}ADC_InjectionConfTypeDef;
/**
  * @}
  */

/**
  * @brief ADC Configuration multi-mode structure definition
  */
typedef struct
{
  uint32_t Mode;              /*!< Configures the ADC to operate in independent or multi mode.
                                   This parameter can be a value of @ref ADCEx_Common_mode */
  uint32_t DMAAccessMode;     /*!< Configures the Direct memory access mode for multi ADC mode.
                                   This parameter can be a value of @ref ADCEx_Direct_memory_access_mode_for_multi_mode */
  uint32_t TwoSamplingDelay;  /*!< Configures the Delay between 2 sampling phases.
                                   This parameter can be a value of @ref ADC_delay_between_2_sampling_phases */
}ADC_MultiModeTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup ADCEx_Exported_Constants ADC Exported Constants
  * @{
  */

/** @defgroup ADCEx_Common_mode ADC Common Mode
  * @{
  */
#define ADC_MODE_INDEPENDENT                  ((uint32_t)0x00000000U)
#define ADC_DUALMODE_REGSIMULT_INJECSIMULT    ((uint32_t)ADC_CCR_MULTI_0)
#define ADC_DUALMODE_REGSIMULT_ALTERTRIG      ((uint32_t)ADC_CCR_MULTI_1)
#define ADC_DUALMODE_INJECSIMULT              ((uint32_t)(ADC_CCR_MULTI_2 | ADC_CCR_MULTI_0))
#define ADC_DUALMODE_REGSIMULT                ((uint32_t)(ADC_CCR_MULTI_2 | ADC_CCR_MULTI_1))
#define ADC_DUALMODE_INTERL                   ((uint32_t)(ADC_CCR_MULTI_2 | ADC_CCR_MULTI_1 | ADC_CCR_MULTI_0))
#define ADC_DUALMODE_ALTERTRIG                ((uint32_t)(ADC_CCR_MULTI_3 | ADC_CCR_MULTI_0))
#define ADC_TRIPLEMODE_REGSIMULT_INJECSIMULT  ((uint32_t)(ADC_CCR_MULTI_4 | ADC_CCR_MULTI_0))
#define ADC_TRIPLEMODE_REGSIMULT_AlterTrig    ((uint32_t)(ADC_CCR_MULTI_4 | ADC_CCR_MULTI_1))
#define ADC_TRIPLEMODE_INJECSIMULT            ((uint32_t)(ADC_CCR_MULTI_4 | ADC_CCR_MULTI_2 | ADC_CCR_MULTI_0))
#define ADC_TRIPLEMODE_REGSIMULT              ((uint32_t)(ADC_CCR_MULTI_4 | ADC_CCR_MULTI_2 | ADC_CCR_MULTI_1))
#define ADC_TRIPLEMODE_INTERL                 ((uint32_t)(ADC_CCR_MULTI_4 | ADC_CCR_MULTI_2 | ADC_CCR_MULTI_1 | ADC_CCR_MULTI_0))
#define ADC_TRIPLEMODE_ALTERTRIG              ((uint32_t)(ADC_CCR_MULTI_4 | ADC_CCR_MULTI_3 | ADC_CCR_MULTI_0))
/**
  * @}
  */

/** @defgroup ADCEx_Direct_memory_access_mode_for_multi_mode ADC Direct Memory Access Mode For Multi Mode
  * @{
  */
#define ADC_DMAACCESSMODE_DISABLED  ((uint32_t)0x00000000U)     /*!< DMA mode disabled */
#define ADC_DMAACCESSMODE_1         ((uint32_t)ADC_CCR_DMA_0)  /*!< DMA mode 1 enabled (2 / 3 half-words one by one - 1 then 2 then 3)*/
#define ADC_DMAACCESSMODE_2         ((uint32_t)ADC_CCR_DMA_1)  /*!< DMA mode 2 enabled (2 / 3 half-words by pairs - 2&1 then 1&3 then 3&2)*/
#define ADC_DMAACCESSMODE_3         ((uint32_t)ADC_CCR_DMA)    /*!< DMA mode 3 enabled (2 / 3 bytes by pairs - 2&1 then 1&3 then 3&2) */
/**
  * @}
  */

/** @defgroup ADCEx_External_trigger_edge_Injected ADC External Trigger Edge Injected
  * @{
  */
#define ADC_EXTERNALTRIGINJECCONVEDGE_NONE           ((uint32_t)0x00000000U)
#define ADC_EXTERNALTRIGINJECCONVEDGE_RISING         ((uint32_t)ADC_CR2_JEXTEN_0)
#define ADC_EXTERNALTRIGINJECCONVEDGE_FALLING        ((uint32_t)ADC_CR2_JEXTEN_1)
#define ADC_EXTERNALTRIGINJECCONVEDGE_RISINGFALLING  ((uint32_t)ADC_CR2_JEXTEN)
/**
  * @}
  */

/** @defgroup ADCEx_External_trigger_Source_Injected ADC External Trigger Source Injected
  * @{
  */
#define ADC_EXTERNALTRIGINJECCONV_T1_TRGO         ((uint32_t)0x00000000U)
#define ADC_EXTERNALTRIGINJECCONV_T1_CC4          ((uint32_t)ADC_CR2_JEXTSEL_0)
#define ADC_EXTERNALTRIGINJECCONV_T2_TRGO         ((uint32_t)ADC_CR2_JEXTSEL_1)
#define ADC_EXTERNALTRIGINJECCONV_T2_CC1          ((uint32_t)(ADC_CR2_JEXTSEL_1 | ADC_CR2_JEXTSEL_0))
#define ADC_EXTERNALTRIGINJECCONV_T3_CC4          ((uint32_t)ADC_CR2_JEXTSEL_2)
#define ADC_EXTERNALTRIGINJECCONV_T4_TRGO         ((uint32_t)(ADC_CR2_JEXTSEL_2 | ADC_CR2_JEXTSEL_0))

#define ADC_EXTERNALTRIGINJECCONV_T8_CC4          ((uint32_t)(ADC_CR2_JEXTSEL_2 | ADC_CR2_JEXTSEL_1 | ADC_CR2_JEXTSEL_0))
#define ADC_EXTERNALTRIGINJECCONV_T1_TRGO2        ((uint32_t)ADC_CR2_JEXTSEL_3)
#define ADC_EXTERNALTRIGINJECCONV_T8_TRGO         ((uint32_t)(ADC_CR2_JEXTSEL_3 | ADC_CR2_JEXTSEL_0))
#define ADC_EXTERNALTRIGINJECCONV_T8_TRGO2        ((uint32_t)(ADC_CR2_JEXTSEL_3 | ADC_CR2_JEXTSEL_1))
#define ADC_EXTERNALTRIGINJECCONV_T3_CC3          ((uint32_t)(ADC_CR2_JEXTSEL_3 | ADC_CR2_JEXTSEL_1 | ADC_CR2_JEXTSEL_0))
#define ADC_EXTERNALTRIGINJECCONV_T5_TRGO         ((uint32_t)(ADC_CR2_JEXTSEL_3 | ADC_CR2_JEXTSEL_2))
#define ADC_EXTERNALTRIGINJECCONV_T3_CC1          ((uint32_t)(ADC_CR2_JEXTSEL_3 | ADC_CR2_JEXTSEL_2 | ADC_CR2_JEXTSEL_0))
#define ADC_EXTERNALTRIGINJECCONV_T6_TRGO         ((uint32_t)(ADC_CR2_JEXTSEL_3 | ADC_CR2_JEXTSEL_2 | ADC_CR2_JEXTSEL_1))
#define ADC_INJECTED_SOFTWARE_START                ((uint32_t)ADC_CR2_JEXTSEL + 1)
/**
  * @}
  */

/** @defgroup ADCEx_injected_rank ADC Injected Rank
  * @{
  */
#define ADC_INJECTED_RANK_1    ((uint32_t)0x00000001U)
#define ADC_INJECTED_RANK_2    ((uint32_t)0x00000002U)
#define ADC_INJECTED_RANK_3    ((uint32_t)0x00000003U)
#define ADC_INJECTED_RANK_4    ((uint32_t)0x00000004U)
/**
  * @}
  */

/** @defgroup ADCEx_channels  ADC Specific Channels
  * @{
  */
#define ADC_CHANNEL_TEMPSENSOR  ((uint32_t)ADC_CHANNEL_18 | 0x10000000U)
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
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup ADCEx_Exported_Functions
  * @{
  */

/** @addtogroup ADCEx_Exported_Functions_Group1
  * @{
  */

/* I/O operation functions ******************************************************/
HAL_StatusTypeDef HAL_ADCEx_InjectedStart(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef HAL_ADCEx_InjectedStop(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef HAL_ADCEx_InjectedPollForConversion(ADC_HandleTypeDef* hadc, uint32_t Timeout);
HAL_StatusTypeDef HAL_ADCEx_InjectedStart_IT(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef HAL_ADCEx_InjectedStop_IT(ADC_HandleTypeDef* hadc);
uint32_t HAL_ADCEx_InjectedGetValue(ADC_HandleTypeDef* hadc, uint32_t InjectedRank);
HAL_StatusTypeDef HAL_ADCEx_MultiModeStart_DMA(ADC_HandleTypeDef* hadc, uint32_t* pData, uint32_t Length);
HAL_StatusTypeDef HAL_ADCEx_MultiModeStop_DMA(ADC_HandleTypeDef* hadc);
uint32_t HAL_ADCEx_MultiModeGetValue(ADC_HandleTypeDef* hadc);
void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc);

/* Peripheral Control functions *************************************************/
HAL_StatusTypeDef HAL_ADCEx_InjectedConfigChannel(ADC_HandleTypeDef* hadc,ADC_InjectionConfTypeDef* sConfigInjected);
HAL_StatusTypeDef HAL_ADCEx_MultiModeConfigChannel(ADC_HandleTypeDef* hadc, ADC_MultiModeTypeDef* multimode);

/**
  * @}
  */

/**
  * @}
  */
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @defgroup ADCEx_Private_Constants ADC Private Constants
  * @{
  */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup ADCEx_Private_Macros ADC Private Macros
  * @{
  */
#define IS_ADC_CHANNEL(CHANNEL) (((CHANNEL) <= ADC_CHANNEL_18)  || \
                                 ((CHANNEL) == ADC_CHANNEL_TEMPSENSOR))

#define IS_ADC_MODE(__MODE__) (((__MODE__) == ADC_MODE_INDEPENDENT)                 || \
                               ((__MODE__) == ADC_DUALMODE_REGSIMULT_INJECSIMULT)   || \
                               ((__MODE__) == ADC_DUALMODE_REGSIMULT_ALTERTRIG)     || \
                               ((__MODE__) == ADC_DUALMODE_INJECSIMULT)             || \
                               ((__MODE__) == ADC_DUALMODE_REGSIMULT)               || \
                               ((__MODE__) == ADC_DUALMODE_INTERL)                  || \
                               ((__MODE__) == ADC_DUALMODE_ALTERTRIG)               || \
                               ((__MODE__) == ADC_TRIPLEMODE_REGSIMULT_INJECSIMULT) || \
                               ((__MODE__) == ADC_TRIPLEMODE_REGSIMULT_AlterTrig)   || \
                               ((__MODE__) == ADC_TRIPLEMODE_INJECSIMULT)           || \
                               ((__MODE__) == ADC_TRIPLEMODE_REGSIMULT)             || \
                               ((__MODE__) == ADC_TRIPLEMODE_INTERL)                || \
                               ((__MODE__) == ADC_TRIPLEMODE_ALTERTRIG))
#define IS_ADC_DMA_ACCESS_MODE(__MODE__) (((__MODE__) == ADC_DMAACCESSMODE_DISABLED) || \
                                          ((__MODE__) == ADC_DMAACCESSMODE_1)        || \
                                          ((__MODE__) == ADC_DMAACCESSMODE_2)        || \
                                          ((__MODE__) == ADC_DMAACCESSMODE_3))
#define IS_ADC_EXT_INJEC_TRIG_EDGE(__EDGE__) (((__EDGE__) == ADC_EXTERNALTRIGINJECCONVEDGE_NONE)    || \
                                              ((__EDGE__) == ADC_EXTERNALTRIGINJECCONVEDGE_RISING)  || \
                                              ((__EDGE__) == ADC_EXTERNALTRIGINJECCONVEDGE_FALLING) || \
                                              ((__EDGE__) == ADC_EXTERNALTRIGINJECCONVEDGE_RISINGFALLING))
#define IS_ADC_EXT_INJEC_TRIG(__INJTRIG__) (((__INJTRIG__) == ADC_EXTERNALTRIGINJECCONV_T1_TRGO)  || \
                                            ((__INJTRIG__) == ADC_EXTERNALTRIGINJECCONV_T1_CC4)   || \
                                            ((__INJTRIG__) == ADC_EXTERNALTRIGINJECCONV_T2_TRGO)  || \
                                            ((__INJTRIG__) == ADC_EXTERNALTRIGINJECCONV_T2_CC1)   || \
                                            ((__INJTRIG__) == ADC_EXTERNALTRIGINJECCONV_T3_CC4)   || \
                                            ((__INJTRIG__) == ADC_EXTERNALTRIGINJECCONV_T4_TRGO)  || \
                                            ((__INJTRIG__) == ADC_EXTERNALTRIGINJECCONV_T8_CC4)   || \
                                            ((__INJTRIG__) == ADC_EXTERNALTRIGINJECCONV_T1_TRGO2) || \
                                            ((__INJTRIG__) == ADC_EXTERNALTRIGINJECCONV_T8_TRGO)  || \
                                            ((__INJTRIG__) == ADC_EXTERNALTRIGINJECCONV_T8_TRGO2) || \
                                            ((__INJTRIG__) == ADC_EXTERNALTRIGINJECCONV_T3_CC3)   || \
                                            ((__INJTRIG__) == ADC_EXTERNALTRIGINJECCONV_T5_TRGO)  || \
                                            ((__INJTRIG__) == ADC_EXTERNALTRIGINJECCONV_T3_CC1)   || \
                                            ((__INJTRIG__) == ADC_EXTERNALTRIGINJECCONV_T6_TRGO)  || \
											((__INJTRIG__) == ADC_INJECTED_SOFTWARE_START))
#define IS_ADC_INJECTED_LENGTH(__LENGTH__) (((__LENGTH__) >= ((uint32_t)1)) && ((__LENGTH__) <= ((uint32_t)4)))
#define IS_ADC_INJECTED_RANK(__RANK__) (((__RANK__) >= ((uint32_t)1)) && ((__RANK__) <= ((uint32_t)4)))

/**
  * @brief  Set the selected injected Channel rank.
  * @param  _CHANNELNB_: Channel number.
  * @param  _RANKNB_: Rank number.
  * @param  _JSQR_JL_: Sequence length.
  * @retval None
  */
#define   ADC_JSQR(_CHANNELNB_, _RANKNB_,_JSQR_JL_) (((uint32_t)((uint16_t)(_CHANNELNB_))) << (5 * (uint8_t)(((_RANKNB_) + 3) - (_JSQR_JL_))))
/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup ADCEx_Private_Functions ADC Private Functions
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

#endif /*__STM32F7xx_ADC_EX_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
