/**
  ******************************************************************************
  * @file    stm32l4xx_hal_adc_ex.h
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   Header file of ADC HAL extended module.
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
#ifndef __STM32L4xx_ADC_EX_H
#define __STM32L4xx_ADC_EX_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal_def.h"
   
/** @addtogroup STM32L4xx_HAL_Driver
  * @{
  */

/** @addtogroup ADCEx ADCEx
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/
/** @defgroup ADCEx_Exported_Types ADC Extended Exported Types
  * @{
  */

/** 
  * @brief  ADC Injected Conversion Oversampling structure definition  
  */
typedef struct
{
  uint32_t Ratio;                         /*!< Configures the oversampling ratio.
                                               This parameter can be a value of @ref ADCEx_Oversampling_Ratio */
                                               
  uint32_t RightBitShift;                 /*!< Configures the division coefficient for the Oversampler.
                                               This parameter can be a value of @ref ADCEx_Right_Bit_Shift */
}ADC_InjOversamplingTypeDef;  



/** 
  * @brief  Structure definition of ADC injected group and ADC channel for injected group  
  * @note   Parameters of this structure are shared within 2 scopes:
  *          - Scope channel: InjectedChannel, InjectedRank, InjectedSamplingTime , InjectedSingleDiff, InjectedOffsetNumber, InjectedOffset
  *          - Scope injected group (affects all channels of injected group): InjectedNbrOfConversion, InjectedDiscontinuousConvMode,
  *            AutoInjectedConv, QueueInjectedContext, ExternalTrigInjecConvEdge, ExternalTrigInjecConv, InjecOversamplingMode, InjecOversampling.
  * @note   The setting of these parameters by function HAL_ADCEx_InjectedConfigChannel() is conditioned by ADC state.
  *         ADC state can be either:
  *          - For all parameters: ADC disabled (this is the only possible ADC state to modify parameter 'InjectedSingleDiff')
  *          - For parameters 'InjectedDiscontinuousConvMode', 'QueueInjectedContext', 'InjecOversampling': ADC enabled without conversion on going on injected group.
  *          - For parameters 'InjectedSamplingTime', 'InjectedOffset', 'InjectedOffsetNumber', 'AutoInjectedConv': ADC enabled without conversion on going on regular and injected groups.
  *          - For parameters 'InjectedChannel', 'InjectedRank', 'InjectedNbrOfConversion', 'ExternalTrigInjecConv', 'ExternalTrigInjecConvEdge': ADC enabled and while conversion on going 
  *            on regular and injected groups.
  *         If ADC is not in the appropriate state to modify some parameters, these parameters setting is bypassed
  *         without error reporting (as it can be the expected behaviour in case of intended action to update another parameter (which fulfills the ADC state condition) on the fly).
  */
typedef struct 
{
  uint32_t InjectedChannel;               /*!< Configure the ADC injected channel.
                                               This parameter can be a value of @ref ADC_channels
                                                Note: Depending on devices and ADC instances, some channels may not be available. Refer to device DataSheet for channels availability. */
  uint32_t InjectedRank;                  /*!< The rank in the injected group sequencer.
                                               This parameter must be a value of @ref ADCEx_injected_rank.
                                               Note: to disable a channel or change order of conversion sequencer, rank containing a previous channel setting can be overwritten by 
                                               the new channel setting (or parameter number of conversions adjusted). */
  uint32_t InjectedSamplingTime;          /*!< Sampling time value to be set for the selected channel.
                                               Unit: ADC clock cycles.
                                               Conversion time is the addition of sampling time and processing time (12.5 ADC clock cycles at ADC resolution 12 bits, 10.5 cycles at 10 bits,
                                               8.5 cycles at 8 bits, 6.5 cycles at 6 bits).
                                               This parameter can be a value of @ref ADC_sampling_times.
                                               Caution: This parameter applies to a channel that can be used in a regular and/or injected group.
                                                        It overwrites the last setting.
                                               Note: In case of usage of internal measurement channels (VrefInt/Vbat/TempSensor),
                                                     sampling time constraints must be respected (sampling time can be adjusted with respect to the ADC clock frequency and sampling time 
                                                     setting). Refer to device DataSheet for timings values. */
  uint32_t InjectedSingleDiff;            /*!< Selection of single-ended or differential input.
                                               In differential mode: Differential measurement is between the selected channel 'i' (positive input) and channel 'i+1' (negative input).
                                               Only channel 'i' has to be configured, channel 'i+1' is configured automatically.
                                               This parameter must be a value of @ref ADCEx_SingleDifferential.
                                               Caution: This parameter applies to a channel that can be used in a regular and/or injected group.
                                                        It overwrites the last setting.
                                               Note: Refer to Reference Manual to ensure the selected channel is available in differential mode.
                                               Note: When configuring a channel 'i' in differential mode, the channel 'i+1' is not usable separately.
                                               Note: This parameter must be modified when ADC is disabled (before ADC start conversion or after ADC stop conversion).
                                               If ADC is enabled, this parameter setting is bypassed without error reporting (as it can be the expected behaviour in case 
                                               of another parameter update on the fly) */
  uint32_t InjectedOffsetNumber;          /*!< Selects the offset number.
                                               This parameter can be a value of @ref ADCEx_OffsetNumber.
                                               Caution: Only one offset is allowed per channel. This parameter overwrites the last setting. */
  uint32_t InjectedOffset;                /*!< Defines the offset to be subtracted from the raw converted data.
                                               Offset value must be a positive number.
                                               Depending of ADC resolution selected (12, 10, 8 or 6 bits), this parameter must be a number between Min_Data = 0x000 and Max_Data = 0xFFF, 
                                               0x3FF, 0xFF or 0x3F respectively.
                                               Note: This parameter must be modified when no conversion is on going on both regular and injected groups (ADC disabled, or ADC enabled 
                                               without continuous mode or external trigger that could launch a conversion). */
  uint32_t InjectedNbrOfConversion;       /*!< Specifies the number of ranks that will be converted within the injected group sequencer.
                                               To use the injected group sequencer and convert several ranks, parameter 'ScanConvMode' must be enabled.
                                               This parameter must be a number between Min_Data = 1 and Max_Data = 4.
                                               Caution: this setting impacts the entire injected group. Therefore, call of HAL_ADCEx_InjectedConfigChannel() to 
                                                        configure a channel on injected group can impact the configuration of other channels previously set. */
  uint32_t InjectedDiscontinuousConvMode; /*!< Specifies whether the conversions sequence of injected group is performed in Complete-sequence/Discontinuous-sequence (main sequence 
                                               subdivided in successive parts).
                                               Discontinuous mode is used only if sequencer is enabled (parameter 'ScanConvMode'). If sequencer is disabled, this parameter is discarded.
                                               Discontinuous mode can be enabled only if continuous mode is disabled. 
                                               This parameter can be set to ENABLE or DISABLE.
                                               Note: This parameter must be modified when ADC is disabled (before ADC start conversion or after ADC stop conversion).
                                               Note: For injected group, discontinuous mode converts the sequence channel by channel (only one channel at a time).
                                               Caution: this setting impacts the entire injected group. Therefore, call of HAL_ADCEx_InjectedConfigChannel() to 
                                                        configure a channel on injected group can impact the configuration of other channels previously set. */
  uint32_t AutoInjectedConv;              /*!< Enables or disables the selected ADC automatic injected group conversion after regular one
                                               This parameter can be set to ENABLE or DISABLE.      
                                               Note: To use Automatic injected conversion, discontinuous mode must be disabled ('DiscontinuousConvMode' and 'InjectedDiscontinuousConvMode' set to DISABLE)
                                               Note: To use Automatic injected conversion, injected group external triggers must be disabled.
                                               Note: In case of DMA used with regular group: if DMA configured in normal mode (single shot) JAUTO will be stopped upon DMA transfer complete.
                                                     To maintain JAUTO always enabled, DMA must be configured in circular mode.
                                               Caution: this setting impacts the entire injected group. Therefore, call of HAL_ADCEx_InjectedConfigChannel() to 
                                                        configure a channel on injected group can impact the configuration of other channels previously set. */
  uint32_t QueueInjectedContext;          /*!< Specifies whether the context queue feature is enabled.
                                               This parameter can be set to ENABLE or DISABLE.
                                               If context queue is enabled, injected sequencer&channels configurations are queued on up to 2 contexts. If a
                                               new injected context is set when queue is full, error is triggered by interruption and through function 
                                               'HAL_ADCEx_InjectedQueueOverflowCallback'.
                                               Caution: This feature request that the sequence is fully configured before injected conversion start.
                                                        Therefore, configure channels with as many calls to HAL_ADCEx_InjectedConfigChannel() as the 'InjectedNbrOfConversion' parameter.
                                               Caution: this setting impacts the entire injected group. Therefore, call of HAL_ADCEx_InjectedConfigChannel() to 
                                                        configure a channel on injected group can impact the configuration of other channels previously set.
                                               Note: This parameter must be modified when ADC is disabled (before ADC start conversion or after ADC stop conversion). */
  uint32_t ExternalTrigInjecConv;         /*!< Selects the external event used to trigger the conversion start of injected group.
                                               If set to ADC_INJECTED_SOFTWARE_START, external triggers are disabled and software trigger is used instead.
                                               This parameter can be a value of @ref ADCEx_Injected_External_Trigger_Source.
                                               Caution: this setting impacts the entire injected group. Therefore, call of HAL_ADCEx_InjectedConfigChannel() to 
                                                        configure a channel on injected group can impact the configuration of other channels previously set. */
  uint32_t ExternalTrigInjecConvEdge;     /*!< Selects the external trigger edge of injected group.
                                               This parameter can be a value of @ref ADCEx_Injected_External_Trigger_Source_Edge.
                                               If trigger edge is set to ADC_EXTERNALTRIGINJECCONV_EDGE_NONE, external triggers are disabled and software trigger is used instead.
                                               Caution: this setting impacts the entire injected group. Therefore, call of HAL_ADCEx_InjectedConfigChannel() to 
                                                        configure a channel on injected group can impact the configuration of other channels previously set. */
                                                                                                
  uint32_t InjecOversamplingMode;             /*!< Specifies whether the oversampling feature is enabled or disabled.
                                              This parameter can be set to ENABLE or DISABLE.
                                              Note: This parameter can be modified only if there is no conversion is ongoing (both ADSTART and JADSTART cleared). */
                                              
  ADC_InjOversamplingTypeDef  InjecOversampling;   /*!< Specifies the Oversampling parameters.
                                                   Caution: this setting overwrites the previous oversampling configuration if oversampling already enabled.    
                                                   Note: This parameter can be modified only if there is no conversion is ongoing (both ADSTART and JADSTART cleared). */                                                          
}ADC_InjectionConfTypeDef;


/** 
  * @brief  Structure definition of ADC multimode
  * @note   The setting of these parameters by function HAL_ADCEx_MultiModeConfigChannel() is conditioned by ADCs state (both Master and Slave ADCs).
  *         Both Master and Slave ADCs must be disabled.
  */
typedef struct
{
  uint32_t Mode;              /*!< Configures the ADC to operate in independent or multimode. 
                                   This parameter can be a value of @ref ADCEx_Common_mode. */
  uint32_t DMAAccessMode;     /*!< Configures the DMA mode for multimode ADC:
                                   selection whether 2 DMA channels (each ADC uses its own DMA channel) or 1 DMA channel (one DMA channel for both ADC, DMA of ADC master)
                                   This parameter can be a value of @ref ADCEx_Direct_memory_access_mode_for_multimode. */
  uint32_t TwoSamplingDelay;  /*!< Configures the Delay between 2 sampling phases.
                                   This parameter can be a value of @ref ADCEx_delay_between_2_sampling_phases.
                                   Delay range depends on selected resolution: 
                                    from 1 to 12 clock cycles for 12 bits, from 1 to 10 clock cycles for 10 bits,
                                    from 1 to 8 clock cycles for 8 bits, from 1 to 6 clock cycles for 6 bits.     */
}ADC_MultiModeTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

  /** @defgroup ADCEx_Exported_Constants ADC Extended Exported Constants
  * @{
  */

