/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_flexio_spi.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.flexio_spi"
#endif

/*! @brief FLEXIO SPI transfer state, which is used for SPI transactiaonl APIs' internal state. */
enum _flexio_spi_transfer_states
{
    kFLEXIO_SPI_Idle = 0x0U, /*!< Nothing in the transmitter/receiver's queue. */
    kFLEXIO_SPI_Busy,        /*!< Transmiter/Receive's queue is not finished. */
};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief Send a piece of data for SPI.
 *
 * This function computes the number of data to be written into D register or Tx FIFO,
 * and write the data into it. At the same time, this function updates the values in
 * master handle structure.
 *
 * @param base pointer to FLEXIO_SPI_Type structure
 * @param handle Pointer to SPI master handle structure.
 */
static void FLEXIO_SPI_TransferSendTransaction(FLEXIO_SPI_Type *base, flexio_spi_master_handle_t *handle);

/*!
 * @brief Receive a piece of data for SPI master.
 *
 * This function computes the number of data to receive from D register or Rx FIFO,
 * and write the data to destination address. At the same time, this function updates
 * the values in master handle structure.
 *
 * @param base pointer to FLEXIO_SPI_Type structure
 * @param handle Pointer to SPI master handle structure.
 */
static void FLEXIO_SPI_TransferReceiveTransaction(FLEXIO_SPI_Type *base, flexio_spi_master_handle_t *handle);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Codes
 ******************************************************************************/

static uint32_t FLEXIO_SPI_GetInstance(FLEXIO_SPI_Type *base)
{
    return FLEXIO_GetInstance(base->flexioBase);
}

static void FLEXIO_SPI_TransferSendTransaction(FLEXIO_SPI_Type *base, flexio_spi_master_handle_t *handle)
{
    uint16_t tmpData = FLEXIO_SPI_DUMMYDATA;

    if (handle->txData != NULL)
    {
        /* Transmit data and update tx size/buff. */
        if (handle->bytePerFrame == 1U)
        {
            tmpData = *(handle->txData);
            handle->txData++;
        }
        else
        {
            if (handle->direction == kFLEXIO_SPI_MsbFirst)
            {
                tmpData = (uint32_t)(handle->txData[0]) << 8U;
                tmpData += handle->txData[1];
            }
            else
            {
                tmpData = (uint32_t)(handle->txData[1]) << 8U;
                tmpData += handle->txData[0];
            }
            handle->txData += 2U;
        }
    }
    else
    {
        tmpData = FLEXIO_SPI_DUMMYDATA;
    }

    handle->txRemainingBytes -= handle->bytePerFrame;

    FLEXIO_SPI_WriteData(base, handle->direction, tmpData);

    if (!handle->txRemainingBytes)
    {
        FLEXIO_SPI_DisableInterrupts(base, kFLEXIO_SPI_TxEmptyInterruptEnable);
    }
}

static void FLEXIO_SPI_TransferReceiveTransaction(FLEXIO_SPI_Type *base, flexio_spi_master_handle_t *handle)
{
    uint16_t tmpData;

    tmpData = FLEXIO_SPI_ReadData(base, handle->direction);

    if (handle->rxData != NULL)
    {
        if (handle->bytePerFrame == 1U)
        {
            *handle->rxData = tmpData;
            handle->rxData++;
        }
        else
        {
            if (handle->direction == kFLEXIO_SPI_MsbFirst)
            {
                *((uint16_t *)(handle->rxData)) = tmpData;
            }
            else
            {
                *((uint16_t *)(handle->rxData)) = (((tmpData << 8) & 0xff00U) | ((tmpData >> 8) & 0x00ffU));
            }
            handle->rxData += 2U;
        }
    }
    handle->rxRemainingBytes -= handle->bytePerFrame;
}

