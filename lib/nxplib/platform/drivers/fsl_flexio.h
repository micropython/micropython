/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_FLEXIO_H_
#define _FSL_FLEXIO_H_

#include "fsl_common.h"

/*!
 * @addtogroup flexio_driver
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief FlexIO driver version 2.0.2. */
#define FSL_FLEXIO_DRIVER_VERSION (MAKE_VERSION(2, 0, 2))
/*@}*/

/*! @brief Calculate FlexIO timer trigger.*/
#define FLEXIO_TIMER_TRIGGER_SEL_PININPUT(x) ((uint32_t)(x) << 1U)
#define FLEXIO_TIMER_TRIGGER_SEL_SHIFTnSTAT(x) (((uint32_t)(x) << 2U) | 0x1U)
#define FLEXIO_TIMER_TRIGGER_SEL_TIMn(x) (((uint32_t)(x) << 2U) | 0x3U)

/*! @brief Define time of timer trigger polarity.*/
typedef enum _flexio_timer_trigger_polarity
{
    kFLEXIO_TimerTriggerPolarityActiveHigh = 0x0U, /*!< Active high. */
    kFLEXIO_TimerTriggerPolarityActiveLow  = 0x1U, /*!< Active low. */
} flexio_timer_trigger_polarity_t;

/*! @brief Define type of timer trigger source.*/
typedef enum _flexio_timer_trigger_source
{
    kFLEXIO_TimerTriggerSourceExternal = 0x0U, /*!< External trigger selected. */
    kFLEXIO_TimerTriggerSourceInternal = 0x1U, /*!< Internal trigger selected. */
} flexio_timer_trigger_source_t;

/*! @brief Define type of timer/shifter pin configuration.*/
typedef enum _flexio_pin_config
{
    kFLEXIO_PinConfigOutputDisabled         = 0x0U, /*!< Pin output disabled. */
    kFLEXIO_PinConfigOpenDrainOrBidirection = 0x1U, /*!< Pin open drain or bidirectional output enable. */
    kFLEXIO_PinConfigBidirectionOutputData  = 0x2U, /*!< Pin bidirectional output data. */
    kFLEXIO_PinConfigOutput                 = 0x3U, /*!< Pin output. */
} flexio_pin_config_t;

/*! @brief Definition of pin polarity.*/
typedef enum _flexio_pin_polarity
{
    kFLEXIO_PinActiveHigh = 0x0U, /*!< Active high. */
    kFLEXIO_PinActiveLow  = 0x1U, /*!< Active low. */
} flexio_pin_polarity_t;

/*! @brief Define type of timer work mode.*/
typedef enum _flexio_timer_mode
{
    kFLEXIO_TimerModeDisabled        = 0x0U, /*!< Timer Disabled. */
    kFLEXIO_TimerModeDual8BitBaudBit = 0x1U, /*!< Dual 8-bit counters baud/bit mode. */
    kFLEXIO_TimerModeDual8BitPWM     = 0x2U, /*!< Dual 8-bit counters PWM mode. */
    kFLEXIO_TimerModeSingle16Bit     = 0x3U, /*!< Single 16-bit counter mode. */
} flexio_timer_mode_t;

/*! @brief Define type of timer initial output or timer reset condition.*/
typedef enum _flexio_timer_output
{
    kFLEXIO_TimerOutputOneNotAffectedByReset = 0x0U,  /*!< Logic one when enabled and is not affected by timer
                                                       reset. */
    kFLEXIO_TimerOutputZeroNotAffectedByReset = 0x1U, /*!< Logic zero when enabled and is not affected by timer
                                                       reset. */
    kFLEXIO_TimerOutputOneAffectedByReset  = 0x2U,    /*!< Logic one when enabled and on timer reset. */
    kFLEXIO_TimerOutputZeroAffectedByReset = 0x3U,    /*!< Logic zero when enabled and on timer reset. */
} flexio_timer_output_t;

