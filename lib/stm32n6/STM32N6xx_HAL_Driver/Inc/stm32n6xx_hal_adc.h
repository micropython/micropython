/**
  ******************************************************************************
  * @file    stm32n6xx_hal_adc.h
  * @author  MCD Application Team
  * @brief   Header file of ADC HAL module.
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
#ifndef STM32N6xx_HAL_ADC_H
#define STM32N6xx_HAL_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

/* Include low level driver */
#include "stm32n6xx_ll_adc.h"

/** @addtogroup STM32N6xx_HAL_Driver
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
  * @brief  ADC group regular oversampling structure definition
  */
typedef struct
{
  uint32_t Ratio;                         /*!< Configures the oversampling ratio.
                                               This parameter can be a value from 1 to 1024 */

  uint32_t RightBitShift;                 /*!< Configures the division coefficient for the Oversampler.
                                               This parameter can be a value of @ref ADC_HAL_EC_OVS_SHIFT */

  uint32_t TriggeredMode;                 /*!< Selects the regular triggered oversampling mode.
                                               This parameter can be a value of @ref ADC_HAL_EC_OVS_DISCONT_MODE */

  uint32_t OversamplingStopReset;         /*!< Selects the regular oversampling mode.
                                               The oversampling is either temporary stopped or reset upon an injected
                                               sequence interruption.
                                               If oversampling is enabled on both regular and injected groups,
                                               this parameter is discarded and forced to setting
                                               "ADC_REGOVERSAMPLING_RESUMED_MODE" (the oversampling buffer is zeroed
                                               during injection sequence).
                                               This parameter can be a value of @ref ADC_HAL_EC_OVS_SCOPE_REG */

} ADC_OversamplingTypeDef;

/**
  * @brief  Structure definition of ADC instance and ADC group regular.
  * @note   ADC clock prescaler (init structure "ClockPrescaler" in other ADC IP versions FW drivers)
  *         is moved on RCC side on this STM32 series (rational: for feature "fixed trigger latency").
  *         Refer to RCC FW drivers to configure ADC clock prescaler.
  * @note   Parameters of this structure are shared within 2 scopes:
  *          - Scope entire ADC (affects ADC groups regular and injected): Resolution,
  *            GainCompensation, ScanConvMode, EOCSelection, LowPowerAutoWait.
  *          - Scope ADC group regular: ContinuousConvMode, NbrOfConversion, DiscontinuousConvMode,
  *            NbrOfDiscConversion, ExternalTrigConv, ExternalTrigConvEdge, DMAContinuousRequests, Overrun,
   *           OversamplingMode, Oversampling, SamplingMode.
  * @note   The setting of these parameters by function HAL_ADC_Init() is conditioned to ADC state.
  *         ADC state can be either:
  *          - For all parameters: ADC disabled
  *          - For all parameters except 'LowPowerAutoWait', 'DMAContinuousRequests' and 'Oversampling':
  *            ADC enabled without conversion on going on group regular.
  *          - For parameters 'LowPowerAutoWait' and 'DMAContinuousRequests': ADC enabled without conversion on going
  *            on groups regular and injected.
  *         If ADC is not in the appropriate state to modify some parameters, these parameters setting is bypassed
  *         without error reporting (as it can be the expected behavior in case of intended action to update another
  *         parameter (which fulfills the ADC state condition) on the fly).
  */
typedef struct
{
  uint32_t Resolution;            /*!< Configure the ADC resolution.
                                       This parameter can be a value of @ref ADC_HAL_EC_RESOLUTION */

  uint32_t GainCompensation;      /*!< Specify the ADC gain compensation coefficient to be applied to ADC raw
                                       conversion data, based on following formula:
                                           DATA = DATA(raw) * (gain compensation coef) / 4096
                                        12 bit format, unsigned: 2 bits exponents / 12 bits mantissa
                                        Gain step is 1/4096 = 0.000244
                                        Gain range is 0.0000 to 3.999756
                                       This parameter value can be:
                                        - value "0": Gain compensation will be disabled and coefficient set to 0
                                        - value in range [0x0001; 0x3FFF]: Gain compensation will enabled
                                          and coefficient set to specified value */

  uint32_t ScanConvMode;          /*!< Configure the sequencer of ADC groups regular and injected.
                                       This parameter can be associated to parameter 'DiscontinuousConvMode'
                                       to have main sequence subdivided in successive parts.
                                       If disabled: Conversion is performed in single mode (one channel converted,
                                                    the one defined in rank 1).
                                                    Parameters 'NbrOfConversion' and 'InjectedNbrOfConversion'
                                                    are discarded (equivalent to set to 1).
                                       If enabled:  Conversions are performed in sequence mode (multiple ranks defined
                                                    by 'NbrOfConversion' or 'InjectedNbrOfConversion' and rank of
                                                    each channel in sequencer).
                                                    Scan direction is upward: from rank 1 to rank 'n'.
                                       This parameter can be a value of @ref ADC_Scan_mode */

  uint32_t EOCSelection;          /*!< Specify which EOC (End Of Conversion) flag is used for conversion by polling and
                                       interruption: end of unitary conversion or end of sequence conversions.
                                       This parameter can be a value of @ref ADC_EOCSelection. */

  FunctionalState LowPowerAutoWait; /*!< Select the dynamic low power Auto Delay: new conversion start only when the
                                       previous conversion (for ADC group regular) or previous sequence (for ADC group
                                       injected) has been retrieved by user software, using function HAL_ADC_GetValue()
                                       or HAL_ADCEx_InjectedGetValue().
                                       This feature automatically adapts the frequency of ADC conversions triggers to
                                       the speed of the system that reads the data. Moreover, this avoids risk of
                                       overrun for low frequency applications.
                                       This parameter can be set to ENABLE or DISABLE.
                                       Note: It is not recommended to use with interruption or DMA (HAL_ADC_Start_IT(),
                                             HAL_ADC_Start_DMA()) since these modes have to clear immediately the EOC
                                             flag (by CPU to free the IRQ pending event or by DMA).
                                             Auto wait will work but fort a very short time, discarding its intended
                                             benefit (except specific case of high load of CPU or DMA transfers which
                                             can justify usage of auto wait).
                                             Do use with polling: 1. Start conversion with HAL_ADC_Start(), 2. Later on,
                                             when ADC conversion data is needed:
                                             use HAL_ADC_PollForConversion() to ensure that conversion is completed and
                                             HAL_ADC_GetValue() to retrieve conversion result and trig another
                                             conversion start. (in case of usage of ADC group injected, use the
                                             equivalent functions HAL_ADCExInjected_Start(),
                                             HAL_ADCEx_InjectedGetValue(), ...). */

  FunctionalState ContinuousConvMode; /*!< Specify whether the conversion is performed in single mode (one conversion)
                                           or continuous mode for ADC group regular, after the first ADC conversion
                                           start trigger occurred (software start or external trigger). This parameter
                                           can be set to ENABLE or DISABLE. */

  uint32_t NbrOfConversion;       /*!< Specify the number of ranks that will be converted within the regular group
                                       sequencer.
                                       To use the regular group sequencer and convert several ranks,
                                       parameter 'ScanConvMode' must be enabled.
                                       This parameter must be a number between Min_Data = 1 and Max_Data = 16.
                                       Note: This parameter must be modified when no conversion is on going on
                                             regular group (ADC disabled, or ADC enabled without continuous mode
                                             or external trigger that could launch a conversion). */

  FunctionalState DiscontinuousConvMode; /*!< Specify whether the conversions sequence of ADC group regular is performed
                                              in Complete-sequence/Discontinuous-sequence (main sequence subdivided in
                                              successive parts).
                                              Discontinuous mode is used only if sequencer is enabled (parameter
                                              'ScanConvMode'). If sequencer is disabled, this parameter is discarded.
                                              Discontinuous mode can be enabled only if continuous mode is disabled.
                                              If continuous mode is enabled, this parameter setting is discarded.
                                              This parameter can be set to ENABLE or DISABLE. */

  uint32_t NbrOfDiscConversion;   /*!< Specifies the number of discontinuous conversions in which the main sequence
                                       of ADC group regular (parameter NbrOfConversion) will be subdivided.
                                       If parameter 'DiscontinuousConvMode' is disabled, this parameter is discarded.
                                       This parameter must be a number between Min_Data = 1 and Max_Data = 8. */

  uint32_t ExternalTrigConv;      /*!< Select the external event source used to trigger ADC group regular conversion
                                       start.
                                       If set to ADC_SOFTWARE_START, external triggers are disabled and software trigger
                                       is used instead.
                                       This parameter can be a value of @ref ADC_regular_external_trigger_source.
                                       Caution: external trigger source is common to all ADC instances. */

  uint32_t ExternalTrigConvEdge;  /*!< Select the external event edge used to trigger ADC group regular
                                       conversion start.
                                       If trigger source is set to ADC_SOFTWARE_START, this parameter is discarded.
                                       This parameter can be a value of @ref ADC_regular_external_trigger_edge */

  uint32_t SamplingMode;          /*!< Select the sampling mode to be used for ADC group regular conversion.
                                       This parameter can be a value of @ref ADC_regular_sampling_mode */

  uint32_t ConversionDataManagement; /*!< Specifies whether the Data conversion data is managed: using the DMA
                                       (oneshot or circular), or stored in the DR register or
                                       transferred to MDF register.
                                       Note: In continuous mode, DMA must be configured in circular mode.
                                             Otherwise an overrun will be triggered when DMA buffer maximum pointer
                                             is reached.
                                       This parameter can be a value of @ref ADC_ConversionDataManagement.
                                       Note: This parameter must be modified when no conversion is on going on both
                                             regular and injected groups (ADC disabled, or ADC enabled without
                                             continuous mode or external trigger that could launch a conversion). */

  uint32_t Overrun;               /*!< Select the behavior in case of overrun: data overwritten or preserved (default).
                                       This parameter applies to ADC group regular only.
                                       This parameter can be a value of @ref ADC_HAL_EC_REG_OVR_DATA_BEHAVIOR.
                                       Note: In case of overrun set to data preserved and usage with programming model
                                             with interruption (HAL_Start_IT()): ADC IRQ handler has to clear end of
                                             conversion flags, this induces the release of the preserved data.
                                             If needed, this data can be saved in function HAL_ADC_ConvCpltCallback(),
                                             placed in user program code (called before end of conversion flags clear).
                                       Note: Error reporting with respect to the conversion mode:
                                             - Usage with ADC conversion by polling for event or interruption:
                                                  Error is reported only if overrun is set to data preserved.
                                                  If overrun is set to data overwritten, user can willingly not read
                                                  all the converted data, this is not considered as an erroneous case.
                                             - Usage with ADC conversion by DMA: Error is reported whatever overrun
                                               setting (DMA is expected to process all data from data register). */

  uint32_t LeftBitShift;             /*!< Configures the left shifting applied to the final result with or without
                                          oversampling.
                                          This parameter can be a value of @ref ADCEx_Left_Bit_Shift */

  FunctionalState OversamplingMode;       /*!< Specify whether the oversampling feature is enabled or disabled.
                                               This parameter can be set to ENABLE or DISABLE.
                                               Note: This parameter can be modified only if there is no conversion
                                                     is ongoing on ADC groups regular and injected */

  ADC_OversamplingTypeDef Oversampling;   /*!< Specify the Oversampling parameters.
                                               Caution: this setting overwrites the previous oversampling configuration
                                                        if oversampling is already enabled. */

} ADC_InitTypeDef;

/**
  * @brief  Structure definition of ADC channel for regular group
  * @note   The setting of these parameters by function HAL_ADC_ConfigChannel() is conditioned to ADC state.
  *         ADC state can be either:
  *          - For all parameters: ADC disabled (this is the only possible ADC state to modify parameter 'SingleDiff')
  *          - For all except parameters 'SamplingTime', 'Offset', 'OffsetNumber': ADC enabled without conversion
  *            on going on regular group.
  *          - For parameters 'SamplingTime', 'Offset', 'OffsetNumber': ADC enabled without conversion on going on
  *            regular and injected groups.
  *         If ADC is not in the appropriate state to modify some parameters, these parameters setting is bypassed
  *         without error reporting (as it can be the expected behavior in case of intended action to update another
  *         parameter (which fulfills the ADC state condition) on the fly.
  */
