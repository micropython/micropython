/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_flexio_mculcd.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.flexio_mculcd"
#endif

/*******************************************************************************
 * Definitations
 ******************************************************************************/

enum _mculcd_transfer_state
{
    kFLEXIO_MCULCD_StateIdle,           /*!< No transfer in progress. */
    kFLEXIO_MCULCD_StateReadArray,      /*!< Reading array in progress. */
    kFLEXIO_MCULCD_StateWriteArray,     /*!< Writing array in progress. */
    kFLEXIO_MCULCD_StateWriteSameValue, /*!< Writing the same value in progress. */
};

/* The TIMCFG[0:7] is used for baud rate divider in dual 8-bit counters baud/bit mode. */
#define FLEXIO_BAUDRATE_DIV_MASK 0xFFU

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * brief Ungates the FlexIO clock, resets the FlexIO module, configures the
 * FlexIO MCULCD hardware, and configures the FlexIO MCULCD with FlexIO MCULCD
 * configuration.
 * The configuration structure can be filled by the user, or be set with default
 * values
 * by the ref FLEXIO_MCULCD_GetDefaultConfig.
 *
 * param base Pointer to the FLEXIO_MCULCD_Type structure.
 * param config Pointer to the flexio_mculcd_config_t structure.
 * param srcClock_Hz FlexIO source clock in Hz.
 * retval kStatus_Success Initialization success.
 * retval kStatus_InvalidArgument Initialization failed because of invalid
 * argument.
 */
status_t FLEXIO_MCULCD_Init(FLEXIO_MCULCD_Type *base, flexio_mculcd_config_t *config, uint32_t srcClock_Hz)
{
    assert(config);

    flexio_config_t flexioConfig = {config->enable, config->enableInDoze, config->enableInDebug,
                                    config->enableFastAccess};

    FLEXIO_Init(base->flexioBase, &flexioConfig);

    if (kStatus_Success != FLEXIO_MCULCD_SetBaudRate(base, config->baudRate_Bps, srcClock_Hz))
    {
        return kStatus_Success;
    }

    base->setCSPin(true);
    base->setRSPin(true);

    return kStatus_Success;
}

/*!
 * brief Resets the FLEXIO_MCULCD timer and shifter configuration.
 *
 * param base Pointer to the FLEXIO_MCULCD_Type.
 */
void FLEXIO_MCULCD_Deinit(FLEXIO_MCULCD_Type *base)
{
    FLEXIO_MCULCD_ClearSingleBeatWriteConfig(base);
    FLEXIO_MCULCD_ClearSingleBeatReadConfig(base);
}

/*!
 * brief Gets the default configuration to configure the FlexIO MCULCD.
 *
 * The default configuration value is:
 * code
 *  config->enable = true;
 *  config->enableInDoze = false;
 *  config->enableInDebug = true;
 *  config->enableFastAccess = true;
 *  config->baudRate_Bps = 96000000U;
 * endcode
 * param Config Pointer to the flexio_mculcd_config_t structure.
 */
void FLEXIO_MCULCD_GetDefaultConfig(flexio_mculcd_config_t *config)
{
    assert(config);

    /* Initializes the configure structure to zero. */
    memset(config, 0, sizeof(*config));

    config->enable           = true;
    config->enableInDoze     = false;
    config->enableInDebug    = true;
    config->enableFastAccess = true;
    config->baudRate_Bps     = 96000000U;
}

/*!
 * brief Set desired baud rate.
 *
 * param base Pointer to the FLEXIO_MCULCD_Type structure.
 * param baudRate_Bps Desired baud rate.
 * param srcClock_Hz FLEXIO clock frequency in Hz.
 * retval kStatus_Success Set successfully.
 * retval kStatus_InvalidArgument Could not set the baud rate.
 */
status_t FLEXIO_MCULCD_SetBaudRate(FLEXIO_MCULCD_Type *base, uint32_t baudRate_Bps, uint32_t srcClock_Hz)
{
    uint32_t baudRateDiv;
    uint32_t baudRatePerDataLine;
    uint32_t timerCompare;

    baudRatePerDataLine = baudRate_Bps / FLEXIO_MCULCD_DATA_BUS_WIDTH;

    baudRateDiv = (srcClock_Hz + baudRatePerDataLine) / (baudRatePerDataLine * 2);

    if ((0 == baudRateDiv) || (baudRateDiv > (FLEXIO_BAUDRATE_DIV_MASK + 1U)))
    {
        return kStatus_InvalidArgument;
    }

    baudRateDiv--;

    timerCompare = base->flexioBase->TIMCMP[base->timerIndex];

    timerCompare = (timerCompare & ~FLEXIO_BAUDRATE_DIV_MASK) | baudRateDiv;

    base->flexioBase->TIMCMP[base->timerIndex] = timerCompare;

    return kStatus_Success;
}

/*!
 * brief Gets FlexIO MCULCD status flags.
 *
 * param base Pointer to the FLEXIO_MCULCD_Type structure.
 * return status flag; OR'ed value or the ref _flexio_mculcd_status_flags.
 *
 * note Don't use this function with DMA APIs.
 */
uint32_t FLEXIO_MCULCD_GetStatusFlags(FLEXIO_MCULCD_Type *base)
{
    uint32_t ret = 0U;
    uint32_t flags;

    /* Get shifter status. */
    flags = FLEXIO_GetShifterStatusFlags(base->flexioBase);

    if (flags & (1U << base->rxShifterEndIndex))
    {
        ret |= kFLEXIO_MCULCD_RxFullFlag;
    }

    if (flags & (1U << base->txShifterStartIndex))
    {
        ret |= kFLEXIO_MCULCD_TxEmptyFlag;
    }

    return ret;
}

/*!
 * brief Clears FlexIO MCULCD status flags.
 *
 * param base Pointer to the FLEXIO_MCULCD_Type structure.
 * param mask Status to clear, it is the OR'ed value of ref
 * _flexio_mculcd_status_flags.
 *
 * note Don't use this function with DMA APIs.
 */
void FLEXIO_MCULCD_ClearStatusFlags(FLEXIO_MCULCD_Type *base, uint32_t mask)
{
    uint32_t flags = 0U;

    /* Clear the shifter flags. */
    if (mask & kFLEXIO_MCULCD_RxFullFlag)
    {
        flags |= (1U << base->rxShifterEndIndex);
    }

    if (mask & kFLEXIO_MCULCD_TxEmptyFlag)
    {
        flags |= (1U << base->txShifterStartIndex);
    }

    FLEXIO_ClearShifterStatusFlags(base->flexioBase, flags);
}

/*!
 * brief Enables the FlexIO MCULCD interrupt.
 *
 * This function enables the FlexIO MCULCD interrupt.
 *
 * param base Pointer to the FLEXIO_MCULCD_Type structure.
 * param mask Interrupts to enable, it is the OR'ed value of ref
 * _flexio_mculcd_interrupt_enable.
 */