/** @defgroup ADCEx_SingleDifferential ADC Extended Single-ended/Differential input mode
  * @{
  */
#define ADC_SINGLE_ENDED                ((uint32_t)0x00000000)       /*!< ADC channel set in single-ended input mode */
#define ADC_DIFFERENTIAL_ENDED          ((uint32_t)ADC_CR_ADCALDIF)  /*!< ADC channel set in differential mode       */
/**
  * @}
  */

/** @defgroup ADCEx_OffsetNumber ADC Extended Offset Number
  * @{
  */
#define ADC_OFFSET_NONE               ((uint32_t)0x00)     /*!< No offset correction                           */
#define ADC_OFFSET_1                  ((uint32_t)0x01)     /*!< Offset correction to apply to a first channel  */
#define ADC_OFFSET_2                  ((uint32_t)0x02)     /*!< Offset correction to apply to a second channel */
#define ADC_OFFSET_3                  ((uint32_t)0x03)     /*!< Offset correction to apply to a third channel  */
#define ADC_OFFSET_4                  ((uint32_t)0x04)     /*!< Offset correction to apply to a fourth channel */
/**
  * @}
  */

/** @defgroup ADCEx_regular_rank ADC Extended Regular Channel Rank
  * @{
  */
#define ADC_REGULAR_RANK_1    ((uint32_t)0x00000001)       /*!< ADC regular conversion rank 1  */
#define ADC_REGULAR_RANK_2    ((uint32_t)0x00000002)       /*!< ADC regular conversion rank 2  */
#define ADC_REGULAR_RANK_3    ((uint32_t)0x00000003)       /*!< ADC regular conversion rank 3  */
#define ADC_REGULAR_RANK_4    ((uint32_t)0x00000004)       /*!< ADC regular conversion rank 4  */
#define ADC_REGULAR_RANK_5    ((uint32_t)0x00000005)       /*!< ADC regular conversion rank 5  */
#define ADC_REGULAR_RANK_6    ((uint32_t)0x00000006)       /*!< ADC regular conversion rank 6  */
#define ADC_REGULAR_RANK_7    ((uint32_t)0x00000007)       /*!< ADC regular conversion rank 7  */
#define ADC_REGULAR_RANK_8    ((uint32_t)0x00000008)       /*!< ADC regular conversion rank 8  */
#define ADC_REGULAR_RANK_9    ((uint32_t)0x00000009)       /*!< ADC regular conversion rank 9  */
#define ADC_REGULAR_RANK_10   ((uint32_t)0x0000000A)       /*!< ADC regular conversion rank 10 */
#define ADC_REGULAR_RANK_11   ((uint32_t)0x0000000B)       /*!< ADC regular conversion rank 11 */
#define ADC_REGULAR_RANK_12   ((uint32_t)0x0000000C)       /*!< ADC regular conversion rank 12 */
#define ADC_REGULAR_RANK_13   ((uint32_t)0x0000000D)       /*!< ADC regular conversion rank 13 */
#define ADC_REGULAR_RANK_14   ((uint32_t)0x0000000E)       /*!< ADC regular conversion rank 14 */
#define ADC_REGULAR_RANK_15   ((uint32_t)0x0000000F)       /*!< ADC regular conversion rank 15 */
#define ADC_REGULAR_RANK_16   ((uint32_t)0x00000010)       /*!< ADC regular conversion rank 16 */
/**
  * @}
  */

/** @defgroup ADCEx_injected_rank ADC Extended Injected Channel Rank
  * @{
  */
#define ADC_INJECTED_RANK_1    ((uint32_t)0x00000001)      /*!< ADC injected conversion rank 1 */
#define ADC_INJECTED_RANK_2    ((uint32_t)0x00000002)      /*!< ADC injected conversion rank 2 */
#define ADC_INJECTED_RANK_3    ((uint32_t)0x00000003)      /*!< ADC injected conversion rank 3 */
#define ADC_INJECTED_RANK_4    ((uint32_t)0x00000004)      /*!< ADC injected conversion rank 4 */
/**injected
  * @}
  */
                        
/** @defgroup ADCEx_Injected_External_Trigger_Source_Edge ADC External Trigger Source Edge for Injected Group
  * @{
  */
#define ADC_EXTERNALTRIGINJECCONV_EDGE_NONE           ((uint32_t)0x00000000)          /*!< Injected conversions hardware trigger detection disabled                             */ 
#define ADC_EXTERNALTRIGINJECCONV_EDGE_RISING         ((uint32_t)ADC_JSQR_JEXTEN_0)   /*!< Injected conversions hardware trigger detection on the rising edge                   */ 
#define ADC_EXTERNALTRIGINJECCONV_EDGE_FALLING        ((uint32_t)ADC_JSQR_JEXTEN_1)   /*!< Injected conversions hardware trigger detection on the falling edge                  */ 
#define ADC_EXTERNALTRIGINJECCONV_EDGE_RISINGFALLING  ((uint32_t)ADC_JSQR_JEXTEN)     /*!< Injected conversions hardware trigger detection on both the rising and falling edges */ 
/**
  * @}
  */

/** @defgroup ADCEx_Injected_External_Trigger_Source ADC Extended External Trigger Source for Injected Group
  * @{
  */
#define ADC_EXTERNALTRIGINJEC_T1_TRGO    ((uint32_t)0x00000000)                                                     /*!< Event 0 triggers injected group conversion start  */
#define ADC_EXTERNALTRIGINJEC_T1_CC4     ((uint32_t)ADC_JSQR_JEXTSEL_0)                                             /*!< Event 1 triggers injected group conversion start  */
#define ADC_EXTERNALTRIGINJEC_T2_TRGO    ((uint32_t)ADC_JSQR_JEXTSEL_1)                                             /*!< Event 2 triggers injected group conversion start  */
#define ADC_EXTERNALTRIGINJEC_T2_CC1     ((uint32_t)(ADC_JSQR_JEXTSEL_1 | ADC_JSQR_JEXTSEL_0))                      /*!< Event 3 triggers injected group conversion start  */
#define ADC_EXTERNALTRIGINJEC_T3_CC4     ((uint32_t)ADC_JSQR_JEXTSEL_2)                                             /*!< Event 4 triggers injected group conversion start  */
#define ADC_EXTERNALTRIGINJEC_T4_TRGO    ((uint32_t)(ADC_JSQR_JEXTSEL_2 | ADC_JSQR_JEXTSEL_0))                      /*!< Event 5 triggers injected group conversion start  */
#define ADC_EXTERNALTRIGINJEC_EXT_IT15   ((uint32_t)(ADC_JSQR_JEXTSEL_2 | ADC_JSQR_JEXTSEL_1))                      /*!< Event 6 triggers injected group conversion start  */
#define ADC_EXTERNALTRIGINJEC_T8_CC4     ((uint32_t)(ADC_JSQR_JEXTSEL_2 | ADC_JSQR_JEXTSEL_1 | ADC_JSQR_JEXTSEL_0)) /*!< Event 7 triggers injected group conversion start  */
#define ADC_EXTERNALTRIGINJEC_T1_TRGO2   ((uint32_t)ADC_JSQR_JEXTSEL_3)                                             /*!< Event 8 triggers injected group conversion start  */
#define ADC_EXTERNALTRIGINJEC_T8_TRGO    ((uint32_t)(ADC_JSQR_JEXTSEL_3 | ADC_JSQR_JEXTSEL_0))                      /*!< Event 9 triggers injected group conversion start  */
#define ADC_EXTERNALTRIGINJEC_T8_TRGO2   ((uint32_t)(ADC_JSQR_JEXTSEL_3 | ADC_JSQR_JEXTSEL_1))                      /*!< Event 10 triggers injected group conversion start */
#define ADC_EXTERNALTRIGINJEC_T3_CC3     ((uint32_t)(ADC_JSQR_JEXTSEL_3 | ADC_JSQR_JEXTSEL_1 | ADC_JSQR_JEXTSEL_0)) /*!< Event 11 triggers injected group conversion start */
#define ADC_EXTERNALTRIGINJEC_T3_TRGO    ((uint32_t)(ADC_JSQR_JEXTSEL_3 | ADC_JSQR_JEXTSEL_2))                      /*!< Event 12 triggers injected group conversion start */
#define ADC_EXTERNALTRIGINJEC_T3_CC1     ((uint32_t)(ADC_JSQR_JEXTSEL_3 | ADC_JSQR_JEXTSEL_2 | ADC_JSQR_JEXTSEL_0)) /*!< Event 13 triggers injected group conversion start */
#define ADC_EXTERNALTRIGINJEC_T6_TRGO    ((uint32_t)(ADC_JSQR_JEXTSEL_3 | ADC_JSQR_JEXTSEL_2 | ADC_JSQR_JEXTSEL_1)) /*!< Event 14 triggers injected group conversion start */
#define ADC_EXTERNALTRIGINJEC_T15_TRGO   ((uint32_t)ADC_JSQR_JEXTSEL)                                               /*!< Event 15 triggers injected group conversion start */

#define ADC_INJECTED_SOFTWARE_START      ((uint32_t)0x00000001)                                                     /*!< Software triggers injected group conversion start */
/**
  * @}
  */

/** @defgroup ADCEx_Common_mode ADC Extended Dual ADC Mode
  * @{
  */
#define ADC_MODE_INDEPENDENT                  ((uint32_t)(0x00000000))                                       /*!< Independent ADC conversions mode                           */
#define ADC_DUALMODE_REGSIMULT_INJECSIMULT    ((uint32_t)(ADC_CCR_DUAL_0))                                   /*!< Combined regular simultaneous + injected simultaneous mode */
#define ADC_DUALMODE_REGSIMULT_ALTERTRIG      ((uint32_t)(ADC_CCR_DUAL_1))                                   /*!< Combined regular simultaneous + alternate trigger mode     */
#define ADC_DUALMODE_REGINTERL_INJECSIMULT    ((uint32_t)(ADC_CCR_DUAL_1 | ADC_CCR_DUAL_0))                  /*!< Combined Interleaved mode + injected simultaneous mode     */
#define ADC_DUALMODE_INJECSIMULT              ((uint32_t)(ADC_CCR_DUAL_2 | ADC_CCR_DUAL_0))                  /*!< Injected simultaneous mode only                            */
#define ADC_DUALMODE_REGSIMULT                ((uint32_t)(ADC_CCR_DUAL_2 | ADC_CCR_DUAL_1))                  /*!< Regular simultaneous mode only                             */
#define ADC_DUALMODE_INTERL                   ((uint32_t)(ADC_CCR_DUAL_2 | ADC_CCR_DUAL_1 | ADC_CCR_DUAL_0)) /*!< Interleaved mode only                                      */
#define ADC_DUALMODE_ALTERTRIG                ((uint32_t)(ADC_CCR_DUAL_3 | ADC_CCR_DUAL_0))                  /*!< Alternate trigger mode only                                */
/**
  * @}
  */

/** @defgroup ADCEx_Direct_memory_access_mode_for_multimode ADC Extended DMA Mode for Dual ADC Mode
  * @{
  */
#define ADC_DMAACCESSMODE_DISABLED      ((uint32_t)0x00000000)       /*!< DMA multimode disabled: each ADC uses its own DMA channel */
#define ADC_DMAACCESSMODE_12_10_BITS    ((uint32_t)ADC_CCR_MDMA_1)   /*!< DMA multimode enabled (one DMA channel for both ADC, DMA of ADC master) for 12 and 10 bits resolution */
#define ADC_DMAACCESSMODE_8_6_BITS      ((uint32_t)ADC_CCR_MDMA)     /*!< DMA multimode enabled (one DMA channel for both ADC, DMA of ADC master) for 8 and 6 bits resolution */
/**
  * @}
  */