/*! @brief Define type of timer decrement.*/
typedef enum _flexio_timer_decrement_source
{
    kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput = 0x0U,   /*!< Decrement counter on FlexIO clock, Shift clock
                                                                equals Timer output. */
    kFLEXIO_TimerDecSrcOnTriggerInputShiftTimerOutput = 0x1U,  /*!< Decrement counter on Trigger input (both edges),
                                                                Shift clock equals Timer output. */
    kFLEXIO_TimerDecSrcOnPinInputShiftPinInput = 0x2U,         /*!< Decrement counter on Pin input (both edges),
                                                                Shift clock equals Pin input. */
    kFLEXIO_TimerDecSrcOnTriggerInputShiftTriggerInput = 0x3U, /*!< Decrement counter on Trigger input (both edges),
                                                                Shift clock equals Trigger input. */
} flexio_timer_decrement_source_t;

/*! @brief Define type of timer reset condition.*/
typedef enum _flexio_timer_reset_condition
{
    kFLEXIO_TimerResetNever                            = 0x0U, /*!< Timer never reset. */
    kFLEXIO_TimerResetOnTimerPinEqualToTimerOutput     = 0x2U, /*!< Timer reset on Timer Pin equal to Timer Output. */
    kFLEXIO_TimerResetOnTimerTriggerEqualToTimerOutput = 0x3U, /*!< Timer reset on Timer Trigger equal to
                                                                Timer Output. */
    kFLEXIO_TimerResetOnTimerPinRisingEdge     = 0x4U,         /*!< Timer reset on Timer Pin rising edge. */
    kFLEXIO_TimerResetOnTimerTriggerRisingEdge = 0x6U,         /*!< Timer reset on Trigger rising edge. */
    kFLEXIO_TimerResetOnTimerTriggerBothEdge   = 0x7U,         /*!< Timer reset on Trigger rising or falling edge. */
} flexio_timer_reset_condition_t;

/*! @brief Define type of timer disable condition.*/
typedef enum _flexio_timer_disable_condition
{
    kFLEXIO_TimerDisableNever                    = 0x0U, /*!< Timer never disabled. */
    kFLEXIO_TimerDisableOnPreTimerDisable        = 0x1U, /*!< Timer disabled on Timer N-1 disable. */
    kFLEXIO_TimerDisableOnTimerCompare           = 0x2U, /*!< Timer disabled on Timer compare. */
    kFLEXIO_TimerDisableOnTimerCompareTriggerLow = 0x3U, /*!< Timer disabled on Timer compare and Trigger Low. */
    kFLEXIO_TimerDisableOnPinBothEdge            = 0x4U, /*!< Timer disabled on Pin rising or falling edge. */
    kFLEXIO_TimerDisableOnPinBothEdgeTriggerHigh = 0x5U, /*!< Timer disabled on Pin rising or falling edge provided
                                                          Trigger is high. */
    kFLEXIO_TimerDisableOnTriggerFallingEdge = 0x6U,     /*!< Timer disabled on Trigger falling edge. */
} flexio_timer_disable_condition_t;

/*! @brief Define type of timer enable condition.*/
typedef enum _flexio_timer_enable_condition
{
    kFLEXIO_TimerEnabledAlways                    = 0x0U, /*!< Timer always enabled. */
    kFLEXIO_TimerEnableOnPrevTimerEnable          = 0x1U, /*!< Timer enabled on Timer N-1 enable. */
    kFLEXIO_TimerEnableOnTriggerHigh              = 0x2U, /*!< Timer enabled on Trigger high. */
    kFLEXIO_TimerEnableOnTriggerHighPinHigh       = 0x3U, /*!< Timer enabled on Trigger high and Pin high. */
    kFLEXIO_TimerEnableOnPinRisingEdge            = 0x4U, /*!< Timer enabled on Pin rising edge. */
    kFLEXIO_TimerEnableOnPinRisingEdgeTriggerHigh = 0x5U, /*!< Timer enabled on Pin rising edge and Trigger high. */
    kFLEXIO_TimerEnableOnTriggerRisingEdge        = 0x6U, /*!< Timer enabled on Trigger rising edge. */
    kFLEXIO_TimerEnableOnTriggerBothEdge          = 0x7U, /*!< Timer enabled on Trigger rising or falling edge. */
} flexio_timer_enable_condition_t;