void FLEXIO_MCULCD_EnableInterrupts(FLEXIO_MCULCD_Type *base, uint32_t mask)
{
    uint32_t interrupts = 0U;

    /* Enable shifter interrupts. */
    if (mask & kFLEXIO_MCULCD_RxFullFlag)
    {
        interrupts |= (1U << base->rxShifterEndIndex);
    }

    if (mask & kFLEXIO_MCULCD_TxEmptyFlag)
    {
        interrupts |= (1U << base->txShifterStartIndex);
    }

    FLEXIO_EnableShifterStatusInterrupts(base->flexioBase, interrupts);
}

/*!
 * brief Disables the FlexIO MCULCD interrupt.
 *
 * This function disables the FlexIO MCULCD interrupt.
 *
 * param base Pointer to the FLEXIO_MCULCD_Type structure.
 * param mask Interrupts to disable, it is the OR'ed value of ref
 * _flexio_mculcd_interrupt_enable.
 */
void FLEXIO_MCULCD_DisableInterrupts(FLEXIO_MCULCD_Type *base, uint32_t mask)
{
    uint32_t interrupts = 0U;

    /* Disable shifter interrupts. */
    if (mask & kFLEXIO_MCULCD_RxFullFlag)
    {
        interrupts |= (1U << base->rxShifterEndIndex);
    }

    if (mask & kFLEXIO_MCULCD_TxEmptyFlag)
    {
        interrupts |= (1U << base->txShifterStartIndex);
    }

    FLEXIO_DisableShifterStatusInterrupts(base->flexioBase, interrupts);
}

/*!
 * brief Read data from the FLEXIO MCULCD RX shifter buffer.
 *
 * Read data from the RX shift buffer directly, it does no check whether the
 * buffer is empty or not.
 *
 * If the data bus width is 8-bit:
 * code
 * uint8_t value;
 * value = (uint8_t)FLEXIO_MCULCD_ReadData(base);
 * endcode
 *
 * If the data bus width is 16-bit:
 * code
 * uint16_t value;
 * value = (uint16_t)FLEXIO_MCULCD_ReadData(base);
 * endcode
 *
 * note This function returns the RX shifter buffer value (32-bit) directly.
 * The return value should be converted according to data bus width.
 *
 * param base Pointer to the FLEXIO_MCULCD_Type structure.
 * return The data read out.
 *
 * note Don't use this function with DMA APIs.
 */
uint32_t FLEXIO_MCULCD_ReadData(FLEXIO_MCULCD_Type *base)
{
#if (8 == FLEXIO_MCULCD_DATA_BUS_WIDTH)
    return base->flexioBase->SHIFTBUFBYS[base->rxShifterEndIndex];
#else
    return base->flexioBase->SHIFTBUFHWS[base->rxShifterEndIndex];
#endif
}

/*!
 * brief Configures the FLEXIO MCULCD to multiple beats write mode.
 *
 * At the begining multiple beats write operation, the FLEXIO MCULCD is configured to
 * multiple beats write mode using this function. After write operation, the configuration
 * is cleared by ref FLEXIO_MCULCD_ClearSingleBeatWriteConfig.
 *
 * param base Pointer to the FLEXIO_MCULCD_Type.
 *
 * note This is an internal used function, upper layer should not use.
 */
void FLEXIO_MCULCD_SetSingleBeatWriteConfig(FLEXIO_MCULCD_Type *base)
{
    /*
     * This function will be called at the beginning of every data writing. For
     * performance consideration, it access the FlexIO registers directly, but not
     * call FlexIO driver APIs.
     */

    uint32_t timerCompare;

    /* Enable the TX Shifter output. */
    base->flexioBase->SHIFTCFG[base->txShifterStartIndex] =
        FLEXIO_SHIFTCFG_PWIDTH(FLEXIO_MCULCD_DATA_BUS_WIDTH - 1U) |
        FLEXIO_SHIFTCFG_INSRC(kFLEXIO_ShifterInputFromNextShifterOutput);

    base->flexioBase->SHIFTCTL[base->txShifterStartIndex] =
        FLEXIO_SHIFTCTL_TIMSEL(base->timerIndex) | FLEXIO_SHIFTCTL_TIMPOL(kFLEXIO_ShifterTimerPolarityOnPositive) |
        FLEXIO_SHIFTCTL_PINCFG(kFLEXIO_PinConfigOutput) | FLEXIO_SHIFTCTL_PINSEL(base->dataPinStartIndex) |
        FLEXIO_SHIFTCTL_PINPOL(kFLEXIO_PinActiveHigh) | FLEXIO_SHIFTCTL_SMOD(kFLEXIO_ShifterModeTransmit);

    timerCompare = base->flexioBase->TIMCMP[base->timerIndex] & 0xFFU;

    /*
     * TIMCMP[15:8] = (number of beats x 2) - 1. Because the number of beat is 1,
     * so the TIMCMP[15:8] is 1.
     */
    base->flexioBase->TIMCMP[base->timerIndex] = (1U << 8U) | timerCompare;

    /* Use TX shifter flag as the inverted timer trigger. Timer output to WR/EN pin. */
    base->flexioBase->TIMCFG[base->timerIndex] =
        FLEXIO_TIMCFG_TIMOUT(kFLEXIO_TimerOutputOneNotAffectedByReset) |
        FLEXIO_TIMCFG_TIMDEC(kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput) |
        FLEXIO_TIMCFG_TIMRST(kFLEXIO_TimerResetNever) | FLEXIO_TIMCFG_TIMDIS(kFLEXIO_TimerDisableOnTimerCompare) |
        FLEXIO_TIMCFG_TIMENA(kFLEXIO_TimerEnableOnTriggerHigh) | FLEXIO_TIMCFG_TSTOP(kFLEXIO_TimerStopBitDisabled) |
        FLEXIO_TIMCFG_TSTART(kFLEXIO_TimerStartBitDisabled);

    base->flexioBase->TIMCTL[base->timerIndex] =
        FLEXIO_TIMCTL_TRGSEL(FLEXIO_TIMER_TRIGGER_SEL_SHIFTnSTAT(base->txShifterStartIndex)) |
        FLEXIO_TIMCTL_TRGPOL(kFLEXIO_TimerTriggerPolarityActiveLow) |
        FLEXIO_TIMCTL_TRGSRC(kFLEXIO_TimerTriggerSourceInternal) | FLEXIO_TIMCTL_PINCFG(kFLEXIO_PinConfigOutput) |
        FLEXIO_TIMCTL_PINSEL(base->ENWRPinIndex) | FLEXIO_TIMCTL_PINPOL(kFLEXIO_PinActiveLow) |
        FLEXIO_TIMCTL_TIMOD(kFLEXIO_TimerModeDual8BitBaudBit);
}

/*!
 * brief Clear the FLEXIO MCULCD multiple beats write mode configuration.
 *
 * Clear the write configuration set by ref FLEXIO_MCULCD_SetSingleBeatWriteConfig.
 *
 * param base Pointer to the FLEXIO_MCULCD_Type.
 *
 * note This is an internal used function, upper layer should not use.
 */
