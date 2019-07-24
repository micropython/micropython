/*
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_QTMR_H_
#define _FSL_QTMR_H_

#include "fsl_common.h"

/*!
 * @addtogroup qtmr
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
#define FSL_QTMR_DRIVER_VERSION (MAKE_VERSION(2, 0, 0)) /*!< Version 2.0.0 */
/*@}*/

/*! @brief Quad Timer primary clock source selection*/
typedef enum _qtmr_primary_count_source
{
    kQTMR_ClockCounter0InputPin = 0, /*!< Use counter 0 input pin */
    kQTMR_ClockCounter1InputPin,     /*!< Use counter 1 input pin */
    kQTMR_ClockCounter2InputPin,     /*!< Use counter 2 input pin */
    kQTMR_ClockCounter3InputPin,     /*!< Use counter 3 input pin */
    kQTMR_ClockCounter0Output,       /*!< Use counter 0 output */
    kQTMR_ClockCounter1Output,       /*!< Use counter 1 output */
    kQTMR_ClockCounter2Output,       /*!< Use counter 2 output */
    kQTMR_ClockCounter3Output,       /*!< Use counter 3 output */
    kQTMR_ClockDivide_1,             /*!< IP bus clock divide by 1 prescaler */
    kQTMR_ClockDivide_2,             /*!< IP bus clock divide by 2 prescaler */
    kQTMR_ClockDivide_4,             /*!< IP bus clock divide by 4 prescaler */
    kQTMR_ClockDivide_8,             /*!< IP bus clock divide by 8 prescaler */
    kQTMR_ClockDivide_16,            /*!< IP bus clock divide by 16 prescaler */
    kQTMR_ClockDivide_32,            /*!< IP bus clock divide by 32 prescaler */
    kQTMR_ClockDivide_64,            /*!< IP bus clock divide by 64 prescaler */
    kQTMR_ClockDivide_128            /*!< IP bus clock divide by 128 prescaler */
} qtmr_primary_count_source_t;

/*! @brief Quad Timer input sources selection*/
typedef enum _qtmr_input_source
{
    kQTMR_Counter0InputPin = 0, /*!< Use counter 0 input pin */
    kQTMR_Counter1InputPin,     /*!< Use counter 1 input pin */
    kQTMR_Counter2InputPin,     /*!< Use counter 2 input pin */
    kQTMR_Counter3InputPin      /*!< Use counter 3 input pin */
} qtmr_input_source_t;

/*! @brief Quad Timer counting mode selection */
typedef enum _qtmr_counting_mode
{
    kQTMR_NoOperation = 0,          /*!< No operation */
    kQTMR_PriSrcRiseEdge,           /*!< Count rising edges or primary source */
    kQTMR_PriSrcRiseAndFallEdge,    /*!< Count rising and falling edges of primary source */
    kQTMR_PriSrcRiseEdgeSecInpHigh, /*!< Count rise edges of pri SRC while sec inp high active */
    kQTMR_QuadCountMode,            /*!< Quadrature count mode, uses pri and sec sources */
    kQTMR_PriSrcRiseEdgeSecDir,     /*!< Count rising edges of pri SRC; sec SRC specifies dir */
    kQTMR_SecSrcTrigPriCnt,         /*!< Edge of sec SRC trigger primary count until compare*/
    kQTMR_CascadeCount              /*!< Cascaded count mode (up/down) */
} qtmr_counting_mode_t;

/*! @brief Quad Timer output mode selection*/
typedef enum _qtmr_output_mode
{
    kQTMR_AssertWhenCountActive = 0,    /*!< Assert OFLAG while counter is active*/
    kQTMR_ClearOnCompare,               /*!< Clear OFLAG on successful compare */
    kQTMR_SetOnCompare,                 /*!< Set OFLAG on successful compare */
    kQTMR_ToggleOnCompare,              /*!< Toggle OFLAG on successful compare */
    kQTMR_ToggleOnAltCompareReg,        /*!< Toggle OFLAG using alternating compare registers */
    kQTMR_SetOnCompareClearOnSecSrcInp, /*!< Set OFLAG on compare, clear on sec SRC input edge */
    kQTMR_SetOnCompareClearOnCountRoll, /*!< Set OFLAG on compare, clear on counter rollover */
    kQTMR_EnableGateClock               /*!< Enable gated clock output while count is active */
} qtmr_output_mode_t;

