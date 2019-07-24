/*
 * Copyright 2017, NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __FSL_DCDC_H__
#define __FSL_DCDC_H__

#include "fsl_common.h"

/*!
 * @addtogroup dcdc
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief DCDC driver version. */
#define FSL_DCDC_DRIVER_VERSION (MAKE_VERSION(2, 0, 0)) /*!< Version 2.0.0. */
                                                        /*!
                                                         * @brief DCDC status flags.
                                                         */
enum _dcdc_status_flags_t
{
    kDCDC_LockedOKStatus = (1U << 0U), /*!< Indicate DCDC status. 1'b1: DCDC already settled 1'b0: DCDC is settling. */
};

/*!
 * @brief The current bias of low power comparator.
 */
typedef enum _dcdc_comparator_current_bias
{
    kDCDC_ComparatorCurrentBias50nA  = 0U, /*!< The current bias of low power comparator is 50nA. */
    kDCDC_ComparatorCurrentBias100nA = 1U, /*!< The current bias of low power comparator is 100nA. */
    kDCDC_ComparatorCurrentBias200nA = 2U, /*!< The current bias of low power comparator is 200nA. */
    kDCDC_ComparatorCurrentBias400nA = 3U, /*!< The current bias of low power comparator is 400nA. */
} dcdc_comparator_current_bias_t;

/*!
 * @brief The threshold of over current detection.
 */
typedef enum _dcdc_over_current_threshold
{
    kDCDC_OverCurrentThresholdAlt0 = 0U, /*!< 1A in the run mode, 0.25A in the power save mode. */
    kDCDC_OverCurrentThresholdAlt1 = 1U, /*!< 2A in the run mode, 0.25A in the power save mode. */
    kDCDC_OverCurrentThresholdAlt2 = 2U, /*!< 1A in the run mode, 0.2A in the power save mode. */
    kDCDC_OverCurrentThresholdAlt3 = 3U, /*!< 2A in the run mode, 0.2A in the power save mode. */
} dcdc_over_current_threshold_t;

/*!
 * @brief The threshold if peak current detection.
 */
typedef enum _dcdc_peak_current_threshold
{
    kDCDC_PeakCurrentThresholdAlt0 = 0U, /*!< 150mA peak current threshold. */
    kDCDC_PeakCurrentThresholdAlt1 = 1U, /*!< 250mA peak current threshold. */
    kDCDC_PeakCurrentThresholdAlt2 = 2U, /*!< 350mA peak current threshold. */
    kDCDC_PeakCurrentThresholdAlt3 = 3U, /*!< 450mA peak current threshold. */
    kDCDC_PeakCurrentThresholdAlt4 = 4U, /*!< 550mA peak current threshold. */
    kDCDC_PeakCurrentThresholdAlt5 = 5U, /*!< 650mA peak current threshold. */
} dcdc_peak_current_threshold_t;

/*!
 * @brief The period of counting the charging times in power save mode.
 */
typedef enum _dcdc_count_charging_time_period
{
    kDCDC_CountChargingTimePeriod8Cycle  = 0U, /*!< Eight 32k cycle. */
    kDCDC_CountChargingTimePeriod16Cycle = 1U, /*!< Sixteen 32k cycle. */
} dcdc_count_charging_time_period_t;

/*!
 * @brief The threshold of the counting number of charging times
 */
typedef enum _dcdc_count_charging_time_threshold
{
    kDCDC_CountChargingTimeThreshold32 = 0U, /*!< 0x0: 32. */
    kDCDC_CountChargingTimeThreshold64 = 1U, /*!< 0x1: 64. */
    kDCDC_CountChargingTimeThreshold16 = 2U, /*!< 0x2: 16. */
    kDCDC_CountChargingTimeThreshold8  = 3U, /*!< 0x3: 8. */
} dcdc_count_charging_time_threshold_t;

/*!
 * @brief Oscillator clock option.
 */
typedef enum _dcdc_clock_source
{
    kDCDC_ClockAutoSwitch  = 0U, /*!< Automatic clock switch from internal oscillator to external clock. */
    kDCDC_ClockInternalOsc = 1U, /*!< Use internal oscillator. */
    kDCDC_ClockExternalOsc = 2U, /*!< Use external 24M crystal oscillator. */
} dcdc_clock_source_t;

/*!
 * @brief Configuration for DCDC detection.
 */