typedef struct
{
  uint32_t Channel;                /*!< Specify the channel to configure into ADC regular group.
                                        This parameter can be a value of @ref ADC_HAL_EC_CHANNEL
                                        Note: Depending on devices and ADC instances, some channels may not be available
                                              on device package pins. Refer to device datasheet for channels
                                              availability. */

  uint32_t Rank;                   /*!< Specify the rank in the regular group sequencer.
                                        This parameter can be a value of @ref ADC_HAL_EC_REG_SEQ_RANKS
                                        Note: to disable a channel or change order of conversion sequencer, rank
                                              containing a previous channel setting can be overwritten by
                                              the new channel setting (or parameter number of conversions adjusted) */

  uint32_t SamplingTime;           /*!< Sampling time value to be set for the selected channel.
                                        Unit: ADC clock cycles
                                        Conversion time is the addition of sampling time and processing time
                                        (13.5 ADC clock cycles at ADC resolution 12 bits, 11.5 cycles at 10 bits,
                                        8.5 cycles at 8 bits, 6.5 cycles at 6 bits).
                                        This parameter can be a value of @ref ADC_HAL_EC_CHANNEL_SAMPLINGTIME
                                        Caution: This parameter applies to a channel that can be used into regular
                                                 and/or injected group. It overwrites the last setting.
                                        Note: In case of usage of internal measurement channels
                                              (VrefInt/Vbat/TempSensor), sampling time constraints must be respected
                                              (sampling time can be adjusted in function of ADC clock frequency
                                              and sampling time setting).
                                              Refer to device datasheet for timings values. */

  uint32_t SingleDiff;             /*!< Select single-ended or differential input.
                                        In differential mode: Differential measurement is carried out between
                                                              the selected channel (positive input) and
                                                              another channel (negative input). Refer to
                                                              reference manual for corresponding channel.
                                                              Only channel of positive input has to be
                                                              configured, the other channel is configured
                                                              automatically.
                                        This parameter must be a value of @ref ADC_HAL_EC_CHANNEL_SINGLE_DIFF_ENDING
                                        Caution: This parameter applies to a channel that can be used in a regular
                                                 and/or injected group. It overwrites the last setting.
                                        Note: Refer to Reference Manual to ensure the selected channel is available
                                              in differential mode.
                                        Note: When configuring a channel 'i' in differential mode, the channel 'i+1'
                                              is not usable separately.
                                        Note: This parameter must be modified when ADC is disabled (before ADC start
                                              conversion or after ADC stop conversion).
                                              If ADC is enabled, this parameter setting is bypassed without error
                                              reporting (as it can be the expected behavior in case of another parameter
                                              update on the fly) */

  uint32_t OffsetNumber;           /*!< Select the offset number
                                        This parameter can be a value of @ref ADC_HAL_EC_OFFSET_NB
                                        Caution: Only one offset is allowed per channel. This parameter overwrites the
                                                 last setting. */

  uint32_t Offset;                 /*!< Define the offset to be subtracted from the raw converted data.
                                        Offset value must be a positive number.
                                        Depending of ADC resolution selected (12, 10, 8 or 6 bits), this parameter
                                        must be a number between Min_Data = 0x000 and Max_Data = 0xFFF,
                                        0xFFF, 0x3FF, 0xFF or 0x3F respectively.
                                        Note: This parameter must be modified when no conversion is on going on both
                                              regular and injected groups (ADC disabled, or ADC enabled without
                                              continuous mode or external trigger that could launch a conversion). */

  FunctionalState OffsetSignedSaturation; /*!< Specify whether the Signed saturation feature is used or not.
                                             This parameter is applied only for 16-bit or 8-bit resolution.
                                             This parameter can be set to ENABLE or DISABLE. */

  FunctionalState OffsetSaturation;   /*!< Define if the offset should be saturated upon under or over flow.
                                        This parameter value can be ENABLE or DISABLE.
                                        Note:
                                              - This parameter must be modified when no conversion is on going on
                                                both regular and injected groups (ADC disabled, or ADC enabled without
                                                continuous mode or external trigger that could launch a conversion).*/

  uint32_t OffsetSign;                /*!< Define if the offset should be subtracted (negative sign) or added
                                           (positive sign) from or to the raw converted data.
                                           This parameter can be a value of @ref ADCEx_OffsetSign.
                                           Note:
                                              - This parameter must be modified when no conversion is on going on
                                                both regular and injected groups (ADC disabled, or ADC enabled without
                                                continuous mode or external trigger that could launch a conversion).*/


} ADC_ChannelConfTypeDef;

/**
  * @brief  Structure definition of ADC analog watchdog
  * @note   The setting of these parameters by function HAL_ADC_AnalogWDGConfig() is conditioned to ADC state.
  *         ADC state can be either:
  *          - For all parameters: ADC disabled or ADC enabled without conversion on going on ADC groups regular
  *            and injected.
  */
typedef struct
{
  uint32_t WatchdogNumber;    /*!< Select which ADC analog watchdog is monitoring the selected channel.
                                   For Analog Watchdog 1: Only 1 channel can be monitored (or overall group of channels
                                                          by setting parameter 'WatchdogMode')
                                   For Analog Watchdog 2 and 3: Several channels can be monitored (by successive calls
                                                                of 'HAL_ADC_AnalogWDGConfig()' for each channel)
                                   This parameter can be a value of @ref ADC_HAL_EC_AWD_NUMBER. */

  uint32_t WatchdogMode;      /*!< Configure the ADC analog watchdog mode: single/all/none channels.
                                   For Analog Watchdog 1: Configure the ADC analog watchdog mode: single channel or all
                                                          channels, ADC groups regular and-or injected.
                                   For Analog Watchdog 2 and 3: Several channels can be monitored by applying
                                                                successively the AWD init structure.
                                   Channels on ADC group regular and injected are not differentiated:
                                            Set value 'ADC_ANALOGWATCHDOG_SINGLE_xxx' to monitor 1 channel,
                                            value 'ADC_ANALOGWATCHDOG_ALL_xxx' to monitor all channels,
                                            'ADC_ANALOGWATCHDOG_NONE' to monitor no channel.
                                   This parameter can be a value of @ref ADC_analog_watchdog_mode. */

  uint32_t Channel;           /*!< Select which ADC channel to monitor by analog watchdog.
                                   For Analog Watchdog 1: this parameter has an effect only if parameter 'WatchdogMode'
                                                          is configured on single channel (only 1 channel can be
                                                          monitored).
                                   For Analog Watchdog 2 and 3: Several channels can be monitored. To use this feature,
                                                                call successively the function HAL_ADC_AnalogWDGConfig()
                                                                for each channel to be added (or removed with value
                                                                'ADC_ANALOGWATCHDOG_NONE').
                                   This parameter can be a value of @ref ADC_HAL_EC_CHANNEL. */

  FunctionalState ITMode;     /*!< Specify whether the analog watchdog is configured in interrupt or polling mode.
                                   This parameter can be set to ENABLE or DISABLE */

  uint32_t HighThreshold;     /*!< Configure the ADC analog watchdog High threshold value.
                                   Depending of ADC resolution selected (12, 10, 8 or 6 bits), this parameter
                                   must be a number between Min_Data = 0x000 and Max_Data = 0xFFF, 0x3FF, 0xFF
                                   or 0x3F respectively. */

  uint32_t LowThreshold;      /*!< Configures the ADC analog watchdog Low threshold value.
                                   Depending of ADC resolution selected (12, 10, 8 or 6 bits), this parameter
                                   must be a number between Min_Data = 0x000 and Max_Data = 0xFFF, 0x3FF, 0xFF
                                   or 0x3F respectively. */

  uint32_t FilteringConfig;   /*!< Specify whether filtering should be use and the number of samples to consider.
                                   Before setting flag or raising interrupt, analog watchdog can wait to have several
                                   consecutive out-of-window samples. This parameter allows to configure this number.
                                   This parameter only applies to Analog watchdog 1. For others, use value
                                   ADC_AWD_FILTERING_NONE.
                                   This parameter can be a value of @ref ADC_analog_watchdog_filtering_config. */

} ADC_AnalogWDGConfTypeDef;

/** @defgroup ADC_States ADC States
  * @{
  */

/**
  * @brief  HAL ADC state machine: ADC states definition (bitfields)
  * @note   ADC state machine is managed by bitfields, state must be compared
  *         with bit by bit.
  *         For example:
  *           " if ((HAL_ADC_GetState(hadc1) & HAL_ADC_STATE_REG_BUSY) != 0UL) "
  *           " if ((HAL_ADC_GetState(hadc1) & HAL_ADC_STATE_AWD1) != 0UL) "
  */
/* States of ADC global scope */
#define HAL_ADC_STATE_RESET             (0x00000000UL)   /*!< ADC not yet initialized or disabled */
#define HAL_ADC_STATE_READY             (0x00000001UL)   /*!< ADC peripheral ready for use */
#define HAL_ADC_STATE_BUSY_INTERNAL     (0x00000002UL)   /*!< ADC is busy due to an internal process (initialization,
                                                              calibration, ...) */
#define HAL_ADC_STATE_TIMEOUT           (0x00000004UL)   /*!< TimeOut occurrence */

/* States of ADC errors */
#define HAL_ADC_STATE_ERROR_INTERNAL    (0x00000010UL)   /*!< Internal error occurrence */
#define HAL_ADC_STATE_ERROR_CONFIG      (0x00000020UL)   /*!< Configuration error occurrence */
#define HAL_ADC_STATE_ERROR_DMA         (0x00000040UL)   /*!< DMA error occurrence */

/* States of ADC group regular */
#define HAL_ADC_STATE_REG_BUSY          (0x00000100UL)   /*!< A conversion on ADC group regular is ongoing or can occur
                                                              (either by continuous mode, external trigger, low power
                                                              auto power-on (if feature available), multimode ADC master
                                                              control (if feature available)) */
#define HAL_ADC_STATE_REG_EOC           (0x00000200UL)   /*!< Conversion data available on group regular */
#define HAL_ADC_STATE_REG_OVR           (0x00000400UL)   /*!< Overrun occurrence */
#define HAL_ADC_STATE_REG_EOSMP         (0x00000800UL)   /*!< Not available on this STM32 series: End Of Sampling flag
                                                              raised  */

/* States of ADC group injected */
#define HAL_ADC_STATE_INJ_BUSY          (0x00001000UL)   /*!< A conversion on ADC group injected is ongoing or can occur
                                                              (either by auto-injection mode, external trigger, low
                                                              power auto power-on (if feature available), multimode
                                                              ADC master control (if feature available)) */
#define HAL_ADC_STATE_INJ_EOC           (0x00002000UL)   /*!< Conversion data available on group injected */

/* States of ADC analog watchdogs */
#define HAL_ADC_STATE_AWD1              (0x00010000UL)   /*!< Out-of-window occurrence of ADC analog watchdog 1 */
#define HAL_ADC_STATE_AWD2              (0x00020000UL)   /*!< Out-of-window occurrence of ADC analog watchdog 2 */
#define HAL_ADC_STATE_AWD3              (0x00040000UL)   /*!< Out-of-window occurrence of ADC analog watchdog 3 */

/* States of ADC multi-mode */
#define HAL_ADC_STATE_MULTIMODE_SLAVE   (0x00100000UL)   /*!< ADC in multimode slave state, controlled by another
                                                              ADC master (when feature available) */

/**
  * @}
  */

/**
  * @brief  ADC handle Structure definition
  */
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
typedef struct __ADC_HandleTypeDef
#else
typedef struct
#endif /* USE_HAL_ADC_REGISTER_CALLBACKS */
{
  ADC_TypeDef                   *Instance;                     /*!< Register base address */
  ADC_InitTypeDef               Init;                          /*!< ADC initialization parameters and regular
                                                                    conversions setting */
  DMA_HandleTypeDef             *DMA_Handle;                   /*!< Pointer DMA Handler */
  HAL_LockTypeDef               Lock;                          /*!< ADC locking object */
  __IO uint32_t                 State;                         /*!< ADC communication state (bitmap of ADC states) */
  __IO uint32_t                 ErrorCode;                     /*!< ADC Error code */

#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
  void (* ConvCpltCallback)(struct __ADC_HandleTypeDef *hadc);              /*!< ADC conversion complete callback */
  void (* ConvHalfCpltCallback)(struct __ADC_HandleTypeDef *hadc);          /*!< ADC conversion DMA half-transfer
                                                                                 callback */
  void (* LevelOutOfWindowCallback)(struct __ADC_HandleTypeDef *hadc);      /*!< ADC analog watchdog 1 callback */
  void (* ErrorCallback)(struct __ADC_HandleTypeDef *hadc);                 /*!< ADC error callback */
  void (* InjectedConvCpltCallback)(struct __ADC_HandleTypeDef *hadc);      /*!< ADC group injected conversion complete
                                                                                 callback */
  void (* LevelOutOfWindow2Callback)(struct __ADC_HandleTypeDef *hadc);     /*!< ADC analog watchdog 2 callback */
  void (* LevelOutOfWindow3Callback)(struct __ADC_HandleTypeDef *hadc);     /*!< ADC analog watchdog 3 callback */
  void (* EndOfSamplingCallback)(struct __ADC_HandleTypeDef *hadc);         /*!< ADC end of sampling callback */
  void (* MspInitCallback)(struct __ADC_HandleTypeDef *hadc);               /*!< ADC Msp Init callback */
  void (* MspDeInitCallback)(struct __ADC_HandleTypeDef *hadc);             /*!< ADC Msp DeInit callback */
#endif /* USE_HAL_ADC_REGISTER_CALLBACKS */
} ADC_HandleTypeDef;

#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
/**
  * @brief  HAL ADC Callback ID enumeration definition
  */
typedef enum
{
  HAL_ADC_CONVERSION_COMPLETE_CB_ID     = 0x00U,  /*!< ADC conversion complete callback ID */
  HAL_ADC_CONVERSION_HALF_CB_ID         = 0x01U,  /*!< ADC conversion DMA half-transfer callback ID */
  HAL_ADC_LEVEL_OUT_OF_WINDOW_1_CB_ID   = 0x02U,  /*!< ADC analog watchdog 1 callback ID */
  HAL_ADC_ERROR_CB_ID                   = 0x03U,  /*!< ADC error callback ID */
  HAL_ADC_INJ_CONVERSION_COMPLETE_CB_ID = 0x04U,  /*!< ADC group injected conversion complete callback ID */
  HAL_ADC_LEVEL_OUT_OF_WINDOW_2_CB_ID   = 0x06U,  /*!< ADC analog watchdog 2 callback ID */
  HAL_ADC_LEVEL_OUT_OF_WINDOW_3_CB_ID   = 0x07U,  /*!< ADC analog watchdog 3 callback ID */
  HAL_ADC_END_OF_SAMPLING_CB_ID         = 0x08U,  /*!< ADC end of sampling callback ID */
  HAL_ADC_MSPINIT_CB_ID                 = 0x09U,  /*!< ADC Msp Init callback ID          */
  HAL_ADC_MSPDEINIT_CB_ID               = 0x0AU   /*!< ADC Msp DeInit callback ID        */
} HAL_ADC_CallbackIDTypeDef;

/**
  * @brief  HAL ADC Callback pointer definition
  */
typedef  void (*pADC_CallbackTypeDef)(ADC_HandleTypeDef *hadc); /*!< pointer to a ADC callback function */

#endif /* USE_HAL_ADC_REGISTER_CALLBACKS */

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
#define HAL_ADC_ERROR_NONE              (0x00U)   /*!< No error                                    */
#define HAL_ADC_ERROR_INTERNAL          (0x01U)   /*!< ADC peripheral internal error (problem of clocking,
                                                       enable/disable, erroneous state, ...)       */
