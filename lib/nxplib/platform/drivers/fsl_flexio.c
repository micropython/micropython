/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_flexio.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.flexio"
#endif

/*< @brief user configurable flexio handle count. */
#define FLEXIO_HANDLE_COUNT 2

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief Pointers to flexio bases for each instance. */
FLEXIO_Type *const s_flexioBases[] = FLEXIO_BASE_PTRS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief Pointers to flexio clocks for each instance. */
const clock_ip_name_t s_flexioClocks[] = FLEXIO_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*< @brief pointer to array of FLEXIO handle. */
static void *s_flexioHandle[FLEXIO_HANDLE_COUNT];

/*< @brief pointer to array of FLEXIO IP types. */
static void *s_flexioType[FLEXIO_HANDLE_COUNT];

/*< @brief pointer to array of FLEXIO Isr. */
static flexio_isr_t s_flexioIsr[FLEXIO_HANDLE_COUNT];

/*******************************************************************************
 * Codes
 ******************************************************************************/

/*!
 * brief Get instance number for FLEXIO module.
 *
 * param base FLEXIO peripheral base address.
 */
uint32_t FLEXIO_GetInstance(FLEXIO_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_flexioBases); instance++)
    {
        if (s_flexioBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_flexioBases));

    return instance;
}

/*!
 * brief Configures the FlexIO with a FlexIO configuration. The configuration structure
 * can be filled by the user or be set with default values by FLEXIO_GetDefaultConfig().
 *
 * Example
   code
   flexio_config_t config = {
   .enableFlexio = true,
   .enableInDoze = false,
   .enableInDebug = true,
   .enableFastAccess = false
   };
   FLEXIO_Configure(base, &config);
   endcode
 *
 * param base FlexIO peripheral base address
 * param userConfig pointer to flexio_config_t structure
*/
void FLEXIO_Init(FLEXIO_Type *base, const flexio_config_t *userConfig)
{
    uint32_t ctrlReg = 0;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    CLOCK_EnableClock(s_flexioClocks[FLEXIO_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    FLEXIO_Reset(base);

    ctrlReg = base->CTRL;
    ctrlReg &= ~(FLEXIO_CTRL_DOZEN_MASK | FLEXIO_CTRL_DBGE_MASK | FLEXIO_CTRL_FASTACC_MASK | FLEXIO_CTRL_FLEXEN_MASK);
    ctrlReg |= (FLEXIO_CTRL_DBGE(userConfig->enableInDebug) | FLEXIO_CTRL_FASTACC(userConfig->enableFastAccess) |
                FLEXIO_CTRL_FLEXEN(userConfig->enableFlexio));
    if (!userConfig->enableInDoze)
    {
        ctrlReg |= FLEXIO_CTRL_DOZEN_MASK;
    }

    base->CTRL = ctrlReg;
}

/*!
 * brief Gates the FlexIO clock. Call this API to stop the FlexIO clock.
 *
 * note After calling this API, call the FLEXO_Init to use the FlexIO module.
 *
 * param base FlexIO peripheral base address
 */
void FLEXIO_Deinit(FLEXIO_Type *base)
{
    FLEXIO_Enable(base, false);
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    CLOCK_DisableClock(s_flexioClocks[FLEXIO_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief Gets the default configuration to configure the FlexIO module. The configuration
 * can used directly to call the FLEXIO_Configure().
 *
 * Example:
   code
   flexio_config_t config;
   FLEXIO_GetDefaultConfig(&config);
   endcode
 *
 * param userConfig pointer to flexio_config_t structure
*/
void FLEXIO_GetDefaultConfig(flexio_config_t *userConfig)
{
    assert(userConfig);

    /* Initializes the configure structure to zero. */
    memset(userConfig, 0, sizeof(*userConfig));

    userConfig->enableFlexio     = true;
    userConfig->enableInDoze     = false;
    userConfig->enableInDebug    = true;
    userConfig->enableFastAccess = false;
}

/*!
 * brief Resets the FlexIO module.
 *
 * param base FlexIO peripheral base address
 */
void FLEXIO_Reset(FLEXIO_Type *base)
{
    /*do software reset, software reset operation affect all other FLEXIO registers except CTRL*/
    base->CTRL |= FLEXIO_CTRL_SWRST_MASK;
    base->CTRL = 0;
}

/*!
 * brief Gets the shifter buffer address for the DMA transfer usage.
 *
 * param base FlexIO peripheral base address
 * param type Shifter type of flexio_shifter_buffer_type_t
 * param index Shifter index
 * return Corresponding shifter buffer index
 */
uint32_t FLEXIO_GetShifterBufferAddress(FLEXIO_Type *base, flexio_shifter_buffer_type_t type, uint8_t index)
{
    assert(index < FLEXIO_SHIFTBUF_COUNT);

    uint32_t address = 0;

    switch (type)
    {
        case kFLEXIO_ShifterBuffer:
            address = (uint32_t) & (base->SHIFTBUF[index]);
            break;

        case kFLEXIO_ShifterBufferBitSwapped:
            address = (uint32_t) & (base->SHIFTBUFBIS[index]);
            break;

        case kFLEXIO_ShifterBufferByteSwapped:
            address = (uint32_t) & (base->SHIFTBUFBYS[index]);
            break;

        case kFLEXIO_ShifterBufferBitByteSwapped:
            address = (uint32_t) & (base->SHIFTBUFBBS[index]);
            break;

#if defined(FSL_FEATURE_FLEXIO_HAS_SHFT_BUFFER_NIBBLE_BYTE_SWAP) && FSL_FEATURE_FLEXIO_HAS_SHFT_BUFFER_NIBBLE_BYTE_SWAP
        case kFLEXIO_ShifterBufferNibbleByteSwapped:
            address = (uint32_t) & (base->SHIFTBUFNBS[index]);
            break;

#endif
#if defined(FSL_FEATURE_FLEXIO_HAS_SHFT_BUFFER_HALF_WORD_SWAP) && FSL_FEATURE_FLEXIO_HAS_SHFT_BUFFER_HALF_WORD_SWAP
        case kFLEXIO_ShifterBufferHalfWordSwapped:
            address = (uint32_t) & (base->SHIFTBUFHWS[index]);
            break;

#endif
#if defined(FSL_FEATURE_FLEXIO_HAS_SHFT_BUFFER_NIBBLE_SWAP) && FSL_FEATURE_FLEXIO_HAS_SHFT_BUFFER_NIBBLE_SWAP
        case kFLEXIO_ShifterBufferNibbleSwapped:
            address = (uint32_t) & (base->SHIFTBUFNIS[index]);
            break;

#endif
        default:
            break;
    }
    return address;
}

/*!
 * brief Configures the shifter with the shifter configuration. The configuration structure
 * covers both the SHIFTCTL and SHIFTCFG registers. To configure the shifter to the proper
 * mode, select which timer controls the shifter to shift, whether to generate start bit/stop
 *  bit, and the polarity of start bit and stop bit.
 *
 * Example
   code
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
   endcode
 *
 * param base FlexIO peripheral base address
 * param index Shifter index
 * param shifterConfig Pointer to flexio_shifter_config_t structure
*/
void FLEXIO_SetShifterConfig(FLEXIO_Type *base, uint8_t index, const flexio_shifter_config_t *shifterConfig)
{
    base->SHIFTCFG[index] = FLEXIO_SHIFTCFG_INSRC(shifterConfig->inputSource)
#if FSL_FEATURE_FLEXIO_HAS_PARALLEL_WIDTH
                            | FLEXIO_SHIFTCFG_PWIDTH(shifterConfig->parallelWidth)
#endif /* FSL_FEATURE_FLEXIO_HAS_PARALLEL_WIDTH */
                            | FLEXIO_SHIFTCFG_SSTOP(shifterConfig->shifterStop) |
                            FLEXIO_SHIFTCFG_SSTART(shifterConfig->shifterStart);

    base->SHIFTCTL[index] =
        FLEXIO_SHIFTCTL_TIMSEL(shifterConfig->timerSelect) | FLEXIO_SHIFTCTL_TIMPOL(shifterConfig->timerPolarity) |
        FLEXIO_SHIFTCTL_PINCFG(shifterConfig->pinConfig) | FLEXIO_SHIFTCTL_PINSEL(shifterConfig->pinSelect) |
        FLEXIO_SHIFTCTL_PINPOL(shifterConfig->pinPolarity) | FLEXIO_SHIFTCTL_SMOD(shifterConfig->shifterMode);
}

/*!
 * brief Configures the timer with the timer configuration. The configuration structure
 * covers both the TIMCTL and TIMCFG registers. To configure the timer to the proper
 * mode, select trigger source for timer and the timer pin output and the timing for timer.
 *
 * Example
   code
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
   endcode
 *
 * param base FlexIO peripheral base address
 * param index Timer index
 * param timerConfig Pointer to the flexio_timer_config_t structure
*/
void FLEXIO_SetTimerConfig(FLEXIO_Type *base, uint8_t index, const flexio_timer_config_t *timerConfig)
{
    base->TIMCFG[index] =
        FLEXIO_TIMCFG_TIMOUT(timerConfig->timerOutput) | FLEXIO_TIMCFG_TIMDEC(timerConfig->timerDecrement) |
        FLEXIO_TIMCFG_TIMRST(timerConfig->timerReset) | FLEXIO_TIMCFG_TIMDIS(timerConfig->timerDisable) |
        FLEXIO_TIMCFG_TIMENA(timerConfig->timerEnable) | FLEXIO_TIMCFG_TSTOP(timerConfig->timerStop) |
        FLEXIO_TIMCFG_TSTART(timerConfig->timerStart);

    base->TIMCMP[index] = FLEXIO_TIMCMP_CMP(timerConfig->timerCompare);

    base->TIMCTL[index] = FLEXIO_TIMCTL_TRGSEL(timerConfig->triggerSelect) |
                          FLEXIO_TIMCTL_TRGPOL(timerConfig->triggerPolarity) |
                          FLEXIO_TIMCTL_TRGSRC(timerConfig->triggerSource) |
                          FLEXIO_TIMCTL_PINCFG(timerConfig->pinConfig) | FLEXIO_TIMCTL_PINSEL(timerConfig->pinSelect) |
                          FLEXIO_TIMCTL_PINPOL(timerConfig->pinPolarity) | FLEXIO_TIMCTL_TIMOD(timerConfig->timerMode);
}

/*!
 * brief Registers the handle and the interrupt handler for the FlexIO-simulated peripheral.
 *
 * param base Pointer to the FlexIO simulated peripheral type.
 * param handle Pointer to the handler for FlexIO simulated peripheral.
 * param isr FlexIO simulated peripheral interrupt handler.
 * retval kStatus_Success Successfully create the handle.
 * retval kStatus_OutOfRange The FlexIO type/handle/ISR table out of range.
 */
status_t FLEXIO_RegisterHandleIRQ(void *base, void *handle, flexio_isr_t isr)
{
    assert(base);
    assert(handle);
    assert(isr);

    uint8_t index = 0;

    /* Find the an empty handle pointer to store the handle. */
    for (index = 0; index < FLEXIO_HANDLE_COUNT; index++)
    {
        if (s_flexioHandle[index] == NULL)
        {
            /* Register FLEXIO simulated driver base, handle and isr. */
            s_flexioType[index]   = base;
            s_flexioHandle[index] = handle;
            s_flexioIsr[index]    = isr;
            break;
        }
    }

    if (index == FLEXIO_HANDLE_COUNT)
    {
        return kStatus_OutOfRange;
    }
    else
    {
        return kStatus_Success;
    }
}

/*!
 * brief Unregisters the handle and the interrupt handler for the FlexIO-simulated peripheral.
 *
 * param base Pointer to the FlexIO simulated peripheral type.
 * retval kStatus_Success Successfully create the handle.
 * retval kStatus_OutOfRange The FlexIO type/handle/ISR table out of range.
 */
status_t FLEXIO_UnregisterHandleIRQ(void *base)
{
    assert(base);

    uint8_t index = 0;

    /* Find the index from base address mappings. */
    for (index = 0; index < FLEXIO_HANDLE_COUNT; index++)
    {
        if (s_flexioType[index] == base)
        {
            /* Unregister FLEXIO simulated driver handle and isr. */
            s_flexioType[index]   = NULL;
            s_flexioHandle[index] = NULL;
            s_flexioIsr[index]    = NULL;
            break;
        }
    }

    if (index == FLEXIO_HANDLE_COUNT)
    {
        return kStatus_OutOfRange;
    }
    else
    {
        return kStatus_Success;
    }
}

void FLEXIO_CommonIRQHandler(void)
{
    uint8_t index;

    for (index = 0; index < FLEXIO_HANDLE_COUNT; index++)
    {
        if (s_flexioHandle[index])
        {
            s_flexioIsr[index](s_flexioType[index], s_flexioHandle[index]);
        }
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void FLEXIO_DriverIRQHandler(void)
{
    FLEXIO_CommonIRQHandler();
}

void FLEXIO0_DriverIRQHandler(void)
{
    FLEXIO_CommonIRQHandler();
}

void FLEXIO1_DriverIRQHandler(void)
{
    FLEXIO_CommonIRQHandler();
}

void UART2_FLEXIO_DriverIRQHandler(void)
{
    FLEXIO_CommonIRQHandler();
}

void FLEXIO2_DriverIRQHandler(void)
{
    FLEXIO_CommonIRQHandler();
}

void FLEXIO3_DriverIRQHandler(void)
{
    FLEXIO_CommonIRQHandler();
}
