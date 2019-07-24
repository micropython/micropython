/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_PMU_H_
#define _FSL_PMU_H_

#include "fsl_common.h"

/*! @addtogroup pmu */
/*! @{ */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief PMU driver version */
#define FSL_PMU_DRIVER_VERSION (MAKE_VERSION(2, 0, 0)) /*!< Version 2.0.0. */
                                                       /*@}*/

/*!
 * @brief Status flags.
 */
enum _pmu_status_flags
{
    kPMU_1P1RegulatorOutputOK = (1U << 0U), /*!< Status bit that signals when the 1p1 regulator output
                                                        is ok. 1 = regulator output > brownout target. */
    kPMU_1P1BrownoutOnOutput = (1U << 1U),  /*!< Status bit that signals when a 1p1 brownout is detected
                                                        on the regulator output. */
    kPMU_3P0RegulatorOutputOK = (1U << 2U), /*!< Status bit that signals when the 3p0 regulator output
                                                        is ok. 1 = regulator output > brownout target. */
    kPMU_3P0BrownoutOnOutput = (1U << 3U),  /*!< Status bit that signals when a 3p0 brownout is detected
                                                        on the regulator output. */
    kPMU_2P5RegulatorOutputOK = (1U << 4U), /*!< Status bit that signals when the 2p5 regulator output
                                                        is ok. 1 = regulator output > brownout target. */
    kPMU_2P5BrownoutOnOutput = (1U << 5U),  /*!< Status bit that signals when a 2p5 brownout is detected
                                                        on the regulator output. */
};

/*!
 * @brief The source for the reference voltage of the weak 1P1 regulator.
 */
typedef enum _pmu_1p1_weak_reference_source
{
    kPMU_1P1WeakReferenceSourceAlt0 = 0U, /*!< Weak-linreg output tracks low-power-bandgap voltage. */
    kPMU_1P1WeakReferenceSourceAlt1 = 1U, /*!< Weak-linreg output tracks VDD_SOC_CAP voltage. */
} pmu_1p1_weak_reference_source_t;

/*!
 * @brief Input voltage source for LDO_3P0 from USB VBus.
 */
typedef enum _pmu_3p0_vbus_voltage_source
{
    kPMU_3P0VBusVoltageSourceAlt0 = 0U, /*!< USB_OTG1_VBUS - Utilize VBUS OTG1 for power. */
    kPMU_3P0VBusVoltageSourceAlt1 = 1U, /*!< USB_OTG2_VBUS - Utilize VBUS OTG2 for power. */
} pmu_3p0_vbus_voltage_source_t;

/*!
 * @brief Regulator voltage ramp rate.
 */
typedef enum _pmu_core_reg_voltage_ramp_rate
{
    kPMU_CoreRegVoltageRampRateFast       = 0U, /*!< Fast. */
    kPMU_CoreRegVoltageRampRateMediumFast = 1U, /*!< Medium Fast. */
    kPMU_CoreRegVoltageRampRateMediumSlow = 2U, /*!< Medium Slow. */
    kPMU_CoreRegVoltageRampRateSlow       = 0U, /*!< Slow. */
} pmu_core_reg_voltage_ramp_rate_t;

#if defined(FSL_FEATURE_PMU_HAS_LOWPWR_CTRL) && FSL_FEATURE_PMU_HAS_LOWPWR_CTRL
/*!
 * @brief Mask values of power gate.
 */
enum _pmu_power_gate
{
    kPMU_PowerGateDisplay      = PMU_LOWPWR_CTRL_MIX_PWRGATE_MASK,      /*!< Display power gate control. */
    kPMU_PowerGateDisplayLogic = PMU_LOWPWR_CTRL_DISPLAY_PWRGATE_MASK,  /*!< Display logic power gate control. */
    kPMU_PowerGateL2           = PMU_LOWPWR_CTRL_L2_PWRGATE_MASK,       /*!< L2 power gate control. */
    kPMU_PowerGateL1           = PMU_LOWPWR_CTRL_L1_PWRGATE_MASK,       /*!< L1 power gate control. */
    kPMU_PowerGateRefTopIBias  = PMU_LOWPWR_CTRL_REFTOP_IBIAS_OFF_MASK, /*!< Low power reftop ibias disable. */
};
#endif /*  FSL_FEATURE_PMU_HAS_LOWPWR_CTRL. */