#define HAL_ADC_ERROR_OVR               (0x02U)   /*!< Overrun error                               */
#define HAL_ADC_ERROR_DMA               (0x04U)   /*!< DMA transfer error                          */
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
#define HAL_ADC_ERROR_INVALID_CALLBACK  (0x10U)   /*!< Invalid Callback error */
#endif /* USE_HAL_ADC_REGISTER_CALLBACKS */
/**
  * @}
  */

/** @defgroup ADC_HAL_EC_RESOLUTION  ADC instance - Resolution
  * @{
  */
#define ADC_RESOLUTION_12B                 (LL_ADC_RESOLUTION_12B)  /*!< ADC resolution 12 bits */
#define ADC_RESOLUTION_10B                 (LL_ADC_RESOLUTION_10B)  /*!< ADC resolution 10 bits */
#define ADC_RESOLUTION_8B                  (LL_ADC_RESOLUTION_8B)   /*!< ADC resolution  8 bits */
#define ADC_RESOLUTION_6B                  (LL_ADC_RESOLUTION_6B)   /*!< ADC resolution  6 bits */
/**
  * @}
  */

/** @defgroup ADC_Scan_mode ADC sequencer scan mode
  * @{
  */
#define ADC_SCAN_DISABLE         (0x00000000UL)       /*!< Scan mode disabled */
#define ADC_SCAN_ENABLE          (0x00000001UL)       /*!< Scan mode enabled  */

/** @defgroup ADC_Private_Constants ADC Private Constants
  * @{
  */

/* Combination of all post-conversion flags bits: EOC/EOS, OVR, AWD */
#define ADC_FLAG_POSTCONV_ALL    (ADC_FLAG_AWD | ADC_FLAG_OVR | ADC_FLAG_EOS | ADC_FLAG_EOC)
/**
  * @}
  */

/* Combination of all CHSELR bits: SQ2...SQ8 */
#define ADC_CHSELR_SQ2_TO_SQ8           (ADC_CHSELR_SQ2 | ADC_CHSELR_SQ3 | ADC_CHSELR_SQ4 | ADC_CHSELR_SQ5\
                                         | ADC_CHSELR_SQ6 | ADC_CHSELR_SQ7 | ADC_CHSELR_SQ8)
/**
  * @}
  */

/** @defgroup ADC_regular_external_trigger_source ADC group regular trigger source
  * @{
  */
#define ADC_SOFTWARE_START                 (LL_ADC_REG_TRIG_SOFTWARE)                /*!< ADC group regular conversion
                                            trigger internal: SW start. */
#define ADC_EXTERNALTRIG_EXT_IT11          (LL_ADC_REG_TRIG_EXT_EXTI_LINE11)         /*!< ADC group regular conversion
                                           trigger from external peripheral: external interrupt line 11. */
#define ADC_EXTERNALTRIG_T1_CC1            (LL_ADC_REG_TRIG_EXT_TIM1_CH1)            /*!< ADC group regular conversion
                                           trigger from external peripheral: TIM1 channel 1 event (capture compare). */
#define ADC_EXTERNALTRIG_T1_CC2            (LL_ADC_REG_TRIG_EXT_TIM1_CH2)            /*!< ADC group regular conversion
                                           trigger from external peripheral: TIM1 channel 2 event (capture compare). */
#define ADC_EXTERNALTRIG_T1_CC3            (LL_ADC_REG_TRIG_EXT_TIM1_CH3)            /*!< ADC group regular conversion
                                           trigger from external peripheral: TIM1 channel 3 event (capture compare). */
#define ADC_EXTERNALTRIG_T1_TRGO           (LL_ADC_REG_TRIG_EXT_TIM1_TRGO)           /*!< ADC group regular conversion
                                           trigger from external peripheral: TIM1 TRGO event. */
#define ADC_EXTERNALTRIG_T1_TRGO2          (LL_ADC_REG_TRIG_EXT_TIM1_TRGO2)          /*!< ADC group regular conversion
                                           trigger from external peripheral: TIM1 TRGO2 event. */
#define ADC_EXTERNALTRIG_T2_CC2            (LL_ADC_REG_TRIG_EXT_TIM2_CH2)            /*!< ADC group regular conversion
                                           trigger from external peripheral: TIM2 channel 2 event (capture compare). */
#define ADC_EXTERNALTRIG_T2_TRGO           (LL_ADC_REG_TRIG_EXT_TIM2_TRGO)           /*!< ADC group regular conversion
                                           trigger from external peripheral: TIM2 TRGO event. */
#define ADC_EXTERNALTRIG_T3_CC4            (LL_ADC_REG_TRIG_EXT_TIM3_CH4)            /*!< ADC group regular conversion
                                           trigger from external peripheral: TIM3 channel 4 event (capture compare). */
#define ADC_EXTERNALTRIG_T3_TRGO           (LL_ADC_REG_TRIG_EXT_TIM3_TRGO)           /*!< ADC group regular conversion
                                           trigger from external peripheral: TIM3 TRGO event. */
#define ADC_EXTERNALTRIG_T4_CC4            (LL_ADC_REG_TRIG_EXT_TIM4_CH4)            /*!< ADC group regular conversion
                                           trigger from external peripheral: TIM4 channel 4 event (capture compare). */
#define ADC_EXTERNALTRIG_T4_TRGO           (LL_ADC_REG_TRIG_EXT_TIM4_TRGO)           /*!< ADC group regular conversion
                                           trigger from external peripheral: TIM4 TRGO event. */
#define ADC_EXTERNALTRIG_T5_TRGO           (LL_ADC_REG_TRIG_EXT_TIM5_TRGO)           /*!< ADC group regular conversion
                                           trigger from external peripheral: TIM5 TRGO event. */
#define ADC_EXTERNALTRIG_T6_TRGO           (LL_ADC_REG_TRIG_EXT_TIM6_TRGO)           /*!< ADC group regular conversion
                                           trigger from external peripheral: TIM6 TRGO event. */
#define ADC_EXTERNALTRIG_T7_TRGO           (LL_ADC_REG_TRIG_EXT_TIM7_TRGO)           /*!< ADC group regular conversion
                                           trigger from external peripheral: TIM7 TRGO event. */
#define ADC_EXTERNALTRIG_T8_TRGO           (LL_ADC_REG_TRIG_EXT_TIM8_TRGO)           /*!< ADC group regular conversion
                                           trigger from external peripheral: TIM8 TRGO event. */
#define ADC_EXTERNALTRIG_T8_TRGO2          (LL_ADC_REG_TRIG_EXT_TIM8_TRGO2)          /*!< ADC group regular conversion
                                           trigger from external peripheral: TIM8 TRGO2 event. */
#define ADC_EXTERNALTRIG_T9_CC1            (LL_ADC_REG_TRIG_EXT_TIM9_CH1)            /*!< ADC group regular conversion
                                           trigger from external peripheral: TIM9 channel 1 event (capture compare). */
#define ADC_EXTERNALTRIG_T9_TRGO           (LL_ADC_REG_TRIG_EXT_TIM9_TRGO)           /*!< ADC group regular conversion
                                           trigger from external peripheral: TIM9 TRGO event. */
#define ADC_EXTERNALTRIG_T12_TRGO          (LL_ADC_REG_TRIG_EXT_TIM12_TRGO)          /*!< ADC group regular conversion
                                           trigger from external peripheral: TIM12 TRGO event. */
#define ADC_EXTERNALTRIG_T15_TRGO          (LL_ADC_REG_TRIG_EXT_TIM15_TRGO)          /*!< ADC group regular conversion
                                           trigger from external peripheral: TIM15 TRGO event. */
#define ADC_EXTERNALTRIG_T18_TRGO          (LL_ADC_REG_TRIG_EXT_TIM18_TRGO)          /*!< ADC group regular conversion
                                           trigger from external peripheral: TIM18 TRGO event. */
#define ADC_EXTERNALTRIG_LPTIM1_CH1        (LL_ADC_REG_TRIG_EXT_LPTIM1_CH1)          /*!< ADC group regular conversion
                                           trigger from external peripheral: LPTIM1 channel 1 event. */
#define ADC_EXTERNALTRIG_LPTIM2_CH1        (LL_ADC_REG_TRIG_EXT_LPTIM2_CH1)          /*!< ADC group regular conversion
                                           trigger from external peripheral: LPTIM2 channel 2 event. */
#define ADC_EXTERNALTRIG_LPTIM3_CH1        (LL_ADC_REG_TRIG_EXT_LPTIM3_CH1)          /*!< ADC group regular conversion
                                           trigger from external peripheral: LPTIM3 channel 3 event. */
/**
  * @}
  */

/** @defgroup ADC_regular_external_trigger_edge ADC group regular trigger edge (when external trigger is selected)
  * @{
  */
#define ADC_EXTERNALTRIGCONVEDGE_NONE           (0x00000000UL)                      /*!< ADC group regular trigger
                                                disabled (SW start)*/
#define ADC_EXTERNALTRIGCONVEDGE_RISING         (LL_ADC_REG_TRIG_EXT_RISING)        /*!< ADC group regular conversion
                                                trigger polarity set to rising edge */
#define ADC_EXTERNALTRIGCONVEDGE_FALLING        (LL_ADC_REG_TRIG_EXT_FALLING)       /*!< ADC group regular conversion
                                                trigger polarity set to falling edge */
#define ADC_EXTERNALTRIGCONVEDGE_RISINGFALLING  (LL_ADC_REG_TRIG_EXT_RISINGFALLING) /*!< ADC group regular conversion
                                                trigger polarity set to both rising and falling edges */
/**
  * @}
  */

/** @defgroup ADC_regular_sampling_mode ADC group regular sampling mode
  * @{
  */
#define ADC_SAMPLING_MODE_NORMAL                (0x00000000UL)      /*!< ADC conversions sampling phase duration is
                                                defined using  @ref ADC_HAL_EC_CHANNEL_SAMPLINGTIME */
#define ADC_SAMPLING_MODE_BULB                  (ADC_CFGR2_BULB)    /*!< ADC conversions sampling phase starts
                                                immediately after end of conversion, and stops upon trigger event.
                                                Note: First conversion is using minimal sampling time
                                                      (see @ref ADC_HAL_EC_CHANNEL_SAMPLINGTIME) */
#define ADC_SAMPLING_MODE_TRIGGER_CONTROLED     (ADC_CFGR2_SMPTRIG) /*!< ADC conversions sampling phase is controlled
                                                by trigger events:
                                                Trigger rising edge  = start sampling
                                                Trigger falling edge = stop sampling and start conversion */
/**
  * @}
  */

/** @defgroup ADC_EOCSelection ADC sequencer end of unitary conversion or sequence conversions
  * @{
  */
#define ADC_EOC_SINGLE_CONV         (ADC_ISR_EOC)                 /*!< End of unitary conversion flag  */
#define ADC_EOC_SEQ_CONV            (ADC_ISR_EOS)                 /*!< End of sequence conversions flag    */
/**
  * @}
  */

/** @defgroup ADC_HAL_EC_REG_OVR_DATA_BEHAVIOR  ADC group regular - Overrun behavior on conversion data
  * @{
  */
#define ADC_OVR_DATA_PRESERVED             (LL_ADC_REG_OVR_DATA_PRESERVED)    /*!< ADC group regular behavior in case
                                           of overrun: data preserved */
#define ADC_OVR_DATA_OVERWRITTEN           (LL_ADC_REG_OVR_DATA_OVERWRITTEN)  /*!< ADC group regular behavior in case
                                           of overrun: data overwritten */
/**
  * @}
  */

/** @defgroup ADC_HAL_EC_REG_SEQ_RANKS  ADC group regular - Sequencer ranks
  * @{
  */
#define ADC_REGULAR_RANK_1                 (LL_ADC_REG_RANK_1)  /*!< ADC group regular sequencer rank 1 */
#define ADC_REGULAR_RANK_2                 (LL_ADC_REG_RANK_2)  /*!< ADC group regular sequencer rank 2 */
#define ADC_REGULAR_RANK_3                 (LL_ADC_REG_RANK_3)  /*!< ADC group regular sequencer rank 3 */
#define ADC_REGULAR_RANK_4                 (LL_ADC_REG_RANK_4)  /*!< ADC group regular sequencer rank 4 */
#define ADC_REGULAR_RANK_5                 (LL_ADC_REG_RANK_5)  /*!< ADC group regular sequencer rank 5 */
#define ADC_REGULAR_RANK_6                 (LL_ADC_REG_RANK_6)  /*!< ADC group regular sequencer rank 6 */
#define ADC_REGULAR_RANK_7                 (LL_ADC_REG_RANK_7)  /*!< ADC group regular sequencer rank 7 */
#define ADC_REGULAR_RANK_8                 (LL_ADC_REG_RANK_8)  /*!< ADC group regular sequencer rank 8 */
#define ADC_REGULAR_RANK_9                 (LL_ADC_REG_RANK_9)  /*!< ADC group regular sequencer rank 9 */
#define ADC_REGULAR_RANK_10                (LL_ADC_REG_RANK_10) /*!< ADC group regular sequencer rank 10 */
#define ADC_REGULAR_RANK_11                (LL_ADC_REG_RANK_11) /*!< ADC group regular sequencer rank 11 */
#define ADC_REGULAR_RANK_12                (LL_ADC_REG_RANK_12) /*!< ADC group regular sequencer rank 12 */
#define ADC_REGULAR_RANK_13                (LL_ADC_REG_RANK_13) /*!< ADC group regular sequencer rank 13 */
#define ADC_REGULAR_RANK_14                (LL_ADC_REG_RANK_14) /*!< ADC group regular sequencer rank 14 */
#define ADC_REGULAR_RANK_15                (LL_ADC_REG_RANK_15) /*!< ADC group regular sequencer rank 15 */
#define ADC_REGULAR_RANK_16                (LL_ADC_REG_RANK_16) /*!< ADC group regular sequencer rank 16 */
/**
  * @}
  */

