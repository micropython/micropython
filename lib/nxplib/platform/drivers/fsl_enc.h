/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_ENC_H_
#define _FSL_ENC_H_

#include "fsl_common.h"

/*!
 * @addtogroup enc
 * @{
 */
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define FSL_ENC_DRIVER_VERSION (MAKE_VERSION(2, 0, 0)) /*!< Version 2.0.0. */

/*!
 * @brief Interrupt enable/disable mask.
 */
enum _enc_interrupt_enable
{
    kENC_HOMETransitionInterruptEnable  = (1U << 0U), /*!< HOME interrupt enable. */
    kENC_INDEXPulseInterruptEnable      = (1U << 1U), /*!< INDEX pulse interrupt enable. */
    kENC_WatchdogTimeoutInterruptEnable = (1U << 2U), /*!< Watchdog timeout interrupt enable. */
    kENC_PositionCompareInerruptEnable  = (1U << 3U), /*!< Position compare interrupt enable. */
    kENC_SimultBothPhaseChangeInterruptEnable =
        (1U << 4U),                                     /*!< Simultaneous PHASEA and PHASEB change interrupt enable. */
    kENC_PositionRollOverInterruptEnable  = (1U << 5U), /*!< Roll-over interrupt enable. */
    kENC_PositionRollUnderInterruptEnable = (1U << 6U), /*!< Roll-under interrupt enable. */
};

/*!
 * @brief Status flag mask.
 *
 * These flags indicate the counter's events.
 */
enum _enc_status_flags
{
    kENC_HOMETransitionFlag        = (1U << 0U), /*!< HOME signal transition interrupt request. */
    kENC_INDEXPulseFlag            = (1U << 1U), /*!< INDEX Pulse Interrupt Request. */
    kENC_WatchdogTimeoutFlag       = (1U << 2U), /*!< Watchdog timeout interrupt request. */
    kENC_PositionCompareFlag       = (1U << 3U), /*!< Position compare interrupt request. */
    kENC_SimultBothPhaseChangeFlag = (1U << 4U), /*!< Simultaneous PHASEA and PHASEB change interrupt request. */
    kENC_PositionRollOverFlag      = (1U << 5U), /*!< Roll-over interrupt request. */
    kENC_PositionRollUnderFlag     = (1U << 6U), /*!< Roll-under interrupt request. */
    kENC_LastCountDirectionFlag    = (1U << 7U), /*!< Last count was in the up direction, or the down direction. */
};

/*!
 * @brief Signal status flag mask.
 *
 * These flags indicate the counter's signal.
 */
enum _enc_signal_status_flags
{
    kENC_RawHOMEStatusFlag       = ENC_IMR_HOME_MASK,  /*!< Raw HOME input. */
    kENC_RawINDEXStatusFlag      = ENC_IMR_INDEX_MASK, /*!< Raw INDEX input. */
    kENC_RawPHBStatusFlag        = ENC_IMR_PHB_MASK,   /*!< Raw PHASEB input. */
    kENC_RawPHAEXStatusFlag      = ENC_IMR_PHA_MASK,   /*!< Raw PHASEA input. */
    kENC_FilteredHOMEStatusFlag  = ENC_IMR_FHOM_MASK,  /*!< The filtered version of HOME input. */
    kENC_FilteredINDEXStatusFlag = ENC_IMR_FIND_MASK,  /*!< The filtered version of INDEX input. */
    kENC_FilteredPHBStatusFlag   = ENC_IMR_FPHB_MASK,  /*!< The filtered version of PHASEB input. */
    kENC_FilteredPHAStatusFlag   = ENC_IMR_FPHA_MASK,  /*!< The filtered version of PHASEA input. */
};

/*!
 * @brief Define HOME signal's trigger mode.
 *
 * The ENC would count the trigger from HOME signal line.
 */
typedef enum _enc_home_trigger_mode
{
    kENC_HOMETriggerDisabled = 0U, /*!< HOME signal's trigger is disabled. */
    kENC_HOMETriggerOnRisingEdge,  /*!< Use positive going edge-to-trigger initialization of position counters. */
    kENC_HOMETriggerOnFallingEdge, /*!< Use negative going edge-to-trigger initialization of position counters. */
} enc_home_trigger_mode_t;

/*!
 * @brief Define INDEX signal's trigger mode.
 *
 * The ENC would count the trigger from INDEX signal line.
 */