/*!
 * @brief Bandgap select.
 */
typedef enum _pmu_power_bandgap
{
    kPMU_NormalPowerBandgap = 0U, /*!< Normal power bandgap. */
    kPMU_LowPowerBandgap    = 1U, /*!< Low power bandgap. */
} pmu_power_bandgap_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

/*!
 * @name Status.
 * @{
 */

/*!
 * @brief Get PMU status flags.
 *
 * @param base PMU peripheral base address.
 * @return PMU status flags.It indicate if regulator output of 1P1,3P0 and 2P5 is ok
 * and brownout output of 1P1,3P0 and 2P5 is detected.
 */
uint32_t PMU_GetStatusFlags(PMU_Type *base);

/*@}*/

/*!
 * @name 1P1 Regular
 * @{
 */

/*!
 * @brief Selects the source for the reference voltage of the weak 1P1 regulator.
 *
 * @param base PMU peripheral base address.
 * @param option The option for reference voltage source, see to #pmu_1p1_weak_reference_source_t.
 */
static inline void PMU_1P1SetWeakReferenceSource(PMU_Type *base, pmu_1p1_weak_reference_source_t option)
{
    base->REG_1P1 = (base->REG_1P1 & ~PMU_REG_1P1_SELREF_WEAK_LINREG_MASK) | PMU_REG_1P1_SELREF_WEAK_LINREG(option);
}

/*!
 * @brief Enables the weak 1P1 regulator.
 *
 * This regulator can be used when the main 1P1 regulator is disabled, under low-power conditions.
 *
 * @param base PMU peripheral base address.
 * @param enable Enable the feature or not.
 */
static inline void PMU_1P1EnableWeakRegulator(PMU_Type *base, bool enable)
{
    if (enable)
    {
        base->REG_1P1 |= PMU_REG_1P1_ENABLE_WEAK_LINREG_MASK;
    }
    else
    {
        base->REG_1P1 &= ~PMU_REG_1P1_ENABLE_WEAK_LINREG_MASK;
    }
}

/*!
 * @brief Adjust the 1P1 regulator output voltage.
 *
 * Each LSB is worth 25mV. Programming examples are detailed below. Other output target voltages
 * may be interpolated from these examples. Choices must be in this range:
 * - 0x1b(1.375V) >= output_trg >= 0x04(0.8V)
 * - 0x04 : 0.8V
 * - 0x10 : 1.1V (typical)
 * - 0x1b : 1.375V
 * NOTE: There may be reduced chip functionality or reliability at the extremes of the programming range.
 *
 * @param base PMU peripheral base address.
 * @param value Setting value for the output.
 */
static inline void PMU_1P1SetRegulatorOutputVoltage(PMU_Type *base, uint32_t value)
{
    base->REG_1P1 = (base->REG_1P1 & ~PMU_REG_1P1_OUTPUT_TRG_MASK) | PMU_REG_1P1_OUTPUT_TRG(value);
}

/*!
 * @brief Adjust the 1P1 regulator brownout offset voltage.
 *
 * Control bits to adjust the regulator brownout offset voltage in 25mV steps. The reset
 * brown-offset is 175mV below the programmed target code.
 * Brownout target = OUTPUT_TRG - BO_OFFSET.
 * Some steps may be irrelevant because of input supply limitations or load operation.
 *
 * @param base PMU peripheral base address.
 * @param value Setting value for the brownout offset. The available range is in 3-bit.
 */
static inline void PMU_1P1SetBrownoutOffsetVoltage(PMU_Type *base, uint32_t value)
{
    base->REG_1P1 = (base->REG_1P1 & ~PMU_REG_1P1_BO_OFFSET_MASK) | PMU_REG_1P1_BO_OFFSET(value);
}

/*!
 * @brief Enable the pull-down circuitry in the regulator.
 *
 * @param base PMU peripheral base address.
 * @param enable Enable the feature or not.
 */
