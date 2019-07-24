/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_ADC_H_
#define _FSL_ADC_H_

#include "fsl_common.h"

/*!
 *  @addtogroup adc_12b1msps_sar
 *  @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief ADC driver version */
#define FSL_ADC_DRIVER_VERSION (MAKE_VERSION(2, 0, 2)) /*!< Version 2.0.2. */

/*!
 * @brief Converter's status flags.
 */
typedef enum _adc_status_flags
{
    kADC_ConversionActiveFlag  = ADC_GS_ADACT_MASK, /*!< Conversion is active,not support w1c. */
    kADC_CalibrationFailedFlag = ADC_GS_CALF_MASK,  /*!< Calibration is failed,support w1c. */
    kADC_AsynchronousWakeupInterruptFlag =
        ADC_GS_AWKST_MASK, /*!< Asynchronous wakeup interrupt occurred, support w1c. */
} adc_status_flags_t;

/*!
 * @brief Reference voltage source.
 */
typedef enum _adc_reference_voltage_source
{
    kADC_ReferenceVoltageSourceAlt0 = 0U, /*!< For external pins pair of VrefH and VrefL. */
} adc_reference_voltage_source_t;

/*!
 * @brief Sample time duration.
 */
typedef enum _adc_sample_period_mode
{
    /* This group of enumeration is for internal use which is related to register setting. */
    kADC_SamplePeriod2or12Clocks = 0U, /*!< Long sample 12 clocks or short sample 2 clocks. */
    kADC_SamplePeriod4or16Clocks = 1U, /*!< Long sample 16 clocks or short sample 4 clocks. */
    kADC_SamplePeriod6or20Clocks = 2U, /*!< Long sample 20 clocks or short sample 6 clocks. */
    kADC_SamplePeriod8or24Clocks = 3U, /*!< Long sample 24 clocks or short sample 8 clocks. */
    /* This group of enumeration is for a public user. */
    /* For long sample mode. */
    kADC_SamplePeriodLong12Clcoks = kADC_SamplePeriod2or12Clocks, /*!< Long sample 12 clocks. */
    kADC_SamplePeriodLong16Clcoks = kADC_SamplePeriod4or16Clocks, /*!< Long sample 16 clocks. */
    kADC_SamplePeriodLong20Clcoks = kADC_SamplePeriod6or20Clocks, /*!< Long sample 20 clocks. */
    kADC_SamplePeriodLong24Clcoks = kADC_SamplePeriod8or24Clocks, /*!< Long sample 24 clocks. */
    /* For short sample mode. */
    kADC_SamplePeriodShort2Clocks = kADC_SamplePeriod2or12Clocks, /*!< Short sample 2 clocks. */
    kADC_SamplePeriodShort4Clocks = kADC_SamplePeriod4or16Clocks, /*!< Short sample 4 clocks. */
    kADC_SamplePeriodShort6Clocks = kADC_SamplePeriod6or20Clocks, /*!< Short sample 6 clocks. */
    kADC_SamplePeriodShort8Clocks = kADC_SamplePeriod8or24Clocks, /*!< Short sample 8 clocks. */
} adc_sample_period_mode_t;

/*!
 * @brief Clock source.
 */
typedef enum _adc_clock_source
{
    kADC_ClockSourceIPG     = 0U, /*!< Select IPG clock to generate ADCK. */
    kADC_ClockSourceIPGDiv2 = 1U, /*!< Select IPG clock divided by 2 to generate ADCK. */
#if !(defined(FSL_FEATURE_ADC_SUPPORT_ALTCLK_REMOVE) && FSL_FEATURE_ADC_SUPPORT_ALTCLK_REMOVE)
    kADC_ClockSourceALT = 2U, /*!< Select alternate clock to generate ADCK. */
#endif
    kADC_ClockSourceAD = 3U, /*!< Select Asynchronous clock to generate ADCK. */
} adc_clock_source_t;

/*!
 * @brief Clock divider for the converter.
 */
typedef enum _adc_clock_drvier
{
    kADC_ClockDriver1 = 0U, /*!< For divider 1 from the input clock to the module. */
    kADC_ClockDriver2 = 1U, /*!< For divider 2 from the input clock to the module. */
    kADC_ClockDriver4 = 2U, /*!< For divider 4 from the input clock to the module. */
    kADC_ClockDriver8 = 3U, /*!< For divider 8 from the input clock to the module. */
} adc_clock_driver_t;

/*!
 * @brief Converter's resolution.
 */