/** @defgroup ADC_HAL_EC_CHANNEL_SAMPLINGTIME  Channel - Sampling time
  * @{
  */
#define ADC_SAMPLETIME_1CYCLE_5          (LL_ADC_SAMPLINGTIME_1CYCLE_5)     /*!< Sampling time 1.5 ADC clock cycle */
#define ADC_SAMPLETIME_2CYCLES_5         (LL_ADC_SAMPLINGTIME_2CYCLES_5)    /*!< Sampling time 2.5 ADC clock cycles */
#define ADC_SAMPLETIME_6CYCLES_5         (LL_ADC_SAMPLINGTIME_6CYCLES_5)    /*!< Sampling time 6.5 ADC clock cycles */
#define ADC_SAMPLETIME_11CYCLES_5        (LL_ADC_SAMPLINGTIME_11CYCLES_5)   /*!< Sampling time 11.5 ADC clock cycles */
#define ADC_SAMPLETIME_23CYCLES_5        (LL_ADC_SAMPLINGTIME_23CYCLES_5)   /*!< Sampling time 23.5 ADC clock cycles */
#define ADC_SAMPLETIME_46CYCLES_5        (LL_ADC_SAMPLINGTIME_46CYCLES_5)   /*!< Sampling time 46.5 ADC clock cycles */
#define ADC_SAMPLETIME_246CYCLES_5       (LL_ADC_SAMPLINGTIME_246CYCLES_5)  /*!< Sampling time 246.5 ADC clock cycles */
#define ADC_SAMPLETIME_1499CYCLES_5      (LL_ADC_SAMPLINGTIME_1499CYCLES_5) /*!< Sampling time 1499.5 ADC clock cycles*/
/**
  * @}
  */

/** @defgroup ADC_HAL_EC_CHANNEL  ADC instance - Channel number
  * @{
  */
/* Note: VrefInt, TempSensor and Vbat internal channels are not available on  */
/*        all ADC instances (refer to Reference Manual).                      */
#define ADC_CHANNEL_0                      (LL_ADC_CHANNEL_0)               /*!< ADC external channel (connected to GPIO
                                           pin) ADCx_IN0  */
#define ADC_CHANNEL_1                      (LL_ADC_CHANNEL_1)               /*!< ADC external channel (connected to GPIO
                                           pin) ADCx_IN1  */
#define ADC_CHANNEL_2                      (LL_ADC_CHANNEL_2)               /*!< ADC external channel (connected to GPIO
                                           pin) ADCx_IN2  */
#define ADC_CHANNEL_3                      (LL_ADC_CHANNEL_3)               /*!< ADC external channel (connected to GPIO
                                           pin) ADCx_IN3  */
#define ADC_CHANNEL_4                      (LL_ADC_CHANNEL_4)               /*!< ADC external channel (connected to GPIO
                                           pin) ADCx_IN4  */
#define ADC_CHANNEL_5                      (LL_ADC_CHANNEL_5)               /*!< ADC external channel (connected to GPIO
                                           pin) ADCx_IN5  */
#define ADC_CHANNEL_6                      (LL_ADC_CHANNEL_6)               /*!< ADC external channel (connected to GPIO
                                           pin) ADCx_IN6  */
#define ADC_CHANNEL_7                      (LL_ADC_CHANNEL_7)               /*!< ADC external channel (connected to GPIO
                                           pin) ADCx_IN7  */
#define ADC_CHANNEL_8                      (LL_ADC_CHANNEL_8)               /*!< ADC external channel (connected to GPIO
                                           pin) ADCx_IN8  */
#define ADC_CHANNEL_9                      (LL_ADC_CHANNEL_9)               /*!< ADC external channel (connected to GPIO
                                           pin) ADCx_IN9  */
#define ADC_CHANNEL_10                     (LL_ADC_CHANNEL_10)              /*!< ADC external channel (connected to GPIO
                                           pin) ADCx_IN10 */
#define ADC_CHANNEL_11                     (LL_ADC_CHANNEL_11)              /*!< ADC external channel (connected to GPIO
                                           pin) ADCx_IN11 */
#define ADC_CHANNEL_12                     (LL_ADC_CHANNEL_12)              /*!< ADC external channel (connected to GPIO
                                           pin) ADCx_IN12 */
#define ADC_CHANNEL_13                     (LL_ADC_CHANNEL_13)              /*!< ADC external channel (connected to GPIO
                                           pin) ADCx_IN13 */
#define ADC_CHANNEL_14                     (LL_ADC_CHANNEL_14)              /*!< ADC external channel (connected to GPIO
                                           pin) ADCx_IN14 */
#define ADC_CHANNEL_15                     (LL_ADC_CHANNEL_15)              /*!< ADC external channel (connected to GPIO
                                           pin) ADCx_IN15 */
#define ADC_CHANNEL_16                     (LL_ADC_CHANNEL_16)              /*!< ADC external channel (connected to GPIO
                                           pin) ADCx_IN16 */
#define ADC_CHANNEL_17                     (LL_ADC_CHANNEL_17)              /*!< ADC external channel (connected to GPIO
                                           pin) ADCx_IN17 */
#define ADC_CHANNEL_18                     (LL_ADC_CHANNEL_18)              /*!< ADC external channel (connected to GPIO
                                           pin) ADCx_IN18 */
#define ADC_CHANNEL_19                     (LL_ADC_CHANNEL_19)              /*!< ADC external channel (connected to GPIO
                                           pin) ADCx_IN19 */
#define ADC_CHANNEL_VREFINT                (LL_ADC_CHANNEL_VREFINT)         /*!< ADC internal channel connected to
                                           VrefInt: Internal voltage reference.
                                           On this STM32 series, ADC channel available only on ADC instance: ADC1. */
#define ADC_CHANNEL_VBAT                   (LL_ADC_CHANNEL_VBAT)            /*!< ADC internal channel connected to
                                           Vbat/4: Vbat voltage through a divider ladder of factor 1/4 to have Vbat
                                           always below Vdda.
                                           On this STM32 series, ADC channel available only on ADC instance: ADC2. */
#define ADC_CHANNEL_VDDCORE                (LL_ADC_CHANNEL_VDDCORE)         /*!< ADC internal channel connected to
                                           VddCore.
                                           On this STM32 series, ADC channel available only on ADC instance: ADC2. */
/**
  * @}
  */

/** @defgroup ADC_ConversionDataManagement ADC Conversion Data Management
  * @{
  */
#define ADC_CONVERSIONDATA_DR              (LL_ADC_REG_DR_TRANSFER)            /*!< ADC conversions data are available
                                           in ADC data register only */
#define ADC_CONVERSIONDATA_DMA_ONESHOT     (LL_ADC_REG_DMA_TRANSFER_LIMITED)   /*!< ADC conversion data are transferred
                                           by DMA, in limited mode (one shot mode): DMA transfer requests are stopped
                                           when number of DMA data transfers (number of ADC conversions) is reached.
                                           This ADC mode is intended to be used with DMA mode non-circular. */
#define ADC_CONVERSIONDATA_DMA_CIRCULAR    (LL_ADC_REG_DMA_TRANSFER_UNLIMITED) /*!< ADC conversion data are transferred
                                           by DMA, in unlimited mode: DMA transfer requests are unlimited,
                                           whatever number of DMA data transferred (number of ADC conversions).
                                           This ADC mode is intended to be used with DMA mode circular. */
#define ADC_CONVERSIONDATA_MDF             (LL_ADC_REG_MDF_TRANSFER)           /*!< ADC conversion data are transferred
                                           to MDF */

/**
  * @}
  */

/** @defgroup ADC_HAL_EC_AWD_NUMBER Analog watchdog - ADC analog watchdog (AWD) number
  * @{
  */
#define ADC_ANALOGWATCHDOG_1               (LL_ADC_AWD1) /*!< ADC analog watchdog number 1 */
#define ADC_ANALOGWATCHDOG_2               (LL_ADC_AWD2) /*!< ADC analog watchdog number 2 */
#define ADC_ANALOGWATCHDOG_3               (LL_ADC_AWD3) /*!< ADC analog watchdog number 3 */
/**
  * @}
  */

/** @defgroup ADC_analog_watchdog_filtering_config ADC Analog Watchdog filtering configuration
  * @{
  */
#define ADC_AWD_FILTERING_NONE          LL_ADC_AWD_FILTERING_NONE      /*!< ADC analog watchdog no filtering,
                                        one out-of-window sample is needed to raise flag or interrupt */
#define ADC_AWD_FILTERING_2SAMPLES      LL_ADC_AWD_FILTERING_2SAMPLES  /*!< ADC analog watchdog 2 consecutives
                                        out-of-window samples are needed to raise flag or interrupt */
#define ADC_AWD_FILTERING_3SAMPLES      LL_ADC_AWD_FILTERING_3SAMPLES  /*!< ADC analog watchdog 3 consecutives
                                        out-of-window samples are needed to raise flag or interrupt */
#define ADC_AWD_FILTERING_4SAMPLES      LL_ADC_AWD_FILTERING_4SAMPLES  /*!< ADC analog watchdog 4 consecutives
                                        out-of-window samples are needed to raise flag or interrupt */
#define ADC_AWD_FILTERING_5SAMPLES      LL_ADC_AWD_FILTERING_5SAMPLES  /*!< ADC analog watchdog 5 consecutives
                                        out-of-window samples are needed to raise flag or interrupt */
#define ADC_AWD_FILTERING_6SAMPLES      LL_ADC_AWD_FILTERING_6SAMPLES  /*!< ADC analog watchdog 6 consecutives
                                        out-of-window samples are needed to raise flag or interrupt */
#define ADC_AWD_FILTERING_7SAMPLES      LL_ADC_AWD_FILTERING_7SAMPLES  /*!< ADC analog watchdog 7 consecutives
                                        out-of-window samples are needed to raise flag or interrupt */
#define ADC_AWD_FILTERING_8SAMPLES      LL_ADC_AWD_FILTERING_8SAMPLES  /*!< ADC analog watchdog 8 consecutives
                                        out-of-window samples are needed to raise flag or interrupt */
/**
  * @}
  */

/** @defgroup ADC_analog_watchdog_mode ADC analog watchdog (AWD) mode
  * @{
  */
#define ADC_ANALOGWATCHDOG_NONE                 (0x00000000UL)                          /*!< ADC AWD not selected */

#define ADC_ANALOGWATCHDOG_SINGLE_REG           (ADC_CFGR1_AWD1SGL | ADC_CFGR1_AWD1EN)  /*!< ADC AWD applied to a
                                                regular group single channel */

#define ADC_ANALOGWATCHDOG_SINGLE_INJEC         (ADC_CFGR1_AWD1SGL | ADC_CFGR1_JAWD1EN) /*!< ADC AWD applied to an
                                                injected group single channel */
#define ADC_ANALOGWATCHDOG_SINGLE_REGINJEC      (ADC_CFGR1_AWD1SGL | ADC_CFGR1_AWD1EN \
                                                 | ADC_CFGR1_JAWD1EN)                   /*!< ADC AWD applied to a
                                                regular and injected groups single channel */
#define ADC_ANALOGWATCHDOG_ALL_REG              (ADC_CFGR1_AWD1EN)                      /*!< ADC AWD applied to regular
                                                group all channels */
#define ADC_ANALOGWATCHDOG_ALL_INJEC            (ADC_CFGR1_JAWD1EN)                     /*!< ADC AWD applied to injected
                                                group all channels */
#define ADC_ANALOGWATCHDOG_ALL_REGINJEC         (ADC_CFGR1_AWD1EN | ADC_CFGR1_JAWD1EN)  /*!< ADC AWD applied to regular
                                                and injected groups all channels */
/**
  * @}
  */

/** @defgroup ADC_HAL_EC_OVS_SHIFT  Oversampling - Data shift
  * @{
  */
#define ADC_RIGHTBITSHIFT_NONE             (LL_ADC_OVS_SHIFT_NONE)    /*!< ADC oversampling no shift   */
#define ADC_RIGHTBITSHIFT_1                (LL_ADC_OVS_SHIFT_RIGHT_1) /*!< ADC oversampling right shift of 1 ranks */
#define ADC_RIGHTBITSHIFT_2                (LL_ADC_OVS_SHIFT_RIGHT_2) /*!< ADC oversampling right shift of 2 ranks */
#define ADC_RIGHTBITSHIFT_3                (LL_ADC_OVS_SHIFT_RIGHT_3) /*!< ADC oversampling right shift of 3 ranks */
#define ADC_RIGHTBITSHIFT_4                (LL_ADC_OVS_SHIFT_RIGHT_4) /*!< ADC oversampling right shift of 4 ranks */
#define ADC_RIGHTBITSHIFT_5                (LL_ADC_OVS_SHIFT_RIGHT_5) /*!< ADC oversampling right shift of 5 ranks */
#define ADC_RIGHTBITSHIFT_6                (LL_ADC_OVS_SHIFT_RIGHT_6) /*!< ADC oversampling right shift of 6 ranks */
#define ADC_RIGHTBITSHIFT_7                (LL_ADC_OVS_SHIFT_RIGHT_7) /*!< ADC oversampling right shift of 7 ranks */
#define ADC_RIGHTBITSHIFT_8                (LL_ADC_OVS_SHIFT_RIGHT_8) /*!< ADC oversampling right shift of 8 ranks */
#define ADC_RIGHTBITSHIFT_9                (LL_ADC_OVS_SHIFT_RIGHT_9) /*!< ADC oversampling right shift of 9 ranks */
#define ADC_RIGHTBITSHIFT_10               (LL_ADC_OVS_SHIFT_RIGHT_10)/*!< ADC oversampling right shift of 10 ranks */
/**
  * @}
  */

/** @defgroup ADCEx_Left_Bit_Shift   ADC Extended Oversampling left Shift
  * @{
  */