/*! @brief Define type of timer stop bit generate condition.*/
typedef enum _flexio_timer_stop_bit_condition
{
    kFLEXIO_TimerStopBitDisabled                    = 0x0U, /*!< Stop bit disabled. */
    kFLEXIO_TimerStopBitEnableOnTimerCompare        = 0x1U, /*!< Stop bit is enabled on timer compare. */
    kFLEXIO_TimerStopBitEnableOnTimerDisable        = 0x2U, /*!< Stop bit is enabled on timer disable. */
    kFLEXIO_TimerStopBitEnableOnTimerCompareDisable = 0x3U, /*!< Stop bit is enabled on timer compare and timer
                                                             disable. */
} flexio_timer_stop_bit_condition_t;

/*! @brief Define type of timer start bit generate condition.*/
typedef enum _flexio_timer_start_bit_condition
{
    kFLEXIO_TimerStartBitDisabled = 0x0U, /*!< Start bit disabled. */
    kFLEXIO_TimerStartBitEnabled  = 0x1U, /*!< Start bit enabled. */
} flexio_timer_start_bit_condition_t;

/*! @brief Define type of timer polarity for shifter control. */
typedef enum _flexio_shifter_timer_polarity
{
    kFLEXIO_ShifterTimerPolarityOnPositive = 0x0U, /*!< Shift on positive edge of shift clock. */
    kFLEXIO_ShifterTimerPolarityOnNegitive = 0x1U, /*!< Shift on negative edge of shift clock. */
} flexio_shifter_timer_polarity_t;

/*! @brief Define type of shifter working mode.*/
typedef enum _flexio_shifter_mode
{
    kFLEXIO_ShifterDisabled            = 0x0U, /*!< Shifter is disabled. */
    kFLEXIO_ShifterModeReceive         = 0x1U, /*!< Receive mode. */
    kFLEXIO_ShifterModeTransmit        = 0x2U, /*!< Transmit mode. */
    kFLEXIO_ShifterModeMatchStore      = 0x4U, /*!< Match store mode. */
    kFLEXIO_ShifterModeMatchContinuous = 0x5U, /*!< Match continuous mode. */
#if FSL_FEATURE_FLEXIO_HAS_STATE_MODE
    kFLEXIO_ShifterModeState = 0x6U, /*!< SHIFTBUF contents are used for storing
                                      programmable state attributes. */
#endif                               /* FSL_FEATURE_FLEXIO_HAS_STATE_MODE */
#if FSL_FEATURE_FLEXIO_HAS_LOGIC_MODE
    kFLEXIO_ShifterModeLogic = 0x7U, /*!< SHIFTBUF contents are used for implementing
                                     programmable logic look up table. */
#endif                               /* FSL_FEATURE_FLEXIO_HAS_LOGIC_MODE */
} flexio_shifter_mode_t;

/*! @brief Define type of shifter input source.*/
typedef enum _flexio_shifter_input_source
{
    kFLEXIO_ShifterInputFromPin               = 0x0U, /*!< Shifter input from pin. */
    kFLEXIO_ShifterInputFromNextShifterOutput = 0x1U, /*!< Shifter input from Shifter N+1. */
} flexio_shifter_input_source_t;

/*! @brief Define of STOP bit configuration.*/
typedef enum _flexio_shifter_stop_bit
{
    kFLEXIO_ShifterStopBitDisable = 0x0U, /*!< Disable shifter stop bit. */
    kFLEXIO_ShifterStopBitLow     = 0x2U, /*!< Set shifter stop bit to logic low level. */
    kFLEXIO_ShifterStopBitHigh    = 0x3U, /*!< Set shifter stop bit to logic high level. */
} flexio_shifter_stop_bit_t;

/*! @brief Define type of START bit configuration.*/
typedef enum _flexio_shifter_start_bit
{
    kFLEXIO_ShifterStartBitDisabledLoadDataOnEnable = 0x0U, /*!< Disable shifter start bit, transmitter loads
                                                             data on enable. */
    kFLEXIO_ShifterStartBitDisabledLoadDataOnShift = 0x1U,  /*!< Disable shifter start bit, transmitter loads
                                                             data on first shift. */
    kFLEXIO_ShifterStartBitLow  = 0x2U,                     /*!< Set shifter start bit to logic low level. */
    kFLEXIO_ShifterStartBitHigh = 0x3U,                     /*!< Set shifter start bit to logic high level. */
} flexio_shifter_start_bit_t;