/*!
 * brief Ungates the FlexIO clock, resets the FlexIO module, configures the FlexIO SPI master hardware,
 * and configures the FlexIO SPI with FlexIO SPI master configuration. The
 * configuration structure can be filled by the user, or be set with default values
 * by the FLEXIO_SPI_MasterGetDefaultConfig().
 *
 * note FlexIO SPI master only support CPOL = 0, which means clock inactive low.
 *
 * Example
   code
   FLEXIO_SPI_Type spiDev = {
   .flexioBase = FLEXIO,
   .SDOPinIndex = 0,
   .SDIPinIndex = 1,
   .SCKPinIndex = 2,
   .CSnPinIndex = 3,
   .shifterIndex = {0,1},
   .timerIndex = {0,1}
   };
   flexio_spi_master_config_t config = {
   .enableMaster = true,
   .enableInDoze = false,
   .enableInDebug = true,
   .enableFastAccess = false,
   .baudRate_Bps = 500000,
   .phase = kFLEXIO_SPI_ClockPhaseFirstEdge,
   .direction = kFLEXIO_SPI_MsbFirst,
   .dataMode = kFLEXIO_SPI_8BitMode
   };
   FLEXIO_SPI_MasterInit(&spiDev, &config, srcClock_Hz);
   endcode
 *
 * param base Pointer to the FLEXIO_SPI_Type structure.
 * param masterConfig Pointer to the flexio_spi_master_config_t structure.
 * param srcClock_Hz FlexIO source clock in Hz.
*/
void FLEXIO_SPI_MasterInit(FLEXIO_SPI_Type *base, flexio_spi_master_config_t *masterConfig, uint32_t srcClock_Hz)
{
    assert(base);
    assert(masterConfig);

    flexio_shifter_config_t shifterConfig;
    flexio_timer_config_t timerConfig;
    uint32_t ctrlReg  = 0;
    uint16_t timerDiv = 0;
    uint16_t timerCmp = 0;

    /* Clear the shifterConfig & timerConfig struct. */
    memset(&shifterConfig, 0, sizeof(shifterConfig));
    memset(&timerConfig, 0, sizeof(timerConfig));

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Ungate flexio clock. */
    CLOCK_EnableClock(s_flexioClocks[FLEXIO_SPI_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    /* Configure FLEXIO SPI Master */
    ctrlReg = base->flexioBase->CTRL;
    ctrlReg &= ~(FLEXIO_CTRL_DOZEN_MASK | FLEXIO_CTRL_DBGE_MASK | FLEXIO_CTRL_FASTACC_MASK | FLEXIO_CTRL_FLEXEN_MASK);
    ctrlReg |= (FLEXIO_CTRL_DBGE(masterConfig->enableInDebug) | FLEXIO_CTRL_FASTACC(masterConfig->enableFastAccess) |
                FLEXIO_CTRL_FLEXEN(masterConfig->enableMaster));
    if (!masterConfig->enableInDoze)
    {
        ctrlReg |= FLEXIO_CTRL_DOZEN_MASK;
    }

    base->flexioBase->CTRL = ctrlReg;

    /* Do hardware configuration. */
    /* 1. Configure the shifter 0 for tx. */
    shifterConfig.timerSelect = base->timerIndex[0];
    shifterConfig.pinConfig   = kFLEXIO_PinConfigOutput;
    shifterConfig.pinSelect   = base->SDOPinIndex;
    shifterConfig.pinPolarity = kFLEXIO_PinActiveHigh;
    shifterConfig.shifterMode = kFLEXIO_ShifterModeTransmit;
    shifterConfig.inputSource = kFLEXIO_ShifterInputFromPin;
    if (masterConfig->phase == kFLEXIO_SPI_ClockPhaseFirstEdge)
    {
        shifterConfig.timerPolarity = kFLEXIO_ShifterTimerPolarityOnNegitive;
        shifterConfig.shifterStop   = kFLEXIO_ShifterStopBitDisable;
        shifterConfig.shifterStart  = kFLEXIO_ShifterStartBitDisabledLoadDataOnEnable;
    }
    else
    {
        shifterConfig.timerPolarity = kFLEXIO_ShifterTimerPolarityOnPositive;
        shifterConfig.shifterStop   = kFLEXIO_ShifterStopBitLow;
        shifterConfig.shifterStart  = kFLEXIO_ShifterStartBitDisabledLoadDataOnShift;
    }

    FLEXIO_SetShifterConfig(base->flexioBase, base->shifterIndex[0], &shifterConfig);

    /* 2. Configure the shifter 1 for rx. */
    shifterConfig.timerSelect  = base->timerIndex[0];
    shifterConfig.pinConfig    = kFLEXIO_PinConfigOutputDisabled;
    shifterConfig.pinSelect    = base->SDIPinIndex;
    shifterConfig.pinPolarity  = kFLEXIO_PinActiveHigh;
    shifterConfig.shifterMode  = kFLEXIO_ShifterModeReceive;
    shifterConfig.inputSource  = kFLEXIO_ShifterInputFromPin;
    shifterConfig.shifterStop  = kFLEXIO_ShifterStopBitDisable;
    shifterConfig.shifterStart = kFLEXIO_ShifterStartBitDisabledLoadDataOnEnable;
    if (masterConfig->phase == kFLEXIO_SPI_ClockPhaseFirstEdge)
    {
        shifterConfig.timerPolarity = kFLEXIO_ShifterTimerPolarityOnPositive;
    }
    else
    {
        shifterConfig.timerPolarity = kFLEXIO_ShifterTimerPolarityOnNegitive;
    }

    FLEXIO_SetShifterConfig(base->flexioBase, base->shifterIndex[1], &shifterConfig);

    /*3. Configure the timer 0 for SCK. */
    timerConfig.triggerSelect   = FLEXIO_TIMER_TRIGGER_SEL_SHIFTnSTAT(base->shifterIndex[0]);
    timerConfig.triggerPolarity = kFLEXIO_TimerTriggerPolarityActiveLow;
    timerConfig.triggerSource   = kFLEXIO_TimerTriggerSourceInternal;
    timerConfig.pinConfig       = kFLEXIO_PinConfigOutput;
    timerConfig.pinSelect       = base->SCKPinIndex;
    timerConfig.pinPolarity     = kFLEXIO_PinActiveHigh;
    timerConfig.timerMode       = kFLEXIO_TimerModeDual8BitBaudBit;
    timerConfig.timerOutput     = kFLEXIO_TimerOutputZeroNotAffectedByReset;
    timerConfig.timerDecrement  = kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput;
    timerConfig.timerReset      = kFLEXIO_TimerResetNever;
    timerConfig.timerDisable    = kFLEXIO_TimerDisableOnTimerCompare;
    timerConfig.timerEnable     = kFLEXIO_TimerEnableOnTriggerHigh;
    timerConfig.timerStop       = kFLEXIO_TimerStopBitEnableOnTimerDisable;
    timerConfig.timerStart      = kFLEXIO_TimerStartBitEnabled;

    timerDiv = srcClock_Hz / masterConfig->baudRate_Bps;
    timerDiv = timerDiv / 2 - 1;

    timerCmp = ((uint32_t)(masterConfig->dataMode * 2 - 1U)) << 8U;
    timerCmp |= timerDiv;

    timerConfig.timerCompare = timerCmp;

    FLEXIO_SetTimerConfig(base->flexioBase, base->timerIndex[0], &timerConfig);

    /* 4. Configure the timer 1 for CSn. */
    timerConfig.triggerSelect   = FLEXIO_TIMER_TRIGGER_SEL_TIMn(base->timerIndex[0]);
    timerConfig.triggerPolarity = kFLEXIO_TimerTriggerPolarityActiveHigh;
    timerConfig.triggerSource   = kFLEXIO_TimerTriggerSourceInternal;
    timerConfig.pinConfig       = kFLEXIO_PinConfigOutput;
    timerConfig.pinSelect       = base->CSnPinIndex;
    timerConfig.pinPolarity     = kFLEXIO_PinActiveLow;
    timerConfig.timerMode       = kFLEXIO_TimerModeSingle16Bit;
    timerConfig.timerOutput     = kFLEXIO_TimerOutputOneNotAffectedByReset;
    timerConfig.timerDecrement  = kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput;
    timerConfig.timerReset      = kFLEXIO_TimerResetNever;
    timerConfig.timerDisable    = kFLEXIO_TimerDisableOnPreTimerDisable;
    timerConfig.timerEnable     = kFLEXIO_TimerEnableOnPrevTimerEnable;
    timerConfig.timerStop       = kFLEXIO_TimerStopBitDisabled;
    timerConfig.timerStart      = kFLEXIO_TimerStartBitDisabled;

    timerConfig.timerCompare = 0xFFFFU;

    FLEXIO_SetTimerConfig(base->flexioBase, base->timerIndex[1], &timerConfig);
}

/*!
 * brief Resets the FlexIO SPI timer and shifter config.
 *
 * param base Pointer to the FLEXIO_SPI_Type.
 */
void FLEXIO_SPI_MasterDeinit(FLEXIO_SPI_Type *base)
{
    base->flexioBase->SHIFTCFG[base->shifterIndex[0]] = 0;
    base->flexioBase->SHIFTCTL[base->shifterIndex[0]] = 0;
    base->flexioBase->SHIFTCFG[base->shifterIndex[1]] = 0;
    base->flexioBase->SHIFTCTL[base->shifterIndex[1]] = 0;
    base->flexioBase->TIMCFG[base->timerIndex[0]]     = 0;
    base->flexioBase->TIMCMP[base->timerIndex[0]]     = 0;
    base->flexioBase->TIMCTL[base->timerIndex[0]]     = 0;
    base->flexioBase->TIMCFG[base->timerIndex[1]]     = 0;
    base->flexioBase->TIMCMP[base->timerIndex[1]]     = 0;
    base->flexioBase->TIMCTL[base->timerIndex[1]]     = 0;
}

/*!
 * brief Gets the default configuration to configure the FlexIO SPI master. The configuration
 * can be used directly by calling the FLEXIO_SPI_MasterConfigure().
 * Example:
   code
   flexio_spi_master_config_t masterConfig;
   FLEXIO_SPI_MasterGetDefaultConfig(&masterConfig);
   endcode
 * param masterConfig Pointer to the flexio_spi_master_config_t structure.
*/
void FLEXIO_SPI_MasterGetDefaultConfig(flexio_spi_master_config_t *masterConfig)
{
    assert(masterConfig);

    /* Initializes the configure structure to zero. */
    memset(masterConfig, 0, sizeof(*masterConfig));

    masterConfig->enableMaster     = true;
    masterConfig->enableInDoze     = false;
    masterConfig->enableInDebug    = true;
    masterConfig->enableFastAccess = false;
    /* Default baud rate 500kbps. */
    masterConfig->baudRate_Bps = 500000U;
    /* Default CPHA = 0. */
    masterConfig->phase = kFLEXIO_SPI_ClockPhaseFirstEdge;
    /* Default bit count at 8. */
    masterConfig->dataMode = kFLEXIO_SPI_8BitMode;
}

/*!
 * brief Ungates the FlexIO clock, resets the FlexIO module, configures the FlexIO SPI slave hardware
 * configuration, and configures the FlexIO SPI with FlexIO SPI slave configuration. The
 * configuration structure can be filled by the user, or be set with default values
 * by the FLEXIO_SPI_SlaveGetDefaultConfig().
 *
 * note Only one timer is needed in the FlexIO SPI slave. As a result, the second timer index is ignored.
 * FlexIO SPI slave only support CPOL = 0, which means clock inactive low.
 * Example
   code
   FLEXIO_SPI_Type spiDev = {
   .flexioBase = FLEXIO,
   .SDOPinIndex = 0,
   .SDIPinIndex = 1,
   .SCKPinIndex = 2,
   .CSnPinIndex = 3,
   .shifterIndex = {0,1},
   .timerIndex = {0}
   };
   flexio_spi_slave_config_t config = {
   .enableSlave = true,
   .enableInDoze = false,
   .enableInDebug = true,
   .enableFastAccess = false,
   .phase = kFLEXIO_SPI_ClockPhaseFirstEdge,
   .direction = kFLEXIO_SPI_MsbFirst,
   .dataMode = kFLEXIO_SPI_8BitMode
   };
   FLEXIO_SPI_SlaveInit(&spiDev, &config);
   endcode
 * param base Pointer to the FLEXIO_SPI_Type structure.
 * param slaveConfig Pointer to the flexio_spi_slave_config_t structure.
*/
void FLEXIO_SPI_SlaveInit(FLEXIO_SPI_Type *base, flexio_spi_slave_config_t *slaveConfig)
{
    assert(base && slaveConfig);

    flexio_shifter_config_t shifterConfig;
    flexio_timer_config_t timerConfig;
    uint32_t ctrlReg = 0;

    /* Clear the shifterConfig & timerConfig struct. */
    memset(&shifterConfig, 0, sizeof(shifterConfig));
    memset(&timerConfig, 0, sizeof(timerConfig));

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Ungate flexio clock. */
    CLOCK_EnableClock(s_flexioClocks[FLEXIO_SPI_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    /* Configure FLEXIO SPI Slave */
    ctrlReg = base->flexioBase->CTRL;
    ctrlReg &= ~(FLEXIO_CTRL_DOZEN_MASK | FLEXIO_CTRL_DBGE_MASK | FLEXIO_CTRL_FASTACC_MASK | FLEXIO_CTRL_FLEXEN_MASK);
    ctrlReg |= (FLEXIO_CTRL_DBGE(slaveConfig->enableInDebug) | FLEXIO_CTRL_FASTACC(slaveConfig->enableFastAccess) |
                FLEXIO_CTRL_FLEXEN(slaveConfig->enableSlave));
    if (!slaveConfig->enableInDoze)
    {
        ctrlReg |= FLEXIO_CTRL_DOZEN_MASK;
    }

    base->flexioBase->CTRL = ctrlReg;

    /* Do hardware configuration. */
    /* 1. Configure the shifter 0 for tx. */
    shifterConfig.timerSelect = base->timerIndex[0];
    shifterConfig.pinConfig   = kFLEXIO_PinConfigOutput;
    shifterConfig.pinSelect   = base->SDOPinIndex;
    shifterConfig.pinPolarity = kFLEXIO_PinActiveHigh;
    shifterConfig.shifterMode = kFLEXIO_ShifterModeTransmit;
    shifterConfig.inputSource = kFLEXIO_ShifterInputFromPin;
    shifterConfig.shifterStop = kFLEXIO_ShifterStopBitDisable;
    if (slaveConfig->phase == kFLEXIO_SPI_ClockPhaseFirstEdge)
    {
        shifterConfig.timerPolarity = kFLEXIO_ShifterTimerPolarityOnNegitive;
        shifterConfig.shifterStart  = kFLEXIO_ShifterStartBitDisabledLoadDataOnEnable;
    }
    else
    {
        shifterConfig.timerPolarity = kFLEXIO_ShifterTimerPolarityOnPositive;
        shifterConfig.shifterStart  = kFLEXIO_ShifterStartBitDisabledLoadDataOnShift;
    }

    FLEXIO_SetShifterConfig(base->flexioBase, base->shifterIndex[0], &shifterConfig);

    /* 2. Configure the shifter 1 for rx. */
    shifterConfig.timerSelect  = base->timerIndex[0];
    shifterConfig.pinConfig    = kFLEXIO_PinConfigOutputDisabled;
    shifterConfig.pinSelect    = base->SDIPinIndex;
    shifterConfig.pinPolarity  = kFLEXIO_PinActiveHigh;
    shifterConfig.shifterMode  = kFLEXIO_ShifterModeReceive;
    shifterConfig.inputSource  = kFLEXIO_ShifterInputFromPin;
    shifterConfig.shifterStop  = kFLEXIO_ShifterStopBitDisable;
    shifterConfig.shifterStart = kFLEXIO_ShifterStartBitDisabledLoadDataOnEnable;
    if (slaveConfig->phase == kFLEXIO_SPI_ClockPhaseFirstEdge)
    {
        shifterConfig.timerPolarity = kFLEXIO_ShifterTimerPolarityOnPositive;
    }
    else
    {
        shifterConfig.timerPolarity = kFLEXIO_ShifterTimerPolarityOnNegitive;
    }

    FLEXIO_SetShifterConfig(base->flexioBase, base->shifterIndex[1], &shifterConfig);

    /*3. Configure the timer 0 for shift clock. */
    timerConfig.triggerSelect   = FLEXIO_TIMER_TRIGGER_SEL_PININPUT(base->CSnPinIndex);
    timerConfig.triggerPolarity = kFLEXIO_TimerTriggerPolarityActiveLow;
    timerConfig.triggerSource   = kFLEXIO_TimerTriggerSourceInternal;
    timerConfig.pinConfig       = kFLEXIO_PinConfigOutputDisabled;
    timerConfig.pinSelect       = base->SCKPinIndex;
    timerConfig.pinPolarity     = kFLEXIO_PinActiveHigh;
    timerConfig.timerMode       = kFLEXIO_TimerModeSingle16Bit;
    timerConfig.timerOutput     = kFLEXIO_TimerOutputZeroNotAffectedByReset;
    timerConfig.timerDecrement  = kFLEXIO_TimerDecSrcOnPinInputShiftPinInput;
    timerConfig.timerReset      = kFLEXIO_TimerResetNever;
    timerConfig.timerEnable     = kFLEXIO_TimerEnableOnTriggerRisingEdge;
    timerConfig.timerStop       = kFLEXIO_TimerStopBitDisabled;
    if (slaveConfig->phase == kFLEXIO_SPI_ClockPhaseFirstEdge)
    {
        /* The configuration kFLEXIO_TimerDisableOnTimerCompare only support continuous
        PCS access, change to kFLEXIO_TimerDisableNever to enable discontinuous PCS access. */
        timerConfig.timerDisable = kFLEXIO_TimerDisableOnTimerCompare;
        timerConfig.timerStart   = kFLEXIO_TimerStartBitDisabled;
    }
    else
    {
        timerConfig.timerDisable = kFLEXIO_TimerDisableOnTriggerFallingEdge;
        timerConfig.timerStart   = kFLEXIO_TimerStartBitEnabled;
    }

    timerConfig.timerCompare = slaveConfig->dataMode * 2 - 1U;

    FLEXIO_SetTimerConfig(base->flexioBase, base->timerIndex[0], &timerConfig);
}

/*!
 * brief Gates the FlexIO clock.
 *
 * param base Pointer to the FLEXIO_SPI_Type.
 */
void FLEXIO_SPI_SlaveDeinit(FLEXIO_SPI_Type *base)
{
    FLEXIO_SPI_MasterDeinit(base);
}

/*!
 * brief Gets the default configuration to configure the FlexIO SPI slave. The configuration
 * can be used directly for calling the FLEXIO_SPI_SlaveConfigure().
 * Example:
   code
   flexio_spi_slave_config_t slaveConfig;
   FLEXIO_SPI_SlaveGetDefaultConfig(&slaveConfig);
   endcode
 * param slaveConfig Pointer to the flexio_spi_slave_config_t structure.
*/
void FLEXIO_SPI_SlaveGetDefaultConfig(flexio_spi_slave_config_t *slaveConfig)
{
    assert(slaveConfig);

    /* Initializes the configure structure to zero. */
    memset(slaveConfig, 0, sizeof(*slaveConfig));

    slaveConfig->enableSlave      = true;
    slaveConfig->enableInDoze     = false;
    slaveConfig->enableInDebug    = true;
    slaveConfig->enableFastAccess = false;
    /* Default CPHA = 0. */
    slaveConfig->phase = kFLEXIO_SPI_ClockPhaseFirstEdge;
    /* Default bit count at 8. */
    slaveConfig->dataMode = kFLEXIO_SPI_8BitMode;
}

/*!
 * brief Enables the FlexIO SPI interrupt.
 *
 * This function enables the FlexIO SPI interrupt.
 *
 * param base Pointer to the FLEXIO_SPI_Type structure.
 * param mask interrupt source. The parameter can be any combination of the following values:
 *        arg kFLEXIO_SPI_RxFullInterruptEnable
 *        arg kFLEXIO_SPI_TxEmptyInterruptEnable
 */
void FLEXIO_SPI_EnableInterrupts(FLEXIO_SPI_Type *base, uint32_t mask)
{
    if (mask & kFLEXIO_SPI_TxEmptyInterruptEnable)
    {
        FLEXIO_EnableShifterStatusInterrupts(base->flexioBase, 1 << base->shifterIndex[0]);
    }
    if (mask & kFLEXIO_SPI_RxFullInterruptEnable)
    {
        FLEXIO_EnableShifterStatusInterrupts(base->flexioBase, 1 << base->shifterIndex[1]);
    }
}

/*!
 * brief Disables the FlexIO SPI interrupt.
 *
 * This function disables the FlexIO SPI interrupt.
 *
 * param base Pointer to the FLEXIO_SPI_Type structure.
 * param mask interrupt source The parameter can be any combination of the following values:
 *        arg kFLEXIO_SPI_RxFullInterruptEnable
 *        arg kFLEXIO_SPI_TxEmptyInterruptEnable
 */
void FLEXIO_SPI_DisableInterrupts(FLEXIO_SPI_Type *base, uint32_t mask)
{
    if (mask & kFLEXIO_SPI_TxEmptyInterruptEnable)
    {
        FLEXIO_DisableShifterStatusInterrupts(base->flexioBase, 1 << base->shifterIndex[0]);
    }
    if (mask & kFLEXIO_SPI_RxFullInterruptEnable)
    {
        FLEXIO_DisableShifterStatusInterrupts(base->flexioBase, 1 << base->shifterIndex[1]);
    }
}

/*!
 * brief Enables/disables the FlexIO SPI transmit DMA. This function enables/disables the FlexIO SPI Tx DMA,
 * which means that asserting the kFLEXIO_SPI_TxEmptyFlag does/doesn't trigger the DMA request.
 *
 * param base Pointer to the FLEXIO_SPI_Type structure.
 * param mask SPI DMA source.
 * param enable True means enable DMA, false means disable DMA.
 */
void FLEXIO_SPI_EnableDMA(FLEXIO_SPI_Type *base, uint32_t mask, bool enable)
{
    if (mask & kFLEXIO_SPI_TxDmaEnable)
    {
        FLEXIO_EnableShifterStatusDMA(base->flexioBase, 1U << base->shifterIndex[0], enable);
    }

    if (mask & kFLEXIO_SPI_RxDmaEnable)
    {
        FLEXIO_EnableShifterStatusDMA(base->flexioBase, 1U << base->shifterIndex[1], enable);
    }
}

/*!
 * brief Gets FlexIO SPI status flags.
 *
 * param base Pointer to the FLEXIO_SPI_Type structure.
 * return status flag; Use the status flag to AND the following flag mask and get the status.
 *          arg kFLEXIO_SPI_TxEmptyFlag
 *          arg kFLEXIO_SPI_RxEmptyFlag
 */

uint32_t FLEXIO_SPI_GetStatusFlags(FLEXIO_SPI_Type *base)
{
    uint32_t shifterStatus = FLEXIO_GetShifterStatusFlags(base->flexioBase);
    uint32_t status        = 0;

    status = ((shifterStatus & (1U << base->shifterIndex[0])) >> base->shifterIndex[0]);
    status |= (((shifterStatus & (1U << base->shifterIndex[1])) >> (base->shifterIndex[1])) << 1U);

    return status;
}

/*!
 * brief Clears FlexIO SPI status flags.
 *
 * param base Pointer to the FLEXIO_SPI_Type structure.
 * param mask status flag
 *      The parameter can be any combination of the following values:
 *          arg kFLEXIO_SPI_TxEmptyFlag
 *          arg kFLEXIO_SPI_RxEmptyFlag
 */

void FLEXIO_SPI_ClearStatusFlags(FLEXIO_SPI_Type *base, uint32_t mask)
{
    if (mask & kFLEXIO_SPI_TxBufferEmptyFlag)
    {
        FLEXIO_ClearShifterStatusFlags(base->flexioBase, 1U << base->shifterIndex[0]);
    }
    if (mask & kFLEXIO_SPI_RxBufferFullFlag)
    {
        FLEXIO_ClearShifterStatusFlags(base->flexioBase, 1U << base->shifterIndex[1]);
    }
}

/*!
 * brief Sets baud rate for the FlexIO SPI transfer, which is only used for the master.
 *
 * param base Pointer to the FLEXIO_SPI_Type structure.
 * param baudRate_Bps Baud Rate needed in Hz.
 * param srcClockHz SPI source clock frequency in Hz.
 */
void FLEXIO_SPI_MasterSetBaudRate(FLEXIO_SPI_Type *base, uint32_t baudRate_Bps, uint32_t srcClockHz)
{
    uint16_t timerDiv       = 0;
    uint16_t timerCmp       = 0;
    FLEXIO_Type *flexioBase = base->flexioBase;

    /* Set TIMCMP[7:0] = (baud rate divider / 2) - 1.*/
    timerDiv = srcClockHz / baudRate_Bps;
    timerDiv = timerDiv / 2 - 1U;

    timerCmp = flexioBase->TIMCMP[base->timerIndex[0]];
    timerCmp &= 0xFF00U;
    timerCmp |= timerDiv;

    flexioBase->TIMCMP[base->timerIndex[0]] = timerCmp;
}

/*!
 * brief Sends a buffer of data bytes.
 *
 * note This function blocks using the polling method until all bytes have been sent.
 *
 * param base Pointer to the FLEXIO_SPI_Type structure.
 * param direction Shift direction of MSB first or LSB first.
 * param buffer The data bytes to send.
 * param size The number of data bytes to send.
 */
void FLEXIO_SPI_WriteBlocking(FLEXIO_SPI_Type *base,
                              flexio_spi_shift_direction_t direction,
                              const uint8_t *buffer,
                              size_t size)
{
    assert(buffer);
    assert(size);

    while (size--)
    {
        /* Wait until data transfer complete. */
        while (!(FLEXIO_SPI_GetStatusFlags(base) & kFLEXIO_SPI_TxBufferEmptyFlag))
        {
        }
        FLEXIO_SPI_WriteData(base, direction, *buffer++);
    }
}

/*!
 * brief Receives a buffer of bytes.
 *
 * note This function blocks using the polling method until all bytes have been received.
 *
 * param base Pointer to the FLEXIO_SPI_Type structure.
 * param direction Shift direction of MSB first or LSB first.
 * param buffer The buffer to store the received bytes.
 * param size The number of data bytes to be received.
 * param direction Shift direction of MSB first or LSB first.
 */
void FLEXIO_SPI_ReadBlocking(FLEXIO_SPI_Type *base,
                             flexio_spi_shift_direction_t direction,
                             uint8_t *buffer,
                             size_t size)
{
    assert(buffer);
    assert(size);

    while (size--)
    {
        /* Wait until data transfer complete. */
        while (!(FLEXIO_SPI_GetStatusFlags(base) & kFLEXIO_SPI_RxBufferFullFlag))
        {
        }
        *buffer++ = FLEXIO_SPI_ReadData(base, direction);
    }
}

/*!
 * brief Receives a buffer of bytes.
 *
 * note This function blocks via polling until all bytes have been received.
 *
 * param base pointer to FLEXIO_SPI_Type structure
 * param xfer FlexIO SPI transfer structure, see #flexio_spi_transfer_t.
 */
void FLEXIO_SPI_MasterTransferBlocking(FLEXIO_SPI_Type *base, flexio_spi_transfer_t *xfer)
{
    flexio_spi_shift_direction_t direction;
    uint8_t bytesPerFrame;
    uint32_t dataMode = 0;
    uint16_t timerCmp = base->flexioBase->TIMCMP[base->timerIndex[0]];
    uint16_t tmpData  = FLEXIO_SPI_DUMMYDATA;

    timerCmp &= 0x00FFU;
    /* Configure the values in handle. */
    switch (xfer->flags)
    {
        case kFLEXIO_SPI_8bitMsb:
            dataMode      = (8 * 2 - 1U) << 8U;
            bytesPerFrame = 1;
            direction     = kFLEXIO_SPI_MsbFirst;
            break;

        case kFLEXIO_SPI_8bitLsb:
            dataMode      = (8 * 2 - 1U) << 8U;
            bytesPerFrame = 1;
            direction     = kFLEXIO_SPI_LsbFirst;
            break;

        case kFLEXIO_SPI_16bitMsb:
            dataMode      = (16 * 2 - 1U) << 8U;
            bytesPerFrame = 2;
            direction     = kFLEXIO_SPI_MsbFirst;
            break;

        case kFLEXIO_SPI_16bitLsb:
            dataMode      = (16 * 2 - 1U) << 8U;
            bytesPerFrame = 2;
            direction     = kFLEXIO_SPI_LsbFirst;
            break;

        default:
            dataMode      = (8 * 2 - 1U) << 8U;
            bytesPerFrame = 1;
            direction     = kFLEXIO_SPI_MsbFirst;
            assert(true);
            break;
    }

    dataMode |= timerCmp;

    /* Configure transfer size. */
    base->flexioBase->TIMCMP[base->timerIndex[0]] = dataMode;

    while (xfer->dataSize)
    {
        /* Wait until data transfer complete. */
        while (!(FLEXIO_SPI_GetStatusFlags(base) & kFLEXIO_SPI_TxBufferEmptyFlag))
        {
        }
        if (xfer->txData != NULL)
        {
            /* Transmit data and update tx size/buff. */
            if (bytesPerFrame == 1U)
            {
                tmpData = *(xfer->txData);
                xfer->txData++;
            }
            else
            {
                if (direction == kFLEXIO_SPI_MsbFirst)
                {
                    tmpData = (uint32_t)(xfer->txData[0]) << 8U;
                    tmpData += xfer->txData[1];
                }
                else
                {
                    tmpData = (uint32_t)(xfer->txData[1]) << 8U;
                    tmpData += xfer->txData[0];
                }
                xfer->txData += 2U;
            }
        }
        else
        {
            tmpData = FLEXIO_SPI_DUMMYDATA;
        }

        xfer->dataSize -= bytesPerFrame;

        FLEXIO_SPI_WriteData(base, direction, tmpData);

        while (!(FLEXIO_SPI_GetStatusFlags(base) & kFLEXIO_SPI_RxBufferFullFlag))
        {
        }
        tmpData = FLEXIO_SPI_ReadData(base, direction);

        if (xfer->rxData != NULL)
        {
            if (bytesPerFrame == 1U)
            {
                *xfer->rxData = tmpData;
                xfer->rxData++;
            }
            else
            {
                if (direction == kFLEXIO_SPI_MsbFirst)
                {
                    *((uint16_t *)(xfer->rxData)) = tmpData;
                }
                else
                {
                    *((uint16_t *)(xfer->rxData)) = (((tmpData << 8) & 0xff00U) | ((tmpData >> 8) & 0x00ffU));
                }
                xfer->rxData += 2U;
            }
        }
    }
}

/*!
 * brief Initializes the FlexIO SPI Master handle, which is used in transactional functions.
 *
 * param base Pointer to the FLEXIO_SPI_Type structure.
 * param handle Pointer to the flexio_spi_master_handle_t structure to store the transfer state.
 * param callback The callback function.
 * param userData The parameter of the callback function.
 * retval kStatus_Success Successfully create the handle.
 * retval kStatus_OutOfRange The FlexIO type/handle/ISR table out of range.
 */
status_t FLEXIO_SPI_MasterTransferCreateHandle(FLEXIO_SPI_Type *base,
                                               flexio_spi_master_handle_t *handle,
                                               flexio_spi_master_transfer_callback_t callback,
                                               void *userData)
{
    assert(handle);

    IRQn_Type flexio_irqs[] = FLEXIO_IRQS;

    /* Zero the handle. */
    memset(handle, 0, sizeof(*handle));

    /* Register callback and userData. */
    handle->callback = callback;
    handle->userData = userData;

    /* Enable interrupt in NVIC. */
    EnableIRQ(flexio_irqs[FLEXIO_SPI_GetInstance(base)]);

    /* Save the context in global variables to support the double weak mechanism. */
    return FLEXIO_RegisterHandleIRQ(base, handle, FLEXIO_SPI_MasterTransferHandleIRQ);
}

/*!
 * brief Master transfer data using IRQ.
 *
 * This function sends data using IRQ. This is a non-blocking function, which returns
 * right away. When all data is sent out/received, the callback function is called.
 *
 * param base Pointer to the FLEXIO_SPI_Type structure.
 * param handle Pointer to the flexio_spi_master_handle_t structure to store the transfer state.
 * param xfer FlexIO SPI transfer structure. See #flexio_spi_transfer_t.
 * retval kStatus_Success Successfully start a transfer.
 * retval kStatus_InvalidArgument Input argument is invalid.
 * retval kStatus_FLEXIO_SPI_Busy SPI is not idle, is running another transfer.
 */
status_t FLEXIO_SPI_MasterTransferNonBlocking(FLEXIO_SPI_Type *base,
                                              flexio_spi_master_handle_t *handle,
                                              flexio_spi_transfer_t *xfer)
{
    assert(handle);
    assert(xfer);

    uint32_t dataMode = 0;
    uint16_t timerCmp = base->flexioBase->TIMCMP[base->timerIndex[0]];
    uint16_t tmpData  = FLEXIO_SPI_DUMMYDATA;

    timerCmp &= 0x00FFU;

    /* Check if SPI is busy. */
    if (handle->state == kFLEXIO_SPI_Busy)
    {
        return kStatus_FLEXIO_SPI_Busy;
    }

    /* Check if the argument is legal. */
    if ((xfer->txData == NULL) && (xfer->rxData == NULL))
    {
        return kStatus_InvalidArgument;
    }

    /* Configure the values in handle */
    switch (xfer->flags)
    {
        case kFLEXIO_SPI_8bitMsb:
            dataMode             = (8 * 2 - 1U) << 8U;
            handle->bytePerFrame = 1U;
            handle->direction    = kFLEXIO_SPI_MsbFirst;
            break;
        case kFLEXIO_SPI_8bitLsb:
            dataMode             = (8 * 2 - 1U) << 8U;
            handle->bytePerFrame = 1U;
            handle->direction    = kFLEXIO_SPI_LsbFirst;
            break;
        case kFLEXIO_SPI_16bitMsb:
            dataMode             = (16 * 2 - 1U) << 8U;
            handle->bytePerFrame = 2U;
            handle->direction    = kFLEXIO_SPI_MsbFirst;
            break;
        case kFLEXIO_SPI_16bitLsb:
            dataMode             = (16 * 2 - 1U) << 8U;
            handle->bytePerFrame = 2U;
            handle->direction    = kFLEXIO_SPI_LsbFirst;
            break;
        default:
            dataMode             = (8 * 2 - 1U) << 8U;
            handle->bytePerFrame = 1U;
            handle->direction    = kFLEXIO_SPI_MsbFirst;
            assert(true);
            break;
    }

    dataMode |= timerCmp;

    /* Configure transfer size. */
    base->flexioBase->TIMCMP[base->timerIndex[0]] = dataMode;

    handle->state            = kFLEXIO_SPI_Busy;
    handle->txData           = xfer->txData;
    handle->rxData           = xfer->rxData;
    handle->rxRemainingBytes = xfer->dataSize;

    /* Save total transfer size. */
    handle->transferSize = xfer->dataSize;

    /* Send first byte of data to trigger the rx interrupt. */
    if (handle->txData != NULL)
    {
        /* Transmit data and update tx size/buff. */
        if (handle->bytePerFrame == 1U)
        {
            tmpData = *(handle->txData);
            handle->txData++;
        }
        else
        {
            if (handle->direction == kFLEXIO_SPI_MsbFirst)
            {
                tmpData = (uint32_t)(handle->txData[0]) << 8U;
                tmpData += handle->txData[1];
            }
            else
            {
                tmpData = (uint32_t)(handle->txData[1]) << 8U;
                tmpData += handle->txData[0];
            }
            handle->txData += 2U;
        }
    }
    else
    {
        tmpData = FLEXIO_SPI_DUMMYDATA;
    }

    handle->txRemainingBytes = xfer->dataSize - handle->bytePerFrame;

    FLEXIO_SPI_WriteData(base, handle->direction, tmpData);

    /* Enable transmit and receive interrupt to handle rx. */
    FLEXIO_SPI_EnableInterrupts(base, kFLEXIO_SPI_RxFullInterruptEnable);

    return kStatus_Success;
}

/*!
 * brief Gets the data transfer status which used IRQ.
 *
 * param base Pointer to the FLEXIO_SPI_Type structure.
 * param handle Pointer to the flexio_spi_master_handle_t structure to store the transfer state.
 * param count Number of bytes transferred so far by the non-blocking transaction.
 * retval kStatus_InvalidArgument count is Invalid.
 * retval kStatus_Success Successfully return the count.
 */
status_t FLEXIO_SPI_MasterTransferGetCount(FLEXIO_SPI_Type *base, flexio_spi_master_handle_t *handle, size_t *count)
{
    assert(handle);

    if (!count)
    {
        return kStatus_InvalidArgument;
    }

    /* Return remaing bytes in different cases. */
    if (handle->rxData)
    {
        *count = handle->transferSize - handle->rxRemainingBytes;
    }
    else
    {
        *count = handle->transferSize - handle->txRemainingBytes;
    }

    return kStatus_Success;
}

/*!
 * brief Aborts the master data transfer, which used IRQ.
 *
 * param base Pointer to the FLEXIO_SPI_Type structure.
 * param handle Pointer to the flexio_spi_master_handle_t structure to store the transfer state.
 */
void FLEXIO_SPI_MasterTransferAbort(FLEXIO_SPI_Type *base, flexio_spi_master_handle_t *handle)
{
    assert(handle);

    FLEXIO_SPI_DisableInterrupts(base, kFLEXIO_SPI_RxFullInterruptEnable);
    FLEXIO_SPI_DisableInterrupts(base, kFLEXIO_SPI_TxEmptyInterruptEnable);

    /* Transfer finished, set the state to idle. */
    handle->state = kFLEXIO_SPI_Idle;

    /* Clear the internal state. */
    handle->rxRemainingBytes = 0;
    handle->txRemainingBytes = 0;
}

/*!
 * brief FlexIO SPI master IRQ handler function.
 *
 * param spiType Pointer to the FLEXIO_SPI_Type structure.
 * param spiHandle Pointer to the flexio_spi_master_handle_t structure to store the transfer state.
 */
void FLEXIO_SPI_MasterTransferHandleIRQ(void *spiType, void *spiHandle)
{
    assert(spiHandle);

    flexio_spi_master_handle_t *handle = (flexio_spi_master_handle_t *)spiHandle;
    FLEXIO_SPI_Type *base;
    uint32_t status;

    if (handle->state == kFLEXIO_SPI_Idle)
    {
        return;
    }

    base   = (FLEXIO_SPI_Type *)spiType;
    status = FLEXIO_SPI_GetStatusFlags(base);

    /* Handle rx. */
    if ((status & kFLEXIO_SPI_RxBufferFullFlag) && (handle->rxRemainingBytes))
    {
        FLEXIO_SPI_TransferReceiveTransaction(base, handle);
    }

    /* Handle tx. */
    if ((status & kFLEXIO_SPI_TxBufferEmptyFlag) && (handle->txRemainingBytes))
    {
        FLEXIO_SPI_TransferSendTransaction(base, handle);
    }

    /* All the transfer finished. */
    if ((handle->txRemainingBytes == 0U) && (handle->rxRemainingBytes == 0U))
    {
        FLEXIO_SPI_MasterTransferAbort(base, handle);
        if (handle->callback)
        {
            (handle->callback)(base, handle, kStatus_FLEXIO_SPI_Idle, handle->userData);
        }
    }
}

/*!
 * brief Initializes the FlexIO SPI Slave handle, which is used in transactional functions.
 *
 * param base Pointer to the FLEXIO_SPI_Type structure.
 * param handle Pointer to the flexio_spi_slave_handle_t structure to store the transfer state.
 * param callback The callback function.
 * param userData The parameter of the callback function.
 * retval kStatus_Success Successfully create the handle.
 * retval kStatus_OutOfRange The FlexIO type/handle/ISR table out of range.
 */
status_t FLEXIO_SPI_SlaveTransferCreateHandle(FLEXIO_SPI_Type *base,
                                              flexio_spi_slave_handle_t *handle,
                                              flexio_spi_slave_transfer_callback_t callback,
                                              void *userData)
{
    assert(handle);

    IRQn_Type flexio_irqs[] = FLEXIO_IRQS;

    /* Zero the handle. */
    memset(handle, 0, sizeof(*handle));

    /* Register callback and userData. */
    handle->callback = callback;
    handle->userData = userData;

    /* Enable interrupt in NVIC. */
    EnableIRQ(flexio_irqs[FLEXIO_SPI_GetInstance(base)]);

    /* Save the context in global variables to support the double weak mechanism. */
    return FLEXIO_RegisterHandleIRQ(base, handle, FLEXIO_SPI_SlaveTransferHandleIRQ);
}

/*!
 * brief Slave transfer data using IRQ.
 *
 * This function sends data using IRQ. This is a non-blocking function, which returns
 * right away. When all data is sent out/received, the callback function is called.
 * param handle Pointer to the flexio_spi_slave_handle_t structure to store the transfer state.
 *
 * param base Pointer to the FLEXIO_SPI_Type structure.
 * param xfer FlexIO SPI transfer structure. See #flexio_spi_transfer_t.
 * retval kStatus_Success Successfully start a transfer.
 * retval kStatus_InvalidArgument Input argument is invalid.
 * retval kStatus_FLEXIO_SPI_Busy SPI is not idle; it is running another transfer.
 */
status_t FLEXIO_SPI_SlaveTransferNonBlocking(FLEXIO_SPI_Type *base,
                                             flexio_spi_slave_handle_t *handle,
                                             flexio_spi_transfer_t *xfer)
{
    assert(handle);
    assert(xfer);

    uint32_t dataMode = 0;

    /* Check if SPI is busy. */
    if (handle->state == kFLEXIO_SPI_Busy)
    {
        return kStatus_FLEXIO_SPI_Busy;
    }

    /* Check if the argument is legal. */
    if ((xfer->txData == NULL) && (xfer->rxData == NULL))
    {
        return kStatus_InvalidArgument;
    }

    /* Configure the values in handle */
    switch (xfer->flags)
    {
        case kFLEXIO_SPI_8bitMsb:
            dataMode             = 8 * 2 - 1U;
            handle->bytePerFrame = 1U;
            handle->direction    = kFLEXIO_SPI_MsbFirst;
            break;
        case kFLEXIO_SPI_8bitLsb:
            dataMode             = 8 * 2 - 1U;
            handle->bytePerFrame = 1U;
            handle->direction    = kFLEXIO_SPI_LsbFirst;
            break;
        case kFLEXIO_SPI_16bitMsb:
            dataMode             = 16 * 2 - 1U;
            handle->bytePerFrame = 2U;
            handle->direction    = kFLEXIO_SPI_MsbFirst;
            break;
        case kFLEXIO_SPI_16bitLsb:
            dataMode             = 16 * 2 - 1U;
            handle->bytePerFrame = 2U;
            handle->direction    = kFLEXIO_SPI_LsbFirst;
            break;
        default:
            dataMode             = 8 * 2 - 1U;
            handle->bytePerFrame = 1U;
            handle->direction    = kFLEXIO_SPI_MsbFirst;
            assert(true);
            break;
    }

    /* Configure transfer size. */
    base->flexioBase->TIMCMP[base->timerIndex[0]] = dataMode;

    handle->state            = kFLEXIO_SPI_Busy;
    handle->txData           = xfer->txData;
    handle->rxData           = xfer->rxData;
    handle->txRemainingBytes = xfer->dataSize;
    handle->rxRemainingBytes = xfer->dataSize;

    /* Save total transfer size. */
    handle->transferSize = xfer->dataSize;

    /* Enable transmit and receive interrupt to handle tx and rx. */
    FLEXIO_SPI_EnableInterrupts(base, kFLEXIO_SPI_TxEmptyInterruptEnable);
    FLEXIO_SPI_EnableInterrupts(base, kFLEXIO_SPI_RxFullInterruptEnable);

    return kStatus_Success;
}

/*!
 * brief FlexIO SPI slave IRQ handler function.
 *
 * param spiType Pointer to the FLEXIO_SPI_Type structure.
 * param spiHandle Pointer to the flexio_spi_slave_handle_t structure to store the transfer state.
 */
void FLEXIO_SPI_SlaveTransferHandleIRQ(void *spiType, void *spiHandle)
{
    assert(spiHandle);

    flexio_spi_master_handle_t *handle = (flexio_spi_master_handle_t *)spiHandle;
    FLEXIO_SPI_Type *base;
    uint32_t status;

    if (handle->state == kFLEXIO_SPI_Idle)
    {
        return;
    }

    base   = (FLEXIO_SPI_Type *)spiType;
    status = FLEXIO_SPI_GetStatusFlags(base);

    /* Handle tx. */
    if ((status & kFLEXIO_SPI_TxBufferEmptyFlag) && (handle->txRemainingBytes))
    {
        FLEXIO_SPI_TransferSendTransaction(base, handle);
    }

    /* Handle rx. */
    if ((status & kFLEXIO_SPI_RxBufferFullFlag) && (handle->rxRemainingBytes))
    {
        FLEXIO_SPI_TransferReceiveTransaction(base, handle);
    }

    /* All the transfer finished. */
    if ((handle->txRemainingBytes == 0U) && (handle->rxRemainingBytes == 0U))
    {
        FLEXIO_SPI_SlaveTransferAbort(base, handle);
        if (handle->callback)
        {
            (handle->callback)(base, handle, kStatus_FLEXIO_SPI_Idle, handle->userData);
        }
    }
}