void FLEXIO_MCULCD_ClearSingleBeatWriteConfig(FLEXIO_MCULCD_Type *base)
{
    /* Disable the timer. */
    base->flexioBase->TIMCTL[base->timerIndex] = 0U;
    base->flexioBase->TIMCFG[base->timerIndex] = 0U;
    /* Clear the timer flag. */
    base->flexioBase->TIMSTAT = (1U << base->timerIndex);
    /* Stop the TX shifter. */
    base->flexioBase->SHIFTCTL[base->txShifterStartIndex] = 0U;
    base->flexioBase->SHIFTCFG[base->txShifterStartIndex] = 0U;
    /* Clear the shifter flag. */
    base->flexioBase->SHIFTSTAT = (1U << base->txShifterStartIndex);
}

/*!
 * brief Configures the FLEXIO MCULCD to multiple beats read mode.
 *
 * At the begining or multiple beats read operation, the FLEXIO MCULCD is configured
 * to multiple beats read mode using this function. After read operation, the configuration
 * is cleared by ref FLEXIO_MCULCD_ClearSingleBeatReadConfig.
 *
 * param base Pointer to the FLEXIO_MCULCD_Type.
 *
 * note This is an internal used function, upper layer should not use.
 */
void FLEXIO_MCULCD_SetSingleBeatReadConfig(FLEXIO_MCULCD_Type *base)
{
    /*
     * This function will be called at the beginning of every data reading. For
     * performance consideration, it access the FlexIO registers directly, but not
     * call FlexIO driver APIs.
     */

    uint8_t timerPin;
    uint32_t timerCompare;
    flexio_pin_polarity_t timerPinPolarity;

    /* Timer output to RD pin (8080 mode), to WR/EN pin in 6800 mode. */
    if (kFLEXIO_MCULCD_8080 == base->busType)
    {
        timerPin         = base->RDPinIndex;
        timerPinPolarity = kFLEXIO_PinActiveLow;
    }
    else
    {
        timerPin         = base->ENWRPinIndex;
        timerPinPolarity = kFLEXIO_PinActiveHigh;
    }

    /* Enable the RX Shifter input. */
    base->flexioBase->SHIFTCFG[base->rxShifterEndIndex] = FLEXIO_SHIFTCFG_PWIDTH(FLEXIO_MCULCD_DATA_BUS_WIDTH - 1U);

    base->flexioBase->SHIFTCTL[base->rxShifterEndIndex] =
        FLEXIO_SHIFTCTL_TIMSEL(base->timerIndex) | FLEXIO_SHIFTCTL_TIMPOL(kFLEXIO_ShifterTimerPolarityOnNegitive) |
        FLEXIO_SHIFTCTL_PINCFG(kFLEXIO_PinConfigOutputDisabled) | FLEXIO_SHIFTCTL_PINSEL(base->dataPinStartIndex) |
        FLEXIO_SHIFTCTL_PINPOL(kFLEXIO_PinActiveHigh) | FLEXIO_SHIFTCTL_SMOD(kFLEXIO_ShifterModeReceive);

    /* Use RX shifter flag as the inverted timer trigger. */
    base->flexioBase->TIMCFG[base->timerIndex] =
        FLEXIO_TIMCFG_TIMOUT(kFLEXIO_TimerOutputOneNotAffectedByReset) |
        FLEXIO_TIMCFG_TIMDEC(kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput) |
        FLEXIO_TIMCFG_TIMRST(kFLEXIO_TimerResetNever) | FLEXIO_TIMCFG_TIMDIS(kFLEXIO_TimerDisableOnTimerCompare) |
        FLEXIO_TIMCFG_TIMENA(kFLEXIO_TimerEnableOnTriggerHigh) |
        FLEXIO_TIMCFG_TSTOP(kFLEXIO_TimerStopBitEnableOnTimerDisable) |
        FLEXIO_TIMCFG_TSTART(kFLEXIO_TimerStartBitDisabled);

    timerCompare = base->flexioBase->TIMCMP[base->timerIndex] & 0xFFU;

    /*
     * TIMCMP[15:8] = (number of beats x 2) - 1. Because the number of beat is 1,
     * so the TIMCMP[15:8] is 1.
     */
    base->flexioBase->TIMCMP[base->timerIndex] = (1U << 8U) | timerCompare;

    base->flexioBase->TIMCTL[base->timerIndex] |=
        FLEXIO_TIMCTL_TRGSEL(FLEXIO_TIMER_TRIGGER_SEL_SHIFTnSTAT(base->rxShifterEndIndex)) |
        FLEXIO_TIMCTL_TRGPOL(kFLEXIO_TimerTriggerPolarityActiveLow) |
        FLEXIO_TIMCTL_TRGSRC(kFLEXIO_TimerTriggerSourceInternal) | FLEXIO_TIMCTL_PINCFG(kFLEXIO_PinConfigOutput) |
        FLEXIO_TIMCTL_PINSEL(timerPin) | FLEXIO_TIMCTL_PINPOL(timerPinPolarity) |
        FLEXIO_TIMCTL_TIMOD(kFLEXIO_TimerModeDual8BitBaudBit);
}

/*!
 * brief Clear the FLEXIO MCULCD multiple beats read mode configuration.
 *
 * Clear the read configuration set by ref FLEXIO_MCULCD_SetSingleBeatReadConfig.
 *
 * param base Pointer to the FLEXIO_MCULCD_Type.
 *
 * note This is an internal used function, upper layer should not use.
 */
void FLEXIO_MCULCD_ClearSingleBeatReadConfig(FLEXIO_MCULCD_Type *base)
{
    /* Disable the timer. */
    base->flexioBase->TIMCTL[base->timerIndex] = 0U;
    base->flexioBase->TIMCFG[base->timerIndex] = 0U;
    /* Clear the timer flag. */
    base->flexioBase->TIMSTAT = (1U << base->timerIndex);
    /* Stop the RX shifter. */
    base->flexioBase->SHIFTCTL[base->rxShifterEndIndex] = 0U;
    base->flexioBase->SHIFTCFG[base->rxShifterEndIndex] = 0U;
    /* Clear the shifter flag. */
    base->flexioBase->SHIFTSTAT = (1U << base->rxShifterEndIndex);
}

/*!
 * brief Configures the FLEXIO MCULCD to multiple beats write mode.
 *
 * At the begining multiple beats write operation, the FLEXIO MCULCD is configured to
 * multiple beats write mode using this function. After write operation, the configuration
 * is cleared by ref FLEXIO_MCULCD_ClearMultBeatsWriteConfig.
 *
 * param base Pointer to the FLEXIO_MCULCD_Type.
 *
 * note This is an internal used function, upper layer should not use.
 */