/*! @brief Define FlexIO shifter buffer type*/
typedef enum _flexio_shifter_buffer_type
{
    kFLEXIO_ShifterBuffer               = 0x0U, /*!< Shifter Buffer N Register. */
    kFLEXIO_ShifterBufferBitSwapped     = 0x1U, /*!< Shifter Buffer N Bit Byte Swapped Register. */
    kFLEXIO_ShifterBufferByteSwapped    = 0x2U, /*!< Shifter Buffer N Byte Swapped Register. */
    kFLEXIO_ShifterBufferBitByteSwapped = 0x3U, /*!< Shifter Buffer N Bit Swapped Register. */
#if defined(FSL_FEATURE_FLEXIO_HAS_SHFT_BUFFER_NIBBLE_BYTE_SWAP) && FSL_FEATURE_FLEXIO_HAS_SHFT_BUFFER_NIBBLE_BYTE_SWAP
    kFLEXIO_ShifterBufferNibbleByteSwapped = 0x4U, /*!< Shifter Buffer N Nibble Byte Swapped Register. */
#endif                                             /*FSL_FEATURE_FLEXIO_HAS_SHFT_BUFFER_NIBBLE_BYTE_SWAP*/
#if defined(FSL_FEATURE_FLEXIO_HAS_SHFT_BUFFER_HALF_WORD_SWAP) && FSL_FEATURE_FLEXIO_HAS_SHFT_BUFFER_HALF_WORD_SWAP
    kFLEXIO_ShifterBufferHalfWordSwapped = 0x5U, /*!< Shifter Buffer N Half Word Swapped Register. */
#endif
#if defined(FSL_FEATURE_FLEXIO_HAS_SHFT_BUFFER_NIBBLE_SWAP) && FSL_FEATURE_FLEXIO_HAS_SHFT_BUFFER_NIBBLE_SWAP
    kFLEXIO_ShifterBufferNibbleSwapped = 0x6U, /*!< Shifter Buffer N Nibble Swapped Register. */
#endif
} flexio_shifter_buffer_type_t;

/*! @brief Define FlexIO user configuration structure. */
typedef struct _flexio_config_
{
    bool enableFlexio;     /*!< Enable/disable FlexIO module */
    bool enableInDoze;     /*!< Enable/disable FlexIO operation in doze mode */
    bool enableInDebug;    /*!< Enable/disable FlexIO operation in debug mode */
    bool enableFastAccess; /*!< Enable/disable fast access to FlexIO registers, fast access requires
                           the FlexIO clock to be at least twice the frequency of the bus clock. */
} flexio_config_t;

/*! @brief Define FlexIO timer configuration structure. */
typedef struct _flexio_timer_config
{
    /* Trigger. */
    uint32_t triggerSelect;                          /*!< The internal trigger selection number using MACROs. */
    flexio_timer_trigger_polarity_t triggerPolarity; /*!< Trigger Polarity. */
    flexio_timer_trigger_source_t triggerSource;     /*!< Trigger Source, internal (see 'trgsel') or external. */
    /* Pin. */
    flexio_pin_config_t pinConfig;     /*!< Timer Pin Configuration. */
    uint32_t pinSelect;                /*!< Timer Pin number Select. */
    flexio_pin_polarity_t pinPolarity; /*!< Timer Pin Polarity. */
    /* Timer. */
    flexio_timer_mode_t timerMode;                  /*!< Timer work Mode. */
    flexio_timer_output_t timerOutput;              /*!< Configures the initial state of the Timer Output and
                                                    whether it is affected by the Timer reset. */
    flexio_timer_decrement_source_t timerDecrement; /*!< Configures the source of the Timer decrement and the
                                                    source of the Shift clock. */
    flexio_timer_reset_condition_t timerReset;      /*!< Configures the condition that causes the timer counter
                                                    (and optionally the timer output) to be reset. */
    flexio_timer_disable_condition_t timerDisable;  /*!< Configures the condition that causes the Timer to be
                                                    disabled and stop decrementing. */
    flexio_timer_enable_condition_t timerEnable;    /*!< Configures the condition that causes the Timer to be
                                                    enabled and start decrementing. */
    flexio_timer_stop_bit_condition_t timerStop;    /*!< Timer STOP Bit generation. */
    flexio_timer_start_bit_condition_t timerStart;  /*!< Timer STRAT Bit generation. */
    uint32_t timerCompare;                          /*!< Value for Timer Compare N Register. */
} flexio_timer_config_t;