/** @defgroup ADCEx_delay_between_2_sampling_phases ADC Extended Delay Between 2 Sampling Phases
  * @{
  */
#define ADC_TWOSAMPLINGDELAY_1CYCLE     ((uint32_t)(0x00000000))                                          /*!< 1 ADC clock cycle delay                                     */ 
#define ADC_TWOSAMPLINGDELAY_2CYCLES    ((uint32_t)(ADC_CCR_DELAY_0))                                     /*!< 2 ADC clock cycles delay                                    */ 
#define ADC_TWOSAMPLINGDELAY_3CYCLES    ((uint32_t)(ADC_CCR_DELAY_1))                                     /*!< 3 ADC clock cycles delay                                    */ 
#define ADC_TWOSAMPLINGDELAY_4CYCLES    ((uint32_t)(ADC_CCR_DELAY_1 | ADC_CCR_DELAY_0))                   /*!< 4 ADC clock cycles delay                                    */ 
#define ADC_TWOSAMPLINGDELAY_5CYCLES    ((uint32_t)(ADC_CCR_DELAY_2))                                     /*!< 5 ADC clock cycles delay                                    */ 
#define ADC_TWOSAMPLINGDELAY_6CYCLES    ((uint32_t)(ADC_CCR_DELAY_2 | ADC_CCR_DELAY_0))                   /*!< 6 ADC clock cycles delay                                    */ 
#define ADC_TWOSAMPLINGDELAY_7CYCLES    ((uint32_t)(ADC_CCR_DELAY_2 | ADC_CCR_DELAY_1))                   /*!< 7 ADC clock cycles delay (lower for non 12-bit resolution)  */ 
#define ADC_TWOSAMPLINGDELAY_8CYCLES    ((uint32_t)(ADC_CCR_DELAY_2 | ADC_CCR_DELAY_1 | ADC_CCR_DELAY_0)) /*!< 8 ADC clock cycles delay (lower for non 12-bit resolution)  */ 
#define ADC_TWOSAMPLINGDELAY_9CYCLES    ((uint32_t)(ADC_CCR_DELAY_3))                                     /*!< 9 ADC clock cycles delay (lower for non 12-bit resolution)  */ 
#define ADC_TWOSAMPLINGDELAY_10CYCLES   ((uint32_t)(ADC_CCR_DELAY_3 | ADC_CCR_DELAY_0))                   /*!< 10 ADC clock cycles delay (lower for non 12-bit resolution) */ 
#define ADC_TWOSAMPLINGDELAY_11CYCLES   ((uint32_t)(ADC_CCR_DELAY_3 | ADC_CCR_DELAY_1))                   /*!< 11 ADC clock cycles delay (lower for non 12-bit resolution) */ 
#define ADC_TWOSAMPLINGDELAY_12CYCLES   ((uint32_t)(ADC_CCR_DELAY_3 | ADC_CCR_DELAY_1 | ADC_CCR_DELAY_0)) /*!< 12 ADC clock cycles delay (lower for non 12-bit resolution) */ 
/**
  * @}
  */

/** @defgroup ADCEx_analog_watchdog_number ADC Extended Analog Watchdog Selection
  * @{
  */
#define ADC_ANALOGWATCHDOG_1                    ((uint32_t)0x00000001)   /*!< Analog watchdog 1 selection */ 
#define ADC_ANALOGWATCHDOG_2                    ((uint32_t)0x00000002)   /*!< Analog watchdog 2 selection */ 
#define ADC_ANALOGWATCHDOG_3                    ((uint32_t)0x00000003)   /*!< Analog watchdog 3 selection */ 
/**
  * @}
  */

/** @defgroup ADCEx_analog_watchdog_mode ADC Extended Analog Watchdog Mode
  * @{
  */
#define ADC_ANALOGWATCHDOG_NONE                 ((uint32_t) 0x00000000)                                             /*!< No analog watchdog selected                                             */
#define ADC_ANALOGWATCHDOG_SINGLE_REG           ((uint32_t)(ADC_CFGR_AWD1SGL | ADC_CFGR_AWD1EN))                    /*!< Analog watchdog applied to a regular group single channel               */
#define ADC_ANALOGWATCHDOG_SINGLE_INJEC         ((uint32_t)(ADC_CFGR_AWD1SGL | ADC_CFGR_JAWD1EN))                   /*!< Analog watchdog applied to an injected group single channel             */
#define ADC_ANALOGWATCHDOG_SINGLE_REGINJEC      ((uint32_t)(ADC_CFGR_AWD1SGL | ADC_CFGR_AWD1EN | ADC_CFGR_JAWD1EN)) /*!< Analog watchdog applied to a regular and injected groups single channel */
#define ADC_ANALOGWATCHDOG_ALL_REG              ((uint32_t) ADC_CFGR_AWD1EN)                                        /*!< Analog watchdog applied to regular group all channels                   */
#define ADC_ANALOGWATCHDOG_ALL_INJEC            ((uint32_t) ADC_CFGR_JAWD1EN)                                       /*!< Analog watchdog applied to injected group all channels                  */
#define ADC_ANALOGWATCHDOG_ALL_REGINJEC         ((uint32_t)(ADC_CFGR_AWD1EN | ADC_CFGR_JAWD1EN))                    /*!< Analog watchdog applied to regular and injected groups all channels     */
/**
  * @}
  */

/** @defgroup ADCEx_conversion_group ADC Extended Conversion Group
  * @{
  */                                                                                                            
#define ADC_REGULAR_GROUP             ((uint32_t)(ADC_FLAG_EOC | ADC_FLAG_EOS))                                 /*!< ADC regular group selection               */ 
#define ADC_INJECTED_GROUP            ((uint32_t)(ADC_FLAG_JEOC | ADC_FLAG_JEOS))                               /*!< ADC injected group selection              */ 
#define ADC_REGULAR_INJECTED_GROUP    ((uint32_t)(ADC_FLAG_EOC | ADC_FLAG_EOS | ADC_FLAG_JEOC | ADC_FLAG_JEOS)) /*!< ADC regular and injected groups selection */ 
/**
  * @}
  */

/** @defgroup ADCEx_Event_type ADC Extended Event Type
  * @{
  */
#define ADC_EOSMP_EVENT          ((uint32_t)ADC_FLAG_EOSMP) /*!< ADC End of Sampling event                                */
#define ADC_AWD1_EVENT           ((uint32_t)ADC_FLAG_AWD1)  /*!< ADC Analog watchdog 1 event (main analog watchdog)       */
#define ADC_AWD2_EVENT           ((uint32_t)ADC_FLAG_AWD2)  /*!< ADC Analog watchdog 2 event (additional analog watchdog) */
#define ADC_AWD3_EVENT           ((uint32_t)ADC_FLAG_AWD3)  /*!< ADC Analog watchdog 3 event (additional analog watchdog) */
#define ADC_OVR_EVENT            ((uint32_t)ADC_FLAG_OVR)   /*!< ADC overrun event                                        */
#define ADC_JQOVF_EVENT          ((uint32_t)ADC_FLAG_JQOVF) /*!< ADC Injected Context Queue Overflow event                */

#define ADC_AWD_EVENT            ADC_AWD1_EVENT      /*!< ADC Analog watchdog 1 event: Naming for compatibility with other STM32 devices having only one analog watchdog */
/**
  * @}
  */

/** @defgroup ADCEx_interrupts_definition ADC Extended Interrupts Definition
  * @{
  */
#define ADC_IT_RDY           ADC_IER_ADRDY      /*!< ADC Ready (ADRDY) interrupt source */
#define ADC_IT_EOSMP         ADC_IER_EOSMP      /*!< ADC End of sampling interrupt source */
#define ADC_IT_EOC           ADC_IER_EOC        /*!< ADC End of regular conversion interrupt source */
#define ADC_IT_EOS           ADC_IER_EOS        /*!< ADC End of regular sequence of conversions interrupt source */
#define ADC_IT_OVR           ADC_IER_OVR        /*!< ADC overrun interrupt source */
#define ADC_IT_JEOC          ADC_IER_JEOC       /*!< ADC End of injected conversion interrupt source */
#define ADC_IT_JEOS          ADC_IER_JEOS       /*!< ADC End of injected sequence of conversions interrupt source */
#define ADC_IT_AWD1          ADC_IER_AWD1       /*!< ADC Analog watchdog 1 interrupt source (main analog watchdog) */
#define ADC_IT_AWD2          ADC_IER_AWD2       /*!< ADC Analog watchdog 2 interrupt source (additional analog watchdog) */
#define ADC_IT_AWD3          ADC_IER_AWD3       /*!< ADC Analog watchdog 3 interrupt source (additional analog watchdog) */
#define ADC_IT_JQOVF         ADC_IER_JQOVF      /*!< ADC Injected Context Queue Overflow interrupt source */

#define ADC_IT_AWD           ADC_IT_AWD1        /*!< ADC Analog watchdog 1 interrupt source: naming for compatibility with other STM32 devices having only one analog watchdog */

/**
  * @}
  */

/** @defgroup ADCEx_flags_definition ADC Extended Flags Definition
  * @{
  */
#define ADC_FLAG_RDY           ADC_ISR_ADRDY    /*!< ADC Ready (ADRDY) flag */
#define ADC_FLAG_EOSMP         ADC_ISR_EOSMP    /*!< ADC End of Sampling flag */
#define ADC_FLAG_EOC           ADC_ISR_EOC      /*!< ADC End of Regular Conversion flag */
#define ADC_FLAG_EOS           ADC_ISR_EOS      /*!< ADC End of Regular sequence of Conversions flag */
#define ADC_FLAG_OVR           ADC_ISR_OVR      /*!< ADC overrun flag */
#define ADC_FLAG_JEOC          ADC_ISR_JEOC     /*!< ADC End of Injected Conversion flag */
#define ADC_FLAG_JEOS          ADC_ISR_JEOS     /*!< ADC End of Injected sequence of Conversions flag */
#define ADC_FLAG_AWD1          ADC_ISR_AWD1     /*!< ADC Analog watchdog 1 flag (main analog watchdog) */
#define ADC_FLAG_AWD2          ADC_ISR_AWD2     /*!< ADC Analog watchdog 2 flag (additional analog watchdog) */
#define ADC_FLAG_AWD3          ADC_ISR_AWD3     /*!< ADC Analog watchdog 3 flag (additional analog watchdog) */
#define ADC_FLAG_JQOVF         ADC_ISR_JQOVF    /*!< ADC Injected Context Queue Overflow flag */

#define ADC_FLAG_AWD           ADC_FLAG_AWD1    /*!< ADC Analog watchdog 1 flag: Naming for compatibility with other STM32 devices having only one analog watchdog */

#define ADC_FLAG_ALL    (ADC_FLAG_RDY | ADC_FLAG_EOSMP | ADC_FLAG_EOC | ADC_FLAG_EOS |  \
                         ADC_FLAG_JEOC | ADC_FLAG_JEOS | ADC_FLAG_OVR | ADC_FLAG_AWD1 | \
                         ADC_FLAG_AWD2 | ADC_FLAG_AWD3 | ADC_FLAG_JQOVF)   /*!< ADC all flags */

/* Combination of all post-conversion flags bits: EOC/EOS, JEOC/JEOS, OVR, AWDx, JQOVF */
#define ADC_FLAG_POSTCONV_ALL (ADC_FLAG_EOC | ADC_FLAG_EOS  | ADC_FLAG_JEOC | ADC_FLAG_JEOS | \
                               ADC_FLAG_OVR | ADC_FLAG_AWD1 | ADC_FLAG_AWD2 | ADC_FLAG_AWD3 | \
                               ADC_FLAG_JQOVF)                             /*!< ADC post-conversion all flags */