void FLEXIO_MCULCD_SetMultiBeatsWriteConfig(FLEXIO_MCULCD_Type *base)
{
    /*
     * This function will be called at the beginning of every data writing. For
     * performance consideration, it access the FlexIO registers directly, but not
     * call FlexIO driver APIs.
     */

    uint32_t timerCompare;
    uint8_t beats;
    uint8_t i;

    /* Enable the TX Shifter output. */
    base->flexioBase->SHIFTCFG[base->txShifterStartIndex] =
        FLEXIO_SHIFTCFG_PWIDTH(FLEXIO_MCULCD_DATA_BUS_WIDTH - 1U) |
        FLEXIO_SHIFTCFG_INSRC(kFLEXIO_ShifterInputFromNextShifterOutput);

    base->flexioBase->SHIFTCTL[base->txShifterStartIndex] =
        FLEXIO_SHIFTCTL_TIMSEL(base->timerIndex) | FLEXIO_SHIFTCTL_TIMPOL(kFLEXIO_ShifterTimerPolarityOnPositive) |
        FLEXIO_SHIFTCTL_PINCFG(kFLEXIO_PinConfigOutput) | FLEXIO_SHIFTCTL_PINSEL(base->dataPinStartIndex) |
        FLEXIO_SHIFTCTL_PINPOL(kFLEXIO_PinActiveHigh) | FLEXIO_SHIFTCTL_SMOD(kFLEXIO_ShifterModeTransmit);

    for (i = base->txShifterStartIndex + 1U; i <= base->txShifterEndIndex; i++)
    {
        base->flexioBase->SHIFTCFG[i] = FLEXIO_SHIFTCFG_PWIDTH(FLEXIO_MCULCD_DATA_BUS_WIDTH - 1U) |
                                        FLEXIO_SHIFTCFG_INSRC(kFLEXIO_ShifterInputFromNextShifterOutput);

        base->flexioBase->SHIFTCTL[i] =
            FLEXIO_SHIFTCTL_TIMSEL(base->timerIndex) | FLEXIO_SHIFTCTL_TIMPOL(kFLEXIO_ShifterTimerPolarityOnPositive) |
            FLEXIO_SHIFTCTL_PINCFG(kFLEXIO_PinConfigOutputDisabled) | FLEXIO_SHIFTCTL_PINSEL(0) |
            FLEXIO_SHIFTCTL_PINPOL(kFLEXIO_PinActiveHigh) | FLEXIO_SHIFTCTL_SMOD(kFLEXIO_ShifterModeTransmit);
    }

    timerCompare = base->flexioBase->TIMCMP[base->timerIndex] & 0xFFU;

#if (8 == FLEXIO_MCULCD_DATA_BUS_WIDTH)
    beats = 4U * (base->txShifterEndIndex - base->txShifterStartIndex + 1U);
#else
    beats = 2U * (base->txShifterEndIndex - base->txShifterStartIndex + 1U);
#endif

    /*
     * TIMCMP[15:8] = (number of beats x 2) - 1.
     */
    base->flexioBase->TIMCMP[base->timerIndex] = ((beats * 2U - 1U) << 8U) | timerCompare;

    /* Use TX shifter flag as the inverted timer trigger. Timer output to WR/EN pin. */
    base->flexioBase->TIMCFG[base->timerIndex] =
        FLEXIO_TIMCFG_TIMOUT(kFLEXIO_TimerOutputOneNotAffectedByReset) |
        FLEXIO_TIMCFG_TIMDEC(kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput) |
        FLEXIO_TIMCFG_TIMRST(kFLEXIO_TimerResetNever) | FLEXIO_TIMCFG_TIMDIS(kFLEXIO_TimerDisableOnTimerCompare) |
        FLEXIO_TIMCFG_TIMENA(kFLEXIO_TimerEnableOnTriggerHigh) | FLEXIO_TIMCFG_TSTOP(kFLEXIO_TimerStopBitDisabled) |
        FLEXIO_TIMCFG_TSTART(kFLEXIO_TimerStartBitDisabled);

    base->flexioBase->TIMCTL[base->timerIndex] =
        FLEXIO_TIMCTL_TRGSEL(FLEXIO_TIMER_TRIGGER_SEL_SHIFTnSTAT(base->txShifterEndIndex)) |
        FLEXIO_TIMCTL_TRGPOL(kFLEXIO_TimerTriggerPolarityActiveLow) |
        FLEXIO_TIMCTL_TRGSRC(kFLEXIO_TimerTriggerSourceInternal) | FLEXIO_TIMCTL_PINCFG(kFLEXIO_PinConfigOutput) |
        FLEXIO_TIMCTL_PINSEL(base->ENWRPinIndex) | FLEXIO_TIMCTL_PINPOL(kFLEXIO_PinActiveLow) |
        FLEXIO_TIMCTL_TIMOD(kFLEXIO_TimerModeDual8BitBaudBit);
}

/*!
 * brief Clear the FLEXIO MCULCD multiple beats write mode configuration.
 *
 * Clear the write configuration set by ref FLEXIO_MCULCD_SetMultBeatsWriteConfig.
 *
 * param base Pointer to the FLEXIO_MCULCD_Type.
 *
 * note This is an internal used function, upper layer should not use.
 */
void FLEXIO_MCULCD_ClearMultiBeatsWriteConfig(FLEXIO_MCULCD_Type *base)
{
    uint8_t i;
    uint32_t statusFlags = 0U;

    /* Disable the timer. */
    base->flexioBase->TIMCTL[base->timerIndex] = 0U;
    base->flexioBase->TIMCFG[base->timerIndex] = 0U;
    /* Clear the timer flag. */
    base->flexioBase->TIMSTAT = (1U << base->timerIndex);

    /* Stop the TX shifter. */
    for (i = base->txShifterStartIndex; i <= base->txShifterEndIndex; i++)
    {
        base->flexioBase->SHIFTCFG[i] = 0U;
        base->flexioBase->SHIFTCTL[i] = 0U;
        statusFlags |= (1U << i);
    }
    /* Clear the shifter flag. */
    base->flexioBase->SHIFTSTAT = statusFlags;
}

/*!
 * brief Configures the FLEXIO MCULCD to multiple beats read mode.
 *
 * At the begining or multiple beats read operation, the FLEXIO MCULCD is configured
 * to multiple beats read mode using this function. After read operation, the configuration
 * is cleared by ref FLEXIO_MCULCD_ClearMultBeatsReadConfig.
 *
 * param base Pointer to the FLEXIO_MCULCD_Type.
 *
 * note This is an internal used function, upper layer should not use.
 */