static inline void PMU_1P1EnablePullDown(PMU_Type *base, bool enable)
{
    if (enable)
    {
        base->REG_1P1 |= PMU_REG_1P1_ENABLE_PULLDOWN_MASK;
    }
    else
    {
        base->REG_1P1 &= ~PMU_REG_1P1_ENABLE_PULLDOWN_MASK;
    }
}

/*!
 * @brief Enable the current-limit circuitry in the regulator.
 *
 * @param base PMU peripheral base address.
 * @param enable Enable the feature or not.
 */
static inline void PMU_1P1EnableCurrentLimit(PMU_Type *base, bool enable)
{
    if (enable)
    {
        base->REG_1P1 |= PMU_REG_1P1_ENABLE_ILIMIT_MASK;
    }
    else
    {
        base->REG_1P1 &= ~PMU_REG_1P1_ENABLE_ILIMIT_MASK;
    }
}

/*!
 * @brief Enable the brownout circuitry in the regulator.
 *
 * @param base PMU peripheral base address.
 * @param enable Enable the feature or not.
 */
static inline void PMU_1P1EnableBrownout(PMU_Type *base, bool enable)
{
    if (enable)
    {
        base->REG_1P1 |= PMU_REG_1P1_ENABLE_BO_MASK;
    }
    else
    {
        base->REG_1P1 &= ~PMU_REG_1P1_ENABLE_BO_MASK;
    }
}

/*!
 * @brief Enable the regulator output.
 *
 * @param base PMU peripheral base address.
 * @param enable Enable the feature or not.
 */
static inline void PMU_1P1EnableOutput(PMU_Type *base, bool enable)
{
    if (enable)
    {
        base->REG_1P1 |= PMU_REG_1P1_ENABLE_LINREG_MASK;
    }
    else
    {
        base->REG_1P1 &= ~PMU_REG_1P1_ENABLE_LINREG_MASK;
    }
}

/*@}*/

/*!
 * @name 3P0 Regular
 * @{
 */

/*!
 * @brief Adjust the 3P0 regulator output voltage.
 *
 * Each LSB is worth 25mV. Programming examples are detailed below. Other output target voltages
 * may be interpolated from these examples. Choices must be in this range:
 * - 0x00(2.625V) >= output_trg >= 0x1f(3.4V)
 * - 0x00 : 2.625V
 * - 0x0f : 3.0V (typical)
 * - 0x1f : 3.4V
 *
 * @param base PMU peripheral base address.
 * @param value Setting value for the output.
 */
static inline void PMU_3P0SetRegulatorOutputVoltage(PMU_Type *base, uint32_t value)
{
    base->REG_3P0 = (base->REG_3P0 & ~PMU_REG_3P0_OUTPUT_TRG_MASK) | PMU_REG_3P0_OUTPUT_TRG(value);
}

/*!
 * @brief Select input voltage source for LDO_3P0.
 *
 * Select input voltage source for LDO_3P0 from either USB_OTG1_VBUS or USB_OTG2_VBUS. If only
 * one of the two VBUS voltages is present, it is automatically selected.
 *
 * @param base PMU peripheral base address.
 * @param option User-defined input voltage source for LDO_3P0.
 */
static inline void PMU_3P0SetVBusVoltageSource(PMU_Type *base, pmu_3p0_vbus_voltage_source_t option)
{
    base->REG_3P0 = (base->REG_3P0 & ~PMU_REG_3P0_VBUS_SEL_MASK) | PMU_REG_3P0_VBUS_SEL(option);
}

/*!
 * @brief Adjust the 3P0 regulator brownout offset voltage.
 *
 * Control bits to adjust the 3P0 regulator brownout offset voltage in 25mV steps. The reset
 * brown-offset is 175mV below the programmed target code.
 * Brownout target = OUTPUT_TRG - BO_OFFSET.
 * Some steps may be irrelevant because of input supply limitations or load operation.
 *
 * @param base PMU peripheral base address.
 * @param value Setting value for the brownout offset. The available range is in 3-bit.
 */