typedef enum _enc_index_trigger_mode
{
    kENC_INDEXTriggerDisabled = 0U, /*!< INDEX signal's trigger is disabled. */
    kENC_INDEXTriggerOnRisingEdge,  /*!< Use positive going edge-to-trigger initialization of position counters. */
    kENC_INDEXTriggerOnFallingEdge, /*!< Use negative going edge-to-trigger initialization of position counters. */
} enc_index_trigger_mode_t;

/*!
 * @brief Define type for decoder work mode.
 *
 * The normal work mode uses the standard quadrature decoder with PHASEA and PHASEB. When in signal phase count mode,
 * a positive transition of the PHASEA input generates a count signal while the PHASEB input and the reverse direction
 * control the counter direction. If the reverse direction is not enabled, PHASEB = 0 means counting up and PHASEB = 1
 * means counting down. Otherwise, the direction is reversed.
 */
typedef enum _enc_decoder_work_mode
{
    kENC_DecoderWorkAsNormalMode = 0U,      /*!< Use standard quadrature decoder with PHASEA and PHASEB. */
    kENC_DecoderWorkAsSignalPhaseCountMode, /*!< PHASEA input generates a count signal while PHASEB input control the
                                               direction. */
} enc_decoder_work_mode_t;

/*!
 * @brief Define type for the condition of POSMATCH pulses.
 */
typedef enum _enc_position_match_mode
{
    kENC_POSMATCHOnPositionCounterEqualToComapreValue = 0U, /*!< POSMATCH pulses when a match occurs between the
                                                               position counters (POS) and the compare value (COMP). */
    kENC_POSMATCHOnReadingAnyPositionCounter, /*!< POSMATCH pulses when any position counter register is read. */
} enc_position_match_mode_t;

/*!
 * @brief Define type for determining how the revolution counter (REV) is incremented/decremented.
 */
typedef enum _enc_revolution_count_condition
{
    kENC_RevolutionCountOnINDEXPulse = 0U, /*!< Use INDEX pulse to increment/decrement revolution counter. */
    kENC_RevolutionCountOnRollOverModulus, /*!< Use modulus counting roll-over/under to increment/decrement revolution
                                              counter. */
} enc_revolution_count_condition_t;

/*!
 * @brief Define type for direction of self test generated signal.
 */
typedef enum _enc_self_test_direction
{
    kENC_SelfTestDirectionPositive = 0U, /*!< Self test generates the signal in positive direction. */
    kENC_SelfTestDirectionNegative,      /*!< Self test generates the signal in negative direction. */
} enc_self_test_direction_t;

/*!
 * @brief Define user configuration structure for ENC module.
 */
typedef struct _enc_config
{
    /* Basic counter. */
    bool enableReverseDirection;             /*!< Enable reverse direction counting. */
    enc_decoder_work_mode_t decoderWorkMode; /*!< Enable signal phase count mode. */

    /* Signal detection. */
    enc_home_trigger_mode_t HOMETriggerMode;   /*!< Enable HOME to initialize position counters. */
    enc_index_trigger_mode_t INDEXTriggerMode; /*!< Enable INDEX to initialize position counters. */
    bool enableTRIGGERClearPositionCounter;    /*!< Clear POSD, REV, UPOS and LPOS on rising edge of TRIGGER, or not. */
    bool enableTRIGGERClearHoldPositionCounter; /*!< Enable update of hold registers on rising edge of TRIGGER, or not.
                                                 */

    /* Watchdog. */
    bool enableWatchdog;           /*!< Enable the watchdog to detect if the target is moving or not. */
    uint16_t watchdogTimeoutValue; /*!< Watchdog timeout count value. It stores the timeout count for the quadrature
                                        decoder module watchdog timer. This field is only available when
                                        "enableWatchdog" = true. The available value is a 16-bit unsigned number.*/

    /* Filter for PHASEA, PHASEB, INDEX and HOME. */
    uint16_t filterCount; /*!< Input Filter Sample Count. This value should be chosen to reduce the probability of
                               noisy samples causing an incorrect transition to be recognized. The value represent the
                               number of consecutive samples that must agree prior to the input filter accepting an
                               input transition. A value of 0x0 represents 3 samples. A value of 0x7 represents 10
                               samples. The Available range is 0 - 7.*/
    uint16_t filterSamplePeriod; /*!< Input Filter Sample Period. This value should be set such that the sampling period
                                      is larger than the period of the expected noise. This value represents the
                                      sampling period (in IPBus clock cycles) of the decoder input signals.
                                      The available range is 0 - 255. */

    /* Position compare. */
    enc_position_match_mode_t positionMatchMode; /*!< The condition of POSMATCH pulses. */
    uint32_t positionCompareValue;               /*!< Position compare value. The available value is a 32-bit number.*/

    /* Modulus counting. */
    enc_revolution_count_condition_t revolutionCountCondition; /*!< Revolution Counter Modulus Enable. */
    bool enableModuloCountMode;                                /*!< Enable Modulo Counting. */
    uint32_t positionModulusValue; /*!< Position modulus value. This value would be available only when
                                        "enableModuloCountMode" = true. The available value is a 32-bit number. */
    uint32_t positionInitialValue; /*!< Position initial value. The available value is a 32-bit number. */
} enc_config_t;