/*! @brief Quad Timer input capture edge mode, rising edge, or falling edge */
typedef enum _qtmr_input_capture_edge
{
    kQTMR_NoCapture = 0,       /*!< Capture is disabled */
    kQTMR_RisingEdge,          /*!< Capture on rising edge (IPS=0) or falling edge (IPS=1)*/
    kQTMR_FallingEdge,         /*!< Capture on falling edge (IPS=0) or rising edge (IPS=1)*/
    kQTMR_RisingAndFallingEdge /*!< Capture on both edges */
} qtmr_input_capture_edge_t;

/*! @brief Quad Timer input capture edge mode, rising edge, or falling edge */
typedef enum _qtmr_preload_control
{
    kQTMR_NoPreload = 0, /*!< Never preload */
    kQTMR_LoadOnComp1,   /*!< Load upon successful compare with value in COMP1 */
    kQTMR_LoadOnComp2    /*!< Load upon successful compare with value in COMP2*/
} qtmr_preload_control_t;

/*! @brief List of Quad Timer run options when in Debug mode */
typedef enum _qtmr_debug_action
{
    kQTMR_RunNormalInDebug = 0U, /*!< Continue with normal operation */
    kQTMR_HaltCounter,           /*!< Halt counter */
    kQTMR_ForceOutToZero,        /*!< Force output to logic 0 */
    kQTMR_HaltCountForceOutZero  /*!< Halt counter and force output to logic 0 */
} qtmr_debug_action_t;

/*! @brief List of Quad Timer interrupts */
typedef enum _qtmr_interrupt_enable
{
    kQTMR_CompareInterruptEnable  = (1U << 0), /*!< Compare interrupt.*/
    kQTMR_Compare1InterruptEnable = (1U << 1), /*!< Compare 1 interrupt.*/
    kQTMR_Compare2InterruptEnable = (1U << 2), /*!< Compare 2 interrupt.*/
    kQTMR_OverflowInterruptEnable = (1U << 3), /*!< Timer overflow interrupt.*/
    kQTMR_EdgeInterruptEnable     = (1U << 4)  /*!< Input edge interrupt.*/
} qtmr_interrupt_enable_t;

/*! @brief List of Quad Timer flags */
typedef enum _qtmr_status_flags
{
    kQTMR_CompareFlag  = (1U << 0), /*!< Compare flag */
    kQTMR_Compare1Flag = (1U << 1), /*!< Compare 1 flag */
    kQTMR_Compare2Flag = (1U << 2), /*!< Compare 2 flag */
    kQTMR_OverflowFlag = (1U << 3), /*!< Timer overflow flag */
    kQTMR_EdgeFlag     = (1U << 4)  /*!< Input edge flag */
} qtmr_status_flags_t;

/*! @brief List of channel selection */
typedef enum _qtmr_channel_selection
{
    kQTMR_Channel_0 = 0U, /*!< TMR Channel 0 */
    kQTMR_Channel_1,      /*!< TMR Channel 1 */
    kQTMR_Channel_2,      /*!< TMR Channel 2 */
    kQTMR_Channel_3,      /*!< TMR Channel 3 */
} qtmr_channel_selection_t;

/*! @brief List of Quad Timer DMA enable */
typedef enum _qtmr_dma_enable
{
    kQTMR_InputEdgeFlagDmaEnable      = (1U << 0), /*!< Input Edge Flag DMA Enable.*/
    kQTMR_ComparatorPreload1DmaEnable = (1U << 1), /*!< Comparator Preload Register 1 DMA Enable.*/
    kQTMR_ComparatorPreload2DmaEnable = (1U << 2), /*!< Comparator Preload Register 2 DMA Enable.*/
} qtmr_dma_enable_t;

/*!
 * @brief Quad Timer config structure
 *
 * This structure holds the configuration settings for the Quad Timer peripheral. To initialize this
 * structure to reasonable defaults, call the QTMR_GetDefaultConfig() function and pass a
 * pointer to your config structure instance.
 *
 * The config struct can be made const so it resides in flash
 */
typedef struct _qtmr_config
{
    qtmr_primary_count_source_t primarySource; /*!< Specify the primary count source */
    qtmr_input_source_t secondarySource;       /*!< Specify the secondary count source */
    bool enableMasterMode;                     /*!< true: Broadcast compare function output to other counters;
                                                    false no broadcast */
    bool enableExternalForce;                  /*!< true: Compare from another counter force state of OFLAG signal
                                                    false: OFLAG controlled by local counter */
    uint8_t faultFilterCount;                  /*!< Fault filter count */
    uint8_t faultFilterPeriod;                 /*!< Fault filter period;value of 0 will bypass the filter */
    qtmr_debug_action_t debugMode;             /*!< Operation in Debug mode */
} qtmr_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief Ungates the Quad Timer clock and configures the peripheral for basic operation.
 *
 * @note This API should be called at the beginning of the application using the Quad Timer driver.
 *
 * @param base     Quad Timer peripheral base address
 * @param channel  Quad Timer channel number
 * @param config   Pointer to user's Quad Timer config structure
 */