void FLEXIO_MCULCD_SetMultiBeatsReadConfig(FLEXIO_MCULCD_Type *base)
{
    /*
     * This function will be called at the beginning of every data reading. For
     * performance consideration, it access the FlexIO registers directly, but not
     * call FlexIO driver APIs.
     */

    uint8_t timerPin;
    uint8_t beats;
    uint8_t i;
    uint32_t timerCompare;
    flexio_pin_polarity_t timerPinPolarity;

    /* Timer output to RD pin (8080 mode), to WR/EN pin in 6800 mode. */
    if (kFLEXIO_MCULCD_8080 == base->busType)
    {
        timerPin         = base->RDPinIndex;
        timerPinPolarity = kFLEXIO_PinActiveLow;
    }
    else
    {
        timerPin         = base->ENWRPinIndex;
        timerPinPolarity = kFLEXIO_PinActiveHigh;
    }

    /* Enable the RX Shifter input. */
    for (i = base->rxShifterStartIndex; i < base->rxShifterEndIndex; i++)
    {
        base->flexioBase->SHIFTCFG[i] = FLEXIO_SHIFTCFG_PWIDTH(FLEXIO_MCULCD_DATA_BUS_WIDTH - 1U) |
                                        FLEXIO_SHIFTCFG_INSRC(kFLEXIO_ShifterInputFromNextShifterOutput);

        base->flexioBase->SHIFTCTL[i] =
            FLEXIO_SHIFTCTL_TIMSEL(base->timerIndex) | FLEXIO_SHIFTCTL_TIMPOL(kFLEXIO_ShifterTimerPolarityOnNegitive) |
            FLEXIO_SHIFTCTL_PINCFG(kFLEXIO_PinConfigOutputDisabled) | FLEXIO_SHIFTCTL_PINSEL(base->dataPinStartIndex) |
            FLEXIO_SHIFTCTL_PINPOL(kFLEXIO_PinActiveHigh) | FLEXIO_SHIFTCTL_SMOD(kFLEXIO_ShifterModeReceive);
    }

    base->flexioBase->SHIFTCFG[base->rxShifterEndIndex] = FLEXIO_SHIFTCFG_PWIDTH(FLEXIO_MCULCD_DATA_BUS_WIDTH - 1U);
    base->flexioBase->SHIFTCTL[base->rxShifterEndIndex] =
        FLEXIO_SHIFTCTL_TIMSEL(base->timerIndex) | FLEXIO_SHIFTCTL_TIMPOL(kFLEXIO_ShifterTimerPolarityOnNegitive) |
        FLEXIO_SHIFTCTL_PINCFG(kFLEXIO_PinConfigOutputDisabled) | FLEXIO_SHIFTCTL_PINSEL(base->dataPinStartIndex) |
        FLEXIO_SHIFTCTL_PINPOL(kFLEXIO_PinActiveHigh) | FLEXIO_SHIFTCTL_SMOD(kFLEXIO_ShifterModeReceive);

    timerCompare = base->flexioBase->TIMCMP[base->timerIndex] & 0xFFU;

#if (8 == FLEXIO_MCULCD_DATA_BUS_WIDTH)
    beats = 4U * (base->rxShifterEndIndex - base->rxShifterStartIndex + 1U);
#else
    beats = 2U * (base->rxShifterEndIndex - base->rxShifterStartIndex + 1U);
#endif

    /*
     * TIMCMP[15:8] = (number of beats x 2) - 1.
     */
    base->flexioBase->TIMCMP[base->timerIndex] = ((beats * 2U - 1U) << 8U) | timerCompare;

    /* Use RX shifter flag as the inverted timer trigger. */
    base->flexioBase->TIMCFG[base->timerIndex] =
        FLEXIO_TIMCFG_TIMOUT(kFLEXIO_TimerOutputOneNotAffectedByReset) |
        FLEXIO_TIMCFG_TIMDEC(kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput) |
        FLEXIO_TIMCFG_TIMRST(kFLEXIO_TimerResetNever) | FLEXIO_TIMCFG_TIMDIS(kFLEXIO_TimerDisableOnTimerCompare) |
        FLEXIO_TIMCFG_TIMENA(kFLEXIO_TimerEnableOnTriggerHigh) |
        FLEXIO_TIMCFG_TSTOP(kFLEXIO_TimerStopBitEnableOnTimerDisable) |
        FLEXIO_TIMCFG_TSTART(kFLEXIO_TimerStartBitDisabled);

    base->flexioBase->TIMCTL[base->timerIndex] |=
        FLEXIO_TIMCTL_TRGSEL(FLEXIO_TIMER_TRIGGER_SEL_SHIFTnSTAT(base->rxShifterEndIndex)) |
        FLEXIO_TIMCTL_TRGPOL(kFLEXIO_TimerTriggerPolarityActiveLow) |
        FLEXIO_TIMCTL_TRGSRC(kFLEXIO_TimerTriggerSourceInternal) | FLEXIO_TIMCTL_PINCFG(kFLEXIO_PinConfigOutput) |
        FLEXIO_TIMCTL_PINSEL(timerPin) | FLEXIO_TIMCTL_PINPOL(timerPinPolarity) |
        FLEXIO_TIMCTL_TIMOD(kFLEXIO_TimerModeDual8BitBaudBit);
}

/*!
 * brief Clear the FLEXIO MCULCD multiple beats read mode configuration.
 *
 * Clear the read configuration set by ref FLEXIO_MCULCD_SetMultBeatsReadConfig.
 *
 * param base Pointer to the FLEXIO_MCULCD_Type.
 *
 * note This is an internal used function, upper layer should not use.
 */
void FLEXIO_MCULCD_ClearMultiBeatsReadConfig(FLEXIO_MCULCD_Type *base)
{
    uint8_t i;
    uint32_t statusFlags = 0U;

    /* Disable the timer. */
    base->flexioBase->TIMCTL[base->timerIndex] = 0U;
    base->flexioBase->TIMCFG[base->timerIndex] = 0U;
    /* Clear the timer flag. */
    base->flexioBase->TIMSTAT = (1U << base->timerIndex);
    /* Stop the RX shifter. */
    for (i = base->rxShifterStartIndex; i <= base->rxShifterEndIndex; i++)
    {
        base->flexioBase->SHIFTCTL[i] = 0U;
        base->flexioBase->SHIFTCFG[i] = 0U;
        statusFlags |= (1U << i);
    }
    /* Clear the shifter flag. */
    base->flexioBase->SHIFTSTAT = statusFlags;
}

/*!
 * brief Wait for transmit data send out finished.
 *
 * Currently there is no effective method to wait for the data send out
 * from the shiter, so here use a while loop to wait.
 *
 * note This is an internal used function.
 */
void FLEXIO_MCULCD_WaitTransmitComplete(void)
{
    uint32_t i = FLEXIO_MCULCD_WAIT_COMPLETE_TIME;

    while (i--)
    {
        __NOP();
    }
}

/*!
 * brief Send command in blocking way.
 *
 * This function sends the command and returns when the command has been sent
 * out.
 *
 * param base Pointer to the FLEXIO_MCULCD_Type structure.
 * param command The command to send.
 */
void FLEXIO_MCULCD_WriteCommandBlocking(FLEXIO_MCULCD_Type *base, uint32_t command)
{
    FLEXIO_Type *flexioBase = base->flexioBase;

    /* De-assert the RS pin. */
    base->setRSPin(false);

    /* For 6800, de-assert the RDWR pin. */
    if (kFLEXIO_MCULCD_6800 == base->busType)
    {
        base->setRDWRPin(false);
    }

    /* Configure the timer and TX shifter. */
    FLEXIO_MCULCD_SetSingleBeatWriteConfig(base);

    /* Write command to shifter buffer. */
    flexioBase->SHIFTBUF[base->txShifterStartIndex] = command;

    /* Wait for command send out. */
    while (!((1U << base->timerIndex) & FLEXIO_GetTimerStatusFlags(flexioBase)))
    {
    }

    /* Stop the timer and TX shifter. */
    FLEXIO_MCULCD_ClearSingleBeatWriteConfig(base);

    /* Assert the RS pin. */
    base->setRSPin(true);
    /* For 6800, assert the RDWR pin. */
    if (kFLEXIO_MCULCD_6800 == base->busType)
    {
        base->setRDWRPin(true);
    }
}