/**
  * @}
  */

       
/** @defgroup ADCEx_injected_rank ADC Extended Injected Channel Rank
  * @{
  */
#define ADC_INJECTED_RANK_1    ((uint32_t)0x00000001)   /*!< ADC injected conversion rank 1 */
#define ADC_INJECTED_RANK_2    ((uint32_t)0x00000002)   /*!< ADC injected conversion rank 2 */
#define ADC_INJECTED_RANK_3    ((uint32_t)0x00000003)   /*!< ADC injected conversion rank 3 */
#define ADC_INJECTED_RANK_4    ((uint32_t)0x00000004)   /*!< ADC injected conversion rank 4 */
/**
  * @}
  */



/** @defgroup ADCEx_Oversampling_Ratio    ADC Extended Oversampling Ratio
  * @{
  */

#define ADC_OVERSAMPLING_RATIO_2      ((uint32_t)0x00000000)                            /*!<  ADC Oversampling ratio 2x   */
#define ADC_OVERSAMPLING_RATIO_4      ((uint32_t)ADC_CFGR2_OVSR_0)                      /*!<  ADC Oversampling ratio 4x   */
#define ADC_OVERSAMPLING_RATIO_8      ((uint32_t)ADC_CFGR2_OVSR_1)                      /*!<  ADC Oversampling ratio 8x   */
#define ADC_OVERSAMPLING_RATIO_16     ((uint32_t)(ADC_CFGR2_OVSR_1 | ADC_CFGR2_OVSR_0)) /*!<  ADC Oversampling ratio 16x  */
#define ADC_OVERSAMPLING_RATIO_32     ((uint32_t)ADC_CFGR2_OVSR_2)                      /*!<  ADC Oversampling ratio 32x  */
#define ADC_OVERSAMPLING_RATIO_64     ((uint32_t)(ADC_CFGR2_OVSR_2 | ADC_CFGR2_OVSR_0)) /*!<  ADC Oversampling ratio 64x  */
#define ADC_OVERSAMPLING_RATIO_128    ((uint32_t)(ADC_CFGR2_OVSR_2 | ADC_CFGR2_OVSR_1)) /*!<  ADC Oversampling ratio 128x */
#define ADC_OVERSAMPLING_RATIO_256    ((uint32_t)(ADC_CFGR2_OVSR))                      /*!<  ADC Oversampling ratio 256x */
/**
  * @}
  */

/** @defgroup ADCEx_Right_Bit_Shift   ADC Extended Oversampling Right Shift
  * @{
  */
#define ADC_RIGHTBITSHIFT_NONE  ((uint32_t)0x00000000)                                               /*!<  ADC No bit shift for oversampling */
#define ADC_RIGHTBITSHIFT_1     ((uint32_t)ADC_CFGR2_OVSS_0)                                         /*!<  ADC 1 bit shift for oversampling  */
#define ADC_RIGHTBITSHIFT_2     ((uint32_t)ADC_CFGR2_OVSS_1)                                         /*!<  ADC 2 bits shift for oversampling */
#define ADC_RIGHTBITSHIFT_3     ((uint32_t)(ADC_CFGR2_OVSS_1 | ADC_CFGR2_OVSS_0))                    /*!<  ADC 3 bits shift for oversampling */
#define ADC_RIGHTBITSHIFT_4     ((uint32_t)ADC_CFGR2_OVSS_2)                                         /*!<  ADC 4 bits shift for oversampling */
#define ADC_RIGHTBITSHIFT_5     ((uint32_t)(ADC_CFGR2_OVSS_2 | ADC_CFGR2_OVSS_0))                    /*!<  ADC 5 bits shift for oversampling */
#define ADC_RIGHTBITSHIFT_6     ((uint32_t)(ADC_CFGR2_OVSS_2 | ADC_CFGR2_OVSS_1))                    /*!<  ADC 6 bits shift for oversampling */
#define ADC_RIGHTBITSHIFT_7     ((uint32_t)(ADC_CFGR2_OVSS_2 | ADC_CFGR2_OVSS_1 | ADC_CFGR2_OVSS_0)) /*!<  ADC 7 bits shift for oversampling */
#define ADC_RIGHTBITSHIFT_8     ((uint32_t)ADC_CFGR2_OVSS_3)                                         /*!<  ADC 8 bits shift for oversampling */
/**
  * @}
  */

/** @defgroup ADCEx_Triggered_Oversampling_Mode   ADC Extended Triggered Regular Oversampling
  * @{
  */
#define ADC_TRIGGEREDMODE_SINGLE_TRIGGER      ((uint32_t)0x00000000)      /*!<  A single trigger for all channel oversampled conversions */
#define ADC_TRIGGEREDMODE_MULTI_TRIGGER       ((uint32_t)ADC_CFGR2_TROVS) /*!<  A trigger for each oversampled conversion                */
/**
  * @}
  */
  
/** @defgroup ADCEx_Regular_Oversampling_Mode   ADC Extended Regular Oversampling Continued or Resumed Mode
  * @{
  */
#define ADC_REGOVERSAMPLING_CONTINUED_MODE    ((uint32_t)0x00000000)      /*!<  Oversampling buffer maintained during injection sequence */
#define ADC_REGOVERSAMPLING_RESUMED_MODE      ((uint32_t)ADC_CFGR2_ROVSM) /*!<  Oversampling buffer zeroed during injection sequence     */
/**
  * @}
  */

/** @defgroup ADC_CFGR_fields ADCx CFGR fields
  * @{
  */
#define ADC_CFGR_FIELDS    (ADC_CFGR_AWD1CH  | ADC_CFGR_JAUTO   | ADC_CFGR_JAWD1EN |\
                            ADC_CFGR_AWD1EN  | ADC_CFGR_AWD1SGL | ADC_CFGR_JQM     |\
                            ADC_CFGR_JDISCEN | ADC_CFGR_DISCNUM | ADC_CFGR_DISCEN  |\
                            ADC_CFGR_AUTDLY  | ADC_CFGR_CONT    | ADC_CFGR_OVRMOD  |\
                            ADC_CFGR_EXTEN   | ADC_CFGR_EXTSEL  | ADC_CFGR_ALIGN   |\
                            ADC_CFGR_RES     | ADC_CFGR_DMACFG  | ADC_CFGR_DMAEN   )
/**
  * @}
  */  
  
/** @defgroup ADC_SMPR1_fields ADCx SMPR1 fields
  * @{
  */
#define ADC_SMPR1_FIELDS    (ADC_SMPR1_SMP9 | ADC_SMPR1_SMP8 | ADC_SMPR1_SMP7 |\
                             ADC_SMPR1_SMP6 | ADC_SMPR1_SMP5 | ADC_SMPR1_SMP4 |\
                             ADC_SMPR1_SMP3 | ADC_SMPR1_SMP2 | ADC_SMPR1_SMP1 |\
                             ADC_SMPR1_SMP0) 
/**
  * @}
  */ 
  
/** @defgroup ADC_CFGR_fields_2 ADCx CFGR sub fields 
  * @{
  */
/* ADC_CFGR fields of parameters that can be updated when no conversion
   (neither regular nor injected) is on-going  */  
#define ADC_CFGR_FIELDS_2  ((uint32_t)(ADC_CFGR_DMACFG | ADC_CFGR_AUTDLY))
/**
  * @}
  */ 
  
/**
  * @}
  */



/* Private macros -----------------------------------------------------------*/

/** @defgroup ADCEx_Private_Macro_internal_HAL_driver ADC Extended Private Macros
  * @{
  */
  
/**
  * @brief Test if conversion trigger of injected group is software start
  *        or external trigger.
  * @param __HANDLE__: ADC handle.
  * @retval SET (software start) or RESET (external trigger).
  */
#define ADC_IS_SOFTWARE_START_INJECTED(__HANDLE__)                       \
       (((__HANDLE__)->Instance->JSQR & ADC_JSQR_JEXTEN) == RESET)

/**
  * @brief Check if conversion is on going on regular or injected groups.
  * @param __HANDLE__: ADC handle.
  * @retval SET (conversion is on going) or RESET (no conversion is on going).
  */
#define ADC_IS_CONVERSION_ONGOING_REGULAR_INJECTED(__HANDLE__)                    \
       (( (((__HANDLE__)->Instance->CR) & (ADC_CR_ADSTART | ADC_CR_JADSTART)) == RESET  \
        ) ? RESET : SET)
        

/**
  * @brief Check if conversion is on going on injected group.
  * @param __HANDLE__: ADC handle.
  * @retval SET (conversion is on going) or RESET (no conversion is on going).
  */
#define ADC_IS_CONVERSION_ONGOING_INJECTED(__HANDLE__)                   \
       (( (((__HANDLE__)->Instance->CR) & ADC_CR_JADSTART) == RESET            \
        ) ? RESET : SET)  
                      

        
/**
  * @brief Check whether or not ADC is independent.
  * @param __HANDLE__: ADC handle.
  * @retval SET (ADC is independent) or RESET (ADC is not).
  */
#define ADC_IS_INDEPENDENT(__HANDLE__)    \
  ( ( ( ((__HANDLE__)->Instance) == ADC3) \
    )?                                    \
     SET                                  \
     :                                    \
     RESET                                \
  ) 
      

/**
  * @brief Set the sample time for Channels numbers between 0 and 9.
  * @param __SAMPLETIME__: Sample time parameter.
  * @param __CHANNELNB__: Channel number.  
  * @retval None
  */
#define ADC_SMPR1(__SAMPLETIME__, __CHANNELNB__) ((__SAMPLETIME__) << (POSITION_VAL(ADC_SMPR1_SMP1) * (__CHANNELNB__)))
    
/**
  * @brief Set the sample time for Channels numbers between 10 and 18.
  * @param __SAMPLETIME__: Sample time parameter.
  * @param __CHANNELNB__: Channel number.  
  * @retval None
  */
#define ADC_SMPR2(__SAMPLETIME__, __CHANNELNB__) ((__SAMPLETIME__) << ((POSITION_VAL(ADC_SMPR2_SMP11) * ((__CHANNELNB__) - 10))))

/**
  * @brief Write SMPR1 register.
  * @param __HANDLE__    : ADC handle.
  * @param __SAMPLETIME__: Sample time parameter.
  * @param __CHANNELNB__ : Channel number.  
  * @retval None
  */
#define ADC_SMPR1_SETTING(__HANDLE__, __SAMPLETIME__, __CHANNELNB__)           \
                    MODIFY_REG((__HANDLE__)->Instance->SMPR1,                  \
                               ADC_SMPR1(ADC_SMPR1_SMP0, (__CHANNELNB__)),     \
                               ADC_SMPR1((__SAMPLETIME__), (__CHANNELNB__)))

/**
  * @brief Write SMPR2 register.
  * @param __HANDLE__    : ADC handle.
  * @param __SAMPLETIME__: Sample time parameter.
  * @param __CHANNELNB__ : Channel number.  
  * @retval None
  */
#define ADC_SMPR2_SETTING(__HANDLE__, __SAMPLETIME__, __CHANNELNB__)           \
                    MODIFY_REG((__HANDLE__)->Instance->SMPR2,                  \
                               ADC_SMPR2(ADC_SMPR2_SMP10, (__CHANNELNB__)),    \
                               ADC_SMPR2((__SAMPLETIME__), (__CHANNELNB__)))


/**
  * @brief Set the selected regular Channel rank for rank between 1 and 4.
  * @param __CHANNELNB__: Channel number.
  * @param __RANKNB__: Rank number.    
  * @retval None
  */                                                                   