void QTMR_Init(TMR_Type *base, qtmr_channel_selection_t channel, const qtmr_config_t *config);

/*!
 * @brief Stops the counter and gates the Quad Timer clock
 *
 * @param base     Quad Timer peripheral base address
 * @param channel  Quad Timer channel number
 */
void QTMR_Deinit(TMR_Type *base, qtmr_channel_selection_t channel);

/*!
 * @brief  Fill in the Quad Timer config struct with the default settings
 *
 * The default values are:
 * @code
 *    config->debugMode = kQTMR_RunNormalInDebug;
 *    config->enableExternalForce = false;
 *    config->enableMasterMode = false;
 *    config->faultFilterCount = 0;
 *    config->faultFilterPeriod = 0;
 *    config->primarySource = kQTMR_ClockDivide_2;
 *    config->secondarySource = kQTMR_Counter0InputPin;
 * @endcode
 * @param config Pointer to user's Quad Timer config structure.
 */
void QTMR_GetDefaultConfig(qtmr_config_t *config);

/*! @}*/

/*!
 * @brief Sets up Quad timer module for PWM signal output.
 *
 * The function initializes the timer module according to the parameters passed in by the user. The
 * function also sets up the value compare registers to match the PWM signal requirements.
 *
 * @param base             Quad Timer peripheral base address
 * @param channel          Quad Timer channel number
 * @param pwmFreqHz        PWM signal frequency in Hz
 * @param dutyCyclePercent PWM pulse width, value should be between 0 to 100
 *                         0=inactive signal(0% duty cycle)...
 *                         100=active signal (100% duty cycle)
 * @param outputPolarity   true: invert polarity of the output signal, false: no inversion
 * @param srcClock_Hz      Main counter clock in Hz.
 *
 * @return Returns an error if there was error setting up the signal.
 */
status_t QTMR_SetupPwm(TMR_Type *base,
                       qtmr_channel_selection_t channel,
                       uint32_t pwmFreqHz,
                       uint8_t dutyCyclePercent,
                       bool outputPolarity,
                       uint32_t srcClock_Hz);

/*!
 * @brief Allows the user to count the source clock cycles until a capture event arrives.
 *
 * The count is stored in the capture register.
 *
 * @param base            Quad Timer peripheral base address
 * @param channel         Quad Timer channel number
 * @param capturePin      Pin through which we receive the input signal to trigger the capture
 * @param inputPolarity   true: invert polarity of the input signal, false: no inversion
 * @param reloadOnCapture true: reload the counter when an input capture occurs, false: no reload
 * @param captureMode     Specifies which edge of the input signal  triggers a capture
 */
void QTMR_SetupInputCapture(TMR_Type *base,
                            qtmr_channel_selection_t channel,
                            qtmr_input_source_t capturePin,
                            bool inputPolarity,
                            bool reloadOnCapture,
                            qtmr_input_capture_edge_t captureMode);

/*!
 * @name Interrupt Interface
 * @{
 */

/*!
 * @brief Enables the selected Quad Timer interrupts
 *
 * @param base      Quad Timer peripheral base address
 * @param channel   Quad Timer channel number
 * @param mask      The interrupts to enable. This is a logical OR of members of the
 *                  enumeration ::qtmr_interrupt_enable_t
 */
void QTMR_EnableInterrupts(TMR_Type *base, qtmr_channel_selection_t channel, uint32_t mask);

/*!
 * @brief Disables the selected Quad Timer interrupts
 *
 * @param base     Quad Timer peripheral base addres
 * @param channel  Quad Timer channel number
 * @param mask The interrupts to enable. This is a logical OR of members of the
 *             enumeration ::qtmr_interrupt_enable_t
 */
void QTMR_DisableInterrupts(TMR_Type *base, qtmr_channel_selection_t channel, uint32_t mask);

/*!
 * @brief Gets the enabled Quad Timer interrupts
 *
 * @param base    Quad Timer peripheral base address
 * @param channel Quad Timer channel number
 *
 * @return The enabled interrupts. This is the logical OR of members of the
 *         enumeration ::qtmr_interrupt_enable_t
 */