/*!
 * brief Send data array in blocking way.
 *
 * This function sends the data array and returns when the data sent out.
 *
 * param base Pointer to the FLEXIO_MCULCD_Type structure.
 * param data The data array to send.
 * param size How many bytes to write.
 */
void FLEXIO_MCULCD_WriteDataArrayBlocking(FLEXIO_MCULCD_Type *base, void *data, size_t size)
{
    assert(size);

    uint32_t i;
#if (8 == FLEXIO_MCULCD_DATA_BUS_WIDTH)
    uint8_t *data8Bit;
#else
    uint16_t *data16Bit;
#endif
    FLEXIO_Type *flexioBase = base->flexioBase;

    /* Assert the RS pin. */
    base->setRSPin(true);
    /* For 6800, de-assert the RDWR pin. */
    if (kFLEXIO_MCULCD_6800 == base->busType)
    {
        base->setRDWRPin(false);
    }

    /* Configure the timer and TX shifter. */
    FLEXIO_MCULCD_SetSingleBeatWriteConfig(base);

/* If data bus width is 8. */
#if (8 == FLEXIO_MCULCD_DATA_BUS_WIDTH)
    data8Bit = (uint8_t *)data;

    for (i = 0; i < size; i++)
    {
        flexioBase->SHIFTBUF[base->txShifterStartIndex] = data8Bit[i];

        /* Wait for the data send out. */
        while (!((1U << base->timerIndex) & flexioBase->TIMSTAT))
        {
        }

        /* Clear the timer stat. */
        flexioBase->TIMSTAT = 1U << base->timerIndex;
    }
#else
    data16Bit = (uint16_t *)data;
    size /= 2;

    for (i = 0; i < size; i++)
    {
        flexioBase->SHIFTBUF[base->txShifterStartIndex] = data16Bit[i];

        /* Wait for the data send out. */
        while (!((1U << base->timerIndex) & flexioBase->TIMSTAT))
        {
        }

        /* Clear the timer stat. */
        flexioBase->TIMSTAT = 1U << base->timerIndex;
    }
#endif

    /* Stop the timer and TX shifter. */
    FLEXIO_MCULCD_ClearSingleBeatWriteConfig(base);
}

/*!
 * brief Read data into array in blocking way.
 *
 * This function reads the data into array and returns when the data read
 * finished.
 *
 * param base Pointer to the FLEXIO_MCULCD_Type structure.
 * param data The array to save the data.
 * param size How many bytes to read.
 */
void FLEXIO_MCULCD_ReadDataArrayBlocking(FLEXIO_MCULCD_Type *base, void *data, size_t size)
{
    assert(size);

    uint32_t i;

#if (8 == FLEXIO_MCULCD_DATA_BUS_WIDTH)
    uint8_t *data8Bit = (uint8_t *)data;
#else
    uint16_t *data16Bit = (uint16_t *)data;
#endif
    FLEXIO_Type *flexioBase = base->flexioBase;

    /* Assert the RS pin. */
    base->setRSPin(true);
    /* For 6800, de-assert the RDWR pin. */
    if (kFLEXIO_MCULCD_6800 == base->busType)
    {
        base->setRDWRPin(false);
    }

    /* Enable the timer and RX shifter. */
    FLEXIO_MCULCD_SetSingleBeatReadConfig(base);

/* If data bus width is 8. */
#if (8 == FLEXIO_MCULCD_DATA_BUS_WIDTH)
    for (i = 0; i < (size - 1); i++)
    {
        /* Wait for shifter buffer full. */
        while (!((1U << base->rxShifterEndIndex) & FLEXIO_GetShifterStatusFlags(flexioBase)))
        {
        }

        data8Bit[i] = (uint8_t)flexioBase->SHIFTBUFBYS[base->rxShifterEndIndex];
    }
#else
    /* Data bus width is 16. */
    size /= 2;

    for (i = 0; i < (size - 1); i++)
    {
        /* Wait for shifter buffer full. */
        while (!((1U << base->rxShifterEndIndex) & FLEXIO_GetShifterStatusFlags(flexioBase)))
        {
        }

        data16Bit[i] = (uint16_t)flexioBase->SHIFTBUFHWS[base->rxShifterEndIndex];
    }
#endif

    /* Wait for shifter buffer full. */
    while (!((1U << base->rxShifterEndIndex) & FLEXIO_GetShifterStatusFlags(flexioBase)))
    {
    }

    /* Stop the timer and disable the RX shifter. */
    FLEXIO_MCULCD_ClearSingleBeatReadConfig(base);

/* Read out the last data. */
#if (8 == FLEXIO_MCULCD_DATA_BUS_WIDTH)
    data8Bit[i] = (uint8_t)flexioBase->SHIFTBUFBYS[base->rxShifterEndIndex];
#else
    data16Bit[i] = (uint16_t)flexioBase->SHIFTBUFHWS[base->rxShifterEndIndex];
#endif
}

/*!
 * brief Send the same value many times in blocking way.
 *
 * This function sends the same value many times. It could be used to clear the
 * LCD screen. If the data bus width is 8, this function will send LSB 8 bits of
 * p sameValue for p size times. If the data bus is 16, this function will send
 * LSB 16 bits of p sameValue for p size / 2 times.
 *
 * param base Pointer to the FLEXIO_MCULCD_Type structure.
 * param sameValue The same value to send.
 * param size How many bytes to send.
 */
void FLEXIO_MCULCD_WriteSameValueBlocking(FLEXIO_MCULCD_Type *base, uint32_t sameValue, size_t size)
{
    assert(size);

    uint32_t i;
    FLEXIO_Type *flexioBase = base->flexioBase;

#if (16 == FLEXIO_MCULCD_DATA_BUS_WIDTH)
    size /= 2;
#endif

    /* Assert the RS pin. */
    base->setRSPin(true);
    /* For 6800, de-assert the RDWR pin. */
    if (kFLEXIO_MCULCD_6800 == base->busType)
    {
        base->setRDWRPin(false);
    }

    /* Configure the timer and TX shifter. */
    FLEXIO_MCULCD_SetSingleBeatWriteConfig(base);

    for (i = 0; i < size; i++)
    {
        flexioBase->SHIFTBUF[base->txShifterStartIndex] = sameValue;

        /* Wait for the data send out. */
        while (!((1U << base->timerIndex) & flexioBase->TIMSTAT))
        {
        }

        /* Clear the timer stat. */
        flexioBase->TIMSTAT = 1U << base->timerIndex;
    }

    /* Stop the timer and TX shifter. */
    FLEXIO_MCULCD_ClearSingleBeatWriteConfig(base);
}