#define ADC_LEFTBITSHIFT_NONE  (LL_ADC_LEFT_BIT_SHIFT_NONE)   /*!<  ADC No bit shift */
#define ADC_LEFTBITSHIFT_1     (LL_ADC_LEFT_BIT_SHIFT_1)      /*!<  ADC 1 bit shift  */
#define ADC_LEFTBITSHIFT_2     (LL_ADC_LEFT_BIT_SHIFT_2)      /*!<  ADC 2 bits shift */
#define ADC_LEFTBITSHIFT_3     (LL_ADC_LEFT_BIT_SHIFT_3)      /*!<  ADC 3 bits shift */
#define ADC_LEFTBITSHIFT_4     (LL_ADC_LEFT_BIT_SHIFT_4)      /*!<  ADC 4 bits shift */
#define ADC_LEFTBITSHIFT_5     (LL_ADC_LEFT_BIT_SHIFT_5)      /*!<  ADC 5 bits shift */
#define ADC_LEFTBITSHIFT_6     (LL_ADC_LEFT_BIT_SHIFT_6)      /*!<  ADC 6 bits shift */
#define ADC_LEFTBITSHIFT_7     (LL_ADC_LEFT_BIT_SHIFT_7)      /*!<  ADC 7 bits shift */
#define ADC_LEFTBITSHIFT_8     (LL_ADC_LEFT_BIT_SHIFT_8)      /*!<  ADC 8 bits shift */
#define ADC_LEFTBITSHIFT_9     (LL_ADC_LEFT_BIT_SHIFT_9)      /*!<  ADC 9 bits shift */
#define ADC_LEFTBITSHIFT_10    (LL_ADC_LEFT_BIT_SHIFT_10)     /*!<  ADC 10 bits shift */
#define ADC_LEFTBITSHIFT_11    (LL_ADC_LEFT_BIT_SHIFT_11)     /*!<  ADC 11 bits shift */
#define ADC_LEFTBITSHIFT_12    (LL_ADC_LEFT_BIT_SHIFT_12)     /*!<  ADC 12 bits shift */
#define ADC_LEFTBITSHIFT_13    (LL_ADC_LEFT_BIT_SHIFT_13)     /*!<  ADC 13 bits shift */
#define ADC_LEFTBITSHIFT_14    (LL_ADC_LEFT_BIT_SHIFT_14)     /*!<  ADC 14 bits shift */
#define ADC_LEFTBITSHIFT_15    (LL_ADC_LEFT_BIT_SHIFT_15)     /*!<  ADC 15 bits shift */
/**
  * @}
  */

/** @defgroup ADC_HAL_EC_OVS_DISCONT_MODE  Oversampling - Discontinuous mode
  * @{
  */
#define ADC_TRIGGEREDMODE_SINGLE_TRIGGER   (LL_ADC_OVS_REG_CONT)          /*!< ADC oversampling discontinuous mode:
                                           continuous mode (all conversions of OVS ratio are done from 1 trigger) */
#define ADC_TRIGGEREDMODE_MULTI_TRIGGER    (LL_ADC_OVS_REG_DISCONT)       /*!< ADC oversampling discontinuous mode:
                                           discontinuous mode (each conversion of OVS ratio needs a trigger) */
/**
  * @}
  */

/** @defgroup ADC_HAL_EC_OVS_SCOPE_REG  Oversampling - Oversampling scope for ADC group regular
  * @{
  */
#define ADC_REGOVERSAMPLING_CONTINUED_MODE (LL_ADC_OVS_GRP_REGULAR_CONTINUED) /*!< Oversampling buffer maintained
                                           during injection sequence */
#define ADC_REGOVERSAMPLING_RESUMED_MODE   (LL_ADC_OVS_GRP_REGULAR_RESUMED)   /*!< Oversampling buffer zeroed during
                                           injection sequence */
/**
  * @}
  */

/** @defgroup ADC_Event_type ADC Event type
  * @{
  */
/**
  * @note Analog watchdog 1 is available on all stm32 series
  *       Analog watchdog 2 and 3 are not available on all series
  */
#define ADC_EOSMP_EVENT          (ADC_FLAG_EOSMP) /*!< ADC End of Sampling event */
#define ADC_AWD1_EVENT           (ADC_FLAG_AWD1)  /*!< ADC Analog watchdog 1 event (main analog watchdog)       */
#define ADC_AWD2_EVENT           (ADC_FLAG_AWD2)  /*!< ADC Analog watchdog 2 event (additional analog watchdog) */
#define ADC_AWD3_EVENT           (ADC_FLAG_AWD3)  /*!< ADC Analog watchdog 3 event (additional analog watchdog) */
#define ADC_OVR_EVENT            (ADC_FLAG_OVR)   /*!< ADC overrun event */
/**
  * @}
  */
#define ADC_AWD_EVENT            ADC_AWD1_EVENT      /*!< ADC Analog watchdog 1 event: Naming for compatibility
                                                          with other STM32 devices having only one analog watchdog */

/** @defgroup ADC_interrupts_definition ADC interrupts definition
  * @{
  */
#define ADC_IT_RDY           ADC_IER_ADRDYIE    /*!< ADC Ready interrupt source */
#define ADC_IT_EOSMP         ADC_IER_EOSMPIE    /*!< ADC End of sampling interrupt source */
#define ADC_IT_EOC           ADC_IER_EOCIE      /*!< ADC End of regular conversion interrupt source */
#define ADC_IT_EOS           ADC_IER_EOSIE      /*!< ADC End of regular sequence of conversions interrupt source */
#define ADC_IT_OVR           ADC_IER_OVRIE      /*!< ADC overrun interrupt source */
#define ADC_IT_JEOC          ADC_IER_JEOCIE     /*!< ADC End of injected conversion interrupt source */
#define ADC_IT_JEOS          ADC_IER_JEOSIE     /*!< ADC End of injected sequence of conversions interrupt source */
#define ADC_IT_AWD1          ADC_IER_AWD1IE     /*!< ADC Analog watchdog 1 interrupt source (main analog watchdog) */
#define ADC_IT_AWD2          ADC_IER_AWD2IE     /*!< ADC Analog watchdog 2 interrupt source (additional analog
                             watchdog) */
#define ADC_IT_AWD3          ADC_IER_AWD3IE     /*!< ADC Analog watchdog 3 interrupt source (additional analog
                             watchdog) */
/**
  * @}
  */

/** @defgroup ADC_flags_definition ADC flags definition
  * @{
  */
#define ADC_FLAG_RDY           ADC_ISR_ADRDY    /*!< ADC Ready flag */
#define ADC_FLAG_EOSMP         ADC_ISR_EOSMP    /*!< ADC End of Sampling flag */
#define ADC_FLAG_EOC           ADC_ISR_EOC      /*!< ADC End of Regular Conversion flag */
#define ADC_FLAG_EOS           ADC_ISR_EOS      /*!< ADC End of Regular sequence of Conversions flag */
#define ADC_FLAG_OVR           ADC_ISR_OVR      /*!< ADC overrun flag */
#define ADC_FLAG_JEOC          ADC_ISR_JEOC     /*!< ADC End of Injected Conversion flag */
#define ADC_FLAG_JEOS          ADC_ISR_JEOS     /*!< ADC End of Injected sequence of Conversions flag */
#define ADC_FLAG_AWD1          ADC_ISR_AWD1     /*!< ADC Analog watchdog 1 flag (main analog watchdog) */
#define ADC_FLAG_AWD2          ADC_ISR_AWD2     /*!< ADC Analog watchdog 2 flag (additional analog watchdog) */
#define ADC_FLAG_AWD3          ADC_ISR_AWD3     /*!< ADC Analog watchdog 3 flag (additional analog watchdog) */
/**
  * @}
  */

/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/

/** @defgroup ADC_Private_Macros ADC Private Macros
  * @{
  */
/* Macro reserved for internal HAL driver usage, not intended to be used in   */
/* code of final user.                                                        */

/**
  * @brief Verify the ADC data conversion setting.
  * @param DATA : programmed DATA conversion mode.
  * @retval SET (DATA is a valid value) or RESET (DATA is invalid)
  */
#define IS_ADC_CONVERSIONDATAMGT(DATA)                                         \
  ((((DATA) == ADC_CONVERSIONDATA_DR))          || \
   (((DATA) == ADC_CONVERSIONDATA_MDF))         || \
   (((DATA) == ADC_CONVERSIONDATA_DMA_ONESHOT)) || \
   (((DATA) == ADC_CONVERSIONDATA_DMA_CIRCULAR)))

/**
  * @brief Return resolution bits in CFGR register RES[1:0] field.
  * @param __HANDLE__ ADC handle
  * @retval Value of bitfield RES in CFGR register.
  */
#define ADC_GET_RESOLUTION(__HANDLE__)                                         \
  (LL_ADC_GetResolution((__HANDLE__)->Instance))

/**
  * @brief Clear ADC error code (set it to no error code "HAL_ADC_ERROR_NONE").
  * @param __HANDLE__ ADC handle
  * @retval None
  */
#define ADC_CLEAR_ERRORCODE(__HANDLE__) ((__HANDLE__)->ErrorCode = HAL_ADC_ERROR_NONE)

/**
  * @brief Verification of ADC state: enabled or disabled.
  * @param __HANDLE__ ADC handle
  * @retval SET (ADC enabled) or RESET (ADC disabled)
  */
#define ADC_IS_ENABLE(__HANDLE__)                                                     \
  ((((((__HANDLE__)->Instance->CR) & (ADC_CR_ADEN | ADC_CR_ADDIS)) == ADC_CR_ADEN) && \
    ((((__HANDLE__)->Instance->ISR) & ADC_FLAG_RDY) == ADC_FLAG_RDY)                  \
   ) ? SET : RESET)

/**
  * @brief Check if conversion is on going on regular group.
  * @param __HANDLE__ ADC handle
  * @retval Value "0" (no conversion is on going) or value "1" (conversion is on going)
  */
#define ADC_IS_CONVERSION_ONGOING_REGULAR(__HANDLE__)                          \
  (LL_ADC_REG_IsConversionOngoing((__HANDLE__)->Instance))


/**
  * @brief Simultaneously clear and set specific bits of the handle State.
  * @note  ADC_STATE_CLR_SET() macro is merely aliased to generic macro MODIFY_REG(),
  *        the first parameter is the ADC handle State, the second parameter is the
  *        bit field to clear, the third and last parameter is the bit field to set.
  * @retval None
  */
#define ADC_STATE_CLR_SET MODIFY_REG

/**
  * @brief Verify that a given value is aligned with the ADC resolution range.
  * @param __RESOLUTION__ ADC resolution (12, 10, 8 or 6 bits).
  * @param __ADC_VALUE__ value checked against the resolution.
  * @retval SET (__ADC_VALUE__ in line with __RESOLUTION__) or RESET (__ADC_VALUE__ not in line with __RESOLUTION__)
  */
#define IS_ADC_RANGE(__RESOLUTION__, __ADC_VALUE__) \
  ((__ADC_VALUE__) <= __LL_ADC_DIGITAL_SCALE(__RESOLUTION__))

/**
  * @brief Verify the length of the scheduled regular conversions group.
  * @param __LENGTH__ number of programmed conversions.
  * @retval SET (__LENGTH__ is within the maximum number of possible programmable regular conversions)
  *         or RESET (__LENGTH__ is null or too large)
  */
#define IS_ADC_REGULAR_NB_CONV(__LENGTH__) (((__LENGTH__) >= (1UL)) && ((__LENGTH__) <= (16UL)))


/**
  * @brief Verify the number of scheduled regular conversions in discontinuous mode.
  * @param NUMBER number of scheduled regular conversions in discontinuous mode.
  * @retval SET (NUMBER is within the maximum number of regular conversions in discontinuous mode)
  *         or RESET (NUMBER is null or too large)
  */
#define IS_ADC_REGULAR_DISCONT_NUMBER(NUMBER) (((NUMBER) >= (1UL)) && ((NUMBER) <= (8UL)))

/**
  * @brief Verify the ADC resolution setting.
  * @param __RESOLUTION__ programmed ADC resolution.
  * @retval SET (__RESOLUTION__ is a valid value) or RESET (__RESOLUTION__ is invalid)
  */
#define IS_ADC_RESOLUTION(__RESOLUTION__) (((__RESOLUTION__) == ADC_RESOLUTION_12B) || \
                                           ((__RESOLUTION__) == ADC_RESOLUTION_10B) || \
                                           ((__RESOLUTION__) == ADC_RESOLUTION_8B)  || \
                                           ((__RESOLUTION__) == ADC_RESOLUTION_6B)    )

/**
  * @brief Verify the ADC resolution setting when limited to 6 or 8 bits.
  * @param __RESOLUTION__ programmed ADC resolution when limited to 6 or 8 bits.
  * @retval SET (__RESOLUTION__ is a valid value) or RESET (__RESOLUTION__ is invalid)
  */
#define IS_ADC_RESOLUTION_8_6_BITS(__RESOLUTION__) (((__RESOLUTION__) == ADC_RESOLUTION_8B) || \
                                                    ((__RESOLUTION__) == ADC_RESOLUTION_6B)   )

/**
  * @brief Verify the ADC gain compensation.
  * @param __GAIN_COMPENSATION__ programmed ADC gain compensation coefficient.
  * @retval SET (__GAIN_COMPENSATION__ is a valid value) or RESET (__GAIN_COMPENSATION__ is invalid)
  */
#define IS_ADC_GAIN_COMPENSATION(__GAIN_COMPENSATION__) ((__GAIN_COMPENSATION__) <= 0x3FFFUL)

/**
  * @brief Verify the ADC offset.
  * @param __OFFSET__ programmed ADC offset.
  * @retval SET (__OFFSET__ is a valid value) or RESET (__OFFSET__ is invalid)
  */
#define IS_ADC_OFFSET(__OFFSET__) ((__OFFSET__) <= 0x3FFFFFUL)

/**
  * @brief Verify the ADC scan mode.
  * @param __SCAN_MODE__ programmed ADC scan mode.
  * @retval SET (__SCAN_MODE__ is valid) or RESET (__SCAN_MODE__ is invalid)
  */