#define ADC_SQR1_RK(__CHANNELNB__, __RANKNB__) ((__CHANNELNB__) << (POSITION_VAL(ADC_SQR1_SQ1) * (__RANKNB__)))

/**
  * @brief Set the selected regular Channel rank for rank between 5 and 9.
  * @param __CHANNELNB__: Channel number.
  * @param __RANKNB__: Rank number.    
  * @retval None
  */
#define ADC_SQR2_RK(__CHANNELNB__, __RANKNB__) ((__CHANNELNB__) << (POSITION_VAL(ADC_SQR2_SQ6) * ((__RANKNB__) - 5)))

/**
  * @brief Set the selected regular Channel rank for rank between 10 and 14.
  * @param __CHANNELNB__: Channel number.
  * @param __RANKNB__: Rank number.    
  * @retval None
  */
#define ADC_SQR3_RK(__CHANNELNB__, __RANKNB__) ((__CHANNELNB__) << (POSITION_VAL(ADC_SQR3_SQ11) * ((__RANKNB__) - 10)))

/**
  * @brief Set the selected regular Channel rank for rank between 15 and 16.
  * @param __CHANNELNB__: Channel number.
  * @param __RANKNB__: Rank number.    
  * @retval None
  */
#define ADC_SQR4_RK(__CHANNELNB__, __RANKNB__) ((__CHANNELNB__) << (POSITION_VAL(ADC_SQR4_SQ16) * ((__RANKNB__) - 15)))

/**
  * @brief Set the selected injected Channel rank.
  * @param __CHANNELNB__: Channel number.
  * @param __RANKNB__: Rank number.   
  * @retval None
  */
#define ADC_JSQR_RK(__CHANNELNB__, __RANKNB__) ((__CHANNELNB__) << ((POSITION_VAL(ADC_JSQR_JSQ1)-2) * (__RANKNB__) +2))


/**
  * @brief Set the Analog Watchdog 1 channel.
  * @param __CHANNEL__: channel to be monitored by Analog Watchdog 1.
  * @retval None
  */
#define ADC_CFGR_SET_AWD1CH(__CHANNEL__) ((__CHANNEL__) << POSITION_VAL(ADC_CFGR_AWD1CH))

/**
  * @brief Configure the channel number in Analog Watchdog 2 or 3.
  * @param __CHANNEL__: ADC Channel
  * @retval None
  */
#define ADC_CFGR_SET_AWD23CR(__CHANNEL__) (1U << (__CHANNEL__)) 

/**
  * @brief Configure ADC injected context queue
  * @param __INJECT_CONTEXT_QUEUE_MODE__: Injected context queue mode.
  * @retval None
  */                                                                                            
#define ADC_CFGR_INJECT_CONTEXT_QUEUE(__INJECT_CONTEXT_QUEUE_MODE__) ((__INJECT_CONTEXT_QUEUE_MODE__) << POSITION_VAL(ADC_CFGR_JQM))

/**
  * @brief Configure ADC discontinuous conversion mode for injected group
  * @param __INJECT_DISCONTINUOUS_MODE__: Injected discontinuous mode.
  * @retval None
  */
#define ADC_CFGR_INJECT_DISCCONTINUOUS(__INJECT_DISCONTINUOUS_MODE__) ((__INJECT_DISCONTINUOUS_MODE__) <<  POSITION_VAL(ADC_CFGR_JDISCEN))

/**
  * @brief Configure ADC discontinuous conversion mode for regular group
  * @param __REG_DISCONTINUOUS_MODE__: Regular discontinuous mode.
  * @retval None
  */                                                                                                  
#define ADC_CFGR_REG_DISCONTINUOUS(__REG_DISCONTINUOUS_MODE__) ((__REG_DISCONTINUOUS_MODE__) << POSITION_VAL(ADC_CFGR_DISCEN))
/**
  * @brief Configure the number of discontinuous conversions for regular group.
  * @param __NBR_DISCONTINUOUS_CONV__: Number of discontinuous conversions.
  * @retval None
  */
#define ADC_CFGR_DISCONTINUOUS_NUM(__NBR_DISCONTINUOUS_CONV__) (((__NBR_DISCONTINUOUS_CONV__) - 1) << POSITION_VAL(ADC_CFGR_DISCNUM))

/**
  * @brief Configure the ADC auto delay mode.
  * @param __AUTOWAIT__: Auto delay bit enable or disable.
  * @retval None
  */
#define ADC_CFGR_AUTOWAIT(__AUTOWAIT__) ((__AUTOWAIT__) << POSITION_VAL(ADC_CFGR_AUTDLY))

/**
  * @brief Configure ADC continuous conversion mode.
  * @param __CONTINUOUS_MODE__: Continuous mode.
  * @retval None
  */
#define ADC_CFGR_CONTINUOUS(__CONTINUOUS_MODE__) ((__CONTINUOUS_MODE__) << POSITION_VAL(ADC_CFGR_CONT))
    
/**
  * @brief Configure the ADC DMA continuous request.
  * @param __DMACONTREQ_MODE__: DMA continuous request mode.
  * @retval None
  */                                                                              
#define ADC_CFGR_DMACONTREQ(__DMACONTREQ_MODE__) ((__DMACONTREQ_MODE__) <<  POSITION_VAL(ADC_CFGR_DMACFG))


/**
  * @brief Configure the channel number into offset OFRx register.
  * @param __CHANNEL__: ADC Channel.
  * @retval None
  */
#define ADC_OFR_CHANNEL(__CHANNEL__) ((__CHANNEL__) << POSITION_VAL(ADC_OFR1_OFFSET1_CH))

/**
  * @brief Configure the channel number into differential mode selection register.
  * @param __CHANNEL__: ADC Channel.
  * @retval None
  */
#define ADC_DIFSEL_CHANNEL(__CHANNEL__) (1U << (__CHANNEL__)) 

/**
  * @brief Configure calibration factor in differential mode to be set into calibration register.
  * @param __CALIBRATION_FACTOR__: Calibration factor value.
  * @retval None
  */
#define ADC_CALFACT_DIFF_SET(__CALIBRATION_FACTOR__) (((__CALIBRATION_FACTOR__) & (ADC_CALFACT_CALFACT_D >> POSITION_VAL(ADC_CALFACT_CALFACT_D)) ) << POSITION_VAL(ADC_CALFACT_CALFACT_D))
/**
  * @brief Calibration factor in differential mode to be retrieved from calibration register.
  * @param __CALIBRATION_FACTOR__: Calibration factor value.
  * @retval None
  */                                                                                
#define ADC_CALFACT_DIFF_GET(__CALIBRATION_FACTOR__) ((__CALIBRATION_FACTOR__) >> POSITION_VAL(ADC_CALFACT_CALFACT_D))
     
/**
  * @brief Configure the analog watchdog high threshold into registers TR1, TR2 or TR3.
  * @param __THRESHOLD__: Threshold value.
  * @retval None
  */
#define ADC_TRX_HIGHTHRESHOLD(__THRESHOLD__) ((__THRESHOLD__) << 16)

/**
  * @brief Configure the ADC DMA continuous request for ADC multimode.
  * @param __DMACONTREQ_MODE__: DMA continuous request mode.
  * @retval None
  */                                                                               
#define ADC_CCR_MULTI_DMACONTREQ(__DMACONTREQ_MODE__) ((__DMACONTREQ_MODE__) << POSITION_VAL(ADC_CCR_DMACFG))

/**
  * @brief Enable the ADC peripheral.
  * @param __HANDLE__: ADC handle.
  * @retval None
  */
#define ADC_ENABLE(__HANDLE__) ((__HANDLE__)->Instance->CR |= ADC_CR_ADEN)

/**
  * @brief Verification of hardware constraints before ADC can be enabled.
  * @param __HANDLE__: ADC handle.
  * @retval SET (ADC can be enabled) or RESET (ADC cannot be enabled)
  */
#define ADC_ENABLING_CONDITIONS(__HANDLE__)                             \
       (( ( ((__HANDLE__)->Instance->CR) &                                    \
            (ADC_CR_ADCAL | ADC_CR_JADSTP | ADC_CR_ADSTP | ADC_CR_JADSTART |  \
             ADC_CR_ADSTART | ADC_CR_ADDIS | ADC_CR_ADEN                    ) \
           ) == RESET                                                         \
        ) ? SET : RESET)
         
/**
  * @brief Disable the ADC peripheral.
  * @param __HANDLE__: ADC handle.
  * @retval None
  */
#define ADC_DISABLE(__HANDLE__)                                          \
  do{                                                                          \
         (__HANDLE__)->Instance->CR |= ADC_CR_ADDIS;                           \
          __HAL_ADC_CLEAR_FLAG((__HANDLE__), (ADC_FLAG_EOSMP | ADC_FLAG_RDY)); \
  } while(0)
    
/**
  * @brief Verification of hardware constraints before ADC can be disabled.
  * @param __HANDLE__: ADC handle.
  * @retval SET (ADC can be disabled) or RESET (ADC cannot be disabled)
  */
#define ADC_DISABLING_CONDITIONS(__HANDLE__)                             \
       (( ( ((__HANDLE__)->Instance->CR) &                                     \
            (ADC_CR_JADSTART | ADC_CR_ADSTART | ADC_CR_ADEN)) == ADC_CR_ADEN   \
        ) ? SET : RESET)
         

/**
  * @brief Shift the offset with respect to the selected ADC resolution. 
  * @note   Offset has to be left-aligned on bit 11, the LSB (right bits) are set to 0.
  *         If resolution 12 bits, no shift.
  *         If resolution 10 bits, shift of 2 ranks on the left.
  *         If resolution 8 bits, shift of 4 ranks on the left.
  *         If resolution 6 bits, shift of 6 ranks on the left.
  *         Therefore, shift = (12 - resolution) = 12 - (12- (((RES[1:0]) >> 3)*2)).
  * @param __HANDLE__: ADC handle
  * @param __OFFSET__: Value to be shifted
  * @retval None
  */
#define ADC_OFFSET_SHIFT_RESOLUTION(__HANDLE__, __OFFSET__) \
        ((__OFFSET__) << ((((__HANDLE__)->Instance->CFGR & ADC_CFGR_RES) >> 3)*2))


/**
  * @brief Shift the AWD1 threshold with respect to the selected ADC resolution.
  * @note  Thresholds have to be left-aligned on bit 11, the LSB (right bits) are set to 0.
  *        If resolution 12 bits, no shift.
  *        If resolution 10 bits, shift of 2 ranks on the left.
  *        If resolution 8 bits, shift of 4 ranks on the left.
  *        If resolution 6 bits, shift of 6 ranks on the left.
  *        Therefore, shift = (12 - resolution) = 12 - (12- (((RES[1:0]) >> 3)*2)).
  * @param __HANDLE__: ADC handle
  * @param __THRESHOLD__: Value to be shifted
  * @retval None
  */
#define ADC_AWD1THRESHOLD_SHIFT_RESOLUTION(__HANDLE__, __THRESHOLD__) \
        ((__THRESHOLD__) << ((((__HANDLE__)->Instance->CFGR & ADC_CFGR_RES) >> 3)*2))

/**
  * @brief Shift the AWD2 and AWD3 threshold with respect to the selected ADC resolution.
  * @note  Thresholds have to be left-aligned on bit 7.
  *        If resolution 12 bits, shift of 4 ranks on the right (the 4 LSB are discarded).
  *        If resolution 10 bits, shift of 2 ranks on the right (the 2 LSB are discarded).
  *        If resolution 8 bits, no shift.
  *        If resolution 6 bits, shift of 2 ranks on the left (the 2 LSB are set to 0).
  * @param __HANDLE__: ADC handle
  * @param __THRESHOLD__: Value to be shifted
  * @retval None
  */