/*! @brief Define FlexIO shifter configuration structure. */
typedef struct _flexio_shifter_config
{
    /* Timer. */
    uint32_t timerSelect;                          /*!< Selects which Timer is used for controlling the
                                                    logic/shift register and generating the Shift clock. */
    flexio_shifter_timer_polarity_t timerPolarity; /*!< Timer Polarity. */
    /* Pin. */
    flexio_pin_config_t pinConfig;     /*!< Shifter Pin Configuration. */
    uint32_t pinSelect;                /*!< Shifter Pin number Select. */
    flexio_pin_polarity_t pinPolarity; /*!< Shifter Pin Polarity. */
    /* Shifter. */
    flexio_shifter_mode_t shifterMode; /*!< Configures the mode of the Shifter. */
#if FSL_FEATURE_FLEXIO_HAS_PARALLEL_WIDTH
    uint32_t parallelWidth;                    /*!< Configures the parallel width when using parallel mode.*/
#endif                                         /* FSL_FEATURE_FLEXIO_HAS_PARALLEL_WIDTH */
    flexio_shifter_input_source_t inputSource; /*!< Selects the input source for the shifter. */
    flexio_shifter_stop_bit_t shifterStop;     /*!< Shifter STOP bit. */
    flexio_shifter_start_bit_t shifterStart;   /*!< Shifter START bit. */
} flexio_shifter_config_t;

/*! @brief typedef for FlexIO simulated driver interrupt handler.*/
typedef void (*flexio_isr_t)(void *base, void *handle);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief Pointers to flexio bases for each instance. */
extern FLEXIO_Type *const s_flexioBases[];

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief Pointers to flexio clocks for each instance. */
extern const clock_ip_name_t s_flexioClocks[];
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /*_cplusplus*/

/*!
 * @name FlexIO Initialization and De-initialization
 * @{
 */

/*!
 * @brief Gets the default configuration to configure the FlexIO module. The configuration
 * can used directly to call the FLEXIO_Configure().
 *
 * Example:
   @code
   flexio_config_t config;
   FLEXIO_GetDefaultConfig(&config);
   @endcode
 *
 * @param userConfig pointer to flexio_config_t structure
*/
void FLEXIO_GetDefaultConfig(flexio_config_t *userConfig);

/*!
 * @brief Configures the FlexIO with a FlexIO configuration. The configuration structure
 * can be filled by the user or be set with default values by FLEXIO_GetDefaultConfig().
 *
 * Example
   @code
   flexio_config_t config = {
   .enableFlexio = true,
   .enableInDoze = false,
   .enableInDebug = true,
   .enableFastAccess = false
   };
   FLEXIO_Configure(base, &config);
   @endcode
 *
 * @param base FlexIO peripheral base address
 * @param userConfig pointer to flexio_config_t structure
*/
void FLEXIO_Init(FLEXIO_Type *base, const flexio_config_t *userConfig);

/*!
 * @brief Gates the FlexIO clock. Call this API to stop the FlexIO clock.
 *
 * @note After calling this API, call the FLEXO_Init to use the FlexIO module.
 *
 * @param base FlexIO peripheral base address
 */
void FLEXIO_Deinit(FLEXIO_Type *base);

/*!
 * @brief Get instance number for FLEXIO module.
 *
 * @param base FLEXIO peripheral base address.
 */
uint32_t FLEXIO_GetInstance(FLEXIO_Type *base);

/* @} */

/*!
 * @name FlexIO Basic Operation
 * @{
 */

/*!
 * @brief Resets the FlexIO module.
 *
 * @param base FlexIO peripheral base address
 */
void FLEXIO_Reset(FLEXIO_Type *base);

/*!
 * @brief Enables the FlexIO module operation.
 *
 * @param base FlexIO peripheral base address
 * @param enable true to enable, false to disable.
 */
static inline void FLEXIO_Enable(FLEXIO_Type *base, bool enable)
{
    if (enable)
    {
        base->CTRL |= FLEXIO_CTRL_FLEXEN_MASK;
    }
    else
    {
        base->CTRL &= ~FLEXIO_CTRL_FLEXEN_MASK;
    }
}