#define IS_ADC_SCAN_MODE(__SCAN_MODE__) (((__SCAN_MODE__) == ADC_SCAN_DISABLE) || \
                                         ((__SCAN_MODE__) == ADC_SCAN_ENABLE)    )

/**
  * @brief Verify the ADC edge trigger setting for regular group.
  * @param __EDGE__ programmed ADC edge trigger setting.
  * @retval SET (__EDGE__ is a valid value) or RESET (__EDGE__ is invalid)
  */
#define IS_ADC_EXTTRIG_EDGE(__EDGE__) (((__EDGE__) == ADC_EXTERNALTRIGCONVEDGE_NONE)         || \
                                       ((__EDGE__) == ADC_EXTERNALTRIGCONVEDGE_RISING)       || \
                                       ((__EDGE__) == ADC_EXTERNALTRIGCONVEDGE_FALLING)      || \
                                       ((__EDGE__) == ADC_EXTERNALTRIGCONVEDGE_RISINGFALLING)  )

/**
  * @brief Verify the ADC regular conversions external trigger.
  * @param __ADC_INSTANCE__ ADC instance
  * @param __REG_TRIG_SOURCE__ programmed ADC regular conversions external trigger.
  * @retval SET (__REG_TRIG_SOURCE__ is a valid value) or RESET (__REG_TRIG_SOURCE__ is invalid)
  */
#define IS_ADC_EXTTRIG(__ADC_INSTANCE__, __REG_TRIG_SOURCE__)            \
  (((__REG_TRIG_SOURCE__) == ADC_SOFTWARE_START)                         \
   || ((__REG_TRIG_SOURCE__) == ADC_EXTERNALTRIG_EXT_IT11)               \
   || ((__REG_TRIG_SOURCE__) == ADC_EXTERNALTRIG_T1_CC1)                 \
   || ((__REG_TRIG_SOURCE__) == ADC_EXTERNALTRIG_T1_CC2)                 \
   || ((__REG_TRIG_SOURCE__) == ADC_EXTERNALTRIG_T1_CC3)                 \
   || ((__REG_TRIG_SOURCE__) == ADC_EXTERNALTRIG_T1_TRGO)                \
   || ((__REG_TRIG_SOURCE__) == ADC_EXTERNALTRIG_T1_TRGO2)               \
   || ((__REG_TRIG_SOURCE__) == ADC_EXTERNALTRIG_T2_CC2)                 \
   || ((__REG_TRIG_SOURCE__) == ADC_EXTERNALTRIG_T2_TRGO)                \
   || ((__REG_TRIG_SOURCE__) == ADC_EXTERNALTRIG_T3_CC4)                 \
   || ((__REG_TRIG_SOURCE__) == ADC_EXTERNALTRIG_T3_TRGO)                \
   || ((__REG_TRIG_SOURCE__) == ADC_EXTERNALTRIG_T4_CC4)                 \
   || ((__REG_TRIG_SOURCE__) == ADC_EXTERNALTRIG_T4_TRGO)                \
   || ((__REG_TRIG_SOURCE__) == ADC_EXTERNALTRIG_T5_TRGO)                \
   || ((__REG_TRIG_SOURCE__) == ADC_EXTERNALTRIG_T6_TRGO)                \
   || ((__REG_TRIG_SOURCE__) == ADC_EXTERNALTRIG_T7_TRGO)                \
   || ((__REG_TRIG_SOURCE__) == ADC_EXTERNALTRIG_T8_TRGO)                \
   || ((__REG_TRIG_SOURCE__) == ADC_EXTERNALTRIG_T8_TRGO2)               \
   || ((__REG_TRIG_SOURCE__) == ADC_EXTERNALTRIG_T9_CC1)                 \
   || ((__REG_TRIG_SOURCE__) == ADC_EXTERNALTRIG_T9_TRGO)                \
   || ((__REG_TRIG_SOURCE__) == ADC_EXTERNALTRIG_T12_TRGO)               \
   || ((__REG_TRIG_SOURCE__) == ADC_EXTERNALTRIG_T15_TRGO)               \
   || ((__REG_TRIG_SOURCE__) == ADC_EXTERNALTRIG_T18_TRGO)               \
   || ((__REG_TRIG_SOURCE__) == ADC_EXTERNALTRIG_LPTIM1_CH1)             \
   || ((__REG_TRIG_SOURCE__) == ADC_EXTERNALTRIG_LPTIM2_CH1)             \
   || ((__REG_TRIG_SOURCE__) == ADC_EXTERNALTRIG_LPTIM3_CH1)             \
  )


/**
  * @brief Verify the ADC regular conversions external trigger.
  * @param __SAMPLINGMODE__ programmed ADC regular conversions external trigger.
  * @retval SET (__SAMPLINGMODE__ is a valid value) or RESET (__SAMPLINGMODE__ is invalid)
  */
#define IS_ADC_SAMPLINGMODE(__SAMPLINGMODE__) (((__SAMPLINGMODE__) == ADC_SAMPLING_MODE_NORMAL)          || \
                                               ((__SAMPLINGMODE__) == ADC_SAMPLING_MODE_BULB)            || \
                                               ((__SAMPLINGMODE__) == ADC_SAMPLING_MODE_TRIGGER_CONTROLED)  )

/**
  * @brief Verify the ADC regular conversions check for converted data availability.
  * @param __EOC_SELECTION__ converted data availability check.
  * @retval SET (__EOC_SELECTION__ is a valid value) or RESET (__EOC_SELECTION__ is invalid)
  */
#define IS_ADC_EOC_SELECTION(__EOC_SELECTION__) (((__EOC_SELECTION__) == ADC_EOC_SINGLE_CONV)    || \
                                                 ((__EOC_SELECTION__) == ADC_EOC_SEQ_CONV)  )

/**
  * @brief Verify the ADC regular conversions overrun handling.
  * @param __OVR__ ADC regular conversions overrun handling.
  * @retval SET (__OVR__ is a valid value) or RESET (__OVR__ is invalid)
  */
#define IS_ADC_OVERRUN(__OVR__) (((__OVR__) == ADC_OVR_DATA_PRESERVED)  || \
                                 ((__OVR__) == ADC_OVR_DATA_OVERWRITTEN)  )

/**
  * @brief Verify the ADC conversions sampling time.
  * @param __SAMPLING_TIME__ ADC conversions sampling time.
  * @retval SET (__SAMPLING_TIME__ is a valid value) or RESET (__SAMPLING_TIME__ is invalid)
  */
#define IS_ADC_SAMPLING_TIME(__SAMPLING_TIME__) (((__SAMPLING_TIME__) == ADC_SAMPLETIME_1CYCLE_5)    || \
                                                 ((__SAMPLING_TIME__) == ADC_SAMPLETIME_2CYCLES_5)   || \
                                                 ((__SAMPLING_TIME__) == ADC_SAMPLETIME_6CYCLES_5)   || \
                                                 ((__SAMPLING_TIME__) == ADC_SAMPLETIME_11CYCLES_5)  || \
                                                 ((__SAMPLING_TIME__) == ADC_SAMPLETIME_23CYCLES_5)  || \
                                                 ((__SAMPLING_TIME__) == ADC_SAMPLETIME_46CYCLES_5)  || \
                                                 ((__SAMPLING_TIME__) == ADC_SAMPLETIME_246CYCLES_5) || \
                                                 ((__SAMPLING_TIME__) == ADC_SAMPLETIME_1499CYCLES_5)   )

/**
  * @brief Verify the ADC regular channel setting.
  * @param  __CHANNEL__ programmed ADC regular channel.
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
  * @}
  */


/* Private constants ---------------------------------------------------------*/

/** @defgroup ADC_Private_Constants ADC Private Constants
  * @{
  */

/* Fixed timeout values for ADC conversion (including sampling time)          */
/* Maximum sampling time is 1499.5 ADC clock cycle (SMPx[2:0] = 0b111)        */
/* Maximum conversion time is 12.5 + Maximum samling time                     */
/*                       or 12.5  + 1499.5 = 1512 ADC clock cycles            */
/* Minimum ADC Clock frequency is 0.7 MHz                                     */
/* (refer to device datasheet, parameter "fADC")                              */
/* Maximum conversion time is                                                 */
/*                      = 1512 / 0.7MHz = 2.16 ms                             */
/* Used timeout value includes a margin versus theoretical max value          */
#define ADC_STOP_CONVERSION_TIMEOUT     (5UL)    /*!< ADC stop time-out value */

/* Delay for temperature sensor stabilization time.                         */
/* Maximum delay is 120us (refer device datasheet, parameter tSTART).       */
/* Unit: us                                                                 */
#define ADC_TEMPSENSOR_DELAY_US         (LL_ADC_DELAY_TEMPSENSOR_STAB_US)

/* Delay for ADC voltage regulator startup time                               */
/* Maximum delay is 10 microseconds                                           */
/* (refer device RM, parameter Tadcvreg_stup).                                */
#define ADC_STAB_DELAY_US               ((uint32_t) 10)     /*!< ADC voltage regulator startup time */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/

/** @defgroup ADC_Exported_Macros ADC Exported Macros
  * @{
  */
/* Macro for internal HAL driver usage, and possibly can be used into code of */
/* final user.                                                                */

/** @defgroup ADC_HAL_EM_HANDLE_IT_FLAG HAL ADC macro to manage HAL ADC handle, IT and flags.
  * @{
  */

/** @brief  Reset ADC handle state.
  * @param __HANDLE__ ADC handle
  * @retval None
  */
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
#define __HAL_ADC_RESET_HANDLE_STATE(__HANDLE__)                               \
  do{                                                                          \
    (__HANDLE__)->State = HAL_ADC_STATE_RESET;                                 \
    (__HANDLE__)->MspInitCallback = NULL;                                      \
    (__HANDLE__)->MspDeInitCallback = NULL;                                    \
  } while(0)
#else
#define __HAL_ADC_RESET_HANDLE_STATE(__HANDLE__)                               \
  ((__HANDLE__)->State = HAL_ADC_STATE_RESET)
#endif /* USE_HAL_ADC_REGISTER_CALLBACKS */

/**
  * @brief Enable ADC interrupt.
  * @param __HANDLE__ ADC handle
  * @param __INTERRUPT__ ADC Interrupt
  *        This parameter can be one of the following values:
  *            @arg @ref ADC_IT_RDY    ADC Ready interrupt source
  *            @arg @ref ADC_IT_EOSMP  ADC End of Sampling interrupt source
  *            @arg @ref ADC_IT_EOC    ADC End of Regular Conversion interrupt source
  *            @arg @ref ADC_IT_EOS    ADC End of Regular sequence of Conversions interrupt source
  *            @arg @ref ADC_IT_OVR    ADC overrun interrupt source
  *            @arg @ref ADC_IT_JEOC   ADC End of Injected Conversion interrupt source
  *            @arg @ref ADC_IT_JEOS   ADC End of Injected sequence of Conversions interrupt source
  *            @arg @ref ADC_IT_AWD1   ADC Analog watchdog 1 interrupt source (main analog watchdog)
  *            @arg @ref ADC_IT_AWD2   ADC Analog watchdog 2 interrupt source (additional analog watchdog)
  *            @arg @ref ADC_IT_AWD3   ADC Analog watchdog 3 interrupt source (additional analog watchdog)
  * @retval None
  */
#define __HAL_ADC_ENABLE_IT(__HANDLE__, __INTERRUPT__)                         \
  (((__HANDLE__)->Instance->IER) |= (__INTERRUPT__))

/**
  * @brief Disable ADC interrupt.
  * @param __HANDLE__ ADC handle
  * @param __INTERRUPT__ ADC Interrupt
  *        This parameter can be one of the following values:
  *            @arg @ref ADC_IT_RDY    ADC Ready interrupt source
  *            @arg @ref ADC_IT_EOSMP  ADC End of Sampling interrupt source
  *            @arg @ref ADC_IT_EOC    ADC End of Regular Conversion interrupt source
  *            @arg @ref ADC_IT_EOS    ADC End of Regular sequence of Conversions interrupt source
  *            @arg @ref ADC_IT_OVR    ADC overrun interrupt source
  *            @arg @ref ADC_IT_JEOC   ADC End of Injected Conversion interrupt source
  *            @arg @ref ADC_IT_JEOS   ADC End of Injected sequence of Conversions interrupt source
  *            @arg @ref ADC_IT_AWD1   ADC Analog watchdog 1 interrupt source (main analog watchdog)
  *            @arg @ref ADC_IT_AWD2   ADC Analog watchdog 2 interrupt source (additional analog watchdog)
  *            @arg @ref ADC_IT_AWD3   ADC Analog watchdog 3 interrupt source (additional analog watchdog)
  * @retval None
  */
#define __HAL_ADC_DISABLE_IT(__HANDLE__, __INTERRUPT__)                        \
  (((__HANDLE__)->Instance->IER) &= ~(__INTERRUPT__))

/** @brief  Checks if the specified ADC interrupt source is enabled or disabled.
  * @param __HANDLE__ ADC handle
  * @param __INTERRUPT__ ADC interrupt source to check
  *          This parameter can be one of the following values:
  *            @arg @ref ADC_IT_RDY    ADC Ready interrupt source
  *            @arg @ref ADC_IT_EOSMP  ADC End of Sampling interrupt source
  *            @arg @ref ADC_IT_EOC    ADC End of Regular Conversion interrupt source
  *            @arg @ref ADC_IT_EOS    ADC End of Regular sequence of Conversions interrupt source
  *            @arg @ref ADC_IT_OVR    ADC overrun interrupt source
  *            @arg @ref ADC_IT_JEOC   ADC End of Injected Conversion interrupt source
  *            @arg @ref ADC_IT_JEOS   ADC End of Injected sequence of Conversions interrupt source
  *            @arg @ref ADC_IT_AWD1   ADC Analog watchdog 1 interrupt source (main analog watchdog)
  *            @arg @ref ADC_IT_AWD2   ADC Analog watchdog 2 interrupt source (additional analog watchdog)
  *            @arg @ref ADC_IT_AWD3   ADC Analog watchdog 3 interrupt source (additional analog watchdog)
  * @retval State of interruption (SET or RESET)
  */