typedef enum _adc_resolution
{
    kADC_Resolution8Bit  = 0U, /*!< Single End 8-bit resolution. */
    kADC_Resolution10Bit = 1U, /*!< Single End 10-bit resolution. */
    kADC_Resolution12Bit = 2U, /*!< Single End 12-bit resolution. */
} adc_resolution_t;

/*!
 * @brief Converter hardware compare mode.
 */
typedef enum _adc_hardware_compare_mode
{
    kADC_HardwareCompareMode0 = 0U, /*!< Compare true if the result is less than the value1. */
    kADC_HardwareCompareMode1 = 1U, /*!< Compare true if the result is greater than or equal to value1. */
    kADC_HardwareCompareMode2 = 2U, /*!< Value1 <= Value2, compare true if the result is less than value1 Or
                                                          the result is Greater than value2.
                                         Value1 >  Value2, compare true if the result is less than value1 And the
                                                          result is greater than value2*/
    kADC_HardwareCompareMode3 = 3U, /*!< Value1 <= Value2, compare true if the result is greater than or equal
                                                          to value1 And the result is less than or equal to value2.
                                         Value1 >  Value2, compare true if the result is greater than or equal to
                                                          value1 Or the result is less than or equal to value2. */
} adc_hardware_compare_mode_t;

/*!
 * @brief Converter hardware average mode.
 */
typedef enum _adc_hardware_average_mode
{
    kADC_HardwareAverageCount4   = 0U, /*!< For hardware average with 4 samples. */
    kADC_HardwareAverageCount8   = 1U, /*!< For hardware average with 8 samples. */
    kADC_HardwareAverageCount16  = 2U, /*!< For hardware average with 16 samples. */
    kADC_HardwareAverageCount32  = 3U, /*!< For hardware average with 32 samples. */
    kADC_HardwareAverageDiasable = 4U, /*!< Disable the hardware average function. */
} adc_hardware_average_mode_t;

/*!
 * @brief Converter configuration.
 */
typedef struct _adc_config
{
    bool enableOverWrite;                                  /*!< Enable the overwriting. */
    bool enableContinuousConversion;                       /*!< Enable the continuous conversion mode. */
    bool enableHighSpeed;                                  /*!< Enable the high-speed mode. */
    bool enableLowPower;                                   /*!< Enable the low power mode. */
    bool enableLongSample;                                 /*!< Enable the long sample mode. */
    bool enableAsynchronousClockOutput;                    /*!< Enable the asynchronous clock output. */
    adc_reference_voltage_source_t referenceVoltageSource; /*!< Select the reference voltage source. */
    adc_sample_period_mode_t samplePeriodMode; /*!< Select the sample period in long sample mode or short mode. */
    adc_clock_source_t clockSource; /*!< Select the input clock source to generate the internal clock ADCK. */
    adc_clock_driver_t clockDriver; /*!< Select the divide ratio used by the ADC to generate the internal clock ADCK. */
    adc_resolution_t resolution;    /*!< Select the ADC resolution mode. */
} adc_config_t;

/*!
 * @brief Converter Offset configuration.
 */
typedef struct _adc_offest_config
{
    bool enableSigned;    /*!< if false,The offset value is added with the raw result.
                               if true,The offset value is subtracted from the raw converted value. */
    uint32_t offsetValue; /*!< User configurable offset value(0-4095). */
} adc_offest_config_t;

/*!
 * @brief ADC hardware compare configuration.
 *
 * In kADC_HardwareCompareMode0, compare true if the result is less than the value1.
 * In kADC_HardwareCompareMode1, compare true if the result is greater than or equal to value1.
 * In kADC_HardwareCompareMode2, Value1 <= Value2, compare true if the result is less than value1 Or the result is
 * Greater than value2.
 *                               Value1 >  Value2, compare true if the result is less than value1 And the result is
 * Greater than value2.
 * In kADC_HardwareCompareMode3, Value1 <= Value2, compare true if the result is greater than or equal to value1 And the
 * result is less than or equal to value2.
 *                               Value1 >  Value2, compare true if the result is greater than or equal to value1 Or the
 * result is less than or equal to value2.
 */
typedef struct _adc_hardware_compare_config
{
    adc_hardware_compare_mode_t hardwareCompareMode; /*!< Select the hardware compare mode.
                                                            See "adc_hardware_compare_mode_t". */
    uint16_t value1;                                 /*!< Setting value1(0-4095) for hardware compare mode. */
    uint16_t value2;                                 /*!< Setting value2(0-4095) for hardware compare mode. */
} adc_hardware_compare_config_t;