typedef struct _dcdc_detection_config
{
    bool enableXtalokDetection;         /*!< Enable xtalok detection circuit. */
    bool powerDownOverVoltageDetection; /*!< Power down over-voltage detection comparator. */
    bool powerDownLowVlotageDetection;  /*!< Power down low-voltage detection comparator. */
    bool powerDownOverCurrentDetection; /*!< Power down over-current detection. */
    bool powerDownPeakCurrentDetection; /*!< Power down peak-current detection. */
    bool powerDownZeroCrossDetection;   /*!< Power down the zero cross detection function for discontinuous conductor
                                           mode. */
    dcdc_over_current_threshold_t OverCurrentThreshold; /*!< The threshold of over current detection. */
    dcdc_peak_current_threshold_t PeakCurrentThreshold; /*!< The threshold of peak current detection. */
} dcdc_detection_config_t;

/*!
 * @brief Configuration for the loop control.
 */
typedef struct _dcdc_loop_control_config
{
    bool enableCommonHysteresis;         /*!< Enable hysteresis in switching converter common mode analog comparators.
                                              This feature will improve transient supply ripple and efficiency. */
    bool enableCommonThresholdDetection; /*!< Increase the threshold detection for common mode analog comparator. */
    bool enableInvertHysteresisSign;     /*!< Invert the sign of the hysteresis in DC-DC analog comparators. */
    bool enableRCThresholdDetection;     /*!< Increase the threshold detection for RC scale circuit. */
    uint32_t enableRCScaleCircuit;      /*!< Available range is 0~7. Enable analog circuit of DC-DC converter to respond
                                             faster under transient load conditions. */
    uint32_t complementFeedForwardStep; /*!< Available range is 0~7. Two's complement feed forward step in duty cycle in
                                           the switching DC-DC converter. Each time this field makes a transition from
                                           0x0, the loop filter of the DC-DC converter is stepped once by a value
                                           proportional to the change. This can be used to force a certain control loop
                                           behavior, such as improving response under known heavy load transients. */
    uint32_t controlParameterMagnitude; /*!< Available range is 0~15. Magnitude of proportional control parameter in the
                                           switching DC-DC converter control loop. */
    uint32_t integralProportionalRatio; /*!< Available range is 0~3.Ratio of integral control parameter to proportional
                                           control parameter in the switching DC-DC converter, and can be used to
                                           optimize efficiency and loop response. */
} dcdc_loop_control_config_t;
/*!
 * @brief Configuration for DCDC low power.
 */
typedef struct _dcdc_low_power_config
{
    bool enableOverloadDetection; /*!< Enable the overload detection in power save mode, if current is larger than the
                                     overloading threshold (typical value is 50 mA), DCDC will switch to the run mode
                                     automatically. */
    bool enableAdjustHystereticValue; /*!< Adjust hysteretic value in low power from 12.5mV to 25mV. */
    dcdc_count_charging_time_period_t
        countChargingTimePeriod; /*!< The period of counting the charging times in power save mode. */
    dcdc_count_charging_time_threshold_t
        countChargingTimeThreshold; /*!< the threshold of the counting number of charging times during
                                         the period that lp_overload_freq_sel sets in power save mode. */
} dcdc_low_power_config_t;

/*!
 * @brief Configuration for DCDC internal regulator.
 */
typedef struct _dcdc_internal_regulator_config
{
    bool enableLoadResistor; /*!< control the load resistor of the internal regulator of DCDC, the load resistor is
                                connected as default "true", and need set to "false" to disconnect the load
                                resistor. */
    uint32_t feedbackPoint;  /*!< Available range is 0~3. Select the feedback point of the internal regulator. */
} dcdc_internal_regulator_config_t;

/*!
 * @brief Configuration for min power setting.
 */