#define ADC_AWD23THRESHOLD_SHIFT_RESOLUTION(__HANDLE__, __THRESHOLD__) \
         ( ((__HANDLE__)->Instance->CFGR & ADC_CFGR_RES) != (ADC_CFGR_RES_1 | ADC_CFGR_RES_0) ? \
            ((__THRESHOLD__) >> (4- ((((__HANDLE__)->Instance->CFGR & ADC_CFGR_RES) >> 3)*2))) : \
            (__THRESHOLD__) << 2 )

        
/**
  * @brief Report ADC common register.
  * @param __HANDLE__: ADC handle.
  * @retval Common control register
  */
#define ADC_COMMON_REGISTER(__HANDLE__)   (ADC123_COMMON)       

/**
  * @brief Report Master Instance.
  * @param __HANDLE__: ADC handle.
  * @note return same instance if ADC of input handle is independent ADC.  
  * @retval Master Instance
  */
#define ADC_MASTER_REGISTER(__HANDLE__)                                          \
  ( ( ((((__HANDLE__)->Instance) == ADC1) || (((__HANDLE__)->Instance) == ADC3)) \
    )?                                                                           \
     ((__HANDLE__)->Instance)                                                    \
     :                                                                           \
     (ADC1)                                                                      \
  )
       

/**
  * @brief Clear Common Control Register.
  * @param __HANDLE__: ADC handle.     
  * @retval None
  */
#define ADC_CLEAR_COMMON_CONTROL_REGISTER(__HANDLE__) CLEAR_BIT(ADC_COMMON_REGISTER(__HANDLE__)->CCR, ADC_CCR_CKMODE | \
                                                                                                      ADC_CCR_PRESC  | \
                                                                                                      ADC_CCR_VBATEN | \
                                                                                                      ADC_CCR_TSEN   | \
                                                                                                      ADC_CCR_VREFEN | \
                                                                                                      ADC_CCR_MDMA   | \
                                                                                                      ADC_CCR_DMACFG | \
                                                                                                      ADC_CCR_DELAY  | \
                                                                                                      ADC_CCR_DUAL  )
                                                      
                                                      
/**                                                   
  * @brief Check whether or not dual conversions are enabled.
  * @param __HANDLE__: ADC handle.
  * @note Return RESET if ADC of input handle is independent ADC.   
  * @retval SET (dual regular conversions are enabled) or RESET (ADC is independent or no dual regular conversions are enabled)
  */
#define ADC_IS_DUAL_CONVERSION_ENABLE(__HANDLE__)                                \
  ( ( ((((__HANDLE__)->Instance) == ADC1) || (((__HANDLE__)->Instance) == ADC2)) \
    )?                                                                           \
     ( ((ADC123_COMMON->CCR & ADC_CCR_DUAL) != ADC_MODE_INDEPENDENT)  )          \
     :                                                                           \
     RESET                                                                       \
  )
       
/**
  * @brief Check whether or not dual regular conversions are enabled.
  * @param __HANDLE__: ADC handle.
  * @retval SET (dual regular conversions are enabled) or RESET (ADC is independent or no dual regular conversions are enabled)
  */
#define ADC_IS_DUAL_REGULAR_CONVERSION_ENABLE(__HANDLE__)                        \
  ( ( ((((__HANDLE__)->Instance) == ADC1) || (((__HANDLE__)->Instance) == ADC2)) \
    )?                                                                           \
     ( (((ADC_COMMON_REGISTER(__HANDLE__))->CCR & ADC_CCR_DUAL) != ADC_MODE_INDEPENDENT)     &&      \
       (((ADC_COMMON_REGISTER(__HANDLE__))->CCR & ADC_CCR_DUAL) != ADC_DUALMODE_INJECSIMULT) &&      \
       (((ADC_COMMON_REGISTER(__HANDLE__))->CCR & ADC_CCR_DUAL) != ADC_DUALMODE_ALTERTRIG) )         \
     :                                                                           \
     RESET                                                                       \
  )
                        

/**
  * @brief Verification of condition for ADC start conversion: ADC must be in non-multimode or multimode with handle of ADC master.
  * @param __HANDLE__: ADC handle.
  * @retval SET (non-multimode or Master handle) or RESET (handle of Slave ADC in multimode)
  */
#define ADC_NONMULTIMODE_OR_MULTIMODEMASTER(__HANDLE__)                        \
  ( ( ((__HANDLE__)->Instance == ADC1) || ((__HANDLE__)->Instance == ADC3)     \
    )?                                                                         \
     SET                                                                       \
     :                                                                         \
     ((ADC123_COMMON->CCR & ADC_CCR_DUAL) == RESET)                            \
  )
  
/**
  * @brief Ensure ADC Instance is Independent or Master, or is not Slave ADC with dual regular conversions enabled.
  * @param __HANDLE__: ADC handle.
  * @retval SET (Independent or Master, or Slave without dual regular conversions enabled) or RESET (Slave ADC with dual regular conversions enabled)
  */
#define ADC_INDEPENDENT_OR_NONMULTIMODEREGULAR_SLAVE(__HANDLE__)            \
  ( ( ((__HANDLE__)->Instance == ADC1) || ((__HANDLE__)->Instance == ADC3)  \
    )?                                                                      \
     SET                                                                    \
     :                                                                      \
     ( ((ADC123_COMMON->CCR & ADC_CCR_DUAL) == ADC_MODE_INDEPENDENT)     || \
       ((ADC123_COMMON->CCR & ADC_CCR_DUAL) == ADC_DUALMODE_INJECSIMULT) || \
       ((ADC123_COMMON->CCR & ADC_CCR_DUAL) == ADC_DUALMODE_ALTERTRIG) ))  

/**
  * @brief Ensure ADC Instance is Independent or Master, or is not Slave ADC with dual injected conversions enabled.
  * @param __HANDLE__: ADC handle.
  * @retval SET (non-multimode or Master, or Slave without dual injected conversions enabled) or RESET (Slave ADC with dual injected conversions enabled)
  */
#define ADC_INDEPENDENT_OR_NONMULTIMODEINJECTED_SLAVE(__HANDLE__)          \
  ( ( ((__HANDLE__)->Instance == ADC1) || ((__HANDLE__)->Instance == ADC3) \
    )?                                                                     \
     SET                                                                   \
     :                                                                     \
     ( ((ADC123_COMMON->CCR & ADC_CCR_DUAL) == ADC_MODE_INDEPENDENT)    || \
       ((ADC123_COMMON->CCR & ADC_CCR_DUAL) == ADC_DUALMODE_REGSIMULT)  || \
       ((ADC123_COMMON->CCR & ADC_CCR_DUAL) == ADC_DUALMODE_INTERL) ))
  
/**
  * @brief Verification of ADC state: enabled or disabled, directly checked on instance as input parameter.
  * @param __INSTANCE__: ADC instance.
  * @retval SET (ADC enabled) or RESET (ADC disabled)
  */
#define ADC_INSTANCE_IS_ENABLED(__INSTANCE__)                                       \
       (( ((((__INSTANCE__)->CR) & (ADC_CR_ADEN | ADC_CR_ADDIS)) == ADC_CR_ADEN) && \
          ((((__INSTANCE__)->ISR) & ADC_FLAG_RDY) == ADC_FLAG_RDY)                  \
        ) ? SET : RESET)  
  
/**
  * @brief Verification of enabled/disabled status of ADCs other than that associated to the input parameter handle.
  * @param __HANDLE__: ADC handle.
  * @retval SET (at least one other ADC is enabled) or RESET (no other ADC is enabled, all other ADCs are disabled)
  */ 
#define ADC_ANY_OTHER_ENABLED(__HANDLE__)                                   \
  ( ( ((__HANDLE__)->Instance == ADC1)                                      \
    )?                                                                      \
     (ADC_INSTANCE_IS_ENABLED(ADC2)) || (ADC_INSTANCE_IS_ENABLED(ADC3))     \
     :                                                                      \
     ( ( ((__HANDLE__)->Instance == ADC2)                                   \
       )?                                                                   \
         (ADC_INSTANCE_IS_ENABLED(ADC1)) || (ADC_INSTANCE_IS_ENABLED(ADC3)) \
        :                                                                   \
          ADC_INSTANCE_IS_ENABLED(ADC1)) || (ADC_INSTANCE_IS_ENABLED(ADC2)) \
     )


/**
  * @brief Set handle instance of the ADC slave associated to the ADC master.
  * @param __HANDLE_MASTER__: ADC master handle.
  * @param __HANDLE_SLAVE__: ADC slave handle.
  * @note if __HANDLE_MASTER__ is the handle of a slave ADC (ADC2) or an independent ADC (ADC3), __HANDLE_SLAVE__ instance is set to NULL.
  * @retval None
  */
#define ADC_MULTI_SLAVE(__HANDLE_MASTER__, __HANDLE_SLAVE__)             \
  ( (((__HANDLE_MASTER__)->Instance == ADC1)) ? ((__HANDLE_SLAVE__)->Instance = ADC2) : ((__HANDLE_SLAVE__)->Instance = NULL) ) 
 

/**
  * @brief Check whether or not multimode is configured in DMA mode.
  * @retval SET (multimode is configured in DMA mode) or RESET (DMA multimode is disabled)
  */ 
#define ADC_MULTIMODE_DMA_ENABLED()                                     \
    ((READ_BIT(ADC123_COMMON->CCR, ADC_CCR_MDMA) == ADC_DMAACCESSMODE_12_10_BITS) \
  || (READ_BIT(ADC123_COMMON->CCR, ADC_CCR_MDMA) == ADC_DMAACCESSMODE_8_6_BITS))  
 

/**
  * @brief Verify the ADC instance connected to the temperature sensor.
  * @param __HANDLE__: ADC handle.
  * @retval SET (ADC instance is valid) or RESET (ADC instance is invalid)
  */  
/*  The temperature sensor measurement path (channel 17) is available on ADC1 and ADC3 */                        
#define ADC_TEMPERATURE_SENSOR_INSTANCE(__HANDLE__)  ((((__HANDLE__)->Instance) == ADC1) || (((__HANDLE__)->Instance) == ADC3))

/**
  * @brief Verify the ADC instance connected to the battery voltage VBAT.
  * @param __HANDLE__: ADC handle.
  * @retval SET (ADC instance is valid) or RESET (ADC instance is invalid)
  */  
/*  The battery voltage measurement path (channel 18) is available on ADC1 and ADC3 */                        
#define ADC_BATTERY_VOLTAGE_INSTANCE(__HANDLE__)  ((((__HANDLE__)->Instance) == ADC1) || (((__HANDLE__)->Instance) == ADC3))

/**
  * @brief Verify the ADC instance connected to the internal voltage reference VREFINT.
  * @param __HANDLE__: ADC handle.
  * @retval SET (ADC instance is valid) or RESET (ADC instance is invalid)
  */  
/*  The internal voltage reference  VREFINT measurement path (channel 0) is available on ADC1 */  
#define ADC_VREFINT_INSTANCE(__HANDLE__)  (((__HANDLE__)->Instance) == ADC1)

 
/**
  * @brief Verify the length of scheduled injected conversions group.
  * @param __LENGTH__: number of programmed conversions.   
  * @retval SET (__LENGTH__ is within the maximum number of possible programmable injected conversions) or RESET (__LENGTH__ is null or too large)
  */  
#define IS_ADC_INJECTED_NB_CONV(__LENGTH__) (((__LENGTH__) >= ((uint32_t)1)) && ((__LENGTH__) <= ((uint32_t)4)))
  
  
/**
  * @brief Calibration factor size verification (7 bits maximum).
  * @param __CALIBRATION_FACTOR__: Calibration factor value.
  * @retval SET (__CALIBRATION_FACTOR__ is within the authorized size) or RESET (__CALIBRATION_FACTOR__ is too large)
  */