static inline void PMU_3P0SetBrownoutOffsetVoltage(PMU_Type *base, uint32_t value)
{
    base->REG_3P0 = (base->REG_3P0 & ~PMU_REG_3P0_BO_OFFSET_MASK) | PMU_REG_3P0_BO_OFFSET(value);
}

/*!
 * @brief Enable the current-limit circuitry in the 3P0 regulator.
 *
 * @param base PMU peripheral base address.
 * @param enable Enable the feature or not.
 */
static inline void PMU_3P0EnableCurrentLimit(PMU_Type *base, bool enable)
{
    if (enable)
    {
        base->REG_3P0 |= PMU_REG_3P0_ENABLE_ILIMIT_MASK;
    }
    else
    {
        base->REG_3P0 &= ~PMU_REG_3P0_ENABLE_ILIMIT_MASK;
    }
}

/*!
 * @brief Enable the brownout circuitry in the 3P0 regulator.
 *
 * @param base PMU peripheral base address.
 * @param enable Enable the feature or not.
 */
static inline void PMU_3P0EnableBrownout(PMU_Type *base, bool enable)
{
    if (enable)
    {
        base->REG_3P0 |= PMU_REG_3P0_ENABLE_BO_MASK;
    }
    else
    {
        base->REG_3P0 &= ~PMU_REG_3P0_ENABLE_BO_MASK;
    }
}

/*!
 * @brief Enable the 3P0 regulator output.
 *
 * @param base PMU peripheral base address.
 * @param enable Enable the feature or not.
 */
static inline void PMU_3P0EnableOutput(PMU_Type *base, bool enable)
{
    if (enable)
    {
        base->REG_3P0 |= PMU_REG_3P0_ENABLE_LINREG_MASK;
    }
    else
    {
        base->REG_3P0 &= ~PMU_REG_3P0_ENABLE_LINREG_MASK;
    }
}

/* @} */

/*!
 * @name 2P5 Regulator
 * @{
 */

/*!
 * @brief Enables the weak 2P5 regulator.
 *
 * This low power regulator is used when the main 2P5 regulator is disabled
 * to keep the 2.5V output roughly at 2.5V. Scales directly with the value of VDDHIGH_IN.
 *
 * @param base PMU peripheral base address.
 * @param enable Enable the feature or not.
 */
static inline void PMU_2P5EnableWeakRegulator(PMU_Type *base, bool enable)
{
    if (enable)
    {
        base->REG_2P5 |= PMU_REG_2P5_ENABLE_WEAK_LINREG_MASK;
    }
    else
    {
        base->REG_2P5 &= ~PMU_REG_2P5_ENABLE_WEAK_LINREG_MASK;
    }
}

/*!
 * @brief Adjust the 1P1 regulator output voltage.
 *
 * Each LSB is worth 25mV. Programming examples are detailed below. Other output target voltages
 * may be interpolated from these examples. Choices must be in this range:
 * - 0x00(2.1V) >= output_trg >= 0x1f(2.875V)
 * - 0x00 : 2.1V
 * - 0x10 : 2.5V (typical)
 * - 0x1f : 2.875V
 * NOTE: There may be reduced chip functionality or reliability at the extremes of the programming range.
 *
 * @param base PMU peripheral base address.
 * @param value Setting value for the output.
 */
static inline void PMU_2P5SetRegulatorOutputVoltage(PMU_Type *base, uint32_t value)
{
    base->REG_2P5 = (base->REG_2P5 & ~PMU_REG_2P5_OUTPUT_TRG_MASK) | PMU_REG_2P5_OUTPUT_TRG(value);
}

/*!
 * @brief Adjust the 2P5 regulator brownout offset voltage.
 *
 * Adjust the regulator brownout offset voltage in 25mV steps. The reset
 * brown-offset is 175mV below the programmed target code.
 * Brownout target = OUTPUT_TRG - BO_OFFSET.
 * Some steps may be irrelevant because of input supply limitations or load operation.
 *
 * @param base PMU peripheral base address.
 * @param value Setting value for the brownout offset. The available range is in 3-bit.
 */
static inline void PMU_2P5SetBrownoutOffsetVoltage(PMU_Type *base, uint32_t value)
{
    base->REG_2P5 = (base->REG_2P5 & ~PMU_REG_2P5_BO_OFFSET_MASK) | PMU_REG_2P5_BO_OFFSET(value);
}