uint32_t QTMR_GetEnabledInterrupts(TMR_Type *base, qtmr_channel_selection_t channel);

/*! @}*/

/*!
 * @name Status Interface
 * @{
 */

/*!
 * @brief Gets the Quad Timer status flags
 *
 * @param base     Quad Timer peripheral base address
 * @param channel  Quad Timer channel number
 *
 * @return The status flags. This is the logical OR of members of the
 *         enumeration ::qtmr_status_flags_t
 */
uint32_t QTMR_GetStatus(TMR_Type *base, qtmr_channel_selection_t channel);

/*!
 * @brief Clears the Quad Timer status flags.
 *
 * @param base     Quad Timer peripheral base address
 * @param channel  Quad Timer channel number
 * @param mask The status flags to clear. This is a logical OR of members of the
 *             enumeration ::qtmr_status_flags_t
 */
void QTMR_ClearStatusFlags(TMR_Type *base, qtmr_channel_selection_t channel, uint32_t mask);

/*! @}*/

/*!
 * @name Read and Write the timer period
 * @{
 */

/*!
 * @brief Sets the timer period in ticks.
 *
 * Timers counts from initial value till it equals the count value set here. The counter
 * will then reinitialize to the value specified in the Load register.
 *
 * @note
 * 1. This function will write the time period in ticks to COMP1 or COMP2 register
 *    depending on the count direction
 * 2. User can call the utility macros provided in fsl_common.h to convert to ticks
 * 3. This function supports cases, providing only primary source clock without secondary source clock.
 *
 * @param base     Quad Timer peripheral base address
 * @param channel  Quad Timer channel number
 * @param ticks Timer period in units of ticks
 */
void QTMR_SetTimerPeriod(TMR_Type *base, qtmr_channel_selection_t channel, uint16_t ticks);

/*!
 * @brief Reads the current timer counting value.
 *
 * This function returns the real-time timer counting value, in a range from 0 to a
 * timer period.
 *
 * @note User can call the utility macros provided in fsl_common.h to convert ticks to usec or msec
 *
 * @param base     Quad Timer peripheral base address
 * @param channel  Quad Timer channel number
 *
 * @return Current counter value in ticks
 */
static inline uint16_t QTMR_GetCurrentTimerCount(TMR_Type *base, qtmr_channel_selection_t channel)
{
    return base->CHANNEL[channel].CNTR;
}

/*! @}*/

/*!
 * @name Timer Start and Stop
 * @{
 */

/*!
 * @brief Starts the Quad Timer counter.
 *
 *
 * @param base        Quad Timer peripheral base address
 * @param channel     Quad Timer channel number
 * @param clockSource Quad Timer clock source
 */
static inline void QTMR_StartTimer(TMR_Type *base, qtmr_channel_selection_t channel, qtmr_counting_mode_t clockSource)
{
    uint16_t reg = base->CHANNEL[channel].CTRL;

    reg &= ~(TMR_CTRL_CM_MASK);
    reg |= TMR_CTRL_CM(clockSource);
    base->CHANNEL[channel].CTRL = reg;
}

/*!
 * @brief Stops the Quad Timer counter.
 *
 * @param base     Quad Timer peripheral base address
 * @param channel  Quad Timer channel number
 */
static inline void QTMR_StopTimer(TMR_Type *base, qtmr_channel_selection_t channel)
{
    base->CHANNEL[channel].CTRL &= ~TMR_CTRL_CM_MASK;
}

/*! @}*/

/*!
 * @name Enable and Disable the Quad Timer DMA
 * @{
 */

/*!
 * @brief Enable the Quad Timer DMA.
 *
 * @param base     Quad Timer peripheral base address
 * @param channel  Quad Timer channel number
 * @param mask     The DMA to enable. This is a logical OR of members of the
 *                  enumeration ::qtmr_dma_enable_t
 */
void QTMR_EnableDma(TMR_Type *base, qtmr_channel_selection_t channel, uint32_t mask);

/*!
 * @brief Disable the Quad Timer DMA.
 *
 * @param base     Quad Timer peripheral base address
 * @param channel  Quad Timer channel number
 * @param mask     The DMA to enable. This is a logical OR of members of the
 *                  enumeration ::qtmr_dma_enable_t
 */
void QTMR_DisableDma(TMR_Type *base, qtmr_channel_selection_t channel, uint32_t mask);

/*! @}*/

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _FSL_QTMR_H_ */