/*!
 * brief Performs a polling transfer.
 *
 * note The API does not return until the transfer finished.
 *
 * param base pointer to FLEXIO_MCULCD_Type structure.
 * param xfer pointer to flexio_mculcd_transfer_t structure.
 */
void FLEXIO_MCULCD_TransferBlocking(FLEXIO_MCULCD_Type *base, flexio_mculcd_transfer_t *xfer)
{
    FLEXIO_MCULCD_StartTransfer(base);

    FLEXIO_MCULCD_WriteCommandBlocking(base, xfer->command);

    if (xfer->dataSize > 0)
    {
        if (kFLEXIO_MCULCD_ReadArray == xfer->mode)
        {
            FLEXIO_MCULCD_ReadDataArrayBlocking(base, (void *)(xfer->dataAddrOrSameValue), xfer->dataSize);
        }
        else if (kFLEXIO_MCULCD_WriteArray == xfer->mode)
        {
            FLEXIO_MCULCD_WriteDataArrayBlocking(base, (void *)(xfer->dataAddrOrSameValue), xfer->dataSize);
        }
        else
        {
            FLEXIO_MCULCD_WriteSameValueBlocking(base, xfer->dataAddrOrSameValue, xfer->dataSize);
        }
    }

    FLEXIO_MCULCD_StopTransfer(base);
}

/*!
 * brief Initializes the FlexIO MCULCD handle, which is used in transactional
 * functions.
 *
 * param base Pointer to the FLEXIO_MCULCD_Type structure.
 * param handle Pointer to the flexio_mculcd_handle_t structure to store the
 * transfer state.
 * param callback The callback function.
 * param userData The parameter of the callback function.
 * retval kStatus_Success Successfully create the handle.
 * retval kStatus_OutOfRange The FlexIO type/handle/ISR table out of range.
 */
status_t FLEXIO_MCULCD_TransferCreateHandle(FLEXIO_MCULCD_Type *base,
                                            flexio_mculcd_handle_t *handle,
                                            flexio_mculcd_transfer_callback_t callback,
                                            void *userData)
{
    assert(handle);

    IRQn_Type flexio_irqs[] = FLEXIO_IRQS;

    /* Zero the handle. */
    memset(handle, 0, sizeof(*handle));

    handle->state = kFLEXIO_MCULCD_StateIdle;

    /* Register callback and userData. */
    handle->completionCallback = callback;
    handle->userData           = userData;

    /* Enable interrupt in NVIC. */
    EnableIRQ(flexio_irqs[FLEXIO_GetInstance(base->flexioBase)]);

    /* Save the context in global variables to support the double weak mechanism.
     */
    return FLEXIO_RegisterHandleIRQ(base, handle, FLEXIO_MCULCD_TransferHandleIRQ);
}

/*!
 * brief Transfer data using IRQ.
 *
 * This function sends data using IRQ. This is a non-blocking function, which
 * returns right away. When all data is sent out/received, the callback
 * function is called.
 *
 * param base Pointer to the FLEXIO_MCULCD_Type structure.
 * param handle Pointer to the flexio_mculcd_handle_t structure to store the
 * transfer state.
 * param xfer FlexIO MCULCD transfer structure. See #flexio_mculcd_transfer_t.
 * retval kStatus_Success Successfully start a transfer.
 * retval kStatus_InvalidArgument Input argument is invalid.
 * retval kStatus_FLEXIO_MCULCD_Busy MCULCD is busy with another transfer.
 */
status_t FLEXIO_MCULCD_TransferNonBlocking(FLEXIO_MCULCD_Type *base,
                                           flexio_mculcd_handle_t *handle,
                                           flexio_mculcd_transfer_t *xfer)
{
    /* If previous transfer is in progress. */
    if (kFLEXIO_MCULCD_StateIdle != handle->state)
    {
        return kStatus_FLEXIO_MCULCD_Busy;
    }

    /* Set the state in handle. */
    if (kFLEXIO_MCULCD_ReadArray == xfer->mode)
    {
        handle->state = kFLEXIO_MCULCD_StateReadArray;
    }
    else if (kFLEXIO_MCULCD_WriteArray == xfer->mode)
    {
        handle->state = kFLEXIO_MCULCD_StateWriteArray;
    }
    else
    {
        handle->state = kFLEXIO_MCULCD_StateWriteSameValue;
    }

    /* Assert the nCS. */
    FLEXIO_MCULCD_StartTransfer(base);

    /* Send the command. */
    FLEXIO_MCULCD_WriteCommandBlocking(base, xfer->command);

    /* If transfer count is 0 (only to send command), return directly. */
    if (0U == xfer->dataSize)
    {
        handle->state = kFLEXIO_MCULCD_StateIdle;

        /* De-assert the nCS. */
        FLEXIO_MCULCD_StopTransfer(base);

        if (handle->completionCallback)
        {
            handle->completionCallback(base, handle, kStatus_FLEXIO_MCULCD_Idle, handle->userData);
        }
    }
    else
    {
#if (8 == FLEXIO_MCULCD_DATA_BUS_WIDTH)
        handle->dataCount = xfer->dataSize;
#else
        handle->dataCount = xfer->dataSize / 2;
#endif

        handle->remainingCount = handle->dataCount;

        handle->dataAddrOrSameValue = xfer->dataAddrOrSameValue;

        /* Enable interrupt. */
        if (kFLEXIO_MCULCD_ReadArray == xfer->mode)
        {
            /* For 6800, assert the RDWR pin. */
            if (kFLEXIO_MCULCD_6800 == base->busType)
            {
                base->setRDWRPin(true);
            }
            FLEXIO_MCULCD_SetSingleBeatReadConfig(base);
            FLEXIO_MCULCD_EnableInterrupts(base, kFLEXIO_MCULCD_RxFullInterruptEnable);
        }
        else
        {
            /* For 6800, de-assert the RDWR pin. */
            if (kFLEXIO_MCULCD_6800 == base->busType)
            {
                base->setRDWRPin(false);
            }
            FLEXIO_MCULCD_SetSingleBeatWriteConfig(base);
            FLEXIO_MCULCD_EnableInterrupts(base, kFLEXIO_MCULCD_TxEmptyInterruptEnable);
        }
    }

    return kStatus_Success;
}

/*!
 * brief Aborts the data transfer, which used IRQ.
 *
 * param base Pointer to the FLEXIO_MCULCD_Type structure.
 * param handle Pointer to the flexio_mculcd_handle_t structure to store the
 * transfer state.
 */