#define IS_ADC_CALFACT(__CALIBRATION_FACTOR__) ((__CALIBRATION_FACTOR__) <= ((uint32_t)0x7F))

 
/**
  * @brief Verify the ADC channel setting.
  * @param __HANDLE__: ADC handle.
  * @param __CHANNEL__: programmed ADC channel. 
  * @retval SET (__CHANNEL__ is valid) or RESET (__CHANNEL__ is invalid)
  */
#define IS_ADC_CHANNEL(__HANDLE__, __CHANNEL__)  (((((__HANDLE__)->Instance) == ADC1)  && \
                                                         (((__CHANNEL__) == ADC_CHANNEL_VREFINT)     || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_1)           || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_2)           || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_3)           || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_4)           || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_5)           || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_6)           || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_7)           || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_8)           || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_9)           || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_10)          || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_11)          || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_12)          || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_13)          || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_14)          || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_15)          || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_16)          || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_TEMPSENSOR)  || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_VBAT)))      || \
                                                        ((((__HANDLE__)->Instance) == ADC2)  && \
                                                         (((__CHANNEL__) == ADC_CHANNEL_1)           || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_2)           || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_3)           || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_4)           || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_5)           || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_6)           || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_7)           || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_8)           || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_9)           || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_10)          || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_11)          || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_12)          || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_13)          || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_14)          || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_15)          || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_16)          || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_17)          || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_18)))        || \
                                                        ((((__HANDLE__)->Instance) == ADC3)  && \
                                                         (((__CHANNEL__) == ADC_CHANNEL_1)           || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_2)           || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_3)           || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_4)           || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_6)           || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_7)           || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_8)           || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_9)           || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_10)          || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_11)          || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_12)          || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_13)          || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_14)          || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_15)          || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_TEMPSENSOR)  || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_VBAT)   ))) 
 
/**
  * @brief Verify the ADC channel setting in differential mode.
  * @param __HANDLE__: ADC handle.
  * @param __CHANNEL__: programmed ADC channel. 
  * @retval SET (__CHANNEL__ is valid) or RESET (__CHANNEL__ is invalid)
  */  
    /* For ADC1 and ADC2, channels 1 to 15 are available in differential mode, 
                          channels 0, 16 to 18 can be only used in single-ended mode. 
       For ADC3, channels 1 to 3 and 6 to 12 are available in differential mode,
                 channels 4, 5 and 13 to 18 can only be used in single-ended mode.  */                         
#define IS_ADC_DIFF_CHANNEL(__HANDLE__, __CHANNEL__)  ((((((__HANDLE__)->Instance) == ADC1)   || \
                                                         (((__HANDLE__)->Instance) == ADC2))  && \
                                                         (((__CHANNEL__) == ADC_CHANNEL_1)    || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_2)    || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_3)    || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_4)    || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_5)    || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_6)    || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_7)    || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_8)    || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_9)    || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_10)   || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_11)   || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_12)   || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_13)   || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_14)   || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_15))) || \
                                                        ((((__HANDLE__)->Instance) == ADC3)  && \
                                                         (((__CHANNEL__) == ADC_CHANNEL_1)   || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_2)   || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_3)   || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_6)   || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_7)   || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_8)   || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_9)   || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_10)  || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_11)  || \
                                                          ((__CHANNEL__) == ADC_CHANNEL_12)   )))
                               
/**                            
  * @brief Verify the ADC single-ended input or differential mode setting.
  * @param __SING_DIFF__: programmed channel setting. 
  * @retval SET (__SING_DIFF__ is valid) or RESET (__SING_DIFF__ is invalid)
  */                           
#define IS_ADC_SINGLE_DIFFERENTIAL(__SING_DIFF__) (((__SING_DIFF__) == ADC_SINGLE_ENDED)      || \
                                                   ((__SING_DIFF__) == ADC_DIFFERENTIAL_ENDED)  )
                               
/**
  * @brief Verify the ADC offset management setting.
  * @param __OFFSET_NUMBER__: ADC offset management. 
  * @retval SET (__OFFSET_NUMBER__ is valid) or RESET (__OFFSET_NUMBER__ is invalid)
  */ 
#define IS_ADC_OFFSET_NUMBER(__OFFSET_NUMBER__) (((__OFFSET_NUMBER__) == ADC_OFFSET_NONE) || \
                                                 ((__OFFSET_NUMBER__) == ADC_OFFSET_1)    || \
                                                 ((__OFFSET_NUMBER__) == ADC_OFFSET_2)    || \
                                                 ((__OFFSET_NUMBER__) == ADC_OFFSET_3)    || \
                                                 ((__OFFSET_NUMBER__) == ADC_OFFSET_4)      ) 

/**
  * @brief Verify the ADC regular channel setting.
  * @param __CHANNEL__: programmed ADC regular channel. 
  * @retval SET (__CHANNEL__ is valid) or RESET (__CHANNEL__ is invalid)
  */
#define IS_ADC_REGULAR_RANK(__CHANNEL__) (((__CHANNEL__) == ADC_REGULAR_RANK_1 ) || \
                                          ((__CHANNEL__) == ADC_REGULAR_RANK_2 ) || \
                                          ((__CHANNEL__) == ADC_REGULAR_RANK_3 ) || \
                                          ((__CHANNEL__) == ADC_REGULAR_RANK_4 ) || \
                                          ((__CHANNEL__) == ADC_REGULAR_RANK_5 ) || \
                                          ((__CHANNEL__) == ADC_REGULAR_RANK_6 ) || \
                                          ((__CHANNEL__) == ADC_REGULAR_RANK_7 ) || \
                                          ((__CHANNEL__) == ADC_REGULAR_RANK_8 ) || \
                                          ((__CHANNEL__) == ADC_REGULAR_RANK_9 ) || \
                                          ((__CHANNEL__) == ADC_REGULAR_RANK_10) || \
                                          ((__CHANNEL__) == ADC_REGULAR_RANK_11) || \
                                          ((__CHANNEL__) == ADC_REGULAR_RANK_12) || \
                                          ((__CHANNEL__) == ADC_REGULAR_RANK_13) || \
                                          ((__CHANNEL__) == ADC_REGULAR_RANK_14) || \
                                          ((__CHANNEL__) == ADC_REGULAR_RANK_15) || \
                                          ((__CHANNEL__) == ADC_REGULAR_RANK_16)   )


/**
  * @brief Verify the ADC injected channel setting.
  * @param __CHANNEL__: programmed ADC injected channel. 
  * @retval SET (__CHANNEL__ is valid) or RESET (__CHANNEL__ is invalid)
  */
#define IS_ADC_INJECTED_RANK(__CHANNEL__) (((__CHANNEL__) == ADC_INJECTED_RANK_1) || \
                                           ((__CHANNEL__) == ADC_INJECTED_RANK_2) || \
                                           ((__CHANNEL__) == ADC_INJECTED_RANK_3) || \
                                           ((__CHANNEL__) == ADC_INJECTED_RANK_4)   ) 

/**
  * @brief Verify the ADC edge trigger setting for injected group.
  * @param __EDGE__: programmed ADC edge trigger setting.
  * @retval SET (__EDGE__ is a valid value) or RESET (__EDGE__ is invalid)
  */ 
#define IS_ADC_EXTTRIGINJEC_EDGE(__EDGE__) (((__EDGE__) == ADC_EXTERNALTRIGINJECCONV_EDGE_NONE)        || \
                                           ((__EDGE__) == ADC_EXTERNALTRIGINJECCONV_EDGE_RISING)       || \
                                           ((__EDGE__) == ADC_EXTERNALTRIGINJECCONV_EDGE_FALLING)      || \
                                           ((__EDGE__) == ADC_EXTERNALTRIGINJECCONV_EDGE_RISINGFALLING) ) 


/**
  * @brief Verify the ADC injected conversions external trigger.
  * @param __INJTRIG__: programmed ADC injected conversions external trigger.
  * @retval SET (__INJTRIG__ is a valid value) or RESET (__INJTRIG__ is invalid)
  */ 
#define IS_ADC_EXTTRIGINJEC(__INJTRIG__) (((__INJTRIG__) == ADC_EXTERNALTRIGINJEC_T1_TRGO)     || \
                                          ((__INJTRIG__) == ADC_EXTERNALTRIGINJEC_T1_CC4)      || \
                                          ((__INJTRIG__) == ADC_EXTERNALTRIGINJEC_T2_TRGO)     || \
                                          ((__INJTRIG__) == ADC_EXTERNALTRIGINJEC_T2_CC1)      || \
                                          ((__INJTRIG__) == ADC_EXTERNALTRIGINJEC_T3_CC4)      || \
                                          ((__INJTRIG__) == ADC_EXTERNALTRIGINJEC_T4_TRGO)     || \
                                          ((__INJTRIG__) == ADC_EXTERNALTRIGINJEC_EXT_IT15)    || \
                                          ((__INJTRIG__) == ADC_EXTERNALTRIGINJEC_T8_CC4)      || \
                                          ((__INJTRIG__) == ADC_EXTERNALTRIGINJEC_T1_TRGO2)    || \
                                          ((__INJTRIG__) == ADC_EXTERNALTRIGINJEC_T8_TRGO)     || \
                                          ((__INJTRIG__) == ADC_EXTERNALTRIGINJEC_T8_TRGO2)    || \
                                          ((__INJTRIG__) == ADC_EXTERNALTRIGINJEC_T3_CC3)      || \
                                          ((__INJTRIG__) == ADC_EXTERNALTRIGINJEC_T3_TRGO)     || \
                                          ((__INJTRIG__) == ADC_EXTERNALTRIGINJEC_T3_CC1)      || \
                                          ((__INJTRIG__) == ADC_EXTERNALTRIGINJEC_T6_TRGO)     || \
                                          ((__INJTRIG__) == ADC_EXTERNALTRIGINJEC_T15_TRGO)    || \
                                                                                                  \
                                          ((__INJTRIG__) == ADC_SOFTWARE_START)                   )  


/**
  * @brief Verify the ADC multimode setting.
  * @param __MODE__: programmed ADC multimode setting.
  * @retval SET (__MODE__ is valid) or RESET (__MODE__ is invalid)
  */ 
#define IS_ADC_MULTIMODE(__MODE__) (((__MODE__) == ADC_MODE_INDEPENDENT)          || \
                               ((__MODE__) == ADC_DUALMODE_REGSIMULT_INJECSIMULT) || \
                               ((__MODE__) == ADC_DUALMODE_REGSIMULT_ALTERTRIG)   || \
                               ((__MODE__) == ADC_DUALMODE_REGINTERL_INJECSIMULT) || \
                               ((__MODE__) == ADC_DUALMODE_INJECSIMULT)           || \
                               ((__MODE__) == ADC_DUALMODE_REGSIMULT)             || \
                               ((__MODE__) == ADC_DUALMODE_INTERL)                || \
                               ((__MODE__) == ADC_DUALMODE_ALTERTRIG)               ) 

/**
  * @brief Verify the ADC multimode DMA access setting.
  * @param __MODE__: programmed ADC multimode DMA access setting.
  * @retval SET (__MODE__ is valid) or RESET (__MODE__ is invalid)
  */
#define IS_ADC_DMA_ACCESS_MULTIMODE(__MODE__) (((__MODE__) == ADC_DMAACCESSMODE_DISABLED)   || \
                                               ((__MODE__) == ADC_DMAACCESSMODE_12_10_BITS) || \
                                               ((__MODE__) == ADC_DMAACCESSMODE_8_6_BITS)     ) 

/**
  * @brief Verify the ADC multimode delay setting.
  * @param __DELAY__: programmed ADC multimode delay setting.
  * @retval SET (__DELAY__ is a valid value) or RESET (__DELAY__ is invalid)
  */