/*!
 * @brief Enable the pull-down circuitry in the 2P5 regulator.
 *
 * @param base PMU peripheral base address.
 * @param enable Enable the feature or not.
 */
static inline void PMU_2P5EnablePullDown(PMU_Type *base, bool enable)
{
    if (enable)
    {
        base->REG_2P5 |= PMU_REG_2P5_ENABLE_PULLDOWN_MASK;
    }
    else
    {
        base->REG_2P5 &= ~PMU_REG_2P5_ENABLE_PULLDOWN_MASK;
    }
}

/*!
 * @brief Enable the pull-down circuitry in the 2P5 regulator.
 * @deprecated Do not use this function.  It has been superceded by @ref PMU_2P5EnablePullDown.
 */
static inline void PMU_2P1EnablePullDown(PMU_Type *base, bool enable)
{
    if (enable)
    {
        base->REG_2P5 |= PMU_REG_2P5_ENABLE_PULLDOWN_MASK;
    }
    else
    {
        base->REG_2P5 &= ~PMU_REG_2P5_ENABLE_PULLDOWN_MASK;
    }
}

/*!
 * @brief Enable the current-limit circuitry in the 2P5 regulator.
 *
 * @param base PMU peripheral base address.
 * @param enable Enable the feature or not.
 */
static inline void PMU_2P5EnableCurrentLimit(PMU_Type *base, bool enable)
{
    if (enable)
    {
        base->REG_2P5 |= PMU_REG_2P5_ENABLE_ILIMIT_MASK;
    }
    else
    {
        base->REG_2P5 &= ~PMU_REG_2P5_ENABLE_ILIMIT_MASK;
    }
}

/*!
 * @brief Enable the brownout circuitry in the 2P5 regulator.
 *
 * @param base PMU peripheral base address.
 * @param enable Enable the feature or not.
 */
static inline void PMU_2P5nableBrownout(PMU_Type *base, bool enable)
{
    if (enable)
    {
        base->REG_2P5 |= PMU_REG_2P5_ENABLE_BO_MASK;
    }
    else
    {
        base->REG_2P5 &= ~PMU_REG_2P5_ENABLE_BO_MASK;
    }
}

/*!
 * @brief Enable the 2P5 regulator output.
 *
 * @param base PMU peripheral base address.
 * @param enable Enable the feature or not.
 */
static inline void PMU_2P5EnableOutput(PMU_Type *base, bool enable)
{
    if (enable)
    {
        base->REG_2P5 |= PMU_REG_2P5_ENABLE_LINREG_MASK;
    }
    else
    {
        base->REG_2P5 &= ~PMU_REG_2P5_ENABLE_LINREG_MASK;
    }
}

/* @} */

/*!
 * @name Core Regulator
 * @{
 */

/*!
 * @brief Increase the gate drive on power gating FETs.
 *
 * If set, increases the gate drive on power gating FETs to reduce leakage in the off state.
 * Care must be taken to apply this bit only when the input supply voltage to the power FET
 * is less than 1.1V.
 * NOTE: This bit should only be used in low-power modes where the external input supply voltage
 * is nominally 0.9V.
 *
 * @param base PMU peripheral base address.
 * @param enable Enable the feature or not.
 */
static inline void PMU_CoreEnableIncreaseGateDrive(PMU_Type *base, bool enable)
{
    if (enable)
    {
        base->REG_CORE |= PMU_REG_CORE_FET_ODRIVE_MASK;
    }
    else
    {
        base->REG_CORE &= ~PMU_REG_CORE_FET_ODRIVE_MASK;
    }
}

/*!
 * @brief Set the CORE regulator voltage ramp rate.
 *
 * @param base PMU peripheral base address.
 * @param option User-defined option for voltage ramp rate, see to #pmu_core_reg_voltage_ramp_rate_t.
 */
static inline void PMU_CoreSetRegulatorVoltageRampRate(PMU_Type *base, pmu_core_reg_voltage_ramp_rate_t option)
{
    base->REG_CORE = (base->REG_CORE & ~PMU_REG_CORE_RAMP_RATE_MASK) | PMU_REG_CORE_RAMP_RATE(option);
}