/*!
 * @brief ADC channel conversion configuration.
 */
typedef struct _adc_channel_config
{
    uint32_t channelNumber;                    /*!< Setting the conversion channel number. The available range is 0-31.
                                                    See channel connection information for each chip in Reference
                                                    Manual document. */
    bool enableInterruptOnConversionCompleted; /*!< Generate an interrupt request once the conversion is completed. */
} adc_channel_config_t;
/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Initialization
 * @{
 */

/*!
 * @brief Initialize the ADC module.
 *
 * @param base ADC peripheral base address.
 * @param config Pointer to "adc_config_t" structure.
 */
void ADC_Init(ADC_Type *base, const adc_config_t *config);

/*!
 * @brief De-initializes the ADC module.
 *
 * @param base ADC peripheral base address.
 */
void ADC_Deinit(ADC_Type *base);

/*!
 * @brief Gets an available pre-defined settings for the converter's configuration.
 *
 * This function initializes the converter configuration structure with available settings. The default values are:
 * @code
 *  config->enableAsynchronousClockOutput = true;
 *  config->enableOverWrite =               false;
 *  config->enableContinuousConversion =    false;
 *  config->enableHighSpeed =               false;
 *  config->enableLowPower =                false;
 *  config->enableLongSample =              false;
 *  config->referenceVoltageSource =        kADC_ReferenceVoltageSourceAlt0;
 *  config->samplePeriodMode =              kADC_SamplePeriod2or12Clocks;
 *  config->clockSource =                   kADC_ClockSourceAD;
 *  config->clockDriver =                   kADC_ClockDriver1;
 *  config->resolution =                    kADC_Resolution12Bit;
 * @endcode
 * @param base   ADC peripheral base address.
 * @param config Pointer to the configuration structure.
 */
void ADC_GetDefaultConfig(adc_config_t *config);

/*!
 * @brief Configures the conversion channel.
 *
 * This operation triggers the conversion when in software trigger mode. When in hardware trigger mode, this API
 * configures the channel while the external trigger source helps to trigger the conversion.
 *
 * Note that the "Channel Group" has a detailed description.
 * To allow sequential conversions of the ADC to be triggered by internal peripherals, the ADC has more than one
 * group of status and control registers, one for each conversion. The channel group parameter indicates which group of
 * registers are used, for example channel group 0 is for Group A registers and channel group 1 is for Group B
 * registers. The
 * channel groups are used in a "ping-pong" approach to control the ADC operation.  At any point, only one of
 * the channel groups is actively controlling ADC conversions. The channel group 0 is used for both software and
 * hardware
 * trigger modes. Channel groups 1 and greater indicate potentially multiple channel group registers for
 * use only in hardware trigger mode. See the chip configuration information in the appropriate MCU reference manual
 * about the
 * number of SC1n registers (channel groups) specific to this device.  None of the channel groups 1 or greater are used
 * for software trigger operation. Therefore, writing to these channel groups does not initiate a new conversion.
 * Updating the channel group 0 while a different channel group is actively controlling a conversion is allowed and
 * vice versa. Writing any of the channel group registers while that specific channel group is actively controlling a
 * conversion aborts the current conversion.
 *
 * @param base          ADC peripheral base address.
 * @param channelGroup  Channel group index.
 * @param config        Pointer to the "adc_channel_config_t" structure for the conversion channel.
 */
void ADC_SetChannelConfig(ADC_Type *base, uint32_t channelGroup, const adc_channel_config_t *config);

/*!
 * @brief  Gets the conversion value.
 *
 * @param  base         ADC peripheral base address.
 * @param  channelGroup Channel group index.
 *
 * @return              Conversion value.
 */
static inline uint32_t ADC_GetChannelConversionValue(ADC_Type *base, uint32_t channelGroup)
{
    assert(channelGroup < FSL_FEATURE_ADC_CONVERSION_CONTROL_COUNT);

    return base->R[channelGroup];
}

/*!
 * @brief Gets the status flags of channel.
 *
 * A conversion is completed when the result of the conversion is transferred into the data
 * result registers. (provided the compare function & hardware averaging is disabled), this is
 * indicated by the setting of COCOn. If hardware averaging is enabled, COCOn sets only,
 * if the last of the selected number of conversions is complete. If the compare function is
 * enabled, COCOn sets and conversion result data is transferred only if the compare
 * condition is true. If both hardware averaging and compare functions are enabled, then
 * COCOn sets only if the last of the selected number of conversions is complete and the
 * compare condition is true.
 *
 * @param base         ADC peripheral base address.
 * @param channelGroup Channel group index.
 *
 * @return             Status flags of channel.return 0 means COCO flag is 0,return 1 means COCOflag is 1.
 */
static inline uint32_t ADC_GetChannelStatusFlags(ADC_Type *base, uint32_t channelGroup)
{
    assert(channelGroup < FSL_FEATURE_ADC_CONVERSION_CONTROL_COUNT);

    /* If flag is set,return 1,otherwise, return 0. */
    return (((base->HS) & (1U << channelGroup)) >> channelGroup);
}

/*!
 * @brief  Automates the hardware calibration.
 *
 * This auto calibration helps to adjust the plus/minus side gain automatically.
 * Execute the calibration before using the converter. Note that the software trigger should be used
 * during calibration.
 *
 * @param  base ADC peripheral base address.
 *
 * @return                 Execution status.
 * @retval kStatus_Success Calibration is done successfully.
 * @retval kStatus_Fail    Calibration has failed.
 */
status_t ADC_DoAutoCalibration(ADC_Type *base);

/*!
 * @brief Set user defined offset.
 *
 * @param base   ADC peripheral base address.
 * @param config Pointer to "adc_offest_config_t" structure.
 */
void ADC_SetOffsetConfig(ADC_Type *base, const adc_offest_config_t *config);

/*!
 * @brief Enables generating the DMA trigger when the conversion is complete.
 *
 * @param base   ADC peripheral base address.
 * @param enable Switcher of the DMA feature. "true" means enabled, "false" means not enabled.
 */
static inline void ADC_EnableDMA(ADC_Type *base, bool enable)
{
    if (enable)
    {
        base->GC |= ADC_GC_DMAEN_MASK;
    }
    else
    {
        base->GC &= ~ADC_GC_DMAEN_MASK;
    }
}

/*!
 * @brief Enables the hardware trigger mode.
 *
 * @param base ADC peripheral base address.
 * @param enable Switcher of the trigger mode. "true" means hardware tirgger mode,"false" means software mode.
 */
#if !(defined(FSL_FEATURE_ADC_SUPPORT_HARDWARE_TRIGGER_REMOVE) && FSL_FEATURE_ADC_SUPPORT_HARDWARE_TRIGGER_REMOVE)
static inline void ADC_EnableHardwareTrigger(ADC_Type *base, bool enable)
{
    if (enable)
    {
        base->CFG |= ADC_CFG_ADTRG_MASK;
    }
    else
    {
        base->CFG &= ~ADC_CFG_ADTRG_MASK;
    }
}
#endif

/*!
 * @brief Configures the hardware compare mode.
 *
 * The hardware compare mode provides a way to process the conversion result automatically by using hardware. Only the
 * result
 * in the compare range is available. To compare the range, see "adc_hardware_compare_mode_t" or the appopriate
 * reference
 * manual for more information.
 *
 * @param base ADC peripheral base address.
 * @param Pointer to "adc_hardware_compare_config_t" structure.
 *
 */
void ADC_SetHardwareCompareConfig(ADC_Type *base, const adc_hardware_compare_config_t *config);

/*!
 * @brief Configures the hardware average mode.
 *
 * The hardware average mode provides a way to process the conversion result automatically by using hardware. The
 * multiple
 * conversion results are accumulated and averaged internally making them easier to read.
 *
 * @param base ADC peripheral base address.
 * @param mode Setting the hardware average mode. See "adc_hardware_average_mode_t".
 */
void ADC_SetHardwareAverageConfig(ADC_Type *base, adc_hardware_average_mode_t mode);

/*!
 * @brief Gets the converter's status flags.
 *
 * @param base ADC peripheral base address.
 *
 * @return Flags' mask if indicated flags are asserted. See "adc_status_flags_t".
 */
static inline uint32_t ADC_GetStatusFlags(ADC_Type *base)
{
    return base->GS;
}

/*!
 * @brief Clears the converter's status falgs.
 *
 * @param base ADC peripheral base address.
 * @param mask Mask value for the cleared flags. See "adc_status_flags_t".
 */
void ADC_ClearStatusFlags(ADC_Type *base, uint32_t mask);

#if defined(__cplusplus)
}
#endif

#endif /* _FSL_ADC_H_ */
