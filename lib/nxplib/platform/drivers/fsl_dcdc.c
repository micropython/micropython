/*
 * Copyright 2017, NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_dcdc.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.dcdc_1"
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Get instance number for DCDC module.
 *
 * @param base DCDC peripheral base address
 */
static uint32_t DCDC_GetInstance(DCDC_Type *base);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief Pointers to DCDC bases for each instance. */
static DCDC_Type *const s_dcdcBases[] = DCDC_BASE_PTRS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief Pointers to DCDC clocks for each instance. */
static const clock_ip_name_t s_dcdcClocks[] = DCDC_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*******************************************************************************
 * Code
 ******************************************************************************/
static uint32_t DCDC_GetInstance(DCDC_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_dcdcBases); instance++)
    {
        if (s_dcdcBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_dcdcBases));

    return instance;
}

/*!
 * brief Enable the access to DCDC registers.
 *
 * param base DCDC peripheral base address.
 */
void DCDC_Init(DCDC_Type *base)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Enable the clock. */
    CLOCK_EnableClock(s_dcdcClocks[DCDC_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief Disable the access to DCDC registers.
 *
 * param base DCDC peripheral base address.
 */
void DCDC_Deinit(DCDC_Type *base)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Disable the clock. */
    CLOCK_DisableClock(s_dcdcClocks[DCDC_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief Configure the DCDC clock source.
 *
 * param base DCDC peripheral base address.
 * param clockSource Clock source for DCDC. See to "dcdc_clock_source_t".
 */
void DCDC_SetClockSource(DCDC_Type *base, dcdc_clock_source_t clockSource)
{
    uint32_t tmp32;

    /* Configure the DCDC_REG0 register. */
    tmp32 = base->REG0 & ~(DCDC_REG0_XTAL_24M_OK_MASK | DCDC_REG0_DISABLE_AUTO_CLK_SWITCH_MASK |
                           DCDC_REG0_SEL_CLK_MASK | DCDC_REG0_PWD_OSC_INT_MASK);
    switch (clockSource)
    {
        case kDCDC_ClockInternalOsc:
            tmp32 |= DCDC_REG0_DISABLE_AUTO_CLK_SWITCH_MASK;
            break;
        case kDCDC_ClockExternalOsc:
            /* Choose the external clock and disable the internal clock. */
            tmp32 |= DCDC_REG0_DISABLE_AUTO_CLK_SWITCH_MASK | DCDC_REG0_SEL_CLK_MASK | DCDC_REG0_PWD_OSC_INT_MASK;
            break;
        case kDCDC_ClockAutoSwitch:
            /* Set to switch from internal ring osc to xtal 24M if auto mode is enabled. */
            tmp32 |= DCDC_REG0_XTAL_24M_OK_MASK;
            break;
        default:
            break;
    }
    base->REG0 = tmp32;
}

/*!
 * brief Get the default setting for detection configuration.
 *
 * The default configuration are set according to responding registers' setting when powered on.
 * They are:
 * code
 *   config->enableXtalokDetection = false;
 *   config->powerDownOverVoltageDetection = true;
 *   config->powerDownLowVlotageDetection = false;
 *   config->powerDownOverCurrentDetection = true;
 *   config->powerDownPeakCurrentDetection = true;
 *   config->powerDownZeroCrossDetection = true;
 *   config->OverCurrentThreshold = kDCDC_OverCurrentThresholdAlt0;
 *   config->PeakCurrentThreshold = kDCDC_PeakCurrentThresholdAlt0;
 * endcode
 *
 * param config Pointer to configuration structure. See to "dcdc_detection_config_t"
 */
void DCDC_GetDefaultDetectionConfig(dcdc_detection_config_t *config)
{
    assert(NULL != config);

    /* Initializes the configure structure to zero. */
    memset(config, 0, sizeof(*config));

    config->enableXtalokDetection         = false;
    config->powerDownOverVoltageDetection = true;
    config->powerDownLowVlotageDetection  = false;
    config->powerDownOverCurrentDetection = true;
    config->powerDownPeakCurrentDetection = true;
    config->powerDownZeroCrossDetection   = true;
    config->OverCurrentThreshold          = kDCDC_OverCurrentThresholdAlt0;
    config->PeakCurrentThreshold          = kDCDC_PeakCurrentThresholdAlt0;
}

/*!
 * breif Configure the DCDC detection.
 *
 * param base DCDC peripheral base address.
 * param config Pointer to configuration structure. See to "dcdc_detection_config_t"
 */
void DCDC_SetDetectionConfig(DCDC_Type *base, const dcdc_detection_config_t *config)
{
    assert(NULL != config);

    uint32_t tmp32;
    /* Configure the DCDC_REG0 register. */
    tmp32 = base->REG0 &
            ~(DCDC_REG0_XTALOK_DISABLE_MASK | DCDC_REG0_PWD_HIGH_VOLT_DET_MASK | DCDC_REG0_PWD_CMP_BATT_DET_MASK |
              DCDC_REG0_PWD_OVERCUR_DET_MASK | DCDC_REG0_PWD_CUR_SNS_CMP_MASK | DCDC_REG0_PWD_ZCD_MASK |
              DCDC_REG0_CUR_SNS_THRSH_MASK | DCDC_REG0_OVERCUR_TRIG_ADJ_MASK);

    tmp32 |= DCDC_REG0_CUR_SNS_THRSH(config->PeakCurrentThreshold) |
             DCDC_REG0_OVERCUR_TRIG_ADJ(config->OverCurrentThreshold);
    if (false == config->enableXtalokDetection)
    {
        tmp32 |= DCDC_REG0_XTALOK_DISABLE_MASK;
    }
    if (config->powerDownOverVoltageDetection)
    {
        tmp32 |= DCDC_REG0_PWD_HIGH_VOLT_DET_MASK;
    }
    if (config->powerDownLowVlotageDetection)
    {
        tmp32 |= DCDC_REG0_PWD_CMP_BATT_DET_MASK;
    }
    if (config->powerDownOverCurrentDetection)
    {
        tmp32 |= DCDC_REG0_PWD_OVERCUR_DET_MASK;
    }
    if (config->powerDownPeakCurrentDetection)
    {
        tmp32 |= DCDC_REG0_PWD_CUR_SNS_CMP_MASK;
    }
    if (config->powerDownZeroCrossDetection)
    {
        tmp32 |= DCDC_REG0_PWD_ZCD_MASK;
    }
    base->REG0 = tmp32;
}

/*!
 * brief Get the default setting for low power configuration.
 *
 * The default configuration are set according to responding registers' setting when powered on.
 * They are:
 * code
 *   config->enableOverloadDetection = true;
 *   config->enableAdjustHystereticValue = false;
 *   config->countChargingTimePeriod = kDCDC_CountChargingTimePeriod8Cycle;
 *   config->countChargingTimeThreshold = kDCDC_CountChargingTimeThreshold32;
 * endcode
 *
 * param config Pointer to configuration structure. See to "dcdc_low_power_config_t"
 */
void DCDC_GetDefaultLowPowerConfig(dcdc_low_power_config_t *config)
{
    assert(NULL != config);

    /* Initializes the configure structure to zero. */
    memset(config, 0, sizeof(*config));

    config->enableOverloadDetection     = true;
    config->enableAdjustHystereticValue = false;
    config->countChargingTimePeriod     = kDCDC_CountChargingTimePeriod8Cycle;
    config->countChargingTimeThreshold  = kDCDC_CountChargingTimeThreshold32;
}

/*!
 * brief Configure the DCDC low power.
 *
 * param base DCDC peripheral base address.
 * param config Pointer to configuration structure. See to "dcdc_low_power_config_t".
 */
void DCDC_SetLowPowerConfig(DCDC_Type *base, const dcdc_low_power_config_t *config)
{
    assert(NULL != config);

    uint32_t tmp32;
    /* Configure the DCDC_REG0 register. */
    tmp32 = base->REG0 & ~(DCDC_REG0_EN_LP_OVERLOAD_SNS_MASK | DCDC_REG0_LP_HIGH_HYS_MASK |
                           DCDC_REG0_LP_OVERLOAD_FREQ_SEL_MASK | DCDC_REG0_LP_OVERLOAD_THRSH_MASK);
    tmp32 |= DCDC_REG0_LP_OVERLOAD_FREQ_SEL(config->countChargingTimePeriod) |
             DCDC_REG0_LP_OVERLOAD_THRSH(config->countChargingTimeThreshold);
    if (config->enableOverloadDetection)
    {
        tmp32 |= DCDC_REG0_EN_LP_OVERLOAD_SNS_MASK;
    }
    if (config->enableAdjustHystereticValue)
    {
        tmp32 |= DCDC_REG0_LP_HIGH_HYS_MASK;
    }
    base->REG0 = tmp32;
}

/*!
 * brief Get DCDC status flags.
 *
 * param base peripheral base address.
 * return Mask of asserted status flags. See to "_dcdc_status_flags_t".
 */
uint32_t DCDC_GetstatusFlags(DCDC_Type *base)
{
    uint32_t tmp32 = 0U;

    if (DCDC_REG0_STS_DC_OK_MASK == (DCDC_REG0_STS_DC_OK_MASK & base->REG0))
    {
        tmp32 |= kDCDC_LockedOKStatus;
    }

    return tmp32;
}

/*!
 * brief Reset current alert signal. Alert signal is generate by peak current detection.
 *
 * param base DCDC peripheral base address.
 * param enable Switcher to reset signal. True means reset signal. False means don't reset signal.
 */
void DCDC_ResetCurrentAlertSignal(DCDC_Type *base, bool enable)
{
    if (enable)
    {
        base->REG0 |= DCDC_REG0_CURRENT_ALERT_RESET_MASK;
    }
    else
    {
        base->REG0 &= ~DCDC_REG0_CURRENT_ALERT_RESET_MASK;
    }
}

/*!
 * brief Get the default setting for loop control configuration.
 *
 * The default configuration are set according to responding registers' setting when powered on.
 * They are:
 * code
 *   config->enableCommonHysteresis = false;
 *   config->enableCommonThresholdDetection = false;
 *   config->enableInvertHysteresisSign = false;
 *   config->enableRCThresholdDetection = false;
 *   config->enableRCScaleCircuit = 0U;
 *   config->complementFeedForwardStep = 0U;
 *   config->controlParameterMagnitude = 2U;
 *   config->integralProportionalRatio = 2U;
 * endcode
 *
 * param config Pointer to configuration structure. See to "dcdc_loop_control_config_t"
 */
void DCDC_GetDefaultLoopControlConfig(dcdc_loop_control_config_t *config)
{
    assert(NULL != config);

    /* Initializes the configure structure to zero. */
    memset(config, 0, sizeof(*config));

    config->enableCommonHysteresis         = false;
    config->enableCommonThresholdDetection = false;
    config->enableInvertHysteresisSign     = false;
    config->enableRCThresholdDetection     = false;
    config->enableRCScaleCircuit           = 0U;
    config->complementFeedForwardStep      = 0U;
    config->controlParameterMagnitude      = 2U;
    config->integralProportionalRatio      = 2U;
}

/*!
 * brief Configure the DCDC loop control.
 *
 * param base DCDC peripheral base address.
 * param config Pointer to configuration structure. See to "dcdc_loop_control_config_t".
 */
void DCDC_SetLoopControlConfig(DCDC_Type *base, const dcdc_loop_control_config_t *config)
{
    assert(NULL != config);

    uint32_t tmp32;

    /* Configure the DCDC_REG1 register. */
    tmp32 = base->REG1 & ~(DCDC_REG1_LOOPCTRL_EN_HYST_MASK | DCDC_REG1_LOOPCTRL_HST_THRESH_MASK);
    if (config->enableCommonHysteresis)
    {
        tmp32 |= DCDC_REG1_LOOPCTRL_EN_HYST_MASK;
    }
    if (config->enableCommonThresholdDetection)
    {
        tmp32 |= DCDC_REG1_LOOPCTRL_HST_THRESH_MASK;
    }
    base->REG1 = tmp32;

    /* configure the DCDC_REG2 register. */
    tmp32 = base->REG2 & ~(DCDC_REG2_LOOPCTRL_HYST_SIGN_MASK | DCDC_REG2_LOOPCTRL_RCSCALE_THRSH_MASK |
                           DCDC_REG2_LOOPCTRL_EN_RCSCALE_MASK | DCDC_REG2_LOOPCTRL_DC_FF_MASK |
                           DCDC_REG2_LOOPCTRL_DC_R_MASK | DCDC_REG2_LOOPCTRL_DC_C_MASK);
    tmp32 |= DCDC_REG2_LOOPCTRL_DC_FF(config->complementFeedForwardStep) |
             DCDC_REG2_LOOPCTRL_DC_R(config->controlParameterMagnitude) |
             DCDC_REG2_LOOPCTRL_DC_C(config->integralProportionalRatio) |
             DCDC_REG2_LOOPCTRL_EN_RCSCALE(config->enableRCScaleCircuit);
    if (config->enableInvertHysteresisSign)
    {
        tmp32 |= DCDC_REG2_LOOPCTRL_HYST_SIGN_MASK;
    }
    if (config->enableRCThresholdDetection)
    {
        tmp32 |= DCDC_REG2_LOOPCTRL_RCSCALE_THRSH_MASK;
    }
    base->REG2 = tmp32;
}

/*!
 * brief Configure for the min power.
 *
 * param base DCDC peripheral base address.
 * param config Pointer to configuration structure. See to "dcdc_min_power_config_t".
 */
void DCDC_SetMinPowerConfig(DCDC_Type *base, const dcdc_min_power_config_t *config)
{
    assert(NULL != config);

    uint32_t tmp32;

    tmp32 = base->REG3 & ~DCDC_REG3_MINPWR_DC_HALFCLK_MASK;
    if (config->enableUseHalfFreqForContinuous)
    {
        tmp32 |= DCDC_REG3_MINPWR_DC_HALFCLK_MASK;
    }
    base->REG3 = tmp32;
}

/*!
 * brief Adjust the target voltage of VDD_SOC in run mode and low power mode.
 *
 * This function is to adjust the target voltage of DCDC output. Change them and finally wait until the output is
 * stabled.
 * Set the target value of run mode the same as low power mode before entering power save mode, because DCDC will switch
 * back to run mode if it detects the current loading is larger than about 50 mA(typical value).
 *
 * param base DCDC peripheral base address.
 * param VDDRun Target value in run mode. 25 mV each step from 0x00 to 0x1F. 00 is for 0.8V, 0x1F is for 1.575V.
 * param VDDStandby Target value in low power mode. 25 mV each step from 0x00 to 0x4. 00 is for 0.9V, 0x4 is for 1.0V.
 */
void DCDC_AdjustTargetVoltage(DCDC_Type *base, uint32_t VDDRun, uint32_t VDDStandby)
{
    uint32_t tmp32;

    /* Unlock the step for the output. */
    base->REG3 &= ~DCDC_REG3_DISABLE_STEP_MASK;

    /* Configure the DCDC_REG3 register. */
    tmp32 = base->REG3 & ~(DCDC_REG3_TARGET_LP_MASK | DCDC_REG3_TRG_MASK);

    tmp32 |= DCDC_REG3_TARGET_LP(VDDStandby) | DCDC_REG3_TRG(VDDRun);
    base->REG3 = tmp32;

    /* DCDC_STS_DC_OK bit will be de-asserted after target register changes. After output voltage settling to new
     * target value, DCDC_STS_DC_OK will be asserted. */
    while (DCDC_REG0_STS_DC_OK_MASK != (DCDC_REG0_STS_DC_OK_MASK & base->REG0))
    {
    }
}

/*!
 * brief Configure the DCDC internal regulator.
 *
 * param base DCDC peripheral base address.
 * param config Pointer to configuration structure. See to "dcdc_internal_regulator_config_t".
 */
void DCDC_SetInternalRegulatorConfig(DCDC_Type *base, const dcdc_internal_regulator_config_t *config)
{
    assert(NULL != config);

    uint32_t tmp32;

    /* Configure the DCDC_REG1 register. */
    tmp32 = base->REG1 & ~(DCDC_REG1_REG_FBK_SEL_MASK | DCDC_REG1_REG_RLOAD_SW_MASK);
    tmp32 |= DCDC_REG1_REG_FBK_SEL(config->feedbackPoint);
    if (config->enableLoadResistor)
    {
        tmp32 |= DCDC_REG1_REG_RLOAD_SW_MASK;
    }
    base->REG1 = tmp32;
}

/*!
 * brief Boot DCDC into DCM(discontinous conduction mode).
 *
 *  pwd_zcd=0x0;
 *  pwd_cmp_offset=0x0;
 *  dcdc_loopctrl_en_rcscale=0x3 or 0x5;
 *  DCM_set_ctrl=1'b1;
 *
 * param base DCDC peripheral base address.
 */
void DCDC_BootIntoDCM(DCDC_Type *base)
{
    base->REG0 &= ~(DCDC_REG0_PWD_ZCD_MASK | DCDC_REG0_PWD_CMP_OFFSET_MASK);
    base->REG2 = (~DCDC_REG2_LOOPCTRL_EN_RCSCALE_MASK & base->REG2) | DCDC_REG2_LOOPCTRL_EN_RCSCALE(0x4U) |
                 DCDC_REG2_DCM_SET_CTRL_MASK;
}

/*!
 * brief Boot DCDC into CCM(continous conduction mode).
 *
 *  pwd_zcd=0x1;
 *  pwd_cmp_offset=0x0;
 *  dcdc_loopctrl_en_rcscale=0x3;
 *
 * param base DCDC peripheral base address.
 */
void DCDC_BootIntoCCM(DCDC_Type *base)
{
    base->REG0 = (~DCDC_REG0_PWD_CMP_OFFSET_MASK & base->REG0) | DCDC_REG0_PWD_ZCD_MASK;
    base->REG2 = (~DCDC_REG2_LOOPCTRL_EN_RCSCALE_MASK & base->REG2) | DCDC_REG2_LOOPCTRL_EN_RCSCALE(0x3U);
}