/*!
 * @brief Define the target voltage for the SOC power domain.
 *
 * Define the target voltage for the SOC power domain. Single-bit increments reflect 25mV core
 * voltage steps. Some steps may not be relevant because of input supply limitations or load operation.
 * - 0x00 : Power gated off.
 * - 0x01 : Target core voltage = 0.725V
 * - 0x02 : Target core voltage = 0.750V
 * - ...
 * - 0x10 : Target core voltage = 1.100V
 * - ...
 * - 0x1e : Target core voltage = 1.450V
 * - 0x1F : Power FET switched full on. No regulation.
 * NOTE: This register is capable of programming an over-voltage condition on the device. Consult the
 * datasheet Operating Ranges table for the allowed voltages.
 *
 * @param base PMU peripheral base address.
 * @param value Setting value for target voltage. 5-bit available
 */
static inline void PMU_CoreSetSOCDomainVoltage(PMU_Type *base, uint32_t value)
{
    base->REG_CORE = (base->REG_CORE & ~PMU_REG_CORE_REG2_TARG_MASK) | PMU_REG_CORE_REG2_TARG(value);
}

/*!
 * @brief Define the target voltage for the ARM Core power domain.
 *
 * Define the target voltage for the ARM Core power domain. Single-bit increments reflect 25mV core
 * voltage steps. Some steps may not be relevant because of input supply limitations or load operation.
 * - 0x00 : Power gated off.
 * - 0x01 : Target core voltage = 0.725V
 * - 0x02 : Target core voltage = 0.750V
 * - ...
 * - 0x10 : Target core voltage = 1.100V
 * - ...
 * - 0x1e : Target core voltage = 1.450V
 * - 0x1F : Power FET switched full on. No regulation.
 * NOTE: This register is capable of programming an over-voltage condition on the device. Consult the
 * datasheet Operating Ranges table for the allowed voltages.
 *
 * @param base PMU peripheral base address.
 * @param value Setting value for target voltage. 5-bit available
 */
static inline void PMU_CoreSetARMCoreDomainVoltage(PMU_Type *base, uint32_t value)
{
    base->REG_CORE = (base->REG_CORE & ~PMU_REG_CORE_REG0_TARG_MASK) | PMU_REG_CORE_REG0_TARG(value);
}

/* @} */

#if defined(FSL_FEATURE_PMU_HAS_LOWPWR_CTRL) && FSL_FEATURE_PMU_HAS_LOWPWR_CTRL
/*!
 * @name Power Gate Controller & other
 * @{
 */

/*!
 * @brief Gate the power to modules.
 *
 * @param base PMU peripheral base address.
 * @param gates Mask value for the module to be gated. See to #_pmu_power_gate.
 */
static inline void PMU_GatePower(PMU_Type *base, uint32_t gates)
{
    base->LOWPWR_CTRL_SET = gates;
}

/*!
 * @brief Ungate the power to modules.
 *
 * @param base PMU peripheral base address.
 * @param gates Mask value for the module to be gated. See to #_pmu_power_gate.
 */
static inline void PMU_UngatePower(PMU_Type *base, uint32_t gates)
{
    base->LOWPWR_CTRL_CLR = gates;
}

/*!
 * @brief Enable the low power bandgap.
 *
 * @param base PMU peripheral base address.
 * @param enable Enable the low power bandgap or use the normal power bandgap.
 * @
 */
static inline void PMU_EnableLowPowerBandgap(PMU_Type *base, bool enable)
{
    if (enable)
    {
        base->LOWPWR_CTRL_SET = PMU_LOWPWR_CTRL_LPBG_SEL_MASK; /* Use the low power bandgap. */
    }
    else
    {
        base->LOWPWR_CTRL_CLR = PMU_LOWPWR_CTRL_LPBG_SEL_MASK; /* Use the normal power bandgap. */
    }
}
#endif /* FSL_FEATURE_PMU_HAS_LOWPWR_CTRL. */
/* @} */

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

/*! @}*/

#endif /* _FSL_PMU_H_*/