void FLEXIO_MCULCD_TransferAbort(FLEXIO_MCULCD_Type *base, flexio_mculcd_handle_t *handle)
{
    /* If no transfer in process, return directly. */
    if (kFLEXIO_MCULCD_StateIdle == handle->state)
    {
        return;
    }

    /* Disable the interrupt. */
    FLEXIO_MCULCD_DisableInterrupts(base, kFLEXIO_MCULCD_RxFullInterruptEnable | kFLEXIO_MCULCD_TxEmptyInterruptEnable);

    if (kFLEXIO_MCULCD_ReadArray == handle->state)
    {
        /* Stop the timer and disable the RX shifter. */
        FLEXIO_MCULCD_ClearSingleBeatReadConfig(base);
    }
    else
    {
        /* Stop the timer and disable the TX shifter. */
        FLEXIO_MCULCD_ClearSingleBeatWriteConfig(base);
    }

    /* Clean the flags. */
    FLEXIO_MCULCD_ClearStatusFlags(base, kFLEXIO_MCULCD_TxEmptyFlag | kFLEXIO_MCULCD_RxFullFlag);

    /* De-assert the nCS. */
    FLEXIO_MCULCD_StopTransfer(base);

    handle->dataCount      = 0;
    handle->remainingCount = 0;
    handle->state          = kFLEXIO_MCULCD_StateIdle;
}

/*!
 * brief Gets the data transfer status which used IRQ.
 *
 * param base Pointer to the FLEXIO_MCULCD_Type structure.
 * param handle Pointer to the flexio_mculcd_handle_t structure to store the
 * transfer state.
 * param count How many bytes transferred so far by the non-blocking transaction.
 * retval kStatus_Success Get the transferred count Successfully.
 * retval kStatus_NoTransferInProgress No transfer in process.
 */
status_t FLEXIO_MCULCD_TransferGetCount(FLEXIO_MCULCD_Type *base, flexio_mculcd_handle_t *handle, size_t *count)
{
    assert(count);

    if (kFLEXIO_MCULCD_StateIdle == handle->state)
    {
        return kStatus_NoTransferInProgress;
    }

    *count = handle->dataCount - handle->remainingCount;

#if (16 == FLEXIO_MCULCD_DATA_BUS_WIDTH)
    *count *= 2;
#endif

    return kStatus_Success;
}

/*!
 * brief FlexIO MCULCD IRQ handler function.
 *
 * param base Pointer to the FLEXIO_MCULCD_Type structure.
 * param handle Pointer to the flexio_mculcd_handle_t structure to store the
 * transfer state.
 */
void FLEXIO_MCULCD_TransferHandleIRQ(void *base, void *handle)
{
    FLEXIO_MCULCD_Type *flexioLcdMcuBase       = (FLEXIO_MCULCD_Type *)base;
    flexio_mculcd_handle_t *flexioLcdMcuHandle = (flexio_mculcd_handle_t *)handle;
    uint32_t statusFlags                       = FLEXIO_MCULCD_GetStatusFlags(flexioLcdMcuBase);
    uint32_t data;

    if (kFLEXIO_MCULCD_StateReadArray == flexioLcdMcuHandle->state)
    {
        /* Handle the reading process. */
        while ((kFLEXIO_MCULCD_RxFullFlag & statusFlags) && (flexioLcdMcuHandle->remainingCount > 0))
        {
            if (1 == flexioLcdMcuHandle->remainingCount)
            {
                /* If this is the last data, stop the RX shifter and timer. */
                FLEXIO_MCULCD_DisableInterrupts(flexioLcdMcuBase, kFLEXIO_MCULCD_RxFullInterruptEnable);
                FLEXIO_MCULCD_ClearSingleBeatReadConfig(flexioLcdMcuBase);
                FLEXIO_MCULCD_StopTransfer(flexioLcdMcuBase);
            }

            /* Read out the data. */
            data = FLEXIO_MCULCD_ReadData(flexioLcdMcuBase);

#if (8 == FLEXIO_MCULCD_DATA_BUS_WIDTH)
            *(uint8_t *)(flexioLcdMcuHandle->dataAddrOrSameValue) = (uint8_t)data;
            flexioLcdMcuHandle->dataAddrOrSameValue++;
#else
            *(uint16_t *)(flexioLcdMcuHandle->dataAddrOrSameValue) = (uint16_t)data;
            flexioLcdMcuHandle->dataAddrOrSameValue += 2U;
#endif

            flexioLcdMcuHandle->remainingCount--;

            /* Transfer finished, call the callback. */
            if (0 == flexioLcdMcuHandle->remainingCount)
            {
                flexioLcdMcuHandle->state = kFLEXIO_MCULCD_StateIdle;

                if (flexioLcdMcuHandle->completionCallback)
                {
                    flexioLcdMcuHandle->completionCallback(flexioLcdMcuBase, flexioLcdMcuHandle,
                                                           kStatus_FLEXIO_MCULCD_Idle, flexioLcdMcuHandle->userData);
                }
            }

            /* Is the shifter buffer ready to send the next data? */
            statusFlags = FLEXIO_MCULCD_GetStatusFlags(flexioLcdMcuBase);
        }
    }
    else
    {
        /* Handle the writing process. */
        while ((kFLEXIO_MCULCD_TxEmptyFlag & statusFlags) && (flexioLcdMcuHandle->remainingCount > 0))
        {
            /* Send the data. */
            if (kFLEXIO_MCULCD_StateWriteSameValue == flexioLcdMcuHandle->state)
            {
                data = flexioLcdMcuHandle->dataAddrOrSameValue;
            }
            else
            {
#if (8 == FLEXIO_MCULCD_DATA_BUS_WIDTH)
                data = *(uint8_t *)(flexioLcdMcuHandle->dataAddrOrSameValue);
                flexioLcdMcuHandle->dataAddrOrSameValue++;
#else
                data = *(uint16_t *)(flexioLcdMcuHandle->dataAddrOrSameValue);
                flexioLcdMcuHandle->dataAddrOrSameValue += 2U;
#endif
            }

            /* If this is the last data to send, delay to wait for the data shift out.  */
            if (1 == flexioLcdMcuHandle->remainingCount)
            {
                FLEXIO_MCULCD_DisableInterrupts(flexioLcdMcuBase, kFLEXIO_MCULCD_TxEmptyInterruptEnable);

                /* Write the last data. */
                FLEXIO_MCULCD_WriteData(flexioLcdMcuBase, data);

                /* Wait for the last data send finished. */
                FLEXIO_MCULCD_WaitTransmitComplete();
                flexioLcdMcuHandle->remainingCount = 0;

                FLEXIO_MCULCD_ClearSingleBeatWriteConfig(flexioLcdMcuBase);
                FLEXIO_MCULCD_StopTransfer(flexioLcdMcuBase);
                flexioLcdMcuHandle->state = kFLEXIO_MCULCD_StateIdle;

                if (flexioLcdMcuHandle->completionCallback)
                {
                    flexioLcdMcuHandle->completionCallback(flexioLcdMcuBase, flexioLcdMcuHandle,
                                                           kStatus_FLEXIO_MCULCD_Idle, flexioLcdMcuHandle->userData);
                }
            }
            else
            {
                FLEXIO_MCULCD_WriteData(flexioLcdMcuBase, data);
                flexioLcdMcuHandle->remainingCount--;
            }
            /* Is the shifter buffer ready to send the next data? */
            statusFlags = FLEXIO_MCULCD_GetStatusFlags(flexioLcdMcuBase);
        }
    }
}