#if defined(FSL_FEATURE_FLEXIO_HAS_PIN_STATUS) && FSL_FEATURE_FLEXIO_HAS_PIN_STATUS
/*!
 * @brief Reads the input data on each of the FlexIO pins.
 *
 * @param base FlexIO peripheral base address
 * @return FlexIO pin input data
 */
static inline uint32_t FLEXIO_ReadPinInput(FLEXIO_Type *base)
{
    return base->PIN;
}
#endif /*FSL_FEATURE_FLEXIO_HAS_PIN_STATUS*/

#if defined(FSL_FEATURE_FLEXIO_HAS_STATE_MODE) && FSL_FEATURE_FLEXIO_HAS_STATE_MODE
/*!
 * @brief Gets the current state pointer for state mode use.
 *
 * @param base FlexIO peripheral base address
 * @return current State pointer
 */
static inline uint8_t FLEXIO_GetShifterState(FLEXIO_Type *base)
{
    return ((base->SHIFTSTATE) & FLEXIO_SHIFTSTATE_STATE_MASK);
}
#endif /*FSL_FEATURE_FLEXIO_HAS_STATE_MODE*/

/*!
 * @brief Configures the shifter with the shifter configuration. The configuration structure
 * covers both the SHIFTCTL and SHIFTCFG registers. To configure the shifter to the proper
 * mode, select which timer controls the shifter to shift, whether to generate start bit/stop
 *  bit, and the polarity of start bit and stop bit.
 *
 * Example
   @code
   flexio_shifter_config_t config = {
   .timerSelect = 0,
   .timerPolarity = kFLEXIO_ShifterTimerPolarityOnPositive,
   .pinConfig = kFLEXIO_PinConfigOpenDrainOrBidirection,
   .pinPolarity = kFLEXIO_PinActiveLow,
   .shifterMode = kFLEXIO_ShifterModeTransmit,
   .inputSource = kFLEXIO_ShifterInputFromPin,
   .shifterStop = kFLEXIO_ShifterStopBitHigh,
   .shifterStart = kFLEXIO_ShifterStartBitLow
   };
   FLEXIO_SetShifterConfig(base, &config);
   @endcode
 *
 * @param base FlexIO peripheral base address
 * @param index Shifter index
 * @param shifterConfig Pointer to flexio_shifter_config_t structure
*/
void FLEXIO_SetShifterConfig(FLEXIO_Type *base, uint8_t index, const flexio_shifter_config_t *shifterConfig);
/*!
 * @brief Configures the timer with the timer configuration. The configuration structure
 * covers both the TIMCTL and TIMCFG registers. To configure the timer to the proper
 * mode, select trigger source for timer and the timer pin output and the timing for timer.
 *
 * Example
   @code
   flexio_timer_config_t config = {
   .triggerSelect = FLEXIO_TIMER_TRIGGER_SEL_SHIFTnSTAT(0),
   .triggerPolarity = kFLEXIO_TimerTriggerPolarityActiveLow,
   .triggerSource = kFLEXIO_TimerTriggerSourceInternal,
   .pinConfig = kFLEXIO_PinConfigOpenDrainOrBidirection,
   .pinSelect = 0,
   .pinPolarity = kFLEXIO_PinActiveHigh,
   .timerMode = kFLEXIO_TimerModeDual8BitBaudBit,
   .timerOutput = kFLEXIO_TimerOutputZeroNotAffectedByReset,
   .timerDecrement = kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput,
   .timerReset = kFLEXIO_TimerResetOnTimerPinEqualToTimerOutput,
   .timerDisable = kFLEXIO_TimerDisableOnTimerCompare,
   .timerEnable = kFLEXIO_TimerEnableOnTriggerHigh,
   .timerStop = kFLEXIO_TimerStopBitEnableOnTimerDisable,
   .timerStart = kFLEXIO_TimerStartBitEnabled
   };
   FLEXIO_SetTimerConfig(base, &config);
   @endcode
 *
 * @param base FlexIO peripheral base address
 * @param index Timer index
 * @param timerConfig Pointer to the flexio_timer_config_t structure
*/
void FLEXIO_SetTimerConfig(FLEXIO_Type *base, uint8_t index, const flexio_timer_config_t *timerConfig);

/* @} */