#define IS_ADC_SAMPLING_DELAY(__DELAY__) (((__DELAY__) == ADC_TWOSAMPLINGDELAY_1CYCLE)   || \
                                          ((__DELAY__) == ADC_TWOSAMPLINGDELAY_2CYCLES)  || \
                                          ((__DELAY__) == ADC_TWOSAMPLINGDELAY_3CYCLES)  || \
                                          ((__DELAY__) == ADC_TWOSAMPLINGDELAY_4CYCLES)  || \
                                          ((__DELAY__) == ADC_TWOSAMPLINGDELAY_5CYCLES)  || \
                                          ((__DELAY__) == ADC_TWOSAMPLINGDELAY_6CYCLES)  || \
                                          ((__DELAY__) == ADC_TWOSAMPLINGDELAY_7CYCLES)  || \
                                          ((__DELAY__) == ADC_TWOSAMPLINGDELAY_8CYCLES)  || \
                                          ((__DELAY__) == ADC_TWOSAMPLINGDELAY_9CYCLES)  || \
                                          ((__DELAY__) == ADC_TWOSAMPLINGDELAY_10CYCLES) || \
                                          ((__DELAY__) == ADC_TWOSAMPLINGDELAY_11CYCLES) || \
                                          ((__DELAY__) == ADC_TWOSAMPLINGDELAY_12CYCLES)   ) 

/**
  * @brief Verify the ADC analog watchdog setting.
  * @param __WATCHDOG__: programmed ADC analog watchdog setting.
  * @retval SET (__WATCHDOG__ is valid) or RESET (__WATCHDOG__ is invalid)
  */
#define IS_ADC_ANALOG_WATCHDOG_NUMBER(__WATCHDOG__) (((__WATCHDOG__) == ADC_ANALOGWATCHDOG_1) || \
                                                     ((__WATCHDOG__) == ADC_ANALOGWATCHDOG_2) || \
                                                     ((__WATCHDOG__) == ADC_ANALOGWATCHDOG_3)   ) 

/**
  * @brief Verify the ADC analog watchdog mode setting.
  * @param __WATCHDOG_MODE__: programmed ADC analog watchdog mode setting.
  * @retval SET (__WATCHDOG_MODE__ is valid) or RESET (__WATCHDOG_MODE__ is invalid)
  */
#define IS_ADC_ANALOG_WATCHDOG_MODE(__WATCHDOG_MODE__) (((__WATCHDOG_MODE__) == ADC_ANALOGWATCHDOG_NONE)             || \
                                                        ((__WATCHDOG_MODE__) == ADC_ANALOGWATCHDOG_SINGLE_REG)       || \
                                                        ((__WATCHDOG_MODE__) == ADC_ANALOGWATCHDOG_SINGLE_INJEC)     || \
                                                        ((__WATCHDOG_MODE__) == ADC_ANALOGWATCHDOG_SINGLE_REGINJEC)  || \
                                                        ((__WATCHDOG_MODE__) == ADC_ANALOGWATCHDOG_ALL_REG)          || \
                                                        ((__WATCHDOG_MODE__) == ADC_ANALOGWATCHDOG_ALL_INJEC)        || \
                                                        ((__WATCHDOG_MODE__) == ADC_ANALOGWATCHDOG_ALL_REGINJEC)       ) 

/**
  * @brief Verify the ADC conversion (regular or injected or both).
  * @param __CONVERSION__: ADC conversion group.
  * @retval SET (__CONVERSION__ is valid) or RESET (__CONVERSION__ is invalid)
  */
#define IS_ADC_CONVERSION_GROUP(__CONVERSION__) (((__CONVERSION__) == ADC_REGULAR_GROUP)     || \
                                             ((__CONVERSION__) == ADC_INJECTED_GROUP)        || \
                                             ((__CONVERSION__) == ADC_REGULAR_INJECTED_GROUP)  )

/**
  * @brief Verify the ADC event type.
  * @param __EVENT__: ADC event.
  * @retval SET (__EVENT__ is valid) or RESET (__EVENT__ is invalid)
  */
#define IS_ADC_EVENT_TYPE(__EVENT__) (((__EVENT__) == ADC_EOSMP_EVENT) || \
                                     ((__EVENT__) == ADC_AWD_EVENT)    || \
                                     ((__EVENT__) == ADC_AWD2_EVENT)   || \
                                     ((__EVENT__) == ADC_AWD3_EVENT)   || \
                                     ((__EVENT__) == ADC_OVR_EVENT)    || \
                                     ((__EVENT__) == ADC_JQOVF_EVENT)  ) 

/**
  * @brief Verify the ADC oversampling ratio. 
  * @param __RATIO__: programmed ADC oversampling ratio.
  * @retval SET (__RATIO__ is a valid value) or RESET (__RATIO__ is invalid)
  */   
#define IS_ADC_OVERSAMPLING_RATIO(__RATIO__)      (((__RATIO__) == ADC_OVERSAMPLING_RATIO_2   ) || \
                                                   ((__RATIO__) == ADC_OVERSAMPLING_RATIO_4   ) || \
                                                   ((__RATIO__) == ADC_OVERSAMPLING_RATIO_8   ) || \
                                                   ((__RATIO__) == ADC_OVERSAMPLING_RATIO_16  ) || \
                                                   ((__RATIO__) == ADC_OVERSAMPLING_RATIO_32  ) || \
                                                   ((__RATIO__) == ADC_OVERSAMPLING_RATIO_64  ) || \
                                                   ((__RATIO__) == ADC_OVERSAMPLING_RATIO_128 ) || \
                                                   ((__RATIO__) == ADC_OVERSAMPLING_RATIO_256 ))

/**
  * @brief Verify the ADC oversampling shift. 
  * @param __SHIFT__: programmed ADC oversampling shift.
  * @retval SET (__SHIFT__ is a valid value) or RESET (__SHIFT__ is invalid)
  */                 
#define IS_ADC_RIGHT_BIT_SHIFT(__SHIFT__)        (((__SHIFT__) == ADC_RIGHTBITSHIFT_NONE) || \
                                                  ((__SHIFT__) == ADC_RIGHTBITSHIFT_1   ) || \
                                                  ((__SHIFT__) == ADC_RIGHTBITSHIFT_2   ) || \
                                                  ((__SHIFT__) == ADC_RIGHTBITSHIFT_3   ) || \
                                                  ((__SHIFT__) == ADC_RIGHTBITSHIFT_4   ) || \
                                                  ((__SHIFT__) == ADC_RIGHTBITSHIFT_5   ) || \
                                                  ((__SHIFT__) == ADC_RIGHTBITSHIFT_6   ) || \
                                                  ((__SHIFT__) == ADC_RIGHTBITSHIFT_7   ) || \
                                                  ((__SHIFT__) == ADC_RIGHTBITSHIFT_8   ))

/**
  * @brief Verify the ADC oversampling triggered mode. 
  * @param __MODE__: programmed ADC oversampling triggered mode. 
  * @retval SET (__MODE__ is valid) or RESET (__MODE__ is invalid)
  */ 
#define IS_ADC_TRIGGERED_OVERSAMPLING_MODE(__MODE__) (((__MODE__) == ADC_TRIGGEREDMODE_SINGLE_TRIGGER) || \
                                                      ((__MODE__) == ADC_TRIGGEREDMODE_MULTI_TRIGGER) ) 

/**
  * @brief Verify the ADC oversampling regular conversion resumed or continued mode. 
  * @param __MODE__: programmed ADC oversampling regular conversion resumed or continued mode. 
  * @retval SET (__MODE__ is valid) or RESET (__MODE__ is invalid)
  */ 
#define IS_ADC_REGOVERSAMPLING_MODE(__MODE__) (((__MODE__) == ADC_REGOVERSAMPLING_CONTINUED_MODE) || \
                                               ((__MODE__) == ADC_REGOVERSAMPLING_RESUMED_MODE) )              

 

                                                  
/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/  
/** @addtogroup ADCEx_Exported_Functions ADC Extended Exported Functions
  * @{
  */ 
          
/* Initialization/de-initialization functions *********************************/

/** @addtogroup ADCEx_Exported_Functions_Group1 Extended Input and Output operation functions
  * @brief    Extended IO operation functions
  * @{
  */ 
/* I/O operation functions ****************************************************/

/* ADC calibration */

HAL_StatusTypeDef       HAL_ADCEx_Calibration_Start(ADC_HandleTypeDef* hadc, uint32_t SingleDiff);
uint32_t                HAL_ADCEx_Calibration_GetValue(ADC_HandleTypeDef *hadc, uint32_t SingleDiff);
HAL_StatusTypeDef       HAL_ADCEx_Calibration_SetValue(ADC_HandleTypeDef *hadc, uint32_t SingleDiff, uint32_t CalibrationFactor);



/* Blocking mode: Polling */
HAL_StatusTypeDef       HAL_ADCEx_InjectedStart(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef       HAL_ADCEx_InjectedStop(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef       HAL_ADCEx_InjectedPollForConversion(ADC_HandleTypeDef* hadc, uint32_t Timeout);

/* Non-blocking mode: Interruption */
HAL_StatusTypeDef       HAL_ADCEx_InjectedStart_IT(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef       HAL_ADCEx_InjectedStop_IT(ADC_HandleTypeDef* hadc);
     

/* ADC multimode */
HAL_StatusTypeDef       HAL_ADCEx_MultiModeStart_DMA(ADC_HandleTypeDef *hadc, uint32_t *pData, uint32_t Length);
HAL_StatusTypeDef       HAL_ADCEx_MultiModeStop_DMA(ADC_HandleTypeDef *hadc); 
uint32_t                HAL_ADCEx_MultiModeGetValue(ADC_HandleTypeDef *hadc);

/* ADC retrieve conversion value intended to be used with polling or interruption */
uint32_t                HAL_ADCEx_InjectedGetValue(ADC_HandleTypeDef* hadc, uint32_t InjectedRank);

/* ADC IRQHandler and Callbacks used in non-blocking modes (Interruption) */
void                    HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc);
void                    HAL_ADCEx_InjectedQueueOverflowCallback(ADC_HandleTypeDef* hadc);
void                    HAL_ADCEx_LevelOutOfWindow2Callback(ADC_HandleTypeDef* hadc);
void                    HAL_ADCEx_LevelOutOfWindow3Callback(ADC_HandleTypeDef* hadc);
void                    HAL_ADCEx_EndOfSamplingCallback(ADC_HandleTypeDef* hadc);


/* ADC Regular conversions stop */
HAL_StatusTypeDef HAL_ADCEx_RegularStop(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef HAL_ADCEx_RegularStop_IT(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef HAL_ADCEx_RegularStop_DMA(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef HAL_ADCEx_RegularMultiModeStop_DMA(ADC_HandleTypeDef* hadc);

/**
  * @}
  */
     
/** @addtogroup ADCEx_Exported_Functions_Group2 Extended Peripheral Control functions
  * @brief    Extended Peripheral Control functions
  * @{
  */ 
/* Peripheral Control functions ***********************************************/
HAL_StatusTypeDef       HAL_ADCEx_InjectedConfigChannel(ADC_HandleTypeDef* hadc,ADC_InjectionConfTypeDef* sConfigInjected);
HAL_StatusTypeDef       HAL_ADCEx_MultiModeConfigChannel(ADC_HandleTypeDef *hadc, ADC_MultiModeTypeDef *multimode);
HAL_StatusTypeDef       HAL_ADCEx_EnableInjectedQueue(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef       HAL_ADCEx_DisableInjectedQueue(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef       HAL_ADCEx_DisableVoltageRegulator(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef       HAL_ADCEx_EnterADCDeepPowerDownMode(ADC_HandleTypeDef* hadc);

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

#endif /*__STM32L4xx_ADC_EX_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