#define __HAL_ADC_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__)                     \
  (((__HANDLE__)->Instance->IER & (__INTERRUPT__)) == (__INTERRUPT__))

/**
  * @brief Check whether the specified ADC flag is set or not.
  * @param __HANDLE__ ADC handle
  * @param __FLAG__ ADC flag
  *        This parameter can be one of the following values:
  *            @arg @ref ADC_FLAG_RDY     ADC Ready flag
  *            @arg @ref ADC_FLAG_EOSMP   ADC End of Sampling flag
  *            @arg @ref ADC_FLAG_EOC     ADC End of Regular Conversion flag
  *            @arg @ref ADC_FLAG_EOS     ADC End of Regular sequence of Conversions flag
  *            @arg @ref ADC_FLAG_OVR     ADC overrun flag
  *            @arg @ref ADC_FLAG_JEOC    ADC End of Injected Conversion flag
  *            @arg @ref ADC_FLAG_JEOS    ADC End of Injected sequence of Conversions flag
  *            @arg @ref ADC_FLAG_AWD1    ADC Analog watchdog 1 flag (main analog watchdog)
  *            @arg @ref ADC_FLAG_AWD2    ADC Analog watchdog 2 flag (additional analog watchdog)
  *            @arg @ref ADC_FLAG_AWD3    ADC Analog watchdog 3 flag (additional analog watchdog)
  * @retval State of flag (TRUE or FALSE).
  */
#define __HAL_ADC_GET_FLAG(__HANDLE__, __FLAG__)                               \
  ((((__HANDLE__)->Instance->ISR) & (__FLAG__)) == (__FLAG__))

/**
  * @brief Clear the specified ADC flag.
  * @param __HANDLE__ ADC handle
  * @param __FLAG__ ADC flag
  *        This parameter can be one of the following values:
  *            @arg @ref ADC_FLAG_RDY     ADC Ready flag
  *            @arg @ref ADC_FLAG_EOSMP   ADC End of Sampling flag
  *            @arg @ref ADC_FLAG_EOC     ADC End of Regular Conversion flag
  *            @arg @ref ADC_FLAG_EOS     ADC End of Regular sequence of Conversions flag
  *            @arg @ref ADC_FLAG_OVR     ADC overrun flag
  *            @arg @ref ADC_FLAG_JEOC    ADC End of Injected Conversion flag
  *            @arg @ref ADC_FLAG_JEOS    ADC End of Injected sequence of Conversions flag
  *            @arg @ref ADC_FLAG_AWD1    ADC Analog watchdog 1 flag (main analog watchdog)
  *            @arg @ref ADC_FLAG_AWD2    ADC Analog watchdog 2 flag (additional analog watchdog)
  *            @arg @ref ADC_FLAG_AWD3    ADC Analog watchdog 3 flag (additional analog watchdog)
  * @retval None
  */
/* Note: bit cleared bit by writing 1 (writing 0 has no effect on any bit of register ISR) */
#define __HAL_ADC_CLEAR_FLAG(__HANDLE__, __FLAG__)                             \
  (((__HANDLE__)->Instance->ISR) = (__FLAG__))

/**
  * @}
  */

/** @defgroup ADC_HAL_EM_HELPER_MACRO HAL ADC helper macro
  * @{
  */

/**
  * @brief  Helper macro to get ADC channel number in decimal format
  *         from literals ADC_CHANNEL_x.
  * @note   Example:
  *           __HAL_ADC_CHANNEL_TO_DECIMAL_NB(ADC_CHANNEL_4)
  *           will return decimal number "4".
  * @note   The input can be a value from functions where a channel
  *         number is returned, either defined with number
  *         or with bitfield (only one bit must be set).
  * @param  __CHANNEL__ This parameter can be one of the following values:
  *         @arg @ref ADC_CHANNEL_0
  *         @arg @ref ADC_CHANNEL_1
  *         @arg @ref ADC_CHANNEL_2
  *         @arg @ref ADC_CHANNEL_3
  *         @arg @ref ADC_CHANNEL_4
  *         @arg @ref ADC_CHANNEL_5
  *         @arg @ref ADC_CHANNEL_6
  *         @arg @ref ADC_CHANNEL_7
  *         @arg @ref ADC_CHANNEL_8
  *         @arg @ref ADC_CHANNEL_9
  *         @arg @ref ADC_CHANNEL_10
  *         @arg @ref ADC_CHANNEL_11
  *         @arg @ref ADC_CHANNEL_12
  *         @arg @ref ADC_CHANNEL_13
  *         @arg @ref ADC_CHANNEL_14
  *         @arg @ref ADC_CHANNEL_15
  *         @arg @ref ADC_CHANNEL_16
  *         @arg @ref ADC_CHANNEL_17
  *         @arg @ref ADC_CHANNEL_18
  *         @arg @ref ADC_CHANNEL_19
  *         @arg @ref ADC_CHANNEL_VREFINT     (1)(4)
  *         @arg @ref ADC_CHANNEL_VBAT        (2)(4)
  *         @arg @ref ADC_CHANNEL_VDDCORE     (2)(4)
  *
  *         (1) On this STM32 series, parameter available only on ADC instance: ADC1.\n
  *         (2) On this STM32 series, parameter available only on ADC instance: ADC2.\n
  * @retval Value between Min_Data=0 and Max_Data=18
  */
#define __HAL_ADC_CHANNEL_TO_DECIMAL_NB(__CHANNEL__)                           \
  __LL_ADC_CHANNEL_TO_DECIMAL_NB((__CHANNEL__))

/**
  * @brief  Helper macro to get ADC channel in literal format ADC_CHANNEL_x
  *         from number in decimal format.
  * @note   Example:
  *           __HAL_ADC_DECIMAL_NB_TO_CHANNEL(4)
  *           will return a data equivalent to "ADC_CHANNEL_4".
  * @param  __DECIMAL_NB__ Value between Min_Data=0 and Max_Data=18
  * @retval Returned value can be one of the following values:
  *         @arg @ref ADC_CHANNEL_0
  *         @arg @ref ADC_CHANNEL_1
  *         @arg @ref ADC_CHANNEL_2
  *         @arg @ref ADC_CHANNEL_3
  *         @arg @ref ADC_CHANNEL_4
  *         @arg @ref ADC_CHANNEL_5
  *         @arg @ref ADC_CHANNEL_6
  *         @arg @ref ADC_CHANNEL_7
  *         @arg @ref ADC_CHANNEL_8
  *         @arg @ref ADC_CHANNEL_9
  *         @arg @ref ADC_CHANNEL_10
  *         @arg @ref ADC_CHANNEL_11
  *         @arg @ref ADC_CHANNEL_12
  *         @arg @ref ADC_CHANNEL_13
  *         @arg @ref ADC_CHANNEL_14
  *         @arg @ref ADC_CHANNEL_15
  *         @arg @ref ADC_CHANNEL_16
  *         @arg @ref ADC_CHANNEL_17
  *         @arg @ref ADC_CHANNEL_18
  *         @arg @ref ADC_CHANNEL_19
  *         @arg @ref ADC_CHANNEL_VREFINT     (1)(3)
  *         @arg @ref ADC_CHANNEL_VBAT        (2)(3)
  *         @arg @ref ADC_CHANNEL_VDDCORE     (2)(3)
  *
  *         (1) On this STM32 series, parameter available only on ADC instance: ADC1.\n
  *         (2) On this STM32 series, parameter available only on ADC instance: ADC2.\n
  *         (3) For ADC channel read back from ADC register,
  *             comparison with internal channel parameter to be done
  *             using helper macro @ref __LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL().
  */
#define __HAL_ADC_DECIMAL_NB_TO_CHANNEL(__DECIMAL_NB__)                        \
  __LL_ADC_DECIMAL_NB_TO_CHANNEL((__DECIMAL_NB__))

/**
  * @brief  Helper macro to determine whether the selected channel
  *         corresponds to literal definitions of driver.
  * @note   The different literal definitions of ADC channels are:
  *         - ADC internal channel:
  *           ADC_CHANNEL_VREFINT, ...
  *         - ADC external channel (channel connected to a GPIO pin):
  *           ADC_CHANNEL_1, ADC_CHANNEL_2, ...
  * @note   The channel parameter must be a value defined from literal
  *         definition of a ADC internal channel (ADC_CHANNEL_VREFINT, ...),
  *         ADC external channel (ADC_CHANNEL_1, ADC_CHANNEL_2, ...),
  *         must not be a value from functions where a channel number is
  *         returned from ADC registers,
  *         because internal and external channels share the same channel
  *         number in ADC registers. The differentiation is made only with
  *         parameters definitions of driver.
  * @param  __CHANNEL__ This parameter can be one of the following values:
  *         @arg @ref ADC_CHANNEL_0
  *         @arg @ref ADC_CHANNEL_1
  *         @arg @ref ADC_CHANNEL_2
  *         @arg @ref ADC_CHANNEL_3
  *         @arg @ref ADC_CHANNEL_4
  *         @arg @ref ADC_CHANNEL_5
  *         @arg @ref ADC_CHANNEL_6
  *         @arg @ref ADC_CHANNEL_7
  *         @arg @ref ADC_CHANNEL_8
  *         @arg @ref ADC_CHANNEL_9
  *         @arg @ref ADC_CHANNEL_10
  *         @arg @ref ADC_CHANNEL_11
  *         @arg @ref ADC_CHANNEL_12
  *         @arg @ref ADC_CHANNEL_13
  *         @arg @ref ADC_CHANNEL_14
  *         @arg @ref ADC_CHANNEL_15
  *         @arg @ref ADC_CHANNEL_16
  *         @arg @ref ADC_CHANNEL_17
  *         @arg @ref ADC_CHANNEL_18
  *         @arg @ref ADC_CHANNEL_19
  *         @arg @ref ADC_CHANNEL_VREFINT     (1)(4)
  *         @arg @ref ADC_CHANNEL_VBAT        (2)(4)
  *         @arg @ref ADC_CHANNEL_VDDCORE     (2)(4)
  *
  *         (1) On this STM32 series, parameter available only on ADC instance: ADC1.\n
  *         (2) On this STM32 series, parameter available only on ADC instance: ADC2.\n
  * @retval Value "0" if the channel corresponds to a parameter definition of a ADC external channel
  *         (channel connected to a GPIO pin).
  *         Value "1" if the channel corresponds to a parameter definition of a ADC internal channel.
  */
#define __HAL_ADC_IS_CHANNEL_INTERNAL(__CHANNEL__)                             \
  __LL_ADC_IS_CHANNEL_INTERNAL((__CHANNEL__))

/**
  * @brief  Helper macro to convert a channel defined from parameter
  *         definition of a ADC internal channel (ADC_CHANNEL_VREFINT, ...),
  *         to its equivalent parameter definition of a ADC external channel
  *         (ADC_CHANNEL_1, ADC_CHANNEL_2, ...).
  * @note   The channel parameter can be, additionally to a value
  *         defined from parameter definition of a ADC internal channel
  *         (ADC_CHANNEL_VREFINT, ...),
  *         a value defined from parameter definition of
  *         ADC external channel (ADC_CHANNEL_1, ADC_CHANNEL_2, ...)
  *         or a value from functions where a channel number is returned
  *         from ADC registers.
  * @param  __CHANNEL__ This parameter can be one of the following values:
  *         @arg @ref ADC_CHANNEL_0
  *         @arg @ref ADC_CHANNEL_1
  *         @arg @ref ADC_CHANNEL_2
  *         @arg @ref ADC_CHANNEL_3
  *         @arg @ref ADC_CHANNEL_4
  *         @arg @ref ADC_CHANNEL_5
  *         @arg @ref ADC_CHANNEL_6
  *         @arg @ref ADC_CHANNEL_7
  *         @arg @ref ADC_CHANNEL_8
  *         @arg @ref ADC_CHANNEL_9
  *         @arg @ref ADC_CHANNEL_10
  *         @arg @ref ADC_CHANNEL_11
  *         @arg @ref ADC_CHANNEL_12
  *         @arg @ref ADC_CHANNEL_13
  *         @arg @ref ADC_CHANNEL_14
  *         @arg @ref ADC_CHANNEL_15
  *         @arg @ref ADC_CHANNEL_16
  *         @arg @ref ADC_CHANNEL_17
  *         @arg @ref ADC_CHANNEL_18
  *         @arg @ref ADC_CHANNEL_19
  *         @arg @ref ADC_CHANNEL_VREFINT     (1)(4)
  *         @arg @ref ADC_CHANNEL_VBAT        (2)(4)
  *         @arg @ref ADC_CHANNEL_VDDCORE     (2)(4)
  *
  *         (1) On this STM32 series, parameter available only on ADC instance: ADC1.\n
  *         (2) On this STM32 series, parameter available only on ADC instance: ADC2.\n
  * @retval Returned value can be one of the following values:
  *         @arg @ref ADC_CHANNEL_0
  *         @arg @ref ADC_CHANNEL_1
  *         @arg @ref ADC_CHANNEL_2
  *         @arg @ref ADC_CHANNEL_3
  *         @arg @ref ADC_CHANNEL_4
  *         @arg @ref ADC_CHANNEL_5
  *         @arg @ref ADC_CHANNEL_6
  *         @arg @ref ADC_CHANNEL_7
  *         @arg @ref ADC_CHANNEL_8
  *         @arg @ref ADC_CHANNEL_9
  *         @arg @ref ADC_CHANNEL_10
  *         @arg @ref ADC_CHANNEL_11
  *         @arg @ref ADC_CHANNEL_12
  *         @arg @ref ADC_CHANNEL_13
  *         @arg @ref ADC_CHANNEL_14
  *         @arg @ref ADC_CHANNEL_15
  *         @arg @ref ADC_CHANNEL_16
  *         @arg @ref ADC_CHANNEL_17
  *         @arg @ref ADC_CHANNEL_18
  *         @arg @ref ADC_CHANNEL_19
  */