/*!
 * @name FlexIO Interrupt Operation
 * @{
 */

/*!
 * @brief Enables the shifter status interrupt. The interrupt generates when the corresponding SSF is set.
 *
 * @param base FlexIO peripheral base address
 * @param mask The shifter status mask which can be calculated by (1 << shifter index)
 * @note For multiple shifter status interrupt enable, for example, two shifter status enable, can calculate
 * the mask by using ((1 << shifter index0) | (1 << shifter index1))
 */
static inline void FLEXIO_EnableShifterStatusInterrupts(FLEXIO_Type *base, uint32_t mask)
{
    base->SHIFTSIEN |= mask;
}

/*!
 * @brief Disables the shifter status interrupt. The interrupt won't generate when the corresponding SSF is set.
 *
 * @param base FlexIO peripheral base address
 * @param mask The shifter status mask which can be calculated by (1 << shifter index)
 * @note For multiple shifter status interrupt enable, for example, two shifter status enable, can calculate
 * the mask by using ((1 << shifter index0) | (1 << shifter index1))
 */
static inline void FLEXIO_DisableShifterStatusInterrupts(FLEXIO_Type *base, uint32_t mask)
{
    base->SHIFTSIEN &= ~mask;
}

/*!
 * @brief Enables the shifter error interrupt. The interrupt generates when the corresponding SEF is set.
 *
 * @param base FlexIO peripheral base address
 * @param mask The shifter error mask which can be calculated by (1 << shifter index)
 * @note For multiple shifter error interrupt enable, for example, two shifter error enable, can calculate
 * the mask by using ((1 << shifter index0) | (1 << shifter index1))
 */
static inline void FLEXIO_EnableShifterErrorInterrupts(FLEXIO_Type *base, uint32_t mask)
{
    base->SHIFTEIEN |= mask;
}

/*!
 * @brief Disables the shifter error interrupt. The interrupt won't generate when the corresponding SEF is set.
 *
 * @param base FlexIO peripheral base address
 * @param mask The shifter error mask which can be calculated by (1 << shifter index)
 * @note For multiple shifter error interrupt enable, for example, two shifter error enable, can calculate
 * the mask by using ((1 << shifter index0) | (1 << shifter index1))
 */
static inline void FLEXIO_DisableShifterErrorInterrupts(FLEXIO_Type *base, uint32_t mask)
{
    base->SHIFTEIEN &= ~mask;
}

/*!
 * @brief Enables the timer status interrupt. The interrupt generates when the corresponding SSF is set.
 *
 * @param base FlexIO peripheral base address
 * @param mask The timer status mask which can be calculated by (1 << timer index)
 * @note For multiple timer status interrupt enable, for example, two timer status enable, can calculate
 * the mask by using ((1 << timer index0) | (1 << timer index1))
 */
static inline void FLEXIO_EnableTimerStatusInterrupts(FLEXIO_Type *base, uint32_t mask)
{
    base->TIMIEN |= mask;
}

/*!
 * @brief Disables the timer status interrupt. The interrupt won't generate when the corresponding SSF is set.
 *
 * @param base FlexIO peripheral base address
 * @param mask The timer status mask which can be calculated by (1 << timer index)
 * @note For multiple timer status interrupt enable, for example, two timer status enable, can calculate
 * the mask by using ((1 << timer index0) | (1 << timer index1))
 */
static inline void FLEXIO_DisableTimerStatusInterrupts(FLEXIO_Type *base, uint32_t mask)
{
    base->TIMIEN &= ~mask;
}

/* @} */

/*!
 * @name FlexIO Status Operation
 * @{
 */

/*!
 * @brief Gets the shifter status flags.
 *
 * @param base FlexIO peripheral base address
 * @return Shifter status flags
 */
static inline uint32_t FLEXIO_GetShifterStatusFlags(FLEXIO_Type *base)
{
    return ((base->SHIFTSTAT) & FLEXIO_SHIFTSTAT_SSF_MASK);
}

/*!
 * @brief Clears the shifter status flags.
 *
 * @param base FlexIO peripheral base address
 * @param mask The shifter status mask which can be calculated by (1 << shifter index)
 * @note For clearing multiple shifter status flags, for example, two shifter status flags, can calculate
 * the mask by using ((1 << shifter index0) | (1 << shifter index1))
 */