/*!
 * @brief Define configuration structure for self test module.
 *
 * The self test module provides a quadrature test signal to the inputs of the quadrature decoder module.
 * This is a factory test feature. It is also useful to customers' software development and testing.
 */
typedef struct _enc_self_test_config
{
    enc_self_test_direction_t signalDirection; /*!< Direction of self test generated signal. */
    uint16_t signalCount;  /*!< Hold the number of quadrature advances to generate. The available range is 0 - 255.*/
    uint16_t signalPeriod; /*!< Hold the period of quadrature phase in IPBus clock cycles.
                                The available range is 0 - 31. */
} enc_self_test_config_t;

#if defined(__cplusplus)
extern "C" {
#endif

/*******************************************************************************
 * API
 ******************************************************************************/

/*!
 * @name Initialization and De-initialization
 * @{
 */

/*!
 * @brief Initialization for the ENC module.
 *
 * This function is to make the initialization for the ENC module. It should be called firstly before any operation to
 * the ENC with the operations like:
 *  - Enable the clock for ENC module.
 *  - Configure the ENC's working attributes.
 *
 * @param base   ENC peripheral base address.
 * @param config Pointer to configuration structure. See to "enc_config_t".
 */
void ENC_Init(ENC_Type *base, const enc_config_t *config);

/*!
 * @brief De-initialization for the ENC module.
 *
 * This function is to make the de-initialization for the ENC module. It could be called when ENC is no longer used with
 * the operations like:
 *  - Disable the clock for ENC module.
 *
 * @param base ENC peripheral base address.
 */
void ENC_Deinit(ENC_Type *base);

/*!
 * @brief Get an available pre-defined settings for ENC's configuration.
 *
 * This function initializes the ENC configuration structure with an available settings, the default value are:
 * @code
 *   config->enableReverseDirection                = false;
 *   config->decoderWorkMode                       = kENC_DecoderWorkAsNormalMode;
 *   config->HOMETriggerMode                       = kENC_HOMETriggerDisabled;
 *   config->INDEXTriggerMode                      = kENC_INDEXTriggerDisabled;
 *   config->enableTRIGGERClearPositionCounter     = false;
 *   config->enableTRIGGERClearHoldPositionCounter = false;
 *   config->enableWatchdog                        = false;
 *   config->watchdogTimeoutValue                  = 0U;
 *   config->filterCount                           = 0U;
 *   config->filterSamplePeriod                    = 0U;
 *   config->positionMatchMode                     = kENC_POSMATCHOnPositionCounterEqualToComapreValue;
 *   config->positionCompareValue                  = 0xFFFFFFFFU;
 *   config->revolutionCountCondition              = kENC_RevolutionCountOnINDEXPulse;
 *   config->enableModuloCountMode                 = false;
 *   config->positionModulusValue                  = 0U;
 *   config->positionInitialValue                  = 0U;
 * @endcode
 * @param config Pointer to a variable of configuration structure. See to "enc_config_t".
 */
void ENC_GetDefaultConfig(enc_config_t *config);

/*!
 * @brief Load the initial position value to position counter.
 *
 * This function is to transfer the initial position value (UINIT and LINIT) contents to position counter (UPOS and
 * LPOS), so that to provide the consistent operation the position counter registers.
 *
 * @param base ENC peripheral base address.
 */
void ENC_DoSoftwareLoadInitialPositionValue(ENC_Type *base);

/*!
 * @brief Enable and configure the self test function.
 *
 * This function is to enable and configuration the self test function. It controls and sets the frequency of a
 * quadrature signal generator. It provides a quadrature test signal to the inputs of the quadrature decoder module.
 * It is a factory test feature; however, it may be useful to customers' software development and testing.
 *
 * @param base   ENC peripheral base address.
 * @param config Pointer to configuration structure. See to "enc_self_test_config_t". Pass "NULL" to disable.
 */
void ENC_SetSelfTestConfig(ENC_Type *base, const enc_self_test_config_t *config);

/*!
 * @brief Enable watchdog for ENC module.
 *
 * @param base ENC peripheral base address
 * @param enable Enables or disables the watchdog
 */
void ENC_EnableWatchdog(ENC_Type *base, bool enable);

/*!
 * @brief Set initial position value for ENC module.
 *
 * @param base ENC peripheral base address
 * @param value Positive initial value
 */
void ENC_SetInitialPositionValue(ENC_Type *base, uint32_t value);

/* @} */

/*!
 * @name Status
 * @{
 */
/*!
 * @brief  Get the status flags.
 *
 * @param  base ENC peripheral base address.
 *
 * @return      Mask value of status flags. For available mask, see to "_enc_status_flags".
 */
uint32_t ENC_GetStatusFlags(ENC_Type *base);

/*!
 * @brief Clear the status flags.
 *
 * @param base ENC peripheral base address.
 * @param mask Mask value of status flags to be cleared. For available mask, see to "_enc_status_flags".
 */
void ENC_ClearStatusFlags(ENC_Type *base, uint32_t mask);

/*!
 * @brief  Get the signals' real-time status.
 *
 * @param  base ENC peripheral base address.
 *
 * @return      Mask value of signals' real-time status. For available mask, see to "_enc_signal_status_flags"
 */
static inline uint16_t ENC_GetSignalStatusFlags(ENC_Type *base)
{
    return base->IMR;
}
/* @} */

/*!
 * @name Interrupts
 * @{
 */

/*!
 * @brief Enable the interrupts.
 *
 * @param base ENC peripheral base address.
 * @param mask Mask value of interrupts to be enabled. For available mask, see to "_enc_interrupt_enable".
 */
void ENC_EnableInterrupts(ENC_Type *base, uint32_t mask);

/*!
 * @brief Disable the interrupts.
 *
 * @param base ENC peripheral base address.
 * @param mask Mask value of interrupts to be disabled. For available mask, see to "_enc_interrupt_enable".
 */
void ENC_DisableInterrupts(ENC_Type *base, uint32_t mask);

/*!
 * @brief  Get the enabled interrupts' flags.
 *
 * @param  base ENC peripheral base address.
 *
 * @return      Mask value of enabled interrupts.
 */
uint32_t ENC_GetEnabledInterrupts(ENC_Type *base);

/* @} */

/*!
 * @name Value Operation
 * @{
 */

/*!
 * @brief  Get the current position counter's value.
 *
 * @param  base ENC peripheral base address.
 *
 * @return     Current position counter's value.
 */
uint32_t ENC_GetPositionValue(ENC_Type *base);

/*!
 * @brief  Get the hold position counter's value.
 *
 * When any of the counter registers is read, the contents of each counter register is written to the corresponding hold
 * register. Taking a snapshot of the counters' values provides a consistent view of a system position and a velocity to
 * be attained.
 *
 * @param  base ENC peripheral base address.
 *
 * @return      Hold position counter's value.
 */
uint32_t ENC_GetHoldPositionValue(ENC_Type *base);

/*!
 * @brief  Get the position difference counter's value.
 *
 * @param  base ENC peripheral base address.
 *
 * @return     The position difference counter's value.
 */
static inline uint16_t ENC_GetPositionDifferenceValue(ENC_Type *base)
{
    return base->POSD;
}

/*!
 * @brief  Get the hold position difference counter's value.
 *
 * When any of the counter registers is read, the contents of each counter register is written to the corresponding hold
 * register. Taking a snapshot of the counters' values provides a consistent view of a system position and a velocity to
 * be attained.
 *
 * @param  base ENC peripheral base address.
 *
 * @return      Hold position difference counter's value.
 */
static inline uint16_t ENC_GetHoldPositionDifferenceValue(ENC_Type *base)
{
    return base->POSDH;
}

/*!
 * @brief  Get the position revolution counter's value.
 *
 * @param  base ENC peripheral base address.
 *
 * @return     The position revolution counter's value.
 */
static inline uint16_t ENC_GetRevolutionValue(ENC_Type *base)
{
    return base->REV;
}
/*!
 * @brief  Get the hold position revolution counter's value.
 *
 * When any of the counter registers is read, the contents of each counter register is written to the corresponding hold
 * register. Taking a snapshot of the counters' values provides a consistent view of a system position and a velocity to
 * be attained.
 *
 * @param  base ENC peripheral base address.
 *
 * @return      Hold position revolution counter's value.
 */
static inline uint16_t ENC_GetHoldRevolutionValue(ENC_Type *base)
{
    return base->REVH;
}

/* @} */

#if defined(__cplusplus)
}
#endif
/*
 * @}
 */
#endif /* _FSL_ENC_H_ */