#define __HAL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL(__CHANNEL__)                    \
  __LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL((__CHANNEL__))

/**
  * @brief  Helper macro to determine whether the internal channel
  *         selected is available on the ADC instance selected.
  * @note   The channel parameter must be a value defined from parameter
  *         definition of a ADC internal channel (ADC_CHANNEL_VREFINT, ...),
  *         must not be a value defined from parameter definition of
  *         ADC external channel (ADC_CHANNEL_1, ADC_CHANNEL_2, ...)
  *         or a value from functions where a channel number is
  *         returned from ADC registers,
  *         because internal and external channels share the same channel
  *         number in ADC registers. The differentiation is made only with
  *         parameters definitions of driver.
  * @param  __ADC_INSTANCE__ ADC instance
  * @param  __CHANNEL__ This parameter can be one of the following values:
  *         @arg @ref ADC_CHANNEL_VREFINT     (1)
  *         @arg @ref ADC_CHANNEL_VBAT        (2)
  *         @arg @ref ADC_CHANNEL_VDDCORE     (2)
  *
  *         (1) On this STM32 series, parameter available only on ADC instance: ADC1.\n
  *         (2) On this STM32 series, parameter available only on ADC instance: ADC2.
  * @retval Value "0" if the internal channel selected is not available on the ADC instance selected.
  *         Value "1" if the internal channel selected is available on the ADC instance selected.
  */
#define __HAL_ADC_IS_CHANNEL_INTERNAL_AVAILABLE(__ADC_INSTANCE__, __CHANNEL__)  \
  __LL_ADC_IS_CHANNEL_INTERNAL_AVAILABLE((__ADC_INSTANCE__), (__CHANNEL__))

#if defined(ADC_MULTIMODE_SUPPORT)
/**
  * @brief  Helper macro to get the ADC multimode conversion data of ADC master
  *         or ADC slave from raw value with both ADC conversion data concatenated.
  * @note   This macro is intended to be used when multimode transfer by DMA
  *         is enabled: refer to function @ref __LL_ADC_MULTI_CONV_DATA_MASTER_SLAVE().
  *         In this case the transferred data need to processed with this macro
  *         to separate the conversion data of ADC master and ADC slave.
  * @param  __ADC_MULTI_MASTER_SLAVE__ This parameter can be one of the following values:
  *         @arg @ref LL_ADC_MULTI_MASTER
  *         @arg @ref LL_ADC_MULTI_SLAVE
  * @param  __ADC_MULTI_CONV_DATA__ Value between Min_Data=0x000 and Max_Data=0xFFF
  * @retval Value between Min_Data=0x000 and Max_Data=0xFFF
  */
#define __HAL_ADC_MULTI_CONV_DATA_MASTER_SLAVE(__ADC_MULTI_MASTER_SLAVE__, __ADC_MULTI_CONV_DATA__)  \
  __LL_ADC_MULTI_CONV_DATA_MASTER_SLAVE((__ADC_MULTI_MASTER_SLAVE__), (__ADC_MULTI_CONV_DATA__))
#endif /* ADC_MULTIMODE_SUPPORT */

/**
  * @brief  Helper macro to select the ADC common instance
  *         to which is belonging the selected ADC instance.
  * @note   ADC common register instance can be used for:
  *         - Set parameters common to several ADC instances
  *         - Multimode (for devices with several ADC instances)
  *         Refer to functions having argument "ADCxy_COMMON" as parameter.
  * @param  __ADCx__ ADC instance
  * @retval ADC common register instance
  */
#define __HAL_ADC_COMMON_INSTANCE(__ADCx__)                                    \
  __LL_ADC_COMMON_INSTANCE((__ADCx__))

/**
  * @brief  Helper macro to check if all ADC instances sharing the same
  *         ADC common instance are disabled.
  * @note   This check is required by functions with setting conditioned to
  *         ADC state:
  *         All ADC instances of the ADC common group must be disabled.
  *         Refer to functions having argument "ADCxy_COMMON" as parameter.
  * @note   On devices with only 1 ADC common instance, parameter of this macro
  *         is useless and can be ignored (parameter kept for compatibility
  *         with devices featuring several ADC common instances).
  * @param  __ADCXY_COMMON__ ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval Value "0" if all ADC instances sharing the same ADC common instance
  *         are disabled.
  *         Value "1" if at least one ADC instance sharing the same ADC common instance
  *         is enabled.
  */
#define __HAL_ADC_IS_ENABLED_ALL_COMMON_INSTANCE(__ADCXY_COMMON__)              \
  __LL_ADC_IS_ENABLED_ALL_COMMON_INSTANCE((__ADCXY_COMMON__))

/**
  * @brief  Helper macro to define the ADC conversion data full-scale digital
  *         value corresponding to the selected ADC resolution.
  * @note   ADC conversion data full-scale corresponds to voltage range
  *         determined by analog voltage references Vref+ and Vref-
  *         (refer to reference manual).
  * @param  __ADC_RESOLUTION__ This parameter can be one of the following values:
  *         @arg @ref ADC_RESOLUTION_12B
  *         @arg @ref ADC_RESOLUTION_10B
  *         @arg @ref ADC_RESOLUTION_8B
  *         @arg @ref ADC_RESOLUTION_6B
  * @retval ADC conversion data full-scale digital value
  */
#define __HAL_ADC_DIGITAL_SCALE(__ADC_RESOLUTION__)                             \
  __LL_ADC_DIGITAL_SCALE((__ADC_RESOLUTION__))

/**
  * @brief  Helper macro to convert the ADC conversion data from
  *         a resolution to another resolution.
  * @param  __DATA__ ADC conversion data to be converted
  * @param  __ADC_RESOLUTION_CURRENT__ Resolution of to the data to be converted
  *         This parameter can be one of the following values:
  *         @arg @ref ADC_RESOLUTION_12B
  *         @arg @ref ADC_RESOLUTION_10B
  *         @arg @ref ADC_RESOLUTION_8B
  *         @arg @ref ADC_RESOLUTION_6B
  * @param  __ADC_RESOLUTION_TARGET__ Resolution of the data after conversion
  *         This parameter can be one of the following values:
  *         @arg @ref ADC_RESOLUTION_12B
  *         @arg @ref ADC_RESOLUTION_10B
  *         @arg @ref ADC_RESOLUTION_8B
  *         @arg @ref ADC_RESOLUTION_6B
  * @retval ADC conversion data to the requested resolution
  */
#define __HAL_ADC_CONVERT_DATA_RESOLUTION(__DATA__,\
                                          __ADC_RESOLUTION_CURRENT__,\
                                          __ADC_RESOLUTION_TARGET__) \
__LL_ADC_CONVERT_DATA_RESOLUTION((__DATA__),\
                                 (__ADC_RESOLUTION_CURRENT__),\
                                 (__ADC_RESOLUTION_TARGET__))

/**
  * @brief  Helper macro to calculate the voltage (unit: mVolt)
  *         corresponding to a ADC conversion data (unit: digital value).
  * @note   Analog reference voltage (Vref+) must be either known from
  *         user board environment.
  * @param  __VREFANALOG_VOLTAGE__ Analog reference voltage (unit: mV)
  * @param  __ADC_DATA__ ADC conversion data (resolution 12 bits)
  *                       (unit: digital value).
  * @param  __ADC_RESOLUTION__ This parameter can be one of the following values:
  *         @arg @ref ADC_RESOLUTION_12B
  *         @arg @ref ADC_RESOLUTION_10B
  *         @arg @ref ADC_RESOLUTION_8B
  *         @arg @ref ADC_RESOLUTION_6B
  * @retval ADC conversion data equivalent voltage value (unit: mVolt)
  */
#define __HAL_ADC_CALC_DATA_TO_VOLTAGE(__VREFANALOG_VOLTAGE__,\
                                       __ADC_DATA__,\
                                       __ADC_RESOLUTION__) \
__LL_ADC_CALC_DATA_TO_VOLTAGE((__VREFANALOG_VOLTAGE__),\
                              (__ADC_DATA__),\
                              (__ADC_RESOLUTION__))

/**
  * @brief  Helper macro to calculate the voltage (unit: mVolt)
  *         corresponding to a ADC conversion data (unit: digital value)
  *         in differential ended mode.
  * @note   Analog reference voltage (Vref+) must be either known from
  *         user board environment.
  * @param  __VREFANALOG_VOLTAGE__ Analog reference voltage (unit: mV)
  * @param  __ADC_DATA__ ADC conversion data (resolution 12 bits)
  *                       (unit: digital value).
  * @param  __ADC_RESOLUTION__ This parameter can be one of the following values:
  *         @arg @ref ADC_RESOLUTION_12B
  *         @arg @ref ADC_RESOLUTION_10B
  *         @arg @ref ADC_RESOLUTION_8B
  *         @arg @ref ADC_RESOLUTION_6B
  * @retval ADC conversion data equivalent voltage value (unit: mVolt)
  */
#define __HAL_ADC_CALC_DIFF_DATA_TO_VOLTAGE(__VREFANALOG_VOLTAGE__,\
                                            __ADC_DATA__,\
                                            __ADC_RESOLUTION__) \
__LL_ADC_CALC_DIFF_DATA_TO_VOLTAGE((__VREFANALOG_VOLTAGE__),\
                                   (__ADC_DATA__),\
                                   (__ADC_RESOLUTION__))


/**
  * @}
  */

/**
  * @}
  */

/* Include ADC HAL Extended module */
#include "stm32n6xx_hal_adc_ex.h"

/* Exported functions --------------------------------------------------------*/
/** @addtogroup ADC_Exported_Functions
  * @{
  */

/** @addtogroup ADC_Exported_Functions_Group1
  * @brief    Initialization and Configuration functions
  * @{
  */
/* Initialization and de-initialization functions  ****************************/
HAL_StatusTypeDef       HAL_ADC_Init(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef       HAL_ADC_DeInit(ADC_HandleTypeDef *hadc);
void                    HAL_ADC_MspInit(ADC_HandleTypeDef *hadc);
void                    HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc);

#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
/* Callbacks Register/UnRegister functions  ***********************************/
HAL_StatusTypeDef HAL_ADC_RegisterCallback(ADC_HandleTypeDef *hadc, HAL_ADC_CallbackIDTypeDef CallbackID,
                                           pADC_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_ADC_UnRegisterCallback(ADC_HandleTypeDef *hadc, HAL_ADC_CallbackIDTypeDef CallbackID);
#endif /* USE_HAL_ADC_REGISTER_CALLBACKS */
/**
  * @}
  */

/** @addtogroup ADC_Exported_Functions_Group2
  * @brief    IO operation functions
  * @{
  */
/* IO operation functions  *****************************************************/

/* Blocking mode: Polling */
HAL_StatusTypeDef       HAL_ADC_Start(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef       HAL_ADC_Stop(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef       HAL_ADC_PollForConversion(ADC_HandleTypeDef *hadc, uint32_t Timeout);
HAL_StatusTypeDef       HAL_ADC_PollForEvent(ADC_HandleTypeDef *hadc, uint32_t EventType, uint32_t Timeout);

/* Non-blocking mode: Interruption */
HAL_StatusTypeDef       HAL_ADC_Start_IT(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef       HAL_ADC_Stop_IT(ADC_HandleTypeDef *hadc);

/* Non-blocking mode: DMA */
HAL_StatusTypeDef       HAL_ADC_Start_DMA(ADC_HandleTypeDef *hadc, const uint32_t *pData, uint32_t Length);
HAL_StatusTypeDef       HAL_ADC_Stop_DMA(ADC_HandleTypeDef *hadc);

/* ADC retrieve conversion value intended to be used with polling or interruption */
uint32_t                HAL_ADC_GetValue(const ADC_HandleTypeDef *hadc);
int32_t                 HAL_ADC_GetSignedValue(const ADC_HandleTypeDef *hadc);

/* ADC sampling control */
HAL_StatusTypeDef HAL_ADC_StartSampling(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADC_StopSampling(ADC_HandleTypeDef *hadc);

/* ADC IRQHandler and Callbacks used in non-blocking modes (Interruption and DMA) */
void                    HAL_ADC_IRQHandler(ADC_HandleTypeDef *hadc);
void                    HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc);
void                    HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc);
void                    HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef *hadc);
void                    HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc);
/**
  * @}
  */

/** @addtogroup ADC_Exported_Functions_Group3 Peripheral Control functions
  *  @brief    Peripheral Control functions
  * @{
  */
/* Peripheral Control functions ***********************************************/
HAL_StatusTypeDef       HAL_ADC_ConfigChannel(ADC_HandleTypeDef *hadc, const ADC_ChannelConfTypeDef *pConfig);
HAL_StatusTypeDef       HAL_ADC_AnalogWDGConfig(ADC_HandleTypeDef *hadc,
                                                const ADC_AnalogWDGConfTypeDef *pAnalogWDGConfig);

/**
  * @}
  */

/* Peripheral State functions *************************************************/
/** @addtogroup ADC_Exported_Functions_Group4
  * @{
  */
uint32_t                HAL_ADC_GetState(const ADC_HandleTypeDef *hadc);
uint32_t                HAL_ADC_GetError(const ADC_HandleTypeDef *hadc);

/**
  * @}
  */

/**
  * @}
  */

/* Private functions -----------------------------------------------------------*/
/** @addtogroup ADC_Private_Functions ADC Private Functions
  * @{
  */
HAL_StatusTypeDef ADC_ConversionStop(ADC_HandleTypeDef *hadc, uint32_t ConversionGroup);
HAL_StatusTypeDef ADC_Enable(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef ADC_Disable(ADC_HandleTypeDef *hadc);
void ADC_DMAConvCplt(DMA_HandleTypeDef *hdma);
void ADC_DMAHalfConvCplt(DMA_HandleTypeDef *hdma);
void ADC_DMAError(DMA_HandleTypeDef *hdma);
void ADC_ConfigureBoostMode(ADC_HandleTypeDef *hadc);

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

#endif /* STM32N6xx_HAL_ADC_H */