static inline void FLEXIO_ClearShifterStatusFlags(FLEXIO_Type *base, uint32_t mask)
{
    base->SHIFTSTAT = mask;
}

/*!
 * @brief Gets the shifter error flags.
 *
 * @param base FlexIO peripheral base address
 * @return Shifter error flags
 */
static inline uint32_t FLEXIO_GetShifterErrorFlags(FLEXIO_Type *base)
{
    return ((base->SHIFTERR) & FLEXIO_SHIFTERR_SEF_MASK);
}

/*!
 * @brief Clears the shifter error flags.
 *
 * @param base FlexIO peripheral base address
 * @param mask The shifter error mask which can be calculated by (1 << shifter index)
 * @note For clearing multiple shifter error flags, for example, two shifter error flags, can calculate
 * the mask by using ((1 << shifter index0) | (1 << shifter index1))
 */
static inline void FLEXIO_ClearShifterErrorFlags(FLEXIO_Type *base, uint32_t mask)
{
    base->SHIFTERR = mask;
}

/*!
 * @brief Gets the timer status flags.
 *
 * @param base FlexIO peripheral base address
 * @return Timer status flags
 */
static inline uint32_t FLEXIO_GetTimerStatusFlags(FLEXIO_Type *base)
{
    return ((base->TIMSTAT) & FLEXIO_TIMSTAT_TSF_MASK);
}

/*!
 * @brief Clears the timer status flags.
 *
 * @param base FlexIO peripheral base address
 * @param mask The timer status mask which can be calculated by (1 << timer index)
 * @note For clearing multiple timer status flags, for example, two timer status flags, can calculate
 * the mask by using ((1 << timer index0) | (1 << timer index1))
 */
static inline void FLEXIO_ClearTimerStatusFlags(FLEXIO_Type *base, uint32_t mask)
{
    base->TIMSTAT = mask;
}

/* @} */

/*!
 * @name FlexIO DMA Operation
 * @{
 */

/*!
 * @brief Enables/disables the shifter status DMA. The DMA request  generates when the corresponding SSF is set.
 *
 * @note For multiple shifter status DMA enables, for example, calculate
 * the mask by using ((1 << shifter index0) | (1 << shifter index1))
 *
 * @param base FlexIO peripheral base address
 * @param mask The shifter status mask which can be calculated by (1 << shifter index)
 * @param enable True to enable, false to disable.
 */
static inline void FLEXIO_EnableShifterStatusDMA(FLEXIO_Type *base, uint32_t mask, bool enable)
{
    if (enable)
    {
        base->SHIFTSDEN |= mask;
    }
    else
    {
        base->SHIFTSDEN &= ~mask;
    }
}

/*!
 * @brief Gets the shifter buffer address for the DMA transfer usage.
 *
 * @param base FlexIO peripheral base address
 * @param type Shifter type of flexio_shifter_buffer_type_t
 * @param index Shifter index
 * @return Corresponding shifter buffer index
 */
uint32_t FLEXIO_GetShifterBufferAddress(FLEXIO_Type *base, flexio_shifter_buffer_type_t type, uint8_t index);

/*!
 * @brief Registers the handle and the interrupt handler for the FlexIO-simulated peripheral.
 *
 * @param base Pointer to the FlexIO simulated peripheral type.
 * @param handle Pointer to the handler for FlexIO simulated peripheral.
 * @param isr FlexIO simulated peripheral interrupt handler.
 * @retval kStatus_Success Successfully create the handle.
 * @retval kStatus_OutOfRange The FlexIO type/handle/ISR table out of range.
 */
status_t FLEXIO_RegisterHandleIRQ(void *base, void *handle, flexio_isr_t isr);

/*!
 * @brief Unregisters the handle and the interrupt handler for the FlexIO-simulated peripheral.
 *
 * @param base Pointer to the FlexIO simulated peripheral type.
 * @retval kStatus_Success Successfully create the handle.
 * @retval kStatus_OutOfRange The FlexIO type/handle/ISR table out of range.
 */
status_t FLEXIO_UnregisterHandleIRQ(void *base);
/* @} */

#if defined(__cplusplus)
}
#endif /*_cplusplus*/
/*@}*/

#endif /*_FSL_FLEXIO_H_*/