typedef struct _dcdc_min_power_config
{
    bool enableUseHalfFreqForContinuous; /*!< Set DCDC clock to half frequency for the continuous mode. */
} dcdc_min_power_config_t;
#if defined(__cplusplus)
extern "C" {
#endif

/*******************************************************************************
 * API
 ******************************************************************************/
/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief Enable the access to DCDC registers.
 *
 * @param base DCDC peripheral base address.
 */
void DCDC_Init(DCDC_Type *base);

/*!
 * @brief Disable the access to DCDC registers.
 *
 * @param base DCDC peripheral base address.
 */
void DCDC_Deinit(DCDC_Type *base);

/* @} */

/*!
 * @name Status
 * @{
 */

/*!
 * @brief Get DCDC status flags.
 *
 * @param base peripheral base address.
 * @return Mask of asserted status flags. See to "_dcdc_status_flags_t".
 */
uint32_t DCDC_GetstatusFlags(DCDC_Type *base);

/* @} */

/*!
 * @name Misc control.
 * @{
 */

/*!
 * @brief Enable the output range comparator.
 *
 * The output range comparator is disabled by default.
 *
 * @param base DCDC peripheral base address.
 * @param enable Enable the feature or not.
 */
static inline void DCDC_EnableOutputRangeComparator(DCDC_Type *base, bool enable)
{
    if (enable)
    {
        base->REG0 &= ~DCDC_REG0_PWD_CMP_OFFSET_MASK;
    }
    else
    {
        base->REG0 |= DCDC_REG0_PWD_CMP_OFFSET_MASK;
    }
}

/*!
 * @brief Configure the DCDC clock source.
 *
 * @param base DCDC peripheral base address.
 * @param clockSource Clock source for DCDC. See to "dcdc_clock_source_t".
 */
void DCDC_SetClockSource(DCDC_Type *base, dcdc_clock_source_t clockSource);

/*!
 * @brief Get the default setting for detection configuration.
 *
 * The default configuration are set according to responding registers' setting when powered on.
 * They are:
 * @code
 *   config->enableXtalokDetection = false;
 *   config->powerDownOverVoltageDetection = true;
 *   config->powerDownLowVlotageDetection = false;
 *   config->powerDownOverCurrentDetection = true;
 *   config->powerDownPeakCurrentDetection = true;
 *   config->powerDownZeroCrossDetection = true;
 *   config->OverCurrentThreshold = kDCDC_OverCurrentThresholdAlt0;
 *   config->PeakCurrentThreshold = kDCDC_PeakCurrentThresholdAlt0;
 * @endcode
 *
 * @param config Pointer to configuration structure. See to "dcdc_detection_config_t"
 */
void DCDC_GetDefaultDetectionConfig(dcdc_detection_config_t *config);

/*!
 * @breif Configure the DCDC detection.
 *
 * @param base DCDC peripheral base address.
 * @param config Pointer to configuration structure. See to "dcdc_detection_config_t"
 */
void DCDC_SetDetectionConfig(DCDC_Type *base, const dcdc_detection_config_t *config);

/*!
 * @brief Get the default setting for low power configuration.
 *
 * The default configuration are set according to responding registers' setting when powered on.
 * They are:
 * @code
 *   config->enableOverloadDetection = true;
 *   config->enableAdjustHystereticValue = false;
 *   config->countChargingTimePeriod = kDCDC_CountChargingTimePeriod8Cycle;
 *   config->countChargingTimeThreshold = kDCDC_CountChargingTimeThreshold32;
 * @endcode
 *
 * @param config Pointer to configuration structure. See to "dcdc_low_power_config_t"
 */
void DCDC_GetDefaultLowPowerConfig(dcdc_low_power_config_t *config);

/*!
 * @brief Configure the DCDC low power.
 *
 * @param base DCDC peripheral base address.
 * @param config Pointer to configuration structure. See to "dcdc_low_power_config_t".
 */
void DCDC_SetLowPowerConfig(DCDC_Type *base, const dcdc_low_power_config_t *config);

/*!
 * @brief Reset current alert signal. Alert signal is generate by peak current detection.
 *
 * @param base DCDC peripheral base address.
 * @param enable Switcher to reset signal. True means reset signal. False means don't reset signal.
 */
void DCDC_ResetCurrentAlertSignal(DCDC_Type *base, bool enable);

/*!
 * @brief Set the bangap trim value to trim bandgap voltage.
 *
 * @param base DCDC peripheral base address.
 * @param TrimValue The bangap trim value. Available range is 0U-31U.
 */
static inline void DCDC_SetBandgapVoltageTrimValue(DCDC_Type *base, uint32_t trimValue)
{
    base->REG1 &= ~DCDC_REG1_VBG_TRIM_MASK;
    base->REG1 |= DCDC_REG1_VBG_TRIM(trimValue);
}

/*!
 * @brief Get the default setting for loop control configuration.
 *
 * The default configuration are set according to responding registers' setting when powered on.
 * They are:
 * @code
 *   config->enableCommonHysteresis = false;
 *   config->enableCommonThresholdDetection = false;
 *   config->enableInvertHysteresisSign = false;
 *   config->enableRCThresholdDetection = false;
 *   config->enableRCScaleCircuit = 0U;
 *   config->complementFeedForwardStep = 0U;
 *   config->controlParameterMagnitude = 2U;
 *   config->integralProportionalRatio = 2U;
 * @endcode
 *
 * @param config Pointer to configuration structure. See to "dcdc_loop_control_config_t"
 */
void DCDC_GetDefaultLoopControlConfig(dcdc_loop_control_config_t *config);

/*!
 * @brief Configure the DCDC loop control.
 *
 * @param base DCDC peripheral base address.
 * @param config Pointer to configuration structure. See to "dcdc_loop_control_config_t".
 */
void DCDC_SetLoopControlConfig(DCDC_Type *base, const dcdc_loop_control_config_t *config);

/*!
 * @brief Configure for the min power.
 *
 * @param base DCDC peripheral base address.
 * @param config Pointer to configuration structure. See to "dcdc_min_power_config_t".
 */
void DCDC_SetMinPowerConfig(DCDC_Type *base, const dcdc_min_power_config_t *config);

/*!
 * @brief Set the current bias of low power comparator.
 *
 * @param base DCDC peripheral base address.
 * @param biasVaule The current bias of low power comparator. Refer to "dcdc_comparator_current_bias_t".
 */
static inline void DCDC_SetLPComparatorBiasValue(DCDC_Type *base, dcdc_comparator_current_bias_t biasVaule)
{
    base->REG1 &= ~DCDC_REG1_LP_CMP_ISRC_SEL_MASK;
    base->REG1 |= DCDC_REG1_LP_CMP_ISRC_SEL(biasVaule);
}

static inline void DCDC_LockTargetVoltage(DCDC_Type *base)
{
    base->REG3 |= DCDC_REG3_DISABLE_STEP_MASK;
}

/*!
 * @brief Adjust the target voltage of VDD_SOC in run mode and low power mode.
 *
 * This function is to adjust the target voltage of DCDC output. Change them and finally wait until the output is
 * stabled.
 * Set the target value of run mode the same as low power mode before entering power save mode, because DCDC will switch
 * back to run mode if it detects the current loading is larger than about 50 mA(typical value).
 *
 * @param base DCDC peripheral base address.
 * @param VDDRun Target value in run mode. 25 mV each step from 0x00 to 0x1F. 00 is for 0.8V, 0x1F is for 1.575V.
 * @param VDDStandby Target value in low power mode. 25 mV each step from 0x00 to 0x4. 00 is for 0.9V, 0x4 is for 1.0V.
 */
void DCDC_AdjustTargetVoltage(DCDC_Type *base, uint32_t VDDRun, uint32_t VDDStandby);

/*!
 * @brief Configure the DCDC internal regulator.
 *
 * @param base DCDC peripheral base address.
 * @param config Pointer to configuration structure. See to "dcdc_internal_regulator_config_t".
 */
void DCDC_SetInternalRegulatorConfig(DCDC_Type *base, const dcdc_internal_regulator_config_t *config);

/*!
 * @brief Ajust delay to reduce ground noise.
 *
 * @param base DCDC peripheral base address.
 * @param enable Enable the feature or not.
 */
static inline void DCDC_EnableAdjustDelay(DCDC_Type *base, bool enable)
{
    if (enable)
    {
        base->REG3 |= DCDC_REG3_MISC_DELAY_TIMING_MASK;
    }
    else
    {
        base->REG3 &= ~DCDC_REG3_MISC_DELAY_TIMING_MASK;
    }
}

/*!
 * @brief Enable/Disable to improve the transition from heavy load to light load. It is valid while zero
 *        cross detection is enabled. If ouput exceeds the threshold, DCDC would return CCM from DCM.
 *
 * @param base DCDC peripheral base address.
 * @param enable Enable the feature or not.
 */
static inline void DCDC_EnableImproveTransition(DCDC_Type *base, bool enable)
{
    if (enable)
    {
        base->REG2 |= DCDC_REG2_DCM_SET_CTRL_MASK;
    }
    else
    {
        base->REG2 &= ~DCDC_REG2_DCM_SET_CTRL_MASK;
    }
}

/* @} */

/*!
 * @name Application guideline.
 * @{
 */

/*!
 * @brief Boot DCDC into DCM(discontinous conduction mode).
 *
 *  pwd_zcd=0x0;
 *  pwd_cmp_offset=0x0;
 *  dcdc_loopctrl_en_rcscale=0x3 or 0x5;
 *  DCM_set_ctrl=1'b1;
 *
 * @param base DCDC peripheral base address.
 */
void DCDC_BootIntoDCM(DCDC_Type *base);

/*!
 * @brief Boot DCDC into CCM(continous conduction mode).
 *
 *  pwd_zcd=0x1;
 *  pwd_cmp_offset=0x0;
 *  dcdc_loopctrl_en_rcscale=0x3;
 *
 * @param base DCDC peripheral base address.
 */
void DCDC_BootIntoCCM(DCDC_Type *base);

#if defined(__cplusplus)
}
#endif

#endif /* __FSL_DCDC_H__ */
